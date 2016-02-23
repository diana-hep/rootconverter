#include <iostream>
#include <sstream>
// #include <stdlib>
#include <string>
#include <vector>

#include <TFile.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>

#define NA ((uint64_t)(-1))

using namespace ROOT::Internal;   // for TTreeReaderValueBase

void help() {
  std::cout << "Usage: root2avro fileLocation1, [fileLocation2, [...]], treeLocation" << std::endl << std::endl
            << "Where fileLocationN are either file paths or URLs TFile::Open can handle" << std::endl
            << "and treeLocation is the path of a TTree in all the files." << std::endl << std::endl
            << "Avro is streamed to standard output and can be redirected to a file." << std::endl << std::endl
            << "Options:" << std::endl
            << "  --start=NUMBER         First entry number to convert." << std::endl
            << "  --end=NUMBER           Entry number after the last to convert." << std::endl
            << "  --exclude=FIELDS       Comma-delimited field (leaf) names to exclude, if any." << std::endl
            << "  --only=FIELDS          Comma-delimited field (leaf) names to restrict to, if provided." << std::endl
            << "  -h, -help, --help      Print this message and exit." << std::endl;
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
  std::vector<std::string> fileLocations;
  std::string treeLocation;
  uint64_t start = NA;
  uint64_t end = NA;
  std::vector<std::string> exclude;
  std::vector<std::string> only;

  // no, I don't care about the inefficiency of creating and recreating strings; this is not an important loop
  for (int i = 0;  i < argc;  i++) {
    if (std::string(argv[i]) == std::string("-h")  ||
        std::string(argv[i]) == std::string("-help")  ||
        std::string(argv[i]) == std::string("--help")) {
      help();
      return 0;
    }
  }

  std::string startPrefix("--start=");
  std::string endPrefix("--end=");
  std::string excludePrefix("--exclude=");
  std::string onlyPrefix("--only=");
  std::string badPrefix("-");

  // nor is this an important loop
  for (int i = 0;  i < argc;  i++) {
    std::string arg(argv[i]);

    if (arg.substr(0, startPrefix.size()) == startPrefix) {
      std::string value = arg.substr(startPrefix.size(), arg.size());
      start = strtoul(value.c_str(), nullptr, 10);
    }

    else if (arg.substr(0, endPrefix.size()) == endPrefix) {
      std::string value = arg.substr(endPrefix.size(), arg.size());
      end = strtoul(value.c_str(), nullptr, 10);
    }

    else if (arg.substr(0, excludePrefix.size()) == excludePrefix)
      exclude = splitByComma(arg.substr(excludePrefix.size(), arg.size()));

    else if (arg.substr(0, onlyPrefix.size()) == onlyPrefix)
      only = splitByComma(arg.substr(onlyPrefix.size(), arg.size()));

    else if (arg.substr(0, badPrefix.size()) == badPrefix) {
      std::cerr << "Only recognized switches are --start, --end, --exclude, and --only." << std::endl;
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

  std::cout << "HERE" << std::endl;
  for (int i = 0;  i < fileLocations.size();  i++)
    std::cout << "fileLocations " << fileLocations[i] << std::endl;
  std::cout << "treeLocation " << treeLocation << std::endl;
  std::cout << "start " << start << " NA is " << NA << std::endl;
  std::cout << "end " << end << " NA is " << NA << std::endl;
  for (int i = 0;  i < exclude.size();  i++)
    std::cout << "exclude " << exclude[i] << std::endl;
  for (int i = 0;  i < only.size();  i++)
    std::cout << "only " << only[i] << std::endl;

  return 0;
}




// int64_t newFile(const char *fileLocation) {
//   return (int64_t)TFile::Open(fileLocation);
// }

// int64_t newReader(int64_t file, const char *treeLocation) {
//   TFile *p_file = (TFile*)file;
//   return (int64_t)(new TTreeReader(treeLocation, p_file));
// }

// bool readerNext(int64_t reader) {
//   TTreeReader *p_reader = (TTreeReader*)reader;
//   return p_reader->Next();
// }

// int64_t newValue_float(int64_t reader, const char *name) {
//   TTreeReader *p_reader = (TTreeReader*)reader;
//   return (int64_t)(new TTreeReaderValue<float>(*p_reader, name));
// }

// float getValue_float(int64_t value) {
//   TTreeReaderValueBase *p_value = (TTreeReaderValueBase*)value;
//   return *((float*)p_value->GetAddress());
// }
