#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TH2F.h>
#include "TFotrackerSelector.cc"
#include <iostream>

//** This module is suppose to do pattern recognition. For the moment it makes just simple 
//** events filtering. It creates the TFotracSelectro object which has overloaded methods
//** for each detector (e.g. stt or ft)


void straw_tracking(const char* fileName, unsigned eventsQty = 1000000){
 //** open file and load the input ntuple
 TTree *tree;
 TFile *f = new TFile(fileName);
	 if (f->IsZombie()) { std::cout << "Error opening file" <<std:: endl; return;}
 f->GetObject("FOTRAC_PR",tree);

 //** create selector for the FOTRA tree. Selector second argument is the name of the output file.
 string  out_file_name = fileName;
 out_file_name = out_file_name.substr(0, out_file_name.size() - 5);
 out_file_name += "_TF.root"; //resulting in outfile name: 'in_fileName_FOTRAC.root'
 TFotrackerSelector selector(tree,out_file_name);
 std::cout<<" FOTRAC_PR selector created\n";

 std::cout<<"Entering main loop\n";
 selector.LoopTraking(eventsQty);

 


return;
}
