#include "datawalker.h"

///////////////////////////////////////////////////////////////////// FieldWalker

FieldWalker::FieldWalker(std::string fieldName, std::string typeName) :
  fieldName(fieldName), typeName(typeName) { }

void FieldWalker::printEscapedString(const char *string, std::ostream &stream) {
  for (const char *c = string;  *c != 0;  c++)
    switch (*c) {
      case '"': stream << "\\\""; break;
      case '\\': stream << "\\\\"; break;
      case '\b': stream << "\\b"; break;
      case '\f': stream << "\\f"; break;
      case '\n': stream << "\\n"; break;
      case '\r': stream << "\\r"; break;
      case '\t': stream << "\\t"; break;
      default:
        if ('\x00' <= *c  &&  *c <= '\x1f')
          stream << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)*c << std::dec;
        else
          stream << *c;
    }
}

std::string FieldWalker::escapedString(const char *string) {
  std::ostringstream stream;
  printEscapedString(string, stream);
  return stream.str();
}

///////////////////////////////////////////////////////////////////// PrimitiveWalkers

PrimitiveWalker::PrimitiveWalker(std::string fieldName, std::string typeName) :
  FieldWalker(fieldName, typeName) { }

bool PrimitiveWalker::empty() { return false; }

bool PrimitiveWalker::resolved() { return true; }

void PrimitiveWalker::resolve(void *address) { }

std::string PrimitiveWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("\"") + typeName + std::string("\"");
}

std::string PrimitiveWalker::avroSchema(int indent, std::set<std::string> &memo) {
  return std::string("\"") + avroTypeName() + std::string("\"");
}

//// BoolWalker

BoolWalker::BoolWalker(std::string fieldName) : PrimitiveWalker(fieldName, "bool") { }

size_t BoolWalker::sizeOf() { return sizeof(bool); }

const std::type_info *BoolWalker::typeId() { return &typeid(bool); }

std::string BoolWalker::avroTypeName() { return "boolean"; }

void BoolWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  schemaBuilder(SchemaBool, this, nullptr, nullptr);
}

void BoolWalker::printJSON(void *address) {
  if (*((bool*)address)) std::cout << "true"; else std::cout << "false";
}

void BoolWalker::printJSON(TTreeReaderArrayBase *readerArrayBase, int i) {
  if (((TTreeReaderArray<bool>*)readerArrayBase)->At(i)) std::cout << "true"; else std::cout << "false";
}

#ifdef AVRO
bool BoolWalker::printAvro(void *address, avro_value_t *avrovalue) {
  if (*((bool*)address))
    avro_value_set_boolean(avrovalue, true);
  else
    avro_value_set_boolean(avrovalue, false);
  return true;
}

bool BoolWalker::printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue) {
  if (((TTreeReaderArray<bool>*)readerArrayBase)->At(i))
    avro_value_set_boolean(avrovalue, true);
  else
    avro_value_set_boolean(avrovalue, false);
  return true;
}
#endif

int BoolWalker::getDataSize(void *address, LeafDimension *dim) {
  return 0;
}

const void *BoolWalker::getData(void *address, int index, LeafDimension *dim) {
  if (index == -1)
    return address;
  else
    return &((TTreeReaderArray<bool>*)address)->At(index);
}

TTreeReaderValueBase *BoolWalker::readerValue(TTreeReader *reader) {
  return new TTreeReaderValue<bool>(*reader, fieldName.c_str());
}

TTreeReaderArrayBase *BoolWalker::readerArray(TTreeReader *reader) {
  return new TTreeReaderArray<bool>(*reader, fieldName.c_str());
}

//// CharWalker

CharWalker::CharWalker(std::string fieldName) : PrimitiveWalker(fieldName, "char") { }

size_t CharWalker::sizeOf() { return sizeof(char); }

const std::type_info *CharWalker::typeId() { return &typeid(char); }

std::string CharWalker::avroTypeName() { return "int"; }

void CharWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  schemaBuilder(SchemaChar, this, nullptr, nullptr);
}

void CharWalker::printJSON(void *address) {
  std::cout << ((int)(*((char*)address)));
}

void CharWalker::printJSON(TTreeReaderArrayBase *readerArrayBase, int i) {
  std::cout << ((int)((TTreeReaderArray<char>*)readerArrayBase)->At(i));
}

#ifdef AVRO
bool CharWalker::printAvro(void *address, avro_value_t *avrovalue) {
  avro_value_set_int(avrovalue, *((char*)address));
  return true;
}

bool CharWalker::printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue) {
  avro_value_set_int(avrovalue, ((TTreeReaderArray<char>*)readerArrayBase)->At(i));
  return true;
}
#endif

int CharWalker::getDataSize(void *address, LeafDimension *dim) {
  return 0;
}

const void *CharWalker::getData(void *address, int index, LeafDimension *dim) {
  if (index == -1)
    return address;
  else
    return &((TTreeReaderArray<char>*)address)->At(index);
}

TTreeReaderValueBase *CharWalker::readerValue(TTreeReader *reader) {
  return new TTreeReaderValue<char>(*reader, fieldName.c_str());
}

TTreeReaderArrayBase *CharWalker::readerArray(TTreeReader *reader) {
  return new TTreeReaderArray<char>(*reader, fieldName.c_str());
}

//// UCharWalker

UCharWalker::UCharWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned char") { }

size_t UCharWalker::sizeOf() { return sizeof(unsigned char); }

const std::type_info *UCharWalker::typeId() { return &typeid(unsigned char); }

std::string UCharWalker::avroTypeName() { return "int"; }

void UCharWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  schemaBuilder(SchemaUChar, this, nullptr, nullptr);
}

void UCharWalker::printJSON(void *address) {
  std::cout << ((int)(*((unsigned char*)address)));
}

void UCharWalker::printJSON(TTreeReaderArrayBase *readerArrayBase, int i) {
  std::cout << ((int)((TTreeReaderArray<unsigned char>*)readerArrayBase)->At(i));
}

#ifdef AVRO
bool UCharWalker::printAvro(void *address, avro_value_t *avrovalue) {
  avro_value_set_int(avrovalue, *((unsigned char*)address));
  return true;
}

bool UCharWalker::printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue) {
  avro_value_set_int(avrovalue, ((TTreeReaderArray<unsigned char>*)readerArrayBase)->At(i));
  return true;
}
#endif

int UCharWalker::getDataSize(void *address, LeafDimension *dim) {
  return 0;
}

const void *UCharWalker::getData(void *address, int index, LeafDimension *dim) {
  if (index == -1)
    return address;
  else
    return &((TTreeReaderArray<unsigned char>*)address)->At(index);
}

TTreeReaderValueBase *UCharWalker::readerValue(TTreeReader *reader) {
  return new TTreeReaderValue<unsigned char>(*reader, fieldName.c_str());
}

TTreeReaderArrayBase *UCharWalker::readerArray(TTreeReader *reader) {
  return new TTreeReaderArray<unsigned char>(*reader, fieldName.c_str());
}

//// ShortWalker

ShortWalker::ShortWalker(std::string fieldName) : PrimitiveWalker(fieldName, "short") { }

size_t ShortWalker::sizeOf() { return sizeof(short); }

const std::type_info *ShortWalker::typeId() { return &typeid(short); }

std::string ShortWalker::avroTypeName() { return "int"; }

void ShortWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  schemaBuilder(SchemaShort, this, nullptr, nullptr);
}

void ShortWalker::printJSON(void *address) {
  std::cout << *((short*)address);
}

void ShortWalker::printJSON(TTreeReaderArrayBase *readerArrayBase, int i) {
  std::cout << ((TTreeReaderArray<short>*)readerArrayBase)->At(i);
}

#ifdef AVRO
bool ShortWalker::printAvro(void *address, avro_value_t *avrovalue) {
  avro_value_set_int(avrovalue, *((short*)address));
  return true;
}

bool ShortWalker::printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue) {
  avro_value_set_int(avrovalue, ((TTreeReaderArray<short>*)readerArrayBase)->At(i));
  return true;
}
#endif

int ShortWalker::getDataSize(void *address, LeafDimension *dim) {
  return 0;
}

const void *ShortWalker::getData(void *address, int index, LeafDimension *dim) {
  if (index == -1)
    return address;
  else
    return &((TTreeReaderArray<short>*)address)->At(index);
}

TTreeReaderValueBase *ShortWalker::readerValue(TTreeReader *reader) {
  return new TTreeReaderValue<short>(*reader, fieldName.c_str());
}

TTreeReaderArrayBase *ShortWalker::readerArray(TTreeReader *reader) {
  return new TTreeReaderArray<short>(*reader, fieldName.c_str());
}

//// UShortWalker

UShortWalker::UShortWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned short") { }

size_t UShortWalker::sizeOf() { return sizeof(unsigned short); }

const std::type_info *UShortWalker::typeId() { return &typeid(unsigned short); }

std::string UShortWalker::avroTypeName() { return "int"; }

void UShortWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  schemaBuilder(SchemaUShort, this, nullptr, nullptr);
}

void UShortWalker::printJSON(void *address) {
  std::cout << *((unsigned short*)address);
}

void UShortWalker::printJSON(TTreeReaderArrayBase *readerArrayBase, int i) {
  std::cout << ((TTreeReaderArray<unsigned short>*)readerArrayBase)->At(i);
}

#ifdef AVRO
bool UShortWalker::printAvro(void *address, avro_value_t *avrovalue) {
  avro_value_set_int(avrovalue, *((unsigned short*)address));
  return true;
}

bool UShortWalker::printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue) {
  avro_value_set_int(avrovalue, ((TTreeReaderArray<unsigned short>*)readerArrayBase)->At(i));
  return true;
}
#endif

int UShortWalker::getDataSize(void *address, LeafDimension *dim) {
  return 0;
}

const void *UShortWalker::getData(void *address, int index, LeafDimension *dim) {
  if (index == -1)
    return address;
  else
    return &((TTreeReaderArray<unsigned short>*)address)->At(index);
}

TTreeReaderValueBase *UShortWalker::readerValue(TTreeReader *reader) {
  return new TTreeReaderValue<unsigned short>(*reader, fieldName.c_str());
}

TTreeReaderArrayBase *UShortWalker::readerArray(TTreeReader *reader) {
  return new TTreeReaderArray<unsigned short>(*reader, fieldName.c_str());
}

//// IntWalker

IntWalker::IntWalker(std::string fieldName) : PrimitiveWalker(fieldName, "int") { }

