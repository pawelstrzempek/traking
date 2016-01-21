#include "TFotracSelector.cc"
#include <TLinearFitter.h>
#include <TF1.h>
#include <iostream>
#include <TH1F.h>


class TFotracprSelector: public TFotracSelector{
public: 
	double a;
	double b;
	double a_err;
	double b_err;
	TLinearFitter *lf;
 	TF1 *f1;
	TH1F *eff;
	TH1F *deltaX; //the distance between the reconstructed prefit and straw center belonging to the track reconstructed
	TH1F *strawIllumination; // number of hits per straw in the middle layer

	double FT2_1_Z_coord;
	double FT2_2_Z_coord;

TFotracprSelector(TTree *tree, std::string fname);
void LoopTraking(unsigned eventsQty, unsigned bin_shift = 0);
void SetTree(TTree *tree);

~TFotracprSelector(){
	out_file->Close();
}


};


void TFotracprSelector::SetTree(TTree *tree){
 Init(tree);
 current_entry_number = 1;
 return;
}

TFotracprSelector::TFotracprSelector(TTree *tree, std::string fname) 
{
//** USER DEFINED COORDINANCE OF LAYERS:
   FT2_1_Z_coord = 91.26;
   FT2_2_Z_coord = 100;
//**===================================
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

   eff = new TH1F("efficiency","Bin1: all hits, bin2: layer1 or layer2, bin3: layer1 and layer2",15,0,15);
   deltaX = new TH1F("deltaX","the distance between the reconstructed prefit and straw center belonging to the track reconstructed",320,-160,160);
   strawIllumination = new TH1F("strawIllumination", "number of hits per straw in the middle layer",100,-1,99);
}

void TFotracprSelector::LoopTraking(unsigned eventsQty, unsigned bin_shift )
{
//** Important remark. This method is calculating the linear fit parameters performed on the straws
//** wire positions. As the TLinearfitter does not work correctly on the wire position in real configuration
//** (X,Z) we change the coordinance order to (Z,X). We perform the fit and then calculated the a and b 
//** coefficients as a_final = 1/a_cal; b_final = -b_cal/a_cal


std::vector<entryHolder> entriesVector;
std::vector<entryHolder> entriesFT2;

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

      if(jentry%10000 == 0)
	std::cout<<"Entries done: "<<jentry<<" of "<<nentries<<"\r";

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
		//** this was original way of getting a and be param
		   a =lf->GetParameter(1);
		   b =lf->GetParameter(0);
		   a_err =lf->GetParError(1);
		   b_err =lf->GetParError(0);
		//** must be change to:
		//   a = 1/(lf->GetParameter(0));
		//   b = -(lf->GetParameter(1)/lf->GetParameter(0));

		//   a_err = 1;
		//   b_err = 1;
                   tree_out->Fill();
                }
	//	std::cout<<currentEvent<<": a_org="<<lf->GetParameter(0)<<"  b_org="<<lf->GetParameter(1)<<"  a="<<a<<"  b="<<b<<"\n";
                local_event_counter++;
		//lf->Eval();
		//std::cout<<"a="<<lf->GetParameter(0)<<"  b="<<lf->GetParameter(1)<<std::endl;
		lf->ClearPoints();
	//** at this stage we have line fitted to hits in ft1 and ft3. We check if hits from ft2 (stored in entriesFT2) belog to fitted track.
		double x_ft2_2 = a*(FT2_2_Z_coord)+b;//(FT2_2_Z_coord-b)/a;//ay+b   //x coordinate of track crossing upper layer of FT2	
		double x_ft2_1 = a*(FT2_1_Z_coord)+b;//(FT2_1_Z_coord-b)/a; //x coordinate of track crossing lower layer of FT2	
		bool lay1_hit = false;
		bool lay2_hit = false;
		for (unsigned j=0; j<entriesFT2.size(); j++){
			if( (entriesFT2[j]).s_Z == FT2_2_Z_coord){//** hit in upper layer
				if( (entriesFT2[j]).s_X < x_ft2_2+25 && (entriesFT2[j]).s_X > x_ft2_2-25 )
					lay2_hit = true;
			//	 std::cout<<"ft2_2: "<<(entriesFT2[j]).s_Z<<std::endl;
				deltaX->Fill(x_ft2_2-(entriesFT2[j]).s_X);
			}
			else //** hit in lower layer
			{
			        if( (entriesFT2[j]).s_X < x_ft2_1+25 && (entriesFT2[j]).s_X > x_ft2_1-25 )
                                        lay1_hit = true;
			//	 std::cout<<"ft2_1: "<<(entriesFT2[j]).s_Z<<std::endl;
				deltaX->Fill(x_ft2_1-(entriesFT2[j]).s_X);
			}
//		strawIllumination->Fill((entriesFT2[j]).s_chNum );
		}
		
		std::cout<<"check eff: a="<<a<<" b="<<b<<" x1="<<x_ft2_1<<" x2="<<x_ft2_2<<std::endl;

	//** eliminate cases with the faulty straw number 60
		if( 136.5 > x_ft2_1+25 || 136.5 < x_ft2_1-25 ){
			eff->Fill(1+bin_shift);
			if(lay1_hit || lay2_hit)
				eff->Fill(2+bin_shift);
			if(lay1_hit && lay2_hit)
				eff->Fill(3+bin_shift);
		}

	//** we clear the buffers:
                entriesVector.clear();
                entriesFT2.clear();
      }
        entryHolder eh = {globEvNum, chNum, ftTDC2, ToT, driftR, X, Z};
        entriesVector.push_back(eh);
	//** lf->AddPoint(&X,Z); //original coordinance must be change to:
	strawIllumination->Fill(chNum);

	//** we add only points from boarder modules (FT1 and FT3). For them Z is 0, -8 and 192 and 200
	if(Z<(FT2_1_Z_coord-10) || Z > (FT2_2_Z_coord+10)){
		lf->AddPoint(&Z,X);
	}
	else
	{
		entriesFT2.push_back(eh);
	}

   }
   out_file->cd();
   tree_out->Write();
   deltaX->Write();
   strawIllumination->Write();
   eff->Write();
}

