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

// #include "TTreeAvroGenerator.h"
// #include <TTreeReaderGenerator.h>

#include "TVirtualStreamerInfo.h"
#include "TObjArray.h"
#include "TClonesArray.h"
#include "TBranch.h"
#include "TBranchElement.h"
#include "TStreamerElement.h"

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

  // std::string empty;
  // TTreeAvroGenerator *generator = new TTreeAvroGenerator(tree, empty.c_str());

  analyzeTree(tree);



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

void indent(int level) {
  for (int i = 0;  i < level;  i++)
    std::cout << "    ";
}

void analyzeTree(TTree *tree) {
  std::cout << "analyzeTree " << tree->GetName() << std::endl;

  TIter next(tree->GetListOfBranches());
  for (TBranch *branch = (TBranch*)next();  branch != nullptr;  branch = (TBranch*)next()) {
    const char *branchname = branch->GetName();
    const char *classname = branch->GetClassName();
    TClass *cl = TClass::GetClass(classname);

    std::cout << "top-level branch " << branchname << " " << classname << std::endl;

    TVirtualStreamerInfo *info = nullptr;

    if (cl != nullptr) {
      if (cl == TClonesArray::Class()) {
        // ... (no analyze)
      }
      else if (cl->GetCollectionProxy() != nullptr) {
        // ... (no analyze)
      }

      TBranchElement *be = dynamic_cast<TBranchElement*>(branch);
      info = (TVirtualStreamerInfo*)be->GetInfo();
    }

    if (branch->GetListOfBranches()->GetEntries() == 0) {
      if (cl != nullptr) {
        std::cout << "top-level NON-split" << std::endl;
      }
      else {
        std::cout << "top-level raw type (AnalyzeOldBranch)" << std::endl;
      }
    }
    else {
      std::cout << "top-level split by " << branch->GetListOfBranches()->GetEntries() << std::endl;

      analyzeBranches(branch->GetListOfBranches(), info, 1);
    }
  }
}

