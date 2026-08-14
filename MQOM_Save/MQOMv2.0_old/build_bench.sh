#!/bin/bash

if [[ "$1" == "reference" ]]; then
    for i in `ls -d ./Reference_Implementation/mqom2_cat*`
        do
        cd $i
        echo make bench for $i
        make bench > /dev/null
        cd ../..
    done

else
    for i in `ls -d ./Optimized_Implementation_AVX2/mqom2_cat*`
        do
        cd $i
        echo make bench for $i
        make bench > /dev/null
        cd ../..
    done
fi