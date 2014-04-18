#!/bin/bash

for n in 1 2 3 ; do
  for l in 1 2 3 ; do
    tail -1 nnp_1+$n/$l/nnp_trace.dat|awk '{print $6}'
  done
done >test_mse.dat
