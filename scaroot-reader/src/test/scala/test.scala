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

import org.dianahep.scaroot.reader._
import org.dianahep.scaroot.reader.schema._

///////////////////////////////////////////////////////////////// Event.root classes

case class Tree(event: Event)

case class Event(
  fType: String,
  fEventName: Option[String],
  fNtrack: Int,
  fNseg: Int,
  fNvertex: Int,
  fFlag: Long,
  fTemperature: Double,
  fMeasures: Seq[Int],
  fMatrix: Seq[Seq[Double]],
  fClosestDistance: Option[Double],
  fEvtHdr: EventHeader,
  fTracks: Option[Seq[Track]],
  fTriggerBits: TBits,
  fIsValid: Boolean)

case class EventHeader(fEvtNum: Int, fRun: Int, fDate: Int)

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

case class TBits(fNbits: Long, fNbytes: Long, fAllBits: Option[Short])

///////////////////////////////////////////////////////////////// Bacon.root classes

  case class Tree2(Info: baconhep.TEventInfo,
                   GenEvtInfo: baconhep.TGenEventInfo,
                   GenParticle: Seq[baconhep.TGenParticle],
                   LHEWeight: Seq[baconhep.TLHEWeight],
                   Electron: Seq[baconhep.TElectron],
                   Muon: Seq[baconhep.TMuon],
                   Tau: Seq[baconhep.TTau],
                   Photon: Seq[baconhep.TPhoton],
                   PV: Seq[baconhep.TVertex],
                   AK4CHS: Seq[baconhep.TJet],
                   AK8CHS: Seq[baconhep.TJet],
                   AddAK8CHS: Seq[baconhep.TAddJet],
                   CA15CHS: Seq[baconhep.TJet],
                   AddCA15CHS: Seq[baconhep.TAddJet],
                   AK4Puppi: Seq[baconhep.TJet],
                   CA8Puppi: Seq[baconhep.TJet],
                   AddCA8Puppi: Seq[baconhep.TAddJet],
                   CA15Puppi: Seq[baconhep.TJet],
                   AddCA15Puppi: Seq[baconhep.TAddJet]) extends Serializable

package baconhep {
  class TEventInfo(val runNum: Long,   // run number, event number, lumi section in data
                   val evtNum: Long,   // run number, event number, lumi section in data
                   val lumiSec: Long,   // run number, event number, lumi section in data
                   val metFilterFailBits: Long,   // MET filter fail bits
                   val nPU: Long,   // number of reconstructed pile-up vertices
                   val nPUm: Long,   // number of reconstructed pile-up vertices
                   val nPUp: Long,   // number of reconstructed pile-up vertices
                   val nPUmean: Float,   // expected number of reconstructed pile-up vertices
                   val nPUmeanm: Float,   // expected number of reconstructed pile-up vertices
                   val nPUmeanp: Float,   // expected number of reconstructed pile-up vertices
                   val pvx: Float,   // best primary vertex
                   val pvy: Float,   // best primary vertex
                   val pvz: Float,   // best primary vertex
                   val bsx: Float,   // beamspot
                   val bsy: Float,   // beamspot
                   val bsz: Float,   // beamspot
                   val caloMET: Float,   // calo MET
                   val caloMETphi: Float,   // calo MET
                   val caloMETCov00: Float,
                   val caloMETCov01: Float,
                   val caloMETCov11: Float,
                   val pfMET: Float,   // particle flow MET
                   val pfMETphi: Float,   // particle flow MET
                   val pfMETCov00: Float,
                   val pfMETCov01: Float,
                   val pfMETCov11: Float,
                   val pfMETC: Float,   // corrected particle flow MET
                   val pfMETCphi: Float,   // corrected particle flow MET
                   val pfMETCCov00: Float,
                   val pfMETCCov01: Float,
                   val pfMETCCov11: Float,
                   val mvaMET: Float,   // MVA MET
                   val mvaMETphi: Float,   // MVA MET
                   val mvaMETCov00: Float,
                   val mvaMETCov01: Float,
                   val mvaMETCov11: Float,
                   val puppET: Float,   // PUPPI MET
                   val puppETphi: Float,   // PUPPI MET
                   val puppETCov00: Float,
                   val puppETCov01: Float,
                   val puppETCov11: Float,
                   val puppETC: Float,   // Type1 PUPPI MET
                   val puppETCphi: Float,   // Type1 PUPPI MET
                   val puppETCCov00: Float,
                   val puppETCCov01: Float,
                   val puppETCCov11: Float,
                   val pfMET30: Float,   // particle flow MET
                   val pfMET30phi: Float,   // particle flow MET
                   val pfMETC30: Float,   // corrected particle flow MET
                   val pfMETC30phi: Float,   // corrected particle flow MET
                   val mvaMET30: Float,   // MVA MET
                   val mvaMET30phi: Float,   // MVA MET
                   val puppET30: Float,   // PUPPI MET
                   val puppET30phi: Float,   // PUPPI MET
                   val puppETC30: Float,   // Type1 PUPPI MET
                   val puppETC30phi: Float,   // Type1 PUPPI MET
                   val alpacaMET: Float,   // Alpaca MET
                   val alpacaMETphi: Float,   // Alpaca MET
                   val pcpMET: Float,   // Alapaca + PUPPI MET
                   val pcpMETphi: Float,   // Alapaca + PUPPI MET
                   val trkMET: Float,   // track MET
                   val trkMETphi: Float,   // track MET
                   val rhoIso: Float,   // event energy density
                   val rhoJet: Float,   // event energy density
                   val hasGoodPV: Boolean)   // event has a good PV?
                   extends Serializable
  }

package baconhep {
  case class TGenEventInfo(id_1: Int,   // parton flavor PDG ID
                           id_2: Int,   // parton flavor PDG ID
                           x_1: Float,   // parton momentum fraction
                           x_2: Float,   // parton momentum fraction
                           scalePDF: Float,   // Q-scale used for PDF evaluation
                           xs: Float,   // cross section from LHE file
                           weight: Float)   // generator-level event weight
                           extends Serializable
  }

package baconhep {
  case class TGenParticle(parent: Int,
                          pdgId: Int,
                          status: Int,
                          pt: Float,
                          eta: Float,
                          phi: Float,
                          mass: Float,
                          y: Float) extends Serializable
  }

package baconhep {
  case class TLHEWeight(id: Int,   // parton flavor PDG ID
                        weight: Float)   // generator-level event weight
                        extends Serializable
  }

package baconhep {
  class TElectron(val pt: Float,   // kinematics
                  val eta: Float,   // kinematics
                  val phi: Float,   // kinematics
                  val scEt: Float,   // supercluster kinematics
                  val scEta: Float,   // supercluster kinematics
                  val scPhi: Float,   // supercluster kinematics
                  val ecalEnergy: Float,   // ECAL energy
                  val pfPt: Float,   // matching PF-candidate kinematics
                  val pfEta: Float,   // matching PF-candidate kinematics
                  val pfPhi: Float,   // matching PF-candidate kinematics
                  val trkIso: Float,   // detector isolation
                  val ecalIso: Float,   // detector isolation
                  val hcalIso: Float,   // detector isolation
                  val hcalDepth1Iso: Float,   // detector isolation
                  val chHadIso: Float,   // PF isolation variables
                  val gammaIso: Float,   // PF isolation variables
                  val neuHadIso: Float,   // PF isolation variables
                  val puIso: Float,   // PF isolation variables
                  val ecalPFClusIso: Float,   // PF cluster isolation variables
                  val hcalPFClusIso: Float,   // PF cluster isolation variables
                  val puppiChHadIso: Float,   // Puppi Isolation R=0.4
                  val puppiGammaIso: Float,   // Puppi Isolation R=0.4
                  val puppiNeuHadIso: Float,   // Puppi Isolation R=0.4
                  val puppiChHadIsoNoLep: Float,   // Puppi Isolation R=0.4 no lep
                  val puppiGammaIsoNoLep: Float,   // Puppi Isolation R=0.4 no lep
                  val puppiNeuHadIsoNoLep: Float,   // Puppi Isolation R=0.4 no lep
                  val d0: Float,   // impact parameter
                  val dz: Float,   // impact parameter
                  val sip3d: Float,   // impact parameter
                  val sieie: Float,   // shower shape
                  val e1x5: Float,   // shower shape
                  val e2x5: Float,   // shower shape
                  val e5x5: Float,   // shower shape
                  val r9: Float,   // shower shape
                  val eoverp: Float,   // E/p
                  val hovere: Float,   // H/E
                  val fbrem: Float,   // brem fraction
                  val dEtaInSeed: Float,   // track-supercluster matching
                  val dEtaIn: Float,   // track-supercluster matching
                  val dPhiIn: Float,   // track-supercluster matching
                  val mva: Float,   // electron ID MVA value
                  val q: Int,   // charge
                  val isConv: Boolean,   // identified by track fit based conversion finder?
                  val nMissingHits: Long,   // number of missing expected inner hits
                  val typeBits: Long,   // electron type
                  val fiducialBits: Long,   // ECAL fiducial region bits
                  val classification: Int,   // electron classification
                  val scID: Int,   // supercluster ID number (unique per event)
                  val trkID: Int)   // track ID number (unique per event)
                  extends Serializable
  }

package baconhep {
  class TMuon(val pt: Float,   // kinematics
              val eta: Float,   // kinematics
              val phi: Float,   // kinematics
              val ptErr: Float,   // kinematics
              val staPt: Float,   // STA track kinematics
              val staEta: Float,   // STA track kinematics
              val staPhi: Float,   // STA track kinematics
              val pfPt: Float,   // matched PFCandidate
              val pfEta: Float,   // matched PFCandidate
              val pfPhi: Float,   // matched PFCandidate
              val trkIso: Float,   // detector isolation (R=0.3)
              val ecalIso: Float,   // detector isolation (R=0.3)
              val hcalIso: Float,   // detector isolation (R=0.3)
              val chHadIso: Float,   // PF isolation variables (R=0.4)
              val gammaIso: Float,   // PF isolation variables (R=0.4)
              val neuHadIso: Float,   // PF isolation variables (R=0.4)
              val puIso: Float,   // PF isolation variables (R=0.4)
              val puppiChHadIso: Float,   // Puppi Isolation R=0.4
              val puppiGammaIso: Float,   // Puppi Isolation R=0.4
              val puppiNeuHadIso: Float,   // Puppi Isolation R=0.4
              val puppiChHadIsoNoLep: Float,   // Puppi Isolation R=0.4 no lep
              val puppiGammaIsoNoLep: Float,   // Puppi Isolation R=0.4 no lep
              val puppiNeuHadIsoNoLep: Float,   // Puppi Isolation R=0.4 no lep
              val d0: Float,   // impact parameter
              val dz: Float,   // impact parameter
              val sip3d: Float,   // impact parameter
              val tkNchi2: Float,   // track fit normalized chi-square
              val muNchi2: Float,   // track fit normalized chi-square
              val trkKink: Float,   // track kink
              val glbKink: Float,   // track kink
              val trkHitFrac: Float,   // fraction of valid tracker hits
              val chi2LocPos: Float,   // TRK-STA position match
              val segComp: Float,   // compatibility of tracker track with muon segment
              val caloComp: Float,   // muon hypothesis compatibility with calo energy
              val q: Int,   // charge
              val nValidHits: Int,   // number of valid muon hits in global fit
              val typeBits: Long,   // muon type bits
              val selectorBits: Long,   // MuonSelector bits
              val pogIDBits: Long,   // POG muon IDs from CMSSW
              val nTkHits: Long,   // number of hits in tracker
              val nPixHits: Long,   // number of hits in tracker
              val nTkLayers: Long,   // number of hit layers in tracker
              val nPixLayers: Long,   // number of hit layers in tracker
              val nMatchStn: Long,   // number of stations with muon segments
              val trkID: Int)   // tracker track ID (unique per event)
              extends Serializable
  }

