package test.scala.scaroot

import scala.collection.mutable

import org.scalatest.FlatSpec
import org.scalatest.junit.JUnitRunner
import org.scalatest.Matchers

import org.dianahep.scaroot.reader._

class DefaultSuite extends FlatSpec with Matchers {
  "stuff" must "work" in {
    // case class TBits(fNbits: Long, fNbytes: Long, fAllBits: Option[Short])
    // val myclasses = Map("TBits" -> My[TBits])
    
    // val iterator = RootTreeIterator[Generic](List("../root2avro/build/multipleLeaves.root"), "t")
    val iterator = RootTreeIterator[Generic](List("../root2avro/test_Event/Event.root"), "T", List("../root2avro/test_Event/Event_cxx.so"))

    println(iterator.schema)

    println(iterator.factory)

    while (true) {
      val beforeTime = System.nanoTime
      var tmp: Generic = null

      var i = 0
      while (iterator.hasNext) {
        tmp = iterator.next()
        i += 1
      }

      println(s"seconds: ${(System.nanoTime - beforeTime) / 1e9} i: $i")
      iterator.reset()
    }

  }
}
