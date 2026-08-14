#!/usr/bin/env bash

if [[ "$1" == "reference" ]]; then
    for i in ./Reference_Implementation/mqom2_cat*; do
        cd "$i"

        echo "build bench for $i"
        make clean 
        make bench BENCHMARK=1 FORCE_PLATFORM_REF=1 

        cd ../..
    done
elif [[ "$1" == "avx2" ]]; then
    for i in ./Optimized_Implementation/mqom2_cat*; do
        cd "$i"
        
        echo "build bench for $i"
        make clean
        make bench BENCHMARK=1 FORCE_PLATFORM_AVX2=1 RIJNDAEL_AES_NI=1
        cd ../..
    done
elif [[ "$1" == "armref" ]]; then
    for i in ./Reference_Implementation/mqom2_cat*; do
        cd "$i"

        echo "build bench for $i"
        make clean 
        make bench BENCHMARK=1 FORCE_PLATFORM_REF=1 

        cd ../..
    done
elif [[ "$1" == "neon" ]]; then
    for i in ./NEON_Implementation/mqom2_cat*; do
        cd "$i"
        
        echo "build bench for $i"
        make clean
        make bench BENCHMARK=1 FORCE_PLATFORM_NEON=1 RIJNDAEL_NEON=1
        cd ../..
    done
elif [[ "$1" == "clean_all" ]]; then

    for i in ./Reference_Implementation/mqom2_cat*; do
        if [[ -d "$i" ]]; then
            echo "Cleaning $i"
            cd "$i"
            make clean
            cd ../..
        fi
    done

    for i in ./Optimized_Implementation/mqom2_cat*; do
        if [[ -d "$i" ]]; then
            echo "Cleaning $i"
            cd "$i"
            make clean
            cd ../..
        fi
    done

    for i in ./NEON_Implementation/mqom2_cat*; do
        if [[ -d "$i" ]]; then
            echo "Cleaning $i"
            cd "$i"
            make clean
            cd ../..
        fi
    done

    echo "all cleaned"


else
    echo "Choose between: reference, avx2, armref, neon"
    exit 1
fi

# sound
paplay /usr/share/sounds/freedesktop/stereo/complete.oga