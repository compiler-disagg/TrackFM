#!/bin/bash

source ../../../runtime/AIFM/aifm/shared.sh


app_tmem=30720
amem=$((app_tmem*1024*1024))
sed "s/constexpr uint64_t kFarMemSize.*/constexpr uint64_t kFarMemSize = $amem;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i

sed "s/\/\/#define TRACKFM_TRACE.*/#define TRACKFM_TRACE 1/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i 

obj_sizes=( 4096 )

for obj_size in "${obj_sizes[@]}"
do

tobj=$((($amem/$obj_size) * 10))
sed "s/#define TOTAL_OBJECTS .*/#define TOTAL_OBJECTS $tobj/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i

sed "s/#define  OBJ_SIZE  .*/#define  OBJ_SIZE  $obj_size/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i

cd /home/TrackFM/runtime/compiler_passes/passes/
make clean
make -j

cd /home/TrackFM/apps/C_dataframe/
./run.sh


figpath="/home/TrackFM/exp/fig14b/TrackFM"
cd $figpath

cp /home/TrackFM/apps/C_dataframe/nyc.bc main.bc
cp /home/datasets/all.csv .

sed "s/#define LOOP_PREFETCH.*/#define LOOP_PREFETCH 1/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i


noelle-norm main.bc -o pmain.bc
llvm-dis pmain.bc
noelle-prof-coverage pmain.bc pmain_pre_prof -lm 
./pmain_pre_prof
llvm-profdata merge default.profraw -output=output_prof
noelle-meta-prof-embed output_prof pmain.bc -o test_with_metadata.bc
noelle-meta-pdg-embed test_with_metadata.bc -o nyc


cache_sizes=( 1 3 5 7 9 11 13 15 17 19 21 23 25 27 29 31)
rm log.*
for cache_size in "${cache_sizes[@]}"
do
    sudo pkill -9 main
    kill_local_iokerneld
    rerun_local_iokerneld_noht
    rerun_mem_server
    sleep 5
    mem=$((cache_size*1024*1024*1024))
    sed "s/constexpr uint64_t local_mem_cache_size.*/constexpr uint64_t local_mem_cache_size = $mem;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i
    make -f make_fig14b clean
    cp nyc main.bc
    cp /home/TrackFM/symbol_redefine.sh .
    make -f make_fig14b -j
    sudo cp libcarmapp.so /usr/local/lib/
    sudo ldconfig
    run_program_noht ./main 1>log.$cache_size 2>&1    
    wc="cat log.$cache_size |grep \"Total:\"|wc -l"
    wco=$(eval "$wc")
    if [[ "$wco" == *"0"* ]];
    then
    	    sudo pkill -9 main
	    kill_local_iokerneld
	    rerun_local_iokerneld_noht
	    rerun_mem_server
	    run_program_noht ./main 1>log.$cache_size 2>&1    
    fi
done
    mv log.* ../../../plotgen/scripts/figgen/results/fig14b/TrackFM/
done
cd ../../../plotgen
python3 scripts/figgen/fig14b.py $figpath fig14b
sed "s/#define LOOP_PREFETCH.*/#define LOOP_PREFETCH 1/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i
cp *.png /home/TrackFM/figs/
