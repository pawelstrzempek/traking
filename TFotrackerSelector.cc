#include "TFotracSelector.cc"
#include <TLinearFitter.h>
#include <TF1.h>
#include <iostream>



class TFotrackerSelector: public TFotracSelector{
public: 
	double a;
	double b;
	double a_err;
	double b_err;
	TLinearFitter *lf;
 	TF1 *f1;

TFotrackerSelector(TTree *tree, std::string fname);
void LoopTraking(unsigned eventsQty);



};


TFotrackerSelector::TFotrackerSelector(TTree *tree, std::string fname) 
{
   f1 = new TF1("f1","pol1");
   lf = new TLinearFitter(f1);
   current_entry_number = 1;
   Init(tree);
   out_file = new TFile(fname.c_str(),"RECREATE");
   std::cout<<"Creating ouput file: "<<fname<<std::endl;
   tree_out = new TTree("FOTRAC_PR_TF","Forward trakers calibrated, patter recognition, track fitted");
   tree_out->Branch("globEvNum", &globEvNum_out);
   tree_out->Branch("chNum", &chNum_out);
   tree_out->Branch("ftTDC2", &ftTDC2_out);
   tree_out->Branch("ToT", &ToT_out);
   tree_out->Branch("driftR", &driftR_out);
   tree_out->Branch("X", &X_out);
   tree_out->Branch("Z", &Z_out);
   
   tree_out->Branch("a", &a);
   tree_out->Branch("b", &b);
   tree_out->Branch("a_err", &a_err);
   tree_out->Branch("b_err", &b_err);

}

void TFotrackerSelector::LoopTraking(unsigned eventsQty)
{
std::vector<entryHolder> entriesVector;

   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();
   if(eventsQty < nentries) //limiting the number of events according to user request
        nentries = eventsQty;

   Long64_t nbytes = 0, nb = 0;
   unsigned currentEvent = 1;
   unsigned local_event_counter = 1;

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      if(currentEvent != (unsigned)globEvNum){ //check if the event ended
        currentEvent = globEvNum;
                //**we save the event
		lf->Eval();
                for (unsigned i=0; i<entriesVector.size(); i++){
                   globEvNum_out = local_event_counter;
                   chNum_out = (entriesVector[i]).s_chNum;
                   ftTDC2_out = (entriesVector[i]).s_ftTDC2;
                   ToT_out = (entriesVector[i]).s_ToT;
                   driftR_out = (entriesVector[i]).s_driftR;
                   X_out = (entriesVector[i]).s_X;
                   Z_out = (entriesVector[i]).s_Z;
		   a =lf->GetParameter(0);
		   b =lf->GetParameter(1);
		   a_err =lf->GetParError(0);
		   b_err =lf->GetParError(1);
                   tree_out->Fill();
                }
                entriesVector.clear();
                local_event_counter++;
		//lf->Eval();
		//std::cout<<"a="<<lf->GetParameter(0)<<"  b="<<lf->GetParameter(1)<<std::endl;
		lf->ClearPoints();
      }
        entryHolder eh = {globEvNum, chNum, ftTDC2, ToT, driftR, X, Z};
        entriesVector.push_back(eh);
	lf->AddPoint(&X,Z);

   }
   tree_out->Write();

}

