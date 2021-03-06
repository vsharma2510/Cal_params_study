#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <stdio.h>
#include <sstream>
#include <vector>

#include "TGraph.h"
#include "TMath.h"
#include "TGraphErrors.h"                                                    
#include "TAxis.h"                                           
#include "TH1F.h"                                                                                       
#include "TH2F.h"                                                                                       
#include "TFrame.h"                                                                                           
#include "TCanvas.h"                                                                             
#include "TPaveStats.h"                                                                 
#include "TStyle.h"                                                                                     
#include "TFile.h"

#include "QCuore.hh"
#include "QError.hh"
#include "QGlobalHandle.hh"
#include "QMessage.hh"
#include "QGlobalDataManager.hh"
#include "QDetChannelCollectionHandle.hh"
#include "QHeader.hh"
#include "QPulseInfo.hh"
#include "QTObject.hh"
#include "QTObjectHandle.hh"
#include "QCalibrationParametersHandle.hh"
#include "QCalibrationResiduals.hh"

using namespace std;
using namespace Cuore;

int main()
{
  //Cuore::QGlobalDataManager dm;
  int channel,dataset;
  double fit_par=1.;
  TF1* calib_fit = NULL;
  //vector<double> standard_deviation;
  //double minZrange=0;
  //double maxZrange=1.0;
  
  /* Setting up names of input cal_coeffs files */
  TString working_dir = "/nfs/cuore1/scratch/vsharma/Miscalibration_cal_coeff/data";

  /* Setting up output file */
  //TFile* map_file = TFile::Open("/nfs/cuore1/scratch/vsharma/Miscalibration_cal_coeff/output/cal_coeff_map.root");

  /*Setting up output histogram*/
  TH1D* cal_par_histogram = new TH1D("cal_par_histogram","cal_par_histogram",5000,0,5);
  
  //Looping over channels
  for(channel=1;channel<989;channel++)
    {
      //Evaluating tower
      int tower=0;
      if(channel%52==0){tower=channel/52;}
      else{tower=(channel/52)+1;}

      //Evaluating floor
      int floor=0;
      floor=channel%13;
      if(floor==0){floor=13;}

      //Evaluating column
      int column=0;
      column=(channel%52)/13+1;
      if(floor==13 && channel%52==0){column=4;}
      else if(floor==13){column--;}

      //cout<<channel<<" T "<<tower<<" F "<<floor<<" C "<<column<<endl;
      
      vector<double> chan_par_vector;
      //Looping over datasets
      for(dataset=3608;dataset<3616;dataset++)
	{
	  TString cal_coeffs_file_name = Form("%s/ds%d/cal_debug_ds%d_tower%.3d.root",working_dir.Data(),dataset,dataset,tower);
	  //cout<<"Accessing file "<<cal_coeffs_file_name<<endl;
	  //Getting fit function
	  Cuore::QGlobalDataManager dm;
	  QTObjectHandle<TF1> fitfunhandle("CalFunction");
	  fitfunhandle.SetChannel(channel);
	  dm.Get("CalCoefficients_heaterTGS",&fitfunhandle,cal_coeffs_file_name.Data());
	  if(fitfunhandle.IsValid()){
	    Cuore::QTObject<TF1> qfit = fitfunhandle.Get();
	    calib_fit = (TF1*)(qfit.fObject.Clone(Form("clone_fit%d",channel)));
	    fit_par = calib_fit->GetParameter(0);
	    //cout<<"fit_par for channel "<<channel<<" is "<<fit_par<<endl;
	  }
	  else{//cout<<"fitfunhandle not valid for channel "<<channel<<"for dataset "<<dataset<<endl;
	    continue;}
	  chan_par_vector.push_back(fit_par);
	}
      double sd=-1.;
      if(chan_par_vector.size()>5)
	{
	  sd = TMath::RMS(chan_par_vector.begin(),chan_par_vector.end());
	  if(sd>5)
	    {
	      cout<<"standard deviation for channel "<<channel<<" exceeds threshold "<<endl;
	    }
	  cal_par_histogram->Fill(sd);
	}
      else{cout<<"channel "<<channel<<" has too few data points"<<endl;}
	  
      //standard_deviation.push_back(sd);
      //cout<<"standard deviation for channel "<<channel<<" is "<<sd<<endl;
      
      //standard_deviation.clear();
      chan_par_vector.clear();
    }
  
  TFile* output_file = TFile::Open("/nfs/cuore1/scratch/vsharma/Miscalibration_cal_coeff/output/cal_par_histo.root","RECREATE");
  output_file->cd();
  cal_par_histogram->Draw();
  cal_par_histogram->Write("cal_par_histogram");
  output_file->Close();
  
  return 0;
}
