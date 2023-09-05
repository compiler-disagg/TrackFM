[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# TrackFM

## Environment
### CloudLab

We run all experiments in /home, make sure you have the right privileges in /home 
```
sudo chmod 777 /home
```

## Toolchain Required to build TrackFM (on all nodes)

### Update package database and Linux kernel version.
```
sudo apt-get update
echo Y | sudo apt-get install linux-headers-5.0.0-20 linux-headers-5.0.0-20-generic linux-hwe-edge-tools-5.0.0-20 linux-image-5.0.0-20-generic linux-modules-5.0.0-20-generic linux-tools-5.0.0-20-generic
sudo reboot
```
### Set bash as the default shell.
```
chsh -s /bin/bash
```
### Install Mellanox OFED.
```
wget "http://content.mellanox.com/ofed/MLNX_OFED-4.6-1.0.1.1/MLNX_OFED_LINUX-4.6-1.0.1.1-ubuntu18.04-x86_64.tgz"
tar xvf MLNX_OFED_LINUX-4.6-1.0.1.1-ubuntu18.04-x86_64.tgz
cd MLNX_OFED_LINUX-4.6-1.0.1.1-ubuntu18.04-x86_64
sudo ./mlnxofedinstall --add-kernel-support --dpdk --upstream-libs # it's fine to see 'Failed to install libibverbs-dev DEB'
sudo /etc/init.d/openibd restart
```
### Install libraries and tools.
```
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
## Build TrackFM runtime (on all nodes)
```
cd /home/TrackFM/runtime
./build.sh
```
### Setup shenango
```
sudo /home/TrackFM/runtime/AIFM/shenango/scripts/setup_machine.sh
```
## Update ssh config on compute node 
update ```MEM_SERVER_SSH_IP``` in ```/home/TrackFM/runtime/AIFM/aifm/configs/ssh``` with memory server IP.

## Make sure compute node can ssh to memory server without password
add compute node public key to memory server

## Compiler ToolChain only required on compute node

### Install llvm 9.0.0
```
git clone https://github.com/llvm/llvm-project.git
cd llvm-project/ 
git checkout llvmorg-9.0.0
mkdir build
cd build
cmake -DLLVM_TARGETS_TO_BUILD=host -DLLVM_ENABLE_PROJECTS="clang;libcxx;libcxxabi;libunwind;compiler-rt"  ../llvm/
make -j8
```
#### Add LLVM installation to PATH

```
LLVM_HOME=/home/llvm-project/build
export PATH=$LLVM_HOME/bin:$PATH
export LD_LIBRARY_PATH=$LLVM_HOME/lib:$LD_LIBRARY_PATH
sudo ln -sf /home/llvm-project/build/bin/llvm-objcopy /usr/bin/objcopy
```

### Install noelle 9.8.0
```
git clone https://github.com/arcana-lab/noelle.git
cd noelle
git checkout v9.8.0
make
```
#### Add NOELLE to path
```
source /home/noelle/enable
export NOELLE_DIR=/home/noelle/
export CARM_DIR=/home/TrackFM/
```
## Python toolchain
```
sudo apt-get install python3-pip
sudo pip3 install pymemcache
sudo pip3 install scipy
sudo pip3 install matplotlib
sudo pip3 install jinja2
```
### Compile TrackFM Passes only on compute node
```
cd /home/TrackFM/runtime/compiler_passes/passes
source set_paths (if LLVM, noelle not installed in /home update this file with the correct paths)
make -j
```



