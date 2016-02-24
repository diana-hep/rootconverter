#include <iostream>

#include "TTreeAvroGenerator.h"

#include "TFriendProxyDescriptor.h"
#include "TBranchProxyDescriptor.h"
#include "TBranchProxyClassDescriptor.h"

#include "TList.h"
#include "Varargs.h"
#include <stdio.h>

class TTree;
class TBranch;
class TStreamerElement;

#include "TClass.h"
#include "TClassEdit.h"
#include "TClonesArray.h"
#include "TError.h"
#include "TROOT.h"
#include "TObjString.h"

#include "TTreeFormula.h"
#include "TFormLeafInfo.h"


#include "TBranchElement.h"
#include "TChain.h"
#include "TFile.h"
#include "TFriendElement.h"
#include "TLeaf.h"
#include "TLeafC.h"
#include "TTree.h"
#include "TVirtualStreamerInfo.h"
#include "TStreamerElement.h"
#include "TSystem.h"
#include "TLeafObject.h"
#include "TVirtualCollectionProxy.h"

void Debug(Int_t level, const char *va_(fmt), ...)
{
   // Use this function in case an error occured.

   if (gDebug>=level) {
      va_list ap;
      va_start(ap,va_(fmt));
      ErrorHandler(kInfo,"TTreeAvroGenerator",va_(fmt), ap);
      va_end(ap);
   }
}

namespace {

   Bool_t AreDifferent(const TString& from, const TString& to)
   {
      FILE *left = fopen(from.Data(),"r");
      FILE *right = fopen(to.Data(),"r");

      char leftbuffer[256];
      char rightbuffer[256];

      char *lvalue,*rvalue;

      Bool_t areEqual = kTRUE;

      do {
         lvalue = fgets(leftbuffer, sizeof(leftbuffer), left);
         rvalue = fgets(rightbuffer, sizeof(rightbuffer), right);

         if (lvalue&&rvalue) {
            if (strstr(lvalue,"by ROOT version")) {
               // skip the comment line with the time and date
            } else {
               areEqual = areEqual && (0 == strncmp(lvalue,rvalue,sizeof(leftbuffer)));
            }
         }
         if (lvalue&&!rvalue) areEqual = kFALSE;
         if (rvalue&&!lvalue) areEqual = kFALSE;

      } while(areEqual && lvalue && rvalue);

      fclose(left);
      fclose(right);

      return !areEqual;
   }
}

namespace ROOT {
namespace Internal {

   TString GetArrayType(TStreamerElement *element, const char *subtype,
                        TTreeAvroGenerator::EContainer container)
   {
      TString result;
      int ndim = 0;
      if (element->InheritsFrom(TStreamerBasicPointer::Class())) {
         TStreamerBasicPointer * elem = (TStreamerBasicPointer*)element;
         const char *countname = elem->GetCountName();
         if (countname && strlen(countname)>0) ndim = 1;
      }
      ndim += element->GetArrayDim();

      TString middle;
      if (container == TTreeAvroGenerator::kClones) {
         middle = "Cla";
      } else if  (container == TTreeAvroGenerator::kSTL) {
         middle = "Stl";
      }

      if (ndim==0) {
         result = "T";
         result += middle;
         result += subtype;
         result += "Proxy";
      } else if (ndim==1) {
         result = "T";
         result += middle;
         result += "Array";
         result += subtype;
         result += "Proxy";
      } else {
         result = "T";
         result += middle;
         result += "ArrayProxy<";
         for(Int_t ind = ndim - 2; ind > 0; --ind) {
            result += "TMultiArrayType<";
         }
         result += "TArrayType<";
         result += element->GetTypeName();
         result += ",";
         result += element->GetMaxIndex(ndim-1);
         result += "> ";
         for(Int_t ind = ndim - 2; ind > 0; --ind) {
            result += ",";
            result += element->GetMaxIndex(ind);
            result += "> ";
         }
         result += ">";
      }
      return result;

      /*
        if (!strcmp("unsigned int", name))
        sprintf(line, "%u", *(unsigned int *)buf);
        else if (!strcmp("int", name))
        sprintf(line, "%d", *(int *)buf);
        else if (!strcmp("unsigned long", name))
        sprintf(line, "%lu", *(unsigned long *)buf);
        else if (!strcmp("long", name))
        sprintf(line, "%ld", *(long *)buf);
        else if (!strcmp("unsigned short", name))
        sprintf(line, "%hu", *(unsigned short *)buf);
        else if (!strcmp("short", name))
        sprintf(line, "%hd", *(short *)buf);
        else if (!strcmp("unsigned char", name))
        sprintf(line, "%u", *(unsigned char *)buf);
        else if (!strcmp("bool", name))
        sprintf(line, "%u", *(unsigned char *)buf);
        else if (!strcmp("char", name))
        sprintf(line, "%d", *(char *)buf);
        else if (!strcmp("float", name))
        sprintf(line, "%g", *(float *)buf);
        else if (!strcmp("double", name))
        sprintf(line, "%g", *(double *)buf);
      */
   }

  TTreeAvroGenerator::TTreeAvroGenerator(TTree* tree, const char *option) :
      TTreeGeneratorBase(tree, option),
      fMaxDatamemberType(2),
      fScript(),
      fCutScript(),
      fPrefix(),
      fHeaderFileName(),
      fOptions(0),
      fCurrentListOfTopProxies(&fListOfTopProxies)
   {
      ParseOptions();
      AnalyzeTree(fTree);
   }

  void TTreeAvroGenerator::AddHeader(TClass *cl) {
    TTreeGeneratorBase::AddHeader(cl);
    std::cout << "HEADER " << cl->GetName() << std::endl;
  }

   Bool_t TTreeAvroGenerator::NeedToEmulate(TClass *cl, UInt_t /* level */)
   {
      // Return true if we should create a nested class representing this class

      return cl!=0 && cl->TestBit(TClass::kIsEmulation);
   }

   TBranchProxyClassDescriptor*
   TTreeAvroGenerator::AddClass( TBranchProxyClassDescriptor* desc )
   {
      // Add a Class Descriptor.

      if (desc==0) return 0;

      TBranchProxyClassDescriptor *existing =
         (TBranchProxyClassDescriptor*)fListOfClasses(desc->GetName());

      int count = 0;
      while (existing) {
         if (! existing->IsEquivalent( desc )  ) {
            TString newname = desc->GetRawSymbol();
            count++;
            newname += "_";
            newname += count;

            desc->SetName(newname);
            existing = (TBranchProxyClassDescriptor*)fListOfClasses(desc->GetName());
         } else {
            // we already have the exact same class
            delete desc;
            return existing;
         }
      }
      fListOfClasses.Add(desc);
      return desc;
   }

