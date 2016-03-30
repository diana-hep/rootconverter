# Example use of ScaROOT-Reader in a compiled Spark project

This is similar to the ["commandline" example](https://github.com/diana-hep/rootconverter/tree/master/spark-examples/commandline), except that it uses sbt to compile a JAR that can be submitted to a remote Spark cluster. It hasn't been brought up-to-date, but the correct build process is there.

To compile, you do

```
sbt assembly
```

and then your new `SparkProject-assembly-0.1.jar` is ready to be submitted:

```
spark-submit --class SparkProject --master local[2] target/scala-2.10/SparkProject-assembly-0.1.jar
```
