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
 
#ifndef TFBOOST_DIGITIZER_H_
#define TFBOOST_DIGITIZER_H_

#include <tfboost/Types.h>
#include <tfboost/Utils.h>


namespace tfboost { 

namespace detail { 

    /*
     * Re-sample a signal (spline) with a specified dT
     * and a random phase clock.
     * Fill the data and time containers
     */
    template<typename Iterable, typename SPLINE, typename RNG>
    inline void DoTimeDigitization(Iterable& data, 
                               Iterable& time, 
                               SPLINE const& signal, 
                               double const& dT, 
                               double const& Tmax, 
                               RNG& rng, 
                               bool const& rndmphase=true){
        
        SAFE_EXIT( !data.empty() || !time.empty(), "In DigitizeSignal: the containers must be empty.")

        hydra_thrust::uniform_real_distribution<double> uniDist(0.0, 1.0);

        double offset = rndmphase? uniDist(rng) * dT : 0 ;
        
        size_t Nsamples = Tmax/dT;
        
        data.reserve(Nsamples);
        time.reserve(Nsamples);

        data.push_back( 0.0 ); 
        time.push_back( 0.0 );
        
        for(size_t i=1; i<Nsamples; ++i){
            double t = offset + i*dT;
            data.push_back( signal(t) ); 
            time.push_back( t ); }
    }

}




/*
 * Re-sample a signal (vector) with a specified dT
 * and a random phase clock.
 * Resize the original containers properly
 */
template<typename Iterable, typename RNG>
inline void TimeDigitizeSignal(Iterable& data, 
                           Iterable& time, 
                            double const& dT, 
                            double const& Tmax, 
                            RNG& rng, 
                            bool const& rndmphase=false){

    HostSignal_t conv_dig;  
    HostSignal_t time_dig; 

    //time and data are the not digitized ones
    auto conv_spline = hydra::make_spiline<double>(time, data );

    tfboost::detail::DoTimeDigitization(conv_dig, time_dig, conv_spline, dT, Tmax, rng, rndmphase);
                                

    // override time and conv_data containers
    data.resize(conv_dig.size());
    time.resize(time_dig.size());
    hydra::copy(conv_dig , data);
    hydra::copy(time_dig , time);

}




/*
 * Get the ADC digitized value given
 * an input value
 */
inline double GetADCvalue(double const& value, 
                          double const& min, 
                          double const& max, 
                          int const& nbits){
  
  const double n     = std::pow(2,nbits);
  const double step  = (max-min)/n;
  return step*std::trunc(value/step);

}



/*
 * Apply an ADC digitization to a signal 
 */
template<typename Iterable>
inline void VoltageDigitizeSignal(Iterable& data, 
                                  double const& ADCmin, 
                                  double const& ADCmax, 
                                  int const& ADCnbits){

    for(auto& x : data) x = tfboost::GetADCvalue(x, ADCmin, ADCmax, ADCnbits);

}




} //namespace tfboost

#endif



