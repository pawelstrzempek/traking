#include <TFile.h>
#include <TTree.h>
#include "TFotraSelector.h"
#include "TFotraSelector.cc"
#include "./geometry/TStrawModule.cc"
#include <TH1F.h>
#include <TH2F.h>
#include <iostream>

void straw_processor(const char* fileName, unsigned eventsQty = 1000000){
 //** Open the file containing ntuple FOTRA which will be rewritten. In the ntuple the drift time 
 //** will be changed with the drift radius (calibration module) and the channel number will be 
 //** changed by the x y coordinance (geometry module).

 //** open file and load the input ntuple
 TTree *tree;
 //TFile *f = new TFile("/home/pandastraws/hldFiles/asicSource/cosmics/1800V/cosmics1800_FOTRA.root");
 TFile *f = new TFile(fileName,"UPDATE");
	 if (f->IsZombie()) { std::cout << "Error opening file" <<std:: endl; return;}
 f->GetObject("FOTRA",tree);
 //** create selector for the FOTRA tree. Selector second argument is the name of the output file.
 string  out_file_name = fileName;
 out_file_name = out_file_name.substr(0, out_file_name.size() - 5);
 out_file_name += "C.root"; //resulting in outfile name: 'in_fileName_FOTRAC.root'
 TFotraSelector selector(tree,out_file_name);
 std::cout<<"Selector created\n";
 //** create the calibration module and attach it to the ntuple selector (TFotraSelector which iterates 
 //** event by event and rewrites them to the new ntuple FOTRAC.
 TH2F *dt_all = (TH2F*)f->Get("dt_barcode");
 //** constructing the calibration object. It contains number of straws, pointer to 3D histogram which is then projected 
 //** as drift time sum spectrum. Then we have to percangates the % of dt sum spectrum counts which are consider to be
 //** before to and the % of dt sum spectrum counts which are consider to be after the spectrum ends. To final arguments
 //** are the straw radius in [mm] and the anode wire in [mm].
 TStrawCalibration *clibratinModule = new TStrawCalibration(96, dt_all, 1, 97, 5.05, 0.02); //the unit is mm
 selector.AttachCalibration(clibratinModule);
 //** create straw detector geometry
 //** FT consist of 3 moldules each has 32 straws
 //TStrawModule::TStrawModule(double x_coordinate, double y_coordinate, double angle_rotation, unsigned int x, unsigned int y, double straw_radius)
 TStrawModule *ft1 = new TStrawModule(0.,0.,0.,32,1,5.05);
 TStrawModule *ft2 = new TStrawModule(0.,100.,0.,32,1,5.05);
 TStrawModule *ft3 = new TStrawModule(0.,200.,0.,32,1,5.05);

 TStrawGeometry *sg = new TStrawGeometry(ft1,0.004); //second argument is scaling factor used only for graphical representation
 sg ->AttachModule(ft2);
 sg ->AttachModule(ft3);
 selector.AttachGeometry(sg);
 std::cout<<"Geometry attached\n";

///TFotraSelector selector(tree);
 std::cout<<"Entering main loop\n";
 selector.Loop(eventsQty);



return;
}
