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
 *  Created on: 08/08/2021
 *    Author: Gian Matteo Cossu
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
  Cfg.readFile("../etc/resampling.cfg");
  const libconf::Setting& cfg_root  = Cfg.getRoot();
  

  const bool SingleFile            = (bool)        cfg_root["SingleFile"];
  const TString InputDirectory     = (const char*) cfg_root["InputDirectory"]; 
  const TString InputFile_current  = (const char*) cfg_root["InputFileCurrent"]; 
  const double dT                  = (double)  cfg_root["dT"];
  const TString file_ext           = (const char*) cfg_root["file_extension"]; 
  const char*  token         = (const char*) cfg_root["token"];
  const int    columnT       = (int)  cfg_root["columnT"];
  const int    columnI       = (int)  cfg_root["columnI"];
  const size_t Nsamples      = (int)  cfg_root["Nsamples"];
  const size_t Nfiles        = (int)  cfg_root["Nfiles"];
  const size_t Nlinestoskip  = (int)  cfg_root["Nlinestoskip"];
  const double scale         = (double) cfg_root["scale_factor"];
  

  /* ----------------------------------------------
   * Initialization of variables and RNGs
   * --------------------------------------------*/

  const double min     = 0.0;
  const double max     = (Nsamples-1)*dT;
  const double min_kernel  = -0.5*(max-min);
  const double max_kernel  =  0.5*(max-min);
  
  
  hydra::SeedRNG S{};
  hydra::default_random_engine engine( S() ); 
  TRandom3 root_rng( S() );

  /* ----------------------------------------------
   * Read Input currents
   * --------------------------------------------*/
  hydra::device::vector<double> time;
  hydra::device::vector<double> idx;
  hydra::device::vector<double> current;
    
  time.reserve(Nsamples);
  current.reserve(Nsamples);
  
  TString line;

  if(SingleFile){
  
  std::ifstream myFile( InputFile_current.Data() );
  
  //skip first lines
  for (int i=0; i<Nlinestoskip; i++){line.ReadLine(myFile);}
    
      // Actual loop on file lines
      if (myFile.is_open()) 
      {
        for(size_t j = 0; j < Nsamples ; ++j)
        {
          line.ReadLine(myFile);
          if (!myFile.good()) break;
          
          TObjArray *tokens = line.Tokenize( token );
          
          TString data_str  = ((TObjString*) tokens->At( columnI ) )->GetString();
          TString time_str  = ((TObjString*) tokens->At( columnT ) )->GetString();
            
          const double data = atof(data_str);
          const double tm   = atof(time_str);
            
          idx.push_back(j);
          time.push_back(tm);
          current.push_back(data);

          //std::cout<<tm<<" "<<data<<std::endl;
            
          tokens->Delete();
          delete tokens;
        }
      } else { SAFE_EXIT(true, "Input file cannot be open.") }
      myFile.close();

      // check timestep size
      double dtIN;
      dtIN = time[10]-time[9];
      std::cout <<"sample size of input file is Dt="<<dtIN<<std::endl;

      // add zeros when the signal is finished until Nsamples is reached
      if(Nsamples - current.size() > 0)
      {
        double t0 = time.back();
        size_t j = 1;
        
        for(size_t k=current.size(); k<Nsamples; ++k)
        {
          idx.push_back(k);
          time.push_back(t0 + j*dtIN);
          current.push_back(0.0);
          ++j;
        }
      }
      
      SAFE_EXIT( current.size() != Nsamples , "In analysis.inl: size of container not equal to Nsamples. ")

      tfboost::TimeDigitizeSignal( current, time, dT, max, engine, false);

      DEBUG(time[0])
      DEBUG(time.back())
      DEBUG(time.size())

      TString newInput;
      newInput = InputFile_current;
      newInput = newInput.ReplaceAll(file_ext,"_new"+file_ext);

      tfboost::SaveConvToFile(current, time, dT, newInput.Data() );


      std::cout <<""<<std::endl;
      std::cout <<"======================================================================================="<<std::endl;
      std::cout <<""<<std::endl;
      std::cout << "processed file is saved in: " << newInput.Data() << std::endl;
      std::cout <<""<<std::endl;
      std::cout <<"======================================================================================="<<std::endl;
      std::cout <<""<<std::endl;

  } //if singlefile

  if(!SingleFile){
  
    /* ----------------------------------------------
    * Preparing the list of input files and
    * related variables for the main loop
    * --------------------------------------------*/
    TList* listoffiles = tfboost::GetFileList(InputDirectory); 
    //listoffiles->Sort();
    TIter nextfile( listoffiles );
    TSystemFile *currentfile;
    TString currentfilename;
    TString line;
     
    TString dir;
    dir =  InputDirectory + "/Resampled";
        mkdir( dir.Data() ,S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IROTH);

    size_t INDEX = 0;

    while( (currentfile = (TSystemFile*) nextfile() ) && INDEX < Nfiles ){
    
        auto start = std::chrono::high_resolution_clock::now();

        currentfilename = currentfile->GetName();

        if ( currentfile->IsDirectory() || !currentfilename.EndsWith(file_ext) ) continue;
        
        RULE_LINE;
        std::cout << "| "<< _START_INFO_ <<"FILE"<< _END_INFO_ << "  : "<<currentfilename<<" " << "\n";
        std::cout << "| "<< _START_INFO_ <<"INDEX"<< _END_INFO_ << " : "<<INDEX<<" "           << "\n";
        RULE_LINE_LIGHT;
        
        ++INDEX;
        
        //Declare and prepare the containers
        HostSignal_t time;     time.reserve(Nsamples);
        HostSignal_t current;  current.reserve(Nsamples);
        
        std::ifstream myFile( (InputDirectory+'/'+ currentfilename).Data() );
        SAFE_EXIT( !myFile.is_open() , "In analysis.inl: input file cannot be open. ")
        
        //Read file N times to skip lines
        for(size_t j=0; j < Nlinestoskip; ++j){line.ReadLine(myFile);}
      
        // Actual loop on file lines
          if (myFile.is_open()) 
          {
            for(size_t j = 0; j < Nsamples ; ++j)
            {
              line.ReadLine(myFile);
              if (!myFile.good()) break;
              
              TObjArray *tokens = line.Tokenize( token );
              
              TString data_str  = ((TObjString*) tokens->At( columnI ) )->GetString();
              TString time_str  = ((TObjString*) tokens->At( columnT ) )->GetString();
                
              const double data = atof(data_str);
              const double tm   = atof(time_str);
                
              idx.push_back(j);
              time.push_back(tm);
              current.push_back(data);

              //std::cout<<tm<<" "<<data<<std::endl;
                
              tokens->Delete();
              delete tokens;
            }
          } else { SAFE_EXIT(true, "Input file cannot be open.") }
          myFile.close();

          // check timestep size
          double dtIN;
          dtIN = time[10]-time[9];
          std::cout <<"sample size of input file is Dt="<<dtIN<<std::endl;

          // add zeros when the signal is finished until Nsamples is reached
          if(Nsamples - current.size() > 0)
          {
            double t0 = time.back();
            size_t j = 1;
            
            for(size_t k=current.size(); k<Nsamples; ++k)
            {
              idx.push_back(k);
              time.push_back(t0 + j*dtIN);
              current.push_back(0.0);
              ++j;
            }
          }
          
          SAFE_EXIT( current.size() != Nsamples , "In resampling.inl: size of container not equal to Nsamples. ")

          tfboost::TimeDigitizeSignal( current, time, dT, max, engine, false);

          DEBUG(time[0])
          DEBUG(time.back())
          DEBUG(time.size())

          tfboost::SaveConvToFile(current, time, dT, InputDirectory + "/Resampled/" + currentfilename.Data() );
        
    } 

  std::cout <<""<<std::endl;
  std::cout <<"======================================================================================="<<std::endl;
  std::cout <<""<<std::endl;
  std::cout << "processed files are saved in folder 'Resampled' inside the InputFolder directory" << std::endl;
  std::cout <<""<<std::endl;
  std::cout <<"======================================================================================="<<std::endl;
  std::cout <<""<<std::endl;

  } //if files in folder

 
  return 0;
}

#endif






