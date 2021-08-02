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



int main(int argv, char** argc)
{

  tfboost::TFBoostHeader();
  
  std::cout.precision(10);
  
 
  /* ----------------------------------------------
   * Get Configuration
   * --------------------------------------------*/
 
  libconf::Config Cfg;
  Cfg.readFile("../etc/singlesignal.cfg");
  const libconf::Setting& cfg_root  = Cfg.getRoot();

  const TString InputFile    = (const char*) cfg_root["InputFile"]; 
  const int    columnT       = (int)  cfg_root["columnT"];
  const int    columnI       = (int)  cfg_root["columnI"];
  const size_t Nlinestoskip  = (int)  cfg_root["Nlinestoskip"];
  const double scale         = (double) cfg_root["scale_factor"];
  const char*  token         = (const char*) cfg_root["token"];

  const int min_noise        = (int) cfg_root["min_noise"];
  const int max_noise        = (int) cfg_root["max_noise"];

  const double LEthr         = (double) cfg_root["LEthr"];
  const double CFD_fr        = (double) cfg_root["CFD_fr"];
  const double RM_delay      = (double) cfg_root["RM_delay"];


  hydra::host::vector<double> time;
  hydra::host::vector<double> voltage;

  std::array<int,2> columns = {columnT, columnI};

  tfboost::ReadSimple(InputFile, columns, token, time, voltage, Nlinestoskip, scale);

  size_t idx   = tfboost::algo::LeadingEdge(voltage , LEthr);
  double vpeak = tfboost::algo::GetVAtPeak(voltage);
    
  size_t TOA_CFD        = tfboost::algo::ConstantFraction(voltage , CFD_fr , vpeak);

  double centroid = tfboost::algo::TimeCentroid(voltage, time);
  std::cout << "Time centroid: " << centroid << std::endl;
  std::cout << "Time LE      : " << time[idx] << std::endl;
  std::cout << "Time CFD     : " << time[TOA_CFD] << std::endl;

  
  
    //   // calculating RMS of noise, if the offset is present
    //   size_t new_offset = c.offset*1e-12 / c.sampling_dT;

    //   double rms_noise = 0.0;
	//   for (size_t i=min_noise; i<max_noise; i++) rms_noise += voltage[i] * voltage[i];
    //   rms_noise = ::sqrt(rms_noise/(max_noise-min_noise));

    //   // initialize indices
    //   size_t timeatmax_idx=0, TOA_LE_noise_idx=0, TOA_CFD_noise_idx=0, TOA_RM_noise_idx=0;

    //   timeatmax_idx     = tfboost::algo::GetTimeAtPeak(voltage);
    //   TOA_LE_noise_idx  = tfboost::algo::LeadingEdge(voltage , LEthr);
      
    //   measures_noise[_tpeak]     = time[timeatmax_idx] ;
    //   measures_noise[_toa_le]    = time[TOA_LE_noise_idx]  ;
    //   measures_noise[_vonth_le]  = voltage[TOA_LE_noise_idx];
    //   measures_noise[_tot]       = tfboost::algo::TimeOverThr(voltage, time, LEthr, LEthr) ;
      
      
      
    //   if(c.MakeLinearFitNearThreshold )
    //   {
    //     auto toa = tfboost::algo::LinearFitNearThr( LEthr, voltage, time, bound_fit, PlotLinFit, "LEfit");
        
    //     measures_noise[_toa_le]  =      std::get<0>(toa);  
    //     measures_noise[_dvdt_le] = 1e-6*std::get<1>(toa) ;
    //   }
      
      
      
    //   if(c.MakeGaussianFitNearVmax )
    //   {
    //     auto gaussfit = tfboost::algo::GaussianFitNearVmax( voltage, time, bound_fit, PlotGausFit );
    //     measures_noise[_tpeak] = time[std::get<1>(gaussfit)] ; 
    //     measures_noise[_vpeak] = std::get<0>(gaussfit);

    //     auto cfd_idx = tfboost::algo::ConstantFraction(voltage , CFD_fr , measures_noise[_vpeak]);
    //     measures_noise[_toa_cfd]    = time[cfd_idx] ;
    //     measures_noise[_vonth_cfd]  = voltage[ cfd_idx ];        

    //     if(c.MakeLinearFitNearThreshold )
    //     {
    //       auto toa_cf = tfboost::algo::LinearFitNearThr( CFD_fr*measures_noise[_vpeak], 
    //                                                    voltage, time, bound_fit, 
    //                                                    /*plot?*/PlotLinFit, "CFDfit");
                                                            
    //       auto rm     = tfboost::algo::TimeRefMethod( voltage, time, 
    //                                                   measures_noise[_vpeak], RM_delay, bound_fit, 
    //                                                   /*noise?*/true, /*plot?*/PlotRMfit);
                                                            
    //       measures_noise[_toa_cfd]   = std::get<0>(toa_cf);
    //       measures_noise[_dvdt_cfd]  = 1e-6 * std::get<1>(toa_cf);
    //       measures_noise[_toa_rm]    = std::get<0>(rm) ;
    //       measures_noise[_vonth_rm]  = std::get<1>(rm);
    //       measures_noise[_dvdt_rm]   =  1e-6 * std::get<2>(rm);
          
    //     }
    //   }


  return 0;
}

#endif






