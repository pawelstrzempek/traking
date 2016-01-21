#include "TStrawModule.cc"
#include <utility>
#include <list>
#include <iostream>
#include <TCanvas.h>
#include <TFile.h>
#include <TLine.h>

class TStrawGeometry{
std::list<TStrawModule*> modules;
double scale_factor;
unsigned straws_qty;
double x_shift;
double y_shift;
TLine *line[2]; 
public:
	

//        TStrawGeometry(TStrawModule *sm, double sf=1.): scale_factor(sf){
  //      modules.push_front(sm);
//	}
        TStrawGeometry(TStrawModule *sm, double sf){
         modules.push_front(sm);
	 scale_factor = sf;
	 straws_qty = sm->GetNumberOfStraws();
	 line[0] = new TLine(0,0,1,1);
	 line[1] = new TLine(0,0,1,1);
	 line[1]->SetLineColor(2);
	 x_shift = 0;
	 y_shift = 0;
	}


        ~TStrawGeometry(){
        }
	std::pair<double,double> GetChannelCoordinance(unsigned channelNumber);
	void AttachModule(TStrawModule *);
	void SaveGeometry(TFile *f);//, double scale_factor = 0.004);
	void Draw(double x_shift, double y_shift);//, double scale_factor);
	void MarkDriftRadius(unsigned channel_number, double dr);
	void Clear();
	void DrawTrack(double a, double b, unsigned select);

};

void TStrawGeometry::AttachModule(TStrawModule *sm){
        modules.push_back(sm);
	straws_qty += sm->GetNumberOfStraws();
}

std::pair<double,double> TStrawGeometry::GetChannelCoordinance(unsigned channelNumber){
//** the assumption behid this metod is that first module contains number of straws form 0 to x, second module
//   contains x to y, third y to z and so on... Thanks to that and to the assumption that straw object creation
//   order is the same as channel numbering (fist straw object is channel number 1) we can shorten time needed
//   for finding the coordinance of given channel.

double x_crd = 0.0;
double y_crd = 0.0;

//std::cout<<"Looking for channel "<<channelNumber<<std::endl;

for (std::list<TStrawModule*>::iterator it = modules.begin(); it != modules.end(); it++){
//	std::cout << "Getting channel:"<<channelNumber<<std::endl;
	if(channelNumber > ((*it)->GetNumberOfStraws()) ){
		channelNumber = channelNumber - (*it)->GetNumberOfStraws();
		continue;
	}
	x_crd = ((*it)->GetStrawXPos(channelNumber-1));
	y_crd = ((*it)->GetStrawYPos(channelNumber-1));
	break;
//	std::cout << "Checking channel:"<<channelNumber<<"  y="<<y_crd<<std::endl;
	//std::cout<<((*it)->GetStrawXPos(channelNumber))<<std::endl;
	//std::cout<<((*it)->GetStrawYPos(channelNumber))<<std::endl;
}

return std::make_pair(x_crd,y_crd);
}

//void TStrawGeometry::SaveGeometry(TFile *f, double scale_factor){
void TStrawGeometry::SaveGeometry(TFile *f){
TCanvas *can = new TCanvas("StrawGeometry","Straw geometry",800,800);

for (std::list<TStrawModule*>::iterator it = modules.begin(); it != modules.end(); it++){
	(*it)->Draw(0.,0.,scale_factor);
}
f->cd();
can->Write();
return;
}


//void TStrawGeometry::Draw(double x_shift, double y_shift, double scale_factor){
void TStrawGeometry::Draw(double x_shift_in, double y_shift_in){
 x_shift = x_shift_in;
 y_shift = y_shift_in;
 for (std::list<TStrawModule*>::iterator it = modules.begin(); it != modules.end(); it++){
         (*it)->Draw(x_shift, y_shift,scale_factor);
 }
}

void TStrawGeometry::MarkDriftRadius(unsigned channel_number, double dr){
 for (std::list<TStrawModule*>::iterator it = modules.begin(); it != modules.end(); it++){
//      std::cout << "Getting channel:"<<channelNumber<<std::endl;
        if(channel_number > ((*it)->GetNumberOfStraws()) ){
                channel_number = channel_number - (*it)->GetNumberOfStraws();
                continue;
        }
        (*it)->MarkStrawDr(channel_number-1, (this->scale_factor)*dr );
        break;
 }
return;
}


void TStrawGeometry::Clear(){
 for(unsigned ch =1; ch < (straws_qty+1) ; ch++){
 MarkDriftRadius(ch,0);
 }


}

void TStrawGeometry::DrawTrack(double a, double b, unsigned select){
if(select != 0 && select != 1)
	return;
//!! TODO: make it more generic
//line = new TLine(0*scale_factor,a*scale_factor*0+b,20*scale_factor,20*scale_factor*a+b);
line[select]->SetX1(((y_shift-b)/(a) + x_shift)*scale_factor);
line[select]->SetY1(y_shift*scale_factor);
line[select]->SetX2(((200+y_shift-b)/(a) + x_shift)*scale_factor);
line[select]->SetY2((200+y_shift)*scale_factor);
//std::cout<<"\nx1="<<(-b/a)*scale_factor<<" y1=0 x2="<<((200-b)/a)*scale_factor<<" y2="<<200*scale_factor<<"===========\n";
std::cout<<"\nx1="<<line[select]->GetX1()<<" y1="<<line[select]->GetY1()<<" x2="<<line[select]->GetX2()<<" y2="<<line[select]->GetY2()<<"===========\n";

line[select]->Draw();
return;
}
