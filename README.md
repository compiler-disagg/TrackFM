[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# TrackFM

TrackFM transparently compiles application code 
to run on systems with disaggregated memory. TrackFM 
builds on top of the [AIFM runtime](https://github.com/AIFM-sys/AIFM) to manage remote memory.
TrackFM uses compiler analysis and runtime optimizations
 to reach within 10-20% of 
state-of-the-art disaggregated solutions that require manual programmer effort. 

- [TrackFM](#trackfm)
  * [Paper](#paper)
  * [Experimental Environment](#experimental-environment)
  * [Build Instructions](#build-instructions)
  * [Testing](#testing)
  * [Reproducing Paper Results](#reproducing-paper-results)
  * [Using TrackFM](#using-trackfm)
  * [Code Structure](#code-structure)
  * [Acknowledgements](#acknowledgements)
  * [Contact](#contact)

## Paper
* [TrackFM: Far-out Compiler Support for a Far Memory World](#)<br>
Brian R. Tauro, Brian Suchy, Simone Campanoni, Peter Dinda, Kyle C. Hale<br>
The 29th ACM International Conference on Architectural Support for Programming Languages and Operating Systems (ASPLOS '24; to appear).

## Experimental Environment
### CloudLab

All experiments in our paper were run on [CloudLab](https://www.cloudlab.us/).
Our CloudLab profile is located in the root directory of this repo (`cloudlab.profile`).
To run TrackFM on CloudLab, you can instantiate this profile.
You'll first need to create a CloudLab account if you don't have one already.

We will run all experiments in `/home`, so make sure you have sufficient privileges in that directory:

```bash
sudo chmod 777 /home
```

TODO: describe the profile in case they want to do it manually. Namely, is it ubuntu? what version?

## Build Instructions

### Toolchain Setup

### Update package database and Linux kernel version.

```bash
sudo apt-get update
echo Y | sudo apt-get install linux-headers-5.0.0-20 linux-headers-5.0.0-20-generic linux-hwe-edge-tools-5.0.0-20 linux-image-5.0.0-20-generic linux-modules-5.0.0-20-generic linux-tools-5.0.0-20-generic
sudo reboot
```

### Set bash as the default shell.

TODO: why do we need to do this?

```bash
chsh -s /bin/bash
```
### Install Mellanox OFED.

TODO: why isn't this a script?

```bash
wget "http://content.mellanox.com/ofed/MLNX_OFED-4.6-1.0.1.1/MLNX_OFED_LINUX-4.6-1.0.1.1-ubuntu18.04-x86_64.tgz"
tar xvf MLNX_OFED_LINUX-4.6-1.0.1.1-ubuntu18.04-x86_64.tgz
cd MLNX_OFED_LINUX-4.6-1.0.1.1-ubuntu18.04-x86_64
sudo ./mlnxofedinstall --add-kernel-support --dpdk --upstream-libs # it's fine to see 'Failed to install libibverbs-dev DEB'
sudo /etc/init.d/openibd restart
```

### Install Libraries and Tools

TODO: why isn't this one apt-get invocation? Why not use `apt-get install -y`?
```bash
sudo apt-get update
echo Y | sudo apt-get --fix-broken install
echo Y | sudo apt-get install libnuma-dev libmnl-dev libnl-3-dev libnl-route-3-dev
echo Y | sudo apt-get install libcrypto++-dev libcrypto++-doc libcrypto++-utils
echo Y | sudo apt-get install software-properties-common
echo Y | sudo apt-get install gcc-9 g++-9 python-pip
echo Y | sudo add-apt-repository ppa:ubuntu-toolchain-r/test
echo Y | sudo apt-get purge cmake
echo Y | sudo apt-get install libjpeg-dev zlib1g-dev
echo Y | sudo apt-get install libevent-dev
sudo pip install --upgrade pip
sudo pip install cmake
sudo pip install wllvm
```
TODO: why do these pip packages need to be sudo? why not `pip install -U ...`?
TODO: why aren't the `pip` invocations in the [python toolchain](#python-toolchain) section below?

### Building TrackFM

```bash
cd /home/TrackFM/runtime
./build.sh
```

### Setup Shenango

TODO: why isn't this in the trackfm build script?
```bash
sudo /home/TrackFM/runtime/AIFM/shenango/scripts/setup_machine.sh
```

## Update ssh config on compute node 
update ```MEM_SERVER_SSH_IP``` in ```/home/TrackFM/runtime/AIFM/aifm/configs/ssh``` with memory server IP.

## Make sure compute node can ssh to memory server without password
add compute node public key to memory server

## Compiler Toolchain only required on compute node

### Install LLVM v9.0.0

TODO: script (install-dependencies.sh?)
```bash
git clone https://github.com/llvm/llvm-project.git
cd llvm-project/ 
git checkout llvmorg-9.0.0
mkdir build
cd build
cmake -DLLVM_TARGETS_TO_BUILD=host -DLLVM_ENABLE_PROJECTS="clang;libcxx;libcxxabi;libunwind;compiler-rt"  ../llvm/
make -j8
```
#### Add LLVM installation to PATH

```bash
LLVM_HOME=/home/llvm-project/build
export PATH=$LLVM_HOME/bin:$PATH
export LD_LIBRARY_PATH=$LLVM_HOME/lib:$LD_LIBRARY_PATH
sudo ln -sf /home/llvm-project/build/bin/llvm-objcopy /usr/bin/objcopy
```

### Install NOELLE v9.8.0

TODO: script (install-dependencies.sh?)
```bash
git clone https://github.com/arcana-lab/noelle.git
cd noelle
git checkout v9.8.0
make
```

#### Add NOELLE to PATH

```bash
source /home/noelle/enable
export NOELLE_DIR=/home/noelle/
export CARM_DIR=/home/TrackFM/
```

## Python toolchain

TODO: IIRC standard practice with `pip` is to create `requirements.txt` then have them run
`pip install -r requirements.txt`

```bash
sudo apt-get install python3-pip
sudo pip3 install pymemcache
sudo pip3 install scipy
sudo pip3 install matplotlib
sudo pip3 install jinja2
sudo pip3 install psutil
sudo pip3 install grpcio 
sudo pip3 install grpcio-tools 
```

### Compile TrackFM Passes only on compute node

```bash
cd /home/TrackFM/runtime/compiler_passes/passes
source set_paths (if LLVM, noelle not installed in /home update this file with the correct paths)
make -j
```

## Testing
TODO: how do they know they did the above correct?

## Reproducing Paper Results
TODO: How do they run the experiments and regenerate the plots in the paper?

## Using TrackFM
TODO: how do they get a new app running with TrackFM? What are the caveats?

## Code Structure
TODO: give them a map of the codebase so they can grok it quickly

## Acknowledgements
<img align="left" src="https://www.nsf.gov/images/logos/NSF_4-Color_bitmap_Logo.png" height=100/>
<img align="left" src="https://upload.wikimedia.org/wikipedia/commons/thumb/e/e2/Seal_of_the_United_States_Department_of_Energy.svg/768px-Seal_of_the_United_States_Department_of_Energy.svg.png" height=100/>
<img align="left" src="https://upload.wikimedia.org/wikipedia/commons/thumb/2/24/Samsung_Logo.svg/2560px-Samsung_Logo.svg.png" height=75/>
<br><br><br><br><br>


TrackFM was made possible with support from the United States [National Science
Foundation](https://nsf.gov) (NSF) via grants CCF-2028958, CNS-1763612,
CNS-2239757, CNS-1763743, CCF-2028851, CCF-2119069, CCF-2107042, CNS-2211315,
and CNS-2211508, the [US Department of Energy](https://www.energy.gov/) (DoE)
via the Exascale Computing Project (17-SC-20-SC) and with grant DE-SC0022268,
as well as with generous support from Samsung Semiconductor, Inc.

## Contact
The TrackFM codebase is currently maintained by Brian Tauro (btauro [at] hawk [dot] iit [dot] edu).