package baconhep {
  class TTau(val pt: Float,   // kinematics
             val eta: Float,   // kinematics
             val phi: Float,   // kinematics
             val m: Float,   // kinematics
             val e: Float,   // kinematics
             val q: Int,   // charge
             val dzLeadChHad: Float,   // longitudinal impact parameter of leading charged hadron
             val nSignalChHad: Long,   // number of charged hadrons in tau
             val nSignalGamma: Long,   // number of photons in tau
             val antiEleMVA5: Float,   // anti-electron MVA5 for Run 2
             val antiEleMVA5Cat: Float,   // anti-electron MVA5 category for Run 2
             val rawMuonRejection: Float,   // raw MVA output of BDT based anti-muon discriminator
             val rawIso3Hits: Float,   // raw Iso 3 hits
             val rawIsoMVA3oldDMwoLT: Float,   // raw Iso MVA3, trained on prong 1 and 3, without lifetime info
             val rawIsoMVA3oldDMwLT: Float,   // raw Iso MVA3, trained on prong 1 and 3, with lifetime info
             val rawIsoMVA3newDMwoLT: Float,   // raw Iso MVA3, trained on prong 1, 2, and 3, without lifetime info
             val rawIsoMVA3newDMwLT: Float,   // raw Iso MVA3, trained on prong 1, 2, and 3, with lifetime info
             val puppiChHadIso: Float,   // Puppi Isolation R=0.4
             val puppiGammaIso: Float,   // Puppi Isolation R=0.4
             val puppiNeuHadIso: Float,   // Puppi Isolation R=0.4
             val puppiChHadIsoNoLep: Float,   // Puppi Isolation R=0.4 no lep
             val puppiGammaIsoNoLep: Float,   // Puppi Isolation R=0.4 no lep
             val puppiNeuHadIsoNoLep: Float,   // Puppi Isolation R=0.4 no lep
             val hpsDisc: Double)   // HPS tau discriminators
             extends Serializable
  }

package baconhep {
  class TPhoton(val pt: Float,   // kinematics
                val eta: Float,   // kinematics
                val phi: Float,   // kinematics
                val scEt: Float,   // SuperCluster kinematics
                val scEta: Float,   // SuperCluster kinematics
                val scPhi: Float,   // SuperCluster kinematics
                val trkIso: Float,   // detector isolation
                val ecalIso: Float,   // detector isolation
                val hcalIso: Float,   // detector isolation
                val chHadIso: Float,   // PF isolation variables
                val gammaIso: Float,   // PF isolation variables
                val neuHadIso: Float,   // PF isolation variables
                val mva: Float,   // Photon MVA ID
                val hovere: Float,   // H/E
                val sthovere: Float,   // Single tower H/E (https://twiki.cern.ch/twiki/bin/viewauth/CMS/HoverE2012)
                val sieie: Float,   // shower shape
                val sipip: Float,   // shower shape
                val r9: Float,   // shower shape
                val fiducialBits: Long,   // ECAL fiducial region
                val typeBits: Long,   // photon type
                val scID: Int,   // supercluster ID (unique per event)
                val hasPixelSeed: Boolean,   // has pixel seed?
                val passElectronVeto: Boolean,   // if false, its not a photon
                val isConv: Boolean) extends Serializable
  }

package baconhep {
  case class TVertex(nTracksFit: Long,   // number of tracks in vertex fit
                     ndof: Float,   // vertex fit number of degrees of freedom
                     chi2: Float,   // vertex fit chi-square
                     x: Float,   // position
                     y: Float,   // position
                     z: Float)   // position
                     extends Serializable
  }

package baconhep {
  class TJet(val pt: Float,   // kinematics
             val eta: Float,   // kinematics
             val phi: Float,   // kinematics
             val mass: Float,   // kinematics
             val ptRaw: Float,   // kinematics
             val unc: Float,   // kinematics
             val area: Float,   // jet area (from FastJet)
             val d0: Float,   // impact parameter of leading charged constituent
             val dz: Float,   // impact parameter of leading charged constituent
             val csv: Float,   // CSV b-tagger for the jet and subjets
             val qgid: Float,   // q/g discriminator and input variables
             val axis2: Float,   // q/g discriminator and input variables
             val ptD: Float,   // q/g discriminator and input variables
             val mult: Int,
             val q: Float,   // Charge for jet and subjets
             val mva: Float,   // PU discriminator MVA
             val beta: Float,   // input variables for PU and q/g discriminators
             val betaStar: Float,   // input variables for PU and q/g discriminators
             val dR2Mean: Float,   // input variables for PU and q/g discriminators
             val pullY: Float,   // Jet pull
             val pullPhi: Float,   // Jet pull
             val chPullY: Float,
             val chPullPhi: Float,
             val neuPullY: Float,
             val neuPullPhi: Float,
             val chEmFrac: Float,   // fractional energy contribution by type
             val neuEmFrac: Float,   // fractional energy contribution by type
             val chHadFrac: Float,   // fractional energy contribution by type
             val neuHadFrac: Float,   // fractional energy contribution by type
             val muonFrac: Float,   // fractional energy contribution by type
             val genpt: Float,   // Matched GenJet
             val geneta: Float,   // Matched GenJet
             val genphi: Float,   // Matched GenJet
             val genm: Float,   // Matched GenJet
             val partonFlavor: Int,   // Flavor
             val hadronFlavor: Int,   // Flavor
             val nCharged: Long,   // constituent multiplicity
             val nNeutrals: Long,   // constituent multiplicity
             val nParticles: Long)   // constituent multiplicity
             extends Serializable
  }

package baconhep {
  class TAddJet(val index: Long,   // index in original jet collection
                val mass_prun: Float,   // groomed jet masses
                val mass_trim: Float,   // groomed jet masses
                val mass_sd0: Float,   // groomed jet masses
                val pt_sd0: Float,   // soft drop
                val eta_sd0: Float,   // soft drop
                val phi_sd0: Float,   // soft drop
                val c2_0: Float,   // Correlation function with various exponents
                val c2_0P2: Float,   // Correlation function with various exponents
                val c2_0P5: Float,   // Correlation function with various exponents
                val c2_1P0: Float,   // Correlation function with various exponents
                val c2_2P0: Float,   // Correlation function with various exponents
                val qjet: Float,   // Q-jet volatility
                val tau1: Float,   // N-subjettiness
                val tau2: Float,   // N-subjettiness
                val tau3: Float,   // N-subjettiness
                val tau4: Float,   // N-subjettiness
                val doublecsv: Float,   // Double b-tag
                val sj1_pt: Float,
                val sj1_eta: Float,
                val sj1_phi: Float,
                val sj1_m: Float,
                val sj1_csv: Float,
                val sj1_qgid: Float,
                val sj1_q: Float,
                val sj2_pt: Float,
                val sj2_eta: Float,
                val sj2_phi: Float,
                val sj2_m: Float,
                val sj2_csv: Float,
                val sj2_qgid: Float,
                val sj2_q: Float,
                val sj3_pt: Float,
                val sj3_eta: Float,
                val sj3_phi: Float,
                val sj3_m: Float,
                val sj3_csv: Float,
                val sj3_qgid: Float,
                val sj3_q: Float,
                val sj4_pt: Float,
                val sj4_eta: Float,
                val sj4_phi: Float,
                val sj4_m: Float,
                val sj4_csv: Float,
                val sj4_qgid: Float,
                val sj4_q: Float,
                val pullAngle: Float,
                val nTracks: Float,   // Number of tracks associated to the jet
                val trackSip3dSig_3: Float,   // 3D SIP (IP value/error) for the most displaced tracks associated to the jet
                val trackSip3dSig_2: Float,   // 3D SIP (IP value/error) for the most displaced tracks associated to the jet
                val trackSip3dSig_1: Float,   // 3D SIP (IP value/error) for the most displaced tracks associated to the jet
                val trackSip3dSig_0: Float,   // 3D SIP (IP value/error) for the most displaced tracks associated to the jet
                val trackSip2dSigAboveCharm: Float,   // 2D SIP of the first tracks that raises the Mass above the bottom (charm) threshold 5.2GeV (1.5GeV)
                val trackSip2dSigAboveBottom: Float,   // 2D SIP of the first tracks that raises the Mass above the bottom (charm) threshold 5.2GeV (1.5GeV)
                val tau1_trackSip3dSig_0: Float,   // 3D SIP of the two tracks with the highest SIP associated to the closest tau axis to the track
                val tau1_trackSip3dSig_1: Float,   // 3D SIP of the two tracks with the highest SIP associated to the closest tau axis to the track
                val nSV: Float,
                val tau_SVmass_nSecondaryVertices: Float,   // Number of SVs (cross check), 2D flight distance significance
                val tau_SVmass_flightDistance2dSig: Float,   // Number of SVs (cross check), 2D flight distance significance
                val tau_SVmass_vertexDeltaR: Float,   // DeltaR between the secondary vertex flight direction and the jet axis
                val tau_SVmass_vertexNTracks: Float,   // DeltaR between the secondary vertex flight direction and the jet axis
                val tau_SVmass_trackEtaRel_2: Float,   // Pseudorapidity of the tracks at the vertex with respect to the tau axis closest to the leading SV in mass
                val tau_SVmass_trackEtaRel_1: Float,   // Pseudorapidity of the tracks at the vertex with respect to the tau axis closest to the leading SV in mass
                val tau_SVmass_trackEtaRel_0: Float,   // Pseudorapidity of the tracks at the vertex with respect to the tau axis closest to the leading SV in mass
                val tau_SVmass_vertexEnergyRatio: Float,   // Energy ratio and vertexMass
                val tau_SVmass_vertexMass: Float,   // Energy ratio and vertexMass
                val tau_SVmass_vertexMass_corrected: Float,   // Energy ratio and vertexMass
                val tau_SVmass_zratio: Float,   // z-ratio
                val tau_SVfd_nSecondaryVertices: Float,
                val tau_SVfd_flightDistance2dSig: Float,
                val tau_SVfd_vertexDeltaR: Float,
                val tau_SVfd_vertexNTracks: Float,
                val tau_SVfd_trackEtaRel_2: Float,
                val tau_SVfd_trackEtaRel_1: Float,
                val tau_SVfd_trackEtaRel_0: Float,
                val tau_SVfd_vertexEnergyRatio: Float,
                val tau_SVfd_vertexMass: Float,
                val tau_SVfd_vertexMass_corrected: Float,
                val tau_SVfd_zratio: Float,
                val topTagType: Long,
                val top_n_subjets: Long,
                val top_m_min: Float,
                val top_m_123: Float,
                val top_fRec: Float,
                val topchi2: Float) extends Serializable
  }

