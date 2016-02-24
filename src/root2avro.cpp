#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// #include <TROOT.h>
// #include <TInterpreter.h>
#include <TFile.h>
#include <TTree.h>
// #include <TTreeReader.h>
// #include <TTreeReaderValue.h>
// #include <TTreeReaderArray.h>

#include "TTreeAvroGenerator.h"

#include <avro.h>

#define NA ((uint64_t)(-1))

using namespace ROOT::Internal;

// global variables for this tiny, single-threaded program (parallelism comes from multiple processes)
std::vector<std::string> fileLocations;
std::string              treeLocation;
uint64_t                 start = NA;
uint64_t                 end = NA;
// std::vector<std::string> exclude;     // replace these with some ROOT selector string, if possible
// std::vector<std::string> only;
bool                     schemaOnly = false;
std::string              codec("null");

avro_schema_t            schema;
avro_schema_error_t      schemaError;
bool                     schemaDefined = false;
avro_file_writer_t       avroWriter;
avro_value_iface_t       *avroInterface;
avro_value_t             avroValue;

void help() {
  std::cerr << "Usage: root2avro fileLocation1, [fileLocation2, [...]], treeLocation" << std::endl << std::endl
            << "Where fileLocationN are either file paths or URLs TFile::Open can handle and treeLocation is the path of the" << std::endl
            << "TTree in all the files." << std::endl << std::endl
            << "Avro is streamed to standard output and can be redirected to a file." << std::endl << std::endl
            << "Options:" << std::endl
            << "  --start=NUMBER         First entry number to convert." << std::endl
            << "  --end=NUMBER           Entry number after the last to convert." << std::endl
            // << "  --exclude=FIELDS       Comma-delimited field (leaf) names to exclude, if any." << std::endl
            // << "  --only=FIELDS          Comma-delimited field (leaf) names to restrict to, if provided." << std::endl
            << "  --schema               Output the schema only (JSON), rather than data (Avro)." << std::endl
            << "  --codec=CODEC          Codec for compressing the Avro output; may be \"null\" (uncompressed, default)," << std::endl
            << "                         \"deflate\", \"snappy\", \"lzma\", depending on libraries installed on your system." << std::endl
            << "  -h, -help, --help      Print this message and exit." << std::endl;
}

// std::vector<std::string> splitByComma(std::string in) {
//   std::vector<std::string> out;
//   std::stringstream ss(in);
//   std::string item;
//   while (std::getline(ss, item, ','))
//     out.push_back(item);
//   return out;
// }

void analyzeTree(TTree *tree);
void analyzeBranches(TObjArray *branches, TVirtualStreamerInfo *info, int level);
void analyzeElement(TBranch *branch, TStreamerElement *element, int level);

int main(int argc, char **argv) {
  // no, I don't care about the inefficiency of creating and recreating strings; this is not an important loop
  for (int i = 1;  i < argc;  i++) {
    if (std::string(argv[i]) == std::string("-h")  ||
        std::string(argv[i]) == std::string("-help")  ||
        std::string(argv[i]) == std::string("--help")) {
      help();
      return 0;
    }
  }

  std::string startPrefix("--start=");
  std::string endPrefix("--end=");
  // std::string excludePrefix("--exclude=");
  // std::string onlyPrefix("--only=");
  std::string codecPrefix("--codec=");
  std::string badPrefix("-");

  // nor is this an important loop
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

    // else if (arg.substr(0, excludePrefix.size()) == excludePrefix)
    //   exclude = splitByComma(arg.substr(excludePrefix.size(), arg.size()));

    // else if (arg.substr(0, onlyPrefix.size()) == onlyPrefix)
    //   only = splitByComma(arg.substr(onlyPrefix.size(), arg.size()));

    else if (arg == std::string("--schema"))
      schemaOnly = true;

    else if (arg.substr(0, codecPrefix.size()) == codecPrefix)
      codec = arg.substr(codecPrefix.size(), arg.size());

    else if (arg.substr(0, badPrefix.size()) == badPrefix) {
      std::cerr << "Recognized switches are: --start, --end, --schema, --codec." << std::endl;
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

  if (start != NA  &&  end != NA  &&  start >= end) {
    std::cerr << "Start must be less than end (if provided)." << std::endl;
    return -1;
  }

  TFile *file = TFile::Open(fileLocations[0].c_str());
  TTree *tree;
  file->GetObject(treeLocation.c_str(), tree);

  TTreeAvroGenerator *generator = new TTreeAvroGenerator(tree);

  std::cout << std::endl << "HEADER:" << std::endl;
  std::cout << generator->header();

  std::cout << std::endl << "INIT:" << std::endl;
  std::cout << generator->init();


  // std::string schemastr("{\"type\": \"record\", \"name\": \"Stuff\", \"fields\": [{\"name\": \"one\", \"type\": \"int\"}, {\"name\": \"two\", \"type\": \"double\"}, {\"name\": \"three\", \"type\": \"string\"}]}");

  // if (avro_schema_from_json(schemastr.c_str(), schemastr.size(), &schema, &schemaError) != 0) {
  //   std::cerr << avro_strerror() << std::endl;
  //   return -1;
  // }
  // schemaDefined = true;

  // avroInterface = avro_generic_class_from_schema(schema);
  // if (avroInterface == nullptr) {
  //   std::cerr << avro_strerror() << std::endl;
  //   return -1;
  // }

  // if (avro_generic_value_new(avroInterface, &avroValue) != 0) {
  //   std::cerr << avro_strerror() << std::endl;
  //   return -1;
  // }

  // avro_value_t one;
  // avro_value_t two;
  // avro_value_t three;

  // if (avro_value_get_by_name(&avroValue, "one", &one, nullptr) != 0) {
  //   std::cerr << avro_strerror() << std::endl;
  //   return -1;
  // }

  // if (avro_value_get_by_name(&avroValue, "two", &two, nullptr) != 0) {
  //   std::cerr << avro_strerror() << std::endl;
  //   return -1;
  // }

  // if (avro_value_get_by_name(&avroValue, "three", &three, nullptr) != 0) {
  //   std::cerr << avro_strerror() << std::endl;
  //   return -1;
  // }

  // std::string path;
  // if (avro_file_writer_create_with_codec_fp(stdout, path.c_str(), true, schema, &avroWriter, codec.c_str(), 0) != 0) {
  //   std::cerr << avro_strerror() << std::endl;
  //   return -1;
  // }

  // for (int i = 0;  i < 10;  i++) {
  //   avro_value_set_int(&one, i);
  //   avro_value_set_double(&two, i + i/10.0);
  //   avro_value_set_string(&three, std::to_string(i).c_str());
  //   avro_file_writer_append_value(avroWriter, &avroValue);
  // }

  // avro_file_writer_close(avroWriter);




  
  // uint64_t entry = 0;
  // for (int i = 0;  i < fileLocations.size();  i++) {
  //   std::string message = convertFile(fileLocations[i], entry);
  //   if (message != std::string("")) {
  //     std::cerr << message << std::endl;
  //     return -1;
  //   }
  // }

  return 0;
}
