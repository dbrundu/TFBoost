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
 *  NoiseFromFileModule.h
 *
 *  Created on: 2026
 *      Author: Davide Brundu
 */

#ifndef TFBOOST_TRANSFORMS_NOISEFROMFILEMODULE_H_
#define TFBOOST_TRANSFORMS_NOISEFROMFILEMODULE_H_

#include <tfboost/Utils.h>
#include <tfboost/core/Signal.h>
#include <tfboost/transforms/ISignalTransform.h>
#include <tfboost/ConfigParser.h>
#include <tfboost/core/Noise.h>
#include <tfboost/InputOutput.h>


namespace tfboost {

namespace transforms {

/*
 *  @class NoiseFromFileModule
 *
 *  Adds, in place, a noise sample read from a file to the signal. The module
 *  owns the list of noise files in the configured noise directory and consumes
 *  one file per call (in directory order), mirroring the historical inline
 *  behaviour.
 */
class NoiseFromFileModule : public ISignalTransform {

public:

    NoiseFromFileModule() = delete;

    explicit NoiseFromFileModule(ConfigParser const& c)
        : fC(c), fFiles( tfboost::GetFileList(c.NoiseDirectory) ), fIter(fFiles)
    {}

    const char* name() const override { return "NoiseFromFile"; }

    void apply(core::Signal& sig, TransformContext& /*ctx*/) const override
    {
        TSystemFile* currentnoisefile = (TSystemFile*) fIter.Next();
        TString      currentnoisefilename = currentnoisefile->GetName();

        HostSignal_t noise_h;
        tfboost::ReadSimple( fC.NoiseDirectory+currentnoisefilename, 0, noise_h, sig.size(), 1e-3);
        tfboost::core::add_noise_samples( sig.amplitude(), noise_h, fC );
    }

private:

    ConfigParser const& fC;
    TList*              fFiles;
    mutable TIter       fIter;   // advances once per apply()

};

} // namespace transforms

} // namespace tfboost

#endif /* TFBOOST_TRANSFORMS_NOISEFROMFILEMODULE_H_ */
