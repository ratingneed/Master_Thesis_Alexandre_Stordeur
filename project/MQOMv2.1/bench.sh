#!/bin/bash

if [[ "$1" == "reference" ]]; then

    log="log_bench_ref.txt"

    > $log

    for i in `ls -d ./Reference_Implementation/mqom2_cat*`
        do
        cd $i
        echo Benchmarking $(basename "$i")
        echo -n "$(basename "$i") - " >> "./../../$log"
        ./bench 50 | grep "Timing in cycles"  >> "./../../$log"
        cd ../..
    done

elif [[ "$1" == "avx2" ]]; then

    log="log_bench_avx2.txt"

    > $log

    for i in `ls -d ./Optimized_Implementation/mqom2_cat*`
        do
        cd $i
        echo Benchmarking $(basename "$i")
        echo -n "$(basename "$i") - " >> "./../../$log"
        ./bench 10000 | grep "Timing in cycles" >> "./../../$log"
        cd ../..
    done
elif [[ "$1" == "armref" ]]; then

    log="log_bench_armref.txt"

    > $log

    for i in `ls -d ./Reference_Implementation/mqom2_cat*`
        do
        cd $i
        echo Benchmarking $(basename "$i")
        echo -n "$(basename "$i") - " >> "./../../$log"
        ./bench 50 | grep "Timing in cycles"  >> "./../../$log"
        cd ../..
    done

elif [[ "$1" == "neon" ]]; then

    log="log_bench_neon.txt"

    > $log

    for i in `ls -d ./NEON_Implementation/mqom2_cat*`
        do
        cd $i
        echo Benchmarking $(basename "$i")
        echo -n "$(basename "$i") - " >> "./../../$log"
        ./bench 10000 | grep "Timing in cycles" >> "./../../$log"
        cd ../..
    done
else
    echo "Choose between: reference, avx2, armref, neon"
    exit 1
fi

# sound
paplay /usr/share/sounds/freedesktop/stereo/complete.oga