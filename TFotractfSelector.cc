#include "TFotracSelector.cc"
#include <TLinearFitter.h>
#include <TF1.h>
#include <iostream>



class TFotractfSelector: public TFotracSelector{
public: 
   // Declaration of leaf types
   Int_t           globEvNum;
   Int_t           chNum;
   Double_t        ftTDC2;
   Double_t        ToT;
   Double_t        driftR;
   Double_t        X;
   Double_t        Z;
   Double_t        a;
   Double_t        b;
   Double_t        a_err;
   Double_t        b_err;

   // List of branches
   TBranch        *b_globEvNum;   //!
   TBranch        *b_chNum;   //!
   TBranch        *b_ftTDC2;   //!
   TBranch        *b_ToT;   //!
   TBranch        *b_driftR;   //!
   TBranch        *b_X;   //!
   TBranch        *b_Z;   //!
   TBranch        *b_a;   //!
   TBranch        *b_b;   //!
   TBranch        *b_a_err;   //!
   TBranch        *b_b_err;   //!


TFotractfSelector(TTree *tree);
void Init(TTree *tree);
};


TFotractfSelector::TFotractfSelector(TTree *tree) 
{
   current_entry_number = 1;
   Init(tree);



}

void TFotractfSelector::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("globEvNum", &globEvNum, &b_globEvNum);
   fChain->SetBranchAddress("chNum", &chNum, &b_chNum);
   fChain->SetBranchAddress("ftTDC2", &ftTDC2, &b_ftTDC2);
   fChain->SetBranchAddress("ToT", &ToT, &b_ToT);
   fChain->SetBranchAddress("driftR", &driftR, &b_driftR);
   fChain->SetBranchAddress("X", &X, &b_X);
   fChain->SetBranchAddress("Z", &Z, &b_Z);
   fChain->SetBranchAddress("a", &a, &b_a);
   fChain->SetBranchAddress("b", &b, &b_b);
   fChain->SetBranchAddress("a_err", &a_err, &b_a_err);
   fChain->SetBranchAddress("b_err", &b_err, &b_b_err);
   Notify();
}
