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
          case SchemaElement.SchemaBool(x) => println(s"""$x $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaChar(x) => println(s"""$x $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaUChar(x) => println(s"""$x $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaShort(x) => println(s"""$x $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaUShort(x) => println(s"""$x $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaInt(x) => println(s"""$x $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaUInt(x) => println(s"""$x $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaLong(x) => println(s"""$x $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaULong(x) => println(s"""$x $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaFloat(x) => println(s"""$x $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaDouble(x) => println(s"""$x $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaString(x) => println(s"""$x $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")

          case SchemaElement.SchemaClassName(x) => println(s"""$x $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaClassDoc(x) => println(s"""$x $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaClassFieldName(x) => println(s"""$x $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaClassFieldDoc(x) => println(s"""$x $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaClassEnd(x) => println(s"""$x $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
          case SchemaElement.SchemaClassReference(x) => println(s"""$x $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")

          case SchemaElement.SchemaPointer(x) => println(s"""$x $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")

          case SchemaElement.SchemaSequence(x) => println(s"""$x $fieldWalker $dim ${if (word == Pointer.NULL) "null" else escape(word.getString(0))}""")
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