   void TTreeAvroGenerator::AddFriend( TFriendProxyDescriptor* desc )
   {
      // Add Friend descriptor.

      if (desc==0) return;

      TFriendProxyDescriptor *existing =
         (TFriendProxyDescriptor*)fListOfFriends(desc->GetName());

      int count = 0;
      while (existing) {
         if (! existing->IsEquivalent( desc )  ) {
            TString newname = desc->GetName();
            count++;
            newname += "_";
            newname += count;

            desc->SetName(newname);
            existing = (TFriendProxyDescriptor*)fListOfFriends(desc->GetName());

         } else {

            desc->SetDuplicate();
            break;
         }
      }

      // Insure uniqueness of the title also.
      TString basetitle = desc->GetTitle();
      TIter next( &fListOfFriends );
      while ( (existing = (TFriendProxyDescriptor*)next()) ) {
         if (strcmp(existing->GetTitle(),desc->GetTitle())==0) {

            TString newtitle = basetitle;
            count++;
            newtitle += "_";
            newtitle += count;

            desc->SetTitle(newtitle);

            // Restart of the begining of the loop.
            next = &fListOfFriends;
         }
      }

      fListOfFriends.Add(desc);
   }

   void TTreeAvroGenerator::AddForward( const char *classname )
   {
      // Add a forward declaration request.

      TObject *obj = fListOfForwards.FindObject(classname);
      if (obj) return;

      if (strstr(classname,"<")!=0) {
         // this is a template instantiation.
         // let's ignore it for now

         if (gDebug>=6) Warning("AddForward","Forward declaration of templated class not implemented yet.");
      } else if (strcmp(classname,"string")==0) {
         // no need to forward declare string
      } else {
         fListOfForwards.Add(new TNamed(classname,Form("class %s;\n",classname)));
      }
      return;
   }

   void TTreeAvroGenerator::AddForward(TClass *cl)
   {
      // Add a forward declaration request.

      if (cl) AddForward(cl->GetName());
   }

   void TTreeAvroGenerator::AddPragma(const char *pragma_text)
   {
      // Add a forward declaration request.

      TIter i( &fListOfPragmas );
      for(TObjString *n = (TObjString*) i(); n; n = (TObjString*)i() ) {
         if (pragma_text == n->GetString()) {
            return;
         }
      }

      fListOfPragmas.Add( new TObjString( pragma_text ) );

   }

   void TTreeAvroGenerator::AddDescriptor(TBranchProxyDescriptor *desc)
   {
      // Add a branch descriptor.

      if (desc) {
         TBranchProxyDescriptor *existing =
            (TBranchProxyDescriptor*)((*fCurrentListOfTopProxies)(desc->GetName()));
         if (existing) {
            Warning("TTreeAvroGenerator","The branch name \"%s\" is duplicated. Only the first instance \n"
               "\twill be available directly. The other instance(s) might be available via their complete name\n"
               "\t(including the name of their mother branche's name).",desc->GetName());
         } else {
            fCurrentListOfTopProxies->Add(desc);
            UInt_t len = strlen(desc->GetTypeName());
            if ((len+2)>fMaxDatamemberType) fMaxDatamemberType = len+2;
         }
      }
   }

   void TTreeAvroGenerator::AddMissingClassAsEnum(const char *clname, Bool_t isscope)
   {
      // Generate an enum for a given type if it is not known in the list of class
      // unless the type itself a template.

      if (!TClassEdit::IsStdClass(clname) && !TClass::GetClass(clname) && gROOT->GetType(clname) == 0) {

         TObject *obj = fListOfForwards.FindObject(clname);
         if (obj) return;

         // The class does not exist, let's create it if ew can.
         if (clname[strlen(clname)-1]=='>') {
            // Template instantiation.
            fListOfForwards.Add(new TNamed(clname,TString::Format("template <> class %s { public: operator int() { return 0; } };\n", clname).Data()));
         } else if (isscope) {
            // a scope

         } else {
            // Class or enum we know nothing about, let's assume it is an enum.
            fListOfForwards.Add(new TNamed(clname,TString::Format("enum %s { kDefault_%s };\n", clname, clname).Data()));
         }
      }
   }

   void TTreeAvroGenerator::CheckForMissingClass(const char *clname)
   {
      // Check if the template parameter refers to an enum and/or a missing class (we can't tell those 2 apart unless
      // the name as template syntax).

      UInt_t len = strlen(clname);
      UInt_t nest = 0;
      UInt_t last = 0;
      //Bool_t istemplate = kFALSE; // mark whether the current right most entity is a class template.

      for (UInt_t i = 0; i < len; ++i) {
         switch (clname[i]) {
            case ':':
               if (nest == 0 && clname[i+1] == ':') {
                  TString incName(clname, i);
                  AddMissingClassAsEnum(incName.Data(), kTRUE);
                  //istemplate = kFALSE;
               }
               break;
            case '<':
               ++nest;
               if (nest == 1) last = i + 1;
               break;
            case '>':
               if (nest == 0) return; // The name is not well formed, give up.
               --nest; /* intentional fall throught to the next case */
            case ',':
               if ((clname[i] == ',' && nest == 1) || (clname[i] == '>' && nest == 0)) {
                  TString incName(clname + last, i - last);
                  incName = TClassEdit::ShortType(incName.Data(), TClassEdit::kDropTrailStar | TClassEdit::kLong64);
                  if (clname[i] == '>' && nest == 1) incName.Append(">");

                  if (isdigit(incName[0])) {
                     // Not a class name, nothing to do.
                  } else {
                     AddMissingClassAsEnum(incName.Data(),kFALSE);
                  }
                  last = i + 1;
               }
         }
      }
      AddMissingClassAsEnum(TClassEdit::ShortType(clname, TClassEdit::kDropTrailStar | TClassEdit::kLong64).c_str(),kFALSE);
   }

   UInt_t TTreeAvroGenerator::AnalyzeBranches(UInt_t level,TBranchProxyClassDescriptor *topdesc,
                                               TBranchElement *branch, TVirtualStreamerInfo *info)
   {
      // Analyze the sub-branch and populate the TTreeAvroGenerator or the topdesc with
      // its findings.

      if (info==0) info = branch->GetInfo();

      TIter branches( branch->GetListOfBranches() );

      return AnalyzeBranches( level, topdesc, branches, info );
   }

