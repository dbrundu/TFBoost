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
 
#ifndef TFBOOST_CONFIGPARSER_H
#define TFBOOST_CONFIGPARSER_H
    
#include <tfboost/Types.h>
#include <tfboost/Utils.h>
#include <tfboost/Initializer.h>

    
namespace tfboost { 

/*
 * Struct containing variables and methods to 
 * parse the libconfig configuration.
 */
struct ConfigParser
{
    ConfigParser( libconfig::Setting const& cfg_root )
    {

        InputDirectory        = (const char*) cfg_root["InputDirectory"];
        OutputDirectory       = (const char*) cfg_root["OutputDirectory"];
        NoiseDirectory        = (const char*) cfg_root["NoiseDirectory"];
        tf_inputfile          = (const char*) cfg_root["TFFile"];
        TransferFunction      = (const char*) cfg_root["TransferFunction"];
        InputFileExtension    = (const char*) cfg_root["InputFileExtension"];
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
        
        AddSimulatedNoise            = (bool) cfg_root["AddSimulatedNoise"];
        DoMeasurementsWithNoise      = (bool) cfg_root["DoMeasurementsWithNoise"];
        AddNoiseFromFiles            = (bool) cfg_root["AddNoiseFromFiles"];
        
        UseRedNoise                  = (bool) cfg_root["UseRedNoise"];
        MakeTheoreticalTOA           = (bool) cfg_root["MakeTheoreticalTOA"];
        UseSameCurve                 = (bool) cfg_root["UseSameCurve"];
        IdxConvtoSave                = (int)  cfg_root["IdxConvtoSave"];
        Nfiles                       = (int)  cfg_root["MaxInputFiles"];
        MakeDigitization             = (bool) cfg_root["MakeDigitization"];
        randomphase                  = (bool) cfg_root["randomphase"];
        TimeReferenceResolution      = (bool) cfg_root["TimeReferenceResolution"];
        TOTcorrection                = (bool) cfg_root["TOTcorrection"];
        PlotRMfit                    = (bool) cfg_root["PlotRMfit"];
        PlotLinFit                   = (bool) cfg_root["PlotLinFit"];
        PlotGausFit                  = (bool) cfg_root["PlotGausFit"];
        DelayMonitoring              = (int)  cfg_root["DelayMonitoring"];
        TOT_a                      = (double) cfg_root["TOT_a"];
        TOT_b                      = (double) cfg_root["TOT_b"];
        LE_reject_nonoise          = (double) cfg_root["LE_reject_nonoise"];
        LE_reject_noise            = (double) cfg_root["LE_reject_noise"]; 
        landaufactor_mean          = (double) cfg_root["landaufactor_mean"];
        landaufactor_sigma         = (double) cfg_root["landaufactor_sigma"]; 
        timeref_sigma              = (double) cfg_root["timeref_sigma"]; 
        

        const libconfig::Setting& cfg_tf  = cfg_root[TransferFunction];

        Nsamples                   = (int)    cfg_tf["Nsamples"];
        Nsamples                   = (int)    tfboost::upper_power_of_two((size_t) Nsamples);
        bound_fit                  = (int)    cfg_tf["bound_fit"];
        ID                         = (int)    cfg_tf["ID"];
        Nbins                      = (int)    cfg_tf["Nbins"];
        dT                         = (double) cfg_tf["dT"];
        LEthr                      = (double) cfg_tf["LEthr"];
        CFD_fr                     = (double) cfg_tf["CFD_fr"];
        sigma_noise                = (double) cfg_tf["sigma_noise"];
        r_rednoise                 = (double) cfg_tf["r_rednoise"];
        sampling_dT                = (double) cfg_tf["sampling_dT"];
        minplot                    = (double) cfg_tf["minplot"];
        maxplot                    = (double) cfg_tf["maxplot"]; 
        RM_delay                   = (double) cfg_tf["RM_delay"]; 
       
        
        if(!InputDirectory.EndsWith("/")) InputDirectory   = InputDirectory+"/";
        if(!OutputDirectory.EndsWith("/")) OutputDirectory = OutputDirectory+"/";
        
        
        dT_0 = dT;
        Nsamples_0    = Nsamples;
        sampling_dT_0 = sampling_dT;
        
        TF_IDs[0] = "TIA_MOS";
        TF_IDs[1] = "TIA_BJT_2stages";
        TF_IDs[2] = "TIA_BJT_2stages_GM";
        TF_IDs[3] = "TIA_BJT_1stage";
        TF_IDs[4] = "TIA_IdealInt";
        TF_IDs[5] = "FromFile";
        
        CheckTrFunctionID();
        CheckBooleansConsistency();
    }
    
    
    
