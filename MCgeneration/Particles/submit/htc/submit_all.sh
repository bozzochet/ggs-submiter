#!/bin/bash

WORKDIR=/storage/gpfs_data/herd/vvagelliherd/herd-vv-svn/MCgeneration/Particles/
GEODATACARD=${WORKDIR}/datacards/geometry_calov2_3030_caloonly.mac

NAME="electrons_1TeV_100TeV_E-1_3030_caloonly"
NMAX=400
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
