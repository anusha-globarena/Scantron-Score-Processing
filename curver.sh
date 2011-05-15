#!/bin/bash
#
# Add a flat curve to a bunch of scores, scale by a factor, and replace ',' by ' '.
#

# Need at least 2 arguments
if test -z "$1" -o -z "$2"; then
    echo "USAGE: $0 <File containing scores> <Flat curve> [Scale Factor]"
    echo ""
    echo " * <File containing scores> is a file containing one line for each student, with the format EID,Score."
    echo " * <Flat curve> is a flat number to be added to each score *before scaling*. Could be a float."
    echo " * [Scale Factor] is the factor by which the scores must be scaled. By default, this is 1."
    echo " * The number of decimal places in the output depends on the number of decimal places in the scale factor and the curve etc. Math is done using bc"
    exit 1;
fi;

# Read arguments
scorefile=$1;
curve=$2;
scale=$3;

# Sub in default values
if [ -z $scale ]; then
    scale="1";
fi;

# Report missing score file
if [ ! -f $scorefile ]; then
    echo "Score file $scorefile does not exist or is not a regular file"
    exit 1;
fi;

# Do the actual computation
for record in `cat $scorefile`; do
    # Extract scores and EIDs
    score=`echo $record | sed "s/^.*,//"`; 
    eid=`echo $record | sed "s/,.*$//"`;

    # Do the math
    finalscore=`echo "(${score}+${curve})*${scale}" | bc`
    
    # Throw out EID <space> finalscore
    echo "$eid $finalscore";
done;