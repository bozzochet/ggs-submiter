#!/bin/bash

#NAME="protons_500GeV"
#LISTFILE=lists/${NAME}.list

only_one_submit=0

#for NAME in "protons_10GeV_1000GeV_E-1" "protons_1TeV_100TeV_E-1" "electrons_10GeV_1000GeV_E-1" "electrons_1TeV_100TeV_E-1"
WORKDIR=${STORAGE}/herd-vv-svn/MCgeneration/Digitize
SUBMITDIR=${WORKDIR}/submit/htc
DATACARDTEMPLATE=${SUBMITDIR}/digitize.calov2_caloonly.eaconf.template
 
for NAME in "electrons_10GeV_1000GeV_E-1_2828_caloonly" "electrons_10GeV_1000GeV_E-1_3030_caloonly"
do
    LISTFILE=lists/${NAME}.list

    fperjob=20

    i=0
    flist=""
    n=0

    while read f; do
	i=$[$i+1]

	flist="$flist $f"
	n=`echo $flist | wc -w`
	
	if [ $n -eq ${fperjob} ]; then 
	    cmd="./submit_one_job.sh $NAME ${DATACARDTEMPLATE} $flist" 
	    echo $cmd
	    $cmd
	    flist=""
	    n=0
	    if [ $only_one_submit -eq 1 ]; then
		break
	    fi
	fi
	
	
    done < $LISTFILE

     if [ $n -gt 0 ]; then
	cmd="./submit_one_job.sh $NAME $flist ${DATACARDTEMPLATE}"
	echo $cmd
	$cmd
    fi

done
