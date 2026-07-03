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
 *  AnalysisApplication.h
 *
 *  Created on: 04/07/2026
 *      Author: Davide Brundu
 */

#ifndef TFBOOST_APP_ANALYSISAPPLICATION_H_
#define TFBOOST_APP_ANALYSISAPPLICATION_H_

// STD
#include <iostream>
#include <fstream>
#include <chrono>
#include <optional>
#include <string>
#include <unistd.h>

// HYDRA & ROOT
#include <tfboost/detail/external/HydraHeaders.h>
#include <tfboost/detail/external/RootHeaders.h>

// OTHERs
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


namespace tfboost {

namespace app {

/*
 *  @class AnalysisApplication
 *
 *  Fluent driver for the standard TFBoost analysis run.
 */
class AnalysisApplication {

public:

    AnalysisApplication() = default;

    AnalysisApplication& initialize();

    // Parse the configuration file, build the typed parsers, create the output
    // tree + logger, derive the run constants, size the histograms, gather the
    // input-file list and preload a tabulated transfer function if requested.
    AnalysisApplication& read_configuration(std::string const& cfg_path = "../etc/configuration.cfg");

    // Assemble the run-time processing chain from the configuration
    AnalysisApplication& generate_pipeline();

    // Execute the main loop over the input files and save all outputs.
    int run();

private:

    // Full processing of one input waveform
    void process_event(TString const& filename, bool plot_conv,
                        std::chrono::high_resolution_clock::time_point start);

    // Final histogram / graph persistence, after the loop.
    void save_outputs();

    ConfigParser&     config()  { return *fConfig; }
    HistogramsManager& histos() { return HistogramsManager::getInstance(); }

    std::chrono::high_resolution_clock::time_point fMainStart{};

    libconfig::Config fCfg; // owns the parsed configuration
    libconfig::Setting const* fCfgTf = nullptr; // selected transfer-function block (into fCfg)
    TString fTransferFunction;
    std::optional<ConfigParser> fConfig;
    std::optional<HistConfigParser> fHistConfig;
    Logger* fLog = nullptr;

    double fSampRate = 0., fMin = 0., fMax = 0., fMinplot = 0., fMaxplot = 0.;
    size_t fUsDelay  = 0;
    bool   fDoDelay  = false;

    // RNGs
    hydra::SeedRNG                              fSeedRng{};
    std::optional<hydra::default_random_engine> fEngine;
    std::optional<TRandom3>                     fRootRng;

    // Processing modules
    std::optional<io::SignalReader>       fReader;
    measurements::CleanMeasurer           fCleanMeasurer;
    measurements::NoisyMeasurer           fNoisyMeasurer;
    transforms::Pipeline                  fConditioning;
    transforms::Pipeline                  fNoise;

    // Preloaded tabulated transfer function (only for TransferFunction=FromFile)
    HostSignal_t fTimeTf;
    HostSignal_t fCurrentTf;

    // Accumulator for the "theoretical TOA" jitter curve
    HostSignal_t fThJitterWithTOA;

