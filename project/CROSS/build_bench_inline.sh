#!/bin/bash

if [ $# -eq 0 ]
then
    echo "Usage: $0 [inline|loop|condition|neon_inline|neon_loop|neon_condition]"
    exit 1
fi

if [ "$1" = "avx2" ]
then
    cd ./Additional_Implementations/Benchmarking
    mkdir -p build_avx2_inline
    cd build_avx2_inline
    cmake -DENABLE_CYCLE_MAIN=ON ../
    make
elif [ "$1" = "avx2_loop" ]
then
    cd ./Additional_Implementations/Benchmarking
    mkdir -p build_avx2_loop
    cd build_avx2_loop
    cmake -DENABLE_CYCLE_MAIN_LOOP=ON ../
    make
elif [ "$1" = "avx2_condition" ]
then
    cd ./Additional_Implementations/Benchmarking
    mkdir -p build_avx2_condition
    cd build_avx2_condition
    cmake -DENABLE_CYCLE_MAIN_CONDITION=ON ../
    make
elif [ "$1" = "neon" ]
then
    cd ./Additional_Implementations/NeonCROSS
    mkdir -p build_neon_inline
    cd build_neon_inline
    cmake -DREFERENCE=3 -DENABLE_CYCLE_MAIN=ON ../
    make
elif [ "$1" = "neon_loop" ]
then
    cd ./Additional_Implementations/NeonCROSS
    mkdir -p build_neon_loop
    cd build_neon_loop
    cmake -DREFERENCE=3 -DENABLE_CYCLE_MAIN_LOOP=ON ../
    make
elif [ "$1" = "neon_condition" ]
then
    cd ./Additional_Implementations/NeonCROSS
    mkdir -p build_neon_condition
    cd build_neon_condition
    cmake -DREFERENCE=3 -DENABLE_CYCLE_MAIN_CONDITION=ON ../
    make
elif [ "$1" = "clean_all" ]
then
    rm -rf ./Additional_Implementations/Benchmarking/build_avx2_inline
    rm -rf ./Additional_Implementations/Benchmarking/build_avx2_loop
    rm -rf ./Additional_Implementations/Benchmarking/build_avx2_condition

    rm -rf ./Additional_Implementations/NeonCROSS/build_neon_inline
    rm -rf ./Additional_Implementations/NeonCROSS/build_neon_loop
    rm -rf ./Additional_Implementations/NeonCROSS/build_neon_condition

    echo "all cleaned"
else
    echo "Choose between: avx2, avx2_loop, avx2_condition, neon, neon_loop, neon_condition"
    exit 1
fi

# sound
paplay /usr/share/sounds/freedesktop/stereo/complete.oga
