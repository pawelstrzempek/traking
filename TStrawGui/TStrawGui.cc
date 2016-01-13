#include <TApplication.h>
#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TH1F.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TRootEmbeddedCanvas.h>
#include <fstream>
#include <iostream>
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

ft1 = new TStrawModule(0.,0.,0.,32,1,5.05);
ft2 = new TStrawModule(0.,100.,0.,32,1,5.05);
ft3 = new TStrawModule(0.,200.,0.,32,1,5.05);

sg = new TStrawGeometry(ft1,0.004);
sg->AttachModule(ft2);
sg->AttachModule(ft3);
sg->Draw(10,20);
// Create a horizontal frame widget with buttons
TGHorizontalFrame *hframe = new TGHorizontalFrame(fMain,800,40);

TGLabel *fLabelFilePath = new TGLabel(fMain,"File full path: ");
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
fTextEntryFilePath->SetText("/home/pandastraws/hldFiles/asicSource/cosmics/1800V/cosmics1800_FOTRAC_PR_TF.root");
//fTextEntryFilePath->SetText("/home/pandastraws/hldFiles/asicSource/cosmics/1800V/cosmics1800_FOTRAC.root");
fTextEntryFilePath->MoveResize(304,520,550,22);

fTextEntryNtupleName = new TGTextEntry(fMain, new TGTextBuffer(14),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
fTextEntryNtupleName->SetMaxLength(64);
fTextEntryNtupleName->SetAlignment(kTextLeft);
fTextEntryNtupleName->Resize(93,fTextEntryFilePath->GetDefaultHeight());
fMain->AddFrame(fTextEntryNtupleName, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
fTextEntryNtupleName->SetText("FOTRAC_PR_TF");
fTextEntryNtupleName->MoveResize(804,520,150,22);



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
fileObj->Close();
fMain->Cleanup();
delete fMain;
  
}
void TStrawGui::DoDrawNext() 
{
 if(fileObj == 0) return;
 sg->Clear();
 TCanvas *fCanvas = fEcanvas->GetCanvas();
 eventNumberDisplay->SetNumber(eventNumberDisplay->GetNumber() + 1 );
 unsigned currEntry = 0; 
 unsigned currEvent = eventNumberDisplay->GetNumber()  ; //= ft_selector->GetCurrentEventNumber()+1;
 if(ft_tf_flag == 0)
 {
 	currEntry = ft_selector->GetCurrentEntryNumber();
 	if(currEntry > ft_selector->GetEntries())
		return;

 	ft_selector->GetEntry(currEntry);
 	while(currEvent >= ft_selector->globEvNum)
 	{
 		std::cout<<"=============>current event"<< currEvent<<"  selectorEvNum="<<ft_selector->globEvNum<<"   |current entry="<<currEntry<<std::endl;
 		ft_selector->Show(currEntry);
		sg->MarkDriftRadius(ft_selector->chNum, ft_selector->driftR); 
		currEntry++;
 		if(currEntry > ft_selector->GetEntries())
			break;
 		ft_selector->GetEntry(currEntry);
 	}
 	ft_selector->SetCurrentEntryNumber(currEntry);
 	//sg->DrawTrack(ft_tf_selector->a,ft_tf_selector->b);
 }
 else
 {
        currEntry = ft_tf_selector->GetCurrentEntryNumber();
        if(currEntry > ft_tf_selector->GetEntries())
                return;

        ft_tf_selector->GetEntry(currEntry);
        while(currEvent >= ft_tf_selector->globEvNum)
        {
                //std::cout<<"=============>current event"<< currEvent<<"  selectorEvNum="<<ft_tf_selector->globEvNum<<"   |current entry="<<currEntry<<std::endl;
                //ft_tf_selector->Show(currEntry);
		std::cout<<ft_tf_selector->X<<","<<ft_tf_selector->Z<<std::endl;
                sg->MarkDriftRadius(ft_tf_selector->chNum, ft_tf_selector->driftR);
                currEntry++;
                if(currEntry > ft_tf_selector->GetEntries())
                        break;
                ft_tf_selector->GetEntry(currEntry);
        }
	ft_tf_selector->Show(currEntry-1);//-1
        ft_tf_selector->SetCurrentEntryNumber(currEntry);
        sg->DrawTrack(ft_tf_selector->a,ft_tf_selector->b);
 }

 fCanvas->Modified();//refresh canvas
 fCanvas->Update();//refresh canvas
 return;
}

void TStrawGui::DoDrawPrev()         
{
 if(fileObj == 0) return;
 sg->Clear();
 TCanvas *fCanvas = fEcanvas->GetCanvas();
 eventNumberDisplay->SetNumber(eventNumberDisplay->GetNumber() - 1 );
 fCanvas->Update();//refresh canvas
 unsigned currEntry = ft_selector->GetCurrentEntryNumber();
 unsigned currEvent = eventNumberDisplay->GetNumber()  ; //= ft_selector->GetCurrentEventNumber()+1;

 if(ft_tf_flag == 0){
	 currEntry = ft_selector->GetCurrentEntryNumber();
	 if(currEntry < 1)
		return;
	
	 ft_selector->GetEntry(currEntry);
	 while(currEvent <= ft_selector->globEvNum)
	 {
	        std::cout<<"=============>current event"<< currEvent<<"  selectorEvNum="<<ft_selector->globEvNum<<"   |current entry="<<currEntry<<std::endl;
	        ft_selector->Show(currEntry);
		sg->MarkDriftRadius(ft_selector->chNum, ft_selector->driftR); 
	        currEntry--;
		if(currEntry<1)
			break;
	        ft_selector->GetEntry(currEntry);
	 }
	 ft_selector->SetCurrentEntryNumber(currEntry);
 } 
 else
 {
         currEntry = ft_tf_selector->GetCurrentEntryNumber();
         if(currEntry < 1)
                return;

         ft_tf_selector->GetEntry(currEntry);
         while(currEvent <= ft_tf_selector->globEvNum)
         {
                std::cout<<"=============>current event"<< currEvent<<"  selectorEvNum="<<ft_tf_selector->globEvNum<<"   |current entry="<<currEntry<<std::endl;
                ft_tf_selector->Show(currEntry);
                sg->MarkDriftRadius(ft_tf_selector->chNum, ft_tf_selector->driftR);
                currEntry--;
                if(currEntry<1)
                        break;
                ft_tf_selector->GetEntry(currEntry);
         }
         ft_tf_selector->SetCurrentEntryNumber(currEntry);
	 sg->DrawTrack(ft_tf_selector->a,ft_tf_selector->b);
 }

 fCanvas->Modified();//refresh canvas
 fCanvas->Update();//refresh canvas
 return;
}

void TStrawGui::InitSelector(){
TTree *tree_buff;
//** chacking which selector to create
std::string buffer = fTextEntryNtupleName->GetText();
 if(buffer.compare(buffer.size()-2,2,"TF") != 0){
	fileObj->GetObject(buffer.c_str(),tree_buff); //reading tree from buff
	ft_selector = new TFotracSelector(tree_buff);
	ft_tf_flag = 0;
 }
 else{
        fileObj->GetObject(buffer.c_str(),tree_buff); //reading tree from buff
        ft_tf_selector = new TFotractfSelector(tree_buff);
	ft_tf_flag = 1;
 }
	
}

void TStrawGui::LoadFile(){
//std::cerr<<"File loaded: "<<fTextEntryFilePath->GetText()<<std::endl;
fileName= fTextEntryFilePath->GetText();
fileObj = new TFile(fileName.c_str());
std::cerr<<"File loaded: "<<fileName<<std::endl;
InitSelector();
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