    inline void ResetInitialValues() const {
        Nsamples = Nsamples_0;
        dT = dT_0;
        sampling_dT = sampling_dT_0;
    }
    
    
    inline void CheckTrFunctionID(){
        auto search = TF_IDs.find(ID);
        SAFE_EXIT( search == TF_IDs.end() , "Wrong Tr.Function configuration ID.")
        SAFE_EXIT( TransferFunction != search->second , "Wrong Tr.Function configuration ID.")
    }
    
    
    inline void CheckBooleansConsistency(){
        SAFE_EXIT( AddSimulatedNoise && AddNoiseFromFiles, "You can't request to simulate noise AND add the noise from file, choose one option.")
    }


    TString InputDirectory, OutputDirectory, NoiseDirectory;
    TString tf_inputfile, TransferFunction, InputFileExtension, conv_inputfile, SingleFile, token;

    bool LandauFluctuation, MakeConvolution, SaveSinglePlotConvolution;
    bool SaveConvDataToFile, MakeLinearFitNearThreshold, MakeGaussianFitNearVmax, TimeReferenceResolution;
    bool AddSimulatedNoise, UseRedNoise, MakeTheoreticalTOA, UseSameCurve, MakeDigitization, randomphase, TOTcorrection;
    bool DoMeasurementsWithNoise, AddNoiseFromFiles;
    bool PlotRMfit, PlotLinFit, PlotGausFit;
    
    int column, ID, Nbins;
    
    size_t offset, NlinesToSkip, IdxConvtoSave, Nfiles, bound_fit, DelayMonitoring;
    
    double LEthr, CFD_fr, sigma_noise, r_rednoise, LE_reject_nonoise, LE_reject_noise, RM_delay;
    double minplot, maxplot, TOT_a, TOT_b;
    double landaufactor_mean, landaufactor_sigma, timeref_sigma;
    
    mutable size_t Nsamples;
    mutable double dT;
    mutable double sampling_dT;
    
    
    
private: // for initial configurations
    
    size_t Nsamples_0;
    double dT_0, sampling_dT_0;
    std::map<int, TString> TF_IDs;


};





struct HistConfig {

    TString name;
    TString title;
    TString xtitle;
    TString ytitle;
    double  min;
    double  max;
    int     Nbins;
    
};


/*
 * Struct containing the histograms
 * array and to configure them,
 * parsing the libconfig configuration.
 */
struct HistConfigParser
{
    using HistConfigs_t = std::array< HistConfig, _num_of_measures>;

    HistConfigParser( libconfig::Setting const& cfg_root )
    {
        const libconfig::Setting& cfg_tf      = cfg_root["HistogramsPars"];
        
        const MeasuresKeys_t  keys            = MeasuresInitializer::get_keys();
        const MeasuresNames_t names           = MeasuresInitializer::get_names();
        
        for(auto const& key : keys){
        
            configs[key].name          = TString("hist") + names[key];
            configs[key].title         = (const char*)  cfg_tf[names[key]]["title"];
            configs[key].xtitle        = (const char*)  cfg_tf[names[key]]["xtitle"];
            configs[key].ytitle        = (const char*)  cfg_tf[names[key]]["ytitle"];
            configs[key].min           = (double)       cfg_tf[names[key]]["min"];
            configs[key].max           = (double)       cfg_tf[names[key]]["max"];
            configs[key].Nbins         = (int)          cfg_tf[names[key]]["Nbins"];
            
            configs_noise[key].name    = TString("hist") + names[key] + TString("_noise");
            configs_noise[key].title   = (const char*)  cfg_tf[names[key]+"_noise"]["title"];
            configs_noise[key].xtitle  = (const char*)  cfg_tf[names[key]+"_noise"]["xtitle"];
            configs_noise[key].ytitle  = (const char*)  cfg_tf[names[key]+"_noise"]["ytitle"];
            configs_noise[key].min     = (double)       cfg_tf[names[key]+"_noise"]["min"];
            configs_noise[key].max     = (double)       cfg_tf[names[key]+"_noise"]["max"];
            configs_noise[key].Nbins   = (int)          cfg_tf[names[key]+"_noise"]["Nbins"];
            
        }
    }
    
HistConfigs_t configs;
HistConfigs_t configs_noise;

};


} //namespace tfboost 
    
    
#endif
