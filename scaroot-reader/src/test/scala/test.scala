package test.scala.scaroot

import scala.collection.mutable

import org.scalatest.FlatSpec
import org.scalatest.junit.JUnitRunner
import org.scalatest.Matchers

import com.sun.jna.Pointer

import org.dianahep.scaroot.reader._

class DefaultSuite extends FlatSpec with Matchers {
  "stuff" must "work" in {
    // case class TBits(fNbits: Long, fNbytes: Long, fAllBits: Option[Short])
    // val myclasses = Map("TBits" -> My[TBits])
    
    val iterator = RootTreeIterator[Generic](List("../root2avro/build/multipleLeaves.root"), "t")

    println(iterator.schema)

    println(iterator.factory)

    while (iterator.hasNext)
      println(iterator.next())


    // val size = 64*1024
    // val buffer = new Memory(size)

    // while (!done) {
    //   buffer.setByte(0, 1)
    //   RootReaderCPPLibrary.copyToBuffer(treeWalker, 1, buffer, new NativeLong(size))
    //   val stringSize = buffer.getInt(1 + 4 + 8)
    //   println(buffer.getByte(0), buffer.getInt(1), buffer.getDouble(1 + 4), stringSize, new String(buffer.getByteArray(1 + 4 + 8 + 4, stringSize), "US-ASCII"))

    //   done = (RootReaderCPPLibrary.next(treeWalker) == 0)
    // }
  }
}
