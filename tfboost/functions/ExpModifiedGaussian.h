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
 
#ifndef EXPMODIFIEDGAUSSIAN_H_
#define EXPMODIFIEDGAUSSIAN_H_
 
    
#include <tfboost/Utils.h>

    
namespace tfboost {

namespace detail {

    inline double expmodifiedgauss(double* x, double* par) {
   
      const static  double Sqrt2 = 1.414213562373095; //\sqrt{2}
      const static  double Sqrtpi = 1.7724538509055160;


      double x0     = x[0];
      double norm   = par[0];
      double mu     = par[1];
      double sigma  = par[2];
      double tau    = par[3];
      double frac   = par[4];
      double sigma2 = sigma*par[5];   

      /*
      double a = norm*sigma*Sqrtpi/(tau*Sqrt2);
      double b = ::exp(0.5*(sigma/tau)*(sigma/tau) - (x0 - mu)/tau );
      double c = ::erfc( (1./Sqrt2) * (sigma/tau - (x0 - mu)/sigma) ); 

      return a*b*c;
      */
      double a = 1./(2.*tau)*::exp((2.*mu + sigma2*sigma2/tau - 2.*x0)/(2.*tau));
      double b = ::erfc((mu+sigma2*sigma2/tau -x0)/(Sqrt2*sigma2));
      double c = 1./Sqrt2*Sqrtpi*sigma * ::exp(-(x0-mu)*(x0-mu)/(2.*sigma*sigma));

      return norm*( (1-frac)*a*b + frac*c) ;

      
   }
}


struct ExpModifiedGaussian {


    ExpModifiedGaussian(){ initialized = false; }


    ExpModifiedGaussian(double xmin, double xmax) {
    
        fun = new TF1("fun", detail::expmodifiedgauss, xmin, xmax, 6 );
        fun->SetParNames ("Norm","Mean_value","Sigma", "Tau", "Frac", "Scale");
        initialized=true;
    
    }
    
    
    inline void SetParameters(double norm, double mu, double sigma, double tau, double frac, double scale){
       fun->SetParameters(norm, mu, sigma, tau, frac, scale);
    }


    inline void SetParLimits(int n, double min, double max){
       fun->SetParLimits(n, min, max);
    }
   
   
   ~ExpModifiedGaussian(){ 
      if(initialized) delete fun;
   }
   
   
   TF1* fun;
   
   private:
   
   bool initialized;


};



   
   
}


#endif
