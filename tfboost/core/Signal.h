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
 *  Signal.h
 *
 *  Created on: 29/06/2026
 *      Author: Davide Brundu
 */

#ifndef TFBOOST_CORE_SIGNAL_H_
#define TFBOOST_CORE_SIGNAL_H_

#include <utility>
#include <tfboost/Types.h>

namespace tfboost {

namespace core {

/*
 *  @class Signal
 *
 *  A sampled waveform: parallel time / amplitude arrays sharing a (nominal)
 *  sampling step dT. A Signal owns its own identity (number of samples and dT),
 *  so transforms that resample it -- e.g. time digitization -- update the Signal
 *  itself instead of mutating shared configuration state.
 *
 *  The amplitude() / time() accessors return mutable references so that the
 *  existing in-place transform helpers (convolution, noise, digitization, ...)
 *  can keep writing through them unchanged.
 */
class Signal {

public:

    Signal() = default;

    Signal(HostSignal_t time, HostSignal_t amplitude, double dT)
        : fTime(std::move(time)), fAmplitude(std::move(amplitude)), fDT(dT)
    {}

    Signal(Signal const&)            = default;
    Signal(Signal&&)                 = default;
    Signal& operator=(Signal const&) = default;
    Signal& operator=(Signal&&)      = default;

    // --- data access (mutable so in-place transforms can write through) ---
    inline HostSignal_t&       amplitude()       { return fAmplitude; }
    inline HostSignal_t const& amplitude() const { return fAmplitude; }
    inline HostSignal_t&       time()            { return fTime; }
    inline HostSignal_t const& time()      const { return fTime; }

    // --- identity ---
    inline size_t size() const { return fAmplitude.size(); }
    inline double dT()   const { return fDT; }
    inline void   setDT(double dT) { fDT = dT; }

    inline bool empty() const { return fAmplitude.empty(); }

    // cubic-spline interpolant over (time, amplitude); the Signal must outlive it
    inline auto spline() const { return hydra::make_spline<double>(fTime, fAmplitude); }

private:

    HostSignal_t fTime;
    HostSignal_t fAmplitude;
    double       fDT = 0.0;

};

} // namespace core

} // namespace tfboost

#endif /* TFBOOST_CORE_SIGNAL_H_ */
