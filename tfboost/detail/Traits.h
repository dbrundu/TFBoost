/*----------------------------------------------------------------------------
 *
 *   Copyright (C) 2020 Davide Brundu, Gianmatteo Cossu
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
 * 
 *
 *  Created on: 07/05/2020
 *      Author: Davide Brundu
 */

#ifndef TRAITS_TFBOOST_H_
#define TRAITS_TFBOOST_H_


namespace tfboost {

namespace detail {

// https://stackoverflow.com/questions/28253399/check-traits-for-all-variadic-template-arguments
template <bool...> struct bool_pack;

template <bool... v>
using all_true = std::is_same<bool_pack<true, v...>, bool_pack<v..., true>>;

}
}
#endif /* TRAITS_TFBOOST_H_ */
