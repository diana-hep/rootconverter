#include "datawalker.h"

extern TTreeReader *getReader();

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

std::string BoolWalker::avroTypeName() { return "boolean"; }

void BoolWalker::printJSON(void *address) {
  if (*((bool*)address)) std::cout << "true"; else std::cout << "false";
}

TTreeReaderValueBase *BoolWalker::readerValue() {
  return new TTreeReaderValue<bool>(*getReader(), fieldName.c_str());
}

//// CharWalker

CharWalker::CharWalker(std::string fieldName) : PrimitiveWalker(fieldName, "char") { }

std::string CharWalker::avroTypeName() { return "int"; }

void CharWalker::printJSON(void *address) {
  std::cout << (int)(*((char*)address));
}

TTreeReaderValueBase *CharWalker::readerValue() {
  return new TTreeReaderValue<char>(*getReader(), fieldName.c_str());
}

//// UCharWalker

UCharWalker::UCharWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned char") { }

std::string UCharWalker::avroTypeName() { return "int"; }

void UCharWalker::printJSON(void *address) {
  std::cout << (int)(*((unsigned char*)address));
}

TTreeReaderValueBase *UCharWalker::readerValue() {
  return new TTreeReaderValue<unsigned char>(*getReader(), fieldName.c_str());
}

//// ShortWalker

ShortWalker::ShortWalker(std::string fieldName) : PrimitiveWalker(fieldName, "short") { }

std::string ShortWalker::avroTypeName() { return "int"; }

void ShortWalker::printJSON(void *address) {
  std::cout << *((short*)address);
}

TTreeReaderValueBase *ShortWalker::readerValue() {
  return new TTreeReaderValue<short>(*getReader(), fieldName.c_str());
}

//// UShortWalker

UShortWalker::UShortWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned short") { }

std::string UShortWalker::avroTypeName() { return "int"; }

void UShortWalker::printJSON(void *address) {
  std::cout << *((unsigned short*)address);
}

TTreeReaderValueBase *UShortWalker::readerValue() {
  return new TTreeReaderValue<unsigned short>(*getReader(), fieldName.c_str());
}

//// IntWalker

IntWalker::IntWalker(std::string fieldName) : PrimitiveWalker(fieldName, "int") { }

std::string IntWalker::avroTypeName() { return "int"; }

void IntWalker::printJSON(void *address) {
  std::cout << *((int*)address);
}

TTreeReaderValueBase *IntWalker::readerValue() {
  return new TTreeReaderValue<int>(*getReader(), fieldName.c_str());
}

//// UIntWalker

UIntWalker::UIntWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned int") { }

std::string UIntWalker::avroTypeName() { return "long"; }

void UIntWalker::printJSON(void *address) {
  std::cout << *((unsigned int*)address);
}

TTreeReaderValueBase *UIntWalker::readerValue() {
  return new TTreeReaderValue<unsigned int>(*getReader(), fieldName.c_str());
}

//// LongWalker

LongWalker::LongWalker(std::string fieldName) : PrimitiveWalker(fieldName, "Long64_t") { }

std::string LongWalker::avroTypeName() { return "long"; }

void LongWalker::printJSON(void *address) {
  std::cout << *((long*)address);
}

TTreeReaderValueBase *LongWalker::readerValue() {
  return new TTreeReaderValue<Long64_t>(*getReader(), fieldName.c_str());
}

//// ULongWalker

ULongWalker::ULongWalker(std::string fieldName) : PrimitiveWalker(fieldName, "ULong64_t") { }

std::string ULongWalker::avroTypeName() { return "double"; }

void ULongWalker::printJSON(void *address) {
  std::cout << *((unsigned long*)address);
}

TTreeReaderValueBase *ULongWalker::readerValue() {
  return new TTreeReaderValue<ULong64_t>(*getReader(), fieldName.c_str());
}

//// FloatWalker

