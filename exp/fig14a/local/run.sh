#!/bin/bash

source ../../../runtime/AIFM/aifm/shared.sh


TRACKFM_PATH=/home/TrackFM
cd /home/TrackFM/apps/C_dataframe/
./run.sh
figpath="/home/TrackFM/exp/fig14a/local"
cd $figpath
cp /home/TrackFM/apps/C_dataframe/nyc $figpath
cp /home/datasets/all.csv .
./nyc
mv log.* $TRACKFM_PATH/plotgen/scripts/figgen/results/fig14a/local/
cd  $figpath
cd ../../../plotgen
python3 scripts/figgen/fig14a.py $figpath fig14a
