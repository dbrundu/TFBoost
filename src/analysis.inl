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
#include <tfboost/measurements/CleanMeasurer.h>
#include <tfboost/measurements/NoisyMeasurer.h>
#include <tfboost/ITCoDe.h>
#include <tfboost/functions/TIA_BJT_1stage.h>
#include <tfboost/functions/TIA_BJT_2stages.h>
#include <tfboost/functions/TIA_BJT_2stages_GM.h>
#include <tfboost/functions/TIA_IdealInt.h>
#include <tfboost/functions/TIA_MOS.h>
#include <tfboost/functions/ExpModifiedGaussian.h>
#include <tfboost/functions/RCFilter.h>
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
   * Measurement modules (no-noise and noisy passes)
   * --------------------------------------------*/
  tfboost::measurements::CleanMeasurer clean_measurer;
  tfboost::measurements::NoisyMeasurer noisy_measurer;

 
 
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

  TH1D hist_JitterCFD("hist_JitterCFD","hist_JitterCFD", 2000, -1e-9, 1e-9 );
  TH1D hist_JitterLE("hist_JitterLE","hist_JitterLE", 2000, -1e-9, 1e-9 );
  TH1D hist_JitterRM("hist_JitterRM","hist_JitterRM", 2000, -1e-9, 1e-9 );

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

  tfboost::transforms::ConvolutionModule convolution( c.ID, cfg_tf, time_tf, current_tf );

  // signal-transformation pipelines, assembled from the configuration:
  //  - conditioning: transforms applied before the no-noise measurements
  //  - noise:        transforms producing the noisy signal
  auto conditioning_pipeline =
      tfboost::transforms::BuildConditioningPipeline( c, max, tfboost::transforms::FilterKind::RC );
  auto noise_pipeline =
      tfboost::transforms::BuildNoisePipeline( c, max, tfboost::transforms::FilterKind::RC );
  
  /* ----------------------------------------------
   * Count number of files in input directory
   * --------------------------------------------*/
  TList* listoffiles2 = tfboost::GetFileList(c.InputDirectory); 
  TIter nextfile2( listoffiles2 );
  TSystemFile *currentfile2;
  int Nf=0; 
  while( (currentfile2 = (TSystemFile*) nextfile2()) ){ 
    currentfilename = currentfile2->GetName();
    if(currentfile2->IsDirectory() || !currentfilename.EndsWith(c.InputFileExtension)) continue;
    Nf++;
    
    }
  std::cout <<"    number of files in folder ="<<Nf <<std::endl;
  
  if(c.UseSameCurve) Nf = c.Nfiles;

  if(!(c.UseSameCurve) && Nf>=c.Nfiles) Nf=c.Nfiles;



  

  /* ----------------------------------------------
   * Main loop on input files
   * --------------------------------------------*/
  while( INDEX < Nf )
  {
    auto start = std::chrono::high_resolution_clock::now();

    if(!(c.UseSameCurve)) {
      currentfile = (TSystemFile*) nextfile() ;
      currentfilename = currentfile->GetName(); 
    }
 
    if(c.UseSameCurve) currentfilename = c.SingleFile;    

    if ( !(c.UseSameCurve) && (currentfile->IsDirectory() || !currentfilename.EndsWith(c.InputFileExtension) ) ) continue;
    
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

    double scale = c.scale_factor;


    // Actual loop on file lines, fill all the containers
    size_t s = c.offset;
    for(size_t j = c.offset; j < c.Nsamples ; ++j)
    {
      line.ReadLine(myFile);
      if (!myFile.good()) break;
      
      TObjArray *tokens = line.Tokenize( c.token.Data() );
      
      TString data_str  = ((TObjString*) tokens->At( c.column ) )->GetString();
      //if(data_str == "0" && s==c.offset) continue; //--> to avoid misaligned TCoDe input files 
        
      double data = scale*landau*atof(data_str);
         
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

    if(PlotConv) tfboost::FillHistWithFunction( hist_signal, sig.spline());


    /* ----------------------------------------------
     * Performing the convolution
     * --------------------------------------------*/
    // convolve the input signal with the configured transfer function
    // (when disabled, `sig` already holds the convoluted input signal)
    if(c.MakeConvolution)
      convolution.Convolve( sig, PlotConv ? &hist_kernel : nullptr );

    // apply the no-noise conditioning transforms (filter / digitization)
    conditioning_pipeline.apply( sig, ctx );



    /* ----------------------------------------------
     * Filling histogram for visualization
     * --------------------------------------------*/
    if(PlotConv) {
        hist_convol.SetBins(sig.size(), min, maxplot);
        tfboost::FillHistWithFunction( hist_convol, sig.spline());
    }



    /* ----------------------------------------------
     * Measurements without noise
     * --------------------------------------------*/
    tfboost::measurements::MeasureContext mctx{ c, SampRate, TR_res };

    auto measures = clean_measurer.measure( sig, mctx );

    // reject "empty" events
    if(!mctx.valid) { WARNING_LINE("Skipping empty event...") continue; }


    if(c.MakeTheoreticalTOA){
      auto prob_curve = tfboost::core::compute_toa_curve( mctx.toa_cfd_idx, mctx.toa_cfd_idx-100, mctx.toa_cfd_idx+100,
                                                         c.CFD_fr, measures[_vpeak], c.sigma_noise,
                                                         idx, sig.amplitude(), "th_jitter.pdf");
                                                         
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

      // apply the noise transforms (simulated noise or noise-from-file,
      // oscilloscope low-pass filter, time/voltage digitization) assembled
      // from the configuration
      noise_pipeline.apply( sig, ctx );


      // measure on the noisy signal (the clean measures provide the
      // electronic-jitter reference)
      mctx.clean_measures = measures;
      auto measures_noise = noisy_measurer.measure( sig, mctx );

      // reject "empty" events
      if(!mctx.valid) { WARNING_LINE("Skipping empty event...") continue; }


     /*-------------------------------------------------
      * Fill histograms for noise measurements
      *------------------------------------------------*/
      histograms.FillMeasures_noise( measures_noise );
      hist_rms_noise.Fill(mctx.rms_noise);

      if(mctx.fill_jitter)
      {
        hist_JitterCFD.Fill(mctx.jitter_cfd);
        hist_JitterLE .Fill(mctx.jitter_le);
        hist_JitterRM .Fill(mctx.jitter_rm);
      }
      
      
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
      std::cout << "RMS of noise             = " << mctx.rms_noise              << " (V)\n";

      
      if(PlotConv) {
          hist_convol.SetBins(sig.size(), min, maxplot);
          tfboost::FillHistWithFunction( hist_convol, sig.spline());
      }

    }


    if(PlotConv)
       tfboost::SaveConvolutionCanvas(c.OutputDirectory + "plots/", "hist_convol_functor",
              hist_convol, hist_signal, hist_kernel);


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
   

  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOT_2d",    "Time [s]", "TOA [s]",  hist_TOTvsTOA,  "colz"); 
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOTvsVmax", "TOT [s]",  "Vmax [V]", hist_TOTvsVmax, "colz");
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "rms_noise", "RMS [V]",  "Counts",   hist_rms_noise);
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "JitterCFD", "Time[s]",  "Counts",   hist_JitterCFD);
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "JitterLE", "Time[s]",  "Counts",   hist_JitterLE);
  tfboost::SaveCanvas(c.OutputDirectory + "plots/", "JitterRM", "Time[s]",  "Counts",   hist_JitterRM);
  
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