FloatWalker::FloatWalker(std::string fieldName) : PrimitiveWalker(fieldName, "float") { }

std::string FloatWalker::avroTypeName() { return "float"; }

void FloatWalker::printJSON(void *address) {
  std::cout << *((float*)address);
}

TTreeReaderValueBase *FloatWalker::readerValue() {
  return new TTreeReaderValue<float>(*getReader(), fieldName.c_str());
}

//// DoubleWalker

DoubleWalker::DoubleWalker(std::string fieldName) : PrimitiveWalker(fieldName, "double") { }

std::string DoubleWalker::avroTypeName() { return "double"; }

void DoubleWalker::printJSON(void *address) {
  std::cout << *((double*)address);
} 

TTreeReaderValueBase *DoubleWalker::readerValue() {
  return new TTreeReaderValue<double>(*getReader(), fieldName.c_str());
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

void CStringWalker::printJSON(void *address) {
  std::cout << "\"";
  printEscapedString((char*)address, std::cout);
  std::cout << "\"";
}

TTreeReaderValueBase *CStringWalker::readerValue() {
  return new TTreeReaderArray<char>(*getReader(), fieldName.c_str());
}

//// StdStringWalker

StdStringWalker::StdStringWalker(std::string fieldName) : AnyStringWalker(fieldName, "string") { }

void StdStringWalker::printJSON(void *address) {
  std::cout << "\"";
  printEscapedString(((std::string*)address)->c_str(), std::cout);
  std::cout << "\"";
}

TTreeReaderValueBase *StdStringWalker::readerValue() {
  // NOTE: ROOT 6.06/00 won't build the following due to "Unknown type and class combination: -1, string" (no dictionary)
  return new TTreeReaderValue<std::string>(*getReader(), fieldName.c_str());
}

//// TStringWalker

TStringWalker::TStringWalker(std::string fieldName) : AnyStringWalker(fieldName, "TString") { }

void TStringWalker::printJSON(void *address) {
  std::cout << "\"";
  printEscapedString(((TString*)address)->Data(), std::cout);
  std::cout << "\"";
}

TTreeReaderValueBase *TStringWalker::readerValue() {
  // NOTE: ROOT 6.06/00 won't build the following due to "Unknown type and class combination: -1, string" (no dictionary)
  return new TTreeReaderValue<TString>(*getReader(), fieldName.c_str());
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
      walker = new ArrayWalker(fieldName, walker, dataMember->GetMaxIndex(i), dataMember->GetUnitSize());
  }
}

FieldWalker *MemberWalker::specializedWalker(std::string fieldName, std::string tn, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) {
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

  else if (tn.substr(0, vectorPrefix.size()) == vectorPrefix  &&  tn.back() == '>') {
    tn = tn.substr(vectorPrefix.size(), tn.size() - vectorPrefix.size() - 1);
    while (!tn.empty()  &&  tn.back() == ' ') tn.pop_back();
    return new StdVectorWalker(fieldName, specializedWalker(fieldName, tn, avroNamespace, defs));
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

void MemberWalker::printJSON(void *address) {
  std::cout << "\"" << fieldName << "\": ";
  walker->printJSON((void*)((size_t)address + offset));
}

///////////////////////////////////////////////////////////////////// ClassWalker

ClassWalker::ClassWalker(std::string fieldName, TClass *tclass, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) :
  FieldWalker(fieldName, tclass->GetName()), tclass(tclass), avroNamespace(avroNamespace), defs(defs) { }

void ClassWalker::fill() {
  TIter nextMember = tclass->GetListOfDataMembers();
  for (TDataMember *dataMember = (TDataMember*)nextMember();  dataMember != nullptr;  dataMember = (TDataMember*)nextMember())
    if (dataMember->GetOffset() > 0) {
      MemberWalker *member = new MemberWalker(dataMember, avroNamespace, defs);
      if (!member->empty())
        members.push_back(member);
    }
}

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

void ClassWalker::printJSON(void *address) {
  std::cout << "{";
  bool first = true;
  for (auto iter = members.begin();  iter != members.end();  ++iter) {
    if (first) first = false; else std::cout << ", ";    
    (*iter)->printJSON(address);
  }
  std::cout << "}";
}

///////////////////////////////////////////////////////////////////// PointerWalker

PointerWalker::PointerWalker(std::string fieldName, FieldWalker *walker) : FieldWalker(fieldName, "*"), walker(walker) { }

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

///////////////////////////////////////////////////////////////////// TRefWalker

TRefWalker::TRefWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) :
  FieldWalker(fieldName, "*"), walker(nullptr) { }

// we won't support TRefs, so report it as empty and let it be culled
bool TRefWalker::empty() { return true; }

bool TRefWalker::resolved() { return true; }

void TRefWalker::resolve(void *address) { }

std::string TRefWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("\"TREF\"");
}

