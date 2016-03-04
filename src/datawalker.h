#ifndef DATAWALKER_H
#define DATAWALKER_H

// C++ includes
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>

// ROOT includes
#include <TClass.h>
#include <TClonesArray.h>
#include <TDataMember.h>
#include <TDictionary.h>
#include <TLeafB.h>
#include <TLeafC.h>
#include <TLeafD.h>
#include <TLeafElement.h>
#include <TLeafF.h>
#include <TLeaf.h>
#include <TLeafI.h>
#include <TLeafL.h>
#include <TLeafObject.h>
#include <TLeafO.h>
#include <TLeafS.h>
#include <TList.h>
#include <TObjArray.h>
#include <TRefArray.h>
#include <TRef.h>
#include <TString.h>
#include <TTree.h>
#include <TTreeReaderArray.h>
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
  void printEscapedString(const char *string, std::ostream &stream);
  std::string escapedString(const char *string);
  virtual size_t sizeOf() = 0;
  virtual const std::type_info *typeId() = 0;
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
  virtual void printJSON(void *address) = 0;
  virtual void printJSON(TTreeReaderArrayBase *readerArrayBase, int i) = 0;
  virtual TTreeReaderValueBase *readerValue() = 0;
  virtual TTreeReaderArrayBase *readerArray() = 0;
};

class BoolWalker : public PrimitiveWalker {
public:
  BoolWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue();
  TTreeReaderArrayBase *readerArray();
};

class CharWalker : public PrimitiveWalker {
public:
  CharWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue();
  TTreeReaderArrayBase *readerArray();
};

class UCharWalker : public PrimitiveWalker {
public:
  UCharWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue();
  TTreeReaderArrayBase *readerArray();
};

class ShortWalker : public PrimitiveWalker {
public:
  ShortWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue();
  TTreeReaderArrayBase *readerArray();
};

class UShortWalker : public PrimitiveWalker {
public:
  UShortWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue();
  TTreeReaderArrayBase *readerArray();
};

class IntWalker : public PrimitiveWalker {
public:
  IntWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue();
  TTreeReaderArrayBase *readerArray();
  int value(TTreeReaderValueBase *readerValue);
};

class UIntWalker : public PrimitiveWalker {
public:
  UIntWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue();
  TTreeReaderArrayBase *readerArray();
};

class LongWalker : public PrimitiveWalker {
public:
  LongWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue();
  TTreeReaderArrayBase *readerArray();
};

class ULongWalker : public PrimitiveWalker {
public:
  ULongWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue();
  TTreeReaderArrayBase *readerArray();
};

class FloatWalker : public PrimitiveWalker {
public:
  FloatWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue();
  TTreeReaderArrayBase *readerArray();
};

class DoubleWalker : public PrimitiveWalker {
public:
  DoubleWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue();
  TTreeReaderArrayBase *readerArray();
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
  size_t sizeOf();
  const std::type_info *typeId();
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue();
  TTreeReaderArrayBase *readerArray();
};

class StdStringWalker : public AnyStringWalker {
public:
  StdStringWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue();
  TTreeReaderArrayBase *readerArray();
};

class TStringWalker : public AnyStringWalker {
public:
  TStringWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  void printJSON(void *address);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue();
  TTreeReaderArrayBase *readerArray();
};

///////////////////////////////////////////////////////////////////// MemberWalker

class MemberWalker : public FieldWalker {
public:
  size_t offset;
  FieldWalker *walker;
  std::string comment;

