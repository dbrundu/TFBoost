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
 *  NoiseModule.h
 *
 *  Created on: 29/06/2026
 *      Author: Davide Brundu
 */

#ifndef TFBOOST_TRANSFORMS_NOISEMODULE_H_
#define TFBOOST_TRANSFORMS_NOISEMODULE_H_

#include <tfboost/core/Signal.h>
#include <tfboost/transforms/ISignalTransform.h>
#include <tfboost/ConfigParser.h>
#include <tfboost/core/Noise.h>


namespace tfboost {

namespace transforms {

/*
 *  @class NoiseModule
 *
 *  Adds simulated (white or red) noise to a Signal in place. The noise
 *  parameters (sigma, red-noise correlation) come from the configuration; the
 *  per-event seed is drawn from the shared SeedRNG carried in the context, so
 *  the random sequence is identical to the historical inline code.
 */
class NoiseModule : public ISignalTransform {

public:

    NoiseModule() = delete;

    explicit NoiseModule(ConfigParser const& c) : fC(c) {}

    const char* name() const override { return "Noise"; }

    void apply(core::Signal& sig, TransformContext& ctx) const override
    {
        tfboost::core::add_simulated_noise( sig.amplitude(),
                                            fC.sigma_noise, fC.UseRedNoise, fC.r_rednoise,
                                            ctx.seed_rng(), fC );
    }

private:

    ConfigParser const& fC;

};

} // namespace transforms

} // namespace tfboost

#endif /* TFBOOST_TRANSFORMS_NOISEMODULE_H_ */
