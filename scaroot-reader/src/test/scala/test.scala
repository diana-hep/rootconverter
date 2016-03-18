package test.scala.scaroot

import scala.collection.mutable
import scala.language.postfixOps

import org.scalatest.FlatSpec
import org.scalatest.junit.JUnitRunner
import org.scalatest.Matchers






import com.sun.jna.Pointer

import org.dianahep.scaroot.reader._

// case class Test(x: Int, y: Double, z: String)
case class Test(x: Seq[Boolean])

class DefaultSuite extends FlatSpec with Matchers {
  "stuff" must "work" in {
    // val libs = RootReaderCPPLibrary.addVectorString(Pointer.NULL, "../root2avro/test_Event/Event_cxx.so")
    // val treeWalker = RootReaderCPPLibrary.newTreeWalker("../root2avro/test_Event/Event.root", "T", "", libs);

    // val treeWalker = RootReaderCPPLibrary.newTreeWalker("../root2avro/build/multipleLeaves.root", "t", "", Pointer.NULL);
    val treeWalker = RootReaderCPPLibrary.newTreeWalker("../root2avro/build/vectorBool.root", "t", "", Pointer.NULL);
    if (RootReaderCPPLibrary.valid(treeWalker) == 0)
      throw new Exception(RootReaderCPPLibrary.errorMessage(treeWalker))

    var done = (RootReaderCPPLibrary.next(treeWalker) == 0)

    while (!done  &&  RootReaderCPPLibrary.resolved(treeWalker) == 0) {
      RootReaderCPPLibrary.resolve(treeWalker)
      done = (RootReaderCPPLibrary.next(treeWalker) == 0)
    }

    RootReaderCPPLibrary.setEntryInCurrentTree(treeWalker, 0L)

    // println(RootReaderCPPLibrary.repr(treeWalker))

    val customizations = List(CustomClass[Test])

    val schema = Schema(treeWalker, customizations, customizations.head)

    println(schema)
    
    var i = 0
    while (!done) {
      println(s"entry $i")

      RootReaderCPPLibrary.printJSON(treeWalker)
      // RootReaderCPPLibrary.printAvro(treeWalker)

      val result = schema.interpret(Pointer.NULL)
      println(result)

      done = (RootReaderCPPLibrary.next(treeWalker) == 0)

      i += 1
      if (i > 10) done = true
    }
  }
}
