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
    
    ERROR_RETURN( it==vout.end(), "In LeadingEdge: no element found.", 0)
    
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
 * Computation of the slope of a noisy signal
 * at a specific position
 * A linear fit in the region near the specified position
 * is done to get the correct slope
 * If errors return -1
 */
template<typename Iterable>
inline double LinearFitNearThr(size_t const& TOA, 
                               Iterable const& data, 
                               size_t const& bound_fit, 
                               bool const& plot=false ) {
        
    //const double bound_fit = (kernel_id == 0 || kernel_id == 4)? 200 : 20;
    
    const size_t min_fit = (TOA - bound_fit); 
    const size_t max_fit = (TOA + bound_fit); 
    
    ERROR_RETURN( min_fit < 0 || max_fit > data.size(), 
                  "In LinearFitNearThr(), out of range limit. Exit with -1", -1.0)
            
    TGraph graph;
    
    #pragma unroll
    for(size_t i=min_fit; i<max_fit+1; ++i)
        graph.SetPoint(graph.GetN(), i , data[i]);

            
    TFitResultPtr r = graph.Fit("pol1","S");

    ERROR_RETURN( r==-1, "Error in LinearFitNearThr. Exit with -1.", -1.0)

    
    if(plot){
        TCanvas p("","",800,800);
        graph.Draw("APL");
        p.SaveAs("LinearFitNearThr.pdf");
    }
            
    return r->Parameter(1);
}



/*
 * Computation of the maximum value of a noisy signal
 * A gaussian fit is done 
 * is done to get the correct max value
 * If errors return -1
 */
template<typename Iterable>
inline double GaussianFitNearVmax(Iterable const& data, size_t const& bound_fit, bool const& plot=false ) {
        
    //const double bound_fit = (kernel_id == 0 || kernel_id == 4)? 200 : 20;
    
    const size_t time = GetTimeAtPeak(data);
            
    const size_t min_fit = (time - bound_fit); 
    const size_t max_fit = (time + bound_fit);

    ERROR_RETURN( min_fit < 0 || max_fit > data.size(), 
                 "In GaussianFitNearVmax(), out of range limit. Exit with -1", -1.0)
            
    TGraph graph;
    
    #pragma unroll
    for(size_t i=min_fit; i<max_fit+1; ++i)
        graph.SetPoint(graph.GetN(), i , data[i]);
    

    TFitResultPtr r = graph.Fit("gaus","S");

    ERROR_RETURN( r==-1, "Error in GaussianFitNearVmax. Exit with -1.", -1.0)

    if(plot)
    {
        TCanvas p("","",800,800);
        graph.Draw("APL");
        p.SaveAs("GaussianFitNearVmax.pdf");
    }
            
    return r->Parameter(0) ;
}



/*
 * Computation of the arrival time of a signal
 * using the Timespot reference method:
 * - shift of the signal by half the rise time
 * - calculate the difference between the original and shifted signals
 * - calculate the max value of the difference (with a gaussian fit)
 * - apply a LE at half the obtained max value (with a linear fit)
 * If error returns 0
 */
template<typename Iterable, typename Iterablet>
inline double TimeRefMethod(Iterable const& vout, Iterablet const& time, double const& vmax, size_t const& bound_fit, bool const& noise=false)
{

    const size_t N = vout.size();
    
    const size_t TOA80 = LeadingEdge(vout , vmax*0.8);
    const size_t TOA20 = LeadingEdge(vout , vmax*0.2);
    
    const size_t delay = (TOA80-TOA20)/2;
    
    hydra::host::vector<double> subtr( N );
    
    #pragma unroll
    for(size_t i=0; i<delay; ++i) 
        subtr[i] = vout[i] - 0.0;
    
    #pragma unroll
    for(size_t i=delay; i<N; ++i) 
        subtr[i] = vout[i] - vout[i-delay];
        
     
    double newthr = (noise)? 0.5 * GaussianFitNearVmax( subtr, bound_fit ) : 0.5 * GetVAtPeak(subtr);

    auto spl = hydra::make_spiline<double>(time, subtr );
    double exdT = time[2]-time[1];
    double indT = 1e-12;

    int min_fit = LeadingEdge(subtr , newthr) - bound_fit;
    int max_fit = LeadingEdge(subtr , newthr) + bound_fit;

    ERROR_RETURN( newthr<0 || min_fit < 0 || max_fit > (int)N, 
                 "Error in TimeRefMethod(), Exit with 0.", 0)
    
    double min = exdT*min_fit;
    double max = exdT*max_fit;
    size_t Np = (max-min)/indT;

    
    for(size_t i=0; i<Np; ++i)
        if( spl(min + indT*i)>newthr ) return min + indT*i; 
        
    return -1;
    
    /*
    return LeadingEdge(subtr , newthr);
    */
    


}


/*
 * Re-sample the signal with a specified dT
 * and a random phase clock
 */
template<typename Iterable, typename SPLINE, typename RNG>
inline void DoDigitization(Iterable& data, Iterable& time, 
                           SPLINE const& signal, double  const& dT, double  const& Tmax, RNG& rng, bool  const& rndmphase=true){
    
    SAFE_EXIT( !data.empty() || !time.empty(), "In DigitizeSignal: the containers must be empty.")

    hydra_thrust::uniform_real_distribution<double> uniDist(0.0, 1.0);

    double offset = rndmphase? uniDist(rng) * dT : 0 ;
    
    size_t Nsamples = Tmax/dT;

    data.push_back( 0.0 ); 
    time.push_back( 0.0 );
    
    for(size_t i=1; i<Nsamples; ++i){
        data.push_back( signal(offset + i*dT) ); 
        time.push_back( offset + i*dT );
    }


}


template<typename Iterable, typename RNG>
inline void DigitizeSignal(Iterable& data, Iterable& time, 
                            double const& dT, double const& Tmax, RNG& rng, bool const& rndmphase=false){

    hydra::host::vector<double> conv_dig;  
    hydra::host::vector<double> time_dig; 

    //time and conv_data_h are the not digitized ones
    auto conv_spline = hydra::make_spiline<double>(time, data );

    tfboost::algo::DoDigitization(conv_dig, time_dig, 
                                conv_spline, dT, Tmax, rng, rndmphase);

    // override time and conv_data containers
    data.erase(data.begin() + conv_dig.size(), data.end());
    time.erase(time.begin() + time_dig.size(), time.end());
    hydra::copy(conv_dig , data);
    hydra::copy(time_dig , time);

}



struct BestTimeInterval {

    BestTimeInterval(TH1D* h1) { 
        m_h1 = h1 ; 
        m_bin_max = h1->GetMaximumBin();
        m_bin_l = m_bin_max;
        m_bin_r = m_bin_max;
    }

    inline double Run(double frac){

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



