cd /home
sudo chmod 777 /home
git clone https://github.com/llvm/llvm-project.git
cd llvm-project/ 
git checkout llvmorg-9.0.0
mkdir build
cd build
cmake -DLLVM_TARGETS_TO_BUILD=host -DLLVM_ENABLE_PROJECTS="clang;libcxx;libcxxabi;libunwind;compiler-rt"  ../llvm/
make -j8
LLVM_HOME=/home/llvm-project/build
export PATH=$LLVM_HOME/bin:$PATH
export LD_LIBRARY_PATH=$LLVM_HOME/lib:$LD_LIBRARY_PATH
sudo ln -sf /home/llvm-project/build/bin/llvm-objcopy /usr/bin/objcopy
cd /home
git clone https://github.com/arcana-lab/noelle.git
cd noelle
git checkout v9.8.0
make
source /home/noelle/enable
export NOELLE_DIR=/home/noelle/
export CARM_DIR=/home/TrackFM/
