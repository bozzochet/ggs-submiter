#!/bin/bash

NAME="electrons_10GeV_1000GeV_E-1"
LISTFILE=lists/${NAME}.list



LISTFILE=lists/${NAME}.list

fperjob=1

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
      exit
  fi


done < $LISTFILE

if [ $n -gt 0 ]; then
    cmd="./submit_one_job.sh $NAME $flist"
    echo $cmd
    $cmd
fi
