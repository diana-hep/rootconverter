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
  out += std::string("TTreeReader *getReader();");
  for (int i = 0;  i < scaffoldSize;  i++)
    out += scaffoldArray[i]->declare(0);
  return out;
}

std::string scaffold::init(scaffold::Node **scaffoldArray, int scaffoldSize) {
  std::string out;
  out += std::string("void init() {\n");
  for (int i = 0;  i < scaffoldSize;  i++)
    out += scaffoldArray[i]->init(2);
  out += std::string("}");
  return out;
}

std::string scaffold::printJSON(scaffold::Node **scaffoldArray, int scaffoldSize) {
  std::string out;
  out += std::string("void printJSON() {\n");
  out += std::string("  while (getReader()->Next()) {\n");
  out += std::string("    std::cout << \"{\";\n");
  for (int i = 0;  i < scaffoldSize;  i++) {
    if (i > 0)
      out += std::string("    std::cout << \", \";\n");
    out += std::string("\n") + scaffoldArray[i]->printJSON(4);
  }
  out += std::string("    std::cout << \"}\" << std::endl;\n");
  out += std::string("  }\n}\n");
  return out;
}

std::string quoteJSON(std::string string) {
  return string;   // TODO! Escape bad characters, especially '\n'
}
