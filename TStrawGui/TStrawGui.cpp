#include <TApplication.h>
#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TH1F.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TRootEmbeddedCanvas.h>
#include <fstream>

#include "TStrawGui.h"

using namespace std;

//FUNCTIONS/////////////
///////////////////////////////////////////////////////////////



TStrawGui::TStrawGui(const TGWindow *p,UInt_t w,UInt_t h)
{ 
// Create a main frame
fMain = new TGMainFrame(p,w,h);
// Create canvas widget
fEcanvas = new TRootEmbeddedCanvas("Ecanvas",fMain,800,800);
fMain->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY,10,10,10,1));
TCanvas *tmp = fEcanvas->GetCanvas();
//tmp->Divide(7,4,0.001,0.001);

SS1 = new TStrawMap(16,2);
SS1->Draw(0.05,0.05);

SS2 = new TStrawMap(16,2);
SS2->Draw(0.05,22*(SS1->GetStrawRadius())+0.05);

SS3 = new TStrawMap(16,2);
SS3->Draw(0.05,2*22*(SS1->GetStrawRadius())+0.05);
// Create a horizontal frame widget with buttons
TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain,800,40);

TGLabel *fLabelFilePath = new TGLabel(fMain,"File full path:");
fLabelFilePath->SetTextJustify(36);
fLabelFilePath->SetMargins(0,0,0,0);
fLabelFilePath->SetWrapLength(-1);
fMain->AddFrame(fLabelFilePath, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
//fLabel2051->MoveResize(184,568,96,18);

TGFont *ufont;         // will reflect user font changes
ufont = gClient->GetFont("-*-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");

TGGC   *uGC;           // will reflect user GC changes
// graphics context changes
GCValues_t valEntryEventNumber;
valEntryEventNumber.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
gClient->GetColorByName("#000000",valEntryEventNumber.fForeground);
gClient->GetColorByName("#e8e8e8",valEntryEventNumber.fBackground);
valEntryEventNumber.fFillStyle = kFillSolid;
valEntryEventNumber.fFont = ufont->GetFontHandle();
valEntryEventNumber.fGraphicsExposures = kFALSE;
uGC = gClient->GetGC(&valEntryEventNumber, kTRUE);

//TGTextEntry *fTextEntryFilePath = new TGTextEntry(fMain, new TGTextBuffer(14),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
fTextEntryFilePath = new TGTextEntry(fMain, new TGTextBuffer(14),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
fTextEntryFilePath->SetMaxLength(4096);
fTextEntryFilePath->SetAlignment(kTextLeft);
//fTextEntryFilePath->SetText("fTextEntryFilePath");
fTextEntryFilePath->Resize(93,fTextEntryFilePath->GetDefaultHeight());
fMain->AddFrame(fTextEntryFilePath, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
fTextEntryFilePath->MoveResize(304,520,250,22);

TGTextButton *loadFileBtn = new TGTextButton(hframe,"&Load");
loadFileBtn->Connect("Clicked()","TStrawGui",this,"LoadFile()");
hframe->AddFrame(loadFileBtn, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

TGTextButton *drawPrev = new TGTextButton(hframe,"&<<");
drawPrev->Connect("Clicked()","TStrawGui",this,"DoDrawPrev()");
hframe->AddFrame(drawPrev, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

TGTextButton *drawNext = new TGTextButton(hframe,"&>>");
drawNext->Connect("Clicked()","TStrawGui",this,"DoDrawNext()");
hframe->AddFrame(drawNext, new TGLayoutHints(kLHintsCenterX,5,5,3,4));


eventNumberDisplay = new TGNumberEntry(fMain, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 0,(TGNumberFormat::EAttribute) 1);
   eventNumberDisplay->SetName("eventNumberDisplay");
   fMain->AddFrame(eventNumberDisplay, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
//   eventNumberDisplay->MoveResize(400,656,59,22);


TGTextButton *exit = new TGTextButton(hframe,"&Exit","gApplication->Terminate(0)");
hframe->AddFrame(exit, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
fMain->AddFrame(hframe, new TGLayoutHints(kLHintsCenterX,2,2,2,2));
// Set a name to the main frame
fMain->SetWindowName("Straw display");
// Map all subwindows of main frame
fMain->MapSubwindows();
// Initialize the layout algorithm
fMain->Resize(fMain->GetDefaultSize());
// Map main frame
fMain->MapWindow();
}


TStrawGui::~TStrawGui()
{
  // Clean up used widgets: frames, buttons, layouthints
fileObj.Close();
fMain->Cleanup();
delete fMain;
  
}
void TStrawGui::DoDrawNext() 
{
TCanvas *fCanvas = fEcanvas->GetCanvas();
	eventNumberDisplay->SetNumber(eventNumberDisplay->GetNumber() + 1 );
//    hist[k]->Draw();
fCanvas->Update();//odswieżanie
}

void TStrawGui::DoDrawPrev()         
{
TCanvas *fCanvas = fEcanvas->GetCanvas();
        eventNumberDisplay->SetNumber(eventNumberDisplay->GetNumber() - 1 );
//    hist[k]->Draw();
fCanvas->Update();//odswieżanie
}






void TStrawGui::LoadFile(){


//std::cerr<<"File loaded: "<<fTextEntryFilePath->GetText()<<std::endl;
fileName= fTextEntryFilePath->GetText();
fileObj = new TFile(fileName.c_str());
std::cerr<<"File loaded: "<<fileName<<std::endl;

}

/*
void example()
{    // Popup the GUI...
new TStrawGui(gClient->GetRoot(),500,500);
 }
*/


/*

int main(int argc, char **argv) {
TApplication theApp("App",&argc,argv);
example();
theApp.Run();
return 0;
}
*/
