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
 *  TimeDigitizerModule.h
 *
 *  Created on: 29/06/2026
 *      Author: Davide Brundu
 */

#ifndef TFBOOST_TRANSFORMS_TIMEDIGITIZERMODULE_H_
#define TFBOOST_TRANSFORMS_TIMEDIGITIZERMODULE_H_

#include <tfboost/core/Signal.h>
#include <tfboost/transforms/ISignalTransform.h>
#include <tfboost/core/Digitization.h>


namespace tfboost {

namespace transforms {

/*
 *  @class TimeDigitizerModule
 *
 *  Re-samples a Signal at the digitizer (TDC) sampling step, optionally with a
 *  random clock phase. The Signal is resized in place and its dT updated to the
 *  new sampling step. The random phase is drawn from the engine in the context.
 */
class TimeDigitizerModule : public ISignalTransform {

public:

    TimeDigitizerModule() = delete;

    TimeDigitizerModule(double sampling_dT, double Tmax, bool random_phase)
        : fSamplingDT(sampling_dT), fTmax(Tmax), fRandomPhase(random_phase)
    {}

    const char* name() const override { return "TimeDigitizer"; }

    void apply(core::Signal& sig, TransformContext& ctx) const override
    {
        tfboost::core::time_digitize( sig.amplitude(), sig.time(),
                                     fSamplingDT, fTmax, ctx.engine, fRandomPhase );
        sig.setDT(fSamplingDT);
    }

private:

    double fSamplingDT;
    double fTmax;
    bool   fRandomPhase;

};

} // namespace transforms

} // namespace tfboost

#endif /* TFBOOST_TRANSFORMS_TIMEDIGITIZERMODULE_H_ */
