#ifndef DATAWALKER_H
#define DATAWALKER_H

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <set>

#include <TTree.h>
#include <TLeaf.h>
#include <TClass.h>
#include <TDataMember.h>
#include <TInterpreter.h>

class ClassWalker;

class FieldWalker {
public:
  FieldWalker(std::string fieldName, std::string typeName) : fieldName(fieldName), typeName(typeName) { }
  std::string fieldName;
  std::string typeName;
  virtual bool empty() = 0;
  virtual bool resolved() = 0;
  virtual void resolve(void *address) = 0;
  virtual std::string repr(int indent, std::set<std::string> &memo) = 0;
  virtual std::string avroTypeName() = 0;
  virtual void printJSON(void *address) = 0;
};

class PrimitiveWalker : public FieldWalker {
public:
  PrimitiveWalker(std::string fieldName, std::string typeName) : FieldWalker(fieldName, typeName) { }
  bool empty() { return false; }
  bool resolved() { return true; }
  void resolve(void *address) { }
  std::string repr(int indent, std::set<std::string> &memo) { return std::string("\"") + typeName + std::string("\""); }
};

class BoolWalker : public PrimitiveWalker {
public:
  BoolWalker(std::string fieldName) : PrimitiveWalker(fieldName, "bool") { }
  std::string avroTypeName() { return "boolean"; }
  void printJSON(void *address) { if (*((bool*)address)) std::cout << "true"; else std::cout << "false"; }
};

class CharWalker : public PrimitiveWalker {
public:
  CharWalker(std::string fieldName) : PrimitiveWalker(fieldName, "char") { }
  std::string avroTypeName() { return "int"; }
  void printJSON(void *address) { std::cout << (int)(*((char*)address)); }
};

class UCharWalker : public PrimitiveWalker {
public:
  UCharWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned char") { }
  std::string avroTypeName() { return "int"; }
  void printJSON(void *address) { std::cout << (int)(*((unsigned char*)address)); }
};

class ShortWalker : public PrimitiveWalker {
public:
  ShortWalker(std::string fieldName) : PrimitiveWalker(fieldName, "short") { }
  std::string avroTypeName() { return "int"; }
  void printJSON(void *address) { std::cout << *((short*)address); }
};

class UShortWalker : public PrimitiveWalker {
public:
  UShortWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned short") { }
  std::string avroTypeName() { return "int"; }
  void printJSON(void *address) { std::cout << *((unsigned short*)address); }
};

class IntWalker : public PrimitiveWalker {
public:
  IntWalker(std::string fieldName) : PrimitiveWalker(fieldName, "int") { }
  std::string avroTypeName() { return "int"; }
  void printJSON(void *address) { std::cout << *((int*)address); }
};

class UIntWalker : public PrimitiveWalker {
public:
  UIntWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned int") { }
  std::string avroTypeName() { return "long"; }
  void printJSON(void *address) { std::cout << *((unsigned int*)address); }
};

class LongWalker : public PrimitiveWalker {
public:
  LongWalker(std::string fieldName) : PrimitiveWalker(fieldName, "long") { }
  std::string avroTypeName() { return "long"; }
  void printJSON(void *address) { std::cout << *((long*)address); }
};

class ULongWalker : public PrimitiveWalker {
public:
  ULongWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned long") { }
  std::string avroTypeName() { return "double"; }
  void printJSON(void *address) { std::cout << *((unsigned long*)address); }
};

class FloatWalker : public PrimitiveWalker {
public:
  FloatWalker(std::string fieldName) : PrimitiveWalker(fieldName, "float") { }
  std::string avroTypeName() { return "float"; }
  void printJSON(void *address) { std::cout << *((float*)address); }
};

class DoubleWalker : public PrimitiveWalker {
public:
  DoubleWalker(std::string fieldName) : PrimitiveWalker(fieldName, "double") { }
  std::string avroTypeName() { return "double"; }
  void printJSON(void *address) { std::cout << *((double*)address); }
};