   UInt_t TTreeAvroGenerator::AnalyzeBranches(UInt_t level,
                                               TBranchProxyClassDescriptor *topdesc,
                                               TIter &branches,
                                               TVirtualStreamerInfo *info)
   {
      // Analyze the list of sub branches of a TBranchElement by looping over
      // the streamer elements and create the appropriate class proxies.

/*

   Find the content class name (GetClassName)
   Record wether this is a collection or not

   Find the StreamerInfo

   For each streamerelement
      if element is base
         if name match, loop over subbranches
         otherwise loop over current branches
      else if eleement is object (or pointer to object?)
         if name match go ahead, loop over subbranches
         if name does not match. loop over current branches (fix names).
      else
         add branch.

*/
      UInt_t lookedAt = 0;
      EContainer container = kNone;
      TString middle;
      TString proxyTypeName;
      TBranchProxyClassDescriptor::ELocation outer_isclones = TBranchProxyClassDescriptor::kOut;
      TString containerName;
      TString subBranchPrefix;
      Bool_t skipped = false;

      {
         TIter peek = branches;
         TBranchElement *branch = (TBranchElement*)peek();
         if (topdesc && topdesc->IsClones()) {
            container = kClones;
            middle = "Cla";
            outer_isclones = TBranchProxyClassDescriptor::kClones;
            containerName = "TClonesArray";
         } else if (topdesc && topdesc->IsSTL()) {
            container = kSTL;
            middle = "Stl";
            outer_isclones = TBranchProxyClassDescriptor::kSTL;
            containerName = topdesc->GetContainerName();
         } else if (!topdesc && branch && branch->GetBranchCount() == branch->GetMother()) {
            if ( ((TBranchElement*)(branch->GetMother()))->GetType()==3)  {
               container = kClones;
               middle = "Cla";
               outer_isclones = TBranchProxyClassDescriptor::kClones;
               containerName = "TClonesArray";
            } else {
               container = kSTL;
               middle = "Stl";
               outer_isclones = TBranchProxyClassDescriptor::kSTL;
               containerName = branch->GetMother()->GetClassName();
            }
         } else if (branch->GetType() == 3) {
            outer_isclones = TBranchProxyClassDescriptor::kClones;
            containerName = "TClonesArray";
         } else if (branch->GetType() == 4) {
            outer_isclones = TBranchProxyClassDescriptor::kSTL;
            containerName = branch->GetMother()->GetSubBranch(branch)->GetClassName();
         }
         if (topdesc) {
            subBranchPrefix = topdesc->GetSubBranchPrefix();
         } else {
            TBranchElement *mom = (TBranchElement*)branch->GetMother();
            subBranchPrefix = mom->GetName();
            if (subBranchPrefix[subBranchPrefix.Length()-1]=='.') {
               subBranchPrefix.Remove(subBranchPrefix.Length()-1);
            } else if (mom->GetType()!=3 && mom->GetType() != 4) {
               subBranchPrefix = "";
            }
         }
      }
      TIter elements( info->GetElements() );
      for( TStreamerElement *element = (TStreamerElement*)elements();
           element;
           element = (TStreamerElement*)elements() )
      {
         Bool_t isBase = false;
         Bool_t usedBranch = kTRUE;
         TString prefix;
         TIter peek = branches;
         TBranchElement *branch = (TBranchElement*)peek();

         if (branch==0) {
            if (topdesc) {
               Error("AnalyzeBranches","Ran out of branches when looking in branch %s, class %s",
                     topdesc->GetBranchName(), info->GetName());
            } else {
               Error("AnalyzeBranches","Ran out of branches when looking in class %s, element %s",
                     info->GetName(),element->GetName());
            }
            return lookedAt;
         }

         if (info->GetClass()->GetCollectionProxy() && strcmp(element->GetName(),"This")==0) {
            // Skip the artifical streamer element.
            continue;
         }

         if (element->GetType()==-1) {
            // This is an ignored TObject base class.
            continue;
         }

         TString branchname = branch->GetName();
         TString branchEndname;
         {
            TLeaf *leaf = (TLeaf*)branch->GetListOfLeaves()->At(0);
            if (leaf && outer_isclones == TBranchProxyClassDescriptor::kOut
                && !(branch->GetType() == 3 || branch->GetType() == 4)) branchEndname = leaf->GetName();
            else branchEndname = branch->GetName();
            Int_t pos;
            pos = branchEndname.Index(".");
            if (pos!=-1) {
               if (subBranchPrefix.Length() &&
                  branchEndname.BeginsWith( subBranchPrefix ) ) {
                     // brprefix += topdesc->GetSubBranchPrefix();
                  branchEndname.Remove(0,subBranchPrefix.Length()+1);
               }
            }
         }

         Bool_t ispointer = false;
         switch(element->GetType()) {

            case TVirtualStreamerInfo::kBool:    { proxyTypeName = "T" + middle + "BoolProxy"; break; }
            case TVirtualStreamerInfo::kChar:    { proxyTypeName = "T" + middle + "CharProxy"; break; }
            case TVirtualStreamerInfo::kShort:   { proxyTypeName = "T" + middle + "ShortProxy"; break; }
            case TVirtualStreamerInfo::kInt:     { proxyTypeName = "T" + middle + "IntProxy"; break; }
            case TVirtualStreamerInfo::kLong:    { proxyTypeName = "T" + middle + "LongProxy"; break; }
            case TVirtualStreamerInfo::kLong64:  { proxyTypeName = "T" + middle + "Long64Proxy"; break; }
            case TVirtualStreamerInfo::kFloat:   { proxyTypeName = "T" + middle + "FloatProxy"; break; }
            case TVirtualStreamerInfo::kFloat16: { proxyTypeName = "T" + middle + "Float16Proxy"; break; }
            case TVirtualStreamerInfo::kDouble:  { proxyTypeName = "T" + middle + "DoubleProxy"; break; }
            case TVirtualStreamerInfo::kDouble32:{ proxyTypeName = "T" + middle + "Double32Proxy"; break; }
            case TVirtualStreamerInfo::kUChar:   { proxyTypeName = "T" + middle + "UCharProxy"; break; }
            case TVirtualStreamerInfo::kUShort:  { proxyTypeName = "T" + middle + "UShortProxy"; break; }
            case TVirtualStreamerInfo::kUInt:    { proxyTypeName = "T" + middle + "UIntProxy"; break; }
            case TVirtualStreamerInfo::kULong:   { proxyTypeName = "T" + middle + "ULongProxy"; break; }
            case TVirtualStreamerInfo::kULong64: { proxyTypeName = "T" + middle + "ULong64Proxy"; break; }
            case TVirtualStreamerInfo::kBits:    { proxyTypeName = "T" + middle + "UIntProxy"; break; }

            case TVirtualStreamerInfo::kCharStar: { proxyTypeName = GetArrayType(element,"Char",container); break; }

               // array of basic types  array[8]
            case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kBool:    { proxyTypeName = GetArrayType(element,"Bool",container ); break; }
            case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kChar:    { proxyTypeName = GetArrayType(element,"Char",container ); break; }
            case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kShort:   { proxyTypeName = GetArrayType(element,"Short",container ); break; }
            case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kInt:     { proxyTypeName = GetArrayType(element,"Int",container ); break; }
            case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kLong:    { proxyTypeName = GetArrayType(element,"Long",container ); break; }
            case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kLong64:  { proxyTypeName = GetArrayType(element,"Long64",container ); break; }
            case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kFloat:   { proxyTypeName = GetArrayType(element,"Float",container ); break; }
            case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kFloat16: { proxyTypeName = GetArrayType(element,"Float16",container ); break; }
            case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kDouble:  { proxyTypeName = GetArrayType(element,"Double",container ); break; }
            case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kDouble32:{ proxyTypeName = GetArrayType(element,"Double32",container ); break; }
            case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kUChar:   { proxyTypeName = GetArrayType(element,"UChar",container ); break; }
            case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kUShort:  { proxyTypeName = GetArrayType(element,"UShort",container ); break; }
            case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kUInt:    { proxyTypeName = GetArrayType(element,"UInt",container ); break; }
            case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kULong:   { proxyTypeName = GetArrayType(element,"ULong",container ); break; }
            case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kULong64: { proxyTypeName = GetArrayType(element,"ULong64",container ); break; }
            case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kBits:    { proxyTypeName = GetArrayType(element,"UInt",container ); break; }

               // pointer to an array of basic types  array[n]
            case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kBool:    { proxyTypeName = GetArrayType(element,"Bool",container ); break; }
            case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kChar:    { proxyTypeName = GetArrayType(element,"Char",container ); break; }
            case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kShort:   { proxyTypeName = GetArrayType(element,"Short",container ); break; }
            case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kInt:     { proxyTypeName = GetArrayType(element,"Int",container ); break; }
            case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kLong:    { proxyTypeName = GetArrayType(element,"Long",container ); break; }
            case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kLong64:  { proxyTypeName = GetArrayType(element,"Long64",container ); break; }
            case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kFloat:   { proxyTypeName = GetArrayType(element,"Float",container ); break; }
            case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kFloat16: { proxyTypeName = GetArrayType(element,"Float16",container ); break; }
            case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kDouble:  { proxyTypeName = GetArrayType(element,"Double",container ); break; }
            case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kDouble32:{ proxyTypeName = GetArrayType(element,"Double32",container ); break; }
            case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kUChar:   { proxyTypeName = GetArrayType(element,"UChar",container ); break; }
            case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kUShort:  { proxyTypeName = GetArrayType(element,"UShort",container ); break; }
            case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kUInt:    { proxyTypeName = GetArrayType(element,"UInt",container ); break; }
            case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kULong:   { proxyTypeName = GetArrayType(element,"ULong",container ); break; }
            case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kULong64: { proxyTypeName = GetArrayType(element,"ULong64",container ); break; }
            case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kBits:    { proxyTypeName = GetArrayType(element,"UInt",container ); break; }

               // array counter //[n]
            case TVirtualStreamerInfo::kCounter: { proxyTypeName = "T" + middle + "IntProxy"; break; }


            case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kObjectp:
            case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kObjectP:
            case TVirtualStreamerInfo::kObjectp:
            case TVirtualStreamerInfo::kObjectP:
            case TVirtualStreamerInfo::kAnyp:
            case TVirtualStreamerInfo::kAnyP:
            case TVirtualStreamerInfo::kSTL + TVirtualStreamerInfo::kObjectp:
            case TVirtualStreamerInfo::kSTL + TVirtualStreamerInfo::kObjectP:
            // set as pointers and fall through to the next switches
               ispointer = true;
            case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kObject:
            case TVirtualStreamerInfo::kObject:
            case TVirtualStreamerInfo::kTString:
            case TVirtualStreamerInfo::kTNamed:
            case TVirtualStreamerInfo::kTObject:
            case TVirtualStreamerInfo::kAny:
            case TVirtualStreamerInfo::kBase:
            case TVirtualStreamerInfo::kSTL: {
               TClass *cl = element->GetClassPointer();
               R__ASSERT(cl);

               proxyTypeName = Form("T%sObjProxy<%s >", middle.Data(), cl->GetName());
               TString cname = cl->GetName();
               TBranchProxyClassDescriptor::ELocation isclones = outer_isclones;
               if (cl==TClonesArray::Class()) {
                  isclones = TBranchProxyClassDescriptor::kClones;
                  cname = GetContainedClassName(branch, element, ispointer);
                  containerName = "TClonesArray";
               } else if (cl->GetCollectionProxy()) {
                  isclones = TBranchProxyClassDescriptor::kSTL;
                  containerName = cl->GetName();
                  TClass *valueClass = cl->GetCollectionProxy()->GetValueClass();
                  if (valueClass) cname = valueClass->GetName();
                  else {
                     CheckForMissingClass(cname);
                     proxyTypeName = Form("TStlSimpleProxy<%s >", cl->GetName());
//                   AddPragma(Form("#pragma create TClass %s;\n", cl->GetName()));
                     if (!cl->IsLoaded()) AddPragma(Form("#pragma link C++ class %s;\n", cl->GetName()));
                  }
               }

               TBranch *parent = branch->GetMother()->GetSubBranch(branch);
               TVirtualStreamerInfo *objInfo = 0;
               if (branch->GetListOfBranches()->GetEntries()) {
                  objInfo = ((TBranchElement*)branch->GetListOfBranches()->At(0))->GetInfo();
               } else {
                  objInfo = branch->GetInfo();
               }
               if (element->IsBase()) {
                  isBase = true;
                  prefix  = "base";

                  if (cl == TObject::Class() && info->GetClass()->CanIgnoreTObjectStreamer())
                  {
                     continue;
                  }

                  TBranchProxyClassDescriptor *cldesc = 0;

                  if (branchEndname == element->GetName()) {
                     // We have a proper node for the base class, recurse

                     if (branch->GetListOfBranches()->GetEntries() == 0) {
                        // The branch contains a non-split base class that we are unfolding!

                        // See AnalyzeTree for similar code!
                        TBranchProxyClassDescriptor *local_cldesc = 0;

                        TVirtualStreamerInfo *binfo = branch->GetInfo();
                        if (strcmp(cl->GetName(),binfo->GetName())!=0) {
                           binfo = cl->GetStreamerInfo(); // might be the wrong version
                        }
                        local_cldesc = new TBranchProxyClassDescriptor(cl->GetName(), binfo,
                                                                       branch->GetName(),
                                                                       isclones, 0 /* unsplit object */,
                                                                       containerName);

                        TStreamerElement *elem = 0;

                        TIter next(binfo->GetElements());
                        while( (elem = (TStreamerElement*)next()) ) {
                           AnalyzeElement(branch,elem,level+1,local_cldesc,"");

                        }
                        if (NeedToEmulate(cl,0)) {
                           proxyTypeName = local_cldesc->GetName();
                           local_cldesc = AddClass(local_cldesc);
                        }

                     } else {

                        Int_t pos = branchname.Last('.');
                        if (pos != -1) {
                           branchname.Remove(pos);
                        }
                        TString local_prefix = topdesc ? topdesc->GetSubBranchPrefix() : parent->GetName();
                        cldesc = new TBranchProxyClassDescriptor(cl->GetName(), objInfo,
                                                                 branchname,
                                                                 local_prefix,
                                                                 isclones, branch->GetSplitLevel(),
                                                                 containerName);
                        lookedAt += AnalyzeBranches( level+1, cldesc, branch, objInfo);
                     }
                  } else {
                     // We do not have a proper node for the base class, we need to loop over
                     // the next branches
                     Int_t pos = branchname.Last('.');
                     if (pos != -1) {
                        branchname.Remove(pos);
                     }
                     TString local_prefix = topdesc ? topdesc->GetSubBranchPrefix() : parent->GetName();
                     objInfo = GetBaseClass( element );
                     if (objInfo == 0) {
                        // There is no data in this base class
                        continue;
                     }
                     cl = objInfo->GetClass();
                     cldesc = new TBranchProxyClassDescriptor(cl->GetName(), objInfo,
                                                              branchname,
                                                              local_prefix,
                                                              isclones, branch->GetSplitLevel(),
                                                              containerName);
                     usedBranch = kFALSE;
                     lookedAt += AnalyzeBranches( level, cldesc, branches, objInfo );
                  }

                  TBranchProxyClassDescriptor *added = AddClass(cldesc);
                  if (added) proxyTypeName = added->GetName();

               } else {
                  TBranchProxyClassDescriptor *cldesc = 0;

                  if (branchEndname == element->GetName()) {

                     // We have a proper node for the base class, recurse
                     if (branch->GetListOfBranches()->GetEntries() == 0) {
                        // The branch contains a non-split object that we are unfolding!

                        // See AnalyzeTree for similar code!
                        TBranchProxyClassDescriptor *local_cldesc = 0;

                        TVirtualStreamerInfo *binfo = branch->GetInfo();
                        if (strcmp(cl->GetName(),binfo->GetName())!=0) {
                           binfo = cl->GetStreamerInfo(); // might be the wrong version
                        }
                        local_cldesc = new TBranchProxyClassDescriptor(cl->GetName(), binfo,
                                                                       branch->GetName(),
                                                                       isclones, 0 /* unsplit object */,
                                                                       containerName);

                        TStreamerElement *elem = 0;

                        TIter next(binfo->GetElements());
                        while( (elem = (TStreamerElement*)next()) ) {
                           AnalyzeElement(branch,elem,level+1,local_cldesc,"");
                        }

                        if (NeedToEmulate(cl,0)) {
                           proxyTypeName = local_cldesc->GetName();
                           local_cldesc = AddClass(local_cldesc);
                        }

                     } else {

                        if (isclones != TBranchProxyClassDescriptor::kOut) {
                           // We have to guess the version number!
                           cl = TClass::GetClass(cname);
                           objInfo = GetStreamerInfo(branch,branch->GetListOfBranches(),cl);
                        }
                        cldesc = new TBranchProxyClassDescriptor(cl->GetName(), objInfo,
                                                                 branch->GetName(),
                                                                 branch->GetName(),
                                                                 isclones, branch->GetSplitLevel(),
                                                                 containerName);
                        lookedAt += AnalyzeBranches( level+1, cldesc, branch, objInfo);
                     }
                  } else {
                     // We do not have a proper node for the base class, we need to loop over
                     // the next branches
                     TString local_prefix = topdesc ? topdesc->GetSubBranchPrefix() : parent->GetName();
                     if (local_prefix.Length()) local_prefix += ".";
                     local_prefix += element->GetName();
                     objInfo = branch->GetInfo();
                     Int_t pos = branchname.Last('.');
                     if (pos != -1) {
                        branchname.Remove(pos);
                     }
                     if (isclones != TBranchProxyClassDescriptor::kOut) {
                        // We have to guess the version number!
                        cl = TClass::GetClass(cname);
                        objInfo = GetStreamerInfo(branch, branches, cl);
                     }
                     cldesc = new TBranchProxyClassDescriptor(cl->GetName(), objInfo,
                                                              branchname,
                                                              local_prefix,
                                                              isclones, branch->GetSplitLevel(),
                                                              containerName);
                     usedBranch = kFALSE;
                     skipped = kTRUE;
                     lookedAt += AnalyzeBranches( level, cldesc, branches, objInfo );
                  }

                  TBranchProxyClassDescriptor *added = AddClass(cldesc);
                  if (added) proxyTypeName = added->GetName();

               }

               AddForward(cl);
               AddHeader(cl);
               break;
            }

            default:
               Error("AnalyzeBranch",
                     "Unsupported type for %s (%d).",
                     branch->GetName(),element->GetType());

         }

        for (UInt_t indent = 0;  indent < level;  indent++)
          std::cout << "    ";
        std::cout << element->GetName() << ": " << proxyTypeName << std::endl;

         TString dataMemberName = element->GetName();
         if (topdesc) {
            topdesc->AddDescriptor(  new TBranchProxyDescriptor( dataMemberName.Data(),
               proxyTypeName, branchname, true, skipped ), isBase );
         } else {
            dataMemberName.Prepend(prefix);
            AddDescriptor( new TBranchProxyDescriptor( dataMemberName.Data(),
               proxyTypeName, branchname, true, skipped ) );
         }

         if (usedBranch) {
            branches.Next();
            ++lookedAt;
         }
      }
      return lookedAt;
   }

