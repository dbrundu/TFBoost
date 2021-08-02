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



#ifndef TFBOOST_ANALYSIS_INL_
#define TFBOOST_ANALYSIS_INL_


// STD
#include <iostream>
#include <fstream>
#include <assert.h>
#include <time.h>
#include <chrono>
#include <vector>
#include <unistd.h>
#include <csignal>

 
// HYDRA & ROOT
#include <tfboost/detail/external/HydraHeaders.h>
#include <tfboost/detail/external/RootHeaders.h>

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
#include <tfboost/functions/ExpModifiedGaussian.h>
#include <tfboost/functions/ButterworthFilter.h>
#include <tfboost/DoConvolution.h>
#include <tfboost/Noise.h>
#include <tfboost/InputOutput.h>
#include <tfboost/Algorithms.h>
#include <tfboost/Digitizer.h>
#include <tfboost/ConfigParser.h>
#include <tfboost/Logger.h>
#include <tfboost/Histograms.h>

namespace libconf = libconfig;






int main(int argv, char** argc)
{

  auto main_start = std::chrono::high_resolution_clock::now();

  tfboost::TFBoostHeader();

  std::cout.precision(10);


  /* ----------------------------------------------
   * Get Configuration
   * Instatiante Config parser and 
   * Hist Config parser 
   * --------------------------------------------*/
  libconf::Config Cfg;
  Cfg.readFile("../etc/configuration.cfg");
  const libconf::Setting& cfg_root  = Cfg.getRoot();
  
  TString TransferFunction        = (const char*) cfg_root["TransferFunction"];
  const libconf::Setting& cfg_tf  = cfg_root[TransferFunction];
  
  const tfboost::ConfigParser      c(cfg_root);
  const tfboost::HistConfigParser  hc(cfg_root);
  
  size_t us_delay = 1e3 * c.DelayMonitoring;
  bool   dodelay  = (us_delay!=0);
  
  
  
  
  /* ----------------------------------------------
   * Initialization of variables, RNGs
   * and algorithm objects
   * --------------------------------------------*/
  tfboost::CreateDirectories( c.OutputDirectory + "plots");
  tfboost::CreateDirectories( c.OutputDirectory + "data");
  tfboost::CreateDirectories( "monitor");
  
  auto& LOG = tfboost::Logger::getInstance( (c.OutputDirectory+TString("LOG.log")).Data() );
  LOG.PrintConfig(c);

  const double SampRate    = 1./c.dT;
  const double min         = 0.0;
  const double max         = (c.Nsamples-1) * c.dT;
  const double minplot     = c.minplot;
  const double maxplot     = c.maxplot;
  const double min_kernel  = -0.5*(max-min);
  const double max_kernel  =  0.5*(max-min);

  HostSignal_t th_jitter_withTOA(120, 0.0);
  
  size_t INDEX = 0;
  
  hydra::SeedRNG S{};
  hydra::default_random_engine engine( S() ); 
  TRandom3 root_rng( S() );
  
  
  
  /* ----------------------------------------------
   * Initialization of measurements
   * --------------------------------------------*/
  Measures_t init = tfboost::MeasuresInitializer::get_values();
  Measures_t measures( init );
  Measures_t measures_noise( init );
  
 
 
  /* ----------------------------------------------
   * Initialization of histograms
   * --------------------------------------------*/
  auto& histograms = tfboost::HistogramsManager::getInstance();
  histograms.SetConfig(hc);
  
  TH2D hist_TOTvsTOA("hist_TOTvsTOA","TOTvsTOA",  100, 0, -1, 100, 0, -1);
  TH2D hist_TOTvsVmax("hist_TOTvsVmax","hist_TOTvsVmax",  100, 0, -1, 100, 0, -1);

  TH1D hist_convol("hist_convol;Time[s];Vout [V]","hist_convol", c.Nsamples, minplot, maxplot );
  TH1D hist_signal("hist_signal;Time[s];Vout [V]","hist_signal", c.Nsamples, minplot, maxplot );
  TH1D hist_kernel("hist_kernel;Time[s];Vout [V]","hist_kernel", c.Nsamples/30, minplot, maxplot );

  TH1D hist_rms_noise("hist_rms_noise;RMS[V];Counts","hist_rms_noise", 100, 0, -1 );



#if TCODE_ENABLE==true
  //TH2D *TOAmaps   = new TH2D("TOAmaps","TOAmaps",   100, 0, TCODE_PIXEL_YMAX, 100, 0, TCODE_PIXEL_XMAX);
  //TH2D *Vmaxmaps  = new TH2D("Vmaxmaps","Vmaxmaps", 100, 0, TCODE_PIXEL_YMAX, 100, 0, TCODE_PIXEL_XMAX);
  TGraph2D *TOAmaps = new TGraph2D();
  TGraph2D *Vmaxmaps = new TGraph2D();
#endif
  
  
  
  
  /* ----------------------------------------------
   * Preparing the list of input files and
   * related variables for the main loop
   * --------------------------------------------*/
  TList* listoffiles = tfboost::GetFileList(c.InputDirectory); 
  //listoffiles->Sort();
  TIter nextfile( listoffiles );
  TSystemFile *currentfile;
  TString currentfilename;
  TString line;

  /* ----------------------------------------------
   * Preparing the list of noise input files 
   * --------------------------------------------*/
  TList* listofnoisefiles = tfboost::GetFileList(c.NoiseDirectory); 
  TIter nextnoisefile( listofnoisefiles );



  /*-----------------------------------------------
   * Check if the a custom Tr.Function
   * by the user has to be preloaded
   *-----------------------------------------------*/
  HostSignal_t time_tf;
  HostSignal_t current_tf;
   
  if(TransferFunction=="FromFile")
  {
     int Nskip = (int) cfg_tf["NlinesToSkip"];
     
     TString tf_infile = (const char*) cfg_tf["TFFile"];
     
     tfboost::ReadTF( tf_infile, Nskip, time_tf, current_tf, /*scaling*/1.0, /*double range?*/true);
  }
   



  /* ----------------------------------------------
   * Main loop on input files
   * --------------------------------------------*/
  while( (currentfile = (TSystemFile*) nextfile() ) && INDEX < c.Nfiles )
  {
    
    auto start = std::chrono::high_resolution_clock::now();

    currentfilename = currentfile->GetName();
    if(c.UseSameCurve) currentfilename = c.SingleFile;

    if ( currentfile->IsDirectory() || !currentfilename.EndsWith(c.InputFileExtension) ) continue;
    
    RULE_LINE;
    std::cout << "| "<< _START_INFO_ <<"FILE"<< _END_INFO_ << "  : "<<currentfilename<<" " << "\n";
    std::cout << "| "<< _START_INFO_ <<"INDEX"<< _END_INFO_ << " : "<<INDEX<<" "           << "\n";
    RULE_LINE_LIGHT;
    
    ++INDEX;
    
    const bool PlotConv = c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave;
    

    //Clean the configuration
    c.ResetInitialValues();
  
    //Declare and prepare the containers
    HostSignal_t time;     time.reserve(c.Nsamples);
    HostSignal_t idx;      idx.reserve(c.Nsamples);
    HostSignal_t current;  current.reserve(c.Nsamples);
     
    //Fill containers until offset
    for(size_t k=0; k < c.offset; ++k) {
      idx.push_back(k);
      time.push_back(k * c.dT);
      current.push_back(0.0); }
    
    
#if TCODE_ENABLE==true
    std::pair<double,double> pos = tfboost::tcode::GetHitPosition<TCODE_SELECT_POSFUNC>(currentfilename);
    double pos_x = pos.first;
    double pos_y = pos.second;
#endif


    std::ifstream myFile( (c.InputDirectory+currentfilename).Data() );
    SAFE_EXIT( !myFile.is_open() , "In analysis.inl: input file cannot be open. ")
    
    //Read file N times to skip lines
    for(size_t j=0; j < c.NlinesToSkip; ++j) 
      line.ReadLine(myFile);
    
    
    // preload a Landau Fluctuation factor
    double landau = c.LandauFluctuation? 
                    root_rng.Landau(c.landaufactor_mean, c.landaufactor_sigma) : 1.0;


    // Actual loop on file lines, fill all the containers
    size_t s = c.offset;
    for(size_t j = c.offset; j < c.Nsamples ; ++j)
    {
      line.ReadLine(myFile);
      if (!myFile.good()) break;
      
      TObjArray *tokens = line.Tokenize( c.token.Data() );
      
      TString data_str  = ((TObjString*) tokens->At( c.column ) )->GetString();
      //if(data_str == "0" && s==c.offset) continue; //--> to avoid misaligned TCoDe input files 
        
      double data = landau*atof(data_str);//landau*atof(data_str);
         
      idx.push_back(s);
      time.push_back( s * c.dT);
      current.push_back(data);
      
      tokens->Delete();
      delete tokens;

      ++s;
    }
    
    myFile.close();


    //continue to fill containers until Nsamples size
    for(size_t k=current.size(); k<c.Nsamples; ++k) {
      idx.push_back(k);
      time.push_back( k * c.dT);
      current.push_back(0.0); }

    SAFE_EXIT( current.size() != c.Nsamples , "In analysis.inl: size of container not equal to Nsamples. ")



    /* ----------------------------------------------
     * Time Reference resolution
     * --------------------------------------------*/ 
    double TR_res = c.TimeReferenceResolution? root_rng.Gaus( 0.0 , c.timeref_sigma) : 0.0;
    


    /* ----------------------------------------------
     * Final signal curve
     * --------------------------------------------*/ 
    DevSignal_t time_d(time.size());
    DevSignal_t current_d(current.size());
    hydra::copy(time, time_d);
    hydra::copy(current, current_d);

    auto signal   = hydra::make_spiline<double>(time, current );
    auto signal_d = hydra::make_spiline<double>(time_d, current_d );
    
    if(PlotConv) tfboost::FillHistWithFunction( hist_signal, signal);
    
    
    
    /* ----------------------------------------------
     * Performing the convolution
     * --------------------------------------------*/
#if HYDRA_DEVICE_SYSTEM!=CUDA
    auto fft_backend = hydra::fft::fftw_f64;
#endif
    
#if HYDRA_DEVICE_SYSTEM==CUDA
    auto fft_backend = hydra::fft::cufft_f64;
#endif
    
    HostSignal_t conv_data_h(c.Nsamples);

    
    if(!c.MakeConvolution){
      // The input signals are already the convoluted ones
      // so skip the convolution and copy the input signals
      // in conv_data_h
      hydra::copy(current, conv_data_h);
      
    } else {

      switch(c.ID) 
      {
        case 0:{
          auto kernel = tfboost::TIA_MOS<double>( cfg_tf );
          tfboost::Do_Convolution(fft_backend, kernel, signal_d, conv_data_h, min, max, c.Nsamples);
          if(PlotConv) tfboost::FillHistWithFunction( hist_kernel, kernel); 
          }break;

        case 1:{
          auto kernel = tfboost::TIA_BJT_2stages<double>( cfg_tf );
          tfboost::Do_Convolution(fft_backend, kernel, signal_d, conv_data_h, min, max, c.Nsamples);
          if(PlotConv) tfboost::FillHistWithFunction( hist_kernel, kernel);
          }break;

        case 2:{
          auto kernel = tfboost::TIA_BJT_2stages_GM<double>( cfg_tf );
          tfboost::Do_Convolution(fft_backend, kernel, signal_d, conv_data_h, min, max, c.Nsamples);
          if(PlotConv) tfboost::FillHistWithFunction( hist_kernel, kernel);
          }break;

        case 3:{
          auto kernel = tfboost::TIA_BJT_1stage<double>( cfg_tf );
          tfboost::Do_Convolution(fft_backend, kernel, signal_d, conv_data_h, min, max, c.Nsamples);
          if(PlotConv) tfboost::FillHistWithFunction( hist_kernel, kernel);
          }break;

        case 4:{
          auto kernel = tfboost::TIA_IdealInt<double>( cfg_tf );
          tfboost::Do_Convolution(fft_backend, kernel, signal_d, conv_data_h, min, max, c.Nsamples);
          if(PlotConv) tfboost::FillHistWithFunction( hist_kernel, kernel);
          }break;

        case 5:{
          auto kernel = hydra::make_spiline<double>(time_tf, current_tf );
          tfboost::Do_Convolution(fft_backend, kernel, signal, conv_data_h, min, max, c.Nsamples);
          if(PlotConv) tfboost::FillHistWithFunction( hist_kernel, kernel);
        } break;

        default:
          SAFE_EXIT( true , "In analysis.inl: bad transfer function ID.")
          
      }//end switch


      // Time Digitization of the signal
      // This step is done only if we are not requesting noise
      if(c.MakeTimeDigitization && !c.DoMeasurementsWithNoise)
      {
        tfboost::TimeDigitizeSignal( conv_data_h, time, c.sampling_dT, max, engine, c.randomphase);

        // Override all the conv information
        c.Nsamples = conv_data_h.size();
        c.dT = c.sampling_dT;
        hist_convol.SetBins(c.Nsamples, min, maxplot);
      }


      // Voltage Digitization of the signal
      // This step is done only if we are not requesting noise
      if(c.MakeVoltageDigitization && !c.DoMeasurementsWithNoise)
        tfboost::VoltageDigitizeSignal( conv_data_h, c.ADCmin, c.ADCmax, c.ADCnbits);

      

    } // end MakeConvolution
      



    /* ----------------------------------------------
     * Filling histogram for visualization
     * --------------------------------------------*/     
    if(PlotConv) {
        auto conv_spline = hydra::make_spiline<double>(time, conv_data_h );
        tfboost::FillHistWithFunction( hist_convol, conv_spline);
    }
    
    
    
    /* ----------------------------------------------
     * Measurements without noise
     * --------------------------------------------*/

    // Condition to avoid to process empty signal
    // reject signal with amplitude < 1 mV
    if( tfboost::algo::LeadingEdge(conv_data_h, c.LE_reject_nonoise) ==  conv_data_h.size() ) { 
        WARNING_LINE("Skipping empty event...")
        continue; }

    size_t TOA_LE         = tfboost::algo::LeadingEdge(conv_data_h, c.LEthr);
    measures[_toa_le]     = time[TOA_LE] ;

    size_t TimeAtPeak     = tfboost::algo::GetTimeAtPeak(conv_data_h);
    measures[_tpeak]      = time[TimeAtPeak] ;
    
    measures[_vpeak]      = tfboost::algo::GetVAtPeak(conv_data_h);
    
    size_t TOA_CFD        = tfboost::algo::ConstantFraction(conv_data_h , c.CFD_fr , measures[_vpeak]);
    measures[_toa_cfd]    = time[TOA_CFD] ;
    
    auto rm_nonoise       = tfboost::algo::TimeRefMethod( conv_data_h, time, measures[_vpeak], c.RM_delay, c.bound_fit,/*noise?*/false, /*plot?*/false );
    measures[_toa_rm]     = std::get<0>( rm_nonoise ) ;
    
    measures[_dvdt_rm]    = 1e-6 * std::get<2>( rm_nonoise ) ;

    measures[_vonth_rm]   =  std::get<1>( rm_nonoise ) ;

    measures[_dvdt_le]    = 1e-6 * SampRate * tfboost::algo::SlopeOnThrs(conv_data_h, TOA_LE);
    
    measures[_dvdt_cfd]   = 1e-6 * SampRate * tfboost::algo::SlopeOnThrs(conv_data_h, TOA_CFD);

    measures[_vonth_le]   = conv_data_h[TOA_LE];
    
    measures[_vonth_cfd]  = conv_data_h[TOA_CFD];
    
    measures[_tot]        = tfboost::algo::TimeOverThr(conv_data_h, time, c.LEthr, c.LEthr) ;
    
    measures[_toa_le]     = c.TOTcorrection? tfboost::algo::CorrectTOA(measures[_toa_le], measures[_tot], c.TOT_a, c.TOT_b) : measures[_toa_le];

    // adding time tagger resolution
    if(c.TimeReferenceResolution)
      for(auto key : {_toa_le, _tpeak, _toa_cfd, _toa_rm, _tot} )
        measures[key] += TR_res;
    

 
    
    if(c.MakeTheoreticalTOA){
      auto prob_curve = tfboost::ComputeTOAcurve( TOA_CFD, TOA_CFD-100, TOA_CFD+100, 
                                                         c.CFD_fr, measures[_vpeak], c.sigma_noise, 
                                                         idx, conv_data_h, "th_jitter.pdf");
                                                         
      for(size_t i=0; i<prob_curve.size(); ++i) th_jitter_withTOA[i] += prob_curve[i];
    }

    histograms.FillMeasures( measures);

#if TCODE_ENABLE==true
    if(measures[_toa_cfd] > -1.0 && pos_x>56){
      //TOAmaps->SetBinContent( TOAmaps->FindBin( pos_x, pos_y), measures[_toa_cfd] );
      TOAmaps->SetPoint(TOAmaps->GetN(),pos_x,pos_y, measures[_toa_cfd] ); 
      //Vmaxmaps->SetBinContent( Vmaxmaps->FindBin( pos_x, pos_y), measures[_vpeak] );
      Vmaxmaps->SetPoint(Vmaxmaps->GetN(),pos_x,pos_y, measures[_vpeak]  ); }
#endif
  
    hist_TOTvsTOA  .Fill( measures[_tot], measures[_toa_le] );
    hist_TOTvsVmax .Fill( measures[_tot], measures[_vpeak]  );

    RULE_LINE_LIGHT;
    std::cout << _START_INFO_;
    std::cout << "Measurements without noise:\n";
    std::cout << _END_INFO_;
    std::cout << "Time on thresholds (LE)  = " << measures[_toa_le]     << " (s)\n";
    std::cout << "Time on thresholds (CFD) = " << measures[_toa_cfd]    << " (s)\n";
    std::cout << "Time on thresholds (RM)  = " << measures[_toa_rm]     << " (s)\n";
    std::cout << "V on thresholds (CFD)    = " << measures[_vonth_cfd]  << " (V)\n";
    std::cout << "V on thresholds (LE)     = " << measures[_vonth_le]   << " (V)\n";
    std::cout << "V on thresholds (RM)     = " << measures[_vonth_rm]   << " (V)\n";
    std::cout << "Tpeak                    = " << measures[_tpeak]      << " (s)\n";
    std::cout << "Vpeak                    = " << measures[_vpeak]      << " (V)\n";
    std::cout << "dv/dt (CFD)              = " << measures[_dvdt_cfd]   << " (uV/ps)\n";
    std::cout << "dv/dt (LE)               = " << measures[_dvdt_le]    << " (uV/ps)\n";
    std::cout << "dv/dt (RM)               = " << measures[_dvdt_rm]    << " (uV/ps)\n";



    /*-------------------------------------------------
     * Section where the noise is added to signal
     *------------------------------------------------*/

    if(c.DoMeasurementsWithNoise)
    {
      
      // if not from file, the noise is simulated
      // using white or red spectrum model
      if(c.AddSimulatedNoise){
          auto noise = tfboost::Noise(c.sigma_noise, c.UseRedNoise, c.r_rednoise);
          noise.AddNoiseToSignal( conv_data_h, S(), c );
      }
      
      
     // Digitization of the signal
     // This step is done only if we are requesting noise
      if(c.MakeTimeDigitization){
        
        tfboost::TimeDigitizeSignal( conv_data_h, time, c.sampling_dT, max, engine, c.randomphase);

        // Override all relevant convolution information
        c.dT        = c.sampling_dT;
        c.Nsamples  = conv_data_h.size();
        hist_convol.SetBins(c.Nsamples, min, maxplot);
      }
      
      
      // Introduce a Low Pass Filter
      // simulating an oscilloscope
      if(c.LowPassFilter && !c.FilterOnlyNoise){
          auto flt       = tfboost::ButterworthFilter<double>( c.LowPassFrequency, c.LowPassOrder, c.dT);
          auto conv_temp = hydra::make_spiline<double>(time, conv_data_h);
          tfboost::Do_Convolution(fft_backend, flt, conv_temp, conv_data_h, min, max, c.Nsamples);
      }
      

      // Voltage Digitization of the signal
      // This step is done only if we are requesting noise
      if(c.MakeVoltageDigitization)
        tfboost::VoltageDigitizeSignal( conv_data_h, c.ADCmin, c.ADCmax, c.ADCnbits);


      // if noise from file, the noise samples are read
      // and added directly to the signal
      if(c.AddNoiseFromFiles){
        TSystemFile* currentnoisefile = (TSystemFile*) nextnoisefile();
        TString currentnoisefilename  = currentnoisefile->GetName();

        HostSignal_t noise_h;
        tfboost::ReadSimple( c.NoiseDirectory+currentnoisefilename, 0, noise_h, c.Nsamples, 1e-3);
        tfboost::AddNoiseToSignal(conv_data_h, noise_h, c);
      }
      


      

      // Condition to avoid to process empty signal
      if( tfboost::algo::LeadingEdge(conv_data_h, c.LE_reject_noise) == conv_data_h.size() ) 
        { WARNING_LINE("Skipping empty event...") continue; }
      
      

     /*-------------------------------------------------
      * Start measurmenets with noise
      *------------------------------------------------*/

      // calculating RMS of noise, if the offset is present
      size_t new_offset = c.offset*1e-12 / c.sampling_dT;

      double rms_noise = 0.0;
	  for (size_t i=0; i<new_offset; i++) rms_noise += conv_data_h[i] * conv_data_h[i];
      rms_noise = ::sqrt(rms_noise/new_offset);

      // initialize indices
      size_t timeatmax_idx=0, TOA_LE_noise_idx=0, TOA_CFD_noise_idx=0, TOA_RM_noise_idx=0;

      timeatmax_idx     = tfboost::algo::GetTimeAtPeak(conv_data_h);
      TOA_LE_noise_idx  = tfboost::algo::LeadingEdge(conv_data_h , c.LEthr);
      
      measures_noise[_tpeak]     = time[timeatmax_idx] ;
      measures_noise[_toa_le]    = time[TOA_LE_noise_idx]  ;
      measures_noise[_vonth_le]  = conv_data_h[TOA_LE_noise_idx];
      measures_noise[_tot]       = tfboost::algo::TimeOverThr(conv_data_h, time, c.LEthr, c.LEthr) ;
      
      
      
      if(c.MakeLinearFitNearThreshold && TOA_LE>1)
      {
        auto toa = tfboost::algo::LinearFitNearThr( c.LEthr, conv_data_h, time, c.bound_fit, c.PlotLinFit, "LEfit");
        
        measures_noise[_toa_le]  =      std::get<0>(toa);  
        measures_noise[_dvdt_le] = 1e-6*std::get<1>(toa) ;
      }
      
      
      
      if(c.MakeGaussianFitNearVmax && TOA_CFD>1)
      {
        auto gaussfit = tfboost::algo::GaussianFitNearVmax( conv_data_h, time, c.bound_fit, c.PlotGausFit );
        measures_noise[_tpeak] = std::get<1>(gaussfit); 
        measures_noise[_vpeak] = std::get<0>(gaussfit);

        auto cfd_idx = tfboost::algo::ConstantFraction(conv_data_h , c.CFD_fr , measures_noise[_vpeak]);
        measures_noise[_toa_cfd]    = time[cfd_idx] ;
        measures_noise[_vonth_cfd]  = conv_data_h[ cfd_idx ];        

        if(c.MakeLinearFitNearThreshold && TOA_LE>1)
        {
          auto toa_cf = tfboost::algo::LinearFitNearThr( c.CFD_fr*measures_noise[_vpeak], 
                                                       conv_data_h, time, c.bound_fit, 
                                                       /*plot?*/c.PlotLinFit, "CFDfit");
                                                            
          auto rm     = tfboost::algo::TimeRefMethod( conv_data_h, time, 
                                                      measures_noise[_vpeak], c.RM_delay, c.bound_fit, 
                                                      /*noise?*/true, /*plot?*/c.PlotRMfit);
                                                            
          measures_noise[_toa_cfd]   = std::get<0>(toa_cf);
          measures_noise[_dvdt_cfd]  = 1e-6 * std::get<1>(toa_cf);
          measures_noise[_toa_rm]    = std::get<0>(rm) ;
          measures_noise[_vonth_rm]  = std::get<1>(rm);
          measures_noise[_dvdt_rm]   =  1e-6 * std::get<2>(rm);
          
        }
      }

      if(c.TOTcorrection)
          measures_noise[_toa_le] = tfboost::algo::CorrectTOA(measures_noise[_toa_le], measures_noise[_tot], c.TOT_a, c.TOT_b);
      
    // adding time tagger resolution
    if(c.TimeReferenceResolution)
      for(auto key : {_toa_le, _tpeak, _toa_cfd, _toa_rm, _tot} )
        measures_noise[key] += TR_res;

     /*-------------------------------------------------
      * Fill histograms for noise measurements
      *------------------------------------------------*/
      histograms.FillMeasures_noise( measures_noise );
      hist_rms_noise.Fill(rms_noise);
      
      
      RULE_LINE_LIGHT;
      std::cout << _START_INFO_;
      std::cout << "Measurements with noise:\n";
      std::cout << _END_INFO_;
      std::cout << "Time on thresholds (LE)  = " << measures_noise[_toa_le]     << " (s)\n";
      std::cout << "Time on thresholds (CFD) = " << measures_noise[_toa_cfd]    << " (s)\n";
      std::cout << "Time on thresholds (RM)  = " << measures_noise[_toa_rm]     << " (s)\n";
      std::cout << "V on thresholds (CFD)    = " << measures_noise[_vonth_cfd]  << " (V)\n";
      std::cout << "V on thresholds (LE)     = " << measures_noise[_vonth_le]   << " (V)\n";
      std::cout << "V on thresholds (RM)     = " << measures_noise[_vonth_rm]   << " (V)\n";
      std::cout << "Vpeak                    = " << measures_noise[_vpeak]      << " (V)\n";
      std::cout << "Tpeak                    = " << measures_noise[_tpeak]      << " (s)\n";
      std::cout << "dv/dt (CFD)              = " << measures_noise[_dvdt_cfd]   << " (uV/ps)\n";
      std::cout << "dv/dt (LE)               = " << measures_noise[_dvdt_le]    << " (uV/ps)\n";
      std::cout << "dv/dt (RM)               = " << measures_noise[_dvdt_rm]    << " (uV/ps)\n";
      std::cout << "RMS of noise             = " << rms_noise                   << " (V)\n";

      
      if(PlotConv) {
          auto conv_spline = hydra::make_spiline<double>(time, conv_data_h );
          tfboost::FillHistWithFunction( hist_convol, conv_spline);
      }

    }
   

    if(PlotConv)
       tfboost::SaveConvolutionCanvas(c.OutputDirectory + "plots/", "hist_convol_functor", 
              hist_convol, hist_signal, hist_kernel);


    if(c.SaveConvDataToFile) 
      tfboost::SaveConvToFile(conv_data_h, time, c.dT, 
                              c.OutputDirectory + "data/" +currentfilename );

    
    if(dodelay) usleep(us_delay);
  
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    RULE_LINE_LIGHT;
    std::cout << _START_INFO_ << "Single iteration time  " << _END_INFO_ << "  = " <<elapsed.count() << " (ms)\n";
    RULE_LINE;
    std::cout << "\n\n";
    
  }//end loop on files
  



  /*-------------------------------------------------
   *  Save all the plots
   *------------------------------------------------*/  
  histograms.SaveHistograms( c.OutputDirectory + "plots/" );
   

  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOT_2d",    "Time [s]", "TOA [s]",  hist_TOTvsTOA,  "colz"); 
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOTvsVmax", "TOT [s]",  "Vmax [V]", hist_TOTvsVmax, "colz");
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "rms_noise", "RMS [V]",  "Counts",   hist_rms_noise);

  
  TProfile* prof =  hist_TOTvsVmax.ProfileX();
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOTvsVmax_profile",   "TOT [s]",    "Vmax [V]", *prof);
  
  TProfile* prof2 =  hist_TOTvsTOA.ProfileX();
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOTvsTOA_profile",   "TOT [s]",    "Vmax [V]", *prof2);


  if(c.DoMeasurementsWithNoise) 
    histograms.SaveHistograms_noise( c.OutputDirectory + "plots/" );




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

