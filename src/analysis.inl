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
#include <hydra/FFTW.h>


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
#include <tfboost/functions/TIA_BJT_1stage.h>
#include <tfboost/functions/TIA_BJT_2stages.h>
#include <tfboost/functions/TIA_BJT_2stages_GM.h>
#include <tfboost/functions/TIA_IdealInt.h>
#include <tfboost/functions/TIA_MOS.h>
#include <tfboost/functions/DoConvolution.h>
#include <tfboost/noise/Noise.h>
#include <tfboost/ReadConvolution.h>
#include <tfboost/Algorithms.h>
#include <tfboost/Utils.h>

namespace libconf = libconfig;



int main(int argv, char** argc)
{
    tfboost::TFBoostHeader();

    std::cout.precision(10);
    
    /* ----------------------------------------------
     * Get global configuration
     * --------------------------------------------*/
    libconf::Config Cfg;
    Cfg.readFile("../etc/configuration.cfg");
    const libconf::Setting& cfg_root  = Cfg.getRoot();
    
    TString InputDirectory                  = (const char*) cfg_root["InputDirectory"];
    TString OutputDirectory                 = (const char*) cfg_root["OutputDirectory"];

    const TString tf_inputfile              = (const char*) cfg_root["TFFile"];
    const TString TransferFunction          = (const char*) cfg_root["TransferFunction"];
    const TString InputFileExtension        = (const char*) cfg_root["InputFileExtension"];
    const TString conv_inputfile            = (const char*) cfg_root["ConvolutionFile"]; 

    const char*   token                     = (const char*) cfg_root["token"];
    const int     column                    = (int)         cfg_root["column"];
    const size_t  offset                    = (int)         cfg_root["offset"]; 
    const size_t  NlinesToSkip              = (int)         cfg_root["NlinesToSkip"];

    const bool MakeConvolution              = (bool) cfg_root["MakeConvolution"];
    const bool SaveSinglePlotConvolution    = (bool) cfg_root["SaveSinglePlotConvolution"];
    const bool SaveConvDataToFile           = (bool) cfg_root["SaveConvDataToFile"];
    const bool MakeLinearFitNearThreshold   = (bool) cfg_root["MakeLinearFitNearThreshold"];
    const bool MakeGaussianFitNearVmax      = (bool) cfg_root["MakeGaussianFitNearVmax"];
    const bool AddNoise                     = (bool) cfg_root["AddNoise"];
    const bool UseRedNoise                  = (bool) cfg_root["UseRedNoise"];
    const bool MakeTheoreticalTOA           = (bool) cfg_root["MakeTheoreticalTOA"];
    const bool UseSameCurve                 = (bool) cfg_root["UseSameCurve"];
    const TString SingleFile                = (const char*) cfg_root["FileName"];
    const size_t IdxConvtoSave              = (int)  cfg_root["IdxConvtoSave"];

    const size_t Nfiles                     = (int)  cfg_root["MaxInputFiles"];
 
    if(!InputDirectory.EndsWith("/")) InputDirectory   = InputDirectory+"/";
    if(!OutputDirectory.EndsWith("/")) OutputDirectory = OutputDirectory+"/";


     /* ----------------------------------------------
     * Configuration of the Transfer Function
     * --------------------------------------------*/
    const libconf::Setting& cfg_tf          = cfg_root[TransferFunction];

    const size_t Nsamples                   = (int)    cfg_tf["Nsamples"];
    const int    ID                         = (int)    cfg_tf["ID"];
    const int    Nbins                      = (int)    cfg_tf["Nbins"];
    const double dT                         = (double) cfg_tf["dT"];
    const double LEthr                      = (double) cfg_tf["LEthr"];
    const double CFD_fr                     = (double) cfg_tf["CFD_fr"];
    const double sigma_noise                = (double) cfg_tf["sigma_noise"];
    const double r_rednoise                 = (double) cfg_tf["r_rednoise"];


    
    /* ----------------------------------------------
     * Initialization of variables and histograms
     * --------------------------------------------*/
    tfboost::CreateDirectories( OutputDirectory + "plots");
    tfboost::CreateDirectories( OutputDirectory + "data");

    const double min         = 0.0;
    const double max         = (Nsamples-1)*dT;
    const double min_kernel  = -0.5*(max-min);
    const double max_kernel  =  0.5*(max-min);

    size_t INDEX = 0;
    
    hydra::SeedRNG S{};

    TList* listoffiles = tfboost::GetFileList(InputDirectory); 
    listoffiles->Sort();
    TIter nextfile( listoffiles );
    TSystemFile *currentfile;
    TString currentfilename;
    TString line;

    TH1D* hist_TOA             = new TH1D("hist_TOA","Time of arrival Leading Edge",Nbins,0,-1);
    TH1D* hist_TOACFD          = new TH1D("hist_TOACFD","Time of arrival CFD at 0.35 of V max",Nbins,0,-1);
    TH1D* hist_TOACFDnoise     = new TH1D("hist_TOACFDnoise","Time of arrival CFD with noise",Nbins,0,-1);
    TH1D* hist_TOALEnoise      = new TH1D("hist_TOALEnoise","Time of arrival LE with noise",Nbins,0,-1);
    TH1D* hist_Vmax            = new TH1D("hist_Vmax","V on peak",Nbins,0,-1);
    TH1D* hist_TimeatVmax      = new TH1D("hist_TimeatVmax","Time at V max",Nbins,0,-1);
    TH1D* hist_Vmax_noise      = new TH1D("hist_Vmax_noise","V on peak with noise",Nbins,0,-1);
    TH1D* hist_Vth_CFD         = new TH1D("hist_Vth_CFD","V on CFD threshold",Nbins,0,-1);
    TH1D* hist_Vth_LE          = new TH1D("hist_Vth_LE","V on LE threshold",Nbins,0,-1);
    TH1D* hist_Vth_CFD_noise   = new TH1D("hist_Vth_CFD_noise","V on CFD threshold with noise",Nbins,0,-1);
    TH1D* hist_Vth_LE_noise    = new TH1D("hist_Vth_LE_noise","V on LE threshold with noise",Nbins,0,-1);
    TH1D* hist_dVdt_LE         = new TH1D("hist_dVdt_LE","Slope (dV/dT) on LE threshold",Nbins,0,-1);
    TH1D* hist_dVdt_CFD        = new TH1D("hist_dVdt_CFD","Slope (dV/dT) on CFD threshold",Nbins,0,-1);
    TH1D* hist_dVdt_CFD_noise  = new TH1D("hist_dVdt_CFD_noise","Slope (dV/dT) on CFD threshold with noise",Nbins,0,-1);
    TH1D* hist_dVdt_LE_noise   = new TH1D("hist_dVdt_LE_noise","Slope (dV/dT) on LE threshold with noise",Nbins,0,-1);

    TH1D *hist_convol          = new TH1D("hist_convol;Time[s];Vout [V]","hist_convol", Nsamples, min, max );
    TH1D *hist_signal          = new TH1D("hist_signal;Time[s];Vout [V]","hist_signal", Nsamples, min, max );
    TH1D *hist_kernel          = new TH1D("hist_kernel;Time[s];Vout [V]","hist_kernel", Nsamples, min, max);

    //TH2D *TOAmaps              = new TH2D("TOAmaps;Y;X","TOAmaps", 200, 0, 200, 56, 0, 56);
    //TH2D *Vmaxmaps             = new TH2D("Vmaxmaps;Y;X","Vmaxmaps", 200, 0, 200, 56, 0, 56);

    hydra::host::vector<double> th_jitter_withTOA(120, 0.0);

    

    /* ----------------------------------------------
     * Main loop on input files
     * --------------------------------------------*/
    while( (currentfile = (TSystemFile*) nextfile() ) && INDEX < Nfiles )
    {

        auto start_d = std::chrono::high_resolution_clock::now();
    
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
      
        currentfilename = currentfile->GetName();

        if(UseSameCurve) currentfilename = SingleFile;
        
        //std::pair<double,double> pos = tfboost::GetHitPosition(currentfilename);
        //int pos_x = pos.first;
        //int pos_y = pos.second;

        
        if ( currentfile->IsDirectory() || !currentfilename.EndsWith(InputFileExtension) ) continue;

        std::cout << "========================================" << "\n";
        std::cout << "|  FILE  : "<<currentfilename<<" " << "\n";
        std::cout << "|  INDEX : "<<INDEX<<" "           << "\n";
        std::cout << "========================================" << "\n\n";
        ++INDEX;


        std::ifstream myFile( (InputDirectory+currentfilename).Data() );
        for(size_t j=0; j<NlinesToSkip; ++j) line.ReadLine(myFile);
        

        // Actual loop on file lines
        if (myFile.is_open()) 
        {
            size_t s = offset;

            for(size_t j = offset; j < Nsamples ; ++j)
            {
                line.ReadLine(myFile);
                if (!myFile.good()) break;
            
                TObjArray *tokens = line.Tokenize( token );
            
                TString data_str  = ((TObjString*) tokens->At( column ) )->GetString();
                if(data_str == "0" && s==offset) continue; //to avoid misaligned TCoDe input files 
                
                const double data = atof(data_str);
                
                idx.push_back(s);
                time.push_back(s*dT);
                current.push_back(data);
                
                tokens->Delete();
                delete tokens;

                ++s;
            }
        } else {
            SAFE_EXIT( true , "In analysis.inl: input file cannot be open. ")
        }

        myFile.close();


        
        if(Nsamples - current.size() > 0)
        {
            for(size_t k=current.size(); k<Nsamples; ++k)
            {
                idx.push_back(k);
                time.push_back(k*dT);
                current.push_back(0.0);
            }
        }
        
        SAFE_EXIT( current.size() != Nsamples , "In analysis.inl: size of container not equal to Nsamples. ")

        auto signal = hydra::make_spiline<double>(time, current );
        
        
        
        
        /* ----------------------------------------------
         * Performing the convolution
         * --------------------------------------------*/

        #if HYDRA_DEVICE_SYSTEM!=CUDA
        auto fft_backend = hydra::fft::fftw_f64;
        #endif
        
        hydra::host::vector<double >    conv_data_h(Nsamples);
        hydra::device::vector<double >  conv_data_d(Nsamples);
        
        if(MakeConvolution){

        switch(ID) 
        {
            case 0:{
                auto kernel = tfboost::TIA_MOS<double>( cfg_tf );
                conv_data_h = tfboost::Do_Convolution(fft_backend, kernel, signal, min, max, Nsamples);
                if(SaveSinglePlotConvolution && INDEX==IdxConvtoSave) {
                    for(size_t i=1;  i < (size_t) Nsamples+1; ++i) 
                        hist_kernel->SetBinContent(i, kernel(hist_kernel->GetBinCenter(i)) );  
                }
                }break;

            case 1:{
                auto kernel = tfboost::TIA_BJT_2stages<double>( cfg_tf );
                conv_data_h = tfboost::Do_Convolution(fft_backend, kernel, signal, min, max, Nsamples);
                if(SaveSinglePlotConvolution && INDEX==IdxConvtoSave) {
                    for(size_t i=1;  i < (size_t) Nsamples+1; ++i) 
                        hist_kernel->SetBinContent(i, kernel(hist_kernel->GetBinCenter(i)) );  
                }
                }break;

            case 2:{
                auto kernel = tfboost::TIA_BJT_2stages_GM<double>( cfg_tf );
                conv_data_h = tfboost::Do_Convolution(fft_backend, kernel, signal, min, max, Nsamples);
                if(SaveSinglePlotConvolution && INDEX==IdxConvtoSave) {
                    for(size_t i=1;  i < (size_t) Nsamples+1; ++i) 
                        hist_kernel->SetBinContent(i, kernel(hist_kernel->GetBinCenter(i)) );  
                }
                }break;

            case 3:{
                auto kernel = tfboost::TIA_BJT_1stage<double>( cfg_tf );
                conv_data_h = tfboost::Do_Convolution(fft_backend, kernel, signal, min, max, Nsamples);
                if(SaveSinglePlotConvolution && INDEX==IdxConvtoSave) {
                    for(size_t i=1;  i < (size_t) Nsamples+1; ++i) 
                        hist_kernel->SetBinContent(i, kernel(hist_kernel->GetBinCenter(i)) );  
                }
                }break;

            case 4:{
                auto kernel = tfboost::TIA_IdealInt<double>( cfg_tf );
                conv_data_h = tfboost::Do_Convolution(fft_backend, kernel, signal, min, max, Nsamples);
                if(SaveSinglePlotConvolution && INDEX==IdxConvtoSave) {
                    for(size_t i=1;  i < (size_t) Nsamples+1; ++i) 
                        hist_kernel->SetBinContent(i, kernel(hist_kernel->GetBinCenter(i)) );  
                }
                }break;

            case 5:{
                hydra::device::vector<double> time2;
                hydra::device::vector<double> current2;

                tfboost::ReadTF( tf_inputfile, time2, current2);

                auto kernel = hydra::make_spiline<double>(time2, current2 );
                
                conv_data_h = tfboost::Do_Convolution(fft_backend, kernel, signal, min, max, Nsamples);
                if(SaveSinglePlotConvolution && INDEX==IdxConvtoSave) {
                    for(size_t i=1;  i < (size_t) Nsamples+1; ++i) 
                        hist_kernel->SetBinContent(i, kernel(hist_kernel->GetBinCenter(i)) );  
                }

            }break;

            default:
                SAFE_EXIT( true , "In analysis.inl: bad transfer function ID.")
        }//end switch

        hydra::copy(conv_data_h , conv_data_d);

        } else {
            tfboost::ReadConvolution( conv_inputfile, conv_data_h);
            hydra::copy(conv_data_h , conv_data_d);
        }
          



          
        /* ----------------------------------------------
         * Filling histogram for visualization
         * --------------------------------------------*/ 
        if(SaveSinglePlotConvolution && INDEX==IdxConvtoSave)
        {
            for(size_t i=1;  i < (size_t) Nsamples+1; ++i)
            {
                hist_convol->SetBinContent(i, conv_data_h[i-1] );
                hist_signal->SetBinContent(i, signal(hist_signal->GetBinCenter(i)) );
            }
        }
               
        
        /* ----------------------------------------------
         * Measurements without noise
         * --------------------------------------------*/

        // Condition to avoid to process empty signal
        // reject signal with amplitude < 1 mV
        if( tfboost::algo::LeadingEdge(conv_data_h, 0.001) == 0) 
        { 
            //TOAmaps->SetBinContent( pos_y+1, pos_x+1, 0.0 ); 
            //Vmaxmaps->SetBinContent( pos_y+1, pos_x+1, 0.0 );
            continue;
        }


        size_t TOA_LE     = tfboost::algo::LeadingEdge(conv_data_h, LEthr);

        size_t timeatth_0 = tfboost::algo::GetTimeAtPeak(conv_data_h);
        double Vpeak      = tfboost::algo::GetVAtPeak(conv_data_d);
        
        size_t TOA_CFD    = tfboost::algo::ConstantFraction(conv_data_h , CFD_fr , Vpeak);
        double VonThLE    = conv_data_h[TOA_LE];
        double VonThCFD   = conv_data_h[TOA_CFD];

        //TOAmaps->SetBinContent( pos_y+1, pos_x+1, (double)dT*TOA_CFD );
        //Vmaxmaps->SetBinContent( pos_y+1, pos_x+1, 1e3 * Vpeak );
 
                
        double dvdt_CFD   = tfboost::algo::SlopeOnThrs(conv_data_h, TOA_CFD);
        double dvdt_LE    = tfboost::algo::SlopeOnThrs(conv_data_h, TOA_LE);
        
        if(MakeTheoreticalTOA){
            auto prob_curve = tfboost::noise::ComputeTOAcurve( TOA_CFD, TOA_CFD-100, TOA_CFD+100, CFD_fr, Vpeak, sigma_noise, idx, conv_data_h, "th_jitter.pdf");
            for(size_t i=0; i<prob_curve.size(); ++i) th_jitter_withTOA[i] += prob_curve[i];
        }
        
        hist_TOA        -> Fill( (double) dT*TOA_LE );
        hist_TOACFD     -> Fill( (double) dT*TOA_CFD);
        hist_Vmax       -> Fill(1e3 * Vpeak);
        hist_TimeatVmax -> Fill(timeatth_0);
        hist_Vth_CFD    -> Fill(1e3*VonThCFD);
        hist_Vth_LE     -> Fill(1e3*VonThLE);
        hist_dVdt_CFD   -> Fill(1e-6 * dvdt_CFD/dT);
        hist_dVdt_LE    -> Fill(1e-6 * dvdt_LE/dT);

        std::cout << "========================================" << "\n";
        std::cout << "Time on thresholds (LE)  = " << dT*TOA_LE  << "  (s)"<< "\n";
        std::cout << "Time on thresholds (CFD) = " << dT*TOA_CFD << "  (s)"<< "\n";
        std::cout << "V on thresholds (CFD)    = " << 1e3*VonThCFD << "  (mV)"<< "\n";
        std::cout << "V on thresholds (LE)     = " << 1e3*VonThLE << "  (mV)"<< "\n";
        std::cout << "Vpeak                    = " << 1e3*Vpeak << "  (mV)"<< "\n";
        std::cout << "dv/dt CFD                = " << 1e-6*dvdt_CFD/dT << "  (uV/ps)"<< "\n";
        std::cout << "dv/dt LE                 = " << 1e-6*dvdt_LE/dT  << "  (uV/ps)"<< "\n";    
        std::cout << "========================================" << "\n";




        /*-------------------------------------------------
         * Adding noise
         *------------------------------------------------*/
        if(AddNoise)
        {
        
            auto noise = tfboost::noise::Noise(sigma_noise, UseRedNoise, r_rednoise);
            
            noise.AddNoiseToSignal( conv_data_h, S() );
            
            
            size_t TOA_LE_noise  = tfboost::algo::LeadingEdge(conv_data_h , LEthr);
            double VonThLE       = conv_data_h[TOA_LE_noise];

            hist_TOALEnoise      -> Fill(TOA_LE_noise);
            hist_Vth_LE_noise    -> Fill(VonThLE);
            
            if(MakeGaussianFitNearVmax && TOA_CFD>1)
            {
                size_t timeatth      = tfboost::algo::GetTimeAtPeak(conv_data_h);
                double vmax          = tfboost::algo::GaussianFitNearVmax( ID,  conv_data_h );
                size_t TOA_CFD_noise = tfboost::algo::ConstantFraction(conv_data_h , CFD_fr , vmax);
                double VonThCFD = conv_data_h[TOA_CFD_noise];

                hist_TOACFDnoise     -> Fill(TOA_CFD_noise);
                hist_Vmax_noise      -> Fill(vmax);
                hist_Vth_CFD_noise   -> Fill(VonThCFD);                
            }
            
            
            if(MakeLinearFitNearThreshold && TOA_LE>1)
            {
                double par1_fit = tfboost::algo::LinearFitNearThr( ID, TOA_CFD, conv_data_h, true );
                std::cout << "dv/dt at CFD fitted      = " << 1e-6*par1_fit/dT << "  (uV/ps)" << "\n";
                hist_dVdt_CFD_noise->Fill(1e-6*par1_fit/dT);
                
                double par2_fit = tfboost::algo::LinearFitNearThr( ID, TOA_LE, conv_data_h );
                std::cout << "dv/dt at LE fitted       = " << 1e-6*par2_fit/dT << "  (uV/ps)" << "\n";
                hist_dVdt_LE_noise->Fill(1e-6*par2_fit/dT);
                std::cout << "========================================" << "\n";
            }

            if(SaveSinglePlotConvolution && INDEX==IdxConvtoSave)
            {
                for(size_t i=1;  i < (size_t) Nsamples+1; ++i)
                {
                    hist_convol->SetBinContent(i, conv_data_h[i-1] );
                }

            }

        }
    
        

        if(SaveSinglePlotConvolution && INDEX==IdxConvtoSave)
        {

            TCanvas canvas("canvas" ,"canvas", 4000, 1000);
            canvas.Divide(3,1);
            canvas.cd(1);
            hist_convol->SetStats(0);
            hist_convol->SetLineColor(4);
            hist_convol->SetLineWidth(1);
            hist_convol->Draw();
            
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
            hist_convol->SetLineColor(1);
            hist_convol->Draw("same");
            
            canvas.SaveAs(OutputDirectory + "plots/hist_convol_functor.pdf");

        }

        if(SaveConvDataToFile) tfboost::SaveConvToFile(conv_data_h, dT, OutputDirectory + "data/" +currentfilename );

        

    }//end loop on files
    



    /*-------------------------------------------------
     *  Save all the plots
     *------------------------------------------------*/  
    tfboost::SaveCanvas(OutputDirectory + "plots/", "TOA_LE",    "Time [s]",      "counts", *hist_TOA);
    tfboost::SaveCanvas(OutputDirectory + "plots/", "TOA_CFD",   "Time [s]",      "counts", *hist_TOACFD);
    tfboost::SaveCanvas(OutputDirectory + "plots/", "dVdT_LE",   "Slope [uV/ps]", "counts", *hist_dVdt_LE);
    tfboost::SaveCanvas(OutputDirectory + "plots/", "dVdT_CFD",  "Slope [uV/ps]", "counts", *hist_dVdt_CFD);
    tfboost::SaveCanvas(OutputDirectory + "plots/", "Vmax",      "Voltage [mV]",  "counts", *hist_Vmax);
    tfboost::SaveCanvas(OutputDirectory + "plots/", "TimeatVmax","Time [ps]",     "counts", *hist_TimeatVmax);
    tfboost::SaveCanvas(OutputDirectory + "plots/", "Vth_CFD",   "Voltage [mV]",  "counts", *hist_Vth_CFD);
    tfboost::SaveCanvas(OutputDirectory + "plots/", "Vth_LE",    "Voltage [mV]",  "counts", *hist_Vth_LE);
    

    if(AddNoise)
    {
        tfboost::SaveCanvas(OutputDirectory + "plots/", "VonTh_LEnoise", "Voltage [mV]", "counts",  *hist_Vth_LE_noise);
        tfboost::SaveCanvas(OutputDirectory + "plots/", "TOA_LEnoise",   "Time [s]",     "counts",  *hist_TOALEnoise);

        if(MakeGaussianFitNearVmax)
        {
            tfboost::SaveCanvas(OutputDirectory + "plots/", "TOA_CFDnoise",   "Time [s]",     "counts", *hist_TOACFDnoise);
            tfboost::SaveCanvas(OutputDirectory + "plots/", "Vmax_noise",     "Voltage [mV]", "counts", *hist_Vmax_noise);
            tfboost::SaveCanvas(OutputDirectory + "plots/", "VonTh_CFDnoise", "Voltage [mV]", "counts", *hist_Vth_CFD_noise);
        }

        if(MakeLinearFitNearThreshold)
        {
            tfboost::SaveCanvas(OutputDirectory + "plots/", "dVdT_CFDnoise", "Slope [uV/ps]", "counts", *hist_dVdt_CFD_noise);
            tfboost::SaveCanvas(OutputDirectory + "plots/", "dVdT_LEnoise",  "Slope [uV/ps]", "counts", *hist_dVdt_LE_noise);    
        }
    }


    if(MakeTheoreticalTOA)
    {

        TGraph hist_thjitter_withTOA;
        for(size_t i=0;  i < th_jitter_withTOA.size(); ++i)
        {
            hist_thjitter_withTOA.SetPoint(hist_thjitter_withTOA.GetN(), i+160 , th_jitter_withTOA[i]);;
        }
                
        TCanvas canv_thjitter_withTOA("canv_thjitter_withTOA", "canv_thjitter_withTOA", 800, 800);
        hist_thjitter_withTOA.SetTitle("Jitter from first principles;time [ps];A.U.");
        hist_thjitter_withTOA.Draw("APL");
        canv_thjitter_withTOA.SaveAs(OutputDirectory + "plots/thjitter_differentcurves.pdf");
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






