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

// #define TCODE_ENABLE

// STD
#include <iostream>
#include <fstream>
#include <assert.h>
#include <time.h>
#include <chrono>
#include <vector>
#include <unistd.h>
#include <csignal>

 
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
#include <TObjArray.h>
#include <TObjString.h>
#include <TFitResultPtr.h>
#include <TFitResult.h>
#include <TStyle.h>
#include <TRandom3.h>
#include <TLine.h>
#include <TProfile.h>

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
#include <tfboost/functions/DoConvolution.h>
#include <tfboost/functions/ExpModifiedGaussian.h>
#include <tfboost/functions/ButterworthFilter.h>
#include <tfboost/noise/Noise.h>
#include <tfboost/ReadConvolution.h>
#include <tfboost/Algorithms.h>
#include <tfboost/ConfigParser.h>
#include <tfboost/Logger.h>

namespace libconf = libconfig;






int main(int argv, char** argc)
{
  tfboost::TFBoostHeader();

  std::cout.precision(10);

  
  /* ----------------------------------------------
   * Get Configuration
   * --------------------------------------------*/
  libconf::Config Cfg;
  Cfg.readFile("../etc/configuration.cfg");
  const libconf::Setting& cfg_root  = Cfg.getRoot();
  
  TString TransferFunction  = (const char*) cfg_root["TransferFunction"];
  const libconf::Setting& cfg_tf  = cfg_root[TransferFunction];
  
  tfboost::ConfigParser    c(cfg_root);
  tfboost::HistConfigParser  hc(cfg_root);
  
  size_t us_delay = 1e3 * c.DelayMonitoring;
  bool dodelay = (us_delay!=0);
  
  
  /* ----------------------------------------------
   * Initialization of variables and RNGs
   * --------------------------------------------*/
  tfboost::CreateDirectories( c.OutputDirectory + "plots");
  tfboost::CreateDirectories( c.OutputDirectory + "data");
  
  auto& LOG = tfboost::Logger::getInstance( (c.OutputDirectory+TString("LOG.log")).Data() );
  LOG.PrintConfig(c);

  const double min     = 0.0;
  const double max     = (c.Nsamples-1) * c.dT;
  const double minplot   = c.minplot;
  const double maxplot   = c.maxplot;
  const double min_kernel  = -0.5*(max-min);
  const double max_kernel  =  0.5*(max-min);

  signal_type_h th_jitter_withTOA(120, 0.0);
  
  size_t INDEX = 0;
  
  hydra::SeedRNG S{};
  hydra::default_random_engine engine( S() );
  TRandom3 root_rng( S() );
  
  tfboost::algo::CorrectTOA correct_tot(c.TOT_a , c.TOT_b);
  
  
  /* ----------------------------------------------
   * Initialization of histograms
   * --------------------------------------------*/
  TH1D            hist_TOA("hist_TOA","Time of arrival Leading Edge", hc.TOALE_Nbins, hc.TOALE_min, hc.TOALE_max);
  TH1D         hist_TOACFD("hist_TOACFD","Time of arrival CFD at 0.35 of V max", hc.TOACFD_Nbins, hc.TOACFD_min, hc.TOACFD_max);
  TH1D          hist_TOARM("hist_TOARM","Time of arrival Ref.Method", hc.TOARM_Nbins, hc.TOARM_min, hc.TOARM_max);
  TH1D    hist_TOACFDnoise("hist_TOACFDnoise","Time of arrival CFD with noise", hc.TOACFDnoise_Nbins, hc.TOACFDnoise_min, hc.TOACFDnoise_max);
  TH1D     hist_TOALEnoise("hist_TOALEnoise","Time of arrival LE with noise", hc.TOALEnoise_Nbins, hc.TOALEnoise_min, hc.TOALEnoise_max);
  TH1D     hist_TOARMnoise("hist_TOARMnoise","Time of arrival RM with noise", hc.TOARMnoise_Nbins, hc.TOARMnoise_min, hc.TOARMnoise_max);
  TH1D           hist_Vmax("hist_Vmax","V on peak", hc.Vmax_Nbins, hc.Vmax_min, hc.Vmax_max);
  TH1D     hist_TimeatVmax("hist_TimeatVmax","Time at V max", hc.TimeatVmax_Nbins, hc.TimeatVmax_min, hc.TimeatVmax_max);
  TH1D     hist_Vmax_noise("hist_Vmax_noise","V on peak with noise",hc.Vmax_noise_Nbins, hc.Vmax_noise_min, hc.Vmax_noise_max);
  TH1D        hist_Vth_CFD("hist_Vth_CFD","V on CFD threshold",hc.Vth_CFD_Nbins, hc.Vth_CFD_min, hc.Vth_CFD_max);
  TH1D         hist_Vth_LE("hist_Vth_LE","V on LE threshold",hc.Vth_LE_Nbins, hc.Vth_LE_min, hc.Vth_LE_max);
  TH1D         hist_Vth_RM("hist_Vth_RM","V on RM threshold",hc.Vth_RM_Nbins, hc.Vth_RM_min, hc.Vth_RM_max);
  TH1D  hist_Vth_CFD_noise("hist_Vth_CFD_noise","V on CFD threshold with noise",hc.Vth_CFD_noise_Nbins, hc.Vth_CFD_noise_min, hc.Vth_CFD_noise_max);
  TH1D   hist_Vth_RM_noise("hist_Vth_RM_noise","V on RM threshold with noise",hc.Vth_RM_noise_Nbins, hc.Vth_RM_noise_min, hc.Vth_RM_noise_max);
  TH1D   hist_Vth_LE_noise("hist_Vth_LE_noise","V on LE threshold with noise",hc.Vth_LE_noise_Nbins, hc.Vth_LE_noise_min, hc.Vth_LE_noise_max);
  TH1D        hist_dVdt_LE("hist_dVdt_LE","Slope (dV/dT) on LE threshold",hc.dVdt_LE_Nbins, hc.dVdt_LE_min, hc.dVdt_LE_max);
  TH1D       hist_dVdt_CFD("hist_dVdt_CFD","Slope (dV/dT) on CFD threshold",hc.dVdt_CFD_Nbins, hc.dVdt_CFD_min, hc.dVdt_CFD_max);
  TH1D        hist_dVdt_RM("hist_dVdt_RM","Slope (dV/dT) on RM threshold",hc.dVdt_RM_Nbins, hc.dVdt_RM_min, hc.dVdt_RM_max);
  TH1D hist_dVdt_CFD_noise("hist_dVdt_CFD_noise","Slope (dV/dT) on CFD threshold with noise",hc.dVdt_CFD_noise_Nbins, hc.dVdt_CFD_noise_min, hc.dVdt_CFD_noise_max);
  TH1D  hist_dVdt_LE_noise("hist_dVdt_LE_noise","Slope (dV/dT) on LE threshold with noise",hc.dVdt_LE_noise_Nbins, hc.dVdt_LE_noise_min, hc.dVdt_LE_noise_max);
  TH1D  hist_VthRMoverVmax("hist_VthRMoverVmax","Vth RM over Vmax with noise",hc.VthRMoverVmax_Nbins, hc.VthRMoverVmax_min, hc.VthRMoverVmax_max);
  
  TH1D            hist_TOT("hist_TOT","Time over threshold", hc.TOT_Nbins, hc.TOT_min, hc.TOT_max);
  TH2D       hist_TOTvsTOA("hist_TOTvsTOA","TOTvsTOA",  hc.TOT_Nbins, hc.TOT_min, hc.TOT_max, hc.TOALE_Nbins, hc.TOALE_min, hc.TOALE_max);
  TH2D      hist_TOTvsVmax("hist_TOTvsVmax","hist_TOTvsVmax",  hc.TOT_Nbins, hc.TOT_min, hc.TOT_max, hc.Vmax_Nbins, hc.Vmax_min, hc.Vmax_max);

  TH1D hist_convol("hist_convol;Time[s];Vout [V]","hist_convol", c.Nsamples, minplot, maxplot );
  TH1D hist_signal("hist_signal;Time[s];Vout [V]","hist_signal", c.Nsamples, minplot, maxplot );
  TH1D hist_kernel("hist_kernel;Time[s];Vout [V]","hist_kernel", c.Nsamples/30, minplot, maxplot );

#ifdef TCODE_ENABLE
  TH2D *TOAmaps   = new TH2D("TOAmaps","TOAmaps", 100, 0, 56, 100, 0, 56);
  TH2D *Vmaxmaps  = new TH2D("Vmaxmaps","Vmaxmaps", 100, 0, 56, 100, 0, 56);
#endif
  
  /* ----------------------------------------------
   * Preparing the list of input files and
   * related variables
   * --------------------------------------------*/
  TList* listoffiles = tfboost::GetFileList(c.InputDirectory); 
  listoffiles->Sort();
  TIter nextfile( listoffiles );
  TSystemFile *currentfile;
  TString currentfilename;
  TString line;



  /*-----------------------------------------------
   * Check if the preload of a Tr.Function
   * has to be done
   *-----------------------------------------------*/
   signal_type_h time_tf;
   signal_type_h current_tf;
   
  if(c.ID==5){
     int Nskip = (int) cfg_tf["NlinesToSkip"];
     TString tf_infile = (const char*) cfg_tf["TFFile"];
     
     tfboost::ReadTF( tf_infile, Nskip, 
                      time_tf, current_tf, 
                      /*scaling*/1.0, /*double range?*/true);
   }


  /* ----------------------------------------------
   * Main loop on input files
   * --------------------------------------------*/
  while( (currentfile = (TSystemFile*) nextfile() ) && INDEX < c.Nfiles )
  {
    
    currentfilename = currentfile->GetName();
    if(c.UseSameCurve) currentfilename = c.SingleFile;

    
    if ( currentfile->IsDirectory() || !currentfilename.EndsWith(c.InputFileExtension) ) continue;
    
    std::cout << "========================================" << "\n";
    std::cout << "|  FILE  : "<<currentfilename<<" " << "\n";
    std::cout << "|  INDEX : "<<INDEX<<" "       << "\n";
    std::cout << "========================================" << "\n\n";
    ++INDEX;
    
    
    
    //Clean the configuration
    c.ResetInitialValues();
  
    //Declare and prepare the containers
    signal_type_h time;
    signal_type_h idx;
    signal_type_h current;
     
    time.reserve(c.Nsamples);
    idx.reserve(c.Nsamples);
    current.reserve(c.Nsamples);
  
    for(size_t k=0; k < c.offset; ++k)
    {
      idx.push_back(k);
      time.push_back(k * c.dT);
      current.push_back(0.0);
    }
    
    
#ifdef TCODE_ENABLE
    std::pair<double,double> pos = tfboost::tcode::GetHitPosition2(currentfilename);
    double pos_x = pos.first;
    double pos_y = pos.second;
    //if(pos_y<56) continue;
#endif


    std::ifstream myFile( (c.InputDirectory+currentfilename).Data() );
    
    for(size_t j=0; j < c.NlinesToSkip; ++j) line.ReadLine(myFile);
    

    // Actual loop on file lines
    // Fill the containers
    
    double landau = root_rng.Landau(1,0.15);
    if (myFile.is_open()) 
    {
      size_t s = c.offset;

      for(size_t j = c.offset; j < c.Nsamples ; ++j)
      {
        line.ReadLine(myFile);
        if (!myFile.good()) break;
      
        TObjArray *tokens = line.Tokenize( c.token.Data() );
        //DEBUG("ARRAY1: " << tokens->GetEntriesFast() )
      
        TString data_str  = ((TObjString*) tokens->At( c.column ) )->GetString();
        if(data_str == "0" && s==c.offset) continue; //to avoid misaligned TCoDe input files 
        
        const double data = c.LandauFluctuation? landau*atof(data_str) : atof(data_str);
         
        idx.push_back(s);
        time.push_back( s * c.dT);
        current.push_back(data);
        
        tokens->Delete();
        delete tokens;

        ++s;
      }
    } 
    
    SAFE_EXIT( !myFile.is_open() , "In analysis.inl: input file cannot be open. ")
        
    myFile.close();

    if(c.Nsamples - current.size() > 0)
    {
      for(size_t k=current.size(); k<c.Nsamples; ++k)
      {
        idx.push_back(k);
        time.push_back( k * c.dT);
        current.push_back(0.0);
      }
    }
    
    SAFE_EXIT( current.size() != c.Nsamples , "In analysis.inl: size of container not equal to Nsamples. ")



    /* ----------------------------------------------
     * Time Reference resolution
     * --------------------------------------------*/ 
    if(c.TimeReferenceResolution){
      double TR_res = root_rng.Gaus( 0.0 , 12e-12);
      for(auto& t : time ) t += TR_res;}
    


    /* ----------------------------------------------
     * Final signal curve
     * --------------------------------------------*/ 
    signal_type_d time_d(time.size());
    signal_type_d current_d(current.size());
    hydra::copy(time, time_d);
    hydra::copy(current, current_d);

    auto signal   = hydra::make_spiline<double>(time, current );
    auto signal_d = hydra::make_spiline<double>(time_d, current_d );

    if(c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave)
      for(size_t i=1;  i < (size_t) c.Nsamples+1; ++i)
        hist_signal.SetBinContent(i, signal(hist_signal.GetBinCenter(i)) );
    
    
    
    
    /* ----------------------------------------------
     * Performing the convolution
     * --------------------------------------------*/
    #if HYDRA_DEVICE_SYSTEM!=CUDA
    auto fft_backend = hydra::fft::fftw_f64;
    #endif
    
    #if HYDRA_DEVICE_SYSTEM==CUDA
    auto fft_backend = hydra::fft::cufft_f64;
    #endif
    
    signal_type_h conv_data_h(c.Nsamples);

    
    if(!c.MakeConvolution){
    
      conv_data_h.clear();
      tfboost::ReadConvolution( c.conv_inputfile, conv_data_h, c.Nsamples);
      
    } else {

      switch(c.ID) 
      {
        case 0:{
          auto kernel = tfboost::TIA_MOS<double>( cfg_tf );
          SAFE_EXIT(kernel.IDX != c.ID , "Wrong Tr.Function configuration ID.")
          tfboost::Do_Convolution(fft_backend, kernel, signal_d, conv_data_h, min, max, c.Nsamples);
          
          if(c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave) 
            for(size_t i=1;  i < (size_t) c.Nsamples+1; ++i) 
              hist_kernel.SetBinContent(i, kernel(hist_kernel.GetBinCenter(i)) );
          
          }break;

        case 1:{
          auto kernel = tfboost::TIA_BJT_2stages<double>( cfg_tf );
          SAFE_EXIT(kernel.IDX != c.ID , "Wrong Tr.Function configuration ID.")
          tfboost::Do_Convolution(fft_backend, kernel, signal_d, conv_data_h, min, max, c.Nsamples);
          
          if(c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave) 
            for(size_t i=1;  i < (size_t) c.Nsamples+1; ++i) 
              hist_kernel.SetBinContent(i, kernel(hist_kernel.GetBinCenter(i)) );  
          
          }break;

        case 2:{
          auto kernel = tfboost::TIA_BJT_2stages_GM<double>( cfg_tf );
          SAFE_EXIT(kernel.IDX != c.ID , "Wrong Tr.Function configuration ID.")
          tfboost::Do_Convolution(fft_backend, kernel, signal_d, conv_data_h, min, max, c.Nsamples);
          
          if(c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave) 
            for(size_t i=1;  i < (size_t) c.Nsamples+1; ++i) 
              hist_kernel.SetBinContent(i, kernel(hist_kernel.GetBinCenter(i)) );  
          
          }break;

        case 3:{
          auto kernel = tfboost::TIA_BJT_1stage<double>( cfg_tf );
          SAFE_EXIT(kernel.IDX != c.ID , "Wrong Tr.Function configuration ID.")
          tfboost::Do_Convolution(fft_backend, kernel, signal_d, conv_data_h, min, max, c.Nsamples);
         
          if(c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave) 
            for(size_t i=1;  i < (size_t) c.Nsamples+1; ++i) 
              hist_kernel.SetBinContent(i, kernel(hist_kernel.GetBinCenter(i)) );  
          
          }break;

        case 4:{
          auto kernel = tfboost::TIA_IdealInt<double>( cfg_tf );
          SAFE_EXIT(kernel.IDX != c.ID , "Wrong Tr.Function configuration ID.")
          tfboost::Do_Convolution(fft_backend, kernel, signal_d, conv_data_h, min, max, c.Nsamples);
          
          if(c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave) 
            for(size_t i=1;  i < (size_t) c.Nsamples+1; ++i) 
              hist_kernel.SetBinContent(i, kernel(hist_kernel.GetBinCenter(i)) );  
          
          }break;

        case 5:{

          auto kernel = hydra::make_spiline<double>(time_tf, current_tf );
          
          tfboost::Do_Convolution(fft_backend, kernel, signal, conv_data_h, min, max, c.Nsamples);

          if(c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave) 
            for(size_t i=1;  i < (size_t) c.Nsamples+1; ++i) 
              hist_kernel.SetBinContent(i, kernel(hist_kernel.GetBinCenter(i)) );  
          

        } break;

        default:
          SAFE_EXIT( true , "In analysis.inl: bad transfer function ID.")
          
      }//end switch


     /*-------------------------------------------------
      * Digitization of the signal
      * This step is done only if we are requesting noise
      *------------------------------------------------*/
      if(c.MakeDigitization && !c.AddNoise)
      {
        tfboost::algo::DigitizeSignal( conv_data_h, time, 
                                       c.sampling_dT, max, 
                                       engine, c.randomphase);

        // Override all the conv information
        c.Nsamples = conv_data_h.size();
        c.dT = c.sampling_dT;
        hist_convol.SetBins(c.Nsamples, min, maxplot);
      }
      

    } // end MakeConvolution
      


    /* ----------------------------------------------
     * Filling histogram for visualization
     * --------------------------------------------*/ 
    auto conv_spline = hydra::make_spiline<double>(time, conv_data_h );

    if(c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave)
      for(size_t i=1;  i < (size_t) c.Nsamples+1; ++i)
         hist_convol.SetBinContent(i, conv_spline(hist_convol.GetBinCenter(i)) );  
         
    
    
    /* ----------------------------------------------
     * Measurements without noise
     * --------------------------------------------*/

    // Condition to avoid to process empty signal
    // reject signal with amplitude < 1 mV
    if( tfboost::algo::LeadingEdge(conv_data_h, c.LE_reject_nonoise) ==  conv_data_h.size() ) 
    { 
      WARNING_LINE("Skipping empty event...")
      continue;
    }


    size_t TOA_LE     = tfboost::algo::LeadingEdge(conv_data_h, c.LEthr);

    size_t timeatth_0 = tfboost::algo::GetTimeAtPeak(conv_data_h);
    
    double Vpeak      = tfboost::algo::GetVAtPeak(conv_data_h);
    
    size_t TOA_CFD    = tfboost::algo::ConstantFraction(conv_data_h , c.CFD_fr , Vpeak);
    
    size_t TOA_RM     = std::get<2>( tfboost::algo::TimeRefMethod( conv_data_h, 
                                                                  time, Vpeak, c.RM_delay, c.bound_fit,
                                                                   /*noise?*/false, /*plot?*/false ) ) ;
    
    double dvdt_CFD   = tfboost::algo::SlopeOnThrs(conv_data_h, TOA_CFD);
    
    double dvdt_LE    = tfboost::algo::SlopeOnThrs(conv_data_h, TOA_LE);
    
    double dvdt_RM    = tfboost::algo::SlopeOnThrs(conv_data_h, TOA_RM);
    
    double VonThLE    = conv_data_h[TOA_LE];
    
    double VonThCFD   = conv_data_h[TOA_CFD];
    
    double VonThRM    = conv_data_h[TOA_RM];
    
    double TOT        = tfboost::algo::TimeOverThr(conv_data_h, time, c.LEthr, c.LEthr);

    //TOAmaps->SetBinContent( pos_y+1, pos_x+1, ::abs(time[TOA_CFD]-0.2432374637e-9)/7.47e-11 );
    //Vmaxmaps->SetBinContent( pos_y+1, pos_x+1, 1e3 * Vpeak );
 
    
    if(c.MakeTheoreticalTOA){
      auto prob_curve = tfboost::noise::ComputeTOAcurve( TOA_CFD, TOA_CFD-100, TOA_CFD+100, 
                                                         c.CFD_fr, Vpeak, c.sigma_noise, 
                                                         idx, conv_data_h, "th_jitter.pdf");
                                                         
      for(size_t i=0; i<prob_curve.size(); ++i) th_jitter_withTOA[i] += prob_curve[i];
    }

    hist_TOA        .Fill( c.TOTcorrection? correct_tot(TOT) : time[TOA_LE] ); 
    hist_TOACFD     .Fill( time[TOA_CFD] ); 
    hist_TOARM      .Fill( time[TOA_RM] ); 
    hist_Vmax       .Fill( 1e3 * Vpeak);
    hist_TimeatVmax .Fill( time[timeatth_0] );
    hist_Vth_CFD    .Fill( 1e3*VonThCFD);
    hist_Vth_LE     .Fill( 1e3*VonThLE);
    hist_Vth_RM     .Fill( 1e3*VonThRM);
    hist_dVdt_CFD   .Fill( 1e-6 * dvdt_CFD/c.dT);
    hist_dVdt_LE    .Fill( 1e-6 * dvdt_LE/c.dT);
    hist_dVdt_RM    .Fill( 1e-6 * dvdt_RM/c.dT);
    hist_TOT        .Fill(TOT);
    hist_TOTvsTOA   .Fill(TOT, time[TOA_LE]);
    hist_TOTvsVmax  .Fill(TOT, Vpeak);

    std::cout << "========================================" << "\n";
    std::cout << "Measurements without noise:             " << "\n";
    std::cout << "Time on thresholds (LE)  = " << time[TOA_LE]  << "  (s)" << "\n";
    std::cout << "Time on thresholds (CFD) = " << time[TOA_CFD] << "  (s)" << "\n";
    std::cout << "Time on thresholds (RM)  = " << time[TOA_RM]  << "  (s)" << "\n";
    std::cout << "V on thresholds (CFD)    = " << 1e3*VonThCFD  << "  (mV)"<< "\n";
    std::cout << "V on thresholds (LE)     = " << 1e3*VonThLE   << "  (mV)"<< "\n";
    std::cout << "V on thresholds (RM)     = " << 1e3*VonThRM   << "  (mV)"<< "\n";
    std::cout << "Vpeak                    = " << 1e3*Vpeak     << "  (mV)"<< "\n";
    std::cout << "dv/dt (CFD)              = " << 1e-6*dvdt_CFD/c.dT << "  (uV/ps)"<< "\n";
    std::cout << "dv/dt (LE)               = " << 1e-6*dvdt_LE/c.dT  << "  (uV/ps)"<< "\n";  
    std::cout << "dv/dt (RM)               = " << 1e-6*dvdt_RM/c.dT  << "  (uV/ps)"<< "\n";  
    std::cout << "========================================" << "\n";




    /*-------------------------------------------------
     * Measurements with noise
     *------------------------------------------------*/
    if(c.AddNoise)
    {
      auto noise = tfboost::noise::Noise(c.sigma_noise, c.UseRedNoise, c.r_rednoise);
      
      noise.AddNoiseToSignal( conv_data_h, S() );

      // initialize measurments
      size_t timeatmax=0, TOA_LE_noise_idx=0, TOA_CFD_noise_idx=0, TOA_RM_noise_idx=0;
      double vmax=0.0, TOA_LE_noise=0.0, TOA_CFD_noise=0.0, TOA_RM_noise=0.0;
      double VonThCFD_noise=0.0, VonThRM_noise=0.0, VonThLE_noise=0.0, VthRMoverVmax=0.0;;
      double dvdt_CFD_noise=0.0, dvdt_LE_noise=0.0, TOTnoise=0.0;
      
      
     /*-------------------------------------------------
      * Digitization of the signal
      * This step is done only if we are requesting noise
      *------------------------------------------------*/ 
      if(c.MakeDigitization)
      {
        tfboost::algo::DigitizeSignal( conv_data_h, time, 
                                       c.sampling_dT, max, 
                                       engine, c.randomphase);

        // Override all the conv information
        c.Nsamples = conv_data_h.size();
        c.dT = c.sampling_dT;
        hist_convol.SetBins(c.Nsamples, min, maxplot);
      }
      
      
      
      // Condition to avoid to process empty signal
      // reject signal with amplitude < 1 mV
      if( tfboost::algo::LeadingEdge(conv_data_h, c.LE_reject_noise) == conv_data_h.size() ) 
        { WARNING_LINE("Skipping empty event...") continue; }
      
      
      
      timeatmax         = tfboost::algo::GetTimeAtPeak(conv_data_h);
      TOA_LE_noise_idx  = tfboost::algo::LeadingEdge(conv_data_h , c.LEthr);
      TOA_LE_noise      = time[TOA_LE_noise_idx];
      VonThLE_noise     = conv_data_h[TOA_LE_noise_idx];
      TOTnoise          = tfboost::algo::TimeOverThr(conv_data_h, time, c.LEthr, c.LEthr);
      
      
      if(c.MakeLinearFitNearThreshold && TOA_LE>1)
      {
        auto toa      = tfboost::algo::LinearFitNearThr( c.LEthr, conv_data_h, time, c.bound_fit, c.PlotLinFit, "LEfit");
        TOA_LE_noise  = toa.first ;
        dvdt_LE_noise = toa.second ;
      }
      
      
      
      if(c.MakeGaussianFitNearVmax && TOA_CFD>1)
      {

        auto gfit = tfboost::algo::GaussianFitNearVmax( conv_data_h, time, c.bound_fit, c.PlotGausFit );
        vmax      = gfit.first ;
        timeatmax = gfit.second ;

        TOA_CFD_noise_idx = tfboost::algo::ConstantFraction(conv_data_h , c.CFD_fr , vmax);
        TOA_CFD_noise     = time[TOA_CFD_noise_idx];
        VonThCFD_noise    = conv_data_h[ TOA_CFD_noise_idx ];

        if(c.MakeLinearFitNearThreshold && TOA_LE>1)
        {
          auto toa       = tfboost::algo::LinearFitNearThr( c.CFD_fr*vmax, 
                                                            conv_data_h, time, c.bound_fit, 
                                                            /*plot?*/c.PlotLinFit, "CFDfit");
          TOA_CFD_noise  = toa.first ;
          dvdt_CFD_noise = toa.second ;
          
          auto rm           = tfboost::algo::TimeRefMethod( conv_data_h, time, 
                                                            vmax, c.RM_delay, c.bound_fit, 
                                                            /*noise?*/true, /*plot?*/c.PlotRMfit);
          TOA_RM_noise_idx  = std::get<2>(rm); 
          TOA_RM_noise      = std::get<0>(rm);
          VthRMoverVmax     = std::get<1>(rm);
          VonThRM_noise     = conv_data_h[ TOA_RM_noise_idx ];
        }

      }

      //if(TOA_RM_noise<0 || TOA_CFD_noise<0) continue;

      hist_TOALEnoise    .Fill( c.TOTcorrection?  correct_tot(TOTnoise) : TOA_LE_noise );
      hist_Vth_LE_noise  .Fill( VonThLE_noise );
      hist_TOARMnoise    .Fill( TOA_RM_noise );
      hist_TOACFDnoise   .Fill( TOA_CFD_noise );
      hist_Vmax_noise    .Fill( vmax );
      hist_Vth_CFD_noise .Fill( VonThCFD_noise );
      hist_Vth_RM_noise  .Fill( VonThRM_noise );
      hist_VthRMoverVmax .Fill( VthRMoverVmax );
      hist_dVdt_CFD_noise .Fill( 1e-6*dvdt_CFD_noise );
      hist_dVdt_LE_noise  .Fill( 1e-6*dvdt_LE_noise );
      
      #ifdef TCODE_ENABLE
      TOAmaps->SetBinContent( TOAmaps->FindBin( pos_x, pos_y), TOA_CFD_noise ); 
      Vmaxmaps->SetBinContent( Vmaxmaps->FindBin( pos_x, pos_y), vmax ); 
      #endif


      std::cout << "========================================" << "\n";
      std::cout << "Measurements with noise:                " << "\n";
      std::cout << "Time on thresholds (LE)  = " << TOA_LE_noise  << "  (s)" << "\n";
      std::cout << "Time on thresholds (CFD) = " << TOA_CFD_noise << "  (s)" << "\n";
      std::cout << "Time on thresholds (RM)  = " << TOA_RM_noise  << "  (s)" << "\n";
      std::cout << "V on thresholds (CFD)    = " << 1e3*VonThCFD_noise  << "  (mV)"<< "\n";
      std::cout << "V on thresholds (LE)     = " << 1e3*VonThLE_noise   << "  (mV)"<< "\n";
      std::cout << "V on thresholds (RM)     = " << 1e3*VonThRM_noise   << "  (mV)"<< "\n";
      std::cout << "Vpeak                    = " << 1e3*vmax   << "  (mV)"<< "\n";
      std::cout << "dv/dt CFD                = " << 1e-6*dvdt_CFD_noise << "  (uV/ps)"<< "\n";
      std::cout << "dv/dt LE                 = " << 1e-6*dvdt_LE_noise  << "  (uV/ps)"<< "\n";  
      std::cout << "========================================" << "\n";

      auto conv_spline = hydra::make_spiline<double>(time, conv_data_h );
      if(c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave)
        for(size_t i=1;  i < (size_t) c.Nsamples+1; ++i)
          hist_convol.SetBinContent(i, conv_spline(hist_convol.GetBinCenter(i)) ); 

    }
   

    if(c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave)
       tfboost::SaveConvolutionCanvas(c.OutputDirectory + "plots/", "hist_convol_functor", 
              hist_convol, hist_signal, hist_kernel);


    if(c.SaveConvDataToFile) 
      tfboost::SaveConvToFile(conv_data_h, time, c.dT, 
                              c.OutputDirectory + "data/" +currentfilename );

    
    if(dodelay) usleep(us_delay);
  
  
  }//end loop on files
  



  /*-------------------------------------------------
   *  Save all the plots
   *------------------------------------------------*/  
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOA_LE",  "Time [s]",    "counts", hist_TOA);
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOA_CFD",   "Time [s]",    "counts", hist_TOACFD);
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOA_RM",  "Time [s]",    "counts", hist_TOARM); 
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOT",     "Time [s]",    "counts", hist_TOT);
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOT_2d",     "Time [s]",    "TOA [s]", hist_TOTvsTOA, "colz"); 
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOTvsVmax",   "TOT [s]",    "Vmax [V]", hist_TOTvsVmax, "colz");
  
  TProfile* prof =  hist_TOTvsVmax.ProfileX();
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOTvsVmax_profile",   "TOT [s]",    "Vmax [V]", *prof);
  
  TProfile* prof2 =  hist_TOTvsTOA.ProfileX();
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOTvsTOA_profile",   "TOT [s]",    "Vmax [V]", *prof2);
  
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "dVdT_LE",   "Slope [uV/ps]", "counts", hist_dVdt_LE);
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "dVdT_CFD",  "Slope [uV/ps]", "counts", hist_dVdt_CFD);
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "dVdT_RM",  "Slope [uV/ps]", "counts", hist_dVdt_RM);

  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "Vmax",    "Voltage [mV]",  "counts", hist_Vmax);
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TimeatVmax","Time [ps]",   "counts", hist_TimeatVmax);
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "Vth_CFD",   "Voltage [mV]",  "counts", hist_Vth_CFD);
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "Vth_LE",  "Voltage [mV]",  "counts", hist_Vth_LE);
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "Vth_RM",  "Voltage [mV]",  "counts", hist_Vth_RM);



  if(c.AddNoise)
  {
     
   /*
   gStyle->SetOptFit(1);
   tfboost::ExpModifiedGaussian f(0.15e-9, 0.6e-9);
   f.SetParFromHist(hist_TOACFDnoise);
   (f.fun)->FixParameter(5,1);
   tfboost::SaveCanvasAndFit(c.OutputDirectory + "plots/", "TOA_CFDnoise", "Time [s]",  "counts", *hist_TOACFDnoise, f.fun );
   */

   
   /*
   tfboost::ExpModifiedGaussian f2(0.15e-9, 0.6e-9);
   f2.SetParFromHist(hist_TOARMnoise);
   (f2.fun)->FixParameter(5,1);
   tfboost::SaveCanvasAndFit(c.OutputDirectory + "plots/", "TOA_RMnoise", "Time [s]",  "counts", *hist_TOARMnoise, f2.fun );
    */

    tfboost::SaveCanvas(c.OutputDirectory + "plots/", "VonTh_LEnoise", "Voltage [mV]", "counts",  hist_Vth_LE_noise);
    tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOA_LEnoise",   "Time [s]",   "counts",  hist_TOALEnoise);
    tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOA_CFDnoise", "Time [s]",  "counts", hist_TOACFDnoise);
    tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOA_RMnoise",  "Time [s]",   "counts", hist_TOARMnoise);
    

  
    tfboost::SaveCanvas(c.OutputDirectory + "plots/", "Vmax_noise",   "Voltage [mV]", "counts", hist_Vmax_noise);
    tfboost::SaveCanvas(c.OutputDirectory + "plots/", "VonTh_CFDnoise", "Voltage [mV]", "counts", hist_Vth_CFD_noise);
    tfboost::SaveCanvas(c.OutputDirectory + "plots/", "VonTh_RMnoise", "Voltage [mV]", "counts",  hist_Vth_RM_noise);
    tfboost::SaveCanvas(c.OutputDirectory + "plots/", "hist_VthRMoverVmax", "Frac. of Vmax", "counts",  hist_VthRMoverVmax);
   
    tfboost::SaveCanvas(c.OutputDirectory + "plots/", "dVdT_CFDnoise", "Slope [uV/ps]", "counts", hist_dVdt_CFD_noise);
    tfboost::SaveCanvas(c.OutputDirectory + "plots/", "dVdT_LEnoise",  "Slope [uV/ps]", "counts", hist_dVdt_LE_noise); 
    
  }


  if(c.MakeTheoreticalTOA)
  {
    TGraph hist_thjitter_withTOA;
    
    for(size_t i=0;  i < th_jitter_withTOA.size(); ++i)
      hist_thjitter_withTOA.SetPoint(hist_thjitter_withTOA.GetN(), i+160 , th_jitter_withTOA[i]);;
    
        
    TCanvas canv_thjitter_withTOA("canv_thjitter_withTOA", "canv_thjitter_withTOA", 800, 800);
    hist_thjitter_withTOA.SetTitle("Jitter from first principles;time [ps];A.U.");
    hist_thjitter_withTOA.Draw("APL");
    canv_thjitter_withTOA.SaveAs(c.OutputDirectory + "plots/thjitter_differentcurves.pdf");
  } 
  
  
  LOG.PrintMessage("Number of files analyzed: ", INDEX);
  LOG.Exit();

  
//  gStyle->SetOptStat(0);
//  gStyle->SetPalette(kRainBow);

//  TCanvas canv_TOAmaps("canv_TOAmaps", "canv_TOAmaps", 4*200, 4*200);
//  TOAmaps->SetMinimum(0.15e-9);
//  TOAmaps->SetMaximum(0.5e-9);
//  //TOAmaps->SetMinimum(0.0);
//  TOAmaps->Draw("colz");
//  canv_TOAmaps.SaveAs( c.OutputDirectory + "plots/" + "canv_TOAmaps.pdf");
//    canv_TOAmaps.SaveAs( c.OutputDirectory + "plots/" + "canv_TOAmaps.C");

//  TCanvas canv_Vmaxmaps("canv_Vmaxmaps", "canv_Vmaxmaps", 4*200, 4*200);
//  //Vmaxmaps->SetMinimum(TOAmaps->GetMinimum(0.));
//    Vmaxmaps->SetMinimum(0.0);
//  Vmaxmaps->Draw("colz");
//  canv_Vmaxmaps.SaveAs( c.OutputDirectory + "plots/" +"canv_Vmaxmaps.pdf");
//  

  return 0;
}

#endif






