#include "scaffold.h"

std::string scaffold::rootDummy(std::string fieldName) {
  return std::string("root_") + fieldName;
}

scaffold::Node **scaffold::newArray(int size) {
  scaffold::Node **out = new scaffold::Node*[size];
  for (int i = 0;  i < size;  i++)
    out[i] = new scaffold::InertNode;
  return out;
}

std::string scaffold::definitions(std::map<const std::string, scaffold::Def*> defs) {
  std::string out;
  for (auto pair = defs.begin();  pair != defs.end();  ++pair) {
    out += pair->second->forward();
  }
  for (auto pair = defs.begin();  pair != defs.end();  ++pair) {
    out += std::string("\n");
    out += pair->second->def();
  }
  return out;
}

std::string scaffold::declarations(scaffold::Node **scaffoldArray, int scaffoldSize) {
  std::string out;
  for (int i = 0;  i < scaffoldSize;  i++)
    out += scaffoldArray[i]->declare(2);
  return out;
}

std::string scaffold::init(scaffold::Node **scaffoldArray, int scaffoldSize, std::vector<std::string> fileLocations, std::string treeLocation) {
  std::string out;
  out += std::string("  TFile *file = TFile::Open(\"") + fileLocations[0] + std::string("\");\n");
  out += std::string("  TTreeReader reader(\"") + treeLocation + std::string("\", file);\n");
  for (int i = 0;  i < scaffoldSize;  i++)
    out += scaffoldArray[i]->init(2);
  return out;
}

std::string scaffold::loop(scaffold::Node **scaffoldArray, int scaffoldSize) {
  std::string out;
  out += std::string("  while (reader.Next()) {\n");
  out += std::string("    std::cout << \"start entry\" << std::endl;\n");
  for (int i = 0;  i < scaffoldSize;  i++)
    out += std::string("\n") + scaffoldArray[i]->loop(4);
  out += std::string("  }\n");
  return out;
}
