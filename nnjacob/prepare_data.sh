#!/bin/bash

for d in 0tr 0ev ; do
  for f in u n ; do
    [ ! -f $f.dat ] && gunzip <$f.dat.gz >$f.dat
  done
  dtf ../plant.tf u.dat y.dat
  dsum y.dat n.dat >ny.dat
done

