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
 *  FilterModule.h
 *
 *  Created on: 29/06/2026
 *      Author: Davide Brundu
 */

#ifndef TFBOOST_TRANSFORMS_FILTERMODULE_H_
#define TFBOOST_TRANSFORMS_FILTERMODULE_H_

#include <tfboost/core/Signal.h>
#include <tfboost/transforms/ISignalTransform.h>
#include <tfboost/ConfigParser.h>
#include <tfboost/core/Convolution.h>
#include <tfboost/functions/RCFilter.h>
#include <tfboost/functions/ButterworthFilter.h>


namespace tfboost {

namespace transforms {

enum class FilterKind { RC, Butterworth };

/*
 *  @class FilterModule
 *
 *  Low-pass filters a Signal in place by convolving it with an RC or
 *  Butterworth filter response (cut-off frequency and order taken from the
 *  configuration). The filter is evaluated at the signal's current sampling
 *  step, so it behaves correctly whether applied before or after digitization.
 */
class FilterModule : public ISignalTransform {

public:

    FilterModule() = delete;

    FilterModule(ConfigParser const& c, FilterKind kind)
        : fC(c), fKind(kind)
    {}

    const char* name() const override
    {
        return fKind==FilterKind::RC ? "RCFilter" : "ButterworthFilter";
    }

    void apply(core::Signal& sig, TransformContext& /*ctx*/) const override
    {
#if HYDRA_DEVICE_SYSTEM!=CUDA
        auto fft_backend = hydra::fft::fftw_f64;
#else
        auto fft_backend = hydra::fft::cufft_f64;
#endif

        const size_t N   = sig.size();
        const double min = 0.0;
        const double max = (N-1) * sig.dT();

        auto signal = sig.spline();

        if(fKind == FilterKind::RC)
        {
            auto flt = tfboost::RCFilter<double>( fC.LowPassFrequency, fC.LowPassOrder, sig.dT());
            tfboost::core::convolve(fft_backend, flt, signal, sig.amplitude(), min, max, N);
        }
        else
        {
            auto flt = tfboost::ButterworthFilter<double>( fC.LowPassFrequency, fC.LowPassOrder, sig.dT());
            tfboost::core::convolve(fft_backend, flt, signal, sig.amplitude(), min, max, N);
        }
    }

private:

    ConfigParser const& fC;
    FilterKind          fKind;

};

} // namespace transforms

} // namespace tfboost

#endif /* TFBOOST_TRANSFORMS_FILTERMODULE_H_ */
