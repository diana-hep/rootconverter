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
  --libs=LIB1,LIB2,...      Comma-separated list of C++ source or .so files defining objects in the TTree
                            (i.e. SOMETHING.cxx to recompile the objects on the local architecture or
                            SOMETHING_cxx.so and SOMETHING_cxx_ACLiC_dict_rdict.pcm to use precompiled binaries).
  --includes=DIR1,DIR2...   Add include directories to the path for use in compiling C++ libs (above).
  --inferTypes              As an alternative to providing --libs, attempt to infer the class structure from the
                            ROOT file itself by inspecting its embedded streamers.
  --name=NAME               Name for TTree class (taken from TTree name if not provided).
  --ns=NAMESPACE            Package namespace for class ("data.root" if not provided).
  --hadoop                  If supplied, make the objects Hadoop writables.
"""

    val libsPrefix = "--libs=(.*)".r
    val includesPrefix = "--includes=(.*)".r
    val inferTypesPrefix = "--inferTypes"
    val namePrefix = "--name=(.*)".r
    val nsPrefix = "--ns=(.*)".r
    val hadoopPrefix = "--hadoop".r

    def main(args: Array[String]) {
      var fileLocation: String = null
      var treeLocation: String = null
      var libs: List[String] = Nil
      var includes: List[String] = Nil
      var inferTypes: Boolean = false
      var name: String = null
      var ns: String = "data.root"
      var hadoop: Boolean = false

      // Handle arguments in the same way as root2avro.
      args.foreach(_.trim match {
        case libsPrefix(x) => libs = x.split(',').toList
        case includesPrefix(x) => includes = x.split(',').toList
        case `inferTypesPrefix` => inferTypes = true
        case namePrefix(x) => name = x
        case nsPrefix(x) => ns = x
        case hadoopPrefix() => hadoop = true
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
      includes foreach {dir => LoadLibsOnce.include(dir)}
      libs foreach {lib => LoadLibsOnce(lib)}

      if (inferTypes) {
        val errorMessage: String = RootReaderCPPLibrary.inferTypes(fileLocation, treeLocation)
        if (!errorMessage.isEmpty)
          throw new RuntimeException(errorMessage)
      }

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
      val schemaClass: SchemaClass = Schema(treeWalker)

      val (ns1, ns2) = {val out = ns.split('.'); (out.init.mkString("."), out.last)}

      val originalName = schemaClass.name
      val renamedSchemaClass = schemaClass.copy(name = if (name == null) schemaClass.name else name)

      println(generateScala(ns1, ns2, originalName, renamedSchemaClass, treeLocation, hadoop))
    }

    def generateScala(ns1: String, ns2: String, originalName: String, schema: Schema, treeLocation: String, hadoop: Boolean) = s"""// These classes represent your data. ScaROOT-Reader will convert ROOT TTree data into instances of these classes for
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

${if (ns1.isEmpty) "" else "package " + ns1 + "\n\n"}${if (hadoop) "import scala.language.implicitConversions\nimport org.apache.hadoop.io.Writable\n\n" else ""}import org.dianahep.scaroot.reader._

