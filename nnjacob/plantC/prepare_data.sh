#!/bin/bash

for d in 0tr 0ev ; do
  mkdir -p $d
  cd $d || exit 1
  for f in u n ; do
    [ ! -f $f.dat ] && gunzip <../../$d/$f.dat.gz >$f.dat
  done
  dtf ../plant.tf u.dat y.dat
  dsum y.dat n.dat >ny.dat
  cd ..
done
