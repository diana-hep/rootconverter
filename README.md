# rootconverter
Converts ROOT trees into different formats to make them accessible in Big Data applications.

There are several projects here, three of which are complete. They all belong in the same git repository because they share code.

  * `root2avro` is a C++ program that converts ROOT TTree data into an equivalent Avro data (which may be saved to a file on disk or streamed into another application.
  * `scaroot-reader` is a hybrid Scala/C++ (through JNA) library that streams ROOT TTree data directly into the JVM. Data representation is controlled with (possibly) user-supplied callbacks.
  * [`Spark examples`](https://github.com/diana-hep/rootconverter/tree/master/spark-examples/commandline) shows how to use ScaROOT-Reader in Spark.

Click on the links to go to specific documentation for each.

What about the others? They're either works in progress or abandoned, since this project took a lot of false turns. Here's a brief description of each:

  * `scaroot-oldreader` does the same thing as `scaroot-reader`, but much slower. It made a separate JNI call between C++ and the JVM for each numeric value, and each of those calls cost 800 ns. That doesn't sound like much, but typical data structures are very complex, and it adds up. The new version makes one call per TTree entry, asking the C++ code to serialize everything in a shared buffer for Scala to read. Unfortunately, that required a radical redesign.
  * `scaroot-avroreader` is an empty stub. The idea was to use Avro as a data serialization format between C++ and Scala with the same in-memory buffer. I decided that it would be more work to "re-use code" by linking into Avro than doing a straightforward serialization myself (`scaroot-reader`). Also, Avro should be slower and it would introduce more dependencies.
  * `scaroot-hadoop` partially complete ROOT-in-Hadoop implementation. I ran into some technical issues, none of which are relevant for Spark, the primary use-case.

Rough performance statistics for 1000 `Event.root` entries on a single machine (my laptop). Take these numbers as _relative._

  * 1.8 sec: read TTree, discard data.
  * 1.8 sec: read TTree, create Scala objects with ScaROOT-Reader (negligible difference). However, repeating this test eventually produced some 3 second spikes, presumably due to garbage collector pauses.
  * 5.7 sec: convert to uncompressed Avro file and save. Reading from Avro file in Java: about 1 sec. Avro file is 2.0 times as large as the original ROOT file.
  * 6.1 sec: convert to Snappy-compressed Avro file and save. Avro file is 1.4 times as large as the original ROOT file.
  * 29 sec: convert to Avro with any other compression method. Avro file is 1.0 times as large as the original ROOT file (suggesting that ROOT uses something like deflate).
  * 18 sec: convert to JSON file and save. The JSON file is huge.
  * 29 sec: abandoned `scaroot-oldreader` version.

Unfortunately, file-reading cannot be parallelized in the same process: you immediately get `libRIO` segmentation faults. Adding a "micro-batch" strategy of copying several entries from C++ to Scala at a time does nothing for performance.
