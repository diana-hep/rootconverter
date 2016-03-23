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
import scala.reflect.runtime.universe.Type

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

  case object BoolFactory extends Factory[Boolean] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.get != 0
  }

  case object CharFactory extends Factory[Char] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.get.toChar
  }

  case object ByteFactory extends Factory[Byte] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.get
  }

  case object UByteFactory extends Factory[Short] {
    def apply(byteBuffer: ByteBuffer) = {
      val out = byteBuffer.get
      if (out < 0)
        (2 * (java.lang.Byte.MAX_VALUE.toShort + 1) + out).toShort
      else
        out.toShort
    }
  }

  case object ShortFactory extends Factory[Short] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.getShort
  }

  case object UShortFactory extends Factory[Int] {
    def apply(byteBuffer: ByteBuffer) = {
      val out = byteBuffer.getShort
      if (out < 0)
        2 * (java.lang.Short.MAX_VALUE.toInt + 1) + out.toInt
      else
        out.toInt
    }
  }

  case object IntFactory extends Factory[Int] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.getInt
  }

  case object UIntFactory extends Factory[Long] {
    def apply(byteBuffer: ByteBuffer) = {
      val out = byteBuffer.getInt
      if (out < 0)
        2L * (java.lang.Integer.MAX_VALUE.toLong + 1L) + out.toLong
      else
        out.toLong
    }
  }

  case object LongFactory extends Factory[Long] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.getLong
  }

  case object ULongFactory extends Factory[Double] {
    def apply(byteBuffer: ByteBuffer) = {
      val out = byteBuffer.getLong
      if (out < 0)
        2.0 * (java.lang.Long.MAX_VALUE.toDouble + 1.0) + out.toDouble
      else
        out.toDouble
    }
  }

  case object FloatFactory extends Factory[Float] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.getFloat
  }

  case object DoubleFactory extends Factory[Double] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.getDouble
  }

  case object BytesFactory extends Factory[Array[Byte]] {
    def apply(byteBuffer: ByteBuffer) = {
      val size = byteBuffer.getInt
      val out = Array.fill[Byte](size)(0)
      byteBuffer.get(out)
      out
    }
  }

  case class OptionFactory[TYPE : ClassTag](factory: Factory[TYPE]) extends Factory[Option[TYPE]] {
    def apply(byteBuffer: ByteBuffer) = {
      val discriminant = byteBuffer.get
      if (discriminant == 0)
        None
      else
        Some(factory(byteBuffer))
    }
    override def toString() = s"OptionFactory[${classTag[TYPE].toString}]($factory)"
  }

  class SequenceFactory[TYPE : ClassTag](val factory: Factory[TYPE], builder: => Builder[TYPE, Iterable[TYPE]]) extends Factory[Iterable[TYPE]] {
    def apply(byteBuffer: ByteBuffer) = {
      val size = byteBuffer.getInt
      builder.sizeHint(size)
      0 until size foreach {i => builder += factory(byteBuffer)}
      builder.result
    }
    override def toString() = s"SequenceFactory[${classTag[TYPE].toString}]($factory)"
  }
  object SequenceFactory {
    def apply[TYPE : ClassTag](factory: Factory[TYPE], builder: => Builder[TYPE, Iterable[TYPE]]) = new SequenceFactory[TYPE](factory, builder)
  }

  case class ArrayFactory[TYPE : ClassTag](factory: Factory[TYPE]) extends Factory[Array[TYPE]] {
    def apply(byteBuffer: ByteBuffer) = {
      val size = byteBuffer.getInt
      val out = Array.fill[TYPE](size)(null.asInstanceOf[TYPE])
      0 until size foreach {i => out(i) = factory(byteBuffer)}
      out
    }
    override def toString() = s"ArrayFactory[${classTag[TYPE].toString}]($factory)"
  }

  abstract class ClassFactory[TYPE : ClassTag](val factories: List[(String, Factory[_])]) extends Factory[TYPE] {
    override def toString() = s"ClassFactory[${classTag[TYPE].toString}]($factories)"
  }

  case class GenericFactory(override val factories: List[(String, Factory[_])]) extends ClassFactory[Generic](factories) {
    def apply(byteBuffer: ByteBuffer) =
      new Generic(factories.map({case (n, f) => (n, f(byteBuffer))}).toMap)
  }

  /////////////////////////////////////////////////// helper functions for user's constructors

  trait My[TYPE] {
    def fieldTypes: List[(String, Type)]
    def apply(factories: List[(String, Factory[_])]): ClassFactory[TYPE]
  }
  object My {
    def apply[TYPE]: My[TYPE] = macro applyImpl[TYPE]

    def applyImpl[TYPE : c.WeakTypeTag](c: Context): c.Expr[My[TYPE]] = {
      import c.universe._
      val dataClass = weakTypeOf[TYPE]

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
        import scala.reflect.runtime.universe._
        import org.dianahep.scaroot.flatreader._

        new My[$dataClass] {
          // What you know at compile-time...
          val fieldTypes = List(..${fieldTypes.result})

          def apply(factories: List[(String, Factory[_])]) =
            new ClassFactory[$dataClass](factories) {
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
}
