#!/bin/bash

npikes=30

for d in 1 2 3 ; do
  nninp=nnp_1+${d}
  cd $nninp || exit 1

  echo "prepare data for jacobian estimation"
  ../../with_delays.sh ../ypikes$npikes.dat $d $npikes 100 >ypikes${npikes}_+${d}.dat
  ../../with_delays.sh ../pikes$npikes.dat 1 $npikes 100 >pikes${npikes}.dat
  paste pikes${npikes}.dat ypikes${npikes}_+${d}.dat >back_in.dat
  drand $npikes 0 0 >dest_out.dat

  for hL in 0 1 2 ; do
    tL=`expr $hL + 1`
    cd $tL || exit 1
    echo "training $nninp/$tL ..."
    dplantid ../../dplantid_template.par >dplantid_out.txt
    echo "evaluating $nninp/$tL ..."
    BackNN nnp_res.nn ../back_in.dat ../dest_out.dat back_out.dat err_in.dat
    paste ../back_in.dat err_in.dat|awk -v n=`expr 2 + $d` '{print -$2/$n}' >../j_bpe_${nninp}_L${tL}.dat
    cd ..
  done
  cd ..
done
