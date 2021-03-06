#include <TH2F.h>
#include <TH1F.h>
#include <TH1D.h>
#include <TF1.h>
#include <TDirectory.h>
#include <TFile.h>
#include <iostream>
#include <vector>
#include <algorithm> //sort method
#include <TVectorD.h>



class TStrawCalibration{
public:
	 TH1F *calibHist;
	 TF1 *calibCurve;
	 //TF1 *calibCurvePart2;
	 TH1D *prj;

	 double t0;//beginning of the dt histogram 
	 double tk;//end of  the dt histogram
//	 double p0Part1,p1Part1,p2Part1,p3Part1; //fitting parameters
	 double p0,p1,p2,p3,p4,p5,p6,p7; //fitting parameters
	 double range_min, range_mid, range_max; //** variables which defines the intervals for two fits

public:
	TStrawCalibration(){};
	TStrawCalibration(int channelsQty, TH2F* dt_all, unsigned t0_param, unsigned tk_param, double r_tube, double r_wire);
	TF1* CalibrationCurveGenerate();
//	TH1F* CalibrationHistogramGenerate();	
	double DriftTimeToRadius(double);
	double GetT0(){return t0;}
	void SaveDiagnosticHistograms(TFile *);
	void SetP0(double);
	std::vector<double> T0OffsetsCalculate(TH2F* dt_all, unsigned t0_param, unsigned channels);
private:
	double T0Calculate( TH1D * prj, unsigned  t0_param);	
	double TkCalculate( TH1D * prj, unsigned  tk_param);	
	double Median(std::vector<double> scores);
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
		TH1D * prj = dt_all-> ProjectionX();                
		t0 = T0Calculate(prj,t0_param);
//		std::cout<<"tk calculate\n";
		tk = TkCalculate(prj,tk_param);
	}

	std::vector<double> TStrawCalibration::T0OffsetsCalculate(TH2F* dt_all, unsigned t0_param, unsigned channels){
	//** This methods calulates offsets between the drift time begins. The
	//** offsets are returned in the vector object.
		TH1D *h1;
		std::vector<double> output;
		std::vector<double> buf;
		for (unsigned i = 1 ; i < channels+1 ; i++ )
		{
	//		std::cout<<i<<std::endl;
       	 		h1 = dt_all->ProjectionX("tmpName",i,i);
       	 		buf.push_back(T0Calculate(h1,t0_param));
		}
		double median_t0  = TStrawCalibration::Median(buf);//we calculate median of all t0's
		for (unsigned i = 0 ; i < channels ; i++ )
                {
		 output.push_back(median_t0 - buf[i]);
		std::cout<<"channel:"<<i<<" median:"<<median_t0<<" t0:"<<buf[i]<<" shift:"<<(median_t0 - buf[i])<<std::endl;
		}
		return output;
	}

	TF1* TStrawCalibration::CalibrationCurveGenerate(){
	//** two pol3 fits would be better 
		range_min = 0.;
	//	range_mid = calibHist->GetBinCenter(calibHist->GetNbinsX()/5.);
		range_max = calibHist->GetBinCenter(calibHist->GetNbinsX());
	//	std::cout<<"Setting ranges to: "<<range_min<<" "<<range_mid<<" "<<range_max<<std::endl;
		
	//	std::cout<<"Fitting pol3 to two different intervals\n";
		//calibCurvePart1 = new TF1("calibCurvePart1", "pol3", 0, (int)(calibHist->GetNbinsX()/2.));
		//calibCurvePart2 = new TF1("calibCurvePart2", "pol3",  (int)(calibHist->GetNbinsX()/2.), (int)(calibHist->GetNbinsX()));
		
		//calibCurve = new TF1("calibCurve","[0]+[1]*x+[2]*(2*x*x-1)+[3]*(4*x*x*x-3*x)+[4]*(8*x*x*x*x-8*x*x+1)+[5]*(16*x*x*x*x*x-20*x*x*x+5*x)",range_min,range_max);
		//calibCurve = new TF1("calibCurve","[0]+[1]*x+[2]*(2*x*x-1)+[3]*(4*x*x*x-3*x)+[4]*(8*x*x*x*x-8*x*x+1)+[5]*(16*x*x*x*x*x-20*x*x*x+5*x)",-10,500);
		calibCurve = new TF1("calibCurve","pol7",-10,500);


	//	calibCurvePart1 = new TF1("calibCurvePart1", "pol3", range_min, range_mid);
	//	calibCurvePart2 = new TF1("calibCurvePart2", "pol3", range_mid, range_max);
	//	calibHist->Fit("calibCurvePart1", "R");
		calibHist->Fit("calibCurve", "R");

		//std::cout<<"=====> przedzial:"<<(int)(calibHist->GetNbinsX()/2.)<<std::endl<<std::endl;

		//TF1 *fitFunction = calibHist->GetFunction("r_t_calibrationCurve");
		//calibCurve = calibHist->GetFunction("pol3");//fitFunction;


		std::cout<<"Gettig parameters\n";
		this->p0 = calibCurve->GetParameter(0);
		this->p1 = calibCurve->GetParameter(1);
		this->p2 = calibCurve->GetParameter(2);
		this->p3 = calibCurve->GetParameter(3);
		this->p4 = calibCurve->GetParameter(4);
		this->p5 = calibCurve->GetParameter(5);
		this->p6 = calibCurve->GetParameter(6);
		this->p7 = calibCurve->GetParameter(7);
		std::cout<<"p0="<<p0<<" p1="<<p1<<" p2="<<p2<<" p3="<<p3<<" p4="<<p4<<" p5="<<p5<<std::endl;


/*
		this->p0Part2 = calibCurvePart2->GetParameter(0);
		this->p1Part2 = calibCurvePart2->GetParameter(1);
		this->p2Part2 = calibCurvePart2->GetParameter(2);
		this->p3Part2 = calibCurvePart2->GetParameter(3);
*/	}


