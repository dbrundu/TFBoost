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
 *      Author: Davide Brundu
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
#include <tfboost/functions/ExpModifiedGaussian.h>
#include <tfboost/noise/Noise.h>
#include <tfboost/ReadConvolution.h>
#include <tfboost/Algorithms.h>
#include <tfboost/ConfigParser.h>
#include <tfboost/functions/BaseTrFunction.h>

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
    
    tfboost::ConfigParser c(cfg_root);
    
   
    
    /* ----------------------------------------------
     * Initialization of variables and histograms
     * --------------------------------------------*/
    tfboost::CreateDirectories( c.OutputDirectory + "plots");
    tfboost::CreateDirectories( c.OutputDirectory + "data");

    const double min         = 0.0;
    const double max         = (c.Nsamples-1) * c.dT;
    const double minplot     = c.minplot;
    const double maxplot     = c.maxplot;
    const double min_kernel  = -0.5*(max-min);
    const double max_kernel  =  0.5*(max-min);

    size_t INDEX = 0;
    
    hydra::SeedRNG S{};
    hydra::default_random_engine engine( S() );
    TRandom3 root_rng( S() );

    TH1D* hist_TOA             = new TH1D("hist_TOA","Time of arrival Leading Edge", c.Nbins,0,-1);
    TH1D* hist_TOACFD          = new TH1D("hist_TOACFD","Time of arrival CFD at 0.35 of V max", c.Nbins,0,-1);
    TH1D* hist_TOARM           = new TH1D("hist_TOARM","Time of arrival Ref.Method", c.Nbins,0,-1);
    TH1D* hist_TOACFDnoise     = new TH1D("hist_TOACFDnoise","Time of arrival CFD with noise", c.Nbins,0,-1);
    TH1D* hist_TOALEnoise      = new TH1D("hist_TOALEnoise","Time of arrival LE with noise", c.Nbins,0,-1);
    TH1D* hist_TOARMnoise      = new TH1D("hist_TOARMnoise","Time of arrival RM with noise", c.Nbins,0,-1);
    TH1D* hist_Vmax            = new TH1D("hist_Vmax","V on peak", c.Nbins,0,-1);
    TH1D* hist_TimeatVmax      = new TH1D("hist_TimeatVmax","Time at V max", c.Nbins,0,-1);
    TH1D* hist_Vmax_noise      = new TH1D("hist_Vmax_noise","V on peak with noise",c.Nbins,0,-1);
    TH1D* hist_Vth_CFD         = new TH1D("hist_Vth_CFD","V on CFD threshold",c.Nbins,0,-1);
    TH1D* hist_Vth_LE          = new TH1D("hist_Vth_LE","V on LE threshold",c.Nbins,0,-1);
    TH1D* hist_Vth_RM          = new TH1D("hist_Vth_RM","V on RM threshold",c.Nbins,0,-1);
    TH1D* hist_Vth_CFD_noise   = new TH1D("hist_Vth_CFD_noise","V on CFD threshold with noise",c.Nbins,0,-1);
    TH1D* hist_Vth_RM_noise    = new TH1D("hist_Vth_RM_noise","V on RM threshold with noise",c.Nbins,0,-1);
    TH1D* hist_Vth_LE_noise    = new TH1D("hist_Vth_LE_noise","V on LE threshold with noise",c.Nbins,0,-1);
    TH1D* hist_dVdt_LE         = new TH1D("hist_dVdt_LE","Slope (dV/dT) on LE threshold",c.Nbins,0,-1);
    TH1D* hist_dVdt_CFD        = new TH1D("hist_dVdt_CFD","Slope (dV/dT) on CFD threshold",c.Nbins,0,-1);
    TH1D* hist_dVdt_RM         = new TH1D("hist_dVdt_RM","Slope (dV/dT) on RM threshold",c.Nbins,0,-1);
    TH1D* hist_dVdt_CFD_noise  = new TH1D("hist_dVdt_CFD_noise","Slope (dV/dT) on CFD threshold with noise",c.Nbins,0,-1);
    TH1D* hist_dVdt_LE_noise   = new TH1D("hist_dVdt_LE_noise","Slope (dV/dT) on LE threshold with noise",c.Nbins,0,-1);

    TH1D *hist_convol          = new TH1D("hist_convol;Time[s];Vout [V]","hist_convol", c.Nsamples, minplot, maxplot );
    TH1D *hist_signal          = new TH1D("hist_signal;Time[s];Vout [V]","hist_signal", c.Nsamples, minplot, maxplot );
    TH1D *hist_kernel          = new TH1D("hist_kernel;Time[s];Vout [V]","hist_kernel", c.Nsamples/30, minplot, maxplot );

    //TH2D *TOAmaps              = new TH2D("TOAmaps;Y;X","TOAmaps", 200, 0, 200, 56, 0, 56);
    //TH2D *Vmaxmaps             = new TH2D("Vmaxmaps;Y;X","Vmaxmaps", 200, 0, 200, 56, 0, 56);

    hydra::host::vector<double> th_jitter_withTOA(120, 0.0);

    
    
    /* ----------------------------------------------
     * Preparing the list of input files
     * --------------------------------------------*/
    TList* listoffiles = tfboost::GetFileList(c.InputDirectory); 
    listoffiles->Sort();
    TIter nextfile( listoffiles );
    TSystemFile *currentfile;
    TString currentfilename;
    TString line;


    /* ----------------------------------------------
     * Main loop on input files
     * --------------------------------------------*/
    while( (currentfile = (TSystemFile*) nextfile() ) && INDEX < c.Nfiles )
    {

        auto start_d = std::chrono::high_resolution_clock::now();

        //Clean the configuration
        c.ResetInitialValues();
    
        hydra::host::vector<double> time;
        hydra::host::vector<double> idx;
        hydra::host::vector<double> current;
       
        time.reserve(c.Nsamples);
        idx.reserve(c.Nsamples);
        current.reserve(c.Nsamples);
    
        for(size_t k=0; k < c.offset; ++k)
        {
            idx.push_back(k);
            time.push_back(k * c.dT);
            current.push_back(0.0);
        }
      
        currentfilename = currentfile->GetName();

        if(c.UseSameCurve) currentfilename = c.SingleFile;
        
        //std::pair<double,double> pos = tfboost::GetHitPosition(currentfilename);
        //int pos_x = pos.first;
        //int pos_y = pos.second;

        
        if ( currentfile->IsDirectory() || !currentfilename.EndsWith(c.InputFileExtension) ) continue;

        std::cout << "========================================" << "\n";
        std::cout << "|  FILE  : "<<currentfilename<<" " << "\n";
        std::cout << "|  INDEX : "<<INDEX<<" "           << "\n";
        std::cout << "========================================" << "\n\n";
        ++INDEX;


        std::ifstream myFile( (c.InputDirectory+currentfilename).Data() );
        for(size_t j=0; j < c.NlinesToSkip; ++j) line.ReadLine(myFile);
        

        // Actual loop on file lines
        double landau = root_rng.Landau(1,0.15);
        
        if (myFile.is_open()) 
        {
            size_t s = c.offset;

            for(size_t j = c.offset; j < c.Nsamples ; ++j)
            {
                line.ReadLine(myFile);
                if (!myFile.good()) break;
            
                TObjArray *tokens = line.Tokenize( c.token.Data() );
            
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

        hydra::device::vector<double> time_d(time.size());
        hydra::device::vector<double> current_d(current.size());
        hydra::copy(time, time_d);
        hydra::copy(current, current_d);

        auto signal   = hydra::make_spiline<double>(time, current );
        auto signal_d = hydra::make_spiline<double>(time_d, current_d );

        
        if(c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave)
            for(size_t i=1;  i < (size_t) c.Nsamples+1; ++i)
                hist_signal->SetBinContent(i, signal(hist_signal->GetBinCenter(i)) );
        
        
        
        
        /* ----------------------------------------------
         * Performing the convolution
         * --------------------------------------------*/

        #if HYDRA_DEVICE_SYSTEM!=CUDA
        auto fft_backend = hydra::fft::fftw_f64;
        #endif
        
        #if HYDRA_DEVICE_SYSTEM==CUDA
        auto fft_backend = hydra::fft::cufft_f64;
        #endif
        
        hydra::host::vector<double> conv_data_h(c.Nsamples);

        
        if(c.MakeConvolution){

        switch(c.ID) 
        {
            case 0:{
                auto kernel = tfboost::TIA_MOS<double>( cfg_tf );
                SAFE_EXIT(kernel.IDX != c.ID , "Wrong Tr.Function configuration ID.")
                conv_data_h = tfboost::Do_Convolution(fft_backend, kernel, signal_d, min, max, c.Nsamples);
                if(c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave) {
                    for(size_t i=1;  i < (size_t) c.Nsamples+1; ++i) 
                        hist_kernel->SetBinContent(i, kernel(hist_kernel->GetBinCenter(i)) );  
                }
                }break;

            case 1:{
                auto kernel = tfboost::TIA_BJT_2stages<double>( cfg_tf );
                SAFE_EXIT(kernel.IDX != c.ID , "Wrong Tr.Function configuration ID.")
                conv_data_h = tfboost::Do_Convolution(fft_backend, kernel, signal_d, min, max, c.Nsamples);
                if(c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave) {
                    for(size_t i=1;  i < (size_t) c.Nsamples+1; ++i) 
                        hist_kernel->SetBinContent(i, kernel(hist_kernel->GetBinCenter(i)) );  
                }
                }break;

            case 2:{
                auto kernel = tfboost::TIA_BJT_2stages_GM<double>( cfg_tf );
                SAFE_EXIT(kernel.IDX != c.ID , "Wrong Tr.Function configuration ID.")
                conv_data_h = tfboost::Do_Convolution(fft_backend, kernel, signal_d, min, max, c.Nsamples);
                if(c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave) {
                    for(size_t i=1;  i < (size_t) c.Nsamples+1; ++i) 
                        hist_kernel->SetBinContent(i, kernel(hist_kernel->GetBinCenter(i)) );  
                }
                }break;

            case 3:{
                auto kernel = tfboost::TIA_BJT_1stage<double>( cfg_tf );
                SAFE_EXIT(kernel.IDX != c.ID , "Wrong Tr.Function configuration ID.")
                conv_data_h = tfboost::Do_Convolution(fft_backend, kernel, signal_d, min, max, c.Nsamples);
                if(c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave) {
                    for(size_t i=1;  i < (size_t) c.Nsamples+1; ++i) 
                        hist_kernel->SetBinContent(i, kernel(hist_kernel->GetBinCenter(i)) );  
                }
                }break;

            case 4:{
                auto kernel = tfboost::TIA_IdealInt<double>( cfg_tf );
                SAFE_EXIT(kernel.IDX != c.ID , "Wrong Tr.Function configuration ID.")
                conv_data_h = tfboost::Do_Convolution(fft_backend, kernel, signal_d, min, max, c.Nsamples);
                if(c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave) {
                    for(size_t i=1;  i < (size_t) c.Nsamples+1; ++i) 
                        hist_kernel->SetBinContent(i, kernel(hist_kernel->GetBinCenter(i)) );  
                }
                }break;

            case 5:{

                hydra::host::vector<double> time2;
                hydra::host::vector<double> current2;

                int Nskip = (int) cfg_tf["NlinesToSkip"];
                TString tf_infile = (const char*) cfg_tf["TFFile"];

                tfboost::ReadTF( tf_infile, Nskip, time2, current2, true);

                auto kernel = hydra::make_spiline<double>(time2, current2 );
                
                conv_data_h = tfboost::Do_Convolution(fft_backend, kernel, signal, min, max, c.Nsamples);

                if(c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave) {
                    for(size_t i=1;  i < (size_t) c.Nsamples+1; ++i) 
                        hist_kernel->SetBinContent(i, kernel(hist_kernel->GetBinCenter(i)) );  
                }

            }break;

            default:
                SAFE_EXIT( true , "In analysis.inl: bad transfer function ID.")
        }//end switch

        


        // This step is done only if we are  not requesting noise
        
        if(c.MakeDigitization && !c.AddNoise)
        {
            hydra::host::vector<double> conv_dig;  
            hydra::host::vector<double> time_dig; 

            //time and conv_data_h are the not digitized ones
            auto conv_spline = hydra::make_spiline<double>(time, conv_data_h );

            tfboost::algo::DigitizeSignal(conv_dig, time_dig, 
                            conv_spline, c.sampling_dT, max, engine, c.randomphase);

            // Override all the conv information
            PRINT_ELEMENTS(10,conv_dig)
            c.Nsamples = conv_dig.size();
            c.dT = c.sampling_dT;
            hist_convol->SetBins(c.Nsamples, min, maxplot);

            // override time and conv_data containers
            conv_data_h.erase(conv_data_h.begin() + conv_dig.size(), conv_data_h.end());
            time.erase(time.begin() + time_dig.size(), time.end());
            hydra::copy(conv_dig , conv_data_h);
            hydra::copy(time_dig , time);
        }
            
            

        } else {
            tfboost::ReadConvolution( c.conv_inputfile, conv_data_h);
        }
          


          
        /* ----------------------------------------------
         * Filling histogram for visualization
         * --------------------------------------------*/ 
        auto conv_spline = hydra::make_spiline<double>(time, conv_data_h );

        if(c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave)
            for(size_t i=1;  i < (size_t) c.Nsamples+1; ++i)
                 hist_convol->SetBinContent(i, conv_spline(hist_convol->GetBinCenter(i)) );  
               
        
        
        
        /* ----------------------------------------------
         * Measurements without noise
         * --------------------------------------------*/

        // Condition to avoid to process empty signal
        // reject signal with amplitude < 1 mV
        if( tfboost::algo::LeadingEdge(conv_data_h, c.LE_reject_nonoise) == 0) 
        { 
            WARNING_LINE("Skipping empty event...")
            //TOAmaps->SetBinContent( pos_y+1, pos_x+1, 0.0 ); 
            //Vmaxmaps->SetBinContent( pos_y+1, pos_x+1, 0.0 );
            continue;
        }


        size_t TOA_LE     = tfboost::algo::LeadingEdge(conv_data_h, c.LEthr);

        size_t timeatth_0 = tfboost::algo::GetTimeAtPeak(conv_data_h);
        
        double Vpeak      = tfboost::algo::GetVAtPeak(conv_data_h);
        
        size_t TOA_CFD    = tfboost::algo::ConstantFraction(conv_data_h , c.CFD_fr , Vpeak);
        
        size_t TOA_RM     = tfboost::algo::TimeRefMethod( conv_data_h, Vpeak, c.bound_fit, /*noise?*/false );
        
        double dvdt_CFD   = tfboost::algo::SlopeOnThrs(conv_data_h, TOA_CFD);
        
        double dvdt_LE    = tfboost::algo::SlopeOnThrs(conv_data_h, TOA_LE);
        
        double dvdt_RM    = tfboost::algo::SlopeOnThrs(conv_data_h, TOA_RM);
        
        double VonThLE    = conv_data_h[TOA_LE];
        
        double VonThCFD   = conv_data_h[TOA_CFD];
        
        double VonThRM    = conv_data_h[TOA_RM];

        //TOAmaps->SetBinContent( pos_y+1, pos_x+1, (double)dT*TOA_CFD );
        //Vmaxmaps->SetBinContent( pos_y+1, pos_x+1, 1e3 * Vpeak );
 
        
        if(c.MakeTheoreticalTOA){
            auto prob_curve = tfboost::noise::ComputeTOAcurve( TOA_CFD, TOA_CFD-100, TOA_CFD+100, c.CFD_fr, Vpeak, c.sigma_noise, idx, conv_data_h, "th_jitter.pdf");
            for(size_t i=0; i<prob_curve.size(); ++i) th_jitter_withTOA[i] += prob_curve[i];
        }
        

        hist_TOA        -> Fill( time[TOA_LE] ); 
        hist_TOACFD     -> Fill( time[TOA_CFD] ); 
        hist_TOARM      -> Fill( time[TOA_CFD] ); 
        hist_Vmax       -> Fill(1e3 * Vpeak);
        hist_TimeatVmax -> Fill( time[timeatth_0] );
        hist_Vth_CFD    -> Fill(1e3*VonThCFD);
        hist_Vth_LE     -> Fill(1e3*VonThLE);
        hist_Vth_RM     -> Fill(1e3*VonThRM);
        hist_dVdt_CFD   -> Fill(1e-6 * dvdt_CFD/c.dT);
        hist_dVdt_LE    -> Fill(1e-6 * dvdt_LE/c.dT);
        hist_dVdt_RM    -> Fill(1e-6 * dvdt_RM/c.dT);

        std::cout << "========================================" << "\n";
        std::cout << "Time on thresholds (LE)  = " << time[TOA_LE]  << "  (s)"<< "\n";
        std::cout << "Time on thresholds (CFD) = " << time[TOA_CFD] << "  (s)"<< "\n";
        std::cout << "Time on thresholds (RM)  = " << time[TOA_RM]  << "  (s)"<< "\n";
        std::cout << "V on thresholds (CFD)    = " << 1e3*VonThCFD << "  (mV)"<< "\n";
        std::cout << "V on thresholds (LE)     = " << 1e3*VonThLE << "  (mV)"<< "\n";
        std::cout << "V on thresholds (RM)     = " << 1e3*VonThRM << "  (mV)"<< "\n";
        std::cout << "Vpeak                    = " << 1e3*Vpeak << "  (mV)"<< "\n";
        std::cout << "dv/dt CFD                = " << 1e-6*dvdt_CFD/c.dT << "  (uV/ps)"<< "\n";
        std::cout << "dv/dt LE                 = " << 1e-6*dvdt_LE/c.dT  << "  (uV/ps)"<< "\n";    
        std::cout << "dv/dt RM                 = " << 1e-6*dvdt_RM/c.dT  << "  (uV/ps)"<< "\n";    
        std::cout << "========================================" << "\n";




        /*-------------------------------------------------
         * Adding noise
         *------------------------------------------------*/
        if(c.AddNoise)
        {
        
            auto noise = tfboost::noise::Noise(c.sigma_noise, c.UseRedNoise, c.r_rednoise);
            
            noise.AddNoiseToSignal( conv_data_h, S() );
            

            // This step is done only if we are requesting noise
            if(c.MakeDigitization)
            {
                hydra::host::vector<double> time_dig;
                hydra::host::vector<double> conv_dig;  

                // time and conv_data_h are the not digitized ones
                auto signalnoise = hydra::make_spiline<double>(time, conv_data_h );

                tfboost::algo::DigitizeSignal(conv_dig, time_dig, 
                                              signalnoise, c.sampling_dT, 
                                              max, engine, c.randomphase);

                // Override all the conv information
                c.Nsamples = conv_dig.size();
                c.dT = c.sampling_dT;
                hist_convol->SetBins(c.Nsamples, min, max);

                // override time and conv_data containers
                conv_data_h.erase(conv_data_h.begin() + c.Nsamples, conv_data_h.end());
                time.erase(time.begin() + time_dig.size(), time.end());
                hydra::copy(conv_dig , conv_data_h);
                hydra::copy(time_dig , time);

            }
            
            
            size_t TOA_LE_noise  = tfboost::algo::LeadingEdge(conv_data_h , c.LEthr);
            double VonThLE       = conv_data_h[TOA_LE_noise];


            // Condition to avoid to process empty signal
            // reject signal with amplitude < 1 mV
            if( tfboost::algo::LeadingEdge(conv_data_h, c.LE_reject_noise) == 0) 
            { 
                WARNING_LINE("Skipping empty event...")
                continue;
            }


            hist_TOALEnoise      -> Fill( time[TOA_LE_noise] );
            hist_Vth_LE_noise    -> Fill( time[VonThLE] );
            
            if(c.MakeGaussianFitNearVmax && TOA_CFD>1)
            {
                size_t timeatth      = tfboost::algo::GetTimeAtPeak(conv_data_h);
                double vmax          = tfboost::algo::GaussianFitNearVmax( conv_data_h, c.bound_fit );
                size_t TOA_CFD_noise = tfboost::algo::ConstantFraction(conv_data_h , c.CFD_fr , vmax);
                size_t TOA_RM_noise  = tfboost::algo::TimeRefMethod( conv_data_h, vmax, c.bound_fit, /*noise?*/true );
                
                double VonThCFD = conv_data_h[TOA_CFD_noise];
                double VonThRM  = conv_data_h[TOA_RM_noise];

                hist_TOARMnoise      -> Fill( time[TOA_RM_noise] );
                hist_TOACFDnoise     -> Fill( time[TOA_CFD_noise] );
                hist_Vmax_noise      -> Fill(vmax);
                hist_Vth_CFD_noise   -> Fill(VonThCFD);
                hist_Vth_RM_noise    -> Fill(VonThRM);

                if(c.MakeLinearFitNearThreshold && TOA_LE>1)
                {
                    double par1_fit = tfboost::algo::LinearFitNearThr( TOA_CFD_noise, conv_data_h, c.bound_fit, true );
                    std::cout << "dv/dt at CFD fitted      = " << 1e-6*par1_fit/c.dT << "  (uV/ps)" << "\n";
                    hist_dVdt_CFD_noise->Fill(1e-6*par1_fit/c.dT);
                }

            }
            
            
            if(c.MakeLinearFitNearThreshold && TOA_LE>1)
            {
                double par2_fit = tfboost::algo::LinearFitNearThr( TOA_LE_noise, conv_data_h, c.bound_fit );
                std::cout << "dv/dt at LE fitted       = " << 1e-6*par2_fit/c.dT << "  (uV/ps)" << "\n";
                hist_dVdt_LE_noise->Fill(1e-6*par2_fit/c.dT);
                std::cout << "========================================" << "\n";
            }


            auto conv_spline = hydra::make_spiline<double>(time, conv_data_h );
            if(c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave)
                for(size_t i=1;  i < (size_t) c.Nsamples+1; ++i)
                    hist_convol->SetBinContent(i, conv_spline(hist_convol->GetBinCenter(i)) ); 

        }
   

        if(c.SaveSinglePlotConvolution && INDEX==c.IdxConvtoSave)
           tfboost::SaveConvolutionCanvas(c.OutputDirectory + "plots/", "hist_convol_functor", 
                          *hist_convol, *hist_signal, *hist_kernel);


        if(c.SaveConvDataToFile) tfboost::SaveConvToFile(conv_data_h, c.dT, c.OutputDirectory + "data/" +currentfilename );

        

    }//end loop on files
    



    /*-------------------------------------------------
     *  Save all the plots
     *------------------------------------------------*/  
    tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOA_LE",    "Time [s]",      "counts", *hist_TOA);
    tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOA_CFD",   "Time [s]",      "counts", *hist_TOACFD);
    tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOA_RM",    "Time [s]",      "counts", *hist_TOARM);
    
    tfboost::SaveCanvas(c.OutputDirectory + "plots/", "dVdT_LE",   "Slope [uV/ps]", "counts", *hist_dVdt_LE);
    tfboost::SaveCanvas(c.OutputDirectory + "plots/", "dVdT_CFD",  "Slope [uV/ps]", "counts", *hist_dVdt_CFD);
    tfboost::SaveCanvas(c.OutputDirectory + "plots/", "dVdT_RM",  "Slope [uV/ps]", "counts", *hist_dVdt_RM);

    tfboost::SaveCanvas(c.OutputDirectory + "plots/", "Vmax",      "Voltage [mV]",  "counts", *hist_Vmax);
    tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TimeatVmax","Time [ps]",     "counts", *hist_TimeatVmax);
    tfboost::SaveCanvas(c.OutputDirectory + "plots/", "Vth_CFD",   "Voltage [mV]",  "counts", *hist_Vth_CFD);
    tfboost::SaveCanvas(c.OutputDirectory + "plots/", "Vth_LE",    "Voltage [mV]",  "counts", *hist_Vth_LE);
    tfboost::SaveCanvas(c.OutputDirectory + "plots/", "Vth_RM",    "Voltage [mV]",  "counts", *hist_Vth_RM);



    if(c.AddNoise){
        tfboost::SaveCanvas(c.OutputDirectory + "plots/", "VonTh_LEnoise", "Voltage [mV]", "counts",  *hist_Vth_LE_noise);
        tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOA_LEnoise",   "Time [s]",     "counts",  *hist_TOALEnoise);

        if(c.MakeGaussianFitNearVmax){
            gStyle->SetOptFit(1);

            double TOACFDmin = hist_TOACFDnoise->GetXaxis()->GetXmin();
            double TOACFDmax = hist_TOACFDnoise->GetXaxis()->GetXmax();

            double mean  = hist_TOACFDnoise->GetBinCenter( hist_TOACFDnoise->GetMaximumBin() );
            double norm  = hist_TOACFDnoise->Integral("width");
            double sigma = 0.37*hist_TOACFDnoise->GetStdDev();

            tfboost::ExpModifiedGaussian f(0.15e-9, 0.6e-9);
            f.SetParameters( norm, mean, sigma, 0.1e-9, 0.8, 1);
            f.SetParLimits(0, 0.6*norm, 1.4*norm);
            f.SetParLimits(1, 0.6*mean, 1.4*mean);
            f.SetParLimits(2, 0.6*sigma, 1.4*sigma);
            f.SetParLimits(4, 0., 1.);
            (f.fun)->FixParameter(5,1);


            tfboost::SaveCanvasAndFit(c.OutputDirectory + "plots/", "TOA_CFDnoise", "Time [s]",  "counts", *hist_TOACFDnoise, f.fun );
            
            tfboost::SaveCanvas(c.OutputDirectory + "plots/", "TOA_RMnoise",    "Time [s]",     "counts", *hist_TOARMnoise);
            tfboost::SaveCanvas(c.OutputDirectory + "plots/", "Vmax_noise",     "Voltage [mV]", "counts", *hist_Vmax_noise);
            tfboost::SaveCanvas(c.OutputDirectory + "plots/", "VonTh_CFDnoise", "Voltage [mV]", "counts", *hist_Vth_CFD_noise);
            tfboost::SaveCanvas(c.OutputDirectory + "plots/", "VonTh_RMnoise", "Voltage [mV]", "counts",  *hist_Vth_RM_noise);  
        }

        if(c.MakeLinearFitNearThreshold){
            tfboost::SaveCanvas(c.OutputDirectory + "plots/", "dVdT_CFDnoise", "Slope [uV/ps]", "counts", *hist_dVdt_CFD_noise);
            tfboost::SaveCanvas(c.OutputDirectory + "plots/", "dVdT_LEnoise",  "Slope [uV/ps]", "counts", *hist_dVdt_LE_noise);    
        } 
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

/*
    gStyle->SetOptStat(0);

    TCanvas canv_TOAmaps("canv_TOAmaps", "canv_TOAmaps", 4*200, 4*56);
    TOAmaps->Draw("colz");
    canv_TOAmaps.SaveAs("canv_TOAmaps.pdf");

    TCanvas canv_Vmaxmaps("canv_Vmaxmaps", "canv_Vmaxmaps", 4*200, 4*56);
    Vmaxmaps->Draw("colz");
    canv_Vmaxmaps.SaveAs("canv_Vmaxmaps.pdf");
*/

    return 0;
}

#endif






