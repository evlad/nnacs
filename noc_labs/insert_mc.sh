#!/bin/bash

# Take pairs of labels and replace each encountered the second label
# (the russian one) by [mc "english label"]

cp /dev/null changed_files
cp /dev/null insert_mc.log
while read line ; do
  englabel=${line%\" \"*}\"
  ruslabel=\"${line#*\" \"}
  echo "$line: rus=$ruslabel eng=$englabel"
  echo -n "  Files changed: "
  i=0
  for tclfile in *.tcl ; do
    if [ -f $tclfile.mc ] ; then
      origfile=$tclfile.mc
    else
      origfile=$tclfile
    fi
    cp $origfile $tclfile.tmp
    sed "s|$ruslabel|[mc $englabel]|g" $tclfile.tmp >$tclfile.mc
    if ! diff $tclfile.tmp $tclfile.mc >/dev/null ; then
      i=`expr $i + 1`
      if ! grep -qw $tclfile changed_files ; then
	echo $tclfile >>changed_files
      fi
      echo -n "$tclfile "
    fi
    echo
    rm $tclfile.tmp
  done
done <dup-uniq-labels >insert_mc.log

for tclfile in *.tcl ; do
  if [ -f $tclfile.mc ] ; then
    mv $tclfile $tclfile.orig
    mv $tclfile.mc $tclfile
    echo "$tclfile"
  fi
done
