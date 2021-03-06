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

import com.sun.jna.Pointer

import org.dianahep.scaroot.reader._
import org.dianahep.scaroot.reader.factory._
import org.dianahep.scaroot.reader.json._

package schema {
  // Numerical enums from C++ that can be matched in case statements.
  // Also used in a stack to build the schema used at runtime.
  // Must be kept in-sync with scaroot-flatreader/src/main/cpp/datawalker.h!

  sealed class SchemaInstruction(val index: Int, val name: String) {
    def unapply(x: Int) = (x == index)
    override def toString() = s"SchemaInstruction.$name"
  }
  object SchemaInstruction {
    val SchemaBool   = new SchemaInstruction(0, "SchemaBool")
    val SchemaChar   = new SchemaInstruction(1, "SchemaChar")
    val SchemaUChar  = new SchemaInstruction(2, "SchemaUChar")
    val SchemaShort  = new SchemaInstruction(3, "SchemaShort")
    val SchemaUShort = new SchemaInstruction(4, "SchemaUShort")
    val SchemaInt    = new SchemaInstruction(5, "SchemaInt")
    val SchemaUInt   = new SchemaInstruction(6, "SchemaUInt")
    val SchemaLong   = new SchemaInstruction(7, "SchemaLong")
    val SchemaULong  = new SchemaInstruction(8, "SchemaULong")
    val SchemaFloat  = new SchemaInstruction(9, "SchemaFloat")
    val SchemaDouble = new SchemaInstruction(10, "SchemaDouble")
    val SchemaString = new SchemaInstruction(11, "SchemaString")

    val SchemaClassName      = new SchemaInstruction(12, "SchemaClassName")
    val SchemaClassPointer   = new SchemaInstruction(13, "SchemaClassPointer")
    val SchemaClassFieldName = new SchemaInstruction(14, "SchemaClassFieldName")
    val SchemaClassFieldDoc  = new SchemaInstruction(15, "SchemaClassFieldDoc")
    val SchemaClassEnd       = new SchemaInstruction(16, "SchemaClassEnd")
    val SchemaClassReference = new SchemaInstruction(17, "SchemaClassReference")

    val SchemaPointer = new SchemaInstruction(18, "SchemaPointer")

    val SchemaSequence = new SchemaInstruction(19, "SchemaSequence")
  }

  /////////////////////////////////////////////////// schemas (type information from ROOT)
  // Note that type information is known only at runtime!

  sealed trait Schema {
    def cpp: String  // just for error messages
    def toString(indent: Int, memo: mutable.Set[String], avoidCycles: Boolean): String = toString()
  }
  object Schema {
    def apply(treeWalker: Pointer): SchemaClass = {
      sealed trait StackElement
      case class I(schemaInstruction: Int, data: Pointer) extends StackElement
      case class S(schema: Schema) extends StackElement

      var stack: List[StackElement] = Nil
      val schemaClasses = mutable.Map[String, Schema]()

      // This function is passed to the TreeWalker, which calls it in
      // a depth-first traversal of the tree. We have to turn that
      // traversal into a tree of nested Schemas.
      object SchemaBuilder extends RootReaderCPPLibrary.SchemaBuilder {
        def apply(schemaInstruction: Int, data: Pointer) {
          stack = I(schemaInstruction, data) :: stack

          // Collapse complete instruction sets into Schema objects.
          var done1 = false
          while (!done1) stack match {
            // Primitives are easy: just convert the instruction into the corresponding Schema.
            case I(SchemaInstruction.SchemaBool(), _)   :: rest  =>  stack = S(SchemaBool) :: rest
            case I(SchemaInstruction.SchemaChar(), _)   :: rest  =>  stack = S(SchemaChar) :: rest
            case I(SchemaInstruction.SchemaUChar(), _)  :: rest  =>  stack = S(SchemaUChar) :: rest
            case I(SchemaInstruction.SchemaShort(), _)  :: rest  =>  stack = S(SchemaShort) :: rest
            case I(SchemaInstruction.SchemaUShort(), _) :: rest  =>  stack = S(SchemaUShort) :: rest
            case I(SchemaInstruction.SchemaInt(), _)    :: rest  =>  stack = S(SchemaInt) :: rest
            case I(SchemaInstruction.SchemaUInt(), _)   :: rest  =>  stack = S(SchemaUInt) :: rest
            case I(SchemaInstruction.SchemaLong(), _)   :: rest  =>  stack = S(SchemaLong) :: rest
            case I(SchemaInstruction.SchemaULong(), _)  :: rest  =>  stack = S(SchemaULong) :: rest
            case I(SchemaInstruction.SchemaFloat(), _)  :: rest  =>  stack = S(SchemaFloat) :: rest
            case I(SchemaInstruction.SchemaDouble(), _) :: rest  =>  stack = S(SchemaDouble) :: rest
            case I(SchemaInstruction.SchemaString(), _) :: rest  =>  stack = S(SchemaString) :: rest

            // Classes are more complex: they're bracketed between SchemaClassName and SchemaClassEnd,
            // with SchemaClassFieldName, <type> pairs in between. Those <types> have already been
            // collapsed down to Schema objects, whether they're primitives or nested classes.
            // (Deterministic pushdown automaton!)
            case I(SchemaInstruction.SchemaClassEnd(), _) :: rest1 =>
              stack = rest1

              // Note: filling the stack reverses the order of the fields, and this operation reverses
              // them back (for the same reason: Lists are filled on the left).
              var fields = List[SchemaField]()
              var done2 = false
              while (!done2) stack match {
                case S(schema) :: I(SchemaInstruction.SchemaClassFieldDoc(), commentPtr) :: I(SchemaInstruction.SchemaClassFieldName(), fieldNamePtr) :: rest2 =>
                  stack = rest2
                  fields = SchemaField(fieldNamePtr.getString(0), commentPtr.getString(0), schema) :: fields
                case _ =>
                  done2 = true
              }

              stack match {
                // The SchemaClassPointer instruction is for scaroot-directreader.
                // Not used here, but we have to step over it.
                case I(SchemaInstruction.SchemaClassPointer(), _) :: I(SchemaInstruction.SchemaClassName(), classNamePtr) :: rest3 =>
                  // Keep this SchemaClass object in a mutable.Map.
                  val className = classNamePtr.getString(0)
                  val schemaClass = SchemaClass(className, fields)
                  schemaClasses(className) = schemaClass

                  stack = S(schemaClass) :: rest3
              }

            // The depth-first traversal fully defines classes only once; thereafter they're referenced
            // by name. This avoids infinite loops for recursive types (e.g. linked lists or trees).
            case I(SchemaInstruction.SchemaClassReference(), classNamePtr) :: rest =>
              // Now we use the mutable.Map to get that SchemaClass back.
              stack = S(schemaClasses(classNamePtr.getString(0))) :: rest

            // Pointers are wrappers around a <type>, which has already been collapsed to a Schema.
            case S(referent) :: I(SchemaInstruction.SchemaPointer(), _) :: rest =>
              stack = S(SchemaPointer(referent)) :: rest

            // So are sequences.
            case S(content) :: I(SchemaInstruction.SchemaSequence(), _) :: rest =>
              stack = S(SchemaSequence(content)) :: rest

            case _ =>
              // We've collapsed this as much as we can without more instructions.
              done1 = true
          }
        }
      }

      // Now we actually pass our function to the TreeWalker.
      RootReaderCPPLibrary.buildSchema(treeWalker, SchemaBuilder)

      // And pluck off the result (making strong assumptions about the state of the stack:
      // will be a runtime error if it's not a single Schema at the end.
      val S(result: SchemaClass) :: Nil = stack
      result
    }
  }

  case object SchemaBool extends Schema { val cpp = "bool" }
  case object SchemaChar extends Schema { val cpp = "char" }
  case object SchemaUChar extends Schema { val cpp = "unsigned char" }
  case object SchemaShort extends Schema { val cpp = "short" }
  case object SchemaUShort extends Schema { val cpp = "unsigned short" }
  case object SchemaInt extends Schema { val cpp = "int" }
  case object SchemaUInt extends Schema { val cpp = "unsigned int" }
  case object SchemaLong extends Schema { val cpp = "long" }
  case object SchemaULong extends Schema { val cpp = "unsigned long" }
  case object SchemaFloat extends Schema { val cpp = "float" }
  case object SchemaDouble extends Schema { val cpp = "double" }
  case object SchemaString extends Schema { val cpp = "STRING" }

  case class SchemaField(name: String, comment: String, schema: Schema) {
    override def toString() = toString(0, mutable.Set[String](), true)
    def toString(indent: Int, memo: mutable.Set[String], avoidCycles: Boolean) =
      s"""SchemaField(${Literal(Constant(name))}, ${Literal(Constant(comment))}, ${schema.toString(indent, memo, avoidCycles)})"""
  }

  class SchemaClass(val name: String, getFields: => List[SchemaField]) extends Schema {
    lazy val fields = getFields
    def cpp = name
    override def toString() = toString(0, mutable.Set[String](), true)
    override def toString(indent: Int, memo: mutable.Set[String], avoidCycles: Boolean) =
      if (avoidCycles  &&  (memo contains name))
        s"""SchemaClass(name = ${Literal(Constant(name)).toString}, fields = <see above>)"""
      else {
        memo += name
        s"""SchemaClass(name = ${Literal(Constant(name)).toString}, fields = List(${fields.map("\n" + " " * indent + "  " + _.toString(indent + 2, memo, avoidCycles)).mkString(",")}${"\n" + " " * indent}))"""
      }
    def copy(name: String = this.name, fields: List[SchemaField] = this.fields) = new SchemaClass(name, fields)
    override def equals(that: Any) = that match {
      case that: SchemaClass => this.name == that.name  &&  this.fields == that.fields
      case _ => false
    }
    override def hashCode() = (name, fields).hashCode()
  }
  object SchemaClass {
    def apply(name: String, getFields: => List[SchemaField]) = new SchemaClass(name, getFields)
    def unapply(x: SchemaClass) = Some((x.name, x.fields))
  }

  case class SchemaPointer(referent: Schema) extends Schema {
    def cpp = s"POINTER<$referent>"
    override def toString(indent: Int, memo: mutable.Set[String], avoidCycles: Boolean) = s"""SchemaPointer(${referent.toString(indent, memo, avoidCycles)})"""
  }

  case class SchemaSequence(content: Schema) extends Schema {
    def cpp = s"SEQUENCE<$content>"
    override def toString(indent: Int, memo: mutable.Set[String], avoidCycles: Boolean) = s"""SchemaSequence(${content.toString(indent, memo, avoidCycles)})"""
  }
}

