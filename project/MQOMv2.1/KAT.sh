if [[ "$1" == "build" ]]; then
    if [[ "$2" == "0" ]]; then
        XOF=0
    else
        XOF=1
    fi
    if [[ "$3" == "0" ]]; then
        ENC=0
    else
        ENC=1
    fi
    for i in ./NEON_Implementation/mqom2_cat*; do
        cd "$i"
        
        echo "build bench for $i"
        make clean 
        make kat_check FORCE_PLATFORM_NEON=1 RIJNDAEL_NEON=1 USE_XOF_X4=$XOF USE_ENC_X8=$ENC
        cd ../..
    done
elif [[ "$1" == "build_avx2" ]]; then
    if [[ "$2" == "0" ]]; then
        XOF=0
    else
        XOF=1
    fi
    if [[ "$3" == "0" ]]; then
        ENC=0
    else
        ENC=1
    fi
    for i in ./Optimized_Implementation/mqom2_cat*; do
        cd "$i"
        
        echo "build bench for $i"
        make clean 
        make kat_check FORCE_PLATFORM_AVX2=1 RIJNDAEL_AES_NI=1 USE_XOF_X4=$XOF USE_ENC_X8=$ENC
        cd ../..
    done
elif [[ "$1" == "test" ]]; then
    log="test_neon.txt"

    cd KAT

    for i in `ls -d ./mqom2_cat*`
        do
        cd $i
        echo Testing $(basename "$i")
        # echo -n "$(basename "$i") - " >> "./../../$log"
        "../../NEON_Implementation/$i/kat_check" #| grep cycles  >> "./../../$log"
        cd ..
    done

elif [[ "$1" == "test_avx2" ]]; then
    
    log="test_neon.txt"

    cd KAT

    for i in `ls -d ./mqom2_cat*`
        do
        cd $i
        echo Testing $(basename "$i")
        # echo -n "$(basename "$i") - " >> "./../../$log"
        "../../Optimized_Implementation/$i/kat_check" #| grep cycles  >> "./../../$log"
        cd ..
        break
    done

else
    echo "Usage: $0 [build|test]"
fi

# sound
paplay /usr/share/sounds/freedesktop/stereo/complete.oga