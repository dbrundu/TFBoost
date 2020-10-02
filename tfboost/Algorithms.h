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
 
#ifndef ALGORITHMS_H_
#define ALGORITHMS_H_

#include <tfboost/Utils.h>


namespace tfboost { 

namespace algo { 


template<typename Iterable>
inline size_t LeadingEdge(Iterable const& vout, double const& Vthr)
{
    size_t Tth = 0;
    
    for (size_t k=0; k<vout.size(); ++k)
    {
        if (Vthr < vout[k]){ Tth = k; break; }
    }

    if(Tth==vout.size() -1 ) return 0;

    return Tth;
}



template<typename Iterable>
inline size_t ConstantFraction(Iterable const& vout, double const& fraction, double const& vmax)
{
    return LeadingEdge(vout , vmax*fraction);
}






template<typename Iterable>
inline size_t GetTimeAtPeak(Iterable const& vout)
{
    auto iterator = hydra_thrust::max_element( vout.begin() , vout.end() );
    size_t i = iterator - vout.begin();
    
    return i;
}




template<typename Iterable>
inline double GetVAtPeak(Iterable const& vout)
{
    const size_t i = GetTimeAtPeak(vout);
    return vout[i];
}





template<typename Iterable>
inline double SlopeOnThrs(Iterable const& vout, size_t const& Tth)
{
    SAFE_EXIT( vout.size() == Tth-1, "In SlopeOnThrs: out of range")
    
    double dvdt = ( vout[Tth+1] - vout[Tth]) ; //* 1e6;
    return dvdt;
}





template<typename Iterable>
inline double LinearFitNearThr(int const& kernel_id, size_t const& TOA, Iterable const& data, bool const& plot=false ) {
        
    const double bound_fit = (kernel_id == 0 || kernel_id == 4)? 200 : 20;
    
            
    const size_t min_fit = (TOA - bound_fit); //-200
    const size_t max_fit = (TOA + bound_fit); //+200
    
    SAFE_EXIT(min_fit < 0 , "In LinearFitNearThr(), out of range limit. Please insert an offset.")
    SAFE_EXIT(max_fit > data.size() , "In LinearFitNearThr(), out of range limit. Please insert an offset.")
            
    TGraph graph;
    for(size_t i=min_fit; i<max_fit+1; ++i)
    {
        graph.SetPoint(graph.GetN(), i , data[i]);
    }
            
    TFitResultPtr r = graph.Fit("pol1","S");
            
    double par1_fit = r->Parameter(1);
    
    if(plot)
    {
        TCanvas p("","",800,800);
        graph.Draw("APL");
        p.SaveAs("LinearFitNearThr.pdf");
    }
            
    return par1_fit;
}




template<typename Iterable>
inline double GaussianFitNearVmax(int const& kernel_id, Iterable const& data, bool const& plot=false ) {
        
    const double bound_fit = (kernel_id == 0 || kernel_id == 4)? 200 : 20;
    
    const size_t time = GetTimeAtPeak(data);
            
    const size_t min_fit = (time - bound_fit); 
    const size_t max_fit = (time + bound_fit);
    
    SAFE_EXIT(min_fit < 0 , "In GaussianFitNearVmax(), out of range limit. Please insert an offset.")
    SAFE_EXIT(max_fit > data.size() , "In GaussianFitNearVmax(), out of range limit. Please insert an offset.")
            
    TGraph graph;
    for(size_t i=min_fit; i<max_fit+1; ++i)
    {
        graph.SetPoint(graph.GetN(), i , data[i]);
    }

    TFitResultPtr r = graph.Fit("gaus","S");
    
    double par1_fit = r->Parameter(0);
    
    if(plot)
    {
        TCanvas p("","",800,800);
        graph.Draw("APL");
        p.SaveAs("GaussianFitNearVmax.pdf");
    }
            
    return par1_fit;
}




template<typename Iterable>
inline size_t TimeRefMethod(int const& kernel_id, Iterable const& vout, double const& vmax)
{

    size_t TOA80 = LeadingEdge(vout , vmax*0.8);
    size_t TOA20 = LeadingEdge(vout , vmax*0.2);
    
    size_t delay = (TOA80-TOA20)/2;
    
     hydra::host::vector<double> subtr( vout.size() );
     
     for(size_t i=0; i<delay; ++i) subtr[i] = vout[i] - 0.0;
     
     for(size_t i=delay; i<vout.size()-delay; ++i) subtr[i] = vout[i] - vout[i-delay];
     
     double newthr = 0.5 * GaussianFitNearVmax( kernel_id,  subtr );
     
     
     return LeadingEdge(subtr , newthr);
}


} //namespace algo
} //namespace tfboost

#endif