package object schema {
  def reprToSchemaRecursive(json: Json, memo: mutable.Map[String, SchemaClass]): Schema = json match {
    case JsonString("bool") | JsonString("Bool_t") => SchemaBool
    case JsonString("char") | JsonString("Char_t") | JsonString("Int8_t") => SchemaChar
    case JsonString("unsigned char") | JsonString("UChar_t") | JsonString("UInt8_t") => SchemaUChar
    case JsonString("short") | JsonString("Short_t") | JsonString("Short16_t") | JsonString("Int16_t") => SchemaShort
    case JsonString("unsigned short") | JsonString("UShort_t") | JsonString("UShort16_t") | JsonString("UInt16_t") => SchemaUShort
    case JsonString("int") | JsonString("Int_t") | JsonString("Int32_t") => SchemaInt
    case JsonString("unsigned int") | JsonString("UInt_t") | JsonString("UInt32_t") => SchemaUInt
    case JsonString("long") | JsonString("long long") | JsonString("Long_t") | JsonString("Long64_t") => SchemaLong
    case JsonString("unsigned long") | JsonString("unsigned long long") | JsonString("ULong_t") | JsonString("ULong64_t") => SchemaULong
    case JsonString("float") | JsonString("Float_t") | JsonString("Float16_t") => SchemaFloat
    case JsonString("double") | JsonString("Double_t") | JsonString("Double32_t") => SchemaDouble
    case JsonString("char*") | JsonString("Char_t*") => SchemaString

    case JsonString(ref) => memo(ref)

    case JsonObject((JsonString("*"), t)) => SchemaPointer(reprToSchemaRecursive(t, memo))

    case JsonObject((JsonString("TObjArray") | JsonString("TRefArray") | JsonString("TClonesArray") | JsonString("std::vector"), t)) =>
      SchemaSequence(reprToSchemaRecursive(t, memo))

    case JsonObject((JsonString("[]"), JsonObject((JsonString("numItems"), _), (JsonString("sizeOf"), _), (JsonString("type"), t)))) =>
      SchemaSequence(reprToSchemaRecursive(t, memo))

    case JsonObject((JsonString(className), JsonObject(fields @ _*))) =>
      val out = SchemaClass(className, fields.toList map {
        case (JsonString(fieldName), t) => SchemaField(fieldName, "", reprToSchemaRecursive(t, memo))
      })
      memo(className) = out
      val tmp = out.fields     // Strictly evaluate the contents so they'll go into memo.
      out                      // Please, compiler, don't optimize this out!

    case x => throw new Exception(x.toString)
  }

  def reprToSchema(repr: String, name: String): SchemaClass = {
    val memo = mutable.Map[String, SchemaClass]()

    Json.parse(repr) match {
      case Some(JsonObject(branches @ _*)) =>
        SchemaClass(name, branches.toList map {
          case (JsonString(branchName), JsonObject(extractorFields @ _*)) => extractorFields.collectFirst({
            case (JsonString("type"), t) => SchemaField(branchName, "", reprToSchemaRecursive(t, memo))
          }).get
          case _ => throw new IllegalArgumentException("invalid repr structure")
        })
      case None =>
        throw new IllegalArgumentException("invalid JSON")
      case _ =>
        throw new IllegalArgumentException("invalid repr structure")
    }
  }

}
