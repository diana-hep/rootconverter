package org.dianahep.scaroot

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

package reader {
  // Numerical enums from C++ that can be matched in case statements.
  // Also used in a stack to build the schema used at runtime.
  // Must be kept in-sync with scaroot-reader/src/main/cpp/datawalker.h!

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

  class GenericClass(val name: String)(fieldNames: String*)(fieldValues: Any*) {
    private val lookup = (fieldNames zip fieldValues).toMap
    def apply(field: String): Any = lookup(field)
  }

  // Custom interpreters for data. (Use '# for arrays.)
  
  sealed trait Customization {
    private[reader] def escape(raw: String): String = {
      import scala.reflect.runtime.universe._
      Literal(Constant(raw)).toString
    }
    private[reader] def customClassName = getClass.getName.split('.').last
  }

  case class Custom[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Customization {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}$customClassName[${classTag[TYPE].runtimeClass.getName}]"""
  }

  class CustomClass[TYPE : ClassTag : SchemaClassMaker](val in: List[String] = Nil, val is: Option[String] = None) extends Customization {
    def ::(x: String) = CustomClass(x :: in, is)
    def ::(x: Symbol) = CustomClass(x.name :: in, is)
    def named(x: String) = CustomClass(in, Some(x))
    def named(x: Symbol) = CustomClass(in, Some(x.name))
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}CustomClass[${classTag[TYPE].runtimeClass.getName}]${is.map(" named " + escape(_)).mkString}"""

    val schemaClassMaker = implicitly[SchemaClassMaker[TYPE]]
  }
  object CustomClass {
    def apply[TYPE : ClassTag : SchemaClassMaker] = new CustomClass[TYPE](Nil, None)
    def apply[TYPE : ClassTag : SchemaClassMaker](in: List[String] = Nil, is: Option[String] = None) = new CustomClass[TYPE](in, is)
    def unapply[TYPE](x: CustomClass[TYPE]) = Some((x.in, x.is))
  }

  case class CustomPointer[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Customization {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}$customClassName[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomSequence[BUILDER <: Builder[_, _] : ClassTag](f: Int => BUILDER, in: List[String] = Nil) extends Customization {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}$customClassName[${classTag[BUILDER].runtimeClass.getName}]"""
  }

  // Immutable schemas that can extract and convert data using interpreters.

  sealed trait Schema[TYPE] {
    def interpret(data: Pointer): TYPE
  }

  object Schema {
    def apply[TYPE](treeWalker: Pointer, customizations: Seq[Customization] = Nil, tmp: CustomClass[TYPE]): Schema[TYPE] = {
      sealed trait StackElement
      case class S(schemaInstruction: Int, data: Pointer) extends StackElement {
        override def toString() = schemaInstruction match {
          case SchemaInstruction.SchemaBool() => s"S(SchemaBool, $data)"
          case SchemaInstruction.SchemaChar() => s"S(SchemaChar, $data)"
          case SchemaInstruction.SchemaUChar() => s"S(SchemaUChar, $data)"
          case SchemaInstruction.SchemaShort() => s"S(SchemaShort, $data)"
          case SchemaInstruction.SchemaUShort() => s"S(SchemaUShort, $data)"
          case SchemaInstruction.SchemaInt() => s"S(SchemaInt, $data)"
          case SchemaInstruction.SchemaUInt() => s"S(SchemaUInt, $data)"
          case SchemaInstruction.SchemaLong() => s"S(SchemaLong, $data)"
          case SchemaInstruction.SchemaULong() => s"S(SchemaULong, $data)"
          case SchemaInstruction.SchemaFloat() => s"S(SchemaFloat, $data)"
          case SchemaInstruction.SchemaDouble() => s"S(SchemaDouble, $data)"
          case SchemaInstruction.SchemaString() => s"S(SchemaString, $data)"
          case SchemaInstruction.SchemaClassName() => s"S(SchemaClassName, ${data.getString(0)})"
          case SchemaInstruction.SchemaClassPointer() => s"S(SchemaClassPointer, $data)"
          case SchemaInstruction.SchemaClassFieldName() => s"S(SchemaClassFieldName, ${data.getString(0)})"
          case SchemaInstruction.SchemaClassEnd() => s"S(SchemaClassEnd, $data)"
          case SchemaInstruction.SchemaClassReference() => s"S(SchemaClassReference, $data)"
          case SchemaInstruction.SchemaPointer() => s"S(SchemaPointer, $data)"
          case SchemaInstruction.SchemaSequence() => s"S(SchemaSequence, $data)"
        }
      }
      case class F(name: String, schema: Schema[_]) extends StackElement

      var stack: List[StackElement] = Nil
      var result: Schema[TYPE] = null

      object schemaBuilder extends RootReaderCPPLibrary.SchemaBuilder {
        def apply(schemaInstruction: Int, data: Pointer) {
          stack = S(schemaInstruction, data) :: stack

          println(stack)

          stack match {
            case S(SchemaInstruction.SchemaBool(), _) :: S(SchemaInstruction.SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaBool()) :: rest

            case S(SchemaInstruction.SchemaChar(), _) :: S(SchemaInstruction.SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaChar()) :: rest

            case S(SchemaInstruction.SchemaUChar(), _) :: S(SchemaInstruction.SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaUChar()) :: rest

            case S(SchemaInstruction.SchemaShort(), _) :: S(SchemaInstruction.SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaShort()) :: rest

            case S(SchemaInstruction.SchemaUShort(), _) :: S(SchemaInstruction.SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaUShort()) :: rest

            case S(SchemaInstruction.SchemaInt(), _) :: S(SchemaInstruction.SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaInt()) :: rest

            case S(SchemaInstruction.SchemaUInt(), _) :: S(SchemaInstruction.SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaUInt()) :: rest

            case S(SchemaInstruction.SchemaLong(), _) :: S(SchemaInstruction.SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaLong()) :: rest

            case S(SchemaInstruction.SchemaULong(), _) :: S(SchemaInstruction.SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaULong()) :: rest

            case S(SchemaInstruction.SchemaFloat(), _) :: S(SchemaInstruction.SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaFloat()) :: rest

            case S(SchemaInstruction.SchemaDouble(), _) :: S(SchemaInstruction.SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaDouble()) :: rest

            case S(SchemaInstruction.SchemaString(), _) :: S(SchemaInstruction.SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaString()) :: rest

            case S(SchemaInstruction.SchemaClassEnd(), _) :: rest1 =>
              stack = rest1

              var fields: List[(String, Schema[_])] = Nil
              while (stack.head.isInstanceOf[F]) stack match {
                case F(fieldName, schema) :: rest2 =>
                  stack = rest2
                  fields = (fieldName, schema) :: fields
              }

              println("fields", fields)

              stack match {
                case S(SchemaInstruction.SchemaClassPointer(), dataProvider) :: S(SchemaInstruction.SchemaClassName(), className) :: S(SchemaInstruction.SchemaClassFieldName(), fieldName) :: rest3 =>
                  stack = F(fieldName.getString(0), tmp.schemaClassMaker(dataProvider, className.getString(0), fields)) :: rest3

                case S(SchemaInstruction.SchemaClassPointer(), dataProvider) :: S(SchemaInstruction.SchemaClassName(), className) :: Nil =>
                  stack = Nil
                  result = tmp.schemaClassMaker(dataProvider, className.getString(0), fields)
              }

            case _ =>
          }
        }
      }

      RootReaderCPPLibrary.buildSchema(treeWalker, schemaBuilder)
      result
    }
  }

  case class SchemaCustom[TYPE : ClassTag](interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(data)
    override def toString() = s"${getClass.getName.split('.').last}[${classTag[TYPE].runtimeClass.getName}]"
  }

  case class SchemaBool() extends Schema[Boolean] {
    def interpret(data: Pointer) = data.getByte(0) != 0
    override def toString() = s"${getClass.getName.split('.').last}[Boolean]"
  }

  case class SchemaChar() extends Schema[Byte] {
    def interpret(data: Pointer) = data.getByte(0)
    override def toString() = s"${getClass.getName.split('.').last}[Char]"
  }

  case class SchemaUChar() extends Schema[Short] {
    def interpret(data: Pointer) = { val out = data.getByte(0); (if (out < 0) 2 * (java.lang.Byte.MAX_VALUE.toShort + 1) + out else out).toShort }
    override def toString() = s"${getClass.getName.split('.').last}[UChar]"
  }

  case class SchemaShort() extends Schema[Short] {
    def interpret(data: Pointer) = data.getShort(0)
    override def toString() = s"${getClass.getName.split('.').last}[Short]"
  }

  case class SchemaUShort() extends Schema[Int] {
    def interpret(data: Pointer) = { val out = data.getShort(0); if (out < 0) 2 * (java.lang.Short.MAX_VALUE.toInt + 1) + out.toInt else out.toInt }
    override def toString() = s"${getClass.getName.split('.').last}[UShort]"
  }

  case class SchemaInt() extends Schema[Int] {
    def interpret(data: Pointer) = data.getInt(0)
    override def toString() = s"${getClass.getName.split('.').last}[Int]"
  }

  case class SchemaUInt() extends Schema[Long] {
    def interpret(data: Pointer) = { val out = data.getInt(0); if (out < 0) 2L * (java.lang.Integer.MAX_VALUE.toLong + 1L) + out.toLong else out.toLong }
    override def toString() = s"${getClass.getName.split('.').last}[UInt]"
  }

  case class SchemaLong() extends Schema[Long] {
    def interpret(data: Pointer) = data.getLong(0)
    override def toString() = s"${getClass.getName.split('.').last}[Long]"
  }

  case class SchemaULong() extends Schema[Double] {
    def interpret(data: Pointer) = { val out = data.getLong(0); if (out < 0) 2.0 * (java.lang.Long.MAX_VALUE.toDouble + 1.0) + out.toDouble else out.toDouble }
    override def toString() = s"${getClass.getName.split('.').last}[ULong]"
  }

  case class SchemaFloat() extends Schema[Float] {
    def interpret(data: Pointer) = data.getFloat(0)
    override def toString() = s"${getClass.getName.split('.').last}[Float]"
  }

  case class SchemaDouble() extends Schema[Double] {
    def interpret(data: Pointer) = data.getDouble(0)
    override def toString() = s"${getClass.getName.split('.').last}[Double]"
  }

  case class SchemaString() extends Schema[String] {
    def interpret(data: Pointer) = data.getString(0)
    override def toString() = s"${getClass.getName.split('.').last}[String]"
  }

  trait SchemaClass[TYPE] extends Schema[TYPE] {
    def name: String
    def fields: Map[String, Schema[_]]
  }

  trait SchemaClassMaker[TYPE] {
    def apply(dataProvider: Pointer, className: String, allPossibleFields: List[(String, Schema[_])]): SchemaClass[TYPE]
  }
  object SchemaClassMaker {
    implicit def materialize[TYPE]: SchemaClassMaker[TYPE] = macro materializeImpl[TYPE]

    def materializeImpl[TYPE](c: Context)(implicit t: c.WeakTypeTag[TYPE]): c.Expr[SchemaClassMaker[TYPE]] = {
      import c.universe._
      val dataClass = weakTypeOf[TYPE]

      println("one", dataClass.declarations)

      val constructorParams = dataClass.declarations.collectFirst {
        case m: MethodSymbol if (m.isPrimaryConstructor) => m
      }.get.paramss.head

      println("two", constructorParams)

      val subSchemas = List.newBuilder[ValDef]
      val reprs = List.newBuilder[Tree]
      val fieldPairs = List.newBuilder[Tree]
      val gets = List.newBuilder[Tree]

      println("three")

      constructorParams.foreach {param =>
        val nameString = param.asTerm.name.decodedName.toString
        val name = stringToTermName("f_" + nameString)
        // val NullaryMethodType(tpe) = param.typeSignature
        val tpe = param.typeSignature

        subSchemas += q"""
          private val $name: (Schema[$tpe], Int) = {
            val index = allPossibleFields.indexWhere(_._1 == $nameString)

println("setting up " + $nameString + " " + index.toString)

            if (index == -1)
              throw new IllegalArgumentException("Scala class has field \"" + $nameString + "\", but this field is not in the Avro schema.")
            else
              (allPossibleFields(index)._2.asInstanceOf[Schema[$tpe]], index)
          }"""
        reprs += q"""$nameString + ": " + $name._1.toString"""
        fieldPairs += q"""$nameString -> $name._1"""
        gets += q"""$name._1.interpret(RootReaderCPPLibrary.getData(dataProvider, data, $name._2))"""
      }

      println("four")

      val makeNew =
        // if (dataClass =:= typeOf[GenericClass])
        //   q"new $dataClass($className)(fields.map(_._1))(..${gets.result})"
        // else
          q"new $dataClass(..${gets.result})"

      println("five")

      val out = c.Expr[SchemaClassMaker[TYPE]](q"""
        import com.sun.jna.Pointer
        import org.dianahep.scaroot.reader._

        new SchemaClassMaker[$dataClass] {
          def apply(dataProvider: Pointer, className: String, allPossibleFields: List[(String, Schema[_])]): SchemaClass[$dataClass] =
            new SchemaClass[$dataClass] {
              ..${subSchemas.result}

              def name = className
              def fields = Map(..${fieldPairs.result})

              def interpret(data: Pointer): $dataClass = $makeNew

              override def toString() = "SchemaClass[" + classOf[$dataClass].getName + "](" + List(..${reprs.result}).mkString(", ") + ")"
            }
          }
      """)
      println(out)
      out
    }
  }

  case class SchemaPointer[TYPE : ClassTag](nullable: Schema[TYPE], dataProvider: Pointer) extends Schema[Option[TYPE]] {
    def interpret(data: Pointer): Option[TYPE] = {
      val result = RootReaderCPPLibrary.getData(dataProvider, data, 0)
      if (result == Pointer.NULL)
        None.asInstanceOf[Option[TYPE]]
      else
        Some(nullable.interpret(result)).asInstanceOf[Option[TYPE]]
    }
    override def toString() = s"${getClass.getName.split('.').last}[${classTag[TYPE].runtimeClass.getName}]($nullable)"
  }

  case class SchemaSequence[ITEMS, TYPE : ClassTag](items: Schema[ITEMS], dataProvider: Pointer, builder: Int => Builder[ITEMS, TYPE] = (SchemaSequence.defaultBuilder _)) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = {
      val size = RootReaderCPPLibrary.getDataSize(dataProvider, data)
      val listBuilder = builder(size)
      var index = 0
      while (index < size) {
        val subdata = RootReaderCPPLibrary.getData(dataProvider, data, index)
        listBuilder += items.interpret(subdata)
        index += 1
      }
      listBuilder.result
    }
    override def toString() = s"${getClass.getName.split('.').last}[${classTag[TYPE].runtimeClass.getName}]($items)"
  }
  object SchemaSequence {
    def defaultBuilder[ITEMS, Seq[ITEMS], BUILDER <: Builder[ITEMS, Seq[ITEMS]]](size: Int) = {
      val builder =
        if (size < 10)    // FIXME: optimize this
          List.newBuilder[ITEMS].asInstanceOf[Builder[ITEMS, Seq[ITEMS]]]
        else
          Vector.newBuilder[ITEMS].asInstanceOf[Builder[ITEMS, Seq[ITEMS]]]
      builder.sizeHint(size)
      builder
    }
  }

  // Example overrides.

  object Example {
    val byteBuffer = {data: Pointer =>
      var end = 0L
      while (data.getByte(end) != 0)
        end += 1L
      data.getByteBuffer(0, end)
    }

    val byteArray = {data: Pointer =>
      var end = 0
      while (data.getByte(end) != 0)
        end += 1
      data.getByteArray(0, end)
    }

    def fixed(size: Int) = {data: Pointer => data.getByteBuffer(0, size)}
    def enum[ENUM <: Enumeration](enumeration: ENUM) = {data: Pointer => enumeration.apply(data.getInt(0))}
  }
}
