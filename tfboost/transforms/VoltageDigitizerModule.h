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
 *  VoltageDigitizerModule.h
 *
 *  Created on: 29/06/2026
 *      Author: Davide Brundu
 */

#ifndef TFBOOST_TRANSFORMS_VOLTAGEDIGITIZERMODULE_H_
#define TFBOOST_TRANSFORMS_VOLTAGEDIGITIZERMODULE_H_

#include <tfboost/core/Signal.h>
#include <tfboost/transforms/ISignalTransform.h>
#include <tfboost/core/Digitization.h>


namespace tfboost {

namespace transforms {

/*
 *  @class VoltageDigitizerModule
 *
 *  Quantizes a Signal's amplitude in place to the ADC dynamic range
 *  ([ADCmin, ADCmax]) with the configured number of bits.
 */
class VoltageDigitizerModule : public ISignalTransform {

public:

    VoltageDigitizerModule() = delete;

    VoltageDigitizerModule(double adc_min, double adc_max, int adc_nbits)
        : fADCmin(adc_min), fADCmax(adc_max), fADCnbits(adc_nbits)
    {}

    const char* name() const override { return "VoltageDigitizer"; }

    void apply(core::Signal& sig, TransformContext& /*ctx*/) const override
    {
        tfboost::core::voltage_digitize( sig.amplitude(), fADCmin, fADCmax, fADCnbits );
    }

private:

    double fADCmin;
    double fADCmax;
    int    fADCnbits;

};

} // namespace transforms

} // namespace tfboost

#endif /* TFBOOST_TRANSFORMS_VOLTAGEDIGITIZERMODULE_H_ */