size_t IntWalker::sizeOf() { return sizeof(int); }

const std::type_info *IntWalker::typeId() { return &typeid(int); }

std::string IntWalker::avroTypeName() { return "int"; }

void IntWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  schemaBuilder(SchemaInt, this, nullptr, nullptr);
}

void IntWalker::printJSON(void *address) {
  std::cout << *((int*)address);
}

void IntWalker::printJSON(TTreeReaderArrayBase *readerArrayBase, int i) {
  std::cout << ((TTreeReaderArray<int>*)readerArrayBase)->At(i);
}

#ifdef AVRO
bool IntWalker::printAvro(void *address, avro_value_t *avrovalue) {
  avro_value_set_int(avrovalue, *((int*)address));
  return true;
}

bool IntWalker::printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue) {
  avro_value_set_int(avrovalue, ((TTreeReaderArray<int>*)readerArrayBase)->At(i));
  return true;
}
#endif

int IntWalker::getDataSize(void *address, LeafDimension *dim) {
  return 0;
}

const void *IntWalker::getData(void *address, int index, LeafDimension *dim) {
  if (index == -1)
    return address;
  else
    return &((TTreeReaderArray<int>*)address)->At(index);
}

TTreeReaderValueBase *IntWalker::readerValue(TTreeReader *reader) {
  return new TTreeReaderValue<int>(*reader, fieldName.c_str());
}

TTreeReaderArrayBase *IntWalker::readerArray(TTreeReader *reader) {
  return new TTreeReaderArray<int>(*reader, fieldName.c_str());
}

int IntWalker::value(TTreeReaderValueBase *readerValue) {
  return *((int*)readerValue->GetAddress());
}

//// UIntWalker

UIntWalker::UIntWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned int") { }

size_t UIntWalker::sizeOf() { return sizeof(unsigned int); }

const std::type_info *UIntWalker::typeId() { return &typeid(unsigned int); }

std::string UIntWalker::avroTypeName() { return "long"; }

void UIntWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  schemaBuilder(SchemaUInt, this, nullptr, nullptr);
}

void UIntWalker::printJSON(void *address) {
  std::cout << *((unsigned int*)address);
}

void UIntWalker::printJSON(TTreeReaderArrayBase *readerArrayBase, int i) {
  std::cout << ((TTreeReaderArray<unsigned int>*)readerArrayBase)->At(i);
}

#ifdef AVRO
bool UIntWalker::printAvro(void *address, avro_value_t *avrovalue) {
  avro_value_set_long(avrovalue, *((unsigned int*)address));
  return true;
}

bool UIntWalker::printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue) {
  avro_value_set_long(avrovalue, ((TTreeReaderArray<unsigned int>*)readerArrayBase)->At(i));
  return true;
}
#endif

int UIntWalker::getDataSize(void *address, LeafDimension *dim) {
  return 0;
}

const void *UIntWalker::getData(void *address, int index, LeafDimension *dim) {
  if (index == -1)
    return address;
  else
    return &((TTreeReaderArray<unsigned int>*)address)->At(index);
}

TTreeReaderValueBase *UIntWalker::readerValue(TTreeReader *reader) {
  return new TTreeReaderValue<unsigned int>(*reader, fieldName.c_str());
}

TTreeReaderArrayBase *UIntWalker::readerArray(TTreeReader *reader) {
  return new TTreeReaderArray<unsigned int>(*reader, fieldName.c_str());
}

//// LongWalker

LongWalker::LongWalker(std::string fieldName) : PrimitiveWalker(fieldName, "Long64_t") { }

size_t LongWalker::sizeOf() { return sizeof(Long64_t); }

const std::type_info *LongWalker::typeId() { return &typeid(Long64_t); }

std::string LongWalker::avroTypeName() { return "long"; }

void LongWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  schemaBuilder(SchemaLong, this, nullptr, nullptr);
}

void LongWalker::printJSON(void *address) {
  std::cout << *((Long64_t*)address);
}

void LongWalker::printJSON(TTreeReaderArrayBase *readerArrayBase, int i) {
  std::cout << ((TTreeReaderArray<Long64_t>*)readerArrayBase)->At(i);
}

#ifdef AVRO
bool LongWalker::printAvro(void *address, avro_value_t *avrovalue) {
  avro_value_set_long(avrovalue, *((Long64_t*)address));
  return true;
}

bool LongWalker::printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue) {
  avro_value_set_long(avrovalue, ((TTreeReaderArray<Long64_t>*)readerArrayBase)->At(i));
  return true;
}
#endif

int LongWalker::getDataSize(void *address, LeafDimension *dim) {
  return 0;
}

const void *LongWalker::getData(void *address, int index, LeafDimension *dim) {
  if (index == -1)
    return address;
  else
    return &((TTreeReaderArray<Long64_t>*)address)->At(index);
}

TTreeReaderValueBase *LongWalker::readerValue(TTreeReader *reader) {
  return new TTreeReaderValue<Long64_t>(*reader, fieldName.c_str());
}

TTreeReaderArrayBase *LongWalker::readerArray(TTreeReader *reader) {
  return new TTreeReaderArray<Long64_t>(*reader, fieldName.c_str());
}

//// ULongWalker

ULongWalker::ULongWalker(std::string fieldName) : PrimitiveWalker(fieldName, "ULong64_t") { }

size_t ULongWalker::sizeOf() { return sizeof(ULong64_t); }

const std::type_info *ULongWalker::typeId() { return &typeid(ULong64_t); }

std::string ULongWalker::avroTypeName() { return "double"; }

void ULongWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  schemaBuilder(SchemaULong, this, nullptr, nullptr);
}

void ULongWalker::printJSON(void *address) {
  std::cout << *((ULong64_t*)address);
}

void ULongWalker::printJSON(TTreeReaderArrayBase *readerArrayBase, int i) {
  std::cout << ((TTreeReaderArray<unsigned long>*)readerArrayBase)->At(i);
}

#ifdef AVRO
bool ULongWalker::printAvro(void *address, avro_value_t *avrovalue) {
  avro_value_set_double(avrovalue, *((ULong64_t*)address));
  return true;
}

bool ULongWalker::printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue) {
  avro_value_set_double(avrovalue, ((TTreeReaderArray<ULong64_t>*)readerArrayBase)->At(i));
  return true;
}
#endif

int ULongWalker::getDataSize(void *address, LeafDimension *dim) {
  return 0;
}

const void *ULongWalker::getData(void *address, int index, LeafDimension *dim) {
  if (index == -1)
    return address;
  else
    return &((TTreeReaderArray<ULong64_t>*)address)->At(index);
}

TTreeReaderValueBase *ULongWalker::readerValue(TTreeReader *reader) {
  return new TTreeReaderValue<ULong64_t>(*reader, fieldName.c_str());
}

TTreeReaderArrayBase *ULongWalker::readerArray(TTreeReader *reader) {
  return new TTreeReaderArray<ULong64_t>(*reader, fieldName.c_str());
}

//// FloatWalker

FloatWalker::FloatWalker(std::string fieldName) : PrimitiveWalker(fieldName, "float") { }

size_t FloatWalker::sizeOf() { return sizeof(float); }

const std::type_info *FloatWalker::typeId() { return &typeid(float); }

std::string FloatWalker::avroTypeName() { return "float"; }

void FloatWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  schemaBuilder(SchemaFloat, this, nullptr, nullptr);
}

void FloatWalker::printJSON(void *address) {
  std::cout << *((float*)address);
}

void FloatWalker::printJSON(TTreeReaderArrayBase *readerArrayBase, int i) {
  std::cout << ((TTreeReaderArray<float>*)readerArrayBase)->At(i);
}

#ifdef AVRO
bool FloatWalker::printAvro(void *address, avro_value_t *avrovalue) {
  avro_value_set_float(avrovalue, *((float*)address));
  return true;
}

bool FloatWalker::printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue) {
  avro_value_set_float(avrovalue, ((TTreeReaderArray<float>*)readerArrayBase)->At(i));
  return true;
}
#endif

int FloatWalker::getDataSize(void *address, LeafDimension *dim) {
  return 0;
}

const void *FloatWalker::getData(void *address, int index, LeafDimension *dim) {
  if (index == -1)
    return address;
  else
    return &((TTreeReaderArray<float>*)address)->At(index);
}

TTreeReaderValueBase *FloatWalker::readerValue(TTreeReader *reader) {
  return new TTreeReaderValue<float>(*reader, fieldName.c_str());
}

TTreeReaderArrayBase *FloatWalker::readerArray(TTreeReader *reader) {
  return new TTreeReaderArray<float>(*reader, fieldName.c_str());
}

//// DoubleWalker

DoubleWalker::DoubleWalker(std::string fieldName) : PrimitiveWalker(fieldName, "double") { }

size_t DoubleWalker::sizeOf() { return sizeof(double); }

const std::type_info *DoubleWalker::typeId() { return &typeid(double); }

std::string DoubleWalker::avroTypeName() { return "double"; }

void DoubleWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  schemaBuilder(SchemaDouble, this, nullptr, nullptr);
}

void DoubleWalker::printJSON(void *address) {
  std::cout << *((double*)address);
} 

void DoubleWalker::printJSON(TTreeReaderArrayBase *readerArrayBase, int i) {
  std::cout << ((TTreeReaderArray<double>*)readerArrayBase)->At(i);
}

#ifdef AVRO
bool DoubleWalker::printAvro(void *address, avro_value_t *avrovalue) {
  avro_value_set_double(avrovalue, *((double*)address));
  return true;
}

bool DoubleWalker::printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue) {
  avro_value_set_double(avrovalue, ((TTreeReaderArray<double>*)readerArrayBase)->At(i));
  return true;
}
#endif

int DoubleWalker::getDataSize(void *address, LeafDimension *dim) {
  return 0;
}

const void *DoubleWalker::getData(void *address, int index, LeafDimension *dim) {
  if (index == -1)
    return address;
  else
    return &((TTreeReaderArray<double>*)address)->At(index);
}

TTreeReaderValueBase *DoubleWalker::readerValue(TTreeReader *reader) {
  return new TTreeReaderValue<double>(*reader, fieldName.c_str());
}

TTreeReaderArrayBase *DoubleWalker::readerArray(TTreeReader *reader) {
  return new TTreeReaderArray<double>(*reader, fieldName.c_str());
}

///////////////////////////////////////////////////////////////////// AnyStringWalkers

AnyStringWalker::AnyStringWalker(std::string fieldName, std::string typeName) :
  PrimitiveWalker(fieldName, typeName) { }

