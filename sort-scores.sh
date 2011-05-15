#!/bin/bash
#
# EID[Separator]Score --> Score,EID (or vice versa)
#
# USAGE: <script>.sh [Separator]
# Reades scores from stdin
# To sort scores: <script>.sh [Separator] | sort | <script>.sh

scorefile=$1;
sep=$2;

if [ -z $sep ]; then
    sep=',';
fi;

for line in `cat $scorefile`; do
    eid=`echo $line | sed 's/^.*/