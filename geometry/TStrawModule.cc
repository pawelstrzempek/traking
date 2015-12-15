#ifndef TStrawModule_cc
#define TStrawModule_cc


#include <vector>
#include <TEllipse.h>
#include "TStraw.cc"
#include <TMath.h>
#include <string>
#include <iostream>
class TStraw;


//namespace Constanses{
//	float sqrt3 = 1.7320508;
//}

class TStrawModule {
	std::vector<TStraw *> straws;
	unsigned int number_of_double_layers;
	unsigned int straws_in_double_layer;
	double x_pos;
	double y_pos;
	double angle_rot; 

//	double scaleFactor;//this factor scales down all dimentions so they can fit into TCanvas. For
			   //the canvas the coordinates are from 0-1, 0 - 0% of width and 1 is 100%.
	double radius;
	unsigned int straws_qty;
	unsigned hit_qty;
public:
	TStrawModule(double x_coordinate, double y_coordinate, double angle_rotation, unsigned int straws_in_double_layer, unsigned int number_of_double_layers, double straw_radius);
	TStrawModule( std::string fileName);
	void Draw(double scale_factor);
	void MarkStraw(unsigned);
	void MarkStraw(unsigned, int);
        void MarkStrawDt(unsigned , double );
	void Clear();
	void ClearDt();
	double GetStrawRadius();
	unsigned GetMultiplicity();
	double GetStrawXPos(unsigned);
	double GetStrawYPos(unsigned);
	unsigned GetNumberOfStraws();
};

TStrawModule::TStrawModule(double x_coordinate, double y_coordinate, double angle_rotation, unsigned int straws_in_double_layer, unsigned int number_of_double_layers, double straw_radius): x_pos(x_coordinate), y_pos(y_coordinate), angle_rot(angle_rotation), straws_in_double_layer(straws_in_double_layer), number_of_double_layers(number_of_double_layers), radius(straw_radius) {
	double sqrt3 = 1.732050808;
//        straws.resize(cols*raws);
	hit_qty = 0;
//	radius = 0.;
//	cols > raws ? radius=0.45/cols : radius = 0.45/raws; //we calculated maximal radius which can fit into canvas
	//radius = 0.02;//5.05;

	//unsigned int straw_iter = 0;
	double x_current = x_pos;
	double y_current = y_pos;

        for (unsigned int y =0 ; y < number_of_double_layers; y++){//we create two raws at once
        	for (unsigned int x = 0 ; x < straws_in_double_layer ; x++){
			if(angle_rotation == 0){
				straws.push_back(new TStraw(x_current,y_current,radius));
			}
			else
			{
			//** implementation of the rotation accorfing to :
				//x' = x*cosB - y*sinB
				//y' = y*sinB + y*cosB //where B i rotation angle
			        straws.push_back(new TStraw(x_current*TMath::Cos(angle_rotation*TMath::DegToRad()) - y_current*TMath::Sin(angle_rotation*TMath::DegToRad()),
							    x_current*TMath::Sin(angle_rotation*TMath::DegToRad()) + y_current*TMath::Cos(angle_rotation*TMath::DegToRad()),radius));
			//	std::cout<<"x="<<x_current<<"  x'="<<x_current*TMath::Cos(angle_rotation*TMath::DegToRad()) - y_current*TMath::Sin(angle_rotation*TMath::DegToRad())<<std::endl;
			//	std::cout<<"y="<<y_current<<"  y'="<<x_current*TMath::Sin(angle_rotation*TMath::DegToRad()) + y_current*TMath::Cos(angle_rotation*TMath::DegToRad())<<std::endl;
			}
			//** setting coordinance for the next straw	
			x_current = x_current + radius;
			(x+1)%2 == 0 ? y_current = y_pos+(2*sqrt3*radius)*(y) : y_current = y_pos+(2*sqrt3*radius)*(y) - sqrt3*radius;
			//straw_iter++;
        	}
	x_current=x_pos;
	y_current=y_pos+(2*sqrt3*radius)*(y+1);
	}
	straws_qty = straws_in_double_layer*number_of_double_layers;

//        for (unsigned int x = 0 ; x < cols ; x++){//all straws are created with zero position.
  //              for (unsigned int y =0 ; y < raws; y++){
   //                     straws.push_back(new TStraw(0,0,radius));
     //           }
	//std::cout<<straws.size()<<std::endl;
	//}
}