   UInt_t TTreeAvroGenerator::AnalyzeOldLeaf(TLeaf *leaf, UInt_t /* level */,
                                              TBranchProxyClassDescriptor *topdesc)
   {
      // Analyze the leaf and populate the `TTreeAvroGenerator or
      // the topdesc with its findings.

      if (leaf->IsA()==TLeafObject::Class()) {
         Error("AnalyzeOldLeaf","TLeafObject not supported yet");
         return 0;
      }

      TString leafTypeName = leaf->GetTypeName();
      Int_t pos = leafTypeName.Last('_');
      if (pos!=-1) leafTypeName.Remove(pos);

      // Int_t len = leaf->GetLen();
      // TLeaf *leafcount = leaf->GetLeafCount();

      UInt_t dim = 0;
      std::vector<Int_t> maxDim;
      //maxDim[0] = maxDim[1] = maxDim[2] = 1;

      TString dimensions;
      TString temp = leaf->GetName();
      pos = temp.Index("[");
      if (pos!=-1) {
         if (pos) temp.Remove(0,pos);
         dimensions.Append(temp);
      }
      temp = leaf->GetTitle();
      pos = temp.Index("[");
      if (pos!=-1) {
         if (pos) temp.Remove(0,pos);
         dimensions.Append(temp);
      }

      Int_t dimlen = dimensions.Length();

      if (dimlen) {
         const char *current = dimensions.Data();

         Int_t index;
         Int_t scanindex ;
         while (current) {
            current++;
            if (current[0] == ']') {
               maxDim.push_back(-1); // maxDim[dim] = -1; // Loop over all elements;
            } else {
               scanindex = sscanf(current,"%d",&index);
               if (scanindex) {
                  maxDim.push_back(index); // maxDim[dim] = index;
               } else {
                  maxDim.push_back(-2); // maxDim[dim] = -2; // Index is calculated via a variable.
               }
            }
            dim ++;
            current = (char*)strstr( current, "[" );
         }

      }
      //char *twodim = (char*)strstr(leaf->GetTitle(),"][");

      //if (leafcount) {
      //   len = leafcount->GetMaximum();
      //}
      if (dim == 0 && leaf->IsA() == TLeafC::Class()) {
         // For C style strings.
         dim = 1;
      }

      TString type;
      switch (dim) {
         case 0: {
            type = "T";
            type += leafTypeName;
            type += "Proxy";
            break;
         }
         case 1: {
            type = "TArray";
            type += leafTypeName;
            type += "Proxy";
            break;
         }
         default: {
            type = "TArrayProxy<";
            for(Int_t ind = dim - 2; ind > 0; --ind) {
               type += "TMultiArrayType<";
            }
            type += "TArrayType<";
            type += leaf->GetTypeName();
            type += ",";
            type += maxDim[dim-1];
            type += "> ";
            for(Int_t ind = dim - 2; ind > 0; --ind) {
               type += ",";
               type += maxDim[ind];
               type += "> ";
            }
            type += ">";
            break;
         }
      }

      TString branchName = leaf->GetBranch()->GetName();
      TString dataMemberName = leaf->GetName();

      if (topdesc) {
         topdesc->AddDescriptor( new TBranchProxyDescriptor( dataMemberName.Data(),
                                                             type,
                                                             branchName.Data(),
                                                             true, false, true ),
                                 0 );
      } else {
         AddDescriptor( new TBranchProxyDescriptor( dataMemberName.Data(),
                                                    type,
                                                    branchName.Data(),
                                                    true, false, true ) );
      }

      return 0;

   }

