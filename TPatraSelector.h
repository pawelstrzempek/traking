//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Dec 10 15:46:41 2015 by ROOT version 5.34/32
// from TTree PATRA/Panda trakers
// found on file: /home/pandastraws/hldFiles/asicSource/cosmics/1800V/cosmics1800_PATRA.root
//////////////////////////////////////////////////////////

#ifndef TPatraSelector_h
#define TPatraSelector_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include "TStrawCalibration.cc"
#include "./geometry/TStrawGeometry.cc"
#include <iostream>
#include <string>
// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class TPatraSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           globEvNum;
   Int_t           chNum;
   Double_t        ftTDC2;
   Double_t        ToT;

   Int_t           globEvNum_out;
   Int_t           chNum_out;
   Double_t        ftTDC2_out;
   Double_t        ToT_out;
   Double_t        Y_out;
   Double_t        X_out;
   Double_t        driftR_out;

   // List of branches
   TBranch        *b_globEvNum;   //!
   TBranch        *b_chNum;   //!
   TBranch        *b_ftTDC2;   //!
   TBranch        *b_ToT;   //!
   // Declaration of the calibration object
   TStrawCalibration *calibModule;
   // Declaration of the geometry object
   TStrawGeometry *geometryModule;
   // OtputFile 
   TFile *out_file;

   TTree *tree_out;




   TPatraSelector(TTree *tree, string);
   virtual ~TPatraSelector();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(unsigned);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   void AttachCalibration(TStrawCalibration *);
   void AttachGeometry(TStrawGeometry *);
};

#endif

#ifdef TPatraSelector_cxx
TPatraSelector::TPatraSelector(TTree *tree, string fname) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
  // if (tree == 0) {
  //    TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/home/pandastraws/hldFiles/asicSource/cosmics/1800V/cosmics1800_PATRA.root");
  //    if (!f || !f->IsOpen()) {
  //       f = new TFile("/home/pandastraws/hldFiles/asicSource/cosmics/1800V/cosmics1800_PATRA.root");
  //    }
  //    f->GetObject("PATRA",tree);

   //}
	
   out_file = new TFile(fname.c_str(),"RECREATE");
   std::cout<<"Creating ouput file: "<<fname<<std::endl;
   Init(tree);
   tree_out = new TTree("PATRAC","Panda trakers");
   tree_out->Branch("globEvNum", &globEvNum_out);
   tree_out->Branch("chNum", &chNum_out);
   tree_out->Branch("ftTDC2", &ftTDC2_out);
   tree_out->Branch("ToT", &ToT_out);
   tree_out->Branch("driftR", &driftR_out);
   tree_out->Branch("X", &X_out);
   tree_out->Branch("Y", &Y_out);


}

TPatraSelector::~TPatraSelector()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t TPatraSelector::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TPatraSelector::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void TPatraSelector::Init(TTree *tree)
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
   Notify();
}

Bool_t TPatraSelector::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TPatraSelector::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TPatraSelector::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef TPatraSelector_cxx
