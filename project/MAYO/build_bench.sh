#!/bin/bash

if [[ "$1" == "reference" ]]; then
    cd ./Reference_Implementation
    mkdir -p build
    cd build
    cmake -DMAYO_BUILD_TYPE=ref -DENABLE_AESNI=OFF ..
    make
    cd ..
elif [[ "$1" == "avx2" ]]; then
    cd ./Optimized_Implementation
    mkdir -p build
    cd build
    cmake -DMAYO_BUILD_TYPE=avx2 -DENABLE_AESNI=ON ..
    make
    cd ..
elif [[ "$1" == "armref" ]]; then
    cd ./Reference_Implementation
    mkdir -p build_arm
    cd build_arm
    cmake -DMAYO_BUILD_TYPE=ref -DENABLE_AESNI=OFF -DENABLE_AESNEON=OFF ..
    make
    cd ..
elif [[ "$1" == "neon" ]]; then
    cd ./Additional_Implementations/NEON
    mkdir -p build
    cd build
    cmake -DMAYO_BUILD_TYPE=neon -DENABLE_AESNEON=ON ..
    make
    cd ..
elif [[ "$1" = "clean_all" ]]; then
    rm -rf ./Reference_Implementation/build
    rm -rf ./Reference_Implementation/build_arm
    rm -rf ./Optimized_Implementation/build
    rm -rf ./Additional_Implementations/NEON/build  

    echo "all cleaned"
else
    echo "Choose between: reference, avx2, armref, neon"
    exit 1
fi

# sound
paplay /usr/share/sounds/freedesktop/stereo/complete.oga