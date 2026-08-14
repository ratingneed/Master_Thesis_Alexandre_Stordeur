#!/bin/bash


if [[ "$1" == "avx2" ]]; then
    log="log_bench_avx2_inline.txt"

    > "$log"

    for i in ./Optimized_Implementation/build_avx2_inline/test/mayo_bench_MAYO_*
    do
        echo $(basename "$i")

        echo $(basename "$i") >> "$log"

        "$i" 10000 2>&1 | sed 's/^.*-//' | grep -e "KeyGen " -e "Sign " -e "Verif " >> "$log"
    done
elif [[ "$1" == "neon" ]]; then
    log="log_bench_neon_inline.txt"

    > "./$log"

    for i in `ls ./Additional_Implementations/NEON/build_neon_inline/test/mayo_bench_MAYO_*`
        do
        echo $(basename "$i")

        echo $(basename "$i") >> "$log"

        "$i" 10000 2>&1 | sed 's/^.*-//' | grep -e "KeyGen " -e "Sign " -e "Verif " >> "$log"
    done
else
    echo "Choose between: avx2, neon"
    exit 1
fi

# sound
paplay /usr/share/sounds/freedesktop/stereo/complete.oga