// Copyright 2016 Jim Pivarski
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package org.dianahep.scaroot

import scala.collection.immutable.SortedSet
import scala.collection.mutable
import scala.language.experimental.macros
import scala.reflect.macros.Context
import scala.reflect.runtime.universe.Type
import scala.reflect.runtime.universe.WeakTypeTag

import com.sun.jna.Pointer
import com.sun.jna.Memory
import com.sun.jna.NativeLong

import org.dianahep.scaroot.reader.schema._
import org.dianahep.scaroot.reader.factory._

package reader {
  /////////////////////////////////////////////////// class to use when no My[TYPE] is supplied

  class Generic(val fields: Map[String, Any]) {
    def apply(field: String): Any = fields(field)
    override def toString() = s"""Generic(Map(${fields.map({case (k, v) => "\"" + k + "\"" + " -> " + v.toString}).mkString(", ")}))"""
  }
  object Generic {
    def apply(fields: Map[String, Any]) = new Generic(fields)
    def unapply(x: Generic) = Some(x.fields)
  }

  /////////////////////////////////////////////////// user-friendly representation of non-String bytes (like Python 3)

  class Bytes(array: Array[Byte]) extends Seq[Byte] {
    def apply(idx: Int) = array(idx)
    def iterator = Iterator[Byte](array: _*)
    def length = array.size
    def decode = new String(array)
    def decode(charset: String) = new String(array, charset)
    override def toString() = s"""Bytes(${array.mkString(", ")})"""
  }
  object Bytes {
    def apply(array: Array[Byte]) = new Bytes(array)
    def apply(iterable: Iterable[Byte]) = new Bytes(iterable.toArray)
    def apply(bytes: Byte*) = new Bytes(bytes.toArray)
    def encode(str: String) = new Bytes(str.getBytes)
    def encode(str: String, charset: String) = new Bytes(str.getBytes(charset))
  }

  /////////////////////////////////////////////////// user's class specification (a factory-factory!)

  abstract class My[TYPE] {
    def dataType: Type
    def name: String
    def fieldTypes: List[(String, Type)]
    def apply(factories: List[(String, Factory[_])]): FactoryClass[TYPE]
  }
  object My {
    implicit def apply[TYPE]: My[TYPE] = macro applyImpl[TYPE]

    def applyImpl[TYPE : c.WeakTypeTag](c: Context): c.Expr[My[TYPE]] = {
      import c.universe._
      val dataClass = weakTypeOf[TYPE]
      val dataClassName = dataClass.toString

      val constructorParams = dataClass.declarations.collectFirst {
        case m: MethodSymbol if (m.isPrimaryConstructor) => m
      }.get.paramss.head

      var i = 0
      val fieldTypes = List.newBuilder[Tree]
      val getFields = List.newBuilder[Tree]

      constructorParams.foreach {param =>
        val name = param.asTerm.name.decodedName.toString
        val tpe = param.typeSignature
        fieldTypes += q"""$name -> weakTypeOf[$tpe]"""
        getFields += q"""factoryArray($i).asInstanceOf[Factory[$tpe]](dataBuffer)"""
        i += 1
      }

      c.Expr[My[TYPE]](q"""
import java.nio.ByteBuffer

        import scala.reflect.runtime.universe.weakTypeOf

        import org.dianahep.scaroot.reader._
        import org.dianahep.scaroot.reader.schema._
        import org.dianahep.scaroot.reader.factory._

        new My[$dataClass] {
          // What you know at compile-time...
          val dataType = weakTypeOf[$dataClass].asInstanceOf[scala.reflect.runtime.universe.Type]
          val name = $dataClassName
          val fieldTypes = List(..${fieldTypes.result})

          def apply(factories: List[(String, Factory[_])]) =
            new FactoryClass[$dataClass](name, factories) {
              // What you know when you read a ROOT schema...
              // (I'll do the type-checking in the factory-builder, not here. Better error messages that way.)

              val factoryArray = factories.map(_._2).toArray

              // Fast runtime loop...
              def apply(dataBuffer: ByteBuffer) = {
                new $dataClass(..${getFields.result})
              }
            }
        }
      """)
    }
  }

  /////////////////////////////////////////////////// entry point for iterating over ROOT files

  object LoadLibsOnce {
    // Avoid conflict between Java's signal handlers and ROOT's (which causes rare segmentation faults).
    private var isready = false
    RootReaderCPPLibrary.resetSignals()
    isready = true
    def ready = isready

    // Keep track of which libraries have already been loaded to avoid loading them multiple times.
    val includeDirs = mutable.Set[String]()
    val loadedLibraries = mutable.Set[String]()

    def include(dir: String) {
      if (!(includeDirs contains dir)) {
        RootReaderCPPLibrary.addInclude(dir)
        includeDirs += dir
      }
    }

    def apply(lib: String) {
      if (!(loadedLibraries contains lib)) {
        RootReaderCPPLibrary.loadLibrary(lib)
        loadedLibraries += lib
      }
    }
  }

  class RootTreeIterator[TYPE : WeakTypeTag : My](fileLocations: Seq[String],
                                                  treeLocation: String,
                                                  includes: Seq[String] = Nil,
                                                  libs: Seq[String] = Nil,
                                                  inferTypes: Boolean = false,
                                                  myclasses: Map[String, My[_]] = Map[String, My[_]](),
                                                  start: Long = 0L,
                                                  end: Long = -1L,
                                                  microBatchSize: Int = 10) extends Iterator[TYPE] {
    if (fileLocations.isEmpty)
      throw new RuntimeException("Cannot build RootTreeIterator over an empty set of files.")
    if (start < 0)
      throw new IllegalArgumentException(s"The start ($start) must be greater than or equal to zero.")
    if (end >= 0  &&  start >= end)
      throw new IllegalArgumentException(s"If an ending index is given (greater than or equal to zero), then start ($start) must be less than end ($end).")
    if (microBatchSize < 1)
      throw new IllegalArgumentException(s"The microBatchSize ($microBatchSize) must be greater than or equal to one.")

    val loadLibsOnce = LoadLibsOnce
    while (!loadLibsOnce.ready) { Thread.sleep(1) }
    includes foreach {dir => loadLibsOnce.include(dir)}
    libs foreach {lib => loadLibsOnce(lib)}

    if (inferTypes) {
      val errorMessage: String = RootReaderCPPLibrary.inferTypes(fileLocations(0), treeLocation)
      if (!errorMessage.isEmpty)
        throw new RuntimeException(errorMessage)
    }

    // Pack of state variables that all have to be kept in sync!
    // Limit user access to setIndex, reset, and incrementIndex, which should preserve interrelationships.
    private var done = true
    private var treeWalker = Pointer.NULL
    private var entryIndex = 0L
    private var fileIndex = 0
    private var entryInFileIndex = 0L
    private var microBatchIndex = 0

    private var entriesInFileArray = Array.fill[Long](fileLocations.size)(-1L)   // opening files is expensive
    private def entriesInFile(i: Int) = {
      if (entriesInFileArray(i) < 0) {
        RootReaderCPPLibrary.reset(treeWalker, fileLocations(i))
        entriesInFileArray(i) = RootReaderCPPLibrary.numEntriesInCurrentTree(treeWalker)
      }
      entriesInFileArray(i)
    }

    def index = entryIndex

    // Go to a random position (not a common feature for an Iterator to have, but useful, particularly for implementing "start").
    def setIndex(index: Long) {
      if (index < start  ||  (end >= 0  &&  index >= end))
        throw new IllegalArgumentException(s"The index ($index) must be between start ($start) and end ($end).")
      entryIndex = 0L
      fileIndex = 0
      entryInFileIndex = 0L
      microBatchIndex = 0
      while (entryIndex < index) {
        if (fileIndex >= entriesInFileArray.size) {
          done = true
          entryIndex = -1L
          throw new IllegalArgumentException(s"Total number of entries is ${entriesInFileArray.sum}, so $index would be beyond the last.")
        }
        if (entryIndex + entriesInFile(fileIndex) <= index) {
          fileIndex += 1
          entryIndex += entriesInFile(fileIndex)
        }
        else {
          entryInFileIndex = index - entryIndex
          entryIndex = index
        }
      }
      RootReaderCPPLibrary.reset(treeWalker, fileLocations(fileIndex))
      done = false
    }

    def reset() { setIndex(0L) }  // synonym

    // Go forward by one (the usual case).
    def incrementIndex() {
      entryIndex += 1L
      entryInFileIndex += 1L
      microBatchIndex += 1

      if (microBatchIndex >= microBatchSize)
        microBatchIndex = 0

      if (entryInFileIndex >= entriesInFile(fileIndex)) {
        fileIndex += 1
        entryInFileIndex = 0L
        microBatchIndex = 0
        if (fileIndex >= entriesInFileArray.size)
          done = true
        else
          RootReaderCPPLibrary.reset(treeWalker, fileLocations(fileIndex))
      }

      if (end >= 0  &&  entryIndex >= end)
        done = true
    }

    val schema: SchemaClass = {
      treeWalker = RootReaderCPPLibrary.newTreeWalker(fileLocations(0), treeLocation, "")

      if (RootReaderCPPLibrary.valid(treeWalker) == 0)
        throw new RuntimeException(RootReaderCPPLibrary.errorMessage(treeWalker))

      done = (RootReaderCPPLibrary.next(treeWalker) == 0)
      while (!done  &&  RootReaderCPPLibrary.resolved(treeWalker) == 0) {
        RootReaderCPPLibrary.resolve(treeWalker)
        done = (RootReaderCPPLibrary.next(treeWalker) == 0)
      }

      Schema(treeWalker)
    }

    import scala.reflect.runtime.universe.weakTypeOf
    val allmyclasses =
      if (!(myclasses.keySet contains schema.name)  &&  !(weakTypeOf[TYPE] =:= weakTypeOf[Generic]  ||  weakTypeOf[TYPE] =:= weakTypeOf[AnyRef]))
        myclasses.updated(schema.name, implicitly[My[TYPE]])
      else
        myclasses

    val factory = FactoryClass[TYPE](schema, allmyclasses)

    setIndex(start)

    private var bufferSize = new NativeLong(64*1024)
    private var buffer = new Memory(bufferSize.longValue)
    private var byteBuffer = buffer.getByteBuffer(0, bufferSize.longValue)
    // private var dataByteBuffer = new DataByteBuffer(byteBuffer)
    private var statusByte = 1.toByte

    def repr = RootReaderCPPLibrary.repr(treeWalker)

    private def thisMicroBatchSize =
      if (entriesInFile(fileIndex) - entryInFileIndex > microBatchSize)
        microBatchSize
      else
        (entriesInFile(fileIndex) - entryInFileIndex).toInt

    def hasNext = !done
    def next() = {
      if (done)
        throw new RuntimeException("next() called on empty RootTreeIterator (create a new one to run over the data again)")

      if (microBatchIndex == 0) {
        // Set the status byte to 1 (writing) and let C++ write to the buffer.
        statusByte = 1
        buffer.setByte(0, statusByte)
        RootReaderCPPLibrary.copyToBuffer(treeWalker, entryInFileIndex, thisMicroBatchSize, buffer, bufferSize)
        byteBuffer.rewind()
      }

      // Check the status byte to find out if copying failed due to a buffer that's too small (the only error we handle).
      statusByte = byteBuffer.get
      while (statusByte == 2) {
        // Get a new, bigger buffer (and let the old one be garbage collected).
        bufferSize = new NativeLong(bufferSize.longValue * 2L)
        buffer = new Memory(bufferSize.longValue)
        byteBuffer = buffer.getByteBuffer(0, bufferSize.longValue)
        // dataByteBuffer = new DataByteBuffer(byteBuffer)

        // Try, try again.
        microBatchIndex = 0
        statusByte = 1
        buffer.setByte(0, statusByte)
        RootReaderCPPLibrary.copyToBuffer(treeWalker, entryInFileIndex, thisMicroBatchSize, buffer, bufferSize)
        byteBuffer.rewind()
        statusByte = byteBuffer.get
      }

      // Interpret the data in the buffer, creating Scala objects.
      val out = factory(byteBuffer)

      // Increment the counter and see if it's time to step to the next file.
      incrementIndex()

      out
    }
  }
  object RootTreeIterator {
    def apply[TYPE : WeakTypeTag : My](fileLocations: Seq[String],
                                       treeLocation: String,
                                       includes: Seq[String] = Nil,
                                       libs: Seq[String] = Nil,
                                       inferTypes: Boolean = false,
                                       myclasses: Map[String, My[_]] = Map[String, My[_]](),
                                       start: Long = 0L,
                                       end: Long = -1L,
                                       microBatchSize: Int = 10) =
      new RootTreeIterator(fileLocations, treeLocation, includes, libs, inferTypes, myclasses, start, end, microBatchSize)
  }

  /////////////////////////////////////////////////// interface to XRootD for creating file sets and splits

  class XRootD(baseurl: String) {
    private val ensureResetSignals = LoadLibsOnce
    private val fs = RootReaderCPPLibrary.xrootdFileSystem(baseurl)

    if (fileSize("/") < 0)
      throw new IllegalArgumentException(s"""XRootD server cannot be found at "$baseurl".""")

    def fileSize(path: String): Long = RootReaderCPPLibrary.xrootdFileSize(fs, path).longValue

    def listDirectory(path: String): List[String] = {
      RootReaderCPPLibrary.xrootdDirectoryBegin(fs, path)

      val builder = List.newBuilder[String]
      var done = false
      while (!done) {
        val item = RootReaderCPPLibrary.xrootdDirectoryEntry(fs)
        if (item == null)
          done = true
        else
          builder += item
      }
      RootReaderCPPLibrary.xrootdDirectoryEnd(fs)
      builder.result
    }
  }
  object XRootD {
    case class File(url: String, size: Long)

    val URLPattern = """(root://[^\/]*)(.*)""".r

    private def matches(xrootd: XRootD, established: String, path: List[String]): List[String] = path match {
      case dir :: rest =>
        val (regex, branches) = globToRegex(dir)
        if (branches) {
          val pattern = java.util.regex.Pattern.compile(regex)
          val results = xrootd.listDirectory(established + "/")
          results.filter(pattern.matcher(_).lookingAt).map(established + "/" + _).flatMap(matches(xrootd, _, rest))
        }
        else
          matches(xrootd, established + "/" + dir, rest)
      case Nil =>
        List(established)
    }

    def apply(globurl: String): Seq[File] = globurl match {
      case URLPattern(baseurl, pathurl) =>
        val xrootd = new XRootD(baseurl)
        val results = matches(xrootd, "", pathurl.split('/').toList).map(baseurl + _).map(x => File(x, xrootd.fileSize(x)))
        // Drop duplicates and sort results with biggest first.
        SortedSet[File](results: _*)(Ordering.by[File, Long](-_.size)).toSeq

      case _ =>
        throw new IllegalArgumentException(s"""Not an XRootD URL: "$globurl"""")
    }

    def balance(globurl: String, partitions: Int): Seq[Seq[File]] = {
      val out = Array.fill(partitions)(mutable.ListBuffer[File]())

      def size(files: Iterable[File]) = files.map(_.size).sum

      def minimum = {
        var best = -1L
        var besti = 0
        0 until partitions foreach {i =>
          val s = size(out(i))
          if (best < 0  ||  s < best) {
            best = s
            besti = i
          }
        }
        besti
      }

      apply(globurl) foreach {file =>
        out(minimum) += file
      }

      out.map(_.toList).toSeq
    }

    // http://stackoverflow.com/a/17369948/1623645
    private def globToRegex(pattern: String): (String, Boolean) = {
      val sb = new java.lang.StringBuilder
      var inGroup = 0
      var inClass = 0
      var firstIndexInClass = -1
      val arr = pattern.toCharArray
      var i = 0
      var branches = false
      while (i < arr.length) {
        var ch = arr(i)
        ch match {
          case '\\' =>
            i += 1
            if (i >= arr.length)
              sb.append('\\')
            else {
              var next = arr(i)
              if (next == 'Q'  ||  next == 'E')
                sb.append('\\')  // extra escape needed
              if (next != ',')
                sb.append('\\')  // only one escape needed
              sb.append(next)
            }
          case '*' if (inClass == 0) =>
            branches = true
            sb.append(".*")
          case '*' =>
            sb.append("*")
          case '?' if (inClass == 0) =>
            branches = true
            sb.append('.')
          case '?' =>
            sb.append('?')
          case '[' =>
            branches = true
            inClass += 1
            firstIndexInClass = i + 1
            sb.append('[')
          case ']' =>
            inClass -= 1
            sb.append(']')
          case '.' | '(' | ')' | '+' | '|' | '^' | '$' | '@' | '%' =>
            if (inClass == 0  ||  (firstIndexInClass == i  &&  ch == '^'))
              sb.append('\\')
            sb.append(ch)
          case '!' =>
            if (firstIndexInClass == i)
              sb.append('^')
            else
              sb.append('!')
          case '{' =>
            branches = true
            inGroup += 1
            sb.append('(')
          case '}' =>
            inGroup -= 1
            sb.append(')')
          case ',' =>
            if (inGroup > 0)
              sb.append('|')
            else
              sb.append(',')
          case _ =>
            sb.append(ch)
        }
        i += 1
      }
      (sb.toString, branches)
    }
  }
}
