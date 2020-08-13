#!/bin/bash

N=502
i=500

while [ ${i} -lt ${N} ]
do
    echo "$i $N"
    ii=`printf "%04d" $i`
    cmd="./submit_one_job.sh ${ii}"
    echo $cmd
    echo ""
    $cmd
    i=$[$i+1]
done
