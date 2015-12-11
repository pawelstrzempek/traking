#include "TStrawModule.cpp"
#include <utility>
#include <list>
#include <iostream>


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
//private:


};

void TStrawGeometry::AttachModule(TStrawModule *sm){
        modules.push_front(sm);
}

std::pair<double,double> TStrawGeometry::GetChannelCoordinance(unsigned channelNumber){
//** the assumption behid this metod is that first module contains number of straws form 0 to x, second module
//   contains x to y, third y to z and so on... Thanks to that and to the assumption that straw object creation
//   order is the same as channel numbering (fist straw object is channel number 1) we can shorten time needed
//   for finding the coordinance of given channel.

double x_crd = 0.0;
double y_crd = 0.0;

for (std::list<TStrawModule*>::iterator it = modules.begin(); it != modules.end(); it++){
	//std::cout << *it << ' ';
	if(channelNumber > ((*it)->GetNumberOfStraws()) ){
		channelNumber = channelNumber - (*it)->GetNumberOfStraws();
		continue;
	}
	x_crd = ((*it)->GetStrawXPos(channelNumber));
	y_crd = ((*it)->GetStrawYPos(channelNumber));
	//std::cout<<((*it)->GetStrawXPos(channelNumber))<<std::endl;
	//std::cout<<((*it)->GetStrawYPos(channelNumber))<<std::endl;
}

return std::make_pair(x_crd,y_crd);
}

