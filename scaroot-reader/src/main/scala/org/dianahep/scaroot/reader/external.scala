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

import scala.collection.JavaConversions._
import scala.reflect.runtime.currentMirror
import scala.reflect.runtime.universe.NoSymbol
import scala.reflect.runtime.universe.Symbol
import scala.reflect.runtime.universe.Type
import scala.reflect.runtime.universe.WeakTypeTag
import scala.reflect.runtime.universe.weakTypeOf

import org.dianahep.scaroot.reader.schema._
import org.dianahep.scaroot.reader.factory._

package external {
  class RootTreeIterator[TYPE : WeakTypeTag : My](fileLocations: Seq[String],
                                                  treeLocation: String,
                                                  includes: Seq[String] = Nil,
                                                  libs: Seq[String] = Nil,
                                                  inferTypes: Boolean = false,
                                                  myclasses: Map[String, My[_]] = Map[String, My[_]](),
                                                  start: Long = 0L,
                                                  end: Long = -1L,
                                                  command: String = "./root2avro",
                                                  environment: Map[String, String] = Map[String, String](),
                                                  numberOfTrials: Int = 4) extends Iterator[TYPE] {
    if (fileLocations.isEmpty)
      throw new RuntimeException("Cannot build RootTreeIterator over an empty set of files.")
    if (start < 0)
      throw new IllegalArgumentException(s"The start ($start) must be greater than or equal to zero.")
    if (end >= 0  &&  start >= end)
      throw new IllegalArgumentException(s"If an ending index is given (greater than or equal to zero), then start ($start) must be less than end ($end).")

    private def packageName(sym: Symbol) = {
      def enclosingPackage(sym: Symbol): Symbol = {
        if (sym == NoSymbol) NoSymbol
        else if (sym.isPackage) sym
        else enclosingPackage(sym.owner)
      }
      val pkg = enclosingPackage(sym)
      if (pkg == currentMirror.EmptyPackageClass) ""
      else pkg.fullName
    }

    val name = weakTypeOf[TYPE].typeSymbol.asClass.name.toString
    val ns = packageName(weakTypeOf[TYPE].typeSymbol.asClass)

    val repr = {
      val processBuilder = new java.lang.ProcessBuilder(arguments("repr", 0L))
      val env = processBuilder.environment
      environment foreach {case (n, v) => env.put(n, v)}

      val process = processBuilder.start()
      val returnCode = process.waitFor

      new java.util.Scanner(process.getInputStream).useDelimiter("\\A").next
    }

    val schema = reprToSchema(repr, name)

    import scala.reflect.runtime.universe.weakTypeOf
    val allmyclasses =
      if (!(myclasses.keySet contains schema.name)  &&  !(weakTypeOf[TYPE] =:= weakTypeOf[Generic]  ||  weakTypeOf[TYPE] =:= weakTypeOf[AnyRef]))
        myclasses.updated(schema.name, implicitly[My[TYPE]])
      else
        myclasses

    val factory = FactoryClass[TYPE](schema, allmyclasses)

    def arguments(mode: String, index: Long = start) = List(command, fileLocations.head, treeLocation, "--mode=" + mode) ++
      (if (includes.isEmpty) Nil else List("--includes=" + includes.mkString(","))) ++
      (if (libs.isEmpty) Nil else List("--libs" + libs.mkString(","))) ++
      (if (!inferTypes) Nil else List("--inferTypes")) ++
      List("--start=" + index.toString) ++
      (if (end < 0L) Nil else List("--end=" + end.toString)) ++
      List("--name=" + name)

    private var entryIndex = -1L
    def index = entryIndex
    var state: Option[(java.lang.Process, DataStream)] = None

    def restartProcess(index: Long) {
      state.foreach(_._1.destroy())
      state = None

      entryIndex = index
      if (end < 0L  ||  index < end) {
        val processBuilder = new java.lang.ProcessBuilder(arguments("dump", index))
        val env = processBuilder.environment
        environment foreach {case (n, v) => env.put(n, v)}

        val process = processBuilder.start()
        val inputStream = process.getInputStream
        val dataStream =
          if (java.nio.ByteOrder.nativeOrder == java.nio.ByteOrder.LITTLE_ENDIAN)
            new LittleEndianDataStream(new java.io.DataInputStream(inputStream))
          else
            new BigEndianDataStream(new java.io.DataInputStream(inputStream))
        state = Some((process, dataStream))
      }
    }
    restartProcess(start)

    private def getNext(trials: Int, exception: Option[Throwable]): Option[TYPE] =
      if (trials == 0) {
        state.foreach(_._1.destroy())
        state = None
        throw new RuntimeException(s"""external process failed: ${arguments("dump", start).mkString(" ")}""", exception.getOrElse(null))
      }
      else state match {
        case Some((_, stream)) =>
          val entry = stream.getLong
          if (entry == -1)
            None                            // normal end of stream
          else if (entry != entryIndex) {
            restartProcess(entryIndex)      // wrong entry number; restart process
            getNext(trials - 1, Some(new java.io.IOException(s"expected entry number $entryIndex but got $entry from root2avro")))
          }
          else {
            val out = Some(factory(stream)) // get data
            entryIndex += 1L
            out
          }

        case None =>
          restartProcess(entryIndex)        // process died on its own; restart it
          getNext(trials - 1, Some(new RuntimeException("external process terminated early")))
      }

    private var theNext = getNext(numberOfTrials, None)

    def hasNext = !theNext.isEmpty
    def next() = {
      val out = theNext.get
      theNext = getNext(numberOfTrials, None)
      out
    }
  }

  object RootTreeIterator {
    def apply[TYPE : WeakTypeTag : My](fileLocations: Seq[String],
                                       treeLocation: String,
                                       includes: Seq[String] = Nil,
                                       libs: Seq[String] = Nil,
                                       inferTypes: Boolean = false,
                                       myclasses: Map[String, My[_]] = Map[String, My[_]](),
                                       start: Long = 0L,
                                       end: Long = -1L,
                                       command: String = "./root2avro",
                                       environment: Map[String, String] = Map[String, String](),
                                       numberOfTrials: Int = 4) =
      new RootTreeIterator[TYPE](fileLocations, treeLocation, includes, libs, inferTypes, myclasses, start, end, command, environment, numberOfTrials)
  }

}
