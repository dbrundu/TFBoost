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
 *  Noise.h
 *
 *  Core signal-processing primitive: noise injection (white / red) and the
 *  related theoretical trigger-probability helpers. Free functions in
 *  namespace tfboost::core, called by the transform modules.
 *
 *  Created on: 07/05/2020
 *      Author: Davide Brundu
 */

#ifndef TFBOOST_CORE_NOISE_H_
#define TFBOOST_CORE_NOISE_H_

#include <tfboost/Types.h>
#include <tfboost/ConfigParser.h>
#include <tfboost/core/Convolution.h>
#include <tfboost/functions/ButterworthFilter.h>


namespace tfboost {

namespace core {

/*
 *  Add simulated noise (white, or red with the given correlation) to a signal
 *  in place. If the configuration requests filtering of the noise only, the
 *  generated noise is low-pass filtered (Butterworth) before being added.
 */
inline void add_simulated_noise(HostSignal_t& data_h,
                                double sigmanoise,
                                bool   userednoise,
                                double correlation,
                                size_t rng_seed,
                                tfboost::ConfigParser const& c)
{
    const size_t N0 = data_h.size();

    // use a larger size to "warm-up" the noise
    const size_t M = N0/3;
    const size_t N  = N0+M;

    hydra::Gaussian<double> gauss(0.0, sigmanoise);

    DevSignal_t  data_d(N0);
    DevSignal_t  noise_d(N);
    HostSignal_t noise_h(N);

    hydra::copy(data_h , data_d);

    hydra::fill_random(noise_d , gauss, rng_seed );

    if(userednoise)
    {
        hydra::copy(noise_d , noise_h);

        HostSignal_t noise_h_final(N);
        noise_h_final[0] = noise_h[0];

        for(size_t i=1; i<N ; ++i)
            noise_h_final[i] = correlation * noise_h_final[i-1] + ::sqrt(1.0 - correlation*correlation)*noise_h[i];

        hydra::copy(noise_h_final , noise_d);
        hydra::copy(noise_h_final , noise_h);
    }

    ///////////////////////////////////
    if(c.LowPassFilter && c.FilterOnlyNoise)
    {
       HostSignal_t time_temp(N);
       for(size_t i=0; i<N; ++i) time_temp[i] = i*c.dT;

       auto flt        = tfboost::ButterworthFilter<double>( c.LowPassFrequency, c.LowPassOrder, c.dT);
       auto conv_temp  = hydra::make_spline<double>(time_temp, noise_h);

       tfboost::core::convolve(hydra::fft::fftw_f64, flt, conv_temp, noise_h, 0, (N-1)*c.dT, N);
       hydra::copy(noise_h , noise_d);
    }
    ///////////////////////////////////

    // erase the warm-up
    noise_d.erase (noise_d.begin(), noise_d.begin()+M);

    auto zipped_range = hydra::zip( data_d, noise_d);

    hydra::for_each( zipped_range, [] __hydra_dual__ ( hydra::tuple<double&, double&> X){
            hydra::get<0>(X) += hydra::get<1>(X);
    });

    hydra::copy(data_d, data_h);
}




/*
 *  Add noise samples, stored in the noise container, to the signal samples.
 */
template<typename SIGNAL, typename NOISE>
inline void add_noise_samples(SIGNAL& signal, NOISE const& noise,  tfboost::ConfigParser const& c){

    const size_t signal_N = signal.size();
    const size_t noise_N  = noise.size();

    SAFE_EXIT(noise_N < signal_N, "Impossible to add noise, too few samples.")

    DevSignal_t  signal_d(signal_N);
    DevSignal_t  noise_d(noise_N);
    HostSignal_t noise_h(noise_N);

    hydra::copy(signal, signal_d);
    hydra::copy(noise, noise_d);
    hydra::copy(noise, noise_h);


    ///////////////////////////////////
    if(c.LowPassFilter && c.FilterOnlyNoise)
    {
        HostSignal_t time_temp(noise_N);
        for(size_t i=0; i<noise_N; ++i) time_temp[i] = i*c.dT;

        auto flt        = tfboost::ButterworthFilter<double>( c.LowPassFrequency, c.LowPassOrder, c.dT);
        auto conv_temp  = hydra::make_spline<double>(time_temp, noise_h);

        tfboost::core::convolve(hydra::fft::fftw_f64, flt, conv_temp, noise_h, 0, (noise_N-1)*c.dT, noise_N);
        hydra::copy(noise_h , noise_d);
    }
    ///////////////////////////////////

    auto zipped_range = hydra::zip(signal_d, noise_d);

    hydra::for_each(zipped_range , [] __hydra_dual__ (hydra::tuple<double&, double&> X) {
        hydra::get<0>(X) += hydra::get<1>(X);
    });

    hydra::copy(signal_d, signal);

}



/*
 *  Compute the theoretical probability to trigger at a specific threshold (VTh)
 *  having a white noise with a specific sigma (sigmanoise).
 */
template<typename Iterable>
inline HostSignal_t theoretical_trigger_probability(Iterable const& vout, double const& VTh, double const& sigmanoise)
{
    double min      = 0.0;
    double constant = 1./::sqrt(2*PI*sigmanoise*sigmanoise);

    HostSignal_t integrals( vout.size() );

    hydra::AnalyticalIntegral< hydra::Gaussian<double> > gaussianIntegrator(VTh, 1e20);

    for(size_t i=1; i<vout.size(); ++i)
    {
        hydra::Gaussian<double> xgauss( vout[i], sigmanoise);
        integrals[i]  = constant * gaussianIntegrator(xgauss).first;
    }

    HostSignal_t prob( vout.size() );
    prob[0] = integrals[0];

    for(size_t i=1; i<vout.size(); ++i)
    {
        prob[i] = integrals[i];
        for(size_t k = 0 ; k<i; ++k) prob[i] *= (1-integrals[k]);
    }

    return prob;
}



/*
 *  As theoretical_trigger_probability, but additionally filters the signal in
 *  the region near the threshold and saves a diagnostic plot.
 */
template<typename Iterablex, typename Iterabley>
inline HostSignal_t compute_toa_curve(size_t const& TOA_CFD,
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

    HostSignal_t xdata(max_fit-min_fit);
    hydra::copy(yrange , xdata);

    auto th_jitter = theoretical_trigger_probability(xdata, CFD_fr*Vpeak, sigmanoise);

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


} // namespace core

} // namespace tfboost

#endif /* TFBOOST_CORE_NOISE_H_ */
