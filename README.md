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

TODO: cloudlab github repo

All experiments in our paper were run on [CloudLab](https://www.cloudlab.us/).
Our CloudLab profile is located in the root directory of this repo (`cloudlab.profile`).
To run TrackFM on CloudLab, you can instantiate this profile.
You'll first need to create a CloudLab account if you don't have one already.

TODO: how do they instantiate the profile in CloudLab?

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

TODO: why is this separate from [libraries and tools](#install-libraries-and-tools)?

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

```bash
sudo apt-get update
sudo apt-get -y --fix-broken install
sudo apt-get install -y libnuma-dev libmnl-dev libnl-3-dev libnl-route-3-dev libcrypto++-dev libcrypto++-doc libcrypto++-utils software-properties-common gcc-9 g++-9 python-pip python3-pip libjpeg-dev zlib1g-dev libevent-dev
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get -y purge cmake
```

TODO: why do these pip packages need to be sudo? why not `pip install -U ...`?
TODO: why aren't the `pip` invocations in the [python toolchain](#python-toolchain) section below?

### Building TrackFM

```bash
cd /home/TrackFM/runtime
./build.sh
```

### Setup Shenango

Each time node reboots, this script  has to run 
before you use TrackFM.
TODO: why?
```bash
sudo /home/TrackFM/runtime/AIFM/shenango/scripts/setup_machine.sh
```

## Update ssh config on compute node 
update ```MEM_SERVER_SSH_IP``` in ```/home/TrackFM/runtime/AIFM/aifm/configs/ssh``` with memory server IP.

## Make sure compute node can ssh to memory server without password
add compute node public key to memory server

## Compiler Toolchain only required on compute node

```
./install-dependencies.sh
```

## Python toolchain

```bash
pip3 install -r requirements.txt
```

TODO: I'd use specific version numbers in `requirements.txt`

### Compile TrackFM Passes only on compute node

```bash
cd /home/TrackFM/runtime/compiler_passes/passes
source set_paths (if LLVM, noelle not installed in /home update this file with the correct paths)
make -j
```

## Testing
TODO: how do they know they did the above correct? Add/use a simple benchmark or test, with a `Makefile` target that
they can run to test that their TrackFM installation is working

## Reproducing Paper Results
Each fig num in the paper has a fig directory in ``` exp/ ```. In order to reproduce the results one has to invoke the ```run.sh``` located in the respective fig directory. For example to 
reproduce fig 14a in the paper the following steps need 
to be followed. 
```
exp
   fig14a
         TrackFM
                run.sh
         Fastswap
                run.sh

cd  exp/fig14a
//reproduce TrackFM datapoints and generates plots
cd TrackFM
./run.sh
//reproduce Fastswap datapoints and generates plots
cd  fastswap
./run.sh
```

TODO: use `tree` command to make above prettier

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
