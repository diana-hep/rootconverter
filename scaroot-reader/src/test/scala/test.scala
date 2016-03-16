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

    println(RootReaderCPPLibrary.repr(treeWalker))

    // RootReaderCPPLibrary.printAvroHeaderOnce(treeWalker, "null", 64*1024)

    def escape(raw: String): String = {
      import scala.reflect.runtime.universe._
      Literal(Constant(raw)).toString
    }

    object PrintOut extends RootReaderCPPLibrary.SchemaBuilder {
      def apply(schemaElement: Int, fieldWalker: Pointer, dim: Pointer, word: Pointer) {
        schemaElement match {
          case SchemaElement.SchemaBool() => println(s"""SchemaBool $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaChar() => println(s"""SchemaChar $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaUChar() => println(s"""SchemaUChar $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaShort() => println(s"""SchemaShort $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaUShort() => println(s"""SchemaUShort $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaInt() => println(s"""SchemaInt $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaUInt() => println(s"""SchemaUInt $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaLong() => println(s"""SchemaLong $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaULong() => println(s"""SchemaULong $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaFloat() => println(s"""SchemaFloat $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaDouble() => println(s"""SchemaDouble $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaString() => println(s"""SchemaString $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")

          case SchemaElement.SchemaClassName() => println(s"""SchemaClassName $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaClassDoc() => println(s"""SchemaClassDoc $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaClassFieldName() => println(s"""SchemaClassFieldName $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaClassFieldDoc() => println(s"""SchemaClassFieldDoc $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaClassEnd() => println(s"""SchemaClassEnd $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaClassReference() => println(s"""SchemaClassReference $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")

          case SchemaElement.SchemaPointer() => println(s"""SchemaPointer $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")

          case SchemaElement.SchemaSequence() => println(s"""SchemaSequence $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
        }
      }
    }

    RootReaderCPPLibrary.buildSchema(treeWalker, PrintOut)

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
