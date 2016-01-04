//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Dec 16 13:26:06 2015 by ROOT version 5.34/32
// from TTree FOTRAC/Panda trakers
// found on file: /home/pandastraws/hldFiles/asicSource/cosmics/1800V/cosmics1800_FOTRAC.root
//////////////////////////////////////////////////////////

#ifndef TFotracSelector_h
#define TFotracSelector_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <string>
#include <iostream>
#include <TH1F.h>
// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class TFotracSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           globEvNum;
   Int_t           chNum;
   Double_t        ftTDC2;
   Double_t        ToT;
   Double_t        driftR;
   Double_t        X;
   Double_t        Z;

   Int_t           globEvNum_out;
   Int_t           chNum_out;
   Double_t        ftTDC2_out;
   Double_t        ToT_out;
   Double_t        driftR_out;
   Double_t        X_out;
   Double_t        Z_out;

   TH1F * diagEff_ft96;
   TH1F * hitsMultiplicity;

   // List of branches
   TBranch        *b_globEvNum;   //!
   TBranch        *b_chNum;   //!
   TBranch        *b_ftTDC2;   //!
   TBranch        *b_ToT;   //!
   TBranch        *b_driftR;   //!
   TBranch        *b_X;   //!
   TBranch        *b_Z;   //!

   // OtputFile 
   TFile *out_file;

   TTree *tree_out;

	
   //**
   //unsigned current_event_number; //physical event. One event consist of several entries
   unsigned current_entry_number; //entry in the tree

   TFotracSelector(TTree *tree=0);
   TFotracSelector(TTree *tree,std::string);
   virtual ~TFotracSelector();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     LoopFt96(unsigned);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   Long64_t 	    GetEntries();
   void SaveDiagnosticHistograms();

/*   unsigned GetCurrentEventNumber(){ return current_event_number;}
   void IncCurrentEventNumber(){current_event_number++;}
   void DecCurrentEventNumber(){current_event_number--;}
   void SetCurrentEventNumber(unsigned current_event) {current_event_number = current_event;}
*/
   unsigned GetCurrentEntryNumber(){ return current_entry_number;}
   void IncCurrentEntryNumber(){current_entry_number++;}
   void DecCurrentEntryNumber(){current_entry_number--;}
   void SetCurrentEntryNumber(unsigned current_entry) {current_entry_number = current_entry;}  

};

#endif

#ifdef TFotracSelector_cxx

TFotracSelector::TFotracSelector(TTree *tree) : fChain(0) 
{
   current_entry_number = 1;
   Init(tree);

}


TFotracSelector::TFotracSelector(TTree *tree, std::string fname) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
//   if (tree == 0) {
//      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/home/pandastraws/hldFiles/asicSource/cosmics/1800V/cosmics1800_FOTRAC.root");
//      if (!f || !f->IsOpen()) {
//         f = new TFile("/home/pandastraws/hldFiles/asicSource/cosmics/1800V/cosmics1800_FOTRAC.root");
//      }
//      f->GetObject("FOTRAC",tree);

//   }
//   current_event_number = 0;
   current_entry_number = 1;
   Init(tree);
   out_file = new TFile(fname.c_str(),"RECREATE");
   std::cout<<"Creating ouput file: "<<fname<<std::endl;
   tree_out = new TTree("FOTRAC_PR","Forward trakers calibrated and pattern recognition done");
   tree_out->Branch("globEvNum", &globEvNum_out);
   tree_out->Branch("chNum", &chNum_out);
   tree_out->Branch("ftTDC2", &ftTDC2_out);
   tree_out->Branch("ToT", &ToT_out);
   tree_out->Branch("driftR", &driftR_out);
   tree_out->Branch("X", &X_out);
   tree_out->Branch("Z", &Z_out);


   diagEff_ft96 = new TH1F("diagEff_ft96","diagEff_ft96",10,0,10);
   hitsMultiplicity = new TH1F("hitsMultiplicity","straws fired per event", 30, 0, 30);

}

TFotracSelector::~TFotracSelector()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
   out_file->Close();
}

Int_t TFotracSelector::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t TFotracSelector::LoadTree(Long64_t entry)
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

void TFotracSelector::Init(TTree *tree)
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
   Notify();
}

Bool_t TFotracSelector::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void TFotracSelector::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t TFotracSelector::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}

Long64_t TFotracSelector:: GetEntries(){
return fChain->GetEntries();
}

void TFotracSelector::SaveDiagnosticHistograms(){
 diagEff_ft96->Write();
 hitsMultiplicity->Write();

 std::cout<<"Dignostic histograms saved\n";
}



#endif // #ifdef TFotracSelector_cxx
