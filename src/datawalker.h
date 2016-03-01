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

class ClassWalker;

class FieldWalker {
public:
  FieldWalker(std::string fieldName, std::string typeName) : fieldName(fieldName), typeName(typeName) { }
  std::string fieldName;
  std::string typeName;
};

class PrimitiveWalker : public FieldWalker {
public:
  PrimitiveWalker(std::string fieldName, std::string typeName) : FieldWalker(fieldName, typeName) { }
};

class BoolWalker : public PrimitiveWalker {
public:
  BoolWalker(std::string fieldName) : PrimitiveWalker(fieldName, "bool") { }
};

class CharWalker : public PrimitiveWalker {
public:
  CharWalker(std::string fieldName) : PrimitiveWalker(fieldName, "char") { }
};

class UCharWalker : public PrimitiveWalker {
public:
  UCharWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned char") { }
};

class ShortWalker : public PrimitiveWalker {
public:
  ShortWalker(std::string fieldName) : PrimitiveWalker(fieldName, "short") { }
};

class UShortWalker : public PrimitiveWalker {
public:
  UShortWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned short") { }
};

class IntWalker : public PrimitiveWalker {
public:
  IntWalker(std::string fieldName) : PrimitiveWalker(fieldName, "int") { }
};

class UIntWalker : public PrimitiveWalker {
public:
  UIntWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned int") { }
};

class LongWalker : public PrimitiveWalker {
public:
  LongWalker(std::string fieldName) : PrimitiveWalker(fieldName, "long") { }
};

class ULongWalker : public PrimitiveWalker {
public:
  ULongWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned long") { }
};

class FloatWalker : public PrimitiveWalker {
public:
  FloatWalker(std::string fieldName) : PrimitiveWalker(fieldName, "float") { }
};

class DoubleWalker : public PrimitiveWalker {
public:
  DoubleWalker(std::string fieldName) : PrimitiveWalker(fieldName, "double") { }
};

class CStringWalker : public PrimitiveWalker {
public:
  CStringWalker(std::string fieldName) : PrimitiveWalker(fieldName, "char*") { }
};

class StdStringWalker : public PrimitiveWalker {
public:
  StdStringWalker(std::string fieldName) : PrimitiveWalker(fieldName, "string") { }
};

class TStringWalker : public PrimitiveWalker {
public:
  TStringWalker(std::string fieldName) : PrimitiveWalker(fieldName, "TString") { }
};

class PointerWalker : public FieldWalker {
public:
  PointerWalker(std::string fieldName, FieldWalker *walker) : FieldWalker(fieldName, "*"), walker(walker) { }
  FieldWalker *walker;
};

class StdVectorWalker : public FieldWalker {
public:
  StdVectorWalker(std::string fieldName, FieldWalker *walker) : FieldWalker(fieldName, "vector"), walker(walker) { }
  FieldWalker *walker;
};

class ArrayWalker : public FieldWalker {
public:
  ArrayWalker(std::string fieldName, int maxIndex) : FieldWalker(fieldName, "[]"), walker(walker), maxIndex(maxIndex) { }
  FieldWalker *walker;
  int maxIndex;
};

class MemberWalker : public FieldWalker {
public:
  MemberWalker(TDataMember *dataMember, std::map<const std::string, ClassWalker*> &classes);
  size_t offset;
  FieldWalker *walker;
  FieldWalker *specializedWalker(std::string fieldName, std::string innerTypeName, std::map<const std::string, ClassWalker*> &classes);
};

class ClassWalker : public FieldWalker {
public:
  ClassWalker(std::string fieldName, TClass *tclass, std::map<const std::string, ClassWalker*> &classes);
  std::vector<MemberWalker*> members;
};

class LeafWalker : public FieldWalker {
public:
  LeafWalker(TLeaf *tleaf, TTree *ttree);
  std::vector<int> dims;
  std::string determineType(TLeaf *tleaf);
};

class ReaderValueWalker : public FieldWalker {
public:
  ReaderValueWalker(std::string fieldName, TBranch *tbranch, std::map<const std::string, ClassWalker*> &classes);
  FieldWalker *walker;
};

class ReaderArrayWalker : public FieldWalker {
public:
  ReaderArrayWalker(std::string fieldName, TBranch *tbranch, std::map<const std::string, ClassWalker*> &classes);
  FieldWalker *walker;
};

class TreeWalker {
public:
  TreeWalker(TTree *ttree);
  std::vector<FieldWalker*> fields;
};

#endif // DATAWALKER_H