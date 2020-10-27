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
 
#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H
    
#include <tfboost/Utils.h>

    
namespace tfboost { 

struct ConfigParser
{
    ConfigParser( libconfig::Setting const& cfg_root )
    {

        InputDirectory        = (const char*) cfg_root["InputDirectory"];
        OutputDirectory       = (const char*) cfg_root["OutputDirectory"];
        tf_inputfile          = (const char*) cfg_root["TFFile"];
        TransferFunction      = (const char*) cfg_root["TransferFunction"];
        InputFileExtension    = (const char*) cfg_root["InputFileExtension"];
        conv_inputfile        = (const char*) cfg_root["ConvolutionFile"]; 
        SingleFile            = (const char*) cfg_root["FileName"];
        token                 = (const char*) cfg_root["token"];
        
        column                       = (int)  cfg_root["column"];
        offset                       = (int)  cfg_root["offset"]; 
        NlinesToSkip                 = (int)  cfg_root["NlinesToSkip"];

        LandauFluctuation            = (bool) cfg_root["LandauFluctuation"];
        MakeConvolution              = (bool) cfg_root["MakeConvolution"];
        SaveSinglePlotConvolution    = (bool) cfg_root["SaveSinglePlotConvolution"];
        SaveConvDataToFile           = (bool) cfg_root["SaveConvDataToFile"];
        MakeLinearFitNearThreshold   = (bool) cfg_root["MakeLinearFitNearThreshold"];
        MakeGaussianFitNearVmax      = (bool) cfg_root["MakeGaussianFitNearVmax"];
        AddNoise                     = (bool) cfg_root["AddNoise"];
        UseRedNoise                  = (bool) cfg_root["UseRedNoise"];
        MakeTheoreticalTOA           = (bool) cfg_root["MakeTheoreticalTOA"];
        UseSameCurve                 = (bool) cfg_root["UseSameCurve"];
        IdxConvtoSave                = (int)  cfg_root["IdxConvtoSave"];
        Nfiles                       = (int)  cfg_root["MaxInputFiles"];
        MakeDigitization             = (bool) cfg_root["MakeDigitization"];
        randomphase                  = (bool) cfg_root["randomphase"];
        TimeReferenceResolution      = (bool) cfg_root["TimeReferenceResolution"];

        LE_reject_nonoise  = (double) cfg_root["LE_reject_nonoise"];
        LE_reject_noise    = (double) cfg_root["LE_reject_noise"]; 

        const libconfig::Setting& cfg_tf  = cfg_root[TransferFunction];

        Nsamples     = (int)    cfg_tf["Nsamples"];
        Nsamples     = (int)    tfboost::upper_power_of_two((size_t) Nsamples);
      
        bound_fit    = (int)    cfg_tf["bound_fit"];
        ID           = (int)    cfg_tf["ID"];
        Nbins        = (int)    cfg_tf["Nbins"];
        dT           = (double) cfg_tf["dT"];
        LEthr        = (double) cfg_tf["LEthr"];
        CFD_fr       = (double) cfg_tf["CFD_fr"];
        sigma_noise  = (double) cfg_tf["sigma_noise"];
        r_rednoise   = (double) cfg_tf["r_rednoise"];
        sampling_dT  = (double) cfg_tf["sampling_dT"];
        minplot      = (double) cfg_tf["minplot"];
        maxplot      = (double) cfg_tf["maxplot"]; 
        RM_delay     = (double) cfg_tf["RM_delay"]; 
        
        if(!InputDirectory.EndsWith("/")) InputDirectory   = InputDirectory+"/";
        if(!OutputDirectory.EndsWith("/")) OutputDirectory = OutputDirectory+"/";
        
        Nsamples_0 = Nsamples;
        dT_0 = dT;
        sampling_dT_0 = sampling_dT;
    }
    
    
    inline void ResetInitialValues(){
        Nsamples = Nsamples_0;
        dT = dT_0;
        sampling_dT = sampling_dT_0;
    }


    TString InputDirectory, OutputDirectory;
    TString tf_inputfile, TransferFunction, InputFileExtension, conv_inputfile, SingleFile, token;

    bool LandauFluctuation, MakeConvolution, SaveSinglePlotConvolution;
    bool SaveConvDataToFile, MakeLinearFitNearThreshold, MakeGaussianFitNearVmax, TimeReferenceResolution;
    bool AddNoise, UseRedNoise, MakeTheoreticalTOA, UseSameCurve, MakeDigitization, randomphase;
    
