TrackFM_PATH=/home/TrackFM/exp/fig17a/local
NAS_PATH=/home/TrackFM/apps/nas_benchmarks/NPB-SER
figpath=/home/TrackFM/exp/fig17a/local
source /home/TrackFM/runtime/compiler_passes/passes/set_paths

cd $NAS_PATH
./compilebmk_local.sh

cd  $figpath
cp $NAS_PATH/bin/cg.D cg.D
cp $NAS_PATH/bin/ft.C ft.C
cp $NAS_PATH/bin/is.D is.D
cp $NAS_PATH/bin/mg.D mg.D 
cp $NAS_PATH/bin/sp.D sp.D 

cd  $figpath
./cg.D &> ../../../plotgen/scripts/figgen/results/fig17a/local/cg.log.2
./ft.C &> ../../../plotgen/scripts/figgen/results/fig17a/local/ft.log.2
./is.D &> ../../../plotgen/scripts/figgen/results/fig17a/local/is.log.9
./mg.D &> ../../../plotgen/scripts/figgen/results/fig17a/local/mg.log.7
./sp.D &> ../../../plotgen/scripts/figgen/results/fig17a/local/sp.log.3

cd ../../../plotgen
python3 scripts/figgen/fig17a.py $figpath fig17a
