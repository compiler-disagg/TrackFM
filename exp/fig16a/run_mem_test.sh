#!/bin/bash

#rm log.*
figpath="/home/TrackFM/exp/fig16a"
sudo pkill -9 main
./run.sh & 
sleep 1200
echo "memclient"
python3 test_memc.py &>>log
mv log ../../plotgen/scripts/figgen/results/fig16a/TrackFM/
cd ../../plotgen
python3 scripts/figgen/fig16a.py $figpath fig16a
