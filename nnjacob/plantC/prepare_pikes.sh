#!/bin/bash

../gen_pikes.sh 30 100 >pikes30.dat
dtf plant.tf pikes30.dat ypikes30.dat
djacob pikes30.dat ypikes30.dat | sed 's/nan/0/g;s/inf/0/g' >j_ideal.dat