class MemberWalker : public FieldWalker {
public:
  MemberWalker(TDataMember *dataMember, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  size_t offset;
  FieldWalker *walker;
  FieldWalker *specializedWalker(std::string fieldName, std::string innerTypeName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  bool empty() { return walker->empty(); }
  bool resolved() { return walker->resolved(); }
  void resolve(void *address) {
    walker->resolve((void*)((size_t)address + offset));
  }
  std::string repr(int indent, std::set<std::string> &memo) { return std::string("\"") + fieldName + std::string("\": ") + walker->repr(indent, memo); }
  std::string avroTypeName() { return walker->avroTypeName(); }
  void printJSON(void *address) {
    std::cout << "\"" << fieldName << "\": ";
    walker->printJSON((void*)((size_t)address + offset));
  }
};

class ClassWalker : public FieldWalker {
public:
  ClassWalker(std::string fieldName, TClass *tclass, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  TClass *tclass;
  std::string avroNamespace;
  std::map<const std::string, ClassWalker*> &defs;
  std::vector<MemberWalker*> members;
  void fill();
  bool empty() { return members.empty(); }
  bool resolved() {
    for (auto iter = members.begin();  iter != members.end();  ++iter)
      if (!(*iter)->resolved())
        return false;
    return true;
  }
  void resolve(void *address) {
    for (auto iter = members.begin();  iter != members.end();  ++iter)
      (*iter)->resolve(address);
  }
  std::string repr(int indent, std::set<std::string> &memo) {
    if (memo.find(typeName) != memo.end())
      return std::string("\"") + typeName + std::string("\"");
    else {
      memo.insert(typeName);
      std::string out;
      out += std::string("{\"") + typeName + std::string("\": {\n") + std::string(indent + 2, ' ');
      bool first = true;
      for (auto iter = members.begin();  iter != members.end();  ++iter) {
        if (first) first = false; else out += std::string(",\n") + std::string(indent + 2, ' ');
        out += (*iter)->repr(indent + 2, memo);
      }
      out += std::string("\n") + std::string(indent, ' ') + std::string("}}");
      return out;
    }
  }
  std::string avroTypeName() {
    std::string out;
    if (!avroNamespace.empty())
      out += avroNamespace + std::string(".");
    out += typeName;
    return out;
  }
  void printJSON(void *address) {
    std::cout << "{";
    bool first = true;
    for (auto iter = members.begin();  iter != members.end();  ++iter) {
      if (first) first = false; else std::cout << ", ";    
      (*iter)->printJSON(address);
    }
    std::cout << "}";
  }
};

class AnyStringWalker : public FieldWalker {
public:
  AnyStringWalker(std::string fieldName, std::string typeName) : FieldWalker(fieldName, typeName) { }
  bool empty() { return false; }
  bool resolved() { return true; }
  void resolve(void *address) { }
  std::string repr(int indent, std::set<std::string> &memo) { return std::string("\"") + typeName + std::string("\""); }
  std::string escapeJSON(std::string string) { return string; }
  std::string avroTypeName() { return "string"; }
};

class CStringWalker : public AnyStringWalker {
public:
  CStringWalker(std::string fieldName) : AnyStringWalker(fieldName, "char*") { }
  void printJSON(void *address) { std::cout << "\"" << escapeJSON((char*)address) << "\""; }
};

class StdStringWalker : public AnyStringWalker {
public:
  StdStringWalker(std::string fieldName) : AnyStringWalker(fieldName, "string") { }
  void printJSON(void *address) { std::cout << "\"" << escapeJSON(*((std::string*)address)) << "\""; }
};

class TStringWalker : public AnyStringWalker {
public:
  TStringWalker(std::string fieldName) : AnyStringWalker(fieldName, "TString") { }
  void printJSON(void *address) { std::cout << "\"" << escapeJSON(((TString*)address)->Data()) << "\""; }
};

class PointerWalker : public FieldWalker {
public:
  PointerWalker(std::string fieldName, FieldWalker *walker) : FieldWalker(fieldName, "*"), walker(walker) { }
  FieldWalker *walker;
  bool empty() { return walker->empty(); }
  bool resolved() { return walker->resolved(); }
  void resolve(void *address) { walker->resolve(*((void**)address)); }
  std::string repr(int indent, std::set<std::string> &memo) { return std::string("{\"*\": ") + walker->repr(indent, memo) + std::string("}"); }
  std::string avroTypeName() { return "union"; }
  void printJSON(void *address) {
    void *dereferenced = *((void**)address);
    if (dereferenced == nullptr)
      std::cout << "null";
    else {
      std::cout << "{\"" << walker->avroTypeName() << "\": ";
      walker->printJSON(dereferenced);
      std::cout << "\"}";
    }
  }
};

class TRefWalker : public FieldWalker {
public:
  TRefWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) : FieldWalker(fieldName, "*"), walker(nullptr) { }
  FieldWalker *walker;
  bool empty() { return false; }
  bool resolved() { return false; }   // stub
  void resolve(void *address) { }   // stub
  std::string repr(int indent, std::set<std::string> &memo) { return std::string("{\"TRef\": ") + (resolved() ? walker->repr(indent, memo) : std::string("\"?\"")) + std::string("}"); }
  std::string avroTypeName() { return "string"; }   // FIXME: can the reference be missing?
  void printJSON(void *address) { std::cout << "TREF"; }   // stub
};

class StdVectorWalker : public FieldWalker {
public:
  StdVectorWalker(std::string fieldName, FieldWalker *walker) : FieldWalker(fieldName, "vector"), walker(walker) { }
  FieldWalker *walker;
  bool empty() { return false; }
  bool resolved() { return walker->resolved(); }
  void resolve(void *address) { walker->resolve(address); }
  std::string repr(int indent, std::set<std::string> &memo) { return std::string("{\"std::vector\": ") + walker->repr(indent, memo) + std::string("}"); }
  std::string avroTypeName() { return "array"; }
  void printJSON(void *address) { std::cout << "STD-VECTOR"; }   // stub
};

class ArrayWalker : public FieldWalker {
public:
  ArrayWalker(std::string fieldName, FieldWalker *walker, int numItems, size_t byteWidth) : FieldWalker(fieldName, "[]"), walker(walker), numItems(numItems), byteWidth(byteWidth) { }
  FieldWalker *walker;
  int numItems;
  size_t byteWidth;
  bool empty() { return false; }
  bool resolved() { return walker->resolved(); }
  void resolve(void *address) { walker->resolve(address); }
  std::string repr(int indent, std::set<std::string> &memo) {
    return std::string("{\"[]\": {\"numItems\": ") + std::to_string(numItems) + std::string(", \"byteWidth\": ") + std::to_string(byteWidth) + std::string(", \"type\": ") + walker->repr(indent, memo) + std::string("}}");
  }
  std::string avroTypeName() { return "array"; }
  void printJSON(void *address) {
    std::cout << "[";
    void *ptr = address;
    bool first = true;
    for (int i = 0;  i < numItems;  i++) {
      if (first) first = false; else std::cout << ", ";
      walker->printJSON(ptr);
      ptr = (void*)((size_t)ptr + byteWidth);
    }
    std::cout << "]";
  }
};

class TObjArrayWalker : public FieldWalker {
public:
  TObjArrayWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) : FieldWalker(fieldName, "TObjArray"), avroNamespace(avroNamespace), defs(defs), walker(nullptr) { }
  std::string avroNamespace;
  std::map<const std::string, ClassWalker*> &defs;
  FieldWalker *walker;
  bool empty() { return false; }
  bool resolved() { return false; }   // stub
  void resolve(void *address) { }   // stub
  std::string repr(int indent, std::set<std::string> &memo) {
    return std::string("{\"TObjArray\": ") + (resolved() ? walker->repr(indent, memo) : std::string("\"?\"")) + std::string("}");
  }
  std::string avroTypeName() { return "array"; }
  void printJSON(void *address) { std::cout << "TOBJARRAY"; }   // stub
};

class TRefArrayWalker : public FieldWalker {
public:
  TRefArrayWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) :
    FieldWalker(fieldName, "TRefArray"), avroNamespace(avroNamespace), defs(defs), walker(nullptr) { }
  std::string avroNamespace;
  std::map<const std::string, ClassWalker*> &defs;
  FieldWalker *walker;
  bool empty() { return false; }
  bool resolved() { return false; }   // stub
  void resolve(void *address) { }   // stub
  std::string repr(int indent, std::set<std::string> &memo) {
    return std::string("{\"TRefArray\": ") + (resolved() ? walker->repr(indent, memo) : std::string("\"?\"")) + std::string("}");
  }
  std::string avroTypeName() { return "array"; }
  void printJSON(void *address) { std::cout << "TREFARRAY"; }   // stub
};

