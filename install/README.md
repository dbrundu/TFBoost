## Dependencies and TFBoost installation
TFBoost depends on [HYDRA >= v.3.2.1](https://github.com/MultithreadCorner/Hydra), [ROOT >= v.6.14](https://github.com/root-project/root), [libconfig >= v1.5](https://hyperrealm.github.io/libconfig/), [TCLAP >= v1.2.1](http://tclap.sourceforge.net/). For the best performances at least TBB or OMP backends are needed. Optionally  [CUDA >= 10.0](https://developer.nvidia.com/cuda-toolkit) is needed for nVidia GPUs. [GCC >= v.8](https://gcc.gnu.org/) is needed. 
If you want to install ROOT and other dependencies first, do the following first steps, otherwise you can skip them and install directly Hydra and TFBoost: 


1. Download all the scripts in this folder where you want to install the application

2. If you need to install ROOT open a terminal and run the following command, otherwise skip this step. A local version of ROOT CERN will be installed in the current folder and the script automatically add a `source .../thisroot.sh` line to your `.bashrc` file, in order to run ROOT just by typing `root` in a terminal.
```bash
bash installRoot
```

3. If you need to install the other dependencies (gcc8, libconfig, TCLAP, TBB etc.), run the following command, otherwise skip this step.
```bash
bash packages
```

4. Finally to install Hydra and TFBoost run the following commands:
```bash
bash bash TFBoost_installer
```
or, if you are using Ubuntu with Windows Linux Subsystem, the following one:
```bash
bash bash TFBoost_installer_WSL
```
After TFBoost is configured and built, you can run it 
with the GUI typing in a terminal: 
```bash
TFB
```
You can launch also the C++ application directly from terminal: you have to setup first the proper configuration using the file `configuration.cgf` inside the `etc/` folder, then build and run the application as:
```bash
make analysis_tbb
./analysis_tbb 
```

