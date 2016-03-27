import org.apache.spark.SparkContext
import org.apache.spark.SparkContext._
import org.apache.spark.SparkConf

import org.dianahep.scaroot.reader._

object SparkProject {
  def main(args: Array[String]) {
    val conf = new SparkConf().setAppName("Spark Project")
    val sc = new SparkContext(conf)

    case class Tree(x: Int, y: Double, z: String)
    val iterator = RootTreeIterator[Tree](List("/home/pivarski/diana-github/rootconverter/root2avro/build/multipleLeaves.root"), "t")
    while (iterator.hasNext)
      println(iterator.next())
  }
}
