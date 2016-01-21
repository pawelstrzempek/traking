#include "TFotracprSelector.cc"
#include <TLinearFitter.h>
#include <TF1.h>
#include <iostream>
#include <TMatrixD.h>
#include <TMinuit.h>
#include <TMath.h>
#include "entryHolderTf.cc"



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

	double driftR_err;//user defined parameter taken to the minuit fit. Define in the constructor.

TFotracprtfSelector(TTree *tree, std::string fname);
TFotracprtfSelector(TTree *tree);
void LoopTrakingMinuit(unsigned eventsQty);
//Int_t TFotracprtfSelector::MinuitFit(Int_t flag, Int_t n, Double_t *x, Double_t *z, Double_t *r, Double_t *p, Double_t pfit[2]); 
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
   driftR_err = 0.0001;
   current_entry_number = 1;
   Init(tree);
   if(fname != ""){
    out_file = new TFile(fname.c_str(),"RECREATE");
    std::cout<<"Creating ouput file: "<<fname<<std::endl;
   }

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

TFotracprtfSelector::TFotracprtfSelector(TTree *tree) : TFotracprSelector(tree) //** we call the base constructor
{
   driftR_err = 0.0001;
   current_entry_number = 1;
   Init(tree);
}

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
    std::cout<<"Point: x="<<entriesVector[i].s_X<<" z="<<entriesVector[i].s_Z<<std::endl;
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
//  p[0] = pfit[0]; p[1] = pfit[1];
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

 for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;

      if(jentry%10000 == 0)
	std::cout<<"Entries done: "<<jentry<<" of "<<nentries<<"\r";

      if(currentEvent != (unsigned)globEvNum){ //check if the event ended
        currentEvent = globEvNum;
		std::cout<<"\n======================================================================="<<std::endl;
		std::cout<<"======EVENT:"<<currentEvent<<"====================================="<<std::endl;
		std::cout<<"======================================================================="<<std::endl;


                //**we save the event
		MinuitFit(entriesVector);
		std::cout<<"a  "<<a<<std::endl;
		std::cout<<"b  "<<b<<std::endl;
		std::cout<<"a_mi  "<<a_mi<<std::endl;
		std::cout<<"b_mi  "<<b_mi<<std::endl;
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
//		   a_mi = 1; 
//        	   b_mi = 1; 
                   a_mi_err = 1;
         	   b_mi_err = 1;
        	   chi = 1;

                   tree_out->Fill();
                }
                entriesVector.clear();
                local_event_counter++;
		//std::cout<<"a="<<lf->GetParameter(0)<<"  b="<<lf->GetParameter(1)<<std::endl;
      }
        entryHolderTf eh = {globEvNum, chNum, ftTDC2, ToT, driftR, X, Z, a, b, a_err, b_err};
        entriesVector.push_back(eh);
 }
 tree_out->Write();

}

