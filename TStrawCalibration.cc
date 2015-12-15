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
	 TF1 *calibCurve;
	 TH1D *prj;
	 double t0;//beginning of the dt histogram 
	 double tk;//end of  the dt histogram
	 double p0,p1,p2,p3; //fitting parameters
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
		std::cout<<"Fitting\n";
		calibHist->Fit("pol3");
		std::cout<<"Getting function\n";
		//TF1 *fitFunction = calibHist->GetFunction("r_t_calibrationCurve");
		calibCurve = calibHist->GetFunction("pol3");//fitFunction;
		std::cout<<"Gettig parameters\n";
		this->p0 = calibCurve->GetParameter(0);
		this->p1 = calibCurve->GetParameter(1);
		this->p2 = calibCurve->GetParameter(2);
		this->p3 = calibCurve->GetParameter(3);
		std::cout<<p0<<"\n"<<p1<<"\n"<<p2<<"\n"<<p3<<"\n";
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
			//dt_in = 0; 
			return 0.;
		if(dt_in > (tk-t0))
			return (p0+p1*(tk-t0)+p2*(tk-t0)*(tk-t0)+p3*(tk-t0)*(tk-t0)*(tk-t0));
			//dt_in = tk;
		//std::cout<<"p0="<<this->p0<<"  p1="<<this->p1<<"  p2="<<this->p2<<"  p3="<<this->p3<<std::endl;
		//std::cout<<"t0="<<t0<<"  tk="<<tk<<"  dt_in_org="<<dt_in_org<<"  dt_in="<<dt_in<<std::endl;
		return (p0+p1*dt_in+p2*dt_in*dt_in+p3*dt_in*dt_in*dt_in);
	}

	void TStrawCalibration::SaveDiagnosticHistograms(TFile *f){
	f->cd();
	TDirectory *cdcalibration = f->mkdir("calibration");
	cdcalibration->cd();
        prj->Write();
	calibHist->Write();
        calibCurve->Write();
	
	f->cd();
	std::cout<<"Histograms saved\n";
	}

	void TStrawCalibration::SetP0(double param){
	p0= param;
	}

	










