#!/bin/bash

source ../../runtime/AIFM/aifm/shared.sh


app_tmem=4096
amem=$((app_tmem*1024*1024))
sed "s/constexpr uint64_t kFarMemSize.*/constexpr uint64_t kFarMemSize = $amem;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i


obj_sizes=( 4096 )

for obj_size in "${obj_sizes[@]}"
do

tobj=$((($amem/$obj_size) * 10))
sed "s/#define TOTAL_OBJECTS .*/#define TOTAL_OBJECTS $tobj/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i

sed "s/#define  OBJ_SIZE  .*/#define  OBJ_SIZE  $obj_size/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i

#cd /home/TrackFM/runtime/compiler_passes/passes/
#make clean
#make -j


figpath="/home/TrackFM/exp/fig8"
cd $figpath
./compile.sh

sed "s/#define LOOP_PREFETCH.*/#define LOOP_PREFETCH 1/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i


noelle-norm main.bc -o pmain.bc
llvm-dis pmain.bc
noelle-prof-coverage pmain.bc pmain_pre_prof -lm 
./pmain_pre_prof
llvm-profdata merge default.profraw -output=output_prof
noelle-meta-prof-embed output_prof pmain.bc -o test_with_metadata.bc
noelle-meta-pdg-embed test_with_metadata.bc -o kmeans


cache_sizes=( 128 256 384 512 640 768 896 1024 )
rm log.*
cp make_chunk_prof Makefile
for cache_size in "${cache_sizes[@]}"
do
    sudo pkill -9 main
    kill_local_iokerneld
    rerun_local_iokerneld_noht
    rerun_mem_server
    sleep 5
    mem=$((cache_size*1024*1024))
    sed "s/constexpr uint64_t local_mem_cache_size.*/constexpr uint64_t local_mem_cache_size = $mem;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i
    make clean
    cp kmeans main.bc
    cp /home/TrackFM/symbol_redefine.sh .
    make -j
    sudo cp libcarmapp.so /usr/local/lib/
    sudo ldconfig
    run_program_noht ./main 1>log.$cache_size 2>&1    
done
    mv log.* ../../plotgen/scripts/figgen/results/fig8/TrackFM/chunk_prof/

cp make_no_chunk Makefile
for cache_size in "${cache_sizes[@]}"
do
    sudo pkill -9 main
    kill_local_iokerneld
    rerun_local_iokerneld_noht
    rerun_mem_server
    sleep 5
    mem=$((cache_size*1024*1024))
    sed "s/constexpr uint64_t local_mem_cache_size.*/constexpr uint64_t local_mem_cache_size = $mem;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i
    make clean
    ./compile.sh
    cp /home/TrackFM/symbol_redefine.sh .
    make -j
    sudo cp libcarmapp.so /usr/local/lib/
    sudo ldconfig
    run_program_noht ./main 1>log.$cache_size 2>&1    
done
    mv log.* ../../plotgen/scripts/figgen/results/fig8/TrackFM/no_chunk/
cp make_chunk Makefile
for cache_size in "${cache_sizes[@]}"
do
    sudo pkill -9 main
    kill_local_iokerneld
    rerun_local_iokerneld_noht
    rerun_mem_server
    sleep 5
    mem=$((cache_size*1024*1024))
    sed "s/constexpr uint64_t local_mem_cache_size.*/constexpr uint64_t local_mem_cache_size = $mem;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i
    make clean
    ./compile.sh
    cp /home/TrackFM/symbol_redefine.sh .
    make -j
    sudo cp libcarmapp.so /usr/local/lib/
    sudo ldconfig
    run_program_noht ./main 1>log.$cache_size 2>&1    
done
    mv log.* ../../plotgen/scripts/figgen/results/fig8/TrackFM/chunk/
done
cd ../../plotgen
python3 scripts/figgen/fig8.py $figpath fig8
