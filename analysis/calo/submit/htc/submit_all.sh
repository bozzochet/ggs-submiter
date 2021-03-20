#!/bin/bash
LISTDIR=${STORAGE}/herd-vv-svn/analysis/calo/submit/htc/lists
OPTS=""
MAXFILE=1000000 #max number of list files to run
OPTS="${OPTS} --parameter histo*,naxisbins,100 --parameter histo*,axismin,10 --parameter histo*,axismax,100000"

#OPTS="${OPTS} --parameter pcaCaloAxisTrackingAlgo,edepThreshold,0.020"
#OPTS="${OPTS} --parameter mcgenspectrum,naxisbin,150 --parameter mcgenspectrum,axismin,1 --parameter mcgenspectrum,axismax,100000 --parameter mcgenspectrum,minmom,10 -p mcgenspectrum,maxmom,1000"

for STREAM in "electrons_10GeV_1000GeV_E-1_2828_caloonly" "electrons_10GeV_1000GeV_E-1_3030_caloonly"
do
    for flist in `ls ${LISTDIR}/${STREAM}/*.list.txt | head -n ${MAXFILE}`
    do
	echo "LIST: $flist"
	#for TH in 0.001 0.002 0.005 0.010 0.015 0.020 0.030 0.050 0.100
	for TH in  0.020
	do
	    BASENAME=`basename $flist .list.txt`
	    #NAME="$BASENAME.edepthreshold_$TH"
	    NAME="$BASENAME"
	    SUFFIX=".0"
	    #SUFFIX=".caloclth_${TH}"
	    LISTFILE=$flist
	    THISOPTS="${OPTS}" 
	    #THISOPTS="${OPTS} --parameter pcaCaloAxisTrackingAlgo,edepThreshold,0.020 --parameter caloDBSCANClustering,threshold,$TH"
	    #THISOPTS="${OPTS} --parameter pcaCaloAxisTrackingAlgo,edepThreshold,0.020"
       	    #echo "$NAME $SUFFIX $LISTFILE $THISOPTS"
	    ./submit_one_job.sh $SUFFIX $LISTFILE "$THISOPTS"
	done
    done
done