    int column, ID, Nbins;
    
    size_t offset, NlinesToSkip, IdxConvtoSave, Nfiles, Nsamples, bound_fit;
    
    double dT, LEthr, CFD_fr, sigma_noise, r_rednoise, sampling_dT, LE_reject_nonoise, LE_reject_noise, RM_delay;
    double minplot, maxplot;
    
    
    
    private: // for initial configurations
    
    size_t Nsamples_0;
    double dT_0, sampling_dT_0;


};




struct HistConfigParser
{
    HistConfigParser( libconfig::Setting const& cfg_root )
    {

    const libconfig::Setting& cfg_tf  = cfg_root["HistogramsPars"];

    TOALE_min = (double) cfg_tf["TOALE_min"];
    TOALE_max = (double) cfg_tf["TOALE_max"];
    TOALE_Nbins = (int)  cfg_tf["TOALE_Nbins"];
    
    TOACFD_min = (double) cfg_tf["TOACFD_min"];
    TOACFD_max = (double) cfg_tf["TOACFD_max"];
    TOACFD_Nbins = (int)  cfg_tf["TOACFD_Nbins"];
    
    TOARM_min = (double) cfg_tf["TOARM_min"];
    TOARM_max = (double) cfg_tf["TOARM_max"];
    TOARM_Nbins = (int)  cfg_tf["TOARM_Nbins"];
    
    TOACFDnoise_min = (double) cfg_tf["TOACFDnoise_min"];
    TOACFDnoise_max = (double) cfg_tf["TOACFDnoise_max"];
    TOACFDnoise_Nbins = (int)  cfg_tf["TOACFDnoise_Nbins"];
    

    TOALEnoise_min = (double) cfg_tf["TOALEnoise_min"];
    TOALEnoise_max = (double) cfg_tf["TOALEnoise_max"];
    TOALEnoise_Nbins = (int)  cfg_tf["TOALEnoise_Nbins"];
    

    TOARMnoise_min = (double) cfg_tf["TOARMnoise_min"];
    TOARMnoise_max = (double) cfg_tf["TOARMnoise_max"];
    TOARMnoise_Nbins = (int)  cfg_tf["TOARMnoise_Nbins"];
    

    Vmax_min = (double) cfg_tf["Vmax_min"];
    Vmax_max = (double) cfg_tf["Vmax_max"];
    Vmax_Nbins = (int)  cfg_tf["Vmax_Nbins"];
    

    TimeatVmax_min = (double) cfg_tf["TimeatVmax_min"];
    TimeatVmax_max = (double) cfg_tf["TimeatVmax_max"];
    TimeatVmax_Nbins = (int)  cfg_tf["TimeatVmax_Nbins"];
    

    Vmax_noise_min = (double) cfg_tf["Vmax_noise_min"];
    Vmax_noise_max = (double) cfg_tf["Vmax_noise_max"];
    Vmax_noise_Nbins = (int)  cfg_tf["Vmax_noise_Nbins"];
    

    Vth_CFD_min = (double) cfg_tf["Vth_CFD_min"];
    Vth_CFD_max = (double) cfg_tf["Vth_CFD_max"];
    Vth_CFD_Nbins = (int)    cfg_tf["Vth_CFD_Nbins"];
    

    Vth_LE_min = (double) cfg_tf["Vth_LE_min"];
    Vth_LE_max = (double) cfg_tf["Vth_LE_max"];
    Vth_LE_Nbins = (int)    cfg_tf["Vth_LE_Nbins"];
    

    Vth_RM_min = (double) cfg_tf["Vth_RM_min"];
    Vth_RM_max = (double) cfg_tf["Vth_RM_max"];
    Vth_RM_Nbins = (int)    cfg_tf["Vth_RM_Nbins"];
    

    Vth_CFD_noise_min = (double) cfg_tf["Vth_CFD_noise_min"];
    Vth_CFD_noise_max = (double) cfg_tf["Vth_CFD_noise_max"];
    Vth_CFD_noise_Nbins = (int)    cfg_tf["Vth_CFD_noise_Nbins"];
    

    Vth_RM_noise_min = (double) cfg_tf["Vth_RM_noise_min"];
    Vth_RM_noise_max = (double) cfg_tf["Vth_RM_noise_max"];
    Vth_RM_noise_Nbins = (int)    cfg_tf["Vth_RM_noise_Nbins"];
    

    Vth_LE_noise_min = (double) cfg_tf["Vth_LE_noise_max"];
    Vth_LE_noise_max = (double) cfg_tf["Vth_LE_noise_max"];
    Vth_LE_noise_Nbins = (int)    cfg_tf["Vth_LE_noise_Nbins"];
    

    dVdt_LE_min = (double) cfg_tf["dVdt_LE_min"];
    dVdt_LE_max = (double) cfg_tf["dVdt_LE_max"];
    dVdt_LE_Nbins = (int)    cfg_tf["dVdt_LE_Nbins"];
    

    dVdt_CFD_min = (double) cfg_tf["dVdt_CFD_min"];
    dVdt_CFD_max = (double) cfg_tf["dVdt_CFD_max"];
    dVdt_CFD_Nbins = (int)    cfg_tf["dVdt_CFD_Nbins"];
    

    dVdt_RM_min = (double) cfg_tf["dVdt_RM_min"];
    dVdt_RM_max = (double) cfg_tf["dVdt_RM_max"];
    dVdt_RM_Nbins = (int)    cfg_tf["dVdt_RM_Nbins"];
    

    dVdt_CFD_noise_min = (double) cfg_tf["dVdt_CFD_noise_min"];
    dVdt_CFD_noise_max = (double) cfg_tf["dVdt_CFD_noise_max"];
    dVdt_CFD_noise_Nbins = (int)    cfg_tf["dVdt_CFD_noise_Nbins"];
    

    dVdt_LE_noise_min = (double) cfg_tf["dVdt_LE_noise_min"];
    dVdt_LE_noise_max = (double) cfg_tf["dVdt_LE_noise_max"];
    dVdt_LE_noise_Nbins = (int)    cfg_tf["dVdt_LE_noise_Nbins"];
 
    VthRMoverVmax_min = (double) cfg_tf["VthRMoverVmax_min"];
    VthRMoverVmax_max = (double) cfg_tf["VthRMoverVmax_max"];   
    VthRMoverVmax_Nbins = (int) cfg_tf["VthRMoverVmax_Nbins"];

        


    }
    



