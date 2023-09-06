TrackFM_PATH=/home/TrackFM/exp/fig17a

app_tmem=34816
amem=$((app_tmem*1024*1024)) 

obj_size=4096

sed "s/constexpr uint64_t kNumGCThreads.*/constexpr uint64_t kNumGCThreads = 8;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i 

sed "s/constexpr uint64_t kFarMemSize.*/constexpr uint64_t kFarMemSize = $amem;/g" /home/TrackFM/runtime/inc/carm_runtime.hpp -i


tobj=$((($amem/$obj_size) * 5))   

sed "s/#define TOTAL_OBJECTS .*/#define TOTAL_OBJECTS $tobj/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i

sed "s/#define  OBJ_SIZE  .*/#define  OBJ_SIZE  $obj_size/g" /home/TrackFM/runtime/inc/carm_object_config.hpp -i

cd /home/TrackFM/runtime/compiler_passes/passes/ 
make clean
make -j

NAS_PATH=/home/TrackFM/apps/nas_benchmarks/NPB-SER
cd $NAS_PATH
./compilebmk.sh

cd $TrackFM_PATH/nas_cg/
cp $NAS_PATH/bin/cg.D . 
echo "CG profile"
./run.sh
cd $TrackFM_PATH/nas_ft/
cp $NAS_PATH/bin/ft.C . 
echo "FT profile"
./run.sh
cd $TrackFM_PATH/nas_is/
cp $NAS_PATH/bin/is.D . 
echo "IS profile"
./run.sh
cd $TrackFM_PATH/nas_mg/
cp $NAS_PATH/bin/mg.D . 
echo "MG profile"
./run.sh
cd $TrackFM_PATH/nas_sp/
cp $NAS_PATH/bin/sp.D . 
echo "SP profile"
./run.sh

cd ..
mv $TrackFM_PATH/nas_cg/log.2 ../../plotgen/scripts/figgen/results/fig17a/TrackFM/cg.log.2
mv $TrackFM_PATH/nas_ft/log.2 ../../plotgen/scripts/figgen/results/fig17a/TrackFM/ft.log.2
mv $TrackFM_PATH/nas_is/log.9 ../../plotgen/scripts/figgen/results/fig17a/TrackFM/is.log.9
mv $TrackFM_PATH/nas_mg/log.7 ../../plotgen/scripts/figgen/results/fig17a/TrackFM/mg.log.7
mv $TrackFM_PATH/nas_sp/log.3 ../../plotgen/scripts/figgen/results/fig17a/TrackFM/sp.log.3
cd ../../plotgen
python3 scripts/figgen/fig17a.py $figpath fig17a
