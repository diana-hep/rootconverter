package test.scala.scaroot

import scala.collection.mutable
import scala.language.postfixOps

import org.scalatest.FlatSpec
import org.scalatest.junit.JUnitRunner
import org.scalatest.Matchers

import com.sun.jna.Pointer

import org.dianahep.rootreader._

class DefaultSuite extends FlatSpec with Matchers {
  "stuff" must "work" in {
    println("here")
  }

}
