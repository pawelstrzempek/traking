#include "TFotracprSelector.cc"
#include <TLinearFitter.h>
#include <TF1.h>
#include <TH2F.h>
#include <TH1D.h>
#include <iostream>
#include <TMatrixD.h>
#include <TMinuit.h>
#include <TMath.h>
#include "entryHolderTf.cc"
#include <algorithm>
#include <TProfile.h>

class TFotracprtfSelector: public TFotracprSelector{
public: /*
	double a;
	double b;
	double a_err;
	double b_err;
	TLinearFitter *lf;
 	TF1 *f1;
	*/
        double a_out;
        double b_out;
        double a_err_out;
        double b_err_out;

	TBranch        *b_a;   //!
	TBranch        *b_b;   //!
	TBranch        *b_a_err;   //!
	TBranch        *b_b_err;   //!

	double a_mi; //a parameter obtained form minuit fit
	double b_mi; //b parameter obtained form minuit fit
	double a_mi_err;
	double b_mi_err;
	double chi;

	double t0;
	double driftR_err;//user defined parameter taken to the minuit fit. Define in the constructor.
	unsigned channelNumber;


	TH2F *residualHist;
	TH2F *dt_vs_tot;
	TH2F *dt_vs_tot_calibHist;
	TProfile *dt_vs_tot_profile;
	TF1 *dt_vs_tot_calibCurve;
	TH2F *calibHist;
	TH2F *residual_vs_dt;
	TH1F *xf_xe;
	TF1 *calib_UI;  //** this is the uploaded form file calib curve form Uniform Illumination method
//	TH2F **dt_vs_tot_channel;

TFotracprtfSelector(TTree *tree, std::string fname);
TFotracprtfSelector(TTree *tree);
void LoopTrakingMinuit(unsigned eventsQty);
//Int_t TFotracprtfSelector::MinuitFit(Int_t flag, Int_t n, Double_t *x, Double_t *z, Double_t *r, Double_t *p, Double_t pfit[2]); 
void ReadT0FromFile(std::string fname);
void Init(TTree *tree);
Int_t MinuitFit( std::vector<entryHolderTf> entriesVector);


~TFotracprtfSelector(){
	if(out_file!= NULL)
		out_file->Close();
}


};

//______________________________________________________________________________
Double_t Point2Line(float x,float y,Double_t *p) //point distance to the line
{
// Double_t value=( (par[0]*par[0])/(x*x)-1)/ ( par[1]+par[2]*y-par[3]*y*y);
 Double_t value = (TMath::Abs(p[0] + p[1]*x -y))/TMath::Sqrt(1+p[1]*p[1]);
 return value;
}// TMath::Abs(p[1]*x-y+p[0])/sqrt(p[1]*p[1]+1);
//______________________________________________________________________________
Double_t Point2Line(float x,float y,Double_t a, Double_t b) //point distance to the line
{
// Double_t value=( (par[0]*par[0])/(x*x)-1)/ ( par[1]+par[2]*y-par[3]*y*y);
 Double_t value = (TMath::Abs(a + b*x -y))/TMath::Sqrt(1+b*b);
 return value;
}// TMath::Abs(p[1]*x-y+p[0])/sqrt(p[1]*p[1]+1);

//______________________________________________________________________________
/*void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
   const Int_t nbins = 5;
   Int_t i;

//calculate chisquare
   Double_t chisq = 0;
   Double_t delta;
   for (i=0;i<nbins; i++) {
     delta  = (z[i]-func(x[i],y[i],par))/errorz[i];
     chisq += delta*delta;
   }
   f = chisq;
}
*/
// *********************************************
// fcn for Minuit fit --- chis = sum(res^2/sigma^2)
void fcnStraightLine(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag) {
  f = 0.;
  Double_t chis = 0.;
  double delta = 0.;

  TMatrixT<Double_t> *mtx = (TMatrixT<Double_t> *) gMinuit->GetObjectFit();
  Int_t hitcounter = mtx->GetNrows();

  for (int i=0; i<hitcounter; i++) {
//    if (mtx[0][i][3])
      delta = (Point2Line(mtx[0][i][0], mtx[0][i][1], par)-mtx[0][i][2]) / mtx[0][i][3];
      //chis += pow(Point2Line(mtx[0][i][0], mtx[0][i][1], par)-mtx[0][i][2], 2) / pow(mtx[0][i][3],2);
      chis += delta*delta;
//    else
//      chis += pow(Point2Line( mtx[0][i][0], mtx[0][i][1], par)-mtx[0][i][2], 2) / 12.;
  }

  if ((hitcounter-2) != 0)
    f = chis/(hitcounter-2);
}


