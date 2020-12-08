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

#ifndef TFBOOST_TYPES_H_
#define TFBOOST_TYPES_H_


using HostSignal_t   = hydra::host::vector<double>;
using DevSignal_t    = hydra::device::vector<double>;

using doublePair_t   = std::tuple<double,double>;
using doubleTriple_t = std::tuple<double,double,double>;

using MeasuresEnum_t = enum MeasuresEnum { _toa_le,
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
                                           _tot,
                                           _num_of_measures};
    
    
using MeasuresKeys_t  = std::array< MeasuresEnum_t , _num_of_measures>;
using MeasuresNames_t = std::array< TString , _num_of_measures>;
using Measures_t      = std::array< double, _num_of_measures>;
using Histograms_t    = std::array< TH1D*, _num_of_measures>;



#endif /* TFBOOST_TYPES_H_ */
