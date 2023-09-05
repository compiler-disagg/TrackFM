#!/bin/bash

#rm log.*
sudo pkill -9 main
./run.sh & 
sleep 1200
echo "memclient"
python3 test_memc.py &>>log
