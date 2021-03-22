#!/bin/bash

WORKDIR=${STORAGE}/MoriStuff/install/ggs-submiter/MCgeneration/Particles/
GEODATACARD=${WORKDIR}/datacards/geo.mac

#NAME="run_ele_hs_1000" 
#NAME="run_ele_hs_100"
NAME="run_pro_hs_1000"
#NAME="run_pro_hs_100"
NMAX=100
i=0

while [ ${i} -lt ${NMAX} ]
do
    echo "$i $NMAX"
    ii=`printf "%04d" $i`
    cmd="./submit_one_job.sh ${NAME} ${GEODATACARD} ${ii}"
    echo $cmd
    echo ""
    $cmd
    i=$[$i+1]
done
