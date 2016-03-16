# rootconverter
Converts ROOT trees into different formats to make them accessible in Big Data applications.

There are two projects here:

  * `root2avro` is a C++ program that converts ROOT TTree data into an equivalent Avro data (which may be saved to a file on disk or streamed into another application.
  * `scaroot-reader` is a hybrid Scala/C++ (through JNA) library that streams ROOT TTree data directly into the JVM. Data representation is controlled with (possibly) user-supplied callbacks.

They share code. Most of the work handles special cases of ROOT objects (which is to say, all the types of objects you can create in C++ with different ways of persisting them in ROOT, developed over the decades). Once retrieved, converting that data into other formats (Avro in `root2avro` and any JVM object in `scaroot-reader`) is straightforward.
