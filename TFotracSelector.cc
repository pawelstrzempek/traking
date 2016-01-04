#define TFotracSelector_cxx
#include "TFotracSelector.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <vector>
#include <iostream>
#include "entryHolder.cc"
/*
class entryHolder{
public:
   Int_t           s_globEvNum;
   Int_t           s_chNum;
   Double_t        s_ftTDC2;
   Double_t        s_ToT;
   Double_t        s_driftR;
   Double_t        s_X;
   Double_t        s_Z;
};
*/

void TFotracSelector::LoopFt96(unsigned eventsQty)
{
std::vector<entryHolder> entriesVector;
unsigned layers[6] = {0,0,0,0,0,0};

   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();
   if(eventsQty < nentries) //limiting the number of events according to user request
        nentries = eventsQty;

   Long64_t nbytes = 0, nb = 0;
   unsigned currentEvent = 1; 
   unsigned currentEventCheck = 1; 
   unsigned local_event_counter = 1;

   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;


      if(currentEvent != (unsigned)globEvNum){ //check if the event ended
	currentEvent = globEvNum;
	currentEventCheck++;
   	diagEff_ft96->Fill(1);
	hitsMultiplicity->Fill(layers[0]+layers[1]+layers[2]+layers[3]+layers[4]+layers[5]);
//	if(layers[5]==0 && layers[4]==0 && layers[3]==0 && layers[2]==0 && layers[1]==0 && layers[0]==0)
	if(currentEventCheck!=currentEvent){
		diagEff_ft96->Fill(2);//** filling empty events
		//currentEventCheck=currentEvent;
		currentEventCheck++;
	}
	if((layers[5]>=1 || layers[4]>=1) && (layers[3]>=1 || layers[2]>=1) && (layers[1]>=1 || layers[0]>=1))
		diagEff_ft96->Fill(3);//** at least one hit per layer
	
	if((layers[5]==0 && layers[4]==0) && (layers[3]>=1 && layers[2]>=1) && (layers[1]>=1 && layers[0]>=1))
		diagEff_ft96->Fill(5);
	if((layers[5]>=1 && layers[4]>=1) && (layers[3]==0 && layers[2]==0) && (layers[1]>=1 && layers[0]>=1))
		diagEff_ft96->Fill(6);
	if((layers[5]>=1 && layers[4]>=1) && (layers[3]>=1 && layers[2]>=1) && (layers[1]==0 && layers[0]==0))
		diagEff_ft96->Fill(7);
	
	if(layers[5]==1 && layers[4]==1 && layers[3]==1 && layers[2]==1 && layers[1]==1 && layers[0]==1){ //make the decision if we take the event
		diagEff_ft96->Fill(4);
		//**we save the event
		for (unsigned i=0; i<entriesVector.size(); i++){
		   globEvNum_out = local_event_counter;
	           chNum_out = (entriesVector[i]).s_chNum;
   		   ftTDC2_out = (entriesVector[i]).s_ftTDC2;
   		   ToT_out = (entriesVector[i]).s_ToT;
   		   driftR_out = (entriesVector[i]).s_driftR;
   		   X_out = (entriesVector[i]).s_X;
   		   Z_out = (entriesVector[i]).s_Z;
		//	std::cout<<"Global event "<<globEvNum<<" saved as "<<local_event_counter<<std::endl;
		   tree_out->Fill();
		}
		entriesVector.clear();
		local_event_counter++;
	}
	else
	{//** event does not meet conditions
		entriesVector.clear();
	}
	layers[0] =0; layers[1] =0; layers[2] =0;
	layers[3] =0; layers[4] =0; layers[5] =0;
      }
      //** read in next entry and add it to the vector. Several entries creates one event.
	entryHolder eh = {globEvNum, chNum, ftTDC2, ToT, driftR, X, Z};
	entriesVector.push_back(eh);
    	if(Z>195) layers[5]++;
	if(Z<195 && Z>105) layers[4]++;
	if(Z<105 && Z>95) layers[3]++;
	if(Z<95 && Z>5)  layers[2]++;
	if(Z<5 && Z>-1)  layers[1]++;
	if(Z<-1)  layers[0]++;
	
   }
   tree_out->Write();
   SaveDiagnosticHistograms();



}
