#ifndef SCAFFOLD_H
#define SCAFFOLD_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace scaffold {
  enum Kind { scalar, array, vector, structure };

  class Type {
    std::string type_;
    Kind kind_;
    std::vector<int> dims_;
  public:
    Type(std::string type, Kind kind) {
      if (kind == array) {
        int i;
        for (i = 0;  i < type.size()  &&  type[i] != '[';  i++)
          type_ += type[i];
        for (;  i < type.size();  i++) {
          if (type[i] == '[')
            dims_.push_back(0);
          else if ('0' <= type[i]  &&  type[i] <= '9'  &&  dims_.size() > 0)
            dims_.back() = 10 * dims_.back() + ((int)type[i] - (int)'0');
        }
      }
      else
        type_ = type;
      kind_ = kind;
    }
    std::string typeName() { return type_; }
    std::string arrayBrackets() {
      std::string out;
      for (int i = 0;  i < dims_.size();  i++)
        out += std::string("[") + std::to_string(i) + std::string("]");
      return out;
    }
    std::string printJSON(int indent) { return std::string(); }
    // std::string loop(std::string name) {
    //   if (kind_ == scalar)
    //     return name + std::string(" << std::endl;\n");
    //   else if (kind_ == array  ||  kind_ == vector) {
    //     std::string out;
    //     for (int n = 0;  n < dims_.size();  n++) {
    //       std::string var = std::string("i") + std::to_string(n);
    //       for (int i = 0;  i < n;  i++)
    //         out += std::string("  ");
    //       out += std::string("for (int ") + var + std::string(" = 0;  ") + var + std::string(" < ") + std::to_string(dims_[n]) + std::string(";  ") + var + std::string("++)\n");
    //     }
    //     for (int i = 0;  i < dims_.size();  i++)
    //       out += std::string("  ");
    //     out += std::string("std::cout << \" \" << ") + name;
    //     for (int n = 0;  n < dims_.size();  n++) {
    //       std::string var = std::string("i") + std::to_string(n);
    //       out += std::string("[") + var + std::string("]");
    //     }
    //     out += std::string(";\n");
    //     out += std::string("  std::cout << std::endl;\n");
    //   }
    //   else if (kind_ == structure) {
    //   }
    // }
  };

  class Def {
    std::string typeName_;
    std::vector<std::string> bases_;
    std::vector<std::string> names_;
    std::vector<Type> types_;
  public:
    Def(std::string typeName) : typeName_(typeName) { }
    std::string typeName() { return typeName_; }
    std::string name(int i) { return names_[i]; }
    Type type(int i) { return types_[i]; }
    void addBase(std::string base) {
      bases_.push_back(base);
    }
    void addField(Type type, std::string name) {
      names_.push_back(name);
      types_.push_back(type);
    }
    bool empty() {
      return (bases_.size() == 0  &&  types_.size() == 0);
    }
    std::string forward() {
      if (empty())
        return std::string();
      else
        return std::string("class ") + typeName_ + std::string(";\n");
    }
    std::string def() {
      if (empty())
        return std::string();
      else {
        std::string out = std::string("class ") + typeName_;
        for (int i = 0;  i < bases_.size();  i++) {
          if (i == 0)
            out += std::string(" : ");
          else
            out += std::string(", ");
          out += std::string("public ") + bases_[i];
        }
        out += std::string(" {\n");
        for (int i = 0;  i < names_.size();  i++) {
          out += std::string("  ") + type(i).typeName() + std::string(" ") + name(i) + type(i).arrayBrackets() + std::string(";\n");
        }
        out += std::string("  void printJSON() {\n");
        for (int i = 0;  i < names_.size();  i++) {
          // out += std::string("    std::cout << \"") + name(i) + std::string(" \";\n") + type(i).loop(name(i));
        }
        out += std::string("  }\n};\n");
        return out;
      }
    }
    std::string ref() { return typeName_; }
    std::string schema(int indent, std::string ns) {
      return std::string();
    }
  };

  std::string rootDummy(std::string fieldName);

  class Node {
  public:
    virtual std::string declare(int indent) = 0;
    virtual std::string init(int indent) = 0;
    virtual std::string printJSON(int indent) = 0;
    virtual std::string schema(int indent, std::string ns) = 0;
  protected:
    std::string indentation(int indent) {
      std::string out;
      for (int i = 0;  i < indent;  i++)
        out += std::string(" ");
      return out;
    }
  };

  class InertNode : public Node {
  public:
    std::string declare(int indent) {
      return std::string("");
    }
    std::string init(int indent) {
      return std::string("");
    }
    std::string printJSON(int indent) {
      return std::string("");
    }
    std::string schema(int indent, std::string ns) {
      return std::string("");
    }
  };

  class ReaderValueNode : public Node {
    std::string type_;
    std::string name_;
    Def *def_;
  public:
    ReaderValueNode(std::string type, std::string name, Def *def) : type_(type), name_(name), def_(def) { }
    std::string declare(int indent) {
      return indentation(indent) + std::string("TTreeReaderValue<") + type_ + std::string(" > *") + rootDummy(name_) + std::string(";\n");
    }
    std::string init(int indent) {
      return indentation(indent) + rootDummy(name_) + " = new " + std::string("TTreeReaderValue<") + type_ + std::string(" >(*getReader(), \"") + name_ + std::string("\");\n");
    }
    std::string printJSON(int indent) {
      std::string out = indentation(indent) + std::string("s(\"\\\"") + name_ + std::string("\\\": \");\n");
      if (def_ != nullptr)
        out += indentation(indent) + rootDummy(name_) + std::string("->Get().printJSON();\n");
      else if (type_ == std::string("Bool_t"))
        out += indentation(indent) + std::string("b(*(") + rootDummy(name_) + std::string("->Get()));\n");
      else if (type_ == std::string("Char_t"))
        out += indentation(indent) + std::string("d(*(") + rootDummy(name_) + std::string("->Get()));\n");   // Char_t -> integer in Avro
      else if (type_ == std::string("Short_t"))
        out += indentation(indent) + std::string("hd(*(") + rootDummy(name_) + std::string("->Get()));\n");
      else if (type_ == std::string("Int_t"))
        out += indentation(indent) + std::string("d(*(") + rootDummy(name_) + std::string("->Get()));\n");
      else if (type_ == std::string("Long_t"))
        out += indentation(indent) + std::string("ld(*(") + rootDummy(name_) + std::string("->Get()));\n");
      else if (type_ == std::string("Long64_t"))
        out += indentation(indent) + std::string("ld(*(") + rootDummy(name_) + std::string("->Get()));\n");
      else if (type_ == std::string("Float_t"))
        out += indentation(indent) + std::string("fg(*(") + rootDummy(name_) + std::string("->Get()));\n");
      else if (type_ == std::string("Float16_t"))
        out += indentation(indent) + std::string("fg(*(") + rootDummy(name_) + std::string("->Get()));\n");
      else if (type_ == std::string("Double_t"))
        out += indentation(indent) + std::string("g(*(") + rootDummy(name_) + std::string("->Get()));\n");
      else if (type_ == std::string("Double32_t"))
        out += indentation(indent) + std::string("g(*(") + rootDummy(name_) + std::string("->Get()));\n");
      else if (type_ == std::string("UChar_t"))
        out += indentation(indent) + std::string("u(*(") + rootDummy(name_) + std::string("->Get()));\n");   // Char_t -> integer in Avro
      else if (type_ == std::string("UShort_t"))
        out += indentation(indent) + std::string("hu(*(") + rootDummy(name_) + std::string("->Get()));\n");
      else if (type_ == std::string("UInt_t"))
        out += indentation(indent) + std::string("u(*(") + rootDummy(name_) + std::string("->Get()));\n");
      else if (type_ == std::string("ULong_t"))
        out += indentation(indent) + std::string("lu(*(") + rootDummy(name_) + std::string("->Get()));\n");
      else if (type_ == std::string("ULong64_t"))
        out += indentation(indent) + std::string("lu(*(") + rootDummy(name_) + std::string("->Get()));\n");
      else
        throw;
      return out;
    }
    std::string schema(int indent, std::string ns) {
      std::string out;
      out += indentation(indent) + std::string("{\"name\": \"") + name_ + std::string("\", \"type\": ");
      if (def_ != nullptr)
        out += def_->schema(indent + 2, ns);
      else if (type_ == std::string("Bool_t"))
        out += std::string("\"boolean\"}");
      else if (type_ == std::string("Char_t"))
        out += std::string("\"int\"}");
      else if (type_ == std::string("Short_t"))
        out += std::string("\"int\"}");
      else if (type_ == std::string("Int_t"))
        out += std::string("\"int\"}");
      else if (type_ == std::string("Long_t"))
        out += std::string("\"long\"}");
      else if (type_ == std::string("Long64_t"))
        out += std::string("\"long\"}");
      else if (type_ == std::string("Float_t"))
        out += std::string("\"float\"}");
      else if (type_ == std::string("Float16_t"))
        out += std::string("\"float\"}");
      else if (type_ == std::string("Double_t"))
        out += std::string("\"double\"}");
      else if (type_ == std::string("Double32_t"))
        out += std::string("\"double\"}");
      else if (type_ == std::string("UChar_t"))
        out += std::string("\"int\"}");
      else if (type_ == std::string("UShort_t"))
        out += std::string("\"int\"}");
      else if (type_ == std::string("UInt_t"))
        out += std::string("\"long\"}");
      else if (type_ == std::string("ULong_t"))
        out += std::string("\"double\"}");
      else if (type_ == std::string("ULong64_t"))
        out += std::string("\"double\"}");
      else
        throw;
      return out;
    }
  };

  class ReaderStringNode : public Node {
    std::string name_;
  public:
    ReaderStringNode(std::string name) : name_(name) { }
    std::string declare(int indent) {
      return indentation(indent) + std::string("TTreeReaderArray<Char_t > *") + rootDummy(name_) + std::string(";\n");
    }
    std::string init(int indent) {
      return indentation(indent) + rootDummy(name_) + " = new " + std::string("TTreeReaderArray<Char_t >(*getReader(), \"") + name_ + std::string("\");\n");
    }
    std::string printJSON(int indent) {
      return indentation(indent) + std::string("s(\"\\\"") + name_ + std::string("\\\": \\\"\"); s(escapeJSON(std::string((char*)") + rootDummy(name_) + std::string("->GetAddress())).c_str()); s(\"\\\"\");\n");
    }
    std::string schema(int indent, std::string ns) {
      return indentation(indent) + std::string("{\"name\": \"") + name_ + std::string("\", \"type\": \"string\"}");
    }
  };

  class ReaderArrayNode : public Node {
    std::string type_;
    std::string name_;
  public:
    ReaderArrayNode(std::string type, std::string name) : type_(type), name_(name) { }
    std::string declare(int indent) {
      return indentation(indent) + std::string("TTreeReaderArray<") + type_ + std::string(" > *") + rootDummy(name_) + std::string(";\n");
    }
    std::string init(int indent) {
      return indentation(indent) + rootDummy(name_) + " = new " + std::string("TTreeReaderArray<") + type_ + std::string(" >(*getReader(), \"") + name_ + std::string("\");\n");
    }
    std::string printJSON(int indent) {
      return indentation(indent) + std::string("std::cout << \"\\\"") + name_ + std::string("\\\": [\";\n") +
             indentation(indent) + std::string("int len_") + rootDummy(name_) + std::string(" = ") + rootDummy(name_) + std::string("->GetSize();\n") +
             indentation(indent) + std::string("for (int i = 0;  i < len_") + rootDummy(name_) + std::string(";  i++) {\n") +
             indentation(indent) + std::string("  if (i != 0) std::cout << \", \";\n");
             indentation(indent) + std::string("  std::cout << (*") + rootDummy(name_) + std::string(")[i];\n") +
             indentation(indent) + std::string("};\n") +
             indentation(indent) + std::string("std::cout << \"]\"\n;");
    }
    std::string schema(int indent, std::string ns) {
      return std::string("");
    }
  };

  class ReaderArrayArrayNode : public Node {
    std::string type_;
    std::string name_;
    int nesting_;
  public:
    ReaderArrayArrayNode(std::string type, std::string name, int nesting) : type_(type), name_(name), nesting_(nesting) { }
    std::string declare(int indent) {
      return indentation(indent) + std::string("TTreeReaderArray<") + type_ + std::string(" > *") + rootDummy(name_) + std::string(";\n");
    }
    std::string init(int indent) {
      return indentation(indent) + rootDummy(name_) + " = new " + std::string("TTreeReaderArray<") + type_ + std::string(" >(*getReader(), \"") + name_ + std::string("\");\n");
    }
    std::string printJSON(int indent) {
      std::string item = std::string("item0_") + rootDummy(name_);
      std::string out = indentation(indent) + std::string("std::cout << \"\\\"") + name_ + std::string("\\\": [\";\n") +
                        indentation(indent) + std::string("int len0_") + rootDummy(name_) + std::string(" = ") + rootDummy(name_) + std::string("->GetSize();\n") +
                        indentation(indent) + std::string("for (int i0 = 0;  i0 < len0_") + rootDummy(name_) + std::string(";  i0++) {\n") +
                        indentation(indent) + std::string("  auto ") + item + std::string(" = ") + rootDummy(name_) + std::string("[i0];\n");
      for (int n = 1;  n <= nesting_;  n++) {
        std::string len = std::string("len") + std::to_string(n) + std::string("_") + rootDummy(name_);
        std::string index = std::string("i") + std::to_string(n);
        std::string newitem = std::string("item") + std::to_string(n) + std::string("_") + rootDummy(name_);
        out += indentation(indent + 2*n) + std::string("int ") + len + std::string(" = ") + item + std::string(".size();\n") +
               indentation(indent + 2*n) + std::string("for (int ") + index + std::string(" = 0;  ") + index + std::string(" < ") + len + std::string("; ") + index + std::string("++) {\n") +
               indentation(indent + 2*n) + std::string("  auto ") + newitem + std::string(" = ") + item + std::string("[") + index + std::string("];\n");
        if (n < nesting_)
          out += indentation(indent + 2*n) + std::string("  if (") + index + std::string(" > 0) std::cout << \", \";\n") +
                 indentation(indent + 2*n) + std::string("  std::cout << \"[\";\n");
        item = newitem;
      }
      out += indentation(indent + 2*nesting_) + std::string("  if (i") + std::to_string(nesting_) + std::string(" != 0) std::cout << \", \";\n") +
             indentation(indent + 2*nesting_) + std::string("  std::cout << ") + item + std::string(";\n");
      for (int n = nesting_;  n >= 0;  n--) {
        if (n < nesting_)
          out += indentation(indent + 2*n) + std::string("  std::cout << \"]\";\n");
        out += indentation(indent + 2*n) + std::string("}\n");
      }
      return out;
    }
    std::string schema(int indent, std::string ns) {
      return std::string("");
    }
  };

  class ReaderNestedArrayNode : public Node {
    std::string type_;
    std::string name_;
    std::vector<int> fixedTail_;   // use this to preserve some nesting
  public:
    ReaderNestedArrayNode(std::string type, std::string name, std::vector<int> fixedTail) : type_(type), name_(name), fixedTail_(fixedTail) { }
    std::string declare(int indent) {
      return indentation(indent) + std::string("TTreeReaderArray<") + type_ + std::string(" > *") + rootDummy(name_) + std::string(";\n");
    }
    std::string init(int indent) {
      return indentation(indent) + rootDummy(name_) + " = new " + std::string("TTreeReaderArray<") + type_ + std::string(" >(*getReader(), \"") + name_ + std::string("\");\n");
    }
    std::string printJSON(int indent) { return std::string(); }
    // std::string loop(int indent) {
    //   return indentation(indent) + std::string("std::cout << \"") + name_ + std::string(": \";\n") +
    //          indentation(indent) + std::string("for (int i = 0;  i < ") + rootDummy(name_) + std::string("->GetSize(); i++)\n") +
    //          indentation(indent + 2) + std::string("std::cout << (*") + rootDummy(name_) + std::string(")[i] << \" \";\n") +
    //          indentation(indent) + std::string("std::cout << std::endl;\n");
    // }
    std::string schema(int indent, std::string ns) {
      return std::string("");
    }
  };

  class RawNode : public Node {
    std::string type_;
    std::string name_;
  public:
    RawNode(std::string type, std::string name) : type_(type), name_(name) { }
    std::string declare(int indent) {
      return indentation(indent) + type_ + " *" + rootDummy(name_) + std::string(";\n") +
             indentation(indent) + "TBranch *b_" + rootDummy(name_) + std::string(";\n");
    }
    std::string init(int indent) {
      return indentation(indent) + rootDummy(name_) + std::string(" = nullptr;\n") +
             indentation(indent) + std::string("b_") + rootDummy(name_) + std::string(" = nullptr;\n") +
             indentation(indent) + std::string("getReader()->GetTree()->SetBranchAddress(\"") + name_ + std::string("\", &") + rootDummy(name_) + std::string(", &b_") + rootDummy(name_) + std::string(");\n");
    }
    std::string printJSON(int indent) { return std::string(); }
    // std::string loop(int indent) {
    //   std::string expr;
    //   if (type_ == std::string("string"))
    //     expr = std::string("*") + rootDummy(name_);
    //   else if (type_ == std::string("TString"))
    //     expr = rootDummy(name_) + std::string("->Data()");
    //   else if (type_ == std::string("TObjArray"))   // for testing; we don't know enough about this type to handle it
    //     expr = std::string("\"NOT ENOUGH INFO\"");
    //   else
    //     throw;
    //   return indentation(indent) + std::string("b_") + rootDummy(name_) + std::string("->GetEntry(getReader()->GetCurrentEntry());\n") +
    //          indentation(indent) + std::string("std::cout << \"") + name_ + std::string(" \" << ") + expr + std::string(" << std::endl;\n");
    // }
    std::string schema(int indent, std::string ns) {
      return std::string("");
    }
  };

  Node **newArray(int size);

  std::string definitions(std::map<const std::string, Def*> defs);
  std::string declarations(Node **scaffoldArray, int scaffoldSize);
  std::string init(Node **scaffoldArray, int scaffoldSize);
  std::string printJSON(Node **scaffoldArray, int scaffoldSize);
  std::string schema(int indent, std::string name, std::string ns, scaffold::Node **scaffoldArray, int scaffoldSize);
}

#endif // SCAFFOLD_H
