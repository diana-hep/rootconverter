package org.dianahep.scaroot

// import scala.language.existentials
import scala.language.experimental.macros
// import scala.language.higherKinds
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

  trait My[TYPE] {
    def name: String
    def fieldTypes: List[(String, Type)]
    def apply(factories: List[(String, Factory[_])]): FactoryClass[TYPE]
  }
  object My {
    def apply[TYPE]: My[TYPE] = macro applyImpl[TYPE]

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
        fieldTypes += q"""$name -> typeOf[$tpe]"""
        getFields += q"""factoryArray($i).asInstanceOf[Factory[$tpe]](byteBuffer)"""
        i += 1
      }

      val out = c.Expr[My[TYPE]](q"""
        import java.nio.ByteBuffer

        import scala.reflect.runtime.universe.typeOf

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
      println(out)
      out
    }
  }

  /////////////////////////////////////////////////// entry point for iterating over ROOT files

  class RootTreeIterator[TYPE](fileLocations: Seq[String], treeLocation: String, libs: Seq[String] = Nil, myclasses: Map[String, My[_]] = Map[String, My[_]]()) extends Iterator[TYPE] {
    // FIXME: if TYPE is not Generic, add 'treeLocation -> My[TYPE]' to myclasses (note: that macro would have to be materialized implicitly; also, it's safe because this class already has a non-trivial constructor).

    private var libscpp = Pointer.NULL
    libs foreach {lib => libscpp = RootReaderCPPLibrary.addVectorString(libscpp, lib)}

    private var done = true
    private var treeWalker = Pointer.NULL
    private var fileIndex = 0

    val schema: SchemaClass =
      if (!fileLocations.isEmpty) {
        treeWalker = RootReaderCPPLibrary.newTreeWalker(fileLocations(fileIndex), treeLocation, "", libscpp)

        if (RootReaderCPPLibrary.valid(treeWalker) == 0)
          throw new RuntimeException(RootReaderCPPLibrary.errorMessage(treeWalker))

        done = (RootReaderCPPLibrary.next(treeWalker) == 0)
        while (!done  &&  RootReaderCPPLibrary.resolved(treeWalker) == 0) {
          RootReaderCPPLibrary.resolve(treeWalker)
          done = (RootReaderCPPLibrary.next(treeWalker) == 0)
        }
        RootReaderCPPLibrary.setEntryInCurrentTree(treeWalker, 0L)

        Schema(treeWalker)
      }
      else
        throw new RuntimeException("Cannot build RootTreeIterator over an empty set of files.")

    val factory = FactoryClass[TYPE](schema, myclasses)

    private var bufferSize = 128*1024   // FIXME: update this when you encounter errors
    private var buffer = new Memory(bufferSize)
    private var byteBuffer = buffer.getByteBuffer(0, bufferSize)

    def hasNext = !done
    def next() = {
      if (done)
        throw new RuntimeException("next() called on empty RootTreeIterator (create a new one to run over the data again)")

      // FIXME: implement multithreaded mode
      buffer.setByte(0, 1)

      RootReaderCPPLibrary.copyToBuffer(treeWalker, 1, buffer, new NativeLong(bufferSize))
      byteBuffer.rewind()
      val statusByte = byteBuffer.get
      // println(s"statusByte $statusByte")

      // FIXME: this is where you'd check the status byte to see if the buffer size needs to be increased or wait for the lock to be released in multithreaded mode

      val out = factory(byteBuffer)

      done = (RootReaderCPPLibrary.next(treeWalker) == 0)
      if (done) {
        fileIndex += 1
        if (fileIndex < fileLocations.size) {
          RootReaderCPPLibrary.reset(treeWalker, fileLocations(fileIndex))
          done = (RootReaderCPPLibrary.next(treeWalker) == 0)
        }
      }

      out
    }

    def reset() {
      fileIndex = 0
      RootReaderCPPLibrary.reset(treeWalker, fileLocations(fileIndex))
      done = (RootReaderCPPLibrary.next(treeWalker) == 0)
    }
  }
  object RootTreeIterator {
    def apply[TYPE](fileLocations: Seq[String], treeLocation: String, libs: Seq[String] = Nil, myclasses: Map[String, My[_]] = Map[String, My[_]]()) = new RootTreeIterator[TYPE](fileLocations, treeLocation, libs, myclasses)
  }
}
