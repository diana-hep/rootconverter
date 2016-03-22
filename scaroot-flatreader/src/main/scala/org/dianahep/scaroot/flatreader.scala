package org.dianahep.scaroot

import java.nio.ByteBuffer

import scala.collection.mutable
import scala.collection.mutable.Builder
import scala.language.existentials
import scala.language.experimental.macros
import scala.language.higherKinds
import scala.reflect.classTag
import scala.reflect.ClassTag
// import scala.reflect.macros.blackbox.Context
import scala.reflect.macros.Context
import scala.reflect.runtime.universe.WeakTypeTag

import com.sun.jna.Pointer

package flatreader {
  // Numerical enums from C++ that can be matched in case statements.
  // Also used in a stack to build the schema used at runtime.
  // Must be kept in-sync with scaroot-flatreader/src/main/cpp/datawalker.h!

  sealed class SchemaInstruction(val index: Int, val name: String) {
    def unapply(x: Int) = (x == index)
    override def toString() = s"SchemaInstruction.$name"
  }

  object SchemaInstruction {
    val SchemaBool   = new SchemaInstruction(0, "SchemaBool")
    val SchemaChar   = new SchemaInstruction(1, "SchemaChar")
    val SchemaUChar  = new SchemaInstruction(2, "SchemaUChar")
    val SchemaShort  = new SchemaInstruction(3, "SchemaShort")
    val SchemaUShort = new SchemaInstruction(4, "SchemaUShort")
    val SchemaInt    = new SchemaInstruction(5, "SchemaInt")
    val SchemaUInt   = new SchemaInstruction(6, "SchemaUInt")
    val SchemaLong   = new SchemaInstruction(7, "SchemaLong")
    val SchemaULong  = new SchemaInstruction(8, "SchemaULong")
    val SchemaFloat  = new SchemaInstruction(9, "SchemaFloat")
    val SchemaDouble = new SchemaInstruction(10, "SchemaDouble")
    val SchemaString = new SchemaInstruction(11, "SchemaString")

    val SchemaClassName      = new SchemaInstruction(12, "SchemaClassName")
    val SchemaClassPointer   = new SchemaInstruction(13, "SchemaClassPointer")
    val SchemaClassFieldName = new SchemaInstruction(14, "SchemaClassFieldName")
    val SchemaClassEnd       = new SchemaInstruction(15, "SchemaClassEnd")
    val SchemaClassReference = new SchemaInstruction(16, "SchemaClassReference")

    val SchemaPointer = new SchemaInstruction(17, "SchemaPointer")

    val SchemaSequence = new SchemaInstruction(18, "SchemaSequence")
  }

  /////////////////////////////////////////////////// class to use when no My[TYPE] is supplied

  class Generic(val fields: Map[String, Any]) {
    def apply(field: String): Any = fields(field)
    override def toString() = s"""Generic(Map(${fields.map({case (k, v) => "\"" + k + "\"" + " -> " + v.toString}).mkString(", ")}))"""
  }

  object Generic {
    def apply(fields: Map[String, Any]) = new Generic(fields)
    def unapply(x: Generic) = Some(x.fields)
  }

  /////////////////////////////////////////////////// factories for various types

  trait Factory[+TYPE] {
    def apply(byteBuffer: ByteBuffer): TYPE
  }

  class BoolFactory extends Factory[Boolean] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.get != 0
  }

  class CharFactory extends Factory[Byte] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.get
  }

  class UCharFactory extends Factory[Short] {
    def apply(byteBuffer: ByteBuffer) = {
      val out = byteBuffer.get
      if (out < 0)
        (2 * (java.lang.Byte.MAX_VALUE.toShort + 1) + out).toShort
      else
        out.toShort
    }
  }

  class ShortFactory extends Factory[Short] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.getShort
  }

  class UShortFactory extends Factory[Int] {
    def apply(byteBuffer: ByteBuffer) = {
      val out = byteBuffer.getShort
      if (out < 0)
        2 * (java.lang.Short.MAX_VALUE.toInt + 1) + out.toInt
      else
        out.toInt
    }
  }

  class IntFactory extends Factory[Int] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.getInt
  }

  class UIntFactory extends Factory[Long] {
    def apply(byteBuffer: ByteBuffer) = {
      val out = byteBuffer.getInt
      if (out < 0)
        2L * (java.lang.Integer.MAX_VALUE.toLong + 1L) + out.toLong
      else
        out.toLong
    }
  }

  class LongFactory extends Factory[Long] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.getLong
  }

  class ULongFactory extends Factory[Double] {
    def apply(byteBuffer: ByteBuffer) = {
      val out = byteBuffer.getLong
      if (out < 0)
        2.0 * (java.lang.Long.MAX_VALUE.toDouble + 1.0) + out.toDouble
      else
        out.toDouble
    }
  }

  class FloatFactory extends Factory[Float] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.getFloat
  }

  class DoubleFactory extends Factory[Double] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.getDouble
  }

  class BytesFactory extends Factory[Array[Byte]] {
    def apply(byteBuffer: ByteBuffer) = {
      val size = byteBuffer.getInt
      val out = Array.fill[Byte](size)(0)
      byteBuffer.get(out)
      out
    }
  }

  trait ClassFactory[TYPE] extends Factory[TYPE]

  class OptionFactory[TYPE](factory: Factory[TYPE]) extends Factory[Option[TYPE]] {
    def apply(byteBuffer: ByteBuffer) = {
      val discriminant = byteBuffer.get
      if (discriminant == 0)
        None
      else
        Some(factory(byteBuffer))
    }
  }

  class SequenceFactory[TYPE](factory: Factory[TYPE], builder: Builder[TYPE, Iterable[TYPE]]) extends Factory[Iterable[TYPE]] {
    def apply(byteBuffer: ByteBuffer) = {
      val size = byteBuffer.getInt
      builder.sizeHint(size)
      0 until size foreach {i => builder += factory(byteBuffer)}
      builder.result
    }
  }

  class ArrayFactory[TYPE : ClassTag](factory: Factory[TYPE]) extends Factory[Array[TYPE]] {
    def apply(byteBuffer: ByteBuffer) = {
      val size = byteBuffer.getInt
      val out = Array.fill[TYPE](size)(null.asInstanceOf[TYPE])
      0 until size foreach {i => out(i) = factory(byteBuffer)}
      out
    }
  }

  /////////////////////////////////////////////////// helper functions for user's constructors

  class IntToEnum[ENUM <: Enumeration](val enumeration: ENUM) extends Function1[Int, ENUM#Value] {
    def apply(x: Int) = enumeration.apply(x)
  }

  class BytesToString(val charset: String = "US-ASCII") extends Function1[Array[Byte], String] {
    // FIXME: it might be more efficient to get a Charset object from that string, first.
    def apply(x: Array[Byte]) = new String(x, charset)
  }

  class BytesToEnum[ENUM <: Enumeration](val enumeration: ENUM) extends Function1[Array[Byte], ENUM#Value] {
    def apply(x: Array[Byte]) = enumeration.withName(new String(x))
  }

  trait My[TYPE] extends Serializable {
    def apply(factories: List[(String, Factory[_])]): ClassFactory[TYPE]
  }

  class MyGeneric extends My[Generic] {
    def apply(factories: List[(String, Factory[_])]) = new ClassFactory[Generic] {
      def apply(byteBuffer: ByteBuffer) =
        new Generic(factories.map({case (n, f) => (n, f(byteBuffer))}).toMap)
    }
  }

  object My {
    def apply[TYPE]: My[TYPE] = macro materializeImpl[TYPE]

    def materializeImpl[TYPE](c: Context)(implicit t: c.WeakTypeTag[TYPE]): c.Expr[My[TYPE]] = {
      import c.universe._
      val dataClass = weakTypeOf[TYPE]

      val constructorParams = dataClass.declarations.collectFirst {
        case m: MethodSymbol if (m.isPrimaryConstructor) => m
      }.get.paramss.head

      val holderVars = List.newBuilder[Tree]
      val fillerFuncs = List.newBuilder[Tree]
      val getHolders = List.newBuilder[Tree]

      constructorParams.foreach {param =>
        val nameString = param.asTerm.name.decodedName.toString
        val holderName = stringToTermName("holder_" + nameString)
        val holderType = param.typeSignature

        val holderVar = q"""var $holderName: $holderType = null.asInstanceOf[$holderType]"""

        val fillerFunc = q"""$nameString -> {x: $holderType => $holderName = x}.asInstanceOf[Any => Unit]"""

        val getHolder = q"""$holderName"""

        holderVars += holderVar
        fillerFuncs += fillerFunc
        getHolders += getHolder
      }
      
      val out = c.Expr[My[TYPE]](q"""
        import java.nio.ByteBuffer
        import org.dianahep.scaroot.flatreader._

        new My[$dataClass] {
          // What you know at compile time...
          ..${holderVars.result}

          val fillerFunc = Map(..${fillerFuncs.result})

          def apply(factories: List[(String, Factory[_])]) = {
            // What you know after the schema has been loaded...
            var i = 0
            val numFactories = factories.size
            val fillers = factories.toArray map {case (fieldName, factory) =>
              fillerFunc.get(fieldName) match {
                case Some(f) => {byteBuffer: ByteBuffer => f(factory(byteBuffer))}
                case None => {byteBuffer: ByteBuffer => factory(byteBuffer); ()}
              }
            }

            new ClassFactory[$dataClass] {
              def apply(byteBuffer: ByteBuffer) = {
                // What has to happen quickly in a tight loop...
                i = 0
                while (i < numFactories) {
                  fillers(i)
                  i += 1
                }
                new $dataClass(..${getHolders.result})
              }
            }
          }
        }
      """)
      println(out)
      out
    }
  }
}