std::string TRefWalker::avroTypeName() { return "TREF"; }

std::string TRefWalker::avroSchema(int indent, std::set<std::string> &memo) { return "TREF"; }

void TRefWalker::printJSON(void *address) { std::cout << "TREF"; }

///////////////////////////////////////////////////////////////////// StdVectorWalker

static int StdVectorExtractorNumber = 0;

StdVectorWalker::StdVectorWalker(std::string fieldName, FieldWalker *walker) :
  FieldWalker(fieldName, "vector"), walker(walker)
{
  std::string codeToDeclare = std::string("class StdVectorExtractor_") + std::to_string(StdVectorExtractorNumber) + std::string(" : public StdVectorInterface {\n") +
                              std::string("public:\n") +
                              std::string("  std::vector<") + typeName + std::string("> *vector;\n") +
                              std::string("  std::vector<") + typeName + std::string(">::iterator iter;\n") +
                              std::string("  void start(void *v) {\n") +
                              std::string("    vector = (std::vector<") + typeName + std::string(">*)v;\n") +
                              std::string("    iter = vector->begin();\n") +
                              std::string("  }\n") +
                              std::string("  void *next() {\n") +
                              std::string("    if (iter == vector->end())\n") +
                              std::string("      return nullptr;\n") +
                              std::string("    std::vector<") + typeName + std::string(">::iterator out = iter;\n") +
                              std::string("    ++iter;\n") +
                              std::string("    return (void*)&(*out);\n") +
                              std::string("  };\n") +
                              std::string("};\n");

  gInterpreter->Declare(codeToDeclare.c_str());

  ClassInfo_t *classInfo = gInterpreter->ClassInfo_Factory((std::string("StdVectorExtractor_") + std::to_string(StdVectorExtractorNumber)).c_str());
  extractorInstance = (StdVectorInterface*)gInterpreter->ClassInfo_New(classInfo);  

  StdVectorExtractorNumber += 1;
}

bool StdVectorWalker::empty() { return false; }

bool StdVectorWalker::resolved() { return walker->resolved(); }

void StdVectorWalker::resolve(void *address) {
  extractorInstance->start(address);
  void *ptr = extractorInstance->next();
  if (ptr != nullptr)
    walker->resolve(ptr);
}

std::string StdVectorWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("{\"std::vector\": ") + walker->repr(indent, memo) + std::string("}");
}

std::string StdVectorWalker::avroTypeName() { return "array"; }

std::string StdVectorWalker::avroSchema(int indent, std::set<std::string> &memo) {
  return std::string("{\"type\": \"array\", \"items\": ") + walker->avroSchema(indent, memo) + std::string("}");
}

void StdVectorWalker::printJSON(void *address) {
  std::cout << "[";
  extractorInstance->start(address);
  for (void *ptr = extractorInstance->next();  ptr != nullptr;  ptr = extractorInstance->next())
    walker->printJSON(address);
  std::cout << "]";
}

///////////////////////////////////////////////////////////////////// ArrayWalker

