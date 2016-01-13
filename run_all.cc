#include <TH1F.h>
#include <TF1.h>
#include <TMath.h>
#include <TH2F.h>
#include <TTree.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TFile.h>
#include <time.h>


void straw_processor(const char* fileName, unsigned int = 1000000);
int straw_preprocessor(int eventsNum, const char* fileName, int referenceChannel = 148);
void straw_pattern_recognition(const char* fileName, unsigned int = 1000000);


int run_all(char* fileName, int eventsNum=100000000)
{
	clock_t tStart = clock();
  
	string newFileName = "bleble";
	newFileName = string(fileName);

	straw_preprocessor(eventsNum,newFileName.c_str(),148);

	newFileName = newFileName.substr(0, newFileName.size() - 5);
	newFileName += "_FOTRA.root";
	
	straw_processor(newFileName.c_str(),eventsNum);
	
	newFileName = newFileName.substr(0, newFileName.size() - 5);
	newFileName += "C.root";
	
	straw_pattern_recognition(newFileName.c_str(),eventsNum);

        newFileName = newFileName.substr(0, newFileName.size() - 5);
        newFileName += "_PR.root";



	/*printf("\n\n >>>>>>>  Calculating Times and Hits\n\n");
	newFileName += ".root";
	calculate_times(eventsNum, newFileName.c_str(), 49, calibFile);


	calculate_hits(eventsNum, newFileName.c_str(), referenceChannel);
	pgenerate_diag_histo_panda(eventsNum,newFileName.c_str(),referenceChannel);
*/
  	printf("\n\n >>>>>>>  Final file: %s\n", newFileName.c_str());
	printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);

    return 0;
}
