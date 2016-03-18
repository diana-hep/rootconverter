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
    val SchemaClassFieldName = new SchemaInstruction(13, "SchemaClassFieldName")
    val SchemaClassField     = new SchemaInstruction(14, "SchemaClassFieldName")
    val SchemaClassEnd       = new SchemaInstruction(15, "SchemaClassEnd")
    val SchemaClassReference = new SchemaInstruction(16, "SchemaClassReference")

    val SchemaPointer = new SchemaInstruction(17, "SchemaPointer")

    val SchemaSequence = new SchemaInstruction(18, "SchemaSequence")
  }

  // Default interpreters for data.

  object Default {
    val bool = {value: Pointer => value.getByte(0) != 0}
    val char = {value: Pointer => value.getByte(0)}
    val uchar = {value: Pointer => val out = value.getByte(0); if (out < 0) 2 * (java.lang.Byte.MAX_VALUE.toShort + 1) + out.toShort else out.toShort}
    val short = {value: Pointer => value.getShort(0)}
    val ushort = {value: Pointer => val out = value.getShort(0); if (out < 0) 2 * (java.lang.Short.MAX_VALUE.toInt + 1) + out.toInt else out.toInt}
    val int = {value: Pointer => value.getInt(0)}
    val uint = {value: Pointer => val out = value.getInt(0); if (out < 0) 2 * (java.lang.Integer.MAX_VALUE.toLong + 1) + out.toLong else out.toLong}
    val long = {value: Pointer => value.getLong(0)}
    val ulong = {value: Pointer => val out = value.getLong(0); if (out < 0) 2 * (java.lang.Long.MAX_VALUE.toDouble + 1) + out.toDouble else out.toDouble}
    val float = {value: Pointer => value.getFloat(0)}
    val double = {value: Pointer => value.getDouble(0)}
    val string = {value: Pointer => value.getString(0)}

    val byteBuffer = {value: Pointer =>
      var end = 0L
      while (value.getByte(end) != 0)
        end += 1L
      value.getByteBuffer(0, end)
    }

    val byteArray = {value: Pointer =>
      var end = 0
      while (value.getByte(end) != 0)
        end += 1
      value.getByteArray(0, end)
    }

    def fixed(size: Int) = {value: Pointer => value.getByteBuffer(0, size)}
    def enum[ENUM <: Enumeration](enumeration: ENUM) = {value: Pointer => enumeration.apply(value.getInt(0))}

    def sequence[ITEMS, Seq[ITEMS], BUILDER <: Builder[ITEMS, Seq[ITEMS]]] = {size: Int =>
      val builder =
        if (size < 10)    // FIXME: optimize this
          List.newBuilder[ITEMS].asInstanceOf[Builder[ITEMS, Seq[ITEMS]]]
        else
          Vector.newBuilder[ITEMS].asInstanceOf[Builder[ITEMS, Seq[ITEMS]]]
      builder.sizeHint(size)
      builder
    }
  }

  class GenericClass(val name: String)(fieldNames: String*)(fieldValues: Any*) {
    private val lookup = (fieldNames zip fieldValues).toMap
    def apply(field: String): Any = lookup(field)
  }

  // Custom interpreters for data. (Use '# for arrays.)
  
  sealed trait Custom {
    private[reader] def escape(raw: String): String = {
      import scala.reflect.runtime.universe._
      Literal(Constant(raw)).toString
    }
    private[reader] def customClassName = getClass.getName.split('.').last
  }

  case class CustomBool[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}$customClassName[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomChar[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}$customClassName[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomUChar[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}$customClassName[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomShort[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}$customClassName[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomUShort[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}$customClassName[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomInt[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}$customClassName[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomUInt[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}$customClassName[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomLong[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}$customClassName[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomULong[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}$customClassName[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomFloat[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}$customClassName[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomDouble[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}$customClassName[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomString[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}$customClassName[${classTag[TYPE].runtimeClass.getName}]"""
  }

  class CustomClass[TYPE : ClassTag : SchemaClassMaker](val in: List[String] = Nil, val is: Option[String] = None) extends Custom {
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

  case class CustomPointer[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}$customClassName[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomSequence[BUILDER <: Builder[_, _] : ClassTag](f: Int => BUILDER, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}$customClassName[${classTag[BUILDER].runtimeClass.getName}]"""
  }

  // Immutable schemas that can extract and convert data using interpreters.

  sealed trait Schema[TYPE] {
    def interpret(data: Pointer): TYPE
    private[reader] def schemaClassName = getClass.getName.split('.').last
  }

  object Schema {
    def schema[TYPE](treeWalker: Pointer, tmp: CustomClass[TYPE])(implicit customizations: Seq[Custom] = Nil): Schema[TYPE] = {
      import SchemaInstruction._
      sealed trait StackElement
      case class S(schemaInstruction: Int, data: Pointer) extends StackElement
      case class F(name: String, schema: Schema[_]) extends StackElement

      var stack: List[StackElement] = Nil
      var result: Schema[TYPE] = null

      object schemaBuilder extends RootReaderCPPLibrary.SchemaBuilder {
        def apply(schemaInstruction: Int, data: Pointer) {
          stack = S(schemaInstruction, data) :: stack

          stack match {
            case S(SchemaBool(), _) :: S(SchemaClassField(), walker) :: S(SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaBool(walker, interpreter = Default.bool)) :: rest

            case S(SchemaChar(), _) :: S(SchemaClassField(), walker) :: S(SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaChar(walker, interpreter = Default.char)) :: rest

            case S(SchemaUChar(), _) :: S(SchemaClassField(), walker) :: S(SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaUChar(walker, interpreter = Default.uchar)) :: rest

            case S(SchemaShort(), _) :: S(SchemaClassField(), walker) :: S(SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaShort(walker, interpreter = Default.short)) :: rest

            case S(SchemaUShort(), _) :: S(SchemaClassField(), walker) :: S(SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaUShort(walker, interpreter = Default.ushort)) :: rest

            case S(SchemaInt(), _) :: S(SchemaClassField(), walker) :: S(SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaInt(walker, interpreter = Default.int)) :: rest

            case S(SchemaUInt(), _) :: S(SchemaClassField(), walker) :: S(SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaUInt(walker, interpreter = Default.uint)) :: rest

            case S(SchemaLong(), _) :: S(SchemaClassField(), walker) :: S(SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaLong(walker, interpreter = Default.long)) :: rest

            case S(SchemaULong(), _) :: S(SchemaClassField(), walker) :: S(SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaULong(walker, interpreter = Default.ulong)) :: rest

            case S(SchemaFloat(), _) :: S(SchemaClassField(), walker) :: S(SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaFloat(walker, interpreter = Default.float)) :: rest

            case S(SchemaDouble(), _) :: S(SchemaClassField(), walker) :: S(SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaDouble(walker, interpreter = Default.double)) :: rest

            case S(SchemaString(), _) :: S(SchemaClassField(), walker) :: S(SchemaClassFieldName(), fieldName) :: rest =>
              stack = F(fieldName.getString(0), new SchemaString(walker, interpreter = Default.string)) :: rest

            case S(SchemaClassEnd(), _) :: rest1 =>
              stack = rest1

              def popFields(): List[(String, Schema[_])] = stack match {
                case F(fieldName, schema) :: rest2 =>
                  stack = rest2
                  (fieldName, schema) :: popFields()
                case _ =>
                  Nil
              }
              val fields = popFields()

              stack match {
                case S(SchemaClassName(), className) :: S(SchemaClassField(), walker) :: S(SchemaClassFieldName(), fieldName) :: rest3 =>
                  stack = F(fieldName.getString(0), tmp.schemaClassMaker(walker, className.getString(0), fields)) :: rest3

                case S(SchemaClassName(), className) :: Nil =>
                  result = tmp.schemaClassMaker(Pointer.NULL, className.getString(0), fields)
              }

            case _ =>
          }
        }
      }

      RootReaderCPPLibrary.buildSchema(treeWalker, schemaBuilder)
      result
    }
  }

  class SchemaBool[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"$schemaClassName[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaChar[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"$schemaClassName[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaUChar[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"$schemaClassName[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaShort[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"$schemaClassName[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaUShort[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"$schemaClassName[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaInt[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"$schemaClassName[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaUInt[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"$schemaClassName[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaLong[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"$schemaClassName[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaULong[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"$schemaClassName[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaFloat[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"$schemaClassName[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaDouble[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"$schemaClassName[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaString[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"$schemaClassName[${classTag[TYPE].runtimeClass.getName}]"
  }

  trait SchemaClass[TYPE] extends Schema[TYPE] {
    def name: String
    def fields: Map[String, Schema[_]]
  }

  trait SchemaClassMaker[TYPE] {
    def apply(walker: Pointer, className: String, allPossibleFields: List[(String, Schema[_])]): SchemaClass[TYPE]
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
            if (index == -1)
              throw new IllegalArgumentException("Scala class has field \"" + $nameString + "\", but this field is not in the Avro schema.")
            else
              (allPossibleFields(index)._2.asInstanceOf[Schema[$tpe]], index)
          }"""
        reprs += q"""$nameString + ": " + $name._1.toString"""
        fieldPairs += q"""$nameString -> $name._1"""
        // gets += q"""$name._1.interpret(RootReaderCPPLibrary.getData(walker, data, $name._2, Pointer.NULL))"""
        gets += q"""$name._1.interpret(Pointer.NULL)"""
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
          def apply(walker: Pointer, className: String, allPossibleFields: List[(String, Schema[_])]): SchemaClass[$dataClass] =
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

  class SchemaPointer[TYPE : ClassTag](walker: Pointer, nullable: Schema[TYPE]) extends Schema[Option[TYPE]] {
    def interpret(data: Pointer): Option[TYPE] = {
      val result = RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL)
      if (result == Pointer.NULL)
        None.asInstanceOf[Option[TYPE]]
      else
        Some(nullable.interpret(result)).asInstanceOf[Option[TYPE]]
    }
    override def toString() = s"$schemaClassName[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaSequence[ITEMS, TYPE : ClassTag](walker: Pointer, dim: Pointer, builder: Int => Builder[ITEMS, TYPE], items: Schema[ITEMS]) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = {
      val size = RootReaderCPPLibrary.getDataSize(walker, data, dim)
      val listBuilder = builder(size)
      var index = 0
      while (index < size) {
        val subdata = RootReaderCPPLibrary.getData(walker, data, index, dim)
        listBuilder += items.interpret(subdata)
        index += 1
      }
      listBuilder.result
    }
    override def toString() = s"$schemaClassName[${classTag[TYPE].runtimeClass.getName}]($items)"
  }
}
