#!/bin/bash

source ../../../runtime/AIFM/aifm/shared.sh

cache_sizes=( 1 )
app_tmem=16384
obj_size=4096
amem=$((app_tmem*1024*1024))
sed "s/constexpr uint64_t kFarMemSize.*/constexpr uint64_t kFarMemSize = $amem;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i
sed "s/constexpr uint64_t kNumGCThreads.*/constexpr uint64_t kNumGCThreads = 4;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i
tobj=$((($amem/$obj_size) * 10))
sed "s/#define TOTAL_OBJECTS .*/#define TOTAL_OBJECTS $tobj/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i
sed "s/#define  OBJ_SIZE  .*/#define  OBJ_SIZE  $obj_size/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i
sed "s/#define LOOP_PREFETCH.*/#define LOOP_PREFETCH 1/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i
sed "s/#define PREFETCH.*/#define PREFETCH 1/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i
cd /home/TrackFM/runtime/compiler_passes/passes/
make clean
make -j
fpath="kickstart/cstream/TrackFM"
cd $fpath 
sudo pkill -9 main
for cache_size in "${cache_sizes[@]}"
do
    make -f make_ks clean
    cp /home/TrackFM/symbol_redefine.sh .
    mem=$((cache_size*1024*1024*1024))
    sed "s/constexpr uint64_t local_mem_cache_size.*/constexpr uint64_t local_mem_cache_size = $mem;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i
    make -f make_ks -j20
    sudo cp libcarmapp.so /usr/local/lib/
    sudo ldconfig
    rerun_local_iokerneld_noht
    rerun_mem_server
    run_program_noht ./main 1>log.$cache_size 2>&1    
done
kill_local_iokerneld
wc="cat log.1 | grep \"Solution Validates: avg error less than 1.000000e-13 on all three arrays\"|wc -l"
wco=$(eval "$wc")
if [[ "$wco" == *"1"* ]];
then
echo "TrackFM installed sucessfully"
else
echo "TrackFM installation failed"
fi
