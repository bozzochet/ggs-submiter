#!/bin/bash

WORKDIR=/storage/gpfs_data/herd/vvagelliherd/herd-vv-svn/MCgeneration/Particles/
GEODATACARD=${WORKDIR}/datacards/geometry_calov2_stk_psdbar.mac

NAME="protons_100TeV"
NMAX=1500
i=500

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
