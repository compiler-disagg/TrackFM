source setup.sh <USERNAME> <MEMORY-SERVER-IP (eno49)>


Install fastswap OS on bothe nodes compute and memory server

sudo chmod 777 /home
cd /home
git clone https://github.com/torvalds/linux.git
cd linux
git checkout v5.0
git apply ../fastswap/kernel/kernel-v5.0.patch
cp ../fastswap/kernel/config-5.0.0-fastswap .config
make -j10





wget "http://content.mellanox.com/ofed/MLNX_OFED-5.1-0.6.6.0/MLNX_OFED_LINUX-5.1-0.6.6.0-ubuntu18.04-x86_64.tgz"
tar xvf MLNX_OFED_LINUX-5.1-0.6.6.0-ubuntu18.04-x86_64.tgz
cd MLNX_OFED_LINUX-5.1-0.6.6.0-ubuntu18.04-x86_64
sudo ./mlnxofedinstall --force
source /home/TrackFM/runtime/compiler_passes/passes/set_paths
