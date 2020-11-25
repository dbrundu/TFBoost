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
 
#ifndef TFBOOST_LOGGER_H
#define TFBOOST_LOGGER_H
    
#include <tfboost/Utils.h>
#include <tfboost/ConfigParser.h>

    
namespace tfboost { 


class Logger
{

    public:
    
        Logger(Logger const&) = delete;
        
        void operator=(Logger const&) = delete;
        
        static Logger& getInstance(const char* filename) {
            static Logger instance(filename); 
            return instance;
        }
        
        inline void PrintConfig( tfboost::ConfigParser const& cfg);
        
        template<typename T, typename U>
        inline void PrintMessage(T message, U val) { m_file << message << val << "\n";}
        
        inline void Exit() { m_file.close() ;}
        
    
    private:
    
        Logger(const char* filename) { 
            m_file.open(filename); 
            SAFE_EXIT( !m_file.is_open() , "Logger file cannot be open. ") 
        } 
        
        std::ofstream m_file;
        
        inline const char* bool_cast(bool const& b) {
            return b ? "true" : "false";
        }

};





inline void Logger::PrintConfig( tfboost::ConfigParser const& c) { 
    
    m_file << "=== General ==="                 << "\n";
    m_file << "InputDirectory:   "              << c.InputDirectory << "\n";
    m_file << "OutputDirectory:  "              << c.OutputDirectory << "\n";
    m_file << "TransferFunction: "              << c.TransferFunction << "\n";
    m_file << "ConvolutionFile:  "              << c.conv_inputfile << "\n\n";
    
    m_file << "=== Flags ==="                   << "\n";
    m_file << "LandauFluctuation:          "    << bool_cast(c.LandauFluctuation) << "\n";
    m_file << "MakeConvolution:            "    << bool_cast(c.MakeConvolution) << "\n";
    m_file << "SaveConvDataToFile:         "    << bool_cast(c.SaveConvDataToFile) << "\n";
    m_file << "MakeLinearFitNearThreshold: "    << bool_cast(c.MakeLinearFitNearThreshold) << "\n";
    m_file << "MakeGaussianFitNearVmax:    "    << bool_cast(c.MakeGaussianFitNearVmax) << "\n";
    m_file << "AddNoise:                   "    << bool_cast(c.AddNoise) << "\n";
    m_file << "UseRedNoise:                "    << bool_cast(c.UseRedNoise) << "\n";
    m_file << "UseSameCurve:               "    << bool_cast(c.UseSameCurve) << "\n";
    
    if(c.UseSameCurve) 
      m_file << "- File used:                "  << c.SingleFile << "\n";
   
    m_file << "MakeDigitization:           "    << bool_cast(c.MakeDigitization) << "\n";
   
    if(c.MakeDigitization) {
      m_file << " - randomphase:             "  << bool_cast(c.randomphase) << "\n";
      m_file << " - sampling_dT:             "  << c.sampling_dT << "\n"; }
   
    m_file << "TimeReferenceResolution:    "    << bool_cast(c.TimeReferenceResolution) << "\n\n";
    
    m_file << "=== Trigger ==="                 << "\n";
    m_file << "LE_reject_nonoise: "             << c.LE_reject_nonoise << "\n";
    m_file << "LE_reject_noise:   "             << c.LE_reject_noise << "\n\n";
    
    m_file << "=== TF properties ==="           << "\n";
    m_file << "bound_fit:         "             << c.bound_fit << "\n";
    m_file << "dT:                "             << c.dT << "\n";
    m_file << "LEthr:             "             << c.LEthr << "\n";
    m_file << "CFD_fr:            "             << c.CFD_fr << "\n";
    m_file << "sigma_noise:       "             << c.sigma_noise << "\n";
    m_file << "r_rednoise:        "             << c.r_rednoise << "\n\n";

}






} //namespace tfboost 
    
    
#endif
