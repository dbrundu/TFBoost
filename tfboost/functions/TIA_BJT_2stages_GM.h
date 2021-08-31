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


#ifndef TIA_BJT_2stages_GM_H_
#define TIA_BJT_2stages_GM_H_


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
 *  @class TIA_BJT_2stages_GM
 *  Funtor that provides the transfer function of a TIA (BJT 2 stages)
 *
 */
template< typename ArgTypeTime, typename Signature = double(ArgTypeTime)  >
class TIA_BJT_2stages_GM: public hydra::BaseFunctor< TIA_BJT_2stages_GM<ArgTypeTime>, Signature, 11>
{

    using ThisBaseFunctor = hydra::BaseFunctor< TIA_BJT_2stages_GM<ArgTypeTime>, Signature, 11 >;
    using ThisBaseFunctor::_par;
    using Param = hydra::Parameter;

public:

    constexpr static int IDX  = 2;

    TIA_BJT_2stages_GM() : 
        ThisBaseFunctor({300, 3000, 25, 0.8, 4000, 25, 0.7, 1.5e-12, 1.5e-12, 1.8e-12, 60e-15}),  fdT(1e-12)
    {}


    TIA_BJT_2stages_GM( libconfig::Setting const& cfg ):
        ThisBaseFunctor({(double) cfg["Rp"], 
                         (double) cfg["Rf"], 
                         (double) cfg["Rl"], 
                         (double) cfg["gm1"], 
                         (double) cfg["Rf2"], 
                         (double) cfg["Rl2"], 
                         (double) cfg["gm2"], 
                         (double) cfg["ci"], 
                         (double) cfg["co"], 
                         (double) cfg["co2"], 
                         (double) cfg["cf"]}), 
        fdT((double) cfg["dT"])
    {}


    TIA_BJT_2stages_GM( Param const& Rp,  Param const& Rf,  Param const& Rl,  Param const& gm1, 
                        Param const& Rf2, Param const& Rl2, Param const& gm2,
                        Param const& ci,  Param const& co,  Param const& co2, Param const& cf, 
                        double const& dT):
        ThisBaseFunctor({Rp, Rf, Rl, gm1, Rf2, Rl2, gm2, ci, co, co2, cf}), fdT(dT)
    {}
    
    

    __hydra_dual__
    TIA_BJT_2stages_GM(TIA_BJT_2stages_GM<ArgTypeTime> const& other):
        ThisBaseFunctor(other), 
        fdT(other.GetdT() )
    {}


    __hydra_dual__
    TIA_BJT_2stages_GM& operator=( TIA_BJT_2stages_GM<ArgTypeTime> const& other){
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
        const double Rf2 = _par[4];
        const double Rl2 = _par[5];
        const double gm2 = _par[6];
        const double ci  = _par[7];
        const double co  = _par[8];
        const double co2 = _par[9];
        const double cf  = _par[10];
        
        const double Ro2=(Rf*Rl2)/(Rf+Rl2);
        const double xi=ci*co+co*cf+cf*ci;
        const double tau=sqrt((Rf*Rl*Rp*xi)/(Rp*(1+gm1*Rl)+Rl+Rf));
        
        //const double G2=gm2*Ro2;
        const double G2=4.2;
        const double tau2nd=Ro2*(cf+co2);
        const double Rm=(Rp*gm1*Rl*Rf-Rp)/(Rf+Rl+Rp*(1+gm1*Rl));

        return fdT * G2*Rm*(x*exp(-x/tau)/(tau*(tau-tau2nd))-tau2nd*exp(-x/tau)/((tau-tau2nd)*(tau-tau2nd))+tau2nd*exp(-x/tau2nd)/((tau-tau2nd)*(tau-tau2nd)));
       


    }
    
    
private:
    

    double fdT;
};


}  // namespace tfboost


#endif /* TIA_BJT_2stages_GM_H_ */
