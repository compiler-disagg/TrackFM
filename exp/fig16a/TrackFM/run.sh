#!/bin/bash

rm log*
figpath="/home/TrackFM/exp/fig16a/TrackFM"
sudo pkill -9 main
./init_server.sh & 
sleep 1200
echo "memclient"
python3 test_memc.py &>>log
wc="cat log |grep \"a :\"|wc -l"
wco=$(eval "$wc")
if [[ "$wco" == *"0"* ]];
then
	./init_server.sh & 
	sleep 1200
	echo "memclient"
	python3 test_memc.py $a &>>log
fi
mv log ../../../plotgen/scripts/figgen/results/fig16a/TrackFM/
cd ../../../plotgen
python3 scripts/figgen/fig16a.py $figpath fig16a
cp *.png /home/TrackFM/figs/
