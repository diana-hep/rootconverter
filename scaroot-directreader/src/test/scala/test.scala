package test.scala.scaroot

import scala.collection.mutable
import scala.language.postfixOps

import org.scalatest.FlatSpec
import org.scalatest.junit.JUnitRunner
import org.scalatest.Matchers






import com.sun.jna.Pointer

import org.dianahep.scaroot.directreader._

// case class Test(x: Int, y: Double, z: String)
// case class Test(x: Seq[Boolean])

class DefaultSuite extends FlatSpec with Matchers {
  "stuff" must "work" in {
    val beforeTime = System.currentTimeMillis

    val libs = RootReaderCPPLibrary.addVectorString(Pointer.NULL, "../root2avro/test_Event/Event_cxx.so")
    val treeWalker = RootReaderCPPLibrary.newTreeWalker("../root2avro/test_Event/Event.root", "T", "", libs);

    // val treeWalker = RootReaderCPPLibrary.newTreeWalker("../root2avro/build/multipleLeaves.root", "t", "", Pointer.NULL);
    // val treeWalker = RootReaderCPPLibrary.newTreeWalker("../root2avro/build/vectorBool.root", "t", "", Pointer.NULL);
    if (RootReaderCPPLibrary.valid(treeWalker) == 0)
      throw new Exception(RootReaderCPPLibrary.errorMessage(treeWalker))

    var done = (RootReaderCPPLibrary.next(treeWalker) == 0)

    while (!done  &&  RootReaderCPPLibrary.resolved(treeWalker) == 0) {
      RootReaderCPPLibrary.resolve(treeWalker)
      done = (RootReaderCPPLibrary.next(treeWalker) == 0)
    }

    // println(RootReaderCPPLibrary.repr(treeWalker))

    val customizations = List[Customization]()

    val schema = Schema[GenericClass](treeWalker, customizations)

    // println(schema)
    
    var midTime = System.currentTimeMillis
    println(s"init ${midTime - beforeTime}")

    RootReaderCPPLibrary.setEntryInCurrentTree(treeWalker, 0L)
    midTime = System.currentTimeMillis
    done = false

    var i = 0
    while (!done) {
      if (i % 100 == 0)
        println(s"entry $i")

      // RootReaderCPPLibrary.printJSON(treeWalker)
      // RootReaderCPPLibrary.printAvro(treeWalker)

      val result = schema.interpret(Pointer.NULL)
      // println(result)

      done = (RootReaderCPPLibrary.next(treeWalker) == 0)

      i += 1
      // if (i > 10) done = true
    }

    val endTime = System.currentTimeMillis

    println(s"loop ${endTime - midTime} items $i")
  }
}