bool AnyStringWalker::empty() { return false; }

bool AnyStringWalker::resolved() { return true; }

void AnyStringWalker::resolve(void *address) { }

std::string AnyStringWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("\"") + typeName + std::string("\"");
}
std::string AnyStringWalker::avroTypeName() { return "string"; }

std::string AnyStringWalker::avroSchema(int indent, std::set<std::string> &memo) { return "\"string\""; }

//// CStringWalker

CStringWalker::CStringWalker(std::string fieldName) : AnyStringWalker(fieldName, "char*") { }

size_t CStringWalker::sizeOf() { return 0; }

const std::type_info *CStringWalker::typeId() { &typeid(char*); }

void CStringWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  schemaBuilder(SchemaString, this, nullptr, nullptr);
}

void CStringWalker::printJSON(void *address) {
  std::cout << "\"";
  printEscapedString((char*)address, std::cout);
  std::cout << "\"";
}

void CStringWalker::printJSON(TTreeReaderArrayBase *readerArrayBase, int i) {
  std::cout << "\"";
  printEscapedString(((TTreeReaderArray<char*>*)readerArrayBase)->At(i), std::cout);
  std::cout << "\"";
}

#ifdef AVRO
bool CStringWalker::printAvro(void *address, avro_value_t *avrovalue) {
  avro_value_set_string(avrovalue, (char*)address);
  return true;
}

bool CStringWalker::printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue) {
  avro_value_set_string(avrovalue, ((TTreeReaderArray<char*>*)readerArrayBase)->At(i));
  return true;
}
#endif

int CStringWalker::getDataSize(void *address, LeafDimension *dim) {
  return 0;
}

const void *CStringWalker::getData(void *address, int index, LeafDimension *dim) {
  if (index == -1)
    return address;
  else
    return &((TTreeReaderArray<char*>*)address)->At(index);
}

TTreeReaderValueBase *CStringWalker::readerValue(TTreeReader *reader) {
  return new TTreeReaderArray<char>(*reader, fieldName.c_str());
}

TTreeReaderArrayBase *CStringWalker::readerArray(TTreeReader *reader) {
  return new TTreeReaderArray<char*>(*reader, fieldName.c_str());
}

//// StdStringWalker

StdStringWalker::StdStringWalker(std::string fieldName) : AnyStringWalker(fieldName, "string") { }

size_t StdStringWalker::sizeOf() { return sizeof(std::string); }

const std::type_info *StdStringWalker::typeId() { return &typeid(std::string); }

void StdStringWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  schemaBuilder(SchemaString, this, nullptr, nullptr);
}

void StdStringWalker::printJSON(void *address) {
  std::cout << "\"";
  printEscapedString(((std::string*)address)->c_str(), std::cout);
  std::cout << "\"";
}

void StdStringWalker::printJSON(TTreeReaderArrayBase *readerArrayBase, int i) {
  std::cout << "\"";
  printEscapedString(((TTreeReaderArray<std::string>*)readerArrayBase)->At(i).c_str(), std::cout);
  std::cout << "\"";
}

#ifdef AVRO
bool StdStringWalker::printAvro(void *address, avro_value_t *avrovalue) {
  avro_value_set_string(avrovalue, ((std::string*)address)->c_str());
  return true;
}

bool StdStringWalker::printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue) {
  avro_value_set_string(avrovalue, ((TTreeReaderArray<std::string>*)readerArrayBase)->At(i).c_str());
  return true;
}
#endif

int StdStringWalker::getDataSize(void *address, LeafDimension *dim) {
  return 0;
}

const void *StdStringWalker::getData(void *address, int index, LeafDimension *dim) {
  if (index == -1)
    return ((std::string*)address)->c_str();
  else
    return ((TTreeReaderArray<std::string>*)address)->At(index).c_str();
}

TTreeReaderValueBase *StdStringWalker::readerValue(TTreeReader *reader) {
  // NOTE: ROOT 6.06/00 won't build the following due to "Unknown type and class combination: -1, string" (no dictionary)
  return new TTreeReaderValue<std::string>(*reader, fieldName.c_str());
}

TTreeReaderArrayBase *StdStringWalker::readerArray(TTreeReader *reader) {
  return new TTreeReaderArray<std::string>(*reader, fieldName.c_str());
}

//// TStringWalker

TStringWalker::TStringWalker(std::string fieldName) : AnyStringWalker(fieldName, "TString") { }

size_t TStringWalker::sizeOf() { return sizeof(TString); }

const std::type_info *TStringWalker::typeId() { return &typeid(TString); }

void TStringWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  schemaBuilder(SchemaString, this, nullptr, nullptr);
}

void TStringWalker::printJSON(void *address) {
  std::cout << "\"";
  printEscapedString(((TString*)address)->Data(), std::cout);
  std::cout << "\"";
}

void TStringWalker::printJSON(TTreeReaderArrayBase *readerArrayBase, int i) {
  std::cout << "\"";
  printEscapedString(((TTreeReaderArray<TString>*)readerArrayBase)->At(i).Data(), std::cout);
  std::cout << "\"";
}

#ifdef AVRO
bool TStringWalker::printAvro(void *address, avro_value_t *avrovalue) {
  avro_value_set_string(avrovalue, ((TString*)address)->Data());
  return true;
}

bool TStringWalker::printAvro(TTreeReaderArrayBase *readerArrayBase, int i, avro_value_t *avrovalue) {
  avro_value_set_string(avrovalue, ((TTreeReaderArray<TString>*)readerArrayBase)->At(i).Data());
  return true;
}
#endif

int TStringWalker::getDataSize(void *address, LeafDimension *dim) {
  return 0;
}

const void *TStringWalker::getData(void *address, int index, LeafDimension *dim) {
  if (index == -1)
    return ((TString*)address)->Data();
  else
    return ((TTreeReaderArray<TString>*)address)->At(index).Data();
}

TTreeReaderValueBase *TStringWalker::readerValue(TTreeReader *reader) {
  // NOTE: ROOT 6.06/00 won't build the following due to "Unknown type and class combination: -1, string" (no dictionary)
  return new TTreeReaderValue<TString>(*reader, fieldName.c_str());
}

TTreeReaderArrayBase *TStringWalker::readerArray(TTreeReader *reader) {
  return new TTreeReaderArray<TString>(*reader, fieldName.c_str());
}

///////////////////////////////////////////////////////////////////// MemberWalker

MemberWalker::MemberWalker(TDataMember *dataMember, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) :
  FieldWalker(dataMember->GetName(), dataMember->GetTrueTypeName()),
  offset(dataMember->GetOffset()),
  comment(dataMember->GetTitle())
{
  int arrayDim = dataMember->GetArrayDim();
  if (arrayDim > 0  &&  (typeName == std::string("char")  ||  typeName == std::string("const char")  ||  typeName == std::string("Char_t")  ||  typeName == std::string("const Char_t")))
    walker = new CStringWalker(fieldName);
  else {
    walker = specializedWalker(fieldName, typeName, avroNamespace, defs);
    for (int i = arrayDim - 1;  i >= 0;  i--)
      walker = new ArrayWalker(fieldName, walker, dataMember->GetMaxIndex(i));
  }
}

FieldWalker *MemberWalker::specializedWalker(std::string fieldName, std::string typeName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) {
  std::string tn(typeName);
  std::string vectorPrefix("vector<");
  std::string constPrefix("const ");

  while (tn.substr(0, constPrefix.size()) == constPrefix)
    tn = tn.substr(constPrefix.size(), tn.size() - constPrefix.size());

  if (tn == std::string("char*")  ||  tn == std::string("Char_t*"))
    return new PointerWalker(fieldName, new CStringWalker(fieldName));

  else if (!tn.empty()  &&  tn.back() == '*')
    return new PointerWalker(fieldName, specializedWalker(fieldName, tn.substr(0, tn.size() - 1), avroNamespace, defs));

  else if (tn == std::string("TRef"))
    return new TRefWalker(fieldName, avroNamespace, defs);

  else if (tn == std::string("vector<bool>"))
    return new StdVectorBoolWalker(fieldName);

  else if (tn.substr(0, vectorPrefix.size()) == vectorPrefix  &&  tn.back() == '>') {
    tn = tn.substr(vectorPrefix.size(), tn.size() - vectorPrefix.size() - 1);
    while (!tn.empty()  &&  tn.back() == ' ') tn.pop_back();
    return new StdVectorWalker(fieldName, typeName, specializedWalker(fieldName, tn, avroNamespace, defs));
  }

  else if (tn == std::string("TObjArray"))
    return new TObjArrayWalker(fieldName, avroNamespace, defs);

  else if (tn == std::string("TRefArray"))
    return new TRefArrayWalker(fieldName, avroNamespace, defs);

  else if (tn == std::string("TClonesArray"))
    return new TClonesArrayWalker(fieldName, avroNamespace, defs);

  else {
    if (tn == std::string("bool")  ||  tn == std::string("Bool_t"))
      return new BoolWalker(fieldName);
    else if (tn == std::string("char")  ||  tn == std::string("Char_t")  ||  tn == std::string("Int8_t"))
      return new CharWalker(fieldName);
    else if (tn == std::string("unsigned char")  ||  tn == std::string("UChar_t")  ||  tn == std::string("UInt8_t"))
      return new UCharWalker(fieldName);
    else if (tn == std::string("short")  ||  tn == std::string("Short_t")  ||  tn == std::string("Short16_t")  ||  tn == std::string("Int16_t"))
      return new ShortWalker(fieldName);
    else if (tn == std::string("unsigned short")  ||  tn == std::string("UShort_t")  ||  tn == std::string("UShort16_t")  ||  tn == std::string("UInt16_t"))
      return new UShortWalker(fieldName);
    else if (tn == std::string("int")  ||  tn == std::string("Int_t")  ||  tn == std::string("Int32_t"))
      return new IntWalker(fieldName);
    else if (tn == std::string("unsigned int")  ||  tn == std::string("UInt_t")  ||  tn == std::string("UInt32_t"))
      return new UIntWalker(fieldName);
    else if (tn == std::string("long")  ||  tn == std::string("long long")  ||  tn == std::string("Long_t")  ||  tn == std::string("Long64_t"))
      return new LongWalker(fieldName);
    else if (tn == std::string("unsigned long")  ||  tn == std::string("unsigned long long")  ||  tn == std::string("ULong_t")  ||  tn == std::string("ULong64_t"))
      return new ULongWalker(fieldName);
    else if (tn == std::string("float")  ||  tn == std::string("Float_t")  ||  tn == std::string("Float16_t"))
      return new FloatWalker(fieldName);
    else if (tn == std::string("double")  ||  tn == std::string("Double_t")  ||  tn == std::string("Double32_t"))
      return new DoubleWalker(fieldName);
    else if (tn == std::string("string"))
      return new StdStringWalker(fieldName);
    else if (tn == std::string("TString"))
      return new TStringWalker(fieldName);

    else {
      TClass *tclass = TClass::GetClass(tn.c_str());
      if (tclass != nullptr) {
        if (defs.count(tn) > 0)
          return defs.at(tn);
        else {
          ClassWalker *out = new ClassWalker(fieldName, tclass, avroNamespace, defs);
          defs.insert(std::pair<const std::string, ClassWalker*>(tn, out));
          out->fill();
          return out;
        }
      }
      else
        throw std::invalid_argument(tn + std::string(" not handled"));
    }
  }
}

