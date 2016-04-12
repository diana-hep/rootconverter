#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TVirtualStreamerInfo.h"
#include "TStreamerElement.h"
#include "TClonesArray.h"
#include "TTreeGeneratorBase.h"

class MemberStructure {
public:
  std::string type;
  bool pointer;
  std::string variable;
  std::string variableWithArray;
  std::string comment;

  MemberStructure(std::string type, bool pointer, std::string variable, std::string variableWithArray, std::string comment) :
    type(type), pointer(pointer), variable(variable), variableWithArray(variableWithArray), comment(comment) { }

  std::string cpp(int indent) {
    std::string out;
    for (int i = 0;  i < indent;  i++) out += " ";
    out += type + " ";
    if (pointer)
      out += "*";
    out += variableWithArray + ";";
    if (!comment.empty())
      out += "    // " + comment;
    return out;
  }
};

class ClassStructure {
public:
  TClass *tclass;
  TBranch *tbranch;
  ClassStructure(TClass *tclass, TBranch *tbranch) : tclass(tclass), tbranch(tbranch) { }
};

void classesFromBranch(TBranch *tbranch, TClass *tclass, std::map<std::string, ClassStructure> &classes, int prefix);

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

    if (tclass != nullptr  &&  tbranch->GetListOfBranches()->GetEntries() > 0)
      classesFromBranch(tbranch, tclass, classes, 0);
  }

  return 0;
}

void classesFromBranch(TBranch *tbranch, TClass *tclass, std::map<std::string, ClassStructure> &classes, int prefix) {
  TVirtualStreamerInfo *tVirtualStreamerInfo = tclass->GetStreamerInfo();

  TIter listOfBranches = tbranch->GetListOfBranches();
  TIter elements = tVirtualStreamerInfo->GetElements();

  std::vector<MemberStructure> bases;
  std::vector<MemberStructure> members;

  TStreamerElement *tStreamerElement;
  TBranch *subbranch;
  while (true) {
    tStreamerElement = (TStreamerElement*)elements.Next();
    subbranch = (TBranch*)listOfBranches.Next();

    if (tStreamerElement == nullptr) break;

    std::string variable = tStreamerElement->GetName();
    std::string variableWithArray = subbranch->GetName();
    variableWithArray = variableWithArray.substr(prefix, std::string::npos);

    std::string comment = tStreamerElement->GetTitle();

    std::string type;
    Bool_t pointer = false;
    switch (tStreamerElement->GetType()) {

    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kBool:
      pointer = true;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kBool:
    case TVirtualStreamerInfo::kBool:
      type = "Bool_t";
      break;

    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kChar:
      pointer = true;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kChar:
    case TVirtualStreamerInfo::kChar:
      type = "Char_t";
      break;

    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kShort:
      pointer = true;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kShort:
    case TVirtualStreamerInfo::kShort:
      type = "Short_t";
      break;

    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kInt:
      pointer = true;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kInt:
    case TVirtualStreamerInfo::kInt:
      type = "Int_t";
      break;

    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kLong:
      pointer = true;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kLong:
    case TVirtualStreamerInfo::kLong:
      type = "Long_t";
      break;

    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kLong64:
      pointer = true;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kLong64:
    case TVirtualStreamerInfo::kLong64:
      type = "Long64_t";
      break;

    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kFloat:
      pointer = true;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kFloat:
    case TVirtualStreamerInfo::kFloat:
      type = "Float_t";
      break;

    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kFloat16:
      pointer = true;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kFloat16:
    case TVirtualStreamerInfo::kFloat16:
      type = "Float16_t";
      break;

    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kDouble:
      pointer = true;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kDouble:
    case TVirtualStreamerInfo::kDouble:
      type = "Double_t";
      break;

    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kDouble32:
      pointer = true;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kDouble32:
    case TVirtualStreamerInfo::kDouble32:
      type = "Double32_t";
      break;

    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kUChar:
      pointer = true;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kUChar:
    case TVirtualStreamerInfo::kUChar:
      type = "UChar_t";
      break;

    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kUShort:
      pointer = true;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kUShort:
    case TVirtualStreamerInfo::kUShort:
      type = "UShort_t";
      break;

    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kUInt:
      pointer = true;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kUInt:
    case TVirtualStreamerInfo::kUInt:
      type = "UInt_t";
      break;

    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kULong:
      pointer = true;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kULong:
    case TVirtualStreamerInfo::kULong:
      type = "ULong_t";
      break;

    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kULong64:
      pointer = true;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kULong64:
    case TVirtualStreamerInfo::kULong64:
      type = "ULong64_t";
      break;

    case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kBits:
      pointer = true;
    case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kBits:
    case TVirtualStreamerInfo::kBits:
      type = "UInt_t";
      break;

    case TVirtualStreamerInfo::kCharStar:
      type = "Char_t*";
      break;

    case TVirtualStreamerInfo::kCounter:
      type = "Int_t";
      break;

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
      type = elementClass->GetName();

      if (elementClass == TClonesArray::Class()) {
        ROOT::Internal::TTreeGeneratorBase ttreeGenerator(tbranch->GetTree(), "");
        std::string className = ttreeGenerator.GetContainedClassName((TBranchElement*)subbranch, tStreamerElement, pointer).Data();
        TClass *otherClass = TClass::GetClass(className.c_str());
        classesFromBranch(subbranch, otherClass, classes, variableWithArray.size() + 1);
      }
    }

    if (tStreamerElement->IsBase())
      bases.push_back(MemberStructure(type, pointer, variable, variableWithArray, comment));
    else
      members.push_back(MemberStructure(type, pointer, variable, variableWithArray, comment));

  }

  std::cout << "class " << tclass->GetName();
  if (!bases.empty()) {
    std::cout << " : public";
    for (int i = 0;  i < bases.size();  i++)
      if (i == 0)
        std::cout << " " << bases[i].type;
      else
        std::cout << ", " << bases[i].type;
  }
  std::cout << " {" << std::endl << "  public:" << std::endl;

  for (int i = 0;  i < members.size();  i++)
    std::cout << members[i].cpp(4) << std::endl;

  std::cout << "};" << std::endl << std::endl;
}
