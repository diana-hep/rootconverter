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

    println(RootReaderCPPLibrary.avroSchema(treeWalker))

    // RootReaderCPPLibrary.printAvroHeaderOnce(treeWalker, "null", 64*1024)

    object PrintOut extends RootReaderCPPLibrary.SchemaBuilder {
      def apply(schemaElement: Int, word: Pointer) {
        println("here " + schemaElement)
      }
    }

    // object PrintOut extends RootReaderCPPLibrary.DataBuilder {
    //   def apply(data: Pointer) {
    //     println("here " + data.getInt(0).toString)
    //   }
    // }

    var i = 0
    while (!done) {
      // RootReaderCPPLibrary.printJSON(treeWalker)
      // RootReaderCPPLibrary.printAvro(treeWalker)

      RootReaderCPPLibrary.buildSchema(treeWalker, PrintOut)
      // RootReaderCPPLibrary.buildData(treeWalker, PrintOut)

      done = (RootReaderCPPLibrary.next(treeWalker) == 0)

      i += 1
      if (i > 10) done = true
    }
  }
}
