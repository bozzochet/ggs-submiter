#!/bin/bash
LISTDIR=${STORAGE}/herd-vv-svn/analysis/calo/submit/htc/lists
#for STREAM in "electrons_1000GeV_10000GeV_E-1" "electrons_10GeV_1000GeV_E-1" 
#for STREAM in "electrons_1000GeV_10000GeV_E-1" "electrons_10GeV_1000GeV_E-1" "protons_10GeV_10000GeV_E-1"
#for STREAM in "protons_10GeV_10000GeV_E-1"
#for STREAM in "electrons_1000GeV_10000GeV_E-1" "electrons_10GeV_1000GeV_E-1" "protons_10GeV_10000GeV_E-1"

OPTS=""

#STREAM="electrons_10GeV_1000GeV_E-1"
#OPTS="${OPTS} --parameter histo*,naxisbins,100 --parameter histo*,axismin,10 --parameter histo*,axismax,100000"
#OPTS="${OPTS} --parameter mcgenspectrum,naxisbin,150 --parameter mcgenspectrum,axismin,1 --parameter mcgenspectrum,axismax,100000 --parameter mcgenspectrum,minmom,10 -p mcgenspectrum,maxmom,1000"

#STREAM="electrons_1000GeV_10000GeV_E-1"
#OPTS="${OPTS} --parameter histo*,naxisbins,100 --parameter histo*,axismin,10 --parameter histo*,axismax,100000"
#OPTS="${OPTS} --parameter mcgenspectrum,naxisbin,150 --parameter mcgenspectrum,axismin,1 --parameter mcgenspectrum,axismax,100000 --parameter mcgenspectrum,minmom,1000 -p mcgenspectrum,maxmom,10000"

#STREAM="protons_10GeV_10000GeV_E-1"
#OPTS="${OPTS} --parameter histo*,naxisbins,100 --parameter histo*,axismin,10 --parameter histo*,axismax,100000"
#OPTS="${OPTS} --parameter mcgenspectrum,naxisbin,150 --parameter mcgenspectrum,axismin,1 --parameter mcgenspectrum,axismax,100000 --parameter mcgenspectrum,minmom,10 -p mcgenspectrum,maxmom,10000"

#STREAM="protons_10GeV"
OPTS="${OPTS} --parameter histo*,naxisbins,100 --parameter histo*,axismin,10 --parameter histo*,axismax,100000"
OPTS="${OPTS} --parameter mcgenspectrum,naxisbin,150 --parameter mcgenspectrum,axismin,1 --parameter mcgenspectrum,axismax,100000 --parameter mcgenspectrum,minmom,10 -p mcgenspectrum,maxmom,1000"

for STREAM in for STREAM in "protons_10GeV" "protons_50GeV" "protons_100GeV" "protons_500GeV" "protons_1TeV" "protons_10TeV" "protons_50TeV" "protons_10GeV_1000GeV_E-1" "electrons_10GeV_1000GeV_E-1"
do
    for flist in `ls ${LISTDIR}/${STREAM}/*.list.txt`
    do
	echo "LIST: $flist"
	#for TH in 0.001 0.002 0.005 0.010 0.015 0.020 0.030 0.050 0.100
	for TH in  0.020
	do
	    BASENAME=`basename $flist .list.txt`
	    #NAME="$BASENAME.edepthreshold_$TH"
	    NAME="$BASENAME"
	    LISTFILE=$flist
	    OPTS="${OPTS} --parameter pcaCaloAxisTrackingAlgo,edepThreshold,$TH"
       	    echo "$NAME $LISTFILE $OPTS"
	    ./submit_one_job.sh $NAME $LISTFILE "$OPTS"
	done
    done
done
