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
    val nul =     {() => None}
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

  // Immutable schemas that can extract and convert data using interpreters.

  sealed trait Schema[TYPE] {
    def interpret(data: Pointer): TYPE
  }

  object Schema {
    case class nul[TYPE : ClassTag](walker: Pointer, interpreter: () => TYPE = Default.nul) extends Schema[TYPE] {
      def interpret(data: Pointer): TYPE = interpreter()
      override def toString() = s"Schema.nul[${classTag[TYPE].runtimeClass.getSimpleName}]"
    }

    case class boolean[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE = Default.boolean) extends Schema[TYPE] {
      def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0))
      override def toString() = s"Schema.boolean[${classTag[TYPE].runtimeClass.getSimpleName}]"
    }

    case class int[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE = Default.int) extends Schema[TYPE] {
      def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0))
      override def toString() = s"Schema.int[${classTag[TYPE].runtimeClass.getSimpleName}]"
    }

    case class long[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE = Default.long) extends Schema[TYPE] {
      def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0))
      override def toString() = s"Schema.long[${classTag[TYPE].runtimeClass.getSimpleName}]"
    }

    case class float[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE = Default.float) extends Schema[TYPE] {
      def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0))
      override def toString() = s"Schema.float[${classTag[TYPE].runtimeClass.getSimpleName}]"
    }

    case class double[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE = Default.double) extends Schema[TYPE] {
      def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0))
      override def toString() = s"Schema.double[${classTag[TYPE].runtimeClass.getSimpleName}]"
    }

    case class string[TYPE : ClassTag](walker: Pointer, interpreter: Pointer => TYPE = Default.string) extends Schema[TYPE] {
      def interpret(data: Pointer): TYPE = interpreter(RootReaderCPPLibrary.getData(walker, data, 0))
      override def toString() = s"Schema.string[${classTag[TYPE].runtimeClass.getSimpleName}]"
    }

    case class bytes[TYPE : ClassTag](walker: Pointer, interpreter: (Pointer, Int) => TYPE = Default.bytes) extends Schema[TYPE] {
      def interpret(data: Pointer): TYPE = {
        val size = RootReaderCPPLibrary.getDataSize(walker, data)
        interpreter(RootReaderCPPLibrary.getData(walker, data, 0), size)
      }
      override def toString() = s"Schema.bytes[${classTag[TYPE].runtimeClass.getSimpleName}]"
    }

    case class array[ITEMS, TYPE : ClassTag](walker: Pointer, builder: Int => Builder[ITEMS, TYPE], items: Schema[ITEMS]) extends Schema[TYPE] {
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
      override def toString() = s"Schema.array[${classTag[TYPE].runtimeClass.getSimpleName}]($items)"
    }
    
    trait Record[TYPE] extends Schema[TYPE]

    def Record[TYPE](walker: Pointer, fields: List[(String, Schema[_])]): Record[TYPE] = macro specializedRecord[TYPE]

    def specializedRecord[TYPE : c.WeakTypeTag](c: Context)(walker: c.Expr[Pointer], fields: c.Expr[List[(String, Schema[_])]]): c.Expr[Record[TYPE]] = {
      import c.universe._
      val dataClass = weakTypeOf[TYPE]




      c.Expr[Record[TYPE]](q"""
        import com.sun.jna.Pointer
        import org.dianahep.scaroot.reader.Schema

        new Schema.Record[$dataClass] {
          val fields = $fields.toArray
          def interpret(data: Pointer): $dataClass = {
            ..$valdefs
            new $dataClass(..$valrefs)
          }
          override def toString() = "Schema.record[" + classOf[$dataClass].getSimpleName + "](" + fields.map({case (n, f) => n + ": " + f.toString}).mkString(", ") + ")"
        }
      """)
    }
  }

}
