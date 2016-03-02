#ifndef DATAWALKER_H
#define DATAWALKER_H

// C++ includes
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <set>

// ROOT includes
#include <TTree.h>
#include <TLeaf.h>
#include <TClass.h>
#include <TDataMember.h>
#include <TInterpreter.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

using namespace ROOT::Internal;

//// forwards
class ClassWalker;

///////////////////////////////////////////////////////////////////// FieldWalker

class FieldWalker {
public:
  std::string fieldName;
  std::string typeName;
  FieldWalker(std::string fieldName, std::string typeName);
  std::string escapeJSON(std::string string);
  virtual bool empty() = 0;
  virtual bool resolved() = 0;
  virtual void resolve(void *address) = 0;
  virtual std::string repr(int indent, std::set<std::string> &memo) = 0;
  virtual std::string avroTypeName() = 0;
  virtual std::string avroSchema(int indent, std::set<std::string> &memo) = 0;
  virtual void printJSON(void *address) = 0;
};

///////////////////////////////////////////////////////////////////// PrimitiveWalkers

class PrimitiveWalker : public FieldWalker {
public:
  PrimitiveWalker(std::string fieldName, std::string typeName);
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroSchema(int indent, std::set<std::string> &memo);
  virtual TTreeReaderValueBase *readerValue(std::string name) = 0;
};

class BoolWalker : public PrimitiveWalker {
public:
  BoolWalker(std::string fieldName);
  std::string avroTypeName();
  void printJSON(void *address);
  TTreeReaderValueBase *readerValue(std::string name);
};

class CharWalker : public PrimitiveWalker {
public:
  CharWalker(std::string fieldName);
  std::string avroTypeName();
  void printJSON(void *address);
  TTreeReaderValueBase *readerValue(std::string name);
};

class UCharWalker : public PrimitiveWalker {
public:
  UCharWalker(std::string fieldName);
  std::string avroTypeName();
  void printJSON(void *address);
  TTreeReaderValueBase *readerValue(std::string name);
};

class ShortWalker : public PrimitiveWalker {
public:
  ShortWalker(std::string fieldName);
  std::string avroTypeName();
  void printJSON(void *address);
  TTreeReaderValueBase *readerValue(std::string name);
};

class UShortWalker : public PrimitiveWalker {
public:
  UShortWalker(std::string fieldName);
  std::string avroTypeName();
  void printJSON(void *address);
  TTreeReaderValueBase *readerValue(std::string name);
};

class IntWalker : public PrimitiveWalker {
public:
  IntWalker(std::string fieldName);
  std::string avroTypeName();
  void printJSON(void *address);
  TTreeReaderValueBase *readerValue(std::string name);
};

class UIntWalker : public PrimitiveWalker {
public:
  UIntWalker(std::string fieldName);
  std::string avroTypeName();
  void printJSON(void *address);
  TTreeReaderValueBase *readerValue(std::string name);
};

class LongWalker : public PrimitiveWalker {
public:
  LongWalker(std::string fieldName);
  std::string avroTypeName();
  void printJSON(void *address);
  TTreeReaderValueBase *readerValue(std::string name);
};

class ULongWalker : public PrimitiveWalker {
public:
  ULongWalker(std::string fieldName);
  std::string avroTypeName();
  void printJSON(void *address);
  TTreeReaderValueBase *readerValue(std::string name);
};

class FloatWalker : public PrimitiveWalker {
public:
  FloatWalker(std::string fieldName);
  std::string avroTypeName();
  void printJSON(void *address);
  TTreeReaderValueBase *readerValue(std::string name);
};

class DoubleWalker : public PrimitiveWalker {
public:
  DoubleWalker(std::string fieldName);
  std::string avroTypeName();
  void printJSON(void *address);
  TTreeReaderValueBase *readerValue(std::string name);
};

///////////////////////////////////////////////////////////////////// AnyStringWalkers

class AnyStringWalker : public PrimitiveWalker {
public:
  AnyStringWalker(std::string fieldName, std::string typeName);
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
};

class CStringWalker : public AnyStringWalker {
public:
  CStringWalker(std::string fieldName);
  void printJSON(void *address);
  TTreeReaderValueBase *readerValue(std::string name);
};

class StdStringWalker : public AnyStringWalker {
public:
  StdStringWalker(std::string fieldName);
  void printJSON(void *address);
  TTreeReaderValueBase *readerValue(std::string name);
};

class TStringWalker : public AnyStringWalker {
public:
  TStringWalker(std::string fieldName);
  void printJSON(void *address);
  TTreeReaderValueBase *readerValue(std::string name);
};

///////////////////////////////////////////////////////////////////// MemberWalker

