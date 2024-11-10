#!/bin/sh

writefile=$1
writestr=$2

writedir=$(dirname "$writefile")

if [ -z "$writefile" -o -z "$writestr" ]; then
    echo "Wrong Params"
    exit 1
fi

if [ ! -d "$writedir" ]; then
    mkdir -p "$writedir"
    if [ ! $? -eq 0 ]; then
        exit 1
    fi
fi

echo "$writestr" > $writefile
