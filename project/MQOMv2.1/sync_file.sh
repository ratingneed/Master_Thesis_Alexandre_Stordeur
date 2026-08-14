#!/bin/bash

FILE="generator/PQCgenKAT_check.c"
PATH="./NEON_Implementation/mqom2_cat1_gf2_fast_r3"
SOURCE="$PATH/$FILE"

for dir in ./NEON_Implementation/mqom2*
do
    if [ "$dir" = "$PATH" ]; then
        continue
    fi

    # /bin/mkdir "$dir/$FILE"
    # /bin/rm "$dir/$FILE"
    /bin/cp "$SOURCE" "$dir/$FILE"
    echo $dir
done

echo end