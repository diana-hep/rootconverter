#ifndef ROOT_TTreeAvroGenerator
#define ROOT_TTreeAvroGenerator

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

      Bool_t NeedToEmulate(TClass *cl, UInt_t level);

      UInt_t AnalyzeBranches(UInt_t level, TBranchProxyClassDescriptor *topdesc, TBranchElement *branch, TVirtualStreamerInfo *info = 0);
      UInt_t AnalyzeBranches(UInt_t level, TBranchProxyClassDescriptor *topdesc, TIter &branches, TVirtualStreamerInfo *info);
      UInt_t AnalyzeOldBranch(TBranch *branch, UInt_t level, TBranchProxyClassDescriptor *desc);
      UInt_t AnalyzeOldLeaf(TLeaf *leaf, UInt_t level);
      void   AnalyzeTree(TTree *tree);
    };
  }
}

#endif
