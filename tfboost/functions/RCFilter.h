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
 *  Created on: 30/08/2021
 *      Author: Gian Matteo Cossu
 *      
 *
 */


#ifndef RCFILTER_H_
#define RCFILTER_H_


#include <cmath>
#include <tuple>
#include <limits>
#include <stdexcept>
#include <assert.h>
#include <utility>
#include <ratio>


#include <hydra/detail/Config.h>
#include <hydra/detail/BackendPolicy.h>
#include <hydra/Types.h>
#include <hydra/Function.h>
#include <hydra/Pdf.h>
#include <hydra/Integrator.h>
#include <hydra/Tuple.h>
#include <hydra/functions/Utils.h>
#include <hydra/functions/Math.h>


namespace tfboost {


/*
 *  @class RCFilter
 *  Funtor that provides the transfer function of a n-order RC Filter
 *
 */
template< typename ArgTypeTime, typename Signature = double(ArgTypeTime)  >
class RCFilter: public hydra::BaseFunctor< RCFilter<ArgTypeTime>, Signature, 2>
{

    using ThisBaseFunctor = hydra::BaseFunctor< RCFilter<ArgTypeTime>, Signature, 2 >;
    using ThisBaseFunctor::_par;
    using Param = hydra::Parameter;

public:


    RCFilter() = delete;
    
    
    RCFilter( Param const& fc,  Param const& n, double dT):
    ThisBaseFunctor({fc,  n}), fdT(dT)
    {}
    
   
    __hydra_dual__
    RCFilter(RCFilter<ArgTypeTime> const& other):
        ThisBaseFunctor(other),
        fdT(other.GetdT() )
    {}


    __hydra_dual__
    RCFilter& operator=( RCFilter<ArgTypeTime> const& other){
        if(this == &other) return *this;
        ThisBaseFunctor::operator=(other);
        fdT = other.GetdT();
        return *this;
    }
    


    __hydra_dual__
    inline double GetdT() const { return fdT; }

    
    __hydra_dual__
    inline void SetdT(double dT) { fdT = dT; }
    
 
    __hydra_dual__ inline
    double Evaluate( ArgTypeTime t )  const  {

        double fc = _par[0];
        int n =_par[1];

        if( t < 0.0 ) return 0.0;

        double tau= 1/(M_PI*2*fc);        

        return fdT * exp(-t/tau)*exp((n-1)*log((t/tau))) / (tau * tgamma(n)) ;
    }

    private:

    double fdT;
    

};





}  // namespace tfboost


#endif /* RCFILTER_H_ */












