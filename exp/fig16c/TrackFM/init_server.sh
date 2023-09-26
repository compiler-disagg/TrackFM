#!/bin/bash

source ../../../runtime/AIFM/aifm/shared.sh

cache_sizes=( 1 )

obj_size=128

app_tmem=20480
amem=$((app_tmem*1024*1024))

sed "s/\/\/#define TRACKFM_TRACE.*/#define TRACKFM_TRACE 1/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i 

sed "s/constexpr uint64_t kNumGCThreads.*/constexpr uint64_t kNumGCThreads = 4;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i

sed "s/constexpr uint64_t kFarMemSize.*/constexpr uint64_t kFarMemSize = $amem;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i

tobj=$((($amem/$obj_size) * 10))
sed "s/#define TOTAL_OBJECTS .*/#define TOTAL_OBJECTS $tobj/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i

sed "s/#define  OBJ_SIZE  .*/#define  OBJ_SIZE  $obj_size/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i

cd /home/TrackFM/runtime/compiler_passes/passes/ 
make clean
make -j

cd /home/TrackFM/apps/memcached-1.2.7/
./compile.sh
figpath="/home/TrackFM/exp/fig16c/TrackFM"
cd $figpath
sudo pkill -9 main
kill_local_iokerneld
for cache_size in "${cache_sizes[@]}"
do
    make -f make_fig16c clean
    cp /home/TrackFM/*.o .
    cp /home/TrackFM/symbol_redefine.sh .
    
    cp ../../../apps/memcached-1.2.7/memcached.bc main.bc
    mem=$((cache_size*1024*1024*1024))
    sed "s/constexpr uint64_t local_mem_cache_size.*/constexpr uint64_t local_mem_cache_size = $mem;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i
    make -f make_fig16c -j20
    sudo cp libcarmapp.so /usr/local/lib/
    sudo ldconfig
    rerun_local_iokerneld_noht
    rerun_mem_server
    run_program_noht ./main 1>log.$cache_size 2>&1 
done
kill_local_iokerneld
sed "s/\/\/#define TRACKFM_TRACE.*/#define TRACKFM_TRACE 0/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i 
