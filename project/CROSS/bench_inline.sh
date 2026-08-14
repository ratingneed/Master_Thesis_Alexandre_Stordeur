#!/bin/bash

if [ "$1" = "avx2" ]
then
    cd ./Additional_Implementations/Benchmarking
    dir="build_avx2_inline/bin"
    log="log_bench_avx2_inline.txt"
elif [ "$1" = "avx2_loop" ]
then
    cd ./Additional_Implementations/Benchmarking
    dir="build_avx2_loop/bin"
    log="log_bench_avx2_loop.txt"
elif [ "$1" = "avx2_condition" ]
then
    cd ./Additional_Implementations/Benchmarking
    dir="build_avx2_condition/bin"
    log="log_bench_avx2_condition.txt"
elif [ "$1" = "neon" ]
then
    cd ./Additional_Implementations/NeonCROSS
    dir="build_neon_inline/bin"
    log="log_bench_neon_inline.txt"
elif [ "$1" = "neon_loop" ]
then
    cd ./Additional_Implementations/NeonCROSS
    dir="build_neon_loop/bin"
    log="log_bench_neon_loop.txt"
elif [ "$1" = "neon_condition" ]
then
    cd ./Additional_Implementations/NeonCROSS
    dir="build_neon_condition/bin"
    log="log_bench_neon_condition.txt"
else
    echo "Choose between: avx2, avx2_loop, avx2_condition, neon, neon_loop, neon_condition"
    exit 1
fi

> "./../../$log"

for i in `ls $dir/CROSS_benchmark*`
do
  echo $i
  echo Benchmarking $i >> "./../../$log"
  taskset --cpu-list 0 $i -T 2>&1 |grep -e "KeyGen " -e "Sign " -e "Verif " >> "./../../$log"
done

# sound
paplay /usr/share/sounds/freedesktop/stereo/complete.oga