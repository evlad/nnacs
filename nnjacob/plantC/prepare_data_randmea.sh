#!/bin/bash

tr=0tr.randmea
ev=0ev.randmea

drandmea 500 20 -3 3 >$tr/u.dat
dtf plant.tf $tr/u.dat $tr/y.dat
drand 500 0 0.1 >$tr/n.dat
dsum $tr/y.dat $tr/n.dat >$tr/ny.dat

drandmea 500 10 20 -3 3 >$ev/u.dat
dtf plant.tf $ev/u.dat $ev/y.dat
drand 500 0 0.1 >$ev/n.dat
dsum $ev/y.dat $ev/n.dat >$ev/ny.dat
