cd KAT

i=mqom2_cat3_gf16_fast_r3
cd $i
echo Testing $(basename "$i")
# echo -n "$(basename "$i") - " >> "./../../$log"
"../../NEON_Implementation/$i/kat_check" #| grep cycles  >> "./../../$log"
cd ..