    // Loop bookkeeping
    size_t fINDEX = 0;
    int    fNf    = 0;
    TList* fListOfFiles = nullptr;
};



inline AnalysisApplication& AnalysisApplication::initialize()
{
    fMainStart = std::chrono::high_resolution_clock::now();

    tfboost::TFBoostHeader();
    std::cout.precision(10);

    // Deterministic seeding
    fEngine.emplace( fSeedRng() );
    fRootRng.emplace( fSeedRng() );

    return *this;
}



inline AnalysisApplication& AnalysisApplication::read_configuration(std::string const& cfg_path)
{
    fCfg.readFile( cfg_path.c_str() );
    libconfig::Setting const& cfg_root = fCfg.getRoot();

    fTransferFunction = (const char*) cfg_root["TransferFunction"];
    fCfgTf            = &cfg_root[fTransferFunction];

    fConfig.emplace( cfg_root );
    fHistConfig.emplace( cfg_root );
    ConfigParser const& c = *fConfig;

    fUsDelay = 1e3 * c.DelayMonitoring;
    fDoDelay = (fUsDelay != 0);

    tfboost::CreateDirectories( c.OutputDirectory + "plots");
    tfboost::CreateDirectories( c.OutputDirectory + "data");
    tfboost::CreateDirectories( "monitor");

    fLog = &tfboost::Logger::getInstance( (c.OutputDirectory + TString("LOG.log")).Data() );
    fLog->PrintConfig(c);

    fSampRate = 1./c.dT;
    fMin      = 0.0;
    fMax      = (c.Nsamples-1) * c.dT;
    fMinplot  = c.minplot;
    fMaxplot  = c.maxplot;

    fThJitterWithTOA = HostSignal_t(120, 0.0);

    histos().SetConfig(*fHistConfig, c);

    // Input-file list and how many of them to process
    fListOfFiles = tfboost::GetFileList(c.InputDirectory);

    // Preload a tabulated transfer function, if requested
    if(fTransferFunction=="FromFile")
    {
        int     Nskip    = (int) (*fCfgTf)["NlinesToSkip"];
        TString tf_infile = (const char*) (*fCfgTf)["TFFile"];

        tfboost::ReadTF( tf_infile, Nskip, fTimeTf, fCurrentTf, 1.0 /*scaling*/, true /*double range?*/ );
    }

    return *this;
}



inline AnalysisApplication& AnalysisApplication::generate_pipeline()
{
    ConfigParser const& c = *fConfig;

    fReader.emplace( c );

    // Signal-transformation pipelines, assembled from the configuration:
    //  - conditioning: convolution + the transforms applied before the no-noise
    //                  measurements (the kernel-shape plot is filled here once)
    //  - noise:        transforms producing the noisy signal
    fConditioning = transforms::BuildConditioningPipeline(
        c, *fCfgTf, fTimeTf, fCurrentTf, fMax,
        transforms::FilterKind::RC,
        c.SaveSinglePlotConvolution ? histos().KernelHist() : nullptr );

    fNoise = transforms::BuildNoisePipeline( c, fMax, transforms::FilterKind::RC );

    // Number of input files to process
    fNf = 0;
    {
        TIter countfile( fListOfFiles );
        TSystemFile* f;
        while( (f = (TSystemFile*) countfile()) ) {
            TString name = f->GetName();
            if(f->IsDirectory() || !name.EndsWith(c.InputFileExtension)) continue;
            ++fNf;
        }
    }
    std::cout << "    number of files in folder = " << fNf << std::endl;

    if(c.UseSameCurve)
        fNf = c.Nfiles;

    if(!c.UseSameCurve && fNf >= c.Nfiles)
        fNf = c.Nfiles;

    return *this;
}



inline void AnalysisApplication::process_event(
    TString const& filename,
    bool plot_conv,
    std::chrono::high_resolution_clock::time_point start
) {
    ConfigParser const& c = *fConfig;
    auto& root_rng = *fRootRng;

    // Context holding the shared random engines, handed to every transform.
    transforms::TransformContext ctx{ *fEngine, *fRootRng, fSeedRng };

    /* Read the input waveform into the working Signal. The convolution (and the
     * later transforms) mutate it in place. */
    auto sig = fReader->read( filename, root_rng );

    // Time Reference resolution (drawn after the reader's Landau draw)
    double TR_res = c.TimeReferenceResolution ? root_rng.Gaus( 0.0, c.timeref_sigma) : 0.0;

    if(plot_conv)
        histos().FillSignalPlot( sig );

    // Convolution + no-noise conditioning transforms (convolution is the first
    // step of this pipeline; when MakeConvolution is off, `sig` already holds
    // the convoluted input signal).
    fConditioning.apply( sig, ctx );

    if(plot_conv)
        histos().FillConvolPlot( sig, fMin, fMaxplot );

    // Measurements without noise
    measurements::MeasureContext mctx{ c, fSampRate, TR_res };
    auto measures = fCleanMeasurer.measure( sig, mctx );

    if(!mctx.valid) { WARNING_LINE("Skipping empty event...") return; }   // reject "empty" events

    if(c.MakeTheoreticalTOA){
        HostSignal_t idx(sig.size());
        for(size_t i=0; i<idx.size(); ++i) idx[i] = i;

        auto prob_curve = tfboost::core::compute_toa_curve(
            mctx.toa_cfd_idx, mctx.toa_cfd_idx-100, mctx.toa_cfd_idx+100,
            c.CFD_fr, measures[_vpeak], c.sigma_noise,
            idx, sig.amplitude(), "th_jitter.pdf" );

        for(size_t i=0; i<prob_curve.size(); ++i)
            fThJitterWithTOA[i] += prob_curve[i];
    }

    histos().FillMeasures( measures );
    fCleanMeasurer.print( measures, mctx );

    /* Section where the noise is added to signal */
    if(c.DoMeasurementsWithNoise)
    {
        // apply the noise transforms (simulated / from-file noise, oscilloscope
        // low-pass filter, time/voltage digitization) assembled from the config
        fNoise.apply( sig, ctx );

        // measure on the noisy signal
        mctx.clean_measures = measures;
        auto measures_noise = fNoisyMeasurer.measure( sig, mctx );

        if(!mctx.valid) { WARNING_LINE("Skipping empty event...") return; }   // reject "empty" events

        histos().FillMeasures_noise( measures_noise );
        histos().FillRMSNoise( mctx.rms_noise );

        if(mctx.fill_jitter)
            histos().FillJitter( mctx.jitter_cfd, mctx.jitter_le, mctx.jitter_rm );

        fNoisyMeasurer.print( measures_noise, mctx );

        if(plot_conv)
            histos().FillConvolPlot( sig, fMin, fMaxplot );
    }

    if(plot_conv)
        histos().SaveConvolutionPlot( c.OutputDirectory + "plots/" );

    if(c.SaveConvDataToFile)
        tfboost::SaveConvToFile( sig.amplitude(), sig.time(), sig.dT(),
                                 c.OutputDirectory + "data/" + filename );

    if(fDoDelay)
        usleep(fUsDelay);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    RULE_LINE_LIGHT;
    std::cout << _START_INFO_ << "Single iteration time  " << _END_INFO_ << "  = " << elapsed.count() << " (ms)\n";
    RULE_LINE;
    std::cout << "\n\n";
}



inline int AnalysisApplication::run()
{
    ConfigParser const& c = *fConfig;

    TIter nextfile( fListOfFiles );
    TSystemFile* currentfile = nullptr;
    TString      currentfilename;

    while( fINDEX < fNf )
    {
        auto start = std::chrono::high_resolution_clock::now();

        if(!(c.UseSameCurve)) {
            currentfile     = (TSystemFile*) nextfile();
            currentfilename = currentfile->GetName();
        }

        if(c.UseSameCurve)
            currentfilename = c.SingleFile;

        if ( !(c.UseSameCurve) && (currentfile->IsDirectory() ||
             !currentfilename.EndsWith(c.InputFileExtension) ) )
                continue;

        RULE_LINE;
        std::cout << "| " << _START_INFO_ << "FILE"  << _END_INFO_ << "  : " << currentfilename << " \n";
        std::cout << "| " << _START_INFO_ << "INDEX" << _END_INFO_ << " : "  << fINDEX          << " \n";
        RULE_LINE_LIGHT;

        ++fINDEX;

        const bool plot_conv = c.SaveSinglePlotConvolution && fINDEX==c.IdxConvtoSave;

        process_event( currentfilename, plot_conv, start );

    } //end loop on files

    save_outputs();

    fLog->PrintMessage("Number of files analyzed: ", fINDEX);
    fLog->Exit();

    auto main_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> main_elapsed = main_end - fMainStart;

    RULE_LINE;
    std::cout << _START_INFO_ << "Total time  " << _END_INFO_ << "  = " << main_elapsed.count() << " (ms)\n";
    RULE_LINE;

    return 0;
}



inline void AnalysisApplication::save_outputs()
{
    ConfigParser const& c = *fConfig;

    histos().SaveHistograms( c.OutputDirectory + "plots/" );

    if(c.DoMeasurementsWithNoise)
        histos().SaveHistograms_noise( c.OutputDirectory + "plots/" );

    if(c.MakeTheoreticalTOA)
    {
        TGraph hist_thjitter_withTOA;

        for(size_t i=0; i < fThJitterWithTOA.size(); ++i)
            hist_thjitter_withTOA.SetPoint(hist_thjitter_withTOA.GetN(), i+160, fThJitterWithTOA[i]);

        TCanvas canv_thjitter_withTOA("canv_thjitter_withTOA", "canv_thjitter_withTOA", 800, 800);
        hist_thjitter_withTOA.SetTitle("Jitter from first principles;time [ps];A.U.");
        hist_thjitter_withTOA.Draw("APL");
        canv_thjitter_withTOA.SaveAs(c.OutputDirectory + "plots/thjitter_differentcurves.pdf");
    }
}


} // namespace app

} // namespace tfboost

#endif /* TFBOOST_APP_ANALYSISAPPLICATION_H_ */