//	double TStrawCalibration::T0Calculate( TH2F* dt_all, unsigned  t0_param)
	double TStrawCalibration::T0Calculate( TH1D * prj, unsigned  t0_param)
	{
	        //TH1F *calibration = new TH1F("r_t_calibration","r_t_calibration",2000,0,200);
	        //TH2F* dt_all = (TH2F*)in_file->Get("dt_barcode");
//	        TH1D * prj = dt_all-> ProjectionX();

	        int binsX = prj->GetNbinsX(); //get number of bins
	        double integralTotal = prj->Integral(0,binsX);
	        int startBin = 0;
	        for(int step = 0 ; step < binsX ; step++)
      		{
       	         if(prj->Integral(0,step) <= integralTotal*0.01*t0_param)
       	                 startBin = step;
	        }
	//t0 =  prj->GetBinCenter(startBin);
	return prj->GetBinCenter(startBin);
	}

//        double TStrawCalibration::TkCalculate( TH2F* dt_all, unsigned  tk_param)
        double TStrawCalibration::TkCalculate( TH1D * prj, unsigned  tk_param)
        {
                //TH1F *calibration = new TH1F("r_t_calibration","r_t_calibration",2000,0,200);
                //TH2F* dt_all = (TH2F*)in_file->Get("dt_barcode");
//                TH1D * prj = dt_all-> ProjectionX();

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
        //tk =  prj->GetBinCenter(stopBin);
	return prj->GetBinCenter(stopBin);
        }




	double TStrawCalibration::DriftTimeToRadius(double dt_in){
		//** we substract calculated t0 from each drift time and then convert it to the drift radius
		//std::cout<<"dt_in="<<dt_in<<"  t0="<<t0<<std::endl;
		dt_in = dt_in - t0;
		double x = dt_in;
		double r = -1.;
		//** it may happen that dt_in-t0<0. Because the t0 is calculated for dt histogram (for all channels) which
		//** is cut at the beginning and at the end accoridng to t0_param and tk_param
		if(dt_in < 0){ 
			r = 0.;
		}
		else if(dt_in > (tk-t0)){
			//return (p0Part2+p1Part2*(tk-t0)+p2Part2*(tk-t0)*(tk-t0)+p3Part2*(tk-t0)*(tk-t0)*(tk-t0));
			x = tk-t0;
			r = p0 + p1*x + p2*x*x + p3*x*x*x + p4*x*x*x*x + p5*x*x*x*x*x + p6*x*x*x*x*x*x + p7*x*x*x*x*x*x*x;
			//return p0+p1*x+p2*(2*x*x-1)+p3*(4*x*x*x-3*x)+p4*(8*x*x*x*x-8*x*x+1)+p5*(16*x*x*x*x*x-20*x*x*x+5*x);
		}
		else{
			r = p0 + p1*x + p2*x*x + p3*x*x*x + p4*x*x*x*x + p5*x*x*x*x*x + p6*x*x*x*x*x*x + p7*x*x*x*x*x*x*x;
		}
		
		//x = dt_in;
		//return p0+p1*x+p2*(2*x*x-1)+p3*(4*x*x*x-3*x)+p4*(8*x*x*x*x-8*x*x+1)+p5*(16*x*x*x*x*x-20*x*x*x+5*x);
		if(r>5.05) r = 5.05;
		if(r<0) r = 0;
	return r;
		//std::cout<<"p0="<<this->p0<<"  p1="<<this->p1<<"  p2="<<this->p2<<"  p3="<<this->p3<<std::endl;
		//std::cout<<"t0="<<t0<<"  tk="<<tk<<"  dt_in_org="<<dt_in_org<<"  dt_in="<<dt_in<<std::endl;
	/*	if(dt_in <= range_mid)
			return (p0Part1+p1Part1*dt_in+p2Part1*dt_in*dt_in+p3Part1*dt_in*dt_in*dt_in);
		else
			return (p0Part2+p1Part2*dt_in+p2Part2*dt_in*dt_in+p3Part2*dt_in*dt_in*dt_in);
	*/
	}

	void TStrawCalibration::SaveDiagnosticHistograms(TFile *f){
 	 f->cd();
	 TDirectory *cdcalibration = f->mkdir("calibration");
	 cdcalibration->cd();
         prj->Write();
	 calibHist->Write();
         calibCurve->Write();
//        calibCurvePart2->Write();
	 	
	 f->cd();
	 TVectorD v(1);
  	 v[0] = t0;
  	 v.Write("t0");
	 std::cout<<"Histograms and t0 saved\n";
	}

	void TStrawCalibration::SetP0(double param){
	 p0= param;
	}


	double TStrawCalibration::Median(std::vector<double> scores)
	{
  	 double median;
  	 size_t size = scores.size();

	 std::sort(scores.begin(), scores.end());

	  if (size  % 2 == 0)
	  {
	      median = (scores[size / 2 - 1] + scores[size / 2]) / 2;
	  }
	  else 
	  {
	      median = scores[size / 2];
	  }

	 return median;
	}
