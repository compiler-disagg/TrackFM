#!/bin/bash

source ../../../runtime/AIFM/aifm/shared.sh


CFM_PATH=/home/cfm
TRACKFM_PATH=/home/TrackFM
cd /home/TrackFM/apps/C_dataframe/
./compile_local.sh
figpath="/home/TrackFM/exp/fig14a/fastswap"
cd $figpath
cp /home/datasets/all.csv $CFM_PATH/NYC/
cp /home/TrackFM/apps/C_dataframe/nyc $CFM_PATH/NYC/main
cp /home/TrackFM/apps/C_dataframe/libdataframe.so $CFM_PATH/NYC/
cd $CFM_PATH
./fig14.sh
mv log.* $TRACKFM_PATH/plotgen/scripts/figgen/results/fig14a/fastswap/
cd  $figpath
cd ../../../plotgen
python3 scripts/figgen/fig14a.py $figpath fig14a
