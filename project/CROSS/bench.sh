#!/bin/bash

if [[ "$1" == "reference" ]]; then
    cd ./Additional_Implementations/Benchmarking
    dir="build_ref/bin"
    log="log_bench_ref.txt"
elif [[ "$1" == "avx2" ]]; then
    cd ./Additional_Implementations/Benchmarking
    dir="build_avx2/bin"
    log="log_bench_avx2.txt"
elif [[ "$1" == "armref" ]]; then
    cd ./Additional_Implementations/NeonCROSS
    dir="build_armref/bin"
    log="log_bench_armref.txt"
elif [[ "$1" == "neon" ]]; then
    cd ./Additional_Implementations/NeonCROSS
    dir="build_neon/bin"
    log="log_bench_neon.txt"
else
    echo "Choose between: reference, avx2, armref, neon"
    exit 1
fi

> "./../../$log"

for i in `ls $dir/CROSS_benchmark*`
do

  if [[ "$1" == "reference" || "$1" == "avx2" ]]; then
      # CPU 0 is a P-core on Intel i7-12700
      echo Benchmarking $i
      taskset --cpu-list 0 $i -T 2>&1 | grep TIME >> "./../../$log"
  elif [[ "$1" == "armref" || "$1" == "neon" ]]; then
      # CPU 0 is a P-core on Cortex-A76
      echo Benchmarking $i
      echo Benchmarking $i >> "./../../$log"
      taskset --cpu-list 0 $i -T 2>&1 | grep "average cycles:" >> "./../../$log"
  fi
  
done

# sound
paplay /usr/share/sounds/freedesktop/stereo/complete.oga