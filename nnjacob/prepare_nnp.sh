#!/bin/bash

prefix=${1:-newnnp}
d=${2:-1}
hL=${3:-0}
case $hL in
  0)
    layers=
    ;;
  1)
    layers=5
    ;;
  2)
    layers="5 3"
    ;;
esac
MakeNN ${prefix}.nn Plant 1 1 1 $d 0 0 $hL $layers
