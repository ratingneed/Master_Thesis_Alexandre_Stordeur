#!/bin/bash

if [[ "$1" == "reference" ]]; then

    log="log_bench_ref.txt"

    for i in `ls -d ./Reference_Implementation/mqom2_cat*`
        do
        cd $i
        echo Benchmarking $(basename "$i")
        echo -n "$(basename "$i") - " >> "./../../$log"
        ./bench 10 | grep cycles  >> "./../../$log"
        cd ../..
    done

else

    log="log_bench_avx2.txt"

    for i in `ls -d ./Optimized_Implementation_AVX2/mqom2_cat*`
        do
        cd $i
        echo Benchmarking $(basename "$i")
        echo -n "$(basename "$i") - " >> "./../../$log"
        ./bench 500 | grep cycles >> "./../../$log"
        cd ../..
    done
fi
