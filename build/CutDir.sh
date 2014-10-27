#!/bin/sh
FILELIST=`ls $1`
for file in $FILELIST
do
    OUTPUT=${1}"/"${file}"_result"
    echo $OUTPUT
    ./test $file 1>$OUTPUT
done
