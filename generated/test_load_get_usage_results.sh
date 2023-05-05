#!/bin/bash

for d in  generated_test_load_kernel_*; do
	echo "Dir: $d"
	rpt=$d/_x/reports/link/imp/impl_1_full_util_placed.rpt
	axibits=$(echo $d | awk -F "axi_" '{print $2}' | awk -F "b_" '{print $1}')
	vect=$(echo $d | awk -F "axi_" '{print $2}' | awk -F "vectorize_" '{print $2}')
	if [ -z $vect ]; then
		vect="no"
	fi
	echo "AXI: $axibits, Vectorize: $vect"
	#cat $rpt | grep "1. CLB Logic" -A 19 | grep "Site Type" -A 14
	#cat $rpt | grep "3. BLOCKRAM" -A 11 | grep Site -A 7
	cat $rpt
done


