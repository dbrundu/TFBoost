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
#include <tfboost/core/Signal.h>
#include <tfboost/transforms/ConvolutionModule.h>
#include <tfboost/transforms/Pipeline.h>
#include <tfboost/functions/TIA_BJT_1stage.h>
#include <tfboost/functions/TIA_BJT_2stages.h>
#include <tfboost/functions/TIA_BJT_2stages_GM.h>
#include <tfboost/functions/TIA_IdealInt.h>
#include <tfboost/functions/TIA_MOS.h>
#include <tfboost/functions/ExpModifiedGaussian.h>
#include <tfboost/functions/ButterworthFilter.h>
#include <tfboost/InputOutput.h>
#include <tfboost/Algorithms.h>
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
  Cfg.readFile("../examples/config.cfg");
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
  histograms.SetConfig(hc, c);




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
   * Instantiate the signal-transformation modules
   * selected by the configuration (run-time), and the
   * context holding the shared random engines.
   * --------------------------------------------*/
  tfboost::transforms::TransformContext  ctx{ engine, root_rng, S };

  // signal-transformation pipelines, assembled from the configuration:
  //  - conditioning: convolution + the transforms applied before the no-noise
  //                  measurements (the kernel-shape plot is filled here once)
  //  - noise:        transforms producing the noisy signal
  auto conditioning_pipeline =
      tfboost::transforms::BuildConditioningPipeline( c, cfg_tf, time_tf, current_tf, max,
                                                      tfboost::transforms::FilterKind::Butterworth,
                                                      c.SaveSinglePlotConvolution ? histograms.KernelHist() : nullptr );
  auto noise_pipeline =
      tfboost::transforms::BuildNoisePipeline( c, max, tfboost::transforms::FilterKind::Butterworth );





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

    //Declare and prepare the containers
    HostSignal_t time;     time.reserve(c.Nsamples);
    HostSignal_t idx;      idx.reserve(c.Nsamples);
    HostSignal_t current;  current.reserve(c.Nsamples);
     
    //Fill containers until offset
    for(size_t k=0; k < c.offset; ++k) {
      idx.push_back(k);
      time.push_back(k * c.dT);
      current.push_back(0.0); }
    
    


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
      if(data_str == "0" && s==c.offset) continue; //--> to avoid misaligned TCoDe input files 
        
      double data = landau*atof(data_str);
         
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
     * Build the working Signal from the input curve.
     * The convolution (and the later transforms) mutate
     * it in place; the loop operates on `sig` from here on.
     * --------------------------------------------*/
    tfboost::core::Signal sig{ std::move(time), std::move(current), c.dT };

    if(PlotConv) histograms.FillSignalPlot( sig );


    /* ----------------------------------------------
     * Convolution + no-noise conditioning transforms
     * (the convolution is the first step of the pipeline;
     *  when MakeConvolution is off, `sig` already holds the
     *  convoluted input signal)
     * --------------------------------------------*/
    conditioning_pipeline.apply( sig, ctx );



    /* ----------------------------------------------
     * Filling histogram for visualization
     * --------------------------------------------*/
    if(PlotConv) histograms.FillConvolPlot( sig, min, maxplot );



    /* ----------------------------------------------
     * Measurements without noise
     * --------------------------------------------*/

    // Condition to avoid to process empty signal
    // reject signal with amplitude < 1 mV
    if( tfboost::algo::LeadingEdge(sig.amplitude(), c.LE_reject_nonoise) ==  sig.size() ) {
        WARNING_LINE("Skipping empty event...")
        continue; }

    size_t TOA_LE         = tfboost::algo::LeadingEdge(sig.amplitude(), c.LEthr);
    measures[_toa_le]     = sig.time()[TOA_LE] ;

    size_t TimeAtPeak     = tfboost::algo::GetTimeAtPeak(sig.amplitude());
    measures[_tpeak]      = sig.time()[TimeAtPeak] ;

    measures[_vpeak]      = tfboost::algo::GetVAtPeak(sig.amplitude());

    size_t TOA_CFD        = tfboost::algo::ConstantFraction(sig.amplitude() , c.CFD_fr , measures[_vpeak]);
    measures[_toa_cfd]    = sig.time()[TOA_CFD] ;

    auto rm_nonoise       = tfboost::algo::TimeRefMethod( sig.amplitude(), sig.time(), measures[_vpeak], c.RM_delay, c.bound_fit,/*noise?*/false, /*plot?*/false );
    measures[_toa_rm]     = std::get<0>( rm_nonoise ) ;

    measures[_dvdt_rm]    = 1e-6 * std::get<2>( rm_nonoise ) ;

    measures[_vonth_rm]   =  std::get<1>( rm_nonoise ) ;

    measures[_dvdt_le]    = 1e-6 * SampRate * tfboost::algo::SlopeOnThrs(sig.amplitude(), TOA_LE);

    measures[_dvdt_cfd]   = 1e-6 * SampRate * tfboost::algo::SlopeOnThrs(sig.amplitude(), TOA_CFD);

    measures[_vonth_le]   = sig.amplitude()[TOA_LE];

    measures[_vonth_cfd]  = sig.amplitude()[TOA_CFD];

    measures[_tot]        = tfboost::algo::TimeOverThr(sig.amplitude(), sig.time(), c.LEthr, c.LEthr) ;
    
    measures[_toa_le]     = c.TOTcorrection? tfboost::algo::CorrectTOA(measures[_toa_le], measures[_tot], c.TOT_a, c.TOT_b) : measures[_toa_le];

    // adding time tagger resolution
    if(c.TimeReferenceResolution)
      for(auto key : {_toa_le, _tpeak, _toa_cfd, _toa_rm, _tot} )
        measures[key] += TR_res;
    

 
    
    if(c.MakeTheoreticalTOA){
      auto prob_curve = tfboost::core::compute_toa_curve( TOA_CFD, TOA_CFD-100, TOA_CFD+100,
                                                         c.CFD_fr, measures[_vpeak], c.sigma_noise,
                                                         idx, sig.amplitude(), "th_jitter.pdf");
                                                         
      for(size_t i=0; i<prob_curve.size(); ++i) th_jitter_withTOA[i] += prob_curve[i];
    }

    histograms.FillMeasures( measures);

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
    std::cout << "Vpeak                    = " << measures[_vpeak]      << " (V)\n";
    std::cout << "dv/dt (CFD)              = " << measures[_dvdt_cfd]   << " (uV/ps)\n";
    std::cout << "dv/dt (LE)               = " << measures[_dvdt_le]    << " (uV/ps)\n";
    std::cout << "dv/dt (RM)               = " << measures[_dvdt_rm]    << " (uV/ps)\n";



    /*-------------------------------------------------
     * Section where the noise is added to signal
     *------------------------------------------------*/

    if(c.DoMeasurementsWithNoise)
    {

      // apply the noise transforms (simulated noise or noise-from-file,
      // oscilloscope low-pass filter, time/voltage digitization) assembled
      // from the configuration
      noise_pipeline.apply( sig, ctx );



      

      // Condition to avoid to process empty signal
      if( tfboost::algo::LeadingEdge(sig.amplitude(), c.LE_reject_noise) == sig.size() )
        { WARNING_LINE("Skipping empty event...") continue; }



     /*-------------------------------------------------
      * Start measurmenets with noise
      *------------------------------------------------*/

      // initialize indices
      size_t timeatmax_idx=0, TOA_LE_noise_idx=0, TOA_CFD_noise_idx=0, TOA_RM_noise_idx=0;

      timeatmax_idx     = tfboost::algo::GetTimeAtPeak(sig.amplitude());
      TOA_LE_noise_idx  = tfboost::algo::LeadingEdge(sig.amplitude() , c.LEthr);

      measures_noise[_tpeak]     = sig.time()[timeatmax_idx] ;
      measures_noise[_toa_le]    = sig.time()[TOA_LE_noise_idx]  ;
      measures_noise[_vonth_le]  = sig.amplitude()[TOA_LE_noise_idx];
      measures_noise[_tot]       = tfboost::algo::TimeOverThr(sig.amplitude(), sig.time(), c.LEthr, c.LEthr) ;



      if(c.MakeLinearFitNearThreshold && TOA_LE>1)
      {
        auto toa = tfboost::algo::LinearFitNearThr( c.LEthr, sig.amplitude(), sig.time(), c.bound_fit, c.PlotLinFit, "LEfit");

        measures_noise[_toa_le]  =      std::get<0>(toa);
        measures_noise[_dvdt_le] = 1e-6*std::get<1>(toa) ;
      }



      if(c.MakeGaussianFitNearVmax && TOA_CFD>1)
      {
        auto gaussfit = tfboost::algo::GaussianFitNearVmax( sig.amplitude(), sig.time(), c.bound_fit, c.PlotGausFit );
        measures_noise[_tpeak] = sig.time()[std::get<1>(gaussfit)] ;
        measures_noise[_vpeak] = std::get<0>(gaussfit);

        auto cfd_idx = tfboost::algo::ConstantFraction(sig.amplitude() , c.CFD_fr , measures_noise[_vpeak]);
        measures_noise[_toa_cfd]    = sig.time()[cfd_idx] ;
        measures_noise[_vonth_cfd]  = sig.amplitude()[ cfd_idx ];

        if(c.MakeLinearFitNearThreshold && TOA_LE>1)
        {
          auto toa_cf = tfboost::algo::LinearFitNearThr( c.CFD_fr*measures_noise[_vpeak],
                                                       sig.amplitude(), sig.time(), c.bound_fit,
                                                       /*plot?*/c.PlotLinFit, "CFDfit");

          auto rm     = tfboost::algo::TimeRefMethod( sig.amplitude(), sig.time(),
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
      std::cout << "dv/dt (CFD)              = " << measures_noise[_dvdt_cfd]   << " (uV/ps)\n";
      std::cout << "dv/dt (LE)               = " << measures_noise[_dvdt_le]    << " (uV/ps)\n";
      std::cout << "dv/dt (RM)               = " << measures_noise[_dvdt_rm]    << " (uV/ps)\n";

      
      if(PlotConv) histograms.FillConvolPlot( sig, min, maxplot );

    }


    if(PlotConv)
       histograms.SaveConvolutionPlot( c.OutputDirectory + "plots/" );


    if(c.SaveConvDataToFile)
      tfboost::SaveConvToFile(sig.amplitude(), sig.time(), sig.dT(),
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

  auto main_end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> main_elapsed = main_end - main_start;

  RULE_LINE;
  std::cout << _START_INFO_ << "Total time  " << _END_INFO_ << "  = " <<main_elapsed.count() << " (ms)\n";
  RULE_LINE;

  return 0;
}

#endif






