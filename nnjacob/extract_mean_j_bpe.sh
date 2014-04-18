#!/bin/bash

for n in 1 2 3 ; do
  for l in 1 2 3 ; do
    dstat nnp_1+$n/j_bpe_nnp_1+${n}_L$l.dat  | tail -1 |awk -v s="1+${n}_L$l" '{print $1, $2, s}'
  done
done >mean_j_bpe.dat
