package org.dianahep.scaroot

import scala.collection.mutable
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

  class SchemaElement(index: Int, name: String) {
    def unapply(x: Int) = if (x == index) Some(this) else None
    override def toString() = s"SchemaElement.$name"
  }

  object SchemaElement {
    val SchemaBool   = new SchemaElement(0, "SchemaBool")
    val SchemaChar   = new SchemaElement(1, "SchemaChar")
    val SchemaUChar  = new SchemaElement(2, "SchemaUChar")
    val SchemaShort  = new SchemaElement(3, "SchemaShort")
    val SchemaUShort = new SchemaElement(4, "SchemaUShort")
    val SchemaInt    = new SchemaElement(5, "SchemaInt")
    val SchemaUInt   = new SchemaElement(6, "SchemaUInt")
    val SchemaLong   = new SchemaElement(7, "SchemaLong")
    val SchemaULong  = new SchemaElement(8, "SchemaULong")
    val SchemaFloat  = new SchemaElement(9, "SchemaFloat")
    val SchemaDouble = new SchemaElement(10, "SchemaDouble")
    val SchemaString = new SchemaElement(11, "SchemaString")

    val SchemaClassName      = new SchemaElement(12, "SchemaClassName")
    val SchemaClassDoc       = new SchemaElement(13, "SchemaClassDoc")
    val SchemaClassFieldName = new SchemaElement(14, "SchemaClassFieldName")
    val SchemaClassFieldDoc  = new SchemaElement(15, "SchemaClassFieldDoc")
    val SchemaClassEnd       = new SchemaElement(16, "SchemaClassEnd")
    val SchemaClassReference = new SchemaElement(17, "SchemaClassReference")

    val SchemaPointer = new SchemaElement(18, "SchemaPointer")

    val SchemaSequence = new SchemaElement(19, "SchemaSequence")
  }

  // Default interpreters for data.

  object Default {
    val boolean = {value: Pointer => value.getByte(0) != 0}
    val int =     {value: Pointer => value.getInt(0)}
    val long =    {value: Pointer => value.getLong(0)}
    val float =   {value: Pointer => value.getFloat(0)}
    val double =  {value: Pointer => value.getDouble(0)}
    val string =  {value: Pointer => value.getString(0)}
    val bytes =   {(value: Pointer, size: Int) => value.getByteBuffer(0, size)}

    def array[ITEMS, Seq[ITEMS], BUILDER <: Builder[ITEMS, Seq[ITEMS]]] = {size: Int =>
      val builder =
        if (size < 10)    // FIXME: optimize this
          List.newBuilder[ITEMS].asInstanceOf[Builder[ITEMS, Seq[ITEMS]]]
        else
          Vector.newBuilder[ITEMS].asInstanceOf[Builder[ITEMS, Seq[ITEMS]]]
      builder.sizeHint(size)
      builder
    }

    def fixed(size: Int) = {value: Pointer => value.getByteBuffer(0, size)}
    def enum[ENUM <: Enumeration](enumeration: ENUM) = {value: Pointer => enumeration.apply(value.getInt(0))}
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

  case class CustomArray[BUILDER <: Builder[_, _] : ClassTag](f: Int => BUILDER, in: List[String] = Nil) extends Custom {
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

  class SchemaArray[ITEMS, TYPE : ClassTag](walker: Pointer, dim: Pointer, builder: Int => Builder[ITEMS, TYPE], items: Schema[ITEMS]) extends Schema[TYPE] {
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
    implicit def schemaClassFrom[TYPE](walker: Pointer, fields: List[(String, Schema[_])]): SchemaClass[TYPE] = macro schemaClassFromImpl[TYPE]

    def schemaClassFromImpl[TYPE](c: Context)(walker: c.Expr[Pointer], fields: c.Expr[List[(String, Schema[_])]])(implicit t: c.WeakTypeTag[TYPE]): c.Expr[SchemaClass[TYPE]] = {
      import c.universe._
      val dataClass = weakTypeOf[TYPE]

      val constructorParams = dataClass.declarations.collectFirst {
        case m: MethodSymbol if (m.isPrimaryConstructor) => m
      }.get.paramss.head

      val subSchemas = List.newBuilder[ValDef]
      val reprs = List.newBuilder[Tree]
      val fieldPairs = List.newBuilder[Tree]
      val gets = List.newBuilder[Tree]

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

      c.Expr[SchemaClass[TYPE]](q"""
        import com.sun.jna.Pointer
        import org.dianahep.scaroot.reader.Schema

        val allPossibleFields = $fields

        new SchemaClass[$dataClass] {
          private val walker: Pointer = $walker

          ..${subSchemas.result}

          def fields = Map(..${fieldPairs.result})

          def interpret(data: Pointer): $dataClass = {
            new $dataClass(..${gets.result})
          }
          override def toString() = "SchemaClass[" + classOf[$dataClass].getName + "](" + List(..${reprs.result}).mkString(", ") + ")"
        }
      """)
    }
  }

}
