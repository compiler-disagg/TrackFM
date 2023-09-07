#!/bin/bash

user=`echo $USERNAME`
ip=`echo $IP`

mem_array=( 1 2 3 4 5 6 7 8 9 10 11 12 13)
rm log.*
scr="screen -dmS fs ; screen -S fs -X stuff 'cd /home/fastswap/farmemserver; make clean; make; ./rmserver 3000 & \n'"
ter="screen -S fs -X stuff './kill.sh; exit; \n'"
ts="screen -XS fs quit \n'"
total_mem=13
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
	fmem=$(python -c "from math import ceil; print(float((float($local_mem)/$total_mem)))")
	cd /home/cfm/
        ./setup/init_bench_cgroups.sh
	python3 benchmark.py COPY $fmem --cpus 1 1>log.$local_mem 2>&1
	./setup/destroy_cgroups.sh
	ssh $user@$ip "$ter"
        cd /home/fastswap/drivers/
	sudo rmmod fastswap.ko
	sudo rmmod fastswap_rdma.ko
done
