 
#include <TQObject.h>
#include <RQ_OBJECT.h>
#include "TStrawGeometry.cc"

#include <TFile.h>
#include <string>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>
//#include "./../TFotracSelector.cc"
//#include "./../TFotracprtfSelector.cc"
#include "./../TFotracprtfpSelector.cc"
#include <TGLabel.h>

class TGWindow;
class TGMainFrame;
class TRootEmbeddedCanvas;
class TStrawGui {
RQ_OBJECT("TStrawGui")
private:
TGMainFrame *fMain;
TRootEmbeddedCanvas *fEcanvas;

int ft_selector_flag;

TStrawModule *ft1; 
TStrawModule *ft2; 
TStrawModule *ft3; 

TStrawGeometry *sg; //pointer to obj storing geometry

TFotracSelector* ft_selector; //pointer to obj which iterates through nTuple. Containst channels and drift radius
TFotracprtfSelector* ft_prtf_selector; // nTuple the same as the ft_selector but has tracks fitted
//TFotracprtpfSelector* ft_prtfp_selector; // nTuple the same as the ft_selector but has tracks fitted


TGTextEntry *fTextEntryNtupleName;
TGTextEntry *fTextEntryFilePath;
TGNumberEntry *eventNumberDisplay;
TFile *fileObj;
std::string  fileName;
unsigned short dic_number;

unsigned short operation_mode_flag;

	
public:
TStrawGui(const TGWindow *p,UInt_t w,UInt_t h);
virtual ~TStrawGui();
void DoDrawPrev();
void DoDrawNext();
void InitSelector(); //this method adds the selector object
void LoadFile();
};
