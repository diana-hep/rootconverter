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

package test.scala.scaroot.reader

import scala.collection.mutable

import org.scalatest.FlatSpec
import org.scalatest.junit.JUnitRunner
import org.scalatest.Matchers

// import org.dianahep.scaroot.avroreader._

// case class Tree(event: Event)

// case class Event(
//   fType: String,
//   fEventName: Option[String],
//   fNtrack: Int,
//   fNseg: Int,
//   fNvertex: Int,
//   fFlag: Long,
//   fTemperature: Double,
//   fMeasures: Seq[Int],
//   fMatrix: Seq[Seq[Double]],
//   fClosestDistance: Option[Double],
//   fEvtHdr: EventHeader,
//   fTracks: Option[Seq[Track]],
//   fTriggerBits: TBits,
//   fIsValid: Boolean)

// case class EventHeader(fEvtNum: Int, fRun: Int, fDate: Int)

// case class Track(
//   fPx: Float,
//   fPy: Float,
//   fPz: Float,
//   fRandom: Float,
//   fMass2: Float,
//   fBx: Float,
//   fBy: Float,
//   fMeanCharge: Float,
//   fXfirst: Float,
//   fXlast: Float,
//   fYfirst: Float,
//   fYlast: Float,
//   fZfirst: Float,
//   fZlast: Float,
//   fCharge: Double,
//   fVertex: Seq[Double],
//   fNpoint: Int,
//   fValid: Short,
//   fNsp: Int,
//   fPointValue: Option[Double],
//   fTriggerBits: TBits)

// case class TBits(fNbits: Long, fNbytes: Long, fAllBits: Option[Short])

import scala.util.{Try, Success, Failure}

import com.gensler.scalavro.types.AvroType
import com.gensler.scalavro.io.AvroTypeIO

case class TwoMuon(mass_mumu: Float, px: Float, py: Float, pz: Float)

class DefaultSuite extends FlatSpec with Matchers {
  "stuff" must "work" in {

    val l = AvroType[Long]
    println(l.schema)

    val inputStream = new java.io.FileInputStream("testraw.avro")

    val t = AvroType[TwoMuon]
    println(t.schema)

    val Success(l1) = l.io read inputStream
    val Success(readResult1) = t.io read inputStream
    println(l1, readResult1)

    val Success(l2) = l.io read inputStream
    val Success(readResult2) = t.io read inputStream
    println(l2, readResult2)

    val Success(l3) = l.io read inputStream
    val Success(readResult3) = t.io read inputStream
    println(l3, readResult3)

    val Success(l4) = l.io read inputStream
    val Success(readResult4) = t.io read inputStream
    println(l4, readResult4)

    val Success(l5) = l.io read inputStream
    val Success(readResult5) = t.io read inputStream
    println(l5, readResult5)

    val Success(l6) = l.io read inputStream
    val Success(readResult6) = t.io read inputStream
    println(l6, readResult6)

    val Success(l7) = l.io read inputStream
    val Success(readResult7) = t.io read inputStream
    println(l7, readResult7)

    val Success(l8) = l.io read inputStream
    val Success(readResult8) = t.io read inputStream
    println(l8, readResult8)

    val Success(l9) = l.io read inputStream
    val Success(readResult9) = t.io read inputStream
    println(l9, readResult9)

    val Success(l10) = l.io read inputStream
    val Success(readResult10) = t.io read inputStream
    println(l10, readResult10)

  }
}
