#!/bin/bash

#for STREAM in "electrons_1000GeV_10000GeV_E-1"      # "electrons_10GeV_1000GeV_E-1" 
for STREAM in "electrons_1000GeV_10000GeV_E-1" "electrons_10GeV_1000GeV_E-1" "protons_10GeV_10000GeV_E-1"
#for STREAM in "protons_10GeV_10000GeV_E-1"
do
    for TH in 0.001 0.002 0.005 0.010 0.015 0.020 0.030 0.050 0.100
#    for TH in 0.002 0.005 0.015
    do
	NAME="$STREAM.edepthreshold_$TH"
	LISTFILE="$STREAM.dig.list.txt"
	OPTS="--parameter caloAxis,edepthreshold,$TH"
       	./submit_one_job.sh $NAME $LISTFILE "$OPTS"    
    done
done
