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
 *  IMeasurement.h
 *
 *  Created on: 2026
 *      Author: Davide Brundu
 */

#ifndef TFBOOST_MEASUREMENTS_IMEASUREMENT_H_
#define TFBOOST_MEASUREMENTS_IMEASUREMENT_H_

#include <tfboost/Types.h>
#include <tfboost/ConfigParser.h>
#include <tfboost/core/Signal.h>


namespace tfboost {

namespace measurements {

/*
 *  @struct MeasureContext
 *
 *  Per-event inputs, cross-pass data and side outputs of a measurement.
 *  Measurements read a (const) Signal and return a Measures_t; everything that
 *  does not fit that single return value travels through this context:
 *   - inputs: the configuration, the sampling rate and the time-reference
 *     smearing TR_res drawn once per event;
 *   - cross-pass: the leading-edge / constant-fraction sample indices and the
 *     clean measures, written by the no-noise pass and read by the noisy pass
 *     (the fit guards and the electronic-jitter need them);
 *   - outputs: the empty-event flag, the noise RMS and the jitter values that
 *     the caller fills into histograms (compute and fill stay separate).
 */
struct MeasureContext {

    // inputs
    ConfigParser const& c;
    double SampRate;
    double TR_res;

    // result flag + cross-pass data
    bool       valid       = true;   // false => empty event, skip
    size_t     toa_le_idx  = 0;      // clean leading-edge index
    size_t     toa_cfd_idx = 0;      // clean constant-fraction index
    Measures_t clean_measures{};     // clean measures (for the noisy jitter)

    // side outputs of the noisy pass
    double rms_noise   = 0.0;
    bool   fill_jitter = false;
    double jitter_le   = 0.0;
    double jitter_cfd  = 0.0;
    double jitter_rm   = 0.0;
};


/*
 *  @class IMeasurement
 *
 *  Common interface for the measurement passes. A measurement reads a Signal
 *  and produces the array of timing/amplitude observables; it does not modify
 *  the signal and does not touch histograms (the caller fills those).
 */
class IMeasurement {

public:

    virtual ~IMeasurement() = default;

    virtual Measures_t  measure(core::Signal const& sig, MeasureContext& mctx) const = 0;
    virtual const char* name() const = 0;

    // print the measured observables to stdout (diagnostic)
    virtual void print(Measures_t const& m, MeasureContext const& mctx) const = 0;

};

} // namespace measurements

} // namespace tfboost

#endif /* TFBOOST_MEASUREMENTS_IMEASUREMENT_H_ */
