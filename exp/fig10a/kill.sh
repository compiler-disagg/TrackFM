#!/bin/bash

source ../../runtime/AIFM/aifm/shared.sh

ns_array=( 1200 )

	kill_local_iokerneld
	rerun_local_iokerneld_noht
	rerun_mem_server