   UInt_t TTreeAvroGenerator::AnalyzeOldBranch(TBranch *branch, UInt_t level,
                                                TBranchProxyClassDescriptor *topdesc)
   {
      // Analyze the branch and populate the TTreeAvroGenerator or the topdesc with
      // its findings.  Sometimes several branch of the mom are also analyzed,
      // the number of such branches is returned (this happens in the case of
      // embedded objects inside an object inside a clones array split more than
      // one level.

      UInt_t extraLookedAt = 0;
      TString prefix;

      TString branchName = branch->GetName();

      TObjArray *leaves = branch->GetListOfLeaves();
      Int_t nleaves = leaves ? leaves->GetEntriesFast() : 0;

      if (nleaves>1) {

         // Create a holder
         TString type = "unknown";
         TBranchProxyClassDescriptor *cldesc = AddClass( new TBranchProxyClassDescriptor(branch->GetName()) );
         if (cldesc) {
            type = cldesc->GetName();

            for(int l=0;l<nleaves;l++) {
               TLeaf *leaf = (TLeaf*)leaves->UncheckedAt(l);
               extraLookedAt += AnalyzeOldLeaf(leaf,level+1,cldesc);
            }
         }

         TString dataMemberName = branchName;

         if (topdesc) {
            topdesc->AddDescriptor(  new TBranchProxyDescriptor( dataMemberName.Data(),
                                                                 type,
                                                                 branchName.Data() ),
                                    0 );
         } else {
            // leafname.Prepend(prefix);
            AddDescriptor( new TBranchProxyDescriptor( dataMemberName.Data(),
                                                       type,
                                                       branchName.Data() ) );
         }

      } else {

         TLeaf *leaf = (TLeaf*)leaves->UncheckedAt(0);
         extraLookedAt += AnalyzeOldLeaf(leaf,level,topdesc);

      }


      return extraLookedAt;

   }

