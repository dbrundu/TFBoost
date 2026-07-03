/*----------------------------------------------------------------------------
 *
 *   Copyright (C) 2026 Davide Brundu, Gian Matteo Cossu
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
 *  NoisyMeasurer.h
 *
 *  Created on: 2026
 *      Author: Davide Brundu
 */

#ifndef TFBOOST_MEASUREMENTS_NOISYMEASURER_H_
#define TFBOOST_MEASUREMENTS_NOISYMEASURER_H_

#include <cmath>
#include <iostream>

#include <tfboost/Types.h>
#include <tfboost/Utils.h>
#include <tfboost/Initializer.h>
#include <tfboost/Algorithms.h>
#include <tfboost/core/Signal.h>
#include <tfboost/measurements/IMeasurement.h>


namespace tfboost {

namespace measurements {

/*
 *  @class NoisyMeasurer
 *
 *  The measurement pass on the noisy signal. In addition to the basic
 *  estimators it optionally refines TOA / Vpeak with a linear fit near the
 *  threshold and a Gaussian fit near the maximum, computes the noise RMS, and
 *  (using the clean measures carried in the context) the electronic jitter that
 *  the caller fills into histograms.
 *
 *  Guard convention kept identical to the historical code:
 *   - the linear-fit guards test the CLEAN leading-edge index (mctx.toa_le_idx);
 *   - the Gaussian-fit guards test the NOISY constant-fraction index computed here.
 */
class NoisyMeasurer : public IMeasurement {

public:

    const char* name() const override { return "NoisyMeasurement"; }

    Measures_t measure(core::Signal const& sig, MeasureContext& mctx) const override
    {
        ConfigParser const& c     = mctx.c;
        Measures_t   const& clean = mctx.clean_measures;
        const size_t TOA_LE       = mctx.toa_le_idx;   // clean leading-edge index (fit guards)

        Measures_t m = tfboost::MeasuresInitializer::get_values();

        // reject "empty" events
        if( tfboost::algo::LeadingEdge(sig.amplitude(), c.LE_reject_noise) == sig.size() )
        {
            mctx.valid = false;
            return m;
        }

        // RMS of the noise over the (digitized) offset region
        size_t new_offset = c.offset*1e-12 / c.sampling_dT;
        double rms_noise  = 0.0;
        for (size_t i=0; i<new_offset; i++) rms_noise += sig.amplitude()[i] * sig.amplitude()[i];
        rms_noise = ::sqrt(rms_noise/new_offset);
        mctx.rms_noise = rms_noise;

        size_t timeatmax_idx    = tfboost::algo::GetTimeAtPeak(sig.amplitude());
        size_t TOA_LE_noise_idx = tfboost::algo::LeadingEdge(sig.amplitude(), c.LEthr);

        m[_tpeak]    = sig.time()[timeatmax_idx] ;
        m[_toa_le]   = sig.time()[TOA_LE_noise_idx]  ;
        m[_vonth_le] = sig.amplitude()[TOA_LE_noise_idx];
        m[_vpeak]    = tfboost::algo::GetVAtPeak(sig.amplitude());

        size_t TOA_CFD = tfboost::algo::ConstantFraction(sig.amplitude(), c.CFD_fr, m[_vpeak]);
        m[_toa_cfd]    = sig.time()[TOA_CFD] ;

        auto rm_noise  = tfboost::algo::TimeRefMethod( sig.amplitude(), sig.time(), m[_vpeak], c.RM_delay, c.bound_fit, /*noise?*/false, /*plot?*/false );
        m[_toa_rm]     = std::get<0>( rm_noise ) ;
        m[_dvdt_rm]    = 1e-6 * std::get<2>( rm_noise ) ;
        m[_vonth_rm]   = std::get<1>( rm_noise ) ;
        m[_dvdt_le]    = 1e-6 * mctx.SampRate * tfboost::algo::SlopeOnThrs(sig.amplitude(), TOA_LE_noise_idx);
        m[_dvdt_cfd]   = 1e-6 * mctx.SampRate * tfboost::algo::SlopeOnThrs(sig.amplitude(), TOA_CFD);
        m[_vonth_le]   = sig.amplitude()[TOA_LE_noise_idx];
        m[_vonth_cfd]  = sig.amplitude()[TOA_CFD];
        m[_tot]        = tfboost::algo::TimeOverThr(sig.amplitude(), sig.time(), c.LEthr, c.LEthr) ;

        mctx.fill_jitter = false;
        if(!(c.MakeGaussianFitNearVmax && TOA_CFD>1))
        {
            // electronic jitter from the basic estimators
            mctx.fill_jitter = true;
            mctx.jitter_cfd  = m[_toa_cfd]-clean[_toa_cfd];
            mctx.jitter_le   = m[_toa_le] -clean[_toa_le];
            mctx.jitter_rm   = m[_toa_rm] -clean[_toa_rm];
        }

        if(c.MakeLinearFitNearThreshold && TOA_LE>1)
        {
            auto toa   = tfboost::algo::LinearFitNearThr( c.LEthr, sig.amplitude(), sig.time(), c.bound_fit, c.PlotLinFit, "LEfit");
            m[_toa_le]  = std::get<0>(toa);
            m[_dvdt_le] = 1e-6*std::get<1>(toa) ;
        }

        if(c.MakeGaussianFitNearVmax && TOA_CFD>1)
        {
            auto gaussfit = tfboost::algo::GaussianFitNearVmax( sig.amplitude(), sig.time(), c.bound_fit, c.PlotGausFit );
            m[_tpeak] = std::get<1>(gaussfit);
            m[_vpeak] = std::get<0>(gaussfit);

            auto cfd_idx  = tfboost::algo::ConstantFraction(sig.amplitude(), c.CFD_fr, m[_vpeak]);
            m[_toa_cfd]   = sig.time()[cfd_idx] ;
            m[_vonth_cfd] = sig.amplitude()[ cfd_idx ];

            if(c.MakeLinearFitNearThreshold && TOA_LE>1)
            {
                auto toa_cf = tfboost::algo::LinearFitNearThr( c.CFD_fr*m[_vpeak],
                                                               sig.amplitude(), sig.time(), c.bound_fit,
                                                               /*plot?*/c.PlotLinFit, "CFDfit");

                auto rm     = tfboost::algo::TimeRefMethod( sig.amplitude(), sig.time(),
                                                            m[_vpeak], c.RM_delay, c.bound_fit,
                                                            /*noise?*/true, /*plot?*/c.PlotRMfit);

                m[_toa_cfd]  = std::get<0>(toa_cf);
                m[_dvdt_cfd] = 1e-6 * std::get<1>(toa_cf);
                m[_toa_rm]   = std::get<0>(rm) ;
                m[_vonth_rm] = std::get<1>(rm);
                m[_dvdt_rm]  = 1e-6 * std::get<2>(rm);

                mctx.fill_jitter = true;
                mctx.jitter_cfd  = m[_toa_cfd]-clean[_toa_cfd];
                mctx.jitter_le   = m[_toa_le] -clean[_toa_le];
                mctx.jitter_rm   = m[_toa_rm] -clean[_toa_rm];
            }
        }

        if(c.TOTcorrection)
            m[_toa_le] = tfboost::algo::CorrectTOA(m[_toa_le], m[_tot], c.TOT_a, c.TOT_b);

        // adding the time-tagger resolution
        if(c.TimeReferenceResolution)
            for(auto key : {_toa_le, _tpeak, _toa_cfd, _toa_rm, _tot} )
                m[key] += mctx.TR_res;

        mctx.valid = true;
        return m;
    }


