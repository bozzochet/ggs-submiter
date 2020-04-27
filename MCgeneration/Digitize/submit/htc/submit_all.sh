#!/bin/bash

NAME="protons_10GeV_10000GeV_E-1"
LISTFILE=lists/protons_10GeV_10000GeV_E-1.list

i=0
while read f; do
  echo "$i $f"
  i=$[$i+1]
  ./submit_one_job.sh $NAME $f
done < $LISTFILE

