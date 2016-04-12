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

#include "streamerToCode.h"

MemberStructure::MemberStructure(std::string type, bool pointer, std::string variable, std::string variableWithArray, std::string comment) :
  type(type), pointer(pointer), variable(variable), variableWithArray(variableWithArray), comment(comment) { }

std::string MemberStructure::cpp(int indent) {
  std::string out = std::string(indent, ' ');
  out += type + " ";
  if (pointer)
    out += "*";
  out += variableWithArray + ";";
  if (!comment.empty())
    out += "    //" + comment;
  return out;
}

ClassStructure::ClassStructure(TClass *tclass, int version) : fullName(tclass->GetName()), splitName(splitCppNamespace(tclass->GetName())), tclass(tclass), version(version) { }

std::vector<std::string> ClassStructure::splitCppNamespace(std::string className) {
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

std::string ClassStructure::cpp(int indent) {
  std::string out;
  std::string ind = std::string(indent, ' ');

  if (!members.empty()) {
    out += ind + "class ";
    out += splitName.back();
    if (!bases.empty()) {
      out += " : public";
      for (int i = 0;  i < bases.size();  i++)
        if (i == 0)
          out += " " + bases[i];
        else
          out += ", " + bases[i];
    }
    out += " {\n" + ind + "  public:\n";

    for (int i = 0;  i < members.size();  i++)
      out += ind + members[i].cpp(indent + 2) + "\n";

    out += ind + "    " + splitName.back() + "() {\n";
    for (int i = 0;  i < members.size();  i++)
      if (members[i].pointer  ||  members[i].type == std::string("Char_t*"))
        out += ind + "      " + members[i].variable + " = nullptr;\n";
    out += ind + "    }\n";

    out += ind + "    virtual ~" + splitName.back() + "() {\n";
    for (int i = 0;  i < members.size();  i++)
      if (members[i].pointer  ||  members[i].type == std::string("Char_t*"))
        out += ind + "      " + members[i].variable + " = nullptr;\n";
    out += ind + "    }\n";

    out += ind + "    ClassDef(" + splitName.back() + "," + std::to_string(version) + ")\n" + ind + "};";
  }
  return out;
}

void declareClasses(std::string code, std::vector<std::string> &classNames) {
  for (int i = 0;  i < classNames.size();  i++)
    gROOT->RemoveClass(TClass::GetClass(classNames[i].c_str()));

  gInterpreter->Declare(code.c_str());
}

std::string generateCodeFromStreamers(std::string url, std::string treeLocation, std::vector<std::string> &classNames, std::string &errorMessage) {
  TFile *tfile = TFile::Open(url.c_str());
  if (tfile == nullptr  ||  !tfile->IsOpen()) {
    errorMessage = std::string("File not found: ") + url;
    return std::string();
  }

  if (tfile->IsZombie()) {
    errorMessage = std::string("Not a ROOT file: ") + url;
    return std::string();
  }

  TTreeReader reader(treeLocation.c_str(), tfile);
  if (reader.IsZombie()) {
    errorMessage = std::string("Not a TTree: ") + treeLocation.c_str() + std::string(" in file: ") + url;
    return std::string();
  }

  TTree *ttree = reader.GetTree();

  std::set<std::string> includes;
  std::vector<ClassStructure> classes;

  TIter listOfBranches = ttree->GetListOfBranches();
  for (TBranch *tbranch = (TBranch*)listOfBranches.Next();  tbranch != nullptr;  tbranch = (TBranch*)listOfBranches.Next()) {
    TClass *tclass = TClass::GetClass(tbranch->GetClassName());
    if (tclass != nullptr  &&  tbranch->GetListOfBranches()->GetEntries() > 0)
      classesFromBranch(tbranch, tclass, classes, 0, includes);
  }

  for (int i = 0;  i < classes.size();  i++)
    classNames.push_back(classes[i].fullName);

  tfile->Close();

  std::string out;

  for (std::set<std::string>::iterator iter = includes.begin();  iter != includes.end();  ++iter)
    out += *iter + "\n";
  out += "\n";
  
  for (std::vector<ClassStructure>::iterator iter = classes.begin();  iter != classes.end();  ++iter) {
    int i = 0;
    for (;  i < iter->splitName.size() - 1;  i++)
      out += std::string(i * 2, ' ') + "namespace " + iter->splitName[i] + " {\n";

    out += std::string(i * 2, ' ') + "class " + iter->splitName.back() + ";\n";
    i--;
    for (;  i >= 0;  i--)
      out += std::string(i * 2, ' ') + "}\n";
  }
  out += "\n";

  for (std::vector<ClassStructure>::iterator iter = classes.begin();  iter != classes.end();  ++iter) {
    int i = 0;
    for (;  i < iter->splitName.size() - 1;  i++)
      out += std::string(i * 2, ' ') + "namespace " + iter->splitName[i] + " {\n";
    out += iter->cpp(i * 2) + "\n";
    i--;
    for (;  i >= 0;  i--)
      out += std::string(i * 2, ' ') + "}\n";
  }

  for (std::vector<ClassStructure>::iterator iter = classes.begin();  iter != classes.end();  ++iter) {
    int i = 0;
    for (;  i < iter->splitName.size() - 1;  i++)
      out += std::string(i * 2, ' ') + "namespace " + iter->splitName[i] + " {\n";
    out += std::string(i * 2, ' ') + "ClassImp(" + iter->splitName.back() + ")\n";
    i--;
    for (;  i >= 0;  i--)
      out += std::string(i * 2, ' ') + "}\n";
  }
  
  return out;
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
    bool classSeen = false;
    for (int i = 0;  i < classes.size();  i++)
      if (classes[i].fullName == className)
        classSeen = true;

    if (!classSeen) {
      TVirtualStreamerInfo *tVirtualStreamerInfo = tclass->GetStreamerInfo();
      int version = tVirtualStreamerInfo->GetClassVersion();

      ClassStructure classStructure(tclass, version);

      if (tVirtualStreamerInfo->GetElements()->GetEntries() == 1  &&
          ((TStreamerElement*)(tVirtualStreamerInfo->GetElements()->First()))->IsBase()  &&
          std::string(tVirtualStreamerInfo->GetElements()->First()->GetName()) == std::string("TObjArray")) {

        TVirtualStreamerInfo *substreamer = ((TBranchElement*)tbranch)->GetInfo();

        if (std::string(substreamer->GetName()) == std::string("TClonesArray")) {
          ROOT::Internal::TTreeGeneratorBase ttreeGenerator(tbranch->GetTree(), "");
          TString className = ttreeGenerator.GetContainedClassName((TBranchElement*)tbranch, (TStreamerElement*)(substreamer->GetElements()->First()), true);
          classesFromBranch(tbranch, TClass::GetClass(className), classes, prefix + std::string(tbranch->GetName()).size() + 1, includes);
        }
      }

      TIter elements = tVirtualStreamerInfo->GetElements();
      for (TStreamerElement *tStreamerElement = (TStreamerElement*)elements.Next();  tStreamerElement != nullptr;  tStreamerElement = (TStreamerElement*)elements.Next()) {
        std::string streamerName = tStreamerElement->GetName();

        if (tStreamerElement->IsBase()) {
          TClass *elementClass = tStreamerElement->GetClassPointer();
          std::string type = elementClass->GetName();
          includes.insert(std::string("#include \"") + type + std::string(".h\""));
          classStructure.bases.push_back(type);
        }

        else {
          TIter listOfBranches = tbranch->GetListOfBranches();
          TBranch *subbranch;
          for (subbranch = (TBranch*)listOfBranches.Next();  subbranch != nullptr;  subbranch = (TBranch*)listOfBranches.Next()) {
            std::string branchName = subbranch->GetName();
            branchName = branchName.substr(prefix, std::string::npos);
            int firstDot = branchName.find('.');
            int firstBracket = branchName.find('[');
            if (firstDot != std::string::npos  &&  firstBracket != std::string::npos)
              branchName = branchName.substr(0, firstDot < firstBracket ? firstDot : firstBracket);
            else if (firstDot != std::string::npos)
              branchName = branchName.substr(0, firstDot);
            else if (firstBracket != std::string::npos)
              branchName = branchName.substr(0, firstBracket);
            if (branchName == streamerName)
              break;
          }
          if (subbranch == nullptr)
            continue;

          std::string branchName = subbranch->GetName();

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
            else
              classesFromBranch(subbranch, elementClass, classes, variableWithArray.size() + 1, includes);
          }
          classStructure.members.push_back(MemberStructure(type, pointer, variable, variableWithArray, comment));
        }
      }

      if (!classStructure.members.empty())
        classes.push_back(classStructure);
    }
  }
}