size_t MemberWalker::sizeOf() { return walker->sizeOf(); }

const std::type_info *MemberWalker::typeId() { return walker->typeId(); }

bool MemberWalker::empty() { return walker->empty(); }

bool MemberWalker::resolved() { return walker->resolved(); }

void MemberWalker::resolve(void *address) {
    walker->resolve((void*)((size_t)address + offset));
  }

std::string MemberWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("\"") + fieldName + std::string("\": ") + walker->repr(indent, memo);
}

std::string MemberWalker::avroTypeName() { return walker->avroTypeName(); }

std::string MemberWalker::avroSchema(int indent, std::set<std::string> &memo) {
  std::string out;
  out += std::string("{\"name\": \"") + fieldName + std::string("\", \"type\": ") + walker->avroSchema(indent, memo);
  if (!comment.empty())
    out += std::string(", \"doc\": \"") + escapedString(comment.c_str()) + std::string("\"");
  out += std::string("}");
  return out;
}

void MemberWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  schemaBuilder(SchemaClassFieldName, this, nullptr, fieldName.c_str());
  walker->buildSchema(schemaBuilder, memo);
}

void MemberWalker::printJSON(void *address) {
  std::cout << "\"" << fieldName << "\": ";
  walker->printJSON((void*)((size_t)address + offset));
}

#ifdef AVRO
bool MemberWalker::printAvro(void *address, avro_value_t *avrovalue) {
  return walker->printAvro((void*)((size_t)address + offset), avrovalue);
}
#endif

int MemberWalker::getDataSize(void *address, LeafDimension *dim) {
  return 0;
}

const void *MemberWalker::getData(void *address, int index, LeafDimension *dim) {
  return (void*)((size_t)address + offset);
}

///////////////////////////////////////////////////////////////////// ClassWalker

ClassWalker::ClassWalker(std::string fieldName, TClass *tclass, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) :
  FieldWalker(fieldName, dropCppNamespace(tclass->GetName())), tclass(tclass), avroNamespace(addCppNamespace(tclass->GetName(), avroNamespace)), defs(defs) { }

void ClassWalker::fill() {
  TIter nextMember = tclass->GetListOfDataMembers();
  for (TDataMember *dataMember = (TDataMember*)nextMember();  dataMember != nullptr;  dataMember = (TDataMember*)nextMember())
    if (dataMember->GetOffset() > 0) {
      MemberWalker *member = new MemberWalker(dataMember, avroNamespace, defs);
      if (!member->empty())
        members.push_back(member);
    }
  sizeOf_ = tclass->Size();
  typeId_ = tclass->GetTypeInfo();
}

std::vector<std::string> ClassWalker::splitCppNamespace(std::string className) {
  std::string delim("::");
  std::vector<std::string> out;
  std::size_t start = 0;
  std::size_t end = 0;
  while ((end = className.find(delim, start)) != std::string::npos) {
    out.push_back(className.substr(start, end - start));
    start = end + 2;
  }
  out.push_back(className.substr(start));
  return out;
}

std::string ClassWalker::dropCppNamespace(std::string className) {
  return splitCppNamespace(className).back();
}

std::string ClassWalker::addCppNamespace(std::string className, std::string ns) {
  std::string out = ns;
  std::vector<std::string> cppns = splitCppNamespace(className);
  for (int i = 0;  i < cppns.size() - 1;  i++)
    out += std::string(".") + cppns[i];
  return out;
}

size_t ClassWalker::sizeOf() { return sizeOf_; }

const std::type_info *ClassWalker::typeId() { return typeId_; }

bool ClassWalker::empty() { return members.empty(); }

bool ClassWalker::resolved() {
  for (auto iter = members.begin();  iter != members.end();  ++iter)
    if (!(*iter)->resolved())
      return false;
  return true;
}

void ClassWalker::resolve(void *address) {
  for (auto iter = members.begin();  iter != members.end();  ++iter)
    (*iter)->resolve(address);
}

std::string ClassWalker::repr(int indent, std::set<std::string> &memo) {
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

std::string ClassWalker::avroSchema(int indent, std::set<std::string> &memo) {
  if (memo.find(avroTypeName()) != memo.end())
    return std::string("\"") + avroTypeName() + std::string("\"");
  else {
    memo.insert(avroTypeName());
    std::string out;
    out += std::string("{\"type\": \"record\",\n") + std::string(indent, ' ') +
           std::string(" \"name\": \"") + typeName + std::string("\",\n") + std::string(indent, ' ');
    if (!avroNamespace.empty())
      out += std::string(" \"namespace\": \"") + avroNamespace + std::string("\",\n") + std::string(indent, ' ');
    out += std::string(" \"fields\": [\n") + std::string(indent + 3, ' ');

    bool first = true;
    for (auto iter = members.begin();  iter != members.end();  ++iter) {
      if (first) first = false; else out += std::string(",\n") + std::string(indent + 3,  ' ');
      out += (*iter)->avroSchema(indent + 3, memo);
    }
    out += std::string("\n") + std::string(indent, ' ') + std::string(" ]\n") + std::string(indent, ' ') + std::string("}");
    return out;
  }
}

std::string ClassWalker::avroTypeName() {
  std::string out;
  if (!avroNamespace.empty())
    out += avroNamespace + std::string(".");
  out += typeName;
  return out;
}

void ClassWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  std::string className(tclass->GetName());

  if (memo.find(className) != memo.end())
    schemaBuilder(SchemaClassReference, nullptr, nullptr, className.c_str());
  else {
    memo.insert(className);

    schemaBuilder(SchemaClassName, nullptr, nullptr, className.c_str());

    for (auto iter = members.begin();  iter != members.end();  ++iter)
      (*iter)->buildSchema(schemaBuilder, memo);

    schemaBuilder(SchemaClassEnd, nullptr, nullptr, nullptr);
  }
}

void ClassWalker::printJSON(void *address) {
  std::cout << "{";
  bool first = true;
  for (auto iter = members.begin();  iter != members.end();  ++iter) {
    if (first) first = false; else std::cout << ", ";    
    (*iter)->printJSON(address);
  }
  std::cout << "}";
}

#ifdef AVRO
bool ClassWalker::printAvro(void *address, avro_value_t *avrovalue) {
  size_t index = 0;
  for (auto iter = members.begin();  iter != members.end();  ++iter) {
    avro_value_t element;
    avro_value_get_by_index(avrovalue, index, &element, nullptr);
    if (!(*iter)->printAvro(address, &element))
      return false;
    index++;
  }
  return true;
}
#endif

int ClassWalker::getDataSize(void *address, LeafDimension *dim) {
  return 0;
}

const void *ClassWalker::getData(void *address, int index, LeafDimension *dim) {
  return nullptr;
}

///////////////////////////////////////////////////////////////////// PointerWalker

PointerWalker::PointerWalker(std::string fieldName, FieldWalker *walker) : FieldWalker(fieldName, "*"), walker(walker) { }

size_t PointerWalker::sizeOf() { return sizeof(void*); }

const std::type_info *PointerWalker::typeId() { return nullptr; }

bool PointerWalker::empty() { return walker->empty(); }

bool PointerWalker::resolved() { return walker->resolved(); }

void PointerWalker::resolve(void *address) { walker->resolve(*((void**)address)); }

std::string PointerWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("{\"*\": ") + walker->repr(indent, memo) + std::string("}");
}

std::string PointerWalker::avroTypeName() { return "union"; }

std::string PointerWalker::avroSchema(int indent, std::set<std::string> &memo) {
  FieldWalker *subWalker = walker;
  for (PointerWalker *pointerWalker = dynamic_cast<PointerWalker*>(subWalker);  pointerWalker != nullptr;  subWalker = pointerWalker->walker);
  return std::string("[\"null\", ") + subWalker->avroSchema(indent, memo) + std::string("]");
}

void PointerWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  FieldWalker *subWalker = walker;
  for (PointerWalker *pointerWalker = dynamic_cast<PointerWalker*>(subWalker);  pointerWalker != nullptr;  subWalker = pointerWalker->walker);

  schemaBuilder(SchemaPointer, this, nullptr, nullptr);
  subWalker->buildSchema(schemaBuilder, memo);
}

void PointerWalker::printJSON(void *address) {
  void *dereferenced = *((void**)address);
  if (dereferenced == nullptr)
    std::cout << "null";
  else {
    std::cout << "{\"" << walker->avroTypeName() << "\": ";
    walker->printJSON(dereferenced);
    std::cout << "\"}";
  }
}

#ifdef AVRO
bool PointerWalker::printAvro(void *address, avro_value_t *avrovalue) {
  avro_type_t t = avro_value_get_type(avrovalue);
  avro_value_t branch;

  void *dereferenced = *((void**)address);

  if (dereferenced == nullptr) {
    avro_value_set_branch(avrovalue, 0, &branch);
    avro_value_set_null(&branch);
    return true;
  }
  else {
    avro_value_set_branch(avrovalue, 1, &branch);
    return walker->printAvro(dereferenced, &branch);
  }

  return true;
}
#endif

int PointerWalker::getDataSize(void *address, LeafDimension *dim) {
  // PointerWalker returns 0 if it contains nullptr, 1 if it contains an item (a "size," in a sense).
  void *dereferenced = *((void**)address);
  if (dereferenced == nullptr)
    return 0;
  else
    return 1;
}

const void *PointerWalker::getData(void *address, int index, LeafDimension *dim) {
  void *dereferenced = *((void**)address);
  if (dereferenced == nullptr)
    return nullptr;
  else
    return dereferenced;
}

///////////////////////////////////////////////////////////////////// TRefWalker

