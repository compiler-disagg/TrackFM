TrackFM_PATH=/home/TrackFM/exp/fig17a/fastswap
NAS_PATH=/home/TrackFM/apps/nas_benchmarks/NPB-SER
figpath=/home/TrackFM/exp/fig17a/fastswap
CFM_PATH=/home/cfm
source /home/TrackFM/runtime/compiler_passes/passes/set_paths

app_tmem=34816
amem=$((app_tmem*1024*1024)) 

obj_size=4096

cd $NAS_PATH
./compilebmk_local.sh

cd $CFM_PATH
cp $NAS_PATH/bin/cg.D nas_cg/cg.D 
cp $NAS_PATH/bin/ft.C nas_ft/ft.C 
cp $NAS_PATH/bin/is.D nas_is/is.D 
cp $NAS_PATH/bin/mg.D nas_mg/mg.D 
cp $NAS_PATH/bin/sp.D nas_sp/sp.D 
./run_nas.sh

cd  $figpath
mv $CFM_PATH/cg.log.2 ../../../plotgen/scripts/figgen/results/fig17a/fastswap/cg.log.2
mv $CFM_PATH/ft.log.2 ../../../plotgen/scripts/figgen/results/fig17a/fastswap/ft.log.2
mv $CFM_PATH/is.log.9 ../../../plotgen/scripts/figgen/results/fig17a/fastswap/is.log.9
mv $CFM_PATH/mg.log.7 ../../../plotgen/scripts/figgen/results/fig17a/fastswap/mg.log.7
mv $CFM_PATH/sp.log.3 ../../../plotgen/scripts/figgen/results/fig17a/fastswap/sp.log.3

cd ../../../plotgen
python3 scripts/figgen/fig17a.py $figpath fig17a
