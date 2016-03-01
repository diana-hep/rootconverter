#ifndef DATAWALKER_H
#define DATAWALKER_H

#include <iostream>
#include <map>
#include <vector>
#include <string>

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
  virtual std::string repr(int indent) = 0;
  virtual void printJSON(void *address) = 0;
};

class PrimitiveWalker : public FieldWalker {
public:
  PrimitiveWalker(std::string fieldName, std::string typeName) : FieldWalker(fieldName, typeName) { }
  bool empty() { return false; };
  std::string repr(int indent) { return std::string("\"") + typeName + std::string("\""); }
};

class BoolWalker : public PrimitiveWalker {
public:
  BoolWalker(std::string fieldName) : PrimitiveWalker(fieldName, "bool") { }
  void printJSON(void *address) { if (*((bool*)address)) std::cout << "true"; else std::cout << "false"; }
};

class CharWalker : public PrimitiveWalker {
public:
  CharWalker(std::string fieldName) : PrimitiveWalker(fieldName, "char") { }
  void printJSON(void *address) { std::cout << (int)(*((char*)address)); }
};

class UCharWalker : public PrimitiveWalker {
public:
  UCharWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned char") { }
  void printJSON(void *address) { std::cout << (int)(*((unsigned char*)address)); }
};

class ShortWalker : public PrimitiveWalker {
public:
  ShortWalker(std::string fieldName) : PrimitiveWalker(fieldName, "short") { }
  void printJSON(void *address) { std::cout << *((short*)address); }
};

class UShortWalker : public PrimitiveWalker {
public:
  UShortWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned short") { }
  void printJSON(void *address) { std::cout << *((unsigned short*)address); }
};

class IntWalker : public PrimitiveWalker {
public:
  IntWalker(std::string fieldName) : PrimitiveWalker(fieldName, "int") { }
  void printJSON(void *address) { std::cout << *((int*)address); }
};

class UIntWalker : public PrimitiveWalker {
public:
  UIntWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned int") { }
  void printJSON(void *address) { std::cout << *((unsigned int*)address); }
};

class LongWalker : public PrimitiveWalker {
public:
  LongWalker(std::string fieldName) : PrimitiveWalker(fieldName, "long") { }
  void printJSON(void *address) { std::cout << *((long*)address); }
};

class ULongWalker : public PrimitiveWalker {
public:
  ULongWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned long") { }
  void printJSON(void *address) { std::cout << *((unsigned long*)address); }
};

class FloatWalker : public PrimitiveWalker {
public:
  FloatWalker(std::string fieldName) : PrimitiveWalker(fieldName, "float") { }
  void printJSON(void *address) { std::cout << *((float*)address); }
};

class DoubleWalker : public PrimitiveWalker {
public:
  DoubleWalker(std::string fieldName) : PrimitiveWalker(fieldName, "double") { }
  void printJSON(void *address) { std::cout << *((double*)address); }
};

class AnyStringWalker : public FieldWalker {
public:
  AnyStringWalker(std::string fieldName, std::string typeName) : FieldWalker(fieldName, typeName) { }
  bool empty() { return false; };
  std::string repr(int indent) { return std::string("\"") + typeName + std::string("\""); }
  std::string escapeJSON(std::string string) { return string; }
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
  std::string repr(int indent) { return std::string("{\"*\": ") + walker->repr(indent) + std::string("}"); }
  void printJSON(void *address) { walker->printJSON(*((void**)address)); }
};

class TRefWalker : public FieldWalker {
public:
  TRefWalker(std::string fieldName, std::map<const std::string, ClassWalker*> &classes) : FieldWalker(fieldName, "*"), walker(nullptr) { }
  FieldWalker *walker;
  bool empty() { return false; };
  std::string repr(int indent) { return std::string("{\"TRef\": ") + (walker == nullptr ? std::string("\"?\"") : walker->repr(indent)) + std::string("}"); }
  void printJSON(void *address) { std::cout << "TREF"; }   // stub
};

class StdVectorWalker : public FieldWalker {
public:
  StdVectorWalker(std::string fieldName, FieldWalker *walker) : FieldWalker(fieldName, "vector"), walker(walker) { }
  FieldWalker *walker;
  bool empty() { return false; };
  std::string repr(int indent) { return std::string("{\"std::vector\": ") + walker->repr(indent) + std::string("}"); }
  void printJSON(void *address) { std::cout << "STD-VECTOR"; }   // stub
};

class ArrayWalker : public FieldWalker {
public:
  ArrayWalker(std::string fieldName, FieldWalker *walker, int maxIndex) : FieldWalker(fieldName, "[]"), walker(walker), maxIndex(maxIndex) { }
  int maxIndex;
  FieldWalker *walker;
  bool empty() { return false; };
  std::string repr(int indent) { return std::string("{\"[]\": {\"type\": ") + walker->repr(indent) + std::string(", \"size\": ") + std::to_string(maxIndex) + std::string("}}"); }
  void printJSON(void *address) { std::cout << "ARRAY"; }   // stub
};

class TObjArrayWalker : public FieldWalker {
public:
  TObjArrayWalker(std::string fieldName, std::map<const std::string, ClassWalker*> &classes) : FieldWalker(fieldName, "TObjArray"), classes(classes), walker(nullptr) { }
  std::map<const std::string, ClassWalker*> &classes;
  FieldWalker *walker;
  bool empty() { return false; };
  std::string repr(int indent) { return std::string("{\"TObjArray\": ") + (walker == nullptr ? std::string("\"?\"") : walker->repr(indent)) + std::string("}"); }
  void printJSON(void *address) { std::cout << "TOBJARRAY"; }   // stub
};

