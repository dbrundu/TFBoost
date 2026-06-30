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
 *  Created on: 22/11/2020
 *      Author: Davide Brundu
 */

#ifndef TFBOOST_HISTOGRAMS_H_
#define TFBOOST_HISTOGRAMS_H_

#include <tfboost/Types.h>
#include <tfboost/Utils.h>
#include <tfboost/ConfigParser.h>
#include <tfboost/core/Signal.h>


namespace tfboost {


/*
 * The class HistogramsManager is a singleton class
 * that allocates the memory for all the histograms and
 * provides an interface to HistConfigParser, to configure
 * and format the histograms, and an interface to the
 * measurements, in order to fill the histograms.
 *
 * It owns every histogram of a run: the per-measurement 1D
 * histograms (no-noise and noise), the TOT correlation 2D
 * histograms, the noise RMS and electronic-jitter histograms,
 * and the input/convolution/kernel histograms of the diagnostic
 * convolution plot.
 */
class HistogramsManager {

    public:

        HistogramsManager(HistogramsManager const&) = delete;

        void operator=(HistogramsManager const&)    = delete;

        static HistogramsManager& getInstance() {
            static HistogramsManager instance;
            return instance;
        }



        inline void SetConfig( tfboost::HistConfigParser const& config_parser,
                               tfboost::ConfigParser     const& c){

            for(auto const& key : MeasuresInitializer::get_keys() ) {

                _setconfig( histograms[key], config_parser.configs[key] );

                _setconfig( histograms_noise[key], config_parser.configs_noise[key] );
            }

            // convolution diagnostic-plot histograms
            hist_signal->SetNameTitle("hist_signal", "hist_signal");
            hist_signal->SetBins( c.Nsamples,    c.minplot, c.maxplot );

            hist_convol->SetNameTitle("hist_convol", "hist_convol");
            hist_convol->SetBins( c.Nsamples,    c.minplot, c.maxplot );

            hist_kernel->SetNameTitle("hist_kernel", "hist_kernel");
            hist_kernel->SetBins( c.Nsamples/30, c.minplot, c.maxplot );

            // noise RMS and electronic-jitter histograms
            hist_rms_noise->SetNameTitle("hist_rms_noise", "hist_rms_noise");
            hist_rms_noise->SetBins( 100, 0, -1 );
            hist_rms_noise->SetBuffer(1000);

            hist_JitterCFD->SetNameTitle("hist_JitterCFD", "hist_JitterCFD");
            hist_JitterCFD->SetBins( 2000, -1e-9, 1e-9 );
            hist_JitterLE ->SetNameTitle("hist_JitterLE",  "hist_JitterLE");
            hist_JitterLE ->SetBins( 2000, -1e-9, 1e-9 );
            hist_JitterRM ->SetNameTitle("hist_JitterRM",  "hist_JitterRM");
            hist_JitterRM ->SetBins( 2000, -1e-9, 1e-9 );
        }



        inline void FillMeasures( Measures_t const& measures){

            for(auto const& key : MeasuresInitializer::get_keys() ) {
                if(measures[key]>-1.0)
                    histograms[key] -> Fill ( measures[key] );
            }

            hist_TOTvsTOA  -> Fill( measures[_tot], measures[_toa_le] );
            hist_TOTvsVmax -> Fill( measures[_tot], measures[_vpeak]  );
        }



        inline void FillMeasures_noise( Measures_t const& measures_n){

            for(auto const& key : MeasuresInitializer::get_keys() ) {
                if(measures_n[key]>-1.0)
                    histograms_noise[key] -> Fill ( measures_n[key] );
            }
        }


        inline void FillRMSNoise( double rms_noise){
            hist_rms_noise->Fill(rms_noise);
        }


        inline void FillJitter( double jitter_cfd, double jitter_le, double jitter_rm){
            hist_JitterCFD->Fill(jitter_cfd);
            hist_JitterLE ->Fill(jitter_le);
            hist_JitterRM ->Fill(jitter_rm);
        }


        // --- convolution diagnostic plot ---

        // histogram of the transfer-function kernel shape, filled by the
        // ConvolutionModule (the kernel is event-independent)
        inline TH1D* KernelHist() { return hist_kernel; }

        inline void FillSignalPlot( core::Signal const& sig){
            tfboost::FillHistWithFunction( *hist_signal, sig.spline());
        }

        inline void FillConvolPlot( core::Signal const& sig, double min, double maxplot){
            hist_convol->SetBins(sig.size(), min, maxplot);
            tfboost::FillHistWithFunction( *hist_convol, sig.spline());
        }

