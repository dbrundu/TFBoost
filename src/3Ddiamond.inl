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
 *  Created on: 19/08/2021
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
#include <cmath>

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

//hydra
#include <hydra/FFTW.h>
#include <hydra/device/System.h>
#include <hydra/Algorithm.h>
#include <hydra/Random.h>
#include <hydra/Zip.h>
#include <hydra/Complex.h>
#include <hydra/functions/UniformShape.h>

#include <hydra/RandomFill.h>
//command line
#include <tclap/CmdLine.h>


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

double Rmz (double x, double tau_np);

int main(int argv, char** argc)
{

  tfboost::TFBoostHeader();
  
  std::cout.precision(10);
  
 
  /* ----------------------------------------------
   * Get Configuration
   * --------------------------------------------*/
 
  libconf::Config Cfg;
  Cfg.readFile("../etc/3Ddiamond.cfg");
  const libconf::Setting& cfg_root  = Cfg.getRoot();
  
  const bool Convolution         = (bool) cfg_root["Convolution"];
  const TString InputDirectory     = (const char*) cfg_root["InputDirectory"]; 
  const TString OutputDirectory    = (const char*) cfg_root["OutputDirectory"]; 

  const double dT                  = (double)  cfg_root["dT"];
  const size_t Nsamples            = (int)  cfg_root["Nsamples"];

  const double Rn                  = (double)  cfg_root["Rn"];
  const double Rp                  = (double)  cfg_root["Rp"];
  const double Cd                  = (double)  cfg_root["Cd"];

  const size_t Ndz                 = (int)  cfg_root["Ndz"];

  const bool Zin                   = (bool) cfg_root["Zin"];
  const TString ZinFile            = (const char*) cfg_root["ZinFile"]; 

  const bool ArbTracks             = (bool) cfg_root["ArbTracks"];
  const size_t NTracks             = (int)  cfg_root["NTracks"];

  const TString Radice             = (const char*) cfg_root["Radice"];
  const TString SumDirectory       = (const char*) cfg_root["SumDirectory"];
  
  const char*  token         = (const char*) cfg_root["token"];
  const int    columnT       = (int)  cfg_root["columnT"];
  const int    columnI       = (int)  cfg_root["columnI"];
  const int    columnIe       = (int)  cfg_root["columnIe"];
  const int    columnIh       = (int)  cfg_root["columnIh"];

  const size_t Nlinestoskip  = (int)  cfg_root["Nlinestoskip"];


  const double SampRate    = 1./dT;
  const double min         = 0.0;
  const double max         = (Nsamples-1) * dT;
  const double min_kernel  = -0.5*(max-min);
  const double max_kernel  =  0.5*(max-min);


  tfboost::CreateDirectories( "3Ddiamond");
  tfboost::CreateDirectories( "3Ddiamond/H3D_TF");
  tfboost::CreateDirectories( "3Ddiamond/H3D_Currents");
  tfboost::CreateDirectories( "3Ddiamond/H3D_Currents/data_arbitrary");
  tfboost::CreateDirectories( "3Ddiamond/H3D_Currents/data_arbitrary/Currents");
  tfboost::CreateDirectories( "3Ddiamond/H3D_Currents/data_NOT_arbitrary");

  bool CurrentSum= false;

  hydra::SeedRNG S{};
  hydra::default_random_engine engine( S() ); 
  TRandom3 root_rng( S() );

  /* ----------------------------------------------
  * ELECTRODES TRANSFER FUNCTION SECTION WITH Zin
  * --------------------------------------------*/
  if(Zin){


  HostSignal_t ztime;      ztime.reserve(Nsamples);
  HostSignal_t zin;        zin.reserve(Nsamples);
   
  std::ifstream myFileZin( ZinFile.Data() );
  SAFE_EXIT( !myFileZin.is_open() , "Zin file cannot be open. ")
  TString zline;

/*   std::ofstream zin_o,zin_o2;
  zin_o.open("zin_o.txt");
  zin_o2.open("zin_o2.txt"); */
  

  while(!myFileZin.eof()){

    zline.ReadLine(myFileZin);
    if (!myFileZin.good()) break;
        
    TObjArray *tokens = zline.Tokenize( token );
    
    TString timez_str  = ((TObjString*) tokens->At( columnT ) )->GetString();
    TString zin_str    = ((TObjString*) tokens->At( columnI ) )->GetString();
    
    double data0 = atof(timez_str);
    ztime.push_back(data0);

    double data1 = atof(zin_str);
    zin.push_back(data1);

    //zin_o<<data0<<" "<<data1<<std::endl;

  }

  myFileZin.close();

  tfboost::TimeDigitizeSignal( zin, ztime, dT, max, engine, false);  
  hydra::device::vector<double> Vzin( Nsamples);
  ztime[Nsamples-1]=dT*(Nsamples);
  zin[Nsamples-1]=zin[Nsamples-2];
  
   int offset=0;

  for(int i=0; i<Nsamples; i++){
   ztime[i]=i*dT;
   if(i<offset){Vzin[i]=0;}
   else Vzin[i]=zin[i-offset];
  } 
  
  auto fft_r2c = hydra::RealToComplexFFTW<double>( Nsamples);
  fft_r2c.LoadInputData(Vzin);
  fft_r2c.Execute();

  auto r2c_out =  fft_r2c.GetOutputData();
/* 
  std::ofstream zin_f;
  zin_f.open("zin_f.txt"); */

  int dz=1;
  

  //loop on z to calculate Hn(z) and Hp(z)
  for (int j=0; j<Ndz; j++){

  hydra::device::vector<double> freq( Nsamples);
  hydra::device::vector<hydra::complex<double>> Hn(Nsamples/2+1);
  hydra::device::vector<hydra::complex<double>> Hp(Nsamples/2+1);
  

    for(int i=0; i<Nsamples/2+1; i++){   
    freq[i]=i/(Nsamples*dT);

    hydra::complex<double> C1;
    
    C1 = r2c_out.first[i]; 

    double RN= Rn*(dz-0.5)/Ndz;  

    double RP= Rp*(Ndz+0.5-dz)/Ndz;    
    
    double ren=0,imn=0,detn=0,rep=0,imp=0,detp=0;

    detn= (1-2*M_PI*freq[i]*Cd*C1.imag()*dT)*(1-2*M_PI*freq[i]*Cd*C1.imag()*dT) + (2*M_PI*freq[i]*Cd*(RN+C1.real()*dT))*(2*M_PI*freq[i]*Cd*(RN+C1.real()*dT));

    ren = (1-2*M_PI*freq[i]*Cd*C1.imag()*dT)/detn;
    
    imn = (-2*M_PI*freq[i]*Cd*(RN+C1.real()*dT))/detn;

    detp= (1-2*M_PI*freq[i]*Cd*C1.imag()*dT)*(1-2*M_PI*freq[i]*Cd*C1.imag()*dT) + (2*M_PI*freq[i]*Cd*(RP+C1.real()*dT))*(2*M_PI*freq[i]*Cd*(RP+C1.real()*dT));

    rep = (1-2*M_PI*freq[i]*Cd*C1.imag()*dT)/detp;
    
    imp = (-2*M_PI*freq[i]*Cd*(RP+C1.real()*dT))/detp;

    Hn[i]=(ren,imn);
    Hp[i]=(rep,imp);

  /*   std::cout<<i<<" "<<  C1.real()*dT<< " "<< C1.imag()*dT  << std::endl;
    zin_f<< freq[i] <<" "<<  re << " "<< im << std::endl; */

    }

    
    auto fft_N = hydra::ComplexToComplexFFTW<double>( Nsamples/2+1);
    fft_N.LoadInputData(Hn);
    fft_N.Execute();

    auto fft_P = hydra::ComplexToComplexFFTW<double>( Nsamples/2+1);
    fft_P.LoadInputData(Hp);
    fft_P.Execute();

    std::ofstream zin_n,zin_p;
    TString Hn_name = OutputDirectory + "/H3D_TF/Hnz";
    TString Hp_name = OutputDirectory + "/H3D_TF/Hpz";

    Hn_name += std::to_string(dz) + ".txt";
    Hp_name += std::to_string(dz) + ".txt";
    zin_n.open(Hn_name);
    zin_p.open(Hp_name);

    auto Hn_out =  fft_N.GetOutputData();
    auto Hp_out =  fft_P.GetOutputData();

    HostSignal_t ztime2;      ztime2.reserve(Nsamples);
    HostSignal_t Hn2;            Hn2.reserve(Nsamples);
    HostSignal_t Hp2;            Hp2.reserve(Nsamples);

    for(int i=0; i<Nsamples; i++){  
    
    if(i<Nsamples/4+1){
    hydra::complex<double>C1,C2;
    C1 = Hn_out.first[i];
    C2 = Hp_out.first[i];

    Hn2[i]=-C1.imag()/Nsamples;
    Hp2[i]=-C2.imag()/Nsamples;
    ztime2[i]=2*ztime[i]; // because of FFT timestep is doubled

    }

    if(i>=Nsamples/4+1){
    Hn2[i]=0.;
    Hp2[i]=0.;
    ztime2[i]=2*ztime[i];
    }

    }
    
    //insert delay in transfer function to help spline
    int del=500;
    for(int i=Nsamples; i>0; i--){
    if(i>=del){
    Hn2[i]=Hn2[i-del];
    Hp2[i]=Hp2[i-del];
    }
    if(i<del){
    Hn2[i]=0.;
    Hp2[i]=0.;
    }
    }

    Hn2[0]=0.;
    Hp2[0]=0.;

    HostSignal_t Hn3;            Hn3.reserve(Nsamples);
    HostSignal_t Hp3;            Hp3.reserve(Nsamples);

    // do linear interpolation to come back to the right timestep inserting values between samples
    for(int i=0; i<Nsamples/2; i++){  
    Hn3[2*i]=Hn2[i];
    Hn3[2*i+1]=0.5*(Hn2[i+1]+ Hn2[i]);  

    Hp3[2*i]=Hp2[i];
    Hp3[2*i+1]=0.5*(Hp2[i+1]+ Hp2[i]);   
    }

    for(int i=0; i<Nsamples; i++){ 
     zin_n<<ztime[i]<<" "<< Hn3[i] <<std::endl;
     zin_p<<ztime[i]<<" "<< Hp3[i] <<std::endl;   
    }    

    std::cout<<" "<<std:: endl;
    std::cout<<"-------------------------------------------"<<std:: endl;
    std::cout<<" TRANSFER FUNCTION Hn(z="<< dz << ") and Hp(z="<< dz <<  ") DONE !  "  <<std:: endl;
    std::cout<<"-------------------------------------------"<<std:: endl;
    std::cout<<" "<<std:: endl;

      //increment z
    dz++;


  }// loop on Ndz

}//end if zin


  /* ----------------------------------------------
   * if tracks are parallel to the columns and 
   * just save the transfer functions
   * --------------------------------------------*/
 if(!ArbTracks){
  hydra::host::vector<double>  Time;
  hydra::host::vector<double>  Rm_n;
  hydra::host::vector<double>  Rm_p;
    
  Time.reserve(Nsamples);
  Rm_n.reserve(Nsamples);
  Rm_p.reserve(Nsamples);

 
  for (int j=1; j<= Ndz; j++){

    double taun = Rn/Ndz * Cd * j  ;
    double taup = Rp/Ndz * Cd * j  ;

    for (int i=0; i<Nsamples; i++){

    double curr_time = i*dT;

    Rm_n[i] += Rmz (curr_time, taun);
    Rm_p[i] += Rmz (curr_time, taup);

    }

  }
  
  if(Rn==Rp){
  // final array of total transimpendance
  hydra::host::vector<double>  RM;
  RM.reserve(Nsamples);

  std::ofstream outdata( OutputDirectory + "/H3D_TF/3Ddiamond.txt" ); 

  for(int i=0; i< Nsamples; i++){
    
    RM[i]   = Rm_n[i]*dT;
    Time[i] = i*dT;

    if (i<10){outdata << Time[i] << " " << 0 <<  std::endl;}

    else outdata << Time[i] << " " << RM[i-10]/Ndz <<  std::endl;

  }

  outdata.close();
      
  std::cout <<""<<std::endl;
  std::cout <<"======================================================================================="<<std::endl;
  std::cout <<""<<std::endl;
  std::cout << "Transfer function for tracks parallel to columns:" << std::endl;
  std::cout << "file '3Ddiamond.txt' saved in 'H3D_TF' folder" << std::endl;
  std::cout <<""<<std::endl;
  std::cout <<"======================================================================================="<<std::endl;
  std::cout <<""<<std::endl;

  }

  if(Rn!=Rp){
  
  std::ofstream outdata1( OutputDirectory + "/H3D_TF/3Ddiamond_Rn.txt" ); 
  std::ofstream outdata2( OutputDirectory + "/H3D_TF/3Ddiamond_Rp.txt" ); 

  hydra::host::vector<double>  RMn;
  RMn.reserve(Nsamples);

  hydra::host::vector<double>  RMp;
  RMp.reserve(Nsamples);

  for(int i=0; i< Nsamples; i++){
    
    RMn[i]   = Rm_n[i]*dT;
    RMp[i]   = Rm_p[i]*dT;
    Time[i] = i*dT;

    if (i<10){
      outdata1 << Time[i] << " " << 0 <<  std::endl;
      outdata2 << Time[i] << " " << 0 <<  std::endl;      
      }

    else {
      outdata1 << Time[i] << " " << RMn[i-10]/Ndz <<  std::endl;
      outdata2 << Time[i] << " " << RMp[i-10]/Ndz <<  std::endl;
      }

  }

  outdata1.close();
  outdata2.close();
      
  std::cout <<""<<std::endl;
  std::cout <<"======================================================================================="<<std::endl;
  std::cout <<""<<std::endl;
  std::cout << "Transfer function for tracks parallel to columns and different Resistances:" << std::endl;
  std::cout << "files '3Ddiamond_Rn.txt' and '3Ddiamond_Rp.txt' saved in 'H3D_TF' folder" << std::endl;
  std::cout <<""<<std::endl;
  std::cout <<"======================================================================================="<<std::endl;
  std::cout <<""<<std::endl;

  }

  }

  /* ----------------------------------------------
   *  CONVOLUTION SECTION
   * --------------------------------------------*/

 if(Convolution){


  std::string currentfilename;
  std::string radice=Radice.Data();
  std::string inputdirectory =InputDirectory.Data();
  std::string path;
  TString line;

  //load transfer functions
  HostSignal_t time_tfn;
  HostSignal_t current_tfn;
  HostSignal_t time_tfp;
  HostSignal_t current_tfp;

  auto kernel1 = hydra::make_spiline<double>(time_tfn, current_tfn );
  auto kernel2 = hydra::make_spiline<double>(time_tfn, current_tfn );

  size_t INDEX = 0;  


    if(ArbTracks){

    for(int nz=1; nz<=Ndz; nz++){

    TString tf_infile1 = OutputDirectory + "/H3D_TF/Hnz";
    TString tf_infile2 = OutputDirectory + "/H3D_TF/Hpz";

    tf_infile1 += std::to_string(nz) + ".txt";
    tf_infile2 += std::to_string(nz) + ".txt";

    int Nskip = 0;

    std::cout<<"read Trasfer functions Hnz path="<< tf_infile1.Data() << std::endl;     
    tfboost::ReadTF( tf_infile1, Nskip, time_tfn, current_tfn, /*scaling*/1.0, /*double range?*/true);
    auto kernel1 = hydra::make_spiline<double>(time_tfn, current_tfn );

    std::cout<<"read Trasfer functions Hpz path="<< tf_infile2.Data() << std::endl;
    tfboost::ReadTF( tf_infile2, Nskip, time_tfp, current_tfp, /*scaling*/1.0, /*double range?*/true);
    auto kernel2 = hydra::make_spiline<double>(time_tfp, current_tfp );

    for(int f=1; f<=NTracks; f++){

      if(nz<10){
        currentfilename = radice + std::to_string(f) + "_z0" + std::to_string(nz) + ".txt";
      }

      else if(nz>=10){
        currentfilename = radice + std::to_string(f) + "_z" + std::to_string(nz) + ".txt";
      }



      RULE_LINE;
    std::cout << "| "<< _START_INFO_ <<"FILE"<< _END_INFO_ << "  : "<<currentfilename<<" " << "\n";
    std::cout << "| "<< _START_INFO_ <<"INDEX"<< _END_INFO_ << " : "<<INDEX<<" "           << "\n";
    RULE_LINE_LIGHT;
    
    ++INDEX;

    path = inputdirectory + "/" + currentfilename;

  
    //Declare and prepare the containers
    HostSignal_t time;     time.reserve(Nsamples);
    HostSignal_t current;  current.reserve(Nsamples);
    HostSignal_t current_e;  current_e.reserve(Nsamples);
    HostSignal_t current_h;  current_h.reserve(Nsamples);

    std::ifstream myFile(path);
    SAFE_EXIT( !myFile.is_open() , "In analysis.inl: input file cannot be open. ")
    
    //Read file N times to skip lines
    for(size_t j=0; j < Nlinestoskip; ++j) 
        line.ReadLine(myFile);

    size_t s = 0;
    // Actual loop on file lines, fill all the containers
    for(size_t j = 0; j < Nsamples ; ++j)
    {
      line.ReadLine(myFile);
      if (!myFile.good()) break;
      
      TObjArray *tokens = line.Tokenize( token );
      
      TString time_str  = ((TObjString*) tokens->At( columnT ) )->GetString();
      TString curr_str  = ((TObjString*) tokens->At( columnI ) )->GetString();
      
      time.push_back( s * dT);

      double data = atof(curr_str);
      current.push_back(data);

      if(ArbTracks || Rn!=Rp){
      TString curr_e_str  = ((TObjString*) tokens->At( columnIe ) )->GetString();
      TString curr_h_str  = ((TObjString*) tokens->At( columnIh ) )->GetString();

      double data_e = atof(curr_e_str);
      double data_h = atof(curr_h_str);
      current_e.push_back(data_e);
      current_h.push_back(data_h);

      }

      tokens->Delete();
      delete tokens;

      s++;

    }
  
    myFile.close();


    //continue to fill containers until Nsamples size
    for(size_t k=current.size(); k<Nsamples; ++k) {
      time.push_back( k * dT);
      current.push_back(0.0); 

      if(ArbTracks || Rn!=Rp){
      current_e.push_back(0.0);
      current_h.push_back(0.0);
      }     
      
      }

    SAFE_EXIT( current.size() != Nsamples , "In analysis.inl: size of container not equal to Nsamples. ")

        #if HYDRA_DEVICE_SYSTEM!=CUDA
    auto fft_backend = hydra::fft::fftw_f64;
    #endif
    
    #if HYDRA_DEVICE_SYSTEM==CUDA
    auto fft_backend = hydra::fft::cufft_f64;
    #endif

    std::cout<< "CONVOLUTION WITH H(Z="<< nz <<")"<< std::endl;
   
     HostSignal_t conv_data_n(Nsamples);
     HostSignal_t conv_data_p(Nsamples);

     auto signal_e = hydra::make_spiline<double>(time, current_e );
     auto signal_h = hydra::make_spiline<double>(time, current_h ); 
     
   
    //convolution
     tfboost::Do_Convolution(fft_backend, kernel1, signal_e, conv_data_n, min, max, Nsamples);
     tfboost::Do_Convolution(fft_backend, kernel2, signal_h, conv_data_p, min, max, Nsamples);

     //check scale factor
     
     double Qint1e=0, Qint2e=0;
     double Qint1h=0, Qint2h=0;

     for (int i=0; i<Nsamples; i++){      
     Qint1e+= current_e [i];
     Qint2e+= conv_data_n[i];
     Qint1h+= current_h [i];
     Qint2h+= conv_data_p[i];
     }

     double ke=Qint1e/Qint2e;
     double kh=Qint1h/Qint2h;

     for (int i=0; i<Nsamples; i++){      
     conv_data_n[i]=ke*conv_data_n[i];
     conv_data_p[i]=kh*conv_data_p[i];
     }

     std::ofstream Curr;
     TString Curr_path =SumDirectory + "/" + currentfilename ;
     Curr.open(Curr_path);
    
     //add the 2 currents and save the file
     for(int i=0; i<Nsamples; i++){
     Curr<< time[i] <<" "<< conv_data_n[i] + conv_data_p[i]  << std:: endl;
     }  


    }// end of Tracks loop
    CurrentSum = true;

    }//end of nz loop


    }//end of if(ArbTracks)

  

    if(!ArbTracks){

    TString tf_infile1 = OutputDirectory + "/H3D_TF/";
    TString tf_infile2 = OutputDirectory + "/H3D_TF/";
    int Nskip = 0;
    
    //preload transfer function
    if(Rn==Rp){
    tf_infile1 += "3Ddiamond.txt";
    std::cout<<"Trasfer functions H(s) path="<< tf_infile1.Data() << std::endl;     
    tfboost::ReadTF( tf_infile1, Nskip, time_tfn, current_tfn, /*scaling*/1.0, /*double range?*/true);
    auto kernel1 = hydra::make_spiline<double>(time_tfn, current_tfn );
    }
    
    //preload transfer functions
    if(Rn!=Rp){
    tf_infile1 += "3Ddiamond_Rn.txt";
    tf_infile2 += "3Ddiamond_Rp.txt";

    std::cout<<"Trasfer functions H(s)_Rn path="<< tf_infile1.Data() << std::endl;     
    tfboost::ReadTF( tf_infile1, Nskip, time_tfn, current_tfn, /*scaling*/1.0, /*double range?*/true);
    auto kernel1 = hydra::make_spiline<double>(time_tfn, current_tfn );

    std::cout<<"Trasfer functions H(s)_Rp path="<< tf_infile2.Data() << std::endl;
    tfboost::ReadTF( tf_infile2, Nskip, time_tfp, current_tfp, /*scaling*/1.0, /*double range?*/true);
    auto kernel2 = hydra::make_spiline<double>(time_tfp, current_tfp );
    }
          
      /* ----------------------------------------------
      * Main loop on input files
      * --------------------------------------------*/
      for(int f=1; f<=NTracks; f++){ 

      currentfilename = radice + std::to_string(f) + ".txt";

      RULE_LINE;
      std::cout << "| "<< _START_INFO_ <<"FILE"<< _END_INFO_ << "  : "<<currentfilename<<" " << "\n";
      std::cout << "| "<< _START_INFO_ <<"INDEX"<< _END_INFO_ << " : "<<INDEX<<" "           << "\n";
      RULE_LINE_LIGHT;

      ++INDEX;

      path = inputdirectory + "/" + currentfilename;

      //Declare and prepare the containers
      HostSignal_t time;     time.reserve(Nsamples);
      HostSignal_t current;  current.reserve(Nsamples);
      HostSignal_t current_e;  current_e.reserve(Nsamples);
      HostSignal_t current_h;  current_h.reserve(Nsamples);

      std::ifstream myFile(path);
      SAFE_EXIT( !myFile.is_open() , "In analysis.inl: input file cannot be open. ")

      //Read file N times to skip lines
      for(size_t j=0; j < Nlinestoskip; ++j) 
          line.ReadLine(myFile);

      size_t s = 0;
      // Actual loop on file lines, fill all the containers
      for(size_t j = 0; j < Nsamples ; ++j)
      {
        line.ReadLine(myFile);
        if (!myFile.good()) break;
        
        TObjArray *tokens = line.Tokenize( token );
        
        TString time_str  = ((TObjString*) tokens->At( columnT ) )->GetString();
        TString curr_str  = ((TObjString*) tokens->At( columnI ) )->GetString();
        
        time.push_back( s * dT);

        double data = atof(curr_str);
        current.push_back(data);

        if(ArbTracks || Rn!=Rp){
        TString curr_e_str  = ((TObjString*) tokens->At( columnIe ) )->GetString();
        TString curr_h_str  = ((TObjString*) tokens->At( columnIh ) )->GetString();

        double data_e = atof(curr_e_str);
        double data_h = atof(curr_h_str);
        current_e.push_back(data_e);
        current_h.push_back(data_h);

        }

        tokens->Delete();
        delete tokens;

        s++;

      }

      myFile.close();


      //continue to fill containers until Nsamples size
      for(size_t k=current.size(); k<Nsamples; ++k) {
        time.push_back( k * dT);
        current.push_back(0.0); 

        if(ArbTracks || Rn!=Rp){
        current_e.push_back(0.0);
        current_h.push_back(0.0);
        }     
        
        }

      SAFE_EXIT( current.size() != Nsamples , "In analysis.inl: size of container not equal to Nsamples. ")

      //convolution section
      #if HYDRA_DEVICE_SYSTEM!=CUDA
      auto fft_backend = hydra::fft::fftw_f64;
      #endif

      #if HYDRA_DEVICE_SYSTEM==CUDA
      auto fft_backend = hydra::fft::cufft_f64;
      #endif

      std::cout<< "CONVOLUTION WITH TRACK="<< f <<""<< std::endl;

  if(Rn!=Rp){

  HostSignal_t conv_data_e(Nsamples);
  HostSignal_t conv_data_h(Nsamples);

  auto signal_e = hydra::make_spiline<double>(time, current_e );
  auto signal_h = hydra::make_spiline<double>(time, current_h ); 

  auto kernel1 = hydra::make_spiline<double>(time_tfn, current_tfn );
  auto kernel2 = hydra::make_spiline<double>(time_tfn, current_tfn );

  tfboost::Do_Convolution(fft_backend, kernel1, signal_e, conv_data_e, min, max, Nsamples);
  tfboost::Do_Convolution(fft_backend, kernel2, signal_h, conv_data_h, min, max, Nsamples);

  HostSignal_t conv_data(Nsamples);

  for(int i=0; i<Nsamples; i++){
    conv_data[i] = conv_data_e[i] + conv_data_h[i];
  }

  tfboost::SaveConvToFile(conv_data, time, dT, 
                            OutputDirectory + "/H3D_Currents/data_NOT_arbitrary/" + currentfilename );
  
  }

  if(Rn==Rp){

  HostSignal_t conv_data(Nsamples);

  auto signal = hydra::make_spiline<double>(time, current);
  auto kernel1 = hydra::make_spiline<double>(time_tfn, current_tfn );

  tfboost::Do_Convolution(fft_backend, kernel1, signal, conv_data, min, max, Nsamples);

  tfboost::SaveConvToFile(conv_data, time, dT, 
                            OutputDirectory + "/H3D_Currents/data_NOT_arbitrary/" + currentfilename );

  }
    
   }//end of loop on file and convolution

  }// end of if Convolution

 }


 if(CurrentSum){


  std::string radice=Radice.Data();
  std::string namefile;
  std::string path;

  for (int i=1; i<=NTracks; i++){

    HostSignal_t time;     time.reserve(Nsamples);
    HostSignal_t current;  current.reserve(Nsamples);
    double Q=0;

    for(int i=0; i<Nsamples; i++){
     current[i]=0;
    }

    for(int nz=1; nz<=Ndz; nz++){

      if(nz<10){
        namefile = radice + std::to_string(i) + "_z0" + std::to_string(nz) + ".txt";
      }

      else if(nz>=10){
        namefile = radice + std::to_string(i) + "_z" + std::to_string(nz) + ".txt";
      }

      path = SumDirectory + "/" + namefile;

      std::ifstream inFile (path);
      double a=0,b=0;

      std::cout <<"PROCESSING TRACK: " << namefile << std::endl;

      for(int j=0; j<Nsamples; j++){

        inFile >> a >> b;     
        time[j]=a;
        current[j]+=b;

      }

      inFile.close();

    }

    // sum currents of same track in a single file    

    path = SumDirectory + "/Currents/" + radice + std::to_string(i) + ".txt";

    std::ofstream outFile( path );
    for(size_t j = 0; j < Nsamples ; ++j)
    {      
      Q+= current[j];
      outFile << time[j] << " " << current[j] << std::endl;   
    }
    
    std::cout <<" "<<std::endl;
    std::cout <<"        current total charge Q= " << Q*dT << std::endl;
    std::cout <<" "<<std::endl;
    outFile.close();



  }




}// end of (CurrentSum)
 
  return 0;
}

double Rmz (double x, double tau_np){

  return exp(-x/tau_np)/tau_np ;

}

#endif