void analyzeBranches(TObjArray *branches, TVirtualStreamerInfo *info, int level) {
  indent(level); std::cout << "analyzeBranches" << std::endl;

  TIter elements(info->GetElements());
  for (TStreamerElement *element = (TStreamerElement*)elements();  element != nullptr;  element = (TStreamerElement*)elements()) {
    if (info->GetClass()->GetCollectionProxy() != nullptr  &&  std::string(element->GetName()) == std::string("This"))
      continue;  // skip artificial streamer element
    if (element->GetType() == -1)
      continue;  // skip ignored TObject base class

    TBranchElement *branch = (TBranchElement*)branches->At(0);

    const char *branchname(branch->GetName());

    bool ispointer = false;
    switch(element->GetType()) {
    case TVirtualStreamerInfo::kBool:     indent(level); std::cout << element->GetName() << ":\t\t" << "bool" << std::endl; break;
    case TVirtualStreamerInfo::kChar:     indent(level); std::cout << element->GetName() << ":\t\t" << "char" << std::endl; break;
    case TVirtualStreamerInfo::kShort:    indent(level); std::cout << element->GetName() << ":\t\t" << "short" << std::endl; break;
    case TVirtualStreamerInfo::kInt:      indent(level); std::cout << element->GetName() << ":\t\t" << "int" << std::endl; break;
    case TVirtualStreamerInfo::kLong:     indent(level); std::cout << element->GetName() << ":\t\t" << "long" << std::endl; break;
    case TVirtualStreamerInfo::kLong64:   indent(level); std::cout << element->GetName() << ":\t\t" << "long64" << std::endl; break;
    case TVirtualStreamerInfo::kFloat:    indent(level); std::cout << element->GetName() << ":\t\t" << "float" << std::endl; break;
    case TVirtualStreamerInfo::kFloat16:  indent(level); std::cout << element->GetName() << ":\t\t" << "float16" << std::endl; break;
    case TVirtualStreamerInfo::kDouble:   indent(level); std::cout << element->GetName() << ":\t\t" << "double" << std::endl; break;
    case TVirtualStreamerInfo::kDouble32: indent(level); std::cout << element->GetName() << ":\t\t" << "double32" << std::endl; break;
    case TVirtualStreamerInfo::kUChar:    indent(level); std::cout << element->GetName() << ":\t\t" << "uchar" << std::endl; break;
    case TVirtualStreamerInfo::kUShort:   indent(level); std::cout << element->GetName() << ":\t\t" << "ushort" << std::endl; break;
    case TVirtualStreamerInfo::kUInt:     indent(level); std::cout << element->GetName() << ":\t\t" << "uint" << std::endl; break;
    case TVirtualStreamerInfo::kULong:    indent(level); std::cout << element->GetName() << ":\t\t" << "ulong" << std::endl; break;
    case TVirtualStreamerInfo::kULong64:  indent(level); std::cout << element->GetName() << ":\t\t" << "ulong64" << std::endl; break;
    case TVirtualStreamerInfo::kBits:     indent(level); std::cout << element->GetName() << ":\t\t" << "bits (uint)" << std::endl; break;

    case TVirtualStreamerInfo::kCharStar: indent(level); std::cout << element->GetName() << ":\t\t" << "char* (string)" << std::endl; break;

      // array of basic types  array[8]
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kBool:     indent(level); std::cout << element->GetName() << ":\t\t" << "bool[]" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kChar:     indent(level); std::cout << element->GetName() << ":\t\t" << "char[]" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kShort:    indent(level); std::cout << element->GetName() << ":\t\t" << "short[]" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kInt:      indent(level); std::cout << element->GetName() << ":\t\t" << "int[]" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kLong:     indent(level); std::cout << element->GetName() << ":\t\t" << "long[]" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kLong64:   indent(level); std::cout << element->GetName() << ":\t\t" << "long64[]" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kFloat:    indent(level); std::cout << element->GetName() << ":\t\t" << "float[]" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kFloat16:  indent(level); std::cout << element->GetName() << ":\t\t" << "float16[]" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kDouble:   indent(level); std::cout << element->GetName() << ":\t\t" << "double[]" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kDouble32: indent(level); std::cout << element->GetName() << ":\t\t" << "double32[]" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kUChar:    indent(level); std::cout << element->GetName() << ":\t\t" << "uchar[]" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kUShort:   indent(level); std::cout << element->GetName() << ":\t\t" << "ushort[]" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kUInt:     indent(level); std::cout << element->GetName() << ":\t\t" << "uint[]" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kULong:    indent(level); std::cout << element->GetName() << ":\t\t" << "ulong[]" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kULong64:  indent(level); std::cout << element->GetName() << ":\t\t" << "ulong64[]" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kBits:     indent(level); std::cout << element->GetName() << ":\t\t" << "uint[]" << std::endl; break;

      // pointer to an array of basic types  array[n]
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kBool:     indent(level); std::cout << element->GetName() << ":\t\t" << "bool*" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kChar:     indent(level); std::cout << element->GetName() << ":\t\t" << "char*" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kShort:    indent(level); std::cout << element->GetName() << ":\t\t" << "short*" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kInt:      indent(level); std::cout << element->GetName() << ":\t\t" << "int*" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kLong:     indent(level); std::cout << element->GetName() << ":\t\t" << "long*" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kLong64:   indent(level); std::cout << element->GetName() << ":\t\t" << "long64*" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kFloat:    indent(level); std::cout << element->GetName() << ":\t\t" << "float*" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kFloat16:  indent(level); std::cout << element->GetName() << ":\t\t" << "float16*" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kDouble:   indent(level); std::cout << element->GetName() << ":\t\t" << "double*" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kDouble32: indent(level); std::cout << element->GetName() << ":\t\t" << "double32*" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kUChar:    indent(level); std::cout << element->GetName() << ":\t\t" << "uchar*" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kUShort:   indent(level); std::cout << element->GetName() << ":\t\t" << "ushort*" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kUInt:     indent(level); std::cout << element->GetName() << ":\t\t" << "uint*" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kULong:    indent(level); std::cout << element->GetName() << ":\t\t" << "ulong*" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kULong64:  indent(level); std::cout << element->GetName() << ":\t\t" << "ulong64*" << std::endl; break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kBits:     indent(level); std::cout << element->GetName() << ":\t\t" << "uint*" << std::endl; break;

      // array counter //[n]
    case TVirtualStreamerInfo::kCounter: indent(level); std::cout << element->GetName() << ":\t\t" << "int (counter)" << std::endl; break;

    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kObjectp:
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kObjectP:
    case TVirtualStreamerInfo::kObjectp:
    case TVirtualStreamerInfo::kObjectP:
    case TVirtualStreamerInfo::kAnyp:
    case TVirtualStreamerInfo::kAnyP:
    case TVirtualStreamerInfo::kSTL + TVirtualStreamerInfo::kObjectp:
    case TVirtualStreamerInfo::kSTL + TVirtualStreamerInfo::kObjectP:
      // set as pointers and fall through to the next switches
      ispointer = true;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kObject:
    case TVirtualStreamerInfo::kObject:
    case TVirtualStreamerInfo::kTString:
    case TVirtualStreamerInfo::kTNamed:
    case TVirtualStreamerInfo::kTObject:
    case TVirtualStreamerInfo::kAny:
    case TVirtualStreamerInfo::kBase:
    case TVirtualStreamerInfo::kSTL: {
      TClass *cl = element->GetClassPointer();

      indent(level); std::cout << element->GetName() << ":\t\t" << cl->GetName() << (ispointer ? "*" : "") << (element->IsBase() ? " (base)" : "") << std::endl;

      if (!element->IsBase()) {
        if (branchEndname == element->GetName()) {
          if (branch->GetListOfBranches()->GetEntries() == 0) {
            TVirtualStreamerInfo *binfo = branch->GetInfo();

            TIter next(binfo->GetElements());
            for (TStreamerElement *elem = (TStreamerElement*)next();  elem != nullptr;  elem = (TStreamerElement*)next())
              AnalyzeElement(branch, elem, level + 1);
          }
          else {
            TVirtualStreamerInfo *objInfo = GetStreamerInfo(branch, branch->GetListOfBranches(), cl);
            analyzeBranches(branches->GetListOfBranches(), objInfo, level + 1);
          }
        }
        else {
          TVirtualStreamerInfo *objInfo = branch->GetInfo();


        }

      }

      break;
    }

    default: indent(level); std::cout << element->GetName() << ":\t\t" << "BAD BAD BAD" << std::endl;
    }
  }

}

void analyzeElement(TBranch *branch, TStreamerElement *element, int level) {
  indent(level); std::cout << "analyzeElement " << branch->GetName() << std::endl;

}
