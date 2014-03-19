#!/bin/bash

u_pikes=../pikes100.dat
nninp=1+2

echo "Direct modeling"
dtf ../plant.tf ${u_pikes} y_pikes.dat

echo "Jacobian estimation"
djacob ${u_pikes} y_pikes.dat | \
  sed 's/nan/0/g;s/inf/0/g' >j_ideal.dat

echo "Neural net plant output estimation"
echo 0 >y_pikes+1.dat
cat y_pikes.dat >>y_pikes+1.dat
paste ${u_pikes} y_pikes.dat y_pikes+1.dat >nnp_in.dat

echo "Linear estimation of NN Jacobian"
for d in 1 2 3 ; do
  EvalNN $d/nnp_res.nn nnp_in.dat $d/nnp_out.dat
  djacob ${u_pikes} $d/nnp_out.dat | \
    sed 's/nan/0/g;s/inf/0/g' >j_nnp_${nninp}_$d.dat
done

# back_in.dat is list of pike lines from nnp_in.dat

echo "NN backpropagated estimation of Jacobian"
for d in 1 2 3 ; do
  BackNN $d/nnp_res.nn back_in.dat dest_out.dat $d/back_out.dat $d/err_in.dat
  paste back_in.dat $d/err_in.dat|awk '{print -$2/$4}' >j_bpe_${nninp}_$d.dat
done