   void TTreeAvroGenerator::AnalyzeTree(TTree *tree)
   {
      // Analyze a TTree and its (potential) friends.

      TIter next( tree->GetListOfBranches() );
      TBranch *branch;
      while ( (branch = (TBranch*)next()) ) {
         TVirtualStreamerInfo *info = 0;
         const char *branchname = branch->GetName();
         std::cout << "BRANCH " << branchname << std::endl;

         const char *classname = branch->GetClassName();
         if (classname && strlen(classname)) {
           std::cout << "FORWARD " << classname << std::endl;

            AddForward( classname );
            // AddHeader( classname );
         }

         TBranchProxyClassDescriptor *desc = 0;
         TClass *cl = TClass::GetClass(classname);
         TString type = "unknown";
         if (cl) {
            TBranchProxyClassDescriptor::ELocation isclones = TBranchProxyClassDescriptor::kOut;
            TString containerName = "";
            if (cl==TClonesArray::Class()) {
              std::cout << "TClonesArray" << std::endl;

               isclones = TBranchProxyClassDescriptor::kClones;
               containerName = "TClonesArray";
               if (branch->IsA()==TBranchElement::Class()) {
                  const char *cname = ((TBranchElement*)branch)->GetClonesName();
                  TClass *ncl = TClass::GetClass(cname);
                  if (ncl) {
                     cl = ncl;
                     info = GetStreamerInfo(branch,branch->GetListOfBranches(),cl);
                  } else {
                     Error("AnalyzeTree",
                           "Introspection of TClonesArray in older file not implemented yet.");
                  }
               } else {
                  TClonesArray **ptr = (TClonesArray**)branch->GetAddress();
                  TClonesArray *clones = 0;
                  if (ptr==0) {
                     clones = new TClonesArray;
                     branch->SetAddress(&clones);
                     ptr = &clones;
                  }
                  branch->GetEntry(0);
                  TClass *ncl = *ptr ? (*ptr)->GetClass() : 0;
                  if (ncl) {
                     cl = ncl;
                  } else {
                     Error("AnalyzeTree",
                           "Introspection of TClonesArray for %s failed.",branch->GetName());
                  }
               }
            } else if (cl->GetCollectionProxy()) {
               isclones = TBranchProxyClassDescriptor::kSTL;
               containerName = cl->GetName();
               std::cout << "STL " << containerName << std::endl;

               if (cl->GetCollectionProxy()->GetValueClass()) {
                  cl = cl->GetCollectionProxy()->GetValueClass();
               } else {
                  CheckForMissingClass(cl->GetName());
                  type = Form("TStlSimpleProxy<%s >", cl->GetName());
                  AddHeader(cl);
                  if (!cl->IsLoaded()) AddPragma(Form("#pragma link C++ class %s;\n", cl->GetName()));
                  AddDescriptor( new TBranchProxyDescriptor( branchname, type, branchname ) );
                  continue;
               }
            }
            if (cl) {
               if (NeedToEmulate(cl,0) || branchname[strlen(branchname)-1] == '.' || branch->GetSplitLevel()) {
                  TBranchElement *be = dynamic_cast<TBranchElement*>(branch);
                  TVirtualStreamerInfo *beinfo = (be && isclones == TBranchProxyClassDescriptor::kOut)
                     ? be->GetInfo() : cl->GetStreamerInfo(); // the 2nd hand need to be fixed

                  std::cout << "TBranchProxyClassDescriptor " << cl->GetName() << " " << beinfo << " " << branchname << " " << isclones << " " << branch->GetSplitLevel() << " " << containerName << std::endl;

                  desc = new TBranchProxyClassDescriptor(cl->GetName(), beinfo, branchname,
                     isclones, branch->GetSplitLevel(),containerName);
                  info = beinfo;
               } else {
                  type = Form("TObjProxy<%s >",cl->GetName());
               }
            }
         }

         if ( branch->GetListOfBranches()->GetEntries() == 0 ) {

            if (cl) {
               // We have a non-split object!

               if (desc) {
                  TVirtualStreamerInfo *cinfo = cl->GetStreamerInfo();
                  TStreamerElement *elem = 0;

                  TIter cnext(cinfo->GetElements());
                  while( (elem = (TStreamerElement*)cnext()) ) {
                     AnalyzeElement(branch,elem,1,desc,"");
                  }

                  desc = AddClass(desc);
                  if (desc) {
                     type = desc->GetName();

                     TString dataMemberName = branchname;

                     AddDescriptor( new TBranchProxyDescriptor( dataMemberName, type, branchname ) );
                  }
               }
            } else {

               // We have a top level raw type.
               AnalyzeOldBranch(branch, 0, 0);
            }

         } else {

            // We have a split object
           std::cout << "SPLIT by " << branch->GetListOfBranches()->GetEntries() << std::endl;

            TIter subnext( branch->GetListOfBranches() );
            if (desc) {
               AnalyzeBranches(1,desc,dynamic_cast<TBranchElement*>(branch),info);
            }
            desc = AddClass(desc);
            if (desc) {
               type = desc->GetName();
               TString dataMemberName = branchname;

               std::cout << "DATA MEMBER " << dataMemberName << " " << type << " " << branchname << std::endl;

               AddDescriptor( new TBranchProxyDescriptor( dataMemberName, type, branchname ) );
            }
            if ( branchname[strlen(branchname)-1] != '.' ) {
               // If there is no dot also include the data member directly

               AnalyzeBranches(1,0,dynamic_cast<TBranchElement*>(branch),info);

               subnext.Reset();
            }

         } // if split or non split
      }

      fCurrentListOfTopProxies = &fListOfTopProxies;
   }