#if TCODE_ENABLE==true
  gStyle->SetOptStat(0);
  gStyle->SetPalette(kRainBow);

  TCanvas canv_TOAmaps( "canv_TOAmaps", "canv_TOAmaps", 4*TCODE_PIXEL_YMAX, 4*TCODE_PIXEL_XMAX);
  //TOAmaps->SetMinimum(0.15e-9);
  //TOAmaps->SetMaximum(0.5e-9);
  //TOAmaps->SetMinimum(0.0);  
  TOAmaps->Draw("colz");
  canv_TOAmaps.SaveAs( c.OutputDirectory + "plots/" + "canv_TOAmaps.pdf");
  //canv_TOAmaps.SaveAs( c.OutputDirectory + "plots/" + "canv_TOAmaps.C");

  TCanvas canv_Vmaxmaps("canv_Vmaxmaps", "canv_Vmaxmaps", 4*TCODE_PIXEL_YMAX, 4*TCODE_PIXEL_XMAX);
  //Vmaxmaps->SetMinimum(TOAmaps->GetMinimum(0.));
  //Vmaxmaps->SetMinimum(0.0);
  Vmaxmaps->Draw("colz");
  canv_Vmaxmaps.SaveAs( c.OutputDirectory + "plots/" +"canv_Vmaxmaps.pdf");
  //canv_Vmaxmaps.SaveAs( c.OutputDirectory + "plots/" +"canv_Vmaxmaps.C");
#endif

  auto main_end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> main_elapsed = main_end - main_start;

  RULE_LINE;
  std::cout << _START_INFO_ << "Total time  " << _END_INFO_ << "  = " <<main_elapsed.count() << " (ms)\n";
  RULE_LINE;

  return 0;
}

#endif






