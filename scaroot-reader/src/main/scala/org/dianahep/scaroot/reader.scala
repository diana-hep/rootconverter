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

  sealed trait SchemaElement

  object SchemaElement {
    object SchemaNull extends SchemaElement {
      def unapply(x: Int) = if (x == 0) Some(this) else None
      override def toString() = "SchemaNull"
    }

    object SchemaBoolean extends SchemaElement {
      def unapply(x: Int) = if (x == 1) Some(this) else None
      override def toString() = "SchemaBoolean"
    }

    object SchemaInt extends SchemaElement {
      def unapply(x: Int) = if (x == 2) Some(this) else None
      override def toString() = "SchemaInt"
    }

    object SchemaLong extends SchemaElement {
      def unapply(x: Int) = if (x == 3) Some(this) else None
      override def toString() = "SchemaLong"
    }

    object SchemaFloat extends SchemaElement {
      def unapply(x: Int) = if (x == 4) Some(this) else None
      override def toString() = "SchemaFloat"
    }

    object SchemaDouble extends SchemaElement {
      def unapply(x: Int) = if (x == 5) Some(this) else None
      override def toString() = "SchemaDouble"
    }

    object SchemaString extends SchemaElement {
      def unapply(x: Int) = if (x == 6) Some(this) else None
      override def toString() = "SchemaString"
    }

    object SchemaBytes extends SchemaElement {
      def unapply(x: Int) = if (x == 7) Some(this) else None
      override def toString() = "SchemaBytes"
    }

    object SchemaArray extends SchemaElement {
      def unapply(x: Int) = if (x == 8) Some(this) else None
      override def toString() = "SchemaArray"
    }

    object SchemaMap extends SchemaElement {
      def unapply(x: Int) = if (x == 9) Some(this) else None
      override def toString() = "SchemaMap"
    }

    object SchemaRecordName extends SchemaElement {
      def unapply(x: Int) = if (x == 10) Some(this) else None
      override def toString() = "SchemaRecordName"
    }

    object SchemaRecordNamespace extends SchemaElement {
      def unapply(x: Int) = if (x == 11) Some(this) else None
      override def toString() = "SchemaRecordNamespace"
    }

    object SchemaRecordDoc extends SchemaElement {
      def unapply(x: Int) = if (x == 12) Some(this) else None
      override def toString() = "SchemaRecordDoc"
    }

    object SchemaRecordFieldName extends SchemaElement {
      def unapply(x: Int) = if (x == 13) Some(this) else None
      override def toString() = "SchemaRecordFieldName"
    }

    object SchemaRecordFieldDoc extends SchemaElement {
      def unapply(x: Int) = if (x == 14) Some(this) else None
      override def toString() = "SchemaRecordFieldDoc"
    }

    object SchemaRecordEnd extends SchemaElement {
      def unapply(x: Int) = if (x == 15) Some(this) else None
      override def toString() = "SchemaRecordEnd"
    }

    object SchemaEnumName extends SchemaElement {
      def unapply(x: Int) = if (x == 16) Some(this) else None
      override def toString() = "SchemaEnumName"
    }

    object SchemaEnumNamespace extends SchemaElement {
      def unapply(x: Int) = if (x == 17) Some(this) else None
      override def toString() = "SchemaEnumNamespace"
    }

    object SchemaEnumDoc extends SchemaElement {
      def unapply(x: Int) = if (x == 18) Some(this) else None
      override def toString() = "SchemaEnumDoc"
    }

    object SchemaEnumSymbol extends SchemaElement {
      def unapply(x: Int) = if (x == 19) Some(this) else None
      override def toString() = "SchemaEnumSymbol"
    }

    object SchemaEnumEnd extends SchemaElement {
      def unapply(x: Int) = if (x == 20) Some(this) else None
      override def toString() = "SchemaEnumEnd"
    }

    object SchemaFixedName extends SchemaElement {
      def unapply(x: Int) = if (x == 21) Some(this) else None
      override def toString() = "SchemaFixedName"
    }

    object SchemaFixedNamespace extends SchemaElement {
      def unapply(x: Int) = if (x == 22) Some(this) else None
      override def toString() = "SchemaFixedNamespace"
    }

    object SchemaFixedDoc extends SchemaElement {
      def unapply(x: Int) = if (x == 23) Some(this) else None
      override def toString() = "SchemaFixedDoc"
    }

    object SchemaFixedSize extends SchemaElement {
      def unapply(x: Int) = if (x == 24) Some(this) else None
      override def toString() = "SchemaFixedSize"
    }

    object SchemaUnionStart extends SchemaElement {
      def unapply(x: Int) = if (x == 25) Some(this) else None
      override def toString() = "SchemaUnionStart"
    }

    object SchemaUnionEnd extends SchemaElement {
      def unapply(x: Int) = if (x == 26) Some(this) else None
      override def toString() = "SchemaUnionEnd"
    }

    object SchemaReference extends SchemaElement {
      def unapply(x: Int) = if (x == 27) Some(this) else None
      override def toString() = "SchemaReference"
    }
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

  case class CustomBoolean[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}CustomBoolean[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomInt[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}CustomInt[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomLong[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}CustomLong[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomFloat[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}CustomFloat[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomDouble[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}CustomDouble[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomString[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}CustomString[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomBytes[TYPE : ClassTag](f: (Pointer, Int) => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}CustomBytes[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomArray[BUILDER <: Builder[_, _] : ClassTag](f: Int => BUILDER, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}CustomArray[${classTag[BUILDER].runtimeClass.getName}]"""
  }

  case class CustomFixed[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}CustomFixed[${classTag[TYPE].runtimeClass.getName}]"""
  }

  case class CustomEnum[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Custom {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}CustomEnum[${classTag[TYPE].runtimeClass.getName}]"""
  }

  class CustomRecord[TYPE : ClassTag](val in: List[String] = Nil, val is: Option[String] = None) extends Custom {
    def ::(x: String) = CustomRecord(x :: in, is)
    def ::(x: Symbol) = CustomRecord(x.name :: in, is)
    def named(x: String) = CustomRecord(in, Some(x))
    def named(x: Symbol) = CustomRecord(in, Some(x.name))
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}CustomRecord[${classTag[TYPE].runtimeClass.getName}]${is.map(" named " + escape(_)).mkString}"""
  }
  object CustomRecord {
    def apply[TYPE : ClassTag] = new CustomRecord[TYPE](Nil, None)
    def apply[TYPE : ClassTag](in: List[String] = Nil, is: Option[String] = None) = new CustomRecord[TYPE](in, is)
    def unapply[TYPE](x: CustomRecord[TYPE]) = Some((x.in, x.is))
  }

  // Immutable schemas that can extract and convert data using interpreters.

  sealed trait Schema[TYPE] {
    def interpret(data: Pointer): TYPE
  }

  class SchemaBoolean[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0))
    override def toString() = s"SchemaBoolean[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaInt[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0))
    override def toString() = s"SchemaInt[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaLong[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0))
    override def toString() = s"SchemaLong[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaFloat[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0))
    override def toString() = s"SchemaFloat[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaDouble[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0))
    override def toString() = s"SchemaDouble[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaString[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0))
    override def toString() = s"SchemaString[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaBytes[TYPE : ClassTag](walker: Pointer, interpreter: (Pointer, Int) => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = {
      val size = RootReaderCPPLibrary.getDataSize(walker, data)
      interpreter(RootReaderCPPLibrary.getData(walker, data, 0), size)
    }
    override def toString() = s"SchemaBytes[${classTag[TYPE].runtimeClass.getName}]"
  }

  class SchemaArray[ITEMS, TYPE : ClassTag](walker: Pointer, builder: Int => Builder[ITEMS, TYPE], items: Schema[ITEMS]) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = {
      val size = RootReaderCPPLibrary.getDataSize(walker, data)
      val listBuilder = builder(size)
      var index = 0
      while (index < size) {
        val subdata = RootReaderCPPLibrary.getData(walker, data, index)
        listBuilder += items.interpret(subdata)
        index += 1
      }
      listBuilder.result
    }
    override def toString() = s"SchemaArray[${classTag[TYPE].runtimeClass.getName}]($items)"
  }
  
  trait SchemaRecord[TYPE] extends Schema[TYPE] {
    def fields: Map[String, Schema[_]]
  }

  class SchemaOption[TYPE](walker: Pointer, nullable: Schema[TYPE]) extends Schema[Option[TYPE]] {
    def interpret(data: Pointer): Option[TYPE] = {
      val discriminant = RootReaderCPPLibrary.getDataSize(walker, data)
      if (discriminant == 0)
        None.asInstanceOf[Option[TYPE]]
      else
        Some(nullable.interpret(RootReaderCPPLibrary.getData(walker, data, discriminant))).asInstanceOf[Option[TYPE]]
    }
  }

  object Schema {
    def record[TYPE](walker: Pointer, fields: List[(String, Schema[_])]): SchemaRecord[TYPE] = macro specializedRecord[TYPE]

    def specializedRecord[TYPE](c: Context)(walker: c.Expr[Pointer], fields: c.Expr[List[(String, Schema[_])]])(implicit t: c.WeakTypeTag[TYPE]): c.Expr[SchemaRecord[TYPE]] = {
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
        gets += q"""$name._1.interpret(RootReaderCPPLibrary.getData(walker, data, $name._2))"""
      }

      c.Expr[SchemaRecord[TYPE]](q"""
        import com.sun.jna.Pointer
        import org.dianahep.scaroot.reader.Schema

        val allPossibleFields = $fields

        new SchemaRecord[$dataClass] {
          private val walker: Pointer = $walker

          ..${subSchemas.result}

          def fields = Map(..${fieldPairs.result})

          def interpret(data: Pointer): $dataClass = {
            new $dataClass(..${gets.result})
          }
          override def toString() = "SchemaRecord[" + classOf[$dataClass].getName + "](" + List(..${reprs.result}).mkString(", ") + ")"
        }
      """)
    }
  }

}