TRefWalker::TRefWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) :
  FieldWalker(fieldName, "*"), walker(nullptr) { }

size_t TRefWalker::sizeOf() { return sizeof(TRef); }

const std::type_info *TRefWalker::typeId() { return &typeid(TRef); }

// we won't support TRefs, so report it as empty and let it be culled
bool TRefWalker::empty() { return true; }

bool TRefWalker::resolved() { return true; }

void TRefWalker::resolve(void *address) { }

std::string TRefWalker::repr(int indent, std::set<std::string> &memo) {
  std::cerr << std::endl << "TREF" << std::endl;
  return std::string("\"TREF\"");
}

std::string TRefWalker::avroTypeName() {
  std::cerr << std::endl << "TREF" << std::endl;
  return "TREF";
}

std::string TRefWalker::avroSchema(int indent, std::set<std::string> &memo) {
  std::cerr << std::endl << "TREF" << std::endl;
  return "TREF";
}

void TRefWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  std::cerr << std::endl << "TREF" << std::endl;
}

void TRefWalker::printJSON(void *address) {
  std::cerr << std::endl << "TREF" << std::endl;
}

#ifdef AVRO
bool TRefWalker::printAvro(void *address, avro_value_t *avrovalue) {
  std::cerr << std::endl << "TREF" << std::endl; return false;
}
#endif

int TRefWalker::getDataSize(void *address, LeafDimension *dim) {
  std::cerr << std::endl << "TREF" << std::endl; return 0;
}

const void *TRefWalker::getData(void *address, int index, LeafDimension *dim) {
  std::cerr << std::endl << "TREF" << std::endl; return nullptr;
}

///////////////////////////////////////////////////////////////////// StdVectorWalker

StdVectorWalker::StdVectorWalker(std::string fieldName, std::string typeName, FieldWalker *walker) :
  FieldWalker(fieldName, typeName),
  walker(walker),
  typeId_(TClass::GetClass(typeName.c_str())->GetTypeInfo()) { }

size_t StdVectorWalker::sizeOf() { return sizeof(std::vector<char>); }

const std::type_info *StdVectorWalker::typeId() { return typeId_; }

bool StdVectorWalker::empty() { return false; }

bool StdVectorWalker::resolved() { return walker->resolved(); }

void StdVectorWalker::resolve(void *address) {
  std::vector<char> *generic = (std::vector<char>*)address;
  int numItems = generic->size() / walker->sizeOf();
  void *ptr = generic->data();
  for (int i = 0;  i < numItems;  i++) {
    walker->resolve(ptr);
    ptr = (void*)((size_t)ptr + walker->sizeOf());
  }
}

std::string StdVectorWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("{\"std::vector\": ") + walker->repr(indent, memo) + std::string("}");
}

std::string StdVectorWalker::avroTypeName() { return "array"; }

std::string StdVectorWalker::avroSchema(int indent, std::set<std::string> &memo) {
  return std::string("{\"type\": \"array\", \"items\": ") + walker->avroSchema(indent, memo) + std::string("}");
}

void StdVectorWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  schemaBuilder(SchemaSequence, this, nullptr, fieldName.c_str());
  walker->buildSchema(schemaBuilder, memo);
}

void StdVectorWalker::printJSON(void *address) {
  std::cout << "[";
  std::vector<char> *generic = (std::vector<char>*)address;
  int numItems = generic->size() / walker->sizeOf();
  void *ptr = generic->data();
  bool first = true;
  for (int i = 0;  i < numItems;  i++) {                     // std::vector<T> are guaranteed to be
    if (first) first = false; else std::cout << ", ";        // contiguous for all T except bool
    walker->printJSON(ptr);
    ptr = (void*)((size_t)ptr + walker->sizeOf());
  }
  std::cout << "]";
}

#ifdef AVRO
bool StdVectorWalker::printAvro(void *address, avro_value_t *avrovalue) {
  avro_value_reset(avrovalue);
  std::vector<char> *generic = (std::vector<char>*)address;
  int numItems = generic->size() / walker->sizeOf();
  void *ptr = generic->data();
  for (int i = 0;  i < numItems;  i++) {                     // see above
    avro_value_t element;
    avro_value_append(avrovalue, &element, nullptr);
    if (!walker->printAvro(ptr, &element))
      return false;
    ptr = (void*)((size_t)ptr + walker->sizeOf());
  }
  return true;
}
#endif

int StdVectorWalker::getDataSize(void *address, LeafDimension *dim) {
  std::vector<char> *generic = (std::vector<char>*)address;
  return generic->size() / walker->sizeOf();
}

const void *StdVectorWalker::getData(void *address, int index, LeafDimension *dim) {
  std::vector<char> *generic = (std::vector<char>*)address;
  int numItems = generic->size() / walker->sizeOf();
  return (void*)((size_t)generic->data() + index * walker->sizeOf());
}

///////////////////////////////////////////////////////////////////// StdVectorBoolWalker

StdVectorBoolWalker::StdVectorBoolWalker(std::string fieldName) :
  FieldWalker(fieldName, "vector<bool>"),
  walker(new BoolWalker(fieldName)) { }

size_t StdVectorBoolWalker::sizeOf() { return sizeof(std::vector<bool>); }

const std::type_info *StdVectorBoolWalker::typeId() { return &typeid(std::vector<bool>); }

bool StdVectorBoolWalker::empty() { return false; }

bool StdVectorBoolWalker::resolved() { return true; }

void StdVectorBoolWalker::resolve(void *address) { }

std::string StdVectorBoolWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("{\"std::vector\": ") + walker->repr(indent, memo) + std::string("}");
}

std::string StdVectorBoolWalker::avroTypeName() { return "array"; }

std::string StdVectorBoolWalker::avroSchema(int indent, std::set<std::string> &memo) {
  return std::string("{\"type\": \"array\", \"items\": ") + walker->avroSchema(indent, memo) + std::string("}");
}

void StdVectorBoolWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  schemaBuilder(SchemaSequence, this, nullptr, fieldName.c_str());
  walker->buildSchema(schemaBuilder, memo);
}

void StdVectorBoolWalker::printJSON(void *address) {
  std::cout << "[";
  std::vector<bool> *vectorBool = (std::vector<bool>*)address;

  int numItems = vectorBool->size();
  bool first = true;
  for (int i = 0;  i < numItems;  i++) {
    if (first) first = false; else std::cout << ", ";
    bool val = vectorBool->at(i);
    walker->printJSON((void*)&val);
  }
  std::cout << "]";
}

#ifdef AVRO
bool StdVectorBoolWalker::printAvro(void *address, avro_value_t *avrovalue) {
  avro_value_reset(avrovalue);
  std::vector<bool> *vectorBool = (std::vector<bool>*)address;
  int numItems = vectorBool->size();
  for (int i = 0;  i < numItems;  i++) {
    avro_value_t element;
    avro_value_append(avrovalue, &element, nullptr);
    bool val = vectorBool->at(i);
    if (!walker->printAvro((void*)&val, &element))
      return false;
  }
  return true;
}
#endif

int StdVectorBoolWalker::getDataSize(void *address, LeafDimension *dim) {
  std::vector<bool> *vectorBool = (std::vector<bool>*)address;
  return vectorBool->size();
}

const void *StdVectorBoolWalker::getData(void *address, int index, LeafDimension *dim) {
  std::vector<bool> *vectorBool = (std::vector<bool>*)address;
  if (vectorBool->at(index))
    return (void*)(&TRUE);
  else
    return (void*)(&FALSE);
}

///////////////////////////////////////////////////////////////////// ArrayWalker

ArrayWalker::ArrayWalker(std::string fieldName, FieldWalker *walker, int numItems) :
  FieldWalker(fieldName, "[]"), walker(walker), numItems(numItems) { }

size_t ArrayWalker::sizeOf() { return 0; }

const std::type_info *ArrayWalker::typeId() { return nullptr; }

bool ArrayWalker::empty() { return false; }

bool ArrayWalker::resolved() { return walker->resolved(); }

void ArrayWalker::resolve(void *address) { walker->resolve(address); }

std::string ArrayWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("{\"[]\": {\"numItems\": ") + std::to_string(numItems) + std::string(", \"sizeOf\": ") + std::to_string(walker->sizeOf()) + std::string(", \"type\": ") + walker->repr(indent, memo) + std::string("}}");
}

std::string ArrayWalker::avroTypeName() { return "array"; }

std::string ArrayWalker::avroSchema(int indent, std::set<std::string> &memo) {
  return std::string("{\"type\": \"array\", \"items\": ") + walker->avroSchema(indent, memo) + std::string("}");
}

void ArrayWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  schemaBuilder(SchemaSequence, this, nullptr, fieldName.c_str());
  walker->buildSchema(schemaBuilder, memo);
}

void ArrayWalker::printJSON(void *address) {
  std::cout << "[";
  void *ptr = address;
  bool first = true;
  for (int i = 0;  i < numItems;  i++) {
    if (first) first = false; else std::cout << ", ";
    walker->printJSON(ptr);
    ptr = (void*)((size_t)ptr + walker->sizeOf());
  }
  std::cout << "]";
}

#ifdef AVRO
bool ArrayWalker::printAvro(void *address, avro_value_t *avrovalue) {
  avro_value_reset(avrovalue);
  void *ptr = address;
  for (int i = 0;  i < numItems;  i++) {
    avro_value_t element;
    avro_value_append(avrovalue, &element, nullptr);
    if (!walker->printAvro(ptr, &element))
      return false;
    ptr = (void*)((size_t)ptr + walker->sizeOf());
  }
  return true;
}
#endif

int ArrayWalker::getDataSize(void *address, LeafDimension *dim) {
  return numItems;
}

const void *ArrayWalker::getData(void *address, int index, LeafDimension *dim) {
  return (void*)((size_t)address + index * walker->sizeOf());
}

///////////////////////////////////////////////////////////////////// TObjArrayWalker

TObjArrayWalker::TObjArrayWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) :
  FieldWalker(fieldName, "TObjArray"), avroNamespace(avroNamespace), defs(defs), walker(nullptr), classToAssert(nullptr) { }

size_t TObjArrayWalker::sizeOf() { return sizeof(TObjArray); }

const std::type_info *TObjArrayWalker::typeId() { return &typeid(TObjArray); }

bool TObjArrayWalker::empty() { return resolved() ? walker->empty() : false; }

bool TObjArrayWalker::resolved() { return walker != nullptr; }

