#!/bin/bash

source /home/TrackFM/runtime/compiler_passes/passes/set_paths 
sudo pkill -9 main
sudo ln -sf /proj/rmt-PG0/noelle_setup/llvm-project/build/bin/llvm-objcopy /usr/bin/objcopy
sudo ln -sf /proj/rmt-PG0/noelle_setup/llvm-project/build/bin/llvm-link /usr/bin/llvm-link

log_folder=`pwd`
export LLVM_COMPILER=clang
CC=wllvm CXX=wllvm++ LLVM_COMPILER=clang make
wllvm -O1 -fno-vectorize -fno-builtin -fPIC  -c nyc.c -I include/ -o nycc
extract-bc libdataframe.so
extract-bc nycc
llvm-link nycc.bc libdataframe.so.bc -o nyc.bc
clang -O3 nyc.c -I include/ -o nyc -lm