        inline void SaveConvolutionPlot( TString const& outputdir){
            tfboost::SaveConvolutionCanvas( outputdir, "hist_convol_functor",
                                            *hist_convol, *hist_signal, *hist_kernel);
        }



        inline void SaveHistograms(TString const& outputdir){

            for(auto const& key : MeasuresInitializer::get_keys() ) {
                tfboost::SaveCanvas( outputdir, *histograms[key] );
            }

            tfboost::SaveCanvas( outputdir, "TOT_2d",    "Time [s]", "TOA [s]",  *hist_TOTvsTOA,  "colz");
            tfboost::SaveCanvas( outputdir, "TOTvsVmax", "TOT [s]",  "Vmax [V]", *hist_TOTvsVmax, "colz");

            TProfile* prof  = hist_TOTvsVmax->ProfileX();
            tfboost::SaveCanvas( outputdir, "TOTvsVmax_profile", "TOT [s]", "Vmax [V]", *prof);

            TProfile* prof2 = hist_TOTvsTOA->ProfileX();
            tfboost::SaveCanvas( outputdir, "TOTvsTOA_profile",  "TOT [s]", "Vmax [V]", *prof2);

            tfboost::SaveCanvas( outputdir, "rms_noise", "RMS [V]", "Counts", *hist_rms_noise);
            tfboost::SaveCanvas( outputdir, "JitterCFD", "Time[s]", "Counts", *hist_JitterCFD);
            tfboost::SaveCanvas( outputdir, "JitterLE",  "Time[s]", "Counts", *hist_JitterLE);
            tfboost::SaveCanvas( outputdir, "JitterRM",  "Time[s]", "Counts", *hist_JitterRM);
        }



        inline void SaveHistograms_noise( TString const& outputdir ){

            for(auto const& key : MeasuresInitializer::get_keys() ) {
                tfboost::SaveCanvas( outputdir, *histograms_noise[key] );
            }
        }


    private:

        HistogramsManager() {
            for(auto& hist : histograms) hist = new TH1D();
            for(auto& hist : histograms_noise) hist = new TH1D();

            hist_TOTvsTOA  = new TH2D("hist_TOTvsTOA", "TOTvsTOA",       100, 0, -1, 100, 0, -1);
            hist_TOTvsVmax = new TH2D("hist_TOTvsVmax","hist_TOTvsVmax", 100, 0, -1, 100, 0, -1);
            // detach from gDirectory so this singleton is the sole owner
            // (otherwise ROOT also deletes them at exit -> double free)
            hist_TOTvsTOA ->SetDirectory(nullptr);
            hist_TOTvsVmax->SetDirectory(nullptr);

            hist_convol    = new TH1D();
            hist_signal    = new TH1D();
            hist_kernel    = new TH1D();
            hist_rms_noise = new TH1D();
            hist_JitterCFD = new TH1D();
            hist_JitterLE  = new TH1D();
            hist_JitterRM  = new TH1D();
        }

        ~HistogramsManager() {
            for(auto& hist : histograms) delete hist;
            for(auto& hist : histograms_noise) delete hist;
            delete hist_TOTvsTOA;
            delete hist_TOTvsVmax;
            delete hist_convol;
            delete hist_signal;
            delete hist_kernel;
            delete hist_rms_noise;
            delete hist_JitterCFD;
            delete hist_JitterLE;
            delete hist_JitterRM;
        }


        inline void _setconfig(TH1D* hist, HistConfig const& conf) const {

            hist->SetNameTitle (conf.name, conf.title);
            hist->SetBins( conf.Nbins, conf.min, conf.max);
            if(conf.max < conf.min) hist->SetBuffer(1000);
            hist->GetXaxis()->SetTitle(conf.xtitle);
            hist->GetYaxis()->SetTitle(conf.ytitle);
            //hist->GetYaxis()->SetLabelSize(0.03);
            //hist->GetYaxis()->SetTitleOffset(1.5);

        }

        Histograms_t histograms;
        Histograms_t histograms_noise;
        TH2D*        hist_TOTvsTOA;
        TH2D*        hist_TOTvsVmax;
        TH1D*        hist_convol;
        TH1D*        hist_signal;
        TH1D*        hist_kernel;
        TH1D*        hist_rms_noise;
        TH1D*        hist_JitterCFD;
        TH1D*        hist_JitterLE;
        TH1D*        hist_JitterRM;



};



} //namespace tfboost

#endif
