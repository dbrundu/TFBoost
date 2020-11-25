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
 
#ifndef TFBOOST_ALGORITHMS_H_
#define TFBOOST_ALGORITHMS_H_

#include <tfboost/Types.h>
#include <tfboost/Utils.h>


namespace tfboost { 

namespace algo { 



/* 
 * Leading edge algorithm return the position 
 * of the first element in the signal vout that
 * is greater than the threshold Vthr
 * If not found returns 0
 */
template<typename Iterable>
inline size_t LeadingEdge(Iterable const& vout, double const& Vthr)
{
    auto trigger = [=] (double v) { return v > Vthr; };
    
    auto it = hydra_thrust::find_if(vout.begin(), vout.end(), trigger );
    
    ERROR_RETURN( it==vout.end(), "In LeadingEdge: no element found.", vout.size() )
    
    return  it-vout.begin();
    
}



/*
 * Constant fraction discrimination
 * applies the LE algorithm at the threshold
 * specified by a fraction of the signal amplitude
 * If not found returns 0
 */
template<typename Iterable>
inline size_t ConstantFraction(Iterable const& vout, double const& fraction, double const& vmax)
{
    return LeadingEdge(vout , vmax*fraction);
}



/* 
 * Time over threshold algorithm return the difference of positions
 * for which the signal 
 * is greater than 2 thresholds, Vthr1 and Vthr2
 * If not found returns 0
 */
template<typename Iterable>
inline double TimeOverThr(Iterable vout, Iterable const& time, double const& Vthr1, double const& Vthr2)
{

    size_t t1 = LeadingEdge(vout , Vthr1);
    
    hydra_thrust::reverse(vout.begin() , vout.end() ); 
    
    size_t t2 = vout.size() - LeadingEdge(vout , Vthr2);
    
    ERROR_RETURN( t1==vout.size() || t2==vout.size(), 
                  "In TimeOverThr: no element found.", vout.size() )
                  
    return  time[t2] - time[t1];
}



/*
 * Return the position of the element
 * corresponding to the maximum signal value
 * If not found returns 0
 */
template<typename Iterable>
inline size_t GetTimeAtPeak(Iterable const& vout)
{
    auto it = hydra_thrust::max_element( vout.begin() , vout.end() );
    size_t i = it - vout.begin();
    
    ERROR_RETURN( it==vout.end(), "In GetTimeAtPeak: no element found.", 0)
    
    return i ;
}




/*
 * Return the the maximum signal value
 * If not found returns -1
 */
template<typename Iterable>
inline double GetVAtPeak(Iterable const& vout)
{
    const size_t i = GetTimeAtPeak(vout);
    
    ERROR_RETURN( i==0, "In GetVAtPeak: no element found.", -1.0)
    
    return vout[i];
}




/*
 * Computation of the slope of a smooth signal
 * (i.e. without noise) at a specific position
 * If out of range exit with -1
 */
template<typename Iterable>
inline double SlopeOnThrs(Iterable const& vout, size_t const& Tth)
{

    ERROR_RETURN( vout.size() == Tth-1, "In SlopeOnThrs: out of range.", -1.0)
    
    return ( vout[Tth+1] - vout[Tth]) ; //* 1e6;
    
}




/*
 * Correct the TOA based on the TOT measurements
 */
inline double CorrectTOA(double const& toa, double const& tot, double const& a, double const& b)
{
    ERROR_RETURN( !std::isnormal(a), "In CorrectTOA: undefined 'a' value.", -1.0)
    
    double x = -b/(2.*a);
    
    return ( a * ( tot + x)  + b ) * (tot - x);

}




/*
 * Functor to compute the slope of a noisy signal
 * at a specific position
 * A linear fit in the region near the specified position
 * is done to get the correct slope.
 */
template<typename Iterable, typename Iterablet>
inline pairDD_type LinearFitNearThr(double const& Thr, 
                                    Iterable const& data, 
                                    Iterablet const& time,
                                    size_t const& bound_fit, 
                                    bool const& plot=false,
                                    TString const& title = "" ) {
    
    // temporary TOA position
    const size_t TOA = LeadingEdge(data, Thr);
   
    const size_t min_fit = (TOA - bound_fit); 
    const size_t max_fit = (TOA + bound_fit); 
    
    ERROR_RETURN( min_fit < 0 || max_fit > data.size(), 
                  "In LinearFitNearThr(), out of range limit. Exit with -1", pairDD_type(-1.0,-1.0)) 
            
    TGraph graph;
    
    for(size_t i=min_fit; i<max_fit+1; ++i)
        graph.SetPoint(graph.GetN(), time[i] , data[i]);

    TFitResultPtr r = graph.Fit("pol1","SQ");

    ERROR_RETURN( r==-1, "Error in LinearFitNearThr. Exit with -1.", pairDD_type(-1.0,-1.0) )
    
    double q   = r->Parameter(0);
    double m   = r->Parameter(1); 
    double toa = (Thr-q)/m ;
    
    if(plot){
        TCanvas p("","",800,800);
        graph.Draw("APL");
        p.SaveAs(TString("LinearFitNearThr")+title+".pdf");
    }
            
    return pairDD_type(toa, m);
}



/*
 * Functor to compute the maximum value of a noisy signal
 * A gaussian fit is done 
 * to get the correct max value
 */
template<typename Iterable, typename Iterablet>
inline pairDD_type GaussianFitNearVmax(Iterable const& data, 
                                       Iterablet const& time, 
                                       size_t const& bound_fit, 
                                       bool const& plot=false,
                                       TString const& title = "Vmax" ) {
    
    const size_t t0 = GetTimeAtPeak(data);
            
    const size_t min_fit = (t0 - bound_fit); 
    const size_t max_fit = (t0 + bound_fit);

    ERROR_RETURN( min_fit < 0 || max_fit > data.size(), 
                 "In GaussianFitNearVmax(), out of range limit. Exit with -1", pairDD_type(-1.0,-1.0) )
            
    TGraph graph;
    
    for(size_t i=min_fit; i<max_fit+1; ++i)
        graph.SetPoint(graph.GetN(), time[i] , data[i]);
    
    TFitResultPtr r = graph.Fit("gaus","SQ");

    ERROR_RETURN( r==-1, "Error in GaussianFitNearVmax. Exit with -1.", pairDD_type(-1.0,-1.0))
    
    double amp = r->Parameter(0);
    double mu  = r->Parameter(1);

    if(plot) 
      SaveMonitorGraph( TString("GaussianFit_")+title, "time", "Voltage", graph );
            
    return pairDD_type(amp,mu) ;
}



/*
 * Computation of the arrival time of a signal
 * using the Timespot reference method:
 * - shift of the signal by half the rise time
 * - calculate the difference between the original and shifted signals
 * - calculate the max value of the difference (with a gaussian fit)
 * - apply a LE at half the obtained max value (interpolated)
 * If error returns (-1.0 , 0)
 */
template<typename Iterable, typename Iterablet>
inline tripletDDD_type TimeRefMethod(Iterable const& vout, 
                                 Iterablet const& time,
                                 double const& vmax,
                                 double const& delay_t,
                                 size_t const& bound_fit,
                                 bool const& noise = false,
                                 bool const& plot  = false,
                                 TString const& title = "tref")
{
    
    const size_t N     = vout.size();
    const double dT    = time[1]-time[0];
    const size_t delay = delay_t/dT;
    
    ERROR_RETURN( delay == 0, 
                 "Error in TimeRefMethod() [delay == 0], Exit with (-1,0)", tripletDDD_type(-1.0, -1.0, -1.0) )

    HostSignal_t subtr( N );

    // subtract the delayed signal
    for(size_t i=0; i<N; ++i) {
      double x = i<delay? 0.0 : vout[i-delay];
      subtr[i] = vout[i] - x;
    }

    double newthr     =  (noise)? 0.5 * GaussianFitNearVmax( subtr, time, bound_fit+3, plot, title ).first : 0.5 * GetVAtPeak(subtr);
    int    min_fit    =  LeadingEdge(subtr , newthr) - bound_fit;
    int    max_fit    =  LeadingEdge(subtr , newthr) + bound_fit;
    double min_fit_t  =  time[min_fit];
    double max_fit_t  =  time[max_fit];
    
    size_t dummy_TOA_RM = LeadingEdge(subtr , newthr);

    ERROR_RETURN( newthr<0 || min_fit < 0 || max_fit > (int) N, 
                 "Error in TimeRefMethod() [out of range], Exit with (-1,0)", tripletDDD_type(-1.0, -1.0, -1.0) )
                 
    
    // Linear fit near thr 
    // of the subtracted signal
    TGraph graph;
    
    for(int i=min_fit; i<max_fit+1; ++i)
        graph.SetPoint(graph.GetN(), time[i] , subtr[i]);
        
    TFitResultPtr r = graph.Fit("pol1","SQ");
    
    ERROR_RETURN( r==-1, "Error in TimeRefMethod() [fit error], Exit with (-1,0)", tripletDDD_type(-1.0, -1.0, -1.0) )
    
    double q   = r->Parameter(0);
    double m   = r->Parameter(1); 
    double toa = (newthr-q)/m ;
    
    
    if(plot)
    {
        TMultiGraph mg;
        TGraph graph2, graph3;
        
        for(size_t i=0; i<subtr.size(); ++i){
            graph2.SetPoint(graph2.GetN(), time[i] , subtr[i]);
            graph3.SetPoint(graph3.GetN(), time[i] , vout[i]);}
        
        TCanvas p2("","",800,800);
        graph3.SetLineColor(kRed);
        mg.Add(&graph2,"lp");
        mg.Add(&graph3,"cp");
        mg.Draw("a");
        p2.Update();
        TLine l(p2.GetUxmin(), newthr , p2.GetUxmax(), newthr);
        l.SetLineStyle(kDashed);
        l.Draw("same");
        p2.SaveAs( TString("SubtratedSignal")+title+".pdf");
        
        TCanvas p("","",800,800);
        graph.Draw("APL");
        p.Update();
        TLine l2(p.GetUxmin(), newthr , p.GetUxmax(), newthr);
        l2.Draw("same");
        p.SaveAs(TString("LinearFit")+title+".pdf");
    }
    
    return tripletDDD_type(toa, newthr, m);
}




/*
 * Algorithm that calculates the best (smaller)
 * time interval of a time distributions
 * given a specific fraction of events to consider.
 */
struct BestTimeInterval {

    BestTimeInterval(TH1D* h1) {
        m_h1 = h1 ; 
        m_bin_max = h1->GetMaximumBin();
        m_bin_l = m_bin_max;
        m_bin_r = m_bin_max;
    }

    inline double operator()(double frac){

        double N = m_h1->GetEntries();
        double M = frac * N;
        double R = 0;

        while(R<M) R = Compare();

        double l  = m_h1->GetBinCenter( m_bin_l );
        double r  = m_h1->GetBinCenter( m_bin_r );

        return 0.5*(r-l);
    }

    inline double Compare(){
        
        double l  = m_h1->GetBinContent( m_bin_l-1 );
        double r  = m_h1->GetBinContent( m_bin_r+1 );

        if( l>r ) --m_bin_l;
        if( l<=r) ++m_bin_r;

        m_DeltaBin = m_bin_r - m_bin_l;

        return m_h1->Integral(m_bin_l, m_bin_r);
    }

    TH1D* m_h1;
    int m_bin_max;
    int m_bin_l;
    int m_bin_r;
    int m_DeltaBin;
    double m_frac;
};



} //namespace algo
} //namespace tfboost

#endif



