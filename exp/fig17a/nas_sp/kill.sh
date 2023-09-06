#!/bin/bash

source ../../../AIFM/aifm/shared.sh

cache_sizes=( 1 2 3 4 5 6 7 8 9)
cache_sizes=( 1 )
#rm log.*
sudo pkill -9 main
kill_local_iokerneld
rerun_local_iokerneld_noht
rerun_mem_server
