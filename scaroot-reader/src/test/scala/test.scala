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

    object PrintOut extends RootReaderCPPLibrary.SchemaBuilder {
      def apply(schemaElement: Int, word: Pointer) {
        val printableSchemaElement = schemaElement match {
          case SchemaElement.SchemaNull(x) => x.toString
          case SchemaElement.SchemaBoolean(x) => x.toString
          case SchemaElement.SchemaInt(x) => x.toString
          case SchemaElement.SchemaLong(x) => x.toString
          case SchemaElement.SchemaFloat(x) => x.toString
          case SchemaElement.SchemaDouble(x) => x.toString
          case SchemaElement.SchemaString(x) => x.toString
          case SchemaElement.SchemaBytes(x) => x.toString
          case SchemaElement.SchemaArray(x) => x.toString
          case SchemaElement.SchemaMap(x) => x.toString
          case SchemaElement.SchemaRecordName(x) => x.toString
          case SchemaElement.SchemaRecordNamespace(x) => x.toString
          case SchemaElement.SchemaRecordDoc(x) => x.toString
          case SchemaElement.SchemaRecordFieldName(x) => x.toString
          case SchemaElement.SchemaRecordFieldDoc(x) => x.toString
          case SchemaElement.SchemaRecordEnd(x) => x.toString
          case SchemaElement.SchemaEnumName(x) => x.toString
          case SchemaElement.SchemaEnumNamespace(x) => x.toString
          case SchemaElement.SchemaEnumDoc(x) => x.toString
          case SchemaElement.SchemaEnumSymbol(x) => x.toString
          case SchemaElement.SchemaEnumEnd(x) => x.toString
          case SchemaElement.SchemaFixedName(x) => x.toString
          case SchemaElement.SchemaFixedNamespace(x) => x.toString
          case SchemaElement.SchemaFixedDoc(x) => x.toString
          case SchemaElement.SchemaFixedSize(x) => x.toString
          case SchemaElement.SchemaUnionStart(x) => x.toString
          case SchemaElement.SchemaUnionEnd(x) => x.toString
          case SchemaElement.SchemaReference(x) => x.toString
        }

        val printableWord =
          if (word == Pointer.NULL)
            "null"
          else
            "\"" + word.getString(0) + "\""

        println("PrintOut(" + printableSchemaElement + ", " + printableWord + ")")
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