//______________________________________________________________________________
TFotracprtfSelector::TFotracprtfSelector(TTree *tree, std::string fname) : TFotracprSelector(tree) //** we call the base constructor
{
   channelNumber = 96;
   driftR_err = 0.1;
   current_entry_number = 1;
   Init(tree);
   ReadT0FromFile(fname);
   if(fname != ""){
    out_file = new TFile(fname.c_str(),"RECREATE");
    std::cout<<"Creating ouput file: "<<fname<<std::endl;
   }
   residualHist = new TH2F("residualHist","residualHist",3000,-15,15,100,0,100);
   dt_vs_tot = new TH2F("dt_vs_tot","dt_vs_tot",2000, -500, 500, 2200,-100,1000);
   dt_vs_tot_calibHist  = new TH2F("dt_vs_tot_calibHist","dt_vs_tot_calibHist",400, -200, 200, 800,0,800);
   //dt_vs_tot_profile;
   //dt_vs_tot_calibCurve = new TF1();

   calibHist = new TH2F("r_t_calibration","r_t_calibration_fromTracks",400,0,200,510,0,5.10);
   residual_vs_dt = new TH2F("residual_vs_dt","residual_vs_dt",2000,0,200,400,-2,2);
   xf_xe = new TH1F("xf_xe","x_fit - x_exp",1020, -10.2,10.2);
//   dt_vs_tot_channel =  new TH2F*[96];
//   for (unsigned i = 0; i < channelNumber; i++) { dt_vs_tot_channel[i] = new TH2F(Form("dt_vs_tot_channel%d", i), Form("dt_vs_tot_channel%d", i),350, -150, 200, 1100,-100,1000); }   

   tree_out = new TTree("FOTRAC_PR_TFP","Forward trakers calibrated, patter recognition, track fitted precise");
   tree_out->Branch("globEvNum", &globEvNum_out);
   tree_out->Branch("chNum", &chNum_out);
   tree_out->Branch("ftTDC2", &ftTDC2_out);
   tree_out->Branch("ToT", &ToT_out);
   tree_out->Branch("driftR", &driftR_out);
   tree_out->Branch("X", &X_out);
   tree_out->Branch("Z", &Z_out);
   
   tree_out->Branch("a", &a_out);
   tree_out->Branch("b", &b_out);
   tree_out->Branch("a_err", &a_err_out);
   tree_out->Branch("b_err", &b_err_out);

   tree_out->Branch("a_mi", &a_mi);
   tree_out->Branch("b_mi", &b_mi);
   tree_out->Branch("chi" , &chi);

}

//_______________________________________________________________________________________________
TFotracprtfSelector::TFotracprtfSelector(TTree *tree) : TFotracprSelector(tree) //** we call the base constructor
{
   driftR_err = 0.0001;
   current_entry_number = 1;
   Init(tree);
}

//_______________________________________________________________________________________________
void TFotracprtfSelector::ReadT0FromFile(std::string fname){
 t0 = 0.;
 std::string calibrationFile = "";
 std::size_t found = fname.find("FOTRAC");
 if(found == std::string::npos){//** pattern not found
 	std::cout<<"Could not open file:"<<fname<<"\n"<<"t0 set to 0"<<std::endl;
 	return;
 }
 //**pattern found
 calibrationFile = fname.substr(0,found+6)+".root";
 std::cout<<"file with t0: "<<calibrationFile<<std::endl;
 TFile* tmp_file = new TFile(calibrationFile.c_str(),"READ");
 TVectorD* v = (TVectorD*)tmp_file->Get("t0");
 t0=(v->GetMatrixArray())[0]; 
 std::cout<<"load t0 ="<<t0<<std::endl;
 TF1* c_tmp = (TF1*)tmp_file->Get("calibration/calibCurve");
 //c_tmp->GetNpar();
 calib_UI = new TF1();
 if(c_tmp != NULL)
 	c_tmp->Copy(*calib_UI);
 tmp_file->Close();
return;
}

//_______________________________________________________________________________________________
void TFotracprtfSelector::Init(TTree *tree)
{
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("a", &a, &b_a);
   fChain->SetBranchAddress("b", &b, &b_b);
   fChain->SetBranchAddress("a_err", &a_err, &b_a_err);
   fChain->SetBranchAddress("b_err", &b_err, &b_b_err);
   Notify();
}