package $ns2 {
${generateScalaClasses(ns2, originalName, schema, treeLocation, hadoop)}
}"""

    def generateScalaClasses(ns2: String, originalName: String, schema: Schema, treeLocation: String, hadoop: Boolean) = {
      def collectClasses(s: Schema, memo: mutable.Set[String]): List[SchemaClass] = s match {
        case SchemaClass(name, _) if (memo contains name) => Nil
        case schemaClass @ SchemaClass(name, fields) => {
          memo += name
          schemaClass :: fields.flatMap(f => collectClasses(f.schema, memo))
        }
        case SchemaPointer(referent) => collectClasses(referent, memo)
        case SchemaSequence(content) => collectClasses(content, memo)
        case _ => Nil
      }
      val schemaClasses = collectClasses(schema, mutable.Set[String]())

      def generateComment(comment: String) =
        if (comment.isEmpty)
          ""
        else
          "   // " + comment

      def generateScalaType(s: Schema): String = s match {
        case SchemaBool   => "Boolean"
        case SchemaChar   => "Byte"
        case SchemaUChar  => "Short"
        case SchemaShort  => "Short"
        case SchemaUShort => "Int"
        case SchemaInt    => "Int"
        case SchemaUInt   => "Long"
        case SchemaLong   => "Long"
        case SchemaULong  => "Double"
        case SchemaFloat  => "Float"
        case SchemaDouble => "Double"
        case SchemaString => "String"
        case SchemaClass(name, _) => name
        case SchemaPointer(referent) => s"Option[${generateScalaType(referent)}]"
        case SchemaSequence(content) => s"Seq[${generateScalaType(content)}]"
      }

      def generateScalaClass(schemaClass: SchemaClass) = {
        val header = s"  case class ${schemaClass.name}("
        val delimiter = "\n" + " " * header.size
        var lastHadComment = false
        val fields = schemaClass.fields.zipWithIndex collect {
          case (SchemaField(name, comment, t), i) =>
            val closer = if (i == schemaClass.fields.size - 1) ")" else ","
            lastHadComment = !comment.isEmpty
            name + ": " + generateScalaType(t) + closer + generateComment(comment)
        }

        header + fields.mkString(delimiter) + (if (lastHadComment) "\n" + " " * header.size else " ") + "extends Serializable"
      }

      val schemaClassesAndNames = {
        val head :: tail = schemaClasses
        (originalName, head) :: tail.map(x => (x.name, x))
      }

      val myclasses =
        "  // Pass 'myclasses' into RootTreeIterator to tell it to fill these classes, rather than 'Generic'.\n  val myclasses = Map(" + schemaClassesAndNames.map({case (n, c) => s"${Literal(Constant(n))} -> My[${c.name}]"}).mkString(", ") + ")"

      val tl =
        s"  // Unless you move it, this is the location of the tree, provided for convenience and reusable code.\n  val treeLocation = ${Literal(Constant(treeLocation))}"

      def generateReadExpression(s: Schema, dummy: Int): String = s match {
        case SchemaBool   => "in.readBoolean()"
        case SchemaChar   => "in.readByte()"
        case SchemaUChar  => "in.readShort()"
        case SchemaShort  => "in.readShort()"
        case SchemaUShort => "in.readInt()"
        case SchemaInt    => "in.readInt()"
        case SchemaUInt   => "in.readLong()"
        case SchemaLong   => "in.readLong()"
        case SchemaULong  => "in.readDouble()"
        case SchemaFloat  => "in.readFloat()"
        case SchemaDouble => "in.readDouble()"
        case SchemaString => "in.readUTF()"
        case SchemaClass(name, _) => s"""{val x$dummy = new ${name}Writable(); x$dummy.readFields(in); x$dummy.datum}"""
        case SchemaPointer(referent) => s"""if (in.readBoolean()) Some(${generateReadExpression(referent, dummy + 1)}) else None"""
        case SchemaSequence(content) => s"""(for (x$dummy <- 0 until in.readInt()) yield {${generateReadExpression(content, dummy + 1)}}).toSeq"""
      }

      def generateWriteStatement(n: String, s: Schema, dummy: Int): String = {
        val indent = "  " * dummy
        s match {
          case SchemaBool   => indent + s"out.writeBoolean($n)"
          case SchemaChar   => indent + s"out.writeByte($n)"
          case SchemaUChar  => indent + s"out.writeShort($n)"
          case SchemaShort  => indent + s"out.writeShort($n)"
          case SchemaUShort => indent + s"out.writeInt($n)"
          case SchemaInt    => indent + s"out.writeInt($n)"
          case SchemaUInt   => indent + s"out.writeLong($n)"
          case SchemaLong   => indent + s"out.writeLong($n)"
          case SchemaULong  => indent + s"out.writeDouble($n)"
          case SchemaFloat  => indent + s"out.writeFloat($n)"
          case SchemaDouble => indent + s"out.writeDouble($n)"
          case SchemaString => indent + s"out.writeUTF($n)"
          case SchemaClass(name, _) => indent + s"""$n.write(out)"""
          case SchemaPointer(referent) => indent + s"""if ($n.isEmpty)
$indent        out.writeBoolean(false)
$indent      else {
$indent        out.writeBoolean(true)
$indent      ${generateWriteStatement(n + ".get", referent, dummy + 1)}
$indent      }"""
          case SchemaSequence(content) => indent + s"""out.writeInt($n.size)
$indent      for (x$dummy <- $n) {
$indent      ${generateWriteStatement("x" + dummy.toString, content, dummy + 1)}
$indent      }"""
        }
      }

      def generateHadoopConverter(schemaClass: SchemaClass) = {
        val writable = s"${schemaClass.name}Writable"
        val header = s"      datum = new ${schemaClass.name}("
        val readExpressions = schemaClass.fields.zipWithIndex collect {
          case (SchemaField(n, _, t), i) => generateReadExpression(t, 0) + (if (i == schemaClass.fields.size - 1) ")" else ",") + "    // " + n
        }
        val writeStatements = schemaClass.fields collect {
          case SchemaField(n, _, t) => generateWriteStatement("datum." + n, t, 0)
        }
        s"""  implicit class $writable(var datum: ${schemaClass.name}) extends Writable {
    def this() { this(null) }
    def readFields(in: java.io.DataInput) {
$header${readExpressions.mkString("\n" + " " * header.size)}
    }
    def write(out: java.io.DataOutput) {
      ${writeStatements.mkString("\n      ")}
    }
  }
  object $writable {
    def read(in: java.io.DataInput) = {
      val out = new $writable()
      out.readFields(in)
      out
    }
  }
  implicit def to${schemaClass.name}(wrapper: $writable) = wrapper.datum"""
      }

      val hadoopConverters =
        if (!hadoop) ""
        else "  // Import these implicits into Hadoop to provide methods for Hadoop's custom serialization.\n" + schemaClasses.map(generateHadoopConverter).mkString("\n\n")

      val packageObject =
        "\n}\n\npackage object " + ns2 + " {\n" + myclasses + "\n\n" + tl + (if (!myclasses.isEmpty  &&  !hadoopConverters.isEmpty) "\n\n" else "") + hadoopConverters
          
      schemaClasses.map(generateScalaClass).mkString("\n\n") + packageObject
    }
  }
}
