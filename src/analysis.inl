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
 *  analysis.inl
 *
 *  Thin entry point: the whole run is orchestrated by AnalysisApplication,
 *  which owns the resources and sequences the stages. See
 *  tfboost/app/AnalysisApplication.h and ARCHITECTURE.md.
 *
 *  Created on: 07/05/2020
 *      Author: Davide Brundu
 */

#ifndef TFBOOST_ANALYSIS_INL_
#define TFBOOST_ANALYSIS_INL_

#include <tfboost/app/AnalysisApplication.h>

int main(int /*argc*/, char** /*argv*/)
{
  return tfboost::app::AnalysisApplication{}
            .initialize()
            .read_configuration()
            .generate_pipeline()
            .run();
}

#endif