ArrayWalker::ArrayWalker(std::string fieldName, FieldWalker *walker, int numItems, size_t byteWidth) :
  FieldWalker(fieldName, "[]"), walker(walker), numItems(numItems), byteWidth(byteWidth) { }

bool ArrayWalker::empty() { return false; }

bool ArrayWalker::resolved() { return walker->resolved(); }

void ArrayWalker::resolve(void *address) { walker->resolve(address); }

std::string ArrayWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("{\"[]\": {\"numItems\": ") + std::to_string(numItems) + std::string(", \"byteWidth\": ") + std::to_string(byteWidth) + std::string(", \"type\": ") + walker->repr(indent, memo) + std::string("}}");
}

std::string ArrayWalker::avroTypeName() { return "array"; }

std::string ArrayWalker::avroSchema(int indent, std::set<std::string> &memo) {
  return std::string("{\"type\": \"array\", \"items\": ") + walker->avroSchema(indent, memo) + std::string("}");
}

void ArrayWalker::printJSON(void *address) {
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

///////////////////////////////////////////////////////////////////// TObjArrayWalker

TObjArrayWalker::TObjArrayWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) :
  FieldWalker(fieldName, "TObjArray"), avroNamespace(avroNamespace), defs(defs), walker(nullptr), classToAssert(nullptr) { }

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

///////////////////////////////////////////////////////////////////// TRefArrayWalker

TRefArrayWalker::TRefArrayWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) :
  FieldWalker(fieldName, "TRefArray"), avroNamespace(avroNamespace), defs(defs), walker(nullptr) { }

// we won't support TRefArrays, so report it as empty and let it be culled
bool TRefArrayWalker::empty() { return true; }

bool TRefArrayWalker::resolved() { return true; }

void TRefArrayWalker::resolve(void *address) { }

std::string TRefArrayWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("\"TREFARRAY\"");
}

std::string TRefArrayWalker::avroTypeName() { return "TREFARRAY"; }

std::string TRefArrayWalker::avroSchema(int indent, std::set<std::string> &memo) { return "TREFARRAY"; }

void TRefArrayWalker::printJSON(void *address) { std::cout << "TREFARRAY"; }

///////////////////////////////////////////////////////////////////// TClonesArrayWalker

TClonesArrayWalker::TClonesArrayWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) :
  FieldWalker(fieldName, "TClonesArray"), avroNamespace(avroNamespace), defs(defs), walker(nullptr) { }

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

///////////////////////////////////////////////////////////////////// ExtractableWalker

ExtractableWalker::ExtractableWalker(std::string fieldName, std::string typeName) :
  FieldWalker(fieldName, typeName) { }

bool ExtractableWalker::empty() { return false; }

///////////////////////////////////////////////////////////////////// LeafWalker

LeafWalker::LeafWalker(TLeaf *tleaf, TTree *ttree) :
  ExtractableWalker(tleaf->GetName(), leafToPrimitive(tleaf)->typeName),
  walker(leafToPrimitive(tleaf)),
  readerValue(walker->readerValue()) { }

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

bool LeafWalker::resolved() { return true; }

void LeafWalker::resolve(void *address) { }

std::string LeafWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("\"") + fieldName + std::string("\": {\"extractor\": \"TLeaf\", \"type\": ") + walker->repr(indent, memo) + std::string("}");
}

std::string LeafWalker::avroTypeName() { return walker->avroTypeName(); }

std::string LeafWalker::avroSchema(int indent, std::set<std::string> &memo) {
  return std::string(indent, ' ') + std::string("{\"name\": \"") + fieldName + std::string("\", \"type\": ") + walker->avroSchema(indent, memo) + std::string("}");
}

void LeafWalker::printJSON(void *address) {
  std::cout << "\"" << fieldName << "\": ";
  walker->printJSON(address);
}

void *LeafWalker::getAddress() { return readerValue->GetAddress(); }

///////////////////////////////////////////////////////////////////// ReaderValueWalker

