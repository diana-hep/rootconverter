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

import org.dianahep.scaroot.avroreader._

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

case class TwoMuon(mass_mumu: Float, px: Float, py: Float, pz: Float)

class DefaultSuite extends FlatSpec with Matchers {
  "stuff" must "work" in {

    val iterator = new RootTreeIterator[TwoMuon](List("/home/pivarski/DEMO/DATA/TrackResonanceNtuple.root"), "TrackResonanceNtuple/twoMuon", command = "../root2avro/build/root2avro", end = 10L)

    while (iterator.hasNext)
      println(iterator.next())

  }
}
