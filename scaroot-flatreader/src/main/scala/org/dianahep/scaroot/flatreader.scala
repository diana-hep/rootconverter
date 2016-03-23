package org.dianahep.scaroot

import java.nio.ByteBuffer

import scala.collection.immutable.SortedSet
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

  sealed trait Schema {
    def cpp: String  // just for error messages
  }
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

  case object SchemaBool extends Schema { val cpp = "bool" }
  case object SchemaChar extends Schema { val cpp = "char" }
  case object SchemaUChar extends Schema { val cpp = "unsigned char" }
  case object SchemaShort extends Schema { val cpp = "short" }
  case object SchemaUShort extends Schema { val cpp = "unsigned short" }
  case object SchemaInt extends Schema { val cpp = "int" }
  case object SchemaUInt extends Schema { val cpp = "unsigned int" }
  case object SchemaLong extends Schema { val cpp = "long" }
  case object SchemaULong extends Schema { val cpp = "unsigned long" }
  case object SchemaFloat extends Schema { val cpp = "float" }
  case object SchemaDouble extends Schema { val cpp = "double" }
  case object SchemaString extends Schema { val cpp = "STRING" }

  case class SchemaClass(name: String, fields: List[(String, Schema)]) extends Schema { def cpp = name }

  case class SchemaPointer(referent: Schema) extends Schema { def cpp = s"${referent}*" }

  case class SchemaSequence(content: Schema) extends Schema { def cpp = s"SEQUENCE<$content>" }

  /////////////////////////////////////////////////// class to use when no My[TYPE] is supplied

  class Generic(val fields: Map[String, Any]) {
    def apply(field: String): Any = fields(field)
    override def toString() = s"""Generic(Map(${fields.map({case (k, v) => "\"" + k + "\"" + " -> " + v.toString}).mkString(", ")}))"""
  }
  object Generic {
    def apply(fields: Map[String, Any]) = new Generic(fields)
    def unapply(x: Generic) = Some(x.fields)
  }

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

  /////////////////////////////////////////////////// factories for building data at runtime
  // Note that type information is compiled in!

  trait Factory[+TYPE] {
    def apply(byteBuffer: ByteBuffer): TYPE
  }
  object Factory {
    private def typeArgs(tpe: Type) = tpe match {case TypeRef(_, _, args) => args}

    private def arrayBuilder(tpe: Type) =
      if      (tpe =:= typeOf[Boolean]) {size: Int => Array.fill[Boolean](size)(false)}
      else if (tpe =:= typeOf[Char])    {size: Int => Array.fill[Char](size)(' ')}
      else if (tpe =:= typeOf[Byte])    {size: Int => Array.fill[Byte](size)(0)}
      else if (tpe =:= typeOf[Short])   {size: Int => Array.fill[Short](size)(0)}
      else if (tpe =:= typeOf[Int])     {size: Int => Array.fill[Int](size)(0)}
      else if (tpe =:= typeOf[Long])    {size: Int => Array.fill[Long](size)(0)}
      else if (tpe =:= typeOf[Float])   {size: Int => Array.fill[Float](size)(0)}
      else if (tpe =:= typeOf[Double])  {size: Int => Array.fill[Double](size)(0)}
      else                              {size: Int => Array.fill[AnyRef](size)(null)}

    private def listBuilder(tpe: Type) =
      if      (tpe =:= typeOf[Boolean]) {size: Int => val out = List.newBuilder[Boolean]; out.sizeHint(size); out}
      else if (tpe =:= typeOf[Char])    {size: Int => val out = List.newBuilder[Char];    out.sizeHint(size); out}
      else if (tpe =:= typeOf[Byte])    {size: Int => val out = List.newBuilder[Byte];    out.sizeHint(size); out}
      else if (tpe =:= typeOf[Short])   {size: Int => val out = List.newBuilder[Short];   out.sizeHint(size); out}
      else if (tpe =:= typeOf[Int])     {size: Int => val out = List.newBuilder[Int];     out.sizeHint(size); out}
      else if (tpe =:= typeOf[Long])    {size: Int => val out = List.newBuilder[Long];    out.sizeHint(size); out}
      else if (tpe =:= typeOf[Float])   {size: Int => val out = List.newBuilder[Float];   out.sizeHint(size); out}
      else if (tpe =:= typeOf[Double])  {size: Int => val out = List.newBuilder[Double];  out.sizeHint(size); out}
      else                              {size: Int => val out = List.newBuilder[AnyRef];  out.sizeHint(size); out}

    private def vectorBuilder(tpe: Type) =
      if      (tpe =:= typeOf[Boolean]) {size: Int => val out = Vector.newBuilder[Boolean]; out.sizeHint(size); out}
      else if (tpe =:= typeOf[Char])    {size: Int => val out = Vector.newBuilder[Char];    out.sizeHint(size); out}
      else if (tpe =:= typeOf[Byte])    {size: Int => val out = Vector.newBuilder[Byte];    out.sizeHint(size); out}
      else if (tpe =:= typeOf[Short])   {size: Int => val out = Vector.newBuilder[Short];   out.sizeHint(size); out}
      else if (tpe =:= typeOf[Int])     {size: Int => val out = Vector.newBuilder[Int];     out.sizeHint(size); out}
      else if (tpe =:= typeOf[Long])    {size: Int => val out = Vector.newBuilder[Long];    out.sizeHint(size); out}
      else if (tpe =:= typeOf[Float])   {size: Int => val out = Vector.newBuilder[Float];   out.sizeHint(size); out}
      else if (tpe =:= typeOf[Double])  {size: Int => val out = Vector.newBuilder[Double];  out.sizeHint(size); out}
      else                              {size: Int => val out = Vector.newBuilder[AnyRef];  out.sizeHint(size); out}

    private def setBuilder(tpe: Type) =
      if      (tpe =:= typeOf[Boolean]) {size: Int => val out = Set.newBuilder[Boolean]; out.sizeHint(size); out}
      else if (tpe =:= typeOf[Char])    {size: Int => val out = Set.newBuilder[Char];    out.sizeHint(size); out}
      else if (tpe =:= typeOf[Byte])    {size: Int => val out = Set.newBuilder[Byte];    out.sizeHint(size); out}
      else if (tpe =:= typeOf[Short])   {size: Int => val out = Set.newBuilder[Short];   out.sizeHint(size); out}
      else if (tpe =:= typeOf[Int])     {size: Int => val out = Set.newBuilder[Int];     out.sizeHint(size); out}
      else if (tpe =:= typeOf[Long])    {size: Int => val out = Set.newBuilder[Long];    out.sizeHint(size); out}
      else if (tpe =:= typeOf[Float])   {size: Int => val out = Set.newBuilder[Float];   out.sizeHint(size); out}
      else if (tpe =:= typeOf[Double])  {size: Int => val out = Set.newBuilder[Double];  out.sizeHint(size); out}
      else                              {size: Int => val out = Set.newBuilder[AnyRef];  out.sizeHint(size); out}

    private def sortedSetBuilder(tpe: Type) =
      if      (tpe =:= typeOf[Boolean]) {size: Int => val out = SortedSet.newBuilder[Boolean]; out.sizeHint(size); out}
      else if (tpe =:= typeOf[Char])    {size: Int => val out = SortedSet.newBuilder[Char];    out.sizeHint(size); out}
      else if (tpe =:= typeOf[Byte])    {size: Int => val out = SortedSet.newBuilder[Byte];    out.sizeHint(size); out}
      else if (tpe =:= typeOf[Short])   {size: Int => val out = SortedSet.newBuilder[Short];   out.sizeHint(size); out}
      else if (tpe =:= typeOf[Int])     {size: Int => val out = SortedSet.newBuilder[Int];     out.sizeHint(size); out}
      else if (tpe =:= typeOf[Long])    {size: Int => val out = SortedSet.newBuilder[Long];    out.sizeHint(size); out}
      else if (tpe =:= typeOf[Float])   {size: Int => val out = SortedSet.newBuilder[Float];   out.sizeHint(size); out}
      else if (tpe =:= typeOf[Double])  {size: Int => val out = SortedSet.newBuilder[Double];  out.sizeHint(size); out}
      else                              {size: Int => val out = SortedSet.newBuilder[AnyRef];  out.sizeHint(size); out}

    private def applyField(schema: Schema, tpe: Type, myclasses: Map[String, My[_]]) = schema match {
      case SchemaBool   if (typeOf[Boolean]     =:= tpe) => FactoryBool
      case SchemaChar   if (typeOf[Char]        =:= tpe) => FactoryChar
      case SchemaUChar  if (typeOf[Char]        =:= tpe) => FactoryChar
      case SchemaChar   if (typeOf[Byte]   weak_<:< tpe) => FactoryByte
      case SchemaUChar  if (typeOf[Short]  weak_<:< tpe) => FactoryUByte
      case SchemaShort  if (typeOf[Short]  weak_<:< tpe) => FactoryShort
      case SchemaUShort if (typeOf[Int]    weak_<:< tpe) => FactoryUShort
      case SchemaInt    if (typeOf[Int]    weak_<:< tpe) => FactoryInt
      case SchemaUInt   if (typeOf[Long]   weak_<:< tpe) => FactoryUInt
      case SchemaLong   if (typeOf[Long]   weak_<:< tpe) => FactoryLong
      case SchemaULong  if (typeOf[Double] weak_<:< tpe) => FactoryULong
      case SchemaFloat  if (typeOf[Float]  weak_<:< tpe) => FactoryFloat
      case SchemaDouble if (typeOf[Double] weak_<:< tpe) => FactoryDouble
      case SchemaString if (typeOf[Bytes]       =:= tpe) => FactoryBytes
      case SchemaString if (typeOf[String]      =:= tpe) => FactoryString("US-ASCII")

      case SchemaClass => apply(schema, myclasses)

      case SchemaPointer(referent) if (tpe <:< typeOf[Option[_]]) =>
        FactoryOption(applyField(referent, typeArgs(tpe).head, myclasses))

      case SchemaSequence(content) =>
        if (tpe <:< typeOf[Seq[_]])
          FactoryWrappedArray(applyField(content, typeArgs(tpe).head, myclasses), arrayBuilder(typeArgs(tpe).head))
        else if (tpe <:< typeOf[Array[_]])
          FactoryArray(applyField(content, typeArgs(tpe).head, myclasses), arrayBuilder(typeArgs(tpe).head))
        else if (tpe <:< typeOf[List[_]])
          FactoryIterable(applyField(content, typeArgs(tpe).head, myclasses), listBuilder(typeArgs(tpe).head))
        else if (tpe <:< typeOf[Vector[_]])
          FactoryIterable(applyField(content, typeArgs(tpe).head, myclasses), vectorBuilder(typeArgs(tpe).head))
        else if (tpe <:< typeOf[Set[_]])
          FactoryIterable(applyField(content, typeArgs(tpe).head, myclasses), setBuilder(typeArgs(tpe).head))
        else if (tpe <:< typeOf[SortedSet[_]])
          FactoryIterable(applyField(content, typeArgs(tpe).head, myclasses), sortedSetBuilder(typeArgs(tpe).head))
        else
          throw new IllegalArgumentException(s"Currently, only Array[T], List[T], Vector[T], Set[T], and SortedSet[T] can be used as containers, not $tpe.")
    }

    private def applyGenericField(schema: Schema, myclasses: Map[String, My[_]]) = schema match {
      case SchemaBool   => FactoryBool
      case SchemaChar   => FactoryByte
      case SchemaUChar  => FactoryUByte
      case SchemaShort  => FactoryShort
      case SchemaUShort => FactoryUShort
      case SchemaInt    => FactoryInt
      case SchemaUInt   => FactoryUInt
      case SchemaLong   => FactoryLong
      case SchemaULong  => FactoryULong
      case SchemaFloat  => FactoryFloat
      case SchemaDouble => FactoryDouble
      case SchemaString => FactoryString("US-ASCII")
      case SchemaClass  => apply(schema, myclasses)
      case SchemaPointer(referent) => FactoryOption(applyGenericField(referent, myclasses))
      case SchemaSequence(content) => content match {
        case SchemaBool   => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Boolean]))
        case SchemaChar   => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Byte]))
        case SchemaUChar  => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Short]))
        case SchemaShort  => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Short]))
        case SchemaUShort => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Int]))
        case SchemaInt    => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Int]))
        case SchemaUInt   => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Long]))
        case SchemaLong   => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Long]))
        case SchemaULong  => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Double]))
        case SchemaFloat  => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Float]))
        case SchemaDouble => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Double]))
        case _            => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[AnyRef]))
      }
    }

    def apply[TYPE](schema: Schema, myclasses: Map[String, My[_]] = Map[String, My[_]]()): Factory[TYPE] = schema match {
      case SchemaClass(name, fields) if (myclasses.keys contains name) =>
        val my = myclasses(name)
        if (fields.size != my.fieldTypes.size)
          throw new IllegalArgumentException(s"ROOT class or TTree $name has ${fields.size} fields but My[${my.name}] has ${my.fieldTypes.size} fields.")

        val factories = fields zip my.fieldTypes map {case ((n1, s), (n2, tpe)) =>
          if (n1 != n2)
            throw new IllegalArgumentException(s"ROOT class or TTree $name has a field named $n1 but the corresponding field in My[${my.name}] is named $n2.")

          val fac =
            try {
              applyField(s, tpe, myclasses)
            }
            catch {
              case _: MatchError => throw new IllegalArgumentException(s"ROOT class or TTree $name has field $n1 with type ${s.cpp} but My[${my.name}] has field $n2 with incompatible type $tpe.")
            }
          (n, fac)
        }

        my(factories).asInstanceOf[Factory[TYPE]]

      case SchemaClass(name, fields) =>
        val factories = fields map {case (n, s) => (n, applyGenericField(s, myclasses))}

        FactoryGeneric(name, factories).asInstanceOf[Factory[TYPE]]

      case _ =>
        throw new IllegalArgumentException("The Factory constructor can only be used on SchemaClass (e.g. the root of a TTree).")
    }
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

  case object FactoryBytes extends Factory[Bytes] {
    def apply(byteBuffer: ByteBuffer) = {
      val size = byteBuffer.getInt
      val out = Array.fill[Byte](size)(0)
      byteBuffer.get(out)
      new Bytes(out)
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

  case class FactoryOption[TYPE](factory: Factory[TYPE]) extends Factory[Option[TYPE]] {
    def apply(byteBuffer: ByteBuffer) = {
      val discriminant = byteBuffer.get
      if (discriminant == 0)
        None
      else
        Some(factory(byteBuffer))
    }
  }

  case class FactoryIterable[TYPE](factory: Factory[TYPE], builder: Int => Builder[TYPE, Iterable[TYPE]]) extends Factory[Iterable[TYPE]] {
    def apply(byteBuffer: ByteBuffer) = {
      val size = byteBuffer.getInt
      val b = builder(size)
      0 until size foreach {i => b += factory(byteBuffer)}
      b.result
    }
  }

  case class FactoryArray[TYPE](factory: Factory[TYPE], builder: Int => Array[TYPE]) extends Factory[Array[TYPE]] {
    def apply(byteBuffer: ByteBuffer) = {
      val size = byteBuffer.getInt
      val b = builder(size)
      0 until size foreach {i => b(i) = factory(byteBuffer)}
      b
    }
  }

  case class FactoryWrappedArray[TYPE](factory: Factory[TYPE], builder: Int => Array[TYPE]) extends Factory[Seq[TYPE]] {
    def apply(byteBuffer: ByteBuffer) = {
      val size = byteBuffer.getInt
      val b = builder(size)
      0 until size foreach {i => b(i) = factory(byteBuffer)}
      b.toSeq
    }
  }

  abstract class FactoryClass[TYPE](val name: String, val factories: List[(String, Factory[_])]) extends Factory[TYPE]

  case class FactoryGeneric(name: String, factories: List[(String, Factory[_])]) extends FactoryClass[Generic](name, factories) {
    def apply(byteBuffer: ByteBuffer) =
      new Generic(factories.map({case (n, f) => (n, f(byteBuffer))}).toMap)
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
        import org.dianahep.scaroot.flatreader._

        new My[$dataClass] {
          // What you know at compile-time...
          val name = $dataClassName
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
