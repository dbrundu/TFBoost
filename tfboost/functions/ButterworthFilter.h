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
 *  Created on: 20/10/2020
 *      Author: Davide Brundu, Gianmatteo Cossu
 *  
 *  Re-implemented using the LRDPRDX implementation:
 *      https://github.com/LRDPRDX/ButterworthRooFit
 *
 */


#ifndef BUTTERWORTHFILTER_H_
#define BUTTERWORTHFILTER_H_


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
 *  @class ButterworthFilter
 *  Funtor that provides the transfer function of a n-order Butterworth Filter
 *
 */
template< typename ArgTypeTime, typename Signature = double(ArgTypeTime)  >
class ButterworthFilter: public hydra::BaseFunctor< ButterworthFilter<ArgTypeTime>, Signature, 2>
{

    using ThisBaseFunctor = hydra::BaseFunctor< ButterworthFilter<ArgTypeTime>, Signature, 2 >;
    using ThisBaseFunctor::_par;
    using Param = hydra::Parameter;

public:


    ButterworthFilter() = delete;
    
    
    ButterworthFilter( Param const& fc,  Param const& n, double dT):
    ThisBaseFunctor({fc,  n}), fdT(dT)
    {}
    
   
    __hydra_dual__
    ButterworthFilter(ButterworthFilter<ArgTypeTime> const& other):
        ThisBaseFunctor(other),
        fdT(other.GetdT() )
    {}


    __hydra_dual__
    ButterworthFilter& operator=( ButterworthFilter<ArgTypeTime> const& other){
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

        if( t < 0.0 ) return 0.0;

        return fdT * H(t).real() ;
    }
    
    
    
    
private:

    double fdT;

    __hydra_dual__
    inline hydra::complex<double>  Sk( size_t k ) const
    {

        const size_t n   = _par[1];
        
        const double arg = M_PI * ( 2 * k + n - 1 ) / ( 2 * n ); 

        return hydra::complex<double>( ::cos(arg), ::sin(arg) );
    }


    __hydra_dual__
    inline hydra::complex<double> J( size_t k ) const
    {

        hydra::complex<double> res(1.0 , 0);
        
        const size_t n   = _par[1];
        
        for( size_t m = 1; m <= n; ++m )
        {
            if( m == k ) continue;

            res /= (Sk( k ) - Sk( m ));
        }
        
        return res;
    }

    __hydra_dual__
    inline hydra::complex<double> H( double x ) const
    {

        hydra::complex<double> res(0., 0.); 
        const size_t n   = _par[1];

        if( x >= 0. )
        {
            for( size_t k = 1; k <= n; ++k )
            {
                hydra::complex<double> a = Sk(k); 
                a *= 2*M_PI*_par[0]*x;
                
                res += J(k) * hydra::exp(a) ;
            }
        }

        return 2*M_PI*_par[0]*res;
    }

};





}  // namespace tfboost


#endif /* BUTTERWORTHFILTER_H_ */












