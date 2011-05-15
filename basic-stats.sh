#!/bin/bash
#
# This program does the basic stats (average, standard deviation), and
# generates the histogram
#

if test -z $1; then
    echo "USAGE: $0 <Quest-formatted score file>"
    exit 1;
fi;


# Generate a histogram from quest-formatted scores
#

file=$1
maxstudents=122
histogramfile="histfile.txt"

echo "" > $histogramfile

sum=0;
squaresum=0;
nstud=0;

for i in `seq 1 $maxstudents`; do 
    numbers=`cat $file | grep ",$i$" | wc -l`
    if test $numbers -ne 0; then
	echo $i, $numbers >> $histogramfile
	let sum=$sum+\($numbers*$i\)
	let squaresum=$squaresum+\($numbers*$i*$i\)
	let nstud=$nstud+\($numbers\)
    fi;
done;
echo "[INFO] We seem to have $nstud students"
mean=`echo "scale=4; $sum/$nstud" | bc`
var=`echo "scale=4; ($squaresum/$nstud)-$mean*$mean" | bc`
sd=`echo "scale=4; sqrt($var)" | bc`

echo "[INFO] Sum(scores) = $sum; Sum(scores^2) = $squaresum"
echo "MEAN: $mean"
echo "VARIANCE: $var"
echo "STD. DEVIATION: $sd"

# Plot histogram
gnuplottempfile="/tmp/scantron-gnuplotfile"
histogramplotfile="histogram.png"

echo "" > $gnuplottempfile
echo "set term png" >> $gnuplottempfile
echo "set output \"$histogramplotfile\"" >> $gnuplottempfile
echo "plot \"$histogramfile\" using 1:2 with boxes title \"Histogram\"" >> $gnuplottempfile

gnuplot $gnuplottempfile

echo "[INFO] Finished. Find the histogram at $histogramplotfile"
