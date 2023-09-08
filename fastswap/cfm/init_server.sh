#!/bin/bash

user=$USERNAME
ip=$IP

mem_array=( 1 )
rm log.*
scr="screen -dmS fs ; screen -S fs -X stuff 'cd /home/fastswap/farmemserver; make clean; make; ./rmserver 3000 & \n'"
ter="screen -S fs -X stuff './kill.sh; exit; \n'"
ts="screen -XS fs quit \n'"
total_mem=12
cd /home/cfm/protocol/
source gen_protocol.sh
for local_mem in "${mem_array[@]}"
do  
	ssh $user@$ip "$scr"
        cd /home/fastswap/drivers/
        make clean
	make BACKEND=RDMA

	sudo insmod fastswap_rdma.ko sport=3000 sip="192.168.6.2 " cip="192.168.6.1" nq=60
	sudo insmod fastswap.ko

	fmem=$(python -c "from math import ceil; print(float(($local_mem/$total_mem)))")
	
	cd /home/cfm/
        ./setup/init_bench_cgroups.sh
	python3 benchmark.py MEMCACHEDV2 $fmem --cpus 1 1>log.$local_mem 2>&1
	./setup/destroy_cgroups.sh

	ssh $user@$ip "$ter"

        cd /home/fastswap/drivers/
	sudo rmmod fastswap.ko
	sudo rmmod fastswap_rdma.ko
done
