#!/bin/bash

#NAME="protons_10GeV_10000GeV_E-1"
#LISTFILE=lists/protons_10GeV_10000GeV_E-1.list
NAME="electrons_1000GeV_10000GeV_E-1"
LISTFILE=lists/electrons_1000GeV_10000GeV_E-1.list

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
  fi

done < $LISTFILE

if [ $n -gt 0 ]; then
    cmd="./submit_one_job.sh $NAME $flist"
    echo $cmd
    $cmd
fi
