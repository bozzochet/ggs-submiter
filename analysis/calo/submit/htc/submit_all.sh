#!/bin/bash
LISTDIR="/storage/gpfs_ams/ams/users/vvagelli/HERD/analysis/herd-vv-svn/analysis/calo/submit/htc/lists"
#for STREAM in "electrons_1000GeV_10000GeV_E-1" "electrons_10GeV_1000GeV_E-1" 
#for STREAM in "electrons_1000GeV_10000GeV_E-1" "electrons_10GeV_1000GeV_E-1" "protons_10GeV_10000GeV_E-1"
#for STREAM in "protons_10GeV_10000GeV_E-1"
for STREAM in "electrons_1000GeV_10000GeV_E-1"
do
    for flist in `ls ${LISTDIR}/${STREAM}/*.list.txt`
    do
	echo "LIST: $flist"
	for TH in 0.001 0.002 0.005 0.010 0.015 0.020 0.030 0.050 0.100
	do
	    BASENAME=`basename $flist .list.txt`
	    NAME="$BASENAME.edepthreshold_$TH"
	    LISTFILE=$flist
	    OPTS="--parameter caloAxisAlgo,edepthreshold,$TH"
       	    echo "$NAME $LISTFILE $OPTS"
	    ./submit_one_job.sh $NAME $LISTFILE "$OPTS"
	done
    done
done
