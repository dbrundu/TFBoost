/*----------------------------------------------------------------------------
 *
 *   Copyright (C) 2020 Davide Brundu, Gianmatteo Cossu
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
 *  Created on: 07/05/2020
 *    Author: Davide Brundu
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
#include <hydra/functions/Gaussian.h>
#include <hydra/device/System.h>
#include <hydra/functions/SpilineFunctor.h>
#include <hydra/functions/Polynomial.h>
#include <hydra/LogLikelihoodFCN.h>
#include <hydra/Filter.h>
#include <hydra/Parameter.h>
#include <hydra/UserParameters.h>
#include <hydra/Pdf.h>
#include <hydra/Random.h>
#include <hydra/Plain.h>
#include <hydra/Sobol.h>
#include <hydra/RandomFill.h>
#include <hydra/Zip.h>
#include <hydra/Range.h>
#include <hydra/SeedRNG.h>
#include <hydra/FFTW.h>


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
#include <TIterator.h>
#include <TString.h>
#include <TDirectory.h>
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
#include <tfboost/Utils.h>
#include <tfboost/functions/TIA_BJT_1stage.h>
#include <tfboost/functions/TIA_BJT_2stages.h>
#include <tfboost/functions/TIA_BJT_2stages_GM.h>
#include <tfboost/functions/TIA_IdealInt.h>
#include <tfboost/functions/TIA_MOS.h>
#include <tfboost/functions/DoConvolution.h>
#include <tfboost/noise/Noise.h>
#include <tfboost/ReadConvolution.h>
#include <tfboost/Algorithms.h>
#include <tfboost/functions/ButterworthFilter.h>


namespace libconf = libconfig;



int main(int argv, char** argc)
{

  tfboost::TFBoostHeader();
  
  std::cout.precision(10);
  
 
  /* ----------------------------------------------
   * Get Configuration
   * --------------------------------------------*/
 
  libconf::Config Cfg;
  Cfg.readFile("../etc/deconvolution.cfg");
  const libconf::Setting& cfg_root  = Cfg.getRoot();
  
  TString OutputDirectory          = (const char*) cfg_root["OutputDirectory"];
  const TString conv_inputfile     = (const char*) cfg_root["InputFileConv"]; 
  const TString InputFile_current  = (const char*) cfg_root["InputFileCurrent"]; 
  const TString OutputFileName     = (const char*) cfg_root["OutputFileName"]; 

  const char*  token         = (const char*) cfg_root["token"];
  const int    column        = (int)  cfg_root["column"];
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


  /* ----------------------------------------------
   * Initialization of histograms
   * --------------------------------------------*/
  TH1D *hist_convol     = new TH1D("hist_deconvol;Time[s];Vout [V]","hist_convol", Nsamples/30, min, max );
  TH1D *hist_signal     = new TH1D("hist_signal;Time[s];Vout [V]","hist_signal", Nsamples, min, 0.1*max );
  TH1D *hist_kernel     = new TH1D("hist_kernel;Time[s];Vout [V]","hist_kernel", Nsamples, min, max);
  
  
  
  /* ----------------------------------------------
   * Read Input currents
   * --------------------------------------------*/
  hydra::device::vector<double> time;
  hydra::device::vector<double> idx;
  hydra::device::vector<double> current;
    
  time.reserve(Nsamples);
  idx.reserve(Nsamples);
  current.reserve(Nsamples);
  
  for(size_t k=0; k<offset; ++k)
  {
    idx.push_back(k);
    time.push_back(k*dT);
    current.push_back(0.0);
  }

  TString line;

  std::ifstream myFile( InputFile_current.Data() );
  line.ReadLine(myFile);
    
  // Actual loop on file lines
  if (myFile.is_open()) 
  {
    for(size_t j = offset; j < Nsamples ; ++j)
    {
      line.ReadLine(myFile);
      if (!myFile.good()) break;
      
      TObjArray *tokens = line.Tokenize( token );
      
      TString data_str  = ((TObjString*) tokens->At( column ) )->GetString();
        
      const double data = atof(data_str);
        
      idx.push_back(j);
      time.push_back(j*dT);
      current.push_back(data);
        
      tokens->Delete();
      delete tokens;
    }
  } else { SAFE_EXIT(true, "Input file cannot be open.") }
  myFile.close();


    
  if(Nsamples - current.size() > 0)
  {
    for(size_t k=current.size(); k<Nsamples; ++k)
    {
      idx.push_back(k);
      time.push_back(k*dT);
      current.push_back(0.0);
    }
  }
   
  SAFE_EXIT( current.size() != Nsamples , "In analysis.inl: size of container not equal to Nsamples. ")

  auto signal = hydra::make_spiline<double>(time, current);
    
    
    
    
  /* ----------------------------------------------
   * Performing the de-convolution
   * --------------------------------------------*/

  #if HYDRA_DEVICE_SYSTEM!=CUDA
  auto fft_backend = hydra::fft::fftw_f64;
  #endif
    
  #if HYDRA_DEVICE_SYSTEM==CUDA
  auto fft_backend = hydra::fft::cufft_f64;
  #endif
    
  hydra::host::vector<double>   conv_data_h(Nsamples);
  hydra::device::vector<double> conv_data_d(Nsamples);
    
  hydra::host::vector<double> time2;
  hydra::host::vector<double> voltage2;
  time2.reserve(Nsamples);
  voltage2.reserve(Nsamples);

  tfboost::ReadTF( conv_inputfile, Nlinestoskip, time2, voltage2, scale);

  auto kernel = hydra::make_spiline<double>(time2, voltage2);
        
  conv_data_h = tfboost::Do_DeConvolution(fft_backend, kernel, signal, min, max, Nsamples);
  
  if(filter){
    auto conv_temp = hydra::make_spiline<double>(time, conv_data_h);
    conv_data_h    = tfboost::Do_Convolution(fft_backend, flt, conv_temp, min, max, Nsamples);
  }

  auto conv = hydra::make_spiline<double>(time, conv_data_h);


  /* ----------------------------------------------
   * Performing plots and save results
   * --------------------------------------------*/
  for(size_t i=1;  i < (size_t) Nsamples+1; ++i) {
    hist_kernel->SetBinContent(i, kernel(hist_kernel->GetBinCenter(i)) ); 
    hist_convol->SetBinContent(i, conv(hist_convol->GetBinCenter(i)));// conv_data_h[i-1] );
    hist_signal->SetBinContent(i, signal(hist_signal->GetBinCenter(i)) ); 
  }

  TCanvas canvas("canvas" ,"canvas", 4000, 1000);
  canvas.Divide(3,1);
  canvas.cd(1);
  hist_convol->SetStats(0);
  hist_convol->Rebuild();
  hist_convol->SetLineColor(4);
  hist_convol->SetLineWidth(1);
  //hist_convol->Fit("chebyshev9","S");
  hist_convol->Draw("C");
      
  canvas.cd(2);
  hist_signal->SetStats(0);
  hist_signal->SetLineColor(4);
  hist_signal->SetLineWidth(1);
  hist_signal->Draw();
      
  canvas.cd(3);
  hist_kernel->SetStats(0);
  hist_kernel->SetLineColor(4);
  hist_kernel->SetLineWidth(1);
  hist_kernel->Draw();
  //hist_convol->SetLineColor(1);
  //hist_convol->Draw("same");
      
  canvas.SaveAs(OutputDirectory + "plots/hist_convol_functor.pdf");
  
  tfboost::SaveConvToFile(conv_data_h, time, dT, OutputDirectory + OutputFileName );
    
  return 0;
}

#endif