///////////////////////////////////////////////////////////////// actual tests

class DefaultSuite extends FlatSpec with Matchers {
  "Event.root" must "work" in {
    val myclasses = Map("Event" -> My[Event], "EventHeader" -> My[EventHeader], "Track" -> My[Track], "TBits" -> My[TBits])

    val iterator = RootTreeIterator[Tree](List("../root2avro/test_Event/Event.root"), "T", inferTypes = true, myclasses = myclasses)

    println(iterator.schema)
    println(iterator.factory)
    println(iterator.repr)

    var i = 0
    while (iterator.hasNext  &&  i < 10) {
      println(iterator.next().event.fEventName)
      i += 1
    }
  }

  "Bacon.root" must "work" in {
    val schema = SchemaClass(name = "Tree2", fields = List(
      SchemaField("Info", "", SchemaClass(name = "baconhep::TEventInfo", fields = List(
        SchemaField("runNum", "run number, event number, lumi section in data", SchemaUInt),
        SchemaField("evtNum", "run number, event number, lumi section in data", SchemaUInt),
        SchemaField("lumiSec", "run number, event number, lumi section in data", SchemaUInt),
        SchemaField("metFilterFailBits", "MET filter fail bits", SchemaUInt),
        SchemaField("nPU", "number of reconstructed pile-up vertices", SchemaUInt),
        SchemaField("nPUm", "number of reconstructed pile-up vertices", SchemaUInt),
        SchemaField("nPUp", "number of reconstructed pile-up vertices", SchemaUInt),
        SchemaField("nPUmean", "expected number of reconstructed pile-up vertices", SchemaFloat),
        SchemaField("nPUmeanm", "expected number of reconstructed pile-up vertices", SchemaFloat),
        SchemaField("nPUmeanp", "expected number of reconstructed pile-up vertices", SchemaFloat),
        SchemaField("pvx", "best primary vertex", SchemaFloat),
        SchemaField("pvy", "best primary vertex", SchemaFloat),
        SchemaField("pvz", "best primary vertex", SchemaFloat),
        SchemaField("bsx", "beamspot", SchemaFloat),
        SchemaField("bsy", "beamspot", SchemaFloat),
        SchemaField("bsz", "beamspot", SchemaFloat),
        SchemaField("caloMET", "calo MET", SchemaFloat),
        SchemaField("caloMETphi", "calo MET", SchemaFloat),
        SchemaField("caloMETCov00", "", SchemaFloat),
        SchemaField("caloMETCov01", "", SchemaFloat),
        SchemaField("caloMETCov11", "", SchemaFloat),
        SchemaField("pfMET", "particle flow MET", SchemaFloat),
        SchemaField("pfMETphi", "particle flow MET", SchemaFloat),
        SchemaField("pfMETCov00", "", SchemaFloat),
        SchemaField("pfMETCov01", "", SchemaFloat),
        SchemaField("pfMETCov11", "", SchemaFloat),
        SchemaField("pfMETC", "corrected particle flow MET", SchemaFloat),
        SchemaField("pfMETCphi", "corrected particle flow MET", SchemaFloat),
        SchemaField("pfMETCCov00", "", SchemaFloat),
        SchemaField("pfMETCCov01", "", SchemaFloat),
        SchemaField("pfMETCCov11", "", SchemaFloat),
        SchemaField("mvaMET", "MVA MET", SchemaFloat),
        SchemaField("mvaMETphi", "MVA MET", SchemaFloat),
        SchemaField("mvaMETCov00", "", SchemaFloat),
        SchemaField("mvaMETCov01", "", SchemaFloat),
        SchemaField("mvaMETCov11", "", SchemaFloat),
        SchemaField("puppET", "PUPPI MET", SchemaFloat),
        SchemaField("puppETphi", "PUPPI MET", SchemaFloat),
        SchemaField("puppETCov00", "", SchemaFloat),
        SchemaField("puppETCov01", "", SchemaFloat),
        SchemaField("puppETCov11", "", SchemaFloat),
        SchemaField("puppETC", "Type1 PUPPI MET", SchemaFloat),
        SchemaField("puppETCphi", "Type1 PUPPI MET", SchemaFloat),
        SchemaField("puppETCCov00", "", SchemaFloat),
        SchemaField("puppETCCov01", "", SchemaFloat),
        SchemaField("puppETCCov11", "", SchemaFloat),
        SchemaField("pfMET30", "particle flow MET", SchemaFloat),
        SchemaField("pfMET30phi", "particle flow MET", SchemaFloat),
        SchemaField("pfMETC30", "corrected particle flow MET", SchemaFloat),
        SchemaField("pfMETC30phi", "corrected particle flow MET", SchemaFloat),
        SchemaField("mvaMET30", "MVA MET", SchemaFloat),
        SchemaField("mvaMET30phi", "MVA MET", SchemaFloat),
        SchemaField("puppET30", "PUPPI MET", SchemaFloat),
        SchemaField("puppET30phi", "PUPPI MET", SchemaFloat),
        SchemaField("puppETC30", "Type1 PUPPI MET", SchemaFloat),
        SchemaField("puppETC30phi", "Type1 PUPPI MET", SchemaFloat),
        SchemaField("alpacaMET", "Alpaca MET", SchemaFloat),
        SchemaField("alpacaMETphi", "Alpaca MET", SchemaFloat),
        SchemaField("pcpMET", "Alapaca + PUPPI MET", SchemaFloat),
        SchemaField("pcpMETphi", "Alapaca + PUPPI MET", SchemaFloat),
        SchemaField("trkMET", "track MET", SchemaFloat),
        SchemaField("trkMETphi", "track MET", SchemaFloat),
        SchemaField("rhoIso", "event energy density", SchemaFloat),
        SchemaField("rhoJet", "event energy density", SchemaFloat),
        SchemaField("hasGoodPV", "event has a good PV?", SchemaBool)
      ))),
      SchemaField("GenEvtInfo", "", SchemaClass(name = "baconhep::TGenEventInfo", fields = List(
        SchemaField("id_1", "parton flavor PDG ID", SchemaInt),
        SchemaField("id_2", "parton flavor PDG ID", SchemaInt),
        SchemaField("x_1", "parton momentum fraction", SchemaFloat),
        SchemaField("x_2", "parton momentum fraction", SchemaFloat),
        SchemaField("scalePDF", "Q-scale used for PDF evaluation", SchemaFloat),
        SchemaField("xs", "cross section from LHE file", SchemaFloat),
        SchemaField("weight", "generator-level event weight", SchemaFloat)
      ))),
      SchemaField("GenParticle", "", SchemaSequence(SchemaClass(name = "baconhep::TGenParticle", fields = List(
        SchemaField("parent", "", SchemaInt),
        SchemaField("pdgId", "", SchemaInt),
        SchemaField("status", "", SchemaInt),
        SchemaField("pt", "", SchemaFloat),
        SchemaField("eta", "", SchemaFloat),
        SchemaField("phi", "", SchemaFloat),
        SchemaField("mass", "", SchemaFloat),
        SchemaField("y", "", SchemaFloat)
      )))),
      SchemaField("LHEWeight", "", SchemaSequence(SchemaClass(name = "baconhep::TLHEWeight", fields = List(
        SchemaField("id", "parton flavor PDG ID", SchemaInt),
        SchemaField("weight", "generator-level event weight", SchemaFloat)
      )))),
      SchemaField("Electron", "", SchemaSequence(SchemaClass(name = "baconhep::TElectron", fields = List(
        SchemaField("pt", "kinematics", SchemaFloat),
        SchemaField("eta", "kinematics", SchemaFloat),
        SchemaField("phi", "kinematics", SchemaFloat),
        SchemaField("scEt", "supercluster kinematics", SchemaFloat),
        SchemaField("scEta", "supercluster kinematics", SchemaFloat),
        SchemaField("scPhi", "supercluster kinematics", SchemaFloat),
        SchemaField("ecalEnergy", "ECAL energy", SchemaFloat),
        SchemaField("pfPt", "matching PF-candidate kinematics", SchemaFloat),
        SchemaField("pfEta", "matching PF-candidate kinematics", SchemaFloat),
        SchemaField("pfPhi", "matching PF-candidate kinematics", SchemaFloat),
        SchemaField("trkIso", "detector isolation", SchemaFloat),
        SchemaField("ecalIso", "detector isolation", SchemaFloat),
        SchemaField("hcalIso", "detector isolation", SchemaFloat),
        SchemaField("hcalDepth1Iso", "detector isolation", SchemaFloat),
        SchemaField("chHadIso", "PF isolation variables", SchemaFloat),
        SchemaField("gammaIso", "PF isolation variables", SchemaFloat),
        SchemaField("neuHadIso", "PF isolation variables", SchemaFloat),
        SchemaField("puIso", "PF isolation variables", SchemaFloat),
        SchemaField("ecalPFClusIso", "PF cluster isolation variables", SchemaFloat),
        SchemaField("hcalPFClusIso", "PF cluster isolation variables", SchemaFloat),
        SchemaField("puppiChHadIso", "Puppi Isolation R=0.4", SchemaFloat),
        SchemaField("puppiGammaIso", "Puppi Isolation R=0.4", SchemaFloat),
        SchemaField("puppiNeuHadIso", "Puppi Isolation R=0.4", SchemaFloat),
        SchemaField("puppiChHadIsoNoLep", "Puppi Isolation R=0.4 no lep", SchemaFloat),
        SchemaField("puppiGammaIsoNoLep", "Puppi Isolation R=0.4 no lep", SchemaFloat),
        SchemaField("puppiNeuHadIsoNoLep", "Puppi Isolation R=0.4 no lep", SchemaFloat),
        SchemaField("d0", "impact parameter", SchemaFloat),
        SchemaField("dz", "impact parameter", SchemaFloat),
        SchemaField("sip3d", "impact parameter", SchemaFloat),
        SchemaField("sieie", "shower shape", SchemaFloat),
        SchemaField("e1x5", "shower shape", SchemaFloat),
        SchemaField("e2x5", "shower shape", SchemaFloat),
        SchemaField("e5x5", "shower shape", SchemaFloat),
        SchemaField("r9", "shower shape", SchemaFloat),
        SchemaField("eoverp", "E/p", SchemaFloat),
        SchemaField("hovere", "H/E", SchemaFloat),
        SchemaField("fbrem", "brem fraction", SchemaFloat),
        SchemaField("dEtaInSeed", "track-supercluster matching", SchemaFloat),
        SchemaField("dEtaIn", "track-supercluster matching", SchemaFloat),
        SchemaField("dPhiIn", "track-supercluster matching", SchemaFloat),
        SchemaField("mva", "electron ID MVA value", SchemaFloat),
        SchemaField("q", "charge", SchemaInt),
        SchemaField("isConv", "identified by track fit based conversion finder?", SchemaBool),
        SchemaField("nMissingHits", "number of missing expected inner hits", SchemaUInt),
        SchemaField("typeBits", "electron type", SchemaUInt),
        SchemaField("fiducialBits", "ECAL fiducial region bits", SchemaUInt),
        SchemaField("classification", "electron classification", SchemaInt),
        SchemaField("scID", "supercluster ID number (unique per event)", SchemaInt),
        SchemaField("trkID", "track ID number (unique per event)", SchemaInt)
      )))),
      SchemaField("Muon", "", SchemaSequence(SchemaClass(name = "baconhep::TMuon", fields = List(
        SchemaField("pt", "kinematics", SchemaFloat),
        SchemaField("eta", "kinematics", SchemaFloat),
        SchemaField("phi", "kinematics", SchemaFloat),
        SchemaField("ptErr", "kinematics", SchemaFloat),
        SchemaField("staPt", "STA track kinematics", SchemaFloat),
        SchemaField("staEta", "STA track kinematics", SchemaFloat),
        SchemaField("staPhi", "STA track kinematics", SchemaFloat),
        SchemaField("pfPt", "matched PFCandidate", SchemaFloat),
        SchemaField("pfEta", "matched PFCandidate", SchemaFloat),
        SchemaField("pfPhi", "matched PFCandidate", SchemaFloat),
        SchemaField("trkIso", "detector isolation (R=0.3)", SchemaFloat),
        SchemaField("ecalIso", "detector isolation (R=0.3)", SchemaFloat),
        SchemaField("hcalIso", "detector isolation (R=0.3)", SchemaFloat),
        SchemaField("chHadIso", "PF isolation variables (R=0.4)", SchemaFloat),
        SchemaField("gammaIso", "PF isolation variables (R=0.4)", SchemaFloat),
        SchemaField("neuHadIso", "PF isolation variables (R=0.4)", SchemaFloat),
        SchemaField("puIso", "PF isolation variables (R=0.4)", SchemaFloat),
        SchemaField("puppiChHadIso", "Puppi Isolation R=0.4", SchemaFloat),
        SchemaField("puppiGammaIso", "Puppi Isolation R=0.4", SchemaFloat),
        SchemaField("puppiNeuHadIso", "Puppi Isolation R=0.4", SchemaFloat),
        SchemaField("puppiChHadIsoNoLep", "Puppi Isolation R=0.4 no lep", SchemaFloat),
        SchemaField("puppiGammaIsoNoLep", "Puppi Isolation R=0.4 no lep", SchemaFloat),
        SchemaField("puppiNeuHadIsoNoLep", "Puppi Isolation R=0.4 no lep", SchemaFloat),
        SchemaField("d0", "impact parameter", SchemaFloat),
        SchemaField("dz", "impact parameter", SchemaFloat),
        SchemaField("sip3d", "impact parameter", SchemaFloat),
        SchemaField("tkNchi2", "track fit normalized chi-square", SchemaFloat),
        SchemaField("muNchi2", "track fit normalized chi-square", SchemaFloat),
        SchemaField("trkKink", "track kink", SchemaFloat),
        SchemaField("glbKink", "track kink", SchemaFloat),
        SchemaField("trkHitFrac", "fraction of valid tracker hits", SchemaFloat),
        SchemaField("chi2LocPos", "TRK-STA position match", SchemaFloat),
        SchemaField("segComp", "compatibility of tracker track with muon segment", SchemaFloat),
        SchemaField("caloComp", "muon hypothesis compatibility with calo energy", SchemaFloat),
        SchemaField("q", "charge", SchemaInt),
        SchemaField("nValidHits", "number of valid muon hits in global fit", SchemaInt),
        SchemaField("typeBits", "muon type bits", SchemaUInt),
        SchemaField("selectorBits", "MuonSelector bits", SchemaUInt),
        SchemaField("pogIDBits", "POG muon IDs from CMSSW", SchemaUInt),
        SchemaField("nTkHits", "number of hits in tracker", SchemaUInt),
        SchemaField("nPixHits", "number of hits in tracker", SchemaUInt),
        SchemaField("nTkLayers", "number of hit layers in tracker", SchemaUInt),
        SchemaField("nPixLayers", "number of hit layers in tracker", SchemaUInt),
        SchemaField("nMatchStn", "number of stations with muon segments", SchemaUInt),
        SchemaField("trkID", "tracker track ID (unique per event)", SchemaInt)
      )))),
      SchemaField("Tau", "", SchemaSequence(SchemaClass(name = "baconhep::TTau", fields = List(
        SchemaField("pt", "kinematics", SchemaFloat),
        SchemaField("eta", "kinematics", SchemaFloat),
        SchemaField("phi", "kinematics", SchemaFloat),
        SchemaField("m", "kinematics", SchemaFloat),
        SchemaField("e", "kinematics", SchemaFloat),
        SchemaField("q", "charge", SchemaInt),
        SchemaField("dzLeadChHad", "longitudinal impact parameter of leading charged hadron", SchemaFloat),
        SchemaField("nSignalChHad", "number of charged hadrons in tau", SchemaUInt),
        SchemaField("nSignalGamma", "number of photons in tau", SchemaUInt),
        SchemaField("antiEleMVA5", "anti-electron MVA5 for Run 2", SchemaFloat),
        SchemaField("antiEleMVA5Cat", "anti-electron MVA5 category for Run 2", SchemaFloat),
        SchemaField("rawMuonRejection", "raw MVA output of BDT based anti-muon discriminator", SchemaFloat),
        SchemaField("rawIso3Hits", "raw Iso 3 hits", SchemaFloat),
        SchemaField("rawIsoMVA3oldDMwoLT", "raw Iso MVA3, trained on prong 1 and 3, without lifetime info", SchemaFloat),
        SchemaField("rawIsoMVA3oldDMwLT", "raw Iso MVA3, trained on prong 1 and 3, with lifetime info", SchemaFloat),
        SchemaField("rawIsoMVA3newDMwoLT", "raw Iso MVA3, trained on prong 1, 2, and 3, without lifetime info", SchemaFloat),
        SchemaField("rawIsoMVA3newDMwLT", "raw Iso MVA3, trained on prong 1, 2, and 3, with lifetime info", SchemaFloat),
        SchemaField("puppiChHadIso", "Puppi Isolation R=0.4", SchemaFloat),
        SchemaField("puppiGammaIso", "Puppi Isolation R=0.4", SchemaFloat),
        SchemaField("puppiNeuHadIso", "Puppi Isolation R=0.4", SchemaFloat),
        SchemaField("puppiChHadIsoNoLep", "Puppi Isolation R=0.4 no lep", SchemaFloat),
        SchemaField("puppiGammaIsoNoLep", "Puppi Isolation R=0.4 no lep", SchemaFloat),
        SchemaField("puppiNeuHadIsoNoLep", "Puppi Isolation R=0.4 no lep", SchemaFloat),
        SchemaField("hpsDisc", "HPS tau discriminators", SchemaULong)
      )))),
      SchemaField("Photon", "", SchemaSequence(SchemaClass(name = "baconhep::TPhoton", fields = List(
        SchemaField("pt", "kinematics", SchemaFloat),
        SchemaField("eta", "kinematics", SchemaFloat),
        SchemaField("phi", "kinematics", SchemaFloat),
        SchemaField("scEt", "SuperCluster kinematics", SchemaFloat),
        SchemaField("scEta", "SuperCluster kinematics", SchemaFloat),
        SchemaField("scPhi", "SuperCluster kinematics", SchemaFloat),
        SchemaField("trkIso", "detector isolation", SchemaFloat),
        SchemaField("ecalIso", "detector isolation", SchemaFloat),
        SchemaField("hcalIso", "detector isolation", SchemaFloat),
        SchemaField("chHadIso", "PF isolation variables", SchemaFloat),
        SchemaField("gammaIso", "PF isolation variables", SchemaFloat),
        SchemaField("neuHadIso", "PF isolation variables", SchemaFloat),
        SchemaField("mva", "Photon MVA ID", SchemaFloat),
        SchemaField("hovere", "H/E", SchemaFloat),
        SchemaField("sthovere", "Single tower H/E (https://twiki.cern.ch/twiki/bin/viewauth/CMS/HoverE2012)", SchemaFloat),
        SchemaField("sieie", "shower shape", SchemaFloat),
        SchemaField("sipip", "shower shape", SchemaFloat),
        SchemaField("r9", "shower shape", SchemaFloat),
        SchemaField("fiducialBits", "ECAL fiducial region", SchemaUInt),
        SchemaField("typeBits", "photon type", SchemaUInt),
        SchemaField("scID", "supercluster ID (unique per event)", SchemaInt),
        SchemaField("hasPixelSeed", "has pixel seed?", SchemaBool),
        SchemaField("passElectronVeto", "if false, its not a photon", SchemaBool),
        SchemaField("isConv", "", SchemaBool)
      )))),
      SchemaField("PV", "", SchemaSequence(SchemaClass(name = "baconhep::TVertex", fields = List(
        SchemaField("nTracksFit", "number of tracks in vertex fit", SchemaUInt),
        SchemaField("ndof", "vertex fit number of degrees of freedom", SchemaFloat),
        SchemaField("chi2", "vertex fit chi-square", SchemaFloat),
        SchemaField("x", "position", SchemaFloat),
        SchemaField("y", "position", SchemaFloat),
        SchemaField("z", "position", SchemaFloat)
      )))),
      SchemaField("AK4CHS", "", SchemaSequence(SchemaClass(name = "baconhep::TJet", fields = List(
        SchemaField("pt", "kinematics", SchemaFloat),
        SchemaField("eta", "kinematics", SchemaFloat),
        SchemaField("phi", "kinematics", SchemaFloat),
        SchemaField("mass", "kinematics", SchemaFloat),
        SchemaField("ptRaw", "kinematics", SchemaFloat),
        SchemaField("unc", "kinematics", SchemaFloat),
        SchemaField("area", "jet area (from FastJet)", SchemaFloat),
        SchemaField("d0", "impact parameter of leading charged constituent", SchemaFloat),
        SchemaField("dz", "impact parameter of leading charged constituent", SchemaFloat),
        SchemaField("csv", "CSV b-tagger for the jet and subjets", SchemaFloat),
        SchemaField("qgid", "q/g discriminator and input variables", SchemaFloat),
        SchemaField("axis2", "q/g discriminator and input variables", SchemaFloat),
        SchemaField("ptD", "q/g discriminator and input variables", SchemaFloat),
        SchemaField("mult", "", SchemaInt),
        SchemaField("q", "Charge for jet and subjets", SchemaFloat),
        SchemaField("mva", "PU discriminator MVA", SchemaFloat),
        SchemaField("beta", "input variables for PU and q/g discriminators", SchemaFloat),
        SchemaField("betaStar", "input variables for PU and q/g discriminators", SchemaFloat),
        SchemaField("dR2Mean", "input variables for PU and q/g discriminators", SchemaFloat),
        SchemaField("pullY", "Jet pull", SchemaFloat),
        SchemaField("pullPhi", "Jet pull", SchemaFloat),
        SchemaField("chPullY", "", SchemaFloat),
        SchemaField("chPullPhi", "", SchemaFloat),
        SchemaField("neuPullY", "", SchemaFloat),
        SchemaField("neuPullPhi", "", SchemaFloat),
        SchemaField("chEmFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("neuEmFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("chHadFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("neuHadFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("muonFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("genpt", "Matched GenJet", SchemaFloat),
        SchemaField("geneta", "Matched GenJet", SchemaFloat),
        SchemaField("genphi", "Matched GenJet", SchemaFloat),
        SchemaField("genm", "Matched GenJet", SchemaFloat),
        SchemaField("partonFlavor", "Flavor", SchemaInt),
        SchemaField("hadronFlavor", "Flavor", SchemaInt),
        SchemaField("nCharged", "constituent multiplicity", SchemaUInt),
        SchemaField("nNeutrals", "constituent multiplicity", SchemaUInt),
        SchemaField("nParticles", "constituent multiplicity", SchemaUInt)
      )))),
      SchemaField("AK8CHS", "", SchemaSequence(SchemaClass(name = "baconhep::TJet", fields = List(
        SchemaField("pt", "kinematics", SchemaFloat),
        SchemaField("eta", "kinematics", SchemaFloat),
        SchemaField("phi", "kinematics", SchemaFloat),
        SchemaField("mass", "kinematics", SchemaFloat),
        SchemaField("ptRaw", "kinematics", SchemaFloat),
        SchemaField("unc", "kinematics", SchemaFloat),
        SchemaField("area", "jet area (from FastJet)", SchemaFloat),
        SchemaField("d0", "impact parameter of leading charged constituent", SchemaFloat),
        SchemaField("dz", "impact parameter of leading charged constituent", SchemaFloat),
        SchemaField("csv", "CSV b-tagger for the jet and subjets", SchemaFloat),
        SchemaField("qgid", "q/g discriminator and input variables", SchemaFloat),
        SchemaField("axis2", "q/g discriminator and input variables", SchemaFloat),
        SchemaField("ptD", "q/g discriminator and input variables", SchemaFloat),
        SchemaField("mult", "", SchemaInt),
        SchemaField("q", "Charge for jet and subjets", SchemaFloat),
        SchemaField("mva", "PU discriminator MVA", SchemaFloat),
        SchemaField("beta", "input variables for PU and q/g discriminators", SchemaFloat),
        SchemaField("betaStar", "input variables for PU and q/g discriminators", SchemaFloat),
        SchemaField("dR2Mean", "input variables for PU and q/g discriminators", SchemaFloat),
        SchemaField("pullY", "Jet pull", SchemaFloat),
        SchemaField("pullPhi", "Jet pull", SchemaFloat),
        SchemaField("chPullY", "", SchemaFloat),
        SchemaField("chPullPhi", "", SchemaFloat),
        SchemaField("neuPullY", "", SchemaFloat),
        SchemaField("neuPullPhi", "", SchemaFloat),
        SchemaField("chEmFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("neuEmFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("chHadFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("neuHadFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("muonFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("genpt", "Matched GenJet", SchemaFloat),
        SchemaField("geneta", "Matched GenJet", SchemaFloat),
        SchemaField("genphi", "Matched GenJet", SchemaFloat),
        SchemaField("genm", "Matched GenJet", SchemaFloat),
        SchemaField("partonFlavor", "Flavor", SchemaInt),
        SchemaField("hadronFlavor", "Flavor", SchemaInt),
        SchemaField("nCharged", "constituent multiplicity", SchemaUInt),
        SchemaField("nNeutrals", "constituent multiplicity", SchemaUInt),
        SchemaField("nParticles", "constituent multiplicity", SchemaUInt)
      )))),
      SchemaField("AddAK8CHS", "", SchemaSequence(SchemaClass(name = "baconhep::TAddJet", fields = List(
        SchemaField("index", "index in original jet collection", SchemaUInt),
        SchemaField("mass_prun", "groomed jet masses", SchemaFloat),
        SchemaField("mass_trim", "groomed jet masses", SchemaFloat),
        SchemaField("mass_sd0", "groomed jet masses", SchemaFloat),
        SchemaField("pt_sd0", "soft drop", SchemaFloat),
        SchemaField("eta_sd0", "soft drop", SchemaFloat),
        SchemaField("phi_sd0", "soft drop", SchemaFloat),
        SchemaField("c2_0", "Correlation function with various exponents", SchemaFloat),
        SchemaField("c2_0P2", "Correlation function with various exponents", SchemaFloat),
        SchemaField("c2_0P5", "Correlation function with various exponents", SchemaFloat),
        SchemaField("c2_1P0", "Correlation function with various exponents", SchemaFloat),
        SchemaField("c2_2P0", "Correlation function with various exponents", SchemaFloat),
        SchemaField("qjet", "Q-jet volatility", SchemaFloat),
        SchemaField("tau1", "N-subjettiness", SchemaFloat),
        SchemaField("tau2", "N-subjettiness", SchemaFloat),
        SchemaField("tau3", "N-subjettiness", SchemaFloat),
        SchemaField("tau4", "N-subjettiness", SchemaFloat),
        SchemaField("doublecsv", "Double b-tag", SchemaFloat),
        SchemaField("sj1_pt", "", SchemaFloat),
        SchemaField("sj1_eta", "", SchemaFloat),
        SchemaField("sj1_phi", "", SchemaFloat),
        SchemaField("sj1_m", "", SchemaFloat),
        SchemaField("sj1_csv", "", SchemaFloat),
        SchemaField("sj1_qgid", "", SchemaFloat),
        SchemaField("sj1_q", "", SchemaFloat),
        SchemaField("sj2_pt", "", SchemaFloat),
        SchemaField("sj2_eta", "", SchemaFloat),
        SchemaField("sj2_phi", "", SchemaFloat),
        SchemaField("sj2_m", "", SchemaFloat),
        SchemaField("sj2_csv", "", SchemaFloat),
        SchemaField("sj2_qgid", "", SchemaFloat),
        SchemaField("sj2_q", "", SchemaFloat),
        SchemaField("sj3_pt", "", SchemaFloat),
        SchemaField("sj3_eta", "", SchemaFloat),
        SchemaField("sj3_phi", "", SchemaFloat),
        SchemaField("sj3_m", "", SchemaFloat),
        SchemaField("sj3_csv", "", SchemaFloat),
        SchemaField("sj3_qgid", "", SchemaFloat),
        SchemaField("sj3_q", "", SchemaFloat),
        SchemaField("sj4_pt", "", SchemaFloat),
        SchemaField("sj4_eta", "", SchemaFloat),
        SchemaField("sj4_phi", "", SchemaFloat),
        SchemaField("sj4_m", "", SchemaFloat),
        SchemaField("sj4_csv", "", SchemaFloat),
        SchemaField("sj4_qgid", "", SchemaFloat),
        SchemaField("sj4_q", "", SchemaFloat),
        SchemaField("pullAngle", "", SchemaFloat),
        SchemaField("nTracks", "Number of tracks associated to the jet", SchemaFloat),
        SchemaField("trackSip3dSig_3", "3D SIP (IP value/error) for the most displaced tracks associated to the jet", SchemaFloat),
        SchemaField("trackSip3dSig_2", "3D SIP (IP value/error) for the most displaced tracks associated to the jet", SchemaFloat),
        SchemaField("trackSip3dSig_1", "3D SIP (IP value/error) for the most displaced tracks associated to the jet", SchemaFloat),
        SchemaField("trackSip3dSig_0", "3D SIP (IP value/error) for the most displaced tracks associated to the jet", SchemaFloat),
        SchemaField("trackSip2dSigAboveCharm", "2D SIP of the first tracks that raises the Mass above the bottom (charm) threshold 5.2GeV (1.5GeV)", SchemaFloat),
        SchemaField("trackSip2dSigAboveBottom", "2D SIP of the first tracks that raises the Mass above the bottom (charm) threshold 5.2GeV (1.5GeV)", SchemaFloat),
        SchemaField("tau1_trackSip3dSig_0", "3D SIP of the two tracks with the highest SIP associated to the closest tau axis to the track", SchemaFloat),
        SchemaField("tau1_trackSip3dSig_1", "3D SIP of the two tracks with the highest SIP associated to the closest tau axis to the track", SchemaFloat),
        SchemaField("nSV", "", SchemaFloat),
        SchemaField("tau_SVmass_nSecondaryVertices", "Number of SVs (cross check), 2D flight distance significance", SchemaFloat),
        SchemaField("tau_SVmass_flightDistance2dSig", "Number of SVs (cross check), 2D flight distance significance", SchemaFloat),
        SchemaField("tau_SVmass_vertexDeltaR", "DeltaR between the secondary vertex flight direction and the jet axis", SchemaFloat),
        SchemaField("tau_SVmass_vertexNTracks", "DeltaR between the secondary vertex flight direction and the jet axis", SchemaFloat),
        SchemaField("tau_SVmass_trackEtaRel_2", "Pseudorapidity of the tracks at the vertex with respect to the tau axis closest to the leading SV in mass", SchemaFloat),
        SchemaField("tau_SVmass_trackEtaRel_1", "Pseudorapidity of the tracks at the vertex with respect to the tau axis closest to the leading SV in mass", SchemaFloat),
        SchemaField("tau_SVmass_trackEtaRel_0", "Pseudorapidity of the tracks at the vertex with respect to the tau axis closest to the leading SV in mass", SchemaFloat),
        SchemaField("tau_SVmass_vertexEnergyRatio", "Energy ratio and vertexMass", SchemaFloat),
        SchemaField("tau_SVmass_vertexMass", "Energy ratio and vertexMass", SchemaFloat),
        SchemaField("tau_SVmass_vertexMass_corrected", "Energy ratio and vertexMass", SchemaFloat),
        SchemaField("tau_SVmass_zratio", "z-ratio", SchemaFloat),
        SchemaField("tau_SVfd_nSecondaryVertices", "", SchemaFloat),
        SchemaField("tau_SVfd_flightDistance2dSig", "", SchemaFloat),
        SchemaField("tau_SVfd_vertexDeltaR", "", SchemaFloat),
        SchemaField("tau_SVfd_vertexNTracks", "", SchemaFloat),
        SchemaField("tau_SVfd_trackEtaRel_2", "", SchemaFloat),
        SchemaField("tau_SVfd_trackEtaRel_1", "", SchemaFloat),
        SchemaField("tau_SVfd_trackEtaRel_0", "", SchemaFloat),
        SchemaField("tau_SVfd_vertexEnergyRatio", "", SchemaFloat),
        SchemaField("tau_SVfd_vertexMass", "", SchemaFloat),
        SchemaField("tau_SVfd_vertexMass_corrected", "", SchemaFloat),
        SchemaField("tau_SVfd_zratio", "", SchemaFloat),
        SchemaField("topTagType", "", SchemaUInt),
        SchemaField("top_n_subjets", "", SchemaUInt),
        SchemaField("top_m_min", "", SchemaFloat),
        SchemaField("top_m_123", "", SchemaFloat),
        SchemaField("top_fRec", "", SchemaFloat),
        SchemaField("topchi2", "", SchemaFloat)
      )))),
      SchemaField("CA15CHS", "", SchemaSequence(SchemaClass(name = "baconhep::TJet", fields = List(
        SchemaField("pt", "kinematics", SchemaFloat),
        SchemaField("eta", "kinematics", SchemaFloat),
        SchemaField("phi", "kinematics", SchemaFloat),
        SchemaField("mass", "kinematics", SchemaFloat),
        SchemaField("ptRaw", "kinematics", SchemaFloat),
        SchemaField("unc", "kinematics", SchemaFloat),
        SchemaField("area", "jet area (from FastJet)", SchemaFloat),
        SchemaField("d0", "impact parameter of leading charged constituent", SchemaFloat),
        SchemaField("dz", "impact parameter of leading charged constituent", SchemaFloat),
        SchemaField("csv", "CSV b-tagger for the jet and subjets", SchemaFloat),
        SchemaField("qgid", "q/g discriminator and input variables", SchemaFloat),
        SchemaField("axis2", "q/g discriminator and input variables", SchemaFloat),
        SchemaField("ptD", "q/g discriminator and input variables", SchemaFloat),
        SchemaField("mult", "", SchemaInt),
        SchemaField("q", "Charge for jet and subjets", SchemaFloat),
        SchemaField("mva", "PU discriminator MVA", SchemaFloat),
        SchemaField("beta", "input variables for PU and q/g discriminators", SchemaFloat),
        SchemaField("betaStar", "input variables for PU and q/g discriminators", SchemaFloat),
        SchemaField("dR2Mean", "input variables for PU and q/g discriminators", SchemaFloat),
        SchemaField("pullY", "Jet pull", SchemaFloat),
        SchemaField("pullPhi", "Jet pull", SchemaFloat),
        SchemaField("chPullY", "", SchemaFloat),
        SchemaField("chPullPhi", "", SchemaFloat),
        SchemaField("neuPullY", "", SchemaFloat),
        SchemaField("neuPullPhi", "", SchemaFloat),
        SchemaField("chEmFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("neuEmFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("chHadFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("neuHadFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("muonFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("genpt", "Matched GenJet", SchemaFloat),
        SchemaField("geneta", "Matched GenJet", SchemaFloat),
        SchemaField("genphi", "Matched GenJet", SchemaFloat),
        SchemaField("genm", "Matched GenJet", SchemaFloat),
        SchemaField("partonFlavor", "Flavor", SchemaInt),
        SchemaField("hadronFlavor", "Flavor", SchemaInt),
        SchemaField("nCharged", "constituent multiplicity", SchemaUInt),
        SchemaField("nNeutrals", "constituent multiplicity", SchemaUInt),
        SchemaField("nParticles", "constituent multiplicity", SchemaUInt)
      )))),
      SchemaField("AddCA15CHS", "", SchemaSequence(SchemaClass(name = "baconhep::TAddJet", fields = List(
        SchemaField("index", "index in original jet collection", SchemaUInt),
        SchemaField("mass_prun", "groomed jet masses", SchemaFloat),
        SchemaField("mass_trim", "groomed jet masses", SchemaFloat),
        SchemaField("mass_sd0", "groomed jet masses", SchemaFloat),
        SchemaField("pt_sd0", "soft drop", SchemaFloat),
        SchemaField("eta_sd0", "soft drop", SchemaFloat),
        SchemaField("phi_sd0", "soft drop", SchemaFloat),
        SchemaField("c2_0", "Correlation function with various exponents", SchemaFloat),
        SchemaField("c2_0P2", "Correlation function with various exponents", SchemaFloat),
        SchemaField("c2_0P5", "Correlation function with various exponents", SchemaFloat),
        SchemaField("c2_1P0", "Correlation function with various exponents", SchemaFloat),
        SchemaField("c2_2P0", "Correlation function with various exponents", SchemaFloat),
        SchemaField("qjet", "Q-jet volatility", SchemaFloat),
        SchemaField("tau1", "N-subjettiness", SchemaFloat),
        SchemaField("tau2", "N-subjettiness", SchemaFloat),
        SchemaField("tau3", "N-subjettiness", SchemaFloat),
        SchemaField("tau4", "N-subjettiness", SchemaFloat),
        SchemaField("doublecsv", "Double b-tag", SchemaFloat),
        SchemaField("sj1_pt", "", SchemaFloat),
        SchemaField("sj1_eta", "", SchemaFloat),
        SchemaField("sj1_phi", "", SchemaFloat),
        SchemaField("sj1_m", "", SchemaFloat),
        SchemaField("sj1_csv", "", SchemaFloat),
        SchemaField("sj1_qgid", "", SchemaFloat),
        SchemaField("sj1_q", "", SchemaFloat),
        SchemaField("sj2_pt", "", SchemaFloat),
        SchemaField("sj2_eta", "", SchemaFloat),
        SchemaField("sj2_phi", "", SchemaFloat),
        SchemaField("sj2_m", "", SchemaFloat),
        SchemaField("sj2_csv", "", SchemaFloat),
        SchemaField("sj2_qgid", "", SchemaFloat),
        SchemaField("sj2_q", "", SchemaFloat),
        SchemaField("sj3_pt", "", SchemaFloat),
        SchemaField("sj3_eta", "", SchemaFloat),
        SchemaField("sj3_phi", "", SchemaFloat),
        SchemaField("sj3_m", "", SchemaFloat),
        SchemaField("sj3_csv", "", SchemaFloat),
        SchemaField("sj3_qgid", "", SchemaFloat),
        SchemaField("sj3_q", "", SchemaFloat),
        SchemaField("sj4_pt", "", SchemaFloat),
        SchemaField("sj4_eta", "", SchemaFloat),
        SchemaField("sj4_phi", "", SchemaFloat),
        SchemaField("sj4_m", "", SchemaFloat),
        SchemaField("sj4_csv", "", SchemaFloat),
        SchemaField("sj4_qgid", "", SchemaFloat),
        SchemaField("sj4_q", "", SchemaFloat),
        SchemaField("pullAngle", "", SchemaFloat),
        SchemaField("nTracks", "Number of tracks associated to the jet", SchemaFloat),
        SchemaField("trackSip3dSig_3", "3D SIP (IP value/error) for the most displaced tracks associated to the jet", SchemaFloat),
        SchemaField("trackSip3dSig_2", "3D SIP (IP value/error) for the most displaced tracks associated to the jet", SchemaFloat),
        SchemaField("trackSip3dSig_1", "3D SIP (IP value/error) for the most displaced tracks associated to the jet", SchemaFloat),
        SchemaField("trackSip3dSig_0", "3D SIP (IP value/error) for the most displaced tracks associated to the jet", SchemaFloat),
        SchemaField("trackSip2dSigAboveCharm", "2D SIP of the first tracks that raises the Mass above the bottom (charm) threshold 5.2GeV (1.5GeV)", SchemaFloat),
        SchemaField("trackSip2dSigAboveBottom", "2D SIP of the first tracks that raises the Mass above the bottom (charm) threshold 5.2GeV (1.5GeV)", SchemaFloat),
        SchemaField("tau1_trackSip3dSig_0", "3D SIP of the two tracks with the highest SIP associated to the closest tau axis to the track", SchemaFloat),
        SchemaField("tau1_trackSip3dSig_1", "3D SIP of the two tracks with the highest SIP associated to the closest tau axis to the track", SchemaFloat),
        SchemaField("nSV", "", SchemaFloat),
        SchemaField("tau_SVmass_nSecondaryVertices", "Number of SVs (cross check), 2D flight distance significance", SchemaFloat),
        SchemaField("tau_SVmass_flightDistance2dSig", "Number of SVs (cross check), 2D flight distance significance", SchemaFloat),
        SchemaField("tau_SVmass_vertexDeltaR", "DeltaR between the secondary vertex flight direction and the jet axis", SchemaFloat),
        SchemaField("tau_SVmass_vertexNTracks", "DeltaR between the secondary vertex flight direction and the jet axis", SchemaFloat),
        SchemaField("tau_SVmass_trackEtaRel_2", "Pseudorapidity of the tracks at the vertex with respect to the tau axis closest to the leading SV in mass", SchemaFloat),
        SchemaField("tau_SVmass_trackEtaRel_1", "Pseudorapidity of the tracks at the vertex with respect to the tau axis closest to the leading SV in mass", SchemaFloat),
        SchemaField("tau_SVmass_trackEtaRel_0", "Pseudorapidity of the tracks at the vertex with respect to the tau axis closest to the leading SV in mass", SchemaFloat),
        SchemaField("tau_SVmass_vertexEnergyRatio", "Energy ratio and vertexMass", SchemaFloat),
        SchemaField("tau_SVmass_vertexMass", "Energy ratio and vertexMass", SchemaFloat),
        SchemaField("tau_SVmass_vertexMass_corrected", "Energy ratio and vertexMass", SchemaFloat),
        SchemaField("tau_SVmass_zratio", "z-ratio", SchemaFloat),
        SchemaField("tau_SVfd_nSecondaryVertices", "", SchemaFloat),
        SchemaField("tau_SVfd_flightDistance2dSig", "", SchemaFloat),
        SchemaField("tau_SVfd_vertexDeltaR", "", SchemaFloat),
        SchemaField("tau_SVfd_vertexNTracks", "", SchemaFloat),
        SchemaField("tau_SVfd_trackEtaRel_2", "", SchemaFloat),
        SchemaField("tau_SVfd_trackEtaRel_1", "", SchemaFloat),
        SchemaField("tau_SVfd_trackEtaRel_0", "", SchemaFloat),
        SchemaField("tau_SVfd_vertexEnergyRatio", "", SchemaFloat),
        SchemaField("tau_SVfd_vertexMass", "", SchemaFloat),
        SchemaField("tau_SVfd_vertexMass_corrected", "", SchemaFloat),
        SchemaField("tau_SVfd_zratio", "", SchemaFloat),
        SchemaField("topTagType", "", SchemaUInt),
        SchemaField("top_n_subjets", "", SchemaUInt),
        SchemaField("top_m_min", "", SchemaFloat),
        SchemaField("top_m_123", "", SchemaFloat),
        SchemaField("top_fRec", "", SchemaFloat),
        SchemaField("topchi2", "", SchemaFloat)
      )))),
      SchemaField("AK4Puppi", "", SchemaSequence(SchemaClass(name = "baconhep::TJet", fields = List(
        SchemaField("pt", "kinematics", SchemaFloat),
        SchemaField("eta", "kinematics", SchemaFloat),
        SchemaField("phi", "kinematics", SchemaFloat),
        SchemaField("mass", "kinematics", SchemaFloat),
        SchemaField("ptRaw", "kinematics", SchemaFloat),
        SchemaField("unc", "kinematics", SchemaFloat),
        SchemaField("area", "jet area (from FastJet)", SchemaFloat),
        SchemaField("d0", "impact parameter of leading charged constituent", SchemaFloat),
        SchemaField("dz", "impact parameter of leading charged constituent", SchemaFloat),
        SchemaField("csv", "CSV b-tagger for the jet and subjets", SchemaFloat),
        SchemaField("qgid", "q/g discriminator and input variables", SchemaFloat),
        SchemaField("axis2", "q/g discriminator and input variables", SchemaFloat),
        SchemaField("ptD", "q/g discriminator and input variables", SchemaFloat),
        SchemaField("mult", "", SchemaInt),
        SchemaField("q", "Charge for jet and subjets", SchemaFloat),
        SchemaField("mva", "PU discriminator MVA", SchemaFloat),
        SchemaField("beta", "input variables for PU and q/g discriminators", SchemaFloat),
        SchemaField("betaStar", "input variables for PU and q/g discriminators", SchemaFloat),
        SchemaField("dR2Mean", "input variables for PU and q/g discriminators", SchemaFloat),
        SchemaField("pullY", "Jet pull", SchemaFloat),
        SchemaField("pullPhi", "Jet pull", SchemaFloat),
        SchemaField("chPullY", "", SchemaFloat),
        SchemaField("chPullPhi", "", SchemaFloat),
        SchemaField("neuPullY", "", SchemaFloat),
        SchemaField("neuPullPhi", "", SchemaFloat),
        SchemaField("chEmFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("neuEmFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("chHadFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("neuHadFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("muonFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("genpt", "Matched GenJet", SchemaFloat),
        SchemaField("geneta", "Matched GenJet", SchemaFloat),
        SchemaField("genphi", "Matched GenJet", SchemaFloat),
        SchemaField("genm", "Matched GenJet", SchemaFloat),
        SchemaField("partonFlavor", "Flavor", SchemaInt),
        SchemaField("hadronFlavor", "Flavor", SchemaInt),
        SchemaField("nCharged", "constituent multiplicity", SchemaUInt),
        SchemaField("nNeutrals", "constituent multiplicity", SchemaUInt),
        SchemaField("nParticles", "constituent multiplicity", SchemaUInt)
      )))),
      SchemaField("CA8Puppi", "", SchemaSequence(SchemaClass(name = "baconhep::TJet", fields = List(
        SchemaField("pt", "kinematics", SchemaFloat),
        SchemaField("eta", "kinematics", SchemaFloat),
        SchemaField("phi", "kinematics", SchemaFloat),
        SchemaField("mass", "kinematics", SchemaFloat),
        SchemaField("ptRaw", "kinematics", SchemaFloat),
        SchemaField("unc", "kinematics", SchemaFloat),
        SchemaField("area", "jet area (from FastJet)", SchemaFloat),
        SchemaField("d0", "impact parameter of leading charged constituent", SchemaFloat),
        SchemaField("dz", "impact parameter of leading charged constituent", SchemaFloat),
        SchemaField("csv", "CSV b-tagger for the jet and subjets", SchemaFloat),
        SchemaField("qgid", "q/g discriminator and input variables", SchemaFloat),
        SchemaField("axis2", "q/g discriminator and input variables", SchemaFloat),
        SchemaField("ptD", "q/g discriminator and input variables", SchemaFloat),
        SchemaField("mult", "", SchemaInt),
        SchemaField("q", "Charge for jet and subjets", SchemaFloat),
        SchemaField("mva", "PU discriminator MVA", SchemaFloat),
        SchemaField("beta", "input variables for PU and q/g discriminators", SchemaFloat),
        SchemaField("betaStar", "input variables for PU and q/g discriminators", SchemaFloat),
        SchemaField("dR2Mean", "input variables for PU and q/g discriminators", SchemaFloat),
        SchemaField("pullY", "Jet pull", SchemaFloat),
        SchemaField("pullPhi", "Jet pull", SchemaFloat),
        SchemaField("chPullY", "", SchemaFloat),
        SchemaField("chPullPhi", "", SchemaFloat),
        SchemaField("neuPullY", "", SchemaFloat),
        SchemaField("neuPullPhi", "", SchemaFloat),
        SchemaField("chEmFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("neuEmFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("chHadFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("neuHadFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("muonFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("genpt", "Matched GenJet", SchemaFloat),
        SchemaField("geneta", "Matched GenJet", SchemaFloat),
        SchemaField("genphi", "Matched GenJet", SchemaFloat),
        SchemaField("genm", "Matched GenJet", SchemaFloat),
        SchemaField("partonFlavor", "Flavor", SchemaInt),
        SchemaField("hadronFlavor", "Flavor", SchemaInt),
        SchemaField("nCharged", "constituent multiplicity", SchemaUInt),
        SchemaField("nNeutrals", "constituent multiplicity", SchemaUInt),
        SchemaField("nParticles", "constituent multiplicity", SchemaUInt)
      )))),
      SchemaField("AddCA8Puppi", "", SchemaSequence(SchemaClass(name = "baconhep::TAddJet", fields = List(
        SchemaField("index", "index in original jet collection", SchemaUInt),
        SchemaField("mass_prun", "groomed jet masses", SchemaFloat),
        SchemaField("mass_trim", "groomed jet masses", SchemaFloat),
        SchemaField("mass_sd0", "groomed jet masses", SchemaFloat),
        SchemaField("pt_sd0", "soft drop", SchemaFloat),
        SchemaField("eta_sd0", "soft drop", SchemaFloat),
        SchemaField("phi_sd0", "soft drop", SchemaFloat),
        SchemaField("c2_0", "Correlation function with various exponents", SchemaFloat),
        SchemaField("c2_0P2", "Correlation function with various exponents", SchemaFloat),
        SchemaField("c2_0P5", "Correlation function with various exponents", SchemaFloat),
        SchemaField("c2_1P0", "Correlation function with various exponents", SchemaFloat),
        SchemaField("c2_2P0", "Correlation function with various exponents", SchemaFloat),
        SchemaField("qjet", "Q-jet volatility", SchemaFloat),
        SchemaField("tau1", "N-subjettiness", SchemaFloat),
        SchemaField("tau2", "N-subjettiness", SchemaFloat),
        SchemaField("tau3", "N-subjettiness", SchemaFloat),
        SchemaField("tau4", "N-subjettiness", SchemaFloat),
        SchemaField("doublecsv", "Double b-tag", SchemaFloat),
        SchemaField("sj1_pt", "", SchemaFloat),
        SchemaField("sj1_eta", "", SchemaFloat),
        SchemaField("sj1_phi", "", SchemaFloat),
        SchemaField("sj1_m", "", SchemaFloat),
        SchemaField("sj1_csv", "", SchemaFloat),
        SchemaField("sj1_qgid", "", SchemaFloat),
        SchemaField("sj1_q", "", SchemaFloat),
        SchemaField("sj2_pt", "", SchemaFloat),
        SchemaField("sj2_eta", "", SchemaFloat),
        SchemaField("sj2_phi", "", SchemaFloat),
        SchemaField("sj2_m", "", SchemaFloat),
        SchemaField("sj2_csv", "", SchemaFloat),
        SchemaField("sj2_qgid", "", SchemaFloat),
        SchemaField("sj2_q", "", SchemaFloat),
        SchemaField("sj3_pt", "", SchemaFloat),
        SchemaField("sj3_eta", "", SchemaFloat),
        SchemaField("sj3_phi", "", SchemaFloat),
        SchemaField("sj3_m", "", SchemaFloat),
        SchemaField("sj3_csv", "", SchemaFloat),
        SchemaField("sj3_qgid", "", SchemaFloat),
        SchemaField("sj3_q", "", SchemaFloat),
        SchemaField("sj4_pt", "", SchemaFloat),
        SchemaField("sj4_eta", "", SchemaFloat),
        SchemaField("sj4_phi", "", SchemaFloat),
        SchemaField("sj4_m", "", SchemaFloat),
        SchemaField("sj4_csv", "", SchemaFloat),
        SchemaField("sj4_qgid", "", SchemaFloat),
        SchemaField("sj4_q", "", SchemaFloat),
        SchemaField("pullAngle", "", SchemaFloat),
        SchemaField("nTracks", "Number of tracks associated to the jet", SchemaFloat),
        SchemaField("trackSip3dSig_3", "3D SIP (IP value/error) for the most displaced tracks associated to the jet", SchemaFloat),
        SchemaField("trackSip3dSig_2", "3D SIP (IP value/error) for the most displaced tracks associated to the jet", SchemaFloat),
        SchemaField("trackSip3dSig_1", "3D SIP (IP value/error) for the most displaced tracks associated to the jet", SchemaFloat),
        SchemaField("trackSip3dSig_0", "3D SIP (IP value/error) for the most displaced tracks associated to the jet", SchemaFloat),
        SchemaField("trackSip2dSigAboveCharm", "2D SIP of the first tracks that raises the Mass above the bottom (charm) threshold 5.2GeV (1.5GeV)", SchemaFloat),
        SchemaField("trackSip2dSigAboveBottom", "2D SIP of the first tracks that raises the Mass above the bottom (charm) threshold 5.2GeV (1.5GeV)", SchemaFloat),
        SchemaField("tau1_trackSip3dSig_0", "3D SIP of the two tracks with the highest SIP associated to the closest tau axis to the track", SchemaFloat),
        SchemaField("tau1_trackSip3dSig_1", "3D SIP of the two tracks with the highest SIP associated to the closest tau axis to the track", SchemaFloat),
        SchemaField("nSV", "", SchemaFloat),
        SchemaField("tau_SVmass_nSecondaryVertices", "Number of SVs (cross check), 2D flight distance significance", SchemaFloat),
        SchemaField("tau_SVmass_flightDistance2dSig", "Number of SVs (cross check), 2D flight distance significance", SchemaFloat),
        SchemaField("tau_SVmass_vertexDeltaR", "DeltaR between the secondary vertex flight direction and the jet axis", SchemaFloat),
        SchemaField("tau_SVmass_vertexNTracks", "DeltaR between the secondary vertex flight direction and the jet axis", SchemaFloat),
        SchemaField("tau_SVmass_trackEtaRel_2", "Pseudorapidity of the tracks at the vertex with respect to the tau axis closest to the leading SV in mass", SchemaFloat),
        SchemaField("tau_SVmass_trackEtaRel_1", "Pseudorapidity of the tracks at the vertex with respect to the tau axis closest to the leading SV in mass", SchemaFloat),
        SchemaField("tau_SVmass_trackEtaRel_0", "Pseudorapidity of the tracks at the vertex with respect to the tau axis closest to the leading SV in mass", SchemaFloat),
        SchemaField("tau_SVmass_vertexEnergyRatio", "Energy ratio and vertexMass", SchemaFloat),
        SchemaField("tau_SVmass_vertexMass", "Energy ratio and vertexMass", SchemaFloat),
        SchemaField("tau_SVmass_vertexMass_corrected", "Energy ratio and vertexMass", SchemaFloat),
        SchemaField("tau_SVmass_zratio", "z-ratio", SchemaFloat),
        SchemaField("tau_SVfd_nSecondaryVertices", "", SchemaFloat),
        SchemaField("tau_SVfd_flightDistance2dSig", "", SchemaFloat),
        SchemaField("tau_SVfd_vertexDeltaR", "", SchemaFloat),
        SchemaField("tau_SVfd_vertexNTracks", "", SchemaFloat),
        SchemaField("tau_SVfd_trackEtaRel_2", "", SchemaFloat),
        SchemaField("tau_SVfd_trackEtaRel_1", "", SchemaFloat),
        SchemaField("tau_SVfd_trackEtaRel_0", "", SchemaFloat),
        SchemaField("tau_SVfd_vertexEnergyRatio", "", SchemaFloat),
        SchemaField("tau_SVfd_vertexMass", "", SchemaFloat),
        SchemaField("tau_SVfd_vertexMass_corrected", "", SchemaFloat),
        SchemaField("tau_SVfd_zratio", "", SchemaFloat),
        SchemaField("topTagType", "", SchemaUInt),
        SchemaField("top_n_subjets", "", SchemaUInt),
        SchemaField("top_m_min", "", SchemaFloat),
        SchemaField("top_m_123", "", SchemaFloat),
        SchemaField("top_fRec", "", SchemaFloat),
        SchemaField("topchi2", "", SchemaFloat)
      )))),
      SchemaField("CA15Puppi", "", SchemaSequence(SchemaClass(name = "baconhep::TJet", fields = List(
        SchemaField("pt", "kinematics", SchemaFloat),
        SchemaField("eta", "kinematics", SchemaFloat),
        SchemaField("phi", "kinematics", SchemaFloat),
        SchemaField("mass", "kinematics", SchemaFloat),
        SchemaField("ptRaw", "kinematics", SchemaFloat),
        SchemaField("unc", "kinematics", SchemaFloat),
        SchemaField("area", "jet area (from FastJet)", SchemaFloat),
        SchemaField("d0", "impact parameter of leading charged constituent", SchemaFloat),
        SchemaField("dz", "impact parameter of leading charged constituent", SchemaFloat),
        SchemaField("csv", "CSV b-tagger for the jet and subjets", SchemaFloat),
        SchemaField("qgid", "q/g discriminator and input variables", SchemaFloat),
        SchemaField("axis2", "q/g discriminator and input variables", SchemaFloat),
        SchemaField("ptD", "q/g discriminator and input variables", SchemaFloat),
        SchemaField("mult", "", SchemaInt),
        SchemaField("q", "Charge for jet and subjets", SchemaFloat),
        SchemaField("mva", "PU discriminator MVA", SchemaFloat),
        SchemaField("beta", "input variables for PU and q/g discriminators", SchemaFloat),
        SchemaField("betaStar", "input variables for PU and q/g discriminators", SchemaFloat),
        SchemaField("dR2Mean", "input variables for PU and q/g discriminators", SchemaFloat),
        SchemaField("pullY", "Jet pull", SchemaFloat),
        SchemaField("pullPhi", "Jet pull", SchemaFloat),
        SchemaField("chPullY", "", SchemaFloat),
        SchemaField("chPullPhi", "", SchemaFloat),
        SchemaField("neuPullY", "", SchemaFloat),
        SchemaField("neuPullPhi", "", SchemaFloat),
        SchemaField("chEmFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("neuEmFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("chHadFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("neuHadFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("muonFrac", "fractional energy contribution by type", SchemaFloat),
        SchemaField("genpt", "Matched GenJet", SchemaFloat),
        SchemaField("geneta", "Matched GenJet", SchemaFloat),
        SchemaField("genphi", "Matched GenJet", SchemaFloat),
        SchemaField("genm", "Matched GenJet", SchemaFloat),
        SchemaField("partonFlavor", "Flavor", SchemaInt),
        SchemaField("hadronFlavor", "Flavor", SchemaInt),
        SchemaField("nCharged", "constituent multiplicity", SchemaUInt),
        SchemaField("nNeutrals", "constituent multiplicity", SchemaUInt),
        SchemaField("nParticles", "constituent multiplicity", SchemaUInt)
      )))),
      SchemaField("AddCA15Puppi", "", SchemaSequence(SchemaClass(name = "baconhep::TAddJet", fields = List(
        SchemaField("index", "index in original jet collection", SchemaUInt),
        SchemaField("mass_prun", "groomed jet masses", SchemaFloat),
        SchemaField("mass_trim", "groomed jet masses", SchemaFloat),
        SchemaField("mass_sd0", "groomed jet masses", SchemaFloat),
        SchemaField("pt_sd0", "soft drop", SchemaFloat),
        SchemaField("eta_sd0", "soft drop", SchemaFloat),
        SchemaField("phi_sd0", "soft drop", SchemaFloat),
        SchemaField("c2_0", "Correlation function with various exponents", SchemaFloat),
        SchemaField("c2_0P2", "Correlation function with various exponents", SchemaFloat),
        SchemaField("c2_0P5", "Correlation function with various exponents", SchemaFloat),
        SchemaField("c2_1P0", "Correlation function with various exponents", SchemaFloat),
        SchemaField("c2_2P0", "Correlation function with various exponents", SchemaFloat),
        SchemaField("qjet", "Q-jet volatility", SchemaFloat),
        SchemaField("tau1", "N-subjettiness", SchemaFloat),
        SchemaField("tau2", "N-subjettiness", SchemaFloat),
        SchemaField("tau3", "N-subjettiness", SchemaFloat),
        SchemaField("tau4", "N-subjettiness", SchemaFloat),
        SchemaField("doublecsv", "Double b-tag", SchemaFloat),
        SchemaField("sj1_pt", "", SchemaFloat),
        SchemaField("sj1_eta", "", SchemaFloat),
        SchemaField("sj1_phi", "", SchemaFloat),
        SchemaField("sj1_m", "", SchemaFloat),
        SchemaField("sj1_csv", "", SchemaFloat),
        SchemaField("sj1_qgid", "", SchemaFloat),
        SchemaField("sj1_q", "", SchemaFloat),
        SchemaField("sj2_pt", "", SchemaFloat),
        SchemaField("sj2_eta", "", SchemaFloat),
        SchemaField("sj2_phi", "", SchemaFloat),
        SchemaField("sj2_m", "", SchemaFloat),
        SchemaField("sj2_csv", "", SchemaFloat),
        SchemaField("sj2_qgid", "", SchemaFloat),
        SchemaField("sj2_q", "", SchemaFloat),
        SchemaField("sj3_pt", "", SchemaFloat),
        SchemaField("sj3_eta", "", SchemaFloat),
        SchemaField("sj3_phi", "", SchemaFloat),
        SchemaField("sj3_m", "", SchemaFloat),
        SchemaField("sj3_csv", "", SchemaFloat),
        SchemaField("sj3_qgid", "", SchemaFloat),
        SchemaField("sj3_q", "", SchemaFloat),
        SchemaField("sj4_pt", "", SchemaFloat),
        SchemaField("sj4_eta", "", SchemaFloat),
        SchemaField("sj4_phi", "", SchemaFloat),
        SchemaField("sj4_m", "", SchemaFloat),
        SchemaField("sj4_csv", "", SchemaFloat),
        SchemaField("sj4_qgid", "", SchemaFloat),
        SchemaField("sj4_q", "", SchemaFloat),
        SchemaField("pullAngle", "", SchemaFloat),
        SchemaField("nTracks", "Number of tracks associated to the jet", SchemaFloat),
        SchemaField("trackSip3dSig_3", "3D SIP (IP value/error) for the most displaced tracks associated to the jet", SchemaFloat),
        SchemaField("trackSip3dSig_2", "3D SIP (IP value/error) for the most displaced tracks associated to the jet", SchemaFloat),
        SchemaField("trackSip3dSig_1", "3D SIP (IP value/error) for the most displaced tracks associated to the jet", SchemaFloat),
        SchemaField("trackSip3dSig_0", "3D SIP (IP value/error) for the most displaced tracks associated to the jet", SchemaFloat),
        SchemaField("trackSip2dSigAboveCharm", "2D SIP of the first tracks that raises the Mass above the bottom (charm) threshold 5.2GeV (1.5GeV)", SchemaFloat),
        SchemaField("trackSip2dSigAboveBottom", "2D SIP of the first tracks that raises the Mass above the bottom (charm) threshold 5.2GeV (1.5GeV)", SchemaFloat),
        SchemaField("tau1_trackSip3dSig_0", "3D SIP of the two tracks with the highest SIP associated to the closest tau axis to the track", SchemaFloat),
        SchemaField("tau1_trackSip3dSig_1", "3D SIP of the two tracks with the highest SIP associated to the closest tau axis to the track", SchemaFloat),
        SchemaField("nSV", "", SchemaFloat),
        SchemaField("tau_SVmass_nSecondaryVertices", "Number of SVs (cross check), 2D flight distance significance", SchemaFloat),
        SchemaField("tau_SVmass_flightDistance2dSig", "Number of SVs (cross check), 2D flight distance significance", SchemaFloat),
        SchemaField("tau_SVmass_vertexDeltaR", "DeltaR between the secondary vertex flight direction and the jet axis", SchemaFloat),
        SchemaField("tau_SVmass_vertexNTracks", "DeltaR between the secondary vertex flight direction and the jet axis", SchemaFloat),
        SchemaField("tau_SVmass_trackEtaRel_2", "Pseudorapidity of the tracks at the vertex with respect to the tau axis closest to the leading SV in mass", SchemaFloat),
        SchemaField("tau_SVmass_trackEtaRel_1", "Pseudorapidity of the tracks at the vertex with respect to the tau axis closest to the leading SV in mass", SchemaFloat),
        SchemaField("tau_SVmass_trackEtaRel_0", "Pseudorapidity of the tracks at the vertex with respect to the tau axis closest to the leading SV in mass", SchemaFloat),
        SchemaField("tau_SVmass_vertexEnergyRatio", "Energy ratio and vertexMass", SchemaFloat),
        SchemaField("tau_SVmass_vertexMass", "Energy ratio and vertexMass", SchemaFloat),
        SchemaField("tau_SVmass_vertexMass_corrected", "Energy ratio and vertexMass", SchemaFloat),
        SchemaField("tau_SVmass_zratio", "z-ratio", SchemaFloat),
        SchemaField("tau_SVfd_nSecondaryVertices", "", SchemaFloat),
        SchemaField("tau_SVfd_flightDistance2dSig", "", SchemaFloat),
        SchemaField("tau_SVfd_vertexDeltaR", "", SchemaFloat),
        SchemaField("tau_SVfd_vertexNTracks", "", SchemaFloat),
        SchemaField("tau_SVfd_trackEtaRel_2", "", SchemaFloat),
        SchemaField("tau_SVfd_trackEtaRel_1", "", SchemaFloat),
        SchemaField("tau_SVfd_trackEtaRel_0", "", SchemaFloat),
        SchemaField("tau_SVfd_vertexEnergyRatio", "", SchemaFloat),
        SchemaField("tau_SVfd_vertexMass", "", SchemaFloat),
        SchemaField("tau_SVfd_vertexMass_corrected", "", SchemaFloat),
        SchemaField("tau_SVfd_zratio", "", SchemaFloat),
        SchemaField("topTagType", "", SchemaUInt),
        SchemaField("top_n_subjets", "", SchemaUInt),
        SchemaField("top_m_min", "", SchemaFloat),
        SchemaField("top_m_123", "", SchemaFloat),
        SchemaField("top_fRec", "", SchemaFloat),
        SchemaField("topchi2", "", SchemaFloat)
      ))))
    ))

    val myclasses = Map("Events" -> My[Tree2], "baconhep::TEventInfo" -> My[baconhep.TEventInfo], "baconhep::TGenEventInfo" -> My[baconhep.TGenEventInfo], "baconhep::TGenParticle" -> My[baconhep.TGenParticle], "baconhep::TLHEWeight" -> My[baconhep.TLHEWeight], "baconhep::TElectron" -> My[baconhep.TElectron], "baconhep::TMuon" -> My[baconhep.TMuon], "baconhep::TTau" -> My[baconhep.TTau], "baconhep::TPhoton" -> My[baconhep.TPhoton], "baconhep::TVertex" -> My[baconhep.TVertex], "baconhep::TJet" -> My[baconhep.TJet], "baconhep::TAddJet" -> My[baconhep.TAddJet])

    val iterator = RootTreeIterator[Tree2](List("../root2avro/test_Bacon/Output.root"), "Events", inferTypes = true, myclasses = myclasses)

    println(iterator.schema)
    println(iterator.factory)
    println(iterator.repr)

    var i = 0
    while (iterator.hasNext  &&  i < 10) {
      val x = iterator.next()
      println(x.PV)
      println(x.Photon)
      i += 1
    }
  }
}
