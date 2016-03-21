package test.scala.scaroot

import scala.collection.mutable

import org.scalatest.FlatSpec
import org.scalatest.junit.JUnitRunner
import org.scalatest.Matchers

import com.sun.jna.Pointer
import org.dianahep.scaroot.directreader._

class DefaultSuite extends FlatSpec with Matchers {
  "GenericClass" must "work" in {
    val iter = RootTreeIterator[GenericClass](List("../root2avro/build/multipleLeaves.root", "../root2avro/build/multipleLeaves.root"), "t")
    while (iter.hasNext)
      println(iter.next())
  }

  "SpecificClass" must "work" in {
    case class SpecificClass(x: Int, y: Double, z: String)

    val customizations = List(
      CustomClass[SpecificClass] named "t"
    )

    val iter = RootTreeIterator[SpecificClass](List("../root2avro/build/multipleLeaves.root"), "t", customizations = customizations)
    while (iter.hasNext)
      println(iter.next())
  }

  "Customizations" must "work" in {
    case class SpecificClass(x: List[Vector[String]])

    val customizations = List(
      CustomClass[SpecificClass] named "t",
      'x :: '# :: '# :: Custom {p: Pointer => ":" + p.getDouble(0).toString + ":"},
      'x :: '# :: CustomSequence {size: Int => Vector.newBuilder[String]},
      'x :: CustomSequence {size: Int => List.newBuilder[Vector[String]]}
    )
    
    val iter = RootTreeIterator[SpecificClass](List("../root2avro/build/vectorVectorDouble.root"), "t", customizations = customizations)
    while (iter.hasNext)
      println(iter.next())
  }
}
