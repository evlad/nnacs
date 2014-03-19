#!/bin/bash

nninp=1+1

# 50 100 500 2000
for me in 50 100 500 2000 ; do
  echo "Max epochs $me"
  for d in 1 2 3 ; do
    echo "  training ${nninp}/$d ..."
    cd $d
    dplantid dplantid.par finish_max_epoch=$me >/dev/null
    cd ..
  done
  echo "  calculating Jacobian ..."
  ./lin_est.sh >/dev/null
  mkdir -p me$me
  cp j_bpe_${nninp}_[123].dat me$me
done
