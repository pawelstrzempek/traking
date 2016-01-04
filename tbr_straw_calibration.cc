#include <TH1F.h>
#include <TH2F.h>
#include <TFile.h>
#include <TDirectory.h>
#include <iostream>
#include <TF1.h>
#include "stt_uj_classSelector.C"
#include <TTree.h>


#define CHANNELS 96
#define T0 1 //% of histogram entries defines the begining of drift time histogram 
#define TK 97 //% of histogram entries defines the ending of drift time histogram
#define RTUBE 0.505
#define RWIRE 0.001
 
double t0calculate(TFile *in_file)
{
        //TH1F *calibration = new TH1F("r_t_calibration","r_t_calibration",2000,0,200);
        TH2F* dt_all = (TH2F*)in_file->Get("dt_barcode");
        TH1D * prj = dt_all-> ProjectionX();

        int binsX = prj->GetNbinsX(); //get number of bins
        double integralTotal = prj->Integral(0,binsX);
        int startBin = 0;
        for(int step = 0 ; step < binsX ; step++)
        {
                if(prj->Integral(0,step) <= integralTotal*0.01*T0)
                        startBin = step;
	}
return prj->GetBinCenter(startBin);
}



TH1F* calibrationHistogramGenerate(TFile *in_file)
{
	TH1F *calibration = new TH1F("r_t_calibration","r_t_calibration",2000,0,200); 
	TH2F *dt_all = (TH2F*)in_file->Get("dt_barcode");
	TH1D *prj = dt_all-> ProjectionX();

        int binsX = prj->GetNbinsX(); //get number of bins
        double integralTotal = prj->Integral(0,binsX);
	int startBin = 0, stopBin = 0;
	double frontCutIntegral =0.0;
	double backCutIntegral =0.0;
	//**** looking for the start and stop bin of the histogram
        for(int step = 0 ; step < binsX ; step++)
        {
		if(prj->Integral(0,step) <= integralTotal*0.01*T0){
                        startBin = step;
			frontCutIntegral = prj->Integral(0,step);
		}
		if(prj->Integral(0,step) >= integralTotal *0.01*TK){
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
//		std::cout<<"step= "<<step<<"  index="<<index<<"  integral="<<prj->Integral(0,step)<<"  integralTotal="<<integralTotal<<std::endl;
		calibration->Fill(index*binning,(prj->Integral(startBin,step)/integralTotal)*(RTUBE-RWIRE)+RWIRE);
//		calibration->Fill(step,(prj->Integral(0,step)/integralTotal)*(RTUBE-RWIRE)+RWIRE);
		index++;
        }
	return calibration;
}

TF1* calibrationCurveGenerate(TH1F* calibHist){
calibHist->Fit("pol3");
//TF1 *fitFunction = calibHist->GetFunction("r_t_calibrationCurve");
return calibHist->GetFunction("pol3");//fitFunction;
}


void straw_calibration(const char* fileName){	
//**opening the file
 TFile* in_file = new TFile(fileName, "UPDATE");
 if(in_file == 0) {
 	std::cout<<"Cannot open file : "<<fileName<<std::endl;
 	}
//**based on file the calibration histogram is generated
 TH1F *calibHist = calibrationHistogramGenerate(in_file);
//**based on calibration histogram the calibration curve is generated
 TF1 *result = calibrationCurveGenerate(calibHist);
//**calculating t0 
double t0 = t0calculate(in_file);
std::cout<<t0<<std::endl;

//**creating subdirectory in the output file 
// TDirectory *cdcalibration = in_file->mkdir("calibration");
// cdcalibration->cd();

 std::cout<<"Print"<<std::endl;
 calibHist->Draw("P");
 result->Draw("same");
//uc calibHist->Write();
//uc result->Write();

 TTree *tree;
 std::cout<<"Init"<<std::endl;
 in_file->GetObject("STT_UJ",tree); 
 stt_uj_classSelector obj(tree);
 std::cout<<"Loop"<<std::endl;
 TH1F *hitsDist = obj.Loop(t0);
hitsDist->Draw();
hitsDist->Write();
}












