#!/bin/bash
username=`echo $USERNAME`
ip=`echo $IP`
mem_array=( 2 )
rm log.*
scr="screen -dmS ft; screen -S ft-X stuff 'cd /home/fastswap/farmemserver; make clean; make; ./rmserver 3000 & \n'"
ter="screen -S ft-X stuff './kill.sh; exit; \n'"
ts="screen -XS ftquit \n'"
total_mem=8
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
	python3 benchmark.py CG $fmem --cpus 1 1>cg.log.$local_mem 2>&1
	./setup/destroy_cgroups.sh

	ssh $user@$ip "$ter"

        cd /home/fastswap/drivers/
	sudo rmmod fastswap.ko
	sudo rmmod fastswap_rdma.ko
done