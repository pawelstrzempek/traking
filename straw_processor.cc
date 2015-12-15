#include <TFile.h>
#include <TTree.h>
#include "TPatraSelector.h"
#include "TPatraSelector.cc"
#include "./geometry/TStrawModule.cc"
#include <TH1F.h>
#include <TH2F.h>
#include <iostream>

void straw_processor(const char* fileName, unsigned eventsQty = 100000){
 //** Open the file containing ntuple PATRA which will be rewritten. In the ntuple the drift time 
 //** will be changed with the drift radius (calibration module) and the channel number will be 
 //** changed by the x y coordinance (geometry module).

 //** open file and load the input ntuple
 TTree *tree;
 //TFile *f = new TFile("/home/pandastraws/hldFiles/asicSource/cosmics/1800V/cosmics1800_PATRA.root");
 TFile *f = new TFile(fileName,"UPDATE");
	 if (f->IsZombie()) { std::cout << "Error opening file" <<std:: endl; return;}
 f->GetObject("PATRA",tree);
 //** create selector for the PATRA tree. Selector second argument is the name of the output file.
 string  out_file_name = fileName;
 out_file_name = out_file_name.substr(0, out_file_name.size() - 5);
 out_file_name += "C.root"; //resulting inj outfile name: 'in_fileName_PATRAC.root'
 TPatraSelector selector(tree,out_file_name);
 std::cout<<"Selector created\n";
 //** create the calibration module and attach it to the ntuple selector (TPatraSelector which iterates 
 //** event by event and rewrites them to the new ntuple PATRAC.
 // TStrawCalibration *clibratinModule new TStrawCalibration(int channelsQty, TH2F* dt_all, unsigned t0_param, unsigned tk_param, double r_tube, double r_wire);
 TH2F *dt_all = (TH2F*)f->Get("dt_barcode");
 TStrawCalibration *clibratinModule = new TStrawCalibration(96, dt_all, 1, 97, 5.05, 0.02); //the unit is mm
//std::cout<<"Calibration module created\n";
//std::cout<<"Generate calibration curve\n";
                ///clibratinModule->CalibrationCurveGenerate();
                ///std::cout<<"t0 calculate\n"<<clibratinModule->p0;
                ///t0 = T0Calculate(dt_all,t0_param);
                ///std::cout<<"tk calculate\n";
                ///tk = TkCalculate(dt_all,tk_param); 
 selector.AttachCalibration(clibratinModule);
// clibratinModule->SaveDiagnosticHistograms(f);
 //** create straw detector geometry
 //** FT consist of 3 moldules each has 32 straws
 //TStrawModule::TStrawModule(double x_coordinate, double y_coordinate, double angle_rotation, unsigned int x, unsigned int y, double straw_radius)
 TStrawModule *ft1 = new TStrawModule(0.,0.,0.,32,1,5.05);
 TStrawModule *ft2 = new TStrawModule(0.,100.,0.,32,1,5.05);
 TStrawModule *ft3 = new TStrawModule(0.,200.,0.,32,1,5.05);

 TStrawGeometry *sg = new TStrawGeometry(ft1);
 sg ->AttachModule(ft2);
 sg ->AttachModule(ft3);
std::cout<<"Geometry attached\n";
 selector.AttachGeometry(sg);

///TPatraSelector selector(tree);
std::cout<<"Entering main loop\n";
selector.Loop(eventsQty);




return;
}
