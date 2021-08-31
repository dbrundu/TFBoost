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
 *  Created on: 01/06/2020
 *      Author: Davide Brundu, Gian Matteo Cossu
 */


#ifndef TIA_BJT_2stages_H_
#define TIA_BJT_2stages_H_


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
 *  @class TIA_BJT_2stages
 *  Funtor that provides the transfer function of a TIA (BJT 2 stages)
 *
 */
template< typename ArgTypeTime, typename Signature = double(ArgTypeTime)  >
class TIA_BJT_2stages: public hydra::BaseFunctor< TIA_BJT_2stages<ArgTypeTime>, Signature, 9>
{

    using ThisBaseFunctor = hydra::BaseFunctor< TIA_BJT_2stages<ArgTypeTime>, Signature, 9 >;
    using ThisBaseFunctor::_par;
    using Param = hydra::Parameter;

public:

    constexpr static int IDX  = 1;

    TIA_BJT_2stages() : 
        ThisBaseFunctor({400, 3000, 25, 0.615, 3000, 25, 0.8, 2e-12, 4e-12}), fdT(1e-12)
    {}



    TIA_BJT_2stages( libconfig::Setting const& cfg ):
        ThisBaseFunctor({(double) cfg["Rp"], 
                         (double) cfg["Rf"], 
                         (double) cfg["Rl"], 
                         (double) cfg["gm1"], 
                         (double) cfg["ci"], 
                         (double) cfg["cf"], 
                         (double) cfg["co"], 
                         (double) cfg["Gain"], 
                         (double) cfg["tau2"]}),
        fdT((double) cfg["dT"])
    {}


    TIA_BJT_2stages( Param const& Rp,  Param const& Rf,  Param const& Rl,  Param const& gm1, 
                     Param const& ci,  Param const& cf, Param const& co, Param const& Gain,Param const& tau2,
                     double const& dT):
    ThisBaseFunctor({Rp, Rf, Rl, gm1, ci, cf, co, Gain, tau2}), fdT(dT)
    {}
    
    

    __hydra_dual__
    TIA_BJT_2stages(TIA_BJT_2stages<ArgTypeTime> const& other):
        ThisBaseFunctor(other), 
        fdT(other.GetdT() )
    {}


    __hydra_dual__
    TIA_BJT_2stages& operator=( TIA_BJT_2stages<ArgTypeTime> const& other){
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
            
        const double Rp  = _par[0];
        const double Rf  = _par[1];
        const double Rl  = _par[2];
        const double gm1 = _par[3];
        const double ci = _par[4];
        const double cf = _par[5];
        const double co = _par[6];
        const double Gain  = _par[7];
        const double tau2  = _par[8];
        
        const double xi=ci*co+co*cf+cf*ci;
        const double tau=::sqrt((Rf*Rl*Rp*xi)/(Rp*(1+gm1*Rl)+Rl+Rf));
        
        //const double G2=gm2*Ro2;
        const double G2=Gain;
        const double tau2nd=tau2;
        const double Rm=(Rp*gm1*Rl*Rf-Rp)/(Rf+Rl+Rp*(1+gm1*Rl));

        return fdT * G2*Rm*(x*exp(-x/tau)/(tau*(tau-tau2nd))-tau2nd*exp(-x/tau)/((tau-tau2nd)*(tau-tau2nd))+tau2nd*exp(-x/tau2nd)/((tau-tau2nd)*(tau-tau2nd)));
        

    }
    
    
private:
    

    double fdT;
};




}  // namespace tfboost


#endif /* TIA_BJT_2stages_H_ */
