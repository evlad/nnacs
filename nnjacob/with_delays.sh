#!/bin/bash

# Prepare NNP input from given series with delays condsidering given
# maximum delay, number of pikes and relaxation period.

data=$1
maxdelay=$2

npikes=$3
relaxp=$4

for i in `seq $npikes` ; do
  startpike=`expr $relaxp \* $i`
  ydelays=""
  for d in `seq $maxdelay` ; do
    delayedpike=`expr $startpike + $d - 1`
    ydelays="$ydelays `head -n $delayedpike $data|tail -1`"
  done
  echo $ydelays
done
