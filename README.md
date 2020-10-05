## TFBoost
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

<img src="logo.svg" width="700">

TFBoost is a C++14 compliant application, highly based on [HYDRA v.3](https://github.com/MultithreadCorner/Hydra), to perform the convolution between a signal (for example a voltage or current signal from a sensor) and a transfer function of a signal analyzer system (for example a readout electronics) in massively parallel platforms on Linux systems. The transfer function can be read from a file as a set of sampling points, or can be chosen from a set of analytical functions provided by the library and fully configurable. The available transfer functions are two Transimpedance (1 and 2 stages), a CSA and an ideal integrator, described as the input impedance of a CSA. The input signals are read from files as a set of sampling points. The application performs also a set of measurements on the convoluted output signals such as: arrival time, with leading edge and constant fraction discrimination, maximum value and its corresponding time, slope and voltange on discrimination thresholds, etc. Morover a simulated noise, white or red, can be added to the output signal. The computation of the convolution can be turned off, and the set of measurements can thus be applied to real signals.

Within the [TCoDe](https://github.com/MultithreadCorner/TCode) application, developed within the TIMESPOT collaboration to simulate the response of solid state sensors, TFBoost will be part of a complete software for simulation of signals in solid state detectors, from the generation in the sensor to the output of the readout electronics.


## Dependencies
TFBoost depends on [HYDRA >= v.3.2.1](https://github.com/MultithreadCorner/Hydra), [ROOT >= v.6.14](https://github.com/root-project/root), [libconfig >= v1.5](https://hyperrealm.github.io/libconfig/), [TCLAP >= v1.2.1](http://tclap.sourceforge.net/). For the best performances at least TBB or OMP backends are needed. Optionally  [CUDA >= 10.0](https://developer.nvidia.com/cuda-toolkit) is needed for nVidia GPUs. [GCC >= v.8](https://gcc.gnu.org/) is needed. 


## Installation, Build and Run
The first step is checkout [HYDRA v.3](https://github.com/MultithreadCorner/Hydra) and TFBoost:
```bash
mkdir <TFBoostDev>
cd <TFBoostDev>
git clone https://github.com/MultithreadCorner/Hydra.git Hydra
git clone https://github.com/dbrundu/TFBoost.git TFBoost
```

Then you can setup the corresponding enveironment variables:
```bash
export CC=/usr/bin/gcc-8
export CXX=/usr/bin/g++-8
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

## Build and Run the examples
An example of a simple analysis is available. The input files are obtained from an ideal silicon sensor and the files are in the `examples/input_files` directory. The configuration file named `config.cfg` is inside the `examples/` directory, while the rusults will be saved inside `<build>/results/`.  After the `cmake` command the example and can be run simply as:
```bash
make simple_analysis_tbb
./simple_analysis_tbb 
```

## Authors
TFBoost was created by [Davide Brundu](https://github.com/dbrundu) and Gian Matteo Cossu, within the TIMESPOT collaboration.

