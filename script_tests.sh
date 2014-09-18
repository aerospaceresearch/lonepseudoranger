#!/bin/bash

args=("$@")
DIR=${args[0]}
echo $DIR

if [ -d $DIR ]; then
    echo "Directory exists"
else
    echo "Directory does not exist"
fi

for FILE in $( ls $DIR/ )
do
    if [ -d $DIR/$FILE ]; then
        echo $FILE;
        ./lonepseudoranger -i $DIR/$FILE/beacon.sat -o "stats$FILE.csv" -d $DIR/$FILE
    fi
done
