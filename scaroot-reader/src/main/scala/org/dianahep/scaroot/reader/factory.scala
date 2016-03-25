package org.dianahep.scaroot.reader

import java.nio.ByteBuffer
import java.nio.charset.Charset

import scala.collection.immutable.SortedSet
import scala.collection.mutable
import scala.collection.mutable.Builder
import scala.reflect.classTag
import scala.reflect.ClassTag
import scala.reflect.runtime.universe.Constant
import scala.reflect.runtime.universe.Literal
import scala.reflect.runtime.universe.Type
import scala.reflect.runtime.universe.typeOf
import scala.reflect.runtime.universe.TypeRefApi

import com.sun.jna.Pointer

import org.dianahep.scaroot.reader._
import org.dianahep.scaroot.reader.schema._

package factory {
  /////////////////////////////////////////////////// factories for building data at runtime
  // Note that type information is compiled in!

  trait Factory[+TYPE] {
    def apply(byteBuffer: ByteBuffer): TYPE
    def toString(indent: Int, memo: mutable.Set[String]): String = toString()
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

  case class FactoryString(charsetName: String) extends Factory[String] {
    val charset = Charset.forName(charsetName)
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
    override def toString(indent: Int, memo: mutable.Set[String]): String = s"""FactoryOption(${factory.toString(indent, memo)})"""
  }

  case class FactoryWrappedArray[TYPE](factory: Factory[TYPE], builder: Int => Array[TYPE]) extends Factory[Seq[TYPE]] {
    def apply(byteBuffer: ByteBuffer) = {
      val size = byteBuffer.getInt
      val b = builder(size)
      var i = 0
      while (i < size) {
        b(i) = factory(byteBuffer)
        i += 1
      }
      b.toSeq
    }
    override def toString(indent: Int, memo: mutable.Set[String]): String = s"""FactoryWrappedArray(${factory.toString(indent, memo)})"""
  }

  case class FactoryArray[TYPE](factory: Factory[TYPE], builder: Int => Array[TYPE]) extends Factory[Array[TYPE]] {
    def apply(byteBuffer: ByteBuffer) = {
      val size = byteBuffer.getInt
      val b = builder(size)
      var i = 0
      while (i < size) {
        b(i) = factory(byteBuffer)
        i += 1
      }
      b
    }
    override def toString(indent: Int, memo: mutable.Set[String]): String = s"""FactoryArray(${factory.toString(indent, memo)})"""
  }

  case class FactoryIterable[TYPE](factory: Factory[TYPE], builder: Int => Builder[TYPE, Iterable[TYPE]]) extends Factory[Iterable[TYPE]] {
    def apply(byteBuffer: ByteBuffer) = {
      val size = byteBuffer.getInt
      val b = builder(size)
      var i = 0
      while (i < size) {
        b += factory(byteBuffer)
        i += 1
      }
      b.result
    }
    override def toString(indent: Int, memo: mutable.Set[String]): String = s"""FactoryIterable(${factory.toString(indent, memo)})"""
  }

  abstract class FactoryClass[TYPE](val name: String, val factories: List[(String, Factory[_])]) extends Factory[TYPE] {
    override def toString() = toString(0, mutable.Set[String]())
    override def toString(indent: Int, memo: mutable.Set[String]): String =
      if (memo contains name)
        s"""FactoryClass(name = ${Literal(Constant(name)).toString}, factories = <see above>)"""
      else {
        memo += name
        s"""FactoryClass(name = ${Literal(Constant(name)).toString}, factories = List(${factories map {case (n, f) => "\n" + " " * indent + "  \"" + n + "\" -> " + f.toString(indent + 2, memo)} mkString(",")}${"\n" + " " * indent}))"""
      }
  }
  object FactoryClass {
    // Scala reflect missing function (it's tpe.typeArgs in Scala 2.11)

    private def typeArgs(tpe: Type) = tpe.asInstanceOf[TypeRefApi].args

    // Build specialized versions of each collection builder to avoid boxing penalty for primitives at runtime.

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

    private def mutableListBuilder(tpe: Type) =
      if      (tpe =:= typeOf[Boolean]) {size: Int => val out = mutable.ListBuffer.newBuilder[Boolean]; out.sizeHint(size); out}
      else if (tpe =:= typeOf[Char])    {size: Int => val out = mutable.ListBuffer.newBuilder[Char];    out.sizeHint(size); out}
      else if (tpe =:= typeOf[Byte])    {size: Int => val out = mutable.ListBuffer.newBuilder[Byte];    out.sizeHint(size); out}
      else if (tpe =:= typeOf[Short])   {size: Int => val out = mutable.ListBuffer.newBuilder[Short];   out.sizeHint(size); out}
      else if (tpe =:= typeOf[Int])     {size: Int => val out = mutable.ListBuffer.newBuilder[Int];     out.sizeHint(size); out}
      else if (tpe =:= typeOf[Long])    {size: Int => val out = mutable.ListBuffer.newBuilder[Long];    out.sizeHint(size); out}
      else if (tpe =:= typeOf[Float])   {size: Int => val out = mutable.ListBuffer.newBuilder[Float];   out.sizeHint(size); out}
      else if (tpe =:= typeOf[Double])  {size: Int => val out = mutable.ListBuffer.newBuilder[Double];  out.sizeHint(size); out}
      else                              {size: Int => val out = mutable.ListBuffer.newBuilder[AnyRef];  out.sizeHint(size); out}

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

    private def mutableSetBuilder(tpe: Type) =
      if      (tpe =:= typeOf[Boolean]) {size: Int => val out = mutable.Set.newBuilder[Boolean]; out.sizeHint(size); out}
      else if (tpe =:= typeOf[Char])    {size: Int => val out = mutable.Set.newBuilder[Char];    out.sizeHint(size); out}
      else if (tpe =:= typeOf[Byte])    {size: Int => val out = mutable.Set.newBuilder[Byte];    out.sizeHint(size); out}
      else if (tpe =:= typeOf[Short])   {size: Int => val out = mutable.Set.newBuilder[Short];   out.sizeHint(size); out}
      else if (tpe =:= typeOf[Int])     {size: Int => val out = mutable.Set.newBuilder[Int];     out.sizeHint(size); out}
      else if (tpe =:= typeOf[Long])    {size: Int => val out = mutable.Set.newBuilder[Long];    out.sizeHint(size); out}
      else if (tpe =:= typeOf[Float])   {size: Int => val out = mutable.Set.newBuilder[Float];   out.sizeHint(size); out}
      else if (tpe =:= typeOf[Double])  {size: Int => val out = mutable.Set.newBuilder[Double];  out.sizeHint(size); out}
      else                              {size: Int => val out = mutable.Set.newBuilder[AnyRef];  out.sizeHint(size); out}

    private def sortedSetBuilder(tpe: Type) =
      if      (tpe =:= typeOf[Boolean])    {size: Int => val out = SortedSet.newBuilder[Boolean]; out.sizeHint(size); out}
      else if (tpe =:= typeOf[Char])       {size: Int => val out = SortedSet.newBuilder[Char];    out.sizeHint(size); out}
      else if (tpe =:= typeOf[Byte])       {size: Int => val out = SortedSet.newBuilder[Byte];    out.sizeHint(size); out}
      else if (tpe =:= typeOf[Short])      {size: Int => val out = SortedSet.newBuilder[Short];   out.sizeHint(size); out}
      else if (tpe =:= typeOf[Int])        {size: Int => val out = SortedSet.newBuilder[Int];     out.sizeHint(size); out}
      else if (tpe =:= typeOf[Long])       {size: Int => val out = SortedSet.newBuilder[Long];    out.sizeHint(size); out}
      else if (tpe =:= typeOf[Float])      {size: Int => val out = SortedSet.newBuilder[Float];   out.sizeHint(size); out}
      else if (tpe =:= typeOf[Double])     {size: Int => val out = SortedSet.newBuilder[Double];  out.sizeHint(size); out}
      else
        throw new IllegalArgumentException(s"SortedSet is currently only supported for primitive types, not $tpe.")

    // Called for each field of a My[class]; choice of factory depends on fields of the class.
    // All of the ".asInstanceOf[Factory[TYPE]]" are to assure the compiler of the type-checks
    // that we do at runtime (because we don't see the ROOT schema until then).

    private def applyField[TYPE](schema: Schema, tpe: Type, myclasses: Map[String, My[_]]): Factory[TYPE] = schema match {
      case SchemaBool   if (typeOf[Boolean]     =:= tpe)  =>  FactoryBool.asInstanceOf[Factory[TYPE]]
      case SchemaChar   if (typeOf[Char]        =:= tpe)  =>  FactoryChar.asInstanceOf[Factory[TYPE]]
      case SchemaUChar  if (typeOf[Char]        =:= tpe)  =>  FactoryChar.asInstanceOf[Factory[TYPE]]
      case SchemaChar   if (typeOf[Byte]   weak_<:< tpe)  =>  FactoryByte.asInstanceOf[Factory[TYPE]]
      case SchemaUChar  if (typeOf[Short]  weak_<:< tpe)  =>  FactoryUByte.asInstanceOf[Factory[TYPE]]
      case SchemaShort  if (typeOf[Short]  weak_<:< tpe)  =>  FactoryShort.asInstanceOf[Factory[TYPE]]
      case SchemaUShort if (typeOf[Int]    weak_<:< tpe)  =>  FactoryUShort.asInstanceOf[Factory[TYPE]]
      case SchemaInt    if (typeOf[Int]    weak_<:< tpe)  =>  FactoryInt.asInstanceOf[Factory[TYPE]]
      case SchemaUInt   if (typeOf[Long]   weak_<:< tpe)  =>  FactoryUInt.asInstanceOf[Factory[TYPE]]
      case SchemaLong   if (typeOf[Long]   weak_<:< tpe)  =>  FactoryLong.asInstanceOf[Factory[TYPE]]
      case SchemaULong  if (typeOf[Double] weak_<:< tpe)  =>  FactoryULong.asInstanceOf[Factory[TYPE]]
      case SchemaFloat  if (typeOf[Float]  weak_<:< tpe)  =>  FactoryFloat.asInstanceOf[Factory[TYPE]]
      case SchemaDouble if (typeOf[Double] weak_<:< tpe)  =>  FactoryDouble.asInstanceOf[Factory[TYPE]]
      case SchemaString if (typeOf[Bytes]       =:= tpe)  =>  FactoryBytes.asInstanceOf[Factory[TYPE]]
      case SchemaString if (typeOf[String]      =:= tpe)  =>  FactoryString("US-ASCII").asInstanceOf[Factory[TYPE]]

      case x: SchemaClass => apply(x, myclasses).asInstanceOf[Factory[TYPE]]

      case SchemaPointer(referent) if (tpe <:< typeOf[Option[_]]) =>
        FactoryOption(applyField(referent, typeArgs(tpe).head, myclasses)).asInstanceOf[Factory[TYPE]]

      case SchemaSequence(content) =>
        if (tpe <:< typeOf[Seq[_]])
          FactoryWrappedArray(applyField(content, typeArgs(tpe).head, myclasses), arrayBuilder(typeArgs(tpe).head).asInstanceOf[Int => Array[TYPE]]).asInstanceOf[Factory[TYPE]]
        else if (tpe <:< typeOf[Array[_]])
          FactoryArray(applyField(content, typeArgs(tpe).head, myclasses), arrayBuilder(typeArgs(tpe).head).asInstanceOf[Int => Array[TYPE]]).asInstanceOf[Factory[TYPE]]
        else if (tpe <:< typeOf[List[_]])
          FactoryIterable(applyField(content, typeArgs(tpe).head, myclasses), listBuilder(typeArgs(tpe).head).asInstanceOf[Int => Builder[TYPE, Iterable[TYPE]]]).asInstanceOf[Factory[TYPE]]
        else if (tpe <:< typeOf[mutable.ListBuffer[_]])
          FactoryIterable(applyField(content, typeArgs(tpe).head, myclasses), mutableListBuilder(typeArgs(tpe).head).asInstanceOf[Int => Builder[TYPE, Iterable[TYPE]]]).asInstanceOf[Factory[TYPE]]
        else if (tpe <:< typeOf[Vector[_]])
          FactoryIterable(applyField(content, typeArgs(tpe).head, myclasses), vectorBuilder(typeArgs(tpe).head).asInstanceOf[Int => Builder[TYPE, Iterable[TYPE]]]).asInstanceOf[Factory[TYPE]]
        else if (tpe <:< typeOf[Set[_]])
          FactoryIterable(applyField(content, typeArgs(tpe).head, myclasses), setBuilder(typeArgs(tpe).head).asInstanceOf[Int => Builder[TYPE, Iterable[TYPE]]]).asInstanceOf[Factory[TYPE]]
        else if (tpe <:< typeOf[mutable.Set[_]])
          FactoryIterable(applyField(content, typeArgs(tpe).head, myclasses), mutableSetBuilder(typeArgs(tpe).head).asInstanceOf[Int => Builder[TYPE, Iterable[TYPE]]]).asInstanceOf[Factory[TYPE]]
        else if (tpe <:< typeOf[SortedSet[_]])
          FactoryIterable(applyField(content, typeArgs(tpe).head, myclasses), sortedSetBuilder(typeArgs(tpe).head).asInstanceOf[Int => Builder[TYPE, Iterable[TYPE]]]).asInstanceOf[Factory[TYPE]]
        else
          throw new IllegalArgumentException(s"Currently, only Array[T], List[T], Vector[T], Set[T], and SortedSet[T] can be used as containers, not $tpe.")
    }

    // Called for each field of a generic class; factory depends only on Schema.
    // All of the ".asInstanceOf[Factory[TYPE]]" are to assure the compiler of the type-checks
    // that we do at runtime (because we don't see the ROOT schema until then).

    private def applyGenericField[TYPE](schema: Schema, myclasses: Map[String, My[_]]): Factory[TYPE] = schema match {
      case SchemaBool   => FactoryBool.asInstanceOf[Factory[TYPE]]
      case SchemaChar   => FactoryByte.asInstanceOf[Factory[TYPE]]
      case SchemaUChar  => FactoryUByte.asInstanceOf[Factory[TYPE]]
      case SchemaShort  => FactoryShort.asInstanceOf[Factory[TYPE]]
      case SchemaUShort => FactoryUShort.asInstanceOf[Factory[TYPE]]
      case SchemaInt    => FactoryInt.asInstanceOf[Factory[TYPE]]
      case SchemaUInt   => FactoryUInt.asInstanceOf[Factory[TYPE]]
      case SchemaLong   => FactoryLong.asInstanceOf[Factory[TYPE]]
      case SchemaULong  => FactoryULong.asInstanceOf[Factory[TYPE]]
      case SchemaFloat  => FactoryFloat.asInstanceOf[Factory[TYPE]]
      case SchemaDouble => FactoryDouble.asInstanceOf[Factory[TYPE]]
      case SchemaString => FactoryString("US-ASCII").asInstanceOf[Factory[TYPE]]

      case x: SchemaClass => apply(x, myclasses).asInstanceOf[Factory[TYPE]]

      case SchemaPointer(referent) => FactoryOption(applyGenericField(referent, myclasses)).asInstanceOf[Factory[TYPE]]

      case SchemaSequence(content) => content match {
        case SchemaBool   => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Boolean]).asInstanceOf[Int => Array[TYPE]]).asInstanceOf[Factory[TYPE]]
        case SchemaChar   => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Byte]).asInstanceOf[Int => Array[TYPE]]).asInstanceOf[Factory[TYPE]]
        case SchemaUChar  => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Short]).asInstanceOf[Int => Array[TYPE]]).asInstanceOf[Factory[TYPE]]
        case SchemaShort  => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Short]).asInstanceOf[Int => Array[TYPE]]).asInstanceOf[Factory[TYPE]]
        case SchemaUShort => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Int]).asInstanceOf[Int => Array[TYPE]]).asInstanceOf[Factory[TYPE]]
        case SchemaInt    => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Int]).asInstanceOf[Int => Array[TYPE]]).asInstanceOf[Factory[TYPE]]
        case SchemaUInt   => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Long]).asInstanceOf[Int => Array[TYPE]]).asInstanceOf[Factory[TYPE]]
        case SchemaLong   => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Long]).asInstanceOf[Int => Array[TYPE]]).asInstanceOf[Factory[TYPE]]
        case SchemaULong  => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Double]).asInstanceOf[Int => Array[TYPE]]).asInstanceOf[Factory[TYPE]]
        case SchemaFloat  => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Float]).asInstanceOf[Int => Array[TYPE]]).asInstanceOf[Factory[TYPE]]
        case SchemaDouble => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[Double]).asInstanceOf[Int => Array[TYPE]]).asInstanceOf[Factory[TYPE]]
        case _            => FactoryWrappedArray(applyGenericField(content, myclasses), arrayBuilder(typeOf[AnyRef]).asInstanceOf[Int => Array[TYPE]]).asInstanceOf[Factory[TYPE]]
      }
    }

    // Entry point for making a new FactoryClass from a SchemaClass.

    def apply[TYPE](schema: SchemaClass, myclasses: Map[String, My[_]] = Map[String, My[_]]()): FactoryClass[TYPE] =
      // Specialized My[class] because the name is in the myclasses lookup table.
      if (myclasses.keySet contains schema.name) {
        val my = myclasses(schema.name)
        if (schema.fields.size != my.fieldTypes.size)
          throw new IllegalArgumentException(s"ROOT class or TTree ${schema.name} has ${schema.fields.size} fields but My[${my.name}] has ${my.fieldTypes.size} fields.")

        val factories = schema.fields zip my.fieldTypes map {case ((n1, s), (n2, tpe)) =>
          if (n1 != n2)
            throw new IllegalArgumentException(s"ROOT class or TTree ${schema.name} has a field named $n1 but the corresponding field in My[${my.name}] is named $n2.")

          val fac =
            try {
              applyField(s, tpe, myclasses)
            }
            catch {
              case _: MatchError => throw new IllegalArgumentException(s"ROOT class or TTree ${schema.name} has field $n1 with type ${s.cpp} but My[${my.name}] has field $n1 with incompatible type $tpe.")
            }
          (n1, fac)
        }

        my(factories).asInstanceOf[FactoryClass[TYPE]]
      }
      // Generic class because the name is not in the myclasses lookup table.
      else {
        val factories = schema.fields map {case (n, s) => (n, applyGenericField(s, myclasses))}
        FactoryGeneric(schema.name, factories).asInstanceOf[FactoryClass[TYPE]]
      }
  }

  case class FactoryGeneric(override val name: String, override val factories: List[(String, Factory[_])]) extends FactoryClass[Generic](name, factories) {
    def apply(byteBuffer: ByteBuffer) =
      new Generic(factories.map({case (n, f) => (n, f(byteBuffer))}).toMap)
    override def toString(indent: Int, memo: mutable.Set[String]): String =
      if (memo contains name)
        s"""FactoryGeneric(name = ${Literal(Constant(name)).toString}, factories = <see above>)"""
      else {
        memo += name
        s"""FactoryGeneric(name = ${Literal(Constant(name)).toString}, factories = List(${factories map {case (n, f) => "\n" + " " * indent + "  \"" + n + "\" -> " + f.toString(indent + 2, memo)} mkString(",")}${"\n" + " " * indent}))"""
      }
  }
}
