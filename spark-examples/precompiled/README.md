sbt assembly

spark-submit --class SparkProject --master local[2] target/scala-2.10/SparkProject-assembly-0.1.jar
