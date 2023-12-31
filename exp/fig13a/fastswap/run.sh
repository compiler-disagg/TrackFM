#!/bin/bash

source ../../../runtime/AIFM/aifm/shared.sh

python3 gen_zipf.py

CFM_PATH=/home/cfm/
TRACKFM_PATH=/home/TrackFM

figpath="/home/TrackFM/exp/fig13a/fastswap"

cd $figpath

rm log.*
python3 test_memc.py
make -f make_fig13a clean
make -f make_fig13a -j
cp main $CFM_PATH"/UMAP/umap"
cp zipf.txt $CFM_PATH"/UMAP/"
cd $CFM_PATH
./fig13.sh
mv log.* ../../../plotgen/scripts/figgen/results/fig13a/fastswap/
cd $TRACKFM_PATH/plotgen
python3 scripts/figgen/fig13a.py $figpath fig13a