// *********************************************
// implementation of the Minuit fit
//Int_t TFotracprtfSelector::MinuitFit(Int_t flag, Int_t n, Double_t *x, Double_t *z, Double_t *r, Double_t *p, Double_t pfit[2]) {
Int_t TFotracprtfSelector::MinuitFit( std::vector<entryHolderTf> entriesVector) {
  //   cout << "MINUIT FIT --- " << n << " hits " << endl;
  TMinuit minimizer(2);
  minimizer.SetPrintLevel(-1);

  TMatrixT<Double_t> fitvect;
  //    cout << "*******MINUIT********" << endl;
  //     cout << "p0 SEED " << p[0] << endl;
  //     cout << "p1 SEED " << p[1] << endl;
  //     cout << "********************" << endl;
  fitvect.ResizeTo(entriesVector.size(), 4);
  //   cout << "======== SETUP FITVECTOR =========== " << endl;
  for(unsigned i = 0; i < entriesVector.size(); i++)
    {
      fitvect[i][0] = entriesVector[i].s_Z;//x[i];
      fitvect[i][1] = entriesVector[i].s_X;//z[i];
      fitvect[i][2] = entriesVector[i].s_driftR;//r[i];
      fitvect[i][3] = driftR_err;//GetError(r[i]);
    //std::cout<<"Point: x="<<entriesVector[i].s_X<<" z="<<entriesVector[i].s_Z<<std::endl;
    }

  minimizer.SetFCN(fcnStraightLine);
//  minimizer.DefineParameter(0, "p0", p[0], 0.1, -5000, 5000);
 // minimizer.DefineParameter(1, "p1", p[1], 0.1, -3000, 3000);
  minimizer.DefineParameter(0, "p0", b, 0.1, -5000, 5000);
  minimizer.DefineParameter(1, "p1", a, 0.1, -3000, 3000);
  minimizer.SetObjectFit(&fitvect);

  minimizer.SetMaxIterations(500);

  minimizer.Migrad();

  Double_t chisquare, errpfit[2];
  Double_t pfit[2];
  minimizer.GetParameter(0, pfit[0], errpfit[0]);
  minimizer.GetParameter(1, pfit[1], errpfit[1]);

       //std::cout << "RESULTS FIT ****************** " << std::endl;
       //std::cout << "p0 " << pfit[0] << std::endl;
       //std::cout << "p1 " << pfit[1] << std::endl;
  a_mi =  pfit[1];
  b_mi =  pfit[0];
  a_mi_err = errpfit[1];
  b_mi_err = errpfit[0];


 return 1;
}


