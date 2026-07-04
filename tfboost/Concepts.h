/*----------------------------------------------------------------------------
 *
 *   Copyright (C) 2020 Davide Brundu, Gian Matteo Cossu
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
 *  Concepts.h
 *
 *  Created on: 04/07/2026
 *      Author: Davide Brundu
 */

#ifndef TFBOOST_CONCEPTS_H_
#define TFBOOST_CONCEPTS_H_

#include <concepts>
#include <cstddef>

#include <tfboost/Types.h>

namespace tfboost {

/*
 *  @concept SignalContainer
 *
 *  A random-access container of sampled values (a waveform)
 */
template<class C>
concept SignalContainer = requires(C& c, std::size_t i) {
    { c.size()  } -> std::convertible_to<std::size_t>;
    { c.begin() };
    { c.end()   };
    { c[i]      } -> std::convertible_to<double>;
};

/*
 *  @concept RandomEngine
 *
 *  An invocable random-number engine
 */
template<class G>
concept RandomEngine = requires(G& g) {
    { g() } -> std::integral;
};


/*
 *  @concept Spline
 *
 *  A continuous re-sampleable signal
 */
template<class S>
concept Spline = requires(S const& s, double t) {
    { s(t) } -> std::convertible_to<double>;
};


/*
 *  @concept TransferFunction
 *
 *  An analytic transfer-function kernel
 */
template<class K>
concept TransferFunction = requires(K const& k, double t) {
    { k(t) } -> std::convertible_to<double>;
};


// The canonical signal buffer models the concept (self-check).
static_assert(SignalContainer<HostSignal_t>,
              "HostSignal_t must model tfboost::SignalContainer");

} // namespace tfboost

#endif /* TFBOOST_CONCEPTS_H_ */
