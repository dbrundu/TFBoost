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

      return norm*( (1-frac)*a*b + frac*c) + par[6] ;

      
   }
}


struct ExpModifiedGaussian {


    ExpModifiedGaussian(){ initialized = false; }


    ExpModifiedGaussian(double xmin, double xmax) {
    
        fun = new TF1("fun", detail::expmodifiedgauss, xmin, xmax, 7 );
        fun->SetParNames ("Norm","Mean","Sigma", "Tau", "Frac", "Scale", "Constant");
        initialized=true;
    
    }
    
    
    inline void SetParameters(double norm, double mu, double sigma, double tau, double frac, double scale, double cst){
       fun->SetParameters(norm, mu, sigma, tau, frac, scale, cst);
    }


    inline void SetParLimits(int n, double min, double max){
       fun->SetParLimits(n, min, max);
    }
    
    
    inline void SetParFromHist(TH1D* hist){
        
        double mu    = hist->GetBinCenter( hist->GetMaximumBin() );
        double norm  = hist->Integral("width");
        double sigma = 0.2*hist->GetStdDev();
        double tau   = sigma;
        double cst   = hist->GetBinCenter( hist->GetNbinsX() - int(0.9*hist->GetNbinsX()) );
        fun->SetParameters(norm, mu, sigma, tau, 0.5, 1.0, cst);
        fun->SetParLimits(0, 0.3*norm, 3*norm);
        fun->SetParLimits(1, 0.3*mu, 2*mu);
        fun->SetParLimits(2, 0.3*sigma, 3*sigma);
        fun->SetParLimits(3, 0.3*tau,   3*tau);
        fun->SetParLimits(4, 0., 1.);
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
