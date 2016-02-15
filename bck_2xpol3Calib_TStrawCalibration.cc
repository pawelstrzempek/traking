#include <TH2F.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TF1.h>
#include <TDirectory.h>
#include <TFile.h>
#include <iostream>

class TStrawCalibration{
public:
	 TH1F *calibHist;
	 TF1 *calibCurvePart1;
	 TF1 *calibCurvePart2;
	 TH1D *prj;
	 double t0;//beginning of the dt histogram 
	 double tk;//end of  the dt histogram
	 double p0Part1,p1Part1,p2Part1,p3Part1; //fitting parameters
	 double p0Part2,p1Part2,p2Part2,p3Part2; //fitting parameters
	 double range_min, range_mid, range_max; //** variables which defines the intervals for two fits

public:
	TStrawCalibration(int channelsQty, TH2F* dt_all, unsigned t0_param, unsigned tk_param, double r_tube, double r_wire);
	TF1* CalibrationCurveGenerate();
//	TH1F* CalibrationHistogramGenerate();	
	double DriftTimeToRadius(double);
	double GetT0(){return t0;}
	void SaveDiagnosticHistograms(TFile *);
	void SetP0(double);
private:
	double T0Calculate( TH2F* dt_all, unsigned  t0_param);	
	double TkCalculate( TH2F* dt_all, unsigned  tk_param);	

};
	TStrawCalibration::TStrawCalibration(int channelsQty, TH2F* dt_all,unsigned t0_param, unsigned tk_param,double r_tube, double r_wire){
		std::cout<<"Try to create obj\n";
        	//TH2F *dt_all = (TH2F*)in_file->Get("dt_barcode");
		calibHist = new TH1F("r_t_calibration","r_t_calibration",2000,0,200);
        	prj = dt_all-> ProjectionX();
		std::cout<<"Projection successful\n";
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
               	 calibHist->Fill(index*binning,(prj->Integral(startBin,step)/integralTotal)*(r_tube-r_wire)+r_wire);
//             	 calibration->Fill(step,(prj->Integral(0,step)/integralTotal)*(RTUBE-RWIRE)+RWIRE);
               	 index++;
        	}
//		std::cout<<"Generate calibration curve\n";
		CalibrationCurveGenerate();
//		std::cout<<"t0 calculate\n";
		t0 = T0Calculate(dt_all,t0_param);
//		std::cout<<"tk calculate\n";
		tk = TkCalculate(dt_all,tk_param);
	}


	TF1* TStrawCalibration::CalibrationCurveGenerate(){
	//** two pol3 fits would be better 
		range_min = 0.;
		range_mid = calibHist->GetBinCenter(calibHist->GetNbinsX()/5.);
		range_max = calibHist->GetBinCenter(calibHist->GetNbinsX());
		std::cout<<"Setting ranges to: "<<range_min<<" "<<range_mid<<" "<<range_max<<std::endl;
		
		std::cout<<"Fitting pol3 to two different intervals\n";
		//calibCurvePart1 = new TF1("calibCurvePart1", "pol3", 0, (int)(calibHist->GetNbinsX()/2.));
		//calibCurvePart2 = new TF1("calibCurvePart2", "pol3",  (int)(calibHist->GetNbinsX()/2.), (int)(calibHist->GetNbinsX()));
		calibCurvePart1 = new TF1("calibCurvePart1", "pol3", range_min, range_mid);
		calibCurvePart2 = new TF1("calibCurvePart2", "pol3", range_mid, range_max);
		calibHist->Fit("calibCurvePart1", "R");
		calibHist->Fit("calibCurvePart2", "R");

		//std::cout<<"=====> przedzial:"<<(int)(calibHist->GetNbinsX()/2.)<<std::endl<<std::endl;

		//TF1 *fitFunction = calibHist->GetFunction("r_t_calibrationCurve");
		//calibCurve = calibHist->GetFunction("pol3");//fitFunction;


		std::cout<<"Gettig parameters\n";
		this->p0Part1 = calibCurvePart1->GetParameter(0);
		this->p1Part1 = calibCurvePart1->GetParameter(1);
		this->p2Part1 = calibCurvePart1->GetParameter(2);
		this->p3Part1 = calibCurvePart1->GetParameter(3);

		this->p0Part2 = calibCurvePart2->GetParameter(0);
		this->p1Part2 = calibCurvePart2->GetParameter(1);
		this->p2Part2 = calibCurvePart2->GetParameter(2);
		this->p3Part2 = calibCurvePart2->GetParameter(3);
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

        double TStrawCalibration::TkCalculate( TH2F* dt_all, unsigned  tk_param)
        {
                //TH1F *calibration = new TH1F("r_t_calibration","r_t_calibration",2000,0,200);
                //TH2F* dt_all = (TH2F*)in_file->Get("dt_barcode");
                TH1D * prj = dt_all-> ProjectionX();

                int binsX = prj->GetNbinsX(); //get number of bins
                double integralTotal = prj->Integral(0,binsX);
                int stopBin = 0;
                for(int step = 0 ; step < binsX ; step++)
                {
                 if(prj->Integral(0,step) >= integralTotal*0.01*tk_param){
                         stopBin = step;
			 break;
		 }
                }
        tk =  prj->GetBinCenter(stopBin);
        }




	double TStrawCalibration::DriftTimeToRadius(double dt_in){
		//** we substract calculated t0 from each drift time and then convert it to the drift radius
		//std::cout<<"dt_in="<<dt_in<<"  t0="<<t0<<std::endl;
		dt_in = dt_in - t0;
		//** it may happen that dt_in-t0<0. Because the t0 is calculated for dt histogram (for all channels) which
		//** is cut at the beginning and at the end accoridng to t0_param and tk_param
		if(dt_in < 0) 
			return 0.;
		if(dt_in > (tk-t0))
			return (p0Part2+p1Part2*(tk-t0)+p2Part2*(tk-t0)*(tk-t0)+p3Part2*(tk-t0)*(tk-t0)*(tk-t0));
		//std::cout<<"p0="<<this->p0<<"  p1="<<this->p1<<"  p2="<<this->p2<<"  p3="<<this->p3<<std::endl;
		//std::cout<<"t0="<<t0<<"  tk="<<tk<<"  dt_in_org="<<dt_in_org<<"  dt_in="<<dt_in<<std::endl;
		if(dt_in <= range_mid)
			return (p0Part1+p1Part1*dt_in+p2Part1*dt_in*dt_in+p3Part1*dt_in*dt_in*dt_in);
		else
			return (p0Part2+p1Part2*dt_in+p2Part2*dt_in*dt_in+p3Part2*dt_in*dt_in*dt_in);
	}

	void TStrawCalibration::SaveDiagnosticHistograms(TFile *f){
	f->cd();
	TDirectory *cdcalibration = f->mkdir("calibration");
	cdcalibration->cd();
        prj->Write();
	calibHist->Write();
        calibCurvePart1->Write();
        calibCurvePart2->Write();
	
	f->cd();
	std::cout<<"Histograms saved\n";
	}

	void TStrawCalibration::SetP0(double param){
	p0Part1= param;
	}

	










