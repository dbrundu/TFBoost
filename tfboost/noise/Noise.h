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
 
#ifndef NOISE_H_
#define NOISE_H_



namespace tfboost { 

namespace noise { 




/*
 *  @class Noise
 *  Class that implments a white or red noise and
 *  add it to an input signal
 *
 */
class Noise 
{

public:

    Noise() = delete;
    
    Noise(double sigmanoise, bool userednoise=false, double correlation=0.0) :
         fsigmanoise(sigmanoise), fuserednoise(userednoise), fcorrelation(correlation)
    {}
    
    
    Noise(Noise const& other):
        fsigmanoise(other.GetSigmaNoise()),
        fuserednoise(other.GetUseRedNoise()),
        fcorrelation(other.GetCorrelation())
    {}
    
    
    Noise& operator=( Noise const& other){
        if(this == &other) return *this;
        fsigmanoise  = other.GetSigmaNoise();
        fuserednoise = other.GetUseRedNoise();
        fcorrelation = other.GetCorrelation();
        return *this;
    }
    
    
    inline double GetSigmaNoise() const { return fsigmanoise; }
    
    inline double GetCorrelation() const { return fcorrelation; }
    
    inline bool GetUseRedNoise() const { return fuserednoise; }
    
    inline void SetSigmaNoise(double sigmanoise) { fsigmanoise = sigmanoise; }
    
    inline void SetCorrelation(double correlation) { fcorrelation = correlation; }
    
    inline void SetUseRedNoise(double userednoise) { fuserednoise = userednoise; }
    
    
    
    inline void AddNoiseToSignal(hydra::host::vector<double>& data_h, size_t rng_seed) 
    {
        const size_t N = data_h.size();
        
        auto gauss   = hydra::Gaussian<double>(0.0, fsigmanoise);
        
        auto noise_d = hydra::device::vector<double>(N);
        auto noise_h = hydra::host::vector<double>(N);
        
        auto data_d  = hydra::device::vector<double>(N);
        
        hydra::copy(data_h , data_d);
        
        hydra::fill_random(noise_d , gauss, rng_seed );
        
        if(fuserednoise){
        
            hydra::copy(noise_d , noise_h);
            
            auto noise_h_final = hydra::host::vector<double>(N);
            noise_h_final[0]   = noise_h[0];
            
            #pragma unroll
            for(size_t i=1; i<N ; ++i)
            {
                noise_h_final[i] = fcorrelation * noise_h_final[i-1] + ::sqrt(1.0 - fcorrelation*fcorrelation)*noise_h[i];
            }
                    
            hydra::copy(noise_h_final , noise_d);
        }
        
        auto zipped_range = hydra::zip( data_d, noise_d);
        
        hydra::for_each( zipped_range, [] __hydra_dual__ ( hydra::tuple<double&, double&> X){
                hydra::get<0>(X) += hydra::get<1>(X);
        });
            
        hydra::copy(data_d, data_h);
    
    }

private:

    double fsigmanoise;
    bool  fuserednoise;
    double fcorrelation;

};





/*
 *  Function that computes the theoretical probability
 *  to trigger at a specific threshold (Vth) having a white noise
 *  with a specific sigma (sigmanoise)
 */
template<typename Iterable>
hydra::host::vector<double> GetTheorProbability(Iterable const& vout, double const& VTh, double const& sigmanoise)
{
    double min      = 0.0;
    double constant = 1./::sqrt(2*PI*sigmanoise*sigmanoise);
    
    hydra::host::vector<double> integrals( vout.size() );
    
    auto gaussianIntegrator = hydra::AnalyticalIntegral< hydra::Gaussian<double> >(VTh, 1e20);
    
    for(size_t i=1; i<vout.size(); ++i) 
    {
        auto xgauss     = hydra::Gaussian<double>( vout[i], sigmanoise);
        integrals[i]    = constant * gaussianIntegrator(xgauss).first;
    }
    
    hydra::host::vector<double> prob( vout.size() );
    prob[0] = integrals[0];
    
    for(size_t i=1; i<vout.size(); ++i)
    {
        prob[i] = integrals[i];
        for(size_t k = 0 ; k<i; ++k) prob[i] *= (1-integrals[k]);
    }
    
    return prob;
}






/*
 *  Function similar to GetTheorProbability, but that
 *  additionally filter the signal in the region near the threshold
 */
template<typename Iterablex, typename Iterabley>
hydra::host::vector<double> ComputeTOAcurve(size_t const& TOA_CFD, 
                                            size_t const& min_fit2,
                                            size_t const& max_fit2,
                                            double const CFD_fr, 
                                            double const& Vpeak, 
                                            double const& sigmanoise,
                                            Iterablex idx, 
                                            Iterabley conv_data_d,
                                            TString const& filename="thjitter.pdf") 
{

    size_t min_fit = 160; //(TOA_CFD-500);
    size_t max_fit = 280; //(TOA_CFD+500);

    TGraph hist_thjitter;
            
    auto filterx = hydra::wrap_lambda( [&] __hydra_dual__ (double x) 
                { return x>=min_fit && x<max_fit; } );
                           
    auto filtery = hydra::wrap_lambda( [&] __hydra_dual__ (double x) 
                { return conv_data_d[x]; } );
                           
    auto xrange = hydra::filter(idx , filterx);
    auto yrange = xrange | filtery;
            
    hydra::host::vector<double> xdata(max_fit-min_fit);
    hydra::copy(yrange , xdata);
            
    auto th_jitter = GetTheorProbability(xdata, CFD_fr*Vpeak, sigmanoise);
            
    for(size_t i=0;  i < max_fit-min_fit; ++i)
    {
        hist_thjitter.SetPoint(hist_thjitter.GetN(), i+min_fit , th_jitter[i]);;
    }
            
    TCanvas canv_thjitter("canv_thjitter", "canv_thjitter", 800, 800);
    hist_thjitter.SetTitle("Time of arrival from first principles;time [ps];A.U.");

    hist_thjitter.Draw("APL");

    canv_thjitter.SaveAs(TString("plots/") + filename );
            
    return th_jitter;
           
}




} //namespace noise
} //namespace tfboost

#endif
