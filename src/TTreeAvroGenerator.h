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
      std::string header() { return scaffold::header(scaffold, scaffoldSize); }
      std::string init() { return scaffold::init(scaffold, scaffoldSize); }
      std::string loop() { return scaffold::loop(scaffold, scaffoldSize); }

      Bool_t NeedToEmulate(TClass *cl, UInt_t level);

      UInt_t AnalyzeBranches(UInt_t level, TBranchProxyClassDescriptor *topdesc, TBranchElement *branch, TVirtualStreamerInfo *info, scaffold::Node **scaffoldArray, int scaffoldItem);
      UInt_t AnalyzeBranches(UInt_t level, TBranchProxyClassDescriptor *topdesc, TIter &branches, TVirtualStreamerInfo *info, scaffold::Node **scaffoldArray, int scaffoldItem);
      UInt_t AnalyzeOldBranch(TBranch *branch, UInt_t level, TBranchProxyClassDescriptor *desc, scaffold::Node **scaffoldArray, int scaffoldItem);
      UInt_t AnalyzeOldLeaf(TLeaf *leaf, UInt_t level, scaffold::Node **scaffoldArray, int scaffoldItem);
      void   AnalyzeTree(TTree *tree);
    };
  }
}

#endif