class TClonesArrayWalker : public FieldWalker {
public:
  TClonesArrayWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) :
    FieldWalker(fieldName, "TClonesArray"), avroNamespace(avroNamespace), defs(defs), walker(nullptr) { }
  std::string avroNamespace;
  std::map<const std::string, ClassWalker*> &defs;
  FieldWalker *walker;
  bool empty() { return resolved() ? walker->empty() : false; }
  bool resolved() { return walker != nullptr; }   // stub
  void resolve(void *address) {
    walker = new ClassWalker(fieldName, ((TClonesArray*)address)->GetClass(), avroNamespace, defs);
    ((ClassWalker*)walker)->fill();
  }
  std::string repr(int indent, std::set<std::string> &memo) {
    return std::string("{\"TClonesArray\": ") + (resolved() ? walker->repr(indent, memo) : std::string("\"?\"")) + std::string("}");
  }
  std::string avroTypeName() { return "array"; }
  void printJSON(void *address) {
    if (!resolved()) resolve(address);
    if (!resolved()) throw std::invalid_argument(std::string("could not resolve TClonesArray"));
    std::cout << "[";
    TIter nextItem = (TClonesArray*)address;
    bool first = true;
    for (void *item = (void*)nextItem();  item != nullptr;  item = (void*)nextItem()) {
      if (first) first = false; else std::cout << ", ";
      walker->printJSON(item);
    }
    std::cout << "]";
  }
};

