//Macro to draw the time evolution of calibration parameters for channels over datasets
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

int main(int argc,char* argv[])
{
  TString working_dir = "/nfs/cuore1/scratch/vsharma/Miscalibration_cal_coeff/data";
  int channel = atoi(argv[1]);
  //Evaluating tower
  int tower=0;
  vector<double> fit_par_0,fit_par_0_err;
  vector<double> fit_par_1,fit_par_1_err;
  vector<double> dataset_v;
  TF1* calib_fit = NULL;
  if(channel%52==0){tower=channel/52;}
  else{tower=(channel/52)+1;}
  for(int dataset=3601;dataset<3616;dataset++)
    {
      TString cal_coeffs_file_name = Form("%s/ds%d/cal_debug_ds%d_tower%.3d.root",working_dir.Data(),dataset,dataset,tower);
      Cuore::QGlobalDataManager dm;
      QTObjectHandle<TF1> fitfunhandle("CalFunction");
      fitfunhandle.SetChannel(channel);
      dm.Get("CalCoefficients_heaterTGS",&fitfunhandle,cal_coeffs_file_name.Data());
      if(fitfunhandle.IsValid())
	{
	  Cuore::QTObject<TF1> qfit = fitfunhandle.Get();
	  calib_fit = (TF1*)(qfit.fObject.Clone(Form("clone_fit%d",channel)));
	  fit_par_0.push_back(calib_fit->GetParameter(0));
	  fit_par_0_err.push_back(calib_fit->GetParError(0));
	  fit_par_1.push_back(calib_fit->GetParameter(1));
	  fit_par_1_err.push_back(calib_fit->GetParError(1));
	  dataset_v.push_back((double)dataset);
	}
    }

  TGraphErrors* cal_par_0_graph = new TGraphErrors(dataset_v.size(),&dataset_v[0],&fit_par_0[0],0,&fit_par_0_err[0]);
  TGraphErrors* cal_par_1_graph = new TGraphErrors(dataset_v.size(),&dataset_v[0],&fit_par_1[0],0,&fit_par_1_err[0]);

  cal_par_0_graph->GetYaxis()->SetTitle("Linear term in calibration fit function");
  cal_par_0_graph->GetXaxis()->SetTitle("dataset");
  cal_par_1_graph->GetYaxis()->SetTitle("Linear term in calibration fit function");
  cal_par_1_graph->GetXaxis()->SetTitle("dataset");
  cal_par_0_graph->SetTitle(" ");
  cal_par_1_graph->SetTitle(" ");
  cal_par_0_graph->SetMarkerStyle(20);
  cal_par_1_graph->SetMarkerStyle(20);
  cal_par_0_graph->SetMarkerSize(0.5);
  cal_par_1_graph->SetMarkerSize(0.5);
  cal_par_0_graph->SetTitle(Form("Channel %d",channel));
  cal_par_1_graph->SetTitle(Form("Channel %d",channel));
  cal_par_0_graph->Draw();
  cal_par_1_graph->Draw();
  TString output_file_name = Form("/nfs/cuore1/scratch/vsharma/Miscalibration_cal_coeff/output/cal_par_graph_%d.root",channel);
  TFile* f = TFile::Open(output_file_name.Data(),"RECREATE");
  f->cd();
  cal_par_0_graph->Write("cal_par_0");
  cal_par_1_graph->Write("cal_par_1");
  f->Close();

  return 0;
}
  
