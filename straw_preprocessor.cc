#include "/home/pandastraws/Unpacker/Unpacker/Unpacker2/EventIII.h"
#include "/home/pandastraws/Unpacker/Unpacker/Unpacker2/TDCHitIII.h"
#include "/home/pandastraws/Unpacker/Unpacker/Unpacker2/Event.h"
#include "/home/pandastraws/Unpacker/Unpacker/Unpacker2/TDCHit.h"
#include "/home/pandastraws/Unpacker/Unpacker/Unpacker2/TDCHitExtended.h"
#include "/home/pandastraws/Unpacker/Unpacker/Unpacker2/TDCChannel.h"
#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TH2F.h>
#include <TTree.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TStyle.h>
#include <TBrowser.h>
#include <iostream>
#include <vector>
#include "TStrawCalibration.cc"


#define CHANNELS_NUMBER 196
#define DATA_CHANNELS 97 //2*48 +2*1 ref channel
//SET UP BINS FOR HISTOS
#define DT_BINS  10000
#define DT_MIN -2000
#define DT_MAX 2000 	 //260


#define TOT_BINS  10000
#define TOT_MIN 0
#define TOT_MAX 1000 	 //260

#define DT_LOW_LIMIT -100
#define DT_UPPER_LIMIT 200
#define TOT_UPPER_LIMIT 100000




TH1F* tot_hist[CHANNELS_NUMBER];
TH1F* mult_hist[CHANNELS_NUMBER];
TH1F* dt_hist[CHANNELS_NUMBER];

TH2F* tot_mult_hist[CHANNELS_NUMBER];
TH2F* tot_barcode;
TH2F* dt_barcode_raw;
TH2F* dt_vs_tot_raw;
TH2F* dt_barcode;
TH2F* dt_vs_tot;

TH1D* dt_all_raw;
TH1D* dt_all;
TH1F* deadChannelsMap;
TH1F* t0_offsets;
TH1F* global_mult;


int DC[] = {8,12,61,66,71,81}; //66 is a new channel
std::vector<int> deadChannels (DC,DC+sizeof(DC)/sizeof(int));
std::vector<int>::iterator it;


//std::map<int,std::pair<int,int> > geometryMapper();



