#!/bin/bash

NAME="electrons_sphere_10GeV_1000GeV_E-1"
LISTFILE=lists/electrons_sphere_10GeV_1000GeV_E-1.list

i=0
while read f; do
  echo "$i $f"
  i=$[$i+1]
  ./submit_one_job.sh $NAME $f
done < $LISTFILE

