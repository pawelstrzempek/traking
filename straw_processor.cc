#include <TFile.h>
#include <TTree.h>
#include "TPatraSelector.h"

void straw_processor(const char* fileName){
TTree *tree;
TFile *f = new TFile("/home/pandastraws/hldFiles/asicSource/cosmics/1800V/cosmics1800_PATRA.root");
f->GetObject("PATRA",tree);

TPatraSelector selector(tree);
selector.Loop();




return;
}
