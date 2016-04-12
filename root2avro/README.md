# root2avro

A pure-C++ project that converts ROOT files into Avro on the standard output. One may use this as a streaming pipe or as a file-to-file converter.

It can query the schema and quit (for use in a tool that controls it as an external process) and it can produce JSON output instead of Avro (usually for debugging). One JSON document per line of text.

**Dependencies:**

  * [Avro-C](http://www-us.apache.org/dist/avro/stable/c/) (not Avro-C++!), which is built with CMake.
  * [Jansson](http://www.digip.org/jansson/) (Avro-C's dependency), which has a configure/make/make install cycle or is available from a package manager.

**Help:**

```
$ root2avro --help
Usage: root2avro fileLocation1, [fileLocation2, [...]], treeLocation

Where fileLocationN are either file paths or URLs TFile::Open can handle and treeLocation is the path of the
TTree in all the files.

Avro is streamed to standard output and can be redirected to a file.

Options:
  --start=NUMBER         First entry number to convert.
  --end=NUMBER           Entry number after the last to convert.
  --libs=LIB1,LIB2,...   Comma-separated list of C++ source or .so files defining objects in the TTree
                         (i.e. SOMETHING.cxx to recompile the objects on the local architecture or
                         SOMETHING_cxx.so and SOMETHING_cxx_ACLiC_dict_rdict.pcm to use precompiled binaries).
  --inferTypes           As an alternative to providing --libs, attempt to infer the class structure from the
                         ROOT file itself by inspecting its embedded streamers.
  --mode=MODE            What to write to standard output: "avro" (Avro file, default), "json" (one JSON
                         object per line), "schema" (Avro schema only), "repr" (ROOT representation only),
                         or "c++" (show C++ code that would be generated from streamers with --inferTypes).
  --codec=CODEC          Codec for compressing the Avro output; may be "null" (uncompressed, default),
                         "deflate", "snappy", "lzma", depending on libraries installed on your system.
  --block=SIZE           Avro block size in KB (default is 64); if too small, no output will be produced.
  --name=NAME            Name for schema (taken from TTree name if not provided).
  --ns=NAMESPACE         Namespace for schema (blank if not provided).
  -d, -debug, --debug    If supplied, only show the generated C++ code and exit; do not run it.
  -h, -help, --help      Print this message and exit.
```
