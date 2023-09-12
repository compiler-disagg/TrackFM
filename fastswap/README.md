# Install fastswap

**Each time the server restarts this script has to be rerun.**

```
#fastswap environment will be set at /home
source setup.sh <USERNAME> <MEMORY-SERVER-IP (eno49)>
```

Install fastswap OS on both compute and memory server

```
sudo chmod 777 /home
cd /home
sudo apt-get update
sudo apt-get install git build-essential kernel-package fakeroot libncurses5-dev libssl-dev ccache bison flex
git clone https://github.com/torvalds/linux.git
cd linux
git checkout v5.0
git apply ../fastswap/kernel/kernel-v5.0.patch
cp ../fastswap/kernel/config-5.0.0-fastswap .config
make -j 10 deb-pkg LOCALVERSION=-fastswap
cd ..
sudo dpkg -i *.deb
```

# setting up cgroups

```
    Open /boot/grub/grub.cfg in your editor of choice
    Find the menuentry for the fastswap kernel
    Add cgroup_no_v1=memory to the end of the line beginning in linux ```boot/vmlinuz-5.0.0-fastswap```
    Save and exit the file
    Run: sudo update-grub
    Reboot

```

# Reinstall OFED for fastswap

```
wget "http://content.mellanox.com/ofed/MLNX_OFED-5.1-0.6.6.0/MLNX_OFED_LINUX-5.1-0.6.6.0-ubuntu18.04-x86_64.tgz"
tar xvf MLNX_OFED_LINUX-5.1-0.6.6.0-ubuntu18.04-x86_64.tgz
cd MLNX_OFED_LINUX-5.1-0.6.6.0-ubuntu18.04-x86_64
sudo ./mlnxofedinstall --force
```

**Note OFED version for TrackFM/AIFM, fastswap are different, each time you switch between TrackFM and fastswap you also need to reinstall OFED**


# set compiler
```
source /home/TrackFM/runtime/compiler_passes/passes/set_paths
```
