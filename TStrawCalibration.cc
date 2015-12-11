#include <TH2F.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TF1.h>


class TStrawCalibration{
	 TH1F *calibHist;
	 TF1 *calibCurve;
	 double t0;
	 double p0,p1,p2,p3;
public:
	TStrawCalibration(int channelsQty, TH2F* dt_all, unsigned t0_param, unsigned tk_param, double r_tube, double r_wire);
	TF1* CalibrationCurveGenerate();
//	TH1F* CalibrationHistogramGenerate();	
	double GetT0(){return t0;}
private:
	double T0Calculate( TH2F* dt_all, unsigned  t0_param);	

};
	TStrawCalibration::TStrawCalibration(int channelsQty, TH2F* dt_all,unsigned t0_param, unsigned tk_param,double r_tube, double r_wire){
		TH1F *calibHist = new TH1F("r_t_calibration","r_t_calibration",2000,0,200);
        	//TH2F *dt_all = (TH2F*)in_file->Get("dt_barcode");
        	TH1D *prj = dt_all-> ProjectionX();

	        int binsX = prj->GetNbinsX(); //get number of bins
	        double integralTotal = prj->Integral(0,binsX);
	        int startBin = 0, stopBin = 0;
	        double frontCutIntegral =0.0;
	        double backCutIntegral =0.0;
	        //**** looking for the start and stop bin of the histogram
       		for(int step = 0 ; step < binsX ; step++)
        	{
               	 if(prj->Integral(0,step) <= integralTotal*0.01*t0_param){
               	         startBin = step;
               	         frontCutIntegral = prj->Integral(0,step);
                 }
               	 if(prj->Integral(0,step) >= integralTotal *0.01*tk_param){
               	         stopBin = step;
               	         backCutIntegral = integralTotal - prj->Integral(0,step);
               	         break;
               	 }
        	}
        	float binning = ((prj->GetXaxis()->GetXmax()) - (prj->GetXaxis()->GetXmin()))/(prj->GetNbinsX());
        	integralTotal = integralTotal -  frontCutIntegral - backCutIntegral;
        	int index =0;
        	for(int step = startBin ; step < stopBin ; step++)
        	{
//             	 std::cout<<"step= "<<step<<"  index="<<index<<"  integral="<<prj->Integral(0,step)<<"  integralTotal="<<integralTotal<<std::endl;
               	 calibHist->Fill(index*binning,(prj->Integral(startBin,step)/integralTotal)*(r_tube,-r_wire)+r_wire);
//             	 calibration->Fill(step,(prj->Integral(0,step)/integralTotal)*(RTUBE-RWIRE)+RWIRE);
               	 index++;
        	}
	CalibrationCurveGenerate();
	t0 = T0Calculate(dt_all,t0_param);
	}


	TF1* TStrawCalibration::CalibrationCurveGenerate(){
	//** two pol3 fits would be better 
		calibHist->Fit("pol3");
		//TF1 *fitFunction = calibHist->GetFunction("r_t_calibrationCurve");
		calibCurve = calibHist->GetFunction("pol3");//fitFunction;
		p0 = calibCurve->GetParameter(0);
		p1 = calibCurve->GetParameter(1);
		p2 = calibCurve->GetParameter(2);
		p3 = calibCurve->GetParameter(3);
	}


	double TStrawCalibration::T0Calculate( TH2F* dt_all, unsigned  t0_param)
	{
	        //TH1F *calibration = new TH1F("r_t_calibration","r_t_calibration",2000,0,200);
	        //TH2F* dt_all = (TH2F*)in_file->Get("dt_barcode");
	        TH1D * prj = dt_all-> ProjectionX();

	        int binsX = prj->GetNbinsX(); //get number of bins
	        double integralTotal = prj->Integral(0,binsX);
	        int startBin = 0;
	        for(int step = 0 ; step < binsX ; step++)
      		{
       	         if(prj->Integral(0,step) <= integralTotal*0.01*t0_param)
       	                 startBin = step;
	        }
	t0 =  prj->GetBinCenter(startBin);
	}