static int ExtractorInterfaceNumber = 0;

ReaderValueWalker::ReaderValueWalker(std::string fieldName, TBranch *tbranch, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) :
  ExtractableWalker(fieldName, tbranch->GetClassName()),
  walker(new ClassWalker(fieldName, TClass::GetClass(tbranch->GetClassName()), avroNamespace, defs))
{
  ((ClassWalker*)walker)->fill();

  std::string codeToDeclare = std::string("class Extractor_") + std::to_string(ExtractorInterfaceNumber) + std::string(" : public ExtractorInterface {\n") +
                              std::string("public:\n") +
                              std::string("  TTreeReaderValue<") + typeName + std::string("> value;\n") +
                              std::string("  Extractor_") + std::to_string(ExtractorInterfaceNumber) + std::string("() : value(*getReader(), \"") + std::string(fieldName) + std::string("\") { }\n") +
                              std::string("  void *getAddress() { return value.GetAddress(); }\n") +
                              std::string("};\n");

  gInterpreter->Declare(codeToDeclare.c_str());

  ClassInfo_t *classInfo = gInterpreter->ClassInfo_Factory((std::string("Extractor_") + std::to_string(ExtractorInterfaceNumber)).c_str());
  extractorInstance = (ExtractorInterface*)gInterpreter->ClassInfo_New(classInfo);  

  ExtractorInterfaceNumber += 1;
}

bool ReaderValueWalker::resolved() { return walker->resolved(); }

void ReaderValueWalker::resolve(void *address) { walker->resolve(address); }

std::string ReaderValueWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("\"") + fieldName + std::string("\": {\"extractor\": \"TTreeReaderValue\", \"type\": ") + walker->repr(indent, memo) + std::string("}");
}

std::string ReaderValueWalker::avroTypeName() { return walker->avroTypeName(); }

std::string ReaderValueWalker::avroSchema(int indent, std::set<std::string> &memo) {
  return std::string(indent, ' ') + std::string("{\"name\": \"") + fieldName + std::string("\", \"type\": ") + walker->avroSchema(indent, memo) + std::string("}");
}

void ReaderValueWalker::printJSON(void *address) {
  std::cout << "\"" << fieldName << "\": ";
  walker->printJSON(address);
}

void *ReaderValueWalker::getAddress() {
  return extractorInstance->getAddress();
}

///////////////////////////////////////////////////////////////////// ReaderArrayWalker

ReaderArrayWalker::ReaderArrayWalker(std::string fieldName, TBranch *tbranch, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) :
  ExtractableWalker(fieldName, tbranch->GetClassName()) { }

bool ReaderArrayWalker::resolved() { return walker->resolved(); }

void ReaderArrayWalker::resolve(void *address) { walker->resolve(address); }

std::string ReaderArrayWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("\"") + fieldName + std::string("\": {\"extractor\": \"TTreeReaderArray\", \"type\": ") + walker->repr(indent, memo) + std::string("}");
}

std::string ReaderArrayWalker::avroTypeName() { return "array"; }

std::string ReaderArrayWalker::avroSchema(int indent, std::set<std::string> &memo) {
  return std::string(indent, ' ') + std::string("{\"name\": \"") + fieldName + std::string("\", \"type\": {\"type\": \"array\", \"items\": ") + walker->avroSchema(indent, memo) + std::string("}}");
}

void ReaderArrayWalker::printJSON(void *address) { }   // stub

void *ReaderArrayWalker::getAddress() { return nullptr; }

///////////////////////////////////////////////////////////////////// RawTBranchWalker

RawTBranchWalker::RawTBranchWalker(std::string fieldName, std::string typeName, FieldWalker *walker) :
  ExtractableWalker(fieldName, typeName),
  tbranch(nullptr),
  walker(walker) { }

bool RawTBranchWalker::resolved() { return true; }

void RawTBranchWalker::resolve(void *address) { }

