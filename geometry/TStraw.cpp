#include <TEllipse.h>
#include <TText.h>
#include <sstream>
#include <string>


class TStraw{
	double x_pos;
	double y_pos;
	double radius;

	unsigned int id;

	long int value;

	TEllipse * shape;
	TEllipse * hitShape;
	TText * lable;
public:
	static unsigned int number;
	TStraw(double x, double y, double r):x_pos(x), y_pos(y),radius(r){
		std::ostringstream s;
		s << number;
		shape = new TEllipse(x_pos, y_pos, radius, radius);
		hitShape = new TEllipse(x_pos, y_pos, 0.0, 0.0);
		hitShape->SetLineColor(2); 
		lable = new TText(x_pos, y_pos, s.str().c_str());
		id = number;
		number++;
		lable = new TText(x_pos-(radius/2),y_pos-(radius/2),(s.str()).c_str() );		
		lable -> SetTextSize(radius);
	}
	~TStraw(){
		number--;
	}
	void Draw();
	void Mark(int);	
	void Mark();	
	void SetX1(double);	
	void SetY1(double);	
	void SetId(unsigned int);
	double GetX();
	double GetY();
	unsigned int GetId();
	void updateDt(double);
private:
	double dtToRadius(double);


};
unsigned int  TStraw::number = 1;


void TStraw::Mark(int color ){
	shape->SetFillColor(color);
}
void TStraw::Mark(){
	shape->SetFillColor(9);
}

void TStraw::Draw(){
	shape->Draw();
	hitShape->Draw();
	lable->Draw();	
}




void TStraw::SetX1(double new_x){
	shape->SetX1(new_x);
	hitShape->SetX1(new_x);
	lable->SetX(new_x-(radius/2));
	x_pos = new_x;
}

void TStraw::SetY1(double new_y){
	shape->SetY1(new_y);
	hitShape->SetY1(new_y);
	lable->SetY(new_y-(radius/2));
	y_pos = new_y;

}

void TStraw::SetId(unsigned int i){
	id = i;
}

unsigned int TStraw::GetId(){
	return id;
}

void TStraw::updateDt(double dt){
	hitShape->SetR1(dtToRadius(dt));
	hitShape->SetR2(dtToRadius(dt));
}

double TStraw::dtToRadius(double dt){
	return (dt*radius)/150.;
}

double TStraw::GetY(){
return y_pos;
}

double TStraw::GetX(){
return x_pos;
}
