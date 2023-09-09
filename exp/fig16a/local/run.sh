#!/bin/bash
rm log*
figpath="/home/TrackFM/exp/fig16a/local"
cd /home/TrackFM/apps/memcached-1.2.7
./compile_local.sh
cp memcached $figpath
cd $figpath
./memcached & 
sleep 120
echo "memclient"
python3 test_memc.py &>>log
mv log ../../../plotgen/scripts/figgen/results/fig16a/linux_local/
cd ../../../plotgen
python3 scripts/figgen/fig16a.py $figpath fig16a
wc="ps -C \"memcached\" -o pid="
wco=$(eval "$wc")
echo $wco
kill -9 $wco