void TObjArrayWalker::resolve(void *address) {
  TObjArray *array = (TObjArray*)address;
  if (!array->IsEmpty()) {
    classToAssert = TClass::GetClass(array->First()->ClassName());
    walker = new ClassWalker(fieldName, classToAssert, avroNamespace, defs);
    ((ClassWalker*)walker)->fill();
    walker->resolve(array->First());
  }
}

std::string TObjArrayWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("{\"TObjArray\": ") + (resolved() ? walker->repr(indent, memo) : std::string("\"?\"")) + std::string("}");
}

std::string TObjArrayWalker::avroTypeName() { return "array"; }

std::string TObjArrayWalker::avroSchema(int indent, std::set<std::string> &memo) {
  if (!resolved())
    throw std::invalid_argument(std::string("resolve TObjArray before attempting to generate Avro schema"));
  return std::string("{\"type\": \"array\", \"items\": ") + walker->avroSchema(indent, memo) + std::string("}");
}

void TObjArrayWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  schemaBuilder(SchemaSequence, this, nullptr, fieldName.c_str());
  walker->buildSchema(schemaBuilder, memo);
}

void TObjArrayWalker::printJSON(void *address) {
  if (!resolved()) resolve(address);
  if (!resolved()) throw std::invalid_argument(std::string("could not resolve TObjArray (is the first one empty?)"));
  TObjArray *array = (TObjArray*)address;
  if (!array->AssertClass(classToAssert))
    throw std::invalid_argument(std::string("TObjArray elements must all have the same class for Avro conversion"));

  std::cout << "[";
  TIter nextItem = (TClonesArray*)address;
  bool first = true;
  for (void *item = (void*)nextItem();  item != nullptr;  item = (void*)nextItem()) {
    if (first) first = false; else std::cout << ", ";
    walker->printJSON(item);
  }
  std::cout << "]";
}

#ifdef AVRO
bool TObjArrayWalker::printAvro(void *address, avro_value_t *avrovalue) {
  avro_value_reset(avrovalue);

  if (!resolved()) resolve(address);
  if (!resolved()) throw std::invalid_argument(std::string("could not resolve TObjArray (is the first one empty?)"));
  TObjArray *array = (TObjArray*)address;
  if (!array->AssertClass(classToAssert))
    throw std::invalid_argument(std::string("TObjArray elements must all have the same class for Avro conversion"));

  TIter nextItem = (TClonesArray*)address;
  for (void *item = (void*)nextItem();  item != nullptr;  item = (void*)nextItem()) {
    avro_value_t element;
    avro_value_append(avrovalue, &element, nullptr);
    if (!walker->printAvro(item, &element))
      return false;
  }

  return true;
}
#endif

int TObjArrayWalker::getDataSize(void *address, LeafDimension *dim) {
  if (!resolved()) resolve(address);
  if (!resolved()) throw std::invalid_argument(std::string("could not resolve TObjArray (is the first one empty?)"));
  TObjArray *array = (TObjArray*)address;
  if (!array->AssertClass(classToAssert))
    throw std::invalid_argument(std::string("TObjArray elements must all have the same class for Avro conversion"));
  return array->GetEntries();
}

const void *TObjArrayWalker::getData(void *address, int index, LeafDimension *dim) {
  if (!resolved()) resolve(address);
  if (!resolved()) throw std::invalid_argument(std::string("could not resolve TObjArray (is the first one empty?)"));
  TObjArray *array = (TObjArray*)address;
  if (!array->AssertClass(classToAssert))
    throw std::invalid_argument(std::string("TObjArray elements must all have the same class for Avro conversion"));
  return (void*)array->At(index);
}

///////////////////////////////////////////////////////////////////// TRefArrayWalker

TRefArrayWalker::TRefArrayWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) :
  FieldWalker(fieldName, "TRefArray"), avroNamespace(avroNamespace), defs(defs), walker(nullptr) { }

size_t TRefArrayWalker::sizeOf() { return sizeof(TRefArray); }

const std::type_info *TRefArrayWalker::typeId() { return &typeid(TRefArray); }

// we won't support TRefArrays, so report it as empty and let it be culled
bool TRefArrayWalker::empty() { return true; }

bool TRefArrayWalker::resolved() { return true; }

void TRefArrayWalker::resolve(void *address) { }

std::string TRefArrayWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("\"TREFARRAY\"");
}

std::string TRefArrayWalker::avroTypeName() {
  std::cerr << std::endl << "TREFARRAY" << std::endl;
  return "TREFARRAY";
}

std::string TRefArrayWalker::avroSchema(int indent, std::set<std::string> &memo) {
  std::cerr << std::endl << "TREFARRAY" << std::endl;
  return "TREFARRAY";
}

void TRefArrayWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  std::cerr << std::endl << "TREFARRAY" << std::endl;
}

void TRefArrayWalker::printJSON(void *address) {
  std::cerr << std::endl << "TREFARRAY" << std::endl;
}

#ifdef AVRO
bool TRefArrayWalker::printAvro(void *address, avro_value_t *avrovalue) {
  std::cerr << std::endl << "TREFARRAY" << std::endl; return false;
}
#endif

int TRefArrayWalker::getDataSize(void *address, LeafDimension *dim) {
  std::cerr << std::endl << "TREFARRAY" << std::endl; return 0;
}

const void *TRefArrayWalker::getData(void *address, int index, LeafDimension *dim) {
  std::cerr << std::endl << "TREFARRAY" << std::endl; return nullptr;
}

///////////////////////////////////////////////////////////////////// TClonesArrayWalker

TClonesArrayWalker::TClonesArrayWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) :
  FieldWalker(fieldName, "TClonesArray"), avroNamespace(avroNamespace), defs(defs), walker(nullptr) { }

size_t TClonesArrayWalker::sizeOf() { return sizeof(TClonesArray); }

const std::type_info *TClonesArrayWalker::typeId() { return &typeid(TClonesArray); }

bool TClonesArrayWalker::empty() { return resolved() ? walker->empty() : false; }

bool TClonesArrayWalker::resolved() { return walker != nullptr; }

void TClonesArrayWalker::resolve(void *address) {
  TClonesArray *array = (TClonesArray*)address;
  walker = new ClassWalker(fieldName, array->GetClass(), avroNamespace, defs);
  ((ClassWalker*)walker)->fill();
  if (!array->IsEmpty())
    walker->resolve(array->First());
}

std::string TClonesArrayWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("{\"TClonesArray\": ") + (resolved() ? walker->repr(indent, memo) : std::string("\"?\"")) + std::string("}");
}

std::string TClonesArrayWalker::avroTypeName() { return "array"; }

std::string TClonesArrayWalker::avroSchema(int indent, std::set<std::string> &memo) {
  if (!resolved())
    throw std::invalid_argument(std::string("resolve TClonesArray before attempting to generate Avro schema"));
  return std::string("{\"type\": \"array\", \"items\": ") + walker->avroSchema(indent, memo) + std::string("}");
}

void TClonesArrayWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  schemaBuilder(SchemaSequence, this, nullptr, fieldName.c_str());
  walker->buildSchema(schemaBuilder, memo);
}

