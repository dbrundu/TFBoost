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
 *  CleanMeasurer.h
 *
 *  Created on: 2026
 *      Author: Davide Brundu
 */

#ifndef TFBOOST_MEASUREMENTS_CLEANMEASURER_H_
#define TFBOOST_MEASUREMENTS_CLEANMEASURER_H_

#include <tfboost/Types.h>
#include <tfboost/Initializer.h>
#include <tfboost/Algorithms.h>
#include <tfboost/core/Signal.h>
#include <tfboost/measurements/IMeasurement.h>


namespace tfboost {

namespace measurements {

/*
 *  @class CleanMeasurer
 *
 *  The no-noise measurement pass: timing (LE / CFD / reference-method), peak
 *  value and time, slopes and voltages on thresholds, computed directly on the
 *  convoluted (un-noised) signal. Sets the empty-event flag and exposes the
 *  leading-edge / constant-fraction indices for the subsequent noisy pass.
 */
class CleanMeasurer : public IMeasurement {

public:

    const char* name() const override { return "CleanMeasurement"; }

    Measures_t measure(core::Signal const& sig, MeasureContext& mctx) const override
    {
        ConfigParser const& c = mctx.c;

        Measures_t m = tfboost::MeasuresInitializer::get_values();

        // reject "empty" events (amplitude below the rejection threshold)
        if( tfboost::algo::LeadingEdge(sig.amplitude(), c.LE_reject_nonoise) == sig.size() )
        {
            mctx.valid = false;
            return m;
        }

        size_t TOA_LE      = tfboost::algo::LeadingEdge(sig.amplitude(), c.LEthr);
        m[_toa_le]         = sig.time()[TOA_LE] ;

        size_t TimeAtPeak  = tfboost::algo::GetTimeAtPeak(sig.amplitude());
        m[_tpeak]          = sig.time()[TimeAtPeak] ;

        m[_vpeak]          = tfboost::algo::GetVAtPeak(sig.amplitude());

        size_t TOA_CFD     = tfboost::algo::ConstantFraction(sig.amplitude(), c.CFD_fr, m[_vpeak]);
        m[_toa_cfd]        = sig.time()[TOA_CFD] ;

        auto rm            = tfboost::algo::TimeRefMethod( sig.amplitude(), sig.time(), m[_vpeak], c.RM_delay, c.bound_fit, /*noise?*/false, /*plot?*/false );
        m[_toa_rm]         = std::get<0>( rm );
        m[_dvdt_rm]        = 1e-6 * std::get<2>( rm );
        m[_vonth_rm]       = std::get<1>( rm );

        m[_dvdt_le]        = 1e-6 * mctx.SampRate * tfboost::algo::SlopeOnThrs(sig.amplitude(), TOA_LE);
        m[_dvdt_cfd]       = 1e-6 * mctx.SampRate * tfboost::algo::SlopeOnThrs(sig.amplitude(), TOA_CFD);

        m[_vonth_le]       = sig.amplitude()[TOA_LE];
        m[_vonth_cfd]      = sig.amplitude()[TOA_CFD];

        // adding the time-tagger resolution
        if(c.TimeReferenceResolution)
            for(auto key : {_toa_le, _tpeak, _toa_cfd, _toa_rm, _tot} )
                m[key] += mctx.TR_res;

        mctx.toa_le_idx  = TOA_LE;
        mctx.toa_cfd_idx = TOA_CFD;
        mctx.valid       = true;

        return m;
    }

};

} // namespace measurements

} // namespace tfboost

#endif /* TFBOOST_MEASUREMENTS_CLEANMEASURER_H_ */
