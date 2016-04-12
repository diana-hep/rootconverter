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

#ifndef STREAMER_TO_CODE_H
#define STREAMER_TO_CODE_H

#include <string>

#include "TClonesArray.h"
#include "TFile.h"
#include "TFormLeafInfo.h"
#include "TInterpreter.h"
#include "TStreamerElement.h"
#include "TTreeGeneratorBase.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TVirtualStreamerInfo.h"

class MemberStructure {
public:
  std::string type;
  bool pointer;
  std::string variable;
  std::string variableWithArray;
  std::string comment;
  MemberStructure(std::string type, bool pointer, std::string variable, std::string variableWithArray, std::string comment);
  std::string cpp(int indent);
};

class ClassStructure {
public:
  std::string fullName;
  std::vector<std::string> splitName;
  TClass *tclass;
  int version;
  std::vector<std::string> bases;
  std::vector<MemberStructure> members;
  ClassStructure(TClass *tclass, int version);
  std::vector<std::string> splitCppNamespace(std::string className);
  std::string cpp(int indent);
};

void declareClasses(std::string code);
std::string generateCodeFromStreamers(std::string url, std::string treeLocation);
void classesFromBranch(TBranch *tbranch, TClass *tclass, std::vector<ClassStructure> &classes, int prefix, std::set<std::string> &includes);

#endif // STREAMER_TO_CODE_H
