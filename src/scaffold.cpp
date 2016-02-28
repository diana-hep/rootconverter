#include "scaffold.h"

namespace scaffold {

  std::string rootDummy(std::string fieldName) {
    return std::string("root_") + fieldName;
  }

  std::string indentation(int indent) {
    std::string out;
    for (int i = 0;  i < indent;  i++)
      out += std::string(" ");
    return out;
  }

  std::string innerPrintJSON(std::string item, Def *def, std::string innerType) {
    if (def != nullptr)
      return item + std::string(".printJSON()");
    else if (innerType == std::string("bool")  ||  innerType == std::string("Bool_t"))
      return std::string("b(") + item + std::string(")");
    else if (innerType == std::string("char")  ||  innerType == std::string("Char_t"))
      return std::string("d(") + item + std::string(")");   // Char_t -> integer for our purposes
    else if (innerType == std::string("short")  ||  innerType == std::string("Short_t"))
      return std::string("hd(") + item + std::string(")");
    else if (innerType == std::string("int")  ||  innerType == std::string("Int_t"))
      return std::string("d(") + item + std::string(")");
    else if (innerType == std::string("long")  ||  innerType == std::string("Long_t")  ||  innerType == std::string("Long64_t"))
      return std::string("ld(") + item + std::string(")");
    else if (innerType == std::string("float")  ||  innerType == std::string("Float_t")  ||  innerType == std::string("Float16_t"))
      return std::string("fg(") + item + std::string(")");
    else if (innerType == std::string("double")  ||  innerType == std::string("Double_t")  ||  innerType == std::string("Double32_t"))
      return std::string("g(") + item + std::string(")");
    else if (innerType == std::string("unsigned char")  ||  innerType == std::string("UChar_t"))
      return std::string("u(") + item + std::string(")");   // Char_t -> integer for our purposes
    else if (innerType == std::string("unsigned short")  ||  innerType == std::string("UShort_t"))
      return std::string("hu(") + item + std::string(")");
    else if (innerType == std::string("unsigned int")  ||  innerType == std::string("UInt_t"))
      return std::string("u(") + item + std::string(")");
    else if (innerType == std::string("unsigned long")  ||  innerType == std::string("ULong_t")  ||  innerType == std::string("ULong64_t"))
      return std::string("lu(") + item + std::string(")");
    else 
      throw std::invalid_argument(std::string("unrecognized type (A): ") + innerType);
  }

  std::string unrollTemplatesPrintJSON(int indent, std::string item, std::string dummy, std::vector<Template> templates, Def *def, std::string innerType) {
    std::string out;
    for (int i = 0;  i < templates.size();  i++)
      if (templates[i] == vectorOf) {
        std::string len = std::string("len") + std::to_string(i) + std::string("_") + dummy;
        std::string index = std::string("i") + std::to_string(i);
        std::string newitem = std::string("item") + std::to_string(i) + std::string("_") + dummy;
        out += indentation(indent + 2*i) + std::string("int ") + len + std::string(" = ") + item + std::string(".size();\n") +
          indentation(indent + 2*i) + std::string("for (int ") + index + std::string(" = 0;  ") + index + std::string(" < ") + len + std::string(";  ") + index + std::string("++) {\n") +
          indentation(indent + 2*i) + std::string("  auto ") + newitem + std::string(" = ") + item + std::string("[") + index + std::string("];\n") +
          indentation(indent + 2*i) + std::string("  if (") + index + std::string(" > 0) s(\", \");\n");
        if (i < templates.size() - 1)
          out += indentation(indent + 2*i) + std::string("  s(\"[\");\n");
        item = newitem;
      }
    out += indentation(indent + 2*templates.size()) + innerPrintJSON(item, def, innerType) + std::string(";\n");
    for (int i = templates.size() - 1;  i >= 0;  i--)
      if (templates[i] == vectorOf) {
        if (i < templates.size() - 1)
          out += indentation(indent + 2*i) + std::string("  s(\"]\");\n");
        out += indentation(indent + 2*i) + std::string("}\n");
      }
    return out;
  }

  std::string innerSchema(int indent, std::string ns, Def *def, std::string innerType) {
    if (def != nullptr)
      return def->schema(indent + 2, ns);
    else if (innerType == std::string("bool")  ||  innerType == std::string("Bool_t"))
      return std::string("\"boolean\"");
    else if (innerType == std::string("char")  ||  innerType == std::string("Char_t"))
      return std::string("\"int\"");
    else if (innerType == std::string("short")  ||  innerType == std::string("Short_t"))
      return std::string("\"int\"");
    else if (innerType == std::string("int")  ||  innerType == std::string("Int_t"))
      return std::string("\"int\"");
    else if (innerType == std::string("long")  ||  innerType == std::string("Long_t")  ||  innerType == std::string("Long64_t"))
      return std::string("\"long\"");
    else if (innerType == std::string("float")  ||  innerType == std::string("Float_t")  ||  innerType == std::string("Float16_t"))
      return std::string("\"float\"");
    else if (innerType == std::string("double")  ||  innerType == std::string("Double_t")  ||  innerType == std::string("Double32_t"))
      return std::string("\"double\"");
    else if (innerType == std::string("unsigned char")  ||  innerType == std::string("UChar_t"))
      return std::string("\"int\"");
    else if (innerType == std::string("unsigned short")  ||  innerType == std::string("UShort_t"))
      return std::string("\"int\"");
    else if (innerType == std::string("unsigned int")  ||  innerType == std::string("UInt_t"))
      return std::string("\"long\"");
    else if (innerType == std::string("unsigned long")  ||  innerType == std::string("ULong_t")  ||  innerType == std::string("ULong64_t"))
      return std::string("\"double\"");
    else
      throw std::invalid_argument(std::string("unrecognized type (B): ") + innerType);
  }

  std::string unrollTemplatesSchema(int indent, std::string ns, std::vector<Template> templates, Def *def, std::string innerType) {
    std::string out;
    for (int i = 0;  i < templates.size();  i++)
      if (templates[i] == vectorOf) {
        out += std::string("\n") + indentation(indent + 2*i + 2) + std::string("{\"type\": \"array\", \"items\": ");
      }
    out += innerSchema(indent + 2*templates.size(), ns, def, innerType);
    for (int i = templates.size() - 1;  i >= 0;  i--)
      if (templates[i] == vectorOf) {
        out += std::string("}\n") + indentation(indent + 2*i);
      }
    return out;
  }

  Node **newArray(int size) {
    Node **out = new Node*[size];
    for (int i = 0;  i < size;  i++)
      out[i] = new InertNode;
    return out;
  }

  std::string definitions(std::map<const std::string, Def*> defs) {
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

  std::string declarations(Node **scaffoldArray, int scaffoldSize) {
    std::string out;
    for (int i = 0;  i < scaffoldSize;  i++)
      out += scaffoldArray[i]->declare(2);
    return out;
  }

  std::string init(Node **scaffoldArray, int scaffoldSize) {
    std::string out;
    out += std::string("  void init() {\n");
    for (int i = 0;  i < scaffoldSize;  i++)
      out += scaffoldArray[i]->init(4);
    out += std::string("  }\n");
    return out;
  }

  std::string printJSON(Node **scaffoldArray, int scaffoldSize) {
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

  std::string schema(int indent, std::string name, std::string ns, Node **scaffoldArray, int scaffoldSize) {
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

}
