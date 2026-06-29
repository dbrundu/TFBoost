## TFBoost
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
![Version](https://img.shields.io/badge/version-v0.3--alpha-yellowgreen)
![Issues](https://img.shields.io/github/issues/dbrundu/TFBoost)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.4265385.svg)](https://doi.org/10.5281/zenodo.4265385)



<img src="logo.svg" width="500">

TFBoost is a C++14 compliant application, highly based on [HYDRA v.3](https://github.com/MultithreadCorner/Hydra) and mainly with a functional design, to perform the convolution between a signal (for example a voltage or current signal from a sensor) and a transfer function of a signal analyzer system (for example a readout electronics) in massively parallel platforms on Linux systems. The transfer function can be read from a file as a set of sampling points, or can be chosen from a set of analytical functions provided by the library and fully configurable. 
The available transfer functions are: 
- Two Transimpedance, with 1 and 2 stages,  
- CSA, 
- Ideal integrator, described as the input impedance of a CSA, 
- Order-n Butterworth filter,
- RC filter.

The input signals are read from files as a set of sampling points. The application performs also a set of measurements on the convoluted output signals such as: arrival time, with leading edge and constant fraction discrimination, maximum value and its corresponding time, slope and voltage on discrimination thresholds, etc. Morover a simulated noise, white or red, can be added to the output signal. The computation of the convolution can be turned off, and the set of measurements can thus be applied to real signals.

TFBoost has also a Graphical User Interface written in Python3:

<img src="gui.png" width="600">
<img src="gui2.png" width="675">

Within the [TCoDe](https://github.com/MultithreadCorner/TCode) application, developed within the TIMESPOT collaboration to simulate the response of solid state sensors, TFBoost will be part of a complete software for simulation of signals in solid state detectors, from the generation in the sensor to the output of the readout electronics.


## Dependencies
TFBoost depends on [HYDRA >= v.3.2.1](https://github.com/MultithreadCorner/Hydra), [ROOT >= v.6.14](https://github.com/root-project/root), [libconfig >= v1.5](https://hyperrealm.github.io/libconfig/) (C++ bindings), [TCLAP >= v1.2.1](http://tclap.sourceforge.net/) and [FFTW3](http://www.fftw.org/). For the best performances at least TBB or OMP backends are needed. Optionally [CUDA >= 10.0](https://developer.nvidia.com/cuda-toolkit) is needed for nVidia GPUs. A C++ compiler with C++14 support ([GCC >= v.8](https://gcc.gnu.org/), Clang or ICC) and [CMake >= v.3.24](https://cmake.org/) are needed.

Two of these dependencies are header-only / source distributions and are *not* installed through the system package manager:

- **HYDRA** is header-only: just clone it (see [Installation, Build and Run](#installation-build-and-run)) and point `HYDRA_INCLUDE_DIR` to it. No build or install step is required.
- **ROOT** must be installed separately (from the [official binaries/packages](https://root.cern/install/) or built from source) and its environment sourced so that `root-config` is on the `PATH` and `ROOTSYS` is set:
  ```bash
  source <path-to-root>/bin/thisroot.sh
  ```

### Installing the system packages

The remaining dependencies (libconfig C++ bindings, TCLAP, FFTW3, the TBB/OpenMP backends and the build toolchain) are available from the system package manager. **For libconfig and TBB make sure to install the development (`-devel` / `-dev`) packages**: the runtime shared libraries alone are not enough, as CMake needs the headers (e.g. `libconfig.h++`) to configure the build.

On **Fedora / RHEL / CentOS** (`dnf`):
```bash
sudo dnf install gcc-c++ cmake libconfig-devel tclap tbb-devel fftw-devel
```

On **Debian / Ubuntu** (`apt`):
```bash
sudo apt install g++ cmake libconfig++-dev libtclap-dev libtbb-dev libfftw3-dev
```

OpenMP support ships with GCC/Clang, so no extra package is needed for the OMP backend. The CUDA backend is built only when the CUDA toolkit is detected; it can be installed separately following the [NVIDIA instructions](https://developer.nvidia.com/cuda-downloads).

> **Note on recent TBB (oneTBB):** modern distributions ship oneTBB (>= 2021), which still works but no longer provides the legacy `tbb/tbb_stddef.h` header. The bundled `cmake/FindTBB.cmake` already handles this; if you use an older/custom TBB finder and configuration fails on a missing `tbb_stddef.h`, this is the cause.


## Disclaimer
TFBoost is currently an `alpha` version and is under development. Please report any problems throught GitHub Issues if necessary. The current design is in a preliminary stage, the final design will be composed of semi-independent modules and algorithms that can be instantiated and chained in run time, depending on the configuration provided by the user.


## Installation, Build and Run
The first step is checkout [HYDRA v.3](https://github.com/MultithreadCorner/Hydra) and TFBoost:
```bash
mkdir <TFBoostDev>
cd <TFBoostDev>
git clone https://github.com/MultithreadCorner/Hydra.git Hydra
git clone https://github.com/gianmatteocossu/TFBoost.git TFBoost
```

Then you have to setup the proper enveironment variables:
```bash
export CC=/usr/bin/gcc-8
export CXX=/usr/bin/g++-8
export ROOTSYS=<path-to-root-build>
export HYDRA_INCLUDE_DIR=<path-to-hydra>
...
```

Starting from the TFBoost folder, please create a `build` directory for convenience and run the cmake command:
```bash
cd TFBoost
mkdir build
cd build
cmake -DHYDRA_INCLUDE_DIR=$HYDRA_INCLUDE_DIR ../
```

At this point you can run the examples: please read at the examples section for a description on how to setup and run them. If you want to run the actual application please setup the proper configuration using the file `configuration.cgf` inside the `etc/` folder, then build and run the application as:
```bash
make analysis_tbb
./analysis_tbb 
```
to build all the applications needed for the GUI:
```bash
make analysis_tbb deconvolution_tbb resampling_tbb 3Ddiamond_tbb
```
to run TFBoost using the GUI, open a terminal in the folder 'TFBoost GUI' and type:
```bash
python3 TFBoostGui.py
```

## Installation under Ubuntu Linux
To install TFBoost on Ubuntu the following scripts can be used:
[TFBoost install](https://downgit.github.io/#/home?url=https://github.com/dbrundu/TFBoost/tree/master/install)

Follow the instruction in file README.md.<br />
After everything is set, the GUI will open simply typing in a terminal:
```bash
TFB
```



## Build and Run the examples
An example of a simple analysis is available. The input files are obtained from an ideal silicon sensor and the files are in the `examples/input_files` directory. The configuration file named `config.cfg` is inside the `examples/` directory, while the rusults will be saved inside `<build>/results/`.  After the `cmake` command the example and can be run simply as:
```bash
make simple_analysis_tbb
./simple_analysis_tbb 
```

## Authors
TFBoost was created by [Davide Brundu](https://github.com/dbrundu) and [Gian Matteo Cossu](https://github.com/gianmatteocossu/TFBoost), within the TIMESPOT collaboration.


## Reporting and Contributing
There are different ways to report bugs, problems or to contribute in general. We prefer that any problem or possible bug is reported through GitHub issues. If you want to contribute to solve a bug or improve the code please open a Pull Request. For general questions on how to proper use the code, or any doubts, please contact directly the authors.

