#!/bin/bash

if [[ "$1" == "avx2" ]]; then

    log="log_bench_avx2_inline.txt"

    > $log

    for i in `ls -d ./Optimized_Implementation/mqom2_cat*`
        do
        cd $i
        echo Benchmarking $(basename "$i")
        echo $(basename "$i") >> "./../../$log"
        ./bench 10000 | sed 's/^.*-//' | grep -e "KeyGen " -e "Sign " -e "Verif " >> "./../../$log"
        cd ../..
    done

elif [[ "$1" == "neon" ]]; then

    log="log_bench_neon_inline.txt"

    > $log

    for i in `ls -d ./NEON_Implementation/mqom2_cat*`
        do
        cd $i
        echo Benchmarking $(basename "$i")
        echo $(basename "$i") >> "./../../$log"
        ./bench 10000 | sed 's/^.*-//' | grep -e "KeyGen " -e "Sign " -e "Verif " >> "./../../$log"
        cd ../..
    done
else
    echo "Choose between: avx2, neon"
    exit 1
fi

# sound
paplay /usr/share/sounds/freedesktop/stereo/complete.oga