#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include "TFotracprtfSelector.cc"
#include <iostream>

//** This module is suppose to do a precise tracking usig the Minuit functionality form ROOT. 
//** It uses the FOTRAC_PR_TF tuple.
//** 


void straw_trackingMinuit(const char* fileName, unsigned eventsQty = 1000000){
 //** open file and load the input ntuple
 TTree *tree;
 TFile *f = new TFile(fileName);
	 if (f->IsZombie()) { std::cout << "Error opening file" <<std:: endl; return;}
 f->GetObject("FOTRAC_PR_TF",tree);

 //** create selector for the FOTRA tree. Selector second argument is the name of the output file.
 string  out_file_name = fileName;
 out_file_name = out_file_name.substr(0, out_file_name.size() - 5);
 out_file_name += "P.root"; //resulting in outfile name: 'in_fileName_FOTRAC_PR_TFP.root' -TFP track fitted precise
 TFotracprtfSelector selector(tree,out_file_name);
 std::cout<<" FOTRAC_PR_TFP selector created\n";

 std::cout<<"Entering main loop\n";
 selector.LoopTrakingMinuit(eventsQty);
 


return;
}
