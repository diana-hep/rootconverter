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
      enum EOption { kNoOption, kNoHist };
      UInt_t   fMaxDatamemberType;
      TString  fScript;
      TString  fCutScript;
      TString  fPrefix;
      TString  fHeaderFileName;
      UInt_t   fOptions;
      TList    fListOfClasses;
      TList    fListOfFriends;
      TList    fListOfPragmas;
      TList    fListOfTopProxies;
      TList   *fCurrentListOfTopProxies; //!
      TList    fListOfForwards;
      TTreeAvroGenerator(TTree* tree, const char *option);

      TBranchProxyClassDescriptor* AddClass(TBranchProxyClassDescriptor *desc);
      void AddDescriptor(TBranchProxyDescriptor *desc);
      void AddForward(TClass *cl);
      void AddForward(const char *classname);
      void AddFriend(TFriendProxyDescriptor *desc);
      void AddMissingClassAsEnum(const char *clname, Bool_t isscope);
      void AddPragma(const char *pragma_text);
      void CheckForMissingClass(const char *clname);

      Bool_t NeedToEmulate(TClass *cl, UInt_t level);

      void   ParseOptions();

      UInt_t AnalyzeBranches(UInt_t level, TBranchProxyClassDescriptor *topdesc, TBranchElement *branch, TVirtualStreamerInfo *info = 0);
      UInt_t AnalyzeBranches(UInt_t level, TBranchProxyClassDescriptor *topdesc, TIter &branches, TVirtualStreamerInfo *info);
      UInt_t AnalyzeOldBranch(TBranch *branch, UInt_t level, TBranchProxyClassDescriptor *desc);
      UInt_t AnalyzeOldLeaf(TLeaf *leaf, UInt_t level, TBranchProxyClassDescriptor *topdesc);
      void   AnalyzeTree(TTree *tree);

      const char *GetFileName() { return fHeaderFileName; }
    };
  }
}

#endif
