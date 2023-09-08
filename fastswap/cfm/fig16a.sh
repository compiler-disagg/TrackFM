#!/bin/bash
rm log*
./init_server.sh & 
sleep 120
echo "memclient"
python3 test_memc.py &>>log
