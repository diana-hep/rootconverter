import org.apache.spark.SparkContext
import org.apache.spark.SparkContext._
import org.apache.spark.SparkConf

import org.dianahep.scaroot.reader._

import data.root._

object SparkProject {
  def main(args: Array[String]) {
    val conf = new SparkConf().setAppName("SparkProject")
    val sc = new SparkContext(conf)

    val iterator = RootTreeIterator[Tree](List("../../root2avro/test_Event/Event.root"), "T", List("../../root2avro/test_Event/Event_cxx.so"), myclasses)

    while (iterator.hasNext)
      println(iterator.next())

  }
}
