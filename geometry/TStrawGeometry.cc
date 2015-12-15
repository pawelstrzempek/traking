#include "TStrawModule.cc"
#include <utility>
#include <list>
#include <iostream>
#include <TCanvas.h>
#include <TFile.h>

class TStrawGeometry{
std::list<TStrawModule*> modules;

public:
        TStrawGeometry(TStrawModule *sm){
        modules.push_front(sm);
	}
        ~TStrawGeometry(){
        }
	std::pair<double,double> GetChannelCoordinance(unsigned channelNumber);
	void AttachModule(TStrawModule *);
	void SaveGeometry(TFile *f, double scale_factor = 0.004);
//private:


};

void TStrawGeometry::AttachModule(TStrawModule *sm){
        modules.push_back(sm);
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

void TStrawGeometry::SaveGeometry(TFile *f, double scale_factor){
TCanvas *can = new TCanvas("StrawGeometry","Straw geometry",800,800);

for (std::list<TStrawModule*>::iterator it = modules.begin(); it != modules.end(); it++){
	(*it)->Draw(scale_factor);
}
f->cd();
can->Write();
return;
}






