// Copyright 2016 Jim Pivarski
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package test.scala.scaroot

import scala.collection.mutable

import org.scalatest.FlatSpec
import org.scalatest.junit.JUnitRunner
import org.scalatest.Matchers

import com.sun.jna.Pointer
import org.dianahep.scaroot.oldreader._

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

  "Complex example" must "work" in {
    val customizations = List(
      'event :: 'fEventName :: CustomPointer {x: Option[String] => x.getOrElse("MISSING")},
      'event :: 'fTracks :: '# :: 'fPointValue :: CustomPointer {x: Option[Double] => x.map(_.toString).getOrElse("MISSING")},
      'event :: 'fClosestDistance :: CustomPointer {x: Option[Double] => x.map(_.toString).getOrElse("MISSING")}
    )

    val iter = RootTreeIterator[GenericClass](List("../root2avro/test_Event/Event.root"), "T", List("../root2avro/test_Event/Event_cxx.so"), customizations)
    var i = 0
    while (iter.hasNext  &&  i < 10) {
      println(iter.next())
      i += 1
    }
  }
}
