#!/bin/bash

# This is to run the script

if [ ! -e $1 ]; then
    echo "Inputfile doesn't exist. Exitting..."
    exit 1
fi
if [ ! -e $2 ]; then
    touch $2
fi   
if [ -e "a.out" ]; then
    ./a.out $1 $2
else
    echo "First run compile.sh to compile the code"
fi
