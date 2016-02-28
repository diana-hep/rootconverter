#ifndef SCAFFOLD_H
#define SCAFFOLD_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

namespace scaffold {
  /////////////////////////////////////////////////////////////////////////////////// helper functions

  enum Template { vectorOf };
  enum Kind { scalar, array, vector, structure };

  class Type;
  class Def;
  class Node;

  std::string rootDummy(std::string fieldName);
  std::string indentation(int indent);
  void setTemplatesInnerType(std::string type, std::vector<Template> &templates, std::string &innerType);
  std::string innerPrintJSON(std::string item, Def *def, std::string innerType);
  std::string unrollTemplatesPrintJSON(int indent, std::string item, std::string dummy, std::vector<Template> templates, Def *def, std::string innerType);
  std::string innerSchema(int indent, std::string ns, Def *def, std::string innerType);
  std::string unrollTemplatesSchema(int indent, std::string ns, std::vector<Template> templates, Def *def, std::string innerType);

  /////////////////////////////////////////////////////////////////////////////////// classes

  // FIXME: Type needs to handle template unrolling and Defs
  class Type {
    std::string type_;
    Kind kind_;
    Def *def_;
    std::vector<int> dims_;
  public:
    Type(std::string type, Kind kind, Def *def);
    std::string typeName();
    std::string arrayBrackets();
    std::string printJSON(int indent, std::string item);
    std::string schema(int indent, std::string ns);
  };

  class Def {
    std::string typeName_;
    std::vector<std::string> bases_;
    std::vector<std::string> names_;
    std::vector<Type> types_;
  public:
    Def(std::string typeName);
    std::string typeName();
    std::string name(int i);
    Type type(int i);
    void addBase(std::string base);
    void addField(Type type, std::string name);
    bool empty();
    std::string forward();
    std::string def();
    std::string ref();
    std::string schema(int indent, std::string ns);
  };

  class Node {
  public:
    virtual std::string declare(int indent) = 0;
    virtual std::string init(int indent) = 0;
    virtual std::string printJSON(int indent) = 0;
    virtual std::string schema(int indent, std::string ns) = 0;
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
    std::vector<Template> templates_;
    std::string innerType_;
  public:
    ReaderValueNode(std::string type, std::string name, Def *def);
    std::string declare(int indent);
    std::string init(int indent);
    std::string printJSON(int indent);
    std::string schema(int indent, std::string ns);
  };

  class ReaderArrayNode : public Node {
    std::string type_;
    std::string name_;
    Def *def_;
    std::vector<Template> templates_;
    std::string innerType_;
  public:
    ReaderArrayNode(std::string type, std::string name, Def *def);
    std::string declare(int indent);
    std::string init(int indent);
    std::string printJSON(int indent);
    std::string schema(int indent, std::string ns);
  };

  // specifically, the char* special case (not really an array; it's a string)
  class ReaderStringNode : public Node {
    std::string name_;
  public:
    ReaderStringNode(std::string name);
    std::string declare(int indent);
    std::string init(int indent);
    std::string printJSON(int indent);
    std::string schema(int indent, std::string ns);
  };

  // see https://sft.its.cern.ch/jira/browse/ROOT-7467 for why we need this special case
  class ReaderVectorBoolNode : public Node {
    std::string name_;
  public:
    ReaderVectorBoolNode(std::string name);
    std::string declare(int indent);
    std::string init(int indent);
    std::string printJSON(int indent);
    std::string schema(int indent, std::string ns);
  };

  // to preserve structure in nested sequences that end with fixed-sized arrays
  class ReaderNestedArrayNode : public Node {
    std::string type_;
    std::string name_;
    std::vector<int> fixedTail_;   // use this to preserve some nesting
  public:
    ReaderNestedArrayNode(std::string type, std::string name, std::vector<int> fixedTail);
    std::string declare(int indent);
    std::string init(int indent);
    std::string printJSON(int indent);
    std::string schema(int indent, std::string ns);
  };

  // handle cases that can't be accessed through TTreeReader (use the original SetBranchAddress method)
  class RawNode : public Node {
    std::string type_;
    std::string name_;
  public:
    RawNode(std::string type, std::string name);
    std::string declare(int indent);
    std::string init(int indent);
    std::string printJSON(int indent);
    std::string schema(int indent, std::string ns);
  };

  /////////////////////////////////////////////////////////////////////////////////// public access functions

  Node **newArray(int size);

  std::string definitions(std::map<const std::string, Def*> defs);
  std::string declarations(Node **scaffoldArray, int scaffoldSize);
  std::string init(Node **scaffoldArray, int scaffoldSize);
  std::string printJSON(Node **scaffoldArray, int scaffoldSize);
  std::string schema(int indent, std::string name, std::string ns, scaffold::Node **scaffoldArray, int scaffoldSize);
}

#endif // SCAFFOLD_H
