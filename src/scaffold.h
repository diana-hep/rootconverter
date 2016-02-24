#ifndef SCAFFOLD_H
#define SCAFFOLD_H

#include <string>
#include <vector>

namespace scaffold {
  class Node {
  public:
    virtual std::string header(int indent) = 0;
    virtual std::string init(int indent) = 0;
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
    std::string header(int indent) {
      return std::string("");
    }
    std::string init(int indent) {
      return std::string("");
    }
  };

  class ReaderValueNode : public Node {
    std::string type_;
    std::string name_;
  public:
    ReaderValueNode(std::string type, std::string name) : type_(type), name_(name) { }
    std::string header(int indent) {
      return indentation(indent) + std::string("TTreeReaderValue<") + type_ + std::string(" > *") + name_ + std::string(";\n");
    }
    std::string init(int indent) {
      return indentation(indent) + name_ + " = new " + std::string("TTreeReaderValue<") + type_ + std::string(" >;\n");
    }
  };

  class ReaderStringNode : public Node {
    std::string name_;
  public:
    ReaderStringNode(std::string name) : name_(name) { }
    std::string header(int indent) {
      return indentation(indent) + std::string("TTreeReaderArray<Char_t > *") + name_ + std::string(";\n");
    }
    std::string init(int indent) {
      return indentation(indent) + name_ + " = new " + std::string("TTreeReaderArray<Char_t >;\n");
    }
  };

  class ReaderArrayNode : public Node {
    std::string type_;
    std::string name_;
  public:
    ReaderArrayNode(std::string type, std::string name) : type_(type), name_(name) { }
    std::string header(int indent) {
      return indentation(indent) + std::string("TTreeReaderArray<") + type_ + std::string(" > *") + name_ + std::string(";\n");
    }
    std::string init(int indent) {
      return indentation(indent) + name_ + " = new " + std::string("TTreeReaderArray<") + type_ + std::string(" >;\n");
    }
  };

  class ReaderArrayArrayNode : public Node {
    std::string type_;
    std::string name_;
    int nesting_;
  public:
    ReaderArrayArrayNode(std::string type, std::string name, int nesting) : type_(type), name_(name), nesting_(nesting) { }
    std::string header(int indent) {
      return indentation(indent) + std::string("TTreeReaderArray<") + type_ + std::string(" > *") + name_ + std::string(";\n");
    }
    std::string init(int indent) {
      return indentation(indent) + name_ + " = new " + std::string("TTreeReaderArray<") + type_ + std::string(" >;\n");
    }
  };

  class ReaderNestedArrayNode : public Node {
    std::string type_;
    std::string name_;
    std::vector<int> fixedTail_;
  public:
    ReaderNestedArrayNode(std::string type, std::string name, std::vector<int> fixedTail) : type_(type), name_(name), fixedTail_(fixedTail) { }
    std::string header(int indent) {
      return indentation(indent) + std::string("TTreeReaderArray<") + type_ + std::string(" > *") + name_ + std::string(";\n");
    }
    std::string init(int indent) {
      return indentation(indent) + name_ + " = new " + std::string("TTreeReaderArray<") + type_ + std::string(" >;\n");
    }
  };

  class RawNode : public Node {
    std::string type_;
    std::string name_;
  public:
    RawNode(std::string type, std::string name) : type_(type), name_(name) { }
    std::string header(int indent) {
      return indentation(indent) + type_ + " *" + name_ + std::string(";\n");
    }
    std::string init(int indent) {
      return indentation(indent) + name_ + " = new " + type_ + std::string(";\n") +
             indentation(indent) + std::string("reader.GetTree()->SetBranchAddress(\"") + name_ + std::string("\", &") + name_ + std::string(");\n");
    }
  };

  Node **newArray(int size);

  std::string header(Node **scaffoldArray, int scaffoldSize);
  std::string init(Node **scaffoldArray, int scaffoldSize);
}

#endif // SCAFFOLD_H
