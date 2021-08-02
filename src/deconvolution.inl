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

template<typename DATA>
double GetVariance(DATA x, double m_x){    
    size_t n = x.size();

    double s_x=0;
    
    for(int i=0; i<n; ++i){
        s_x += (x[i] - m_x)*(x[i] - m_x);
    }
    s_x /= n;
    return s_x;
}


void getGreenToRed(double percent){
            double r = percent<50 ? 255 : std::floor(255-(percent*2-100)*255/100);
            double g = percent>50 ? 255 : std::floor((percent*2)*255/100);
            std::cout << r << " " << g << std::endl;
 }

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


    
  if(Nsamples - current.size() > 0)
  {
    double t0 = time.back();
    size_t j = 1;
    
    for(size_t k=current.size(); k<Nsamples; ++k)
    {
      idx.push_back(k);
      time.push_back(t0 + j*dT);
      current.push_back(0.0);
      ++j;
    }
  }
   
  SAFE_EXIT( current.size() != Nsamples , "In analysis.inl: size of container not equal to Nsamples. ")

  auto signal = hydra::make_spiline<double>(time, current);
  DEBUG(time[0])
  DEBUG(time.back())
  DEBUG(time.size())

    
    
    
    
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

  tfboost::ReadTF( conv_inputfile, Nlinestoskip, time2, voltage2, time.back(), dT, scale, true, true);
  
  //hydra::default_random_engine engine( S() ); 
  //tfboost::TimeDigitizeSignal( voltage2, time2, 50e-12, time.back(), engine, false);

  ///////////////////////////////
  // Remove baseline
  ///////////////////////////////
  
  /*
    double mean = 0.0;
  for(size_t i=400; i<500; ++i){
    mean += voltage2[i];
  }
  mean /= (100);
  
  for(auto& x : voltage2) {x -= mean; 
    //x*=1.56; 
    // x+=root_rng.Gaus( 0.0 , 0.0005);
     } */

  auto kernel = hydra::make_spiline<double>(time2, voltage2);

  DEBUG(time2[0])
  DEBUG(time2.back())
  DEBUG(time2.size()) 
        

        
  if(deconvolution){
    tfboost::Do_DeConvolution(fft_backend, kernel, signal, conv_data_h, min, max, Nsamples);
  } else {
    tfboost::Do_Convolution(fft_backend, kernel, signal, conv_data_h, min, max, Nsamples);
  }  
  
  if(filter){
    auto conv_temp = hydra::make_spiline<double>(time, conv_data_h);
    tfboost::Do_Convolution(fft_backend, flt, conv_temp, conv_data_h, min, max, Nsamples);
  }

  hydra::host::vector<double> time_f;
  double dT_f = (max-min)/(Nsamples-1);
  for(size_t i=0; i<conv_data_h.size(); ++i) time_f.push_back(i*dT_f);

  auto conv = hydra::make_spiline<double>(time_f, conv_data_h);
  DEBUG(time_f[0])
  DEBUG(time_f.back())
  DEBUG(time_f.size())

//  PRINT_ELEMENTS(conv_data_h.size(), conv_data_h)


  /* ----------------------------------------------
   * Performing plots and save results
   * --------------------------------------------*/
  for(size_t i=1;  i < (size_t) Nsamples; ++i) {
    hist_kernel->SetBinContent(i, kernel(hist_kernel->GetBinCenter(i)) ); 
    hist_convol->SetBinContent(i, conv(hist_convol->GetBinCenter(i)));
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
      
  canvas.SaveAs(OutputDirectory + "plots/" + OutputFileName + ".pdf");


  //1ns delay for Transfer Function
  for(int i=0; i<Nsamples; i++){

  conv_data_h[Nsamples-i]=conv_data_h[Nsamples-i-1000];

  if(Nsamples-1-1000<0){
    conv_data_h[Nsamples-i-1000]=0;
  }


  }
 

  
  tfboost::SaveConvToFile(conv_data_h, time_f, dT, OutputDirectory + OutputFileName + ".txt" );
    
// int a=0;
// int b=0;
// double max[11] = {14.3856, 14.883, 14.4768, 15.5438, 15.2651, 9.77783, 6.36696, 6.04976, 15.062, 14.5116, 10.7266};

// for(auto l : {"0","1","2","3","4","5","6","7","8","9","10"} ){
// b=0;
//     for(auto m : {"0","1","2","3","4","5","6","7","8","9","10"} ){


//   hydra::host::vector<double> time_10;
//   hydra::host::vector<double> voltage_10;
//   time_10.reserve(16384);
//   voltage_10.reserve(16384);

//   tfboost::ReadTF( TString("results_deconv/laserIR_")+l+"_150.txt", 0, time_10, voltage_10, 16384*1e-12, 1e-12, 1.0, true);
//   for(auto& x : voltage_10) x/=max[a];
//   double mean_10 = 0.0;
//   for(auto const& x : voltage_10) mean_10 += x;
//   mean_10 /= 16384.;
//   double var_10 = GetVariance(voltage_10, mean_10);

//   hydra::host::vector<double> time_0;
//   hydra::host::vector<double> voltage_0;
//   time_0.reserve(16384);
//   voltage_0.reserve(16384);

//   tfboost::ReadTF( TString("results_deconv/laserIR_")+m+"_150.txt", 0, time_0, voltage_0, 16384*1e-12, 1e-12, 1.0, true);
//   for(auto& x : voltage_0) x/=max[b];
//   double mean_0 = 0.0;
//   for(auto const& x : voltage_0) mean_0 += x;
//   mean_0 /= 16384.;
//   double var_0 = GetVariance(voltage_0, mean_0);


//   double cross_corr = 0.0;
//   for(size_t i = 0; i<16384; ++i){
//       cross_corr += (voltage_10[i] - mean_10)*(voltage_0[i] - mean_0);
//   }
//   cross_corr /= 16383.;

//   cross_corr /= (sqrt(var_10*var_0));

//   std::cout <<  "l="<<l << ", m="<<m << ", cross_corr=" << 534.1709*cross_corr - 434.1709 << " -> ";
//   getGreenToRed(534.1709*cross_corr - 434.1709);
//   b++;
//   }
// a++;
// }
  
  return 0;
}

#endif






