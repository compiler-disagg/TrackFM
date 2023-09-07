#!/bin/bash

source ../../../runtime/AIFM/aifm/shared.sh

CFM_PATH=/home/cfm/
TRACKFM_PATH=/home/TrackFM

figpath="/home/TrackFM/exp/fig12/fatswap"

cd $figpath

cache_sizes=( 1 2 3 4 5 6 7 8 9 10 11 12)
rm log.*

cp main_sum.cpp main.cpp
for cache_size in "${cache_sizes[@]}"
do
    make clean
    make -j
    cp main $CFM_PATH/SUM/sum 
    cd CFM_PATH
    ./fig12a.sh
done
    mv log.* $TRACKFM_PATH/plotgen/scripts/figgen/results/fig11/fastswap/sum/
cd $figpath
##copy
cp main_copy.cpp main.cpp
for cache_size in "${cache_sizes[@]}"
do
    make clean
    make -j
    cp main $CFM_PATH/COPY/copy 
    cd CFM_PATH
    ./fig12b.sh
done
    mv log.* $TRACKFM_PATH/plotgen/scripts/figgen/results/fig11/fastswap/copy/
