#!/bin/bash

${TESTPROG:=./TrainNN.lm_pure}
MAX_EPOCHS=10 $TESTPROG test2_1.nn xor_in.dat xor_out.dat  xor_in.dat xor_out.dat xor_results.nn >_temp.log

if diff _temp.log xor_results_lm.log ; then
  echo "LM test passed"
else
  echo "LM test failed"
fi
