// These classes represent your data. ScaROOT-Reader will convert ROOT
// TTree data into instances of these classes for you to perform your
// analysis.
//
// You can make modifications to thils file, with some constraints.
//
//     * You can add member data or member functions to the classes by
//       giving them a curly-bracket body and "val", "var", or "def"
//       statements in Scala syntax (see example below).
//
//     * You may not change the names, number, or order of constructor
//       arguments. ScaROOT-Reader fills the classes through their
//       constructors.
//
//     * You can, however, change some of the constructor types. In
//       particular, sequences (marked with Seq) can be changed to
//       any of the following: Array, List, mutable.ListBuffer,
//       Vector, Set, or mutable.Set. See Scala documentation for the
//       relative advantages of each.
//
//     * Any numeric type can be replaced with a wider type: e.g.
//       Int can be replaced with Long or Double, but not the other
//       way around.
//
// Note that the JVM (and therefore Scala) has no unsigned integer
// types. Unsigned integers are mapped to the next larger numeric
// type (e.g. "unsigned int" goes to Long and "unsigned long" goes
// to Double).
//
// Example members added to a class (quick Scala primer). Only the
// class name and original constructor arguments are automatically
// generated.
//
// case class Muon(px: Double,
//                 py: Double,
//                 pz: Double) {       // add the curly braces
//
//   val mass = 0.105                  // "val" makes a constant
//
//   var variable: Double = 0.0        // "var" makes a variable
//                                     // (type annotations are
//                                     // optional)
//
//   def pt = Math.sqrt(px*px + py*py) // "def" without arguments
//                                     // is evaluated when asked
//
//   def deltaR(candidate: GenParticle): Double = {
//     var tmpVariable = ...           // full function body has
//     ...                             // arguments, return type
//     result             // and last expression is return value
//   }
//
//   var bestGenParticle: Option[GenParticle] = None
//   // use "Option" to make variables that aren't known until
//   // runtime and fill them with "Some(genParticle)"
// }

package data

import scala.collection.mutable
import scala.collection.JavaConversions._

import org.dianahep.scaroot.reader._

package root {
  case class Tree(event: Event)

  case class Event(fType: String,   // event type
                   fEventName: Option[String],   // run+event number in character format
                   fNtrack: Int,   // Number of tracks
                   fNseg: Int,   // Number of track segments
                   fNvertex: Int,
                   fFlag: Long,
                   fTemperature: Double,
                   fMeasures: Seq[Int],
                   fMatrix: Seq[Seq[Double]],
                   fClosestDistance: Option[Double],   // [fNvertex][0,0,6]
                   fEvtHdr: EventHeader,
                   fTracks: Option[Seq[Track]],   // ->array with all tracks
                   fTriggerBits: TBits,   // Bits triggered by this event.
                   fIsValid: Boolean)

  case class EventHeader(fEvtNum: Int,
                         fRun: Int,
                         fDate: Int)

  case class Track(
    fPx: Float,
    fPy: Float,
    fPz: Float,
    fRandom: Float,
    fMass2: Float,
    fBx: Float,
    fBy: Float,
    fMeanCharge: Float,
    fXfirst: Float,
    fXlast: Float,
    fYfirst: Float,
    fYlast: Float,
    fZfirst: Float,
    fZlast: Float,
    fCharge: Double,
    fVertex: Seq[Double],
    fNpoint: Int,
    fValid: Short,
    fNsp: Int,
    fPointValue: Option[Double],
    fTriggerBits: TBits)

  case class TBits(fNbits: Long,   // Highest bit set + 1
                   fNbytes: Long,   // Number of UChars in fAllBits
                   fAllBits: Option[Short])   // [fNbytes] array of UChars
}

package object root {
  val myclasses = Map("Event" -> My[Event], "EventHeader" -> My[EventHeader], "Track" -> My[Track], "TBits" -> My[TBits])
}
