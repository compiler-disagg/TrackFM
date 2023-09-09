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

## Setting Up the Experimental Environment

### CloudLab

All experiments in our paper were run on the
[CloudLab](https://www.cloudlab.us/) platform. Our CloudLab profile, which
describes a two-node Ubuntu 18.04 setup, is in
a separate repo. You'll first need to create a CloudLab account if you don't
have one already. Once you have an account, to instantiate our profile from
your CloudLab dashboard, at the top of the page there should be button for
"Experiments." Click on that and choose "Create Experiment Profile." Provide
a name of your choosing for the profile, and for the "Source code" option,
click the button that says "Git Repo." Provide this URL:
https://github.com/compiler-disagg/trackfm-cloudlab. 

### Launching the Experimental Environment

**SSH Key Configuration**

You'll first want to make sure you have a public key set up so that you can log
in to the experiment node(s). On the top right of the CloudLab Dashboard, click
the button with your username, and select "Manage SSH Keys." You can then upload
a key of your choice. 

**Launching Instances**

Once you've set up the CloudLab profile, you should be able to launch the
necessary instances by going to "Experiments -> Start Experiment," then "Change
Profile" and select the one that you created
[above](#setting-up-the-experimental-environment) then choose "Select Profile,"
then hit "Next". You should not need to change any options on the
"Parameterize" tab, so hit "Next" again. On the "Finalize" tab, provide a name
of your choosing, then hit "Next." We want to instantiate the nodes now, so do
not provide a schedule, and hit "Finish." If there are sufficient resources
available in CloudLab, then the nodes should launch successfully. Note that we
require the use of the xl170 node type in CloudLab since they have the
necessary InfiniBand hardware required to support the
[Shenango](https://github.com/shenango/shenango) runtime. Note that you may
want to request more than one day since several of the experiments can run for
quite a long time (several hours). We have only tested on CloudLab, so we do not
currently support other platforms (e.g., AWS). 

Once the nodes have launched successfully, you should be able to select "List
View" to see the nodes, and to view the public hostnames that you can SSH to.

## Build Instructions

The default storage allocation on CloudLab is insufficient for our experiments. 
We will instead use a custom `/home` directory, created by the profile. You'll first need
to set up permissive access to this directory:

```bash
sudo chmod 777 /home
```

Note that unless otherwise explicitly stated, it is assumed that you will run these
setup instructions on both nodes in CloudLab (the _compute node_ and the _memory server_).

### Build Prerequisites

We can first set up the TrackFM repo:

```bash
cd /home
git clone https://github.com/compiler-disagg/trackfm
```

**Dev Packages**

We need to install relevant packages packages for TrackFM, AIFM, Shenango, and
our example applications. We'll first need to upgrade the Linux kernel version, which
will necessitate a reboot:

```bash
sudo apt-get update
sudo apt-get install -y linux-headers-5.0.0-20 \
                        linux-headers-5.0.0-20-generic \
                        linux-hwe-edge-tools-5.0.0-20 \
                        linux-image-5.0.0-20-generic \
                        linux-modules-5.0.0-20-generic \
                        linux-tools-5.0.0-20-generic
sudo reboot
```

After reboot, verify that the instance is using the 5.0 kernel by running `uname -r`. 
We can then install the necessary packages:

```bash
sudo apt-get update
sudo apt-get -y --fix-broken install
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get install -y libnuma-dev \
                        libmnl-dev \
                        libnl-3-dev \
                        libnl-route-3-dev \
                        libcrypto++-dev \
                        libcrypto++-doc \
                        libcrypto++-utils \
                        software-properties-common \
                        gcc-9 \
                        g++-9 \
                        python-pip \
                        python3-pip \
                        libjpeg-dev \
                        zlib1g-dev \
                        libevent-dev
sudo apt-get -y purge cmake
```

**Mellanox OFED Stack**

Shenango and AIFM require support for the Mellanox InfiniBand cards. This needs to be set up manually:

```bash
wget "http://content.mellanox.com/ofed/MLNX_OFED-4.6-1.0.1.1/MLNX_OFED_LINUX-4.6-1.0.1.1-ubuntu18.04-x86_64.tgz"
tar xvf MLNX_OFED_LINUX-4.6-1.0.1.1-ubuntu18.04-x86_64.tgz
cd MLNX_OFED_LINUX-4.6-1.0.1.1-ubuntu18.04-x86_64
sudo ./mlnxofedinstall --add-kernel-support --dpdk --upstream-libs 
sudo /etc/init.d/openibd restart
```

Note that when you run `./mlnxofedinstall` above, you may see a message like `Failed to install libibverbs-dev DEB'. This is okay, and it should still work. 

**Python Toolchain**

We require several Python modules for AIFM, for plot generation, and for our benchmarks:

```bash
cd /home/trackfm
sudo pip3 install -r requirements.txt
sudo pip install --upgrade pip
sudo pip install cmake
```
TODO: I'd use specific version numbers in `requirements.txt`

**Compiler Toolchain**

TrackFM relies on the [NOELLE compiler toolchain](https://github.com/arcana-lab/noelle), which
we need to install. **Note that you only need to install this on the compute node (the node
where you launch applications), not the memory server node**. This will take a couple of hours,
so sit back and enjoy a coffee. 

```bash
./install-dependencies.sh
```

### Building TrackFM, AIFM, and Shenango

```bash
chsh -s /bin/bash
```

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


### Datasets
We use Kaggle to store our datasets. In order to download datasets we require you 
to have a Kaggle account. Once your account is created, you can click on the profile 
button in the Kaggle homepage to generate an api token. This will download a JSON file. 
Copy the JSON file to the compute server in /home/datasets. 
 
```bash
cd /home
mkdir datasets
#copy JSON API TOKEN in this folder
cd  datasets
pip3 install --user kaggle
#upload your kaggle api token to the cloudlab server for example using scp
cp kaggle.json ~/.kaggle/   
kaggle datasets download -d btauro/kmeans
kaggle datasets download -d btauro/nyc-dataframe
unzip nyc-dataframe.zip
unzip kmeans.zip
```


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
fig14a
├── fastswap
│   ├── Makefile
│   └── run.sh
└── TrackFM
    ├── Makefile
    └── run.sh

cd  exp/fig14a
cd TrackFM
//reproduce TrackFM datapoints and generate plots
./run.sh
cd  fastswap
//reproduce Fastswap datapoints and generate plots
./run.sh
```
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