std::string RawTBranchWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("\"") + fieldName + std::string("\": {\"extractor\": \"TBranch\", \"type\": ") + typeName + std::string("}");
}

std::string RawTBranchWalker::avroTypeName() { return walker->avroTypeName(); }

std::string RawTBranchWalker::avroSchema(int indent, std::set<std::string> &memo) {
  return std::string(indent, ' ') + std::string("{\"name\": \"") + fieldName + std::string("\", \"type\": ") + walker->avroSchema(indent, memo) + std::string("}");
}

void RawTBranchWalker::printJSON(void *address) {
  std::cout << "\"" << fieldName << "\": ";
  walker->printJSON(address);
}

//// RawTBranchStdStringWalker

RawTBranchStdStringWalker::RawTBranchStdStringWalker(std::string fieldName) :
  RawTBranchWalker(fieldName, "string", new StdStringWalker(fieldName)),
  data(nullptr)
{
  getReader()->GetTree()->SetBranchAddress(fieldName.c_str(), &data, &tbranch);
}

void *RawTBranchStdStringWalker::getAddress() {
  tbranch->GetEntry(getReader()->GetCurrentEntry());
  return data;
}

//// RawTBranchTStringWalker

RawTBranchTStringWalker::RawTBranchTStringWalker(std::string fieldName) :
  RawTBranchWalker(fieldName, "TString", new TStringWalker(fieldName)),
  data(nullptr)
{
  getReader()->GetTree()->SetBranchAddress(fieldName.c_str(), &data, &tbranch);
}

void *RawTBranchTStringWalker::getAddress() {
  tbranch->GetEntry(getReader()->GetCurrentEntry());
  return data;
}

///////////////////////////////////////////////////////////////////// TreeWalker

TreeWalker::TreeWalker(std::string avroNamespace) : avroNamespace(avroNamespace) {
  std::string codeToDeclare = std::string("class StdVectorInterface {\n") +
                              std::string("public:\n") +
                              std::string("  virtual void start(void *vector) = 0;\n") +
                              std::string("  virtual void *next() = 0;\n") +
                              std::string("};\n") +
                              std::string("class ExtractorInterface {\n") +
                              std::string("public:\n") +
                              std::string("  virtual void *getAddress() = 0;\n") +
                              std::string("};\n") +
                              std::string("TTreeReader *getReader();\n");
  gInterpreter->Declare(codeToDeclare.c_str());

  TTree *ttree = getReader()->GetTree();

  TIter nextBranch = ttree->GetListOfBranches();
  for (TBranch *tbranch = (TBranch*)nextBranch();  tbranch != nullptr;  tbranch = (TBranch*)nextBranch()) {
    std::string branchName = tbranch->GetName();
    std::string className = std::string(tbranch->GetClassName());

    if (className.empty()) {
      TIter nextLeaf = tbranch->GetListOfLeaves();
      for (TLeaf *tleaf = (TLeaf*)nextLeaf();  tleaf != nullptr;  tleaf = (TLeaf*)nextLeaf())
        fields.push_back(new LeafWalker(tleaf, ttree));
    }
    else if (className == std::string("string"))
      fields.push_back(new RawTBranchStdStringWalker(branchName));
    else if (className == std::string("TString"))
      fields.push_back(new RawTBranchTStringWalker(branchName));
    else
      fields.push_back(new ReaderValueWalker(branchName, tbranch, avroNamespace, defs));
  }
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
         std::string(" \"name\": \"") + getReader()->GetTree()->GetName() + std::string("\",\n");
  if (!avroNamespace.empty())
    out += std::string(" \"namespace\": \"") + avroNamespace + std::string("\",\n");
  out += std::string(" \"fields\": [\n");

  bool first = true;
  for (auto iter = fields.begin();  iter != fields.end();  ++iter) {
    if (first) first = false; else out += std::string(",\n");
    out += (*iter)->avroSchema(3, memo);
  }

  out += std::string("\n ]\n}\n");
  return out;
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
