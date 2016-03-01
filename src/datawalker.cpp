#include <TList.h>
#include <TLeafO.h>
#include <TLeafB.h>
#include <TLeafS.h>
#include <TLeafI.h>
#include <TLeafL.h>
#include <TLeafF.h>
#include <TLeafD.h>
#include <TLeafC.h>
#include <TLeafElement.h>
#include <TLeafObject.h>
#include <TClonesArray.h>

#include "datawalker.h"

#include "../test_Event/Event.h"

///////////////////////////////////////////////////////////////////// FieldWalker

FieldWalker::FieldWalker(std::string fieldName, std::string typeName) :
  fieldName(fieldName), typeName(typeName) { }

///////////////////////////////////////////////////////////////////// PrimitiveWalkers

PrimitiveWalker::PrimitiveWalker(std::string fieldName, std::string typeName) :
  FieldWalker(fieldName, typeName) { }

bool PrimitiveWalker::empty() { return false; }

bool PrimitiveWalker::resolved() { return true; }

void PrimitiveWalker::resolve(void *address) { }

std::string PrimitiveWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("\"") + typeName + std::string("\"");
}

//// BoolWalker

BoolWalker::BoolWalker(std::string fieldName) : PrimitiveWalker(fieldName, "bool") { }

std::string BoolWalker::avroTypeName() { return "boolean"; }

void BoolWalker::printJSON(void *address) {
  if (*((bool*)address)) std::cout << "true"; else std::cout << "false";
}

//// CharWalker

CharWalker::CharWalker(std::string fieldName) : PrimitiveWalker(fieldName, "char") { }

std::string CharWalker::avroTypeName() { return "int"; }

void CharWalker::printJSON(void *address) {
  std::cout << (int)(*((char*)address));
}

//// UCharWalker

UCharWalker::UCharWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned char") { }

std::string UCharWalker::avroTypeName() { return "int"; }

void UCharWalker::printJSON(void *address) {
  std::cout << (int)(*((unsigned char*)address));
}

//// ShortWalker

ShortWalker::ShortWalker(std::string fieldName) : PrimitiveWalker(fieldName, "short") { }

std::string ShortWalker::avroTypeName() { return "int"; }

void ShortWalker::printJSON(void *address) {
  std::cout << *((short*)address);
}

//// UShortWalker

UShortWalker::UShortWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned short") { }

std::string UShortWalker::avroTypeName() { return "int"; }

void UShortWalker::printJSON(void *address) {
  std::cout << *((unsigned short*)address);
}

//// IntWalker

IntWalker::IntWalker(std::string fieldName) : PrimitiveWalker(fieldName, "int") { }

std::string IntWalker::avroTypeName() { return "int"; }

void IntWalker::printJSON(void *address) {
  std::cout << *((int*)address);
}

//// UIntWalker

UIntWalker::UIntWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned int") { }

std::string UIntWalker::avroTypeName() { return "long"; }

void UIntWalker::printJSON(void *address) {
  std::cout << *((unsigned int*)address);
}

//// LongWalker

LongWalker::LongWalker(std::string fieldName) : PrimitiveWalker(fieldName, "long") { }

std::string LongWalker::avroTypeName() { return "long"; }

void LongWalker::printJSON(void *address) {
  std::cout << *((long*)address);
}

//// ULongWalker

ULongWalker::ULongWalker(std::string fieldName) : PrimitiveWalker(fieldName, "unsigned long") { }

std::string ULongWalker::avroTypeName() { return "double"; }

void ULongWalker::printJSON(void *address) {
  std::cout << *((unsigned long*)address);
}

//// FloatWalker

FloatWalker::FloatWalker(std::string fieldName) : PrimitiveWalker(fieldName, "float") { }

std::string FloatWalker::avroTypeName() { return "float"; }

void FloatWalker::printJSON(void *address) {
  std::cout << *((float*)address);
}

//// DoubleWalker

