#!/bin/bash
CFM_PATH=/home/cfm
rm log*
figpath="/home/TrackFM/exp/fig16a/fastswap"
cd /home/TrackFM/apps/memcached-1.2.7
./compile_local.sh
cp memcached $CFM_PATH/memcachedv2/
cd  $CFM_PATH
./fig16a.sh
cd $figpath
mv $CFM_PATH/log ../../../plotgen/scripts/figgen/results/fig16a/fastswap/
cd ../../../plotgen
python3 scripts/figgen/fig16a.py $figpath fig16a
