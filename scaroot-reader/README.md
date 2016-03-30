# ScaROOT-Reader

Reads data from ROOT files into Scala objects using Java Native Interface (JNI) calls. General strategy:

  * uses the same "scaffolding" over C++ classes as `root2avro`
  * single-threaded: Scala waits while C++ reads (Scala's processing time is negligible compared to C++'s reading time; get parallelism from Scala-side actors)
  * shared memory buffer: C++ fills the buffer with a binary encoding of the data that Scala reads (buffer size is adaptive)
  * micro-batches: several TTree entries (10 by default) are loaded at a time, but this has no impact on performance (thought it might)
  * Scala macros create specialized code to fill user's classes with minimal overhead
  * code generator to make user classes from an example ROOT file (like ROOT's `MakeProject`).

## Installing

Be sure you have ROOT installed with `PATH` and `LD_LIBRARY_PATH` properly configured. Check out this directory and run

```
mvn install
```

with Maven 3 or later and Java 7 or later.

## Simple example

Reading data from a file named `fileName` with TTree `treeName`:

```scala
import org.dianahep.scaroot.reader._

val iterator = RootTreeIterator[Generic](List(fileName), treeName)

while (iterator.hasNext)
  println(iterator.next())
```

The `fileName` can be an XRootD URL (e.g. `root://server.fnal.gov//path/to/file.root`) and `treeName` can include an internal directory path. Unlike most Java iterators, this iterator is rewindable and seekable.

## Example with user classes

The example above would fill a class named `Generic` with data, and `Generic` is essentially just a hashmap (string-based field lookup: `genericData("fieldName")`). This is inconvenient and inefficient.

Instead, the user can supply classes for ScaROOT-Reader to fill:

```scala
case class TwoMuon(mass_mumu: Float, px: Float, py: Float, pz: Float)

val iterator = RootTreeIterator[TwoMuon](List("TrackResonanceNtuple.root"), "TrackResonanceNtuple/twoMuon")

while (iterator.hasNext) {
  val datum = iterator.next()
  println(s"mass: ${datum.mass}")
}
```

Only the class's constructor needs to match the structure of the ROOT file. It can have any methods the user finds convenient.

```scala
case class TwoMuon(mass_mumu: Float, px: Float, py: Float, pz: Float) {
  def pt = Math.sqrt(px*px + py*py)
}
```

## Complex example

The ROOT file doesn't need to be flat. Nested classes, arrays, C++ vectors, `TObjArray`, `TClonesArray`, C-style strings, C++ strings, `TString`, and pointers (which could be `null`) are all handled, just as in `root2avro`. (They share this code.)

| C++ entity | Scala equivalent |
|:-----------|:-----------------|
| `bool` | `Boolean` |
| signed integer | integer of the same bit-width (`Byte`, `Short`, `Int`, `Long`) |
| unsigned integer | number of the next-higher bit-width |
| floating-point number | floating-point number of the same bit-width |
| any kind of string | Java `String` or `Bytes` |
| any depth of pointer | Scala `Option`, which can be `Some(data)` or `None` |
| any kind of array/list | Scala `Iterable` (doesn't have to correspond) |
| nested class instance | nested class instance |

To read structured data from ROOT, one must provide a ROOT dictionary (dynamic library ending in `_cxx.so` and a corresponding `.pcm`).

To build (non-`Generic`) structures in Scala, one must provide class definitions and a `myclasses` map from C++ name to instrumented Scala code. For example,

```scala
case class Tree(...)
case class Event(...)
case class EventHeader(...)
case class Track(...)
case class TBits(...)

val myclasses = Map("T" -> My[Tree], "Event" -> My[Event], "EventHeader" -> My[EventHeader], "Track" -> My[Track], "TBits" -> My[TBits])
```

To generate these classes automatically, use the ScaROOT-Reader JAR as a command:

```
java -jar scaroot-reader.jar Event.root T --libs=Event_cxx.so --name=Tree
```

where `scaroot-reader.jar` is the compiled ScaROOT-Reader JAR (probably named `scaroot-reader-0.1-scala_2.10-root_6.06.jar` in the `target` directory), `Event.root` is the name of the ROOT file, `T` is the name of the tree (with internal directories), `Event_cxx.so` is a (comma-separated) list of libraries, and `--name=Tree` overrides naming the tree class `T`.

The resulting code goes to standard output for inspection; pipe it to a file named `myclasses.scala` and put it in your project. (See [Spark example](https://github.com/diana-hep/rootconverter/tree/master/spark-examples/commandline) project.) By default, they go in a package named `data.root` (override with the `--ns` flag; see `--help` for all flags).
