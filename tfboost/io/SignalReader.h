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
 *  SignalReader.h
 *
 *  Created on: 2026
 *      Author: Davide Brundu
 */

#ifndef TFBOOST_IO_SIGNALREADER_H_
#define TFBOOST_IO_SIGNALREADER_H_

#include <fstream>
#include <cstdlib>
#include <utility>

#include <tfboost/Types.h>
#include <tfboost/Utils.h>
#include <tfboost/ConfigParser.h>
#include <tfboost/core/Signal.h>


namespace tfboost {

namespace io {

/*
 *  @class SignalReader
 *
 *  Reads one input waveform file into a Signal. The samples are laid out on a
 *  uniform time grid of step dT and length Nsamples: a leading `offset` of
 *  zeros, the values read from the configured column of the file (scaled by
 *  `scale_factor` and by a per-file Landau fluctuation, if enabled), and a
 *  trailing zero padding up to Nsamples.
 *
 *  The Landau draw consumes one value from the ROOT RNG passed to read(),
 *  preserving the historical random sequence (it happens once per file, before
 *  the per-event time-reference smearing drawn in the caller).
 */
class SignalReader {

public:

    SignalReader() = delete;

    explicit SignalReader(ConfigParser const& c) : fC(c) {}

    core::Signal read(TString const& filename, TRandom3& root_rng) const
    {
        HostSignal_t time;     time.reserve(fC.Nsamples);
        HostSignal_t current;  current.reserve(fC.Nsamples);

        // leading offset of zeros
        for(size_t k=0; k < fC.offset; ++k) {
            time.push_back(k * fC.dT);
            current.push_back(0.0); }

        std::ifstream myFile( (fC.InputDirectory+filename).Data() );
        SAFE_EXIT( !myFile.is_open() , "In SignalReader::read: input file cannot be opened.")

        TString line;

        // skip the requested number of header lines
        for(size_t j=0; j < fC.NlinesToSkip; ++j)
            line.ReadLine(myFile);

        // per-file Landau fluctuation factor (consumes one RNG draw)
        double landau = fC.LandauFluctuation?
                        root_rng.Landau(fC.landaufactor_mean, fC.landaufactor_sigma) : 1.0;

        double scale = fC.scale_factor;

        // read the samples from the configured column
        size_t s = fC.offset;
        for(size_t j = fC.offset; j < fC.Nsamples ; ++j)
        {
            line.ReadLine(myFile);
            if (!myFile.good()) break;

            TObjArray *tokens = line.Tokenize( fC.token.Data() );

            TString data_str  = ((TObjString*) tokens->At( fC.column ) )->GetString();

            double data = scale*landau*atof(data_str);

            time.push_back( s * fC.dT);
            current.push_back(data);

            tokens->Delete();
            delete tokens;

            ++s;
        }

        myFile.close();

        // trailing zero padding up to Nsamples
        for(size_t k=current.size(); k<fC.Nsamples; ++k) {
            time.push_back( k * fC.dT);
            current.push_back(0.0); }

        SAFE_EXIT( current.size() != fC.Nsamples ,
                   "In SignalReader::read: size of container not equal to Nsamples.")

        return core::Signal{ std::move(time), std::move(current), fC.dT };
    }

private:

    ConfigParser const& fC;

};

} // namespace io

} // namespace tfboost

#endif /* TFBOOST_IO_SIGNALREADER_H_ */
