// These classes represent your data. ScaROOT-Reader will convert ROOT TTree data into instances of these classes for
// you to perform your analysis.
//
// You can add member data or member functions to these classes (in curly brackets after the "extends Serializable").
// However, you cannot change the constructor arguments (in parentheses after the class name).
//
// Use the example below as a guide to adding functionality:
//
// case class Muon(px: Double,                          // The constructor arguments are automatically generated.
//                 py: Double,
//                 pz: Double) extends Serializable {   // Add curly braces to define members for this class.
//
//   val mass = 0.105                                   // "val" makes a constant (which may be from a formula).
//
//   var variable: Double = 0.0                         // "var" makes a variable (type annotations are optional).
//
//   def pt = Math.sqrt(px*px + py*py)                  // "def" without arguments is evaluated when asked.
//
//   def deltaR(candidate: GenParticle): Double = {     // Full function body has arguments and a return type.
//     var tmpVariable = ...
//     ...
//     result                                           // The last expression is return value.
//   }
//
//   var bestGenParticle: Option[GenParticle] = None    // Use "Option" to make variables that aren't known until
// }                                                    // runtime and fill them with "Some(genParticle)" or None.
//
// Note that the JVM (and therefore Scala) has no unsigned integer types. Unsigned integers are mapped to the next
// larger numeric type (e.g. "unsigned int" goes to Long and "unsigned long" goes to Double).

package data

import org.dianahep.scaroot.reader._

package root {
  case class Tree(event_split: Event,
                  event_not_split: Event) extends Serializable

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
                   fIsValid: Boolean) extends Serializable

  case class EventHeader(fEvtNum: Int,
                         fRun: Int,
                         fDate: Int) extends Serializable

  case class Track(fPx: Float,   // X component of the momentum
                   fPy: Float,   // Y component of the momentum
                   fPz: Float,   // Z component of the momentum
                   fRandom: Float,   // A random track quantity
                   fMass2: Float,   // [0,0,8] The mass square of this particle
                   fBx: Float,   // [0,0,10] X intercept at the vertex
                   fBy: Float,   // [0,0,10] Y intercept at the vertex
                   fMeanCharge: Float,   // Mean charge deposition of all hits of this track
                   fXfirst: Float,   // X coordinate of the first point
                   fXlast: Float,   // X coordinate of the last point
                   fYfirst: Float,   // Y coordinate of the first point
                   fYlast: Float,   // Y coordinate of the last point
                   fZfirst: Float,   // Z coordinate of the first point
                   fZlast: Float,   // Z coordinate of the last point
                   fCharge: Double,   // [-1,1,2] Charge of this track
                   fVertex: Seq[Double],   // [-30,30,16] Track vertex position
                   fNpoint: Int,   // Number of points for this track
                   fValid: Short,   // Validity criterion
                   fNsp: Int,   // Number of points for this track with a special value
                   fPointValue: Option[Double],   // [fNsp][0,3] a special quantity for some point.
                   fTriggerBits: TBits)   // Bits triggered by this track.
                   extends Serializable

  case class TBits(fNbits: Long,   // Highest bit set + 1
                   fNbytes: Long,   // Number of UChars in fAllBits
                   fAllBits: Option[Short])   // [fNbytes] array of UChars
                   extends Serializable
}

package object root {
  // Pass 'myclasses' into RootTreeIterator to tell it to fill these classes, rather than 'Generic'.
  val myclasses = Map("t4" -> My[Tree], "Event" -> My[Event], "EventHeader" -> My[EventHeader], "Track" -> My[Track], "TBits" -> My[TBits])

  // Unless you move it, this is the location of the tree, provided for convenience and reusable code.
  val treeLocation = "t4"
}
