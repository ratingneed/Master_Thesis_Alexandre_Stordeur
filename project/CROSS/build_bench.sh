#!/bin/bash

if [[ "$1" == "reference" ]]; then
    cd ./Additional_Implementations/Benchmarking
    mkdir -p build_ref
    cd build_ref
    cmake ../ -DREFERENCE=1
    make
elif [[ "$1" == "avx2" ]]; then
    cd ./Additional_Implementations/Benchmarking
    mkdir -p build_avx2
    cd build_avx2
    cmake ../
    make
elif [[ "$1" == "armref" ]]; then
    cd ./Additional_Implementations/NeonCROSS
    mkdir -p build_armref
    cd build_armref
    cmake ../ -DREFERENCE=1  # alternatively, build reference implementation
    make
elif [[ "$1" == "neon" ]]; then
    cd ./Additional_Implementations/NeonCROSS
    mkdir -p build_neon
    cd build_neon
    cmake ../ -DREFERENCE=3    # build with Neon optimization
    make
elif [[ "$1" == "clean_all" ]]; then

    rm -rf ./Additional_Implementations/Benchmarking/build_ref
    rm -rf ./Additional_Implementations/Benchmarking/build_avx2
    rm -rf ./Additional_Implementations/NeonCROSS/build_armref
    rm -rf ./Additional_Implementations/NeonCROSS/build_neon

    echo "all cleaned"
else
    echo "Choose between: reference, avx2, armref, neon"
    exit 1
fi

# sound
paplay /usr/share/sounds/freedesktop/stereo/complete.oga