class MemberWalker : public FieldWalker {
public:
  size_t offset;
  FieldWalker *walker;
  std::string comment;

  MemberWalker(TDataMember *dataMember, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  FieldWalker *specializedWalker(std::string fieldName, std::string innerTypeName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void printJSON(void *address);
};

///////////////////////////////////////////////////////////////////// ClassWalker

class ClassWalker : public FieldWalker {
public:
  TClass *tclass;
  std::string avroNamespace;
  std::map<const std::string, ClassWalker*> &defs;
  std::vector<MemberWalker*> members;

  ClassWalker(std::string fieldName, TClass *tclass, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  void fill();    // has side-effects, must be called soon after constructor
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void printJSON(void *address);
};

///////////////////////////////////////////////////////////////////// PointerWalker

class PointerWalker : public FieldWalker {
public:
  FieldWalker *walker;

  PointerWalker(std::string fieldName, FieldWalker *walker);
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void printJSON(void *address);
};

///////////////////////////////////////////////////////////////////// TRefWalker

class TRefWalker : public FieldWalker {
public:
  FieldWalker *walker;

  TRefWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void printJSON(void *address);
};

///////////////////////////////////////////////////////////////////// StdVectorWalker

class StdVectorInterface {
public:
  virtual void start(void *vector) = 0;
  virtual void *next() = 0;
};

class StdVectorWalker : public FieldWalker {
public:
  FieldWalker *walker;
  StdVectorInterface *extractorInstance;

  StdVectorWalker(std::string fieldName, FieldWalker *walker);
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void printJSON(void *address);
};

///////////////////////////////////////////////////////////////////// ArrayWalker

class ArrayWalker : public FieldWalker {
public:
  FieldWalker *walker;
  int numItems;
  size_t byteWidth;

  ArrayWalker(std::string fieldName, FieldWalker *walker, int numItems, size_t byteWidth);
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void printJSON(void *address);
};

///////////////////////////////////////////////////////////////////// TObjArrayWalker

class TObjArrayWalker : public FieldWalker {
public:
  std::string avroNamespace;
  std::map<const std::string, ClassWalker*> &defs;
  FieldWalker *walker;
  TClass *classToAssert;

  TObjArrayWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void printJSON(void *address);
};

///////////////////////////////////////////////////////////////////// TRefArrayWalker

class TRefArrayWalker : public FieldWalker {
public:
  std::string avroNamespace;
  std::map<const std::string, ClassWalker*> &defs;
  FieldWalker *walker;

  TRefArrayWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void printJSON(void *address);
};

///////////////////////////////////////////////////////////////////// TClonesArrayWalker

class TClonesArrayWalker : public FieldWalker {
public:
  std::string avroNamespace;
  std::map<const std::string, ClassWalker*> &defs;
  FieldWalker *walker;

  TClonesArrayWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void printJSON(void *address);
};

///////////////////////////////////////////////////////////////////// ExtractableWalker

class ExtractorInterface {
public:
  virtual void *getAddress() = 0;
};

class ExtractableWalker : public FieldWalker {
public:
  ExtractableWalker(std::string fieldName, std::string typeName);
  bool empty();
  virtual void *getAddress() = 0;
};

///////////////////////////////////////////////////////////////////// LeafWalker

class LeafWalker : public ExtractableWalker {
public:
  std::vector<int> dims;
  PrimitiveWalker *walker;
  TTreeReaderValueBase *readerValue;

  LeafWalker(TLeaf *tleaf, TTree *ttree);
  PrimitiveWalker *leafToPrimitive(TLeaf *tleaf);
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void printJSON(void *address);
  void *getAddress();
};

///////////////////////////////////////////////////////////////////// ReaderValueWalker

class ReaderValueWalker : public ExtractableWalker {
public:
  FieldWalker *walker;
  CallFunc_t *extractorMethod;
  ExtractorInterface *extractorInstance;

  ReaderValueWalker(std::string fieldName, TBranch *tbranch, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void printJSON(void *address);
  void *getAddress();
};

///////////////////////////////////////////////////////////////////// ReaderArrayWalker

class ReaderArrayWalker : public ExtractableWalker {
public:
  FieldWalker *walker;

  ReaderArrayWalker(std::string fieldName, TBranch *tbranch, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void printJSON(void *address);
  void *getAddress();
};

///////////////////////////////////////////////////////////////////// TreeWalker

class TreeWalker {
public:
  std::map<const std::string, ClassWalker*> defs;
  std::vector<ExtractableWalker*> fields;
  std::string avroNamespace;

  TreeWalker(std::string avroNamespace = "");
  bool resolved();
  void resolve();
  std::string repr();
  std::string avroSchema();
  void printJSON();
};

#endif // DATAWALKER_H
