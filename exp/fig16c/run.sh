#!/bin/bash

rm log*
figpath="/home/TrackFM/exp/fig16b"

d=( 1.01 1.02 1.04 1.08 1.1 1.12 1.14 1.18 1.2 1.22 1.24 1.28 1.3 )
#rm log.*
sudo pkill -9 main
for a in "${d[@]}"
do
	./init_server.sh & 
	sleep 200
	echo "memclient"
	python3 test_memc.py $a &>>log.$a
	wc="ps -C \"main\" -o pid="
	wco=$(eval "$wc")
	echo $wco
	sudo gdb --batch --command=debug.gdb -p $wco &>>log.$a
done
mv log.$a ../../plotgen/scripts/figgen/results/fig16c/TrackFM/
cd ../../plotgen
python3 scripts/figgen/fig16c.py $figpath fig16c
