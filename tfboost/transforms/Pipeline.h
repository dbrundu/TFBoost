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
 *  Pipeline.h
 *
 *  An ordered, runtime-assembled chain of signal transforms, plus factory
 *  functions that build the chain from the configuration.
 *
 *  Created on: 2026
 *      Author: Davide Brundu
 */

#ifndef TFBOOST_TRANSFORMS_PIPELINE_H_
#define TFBOOST_TRANSFORMS_PIPELINE_H_

#include <vector>
#include <memory>

#include <tfboost/ConfigParser.h>
#include <tfboost/core/Signal.h>
#include <tfboost/transforms/ISignalTransform.h>
#include <tfboost/transforms/NoiseModule.h>
#include <tfboost/transforms/FilterModule.h>
#include <tfboost/transforms/TimeDigitizerModule.h>
#include <tfboost/transforms/VoltageDigitizerModule.h>


namespace tfboost {

namespace transforms {

/*
 *  @class Pipeline
 *
 *  An ordered list of signal transforms applied in sequence to a Signal.
 *  Transforms are owned by the pipeline (type-erased through ISignalTransform),
 *  so a chain selected at run time from the configuration is just a vector of
 *  modules with a single apply() entry point.
 */
class Pipeline {

public:

    Pipeline() = default;

    void add(std::unique_ptr<ISignalTransform> transform)
    {
        if(transform) fTransforms.push_back(std::move(transform));
    }

    void apply(core::Signal& sig, TransformContext& ctx) const
    {
        for(auto const& t : fTransforms) t->apply(sig, ctx);
    }

    bool        empty() const { return fTransforms.empty(); }
    std::size_t size()  const { return fTransforms.size();  }

private:

    std::vector<std::unique_ptr<ISignalTransform>> fTransforms;

};



/*
 *  Build the chain of transforms applied (after the convolution) to produce the
 *  signal on which the no-noise measurements are run. It is empty when the run
 *  requests measurements with noise (then the clean measurements are taken on
 *  the bare convolution output). The digitization steps are applied only when a
 *  convolution was actually performed, mirroring the historical behaviour.
 */
inline Pipeline BuildConditioningPipeline(ConfigParser const& c, double Tmax, FilterKind filter_kind)
{
    Pipeline p;

    if(c.DoMeasurementsWithNoise) return p;

    if(c.LowPassFilter)
        p.add( std::make_unique<FilterModule>(c, filter_kind) );

    if(c.MakeConvolution)
    {
        if(c.MakeTimeDigitization)
            p.add( std::make_unique<TimeDigitizerModule>(c.sampling_dT, Tmax, c.randomphase) );

        if(c.MakeVoltageDigitization)
            p.add( std::make_unique<VoltageDigitizerModule>(c.ADCmin, c.ADCmax, c.ADCnbits) );
    }

    return p;
}



/*
 *  Build the chain of transforms producing the noisy signal on which the
 *  noise measurements are run: simulated noise, an optional oscilloscope
 *  low-pass filter, and the time/voltage digitization.
 *
 *  (Reading noise from files is data-driven and stays in the caller.)
 */
inline Pipeline BuildNoisePipeline(ConfigParser const& c, double Tmax, FilterKind filter_kind)
{
    Pipeline p;

    if(c.AddSimulatedNoise)
        p.add( std::make_unique<NoiseModule>(c) );

    if(c.LowPassFilter && !c.FilterOnlyNoise)
        p.add( std::make_unique<FilterModule>(c, filter_kind) );

    if(c.MakeTimeDigitization)
        p.add( std::make_unique<TimeDigitizerModule>(c.sampling_dT, Tmax, c.randomphase) );

    if(c.MakeVoltageDigitization)
        p.add( std::make_unique<VoltageDigitizerModule>(c.ADCmin, c.ADCmax, c.ADCnbits) );

    return p;
}


} // namespace transforms

} // namespace tfboost

#endif /* TFBOOST_TRANSFORMS_PIPELINE_H_ */
