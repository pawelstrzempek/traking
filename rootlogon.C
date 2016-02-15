{

gSystem->Load("libGraf");
gSystem->Load("/home/pandastraws/Unpacker/Unpacker/Unpacker2/libUnpacker2.so");
printf("UNPACKER2 LOADED\n"); 

gROOT->ProcessLine(".L straw_processor.cc+");
gROOT->ProcessLine(".L straw_preprocessor.cc+");
gROOT->ProcessLine(".L straw_pattern_recognition.cc+");
gROOT->ProcessLine(".L straw_traking.cc+");
gROOT->ProcessLine(".L straw_trakingMinuit.cc+");
gROOT->ProcessLine(".L run_all.cc+");
/*gROOT->ProcessLine(".L ../Unpacker/main_tools/pgenerate_diag_histo_panda.cc+");
gROOT->ProcessLine(".L ../Unpacker/main_tools/run_calibration.cc+");
gROOT->ProcessLine(".L ./straw_pgenerate_diag_histo_panda.cc+");
printf("TRB3 Utilities loaded\n");
*/
gStyle->SetPalette(1);
gStyle->SetOptFit(1111);
}
