#ifndef SCAFFOLD_H
#define SCAFFOLD_H

#include <string>

namespace scaffold {
  class Node {
  public:
    virtual std::string generateHeader(int indent) = 0;
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
    std::string generateHeader(int indent) {
      return std::string("");
    }
  };

  class ReaderValueNode : public Node {
    std::string type_;
    std::string name_;
  public:
    ReaderValueNode(std::string type, std::string name) : type_(type), name_(name) { };
    std::string generateHeader(int indent) {
      return indentation(indent) + std::string("TTreeReaderValue<") + type_ + std::string("> ") + name_ + std::string(";\n");
    }
  };

  class ReaderStringNode : public Node {
    std::string name_;
  public:
    ReaderStringNode(std::string name) : name_(name) { };
    std::string generateHeader(int indent) {
      return indentation(indent) + std::string("TTreeReaderArray<Char_t> ") + name_ + std::string(";\n");
    }
  };

  class ReaderArrayNode : public Node {
    std::string type_;
    std::string name_;
  public:
    ReaderArrayNode(std::string type, std::string name) : type_(type), name_(name) { };
    std::string generateHeader(int indent) {
      return indentation(indent) + std::string("TTreeReaderArray<") + type_ + std::string("> ") + name_ + std::string(";\n");
    }
  };

  Node **newArray(int size);

  std::string generateHeader(Node **scaffoldArray, int scaffoldSize);
}

#endif // SCAFFOLD_H