TStrawModule::TStrawModule( std::string fileName){
//** this constructor is taking file name as an argument. The file shall contain the channels mapping with their x,y coordinance



}

void TStrawModule::Draw(double scale_factor){
	
float sqrt3 = 1.7320508;
//	float x_step = 0.9/
//	float y_step

	
	//float x_pos = 0.05 + radius;
	//float y_pos = 0.05 + radius;
	//float x_pos = startPointX;
	//float y_pos = startPointY + 2*radius;


	//unsigned int straw_iter = 0;
/*	for (unsigned int straw_iter = 0; straw_iter< raws*cols; straw_iter++){
                        straws[straw_iter] -> SetX1(x_pos);
                        straws[straw_iter] -> SetY1(y_pos);
			x_pos = x_pos + radius;
			//straw_iter%2 != 0 ? y_pos = y_pos + Constanses::sqrt3*radius : y_pos = y_pos - Constanses::sqrt3*radius;
			straw_iter%2 != 0 ? y_pos = y_pos + sqrt3*radius : y_pos = y_pos - sqrt3*radius;
			straws[straw_iter]->Draw();
	}
*/
//std::cout<<"EnterLoop\n"<<"straws_in_double_layer="<<straws_in_double_layer<<"\nnumber_of_double_layers"<<number_of_double_layers<<std::endl;
	for (unsigned int straw_iter = 0; straw_iter< straws_qty; straw_iter++){
	straws[straw_iter]->Draw(scale_factor);
	//std::cout<<straw_iter<<std::endl;
	}

/*	for (unsigned int x = 0 ; x < cols ; x++){
		for (unsigned int y =0 ; y < raws; y++){
			//straws.push_back(new TStraw(x_pos,y_pos,radius));
			straws[straw_iter] -> SetX1(x_pos);
			straws[straw_iter] -> SetY1(y_pos);
			x_pos = x_pos + 2*radius;
//			straws.back()->Draw();
			straws[straw_iter]->Draw();
			straw_iter++;
		}
		y_pos = y_pos + Constanses::sqrt3*radius;
		x%2 != 0 ? x_pos = 0.05+2*radius : x_pos = 0.05+radius;
		//x_pos = 0.05 +radius;
	}
*/	

}

void TStrawModule::MarkStraw(unsigned st_number, int color){
	if(st_number <= straws.size()){
		hit_qty++;
		straws[st_number]->Mark(color);	
	}
}

void TStrawModule::MarkStraw(unsigned  st_number){
	if(st_number <= straws.size()){
		hit_qty++;
		straws[st_number]->Mark();
	}	
}

void TStrawModule::MarkStrawDt(unsigned st_number, double dt){
	if(st_number <= straws.size()){
		hit_qty++;
        	straws[st_number]->updateDt(dt);
	}
}

void TStrawModule::Clear(){
	for (std::vector<TStraw *>::iterator it = straws.begin() ; it != straws.end(); ++it)
    	(*it)->Mark(0);	
	//straws[st_number]->Mark();	
	hit_qty=0;
}

void TStrawModule::ClearDt(){
	for (std::vector<TStraw *>::iterator it = straws.begin() ; it != straws.end(); ++it)
    	(*it)->updateDt(0);	
	//straws[st_number]->Mark();	
	hit_qty=0;
}

double TStrawModule::GetStrawRadius(){
	return radius;
}

unsigned TStrawModule::GetMultiplicity(){
	return hit_qty;
}

double TStrawModule::GetStrawXPos(unsigned channel){
//** the assumption that straw object creation
//   order is the same as channel numbering (fist straw object is channel number 1)
	if(channel> straws_qty)
		return -1.;
	else
		return (straws.at(channel))->GetX();
}

double TStrawModule::GetStrawYPos(unsigned channel){
//** the assumption that straw object creation
//   order is the same as channel numbering (fist straw object is channel number 1)
        if(channel> straws_qty)
                return -1.;
        else
		return (straws.at(channel))->GetY();
}

unsigned TStrawModule::GetNumberOfStraws(){
return straws_qty;
}
#endif
