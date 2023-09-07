#!/bin/bash

user="btauro"
ip="128.110.218.197"

ns_array=( 1 2 3 4 5 6 7 8 9 )
ns_array=( 256 )
rm log.*
scr="screen -dmS bt ; screen -S bt -X stuff 'cd /home/fastswap/farmemserver; make clean; make; ./rmserver 3000 & \n'"
ter="screen -S bt -X stuff './kill.sh; exit; \n'"
ts="screen -XS bt quit \n'"
total_mem=700
source /proj/rmt-PG0/python_install/conda_bash
cd /home/cfm/protocol/
source gen_protocol.sh
for delay_ns in "${ns_array[@]}"
do  
	ssh $user@$ip "$scr"
        cd /home/fastswap/drivers/
        make clean
	make BACKEND=RDMA

	sudo insmod fastswap_rdma.ko sport=3000 sip="192.168.6.2 " cip="192.168.6.1" nq=60
	sudo insmod fastswap.ko

	fmem=$(python -c "from math import ceil; print(float(($delay_ns/$total_mem)))")
	
	source /proj/rmt-PG0/python_install/conda_bash
	cd /home/cfm/
        ./setup/init_bench_cgroups.sh
	python3 benchmark.py BT $fmem --cpus 1 1>bt.log.$delay_ns 2>&1
	./setup/destroy_cgroups.sh

	ssh $user@$ip "$ter"

        cd /home/fastswap/drivers/
	sudo rmmod fastswap.ko
	sudo rmmod fastswap_rdma.ko
done
