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
 *  Created on: 22/11/2020
 *      Author: Davide Brundu
 */
 
#ifndef TFBOOST_INITIALIZER_H_
#define TFBOOST_INITIALIZER_H_

#include <tfboost/Types.h>


namespace tfboost { 



/*
 * The struct MeasuresInitializer provides some
 * static methods to access the static constexpr
 * arrays of keys (enums of the measurments)
 * and the array of measurments names 
 * (used to retrieve the configuration)
 *
 */
struct MeasuresInitializer {
    
    static constexpr Measures_t  _values_init  = { 0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0. };
    
    static constexpr MeasuresKeys_t  _keys     = { _toa_le, 
                                                   _toa_cfd, 
                                                   _toa_rm,
                                                   _tpeak, 
                                                   _vpeak, 
                                                   _dvdt_le, 
                                                   _dvdt_cfd, 
                                                   _dvdt_rm, 
                                                   _vonth_le, 
                                                   _vonth_cfd, 
                                                   _vonth_rm, 
                                                   _tot};
    
    static const MeasuresKeys_t  get_keys()    { return _keys; } 


    static const Measures_t      get_values()  { return _values_init; } 
          
          
          
    static const MeasuresNames_t get_names()   { 
        
        static const MeasuresNames_t _names    = { "TOA_LE", 
                                                   "TOA_CFD", 
                                                   "TOA_RM",  
                                                   "Tpeak", 
                                                   "Vpeak", 
                                                   "dvdt_LE",  
                                                   "dvdt_CFD", 
                                                   "dvdt_RM", 
                                                   "VonThLE", 
                                                   "VonThCFD", 
                                                   "VonThRM", 
                                                   "TOT"};
        return _names; 
    } 
  
  
    static const MeasuresNames_t get_names_noise() {
        MeasuresNames_t a = get_names();
        for(auto& x : a) x+"_noise";
        return a;
    } 
    
};



} //namespace tfboost

#endif