class ExtractorInterface {
public:
  virtual void *getAddress() = 0;
};

class ExtractableWalker : public FieldWalker {
public:
  ExtractableWalker(std::string fieldName, std::string typeName) : FieldWalker(fieldName, typeName) { }
  bool empty() { return false; }
  virtual void *getAddress() = 0;
};

class LeafWalker : public ExtractableWalker {
public:
  LeafWalker(TLeaf *tleaf, TTree *ttree);
  std::vector<int> dims;
  std::string determineType(TLeaf *tleaf);
  FieldWalker *walker;
  bool resolved() { return true; }
  void resolve(void *address) { }
  std::string repr(int indent, std::set<std::string> &memo) {
    return std::string("\"") + fieldName + std::string("\": {\"extractor\": \"TLeaf\", \"type\": ") + walker->repr(indent, memo) + std::string("}");
  }
  std::string avroTypeName() { return walker->avroTypeName(); }
  void printJSON(void *address) { }   // stub
  void *getAddress() { return nullptr; }
};

class ReaderValueWalker : public ExtractableWalker {
public:
  ReaderValueWalker(std::string fieldName, TBranch *tbranch, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  FieldWalker *walker;
  CallFunc_t *extractorMethod;
  ExtractorInterface *extractorInstance;
  bool resolved() { return walker->resolved(); }
  void resolve(void *address) { walker->resolve(address); }
  std::string repr(int indent, std::set<std::string> &memo) {
    return std::string("\"") + fieldName + std::string("\": {\"extractor\": \"TTreeReaderValue\", \"type\": ") + walker->repr(indent, memo) + std::string("}");
  }
  std::string avroTypeName() { return walker->avroTypeName(); }
  void printJSON(void *address);
  void *getAddress();
};

class ReaderArrayWalker : public ExtractableWalker {
public:
  ReaderArrayWalker(std::string fieldName, TBranch *tbranch, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  FieldWalker *walker;
  bool resolved() { return walker->resolved(); }
  void resolve(void *address) { walker->resolve(address); }
  std::string repr(int indent, std::set<std::string> &memo) {
    return std::string("\"") + fieldName + std::string("\": {\"extractor\": \"TTreeReaderArray\", \"type\": ") + walker->repr(indent, memo) + std::string("}");
  }
  std::string avroTypeName() { return "array"; }
  void printJSON(void *address) { }   // stub
  void *getAddress() { return nullptr; }
};

class TreeWalker {
public:
  TreeWalker(TTree *ttree, std::string avroNamespace = "");
  std::map<const std::string, ClassWalker*> defs;
  std::vector<ExtractableWalker*> fields;
  bool resolved() {
    for (auto iter = fields.begin();  iter != fields.end();  ++iter)
      if (!(*iter)->resolved())
        return false;
    return true;
  }
  void resolve() {
    for (auto iter = fields.begin();  iter != fields.end();  ++iter)
      (*iter)->resolve((*iter)->getAddress());
  }
  std::string repr() {
    std::set<std::string> memo;
    std::string out;
    out += std::string("{");
    bool first = true;
    for (auto iter = fields.begin();  iter != fields.end();  ++iter) {
      if (first) first = false; else out += std::string(",\n") + std::string(1, ' ');
      out += (*iter)->repr(1, memo);
    }
    out += std::string("}");
    return out;
  }
  void printJSON() {
    std::cout << "{";
    bool first = true;
    for (auto iter = fields.begin();  iter != fields.end();  ++iter) {
      if (first) first = false; else std::cout << ", ";
      (*iter)->printJSON((*iter)->getAddress());
    }
    std::cout << "}" << std::endl;
  }
};

#endif // DATAWALKER_H
