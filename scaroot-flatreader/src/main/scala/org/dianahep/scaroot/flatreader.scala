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
import scala.reflect.runtime.universe.typeOf
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

  /////////////////////////////////////////////////// schemas (type information from ROOT)
  // Note that type information is known only at runtime!

  sealed trait Schema
  object Schema {
    def apply(treeWalker: Pointer): Schema = {
      sealed trait StackElement
      case class I(schemaInstruction: Int, data: Pointer) extends StackElement
      case class S(schema: Schema) extends StackElement

      var stack: List[StackElement] = Nil
      val schemaClasses = mutable.Map[String, Schema]()

      // This function is passed to the TreeWalker, which calls it in
      // a depth-first traversal of the tree. We have to turn that
      // traversal into a tree of nested Schemas.
      object SchemaBuilder extends RootReaderCPPLibrary.SchemaBuilder {
        def apply(schemaInstruction: Int, data: Pointer) {
          stack = I(schemaInstruction, data) :: stack

          // Collapse complete instruction sets into Schema objects.
          var done1 = false
          while (!done1) stack match {
            // Primitives are easy: just convert the instruction into the corresponding Schema.
            case I(SchemaInstruction.SchemaBool(), _)   :: rest  =>  stack = S(SchemaBool) :: rest
            case I(SchemaInstruction.SchemaChar(), _)   :: rest  =>  stack = S(SchemaChar) :: rest
            case I(SchemaInstruction.SchemaUChar(), _)  :: rest  =>  stack = S(SchemaUChar) :: rest
            case I(SchemaInstruction.SchemaShort(), _)  :: rest  =>  stack = S(SchemaShort) :: rest
            case I(SchemaInstruction.SchemaUShort(), _) :: rest  =>  stack = S(SchemaUShort) :: rest
            case I(SchemaInstruction.SchemaInt(), _)    :: rest  =>  stack = S(SchemaInt) :: rest
            case I(SchemaInstruction.SchemaUInt(), _)   :: rest  =>  stack = S(SchemaUInt) :: rest
            case I(SchemaInstruction.SchemaLong(), _)   :: rest  =>  stack = S(SchemaLong) :: rest
            case I(SchemaInstruction.SchemaULong(), _)  :: rest  =>  stack = S(SchemaULong) :: rest
            case I(SchemaInstruction.SchemaFloat(), _)  :: rest  =>  stack = S(SchemaFloat) :: rest
            case I(SchemaInstruction.SchemaDouble(), _) :: rest  =>  stack = S(SchemaDouble) :: rest
            case I(SchemaInstruction.SchemaString(), _) :: rest  =>  stack = S(SchemaString) :: rest

            // Classes are more complex: they're bracketed between SchemaClassName and SchemaClassEnd,
            // with SchemaClassFieldName, <type> pairs in between. Those <types> have already been
            // collapsed down to Schema objects, whether they're primitives or nested classes.
            // (Deterministic pushdown automaton!)
            case I(SchemaInstruction.SchemaClassEnd(), _) :: rest1 =>
              stack = rest1

              // Note: filling the stack reverses the order of the fields, and this operation reverses
              // them back (for the same reason: Lists are filled on the left).
              var fields = List[(String, Schema)]()
              var done2 = false
              while (!done2) stack match {
                case S(schema) :: I(SchemaInstruction.SchemaClassFieldName(), fieldNamePtr) :: rest2 =>
                  stack = rest2
                  fields = (fieldNamePtr.getString(0), schema) :: fields
                case _ =>
                  done2 = true
              }

              stack match {
                // The SchemaClassPointer instruction is for scaroot-directreader.
                // Not used here, but we have to step over it.
                case I(SchemaInstruction.SchemaClassPointer(), _) :: I(SchemaInstruction.SchemaClassName(), classNamePtr) :: rest3 =>
                  // Keep this SchemaClass object in a mutable.Map.
                  val className = classNamePtr.getString(0)
                  val schemaClass = SchemaClass(className, fields)
                  schemaClasses(className) = schemaClass

                  stack = S(schemaClass) :: rest3
              }

            // The depth-first traversal fully defines classes only once; thereafter they're referenced
            // by name. This avoids infinite loops for recursive types (e.g. linked lists or trees).
            case I(SchemaInstruction.SchemaClassReference(), classNamePtr) :: rest =>
              // Now we use the mutable.Map to get that SchemaClass back.
              stack = S(schemaClasses(classNamePtr.getString(0))) :: rest

            // Pointers are wrappers around a <type>, which has already been collapsed to a Schema.
            case S(referent) :: I(SchemaInstruction.SchemaPointer(), _) :: rest =>
              stack = S(SchemaPointer(referent)) :: rest

            // So are sequences.
            case S(content) :: I(SchemaInstruction.SchemaSequence(), _) :: rest =>
              stack = S(SchemaSequence(content)) :: rest

            case _ =>
              // We've collapsed this as much as we can without more instructions.
              done1 = true
          }
        }
      }

      // Now we actually pass our function to the TreeWalker.
      RootReaderCPPLibrary.buildSchema(treeWalker, SchemaBuilder)

      // And pluck off the result (making strong assumptions about the state of the stack:
      // will be a runtime error if it's not a single Schema at the end.
      val S(result) :: Nil = stack
      result
    }
  }

  case object SchemaBool extends Schema
  case object SchemaChar extends Schema
  case object SchemaUChar extends Schema
  case object SchemaShort extends Schema
  case object SchemaUShort extends Schema
  case object SchemaInt extends Schema
  case object SchemaUInt extends Schema
  case object SchemaLong extends Schema
  case object SchemaULong extends Schema
  case object SchemaFloat extends Schema
  case object SchemaDouble extends Schema
  case object SchemaString extends Schema

  case class SchemaClass(name: String, fields: List[(String, Schema)]) extends Schema

  case class SchemaPointer(referent: Schema) extends Schema

  case class SchemaSequence(content: Schema) extends Schema

  /////////////////////////////////////////////////// class to use when no My[TYPE] is supplied

  class Generic(val fields: Map[String, Any]) {
    def apply(field: String): Any = fields(field)
    override def toString() = s"""Generic(Map(${fields.map({case (k, v) => "\"" + k + "\"" + " -> " + v.toString}).mkString(", ")}))"""
  }
  object Generic {
    def apply(fields: Map[String, Any]) = new Generic(fields)
    def unapply(x: Generic) = Some(x.fields)
  }

  /////////////////////////////////////////////////// factories for building data at runtime
  // Note that type information is compiled in!

  trait Factory[+TYPE] {
    def apply(byteBuffer: ByteBuffer): TYPE
  }
  object Factory {
    def apply(schema: Schema,
              myclasses: Map[String, My[_]] = Map[String, My[_]](),
              translation: Map[(Schema, Type), Factory[_]] = defaultTranslation) = {
      FactoryBool
    }

    val defaultTranslation = Map[(Schema, Type), Factory[_]](
      (SchemaBool,  typeOf[Boolean]) -> FactoryBool,
      (SchemaChar,  typeOf[Char])    -> FactoryChar,
      (SchemaUChar, typeOf[Char])    -> FactoryChar,
      (SchemaChar,  typeOf[Byte])    -> FactoryByte,
      (SchemaUChar, typeOf[Short])   -> FactoryUByte
      // ...
    )
  }

  case object FactoryBool extends Factory[Boolean] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.get != 0
  }

  case object FactoryChar extends Factory[Char] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.get.toChar
  }

  case object FactoryByte extends Factory[Byte] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.get
  }

  case object FactoryUByte extends Factory[Short] {
    def apply(byteBuffer: ByteBuffer) = {
      val out = byteBuffer.get
      if (out < 0)
        (2 * (java.lang.Byte.MAX_VALUE.toShort + 1) + out).toShort
      else
        out.toShort
    }
  }

  case object FactoryShort extends Factory[Short] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.getShort
  }

  case object FactoryUShort extends Factory[Int] {
    def apply(byteBuffer: ByteBuffer) = {
      val out = byteBuffer.getShort
      if (out < 0)
        2 * (java.lang.Short.MAX_VALUE.toInt + 1) + out.toInt
      else
        out.toInt
    }
  }

  case object FactoryInt extends Factory[Int] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.getInt
  }

  case object FactoryUInt extends Factory[Long] {
    def apply(byteBuffer: ByteBuffer) = {
      val out = byteBuffer.getInt
      if (out < 0)
        2L * (java.lang.Integer.MAX_VALUE.toLong + 1L) + out.toLong
      else
        out.toLong
    }
  }

  case class FactoryEnumFromInt[ENUM <: Enumeration](enumeration: ENUM) extends Factory[ENUM#Value] {
    def apply(byteBuffer: ByteBuffer) = enumeration.apply(byteBuffer.getInt)
  }

  case object FactoryLong extends Factory[Long] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.getLong
  }

  case object FactoryULong extends Factory[Double] {
    def apply(byteBuffer: ByteBuffer) = {
      val out = byteBuffer.getLong
      if (out < 0)
        2.0 * (java.lang.Long.MAX_VALUE.toDouble + 1.0) + out.toDouble
      else
        out.toDouble
    }
  }

  case object FactoryFloat extends Factory[Float] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.getFloat
  }

  case object FactoryDouble extends Factory[Double] {
    def apply(byteBuffer: ByteBuffer) = byteBuffer.getDouble
  }

  case object FactoryBytes extends Factory[Array[Byte]] {
    def apply(byteBuffer: ByteBuffer) = {
      val size = byteBuffer.getInt
      val out = Array.fill[Byte](size)(0)
      byteBuffer.get(out)
      out
    }
  }

  case class FactoryString(charset: String) extends Factory[String] {
    // FIXME: it might be more efficient to get a Charset object from that string, first.
    def apply(byteBuffer: ByteBuffer) = {
      val size = byteBuffer.getInt
      val out = Array.fill[Byte](size)(0)
      byteBuffer.get(out)
      new String(out, charset)
    }
  }

  case class FactoryEnumFromString[ENUM <: Enumeration](enumeration: ENUM) extends Factory[ENUM#Value] {
    def apply(byteBuffer: ByteBuffer) = {
      val size = byteBuffer.getInt
      val out = Array.fill[Byte](size)(0)
      byteBuffer.get(out)
      enumeration.withName(new String(out))
    }
  }

  case class FactoryOption[TYPE : ClassTag](factory: Factory[TYPE]) extends Factory[Option[TYPE]] {
    def apply(byteBuffer: ByteBuffer) = {
      val discriminant = byteBuffer.get
      if (discriminant == 0)
        None
      else
        Some(factory(byteBuffer))
    }
    override def toString() = s"FactoryOption[${classTag[TYPE].toString}]($factory)"
  }

  class FactorySequence[TYPE : ClassTag](val factory: Factory[TYPE], builder: => Builder[TYPE, Iterable[TYPE]]) extends Factory[Iterable[TYPE]] {
    def apply(byteBuffer: ByteBuffer) = {
      val size = byteBuffer.getInt
      builder.sizeHint(size)
      0 until size foreach {i => builder += factory(byteBuffer)}
      builder.result
    }
    override def toString() = s"FactorySequence[${classTag[TYPE].toString}]($factory)"
  }
  object FactorySequence {
    def apply[TYPE : ClassTag](factory: Factory[TYPE], builder: => Builder[TYPE, Iterable[TYPE]]) = new FactorySequence[TYPE](factory, builder)
  }

  case class FactoryArray[TYPE : ClassTag](factory: Factory[TYPE]) extends Factory[Array[TYPE]] {
    def apply(byteBuffer: ByteBuffer) = {
      val size = byteBuffer.getInt
      val out = Array.fill[TYPE](size)(null.asInstanceOf[TYPE])
      0 until size foreach {i => out(i) = factory(byteBuffer)}
      out
    }
    override def toString() = s"FactoryArray[${classTag[TYPE].toString}]($factory)"
  }

  abstract class FactoryClass[TYPE : ClassTag](val factories: List[(String, Factory[_])]) extends Factory[TYPE] {
    override def toString() = s"FactoryClass[${classTag[TYPE].toString}]($factories)"
  }

  case class FactoryGeneric(override val factories: List[(String, Factory[_])]) extends FactoryClass[Generic](factories) {
    def apply(byteBuffer: ByteBuffer) =
      new Generic(factories.map({case (n, f) => (n, f(byteBuffer))}).toMap)
  }

  /////////////////////////////////////////////////// user's class specification (a factory-factory!)

  trait My[TYPE] {
    def fieldTypes: List[(String, Type)]
    def apply(factories: List[(String, Factory[_])]): FactoryClass[TYPE]
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
        import scala.reflect.runtime.universe.typeOf
        import org.dianahep.scaroot.flatreader._

        new My[$dataClass] {
          // What you know at compile-time...
          val fieldTypes = List(..${fieldTypes.result})

          def apply(factories: List[(String, Factory[_])]) =
            new FactoryClass[$dataClass](factories) {
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
}
