#!/bin/bash

rm log*

d=( 1.01 1.02 1.04 1.08 1.1 1.12 1.14 1.18 1.2 1.22 1.24 1.28 1.3 )
#rm log.*
sudo pkill -9 main
for a in "${d[@]}"
do
	./init_server.sh & 
	sleep 120
	echo "memclient"
	python3 test_memc_b.py $a &>>log.$a
	wc="ps -C \"memcached\" -o pid="
	wco=$(eval "$wc")
	echo $wco
	kill -9 $wco
done
