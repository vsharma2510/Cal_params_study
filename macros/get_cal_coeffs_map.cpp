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
  double minZrange=0;
  double maxZrange=1.0;
  
  /* Setting up names of input cal_coeffs files */
  TString working_dir = "/nfs/cuore1/scratch/vsharma/Miscalibration_cal_coeff/data";

  /* Setting up output file */
  //TFile* map_file = TFile::Open("/nfs/cuore1/scratch/vsharma/Miscalibration_cal_coeff/output/cal_coeff_map.root");
  

  /*Setting up map*/
  TH2F *map_tower[19];

  for(int t=0;t<19;t++)
    {
      map_tower[t] = new TH2F(Form("Tower%03d",t+1),Form("Tower%03d",t+1),4,1,4,13,1,13);
      map_tower[t]->GetXaxis()->SetTitle("Column");
      map_tower[t]->GetYaxis()->SetTitle("Floor");
      map_tower[t]->GetXaxis()->SetBinLabel(1,"1");
      map_tower[t]->GetXaxis()->SetBinLabel(2,"2");
      map_tower[t]->GetXaxis()->SetBinLabel(3,"3");
      map_tower[t]->GetXaxis()->SetBinLabel(4,"4");
      map_tower[t]->GetYaxis()->SetBinLabel(1,"1");
      map_tower[t]->GetYaxis()->SetBinLabel(2,"2");
      map_tower[t]->GetYaxis()->SetBinLabel(3,"3");
      map_tower[t]->GetYaxis()->SetBinLabel(4,"4");
      map_tower[t]->GetYaxis()->SetBinLabel(5,"5");
      map_tower[t]->GetYaxis()->SetBinLabel(6,"6");
      map_tower[t]->GetYaxis()->SetBinLabel(7,"7");
      map_tower[t]->GetYaxis()->SetBinLabel(8,"8");
      map_tower[t]->GetYaxis()->SetBinLabel(9,"9");
      map_tower[t]->GetYaxis()->SetBinLabel(10,"10");
      map_tower[t]->GetYaxis()->SetBinLabel(11,"11");
      map_tower[t]->GetYaxis()->SetBinLabel(12,"12");
      map_tower[t]->GetYaxis()->SetBinLabel(13,"13");
      map_tower[t]->GetXaxis()->SetNdivisions(4,kFALSE);
      map_tower[t]->GetYaxis()->SetNdivisions(13,kFALSE);
      map_tower[t]->GetXaxis()->SetLabelSize(0.06);
      map_tower[t]->GetYaxis()->SetLabelSize(0.06);
      map_tower[t]->GetYaxis()->SetTitleOffset(1.3);
      
      gStyle->SetPaintTextFormat("4.1f"); 
      gStyle->SetOptStat(0);
      map_tower[t]->SetMarkerSize(2);

      Int_t colors[] = {882,890,600,867,432,840,416,828,400,798,801,807,632};
      gStyle->SetPalette((sizeof(colors)/sizeof(Int_t)),colors);
      Double_t levels[] = {-3.4e38,1.17e-38,0.90,0.95,1.00,1.05,1.10,3.4e38};
      map_tower[t]->GetZaxis()->SetRangeUser(minZrange,maxZrange);
    }
      
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
      
      vector<double> fit_par_vector;
      //Looping over datasets
      for(dataset=3601;dataset<3616;dataset++)
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
	  else{cout<<"fitfunhandle not valid"<<endl;continue;}
	  fit_par_vector.push_back(fit_par);
	}
      double sd=-1.;
      sd = TMath::RMS(fit_par_vector.begin(),fit_par_vector.end());
      //standard_deviation.push_back(sd);
      cout<<"standard deviation for channel "<<channel<<" is "<<sd<<endl;
      
      if(map_tower[tower-1]->GetBinContent(column,floor)==0){map_tower[tower-1]->SetBinContent(column,floor,sd);}
      //standard_deviation.clear();
      fit_par_vector.clear();
    }

  //TFile map_file("cal_coeff_map.root","RECREATE");
  //map_file.cd();
  TCanvas* c1 = new TCanvas("TowersMap","Towers Map all channels canvas",1000,800);
  c1->Divide(5,4);
  for(int k=0;k<19;k++)
    {
      //map_tower[k]->Write();
      c1->cd(k+1);
      map_tower[k]->Draw("COLZ TEXT");
    }

  c1->SaveAs("/nfs/cuore1/scratch/vsharma/Miscalibration_cal_coeff/output/cal_par_map.pdf");
  
  return 0;
}