DoubleWalker::DoubleWalker(std::string fieldName) : PrimitiveWalker(fieldName, "double") { }

std::string DoubleWalker::avroTypeName() { return "double"; }

void DoubleWalker::printJSON(void *address) {
  std::cout << *((double*)address);
} 

///////////////////////////////////////////////////////////////////// MemberWalker

MemberWalker::MemberWalker(TDataMember *dataMember, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) :
  FieldWalker(dataMember->GetName(), dataMember->GetTrueTypeName()),
  offset(dataMember->GetOffset())
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
    else if (tn == std::string("long")  ||  tn == std::string("Long_t")  ||  tn == std::string("Long64_t"))
      return new LongWalker(fieldName);
    else if (tn == std::string("unsigned long")  ||  tn == std::string("ULong_t")  ||  tn == std::string("ULong64_t"))
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

///////////////////////////////////////////////////////////////////// AnyStringWalkers

AnyStringWalker::AnyStringWalker(std::string fieldName, std::string typeName) :
  FieldWalker(fieldName, typeName) { }

bool AnyStringWalker::empty() { return false; }

bool AnyStringWalker::resolved() { return true; }

void AnyStringWalker::resolve(void *address) { }

std::string AnyStringWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("\"") + typeName + std::string("\"");
}
std::string AnyStringWalker::escapeJSON(std::string string) { return string; }

std::string AnyStringWalker::avroTypeName() { return "string"; }

//// CStringWalker

CStringWalker::CStringWalker(std::string fieldName) : AnyStringWalker(fieldName, "char*") { }

void CStringWalker::printJSON(void *address) {
  std::cout << "\"" << escapeJSON((char*)address) << "\"";
}

//// StdStringWalker

StdStringWalker::StdStringWalker(std::string fieldName) : AnyStringWalker(fieldName, "string") { }

void StdStringWalker::printJSON(void *address) {
  std::cout << "\"" << escapeJSON(*((std::string*)address)) << "\"";
}

//// TStringWalker

TStringWalker::TStringWalker(std::string fieldName) : AnyStringWalker(fieldName, "TString") { }

void TStringWalker::printJSON(void *address) {
  std::cout << "\"" << escapeJSON(((TString*)address)->Data()) << "\"";
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

bool TRefWalker::empty() { return false; }

bool TRefWalker::resolved() { return false; }   // stub

void TRefWalker::resolve(void *address) { }   // stub

std::string TRefWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("{\"TRef\": ") + (resolved() ? walker->repr(indent, memo) : std::string("\"?\"")) + std::string("}");
}

std::string TRefWalker::avroTypeName() { return "string"; }   // FIXME: can the reference be missing?

void TRefWalker::printJSON(void *address) { std::cout << "TREF"; }   // stub

///////////////////////////////////////////////////////////////////// StdVectorWalker

StdVectorWalker::StdVectorWalker(std::string fieldName, FieldWalker *walker) :
  FieldWalker(fieldName, "vector"), walker(walker) { }

bool StdVectorWalker::empty() { return false; }

bool StdVectorWalker::resolved() { return walker->resolved(); }

void StdVectorWalker::resolve(void *address) { walker->resolve(address); }

std::string StdVectorWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("{\"std::vector\": ") + walker->repr(indent, memo) + std::string("}");
}

std::string StdVectorWalker::avroTypeName() { return "array"; }

void StdVectorWalker::printJSON(void *address) { std::cout << "STD-VECTOR"; }   // stub

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
  FieldWalker(fieldName, "TObjArray"), avroNamespace(avroNamespace), defs(defs), walker(nullptr) { }

bool TObjArrayWalker::empty() { return false; }

bool TObjArrayWalker::resolved() { return false; }   // stub

void TObjArrayWalker::resolve(void *address) { }   // stub

std::string TObjArrayWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("{\"TObjArray\": ") + (resolved() ? walker->repr(indent, memo) : std::string("\"?\"")) + std::string("}");
}
std::string TObjArrayWalker::avroTypeName() { return "array"; }