void TClonesArrayWalker::printJSON(void *address) {
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

#ifdef AVRO
bool TClonesArrayWalker::printAvro(void *address, avro_value_t *avrovalue) {
  avro_value_reset(avrovalue);
  if (!resolved()) resolve(address);
  if (!resolved()) throw std::invalid_argument(std::string("could not resolve TClonesArray"));
  TIter nextItem = (TClonesArray*)address;
  for (void *item = (void*)nextItem();  item != nullptr;  item = (void*)nextItem()) {
    avro_value_t element;
    avro_value_append(avrovalue, &element, nullptr);
    if (!walker->printAvro(item, &element))
      return false;
  }
  return true;
}
#endif

int TClonesArrayWalker::getDataSize(void *address, LeafDimension *dim) {
  if (!resolved()) resolve(address);
  if (!resolved()) throw std::invalid_argument(std::string("could not resolve TClonesArray"));
  TClonesArray *array = (TClonesArray*)address;
  return array->GetEntries();
}

const void *TClonesArrayWalker::getData(void *address, int index, LeafDimension *dim) {
  if (!resolved()) resolve(address);
  if (!resolved()) throw std::invalid_argument(std::string("could not resolve TClonesArray"));
  TClonesArray *array = (TClonesArray*)address;
  return (void*)array->At(index);
}

///////////////////////////////////////////////////////////////////// ExtractableWalker

ExtractableWalker::ExtractableWalker(std::string fieldName, std::string typeName) :
  FieldWalker(fieldName, typeName) { }

bool ExtractableWalker::empty() { return false; }

///////////////////////////////////////////////////////////////////// LeafWalker (and LeafDimension)

//// LeafDimension

LeafDimension::LeafDimension(LeafDimension *next, int size) : next_(next), size_(size), counter(nullptr), counterReaderValue(nullptr) { }

LeafDimension::LeafDimension(LeafDimension *next, IntWalker *walker, TTreeReader *reader) : next_(next), size_(-1), counter(walker), counterReaderValue(counter->readerValue(reader)) { }

void LeafDimension::reset(TTreeReader *reader) {
  if (counterReaderValue != nullptr) {
    // FIXME: delete old counterReaderValue?
    counterReaderValue = counter->readerValue(reader);
  }
  if (next_ != nullptr) {
    next_->reset(reader);
  }
}

std::string LeafDimension::repr() {
  if (counter != nullptr)
    return std::string("{\"counter\": \"") + counter->fieldName + std::string("\"}");
  else
    return std::to_string(size_);
}

LeafDimension *LeafDimension::next() { return next_; }

int LeafDimension::size() {
  if (counter != nullptr)
    return counter->value(counterReaderValue);
  else
    return size_;
}

int LeafDimension::flatSize() {
  int out = size();
  if (next_ != nullptr)
    out *= next_->flatSize();
  return out;
}

//// LeafWalker

LeafWalker::LeafWalker(TLeaf *tleaf, TTree *ttree, TTreeReader *reader) :
  ExtractableWalker(tleaf->GetName(), leafToPrimitive(tleaf)->typeName),
  walker(leafToPrimitive(tleaf)),
  readerValue(nullptr),
  readerArray(nullptr)
{
  std::vector<int> intdims;
  std::vector<std::string> strdims;

  const char *title = tleaf->GetTitle();
  for (const char *c = title;  *c != 0;  c++) {
    if (*c == '[') {
      intdims.push_back(0);
      strdims.push_back(std::string());
    }
    if (!intdims.empty()) {
      if ('0' <= *c  &&  *c <= '9')
        intdims.back() = intdims.back() * 10 + (*c - '0');
      if (('0' <= *c  &&  *c <= '9')  ||  ('a' <= *c  &&  *c <= 'z')  ||  ('A' <= *c  &&  *c <= 'Z'))
        strdims.back() = strdims.back() + *c;
    }
  }

  dimensions = intdims.size();
  dims = nullptr;
  for (int i = dimensions - 1;  i >= 0;  i--) {
    if (ttree->GetLeaf(strdims[i].c_str()) != nullptr)
      dims = new LeafDimension(dims, new IntWalker(strdims[i].c_str()), reader);
    else
      dims = new LeafDimension(dims, intdims[i]);
  }

  if (dimensions == 0)
    readerValue = walker->readerValue(reader);
  else
    readerArray = walker->readerArray(reader);
}

PrimitiveWalker *LeafWalker::leafToPrimitive(TLeaf *tleaf) {
  if (tleaf->IsA() == TLeafO::Class()) {
    return new BoolWalker(tleaf->GetName());
  }
  else if (tleaf->IsA() == TLeafB::Class()  &&  !tleaf->IsUnsigned()) {
    return new CharWalker(tleaf->GetName());
  }
  else if (tleaf->IsA() == TLeafB::Class()  &&  tleaf->IsUnsigned()) {
    return new UCharWalker(tleaf->GetName());
  }
  else if (tleaf->IsA() == TLeafS::Class()  &&  !tleaf->IsUnsigned()) {
    return new ShortWalker(tleaf->GetName());
  }
  else if (tleaf->IsA() == TLeafS::Class()  &&  tleaf->IsUnsigned()) {
    return new UShortWalker(tleaf->GetName());
  }
  else if (tleaf->IsA() == TLeafI::Class()  &&  !tleaf->IsUnsigned()) {
    return new IntWalker(tleaf->GetName());
  }
  else if (tleaf->IsA() == TLeafI::Class()  &&  tleaf->IsUnsigned()) {
    return new UIntWalker(tleaf->GetName());
  }
  else if (tleaf->IsA() == TLeafL::Class()  &&  !tleaf->IsUnsigned()) {
    return new LongWalker(tleaf->GetName());
  }
  else if (tleaf->IsA() == TLeafL::Class()  &&  tleaf->IsUnsigned()) {
    return new ULongWalker(tleaf->GetName());
  }
  else if (tleaf->IsA() == TLeafF::Class()  &&  !tleaf->IsUnsigned()) {
    return new FloatWalker(tleaf->GetName());
  }
  else if (tleaf->IsA() == TLeafD::Class()  &&  !tleaf->IsUnsigned()) {
    return new DoubleWalker(tleaf->GetName());
  }
  else if (tleaf->IsA() == TLeafC::Class()) {
    return new CStringWalker(tleaf->GetName());
  }
  else if (tleaf->IsA() == TLeafElement::Class())
    throw std::invalid_argument(std::string("TLeafElement not handled"));
  else if (tleaf->IsA() == TLeafObject::Class())
    throw std::invalid_argument(std::string("TLeafObject not handled"));
  else
    throw std::invalid_argument(std::string(tleaf->IsA()->GetName()) + std::string(" not handled"));
}

size_t LeafWalker::sizeOf() { return walker->sizeOf(); }

const std::type_info *LeafWalker::typeId() { return walker->typeId(); }

bool LeafWalker::resolved() { return true; }

void LeafWalker::resolve(void *address) { }

std::string LeafWalker::repr(int indent, std::set<std::string> &memo) {
  std::string out;
  out += std::string("\"") + fieldName + std::string("\": {\"extractor\": \"TLeaf\", \"dimensions\": [");
  bool first = true;
  for (LeafDimension *dim = dims;  dim != nullptr;  dim = dim->next()) {
    if (first) first = false; else out += std::string(", ");
    out += dim->repr();
  }
  out += std::string("], \"type\": ") + walker->repr(indent, memo) + std::string("}");
  return out;
}

std::string LeafWalker::avroTypeName() { return walker->avroTypeName(); }

std::string LeafWalker::avroSchema(int indent, std::set<std::string> &memo) {
  std::string out = std::string(indent, ' ') + std::string("{\"name\": \"") + fieldName + std::string("\", \"type\": ");

  for (int i = 0;  i < dimensions;  i++)
    out += std::string("{\"type\": \"array\", \"items\": ");

  out += walker->avroSchema(indent, memo) + std::string(dimensions, '}') + std::string("}");
  return out;
}

void LeafWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  for (LeafDimension *d = dims;  d != nullptr;  d = d->next()) {
    std::cout << "LeafWalker::buildSchema adding LeafDimension" << std::endl;

    schemaBuilder(SchemaSequence, this, d, nullptr);
  }

  std::cout << "LeafWalker::buildSchema" << std::endl;

  walker->buildSchema(schemaBuilder, memo);
}

int LeafWalker::printJSONDeep(int readerIndex, int readerSize, LeafDimension *dim) {
  int dimSize = dim->size();

  std::cout << "[";
  bool first = true;
  for (int dimIndex = 0;  dimIndex < dimSize  &&  readerIndex < readerSize;  dimIndex++) {
    if (first) first = false; else std::cout << ", ";
    if (dim->next() == nullptr) {
      walker->printJSON(readerArray, readerIndex);
      readerIndex += 1;
    }
    else
      readerIndex = printJSONDeep(readerIndex, readerSize, dim->next());
  }
  std::cout << "]";

  return readerIndex;
}

void LeafWalker::printJSON(void *address) {
  std::cout << "\"" << fieldName << "\": ";
  if (address != nullptr)
    walker->printJSON(address);
  else
    printJSONDeep(0, dims->flatSize(), dims);
}

#ifdef AVRO
int LeafWalker::printAvroDeep(int readerIndex, int readerSize, LeafDimension *dim, avro_value_t *avrovalue) {
  int dimSize = dim->size();

  for (int dimIndex = 0;  dimIndex < dimSize  &&  readerIndex < readerSize;  dimIndex++) {
    if (dim->next() == nullptr) {
      avro_value_t element;
      avro_value_append(avrovalue, &element, nullptr);
      if (!walker->printAvro(readerArray, readerIndex, &element))
        return -1;
      readerIndex += 1;
    }
    else {
      avro_value_t element;
      avro_value_append(avrovalue, &element, nullptr);
      readerIndex = printAvroDeep(readerIndex, readerSize, dim->next(), &element);
      if (readerIndex == -1)
        return -1;
    }
  }

  return readerIndex;
}

bool LeafWalker::printAvro(void *address, avro_value_t *avrovalue) {
  if (address != nullptr)
    return walker->printAvro(address, avrovalue);
  else {
    avro_value_reset(avrovalue);
    if (printAvroDeep(0, dims->flatSize(), dims, avrovalue) == -1)
      return false;
    else
      return true;
  }
}
#endif

int LeafWalker::getDataSize(void *address, LeafDimension *dim) {
  if (address != nullptr)
    return 0;
  else
    return dim->size();
}

const void *LeafWalker::getData(void *address, int index, LeafDimension *dim) {
  std::cout << "LeafWalker::getData " << address << " " << index << " " << dim << std::endl;

  if (readerValue != nullptr) {
    std::cout << "HERE " << readerValue->GetAddress() << std::endl;

    return walker->getData(readerValue->GetAddress(), -1, dim);
  }
  else {
    int readerIndex = index;
    for (LeafDimension *d = dims;  d != dim;  d = d->next())
      readerIndex += d->size();
    return walker->getData(readerArray, readerIndex, nullptr);
  }
}

void LeafWalker::reset(TTreeReader *reader) {
  // FIXME: delete old readerValue/readerArray?
  if (dimensions == 0)
    readerValue = walker->readerValue(reader);
  else {
    readerArray = walker->readerArray(reader);
    dims->reset(reader);
  }
}

void *LeafWalker::getAddress() {
  if (readerValue != nullptr)
    return readerValue->GetAddress();
  else
    return nullptr;
}

///////////////////////////////////////////////////////////////////// ReaderValueWalker

GenericReaderValue::GenericReaderValue() {}

GenericReaderValue::GenericReaderValue(std::string fieldName, std::string typeName, TTreeReader *reader, FieldWalker *walker) :
  TTreeReaderValueBase(reader, fieldName.c_str(), TDictionary::GetDictionary(*walker->typeId())),
  typeName(typeName) { }

const char *GenericReaderValue::GetDerivedTypeName() const { return typeName.c_str(); }

ReaderValueWalker::ReaderValueWalker(std::string fieldName, TBranch *tbranch, TTreeReader *reader, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) :
  ExtractableWalker(fieldName, tbranch->GetClassName()),
  walker(MemberWalker::specializedWalker(fieldName, typeName, avroNamespace, defs)),
  value(new GenericReaderValue(fieldName, typeName, reader, walker)) { }

size_t ReaderValueWalker::sizeOf() { return walker->sizeOf(); }

const std::type_info *ReaderValueWalker::typeId() { return walker->typeId(); }

bool ReaderValueWalker::resolved() { return walker->resolved(); }

void ReaderValueWalker::resolve(void *address) { walker->resolve(address); }

std::string ReaderValueWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("\"") + fieldName + std::string("\": {\"extractor\": \"TTreeReaderValue\", \"type\": ") + walker->repr(indent, memo) + std::string("}");
}

std::string ReaderValueWalker::avroTypeName() { return walker->avroTypeName(); }

std::string ReaderValueWalker::avroSchema(int indent, std::set<std::string> &memo) {
  return std::string(indent, ' ') + std::string("{\"name\": \"") + fieldName + std::string("\", \"type\": ") + walker->avroSchema(indent, memo) + std::string("}");
}

void ReaderValueWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  walker->buildSchema(schemaBuilder, memo);
}

void ReaderValueWalker::printJSON(void *address) {
  std::cout << "\"" << fieldName << "\": ";
  walker->printJSON(address);
}

#ifdef AVRO
bool ReaderValueWalker::printAvro(void *address, avro_value_t *avrovalue) {
  return walker->printAvro(address, avrovalue);
}
#endif

int ReaderValueWalker::getDataSize(void *address, LeafDimension *dim) {
  return 0;
}

const void *ReaderValueWalker::getData(void *address, int index, LeafDimension *dim) {
  return getAddress();
}

void ReaderValueWalker::reset(TTreeReader *reader) {
  // FIXME: delete old value?
  value = new GenericReaderValue(fieldName, typeName, reader, walker);
}

