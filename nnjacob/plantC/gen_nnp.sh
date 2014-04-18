#!/bin/bash

for d in 1 2 3 ; do
  mkdir -p nnp_1+${d}
  cd nnp_1+${d}
  for hL in 0 1 2 ; do
    tL=`expr $hL + 1`
    mkdir -p $tL
    cd $tL
    ../../../prepare_nnp.sh nnp_ini $d $hL
    cd ..
  done
  cd ..
done
