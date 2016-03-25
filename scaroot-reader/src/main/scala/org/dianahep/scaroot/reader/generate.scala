package org.dianahep.scaroot.reader

import scala.collection.mutable
import scala.reflect.runtime.universe.Constant
import scala.reflect.runtime.universe.Literal

import org.dianahep.scaroot.reader._
import org.dianahep.scaroot.reader.schema._

package generate {
  object Main {
    val help = """Usage: java -jar scaroot-reader.jar fileLocation treeLocation
Where fileLocation is either a file path or URL TFile::Open can handle and treeLocation is the path of the TTree.
Options:
  --libs=LIB1,LIB2,...   Comma-separated list of .so files defining objects in the TTree (i.e. X_cxx.so with associated X_cxx_ACLiC_dict_rdict.pcm).
  --name=NAME            Name for TTree class (taken from TTree name if not provided).
  --ns=NAMESPACE         Package namespace for class ("data.root" if not provided).
"""

    val libsPrefix = "--libs=(.*)".r
    val namePrefix = "--name=(.*)".r
    val nsPrefix = "--ns=(.*)".r

    def main(args: Array[String]) {
      var fileLocation: String = null
      var treeLocation: String = null
      var libs: List[String] = Nil
      var name: String = null
      var ns: String = "data.root"

      // Handle arguments in the same way as root2avro.
      args.foreach(_.trim match {
        case libsPrefix(x) => libs = x.split(',').toList
        case namePrefix(x) => name = x
        case nsPrefix(x) => ns = x
        case x if (fileLocation == null) => fileLocation = x
        case x if (treeLocation == null) => treeLocation = x
        case _ =>
          System.err.println("Too many arguments.\n\n" + help)
          System.exit(-1)
      })

      if (fileLocation == null  ||  treeLocation == null) {
          System.err.println("Too few arguments.\n\n" + help)
          System.exit(-1)
      }

      // Load dynamic libraries, if necessary.
      libs foreach {lib => LoadLibsOnce(lib)}

      // Build a TreeWalker.
      val treeWalker = RootReaderCPPLibrary.newTreeWalker(fileLocation, treeLocation, "")
      if (RootReaderCPPLibrary.valid(treeWalker) == 0)
        throw new RuntimeException(RootReaderCPPLibrary.errorMessage(treeWalker))

      // Walk through the elements until the schema is well defined.
      var done = (RootReaderCPPLibrary.next(treeWalker) == 0)
      while (!done  &&  RootReaderCPPLibrary.resolved(treeWalker) == 0) {
        RootReaderCPPLibrary.resolve(treeWalker)
        done = (RootReaderCPPLibrary.next(treeWalker) == 0)
      }

      // Get a schema.
      val schema: SchemaClass = Schema(treeWalker)
      if (name == null) name = schema.name

      println(generateScala(name, ns, schema))
    }

    def generateScala(name: String, ns: String, schema: Schema) = s"""package $ns;

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

import scala.collection.mutable
import scala.collection.JavaConversions._

${generateScalaClass(name, schema)}
"""

    def generateScalaClass(name: String, schema: Schema) = s"""// HERE"""
  }
}
