package org.dianahep.scaroot

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
        getFields += q"""factoryArray($i).asInstanceOf[Factory[$tpe]](byteBuffer)"""
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
          val name = $dataClassName
          val fieldTypes = List(..${fieldTypes.result})

          def apply(factories: List[(String, Factory[_])]) =
            new FactoryClass[$dataClass](name, factories) {
              // What you know when you read a ROOT schema...
              // (I'll do the type-checking in the factory-builder, not here. Better error messages that way.)

              val factoryArray = factories.map(_._2).toArray

              // Fast runtime loop...
              def apply(byteBuffer: ByteBuffer) = {
                new $dataClass(..${getFields.result})
              }
            }
        }
      """)
    }
  }

  /////////////////////////////////////////////////// entry point for iterating over ROOT files
  
  trait RootTreeIterator[TYPE] extends Iterator[TYPE] {
    def fileLocations: Seq[String]
    def treeLocation: String
    def libs: Seq[String]
    def myclasses: Map[String, My[_]]
  }

  case class RootTreeRoundRobinIterator[TYPE : WeakTypeTag : My](fileLocations: Seq[String],
                                                                 treeLocation: String,
                                                                 libs: Seq[String] = Nil,
                                                                 myclasses: Map[String, My[_]] = Map[String, My[_]](),
                                                                 numberOfThreads: Int = 10) extends RootTreeIterator[TYPE] {
    if (numberOfThreads < 1)
      throw new IllegalArgumentException(s"Number of threads must be at least one, not $numberOfThreads.")

    private var pointer = 0
    private val pipeline =
      0 until numberOfThreads map {i =>
        RootTreeSingleIteratorThread(RootTreeSingleIterator(fileLocations, treeLocation, libs, myclasses,
          start = i, run = 1, skip = numberOfThreads - 1))
      }

    pipeline.foreach(_.start())

    def hasNext = pipeline(pointer).hasNext

    def next() = {
      val out = pipeline(pointer).next()
      pointer = (pointer + 1) % numberOfThreads
      out
    }
  }

  case class RootTreeSingleIteratorThread[TYPE : WeakTypeTag : My](rootTreeSingleIterator: RootTreeSingleIterator[TYPE]) extends Thread with Iterator[TYPE] {
    setDaemon(true)

    private var endOfInput = false
    private val mvar = new RootTreeSingleIteratorThread.MVar[TYPE]

    // Executed by the consumer thread (RootTreeRoundRobinIterator).
    def hasNext = !(endOfInput  &&  mvar.isEmpty)
    def next() = mvar.take()

    // Executed by this thread, the producer thread.
    override def run() {
      while (rootTreeSingleIterator.hasNext) {
        mvar.put(rootTreeSingleIterator.next())
      }
      endOfInput = true
    }
  }
  object RootTreeSingleIteratorThread {
    // Concurrency atom from Haskell.
    class MVar[TYPE] {
      private var message: Option[TYPE] = None
      def isEmpty = message.isEmpty
      def take(): TYPE = synchronized {
        while (message.isEmpty) wait()
        val out = message.get
        message = None
        notify()
        out
      }
      def put(x: TYPE): Unit = synchronized {
        while (!message.isEmpty) wait()
        message = Some(x)
        notify()
      }
    }
  }

  object LoadLibsOnce {
    RootReaderCPPLibrary.resetSignals()
    val alreadyLoaded = mutable.Set[String]()
    def apply(lib: String) {
      if (!(alreadyLoaded contains lib)) {
        RootReaderCPPLibrary.loadLibrary(lib)
        alreadyLoaded += lib
      }
    }
  }

  case class RootTreeSingleIterator[TYPE : WeakTypeTag : My](fileLocations: Seq[String],
                                                             treeLocation: String,
                                                             libs: Seq[String] = Nil,
                                                             myclasses: Map[String, My[_]] = Map[String, My[_]](),
                                                             start: Long = 0L,
                                                             end: Long = -1L,
                                                             run: Long = 1L,
                                                             skip: Long = 0L) extends RootTreeIterator[TYPE] {
    if (start < 0)
      throw new IllegalArgumentException(s"The start ($start) must be greater than or equal to zero.")
    if (end >= 0  &&  start >= end)
      throw new IllegalArgumentException(s"If an ending index is given (greater than or equal to zero), then start ($start) must be less than end ($end).")
    if (run < 1)
      throw new IllegalArgumentException(s"The run ($run) must be strictly greater than zero.")
    if (skip < 0)
      throw new IllegalArgumentException(s"The skip ($skip) must be greater than or equal to zero.")

    libs foreach {lib => LoadLibsOnce(lib)}

    // Pack of state variables that all have to be kept in sync!
    // Limit user access to setIndex, reset(), and incrementIndex(), which should preserve interrelationships.
    private var done = true
    private var treeWalker = Pointer.NULL
    private var entryIndex = 0L
    private var fileIndex = 0
    private var entryInFileIndex = 0L
    private var pattern = 0L

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
      pattern = (entryIndex - start) % (run + skip)
    }

    def reset() { setIndex(0L) }  // synonym

    // Go forward by one (the usual case).
    def incrementIndex() {
      entryIndex += 1L
      entryInFileIndex += 1L
      if (entryInFileIndex >= entriesInFile(fileIndex)) {
        fileIndex += 1
        entryInFileIndex = 0L
        if (fileIndex >= entriesInFileArray.size)
          done = true
        else
          RootReaderCPPLibrary.reset(treeWalker, fileLocations(fileIndex))
      }
      if (end >= 0  &&  entryIndex >= end)
        done = true
      pattern = (entryIndex - start) % (run + skip)
    }

    val schema: SchemaClass =
      if (!fileLocations.isEmpty) {
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
      else
        throw new RuntimeException("Cannot build RootTreeIterator over an empty set of files.")

    import scala.reflect.runtime.universe.weakTypeOf
    val allmyclasses =
      if (!(weakTypeOf[TYPE] =:= weakTypeOf[Generic]))
        myclasses.updated(schema.name, implicitly[My[TYPE]])
      else
        myclasses

    val factory = FactoryClass[TYPE](schema, allmyclasses)

    setIndex(start)

    private var bufferSize = new NativeLong(64*1024)
    private var buffer = new Memory(bufferSize.longValue)
    private var byteBuffer = buffer.getByteBuffer(0, bufferSize.longValue)
    private var statusByte = 1.toByte

    def hasNext = !done
    def next() = {
      if (done)
        throw new RuntimeException("next() called on empty RootTreeIterator (create a new one to run over the data again)")

      // Set the status byte to 1 (writing) and let C++ write to the buffer.
      statusByte = 1
      buffer.setByte(0, statusByte)
      RootReaderCPPLibrary.copyToBuffer(treeWalker, entryInFileIndex, buffer, bufferSize)
      byteBuffer.rewind()

      // Check the status byte to find out if copying failed due to a buffer that's too small (the only error we handle).
      statusByte = byteBuffer.get
      while (statusByte == 2) {
        // Get a new, bigger buffer (and let the old one be garbage collected).
        bufferSize = new NativeLong(bufferSize.longValue * 2L)
        buffer = new Memory(bufferSize.longValue)
        byteBuffer = buffer.getByteBuffer(0, bufferSize.longValue)

        // Try, try again.
        statusByte = 1
        buffer.setByte(0, statusByte)
        RootReaderCPPLibrary.copyToBuffer(treeWalker, entryInFileIndex, buffer, bufferSize)
        byteBuffer.rewind()
        statusByte = byteBuffer.get
      }

      // Interpret the data in the buffer, creating Scala objects.
      val out = factory(byteBuffer)

      // Increment the counter and see if it's time to step to the next file.
      incrementIndex()
      while (pattern >= run  &&  !done)
        incrementIndex()      // inefficient if skip is large, but that's not our use-case

      out
    }
  }
}