void TObjArrayWalker::printJSON(void *address) { std::cout << "TOBJARRAY"; }   // stub

///////////////////////////////////////////////////////////////////// TRefArrayWalker

TRefArrayWalker::TRefArrayWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) :
  FieldWalker(fieldName, "TRefArray"), avroNamespace(avroNamespace), defs(defs), walker(nullptr) { }

bool TRefArrayWalker::empty() { return false; }

bool TRefArrayWalker::resolved() { return false; }   // stub

void TRefArrayWalker::resolve(void *address) { }   // stub

std::string TRefArrayWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("{\"TRefArray\": ") + (resolved() ? walker->repr(indent, memo) : std::string("\"?\"")) + std::string("}");
}

std::string TRefArrayWalker::avroTypeName() { return "array"; }

void TRefArrayWalker::printJSON(void *address) { std::cout << "TREFARRAY"; }   // stub

///////////////////////////////////////////////////////////////////// TClonesArrayWalker

TClonesArrayWalker::TClonesArrayWalker(std::string fieldName, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) :
  FieldWalker(fieldName, "TClonesArray"), avroNamespace(avroNamespace), defs(defs), walker(nullptr) { }

bool TClonesArrayWalker::empty() { return resolved() ? walker->empty() : false; }

bool TClonesArrayWalker::resolved() { return walker != nullptr; }   // stub

void TClonesArrayWalker::resolve(void *address) {
  walker = new ClassWalker(fieldName, ((TClonesArray*)address)->GetClass(), avroNamespace, defs);
  ((ClassWalker*)walker)->fill();
}

std::string TClonesArrayWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("{\"TClonesArray\": ") + (resolved() ? walker->repr(indent, memo) : std::string("\"?\"")) + std::string("}");
}

std::string TClonesArrayWalker::avroTypeName() { return "array"; }

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
  ExtractableWalker(tleaf->GetName(), determineType(tleaf)) { }

