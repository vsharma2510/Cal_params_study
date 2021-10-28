#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

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
  int tower,channel,dataset;
  double fit_par=1.;
  TF1* calib_fit = NULL;

  /* Setting up names of input cal_coeffs files */
  TString working_dir = "/nfs/cuore1/scratch/vsharma/Miscalibration_cal_coeff/data";

  /* Setting up output file */
  ofstream calib_par_file;
  calib_par_file.open("/nfs/cuore1/scratch/vsharma/Miscalibration_cal_coeff/output/calibration_parameters.txt");
  
  //Looping over datasets
  for(dataset=3601;dataset<3616;dataset++)
    {
      calib_par_file<<dataset<<"\t";
      //Looping over channels
      for(channel=1;channel<989;channel++)
	{
	  //Evaluating tower
	  if(channel%52==0){tower=channel/52;}
	  else{tower=(channel/52)+1;}
	  TString cal_coeffs_file_name = Form("%s/ds%d/cal_debug_ds%d_tower%.3d.root",working_dir.Data(),dataset,dataset,tower);
	  cout<<"Accessing file "<<cal_coeffs_file_name<<endl;
	  //Getting fit function
	  Cuore::QGlobalDataManager dm;
	  QTObjectHandle<TF1> fitfunhandle("CalFunction");
	  fitfunhandle.SetChannel(channel);
	  dm.Get("CalCoefficients_heaterTGS",&fitfunhandle,cal_coeffs_file_name.Data());
	  if(fitfunhandle.IsValid()){
	    Cuore::QTObject<TF1> qfit = fitfunhandle.Get();
	    calib_fit = (TF1*)(qfit.fObject.Clone(Form("clone_fit%d",channel)));
	    fit_par = calib_fit->GetParameter(0);
	    cout<<"fit_par for channel "<<channel<<" is "<<fit_par<<endl;
	  }
	  else{cout<<"fitfunhandle not valid"<<endl;}
	  if(channel==988){calib_par_file<<fit_par<<"\n";}
	  else{calib_par_file<<fit_par<<"\t";}
	  
	}
    }
  calib_par_file.close();
  return 0;
}
