# Example use of ScaROOT-Reader in Spark

Since ScaROOT-Reader exposes ROOT trees in Scala, they are immediately available in Spark, without (much) extra work. This example uses sbt to compile a project and provide an interactive prompt. Note that sbt downloads Spark-Standalone: you don't even need to have a copy of Spark! You do, however, [need a copy of sbt](http://www.scala-sbt.org/download.html), and you have to have compiled the `scaroot-reader` project with Maven (`mvn install`).

## Step 1 (optional): make container classes

These are classes that ROOT data will be copied into. Without them, ScaROOT-Reader would read all structures as a class called `Generic`, which is essentially just a hashmap (string-based lookup). Container classes make it easier to navigate the data (with an `event.track` style interface and tab completion) and they accelerate loading and processing (since fields are referenced by pointer, rather than string-based lookup).

For this, you'll need

   * a compiled `scaroot-reader.jar`
   * a ROOT dictionary for your ROOT file's classes, if any (here, `Event_cxx.so`)
   * a sample of the ROOT files you want to analyze. This example uses files on an XRootD server (note the `root://` prefix).

```
SCAROOT_READER=../../scaroot-reader/target/scaroot-reader-0.1-scala_2.10-root_6.06.jar

CXX_CLASSES=../../root2avro/test_Event/Event_cxx.so 

SAMPLE_FILE=root://cmseos.fnal.gov/store/user/pivarski/tree4/Event-0.root

java -jar $SCAROOT_READER $SAMPLE_FILE t4 --libs=$CXX_CLASSES --name=Tree > myclasses.scala
```

Now you have a new `myclasses.scala` file with Scala versions of the C++ classes. You can add your own methods to these classes; see the comments for details. Since it's in this directory, sbt will automatically compile it and load it into the interactive session.

## Step 2: configure sbt build file

Look at [`build.sbt`](https://github.com/diana-hep/rootconverter/blob/master/spark-examples/commandline/build.sbt). It tells sbt to download Spark and to use your personal Maven repository to find ScaROOT-Reader. This assumes that you compiled it with `mvn install`.

It also has a few startup commands in it, to load a Spark context and import your classes.

## Step 3: start Spark

Start the interactive session. It will take a while to download all the packages the first time only. This is actually an ordinary Scala prompt, since sbt is a Scala build tool, but the startup commands define a local `SparkContext`, which is what makes a Spark prompt a Spark prompt.

```
sbt console
```

## Step 4: create an RDD

Since the data are being pulled from a distinct server from the Spark cluster, we don't need to worry about data locality. (None of the data are local.) Therefore, we'll create a file list RDD and use that to populate a dataset.

The number of partitions (here, 5) is the number of concurrent map tasks: it should be the number of available processors on your Spark cluster. The `XRootD.balance` function comes from ScaROOT-Reader; it looks up files on XRootD and makes a set of file sets that are balanced by size. It accepts wildcards in the path.

```scala
val numPartitions = 5

val fileList = XRootD.balance("root://cmseos.fnal.gov/store/user/pivarski/tree4/*.root", numPartitions)

val fileListRDD = sc.parallelize(fileList, numPartitions)
```

Each element of the `fileListRDD` is a set of files to process. The `RootTreeIterator` turns a list of files into an iterator over their data. The only tricky part is to note that we want to do a `flatMap` over these iterators to get one continuous stream of data from all the files.

```scala
val libs = List("../../root2avro/test_Event/Event_cxx.so")

val rdd = fileListRDD.flatMap(x => RootTreeIterator[Tree](x.map(_.url), treeLocation, libs, myclasses))
```

## Step 5: show that it works

We can do all the usual Spark things. For instance, dumping the first 10 elements of one of the fields:

```scala
rdd.map(_.event_split.fTemperature).take(10)
```

Performing calculations over deeply nested objects. In this case, `fTracks` is a pointer to a `TClonesArray` of tracks, which ScaROOT-Reader maps to `Option[Seq[Track]]`. The pointer could be null, so the `Option` could be `None`. To treat the `None` as an empty list, we do `.toList.flatten` on the `fTracks`. To create a dataset of tracks, rather than events, we do a `flatMap` on them.

```scala
def pt(t: Track) = Math.sqrt(t.fPx*t.fPx + t.fPy*t.fPy)

rdd.flatMap(_.event_split.fTracks.toList.flatten).map(pt).take(10)
```

This RDD can be persisted (cached) for faster access. We apply the `persist` after the `RootTreeIterator` has been unpacked, so that we're caching Scala objects in memory, not file names!

```scala
rdd.persist()

def getTemperatures = {
  val before = System.nanoTime
  val out = rdd.map(_.event_split.fTemperature).collect
  println(s"${(System.nanoTime - before) / 1e9} seconds")
  out
}

val temperatures = getTemperatures

88.935740554 seconds

val temperatures2 = getTemperatures

0.067679604 seconds

temperatures == temperatures2                     // usually false
temperatures.toSet == temperatures2.toSet         // always true

val numTracks = rdd.map(_.event_split.fNtrack).collect    // also takes ~0.03 seconds
```
