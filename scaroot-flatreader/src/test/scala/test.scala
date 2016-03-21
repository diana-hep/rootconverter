package test.scala.scaroot

import scala.collection.mutable

import org.scalatest.FlatSpec
import org.scalatest.junit.JUnitRunner
import org.scalatest.Matchers

import com.sun.jna.Pointer
import com.sun.jna.Memory
import com.sun.jna.NativeLong
import org.dianahep.scaroot.flatreader._

class DefaultSuite extends FlatSpec with Matchers {
  "stuff" must "work" in {
    val fileLocations = List("../root2avro/build/multipleLeaves.root")
    val treeLocation = "t"
    val libs = List[String]()
    val customizations = List[Customization]()

    var libscpp = Pointer.NULL
    libs foreach {lib => libscpp = RootReaderCPPLibrary.addVectorString(libscpp, lib)}

    var done = true
    var treeWalker = Pointer.NULL
    var remainingFiles = fileLocations.toList
    var schema: Schema[GenericClass] = null

    if (!fileLocations.isEmpty) {
      treeWalker = RootReaderCPPLibrary.newTreeWalker(remainingFiles.head, treeLocation, "", libscpp)
      remainingFiles = remainingFiles.tail

      if (RootReaderCPPLibrary.valid(treeWalker) == 0)
        throw new RuntimeException(RootReaderCPPLibrary.errorMessage(treeWalker))

      done = (RootReaderCPPLibrary.next(treeWalker) == 0)
      while (!done  &&  RootReaderCPPLibrary.resolved(treeWalker) == 0) {
        RootReaderCPPLibrary.resolve(treeWalker)
        done = (RootReaderCPPLibrary.next(treeWalker) == 0)
      }

      schema = Schema[GenericClass](treeWalker, customizations)

      RootReaderCPPLibrary.setEntryInCurrentTree(treeWalker, 0L)
    }

    val size = 64*1024
    val buffer = new Memory(size)
    buffer.setByte(0, 1)
    println("before")
    RootReaderCPPLibrary.copyToBuffer(treeWalker, 1, buffer, new NativeLong(size))
    println("after")
    println(buffer.getByte(0))
    println(buffer.getInt(1))
    println(buffer.getDouble(1 + 4))
    val stringSize = buffer.getInt(1 + 4 + 8)
    println(stringSize)
    println(new String(buffer.getByteArray(1 + 4 + 8 + 1, stringSize)))
    println("done")

  }
}
