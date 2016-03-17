package test.scala.scaroot

import scala.collection.mutable
import scala.language.postfixOps

import org.scalatest.FlatSpec
import org.scalatest.junit.JUnitRunner
import org.scalatest.Matchers




import scala.language.existentials

import com.sun.jna.Pointer

import org.dianahep.scaroot.reader._

case class Test(x: Int, y: Double, z: String)

class DefaultSuite extends FlatSpec with Matchers {
  def schema[TYPE](treeWalker: Pointer)(implicit customizations: Seq[Custom] = Nil): Schema[TYPE] = {
    import SchemaInstruction._
    sealed trait StackElement
    case class S(schemaInstruction: Int, fieldWalker: Pointer, dim: Pointer, word: Pointer) extends StackElement
    case class F(name: String, schema: Schema[_]) extends StackElement

    var stack = List[StackElement](S(SchemaClassFieldName.index, Pointer.NULL, Pointer.NULL, Pointer.NULL))

    object schemaBuilder extends RootReaderCPPLibrary.SchemaBuilder {
      def apply(schemaInstruction: Int, fieldWalker: Pointer, dim: Pointer, word: Pointer) {
        stack = S(schemaInstruction, fieldWalker, dim, word) :: stack

        stack match {
          case S(SchemaBool(), walker, _, _) :: S(SchemaClassFieldName(), _, _, fieldName) :: rest =>
            stack = F(fieldName.getString(0), new SchemaBool(walker, interpreter = Default.bool)) :: rest

          case S(SchemaChar(), walker, _, _) :: S(SchemaClassFieldName(), _, _, fieldName) :: rest =>
            stack = F(fieldName.getString(0), new SchemaChar(walker, interpreter = Default.char)) :: rest

          case S(SchemaUChar(), walker, _, _) :: S(SchemaClassFieldName(), _, _, fieldName) :: rest =>
            stack = F(fieldName.getString(0), new SchemaUChar(walker, interpreter = Default.uchar)) :: rest

          case S(SchemaShort(), walker, _, _) :: S(SchemaClassFieldName(), _, _, fieldName) :: rest =>
            stack = F(fieldName.getString(0), new SchemaShort(walker, interpreter = Default.short)) :: rest

          case S(SchemaUShort(), walker, _, _) :: S(SchemaClassFieldName(), _, _, fieldName) :: rest =>
            stack = F(fieldName.getString(0), new SchemaUShort(walker, interpreter = Default.ushort)) :: rest

          case S(SchemaInt(), walker, _, _) :: S(SchemaClassFieldName(), _, _, fieldName) :: rest =>
            stack = F(fieldName.getString(0), new SchemaInt(walker, interpreter = Default.int)) :: rest

          case S(SchemaUInt(), walker, _, _) :: S(SchemaClassFieldName(), _, _, fieldName) :: rest =>
            stack = F(fieldName.getString(0), new SchemaUInt(walker, interpreter = Default.uint)) :: rest

          case S(SchemaLong(), walker, _, _) :: S(SchemaClassFieldName(), _, _, fieldName) :: rest =>
            stack = F(fieldName.getString(0), new SchemaLong(walker, interpreter = Default.long)) :: rest

          case S(SchemaULong(), walker, _, _) :: S(SchemaClassFieldName(), _, _, fieldName) :: rest =>
            stack = F(fieldName.getString(0), new SchemaULong(walker, interpreter = Default.ulong)) :: rest

          case S(SchemaFloat(), walker, _, _) :: S(SchemaClassFieldName(), _, _, fieldName) :: rest =>
            stack = F(fieldName.getString(0), new SchemaFloat(walker, interpreter = Default.float)) :: rest

          case S(SchemaDouble(), walker, _, _) :: S(SchemaClassFieldName(), _, _, fieldName) :: rest =>
            stack = F(fieldName.getString(0), new SchemaDouble(walker, interpreter = Default.double)) :: rest

          case S(SchemaString(), walker, _, _) :: S(SchemaClassFieldName(), _, _, fieldName) :: rest =>
            stack = F(fieldName.getString(0), new SchemaString(walker, interpreter = Default.string)) :: rest

          case S(SchemaClassEnd(), _, _, _) :: rest1 =>
            stack = rest1

            def popFields(): List[(String, Schema[_])] = stack match {
              case F(fieldName, schema) :: rest2 =>
                stack = rest2
                (fieldName, schema) :: popFields()
              case _ =>
                Nil
            }
            val fields = popFields()

            val S(SchemaClassName(), walker, _, className) :: S(SchemaClassFieldName(), _, _, fieldName) :: rest3 = stack

            val fieldNameMayBeFake =
              if (fieldName == Pointer.NULL)
                ""
              else
                fieldName.getString(0)

            stack = F(fieldNameMayBeFake, Schema.schemaClassFrom[Test](walker, className.getString(0), fields)) :: rest3

          case _ =>
        }
      }
    }

    RootReaderCPPLibrary.buildSchema(treeWalker, schemaBuilder)

    val F(_, result: Schema[TYPE]) :: Nil = stack
    result
  }

  "stuff" must "work" in {
    // val libs = RootReaderCPPLibrary.addVectorString(Pointer.NULL, "../root2avro/test_Event/Event_cxx.so")
    // val treeWalker = RootReaderCPPLibrary.newTreeWalker("../root2avro/test_Event/Event.root", "T", "", libs);

    val treeWalker = RootReaderCPPLibrary.newTreeWalker("../root2avro/build/multipleLeaves.root", "t", "", Pointer.NULL);
    if (RootReaderCPPLibrary.valid(treeWalker) == 0)
      throw new Exception(RootReaderCPPLibrary.errorMessage(treeWalker))

    var done = (RootReaderCPPLibrary.next(treeWalker) == 0)

    while (!done  &&  RootReaderCPPLibrary.resolved(treeWalker) == 0) {
      RootReaderCPPLibrary.resolve(treeWalker)
      done = (RootReaderCPPLibrary.next(treeWalker) == 0)
    }

    RootReaderCPPLibrary.setEntryInCurrentTree(treeWalker, 0L)

    // println(RootReaderCPPLibrary.repr(treeWalker))

    // implicit val customizations = List('one :: 'two :: 'three :: CustomInt({x: Pointer => x.getLong(0)}))

    val s = schema(treeWalker)
    println(s)

    var i = 0
    while (!done) {
      // RootReaderCPPLibrary.printJSON(treeWalker)
      // RootReaderCPPLibrary.printAvro(treeWalker)

      println(s"entry $i")
      val result = s.interpret(Pointer.NULL)
      println(result)

      done = (RootReaderCPPLibrary.next(treeWalker) == 0)

      i += 1
      if (i > 10) done = true
    }
  }
}