   void TTreeAvroGenerator::AnalyzeElement(TBranch *branch, TStreamerElement *element,
                                            UInt_t level, TBranchProxyClassDescriptor *topdesc,
                                            const char *path)
   {
      // Analyze the element and populate the TTreeAvroGenerator or the topdesc with
      // its findings.

      TString dataMemberName;
      TString pxDataMemberName;
      TString type;

      // TString prefix;
      Bool_t isBase = false;
      TString cname;
      TString middle;
      TBranchProxyClassDescriptor::ELocation isclones = TBranchProxyClassDescriptor::kOut;
      TString containerName;
      EContainer container = kNone;
      if (topdesc) {
         if (topdesc->IsClones()) {
            container = kClones;
            middle = "Cla";
            isclones = TBranchProxyClassDescriptor::kClones;
            containerName = "TClonesArray";
         } else if (topdesc->IsSTL()) {
            container = kSTL;
            middle = "Stl";
            isclones = TBranchProxyClassDescriptor::kSTL;
            containerName = topdesc->GetContainerName();
         }
      }

      if (!element) return;

      if (strcmp(element->GetName(),"This")==0) {
         TClass *cl = element->GetClassPointer();
         containerName = cl->GetName();
         cl = cl->GetCollectionProxy()->GetValueClass();
         if (!cl) {
            // Skip the artifical streamer element.
            return;
         }
         // else return;

         // In case the content is a class, move forward
         AddForward(cl);
         AddHeader(cl);

         if (true) {

            // See AnalyzeTree for similar code!
            // TBranchProxyClassDescriptor *cldesc;
            if (cl && cl->CanSplit()) {
               // cldesc = new TBranchProxyClassDescriptor(cl->GetName(), cl->GetStreamerInfo(),
               //                                          branch->GetName(),
               //                                          isclones, 0 /* non-split object */,
               //                                          containerName);

               TVirtualStreamerInfo *info = cl->GetStreamerInfo();
               TStreamerElement *elem = 0;

               TString subpath = path;
               if (subpath.Length()>0) subpath += ".";
               subpath += dataMemberName;

               TIter next(info->GetElements());
               while( (elem = (TStreamerElement*)next()) ) {
                  AnalyzeElement(branch, elem, level+1, topdesc, subpath.Data());
               }

               // TBranchProxyClassDescriptor *added = AddClass(cldesc);
               // if (added) type = added->GetName();
            }
         }
         return;
      }

      if (element->GetType()==-1) {
         // This is an ignored TObject base class.
         return;
      }


      // Bool_t ispointer = false;
      switch(element->GetType()) {

         case TVirtualStreamerInfo::kBool:    { type = "T" + middle + "BoolProxy"; break; }
         case TVirtualStreamerInfo::kChar:    { type = "T" + middle + "CharProxy"; break; }
         case TVirtualStreamerInfo::kShort:   { type = "T" + middle + "ShortProxy"; break; }
         case TVirtualStreamerInfo::kInt:     { type = "T" + middle + "IntProxy"; break; }
         case TVirtualStreamerInfo::kLong:    { type = "T" + middle + "LongProxy"; break; }
         case TVirtualStreamerInfo::kLong64:  { type = "T" + middle + "Long64Proxy"; break; }
         case TVirtualStreamerInfo::kFloat:   { type = "T" + middle + "FloatProxy"; break; }
         case TVirtualStreamerInfo::kFloat16: { type = "T" + middle + "Float16Proxy"; break; }
         case TVirtualStreamerInfo::kDouble:  { type = "T" + middle + "DoubleProxy"; break; }
         case TVirtualStreamerInfo::kDouble32:{ type = "T" + middle + "Double32Proxy"; break; }
         case TVirtualStreamerInfo::kUChar:   { type = "T" + middle + "UCharProxy"; break; }
         case TVirtualStreamerInfo::kUShort:  { type = "T" + middle + "UShortProxy"; break; }
         case TVirtualStreamerInfo::kUInt:    { type = "T" + middle + "UIntProxy"; break; }
         case TVirtualStreamerInfo::kULong:   { type = "T" + middle + "ULongProxy"; break; }
         case TVirtualStreamerInfo::kULong64: { type = "T" + middle + "ULong64Proxy"; break; }
         case TVirtualStreamerInfo::kBits:    { type = "T" + middle + "UIntProxy"; break; }

         case TVirtualStreamerInfo::kCharStar: { type = GetArrayType(element,"Char",container); break; }

            // array of basic types  array[8]
         case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kBool:    { type = GetArrayType(element,"Bool",container ); break; }
         case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kChar:    { type = GetArrayType(element,"Char",container ); break; }
         case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kShort:   { type = GetArrayType(element,"Short",container ); break; }
         case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kInt:     { type = GetArrayType(element,"Int",container ); break; }
         case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kLong:    { type = GetArrayType(element,"Long",container ); break; }
         case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kLong64:  { type = GetArrayType(element,"Long64",container ); break; }
         case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kFloat:   { type = GetArrayType(element,"Float",container ); break; }
         case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kFloat16: { type = GetArrayType(element,"Float16",container ); break; }
         case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kDouble:  { type = GetArrayType(element,"Double",container ); break; }
         case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kDouble32:{ type = GetArrayType(element,"Double32",container ); break; }
         case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kUChar:   { type = GetArrayType(element,"UChar",container ); break; }
         case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kUShort:  { type = GetArrayType(element,"UShort",container ); break; }
         case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kUInt:    { type = GetArrayType(element,"UInt",container ); break; }
         case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kULong:   { type = GetArrayType(element,"ULong",container ); break; }
         case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kULong64: { type = GetArrayType(element,"ULong64",container ); break; }
         case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kBits:    { type = GetArrayType(element,"UInt",container ); break; }

            // pointer to an array of basic types  array[n]
         case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kBool:    { type = GetArrayType(element,"Bool",container ); break; }
         case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kChar:    { type = GetArrayType(element,"Char",container ); break; }
         case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kShort:   { type = GetArrayType(element,"Short",container ); break; }
         case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kInt:     { type = GetArrayType(element,"Int",container ); break; }
         case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kLong:    { type = GetArrayType(element,"Long",container ); break; }
         case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kLong64:  { type = GetArrayType(element,"Long64",container ); break; }
         case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kFloat:   { type = GetArrayType(element,"Float",container ); break; }
         case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kFloat16: { type = GetArrayType(element,"Float16",container ); break; }
         case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kDouble:  { type = GetArrayType(element,"Double",container ); break; }
         case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kDouble32:{ type = GetArrayType(element,"Double32",container ); break; }
         case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kUChar:   { type = GetArrayType(element,"UChar",container ); break; }
         case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kUShort:  { type = GetArrayType(element,"UShort",container ); break; }
         case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kUInt:    { type = GetArrayType(element,"UInt",container ); break; }
         case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kULong:   { type = GetArrayType(element,"ULong",container ); break; }
         case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kULong64: { type = GetArrayType(element,"ULong64",container ); break; }
         case TVirtualStreamerInfo::kOffsetP + TVirtualStreamerInfo::kBits:    { type = GetArrayType(element,"UInt",container ); break; }

            // array counter //[n]
         case TVirtualStreamerInfo::kCounter: { type = "T" + middle + "IntProxy"; break; }


         case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kObjectp:
         case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kObjectP:
         case TVirtualStreamerInfo::kObjectp:
         case TVirtualStreamerInfo::kObjectP:
         case TVirtualStreamerInfo::kAnyp:
         case TVirtualStreamerInfo::kAnyP:
         case TVirtualStreamerInfo::kSTL + TVirtualStreamerInfo::kObjectp:
         case TVirtualStreamerInfo::kSTL + TVirtualStreamerInfo::kObjectP:
            // set as pointers and fall through to the next switches
            // ispointer = true;
         case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kObject:
         case TVirtualStreamerInfo::kObject:
         case TVirtualStreamerInfo::kTString:
         case TVirtualStreamerInfo::kTNamed:
         case TVirtualStreamerInfo::kTObject:
         case TVirtualStreamerInfo::kAny:
         case TVirtualStreamerInfo::kOffsetL + TVirtualStreamerInfo::kAny:
         case TVirtualStreamerInfo::kSTL:
         case TVirtualStreamerInfo::kBase: {
            TClass *cl = element->GetClassPointer();
            if (cl) {
               type = Form("T%sObjProxy<%s >",
                           middle.Data(),cl->GetName());
               cname = cl->GetName();
               if (cl==TClonesArray::Class()) {
                  isclones = TBranchProxyClassDescriptor::kClones;
                  containerName = "TClonesArray";

                  Long64_t i = branch->GetTree()->GetReadEntry();
                  if (i<0) i = 0;
                  branch->GetEntry(i);

                  //char *obj = branch->GetObject();

                  // now need to follow it through to this pointer!

                  TClonesArray *arr;

                  TString fullpath = branch->GetName();
                  fullpath += ".";
                  if (path && strlen(path)>0) fullpath.Append(path).Append(".");
                  fullpath += element->GetName();

                  TTreeFormula *formula = new TTreeFormula("clones",fullpath,branch->GetTree());

                  TFormLeafInfo *leafinfo = formula->GetLeafInfo(0);
                  TLeaf *leaf = formula->GetLeaf(0);
                  R__ASSERT(leaf && leafinfo);

                  arr = (TClonesArray*)leafinfo->GetLocalValuePointer(leaf,0);

                  /*
                    if (ispointer) {
                    arr = (TClonesArray*)*(void**)(obj+lOffset);
                    } else {
                    arr = (TClonesArray*)(obj+lOffset);
                    }
                  */
                  if (arr) cname = arr->GetClass()->GetName();

                  if (cname.Length()==0) {
                     Error("AnalyzeTree",
                           "Introspection of TClonesArray in older file not implemented yet.");
                  }
                  delete formula;
               } else if (cl->GetCollectionProxy()) {
                  isclones = TBranchProxyClassDescriptor::kSTL;
                  containerName = cl->GetName();
                  cl = cl->GetCollectionProxy()->GetValueClass();
               }
            }
            else Error("AnalyzeTree","missing class for %s.",branch->GetName());
            if (element->IsA()==TStreamerBase::Class()) {
               // prefix  = "base";
               isBase = true;
            }
            AddForward(cl);
            AddHeader(cl);
            break;
         }

         default:
            Error("AnalyzeTree",
                  "Unsupported type for %s %s %d",
                  branch->GetName(), element->GetName(), element->GetType());

      }

      dataMemberName = element->GetName();

      if (true) {

         // See AnalyzeTree for similar code!
         TBranchProxyClassDescriptor *cldesc;

         TClass *cl = TClass::GetClass(cname);
         if (cl && cl->CanSplit()) {
            cldesc = new TBranchProxyClassDescriptor(cl->GetName(), cl->GetStreamerInfo(),
                                                     branch->GetName(),
                                                     isclones, 0 /* non-split object */,
                                                     containerName);

            TVirtualStreamerInfo *info = cl->GetStreamerInfo();
            TStreamerElement *elem = 0;

            TString subpath = path;
            if (subpath.Length()>0) subpath += ".";
            subpath += dataMemberName;

            TIter next(info->GetElements());
            while( (elem = (TStreamerElement*)next()) ) {
               AnalyzeElement(branch, elem, level+1, cldesc, subpath.Data());
            }

            TBranchProxyClassDescriptor *added = AddClass(cldesc);
            if (added) type = added->GetName();
         }

      }

      pxDataMemberName = /* prefix + */ dataMemberName;
      if (topdesc) {
         topdesc->AddDescriptor( new TBranchProxyDescriptor( pxDataMemberName.Data(), type,
                                                             dataMemberName.Data(), false),
                                 isBase );
      } else {
         Error("AnalyzeTree","topdesc should not be null in TTreeAvroGenerator::AnalyzeElement.");
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   /// Parse the options string.

   void TTreeAvroGenerator::ParseOptions()
   {
      TString opt = fOptionStr;

      fOptions = 0;
      if ( opt.Contains("nohist") ) {
         opt.ReplaceAll("nohist","");
         fOptions |= kNoHist;
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   /// Add the "pragma C++ class" if needed and return
   /// true if it has been added _or_ if it is known to
   /// not be needed.
   /// (I.e. return kFALSE if a container of this class
   /// can not have a "pragma C++ class"

   static Bool_t R__AddPragmaForClass(TTreeAvroGenerator *gen, TClass *cl)
   {
      if (!cl) return kFALSE;
      if (cl->GetCollectionProxy()) {
         TClass *valcl = cl->GetCollectionProxy()->GetValueClass();
         if (!valcl) {
            if (!cl->IsLoaded()) gen->AddPragma(Form("#pragma link C++ class %s;\n", cl->GetName()));
            return kTRUE;
         } else if (R__AddPragmaForClass(gen, valcl)) {
            if (!cl->IsLoaded()) gen->AddPragma(Form("#pragma link C++ class %s;\n", cl->GetName()));
            return kTRUE;
         }
      }
      if (cl->IsLoaded()) return kTRUE;
      return kFALSE;
   }

   /////////////////////////////////////////////////////////////////////////////
   /// Add the "pragma C++ class" if needed and return
   /// true if it has been added _or_ if it is known to
   /// not be needed.
   /// (I.e. return kFALSE if a container of this class
   /// can not have a "pragma C++ class"

   static Bool_t R__AddPragmaForClass(TTreeAvroGenerator *gen, const char *classname)
   {
      return R__AddPragmaForClass( gen, TClass::GetClass(classname) );

   }

} // namespace Internal
} // namespace ROOT
