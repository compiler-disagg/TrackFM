#!/bin/bash

source ../../../runtime/AIFM/aifm/shared.sh

CFM_PATH=/home/cfm/
TRACKFM_PATH=/home/TrackFM

figpath="/home/TrackFM/exp/fig12/fastswap"

cd $figpath

rm log.*

cp main_sum.cpp main.cpp
make clean
make -j
cp main $CFM_PATH"/SUM/sum"
cd $CFM_PATH
./fig12a.sh
mv log.* $TRACKFM_PATH"/plotgen/scripts/figgen/results/fig11/fastswap/sum/"
cd $figpath
##copy
cp main_copy.cpp main.cpp
make clean
make -j
cp main $CFM_PATH"/COPY/copy" 
cd $CFM_PATH
./fig12b.sh
mv log.* $TRACKFM_PATH"/plotgen/scripts/figgen/results/fig11/fastswap/copy/"
