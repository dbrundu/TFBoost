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
 *  ConvolutionModule.h
 *
 *  Created on: 2026
 *      Author: Davide Brundu
 */

#ifndef TFBOOST_TRANSFORMS_CONVOLUTIONMODULE_H_
#define TFBOOST_TRANSFORMS_CONVOLUTIONMODULE_H_

#include <libconfig.h++>

#include <tfboost/Types.h>
#include <tfboost/Utils.h>
#include <tfboost/core/Signal.h>
#include <tfboost/transforms/ISignalTransform.h>
#include <tfboost/core/Convolution.h>
#include <tfboost/functions/TIA_BJT_1stage.h>
#include <tfboost/functions/TIA_BJT_2stages.h>
#include <tfboost/functions/TIA_BJT_2stages_GM.h>
#include <tfboost/functions/TIA_IdealInt.h>
#include <tfboost/functions/TIA_MOS.h>


namespace tfboost {

namespace transforms {

/*
 *  @class ConvolutionModule
 *
 *  Convolves a Signal in place with the transfer function selected by the
 *  configuration. The transfer-function dispatch (the former switch on the
 *  configuration ID) and the device-side spline preparation are encapsulated
 *  here, so the analytic-kernel template types never leak into the caller, and
 *  the module plugs into the pipeline like any other ISignalTransform.
 *
 *  Selection (kept identical to the historical numbering):
 *    0 -> TIA_MOS,  1 -> TIA_BJT_2stages,  2 -> TIA_BJT_2stages_GM,
 *    3 -> TIA_BJT_1stage,  4 -> TIA_IdealInt,  5 -> tabulated TF from file.
 *
 *  For the analytic kernels (0-4) the signal is convolved as a device-resident
 *  spline; the tabulated transfer function (5) convolves the host spline of the
 *  signal with the host spline of the loaded TF samples.
 *
 *  If a (non-null) kernel histogram is given at construction it is filled once
 *  with the transfer-function shape -- the kernel is event-independent, so the
 *  diagnostic plot does not need to be produced inside the event loop.
 */
class ConvolutionModule : public ISignalTransform {

public:

    ConvolutionModule() = delete;

    ConvolutionModule(int ID,
                      libconfig::Setting const& cfg_tf,
                      HostSignal_t       const& time_tf,
                      HostSignal_t       const& current_tf,
                      TH1D*                     kernel_hist = nullptr)
        : fID(ID), fCfgTf(cfg_tf), fTimeTF(time_tf), fCurrentTF(current_tf)
    {
        if(kernel_hist) process(nullptr, kernel_hist);
    }

    const char* name() const override { return "Convolution"; }

    void apply(core::Signal& sig, TransformContext& /*ctx*/) const override
    {
        process(&sig, nullptr);
    }


private:

    /*
     *  Build the configured transfer-function kernel and, depending on the
     *  arguments, fill `kernelHist` with its shape and/or convolve `sig` with it.
     *  Passing sig==nullptr produces the kernel plot only (no convolution).
     */
    void process(core::Signal* sig, TH1D* kernelHist) const
    {
        if(fID == 5)
        {
            auto kernel = hydra::make_spline<double>(fTimeTF, fCurrentTF);
            if(kernelHist) tfboost::FillHistWithFunction( *kernelHist, kernel);
            if(sig)
            {
#if HYDRA_DEVICE_SYSTEM!=CUDA
                auto fft_backend = hydra::fft::fftw_f64;
#else
                auto fft_backend = hydra::fft::cufft_f64;
#endif
                const size_t N = sig->size();
                HostSignal_t out(N);
                auto signal = sig->spline();
                tfboost::core::convolve( fft_backend, kernel, signal, out, 0.0, (N-1)*sig->dT(), N );
                sig->amplitude() = std::move(out);
            }
            return;
        }

        switch(fID)
        {
            case 0: do_analytic( tfboost::TIA_MOS<double>( fCfgTf ),            sig, kernelHist); break;
            case 1: do_analytic( tfboost::TIA_BJT_2stages<double>( fCfgTf ),    sig, kernelHist); break;
            case 2: do_analytic( tfboost::TIA_BJT_2stages_GM<double>( fCfgTf ), sig, kernelHist); break;
            case 3: do_analytic( tfboost::TIA_BJT_1stage<double>( fCfgTf ),     sig, kernelHist); break;
            case 4: do_analytic( tfboost::TIA_IdealInt<double>( fCfgTf ),       sig, kernelHist); break;
            default:
                SAFE_EXIT( true , "In ConvolutionModule: bad transfer function ID.")
        }
    }

    /*
     *  Convolve `sig` (as a device-resident spline) with an analytic `kernel`,
     *  optionally filling the kernel-shape histogram first.
     */
    template<typename KERNEL>
    void do_analytic(KERNEL const& kernel, core::Signal* sig, TH1D* kernelHist) const
    {
        if(kernelHist) tfboost::FillHistWithFunction( *kernelHist, kernel);
        if(!sig) return;

#if HYDRA_DEVICE_SYSTEM!=CUDA
        auto fft_backend = hydra::fft::fftw_f64;
#else
        auto fft_backend = hydra::fft::cufft_f64;
#endif

        const size_t N = sig->size();

        DevSignal_t time_d(N);
        DevSignal_t amp_d(N);
        hydra::copy(sig->time(),      time_d);
        hydra::copy(sig->amplitude(), amp_d);
        auto signal_d = hydra::make_spline<double>(time_d, amp_d);

        HostSignal_t out(N);
        tfboost::core::convolve( fft_backend, kernel, signal_d, out, 0.0, (N-1)*sig->dT(), N );
        sig->amplitude() = std::move(out);
    }


    int                       fID;
    libconfig::Setting const& fCfgTf;
    HostSignal_t       const& fTimeTF;
    HostSignal_t       const& fCurrentTF;

};

} // namespace transforms

} // namespace tfboost

#endif /* TFBOOST_TRANSFORMS_CONVOLUTIONMODULE_H_ */
