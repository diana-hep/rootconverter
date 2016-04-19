// Copyright 2016 Jim Pivarski
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "datawalker.h"
#include "streamerToCode.h"

#define NA ((uint64_t)(-1))

using namespace ROOT::Internal;
// using namespace ROOT;

// global variables for this tiny, single-threaded program (parallelism comes from multiple processes)
std::vector<std::string> fileLocations;
std::string              treeLocation;
uint64_t                 start = NA;
uint64_t                 end = NA;
std::vector<std::string> libs;
std::vector<std::string> includes;
bool                     inferTypes = false;
std::string              mode = "avro";
std::string              codec = "null";
int                      blockKB = 64;
std::string              schemaName = "";
std::string              ns = "";
bool                     debug = false;

void help(bool banner) {
  if (banner)
    std::cerr << "*****************************************************************************************************************" << std::endl
              << "* root2avro version " << VERSION << std::endl
              << "*****************************************************************************************************************" << std::endl << std::endl;
  std::cerr << "Usage: root2avro fileLocation1, [fileLocation2, [...]], treeLocation" << std::endl << std::endl
            << "Where fileLocationN are either file paths or URLs TFile::Open can handle and treeLocation is the path of the" << std::endl
            << "TTree in all the files." << std::endl << std::endl
            << "Avro is streamed to standard output and can be redirected to a file." << std::endl << std::endl
            << "Options:" << std::endl
            << "  --start=NUMBER            First entry number to convert." << std::endl
            << "  --end=NUMBER              Entry number after the last to convert." << std::endl
            << "  --libs=LIB1,LIB2,...      Comma-separated list of C++ source or .so files defining objects in the TTree" << std::endl
            << "                            (i.e. SOMETHING.cxx to recompile the objects on the local architecture or" << std::endl
            << "                            SOMETHING_cxx.so and SOMETHING_cxx_ACLiC_dict_rdict.pcm to use precompiled binaries)." << std::endl
            << "  --includes=DIR1,DIR2...   Add include directories to the path for use in compiling C++ libs (above)." << std::endl
            << "  --inferTypes              As an alternative to providing --libs, attempt to infer the class structure from the" << std::endl
            << "                            ROOT file itself by inspecting its embedded streamers." << std::endl
            << "  --mode=MODE               What to write to standard output:" << std::endl
            << "                                * \"avro\" (Avro file, default)" << std::endl
            << "                                * \"avro-stream\" (schemaless Avro fragments with entry numbers)" << std::endl
            << "                                * \"dump\" (raw dump of data that can be interpreted by ScaROOT-Reader)" << std::endl
            << "                                * \"json\" (one JSON object per line, schemaless)" << std::endl
            << "                                * \"schema\" (just the Avro schema as a JSON document)" << std::endl
            << "                                * \"repr\" (custom JSON schema representing the ROOT source)" << std::endl
            << "                                * \"c++\" (C++ code that would be generated from streamers with --inferTypes)" << std::endl
            << "  --codec=CODEC             Codec for compressing the Avro output; may be \"null\" (uncompressed, default)," << std::endl
            << "                            \"deflate\", \"snappy\", \"lzma\", depending on libraries installed on your system." << std::endl
            << "  --block=SIZE              Avro block size in KB (default is 64); if too small, no output will be produced." << std::endl
            << "  --name=NAME               Name for schema (taken from TTree name if not provided)." << std::endl
            << "  --ns=NAMESPACE            Namespace for schema (blank if not provided)." << std::endl
            << "  -d, -debug, --debug       If supplied, only show the generated C++ code and exit; do not run it." << std::endl
            << "  -h, -help, --help         Print this message and exit." << std::endl;
}

std::vector<std::string> splitByComma(std::string in) {
  std::vector<std::string> out;
  std::stringstream ss(in);
  std::string item;
  while (std::getline(ss, item, ','))
    out.push_back(item);
  return out;
}

