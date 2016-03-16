package test.scala.scaroot

import scala.collection.mutable
import scala.language.postfixOps

import org.scalatest.FlatSpec
import org.scalatest.junit.JUnitRunner
import org.scalatest.Matchers

import com.sun.jna.Pointer

import org.dianahep.scaroot.reader._

class DefaultSuite extends FlatSpec with Matchers {
  "stuff" must "work" in {
    val libs = RootReaderCPPLibrary.addVectorString(Pointer.NULL, "../root2avro/test_Event/Event_cxx.so")
    val treeWalker = RootReaderCPPLibrary.newTreeWalker("../root2avro/test_Event/Event.root", "T", "", libs);
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

    val schema = Schema(treeWalker)

    println(schema)

    // RootReaderCPPLibrary.printAvroHeaderOnce(treeWalker, "null", 64*1024)

    // def escape(raw: String): String = {
    //   import scala.reflect.runtime.universe._
    //   Literal(Constant(raw)).toString
    // }

    // object PrintOut extends RootReaderCPPLibrary.SchemaBuilder {
    //   def apply(schemaInstruction: Int, fieldWalker: Pointer, dim: Pointer, word: Pointer) {
    //     schemaInstruction match {
    //       case SchemaInstruction.SchemaBool() => println(s"""SchemaBool $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
    //       case SchemaInstruction.SchemaChar() => println(s"""SchemaChar $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
    //       case SchemaInstruction.SchemaUChar() => println(s"""SchemaUChar $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
    //       case SchemaInstruction.SchemaShort() => println(s"""SchemaShort $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
    //       case SchemaInstruction.SchemaUShort() => println(s"""SchemaUShort $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
    //       case SchemaInstruction.SchemaInt() => println(s"""SchemaInt $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
    //       case SchemaInstruction.SchemaUInt() => println(s"""SchemaUInt $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
    //       case SchemaInstruction.SchemaLong() => println(s"""SchemaLong $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
    //       case SchemaInstruction.SchemaULong() => println(s"""SchemaULong $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
    //       case SchemaInstruction.SchemaFloat() => println(s"""SchemaFloat $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
    //       case SchemaInstruction.SchemaDouble() => println(s"""SchemaDouble $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
    //       case SchemaInstruction.SchemaString() => println(s"""SchemaString $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")

    //       case SchemaInstruction.SchemaClassName() => println(s"""SchemaClassName $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
    //       case SchemaInstruction.SchemaClassDoc() => println(s"""SchemaClassDoc $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
    //       case SchemaInstruction.SchemaClassFieldName() => println(s"""SchemaClassFieldName $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
    //       case SchemaInstruction.SchemaClassFieldDoc() => println(s"""SchemaClassFieldDoc $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
    //       case SchemaInstruction.SchemaClassEnd() => println(s"""SchemaClassEnd $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
    //       case SchemaInstruction.SchemaClassReference() => println(s"""SchemaClassReference $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")

    //       case SchemaInstruction.SchemaPointer() => println(s"""SchemaPointer $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")

    //       case SchemaInstruction.SchemaSequence() => println(s"""SchemaSequence $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
    //     }
    //   }
    // }

    // RootReaderCPPLibrary.buildSchema(treeWalker, PrintOut)

    // object PrintOut extends RootReaderCPPLibrary.DataBuilder {
    //   def apply(data: Pointer) {
    //     println("here " + data.getInt(0).toString)
    //   }
    // }

    // var i = 0
    // while (!done) {
    //   // RootReaderCPPLibrary.printJSON(treeWalker)
    //   // RootReaderCPPLibrary.printAvro(treeWalker)

    //   RootReaderCPPLibrary.buildData(treeWalker, PrintOut)

    //   done = (RootReaderCPPLibrary.next(treeWalker) == 0)

    //   i += 1
    //   if (i > 10) done = true
    // }
  }
}
