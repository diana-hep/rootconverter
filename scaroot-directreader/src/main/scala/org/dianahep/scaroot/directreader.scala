package org.dianahep.scaroot

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

import com.sun.jna.Pointer

package directreader {
  // Numerical enums from C++ that can be matched in case statements.
  // Also used in a stack to build the schema used at runtime.
  // Must be kept in-sync with scaroot-directreader/src/main/cpp/datawalker.h!

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

  class GenericClass(val fields: Map[String, Any]) {
    def apply(field: String): Any = fields(field)
    override def toString() = s"""GenericClass(Map(${fields.map({case (k, v) => "\"" + k + "\"" + " -> " + v.toString}).mkString(", ")}))"""
  }
  object GenericClass {
    def apply(fields: Map[String, Any]) = new GenericClass(fields)
    def unapply(x: GenericClass) = Some(x.fields)
  }

  // Custom interpreters for data. (Use '# for arrays.)
  
  sealed trait Customization {
    private[directreader] def escape(raw: String): String = {
      import scala.reflect.runtime.universe._
      Literal(Constant(raw)).toString
    }
    private[directreader] def customClassName = getClass.getName.split('.').last

    def in: List[String]
    def is: Option[String] = None
    def matches(in: List[String], is: Option[String]) =
      (this.in.isEmpty  ||  this.in == in)  &&  (this.is.isEmpty  ||  this.is == is)
  }

  case class Custom[TYPE : ClassTag](f: Pointer => TYPE, in: List[String] = Nil) extends Customization {
    def ::(x: String) = this.copy(in = x :: in)
    def ::(x: Symbol) = this.copy(in = x.name :: in)
    override def toString() = s"""${if (in.isEmpty) "" else in.map(escape).mkString(" :: ") + " :: "}$customClassName[${classTag[TYPE].runtimeClass.getName}]"""
  }

  class CustomClass[TYPE : ClassTag : SchemaClassMaker](val in: List[String] = Nil, override val is: Option[String] = None) extends Customization {
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

  case class CustomPointer[REFERENT : ClassTag, TYPE : ClassTag](f: Option[REFERENT] => TYPE, in: List[String] = Nil) extends Customization {
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

  sealed trait Schema[+TYPE] {
    def interpret(data: Pointer): TYPE
  }

  object Schema {
    def apply[TYPE](treeWalker: Pointer, customizations: Seq[Customization] = Nil): Schema[TYPE] = {
      sealed trait StackElement
      case class I(schemaInstruction: Int, data: Pointer, namePath: List[String]) extends StackElement
      case class S(schema: Schema[_], namePath: List[String]) extends StackElement

      var stack: List[StackElement] = Nil
      val schemaClasses = mutable.Map[String, Schema[_]]()

      object schemaBuilder extends RootReaderCPPLibrary.SchemaBuilder {
        def apply(schemaInstruction: Int, data: Pointer) {
          val oldNamePath: List[String] = (schemaInstruction match {
            case SchemaInstruction.SchemaClassFieldName() =>
              stack.collectFirst {case I(SchemaInstruction.SchemaClassName(), _, np) => np}
            case _ =>
              stack.headOption.collect {case I(_, _, np) => np; case S(_, np) => np }
          }).getOrElse(Nil)

          val namePath: List[String] =
            schemaInstruction match {
              case SchemaInstruction.SchemaClassFieldName() =>
                data.getString(0) :: oldNamePath
              case SchemaInstruction.SchemaSequence() =>
                "#" :: oldNamePath
              case _ =>
                oldNamePath
            }

          stack = I(schemaInstruction, data, namePath) :: stack

          def findCustom(np: List[String]) = customizations collectFirst {case c @ Custom(f, in) if (c.matches(np.reverse, None)) => SchemaCustom[Any](f)}

          var done1 = false
          while (!done1) stack match {
            case I(SchemaInstruction.SchemaBool(), _, np)   :: rest  =>  stack = S(findCustom(np).getOrElse(SchemaBool()), np)   :: rest
            case I(SchemaInstruction.SchemaChar(), _, np)   :: rest  =>  stack = S(findCustom(np).getOrElse(SchemaChar()), np)   :: rest
            case I(SchemaInstruction.SchemaUChar(), _, np)  :: rest  =>  stack = S(findCustom(np).getOrElse(SchemaUChar()), np)  :: rest
            case I(SchemaInstruction.SchemaShort(), _, np)  :: rest  =>  stack = S(findCustom(np).getOrElse(SchemaShort()), np)  :: rest
            case I(SchemaInstruction.SchemaUShort(), _, np) :: rest  =>  stack = S(findCustom(np).getOrElse(SchemaUShort()), np) :: rest
            case I(SchemaInstruction.SchemaInt(), _, np)    :: rest  =>  stack = S(findCustom(np).getOrElse(SchemaInt()), np)    :: rest
            case I(SchemaInstruction.SchemaUInt(), _, np)   :: rest  =>  stack = S(findCustom(np).getOrElse(SchemaUInt()), np)   :: rest
            case I(SchemaInstruction.SchemaLong(), _, np)   :: rest  =>  stack = S(findCustom(np).getOrElse(SchemaLong()), np)   :: rest
            case I(SchemaInstruction.SchemaULong(), _, np)  :: rest  =>  stack = S(findCustom(np).getOrElse(SchemaULong()), np)  :: rest
            case I(SchemaInstruction.SchemaFloat(), _, np)  :: rest  =>  stack = S(findCustom(np).getOrElse(SchemaFloat()), np)  :: rest
            case I(SchemaInstruction.SchemaDouble(), _, np) :: rest  =>  stack = S(findCustom(np).getOrElse(SchemaDouble()), np) :: rest
            case I(SchemaInstruction.SchemaString(), _, np) :: rest  =>  stack = S(findCustom(np).getOrElse(SchemaString()), np) :: rest

            case I(SchemaInstruction.SchemaClassEnd(), _, np) :: rest1 =>
              stack = rest1

              var fields: List[(String, Schema[_])] = Nil
              var done2 = false
              while (!done2) stack match {
                case S(schema, _) :: I(SchemaInstruction.SchemaClassFieldName(), fieldNamePtr, _) :: rest2 =>
                  stack = rest2
                  fields = (fieldNamePtr.getString(0), schema) :: fields
                case _ =>
                  done2 = true
              }

              stack match {
                case I(SchemaInstruction.SchemaClassPointer(), dataProvider, _) :: I(SchemaInstruction.SchemaClassName(), classNamePtr, np2) :: rest3 =>
                  val className = classNamePtr.getString(0)

                  val customClass = customizations collectFirst {case x: CustomClass[_] if (x.matches(np2.reverse, Some(className))) => x}
                  val schemaClass = customClass match {
                    case None => SchemaClassMakerGeneric(dataProvider, className, fields)
                    case Some(custom) => custom.schemaClassMaker(dataProvider, className, fields)
                  }
                  schemaClasses(className) = schemaClass

                  stack = S(schemaClass, np2) :: rest3
              }

            case I(SchemaInstruction.SchemaClassReference(), classNamePtr, np) :: rest =>
              val schemaClass = schemaClasses(classNamePtr.getString(0))
              stack = S(schemaClass, np) :: rest

            case S(referent, _) :: I(SchemaInstruction.SchemaPointer(), dataProvider, np) :: rest =>
              val policy =
                customizations collectFirst {case c @ CustomPointer(f, in) if (c.matches(np.reverse, None)) => f} getOrElse({x: Option[Any] => x})
              stack = S(SchemaPointer(referent, dataProvider, policy.asInstanceOf[Option[Any] => Any]), np) :: rest

            case S(items, "#" :: np) :: I(SchemaInstruction.SchemaSequence(), dataProvider, _) :: rest =>
              val schemaSequence =
                customizations collectFirst {
                  case c @ CustomSequence(f, in) if (c.matches(np.reverse, None)) =>
                    SchemaSequence(items, dataProvider, f.asInstanceOf[Int => Builder[Any, Iterable[Any]]])
                } getOrElse SchemaSequence(items, dataProvider)

              val np2 = rest collectFirst {case I(_, _, x) => x} getOrElse Nil
              stack = S(schemaSequence, np2) :: rest

            case _ =>
              done1 = true
          }
        }
      }

      RootReaderCPPLibrary.buildSchema(treeWalker, schemaBuilder)

      val S(result: Schema[TYPE], _) :: Nil = stack
      result
    }
  }

  case class SchemaCustom[TYPE](interpreter: Pointer => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer): TYPE = interpreter(data)
    override def toString() = s"${getClass.getName.split('.').last}"
  }

  case class SchemaBool() extends Schema[Boolean] {
    def interpret(data: Pointer) = data.getByte(0) != 0
    override def toString() = s"${getClass.getName.split('.').last}"
  }

  case class SchemaChar() extends Schema[Byte] {
    def interpret(data: Pointer) = data.getByte(0)
    override def toString() = s"${getClass.getName.split('.').last}"
  }

  case class SchemaUChar() extends Schema[Short] {
    def interpret(data: Pointer) = { val out = data.getByte(0); (if (out < 0) 2 * (java.lang.Byte.MAX_VALUE.toShort + 1) + out else out).toShort }
    override def toString() = s"${getClass.getName.split('.').last}"
  }

  case class SchemaShort() extends Schema[Short] {
    def interpret(data: Pointer) = data.getShort(0)
    override def toString() = s"${getClass.getName.split('.').last}"
  }

  case class SchemaUShort() extends Schema[Int] {
    def interpret(data: Pointer) = { val out = data.getShort(0); if (out < 0) 2 * (java.lang.Short.MAX_VALUE.toInt + 1) + out.toInt else out.toInt }
    override def toString() = s"${getClass.getName.split('.').last}"
  }

  case class SchemaInt() extends Schema[Int] {
    def interpret(data: Pointer) = data.getInt(0)
    override def toString() = s"${getClass.getName.split('.').last}"
  }

  case class SchemaUInt() extends Schema[Long] {
    def interpret(data: Pointer) = { val out = data.getInt(0); if (out < 0) 2L * (java.lang.Integer.MAX_VALUE.toLong + 1L) + out.toLong else out.toLong }
    override def toString() = s"${getClass.getName.split('.').last}"
  }

  case class SchemaLong() extends Schema[Long] {
    def interpret(data: Pointer) = data.getLong(0)
    override def toString() = s"${getClass.getName.split('.').last}"
  }

  case class SchemaULong() extends Schema[Double] {
    def interpret(data: Pointer) = { val out = data.getLong(0); if (out < 0) 2.0 * (java.lang.Long.MAX_VALUE.toDouble + 1.0) + out.toDouble else out.toDouble }
    override def toString() = s"${getClass.getName.split('.').last}"
  }

  case class SchemaFloat() extends Schema[Float] {
    def interpret(data: Pointer) = data.getFloat(0)
    override def toString() = s"${getClass.getName.split('.').last}"
  }

  case class SchemaDouble() extends Schema[Double] {
    def interpret(data: Pointer) = data.getDouble(0)
    override def toString() = s"${getClass.getName.split('.').last}"
  }

  case class SchemaString() extends Schema[String] {
    def interpret(data: Pointer) = data.getString(0)
    override def toString() = s"${getClass.getName.split('.').last}"
  }

  abstract class SchemaClass[TYPE] extends Schema[TYPE] {
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
        gets += q"""$name._1.interpret(RootReaderCPPLibrary.getData(dataProvider, data, $name._2))"""
      }

      c.Expr[SchemaClassMaker[TYPE]](q"""
        import com.sun.jna.Pointer
        import org.dianahep.scaroot.directreader._

        new SchemaClassMaker[$dataClass] {
          def apply(dataProvider: Pointer, className: String, allPossibleFields: List[(String, Schema[_])]): SchemaClass[$dataClass] =
            new SchemaClass[$dataClass] {
              ..${subSchemas.result}

              def name = className
              def fields = Map(..${fieldPairs.result})

              def interpret(data: Pointer): $dataClass = new $dataClass(..${gets.result})

              override def toString() = "SchemaClass[" + classOf[$dataClass].getName + "](" + List(..${reprs.result}).mkString(", ") + ")"
            }
          }
      """)
    }
  }

  object SchemaClassMakerGeneric extends SchemaClassMaker[GenericClass] {
    def apply(dataProvider: Pointer, className: String, allPossibleFields: List[(String, Schema[_])]): SchemaClass[GenericClass] =
      new SchemaClass[GenericClass] {
        def name = className
        def fields = allPossibleFields.toMap
        private def lookup = allPossibleFields.zipWithIndex.toMap

        def interpret(data: Pointer): GenericClass = new GenericClass(lookup.map({case ((n, s), i) => (n, s.interpret(RootReaderCPPLibrary.getData(dataProvider, data, i)))}))

        override def toString() = "SchemaClass[GenericClass](" + fields.map({case (n, s) => n + ": " + s.toString}).mkString(", ") + ")"
      }
  }

  case class SchemaPointer[REFERENT, TYPE](referent: Schema[REFERENT], dataProvider: Pointer, policy: Option[REFERENT] => TYPE) extends Schema[TYPE] {
    def interpret(data: Pointer) = {
      val result = RootReaderCPPLibrary.getData(dataProvider, data, 0)
      val maybe =
        if (result == Pointer.NULL)
          None.asInstanceOf[Option[REFERENT]]
        else
          Some(referent.interpret(result)).asInstanceOf[Option[REFERENT]]
      policy(maybe)
    }
    override def toString() = s"${getClass.getName.split('.').last}($referent)"
  }

  case class SchemaSequence[TYPE](items: Schema[TYPE], dataProvider: Pointer, builder: Int => Builder[TYPE, Iterable[TYPE]] = SchemaSequence.defaultBuilder[TYPE, Builder[TYPE, Iterable[TYPE]]]) extends Schema[Iterable[TYPE]] {
    def interpret(data: Pointer): Iterable[TYPE] = {
      val size = RootReaderCPPLibrary.getDataSize(dataProvider, data)
      val listBuilder = builder(size)

      var index = 0
      while (index < size) {
        val subdata = RootReaderCPPLibrary.getData(dataProvider, data, index)
        listBuilder += items.interpret(subdata).asInstanceOf[TYPE]
        index += 1
      }

      listBuilder.result.asInstanceOf[Iterable[TYPE]]
    }
    override def toString() = s"${getClass.getName.split('.').last}($items)"
  }
  object SchemaSequence {
    def defaultBuilder[TYPE, BUILDER <: Builder[TYPE, Iterable[TYPE]]] = {size: Int =>
      val builder =
        if (size < 10)    // FIXME: optimize this
          List.newBuilder[TYPE].asInstanceOf[Builder[TYPE, Iterable[TYPE]]]
        else
          Vector.newBuilder[TYPE].asInstanceOf[Builder[TYPE, Iterable[TYPE]]]
      builder.sizeHint(size)
      builder
    }
  }

  class RootTreeIterator[TYPE](fileLocations: Seq[String], treeLocation: String, libs: Seq[String] = Nil, customizations: Seq[Customization] = Nil) extends Iterator[TYPE] {
    private var libscpp = Pointer.NULL
    libs foreach {lib => libscpp = RootReaderCPPLibrary.addVectorString(libscpp, lib)}

    private var done = true
    private var treeWalker = Pointer.NULL
    private var remainingFiles = fileLocations.toList
    private var schema: Schema[TYPE] = null

    if (!fileLocations.isEmpty) {
      treeWalker = RootReaderCPPLibrary.newTreeWalker(remainingFiles.head, treeLocation, "", libscpp)
      remainingFiles = remainingFiles.tail

      if (RootReaderCPPLibrary.valid(treeWalker) == 0)
        throw new RuntimeException(RootReaderCPPLibrary.errorMessage(treeWalker))

      done = (RootReaderCPPLibrary.next(treeWalker) == 0)
      while (!done  &&  RootReaderCPPLibrary.resolved(treeWalker) == 0) {
        RootReaderCPPLibrary.resolve(treeWalker)
        done = (RootReaderCPPLibrary.next(treeWalker) == 0)
      }

      schema = Schema[TYPE](treeWalker, customizations)

      RootReaderCPPLibrary.setEntryInCurrentTree(treeWalker, 0L)
    }

    def hasNext = !done
    def next() = {
      if (!hasNext)
        throw new RuntimeException("next() called on empty RootTreeIterator (create a new one to run over the data again)")

      val out = schema.interpret(Pointer.NULL)
      done = (RootReaderCPPLibrary.next(treeWalker) == 0)
      if (done  &&  !remainingFiles.isEmpty) {
        RootReaderCPPLibrary.reset(treeWalker, remainingFiles.head)
        remainingFiles = remainingFiles.tail
        done = (RootReaderCPPLibrary.next(treeWalker) == 0)
      }
      out
    }
  }
  object RootTreeIterator {
    def apply[TYPE](fileLocations: Seq[String], treeLocation: String, libs: Seq[String] = Nil, customizations: Seq[Customization] = Nil) = new RootTreeIterator[TYPE](fileLocations, treeLocation, libs, customizations)
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

    def fixedByteBuffer(size: Int) = {data: Pointer => data.getByteBuffer(0, size)}

    def fixedByteArray(size: Int) = {data: Pointer => data.getByteArray(0, size)}

    def enumFromInt[ENUM <: Enumeration](enumeration: ENUM) = {data: Pointer => enumeration.apply(data.getInt(0))}
  }
}
