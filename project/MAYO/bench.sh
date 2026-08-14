#!/bin/bash

if [[ "$1" == "reference" ]]; then

    log="log_bench_ref.txt"

    for i in "MAYO-1" "MAYO-2" "MAYO-3" "MAYO-5"
        do
        echo Benchmarking $i
        ./Reference_Implementation/build/test/mayo_bench $i 10000 >> "./$log"
        echo >> "./$log"
    done
elif [[ "$1" == "avx2" ]]; then
    log="log_bench_avx2.txt"

    for i in `ls ./Optimized_Implementation/build/test/mayo_bench_MAYO_*`
        do
        echo Benchmarking $(basename "$i")
        $i 10000 >> "./$log"
        echo >> "./$log"
    done
elif [[ "$1" == "armref" ]]; then

    log="log_bench_arm_ref.txt"

    for i in "MAYO-1" "MAYO-2" "MAYO-3" "MAYO-5"
        do
        echo Benchmarking $i
        ./Reference_Implementation/build_arm/test/mayo_bench $i 10000 >> "./$log"
        echo >> "./$log"
    done
elif [[ "$1" == "neon" ]]; then
    log="log_bench_neon.txt"

    for i in `ls ./Additional_Implementations/NEON/build/test/mayo_bench_MAYO_*`
        do
        echo Benchmarking $(basename "$i")
        $i 10000 >> "./$log"
        echo >> "./$log"
    done
else
    echo "Choose between: reference, avx2, armref, neon"
    exit 1
fi

# sound
paplay /usr/share/sounds/freedesktop/stereo/complete.oga