class TRefArrayWalker : public FieldWalker {
public:
  TRefArrayWalker(std::string fieldName, std::map<const std::string, ClassWalker*> &classes) : FieldWalker(fieldName, "TRefArray"), classes(classes), walker(nullptr) { }
  std::map<const std::string, ClassWalker*> &classes;
  FieldWalker *walker;
  bool empty() { return false; };
  std::string repr(int indent) { return std::string("{\"TRefArray\": ") + (walker == nullptr ? std::string("\"?\"") : walker->repr(indent)) + std::string("}"); }
  void printJSON(void *address) { std::cout << "TREFARRAY"; }   // stub
};

class TClonesArrayWalker : public FieldWalker {
public:
  TClonesArrayWalker(std::string fieldName, std::map<const std::string, ClassWalker*> &classes) : FieldWalker(fieldName, "TClonesArray"), classes(classes), walker(nullptr), maxIndex(-1) { }
  std::map<const std::string, ClassWalker*> &classes;
  FieldWalker *walker;
  int maxIndex;
  bool empty() { return false; };
  std::string repr(int indent) { return std::string("{\"TClonesArray\": {\"type\": ") + (walker == nullptr ? std::string("\"?\"") : walker->repr(indent)) + std::string(", \"size\": ") + (maxIndex >= 0 ? std::to_string(maxIndex) : std::string("\"?\"")) + std::string("}}"); }
  void printJSON(void *address) { std::cout << "TCLONESARRAY"; }   // stub
};

class MemberWalker : public FieldWalker {
public:
  MemberWalker(TDataMember *dataMember, std::map<const std::string, ClassWalker*> &classes);
  size_t offset;
  FieldWalker *walker;
  FieldWalker *specializedWalker(std::string fieldName, std::string innerTypeName, std::map<const std::string, ClassWalker*> &classes);
  bool empty() { return walker->empty(); };
  std::string repr(int indent) { return std::string("\"") + fieldName + std::string("\": ") + walker->repr(indent); }
  void printJSON(void *address);
};

class ClassWalker : public FieldWalker {
public:
  ClassWalker(std::string fieldName, TClass *tclass, std::map<const std::string, ClassWalker*> &classes);
  TClass *tclass;
  std::vector<MemberWalker*> members;
  bool empty() { return members.empty(); }
  std::string repr(int indent) {
    std::string out;
    out += std::string("{\n") + std::string(indent + 2, ' ');
    bool first = true;
    for (auto iter = members.begin();  iter != members.end();  ++iter) {
      if (first) first = false; else out += std::string(",\n") + std::string(indent + 2, ' ');
      out += (*iter)->repr(indent + 2);
    }
    out += std::string("\n") + std::string(indent, ' ') + std::string("}");
    return out;
  }
  void printJSON(void *address);
};

class ExtractorInterface {
public:
  virtual void *getAddress() = 0;
};

class ExtractableWalker : public FieldWalker {
public:
  ExtractableWalker(std::string fieldName, std::string typeName) : FieldWalker(fieldName, typeName) { }
  bool empty() { return false; };
  virtual void *getAddress() = 0;
};

class LeafWalker : public ExtractableWalker {
public:
  LeafWalker(TLeaf *tleaf, TTree *ttree);
  std::vector<int> dims;
  std::string determineType(TLeaf *tleaf);
  FieldWalker *walker;
  std::string repr(int indent) { return std::string("\"") + fieldName + std::string("\": {\"extractor\": \"TLeaf\", \"type\": ") + walker->repr(indent) + std::string("}"); }
  void printJSON(void *address) { }   // stub
  void *getAddress() { return nullptr; }
};

class ReaderValueWalker : public ExtractableWalker {
public:
  ReaderValueWalker(std::string fieldName, TBranch *tbranch, std::map<const std::string, ClassWalker*> &classes);
  FieldWalker *walker;
  CallFunc_t *extractorMethod;
  ExtractorInterface *extractorInstance;
  std::string repr(int indent) { return std::string("\"") + fieldName + std::string("\": {\"extractor\": \"TTreeReaderValue\", \"type\": ") + walker->repr(indent) + std::string("}"); }
  void printJSON(void *address);
  void *getAddress();
};

class ReaderArrayWalker : public ExtractableWalker {
public:
  ReaderArrayWalker(std::string fieldName, TBranch *tbranch, std::map<const std::string, ClassWalker*> &classes);
  FieldWalker *walker;
  std::string repr(int indent) { return std::string("\"") + fieldName + std::string("\": {\"extractor\": \"TTreeReaderArray\", \"type\": ") + walker->repr(indent) + std::string("}"); }
  void printJSON(void *address) { }   // stub
  void *getAddress() { return nullptr; }
};

class TreeWalker {
public:
  TreeWalker(TTree *ttree);
  std::vector<ExtractableWalker*> fields;
  std::string repr() {
    std::string out;
    out += std::string("{");
    bool first = true;
    for (auto iter = fields.begin();  iter != fields.end();  ++iter) {
      if (first) first = false; else out += std::string(",\n") + std::string(1, ' ');
      out += (*iter)->repr(1);
    }
    out += std::string("}");
    return out;
  }
  void printJSON();
};

#endif // DATAWALKER_H
