#!/bin/sh
./dstatsb 0 1 "ALL" dstatsb_test.dat dstatsb_test_slbase=0.dat
./dstatsb 2 1 "MEAN,RMS,TIME" dstatsb_test.dat dstatsb_test_slbase=2.dat
./dstatsb 3 1 "MEAN,RMS,TIME" dstatsb_test.dat dstatsb_test_slbase=3.dat
./dstatsb 4 1 "MEAN,RMS,TIME" dstatsb_test.dat dstatsb_test_slbase=4.dat
