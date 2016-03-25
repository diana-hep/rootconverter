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

      println(fileLocation, treeLocation, libs, name, ns)


    }
  }
}
