#!/bin/bash

source ../../runtime/AIFM/aifm/shared.sh

cache_sizes=( 64 )

#rm log.*
sudo pkill -9 main
figpath=/home/TrackFM/exp/fig6
obj_size=32768
app_tmem=1024
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
cd $figpath
for cache_size in "${cache_sizes[@]}"
do
    cp /home/TrackFM/*.o .
    cp /home/TrackFM/symbol_redefine.sh .
    mem=$((cache_size*1024*1024))
    sed "s/constexpr uint64_t local_mem_cache_size.*/constexpr uint64_t local_mem_cache_size = $mem;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i
    make -f make_chunk clean
    make -f make_chunk -j
    sudo cp libcarmapp.so /usr/local/lib/
    sudo ldconfig
    rerun_local_iokerneld_noht
    rerun_mem_server
    run_program_noht ./main 1>log.$obj_size 2>&1    
    wc="cat log.$obj_size |grep \"e : 32768\"|wc -l"
    wco=$(eval "$wc")
    if [[ "$wco" == *"0"* ]];
    then
    	    sudo pkill -9 main
	    kill_local_iokerneld
	    rerun_local_iokerneld_noht
	    rerun_mem_server
	    run_program_noht ./main 1>log.$obj_size  2>&1    
    fi
    mv log.$obj_size ../../plotgen/scripts/figgen/results/fig6/chunk/
    make -f make_no_chunk clean
    make -f make_no_chunk -j
    sudo cp libcarmapp.so /usr/local/lib/
    sudo ldconfig
    rerun_local_iokerneld_noht
    rerun_mem_server
    run_program_noht ./main 1>log.$obj_size 2>&1    
    wc="cat log.$obj_size |grep \"e : 32768\"|wc -l"
    wco=$(eval "$wc")
    if [[ "$wco" == *"0"* ]];
    then
    	    sudo pkill -9 main
	    kill_local_iokerneld
	    rerun_local_iokerneld_noht
	    rerun_mem_server
	    run_program_noht ./main 1>log.$obj_size  2>&1    
    fi
    mv log.$obj_size ../../plotgen/scripts/figgen/results/fig6/no_chunk/
done
kill_local_iokerneld
sed "s/#define LOOP_PREFETCH.*/#define LOOP_PREFETCH 1/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i
sed "s/#define PREFETCH.*/#define PREFETCH 1/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i
cd ../../plotgen
python3 scripts/figgen/fig6.py $figpath fig6
cp *.png /home/TrackFM/figs/
