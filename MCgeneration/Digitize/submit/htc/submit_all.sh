#!/bin/bash

#NAME="protons_500GeV"
#LISTFILE=lists/${NAME}.list

#for NAME in "protons_10GeV"  "protons_50GeV" "protons_100GeV" "protons_500GeV" "protons_1TeV" "protons_5TeV" "protons_10TeV" "protons_50TeV" "protons_100TeV" "protons_10GeV_1000GeV_E-1" "protons_1TeV_100TeV_E-1" "electrons_10GeV_1000GeV_E-1" "electrons_1TeV_100TeV_E-1"

only_one_submit=0

#for NAME in "protons_10GeV_1000GeV_E-1" "protons_1TeV_100TeV_E-1" "electrons_10GeV_1000GeV_E-1" "electrons_1TeV_100TeV_E-1"
for NAME in "electrons_10GeV_1000GeV_E-1" "protons_10GeV_1000GeV_E-1" 
do
    LISTFILE=lists/${NAME}.list

    fperjob=5

    i=0
    flist=""
    n=0

    while read f; do
	i=$[$i+1]

	flist="$flist $f"
	n=`echo $flist | wc -w`
	
	if [ $n -eq ${fperjob} ]; then 
	    cmd="./submit_one_job.sh $NAME $flist"
	    echo $cmd
	    $cmd
	    flist=""
	    if [ $only_one_submit -eq 1 ]; then
		break
	    fi
	fi
	
	
    done < $LISTFILE

    if [ $n -gt 0 ]; then
	cmd="./submit_one_job.sh $NAME $flist"
	echo $cmd
	$cmd
    fi

done