    double TOALE_min,
        TOALE_max,
        TOACFD_min,
        TOACFD_max,
        TOARM_min,
        TOARM_max,
        TOACFDnoise_min,
        TOACFDnoise_max,
        TOALEnoise_min,
        TOALEnoise_max,
        TOARMnoise_min,
        TOARMnoise_max,
        Vmax_min,
        Vmax_max,
        TimeatVmax_min,
        TimeatVmax_max,
        Vmax_noise_min,
        Vmax_noise_max,
        Vth_CFD_min,
        Vth_CFD_max,
        Vth_LE_min,
        Vth_LE_max,
        Vth_RM_min,
        Vth_RM_max,
        Vth_CFD_noise_min,
        Vth_CFD_noise_max,
        Vth_RM_noise_min,
        Vth_RM_noise_max,
        Vth_LE_noise_min,
        Vth_LE_noise_max,
        dVdt_LE_min,
        dVdt_LE_max,
        dVdt_CFD_min,
        dVdt_CFD_max,
        dVdt_RM_min,
        dVdt_RM_max,
        dVdt_CFD_noise_min,
        dVdt_CFD_noise_max,
        dVdt_LE_noise_min,
        dVdt_LE_noise_max,
        VthRMoverVmax_min,
        VthRMoverVmax_max;

    int  TOALE_Nbins, TOACFD_Nbins, TOARM_Nbins, TOACFDnoise_Nbins, 
        TOALEnoise_Nbins,
        TOARMnoise_Nbins,
        Vmax_Nbins,
        TimeatVmax_Nbins,
        Vmax_noise_Nbins,
        Vth_CFD_Nbins,
        Vth_LE_Nbins,
        Vth_RM_Nbins,
        Vth_CFD_noise_Nbins,
        Vth_RM_noise_Nbins,
        Vth_LE_noise_Nbins,
        dVdt_LE_Nbins,
        dVdt_CFD_Nbins,
        dVdt_RM_Nbins,
        dVdt_CFD_noise_Nbins,
        dVdt_LE_noise_Nbins,
        VthRMoverVmax_Nbins;


};



} //namespace tfboost 
    
    
#endif
