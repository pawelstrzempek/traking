#define TFotraSelector_cxx
#include "TFotraSelector.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <utility>
void TFotraSelector::Loop(unsigned eventsQty)
{
//   In a ROOT session, you can do:
//      Root > .L TFotraSelector.C
//      Root > TFotraSelector t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();
   if(eventsQty < nentries) //limiting the number of events according to user request
	nentries = eventsQty;



   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
	//std::cout<<"dt="<<ftTDC2<<"  r="<<calibModule->DriftTimeToRadius(ftTDC2)<<std::endl;
        //std::cout<<"chNum="<<chNum<<"  x="<<(geometryModule->GetChannelCoordinance(chNum)).first<<"  y="<<(geometryModule->GetChannelCoordinance(chNum)).second<<"\n"<<std::endl;
	std::pair<double, double> receiver = geometryModule->GetChannelCoordinance(chNum);

	globEvNum_out = globEvNum;
	chNum_out = chNum;
	ftTDC2_out= ftTDC2;
	ToT_out = ToT;
	Z_out = receiver.second;
//	std::cout<<receiver.second<<std::endl<<std::endl;

	X_out = receiver.first;
	driftR_out = calibModule->DriftTimeToRadius(ftTDC2);
	tree_out->Fill();

   }
tree_out->Write();
calibModule->SaveDiagnosticHistograms(out_file);
geometryModule->SaveGeometry(out_file);
}

void TFotraSelector::AttachCalibration(TStrawCalibration *in_calib){
calibModule = in_calib;
}
void TFotraSelector::AttachGeometry(TStrawGeometry *in_geom){
geometryModule = in_geom;
}

