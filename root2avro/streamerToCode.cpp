#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TVirtualStreamerInfo.h"
#include "TStreamerElement.h"

class ClassStructure {
public:
  TClass *tclass;
  TBranch *tbranch;
  ClassStructure(TClass *tclass, TBranch *tbranch) : tclass(tclass), tbranch(tbranch) { }
};

void classesFromBranch(TBranch *tbranch, TClass *tclass, std::map<std::string, ClassStructure> &classes);

int main(int argc, char **argv) {
  TTree *ttree;

  // TFile *tfile = TFile::Open("test_Event/Event.root");
  // tfile->GetObject("T", ttree);

  TFile *tfile = TFile::Open("Event-0.root");
  tfile->GetObject("t4", ttree);

  std::map<std::string, ClassStructure> classes;

  TIter listOfBranches = ttree->GetListOfBranches();
  for (TBranch *tbranch = (TBranch*)listOfBranches.Next();  tbranch != nullptr;  tbranch = (TBranch*)listOfBranches.Next()) {
    std::cout << std::endl << "******** TTree branch: " << tbranch->GetName() << " " << tbranch->GetClassName() << std::endl;

    TClass *tclass = TClass::GetClass(tbranch->GetClassName());
    std::cout << "TClass " << tclass << std::endl;

    if (tclass != nullptr)
      classesFromBranch(tbranch, tclass, classes);
  }

  return 0;
}

void classesFromBranch(TBranch *tbranch, TClass *tclass, std::map<std::string, ClassStructure> &classes) {
  std::cout << "tbranch->GetName() " << tbranch->GetName() << std::endl;

  TVirtualStreamerInfo *tVirtualStreamerInfo = tclass->GetStreamerInfo();
  std::cout << "tVirtualStreamerInfo " << tVirtualStreamerInfo->GetElements()->GetEntries() << std::endl;

  TIter listOfBranches = tbranch->GetListOfBranches();
  TIter elements = tVirtualStreamerInfo->GetElements();

  TStreamerElement *tStreamerElement;
  TBranch *subbranch;
  while (tStreamerElement != nullptr) {
    tStreamerElement = (TStreamerElement*)elements.Next();
    subbranch = (TBranch*)listOfBranches.Next();
    if (tStreamerElement == nullptr) break;

    if (subbranch == nullptr)
      std::cout << "TStreamerElement " << tStreamerElement->GetName() << " (" << tStreamerElement->GetTitle() << ")" << std::endl;
    else
      std::cout << "TStreamerElement/TBranch " << tStreamerElement->GetName() << " " << subbranch->GetName() << " (" << tStreamerElement->GetTitle() << ") (" << subbranch->GetTitle() << ")" << std::endl;

    Bool_t pointer = false;
    switch (tStreamerElement->GetType()) {
    case TVirtualStreamerInfo::kBool:     std::cout << "    Bool_t" << std::endl;  break;
    case TVirtualStreamerInfo::kChar:     std::cout << "    Char_t" << std::endl;  break;
    case TVirtualStreamerInfo::kShort:    std::cout << "    Short_t" << std::endl;  break;
    case TVirtualStreamerInfo::kInt:      std::cout << "    Int_t" << std::endl;  break;
    case TVirtualStreamerInfo::kLong:     std::cout << "    Long_t" << std::endl;  break;
    case TVirtualStreamerInfo::kLong64:   std::cout << "    Long64_t" << std::endl;  break;
    case TVirtualStreamerInfo::kFloat:    std::cout << "    Float_t" << std::endl;  break;
    case TVirtualStreamerInfo::kFloat16:  std::cout << "    Float16_t" << std::endl;  break;
    case TVirtualStreamerInfo::kDouble:   std::cout << "    Double_t" << std::endl;  break;
    case TVirtualStreamerInfo::kDouble32: std::cout << "    Double32_t" << std::endl;  break;
    case TVirtualStreamerInfo::kUChar:    std::cout << "    UChar_t" << std::endl;  break;
    case TVirtualStreamerInfo::kUShort:   std::cout << "    UShort_t" << std::endl;  break;
    case TVirtualStreamerInfo::kUInt:     std::cout << "    UInt_t" << std::endl;  break;
    case TVirtualStreamerInfo::kULong:    std::cout << "    ULong_t" << std::endl;  break;
    case TVirtualStreamerInfo::kULong64:  std::cout << "    ULong64_t" << std::endl;  break;
    case TVirtualStreamerInfo::kBits:     std::cout << "    UInt_t" << std::endl;  break;

    case TVirtualStreamerInfo::kCharStar:
      std::cout << "    Char_t*" << std::endl;  break;

    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kBool:     std::cout << "    Bool_t[]" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kChar:     std::cout << "    Char_t[]" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kShort:    std::cout << "    Short_t[]" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kInt:      std::cout << "    Int_t[]" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kLong:     std::cout << "    Long_t[]" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kLong64:   std::cout << "    Long64_t[]" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kFloat:    std::cout << "    Float_t[]" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kFloat16:  std::cout << "    Float16_t[]" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kDouble:   std::cout << "    Double_t[]" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kDouble32: std::cout << "    Double32_t[]" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kUChar:    std::cout << "    UChar_t[]" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kUShort:   std::cout << "    UShort_t[]" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kUInt:     std::cout << "    UInt_t[]" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kULong:    std::cout << "    ULong_t[]" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kULong64:  std::cout << "    ULong64_t[]" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kBits:     std::cout << "    UInt_t[]" << std::endl;  break;

    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kBool:     std::cout << "    Bool_t*" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kChar:     std::cout << "    Char_t*" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kShort:    std::cout << "    Short_t*" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kInt:      std::cout << "    Int_t*" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kLong:     std::cout << "    Long_t*" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kLong64:   std::cout << "    Long64_t*" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kFloat:    std::cout << "    Float_t*" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kFloat16:  std::cout << "    Float16_t*" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kDouble:   std::cout << "    Double_t*" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kDouble32: std::cout << "    Double32_t*" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kUChar:    std::cout << "    UChar_t*" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kUShort:   std::cout << "    UShort_t*" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kUInt:     std::cout << "    UInt_t*" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kULong:    std::cout << "    ULong_t*" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kULong64:  std::cout << "    ULong64_t*" << std::endl;  break;
    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kBits:     std::cout << "    UInt_t*" << std::endl;  break;

    case TVirtualStreamerInfo::kCounter:
      std::cout << "    Int_t" << std::endl;  break;

    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kObjectp:
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kObjectP:
    case TVirtualStreamerInfo::kObjectp:
    case TVirtualStreamerInfo::kObjectP:
    case TVirtualStreamerInfo::kAnyp:
    case TVirtualStreamerInfo::kAnyP:
    case TVirtualStreamerInfo::kSTL + TVirtualStreamerInfo::kObjectp:
    case TVirtualStreamerInfo::kSTL + TVirtualStreamerInfo::kObjectP:
      pointer = true;

    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kObject:
    case TVirtualStreamerInfo::kObject:
    case TVirtualStreamerInfo::kTString:
    case TVirtualStreamerInfo::kTNamed:
    case TVirtualStreamerInfo::kTObject:
    case TVirtualStreamerInfo::kAny:
    case TVirtualStreamerInfo::kBase:
    case TVirtualStreamerInfo::kSTL:
      TClass *elementClass = tStreamerElement->GetClassPointer();

      std::cout << "    " << elementClass->GetName() << std::endl;  break;

    }
  }
}
