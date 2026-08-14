#!/bin/bash

if [[ "$1" == "avx2" ]]; then
    cd ./Optimized_Implementation
    mkdir -p build_avx2_inline
    cd build_avx2_inline
    cmake -DMAYO_BUILD_TYPE=avx2 -DENABLE_AESNI=ON -DENABLE_CYCLE_MAIN=ON ..
    make
    cd ..
elif [[ "$1" == "neon" ]]; then
    cd ./Additional_Implementations/NEON
    mkdir -p build_neon_inline
    cd build_neon_inline
    cmake -DMAYO_BUILD_TYPE=neon -DENABLE_AESNEON=ON -DENABLE_CYCLE_MAIN=ON ..
    make
    cd ..
elif [ "$1" = "clean_all" ]
then
    rm -rf ./Optimized_Implementation/build_avx2_inline
    rm -rf ./Additional_Implementations/NEON/build_neon_inline
    
    echo "all cleaned"
else
    echo "Choose between: avx2, neon"
    exit 1
fi

# sound
paplay /usr/share/sounds/freedesktop/stereo/complete.oga