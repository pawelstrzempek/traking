#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TH2F.h>
#include <TTree.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TFile.h>
#include <time.h>


void straw_processor(const char* fileName, unsigned int = 1000000);
int straw_preprocessor(int eventsNum, const char* fileName, int referenceChannel = 148);
void straw_pattern_recognition(const char* fileName, unsigned int = 1000000);
void straw_tracking(const char* fileName, unsigned eventsQty = 1000000);
void straw_trackingMinuit(const char* fileName, unsigned eventsQty = 1000000);

int run_all(char* fileName, int eventsNum=100000000)
{
	clock_t tStart = clock();
//**
//**	taking raw data file containing hits only and changing it to file with ntuple
//**	containig the channel number, tot and drif time  
	string newFileName = "bleble";
	newFileName = string(fileName);
	straw_preprocessor(eventsNum,newFileName.c_str(),148);

//**	taking previously generated file and processing it by including detector
//**	geometry and calibration
	newFileName = newFileName.substr(0, newFileName.size() - 5);
	newFileName += "_FOTRA.root";
	straw_processor(newFileName.c_str(),eventsNum);
	
//**	the patter recognition stage filters the events in which there are anough hits
//**
	newFileName = newFileName.substr(0, newFileName.size() - 5);
	newFileName += "C.root";
	straw_pattern_recognition(newFileName.c_str(),eventsNum);

//**	the traking macro runs traking to the center of straw tubes (preparing the prefit)
//**
        newFileName = newFileName.substr(0, newFileName.size() - 5);
        newFileName += "_PR.root";
        straw_tracking(newFileName.c_str(),eventsNum);

//**	the trakingMinuit macro uses the ROOT minuit functionality in order to fit tracks to the
//**	drift circles
	newFileName = newFileName.substr(0, newFileName.size() - 5);
        newFileName += "_TF.root";	
	straw_trackingMinuit(newFileName.c_str(),eventsNum);
  	
	
	printf("\n\n >>>>>>>  Final file: %s\n", newFileName.c_str());
	printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);

    return 0;
}
