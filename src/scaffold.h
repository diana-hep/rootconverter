#ifndef SCAFFOLD_H
#define SCAFFOLD_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace scaffold {
  std::string rootDummy(std::string fieldName);

  class Node {
  public:
    virtual std::string declare(int indent) = 0;
    virtual std::string init(int indent) = 0;
    virtual std::string loop(int indent) = 0;
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
    std::string loop(int indent) {
      return std::string("");
    }
  };

  class ReaderValueNode : public Node {
    std::string type_;
    std::string name_;
  public:
    ReaderValueNode(std::string type, std::string name) : type_(type), name_(name) { }
    std::string declare(int indent) {
      return indentation(indent) + std::string("TTreeReaderValue<") + type_ + std::string(" > *") + rootDummy(name_) + std::string(";\n");
    }
    std::string init(int indent) {
      return indentation(indent) + rootDummy(name_) + " = new " + std::string("TTreeReaderValue<") + type_ + std::string(" >(reader, \"") + name_ + std::string("\");\n");
    }
    std::string loop(int indent) {
      return indentation(indent) + std::string("std::cout << \"") + name_ + std::string(" \" << *(") + rootDummy(name_) + std::string("->Get()) << std::endl;\n");
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
      return indentation(indent) + rootDummy(name_) + " = new " + std::string("TTreeReaderArray<Char_t >(reader, \"") + name_ + std::string("\");\n");
    }
    std::string loop(int indent) {
      return indentation(indent) + std::string("std::cout << \"") + name_ + std::string(" \" << (char*)") + rootDummy(name_) + std::string("->GetAddress() << std::endl;\n");
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
      return indentation(indent) + rootDummy(name_) + " = new " + std::string("TTreeReaderArray<") + type_ + std::string(" >(reader, \"") + name_ + std::string("\");\n");
    }
    std::string loop(int indent) {
      return indentation(indent) + std::string("std::cout << \"") + name_ + std::string(": \";\n") +
             indentation(indent) + std::string("for (int i = 0;  i < ") + rootDummy(name_) + std::string("->GetSize(); i++)\n") +
             indentation(indent + 2) + std::string("std::cout << (*") + rootDummy(name_) + std::string(")[i] << \" \";\n") +
             indentation(indent) + std::string("std::cout << std::endl;\n");
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
      return indentation(indent) + rootDummy(name_) + " = new " + std::string("TTreeReaderArray<") + type_ + std::string(" >(reader, \"") + name_ + std::string("\");\n");
    }
    std::string loop(int indent) {
      std::string out = indentation(indent) + std::string("std::cout << \"") + name_ + std::string(": \";\n") +
                        indentation(indent) + std::string("for (int i0 = 0;  i0 < ") + rootDummy(name_) + std::string("->GetSize(); i0++)\n");
      std::string prev = std::string("i0");
      std::string reach = std::string("(*") + rootDummy(name_) + std::string(")[") + prev + std::string("]");
      for (int n = 1;  n <= nesting_;  n++) {
        std::string var = std::string("i") + std::to_string(n);
        out += indentation(indent + 2*n) + std::string("for (int ") + var + std::string(" = 0;  ") + var + std::string(" < ") + reach + std::string(".size();  ") + var + std::string("++)\n");
        reach = reach + std::string("[") + var + std::string("]");
        prev = var;
      }
      out += indentation(indent + 2*nesting_ + 2) + std::string("std::cout << ") + reach + std::string(" << \" \";\n");
      out += indentation(indent) + std::string("std::cout << std::endl;\n");
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
      return indentation(indent) + rootDummy(name_) + " = new " + std::string("TTreeReaderArray<") + type_ + std::string(" >(reader, \"") + name_ + std::string("\");\n");
    }
    std::string loop(int indent) {
      return indentation(indent) + std::string("std::cout << \"") + name_ + std::string(": \";\n") +
             indentation(indent) + std::string("for (int i = 0;  i < ") + rootDummy(name_) + std::string("->GetSize(); i++)\n") +
             indentation(indent + 2) + std::string("std::cout << (*") + rootDummy(name_) + std::string(")[i] << \" \";\n") +
             indentation(indent) + std::string("std::cout << std::endl;\n");
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
             indentation(indent) + std::string("reader.GetTree()->SetBranchAddress(\"") + name_ + std::string("\", &") + rootDummy(name_) + std::string(", &b_") + rootDummy(name_) + std::string(");\n");
    }
    std::string loop(int indent) {
      std::string expr;
      if (type_ == std::string("string"))
        expr = std::string("*") + rootDummy(name_);
      else if (type_ == std::string("TString"))
        expr = rootDummy(name_) + std::string("->Data()");
      else if (type_ == std::string("TObjArray"))   // for testing; we don't know enough about this type to handle it
        expr = std::string("\"NOT ENOUGH INFO\"");
      else
        throw;
      return indentation(indent) + std::string("b_") + rootDummy(name_) + std::string("->GetEntry(reader.GetCurrentEntry());\n") +
             indentation(indent) + std::string("std::cout << \"") + name_ + std::string(" \" << ") + expr + std::string(" << std::endl;\n");
    }
  };

  class Def {
    std::string typeName_;
    std::vector<std::string> bases_;
    std::vector<std::string> names_;
    std::vector<std::string> types_;
  public:
    Def(std::string typeName) : typeName_(typeName) { }
    std::string typeName() { return typeName_; }
    std::string name(int i) { return names_[i]; }
    std::string type(int i, bool base) {
      if (!base)
        return types_[i];
      else {
        std::string out;
        std::string t = types_[i];
        for (int j = 0;  j < t.size()  &&  t[j] != '[';  j++)
          out += t[j];
        return out;
      }
    }
    std::vector<int> dims(int i) {
      std::vector<int> out;
      std::string t = types_[i];
      for (int j = 0;  j < t.size()  &&  t[j];  j++) {
        if (t[j] == '[')
          out.push_back(0);
        else if ('0' <= t[j]  &&  t[j] <= '9'  &&  out.size() > 0)
          out.back() = 10 * out.back() + ((int)t[j] - (int)'0');
      }
      return out;
    }
    void addBase(std::string base) {
      bases_.push_back(base);
    }
    void addField(std::string type, std::string name) {
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
          std::vector<int> d = dims(i);
          std::string dd;
          for (int j = 0;  j < d.size();  j++)
            dd += std::string("[") + std::to_string(d[j]) + std::string("]");
          out += std::string("  ") + type(i, true) + std::string(" ") + names_[i] + dd + std::string(";\n");
        }
        out += std::string("};\n");
        return out;
      }
    }
    std::string ref() { return typeName_; }
  };

  Node **newArray(int size);

  std::string definitions(std::map<const std::string, Def*> defs);
  std::string declarations(Node **scaffoldArray, int scaffoldSize);
  std::string init(Node **scaffoldArray, int scaffoldSize, std::vector<std::string> fileLocations, std::string treeLocation);
  std::string loop(Node **scaffoldArray, int scaffoldSize);
}

#endif // SCAFFOLD_H
