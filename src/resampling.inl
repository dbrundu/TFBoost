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
 *  Created on: 08/08/2021
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

int main(int argv, char** argc)
{

  tfboost::TFBoostHeader();
  
  std::cout.precision(10);
  
 
  /* ----------------------------------------------
   * Get Configuration
   * --------------------------------------------*/
 
  libconf::Config Cfg;
  Cfg.readFile("../etc/resampling.cfg");
  const libconf::Setting& cfg_root  = Cfg.getRoot();
  
  TString OutputDirectory          = (const char*) cfg_root["OutputDirectory"];
  const TString conv_inputfile     = (const char*) cfg_root["InputFileConvorTF"]; 
  const TString InputFile_current  = (const char*) cfg_root["InputFileCurrent"]; 
  const TString OutputFileName     = (const char*) cfg_root["OutputFileName"]; 
  const bool deconvolution         = (bool)        cfg_root["Deconvolution"];

  const char*  token         = (const char*) cfg_root["token"];
  const int    columnT       = (int)  cfg_root["columnT"];
  const int    columnI       = (int)  cfg_root["columnI"];
  const size_t offset        = (int)  cfg_root["offset"];
  const size_t Nsamples      = (int)  cfg_root["Nsamples"];
  const size_t Nlinestoskip  = (int)  cfg_root["Nlinestoskip"];
  const double scale         = (double) cfg_root["scale_factor"];
  
  const bool filter       = (bool)   cfg_root["filter"];
  const int order         = (int)    cfg_root["order"];
  const double frequency  = (double) cfg_root["frequency"];


  /* ----------------------------------------------
   * Initialization of variables and RNGs
   * --------------------------------------------*/
  if(!OutputDirectory.EndsWith("/")) OutputDirectory = OutputDirectory+"/";
  
  tfboost::CreateDirectories( OutputDirectory + "plots");
  tfboost::CreateDirectories( OutputDirectory + "data");

  const double dT      = 1e-12;
  const double min     = 0.0;
  const double max     = (Nsamples-1)*dT;
  const double min_kernel  = -0.5*(max-min);
  const double max_kernel  =  0.5*(max-min);
  
  auto flt = tfboost::ButterworthFilter<double>( frequency, order, dT);
  
  hydra::SeedRNG S{};
  hydra::default_random_engine engine( S() ); 
  TRandom3 root_rng( S() );


  /* ----------------------------------------------
   * Initialization of histograms
   * --------------------------------------------*/
  TH1D *hist_convol     = new TH1D("hist_deconvol;Time[s];Vout [V]",deconvolution? "Deconvoluted kernel" : "Output signal", Nsamples+1, min, max/4 );
  TH1D *hist_signal     = new TH1D("hist_signal;Time[s];Vout [V]","signal", Nsamples+1, min, max/4 );
  TH1D *hist_kernel     = new TH1D("hist_kernel;Time[s];Vout [V]",deconvolution? "Output Signal" : "kernel", Nsamples+1, min, max/4);
  
  
  
  /* ----------------------------------------------
   * Read Input currents
   * --------------------------------------------*/
  hydra::device::vector<double> time;
  hydra::device::vector<double> idx;
  hydra::device::vector<double> current;
    
  time.reserve(Nsamples);
  idx.reserve(Nsamples);
  current.reserve(Nsamples);
  
  TString line;

  std::ifstream myFile( InputFile_current.Data() );
  //line.ReadLine(myFile);
    
  // Actual loop on file lines
  if (myFile.is_open()) 
  {
    for(size_t j = offset; j < Nsamples ; ++j)
    {
      line.ReadLine(myFile);
      if (!myFile.good()) break;
      
      TObjArray *tokens = line.Tokenize( token );
      
      TString data_str  = ((TObjString*) tokens->At( columnI ) )->GetString();
      TString time_str  = ((TObjString*) tokens->At( columnT ) )->GetString();
        
      const double data = atof(data_str);
      const double tm   = dT*offset + atof(time_str);
        
      idx.push_back(j);
      time.push_back(tm);
      current.push_back(data);

      //std::cout<<tm<<" "<<data<<std::endl;
        
      tokens->Delete();
      delete tokens;
    }
  } else { SAFE_EXIT(true, "Input file cannot be open.") }
  myFile.close();

  // check timestep size
  double dtIN;
  dtIN = time[10]-time[9];
  std::cout <<"sample size of input file is Dt="<<dtIN<<std::endl;

  // add zeros when the signal is finished until Nsamples is reached
  if(Nsamples - current.size() > 0)
  {
    double t0 = time.back();
    size_t j = 1;
    
    for(size_t k=current.size(); k<Nsamples; ++k)
    {
      idx.push_back(k);
      time.push_back(t0 + j*dtIN);
      current.push_back(0.0);
      ++j;
    }
  }
   
  SAFE_EXIT( current.size() != Nsamples , "In analysis.inl: size of container not equal to Nsamples. ")

  tfboost::TimeDigitizeSignal( current, time, dT, max, engine, false);

  DEBUG(time[0])
  DEBUG(time.back())
  DEBUG(time.size())

  tfboost::SaveConvToFile(current, time, dT, OutputDirectory + OutputFileName + ".txt" );
    
    
  return 0;
}

#endif