int main(int argc, char **argv) {
  for (int i = 1;  i < argc;  i++) {
    if (std::string(argv[i]) == std::string("-h")  ||
        std::string(argv[i]) == std::string("-help")  ||
        std::string(argv[i]) == std::string("--help")) {
      help(true);
      return 0;
    }
  }

  std::string startPrefix("--start=");
  std::string endPrefix("--end=");
  std::string libsPrefix("--libs=");
  std::string includesPrefix("--includes=");
  std::string inferTypesPrefix("--inferTypes");
  std::string modePrefix("--mode=");
  std::string codecPrefix("--codec=");
  std::string blockPrefix("--block=");
  std::string namePrefix("--name=");
  std::string nsPrefix("--ns=");
  std::string badPrefix("-");

  for (int i = 1;  i < argc;  i++) {
    std::string arg(argv[i]);

    if (arg.substr(0, startPrefix.size()) == startPrefix) {
      std::string value = arg.substr(startPrefix.size(), arg.size());
      start = strtoul(value.c_str(), nullptr, 10);
    }

    else if (arg.substr(0, endPrefix.size()) == endPrefix) {
      std::string value = arg.substr(endPrefix.size(), arg.size());
      end = strtoul(value.c_str(), nullptr, 10);
    }

    else if (arg.substr(0, libsPrefix.size()) == libsPrefix) {
      libs = splitByComma(arg.substr(libsPrefix.size(), arg.size()));
    }

    else if (arg.substr(0, includesPrefix.size()) == includesPrefix) {
      includes = splitByComma(arg.substr(includesPrefix.size(), arg.size()));
    }

    else if (arg.substr(0, inferTypesPrefix.size()) == inferTypesPrefix) {
      inferTypes = true;
    }

    else if (arg.substr(0, modePrefix.size()) == modePrefix) {
      mode = arg.substr(modePrefix.size(), arg.size());
    }

    else if (arg.substr(0, codecPrefix.size()) == codecPrefix)
      codec = arg.substr(codecPrefix.size(), arg.size());

    else if (arg.substr(0, blockPrefix.size()) == blockPrefix) {
      std::string value = arg.substr(blockPrefix.size(), arg.size());
      blockKB = atoi(value.c_str());
    }

    else if (arg.substr(0, namePrefix.size()) == namePrefix) {
      schemaName = arg.substr(namePrefix.size(), arg.size());
    }

    else if (arg.substr(0, nsPrefix.size()) == nsPrefix) {
      ns = arg.substr(nsPrefix.size(), arg.size());
    }

    else if (arg == std::string("-d")  ||  arg == std::string("-debug")  ||  arg == std::string("--debug"))
      debug = true;

    else if (arg.substr(0, badPrefix.size()) == badPrefix) {
      std::cerr << "Recognized switches are: --start, --end, --mode, --codec, --libs, --includes, --inferTypes, --name, --ns, --debug, --help." << std::endl;
      return -1;
    }

    else
      fileLocations.push_back(arg);
  }

  if (fileLocations.size() < 2) {
    std::cerr << "At least two (non-switch) arguments are required." << std::endl;
    return -1;
  }
  treeLocation = fileLocations.back();
  fileLocations.pop_back();

  if (start != NA  &&  end != NA  &&  start > end) {
    std::cerr << "Start must be less than or equal to end (if provided)." << std::endl;
    return -1;
  }

  // ROOT initialization
  resetSignals();

  for (auto include = includes.begin();  include != includes.end();  ++include)
    addInclude(include->c_str());

  for (auto lib = libs.begin();  lib != libs.end();  ++lib)
    loadLibrary(lib->c_str());

  // C++ code generation from inferTypes
  if (inferTypes  ||  mode == std::string("c++")) {
    std::string url = fileLocations[0];
    if (url.find(std::string("://")) == std::string::npos)
      url = std::string("file://") + url;

    std::vector<std::string> classNames;
    std::string errorMessage;
    std::string code = generateCodeFromStreamers(url, treeLocation, classNames, errorMessage);

    if (code.empty()) {
      std::cerr << errorMessage << std::endl;
      return -1;
    }

    if (mode == std::string("c++")) {
      std::cout << code << std::endl;
      return 0;
    }
    else
      declareClasses(code, classNames);
  }

  TreeWalker *treeWalker = nullptr;

  // main loop
  uint64_t currentEntry = 0;
  for (int fileIndex = 0;  fileIndex < fileLocations.size();  fileIndex++) {
    std::string url = fileLocations[fileIndex];
    if (url.find(std::string("://")) == std::string::npos)
      url = std::string("file://") + url;

    // set up or update the TreeWalker
    if (treeWalker != nullptr) {
      treeWalker->reset(url);
      if (treeWalker->valid) treeWalker->next();
    }
    else {
      treeWalker = new TreeWalker(url, treeLocation, schemaName, ns);
      while (treeWalker->valid  &&  !treeWalker->resolved()  &&  treeWalker->next())
        treeWalker->resolve();
      if (!treeWalker->resolved()) {
        std::cerr << "Could not resolve dynamic types (e.g. TClonesArray); is the first file empty?" << std::endl;
        return -1;
      }
    }
    if (!treeWalker->valid) {
      std::cerr << treeWalker->errorMessage << std::endl;
      return -1;
    }

    // skip this file if the first requested entry comes after it
    if (start != NA  &&  start >= currentEntry + treeWalker->numEntriesInCurrentTree()) {
      currentEntry += treeWalker->numEntriesInCurrentTree();
      continue;
    }

    // print out JSON strings (one JSON document per line)
    if (mode == std::string("json")) {
      if (start != NA  &&  start > currentEntry) {
        treeWalker->setEntryInCurrentTree(start - currentEntry);
        currentEntry = start;
      }
      else
      treeWalker->setEntryInCurrentTree(0);

      do {
        if (end != NA  &&  currentEntry >= end)
          return 0;

        treeWalker->printJSON();
        currentEntry += 1;
      } while (treeWalker->next());
    }

#ifdef AVRO
    // print out Avro bytes (with an "Obj" header)
    else if (mode == std::string("avro")) {
      if (start != NA  &&  start > currentEntry) {
        treeWalker->setEntryInCurrentTree(start - currentEntry);
        currentEntry = start;
      }
      else
      treeWalker->setEntryInCurrentTree(0);

      if (!treeWalker->printAvroHeaderOnce(codec, blockKB * 1024, false)) return -1;
      do {
        if (end != NA  &&  currentEntry >= end) {
          treeWalker->closeAvro();
          return 0;
        }

        if (!treeWalker->printAvro(false, currentEntry)) {
          treeWalker->closeAvro();
          return -1;
        }

        currentEntry += 1;
      } while (treeWalker->next());
    }

    // print out Avro bytes (WITHOUT the "Obj" header)
    else if (mode == std::string("avro-stream")) {
      if (start != NA  &&  start > currentEntry) {
        treeWalker->setEntryInCurrentTree(start - currentEntry);
        currentEntry = start;
      }
      else
      treeWalker->setEntryInCurrentTree(0);

      if (!treeWalker->printAvroHeaderOnce(codec, blockKB * 1024, true)) return -1;
      do {
        if (end != NA  &&  currentEntry >= end) {
          treeWalker->closeAvro();
          return 0;
        }

        if (!treeWalker->printAvro(true, currentEntry)) {
          treeWalker->closeAvro();
          return -1;
        }

        currentEntry += 1;
      } while (treeWalker->next());
    }

    // print the schema and exit
    else if (mode == std::string("schema")) {
      std::cout << treeWalker->avroSchema() << std::endl;
      return 0;
    }
#endif // AVRO

    // dump data for ScaROOT-Reader
    else if (mode == std::string("dump")) {
      if (start != NA  &&  start > currentEntry) {
        treeWalker->setEntryInCurrentTree(start - currentEntry);
        currentEntry = start;
      }
      else
      treeWalker->setEntryInCurrentTree(0);

      do {
        if (end != NA  &&  currentEntry >= end) {
          int64_t endMarker = -1;
          fwrite(&endMarker, sizeof(endMarker), 1, stdout);
          return 0;
        }

        treeWalker->dumpRaw(currentEntry);

        currentEntry += 1;
      } while (treeWalker->next());
    }

    // print the ROOT representation and exit
    else if (mode == std::string("repr")) {
      std::cout << treeWalker->repr() << std::endl;
      return 0;
    }

    else {
      std::cerr << "Unrecognized mode: " << mode << std::endl;
      return -1;
    }
  }

  if (mode == std::string("dump")) {
    int64_t endMarker = -1;
    fwrite(&endMarker, sizeof(endMarker), 1, stdout);
  }

#ifdef AVRO
  treeWalker->closeAvro();
#endif
  return 0;
}