std::string LeafWalker::determineType(TLeaf *tleaf) {
  if (tleaf->IsA() == TLeafO::Class()) {
    walker = new BoolWalker(tleaf->GetName());
    return std::string("bool");
  }
  else if (tleaf->IsA() == TLeafB::Class()  &&  !tleaf->IsUnsigned()) {
    walker = new CharWalker(tleaf->GetName());
    return std::string("char");
  }
  else if (tleaf->IsA() == TLeafB::Class()  &&  tleaf->IsUnsigned()) {
    walker = new UCharWalker(tleaf->GetName());
    return std::string("unsigned char");
  }
  else if (tleaf->IsA() == TLeafS::Class()  &&  !tleaf->IsUnsigned()) {
    walker = new ShortWalker(tleaf->GetName());
    return std::string("short");
  }
  else if (tleaf->IsA() == TLeafS::Class()  &&  tleaf->IsUnsigned()) {
    walker = new UShortWalker(tleaf->GetName());
    return std::string("unsigned short");
  }
  else if (tleaf->IsA() == TLeafI::Class()  &&  !tleaf->IsUnsigned()) {
    walker = new IntWalker(tleaf->GetName());
    return std::string("int");
  }
  else if (tleaf->IsA() == TLeafI::Class()  &&  tleaf->IsUnsigned()) {
    walker = new UIntWalker(tleaf->GetName());
    return std::string("unsigned int");
  }
  else if (tleaf->IsA() == TLeafL::Class()  &&  !tleaf->IsUnsigned()) {
    walker = new LongWalker(tleaf->GetName());
    return std::string("long");
  }
  else if (tleaf->IsA() == TLeafL::Class()  &&  tleaf->IsUnsigned()) {
    walker = new ULongWalker(tleaf->GetName());
    return std::string("unsigned long");
  }
  else if (tleaf->IsA() == TLeafF::Class()  &&  !tleaf->IsUnsigned()) {
    walker = new FloatWalker(tleaf->GetName());
    return std::string("float");
  }
  else if (tleaf->IsA() == TLeafD::Class()  &&  !tleaf->IsUnsigned()) {
    walker = new DoubleWalker(tleaf->GetName());
    return std::string("double");
  }
  else if (tleaf->IsA() == TLeafC::Class()) {
    walker = new CStringWalker(tleaf->GetName());
    return std::string("string");
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

void LeafWalker::printJSON(void *address) { }   // stub

void *LeafWalker::getAddress() { return nullptr; }

///////////////////////////////////////////////////////////////////// ReaderValueWalker

ReaderValueWalker::ReaderValueWalker(std::string fieldName, TBranch *tbranch, std::string avroNamespace, std::map<const std::string, ClassWalker*> &defs) : ExtractableWalker(fieldName, tbranch->GetClassName()) {
  walker = new ClassWalker(fieldName, TClass::GetClass(tbranch->GetClassName()), avroNamespace, defs);
  ((ClassWalker*)walker)->fill();

  std::string codeToDeclare = std::string("class Get_") + fieldName + std::string(" : public ExtractorInterface {\n") +
                              std::string("public:\n") +
                              std::string("  TTreeReaderValue<") + typeName + std::string("> value;\n") +
                              std::string("  Get_") + fieldName + std::string("() : value(*getReader(), \"") + std::string(fieldName) + std::string("\") { }\n") +
                              std::string("  void *getAddress() { return value.GetAddress(); }\n") +
                              std::string("};\n");

  gInterpreter->Declare(codeToDeclare.c_str());

  ClassInfo_t *classInfo = gInterpreter->ClassInfo_Factory((std::string("Get_") + fieldName).c_str());
  extractorInstance = (ExtractorInterface*)gInterpreter->ClassInfo_New(classInfo);  
}

bool ReaderValueWalker::resolved() { return walker->resolved(); }

void ReaderValueWalker::resolve(void *address) { walker->resolve(address); }

std::string ReaderValueWalker::repr(int indent, std::set<std::string> &memo) {
  return std::string("\"") + fieldName + std::string("\": {\"extractor\": \"TTreeReaderValue\", \"type\": ") + walker->repr(indent, memo) + std::string("}");
}

std::string ReaderValueWalker::avroTypeName() { return walker->avroTypeName(); }

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

void ReaderArrayWalker::printJSON(void *address) { }   // stub

void *ReaderArrayWalker::getAddress() { return nullptr; }

///////////////////////////////////////////////////////////////////// TreeWalker

TreeWalker::TreeWalker(TTree *ttree, std::string avroNamespace) {
  std::string codeToDeclare = std::string("class ExtractorInterface {\n") +
                              std::string("public:\n") +
                              std::string("  virtual void *getAddress() = 0;\n") +
                              std::string("};\n") +
                              std::string("TTreeReader *getReader();\n");
  gInterpreter->Declare(codeToDeclare.c_str());

  TIter nextBranch = ttree->GetListOfBranches();
  for (TBranch *tbranch = (TBranch*)nextBranch();  tbranch != nullptr;  tbranch = (TBranch*)nextBranch()) {
    std::string branchName = tbranch->GetName();
    if (std::string(tbranch->GetClassName()).empty()) {
      TIter nextLeaf = tbranch->GetListOfLeaves();
      for (TLeaf *tleaf = (TLeaf*)nextLeaf();  tleaf != nullptr;  tleaf = (TLeaf*)nextLeaf())
        fields.push_back(new LeafWalker(tleaf, ttree));
    }
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

void TreeWalker::printJSON() {
  std::cout << "{";
  bool first = true;
  for (auto iter = fields.begin();  iter != fields.end();  ++iter) {
    if (first) first = false; else std::cout << ", ";
    (*iter)->printJSON((*iter)->getAddress());
  }
  std::cout << "}" << std::endl;
}
