{

gSystem->Load("libGraf");
gSystem->Load("/home/pandastraws/Unpacker/Unpacker/Unpacker2/libUnpacker2.so");
printf("UNPACKER2 LOADED\n"); 
/*
gROOT->ProcessLine(".L ../Unpacker/main_tools/calculate_times.cc+");
gROOT->ProcessLine(".L ../Unpacker/main_tools/calculate_hits.cc+");
gROOT->ProcessLine(".L ../Unpacker/main_tools/calculate_tot_offsets.cc+");
gROOT->ProcessLine(".L ../Unpacker/main_tools/calculate_dnl_params.cc+");
gROOT->ProcessLine(".L ../Unpacker/main_tools/pgenerate_diag_histo_panda.cc+");
gROOT->ProcessLine(".L ../Unpacker/main_tools/run_calibration.cc+");
gROOT->ProcessLine(".L ../Unpacker/main_tools/run_analysis.cc+");
gROOT->ProcessLine(".L ./straw_pgenerate_diag_histo_panda.cc+");
printf("TRB3 Utilities loaded\n");
*/
gStyle->SetPalette(1);
gStyle->SetOptFit(1111);
}