void *ReaderValueWalker::getAddress() {
  return value->GetAddress();
}

///////////////////////////////////////////////////////////////////// RawTBranchWalker

RawTBranchWalker::RawTBranchWalker(std::string fieldName, std::string typeName, FieldWalker *walker) :
  ExtractableWalker(fieldName, typeName),
  tbranch(nullptr),
  walker(walker) { }

size_t RawTBranchWalker::sizeOf() { return walker->sizeOf(); }

const std::type_info *RawTBranchWalker::typeId() { return walker->typeId(); }

bool RawTBranchWalker::resolved() { return true; }

void RawTBranchWalker::resolve(void *address) { }

std::string RawTBranchWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("\"") + fieldName + std::string("\": {\"extractor\": \"TBranch\", \"type\": ") + typeName + std::string("}");
}

std::string RawTBranchWalker::avroTypeName() { return walker->avroTypeName(); }

std::string RawTBranchWalker::avroSchema(int indent, std::set<std::string> &memo) {
  return std::string(indent, ' ') + std::string("{\"name\": \"") + fieldName + std::string("\", \"type\": ") + walker->avroSchema(indent, memo) + std::string("}");
}

void RawTBranchWalker::buildSchema(SchemaBuilder schemaBuilder, std::set<std::string> &memo) {
  walker->buildSchema(schemaBuilder, memo);
}

void RawTBranchWalker::printJSON(void *address) {
  std::cout << "\"" << fieldName << "\": ";
  walker->printJSON(address);
}

#ifdef AVRO
bool RawTBranchWalker::printAvro(void *address, avro_value_t *avrovalue) {
  return walker->printAvro(address, avrovalue);
}
#endif

int RawTBranchWalker::getDataSize(void *address, LeafDimension *dim) {
  return 0;
}

const void *RawTBranchWalker::getData(void *address, int index, LeafDimension *dim) {
  return address;
}
// FIXME: you probably (almost certainly) want to give RawTBranchStdStringWalker and RawTBranchTStringWalker
// custom ::getData methods that know how to pull c_str() or Data() out of the object.
// But you'll find that in the unit testing...


//// RawTBranchStdStringWalker

RawTBranchStdStringWalker::RawTBranchStdStringWalker(std::string fieldName, TTreeReader *reader) :
  RawTBranchWalker(fieldName, "string", new StdStringWalker(fieldName)),
  data(nullptr),
  reader(reader)
{
  reader->GetTree()->SetBranchAddress(fieldName.c_str(), &data, &tbranch);
}

size_t RawTBranchStdStringWalker::sizeOf() { return sizeof(std::string); }

const std::type_info *RawTBranchStdStringWalker::typeId() { return &typeid(std::string); }

void RawTBranchStdStringWalker::reset(TTreeReader *reader) {
  this->reader = reader;
  reader->GetTree()->SetBranchAddress(fieldName.c_str(), &data, &tbranch);
}

void *RawTBranchStdStringWalker::getAddress() {
  tbranch->GetEntry(reader->GetCurrentEntry());
  return data;
}

//// RawTBranchTStringWalker

RawTBranchTStringWalker::RawTBranchTStringWalker(std::string fieldName, TTreeReader *reader) :
  RawTBranchWalker(fieldName, "TString", new TStringWalker(fieldName)),
  data(nullptr),
  reader(reader)
{
  reader->GetTree()->SetBranchAddress(fieldName.c_str(), &data, &tbranch);
}

size_t RawTBranchTStringWalker::sizeOf() { return sizeof(TString); }

const std::type_info *RawTBranchTStringWalker::typeId() { return &typeid(TString); }

void RawTBranchTStringWalker::reset(TTreeReader *reader) {
  this->reader = reader;
  reader->GetTree()->SetBranchAddress(fieldName.c_str(), &data, &tbranch);
}

void *RawTBranchTStringWalker::getAddress() {
  tbranch->GetEntry(reader->GetCurrentEntry());
  return data;
}

///////////////////////////////////////////////////////////////////// TreeWalker

TreeWalker::TreeWalker(std::string fileLocation, std::string treeLocation, std::string avroNamespace, std::vector<std::string> libs) :
  fileLocation(fileLocation), treeLocation(treeLocation), avroNamespace(avroNamespace)
{
  // load the libraries needed to interpret the data
  for (int i = 0;  i < libs.size();  i++)
    gInterpreter->ProcessLine((std::string(".L ") + libs[i]).c_str());

  valid = tryToOpenFile();
  if (!valid) return;

  TTree *ttree = reader->GetTree();

  TIter nextBranch = ttree->GetListOfBranches();
  for (TBranch *tbranch = (TBranch*)nextBranch();  tbranch != nullptr;  tbranch = (TBranch*)nextBranch()) {
    std::string branchName = tbranch->GetName();
    std::string className = std::string(tbranch->GetClassName());

    if (className.empty()) {
      TIter nextLeaf = tbranch->GetListOfLeaves();
      for (TLeaf *tleaf = (TLeaf*)nextLeaf();  tleaf != nullptr;  tleaf = (TLeaf*)nextLeaf())
        fields.push_back(new LeafWalker(tleaf, ttree, reader));
    }
    else if (className == std::string("string"))
      fields.push_back(new RawTBranchStdStringWalker(branchName, reader));
    else if (className == std::string("TString"))
      fields.push_back(new RawTBranchTStringWalker(branchName, reader));
    else
      fields.push_back(new ReaderValueWalker(branchName, tbranch, reader, avroNamespace, defs));
  }
}

bool TreeWalker::tryToOpenFile() {
  file = TFile::Open(fileLocation.c_str());
  if (file == nullptr  ||  !file->IsOpen()) {
    errorMessage = std::string("File not found: ") + fileLocation;
    return false;
  }

  if (file->IsZombie()) {
    errorMessage = std::string("Not a ROOT file: ") + fileLocation;
    return false;
  }

  reader = new TTreeReader(treeLocation.c_str(), file);
  if (reader->IsZombie()) {
    errorMessage = std::string("Not a TTree: ") + treeLocation.c_str() + std::string(" in file: ") + fileLocation;
    return false;
  }

  return true;
}

void TreeWalker::reset(std::string fileLocation) {
  file->Close();
  delete file;
  delete this->reader;

  this->fileLocation = fileLocation;
  valid = tryToOpenFile();
  if (!valid) return;

  for (auto iter = fields.begin();  iter != fields.end();  ++iter)
    (*iter)->reset(reader);
}

bool TreeWalker::next() {
  return reader->Next();
}

long TreeWalker::numEntriesInCurrentTree() {
  return reader->GetTree()->GetEntries();
}

void TreeWalker::setEntryInCurrentTree(long entry) {
  reader->SetEntry(entry);
}

bool TreeWalker::resolved() {
  for (auto iter = fields.begin();  iter != fields.end();  ++iter)
    if (!(*iter)->resolved())
      return false;
  return true;
}

void TreeWalker::resolve() {
  for (auto iter = fields.begin();  iter != fields.end();  ++iter)
    (*iter)->resolve((*iter)->getAddress());
}

std::string TreeWalker::repr() {
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

std::string TreeWalker::avroSchema() {
  std::set<std::string> memo;
  std::string out;

  out += std::string("{\"type\": \"record\",\n") +
         std::string(" \"name\": \"") + reader->GetTree()->GetName() + std::string("\",\n");
  if (!avroNamespace.empty())
    out += std::string(" \"namespace\": \"") + avroNamespace + std::string("\",\n");
  out += std::string(" \"fields\": [\n");

  bool first = true;
  for (auto iter = fields.begin();  iter != fields.end();  ++iter) {
    if (first) first = false; else out += std::string(",\n");
    out += (*iter)->avroSchema(3, memo);
  }

  out += std::string("\n ]\n}");
  return out;
}

void TreeWalker::buildSchema(SchemaBuilder schemaBuilder) {
  std::set<std::string> memo;

  schemaBuilder(SchemaClassName, nullptr, nullptr, reader->GetTree()->GetName());

  for (auto iter = fields.begin();  iter != fields.end();  ++iter) {
    schemaBuilder(SchemaClassFieldName, *iter, nullptr, (*iter)->fieldName.c_str());
    (*iter)->buildSchema(schemaBuilder, memo);
  }

  schemaBuilder(SchemaClassEnd, nullptr, nullptr, nullptr);
}

void TreeWalker::printJSON() {
  std::cout << "{";
  bool first = true;
  for (auto iter = fields.begin();  iter != fields.end();  ++iter) {
    if (first) first = false; else std::cout << ", ";
    (*iter)->printJSON((*iter)->getAddress());
  }
  std::cout << "}" << std::endl;
}

#ifdef AVRO
bool TreeWalker::printAvroHeaderOnce(std::string &codec, int blockSize) {
  if (!avroHeaderPrinted) {
    std::string schemastr = avroSchema();

    avro_schema_error_t schemaError;
    if (avro_schema_from_json(schemastr.c_str(), schemastr.size(), &schema, &schemaError) != 0) {
      std::cerr << avro_strerror() << std::endl;
      return false;
    }

    avroInterface = avro_generic_class_from_schema(schema);
    if (avroInterface == nullptr) {
      std::cerr << avro_strerror() << std::endl;
      return false;
    }

    if (avro_generic_value_new(avroInterface, &avroValue) != 0) {
      std::cerr << avro_strerror() << std::endl;
      return false;
    }

    for (auto iter = fields.begin();  iter != fields.end();  ++iter)
      if (avro_value_get_by_name(&avroValue, (*iter)->fieldName.c_str(), &((*iter)->avroValue), nullptr) != 0) {
        std::cerr << avro_strerror() << std::endl;
        return false;
      }
    
    std::string path;
    if (avro_file_writer_create_with_codec_fp(stdout, path.c_str(), true, schema, &avroWriter, codec.c_str(), blockSize) != 0) {
      std::cerr << avro_strerror() << std::endl;
      return false;
    }

    avroHeaderPrinted = true;
  }
  return true;
}

bool TreeWalker::printAvro() {
  for (auto iter = fields.begin();  iter != fields.end();  ++iter)
    if (!(*iter)->printAvro((*iter)->getAddress(), &(*iter)->avroValue)) {
      std::cerr << avro_strerror() << std::endl;
      return false;
    }
  avro_file_writer_append_value(avroWriter, &avroValue);
  return true;
}

void TreeWalker::closeAvro() {
  if (avroHeaderPrinted)
    avro_file_writer_close(avroWriter);
}
#endif // AVRO