void TFotracprtfSelector::LoopTrakingMinuit(unsigned eventsQty)
{
 std::vector<entryHolderTf> entriesVector;

 if (fChain == 0) return;

 Long64_t nentries = fChain->GetEntriesFast();
 if(eventsQty < nentries) //limiting the number of events according to user request
        nentries = eventsQty;

 Long64_t nbytes = 0, nb = 0;
 unsigned currentEvent = 1;
 unsigned local_event_counter = 1;
 Double_t distance_tmp =0.;

 std::vector<std::pair<double,unsigned> > container;//** this container stores the index and distance of entriesVector

 for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      if(jentry%10000 == 0)
	std::cout<<"Entries done: "<<jentry<<" of "<<nentries<<"\r";
	//**filter the hits which are not aligned to the minuit fit
	short counter = 0;
      if(currentEvent != (unsigned)globEvNum){
	 MinuitFit(entriesVector);
	 for (unsigned i=0; i<entriesVector.size(); i++){
			distance_tmp = Point2Line((entriesVector[i]).s_Z,(entriesVector[i]).s_X,b_mi, a_mi);
			container.push_back(std::make_pair(TMath::Abs(distance_tmp-entriesVector[i].s_driftR),i));
//			if(distance_tmp > (entriesVector[i]).s_driftR + 0.2 || distance_tmp <  (entriesVector[i]).s_driftR - 0.2){
//				entriesVector.erase (entriesVector.begin()+i);
				//std::cout<<distance_tmp<<"    r="<<(entriesVector[i]).s_driftR<<std::endl;
//				counter++;
			//}	
//		 if (counter == 2) break;
		}
//	for(unsigned k =0; k< entriesVector.size() ; k++){
//			std::cout<<(container.at(k)).first<<" "<<(container.at(k)).second<<std::endl;
//		}

	std::sort(container.begin(), container.end());
//	std::cout<<"VVVVVVV sorted VVVVVV"<<std::endl;

//	for(unsigned k =0; k< entriesVector.size() ; k++){
//			std::cout<<(container.at(k)).first<<" "<<(container.at(k)).second<<std::endl;
//		}
	//**if the worst aligned straw is missing the trak more then 200 um, we delete it and fit the minuit again
	if(  (container.back()).first  > 0.2  ){
		entriesVector.erase(entriesVector.begin() + (container.back()).second);
		
		std::cout<<"hits deleted:"<<(container.back()).second<<" diff="<<(container.back()).first <<""<<std::endl;
	}
	container.clear();
      }


      //** saving data to output tree
      if(currentEvent != (unsigned)globEvNum){ //check if the event ended
        currentEvent = globEvNum;

		//std::cout<<"\n======================================================================="<<std::endl;
		//std::cout<<"======EVENT:"<<currentEvent<<"====================================="<<std::endl;
		//std::cout<<"======================================================================="<<std::endl;

                //**we save the event
		MinuitFit(entriesVector);
//		std::cout<<"a  "<<a<<std::endl;
//		std::cout<<"b  "<<b<<std::endl;
//		std::cout<<"a_mi  "<<a_mi<<std::endl;
//		std::cout<<"b_mi  "<<b_mi<<std::endl;
                for (unsigned i=0; i<entriesVector.size(); i++){
                   globEvNum_out = local_event_counter;
                   chNum_out = (entriesVector[i]).s_chNum;
                   ftTDC2_out = (entriesVector[i]).s_ftTDC2;
                   ToT_out = (entriesVector[i]).s_ToT;
                   driftR_out = (entriesVector[i]).s_driftR;
                   X_out = (entriesVector[i]).s_X;
                   Z_out = (entriesVector[i]).s_Z;
		   a_out = (entriesVector[i]).s_a;
		   b_out = (entriesVector[i]).s_b;
		   a_err_out = (entriesVector[i]).s_a_err;
		   b_err_out = (entriesVector[i]).s_b_err;
		   a_mi = a_mi; 
        	   b_mi = b_mi; 
                   a_mi_err = a_mi_err;
         	   b_mi_err = b_mi_err;

		   distance_tmp = Point2Line(Z_out, X_out, b_mi, a_mi);
		//	std::cout<<"dist="<<distance_tmp<<" r="<<driftR_out<<" d-r="<<distance_tmp - driftR_out<<std::endl;
		   residualHist->Fill( distance_tmp - driftR_out , chNum_out );

        	   chi = ( distance_tmp - driftR_out )/ driftR_err;
		   chi = chi*chi;
                   tree_out->Fill();
		   dt_vs_tot->Fill(ftTDC2_out,ToT_out);
		   if(ToT_out > 40 &&  ToT_out < 350)
		   	dt_vs_tot_calibHist -> Fill(ftTDC2_out,ToT_out);
	           //dt_vs_tot_channel[chNum_out-1]->Fill(ftTDC2_out,ToT_out);
  		   calibHist->Fill(ftTDC2_out-t0,distance_tmp);
		   residual_vs_dt->Fill(ftTDC2_out-t0, distance_tmp - driftR_out);

		//	std::cout<<"a_mi="<<a_mi<<" b_mi="<<b_mi<<" Z="<<Z_out<<" X="<<X_out<<"Xcal="<<((Z_out-a_mi)/b_mi)<<std::endl;
		   xf_xe->Fill(((Z_out*a_mi)+b_mi) - X_out);
                }
                entriesVector.clear();
                local_event_counter++;
		//std::cout<<"a="<<lf->GetParameter(0)<<"  b="<<lf->GetParameter(1)<<std::endl;
      }
        entryHolderTf eh = {globEvNum, chNum, ftTDC2, ToT, driftR, X, Z, a, b, a_err, b_err};
        entriesVector.push_back(eh);
 }
 //** generating profile from dt_vs_tot_calib. The calib hist is the dt_vs_tot but in window of tot. 
 //** the profile histogram is used to fit the pol3 function which is later used for tot calibration 
 //** due to straw geometry (small tots far from anod wire must be compensated)
 dt_vs_tot_profile = dt_vs_tot_calibHist->ProfileX();
 dt_vs_tot_calibCurve = new TF1("dt_vs_tot_calibCurve","pol3",-200,200);
 dt_vs_tot_profile->Fit("dt_vs_tot_calibCurve", "R");
 
 //dt_vs_tot_calibCurve = new TF1();




 std::cout<<"Saving histograms\n";
 tree_out->Write();
 TH1D * profile = residualHist->ProjectionX("resProjection"); 
 dt_vs_tot->Write();
 residualHist->Write();
 profile->Write();
 calibHist->Write();
 residual_vs_dt->Write();
 dt_vs_tot_calibHist->Write();
 dt_vs_tot_profile->Write();
 dt_vs_tot_calibCurve->Write();
 xf_xe->Write();
 calib_UI->Write();
 //TDirectory *cdtot = out_file->mkdir("tot");
 //cdtot->cd();
 //for (unsigned i = 0; i < channelNumber; i++) { dt_vs_tot_channel[i]->Write(); }

}


