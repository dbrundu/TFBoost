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

        const libconfig::Setting& cfg_tf  = cfg_root[TransferFunction];

        Nsamples     = (int)    cfg_tf["Nsamples"];
        ID           = (int)    cfg_tf["ID"];
        Nbins        = (int)    cfg_tf["Nbins"];
        dT           = (double) cfg_tf["dT"];
        LEthr        = (double) cfg_tf["LEthr"];
        CFD_fr       = (double) cfg_tf["CFD_fr"];
        sigma_noise  = (double) cfg_tf["sigma_noise"];
        r_rednoise   = (double) cfg_tf["r_rednoise"];
        
        if(!InputDirectory.EndsWith("/")) InputDirectory   = InputDirectory+"/";
        if(!OutputDirectory.EndsWith("/")) OutputDirectory = OutputDirectory+"/";
        
    }


    TString InputDirectory, OutputDirectory;
    TString tf_inputfile, TransferFunction, InputFileExtension, conv_inputfile, SingleFile, token;

    bool LandauFluctuation, MakeConvolution, SaveSinglePlotConvolution;
    bool SaveConvDataToFile, MakeLinearFitNearThreshold, MakeGaussianFitNearVmax;
    bool AddNoise, UseRedNoise, MakeTheoreticalTOA, UseSameCurve;
    
    int column, ID, Nbins;
    
    size_t offset, NlinesToSkip, IdxConvtoSave, Nfiles, Nsamples;
    

    double dT, LEthr, CFD_fr, sigma_noise, r_rednoise;


};


} //namespace tfboost 
    
    
#endif
