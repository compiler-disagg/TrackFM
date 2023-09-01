#!/bin/bash

source ../../runtime/AIFM/aifm/shared.sh


python3 test_memc.py
app_tmem=16384
amem=$((app_tmem*1024*1024))
sed "s/constexpr uint64_t kFarMemSize.*/constexpr uint64_t kFarMemSize = $amem;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i

sed "s/\/\/#define TRACKFM_TRACE.*/#define TRACKFM_TRACE 1/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i


obj_sizes=( 64 )

for obj_size in "${obj_sizes[@]}"
do

tobj=$((($amem/$obj_size) * 3))
sed "s/#define TOTAL_OBJECTS .*/#define TOTAL_OBJECTS $tobj/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i

sed "s/#define  OBJ_SIZE  .*/#define  OBJ_SIZE  $obj_size/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i

cd /home/TrackFM/runtime/compiler_passes/passes/ 

make clean
make -j

cd /home/TrackFM/exp/fig13b

cache_sizes=( 128 256 384 512 768 1024 1536 2048 4096)
rm log.*
sudo pkill -9 main
for cache_size in "${cache_sizes[@]}"
do
    kill_local_iokerneld
    rerun_local_iokerneld_noht
    rerun_mem_server
    mem=$((cache_size*1024*1024))
    sed "s/constexpr uint64_t local_mem_cache_size.*/constexpr uint64_t local_mem_cache_size = $mem;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i
    make clean
    cp  ../compile_bitcodes/libc++_$obj_size"_obj_size"/*.o .
    cp /home/TrackFM/symbol_redefine.sh .
    make -j
    sudo cp libcarmapp.so /usr/local/lib/
    sudo ldconfig
    run_program_noht ./main 1>log.$cache_size 2>&1    
done
		mv log.* $obj_size/
done
sed "s/#define TRACKFM_TRACE.*/\/\/#define TRACKFM_TRACE 1/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i
