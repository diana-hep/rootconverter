#ifndef SCAFFOLD_H
#define SCAFFOLD_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

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

  enum Template { vectorOf };

  class ReaderValueNode : public Node {
    std::string type_;
    std::string name_;
    Def *def_;
    std::vector<Template> templates_;
    std::string innerType_;
  public:
    ReaderValueNode(std::string type, std::string name, Def *def) : type_(type), name_(name), def_(def) {
      std::string vectorOfPrefix("vector<");
      int start = 0;
      while (true)
        if (type_.substr(start, vectorOfPrefix.size()) == vectorOfPrefix) {
          templates_.push_back(vectorOf);
          start += vectorOfPrefix.size();
        }
        else
          break;
      int stop;
      for (stop = start;  stop < type_.size()  &&  type_[stop] != '>';  stop++);
      innerType_ = type_.substr(start, stop - start);
    }
    std::string declare(int indent) {
      return indentation(indent) + std::string("TTreeReaderValue<") + type_ + std::string(" > *") + rootDummy(name_) + std::string(";\n");
    }
    std::string init(int indent) {
      return indentation(indent) + rootDummy(name_) + " = new " + std::string("TTreeReaderValue<") + type_ + std::string(" >(*getReader(), \"") + name_ + std::string("\");\n");
    }
    std::string innerPrintJSON(std::string var) {
      if (def_ != nullptr)
        return var + std::string(".printJSON()");
      else if (innerType_ == std::string("bool")  ||  innerType_ == std::string("Bool_t"))
        return std::string("b(") + var + std::string(")");
      else if (innerType_ == std::string("char")  ||  innerType_ == std::string("Char_t"))
        return std::string("d(") + var + std::string(")");   // Char_t -> integer for our purposes
      else if (innerType_ == std::string("short")  ||  innerType_ == std::string("Short_t"))
        return std::string("hd(") + var + std::string(")");
      else if (innerType_ == std::string("int")  ||  innerType_ == std::string("Int_t"))
        return std::string("d(") + var + std::string(")");
      else if (innerType_ == std::string("long")  ||  innerType_ == std::string("Long_t")  ||  innerType_ == std::string("Long64_t"))
        return std::string("ld(") + var + std::string(")");
      else if (innerType_ == std::string("float")  ||  innerType_ == std::string("Float_t")  ||  innerType_ == std::string("Float16_t"))
        return std::string("fg(") + var + std::string(")");
      else if (innerType_ == std::string("double")  ||  innerType_ == std::string("Double_t")  ||  innerType_ == std::string("Double32_t"))
        return std::string("g(") + var + std::string(")");
      else if (innerType_ == std::string("unsigned char")  ||  innerType_ == std::string("UChar_t"))
        return std::string("u(") + var + std::string(")");   // Char_t -> integer for our purposes
      else if (innerType_ == std::string("unsigned short")  ||  innerType_ == std::string("UShort_t"))
        return std::string("hu(") + var + std::string(")");
      else if (innerType_ == std::string("unsigned int")  ||  innerType_ == std::string("UInt_t"))
        return std::string("u(") + var + std::string(")");
      else if (innerType_ == std::string("unsigned long")  ||  innerType_ == std::string("ULong_t")  ||  innerType_ == std::string("ULong64_t"))
        return std::string("lu(") + var + std::string(")");
      else 
        throw std::invalid_argument(std::string("unrecognized type (A): ") + innerType_);
    }
    std::string printJSON(int indent) {
      std::string out;
      std::string item = std::string("item_") + rootDummy(name_);
      out += indentation(indent) + std::string("s(\"\\\"") + name_ + std::string("\\\": \");\n") +
             indentation(indent) + std::string("auto ") + item + std::string(" = *(") + rootDummy(name_) + std::string("->Get());\n");
      for (int i = 0;  i < templates_.size();  i++)
        if (templates_[i] == vectorOf) {
          std::string len = std::string("len") + std::to_string(i) + std::string("_") + rootDummy(name_);
          std::string index = std::string("i") + std::to_string(i);
          std::string newitem = std::string("item") + std::to_string(i) + std::string("_") + rootDummy(name_);
          out += indentation(indent + 2*i) + std::string("int ") + len + std::string(" = ") + item + std::string(".size();\n") +
                 indentation(indent + 2*i) + std::string("for (int ") + index + std::string(" = 0;  ") + index + std::string(" < ") + len + std::string(";  ") + index + std::string("++) {\n") +
                 indentation(indent + 2*i) + std::string("  auto ") + newitem + std::string(" = ") + item + std::string("[") + index + std::string("];\n") +
                 indentation(indent + 2*i) + std::string("  if (") + index + std::string(" > 0) s(\", \");\n");
          if (i < templates_.size() - 1)
            out += indentation(indent + 2*i) + std::string("  s(\"[\");\n");
          item = newitem;
        }
      out += indentation(indent + 2*templates_.size()) + innerPrintJSON(item) + std::string(";\n");
      for (int i = templates_.size() - 1;  i >= 0;  i--)
        if (templates_[i] == vectorOf) {
          if (i < templates_.size() - 1)
            out += indentation(indent + 2*i) + std::string("  s(\"]\");\n");
          out += indentation(indent + 2*i) + std::string("}\n");
        }
      return out;
    }
    std::string innerSchema(int indent, std::string ns) {
      if (def_ != nullptr)
        return def_->schema(indent + 2, ns);
      else if (innerType_ == std::string("bool")  ||  innerType_ == std::string("Bool_t"))
        return std::string("\"boolean\"");
      else if (innerType_ == std::string("char")  ||  innerType_ == std::string("Char_t"))
        return std::string("\"int\"");
      else if (innerType_ == std::string("short")  ||  innerType_ == std::string("Short_t"))
        return std::string("\"int\"");
      else if (innerType_ == std::string("int")  ||  innerType_ == std::string("Int_t"))
        return std::string("\"int\"");
      else if (innerType_ == std::string("long")  ||  innerType_ == std::string("Long_t")  ||  innerType_ == std::string("Long64_t"))
        return std::string("\"long\"");
      else if (innerType_ == std::string("float")  ||  innerType_ == std::string("Float_t")  ||  innerType_ == std::string("Float16_t"))
        return std::string("\"float\"");
      else if (innerType_ == std::string("double")  ||  innerType_ == std::string("Double_t")  ||  innerType_ == std::string("Double32_t"))
        return std::string("\"double\"");
      else if (innerType_ == std::string("unsigned char")  ||  innerType_ == std::string("UChar_t"))
        return std::string("\"int\"");
      else if (innerType_ == std::string("unsigned short")  ||  innerType_ == std::string("UShort_t"))
        return std::string("\"int\"");
      else if (innerType_ == std::string("unsigned int")  ||  innerType_ == std::string("UInt_t"))
        return std::string("\"long\"");
      else if (innerType_ == std::string("unsigned long")  ||  innerType_ == std::string("ULong_t")  ||  innerType_ == std::string("ULong64_t"))
        return std::string("\"double\"");
      else
        throw std::invalid_argument(std::string("unrecognized type (B): ") + innerType_);
    }
    std::string schema(int indent, std::string ns) {
      std::string out;
      out += indentation(indent) + std::string("{\"name\": \"") + name_ + std::string("\", \"type\": ");
      for (int i = 0;  i < templates_.size();  i++)
        if (templates_[i] == vectorOf) {
          out += std::string("\n") + indentation(indent + 2*i + 2) + std::string("{\"type\": \"array\", \"items\": ");
        }
      out += innerSchema(indent + 2*templates_.size(), ns);
      for (int i = templates_.size() - 1;  i >= 0;  i--)
        if (templates_[i] == vectorOf) {
          out += std::string("}\n") + indentation(indent + 2*i);
        }
      out += std::string("}");
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

  class ReaderVectorBoolNode : public Node {   // see https://sft.its.cern.ch/jira/browse/ROOT-7467 for why we need this special case
    std::string name_;
  public:
    ReaderVectorBoolNode(std::string name) : name_(name) { }
    std::string declare(int indent) {
      return indentation(indent) + std::string("TTreeReaderValue<std::vector<bool> > *") + rootDummy(name_) + std::string(";\n");
    }
    std::string init(int indent) {
      return indentation(indent) + rootDummy(name_) + " = new " + std::string("TTreeReaderValue<std::vector<bool> >(*getReader(), \"") + name_ + std::string("\");\n");
    }
    std::string printJSON(int indent) {
      return indentation(indent) + std::string("s(\"\\\"") + name_ + std::string("\\\": [\");\n") +
             indentation(indent) + std::string("std::vector<bool> item_") + rootDummy(name_) + std::string(" = *(") + rootDummy(name_) + std::string("->Get());\n") +
             indentation(indent) + std::string("int len_") + rootDummy(name_) + std::string(" = item_") + rootDummy(name_) + std::string(".size();\n") +
             indentation(indent) + std::string("for (int i = 0;  i < len_") + rootDummy(name_) + std::string("; i++) {\n") +
             indentation(indent) + std::string("  if (i != 0) s(\", \");\n") +
             indentation(indent) + std::string("  b(item_") + rootDummy(name_) + std::string("[i]);\n") +
             indentation(indent) + std::string("}\n") +
             indentation(indent) + std::string("s(\"]\");\n");
    }
    std::string schema(int indent, std::string ns) {
      return indentation(indent) + std::string("{\"name\": \"") + name_ + std::string("\", \"type\": {\"type\": \"array\", \"items\": \"boolean\"}}");
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
      std::string out;
      out += indentation(indent) + std::string("s(\"\\\"") + name_ + std::string("\\\": [\");\n") +
             indentation(indent) + std::string("int len_") + rootDummy(name_) + std::string(" = ") + rootDummy(name_) + std::string("->GetSize();\n") +
             indentation(indent) + std::string("for (int i = 0;  i < len_") + rootDummy(name_) + std::string("; i++) {\n") +
             indentation(indent) + std::string("  if (i != 0) s(\", \");\n");
      
      if (type_ == std::string("Bool_t"))
        out += indentation(indent) + std::string("  b((*") + rootDummy(name_) + std::string(")[i]);\n");
      else if (type_ == std::string("Char_t"))
        out += indentation(indent) + std::string("  d((*") + rootDummy(name_) + std::string(")[i]);\n");   // Char_t -> integer for our purposes
      else if (type_ == std::string("Short_t"))
        out += indentation(indent) + std::string("  hd((*") + rootDummy(name_) + std::string(")[i]);\n");
      else if (type_ == std::string("Int_t"))
        out += indentation(indent) + std::string("  d((*") + rootDummy(name_) + std::string(")[i]);\n");
      else if (type_ == std::string("Long_t"))
        out += indentation(indent) + std::string("  ld((*") + rootDummy(name_) + std::string(")[i]);\n");
      else if (type_ == std::string("Long64_t"))
        out += indentation(indent) + std::string("  ld((*") + rootDummy(name_) + std::string(")[i]);\n");
      else if (type_ == std::string("Float_t"))
        out += indentation(indent) + std::string("  fg((*") + rootDummy(name_) + std::string(")[i]);\n");
      else if (type_ == std::string("Float16_t"))
        out += indentation(indent) + std::string("  fg((*") + rootDummy(name_) + std::string(")[i]);\n");
      else if (type_ == std::string("Double_t"))
        out += indentation(indent) + std::string("  g((*") + rootDummy(name_) + std::string(")[i]);\n");
      else if (type_ == std::string("Double32_t"))
        out += indentation(indent) + std::string("  g((*") + rootDummy(name_) + std::string(")[i]);\n");
      else if (type_ == std::string("UChar_t"))
        out += indentation(indent) + std::string("  u((*") + rootDummy(name_) + std::string(")[i]);\n");   // Char_t -> integer for our purposes
      else if (type_ == std::string("UShort_t"))
        out += indentation(indent) + std::string("  hu((*") + rootDummy(name_) + std::string(")[i]);\n");
      else if (type_ == std::string("UInt_t"))
        out += indentation(indent) + std::string("  u((*") + rootDummy(name_) + std::string(")[i]);\n");
      else if (type_ == std::string("ULong_t"))
        out += indentation(indent) + std::string("  lu((*") + rootDummy(name_) + std::string(")[i]);\n");
      else if (type_ == std::string("ULong64_t"))
        out += indentation(indent) + std::string("  lu((*") + rootDummy(name_) + std::string(")[i]);\n");
      else if (type_ == std::string("string"))
        out += indentation(indent) + std::string("  s(\"\\\"\"); s(escapeJSON((*") + rootDummy(name_) + std::string(")[i]).c_str()); s(\"\\\"\");\n");
      else if (type_ == std::string("TString"))
        out += indentation(indent) + std::string("  s(\"\\\"\"); s(escapeJSON(std::string((*") + rootDummy(name_) + std::string(")[i].Data())).c_str()); s(\"\\\"\");\n");
      else
        throw std::invalid_argument(std::string("unrecognized type (C): ") + type_);

      out += indentation(indent) + std::string("};\n") +
             indentation(indent) + std::string("s(\"]\");\n");

      return out;
    }
    std::string schema(int indent, std::string ns) {
      std::string out;
      out += indentation(indent) + std::string("{\"name\": \"") + name_ + std::string("\", \"type\": {\"type\": \"array\", \"items\": ");
      if (type_ == std::string("Bool_t"))
        out += std::string("\"boolean\"}}");
      else if (type_ == std::string("Char_t"))
        out += std::string("\"int\"}}");
      else if (type_ == std::string("Short_t"))
        out += std::string("\"int\"}}");
      else if (type_ == std::string("Int_t"))
        out += std::string("\"int\"}}");
      else if (type_ == std::string("Long_t"))
        out += std::string("\"long\"}}");
      else if (type_ == std::string("Long64_t"))
        out += std::string("\"long\"}}");
      else if (type_ == std::string("Float_t"))
        out += std::string("\"float\"}}");
      else if (type_ == std::string("Float16_t"))
        out += std::string("\"float\"}}");
      else if (type_ == std::string("Double_t"))
        out += std::string("\"double\"}}");
      else if (type_ == std::string("Double32_t"))
        out += std::string("\"double\"}}");
      else if (type_ == std::string("UChar_t"))
        out += std::string("\"int\"}}");
      else if (type_ == std::string("UShort_t"))
        out += std::string("\"int\"}}");
      else if (type_ == std::string("UInt_t"))
        out += std::string("\"long\"}}");
      else if (type_ == std::string("ULong_t"))
        out += std::string("\"double\"}}");
      else if (type_ == std::string("ULong64_t"))
        out += std::string("\"double\"}}");
      else if (type_ == std::string("string"))
        out += std::string("\"string\"}}");
      else if (type_ == std::string("TString"))
        out += std::string("\"string\"}}");
      else
        throw std::invalid_argument(std::string("unrecognized type (D): ") + type_);
      return out;
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
    std::string printJSON(int indent) {
      std::string out = indentation(indent) + std::string("b_") + rootDummy(name_) + std::string("->GetEntry(getReader()->GetCurrentEntry());\n") +
                        indentation(indent) + std::string("s(\"\\\"") + name_ + std::string("\\\": \");\n");
      if (type_ == std::string("string"))
        out += indentation(indent) + std::string("  s(\"\\\"\"); s(escapeJSON(*") + rootDummy(name_) + std::string(").c_str()); s(\"\\\"\");\n");
      else if (type_ == std::string("TString"))
        out += indentation(indent) + std::string("  s(\"\\\"\"); s(escapeJSON(std::string(") + rootDummy(name_) + std::string("->Data())).c_str()); s(\"\\\"\");\n");
      else
        throw std::invalid_argument(std::string("unrecognized type (E): ") + type_);
      return out;
    }
    std::string schema(int indent, std::string ns) {
      std::string out;
      out += indentation(indent) + std::string("{\"name\": \"") + name_ + std::string("\", \"type\": ");
      if (type_ == std::string("string"))
        out += std::string("\"string\"}");
      else if (type_ == std::string("TString"))
        out += std::string("\"string\"}");
      else
        throw std::invalid_argument(std::string("unrecognized type (F): ") + type_);
      return out;
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
