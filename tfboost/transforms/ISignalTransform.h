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
 *  ISignalTransform.h
 *
 *  Created on: 29/06/2026
 *      Author: Davide Brundu
 */

#ifndef TFBOOST_TRANSFORMS_ISIGNALTRANSFORM_H_
#define TFBOOST_TRANSFORMS_ISIGNALTRANSFORM_H_

#include <tfboost/core/Signal.h>


namespace tfboost {

namespace transforms {

/*
 *  @struct TransformContext
 *
 *  Per-run resources shared by the signal transforms that need them
 *  (e.g. the random engines used by noise injection and random-phase
 *  time digitization). Passing them through a context keeps the
 *  ISignalTransform::apply() signature uniform across all modules.
 */
struct TransformContext {
    hydra::default_random_engine& engine;   // device-side RNG (random phase)
    TRandom3&                     root_rng; // ROOT RNG (Landau, time-ref smearing)
    hydra::SeedRNG&               seed_rng; // seed source (noise injection)
};


/*
 *  @class ISignalTransform
 *
 *  Common interface for every signal-transformation module
 *  (convolution, noise, filtering, digitization, ...). A transform
 *  mutates a Signal in place. Modules are meant to be instantiated at
 *  run time from the configuration and chained into a pipeline.
 */
class ISignalTransform {

public:

    virtual ~ISignalTransform() = default;

    // Apply the transform to `sig` in place.
    virtual void apply(core::Signal& sig, TransformContext& ctx) const = 0;

    // Human-readable name, for logging / diagnostics.
    virtual const char* name() const = 0;

};

} // namespace transforms

} // namespace tfboost

#endif /* TFBOOST_TRANSFORMS_ISIGNALTRANSFORM_H_ */
