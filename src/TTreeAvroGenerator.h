#ifndef ROOT_TTreeAvroGenerator
#define ROOT_TTreeAvroGenerator

#include "scaffold.h"
#include "TTreeGeneratorBase.h"

class TBranch;
class TBranchElement;
class TLeaf;
class TStreamerElement;

namespace ROOT {
  namespace Internal {
    class TFriendProxyDescriptor;
    class TBranchProxyDescriptor;
    class TBranchProxyClassDescriptor;

    class TTreeAvroGenerator : public TTreeGeneratorBase
    {
    public:
      enum EContainer { kNone, kClones, kSTL };
      TTreeAvroGenerator(TTree* tree);

      int scaffoldSize;
      scaffold::Node **scaffold;
      std::map<const std::string, scaffold::Def*> defs;
      std::string definitions() { return scaffold::definitions(defs); }
      std::string declarations() { return scaffold::declarations(scaffold, scaffoldSize); }
      std::string init() { return scaffold::init(scaffold, scaffoldSize); }
      std::string printJSON() { return scaffold::printJSON(scaffold, scaffoldSize); }
      std::string schema(int indent, std::string name, std::string ns) { return scaffold::schema(indent, name, ns, scaffold, scaffoldSize); }

      Bool_t NeedToEmulate(TClass *cl, UInt_t level);

      UInt_t AnalyzeBranches(UInt_t level, TBranchProxyClassDescriptor *topdesc, TBranchElement *branch, TVirtualStreamerInfo *info, scaffold::Node **scaffoldArray, int scaffoldItem, scaffold::Def *defClass);
      UInt_t AnalyzeBranches(UInt_t level, TBranchProxyClassDescriptor *topdesc, TIter &branches, TVirtualStreamerInfo *info, scaffold::Node **scaffoldArray, int scaffoldItem, scaffold::Def *defClass);
      UInt_t AnalyzeOldBranch(TBranch *branch, UInt_t level, TBranchProxyClassDescriptor *desc, scaffold::Node **scaffoldArray, int scaffoldItem);
      UInt_t AnalyzeOldLeaf(TLeaf *leaf, UInt_t level, scaffold::Node **scaffoldArray, int scaffoldItem);
      void   AnalyzeTree(TTree *tree);
    };
  }
}

#endif
