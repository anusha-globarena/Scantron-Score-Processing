#!/bin/bash
#
# Generate a histogram from quest-formatted scores
#

file=$1
maxstudents=122

for i in `seq 1 $maxstudents`; do 
    numbers=`cat $file | grep ",$i$" | wc -l`
    if test $numbers -ne 0; then
	echo $i, $numbers
    fi;
done;
