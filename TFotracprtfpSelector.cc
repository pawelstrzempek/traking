#include "TFotracprtfSelector.cc"
#include <TLinearFitter.h>
#include <TF1.h>
#include <iostream>
#include <TMatrixD.h>
#include <TMinuit.h>
#include <TMath.h>
#include "entryHolderTfp.cc"



class TFotracprtfpSelector: public TFotracprtfSelector{
public: 
	TBranch        *b_a_mi;   //!
	TBranch        *b_b_mi;   //!
	TBranch        *b_a_mi_err;   //!
	TBranch        *b_b_mi_err;   //!
	TBranch	       *b_chi;   //!

TFotracprtfpSelector(TTree *tree);
void Init(TTree *tree);


~TFotracprtfpSelector(){
	if(out_file != NULL)
		out_file->Close();
}


};


//______________________________________________________________________________
TFotracprtfpSelector::TFotracprtfpSelector(TTree *tree) : TFotracprtfSelector(tree) //** we call the base constructor
{
   current_entry_number = 1;
   Init(tree);
}

void TFotracprtfpSelector::Init(TTree *tree)
{
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("a_mi", &a_mi, &b_a_mi);
   fChain->SetBranchAddress("b_mi", &b_mi, &b_b_mi);
   fChain->SetBranchAddress("a_mi_err", &a_mi_err, &b_a_mi_err);
   fChain->SetBranchAddress("b_mi_err", &b_mi_err, &b_b_mi_err);
   Notify();
}