    void print(Measures_t const& m, MeasureContext const& mctx) const override
    {
        RULE_LINE_LIGHT;
        std::cout << _START_INFO_;
        std::cout << "Measurements with noise:\n";
        std::cout << _END_INFO_;
        std::cout << "Time on thresholds (LE)  = " << m[_toa_le]    << " (s)\n";
        std::cout << "Time on thresholds (CFD) = " << m[_toa_cfd]   << " (s)\n";
        std::cout << "Time on thresholds (RM)  = " << m[_toa_rm]    << " (s)\n";
        std::cout << "V on thresholds (CFD)    = " << m[_vonth_cfd] << " (V)\n";
        std::cout << "V on thresholds (LE)     = " << m[_vonth_le]  << " (V)\n";
        std::cout << "V on thresholds (RM)     = " << m[_vonth_rm]  << " (V)\n";
        std::cout << "Vpeak                    = " << m[_vpeak]     << " (V)\n";
        std::cout << "Tpeak                    = " << m[_tpeak]     << " (s)\n";
        std::cout << "dv/dt (CFD)              = " << m[_dvdt_cfd]  << " (uV/ps)\n";
        std::cout << "dv/dt (LE)               = " << m[_dvdt_le]   << " (uV/ps)\n";
        std::cout << "dv/dt (RM)               = " << m[_dvdt_rm]   << " (uV/ps)\n";
        std::cout << "RMS of noise             = " << mctx.rms_noise << " (V)\n";
    }

};

} // namespace measurements

} // namespace tfboost

#endif /* TFBOOST_MEASUREMENTS_NOISYMEASURER_H_ */
