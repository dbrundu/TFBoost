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
 *  Created on: 01/06/2020
 *      Author: Davide Brundu, Gianmatteo Cossu
 */


#ifndef TIA_IDEALINT_H_
#define TIA_IDEALINT_H_


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
 *  @class TIA_IdealInt
 *  Funtor that provides the transfer function of a TIA (MOS)
 *
 */
template< typename ArgTypeTime, typename Signature = double(ArgTypeTime)  >
class TIA_IdealInt: public hydra::BaseFunctor< TIA_IdealInt<ArgTypeTime>, Signature, 4>
{

    using ThisBaseFunctor = hydra::BaseFunctor< TIA_IdealInt<ArgTypeTime>, Signature, 4 >;
    using ThisBaseFunctor::_par;
    using Param = hydra::Parameter;

public:

    constexpr static int IDX  = 4;


    TIA_IdealInt() : 
        ThisBaseFunctor({110e3,  7.74e-9,  18.81e-9, 12.66e-9}), fdT(1e-12)
    {}

    
    
    TIA_IdealInt(libconfig::Setting const& cfg) : 
        ThisBaseFunctor({(double) cfg["z0"], 
                         (double) cfg["tau1"], 
                         (double) cfg["tau2"],
                         (double) cfg["tauz"]}), 
        fdT( (double) cfg["dT"] )
    {}
    

    
    
    TIA_IdealInt( Param const& z0,  Param const& tau1,  Param const& tau2, 
                  Param const& tauz, double const& dT):
    ThisBaseFunctor({z0,  tau1,  tau2, tauz}), fdT(dT)
    {}
    
    



    __hydra_dual__
    TIA_IdealInt(TIA_IdealInt<ArgTypeTime> const& other):
        ThisBaseFunctor(other), 
        fdT(other.GetdT() )
    {}


    __hydra_dual__
    TIA_IdealInt& operator=( TIA_IdealInt<ArgTypeTime> const& other){
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
    double Evaluate( ArgTypeTime x )  const  {

        if( x < 0.0 ) return 0.0;
        
        double z0   = _par[0];
        double tau1 = _par[1];
        double tau2 = _par[2];
        double tauz = _par[3];
        
        double a = ( tau2-tauz ) / (tau2*(tau2-tau1)) ;
        a *= exp(-x/tau2);
        
        double b = ( tauz-tau1 ) / (tau1*(tau2-tau1)) ;
        b *= exp(-x/tau1);
        
        return fdT * z0 * (a+b);
        
    }
    
    
private:
    

    double fdT;

};


/**
 * Convenience function to get the proper kernel
 * based on the funtor index
 */
template<int I, typename T, typename dummy = typename std::enable_if<I==TIA_IdealInt<T>::IDX, void>::type>
TIA_IdealInt<T> get_kernel(){ return TIA_IdealInt<T>(); }



}  // namespace tfboost


#endif /* TIA_IDEALINT_H_ */












