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


#ifndef TIA_MOS_H_
#define TIA_MOS_H_


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
 *  @class TIA_MOS
 *  Funtor that provides the transfer function of a TIA (MOS)
 *
 */
template< typename ArgTypeTime, typename Signature = double(ArgTypeTime)  >
class TIA_MOS: public hydra::BaseFunctor< TIA_MOS<ArgTypeTime>, Signature, 6>
{

    using ThisBaseFunctor = hydra::BaseFunctor< TIA_MOS<ArgTypeTime>, Signature, 6 >;
    using ThisBaseFunctor::_par;
    using Param = hydra::Parameter;

public:

    constexpr static int IDX  = 0;

    TIA_MOS() : 
        ThisBaseFunctor({3000000,  570000,  55e-6, 105e-15, 21e-15, 5e-15}), fdT(1e-12)
    {}


    TIA_MOS( libconfig::Setting const& cfg):
        ThisBaseFunctor({(double) cfg["Rf"],  
                         (double) cfg["Rl"],  
                         (double) cfg["gm"], 
                         (double) cfg["ci"], 
                         (double) cfg["co"], 
                         (double) cfg["cf"]}), 
        fdT((double) cfg["dT"])
    {}
    
    
    
    TIA_MOS( Param const& Rf,  Param const& Rl,  Param const& gm, 
             Param const& ci,  Param const& co, Param const& cf, 
             double const& dT):
        ThisBaseFunctor({Rf,  Rl,  gm, ci, co, cf}), fdT(dT)
    {}
    


    __hydra_dual__
    TIA_MOS(TIA_MOS<ArgTypeTime> const& other):
        ThisBaseFunctor(other), 
        fdT(other.GetdT() )
    {}


    __hydra_dual__
    TIA_MOS& operator=( TIA_MOS<ArgTypeTime> const& other){
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
            
        double Rf = _par[0]; //3000000;
        double Rl = _par[1]; //570000;
        double gm = _par[2]; //55e-6;
        double ci = _par[3]; //105e-15;    
        double co = _par[4]; //21e-15;
        double cf = _par[5]; //5e-15;
        
        double xi = ci*co+co*cf+cf*ci;
        double Ro = (Rf*Rl)/(Rf+Rl);
        double G0 = gm*Ro-(Ro/Rf);
        double tau = ::sqrt( (Rf*Ro*xi)/(1+G0) );
        double zeta_mos=0.5*((Rf*(ci+cf*(1+G0))+Ro*co)/(::sqrt((1+G0)*Ro*Rf*xi)));
        double tauz=Ro*cf/G0;    

        //return /* (1./2.87184e-06) */  (Rf*G0/(1+G0))*exp(-x/tau)*(tau*x-tau*tauz+x*tauz)/(tau*tau*tau);
        
        if (zeta_mos>1.05){ // overdumped solution
            double tau1=tau/(zeta_mos+::sqrt(zeta_mos*zeta_mos-1));
            double tau2=tau/(zeta_mos-::sqrt(zeta_mos*zeta_mos-1));
            return fdT*(Rf*G0/(1+G0))*((exp(-x/tau2)*(tau2+tauz)/(tau2*(tau2-tau1)))-(exp(-x/tau1)*(tau1+tauz)/(tau1*(tau2-tau1))));
        } 
        else if (zeta_mos>0.95 && zeta_mos<=1.05){ //critically dumped solution
            return fdT*(Rf*G0/(1+G0))*exp(-x/tau)*(tau*x-tau*tauz+x*tauz)/(tau*tau*tau);
        } 
        else if (zeta_mos>0. && zeta_mos<=0.95){  //underdumped solution
			double a=zeta_mos;
			double b=::sqrt(1-a*a);
            double expaxtau = ::exp(-a*x/tau);
            double s_bxtau = ::sin(b*x/tau);
            double c_bxtau = ::cos(b*x/tau);
            double tau2 = tau*tau;

			double first=((a*a*a*tauz)/(tau2*b))*expaxtau*s_bxtau;
			double second=((a*a*tauz)/(tau2))*expaxtau*c_bxtau;
		    double third=((a*a)/(tau*b))*expaxtau*s_bxtau;
			double forth=((b*b*tauz)/(tau2))*expaxtau*c_bxtau;
			double fifth=((a*b*tauz)/(tau2))*expaxtau*s_bxtau;
			double sixth=((b)/(tau))*expaxtau*s_bxtau;

            return fdT*(Rf*G0/(1+G0))*(first-second+third-forth+fifth+sixth);

        } else { return 0.0; }
        
    }
    
    
private:
    
    double fdT;
};





}  // namespace tfboost


#endif /* TIA_MOS_H */
