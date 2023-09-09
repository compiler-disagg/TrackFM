#!/bin/bash
rm log*
./init_server.sh & 
sleep 120
echo "memclient"
python3 test_memc.py &>>log
wc="ps -C \"memcached\" -o pid="
wco=$(eval "$wc")
echo $wco
kill -9 $wco
