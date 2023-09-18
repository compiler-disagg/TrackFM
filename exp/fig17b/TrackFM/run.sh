TrackFM_PATH=/home/TrackFM/exp/fig17b/TrackFM
NAS_PATH=/home/TrackFM/apps/nas_benchmarks/NPB-SER
figpath=/home/TrackFM/exp/fig17b/TrackFM

app_tmem=34816
amem=$((app_tmem*1024*1024)) 

obj_size=4096

sed "s/constexpr uint64_t kNumGCThreads.*/constexpr uint64_t kNumGCThreads = 8;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i 

sed "s/constexpr uint64_t kFarMemSize.*/constexpr uint64_t kFarMemSize = $amem;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i


tobj=$((($amem/$obj_size) * 10))   

sed "s/#define TOTAL_OBJECTS .*/#define TOTAL_OBJECTS $tobj/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i

sed "s/#define  OBJ_SIZE  .*/#define  OBJ_SIZE  $obj_size/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i

cd /home/TrackFM/runtime/compiler_passes/passes/ 
make clean
make -j

cd $NAS_PATH
./compilebmk_O1.sh

cd $TrackFM_PATH/nas_ft/
cp $NAS_PATH/bin/ft.C . 
echo "FT profile"
cd $TrackFM_PATH/nas_sp/
cp $NAS_PATH/bin/sp.D . 
echo "SP profile"
./run.sh
cd ..
mv $TrackFM_PATH/nas_ft/log.2 ../../../plotgen/scripts/figgen/results/fig17b/TrackFM/ft.log.2
mv $TrackFM_PATH/nas_sp/log.3 ../../../plotgen/scripts/figgen/results/fig17b/TrackFM/sp.log.3
cd ../../../plotgen
python3 scripts/figgen/fig17b.py $figpath fig17b
