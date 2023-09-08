#!/bin/bash

source ../../../AIFM/aifm/shared.sh

cache_sizes=( 1 2 3 4 5 6 7 8 9)
cache_sizes=( 1 )
#rm log.*
sudo pkill -9 main
for cache_size in "${cache_sizes[@]}"
do
#    cp /home/CARM/makefile_exp_stream Makefile
#    make clean
    cp /home/CARM/*.o .
    cp /home/CARM/symbol_redefine.sh .
    mem=$((cache_size*1024*1024*1024))
    sed "s/constexpr uint64_t local_mem_cache_size.*/constexpr uint64_t local_mem_cache_size = $mem;/g" /home/CARM/runtime/inc/carm_runtime.hpp -i
    #sed "s/constexpr uint64_t obj_cache_size.*/constexpr uint64_t obj_cache_size = $mem;/g" /home/CARM/runtime/inc/carm_runtime.hpp -i
    make -j20
    sudo cp libcarmapp.so /usr/local/lib/
    sudo ldconfig
    rerun_local_iokerneld_noht
    rerun_mem_server
    run_program_noht ./main 1>log.$cache_size 2>&1    
done
kill_local_iokerneld
