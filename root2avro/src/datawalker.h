// Copyright 2016 Jim Pivarski
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef DATAWALKER_H
#define DATAWALKER_H

// C includes
#include <time.h>

// C++ includes
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>

// Avro includes
#ifdef AVRO
#include <avro.h>
#endif

// ROOT includes
#include <TClass.h>
#include <TClonesArray.h>
#include <TDataMember.h>
#include <TDictionary.h>
#include <TFile.h>
#include <TInterpreter.h>
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
#include <TSystem.h>
#include <TTree.h>
#include <TTreeReaderArray.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

using namespace ROOT::Internal;
// using namespace ROOT;

// Must be kept in-sync with scaroot-flatreader/src/main/scala/org/dianahep/scaroot/flatreader.scala!
enum SchemaInstruction {
  SchemaBool           = 0,
  SchemaChar           = 1,
  SchemaUChar          = 2,
  SchemaShort          = 3,
  SchemaUShort         = 4,
  SchemaInt            = 5,
  SchemaUInt           = 6,
  SchemaLong           = 7,
  SchemaULong          = 8,
  SchemaFloat          = 9,
  SchemaDouble         = 10,
  SchemaString         = 11,

  SchemaClassName      = 12,
  SchemaClassPointer   = 13,
  SchemaClassFieldName = 14,
  SchemaClassFieldDoc  = 15,
  SchemaClassEnd       = 16,
  SchemaClassReference = 17,

  SchemaPointer        = 18,

  SchemaSequence       = 19,
};

typedef void (*SchemaBuilder)(SchemaInstruction schemaInstruction, const void *data);

class DataProvider {
public:
  virtual int getDataSize(const void *address) = 0;
  virtual const void *getData(const void *address, int index) = 0;
};

enum BufferStatus {
  StatusReading        = 0,
  StatusWriting        = 1,
  StatusTooSmall       = 2,
};

#define MAX_STRING_LENGTH 2147483647
#define MAX_SEQUENCE_LENGTH 2147483647

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
  virtual TDictionary *tdictionary();
  virtual bool empty() = 0;
  virtual bool resolved() = 0;
  virtual void resolve(const void *address) = 0;
  virtual std::string repr(int indent, std::set<std::string> &memo) = 0;
  virtual std::string avroTypeName() = 0;
  virtual std::string avroSchema(int indent, std::set<std::string> &memo) = 0;
  virtual void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) = 0;
  virtual void printJSON(void *address, std::ostream &stream) = 0;
#ifdef AVRO
  virtual bool printAvro(void *address, avro_value_t *avrovalue) = 0;
#endif
  virtual const void *unpack(const void *address) = 0;
  virtual void *copyToBuffer(void *ptr, void *limit, void *address) = 0;
};

///////////////////////////////////////////////////////////////////// PrimitiveWalkers

class PrimitiveWalker : public FieldWalker {
public:
  PrimitiveWalker(std::string fieldName, std::string typeName);
  bool empty();
  bool resolved();
  void resolve(const void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroSchema(int indent, std::set<std::string> &memo);
  virtual void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) = 0;
  virtual void printJSON(void *address, std::ostream &stream) = 0;
  virtual void printJSON(TTreeReaderArrayBase *readerArrayBase, int i, std::ostream &stream) = 0;
#ifdef AVRO
  virtual bool printAvro(void *address, avro_value_t *avrovalue) = 0;
  virtual bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue) = 0;
#endif
  virtual const void *unpack(const void *address) = 0;
  virtual const void *unpack(TTreeReaderArrayBase *readerArrayBase, int i) = 0;
  virtual void *copyToBuffer(void *ptr, void *limit, void *address) = 0;
  virtual void *copyToBuffer(void *ptr, void *limit, TTreeReaderArrayBase *readerArrayBase, int i) = 0;
  virtual TTreeReaderValueBase *readerValue(TTreeReader *reader) = 0;
  virtual TTreeReaderArrayBase *readerArray(TTreeReader *reader) = 0;
};

