#!/bin/bash

while read labels ; do
  echo "::msgcat::mcset ru_RU $labels"
done