int  straw_preprocessor(int eventsNum, const char* fileName, int referenceChannel = 51)
{
  TStrawCalibration *shifter = new TStrawCalibration();

  unsigned globEv = 0;
  int chNum = -1;
  double drifttime_tree = -1000000;
  double tot_tree = -1000000;

  TTree *tree = new TTree("FOTRA","Panda trakers");
  tree->Branch("globEvNum", &globEv);
  tree->Branch("chNum", &chNum);
  tree->Branch("ftTDC2", &drifttime_tree);
  tree->Branch("ToT", &tot_tree);

  TChain chain("T");
  chain.Add(fileName);  
  int i;
  
  EventIII* pEvent = 0;
  TDCChannel* pHit = 0;
  TClonesArray* pArray = 0;
  chain.SetBranchAddress("eventIII", &pEvent);

  string newFileName(fileName);
  newFileName = newFileName.substr(0, newFileName.size() - 5);
  newFileName += "_FOTRA.root";

  TFile* new_file = new TFile(newFileName.c_str(), "RECREATE");
  new_file->cd();
  
  Int_t entries = (Int_t)chain.GetEntries();
  cout<<"Entries = " <<entries<<endl;
  if(eventsNum<entries){
	entries = eventsNum;
	cerr<<"Entries to analyze: "<<entries<<endl;
  }
  // ------ create histograms 
	// tot histograms created with 1us width
  for (int i = 0; i < CHANNELS_NUMBER; i++) { tot_hist[i] = new TH1F(Form("tot_hist_ch%d", i), Form("tot_hist_ch%d", i), TOT_BINS, TOT_MIN, TOT_MAX); }
	// tot vs multiplicity
  for (int i = 0; i < CHANNELS_NUMBER; i++) { tot_mult_hist[i] = new TH2F(Form("tot_mult_hist_ch%d", i), Form("tot_mult_hist_ch%d", i), TOT_BINS,TOT_MIN,TOT_MAX, 128, 0, 128); }
	// multiplicity
  for (int i = 0; i < CHANNELS_NUMBER; i++) { mult_hist[i] = new TH1F(Form("mult_hist_ch%d", i), Form("mult_hist_ch%d", i), 128, 0, 128); }
	// tot barcode over channels
  tot_barcode = new TH2F("tot_barcode", "tot_barcode", 10000, 0, 1000, CHANNELS_NUMBER, 0, CHANNELS_NUMBER);
  dt_barcode_raw = new TH2F("dt_barcode_raw", "dt_barcode_raw", DT_BINS, DT_MIN, DT_MAX, CHANNELS_NUMBER, 0, CHANNELS_NUMBER);
  dt_barcode = new TH2F("dt_barcode", "dt_barcode", DT_BINS, DT_MIN, DT_MAX, CHANNELS_NUMBER, 0, CHANNELS_NUMBER);
  //dt_vs_tot_raw = new TH2F("dt_vs_tot_raw", "dt_vs_tot_raw", DT_BINS, DT_MIN, DT_MAX, TOT_BINS,TOT_MIN,TOT_MAX);
  dt_vs_tot_raw = new TH2F("dt_vs_tot_raw", "dt_vs_tot_raw", 2000, -500, 500, 2200,-100,1000);
  dt_vs_tot = new TH2F("dt_vs_tot", "dt_vs_tot", 2000, -500, 500, 2200,-100,1000);
  //plane2eff = new TH1F("plane2eff","plane2eff",6,0,6);
  //planemult3 = new TH1F("planemult3","planemult3",5,0,5);
  //planemult3x1 = new TH1F("planemult3x1","planemult3x1",5,0,5);
  //planemult6 = new TH1F("planemult6","planemult6",5,0,5);

	// drift time histograms
  for (int i = 0; i < CHANNELS_NUMBER; i++) { dt_hist[i] = new TH1F(Form("dt_hist%d", i), Form("dt_hist%d", i), DT_BINS,DT_MIN,DT_MAX); }
  //refTimesDiff = new TH1F("refTimesDiff","refTimesDiff", 20000, -1000, 1000);

  t0_offsets = new TH1F ("t0_offsets","offsets from time of arrivial of the lead edge of the signal to the TDC",200,-10,10);
  deadChannelsMap = new TH1F("deadChannelsMap","deadChannelsMap",CHANNELS_NUMBER,1,CHANNELS_NUMBER+1);
  global_mult = new TH1F("global_mult","global_mult",10,0,10);

  for(it = deadChannels.begin(); it != deadChannels.end(); it++)
	deadChannelsMap->Fill((*it));

// ------ pre loop to find dt offsets --------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
  for(Int_t i = 0; i < entries; i++)
        {
    if (i % 10000 == 0) cerr<<i<<" of "<<entries<<"\r";
    if (i == eventsNum) break;
    chain.GetEntry(i);
    pArray = pEvent->GetTDCChannelsArray();
    if (pArray == 0) continue;
    TIter iter(pArray);
   // globEv++;

   double refTime = -200000;
   double refTime0 = -200000;
   double refTime49 = -200000;
   double refTime98 = -200000;
   double refTime147 = -200000;
   //extracting reference time
   while( pHit = (TDCChannel*) iter.Next() )
   {
         // fetch the ref time
         if (pHit->GetChannel() == referenceChannel && pHit->GetMult() > 0){
                refTime = pHit->GetLeadTime1();
        }
        if (pHit->GetChannel() == 0 && pHit->GetMult() > 0)
                refTime0 = pHit->GetLeadTime1();
        if(pHit->GetChannel() == 49 && pHit->GetMult() > 0 )
                refTime49 = pHit->GetLeadTime1();
//      if(pHit->GetChannel() == 98 && pHit->GetMult() > 0 )
//              refTime98 = pHit->GetLeadTime1();
        if(pHit->GetChannel() == 147 && pHit->GetMult() > 0 )
                refTime147 = pHit->GetLeadTime1();
   }

    if (refTime == -200000) //skipping events with no ref time set
        continue;
    if (refTime0 == -200000) //skipping events with no ref time set
        continue;
    if (refTime49 == -200000) //skipping events with no ref time set
        continue;
//    if (refTime98 == -200000) //skipping events with no ref time set
//        continue;
    if (refTime147 == -200000) //skipping events with no ref time set
        continue;
    iter.Begin();
			//std::cout<<"Next event=====================\n";
    while( pHit = (TDCChannel*) iter.Next() )
                {
                        //current channel 
                        int currentChannel = pHit->GetChannel();
                        if(currentChannel == 0 || currentChannel == 49 || currentChannel>97) continue;//omitting high channels with no real data
                        //skipping dead channels
                        it = find (deadChannels.begin(), deadChannels.end(), currentChannel);
                        if (it != deadChannels.end()){
                                continue;
                        }
                        if(currentChannel > 49)
                                currentChannel--;
                        if(currentChannel > 98)
                                currentChannel--;
                        if(currentChannel > 147)
                                currentChannel--;
                        double driftTime = -100000000;
			//std::cout<<"ch "<<currentChannel<<"\n";
                        for(int j = 0; j < pHit->GetMult(); j++) {
                                                                  driftTime = pHit->GetLeadTime(j)-refTime;
                                                                  if(driftTime > DT_LOW_LIMIT && driftTime < DT_UPPER_LIMIT && pHit->GetTOT(j) < TOT_UPPER_LIMIT){//250
                                                                                                        dt_vs_tot_raw->Fill(driftTime,pHit->GetTOT(j));
                                                                                                        dt_barcode_raw->Fill( driftTime , currentChannel);
                             //                                  	  std::cout<<driftTime<<std::endl;
								  }
                        }
                }//loop over on event ready
}

vector<double> shifts = shifter->T0OffsetsCalculate(dt_barcode_raw,3,DATA_CHANNELS);
//** fill in the t0 offsets to the hist
for(unsigned j =0; j< shifts.size(); j++)
	t0_offsets->Fill(shifts.at(j)); 



// ------ loop over events--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------------
  for(Int_t i = 0; i < entries; i++)
	{
    if (i % 10000 == 0) cerr<<i<<" of "<<entries<<"\r";
    if (i == eventsNum) break;
    chain.GetEntry(i);
    pArray = pEvent->GetTDCChannelsArray();
    if (pArray == 0) continue;
    TIter iter(pArray);
    globEv++;

   bool layers[6] = {false,false,false,false,false,false};
   double refTime = -200000;
   double refTime0 = -200000;
   double refTime49 = -200000;
   double refTime98 = -200000;
   double refTime147 = -200000;
   //extracting reference time
   short multbuff = 0;
   double dt1_buf=0., dt2_buf=0.;
   while( pHit = (TDCChannel*) iter.Next() )
   {
         // fetch the ref time
         if (pHit->GetChannel() == referenceChannel && pHit->GetMult() > 0){
	        refTime = pHit->GetLeadTime1();
	}
	if (pHit->GetChannel() == 0 && pHit->GetMult() > 0)
		refTime0 = pHit->GetLeadTime1();
	if(pHit->GetChannel() == 49 && pHit->GetMult() > 0 )
		refTime49 = pHit->GetLeadTime1();
//	if(pHit->GetChannel() == 98 && pHit->GetMult() > 0 )
//		refTime98 = pHit->GetLeadTime1();
	if(pHit->GetChannel() == 147 && pHit->GetMult() > 0 )
		refTime147 = pHit->GetLeadTime1();
   }

    if (refTime == -200000) //skipping events with no ref time set
	continue;
    if (refTime0 == -200000) //skipping events with no ref time set
	continue;
    if (refTime49 == -200000) //skipping events with no ref time set
	continue;
//    if (refTime98 == -200000) //skipping events with no ref time set
//        continue;
    if (refTime147 == -200000) //skipping events with no ref time set
        continue;

	//	refTimesDiff->Fill(refTime0 - refTime98);

    iter.Begin();
    while( pHit = (TDCChannel*) iter.Next() )
		{
			//current channel 
			int currentChannel = pHit->GetChannel();
			if(currentChannel == 0 || currentChannel == 49 || currentChannel>97) continue;//omitting high channels with no real data
 			//skipping dead channels
			it = find (deadChannels.begin(), deadChannels.end(), currentChannel);
  			if (it != deadChannels.end()){
    				//std::cout << "Skipping channel " << *it << '\n';
				continue;
			}
			if(currentChannel > 49)
				currentChannel--;
			if(currentChannel > 98)
				currentChannel--;
			if(currentChannel > 147)
				currentChannel--;
			// fill the tot vs multiplicity histograms
			//for(int j = 0; j < pHit->GetMult(); j++) { tot_mult_hist[pHit->GetChannel()]->Fill(pHit->GetTOT(j), j); }

			// fill the tot barcode histogram
			//for(int j = 0; j < pHit->GetMult(); j++) { tot_barcode->Fill(pHit->GetTOT(j), pHit->GetChannel()-1); }
			//fill the drift time barcode histogram
			double driftTime = -100000000;
			multbuff = 0;
			dt1_buf=-1000000.;
			dt2_buf=-1000000.;
			for(int j = 0; j < pHit->GetMult(); j++) {
                                                                  driftTime = pHit->GetLeadTime(j)-refTime;
                                                                  //if(driftTime > -100 && driftTime < 200 && pHit->GetTOT(j) < 100000){//250
                                                                  if(driftTime > DT_LOW_LIMIT && driftTime < DT_UPPER_LIMIT && pHit->GetTOT(j) < TOT_UPPER_LIMIT)//250
								  	multbuff++;
								  if(multbuff== 1)
									dt1_buf=driftTime+shifts[currentChannel];
								  if(multbuff== 2)
									dt2_buf=driftTime+shifts[currentChannel];
								 }	
	
			for(int j = 0; j < pHit->GetMult(); j++) {
								  if(multbuff != 1) break;  
								  driftTime = pHit->GetLeadTime(j)-refTime;
								  //if(driftTime > -100 && driftTime < 200 && pHit->GetTOT(j) < 100000){//250
                                                                  if(driftTime > DT_LOW_LIMIT && driftTime < DT_UPPER_LIMIT && pHit->GetTOT(j) < TOT_UPPER_LIMIT){//250
								  					tot_hist[currentChannel]->Fill(pHit->GetTOT(j));	
								  					tot_mult_hist[currentChannel]->Fill(pHit->GetTOT(j), j);
							 	  					tot_barcode->Fill(pHit->GetTOT(j), currentChannel);
								  					//if(currentChannel != 49 && currentChannel != 0)
													dt_vs_tot->Fill(driftTime+shifts[currentChannel],pHit->GetTOT(j));
								  					dt_barcode->Fill( driftTime+shifts[currentChannel] , currentChannel); 
													
													//****FILLING TREE***
													drifttime_tree = driftTime;
								 					tot_tree = pHit->GetTOT(j); 
													//fpl = strawMap[currentChannel].first;
													//fel = strawMap[currentChannel].second;
													chNum = currentChannel;
													tree->Fill();
													//*****************		
													//std::cerr<<"currentChannel="<<currentChannel<<"  fpl="<<fpl<<std::endl;
								 }
			//					  std::cerr<< pHit->GetLeadTime(j)<<"   "<<refTime<<""<<(pHit->GetLeadTime(j)-refTime)<<std::endl;
			}
			global_mult->Fill(multbuff);

//			if(multbuff == 2)
//				std::cout<<"t1:"<< dt1_buf << "  t2:"<< dt2_buf<<std::endl;
			//leadTimeTmp = pHit->GetLeadTime(0);

			if(pHit->GetMult() > 0 ) 
			{
			 for(int j = 0; j < pHit->GetMult(); j++) { dt_hist[currentChannel]-> Fill(pHit->GetLeadTime(j)-refTime);}
			 //dt_hist[pHit->GetChannel()]-> Fill(pHit->GetLeadTime1()-refTime);
			 //std::cout<<"refTime: "<<refTime<<"  leadTime: "<<pHit->GetLeadTime1()<<"  dt:"<<pHit->GetLeadTime1()-refTime<<std::endl;		
			}
			else
			{
			 dt_hist[pHit->GetChannel()]-> Fill(-2222222);
			 //std::cout<<"Mult <0 refTime: "<<refTime<<"  leadTime: "<<pHit->GetLeadTime1()<<std::endl;
			}
			// fill the multiplicity histograms
			mult_hist[currentChannel]->Fill(pHit->GetMult());
		}//loop over on event ready
	}//loop over all events ready

	// ------ saving histograms
	std::cout<<"Saving histograms"<<std::endl;
	tree->Write();
	tot_barcode->Write();
	dt_barcode_raw->Write();
	dt_all_raw = dt_barcode_raw->ProjectionX();
        dt_all_raw->Write();
	dt_vs_tot_raw->Write();
	dt_barcode->Write();
	dt_all = dt_barcode-> ProjectionX();
	dt_all->Write();
	dt_vs_tot->Write();
	global_mult->Write();

	t0_offsets->Write();
	deadChannelsMap->Write();

	TDirectory *cdtot = new_file->mkdir("tot");
   	cdtot->cd();
	for (int i = 0; i < CHANNELS_NUMBER; i++) { tot_hist[i]->Write();  }
	TDirectory *cdmult = new_file->mkdir("mult");
   	cdmult->cd();
	for (int i = 0; i < CHANNELS_NUMBER; i++) { mult_hist[i]->Write(); }
	TDirectory *cddt = new_file->mkdir("dt");
   	cddt->cd();
	for (int i = 0; i < CHANNELS_NUMBER; i++) { dt_hist[i]->Write();   }
	new_file->Close();
        new_file = new TFile(newFileName.c_str(),"READ");
        new_file->cd();

	return 0;
}


