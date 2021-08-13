/*----------------------------------------------------------------------------
 *
 *   Copyright (C) 2020 Davide Brundu, Gian Matteo Cossu
 *
 *   This file is part of TFBoost Library.
 *
 *   TFBoost is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   TFBoost is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with TFBoost.  If not, see <http://www.gnu.org/licenses/>.
 *
 *---------------------------------------------------------------------------*/
/*
 * 
 *
 *  Created on: 11/08/2021
 *    Author: Gian Matteo Cossu
 */



#ifndef ANALYSIS_INL_
#define ANALYSIS_INL_

// STD
#include <iostream>
#include <fstream>
#include <assert.h>
#include <time.h>
#include <chrono>
#include <vector>
#include <cmath>

// HYDRA
#include <hydra/Function.h>
#include <hydra/GaussKronrodQuadrature.h>
#include <hydra/Lambda.h>
#include <hydra/Algorithm.h>
#include <hydra/Convolution.h>
#include <hydra/functions/Gaussian.h>
#include <hydra/device/System.h>
#include <hydra/functions/ConvolutionFunctor.h>
#include <hydra/functions/SpilineFunctor.h>
#include <hydra/functions/Polynomial.h>
#include <hydra/Filter.h>
#include <hydra/Parameter.h>
#include <hydra/Random.h>
#include <hydra/Plain.h>
#include <hydra/Sobol.h>
#include <hydra/RandomFill.h>
#include <hydra/Zip.h>
#include <hydra/Range.h>
#include <hydra/SeedRNG.h>
#if HYDRA_DEVICE_SYSTEM == CUDA
#include <hydra/CuFFT.h>
#endif
#if HYDRA_DEVICE_SYSTEM != CUDA
#include <hydra/FFTW.h>
#endif


// ROOT
#include <TROOT.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>
#include <TSystem.h>
#include <TFile.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TIterator.h>
#include <TString.h>
#include <TDirectory.h>
#include <TLine.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TFitResultPtr.h>
#include <TFitResult.h>
#include <TStyle.h>
#include <TRandom3.h>

// OTHERs
#include <tclap/CmdLine.h>
#include <libconfig.h++>
#include <sys/stat.h>

// TFBOOST
#include <tfboost/Types.h>
#include <tfboost/Utils.h>
#include <tfboost/ITCoDe.h>
#include <tfboost/functions/TIA_BJT_1stage.h>
#include <tfboost/functions/TIA_BJT_2stages.h>
#include <tfboost/functions/TIA_BJT_2stages_GM.h>
#include <tfboost/functions/TIA_IdealInt.h>
#include <tfboost/functions/TIA_MOS.h>
#include <tfboost/functions/ButterworthFilter.h>
#include <tfboost/DoConvolution.h>
#include <tfboost/Noise.h>
#include <tfboost/InputOutput.h>
#include <tfboost/Algorithms.h>
#include <tfboost/Digitizer.h>


namespace libconf = libconfig;

double Rmz (double x, double tau_np);

int main(int argv, char** argc)
{

  tfboost::TFBoostHeader();
  
  std::cout.precision(10);
  
 
  /* ----------------------------------------------
   * Get Configuration
   * --------------------------------------------*/
 
  libconf::Config Cfg;
  Cfg.readFile("../etc/3Ddiamond.cfg");
  const libconf::Setting& cfg_root  = Cfg.getRoot();
  

  const TString OutputDirectory    = (const char*) cfg_root["OutputDirectory"]; 

  const double dT                  = (double)  cfg_root["dT"];
  const size_t Nsamples            = (int)  cfg_root["Nsamples"];

  const double Rn                  = (double)  cfg_root["Rn"];
  const double Rp                  = (double)  cfg_root["Rp"];
  const double Cd                  = (double)  cfg_root["Cd"];

  const size_t Ndz                 = (int)  cfg_root["Ndz"];
   
  hydra::host::vector<double>  Time;
  hydra::host::vector<double>  Rm_n;
  hydra::host::vector<double>  Rm_p;
    
  Time.reserve(Nsamples);
  Rm_n.reserve(Nsamples);
  Rm_p.reserve(Nsamples);

 
  for (int j=1; j<= Ndz; j++){

    double taun = Rn/Ndz * Cd * j  ;
    double taup = Rp/Ndz * Cd * j  ;

    for (int i=0; i<Nsamples; i++){

    double curr_time = i*dT;

    Rm_n[i] += Rmz (curr_time, taun);
    Rm_p[i] += Rmz (curr_time, taup);

    }

  }

  // final array of total transimpendance
  hydra::host::vector<double>  RM;
  RM.reserve(Nsamples);

  std::ofstream outdata( OutputDirectory + "/3Ddiamond.txt" ); 

  for(int i=0; i< Nsamples; i++){
    
    RM[i]   = (Rm_n[i] + Rm_p[i])*dT;
    Time[i] = i*dT;

    if (i<10){outdata << Time[i] << " " << 0 <<  std::endl;}

    else outdata << Time[i] << " " << 0.5*RM[i-10]/Ndz <<  std::endl;

  }

  outdata.close();
      
  std::cout <<""<<std::endl;
  std::cout <<"======================================================================================="<<std::endl;
  std::cout <<""<<std::endl;
  std::cout << "Transfer function '3Ddiamond.txt' saved in: examples/conv_input_files/" << std::endl;
  std::cout <<""<<std::endl;
  std::cout <<"======================================================================================="<<std::endl;
  std::cout <<""<<std::endl;

 
  return 0;
}


double Rmz (double x, double tau_np){

  return exp(-x/tau_np)/tau_np ;

}

#endif






