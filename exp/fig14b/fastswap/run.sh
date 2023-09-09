#!/bin/bash

source ../../../runtime/AIFM/aifm/shared.sh


CFM_PATH=/home/cfm
TRACKFM_PATH=/home/TrackFM
cd /home/TrackFM/apps/C_dataframe/
./run.sh
figpath="/home/TrackFM/exp/fig14b/fastswap"
cd $figpath
cp /home/datasets/all.csv $CFM_PATH/NYC/
cp /home/TrackFM/apps/C_dataframe/nyc $CFM_PATH/NYC/main
cd $CFM_PATH
./fig14.sh
mv log.* $TRACKFM_PATH/plotgen/scripts/figgen/results/fig14a/fastswap/
cd  $figpath
cd ../../../plotgen
python3 scripts/figgen/fig14b.py $figpath fig14b
