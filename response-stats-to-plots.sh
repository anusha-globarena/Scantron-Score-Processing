#!/bin/bash

if [ -z $1 ]; then
    echo "USAGE: $0 <Response Statistics File>"
    exit 1;
fi;

datafile=$1;
plotfileprefix=$datafile; # The plots are prefixed by the data file name

#### Generate plots as PDFs
# For each question...
for line in `cat $datafile | grep -v ^#`; do

    qnumnozeroes=`echo $line | sed 's/,.*$//'`
    qnum=`echo $qnumnozeroes | sed 's/^\s*/00/' | sed 's/^.*\([0-9][0-9][0-9]\)$/\1/'` # Question number padded with zeros


    # Generate a plot data file
    tempfile="/tmp/scantron-plotdatafile"
    responsedata=`echo $line | sed 's/\(,0\)*$//'`; # Remove trailing zeros
    echo $responsedata | sed 's/^[0-9]*,//;s/,/\n/g;' > $tempfile # Produces the list of percentages (Y-axis) without the option numbers (X-axis)


    # Generate a command set for gnuplot
    gnuplottempfile="/tmp/scantron-gnuplotfile"
    nlines=`cat $tempfile | wc -l`; # The number of lines is 1 more the number of options.
    echo "set terminal pdfcairo; set output \"${plotfileprefix}-question-${qnum}.pdf\";" > $gnuplottempfile # Set terminal type and output file name
    echo "set xtics 1,1,$(($nlines-1)); set ytics 0,10,100;" >> $gnuplottempfile
    echo "set title \"Question $qnumnozeroes\"" >> $gnuplottempfile
#    echo "set xlabel \"Options\"; set ylabel \"Percentage\";" >> $gnuplottempfile

    incrementer=0;
    for value in `cat $tempfile`; do
	if [ $incrementer -ne 0 ]; then # Except for the initial 0 entry, produce a percentage label
	    roundedvalue=`echo $value | sed 's!\([0-9]*\)\.[0-5].*$!\1!;s!\([0-9]*\)\.[6-9].*$!\1+1!' | bc` # Round the value to its nearest integer
	    x="$(($incrementer-1)).85";
	    y=$(($roundedvalue+3));
	    echo "set label \"${roundedvalue}%\" at $x, $y" >> $gnuplottempfile # Put a label at the appropriate location
	fi;
	incrementer=$(($incrementer+1))
    done;

    echo "plot [x=0:$nlines] [y=0:110] \"$tempfile\" with boxes title \"\"" >> $gnuplottempfile

    # Run GNUPlot to produce the plot
    gnuplot $gnuplottempfile
done;

#### Collate all plots into a single PDF using pdflatex
latexttempfile="scantron-latexfile.tex"
echo "\documentclass{article}" > $latexttempfile
echo "\usepackage{graphicx}" >> $latexttempfile
echo "\usepackage[top=0.5cm, bottom=0.5cm, left=0.5cm, right=0.5cm]{geometry}" >> $latexttempfile
echo "\begin{document}" >> $latexttempfile
echo "\begin{table}[h!]" >> $latexttempfile
echo "\begin{center}" >> $latexttempfile
echo "\begin{tabular}{lr}" >> $latexttempfile
odd=1;
count=0;
for file in ${plotfileprefix}-question-*.pdf; do
    echo "  \includegraphics[scale=0.8]{$file}" >> $latexttempfile
    echo "" >> $latexttempfile
    count=$(($count+1))
    if [ $odd -ne 0 ]; then
	echo "&" >> $latexttempfile
	odd=0;
    else
	echo '\\' >> $latexttempfile
#	echo '\hline' >> $latexttempfile
	odd=1;
    fi;
    if [ $count -eq 8 ]; then
	# Break to the next page
	echo "\end{tabular}" >> $latexttempfile
	echo "\end{center}" >> $latexttempfile
	echo "\end{table}" >> $latexttempfile
#	echo "\\" >> $latexttempfile
	echo "\begin{table}[h!]" >> $latexttempfile
	echo "\begin{center}" >> $latexttempfile
	echo "\begin{tabular}{lr}" >> $latexttempfile
	count=0;
    fi;
done;
echo "\end{tabular}" >> $latexttempfile
echo "\end{center}" >> $latexttempfile
echo "\end{table}" >> $latexttempfile
echo "\end{document}" >> $latexttempfile

pdflatex -interaction=nonstopmode $latexttempfile > /dev/null

# Also make a multi-page PDF with all the plots
latexttempfile2="multi-page-all-plots.tex"
echo "\documentclass{article}" > $latexttempfile2
echo "\usepackage{graphicx}" >> $latexttempfile2
echo "\usepackage[landscape,top=1.0cm,bottom=1.0cm,left=1.0cm,right=2.0cm]{geometry}" >> $latexttempfile2
echo "\begin{document}" >> $latexttempfile2
echo "\pagestyle{empty}" >> $latexttempfile2
echo "\begin{center}" >> $latexttempfile2
for file in ${plotfileprefix}-question-*.pdf; do
    echo "\begin{figure}[hbtp]" >> $latexttempfile2
    echo "  \includegraphics[scale=2.0]{$file}" >> $latexttempfile2
    echo "\end{figure}" >> $latexttempfile2
    echo "\end{center}" >> $latexttempfile2
    echo "\begin{center}" >> $latexttempfile2
#    echo "\\" >> $latexttempfile
done;
echo "\end{center}" >> $latexttempfile2
echo "\end{document}" >> $latexttempfile2

pdflatex -interaction=nonstopmode $latexttempfile2 > /dev/null

# Move plot files to a dedicated directory
mkdir -p plots
mv -f ${plotfileprefix}-question-*.pdf plots/

## Tarball all the individual response files
tar czf ${plotfileprefix}-all-plots.tar.gz plots/${plotfileprefix}-question-*.pdf

# Cleanup
rm -f plots/${plotfileprefix}-question-*.pdf
rmdir --ignore-fail-on-non-empty plots
rm -f $gnuplottempfile
rm -f $tempfile
rm -f $latexttempfile
mv `echo $latexttempfile | sed 's!\.tex$!.pdf!'` ${plotfileprefix}-collated-plots.pdf
rm -f `echo $latexttempfile | sed 's!\.tex$!.*!'`
rm -f $latexttempfile2
mv `echo $latexttempfile2 | sed 's!\.tex$!.pdf!'` ${plotfileprefix}-multi-page-all-plots.pdf
rm -f `echo $latexttempfile2 | sed 's!\.tex$!.*!'`
