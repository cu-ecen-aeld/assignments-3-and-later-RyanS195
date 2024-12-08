#!/bin/sh

filesdir=$1
searchstr=$2

if [ -z "$filesdir" -o -z "$searchstr" ]; then
    return 1
fi

if [ ! -d "$filesdir" ]; then
    return 1
fi

X=$(ls "$filesdir" | wc -l)
Y=$(grep -RI "$searchstr" "$filesdir" | wc -l)

echo "The number of files are ${X} and the number of matching lines are ${Y}"