  MemberWalker(TDataMember *dataMember, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  static FieldWalker *specializedWalker(std::string fieldName, std::string typeName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  size_t sizeOf();
  const std::type_info *typeId();
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
private:
  size_t sizeOf_;
  const std::type_info *typeId_;
public:
  TClass *tclass;
  std::string avroNamespace;
  std::map<const std::string, ClassWalker*> &defs;
  std::vector<MemberWalker*> members;

  ClassWalker(std::string fieldName, TClass *tclass, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  void fill();    // has side-effects, must be called soon after constructor
  size_t sizeOf();
  const std::type_info *typeId();
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
  size_t sizeOf();
  const std::type_info *typeId();
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
  size_t sizeOf();
  const std::type_info *typeId();
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void printJSON(void *address);
};

///////////////////////////////////////////////////////////////////// StdVectorWalker

class StdVectorWalker : public FieldWalker {
private:
  const std::type_info *typeId_;
public:
  FieldWalker *walker;

  StdVectorWalker(std::string fieldName, std::string typeName, FieldWalker *walker);
  size_t sizeOf();
  const std::type_info *typeId();
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void printJSON(void *address);
};

///////////////////////////////////////////////////////////////////// StdVectorBoolWalker

class StdVectorBoolWalker : public FieldWalker {
public:
  FieldWalker *walker;

  StdVectorBoolWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
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

  ArrayWalker(std::string fieldName, FieldWalker *walker, int numItems);
  size_t sizeOf();
  const std::type_info *typeId();
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
  size_t sizeOf();
  const std::type_info *typeId();
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
  size_t sizeOf();
  const std::type_info *typeId();
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
  size_t sizeOf();
  const std::type_info *typeId();
  bool empty();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void printJSON(void *address);
};

///////////////////////////////////////////////////////////////////// ExtractableWalker

class ExtractableWalker : public FieldWalker {
public:
  ExtractableWalker(std::string fieldName, std::string typeName);
  bool empty();
  virtual void *getAddress() = 0;
};

///////////////////////////////////////////////////////////////////// LeafWalker

class LeafDimension {
private:
  LeafDimension *next_;
  int size_;
  IntWalker *counter;
  TTreeReaderValueBase *counterReaderValue;
public:
  LeafDimension(LeafDimension *next, int size);
  LeafDimension(LeafDimension *next, IntWalker *counter);
  std::string repr();
  LeafDimension *next();   // linked list makes the recursive function in LeafWalker easier to understand
  int size();
  int flatSize();
};

class LeafWalker : public ExtractableWalker {
public:
  PrimitiveWalker *walker;
  TTreeReaderValueBase *readerValue;
  TTreeReaderArrayBase *readerArray;
  int dimensions;
  LeafDimension *dims;

  LeafWalker(TLeaf *tleaf, TTree *ttree);
  PrimitiveWalker *leafToPrimitive(TLeaf *tleaf);
  size_t sizeOf();
  const std::type_info *typeId();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  int printJSONDeep(int readerIndex, int readerSize, LeafDimension *dim);
  void printJSON(void *address);
  void *getAddress();
};

///////////////////////////////////////////////////////////////////// ReaderValueWalker

class GenericReaderValue: public TTreeReaderValueBase {
public:
  std::string typeName;
  GenericReaderValue();
  GenericReaderValue(TTreeReader& tr, std::string fieldName, std::string typeName, FieldWalker *walker);
  const char *GetDerivedTypeName() const;
};

class ReaderValueWalker : public ExtractableWalker {
public:
  FieldWalker *walker;
  GenericReaderValue value;

  ReaderValueWalker(std::string fieldName, TBranch *tbranch, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  size_t sizeOf();
  const std::type_info *typeId();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void printJSON(void *address);
  void *getAddress();
};

///////////////////////////////////////////////////////////////////// RawTBranchWalker

class RawTBranchWalker : public ExtractableWalker {
public:
  TBranch *tbranch;
  FieldWalker *walker;

  RawTBranchWalker(std::string fieldName, std::string typeName, FieldWalker *walker);
  size_t sizeOf();
  const std::type_info *typeId();
  bool resolved();
  void resolve(void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void printJSON(void *address);
};

class RawTBranchStdStringWalker : public RawTBranchWalker {
public:
  std::string *data;
  RawTBranchStdStringWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  void *getAddress();
};

class RawTBranchTStringWalker : public RawTBranchWalker {
public:
  TString *data;
  RawTBranchTStringWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  void *getAddress();
};

///////////////////////////////////////////////////////////////////// TreeWalker

class TreeWalker {
public:
  std::map<const std::string, ClassWalker*> defs;
  std::vector<ExtractableWalker*> fields;
  std::string avroNamespace;

  TreeWalker(std::string avroNamespace = "");
  void reset(TTreeReader *reader);

  bool resolved();
  void resolve();
  std::string repr();
  std::string avroSchema();
  void printJSON();
};

#endif // DATAWALKER_H