/*
std::map<int,std::pair<int,int> > geometryMapper(){
std::map<int,std::pair<int,int> > myMap;

//myMap[std::make_pair(10,20)] = 25;
//FEB1
    //channel		//plane, straw
myMap[1] = std::make_pair(2,1);
myMap[2] = std::make_pair(1,1);
myMap[3] = std::make_pair(2,2);
myMap[4] = std::make_pair(1,2);
myMap[5] = std::make_pair(2,3);
myMap[6] = std::make_pair(1,3);
myMap[7] = std::make_pair(2,4);
myMap[8] = std::make_pair(1,4);
myMap[9] = std::make_pair(2,5);
myMap[10] = std::make_pair(1,5);
myMap[11] = std::make_pair(2,6);
myMap[12] = std::make_pair(1,6);
myMap[13] = std::make_pair(2,7);
myMap[14] = std::make_pair(1,7);
myMap[15] = std::make_pair(2,8);
myMap[16] = std::make_pair(1,8);

//FEB2
    //channel		//plane, straw
myMap[17] = std::make_pair(2,9);
myMap[18] = std::make_pair(1,9);
myMap[19] = std::make_pair(2,10);
myMap[20] = std::make_pair(1,10);
myMap[21] = std::make_pair(2,11);
myMap[22] = std::make_pair(1,11);
myMap[23] = std::make_pair(2,12);
myMap[24] = std::make_pair(1,12);
myMap[25] = std::make_pair(2,13);
myMap[26] = std::make_pair(1,13);
myMap[27] = std::make_pair(2,14);
myMap[28] = std::make_pair(1,14);
myMap[29] = std::make_pair(2,15);
myMap[30] = std::make_pair(1,15);
myMap[31] = std::make_pair(2,16);
myMap[32] = std::make_pair(1,16);

//FEB3
    //channel		//plane, straw
myMap[33] = std::make_pair(4,1);
myMap[34] = std::make_pair(3,1);
myMap[35] = std::make_pair(4,2);
myMap[36] = std::make_pair(3,2);
myMap[37] = std::make_pair(4,3);
myMap[38] = std::make_pair(3,3);
myMap[39] = std::make_pair(4,4);
myMap[40] = std::make_pair(3,4);
myMap[41] = std::make_pair(4,5);
myMap[42] = std::make_pair(3,5);
myMap[43] = std::make_pair(4,6);
myMap[44] = std::make_pair(3,6);
myMap[45] = std::make_pair(4,7);
myMap[46] = std::make_pair(3,7);
myMap[47] = std::make_pair(4,8);
myMap[48] = std::make_pair(3,8);
//FEB4
    //channel		//plane, straw
myMap[49] = std::make_pair(4,9);
myMap[50] = std::make_pair(3,9);
myMap[51] = std::make_pair(4,10);
myMap[52] = std::make_pair(3,10);
myMap[53] = std::make_pair(4,11);
myMap[54] = std::make_pair(3,11);
myMap[55] = std::make_pair(4,12);
myMap[56] = std::make_pair(3,12);
myMap[57] = std::make_pair(4,13);
myMap[58] = std::make_pair(3,13);
myMap[59] = std::make_pair(4,14);
myMap[60] = std::make_pair(3,14);
myMap[61] = std::make_pair(4,15);
myMap[62] = std::make_pair(3,15);
myMap[63] = std::make_pair(4,16);
myMap[64] = std::make_pair(3,16);
//FEB5
    //channel		//plane, straw
myMap[65] = std::make_pair(6,1);
myMap[66] = std::make_pair(5,1);
myMap[67] = std::make_pair(6,2);
myMap[68] = std::make_pair(5,2);
myMap[69] = std::make_pair(6,3);
myMap[70] = std::make_pair(5,3);
myMap[71] = std::make_pair(6,4);
myMap[72] = std::make_pair(5,4);
myMap[73] = std::make_pair(6,5);
myMap[74] = std::make_pair(5,5);
myMap[75] = std::make_pair(6,6);
myMap[76] = std::make_pair(5,6);
myMap[77] = std::make_pair(6,7);
myMap[78] = std::make_pair(5,7);
myMap[79] = std::make_pair(6,8);
myMap[80] = std::make_pair(5,8);
//FEB6
    //channel		//plane, straw
myMap[81] = std::make_pair(6,9);
myMap[82] = std::make_pair(5,9);
myMap[83] = std::make_pair(6,10);
myMap[84] = std::make_pair(5,10);
myMap[85] = std::make_pair(6,11);
myMap[86] = std::make_pair(5,11);
myMap[87] = std::make_pair(6,12);
myMap[88] = std::make_pair(5,12);
myMap[89] = std::make_pair(6,13);
myMap[90] = std::make_pair(5,13);
myMap[91] = std::make_pair(6,14);
myMap[92] = std::make_pair(5,14);
myMap[93] = std::make_pair(6,15);
myMap[94] = std::make_pair(5,15);
myMap[95] = std::make_pair(6,16);
myMap[96] = std::make_pair(5,16);
return myMap;
}*/
