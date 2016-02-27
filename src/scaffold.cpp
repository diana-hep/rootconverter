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

std::string scaffold::init(scaffold::Node **scaffoldArray, int scaffoldSize) {
  std::string out;
  out += std::string("  void init() {\n");
  for (int i = 0;  i < scaffoldSize;  i++)
    out += scaffoldArray[i]->init(4);
  out += std::string("  }\n");
  return out;
}

std::string scaffold::printJSON(scaffold::Node **scaffoldArray, int scaffoldSize) {
  std::string out;
  out += std::string("  std::string escapeJSON(std::string string) {\n");
  out += std::string("    return string;\n");   // TODO! Escape bad characters, especially '\n'
  out += std::string("  }\n");
  out += std::string("\n");
  out += std::string("  void s(const char *x) { fputs(x, stdout); }\n");
  out += std::string("  void c(char x) { printf(\"%c\", x); }\n");
  out += std::string("  void b(char x) { if (x == 0) fputs(\"false\", stdout); else fputs(\"true\", stdout); }\n");
  out += std::string("  void hd(short x) { printf(\"%hd\", x); }\n");
  out += std::string("  void hu(unsigned short x) { printf(\"%hu\", x); }\n");
  out += std::string("  void d(int x) { printf(\"%d\", x); }\n");
  out += std::string("  void u(unsigned int x) { printf(\"%u\", x); }\n");
  out += std::string("  void ld(long x) { printf(\"%ld\", x); }\n");
  out += std::string("  void lu(unsigned long x) { printf(\"%lu\", x); }\n");
  out += std::string("  void fg(float x) { printf(\"%.9g\", x); }\n");
  out += std::string("  void g(double x) { printf(\"%.17g\", x); }\n\n");
  out += std::string("\n");
  out += std::string("  void run() {\n");
  out += std::string("    init();\n");
  out += std::string("    while (getReader()->Next()) {\n");
  out += std::string("      s(\"{\");\n");
  for (int i = 0;  i < scaffoldSize;  i++) {
    if (i > 0)
      out += std::string("      s(\", \");\n");
    out += std::string("\n") + scaffoldArray[i]->printJSON(6);
  }
  out += std::string("      s(\"}\\n\");\n");
  out += std::string("    }\n  }\n");
  return out;
}

std::string indentation(int indent) {
  std::string out;
  for (int i = 0;  i < indent;  i++)
    out += std::string(" ");
  return out;
}

std::string scaffold::schema(int indent, std::string name, std::string ns, scaffold::Node **scaffoldArray, int scaffoldSize) {
  std::string out;
  out += indentation(indent) + std::string("{\"type\": \"record\",\n");
  out += indentation(indent) + std::string(" \"name\": \"") + name + std::string("\",\n");
  if (ns.size() > 0)
    out += indentation(indent) + std::string(" \"namespace\": \"") + ns + std::string("\",\n");
  out += indentation(indent) + std::string(" \"fields\": [\n");
  for (int i = 0;  i < scaffoldSize;  i++) {
    out += scaffoldArray[i]->schema(3, ns);
    if (i < scaffoldSize - 1)
      out += std::string(",\n");
    else
      out += std::string("\n ]\n");
  }
  out += indentation(indent) + std::string("}\n");
  return out;
}
