#include <iostream>

#include "TFile.h"
#include "TTree.h"
#include "TVirtualStreamerInfo.h"
#include "TStreamerElement.h"
#include "TClonesArray.h"
#include "TTreeGeneratorBase.h"
#include "TFormLeafInfo.h"

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
      out += "    //" + comment;
    return out;
  }
};

class ClassStructure {
public:
  std::string name;
  TClass *tclass;
  int version;
  std::vector<MemberStructure> bases;
  std::vector<MemberStructure> members;
  ClassStructure(TClass *tclass, int version) : name(tclass->GetName()), tclass(tclass), version(version) { }

  std::string cpp(int indent) {
    std::string out;
    std::string ind;
    for (int i = 0;  i < indent;  i++)
      ind += " ";

    if (!members.empty()) {
      out += ind + "class ";
      out += name;
      if (!bases.empty()) {
        out += " : public";
        for (int i = 0;  i < bases.size();  i++)
          if (i == 0)
            out += " " + bases[i].type;
          else
            out += ", " + bases[i].type;
      }
      out += " {\n" + ind + "  public:\n";

      for (int i = 0;  i < members.size();  i++)
        out += ind + members[i].cpp(indent + 4) + "\n";

      out += ind + "    " + name + "() {\n";
      for (int i = 0;  i < members.size();  i++)
        if (members[i].pointer  ||  members[i].type == std::string("Char_t*"))
          out += ind + "      " + members[i].variable + " = nullptr;\n";
      out += ind + "    }\n";

      out += ind + "    virtual ~" + name + "() {\n";
      for (int i = 0;  i < members.size();  i++)
        if (members[i].pointer  ||  members[i].type == std::string("Char_t*"))
          out += ind + "      " + members[i].variable + " = nullptr;\n";
      out += ind + "    }\n";

      out += ind + "    ClassDef(" + name + "," + std::to_string(version) + ")\n" + ind + "};\n";
    }
    return out;
  }
};

void classesFromBranch(TBranch *tbranch, TClass *tclass, std::vector<ClassStructure> &classes, int prefix, std::set<std::string> &includes);

int main(int argc, char **argv) {
  TTree *ttree;

  TFile *tfile = TFile::Open("test_Event/Event.root");
  tfile->GetObject("T", ttree);

  // TFile *tfile = TFile::Open("Event-0.root");
  // tfile->GetObject("t4", ttree);

  std::set<std::string> includes;
  std::vector<ClassStructure> classes;

  TIter listOfBranches = ttree->GetListOfBranches();
  for (TBranch *tbranch = (TBranch*)listOfBranches.Next();  tbranch != nullptr;  tbranch = (TBranch*)listOfBranches.Next()) {
    TClass *tclass = TClass::GetClass(tbranch->GetClassName());
    if (tclass != nullptr  &&  tbranch->GetListOfBranches()->GetEntries() > 0)
      classesFromBranch(tbranch, tclass, classes, 0, includes);
  }

  for (std::set<std::string>::iterator iter = includes.begin();  iter != includes.end();  ++iter)
    std::cout << *iter << std::endl;

  std::cout << std::endl;

  for (std::vector<ClassStructure>::iterator iter = classes.begin();  iter != classes.end();  ++iter)
    std::cout << "class " << iter->name << ";" << std::endl;

  std::cout << std::endl;

  for (std::vector<ClassStructure>::iterator iter = classes.begin();  iter != classes.end();  ++iter)
    std::cout << iter->cpp(0) << std::endl;

  for (std::vector<ClassStructure>::iterator iter = classes.begin();  iter != classes.end();  ++iter)
    std::cout << "ClassImp(" << iter->name << ")" << std::endl;

  return 0;
}

void classesFromBranch(TBranch *tbranch, TClass *tclass, std::vector<ClassStructure> &classes, int prefix, std::set<std::string> &includes) {
  std::string className = tclass->GetName();

  if (className == std::string("TObject"))
    includes.insert("#include \"TObject.h\"");
  else if (className == std::string("TRef"))
    includes.insert("#include \"TRef.h\"");
  else if (className == std::string("TRefArray"))
    includes.insert("#include \"TRefArray.h\"");
  else if (className == std::string("TH1"))
    includes.insert("#include \"TH1.h\"");
  else if (className == std::string("TBits")) {
    includes.insert("#include \"TBits.h\"");
  }
  else {
    bool seen = false;
    for (int i = 0;  i < classes.size();  i++)
      if (classes[i].name == className)
        seen = true;

    if (!seen) {
      TVirtualStreamerInfo *tVirtualStreamerInfo = tclass->GetStreamerInfo();
      int version = tVirtualStreamerInfo->GetClassVersion();

      ClassStructure classStructure(tclass, version);

      TIter listOfBranches = tbranch->GetListOfBranches();
      TIter elements = tVirtualStreamerInfo->GetElements();

      TStreamerElement *tStreamerElement;
      TBranch *subbranch;
      while (true) {
        tStreamerElement = (TStreamerElement*)elements.Next();
        subbranch = (TBranch*)listOfBranches.Next();
        if (tStreamerElement == nullptr  ||  subbranch == nullptr) break;

        std::string streamerName = tStreamerElement->GetName();
        std::string branchName = subbranch->GetName();
        if (branchName.find(streamerName) == std::string::npos)
          subbranch = (TBranch*)listOfBranches.Next();
        if (subbranch == nullptr) break;

        std::string variable = tStreamerElement->GetName();
        std::string variableWithArray = subbranch->GetName();
        variableWithArray = variableWithArray.substr(prefix, std::string::npos);
        variableWithArray = variableWithArray.substr(0, variableWithArray.find('.'));

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
            TString className = ttreeGenerator.GetContainedClassName((TBranchElement*)subbranch, tStreamerElement, pointer);
            if (className != nullptr) {
              TClass *otherClass = TClass::GetClass(className);
              classesFromBranch(subbranch, otherClass, classes, variableWithArray.size() + 1, includes);
              includes.insert("#include \"TClonesArray.h\"");
            }
          }
          else if (elementClass->GetCollectionProxy() != nullptr  &&  elementClass->GetCollectionProxy()->GetValueClass() != nullptr) {
            TClass *otherClass = elementClass->GetCollectionProxy()->GetValueClass();
            classesFromBranch(subbranch, otherClass, classes, variableWithArray.size() + 1, includes);
            includes.insert(std::string("#include <") + type + std::string(">"));
          }
          else if (tStreamerElement->IsBase())
            includes.insert(std::string("#include \"") + type + std::string(".h\""));
          else
            classesFromBranch(subbranch, elementClass, classes, variableWithArray.size() + 1, includes);
        }

        if (tStreamerElement->IsBase())
          classStructure.bases.push_back(MemberStructure(type, pointer, variable, variableWithArray, comment));
        else
          classStructure.members.push_back(MemberStructure(type, pointer, variable, variableWithArray, comment));
      }

      if (!classStructure.members.empty())
        classes.push_back(classStructure);
    }
  }
}
