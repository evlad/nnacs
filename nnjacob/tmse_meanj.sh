#!/bin/bash

../extract_test_mse.sh
../extract_mean_j_bpe.sh
paste test_mse.dat mean_j_bpe.dat | awk '{print $1, $2, $4}' >tmse_meanj.dat
awk '{print $1, $2}' tmse_meanj.dat >tmse_meanj2.dat
