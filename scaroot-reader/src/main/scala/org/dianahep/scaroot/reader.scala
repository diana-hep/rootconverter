package org.dianahep.scaroot

import scala.collection.mutable.Builder
import scala.language.experimental.macros
import scala.language.higherKinds
import scala.reflect.classTag
import scala.reflect.ClassTag
// import scala.reflect.macros.blackbox.Context
import scala.reflect.macros.Context

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
    val SchemaClassEnd       = new SchemaInstruction(14, "SchemaClassEnd")
    val SchemaClassReference = new SchemaInstruction(15, "SchemaClassReference")

    val SchemaPointer = new SchemaInstruction(16, "SchemaPointer")

    val SchemaSequence = new SchemaInstruction(17, "SchemaSequence")
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
  }

  case class CustomBool[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomChar[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomUChar[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomShort[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomUShort[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomInt[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomUInt[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomLong[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomULong[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomFloat[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomDouble[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomString[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"""
  }

  class CustomClass[TYPE : ClassTag](val in: List[String] = Nil, val is: Option[String] = None) extends Custom {
    def ::(x: String) = CustomClass(x :: in, is)
    def ::(x: Symbol) = CustomClass(x.name :: in, is)
    def named(x: String) = CustomClass(in, Some(x))
    def named(x: Symbol) = CustomClass(in, Some(x.name))
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}CustomClass[${classTag[TYPE].runtimeClass.getName}]${is.map(" named " + escape(_)).mkString}"""
  }
  object CustomClass {
    def apply[TYPE : ClassTag] = new CustomClass[TYPE](Nil, None)
    def apply[TYPE : ClassTag](in: List[String] = Nil, is: Option[String] = None) = new CustomClass[TYPE](in, is)
    def unapply[TYPE](x: CustomClass[TYPE]) = Some((x.in, x.is))
  }

  case class CustomPointer[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomSequence[BUILDER <: Builder[_, _] : ClassTag](f: Int => BUILDER, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}${getClass.getName}[${classTag[BUILDER].runtimeClass.getName}]"""
  }

  // Immutable schemas that can extract and convert data using interpreters.

  sealed trait Schema[TYPE] {
    def interpret(data: Pointer): TYPE
  }

  class SchemaBool[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaChar[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaUChar[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaShort[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaUShort[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaInt[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaUInt[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaLong[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaULong[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaFloat[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaDouble[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaString[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL))
    override def toString() = s"${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"
  }

  trait SchemaClass[TYPE] extends Schema[TYPE] {
    def name: String
    def fields: Map[String, Schema[_]]
  }

  class SchemaPointer[TYPE : ClassTag](walker: Pointer, nullable: Schema[TYPE]) extends Schema[Option[TYPE]] {
    def interpret(data: Pointer): Option[TYPE] = {
      val result = RootReaderCPPLibrary.getData(walker, data, 0, Pointer.NULL)
      if (result == Pointer.NULL)
        None.asInstanceOf[Option[TYPE]]
      else
        Some(nullable.interpret(result)).asInstanceOf[Option[TYPE]]
    }
    override def toString() = s"${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]"
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
    override def toString() = s"${getClass.getName}[${classTag[TYPE].runtimeClass.getName}]($items)"
  }

  object Schema {
    def schemaClassFrom[TYPE](walker: Pointer, className: String, fields: List[(String, Schema[_])]): SchemaClass[TYPE] = macro schemaClassFromImpl[TYPE]

    def schemaClassFromImpl[TYPE](c: Context)(walker: c.Expr[Pointer], className: c.Expr[String], fields: c.Expr[List[(String, Schema[_])]])(implicit t: c.WeakTypeTag[TYPE]): c.Expr[SchemaClass[TYPE]] = {
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
        gets += q"""$name._1.interpret(RootReaderCPPLibrary.getData(walker, data, $name._2, Pointer.NULL))"""
      }

      println("four")

      val makeNew =
        if (dataClass =:= typeOf[GenericClass])
          q"new $dataClass($className)(fields.map(_._1))(..${gets.result})"
        else
          q"new $dataClass(..${gets.result})"

      println("five")

      c.Expr[SchemaClass[TYPE]](q"""
        import com.sun.jna.Pointer
        import org.dianahep.scaroot.reader.Schema

        val allPossibleFields = $fields

        new SchemaClass[$dataClass] {
          private val walker: Pointer = $walker

          ..${subSchemas.result}

          def name = $className
          def fields = Map(..${fieldPairs.result})

          def interpret(data: Pointer): $dataClass = $makeNew

          override def toString() = "SchemaClass[" + classOf[$dataClass].getName + "](" + List(..${reprs.result}).mkString(", ") + ")"
        }
      """)
    }
  }
}
