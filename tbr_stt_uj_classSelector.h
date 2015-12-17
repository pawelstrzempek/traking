//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Dec  8 15:10:22 2015 by ROOT version 5.34/32
// from TTree STT_UJ/ToT
// found on file: /home/pandastraws/hldFiles/asicSource/cosmics/1800V/cosmics1800_histos.root
//////////////////////////////////////////////////////////

#ifndef stt_uj_classSelector_h
#define stt_uj_classSelector_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1F.h>

// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class stt_uj_classSelector {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           globEvNum;
   Int_t           chNum;
   Int_t           fplTDC;
   Int_t           felTDC;
   Double_t        ftTDC2;
   Double_t        ToT;

   // List of branches
   TBranch        *b_globEvNum;   //!
   TBranch        *b_chNum;   //!
   TBranch        *b_fplTDC;   //!
   TBranch        *b_felTDC;   //!
   TBranch        *b_ftTDC2;   //!
   TBranch        *b_ToT;   //!

   stt_uj_classSelector(TTree *tree=0);
   virtual ~stt_uj_classSelector();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual TH1F*     Loop(double);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef stt_uj_classSelector_cxx
stt_uj_classSelector::stt_uj_classSelector(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("/home/pandastraws/hldFiles/asicSource/cosmics/1800V/cosmics1800_histos.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("/home/pandastraws/hldFiles/asicSource/cosmics/1800V/cosmics1800_histos.root");
      }
      f->GetObject("STT_UJ",tree);

   }
   Init(tree);
}

stt_uj_classSelector::~stt_uj_classSelector()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t stt_uj_classSelector::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t stt_uj_classSelector::LoadTree(Long64_t entry)
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

void stt_uj_classSelector::Init(TTree *tree)
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
   fChain->SetBranchAddress("fplTDC", &fplTDC, &b_fplTDC);
   fChain->SetBranchAddress("felTDC", &felTDC, &b_felTDC);
   fChain->SetBranchAddress("ftTDC2", &ftTDC2, &b_ftTDC2);
   fChain->SetBranchAddress("ToT", &ToT, &b_ToT);
   Notify();
}

Bool_t stt_uj_classSelector::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void stt_uj_classSelector::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t stt_uj_classSelector::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef stt_uj_classSelector_cxx
