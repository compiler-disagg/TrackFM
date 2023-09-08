#!/bin/bash

figpath="/home/TrackFM/exp/fig16b/fastswap"
CFM_PATH=/home/cfm

d=( 1.01 1.02 1.04 1.08 1.1 1.12 1.14 1.18 1.2 1.22 1.24 1.28 1.3 )
#rm log.*
cd  $CFM_PATH
./fig16b.sh
mv $CFM_PATH/log.* ../../../plotgen/scripts/figgen/results/fig16b/fastswap/
cd ../../../plotgen
python3 scripts/figgen/fig16b.py $figpath fig16b
