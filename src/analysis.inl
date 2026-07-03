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
 *  analysis.inl
 *
 *  Created on: 07/05/2020
 *      Author: Davide Brundu
 */

#ifndef TFBOOST_ANALYSIS_INL_
#define TFBOOST_ANALYSIS_INL_

// STD
#include <iostream>
#include <fstream>
#include <chrono>
#include <unistd.h>

// HYDRA & ROOT
#include <tfboost/detail/external/HydraHeaders.h>
#include <tfboost/detail/external/RootHeaders.h>

// OTHERs
#include <tclap/CmdLine.h>
#include <libconfig.h++>

// TFBOOST
#include <tfboost/Types.h>
#include <tfboost/Utils.h>
#include <tfboost/ConfigParser.h>
#include <tfboost/Logger.h>
#include <tfboost/InputOutput.h>
#include <tfboost/Histograms.h>
#include <tfboost/core/Signal.h>
#include <tfboost/core/Noise.h>
#include <tfboost/io/SignalReader.h>
#include <tfboost/transforms/Pipeline.h>
#include <tfboost/measurements/CleanMeasurer.h>
#include <tfboost/measurements/NoisyMeasurer.h>

namespace libconf = libconfig;

int main(int argv, char** argc)
{
  auto main_start = std::chrono::high_resolution_clock::now();

  tfboost::TFBoostHeader();

  std::cout.precision(10);


  /* ----------------------------------------------
   * Configuration (run + histogram parsers)
   * --------------------------------------------*/
  libconf::Config Cfg;
  Cfg.readFile("../etc/configuration.cfg");
  const libconf::Setting& cfg_root = Cfg.getRoot();

  TString TransferFunction = (const char*) cfg_root["TransferFunction"];
  const libconf::Setting& cfg_tf = cfg_root[TransferFunction];

  const tfboost::ConfigParser     c(cfg_root);
  const tfboost::HistConfigParser hc(cfg_root);

  const size_t us_delay = 1e3 * c.DelayMonitoring;
  const bool   dodelay  = (us_delay != 0);


  /* ----------------------------------------------
   * Initialization of variables, RNGs
   * and algorithm objects
   * --------------------------------------------*/
  tfboost::CreateDirectories( c.OutputDirectory + "plots");
  tfboost::CreateDirectories( c.OutputDirectory + "data");
  tfboost::CreateDirectories( "monitor");

  auto& LOG = tfboost::Logger::getInstance( (c.OutputDirectory+TString("LOG.log")).Data() );
  LOG.PrintConfig(c);

  const double SampRate = 1./c.dT;
  const double min      = 0.0;
  const double max      = (c.Nsamples-1) * c.dT;
  const double minplot  = c.minplot;
  const double maxplot  = c.maxplot;

  HostSignal_t th_jitter_withTOA(120, 0.0);

  size_t INDEX = 0;

  hydra::SeedRNG S{};
  hydra::default_random_engine engine( S() );
  TRandom3 root_rng( S() );


  /* ----------------------------------------------
   * Input reader and measurement modules
   * --------------------------------------------*/
  tfboost::io::SignalReader            reader( c );
  tfboost::measurements::CleanMeasurer clean_measurer;
  tfboost::measurements::NoisyMeasurer noisy_measurer;


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
  TIter nextfile( listoffiles );
  TSystemFile *currentfile;
  TString currentfilename;


  /* ----------------------------------------------
   * Preload a tabulated transfer function, if requested
   * --------------------------------------------*/
  HostSignal_t time_tf;
  HostSignal_t current_tf;

  if(TransferFunction=="FromFile")
  {
    int Nskip = (int) cfg_tf["NlinesToSkip"];

    TString tf_infile = (const char*) cfg_tf["TFFile"];

    tfboost::ReadTF(
      tf_infile,
      Nskip,
      time_tf,
      current_tf,
      1.0, /*scaling*/
      true /*double range?*/
    );
  }


  /* ----------------------------------------------
   * Instantiate the signal-transformation modules
   * selected by the configuration (run-time), and the
   * context holding the shared random engines.
   * --------------------------------------------*/
  tfboost::transforms::TransformContext  ctx{ engine, root_rng, S };

  // Signal-transformation pipelines, assembled from the configuration:
  //  - conditioning: convolution + the transforms applied before the no-noise
  //                  measurements (the kernel-shape plot is filled here once)
  //  - noise:        transforms producing the noisy signal
  auto conditioning_pipeline = tfboost::transforms::BuildConditioningPipeline(
    c,
    cfg_tf,
    time_tf,
    current_tf,
    max,
    tfboost::transforms::FilterKind::RC,
    c.SaveSinglePlotConvolution ?
      histograms.KernelHist() : nullptr
  );

  auto noise_pipeline = tfboost::transforms::BuildNoisePipeline(
    c,
    max,
    tfboost::transforms::FilterKind::RC
  );


  /* ----------------------------------------------
   * Number of input files to process
   * --------------------------------------------*/
  int Nf = 0;
  {
    TIter countfile( listoffiles );
    TSystemFile* f;
    while( (f = (TSystemFile*) countfile()) ) {
      TString name = f->GetName();
      if(f->IsDirectory() || !name.EndsWith(c.InputFileExtension)) continue;
      ++Nf;
    }
  }
  std::cout << "    number of files in folder = " << Nf << std::endl;

  if(c.UseSameCurve)
    Nf = c.Nfiles;

  if(!c.UseSameCurve && Nf >= c.Nfiles)
    Nf = c.Nfiles;


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

    if(c.UseSameCurve)
      currentfilename = c.SingleFile;

    if ( !(c.UseSameCurve) && (currentfile->IsDirectory() ||
      !currentfilename.EndsWith(c.InputFileExtension) ) )
        continue;

    RULE_LINE;
    std::cout << "| " << _START_INFO_ << "FILE"  << _END_INFO_ << "  : " << currentfilename << " \n";
    std::cout << "| " << _START_INFO_ << "INDEX" << _END_INFO_ << " : "  << INDEX           << " \n";
    RULE_LINE_LIGHT;

    ++INDEX;

    const bool PlotConv = c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave;

    /* ----------------------------------------------
     * Read the input waveform into the working Signal.
     * The convolution (and the later transforms) mutate
     * it in place; the loop operates on `sig` from here on.
     * --------------------------------------------*/
    auto sig = reader.read( currentfilename, root_rng );


    /* ----------------------------------------------
     * Time Reference resolution
     * --------------------------------------------*/
    double TR_res = c.TimeReferenceResolution?
      root_rng.Gaus( 0.0 , c.timeref_sigma) : 0.0;

    if(PlotConv)
      histograms.FillSignalPlot( sig );


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
    if(PlotConv)
      histograms.FillConvolPlot( sig, min, maxplot );


    /* ----------------------------------------------
     * Measurements without noise
     * --------------------------------------------*/
    tfboost::measurements::MeasureContext mctx{ c, SampRate, TR_res };

    auto measures = clean_measurer.measure( sig, mctx );

    // reject "empty" events
    if(!mctx.valid) { WARNING_LINE("Skipping empty event...") continue; }

    if(c.MakeTheoreticalTOA){
      HostSignal_t idx(sig.size());
      for(size_t i=0; i<idx.size(); ++i) idx[i] = i;

      auto prob_curve = tfboost::core::compute_toa_curve(
        mctx.toa_cfd_idx,
        mctx.toa_cfd_idx-100,
        mctx.toa_cfd_idx+100,
        c.CFD_fr,
        measures[_vpeak],
        c.sigma_noise,
        idx,
        sig.amplitude(),
        "th_jitter.pdf"
      );

      for(size_t i=0; i<prob_curve.size(); ++i)
        th_jitter_withTOA[i] += prob_curve[i];
    }

    histograms.FillMeasures( measures);
    clean_measurer.print( measures, mctx );


    /*-------------------------------------------------
     * Section where the noise is added to signal
     *------------------------------------------------*/
    if(c.DoMeasurementsWithNoise)
    {
      // apply the noise transforms (simulated noise or noise-from-file,
      // oscilloscope low-pass filter, time/voltage digitization) assembled
      // from the configuration
      noise_pipeline.apply( sig, ctx );

      // measure on the noisy signal
      mctx.clean_measures = measures;
      auto measures_noise = noisy_measurer.measure( sig, mctx );

      // reject "empty" events
      if(!mctx.valid) { WARNING_LINE("Skipping empty event...") continue; }

      // fill histograms for noise measurements
      histograms.FillMeasures_noise( measures_noise );
      histograms.FillRMSNoise( mctx.rms_noise );

      if(mctx.fill_jitter)
        histograms.FillJitter( mctx.jitter_cfd, mctx.jitter_le, mctx.jitter_rm );

        noisy_measurer.print( measures_noise, mctx );

        if(PlotConv)
          histograms.FillConvolPlot( sig, min, maxplot );
    }


    if(PlotConv)
       histograms.SaveConvolutionPlot( c.OutputDirectory + "plots/" );

    if(c.SaveConvDataToFile)
      tfboost::SaveConvToFile(
        sig.amplitude(),
        sig.time(),
        sig.dT(),
        c.OutputDirectory + "data/" +currentfilename
      );

    if(dodelay)
      usleep(us_delay);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    RULE_LINE_LIGHT;
    std::cout << _START_INFO_ << "Single iteration time  " << _END_INFO_ << "  = " <<elapsed.count() << " (ms)\n";
    RULE_LINE;
    std::cout << "\n\n";

  } //end loop on files


  /*-------------------------------------------------
   *  Save all the plots
   *------------------------------------------------*/
  histograms.SaveHistograms( c.OutputDirectory + "plots/" );

  if(c.DoMeasurementsWithNoise)
    histograms.SaveHistograms_noise( c.OutputDirectory + "plots/" );

  if(c.MakeTheoreticalTOA)
  {
    TGraph hist_thjitter_withTOA;

    for(size_t i=0; i < th_jitter_withTOA.size(); ++i)
      hist_thjitter_withTOA.SetPoint(hist_thjitter_withTOA.GetN(), i+160, th_jitter_withTOA[i]);

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