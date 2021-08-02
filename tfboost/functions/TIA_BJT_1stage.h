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
 *      Author: Davide Brundu, Gianmatteo Cossu
 */


#ifndef TIA_BJT_1stage_H_
#define TIA_BJT_1stage_H_


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
 *  @class TIA_BJT_1stage
 *  Funtor that provides the transfer function of a TIA (MOS)
 *
 */
template< typename ArgTypeTime, typename Signature = double(ArgTypeTime)  >
class TIA_BJT_1stage: public hydra::BaseFunctor< TIA_BJT_1stage<ArgTypeTime>, Signature, 7>
{

    using ThisBaseFunctor = hydra::BaseFunctor< TIA_BJT_1stage<ArgTypeTime>, Signature, 7 >;
    using ThisBaseFunctor::_par;
    using Param = hydra::Parameter;

public:

    constexpr static int IDX  = 3;

    TIA_BJT_1stage() : 
        ThisBaseFunctor({400, 4000, 35, 0.615, 2e-12, 4e-12, 60e-15}), fdT(1e-12)
    {}
    
    
    TIA_BJT_1stage( libconfig::Setting const& cfg):
    ThisBaseFunctor({(double) cfg["Rp"], 
                     (double) cfg["Rf"],  
                     (double) cfg["Rl"],  
                     (double) cfg["gm"], 
                     (double) cfg["ci"], 
                     (double) cfg["co"], 
                     (double) cfg["cf"]}), 
    fdT((double) cfg["dT"])
    {}
    
    
    TIA_BJT_1stage( Param const& Rp,    Param const& Rf,  Param const& Rl,  Param const& gm, 
                    Param const& ci,    Param const& co,  Param const& cf,
                    double const& dT):
    ThisBaseFunctor({Rp, Rf,  Rl,  gm, ci, co, cf}), fdT(dT)
    {}
    
    

    __hydra_dual__
    TIA_BJT_1stage(TIA_BJT_1stage<ArgTypeTime> const& other):
        ThisBaseFunctor(other), 
        fdT(other.GetdT() )
    {}


    __hydra_dual__
    TIA_BJT_1stage& operator=( TIA_BJT_1stage<ArgTypeTime> const& other){
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
        
        const double Rp = _par[0];
        const double Rf = _par[1];
        const double Rl = _par[2];
        const double gm = _par[3];
        const double ci = _par[4];
        const double co = _par[5];
        const double cf = _par[6];

        
        const double xi=ci*co+co*cf+cf*ci;
        const double tau=sqrt((Rf*Rl*Rp*xi)/(Rp*(1+gm*Rl)+Rl+Rf));
        const double zeta_bjt=0.5*((Rp*ci*(Rf+Rl)+Rf*cf*(Rp*(1+gm*Rl)+Rl)+Rl*co*(Rf+Rp))/(sqrt((Rp*(1+gm*Rl)+Rl+Rf)*Rf*Rl*Rp*xi)));
        const double Rm=(Rp*gm*Rl*Rf-Rp)/(Rf+Rl+Rp*(1+gm*Rl));

        return fdT * (Rm*x*exp(-x/tau))/(tau*tau);
        
    }
    
    
    
private:
    

    double fdT;

};




}  // namespace tfboost


#endif /* TIA_BJT_1stage_H_ */
