## TFBoost
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

TFboost is a C++14 compliant application, highly based on [HYDRA v.3](https://github.com/MultithreadCorner/Hydra), to perform the convolution between a signal (a voltage or current signal from a sensor for example), to be read from files, and a Transfer Function of a readout electronics. The transfer function can be read from a file as a set of sampling points, or can be chosen from a set of analytical functions provided by the library and fully configurable. The application performs also a set of measurements on the output signal such as: arrival time (with leading edge and constant fraction discrimination), maximum value of output signal, slope and voltange on threshold, etc. Morover a simulated white or red noise can be added to the output signal. The possibility to perform the convolution can be turned off, and the set of measurments can thus be applied to real signals.


## Dependencies
TFboost depends on [HYDRA >= v.3.2.1](https://github.com/MultithreadCorner/Hydra), [ROOT >= v.6.14](https://github.com/root-project/root), [libconfig >= v1.5](https://hyperrealm.github.io/libconfig/), [TCLAP >= v1.2.1](http://tclap.sourceforge.net/). For the best performances at least TBB or OMP backends are needed. Optionally  [CUDA >= 10.0](https://developer.nvidia.com/cuda-toolkit) is needed for nVidia GPUs. [GCC >= v.8] is needed.


## Installation and Run
The first step is checkout [HYDRA v.3](https://github.com/MultithreadCorner/Hydra). Please create a Hydra folder somewhere and clone the repository:
```bash
mkdir Hydra
cd Hydra
git clone https://github.com/MultithreadCorner/Hydra.git
```
Then you can clone the TFboost repository:
```bash
cd ..
mkdir TFboost
cd TFboost
git clone https://github.com/dbrundu/TFBoost.git
```

Then you can setup the corresponding enveironment variables and compile the TFboost application in the following way: starting from the TFBoost folder, please create a build directory for convenience:
```bash
mkdir build
cd build
export CC=/usr/bin/gcc-8
export CXX=/usr/bin/g++-8
export HYDRA_INCLUDE_DIR=<path-to-hydra>
cmake -DHYDRA_INCLUDE_DIR=$HYDRA_INCLUDE_DIR ../
make analysis_tbb
```

Please setup the configuration using the file `configuration.cgf` inside the `etc/` folder. Then you can run the application as:
```bash
./analysis_tbb
```

## Authors
TFboost was created by [Davide Brundu](https://github.com/dbrundu) and [Gian Matteo Cossu], within the TIMESPOT collaboration.

