#!/bin/bash

source ../../runtime/AIFM/aifm/shared.sh


app_tmem=16384
amem=$((app_tmem*1024*1024))
sed "s/constexpr uint64_t kFarMemSize.*/constexpr uint64_t kFarMemSize = $amem;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i
sed "s/constexpr uint64_t kNumGCThreads.*/constexpr uint64_t kNumGCThreads = 3;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i


obj_sizes=( 4096 )

for obj_size in "${obj_sizes[@]}"
do

tobj=$((($amem/$obj_size) * 3))
sed "s/#define TOTAL_OBJECTS .*/#define TOTAL_OBJECTS $tobj/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i

sed "s/#define  OBJ_SIZE  .*/#define  OBJ_SIZE  $obj_size/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i

cd /home/TrackFM/runtime/compiler_passes/passes/
make clean
make -j

figpath="/home/TrackFM/exp/fig12"
cd $figpath

cache_sizes=( 1 2 3 4 5 6 7 8 9 10 11 12)
rm log.*

cp main_sum.cpp main.cpp
sudo pkill -9 main
for cache_size in "${cache_sizes[@]}"
do
    rerun_local_iokerneld_noht
    rerun_mem_server
    sed "s/#define LOOP_PREFETCH.*/#define LOOP_PREFETCH 1/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i
    mem=$((cache_size*1024*1024*1024))
    sed "s/constexpr uint64_t local_mem_cache_size.*/constexpr uint64_t local_mem_cache_size = $mem;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i
    make clean
    cp /home/TrackFM/symbol_redefine.sh .
    cp  ../compile_bitcodes/libc++_$obj_size"_obj_size"/*.o .
    make -j
    sudo cp libcarmapp.so /usr/local/lib/
    sudo ldconfig
    run_program_noht ./main 1>log.$cache_size 2>&1    
    rerun_local_iokerneld_noht
    rerun_mem_server
    sed "s/#define LOOP_PREFETCH.*/#define LOOP_PREFETCH 0/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i
    make clean
    cp  ../compile_bitcodes/libc++_$obj_size"_obj_size"/*.o .
    cp /home/TrackFM/symbol_redefine.sh .
    make -j
    sudo cp libcarmapp.so /usr/local/lib/
    sudo ldconfig
    run_program_noht ./main 1>log_no_prefetch.$cache_size 2>&1    
done
    mv log.* ../../plotgen/scripts/figgen/results/fig11/TrackFM/sum_chunk_prefetch/
    mv log_no_prefetch.* ../../plotgen/scripts/figgen/results/fig11/TrackFM/sum_chunk/
##copy
cp main_copy.cpp main.cpp
for cache_size in "${cache_sizes[@]}"
do
    rerun_local_iokerneld_noht
    rerun_mem_server
    sed "s/#define LOOP_PREFETCH.*/#define LOOP_PREFETCH 1/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i
    mem=$((cache_size*1024*1024*1024))
    sed "s/constexpr uint64_t local_mem_cache_size.*/constexpr uint64_t local_mem_cache_size = $mem;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i
    make clean
    cp  ../compile_bitcodes/libc++_$obj_size"_obj_size"/*.o .
    cp /home/TrackFM/symbol_redefine.sh .
    make -j
    sudo cp libcarmapp.so /usr/local/lib/
    sudo ldconfig
    run_program_noht ./main 1>log.$cache_size 2>&1    
    rerun_local_iokerneld_noht
    rerun_mem_server
    sed "s/#define LOOP_PREFETCH.*/#define LOOP_PREFETCH 0/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i
    make clean
    cp  ../compile_bitcodes/libc++_$obj_size"_obj_size"/*.o .
    cp /home/TrackFM/symbol_redefine.sh .
    make -j
    sudo cp libcarmapp.so /usr/local/lib/
    sudo ldconfig
    run_program_noht ./main 1>log_no_prefetch.$cache_size 2>&1    
done
    mv log.* ../../plotgen/scripts/figgen/results/fig11/TrackFM/copy_chunk_prefetch/
    mv log_no_prefetch.* ../../plotgen/scripts/figgen/results/fig11/TrackFM/copy_chunk/
done
kill_local_iokerneld
cd ../../plotgen
python3 scripts/figgen/fig12.py $figpath fig12