class BoolWalker : public PrimitiveWalker {
public:
  BoolWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  const void *unpack(TTreeReaderArrayBase *readerArrayBase, int i);
  void *copyToBuffer(void *ptr, void *limit, void *address);
  void *copyToBuffer(void *ptr, void *limit, TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class CharWalker : public PrimitiveWalker {
public:
  CharWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  const void *unpack(TTreeReaderArrayBase *readerArrayBase, int i);
  void *copyToBuffer(void *ptr, void *limit, void *address);
  void *copyToBuffer(void *ptr, void *limit, TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class UCharWalker : public PrimitiveWalker {
public:
  UCharWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  const void *unpack(TTreeReaderArrayBase *readerArrayBase, int i);
  void *copyToBuffer(void *ptr, void *limit, void *address);
  void *copyToBuffer(void *ptr, void *limit, TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class ShortWalker : public PrimitiveWalker {
public:
  ShortWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  const void *unpack(TTreeReaderArrayBase *readerArrayBase, int i);
  void *copyToBuffer(void *ptr, void *limit, void *address);
  void *copyToBuffer(void *ptr, void *limit, TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class UShortWalker : public PrimitiveWalker {
public:
  UShortWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  const void *unpack(TTreeReaderArrayBase *readerArrayBase, int i);
  void *copyToBuffer(void *ptr, void *limit, void *address);
  void *copyToBuffer(void *ptr, void *limit, TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class IntWalker : public PrimitiveWalker {
public:
  IntWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  const void *unpack(TTreeReaderArrayBase *readerArrayBase, int i);
  void *copyToBuffer(void *ptr, void *limit, void *address);
  void *copyToBuffer(void *ptr, void *limit, TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
  int value(TTreeReaderValueBase *readerValue);
};

class UIntWalker : public PrimitiveWalker {
public:
  UIntWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  const void *unpack(TTreeReaderArrayBase *readerArrayBase, int i);
  void *copyToBuffer(void *ptr, void *limit, void *address);
  void *copyToBuffer(void *ptr, void *limit, TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class LongWalker : public PrimitiveWalker {
public:
  LongWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  const void *unpack(TTreeReaderArrayBase *readerArrayBase, int i);
  void *copyToBuffer(void *ptr, void *limit, void *address);
  void *copyToBuffer(void *ptr, void *limit, TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class ULongWalker : public PrimitiveWalker {
public:
  ULongWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  const void *unpack(TTreeReaderArrayBase *readerArrayBase, int i);
  void *copyToBuffer(void *ptr, void *limit, void *address);
  void *copyToBuffer(void *ptr, void *limit, TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class FloatWalker : public PrimitiveWalker {
public:
  FloatWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  const void *unpack(TTreeReaderArrayBase *readerArrayBase, int i);
  void *copyToBuffer(void *ptr, void *limit, void *address);
  void *copyToBuffer(void *ptr, void *limit, TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class DoubleWalker : public PrimitiveWalker {
public:
  DoubleWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  std::string avroTypeName();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  const void *unpack(TTreeReaderArrayBase *readerArrayBase, int i);
  void *copyToBuffer(void *ptr, void *limit, void *address);
  void *copyToBuffer(void *ptr, void *limit, TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

///////////////////////////////////////////////////////////////////// AnyStringWalkers

class AnyStringWalker : public PrimitiveWalker {
public:
  AnyStringWalker(std::string fieldName, std::string typeName);
  bool empty();
  bool resolved();
  void resolve(const void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  virtual void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) = 0;
};

class CStringWalker : public AnyStringWalker {
public:
  CStringWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  const void *unpack(TTreeReaderArrayBase *readerArrayBase, int i);
  void *copyToBuffer(void *ptr, void *limit, void *address);
  void *copyToBuffer(void *ptr, void *limit, TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class StdStringWalker : public AnyStringWalker {
public:
  StdStringWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  const void *unpack(TTreeReaderArrayBase *readerArrayBase, int i);
  void *copyToBuffer(void *ptr, void *limit, void *address);
  void *copyToBuffer(void *ptr, void *limit, TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

class TStringWalker : public AnyStringWalker {
public:
  TStringWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
  void printJSON(TTreeReaderArrayBase *readerArrayBase, int i, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
  bool printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  const void *unpack(TTreeReaderArrayBase *readerArrayBase, int i);
  void *copyToBuffer(void *ptr, void *limit, void *address);
  void *copyToBuffer(void *ptr, void *limit, TTreeReaderArrayBase *readerArrayBase, int i);
  TTreeReaderValueBase *readerValue(TTreeReader *reader);
  TTreeReaderArrayBase *readerArray(TTreeReader *reader);
};

///////////////////////////////////////////////////////////////////// MemberWalker

class ClassWalker;

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
  void resolve(const void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  void *copyToBuffer(void *ptr, void *limit, void *address);
};

///////////////////////////////////////////////////////////////////// ClassWalker

class ClassWalkerDataProvider : public DataProvider {
public:
  ClassWalker *classWalker;

  ClassWalkerDataProvider(ClassWalker *classWalker);
  int getDataSize(const void *address);
  const void *getData(const void *address, int index);
};

class ClassWalker : public FieldWalker {
private:
  size_t sizeOf_;
  const std::type_info *typeId_;
public:
  TClass *tclass;
  std::string avroNamespace;
  std::map<const std::string, ClassWalker*> &defs;
  std::vector<MemberWalker*> members;
  ClassWalkerDataProvider dataProvider;

  ClassWalker(std::string fieldName, TClass *tclass, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  void fill();    // has side-effects, must be called soon after constructor

  std::vector<std::string> splitCppNamespace(std::string className);
  std::string dropCppNamespace(std::string className);
  std::string addCppNamespace(std::string className, std::string ns);

  size_t sizeOf();
  const std::type_info *typeId();
  TDictionary *tdictionary();
  bool empty();
  bool resolved();
  void resolve(const void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  void *copyToBuffer(void *ptr, void *limit, void *address);
};

///////////////////////////////////////////////////////////////////// PointerWalker

class PointerWalker;

class PointerWalkerDataProvider : public DataProvider {
public:
  PointerWalker *pointerWalker;

  PointerWalkerDataProvider(PointerWalker *pointerWalker);
  int getDataSize(const void *address);
  const void *getData(const void *address, int index);
};

class PointerWalker : public FieldWalker {
public:
  FieldWalker *walker;
  PointerWalkerDataProvider dataProvider;

  PointerWalker(std::string fieldName, FieldWalker *walker);
  size_t sizeOf();
  const std::type_info *typeId();
  bool empty();
  bool resolved();
  void resolve(const void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  void *copyToBuffer(void *ptr, void *limit, void *address);
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
  void resolve(const void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  void *copyToBuffer(void *ptr, void *limit, void *address);
};

///////////////////////////////////////////////////////////////////// StdVectorWalker

class StdVectorWalker;

class StdVectorWalkerDataProvider : public DataProvider {
public:
  StdVectorWalker *stdVectorWalker;

  StdVectorWalkerDataProvider(StdVectorWalker *stdVectorWalker);
  int getDataSize(const void *address);
  const void *getData(const void *address, int index);
};
class StdVectorWalker : public FieldWalker {
private:
  const std::type_info *typeId_;
public:
  FieldWalker *walker;
  StdVectorWalkerDataProvider dataProvider;

  StdVectorWalker(std::string fieldName, std::string typeName, FieldWalker *walker);
  size_t sizeOf();
  const std::type_info *typeId();
  bool empty();
  bool resolved();
  void resolve(const void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  void *copyToBuffer(void *ptr, void *limit, void *address);
};

///////////////////////////////////////////////////////////////////// StdVectorBoolWalker

class StdVectorBoolWalker;

class StdVectorBoolWalkerDataProvider : public DataProvider {
public:
  StdVectorBoolWalker *stdVectorBoolWalker;
  const bool FALSE = false;
  const bool TRUE = true;

  StdVectorBoolWalkerDataProvider(StdVectorBoolWalker *stdVectorBoolWalker);
  int getDataSize(const void *address);
  const void *getData(const void *address, int index);
};

class StdVectorBoolWalker : public FieldWalker {
public:
  FieldWalker *walker;
  StdVectorBoolWalkerDataProvider dataProvider;

  StdVectorBoolWalker(std::string fieldName);
  size_t sizeOf();
  const std::type_info *typeId();
  bool empty();
  bool resolved();
  void resolve(const void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  void *copyToBuffer(void *ptr, void *limit, void *address);
};

///////////////////////////////////////////////////////////////////// ArrayWalker

class ArrayWalker;

class ArrayWalkerDataProvider : public DataProvider {
public:
  ArrayWalker *arrayWalker;

  ArrayWalkerDataProvider(ArrayWalker *arrayWalker);
  int getDataSize(const void *address);
  const void *getData(const void *address, int index);
};

class ArrayWalker : public FieldWalker {
public:
  FieldWalker *walker;
  int numItems;
  ArrayWalkerDataProvider dataProvider;

  ArrayWalker(std::string fieldName, FieldWalker *walker, int numItems);
  size_t sizeOf();
  const std::type_info *typeId();
  bool empty();
  bool resolved();
  void resolve(const void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  void *copyToBuffer(void *ptr, void *limit, void *address);
};

///////////////////////////////////////////////////////////////////// TObjArrayWalker

class TObjArrayWalker;

class TObjArrayWalkerDataProvider : public DataProvider {
public:
  TObjArrayWalker *tObjArrayWalker;

  TObjArrayWalkerDataProvider(TObjArrayWalker *tObjArrayWalker);
  int getDataSize(const void *address);
  const void *getData(const void *address, int index);
};

class TObjArrayWalker : public FieldWalker {
public:
  std::string avroNamespace;
  std::map<const std::string, ClassWalker*> &defs;
  FieldWalker *walker;
  TClass *classToAssert;
  TObjArrayWalkerDataProvider dataProvider;

  TObjArrayWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  size_t sizeOf();
  const std::type_info *typeId();
  bool empty();
  bool resolved();
  void resolve(const void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  void *copyToBuffer(void *ptr, void *limit, void *address);
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
  void resolve(const void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  void *copyToBuffer(void *ptr, void *limit, void *address);
};

///////////////////////////////////////////////////////////////////// TClonesArrayWalker

class TClonesArrayWalker;

class TClonesArrayWalkerDataProvider : public DataProvider {
public:
  TClonesArrayWalker *tClonesArrayWalker;

  TClonesArrayWalkerDataProvider(TClonesArrayWalker *tClonesArrayWalker);
  int getDataSize(const void *address);
  const void *getData(const void *address, int index);
};

class TClonesArrayWalker : public FieldWalker {
public:
  std::string avroNamespace;
  std::map<const std::string, ClassWalker*> &defs;
  FieldWalker *walker;
  TClonesArrayWalkerDataProvider dataProvider;

  TClonesArrayWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  size_t sizeOf();
  const std::type_info *typeId();
  bool empty();
  bool resolved();
  void resolve(const void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  void *copyToBuffer(void *ptr, void *limit, void *address);
};

///////////////////////////////////////////////////////////////////// ExtractableWalker

class ExtractableWalker : public FieldWalker {
public:
#ifdef AVRO
  avro_value_t avroValue;
#endif

  ExtractableWalker(std::string fieldName, std::string typeName);
  bool empty();
  virtual void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) = 0;
  virtual void reset(TTreeReader *reader) = 0;
  virtual void *getAddress() = 0;
};

///////////////////////////////////////////////////////////////////// LeafWalker

class LeafWalker;

class LeafDimension : public DataProvider {
private:
  LeafWalker *leafWalker;
  LeafDimension *next_;
  int size_;
  IntWalker *counter;
  TTreeReaderValueBase *counterReaderValue;
  int getDataLastIndex = -1;
public:
  LeafDimension(LeafWalker *leafWalker, LeafDimension *next, int size);
  LeafDimension(LeafWalker *leafWalker, LeafDimension *next, IntWalker *counter, TTreeReader *reader);
  void reset(TTreeReader *reader);
  std::string repr();
  LeafDimension *next();   // linked list makes the recursive function in LeafWalker easier to understand
  int size();
  int flatSize();
  int getDataSize(const void *address);
  const void *getData(const void *address, int index);
};

class LeafWalker : public ExtractableWalker {
public:
  PrimitiveWalker *walker;
  TTreeReaderValueBase *readerValue;
  TTreeReaderArrayBase *readerArray;
  int dimensions;
  LeafDimension *dims;

  LeafWalker(TLeaf *tleaf, TTree *ttree, TTreeReader *reader);
  PrimitiveWalker *leafToPrimitive(TLeaf *tleaf);

  size_t sizeOf();
  const std::type_info *typeId();
  bool resolved();
  void resolve(const void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  int printJSONDeep(int readerIndex, int readerSize, LeafDimension *dim, std::ostream &stream);
  void printJSON(void *address, std::ostream &stream);
#ifdef AVRO
  int printAvroDeep(int readerIndex, int readerSize, LeafDimension *dim, avro_value_t *avrovalue);
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  int copyToBufferDeep(void **ptr, void *limit, int readerIndex, int readerSize, LeafDimension *dim);
  void *copyToBuffer(void *ptr, void *limit, void *address);
  void reset(TTreeReader *reader);
  void *getAddress();
};

///////////////////////////////////////////////////////////////////// ReaderValueWalker

class GenericReaderValue: public TTreeReaderValueBase {
public:
  std::string typeName;
  GenericReaderValue();
  GenericReaderValue(std::string fieldName, std::string typeName, TTreeReader *reader, FieldWalker *walker);
  const char *GetDerivedTypeName() const;
};

class ReaderValueWalker : public ExtractableWalker {
public:
  FieldWalker *walker;
  GenericReaderValue *value;

  ReaderValueWalker(std::string fieldName, TBranch *tbranch, TTreeReader *reader, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs);
  size_t sizeOf();
  const std::type_info *typeId();
  bool resolved();
  void resolve(const void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  void *copyToBuffer(void *ptr, void *limit, void *address);
  void reset(TTreeReader *reader);
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
  void resolve(const void *address);
  std::string repr(int indent, std::set<std::string> &memo);
  std::string avroTypeName();
  std::string avroSchema(int indent, std::set<std::string> &memo);
  void buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo);
  void printJSON(void *address, std::ostream &stream);
#ifdef AVRO
  bool printAvro(void *address, avro_value_t *avrovalue);
#endif
  const void *unpack(const void *address);
  void *copyToBuffer(void *ptr, void *limit, void *address);
};

class RawTBranchStdStringWalker : public RawTBranchWalker {
public:
  std::string *data;
  TTreeReader *reader;

  RawTBranchStdStringWalker(std::string fieldName, TTreeReader *reader);
  size_t sizeOf();
  const std::type_info *typeId();
  void reset(TTreeReader *reader);
  void *getAddress();
};

class RawTBranchTStringWalker : public RawTBranchWalker {
public:
  TString *data;
  TTreeReader *reader;

  RawTBranchTStringWalker(std::string fieldName, TTreeReader *reader);
  size_t sizeOf();
  const std::type_info *typeId();
  void reset(TTreeReader *reader);
  void *getAddress();
};

///////////////////////////////////////////////////////////////////// TreeWalker

class TreeWalker : public DataProvider {
public:
  std::string fileLocation;
  std::string treeLocation;
  std::string avroNamespace;
  std::string stringHolder;

  bool valid = false;
  std::string errorMessage = "";
  TFile *file;
  TTreeReader *reader;

  std::map<const std::string, ClassWalker*> defs;
  std::vector<ExtractableWalker*> fields;

#ifdef AVRO
  bool avroHeaderPrinted = false;
  avro_schema_t schema;
  avro_file_writer_t avroWriter;
  avro_value_iface_t *avroInterface;
  avro_value_t avroValue;
#endif

  TreeWalker(std::string fileLocation, std::string treeLocation, std::string avroNamespace);
  bool tryToOpenFile();
  void reset(std::string fileLocation);

  bool next();
  long numEntriesInCurrentTree();
  void setEntryInCurrentTree(long entry);

  bool resolved();
  void resolve();
  std::string repr();
  void printJSON();
  void buildSchema(SchemaBuilder schemaBuilder);
  std::string stringJSON();
#ifdef AVRO
  std::string avroSchema();
  bool printAvroHeaderOnce(std::string &codec, int blockSize);
  bool printAvro();
  void closeAvro();
#endif
  int getDataSize(const void *address);
  const void *getData(const void *address, int index);
  void copyToBuffer(int64_t entry, int microBatchSize, void *buffer, size_t size);
};

extern "C" {
  void resetSignals();
  void addInclude(const char *include);
  void loadLibrary(const char *lib);
}

#endif // DATAWALKER_H
