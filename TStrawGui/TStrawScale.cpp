#include <TText.h>
#include <TLine.h>


class TStrawScale{


double pos_x;
double pos_y;
double strawRadius;
TText *t;
TLine *l;
public:
TStrawScale(double xx, double yy, double sR):pos_x(xx), pos_y(yy), strawRadius(sR){
	t = new TText(pos_x, pos_y+0.01, "Scale 10.1 [mm]");
	l = new TLine(pos_x, pos_y, pos_x+2*strawRadius, pos_y);
	t->SetTextSize(strawRadius);
}

void Draw(){
	t->Draw();
	l->Draw();
}



};
