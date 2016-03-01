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

MemberWalker::MemberWalker(TDataMember *dataMember, std::map<const std::string, ClassWalker*> &classes) :
  FieldWalker(dataMember->GetName(), dataMember->GetTrueTypeName()),
  offset(dataMember->GetOffset())
{
  std::cout << "MemberWalker " << fieldName << "\t\t" << typeName << "\t\t" << offset << std::endl;

  walker = specializedWalker(fieldName, typeName, classes);

  for (int i = dataMember->GetArrayDim() - 1;  i >= 0;  i--)
    walker = new ArrayWalker(fieldName, dataMember->GetMaxIndex(i));
}

FieldWalker *MemberWalker::specializedWalker(std::string fieldName, std::string tn, std::map<const std::string, ClassWalker*> &classes) {
  std::string vectorPrefix("vector<");
  std::string constPrefix("const ");

  while (tn.substr(0, constPrefix.size()) == constPrefix)
    tn = tn.substr(constPrefix.size(), tn.size() - constPrefix.size());

  if (tn == std::string("char*"))
    return new CStringWalker(fieldName);

  else if (!tn.empty()  &&  tn.back() == '*')
    return new PointerWalker(fieldName, specializedWalker(fieldName, tn.substr(0, tn.size() - 1), classes));

  else if (tn.substr(0, vectorPrefix.size()) == vectorPrefix  &&  tn.back() == '>') {
    tn = tn.substr(vectorPrefix.size(), tn.size() - vectorPrefix.size() - 1);
    while (!tn.empty()  &&  tn.back() == ' ') tn.pop_back();
    return new StdVectorWalker(fieldName, specializedWalker(fieldName, tn, classes));
  }

  else if (tn == std::string("TObjArray"))
    return new TObjArrayWalker(fieldName, classes);

  else if (tn == std::string("TClonesArray"))
    return new TClonesArrayWalker(fieldName, classes);

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
        if (classes.count(tn) > 0)
          return classes.at(tn);
        else {
          classes.insert(std::pair<const std::string, ClassWalker*>(tn, nullptr));
          ClassWalker *out = new ClassWalker(fieldName, tclass, classes);
          classes.insert(std::pair<const std::string, ClassWalker*>(tn, out));
          return out;
        }
      }
      else
        throw std::invalid_argument(tn + std::string(" not handled"));
    }
  }
}

ClassWalker::ClassWalker(std::string fieldName, TClass *tclass, std::map<const std::string, ClassWalker*> &classes) : FieldWalker(fieldName, tclass->GetName()) {
  std::cout << "     ClassWalker " << typeName << std::endl;

  TIter next = tclass->GetListOfDataMembers();
  for (TDataMember *dataMember = (TDataMember*)next();  dataMember != nullptr;  dataMember = (TDataMember*)next())
    if (dataMember->GetOffset() > 0)
      members.push_back(new MemberWalker(dataMember, classes));
  std::cout << "     ClassWalker " << typeName << " DONE" << std::endl;
}

LeafWalker::LeafWalker(TLeaf *tleaf, TTree *ttree) : FieldWalker(tleaf->GetName(), determineType(tleaf)) {
  std::cout << "LeafWalker " << fieldName << " " << typeName << std::endl;
}

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

ReaderValueWalker::ReaderValueWalker(std::string fieldName, TBranch *tbranch, std::map<const std::string, ClassWalker*> &classes) : FieldWalker(fieldName, tbranch->GetClassName()) {
  std::cout << "ReaderValueWalker " << fieldName << std::endl;
  walker = new ClassWalker(fieldName, TClass::GetClass(tbranch->GetClassName()), classes);
}

ReaderArrayWalker::ReaderArrayWalker(std::string fieldName, TBranch *tbranch, std::map<const std::string, ClassWalker*> &classes) : FieldWalker(fieldName, tbranch->GetClassName()) {
  std::cout << "ReaderArrayWalker " << fieldName << std::endl;
}

TreeWalker::TreeWalker(TTree *ttree) {
  std::map<const std::string, ClassWalker*> classes;

  TIter nextBranch = ttree->GetListOfBranches();
  for (TBranch *tbranch = (TBranch*)nextBranch();  tbranch != nullptr;  tbranch = (TBranch*)nextBranch()) {
    std::string branchName = tbranch->GetName();
    if (std::string(tbranch->GetClassName()).empty()) {
      TIter nextLeaf = tbranch->GetListOfLeaves();
      for (TLeaf *tleaf = (TLeaf*)nextLeaf();  tleaf != nullptr;  tleaf = (TLeaf*)nextLeaf())
        fields.push_back(new LeafWalker(tleaf, ttree));
    }
    else
      fields.push_back(new ReaderValueWalker(branchName, tbranch, classes));
  }
}
