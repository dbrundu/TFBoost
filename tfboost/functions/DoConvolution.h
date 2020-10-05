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
 *  Created on: 01/06/2020
 *      Author: Davide Brundu, Gianmatteo Cossu
 */


#ifndef DO_CONVOLUTION_H_
#define DO_CONVOLUTION_H_


#include <cmath>
#include <tuple>
#include <limits>
#include <stdexcept>
#include <assert.h>
#include <utility>
#include <ratio>


#include <hydra/detail/Config.h>
#include <hydra/detail/BackendPolicy.h>
#include <hydra/Types.h>
#include <hydra/Function.h>
#include <hydra/Pdf.h>
#include <hydra/Integrator.h>
#include <hydra/Tuple.h>
#include <hydra/functions/Utils.h>
#include <hydra/functions/Math.h>

#include <hydra/Convolution.h>
#include <hydra/functions/ConvolutionFunctor.h>


#include <tfboost/detail/external/hydra/DeConvolution.h>
#include <tfboost/detail/external/hydra/functions/DeConvolutionFunctor.h>


namespace tfboost {


template<typename KERNEL, typename FFT_BACKEND, typename SIGNAL>
hydra::host::vector<double> Do_Convolution(FFT_BACKEND fft_backend, 
                                           KERNEL const& kernel, 
                                           SIGNAL const& signal, 
                                           double const& min,
                                           double const& max,
                                           size_t const& N)
{
        hydra::host::vector<double >  conv_data_h(N);
       
        auto convolution = hydra::make_convolution<double>(  hydra::device::sys,  fft_backend, signal, kernel, min, max,  N, true, true );

        auto conv_data    = hydra::make_range(convolution.GetDeviceData(), convolution.GetDeviceData()+N);
        
        hydra::copy(conv_data, conv_data_h);
        
        convolution.Dispose();

        return conv_data_h;

}



template<typename CONVOL, typename FFT_BACKEND, typename SIGNAL>
hydra::host::vector<double> Do_DeConvolution(FFT_BACKEND fft_backend, 
                                           CONVOL const& conv, 
                                           SIGNAL const& signal, 
                                           double const& min,
                                           double const& max,
                                           size_t const& N)
{

        hydra::host::vector<double >  conv_data_h(N);
       
        auto deconvolution = hydra::make_deconvolution<double>(  hydra::device::sys,  fft_backend, signal, conv, min, max,  N, true, true );

        auto conv_data    = hydra::make_range(deconvolution.GetDeviceData(), deconvolution.GetDeviceData()+N);
        
        hydra::copy(conv_data, conv_data_h);
        
        deconvolution.Dispose();

        return conv_data_h;

}


}  // namespace tfboost


#endif /* DO_CONVOLUTION_H_ */


