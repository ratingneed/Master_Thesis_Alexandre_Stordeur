# 0 "neon/KeccakP-1600-armv8a-neon.S"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "neon/KeccakP-1600-armv8a-neon.S"
# 50 "neon/KeccakP-1600-armv8a-neon.S"
.macro LoadState
    ld4 { v19.2d, v20.2d, v21.2d, v22.2d }, [x0], #64
    ld4 { v23.2d, v24.2d, v25.2d, v26.2d }, [x0], #64
    ld4 { v27.2d, v28.2d, v29.2d, v30.2d }, [x0], #64
    ld1 { v31.d }[0], [x0], #8
    sub x0, x0, #200
    movi v16.2d, #0
    .endm

.macro StoreState
    st4 { v19.2d, v20.2d, v21.2d, v22.2d }, [x0], #64
    st4 { v23.2d, v24.2d, v25.2d, v26.2d }, [x0], #64
    st4 { v27.2d, v28.2d, v29.2d, v30.2d }, [x0], #64
    st1 { v31.d }[0], [x0], #8
    .endm

.macro RhoPi dst, src, sav, rot
    ror \src, \src, #64-\rot
    mov \sav, \dst
    mov \dst, \src
    .endm


.macro ROTL64 dst, src, rot
    .if (\rot & 7) != 0
    shl \dst\().2d, \src\().2d, #\rot
    sri \dst\().2d, \src\().2d, #64-\rot
    .else
    ext \dst\().16b, \src\().16b, \src\().16b, #\rot/8
    .endif
    .endm

.macro KeccakRound

    eor v0.16b, v19.16b, v25.16b
    eor v1.16b, v20.16b, v26.16b
    eor v2.16b, v21.16b, v28.16b
    eor v3.16b, v22.16b, v23.16b
    eor v4.16b, v24.16b, v30.16b

    eor v1.16b, v1.16b, v27.16b
    eor v3.16b, v3.16b, v29.16b

    trn1 v5.2d, v0.2d, v1.2d
    trn2 v6.2d, v1.2d, v2.2d
    eor v1.16b, v5.16b, v6.16b

    ext v5.16b, v4.16b, v2.16b, #8
    eor v3.16b, v3.16b, v5.16b

    mov v5.2d[0], v0.2d[1]
    eor v4.16b, v4.16b, v5.16b
    eor v4.16b, v4.16b, v31.16b

    ext v2.16b, v1.16b, v1.16b, #8
    mov v4.2d[1], v3.2d[0]
    trn2 v0.2d, v3.2d, v1.2d

    ROTL64 v5, v2, 1
    ROTL64 v6, v3, 1
    ROTL64 v7, v4, 1

    eor v18.16b, v4.16b, v5.16b
    eor v2.16b, v2.16b, v6.16b
    eor v0.16b, v0.16b, v7.16b

    ext v7.16b, v5.16b, v7.16b, #8
    eor v7.16b, v3.16b, v7.16b

    ext v6.16b, v6.16b, v5.16b, #8
    trn1 v4.2d, v1.2d, v4.2d
    eor v6.16b, v4.16b, v6.16b


    eor v19.16b, v19.16b, v18.16b
    eor v20.16b, v20.16b, v6.16b
    eor v21.16b, v21.16b, v2.16b
    eor v22.16b, v22.16b, v0.16b
    eor v23.16b, v23.16b, v0.16b
    eor v24.16b, v24.16b, v7.16b
    eor v25.16b, v25.16b, v18.16b
    eor v26.16b, v26.16b, v6.16b
    eor v27.16b, v27.16b, v6.16b
    eor v28.16b, v28.16b, v2.16b
    eor v29.16b, v29.16b, v0.16b
    eor v30.16b, v30.16b, v7.16b
    eor v31.16b, v31.16b, v7.16b


    mov x11, v20.2d[0]

    RhoPi v25.2d[0], x11, x10, 1
    RhoPi v22.2d[1], x10, x11, 3
    RhoPi v26.2d[0], x11, x10, 6
    RhoPi v28.2d[0], x10, x11, 10
    RhoPi v29.2d[0], x11, x10, 15
    RhoPi v22.2d[0], x10, x11, 21
    RhoPi v20.2d[1], x11, x10, 28
    RhoPi v27.2d[0], x10, x11, 36
    RhoPi v23.2d[0], x11, x10, 45
    RhoPi v28.2d[1], x10, x11, 55
    RhoPi v31.2d[0], x11, x10, 2
    RhoPi v19.2d[1], x10, x11, 14
    RhoPi v26.2d[1], x11, x10, 27
    RhoPi v30.2d[1], x10, x11, 41
    RhoPi v30.2d[0], x11, x10, 56
    RhoPi v24.2d[1], x10, x11, 8
    RhoPi v23.2d[1], x11, x10, 25
    RhoPi v21.2d[0], x10, x11, 43
    RhoPi v27.2d[1], x11, x10, 62
    RhoPi v25.2d[1], x10, x11, 18
    RhoPi v29.2d[1], x11, x10, 39
    RhoPi v24.2d[0], x10, x11, 61
    RhoPi v21.2d[1], x11, x10, 20

    ror x10, x10, #20
    mov v20.2d[0], x10


    ext v18.16b, v26.16b, v31.16b, #8
    bic v6.16b, v27.16b, v18.16b

    ext v17.16b, v26.16b, v31.16b, #8
    bic v5.16b, v17.16b, v30.16b

    bic v3.16b, v30.16b, v29.16b

    eor v30.16b, v30.16b, v6.16b

    trn1 v18.2d, v26.2d, v25.2d
    ext v17.16b, v23.16b, v26.16b, #8
    bic v7.16b, v17.16b, v18.16b

    trn2 v18.2d, v20.2d, v25.2d
    ext v17.16b, v21.16b, v25.16b, #8
    bic v6.16b, v17.16b, v18.16b

    trn1 v18.2d, v20.2d, v19.2d
    trn1 v17.2d, v21.2d, v20.2d
    bic v1.16b, v17.16b, v18.16b

    ext v18.16b, v19.16b, v23.16b, #8
    trn1 v17.2d, v19.2d, v24.2d
    bic v0.16b, v17.16b, v18.16b

    ext v18.16b, v23.16b, v27.16b, #8
    ext v17.16b, v24.16b, v28.16b, #8
    bic v4.16b, v17.16b, v18.16b

    mov v18.2d[0], v27.2d[1]
    mov v17.2d[0], v28.2d[1]
    bic v2.16b, v17.16b, v18.16b
    eor v31.16b, v31.16b, v2.16b

    bic v2.16b, v29.16b, v28.16b
    eor v27.16b, v27.16b, v2.16b

    bic v2.16b, v22.16b, v21.16b

    eor v28.16b, v28.16b, v3.16b
    eor v29.16b, v29.16b, v5.16b

    ext v17.16b, v19.16b, v23.16b, #8
    bic v3.16b, v17.16b, v22.16b

    trn2 v17.2d, v20.2d, v25.2d
    bic v5.16b, v17.16b, v24.16b


    eor v19.16b, v19.16b, v1.16b
    eor v20.16b, v20.16b, v2.16b
    eor v21.16b, v21.16b, v3.16b
    eor v22.16b, v22.16b, v0.16b
    eor v23.16b, v23.16b, v5.16b
    eor v24.16b, v24.16b, v6.16b
    eor v25.16b, v25.16b, v7.16b
    eor v26.16b, v26.16b, v4.16b


    ld1 { v16.d }[0], [x1], #8
    eor v19.16b, v19.16b, v16.16b
    .endm

.align 8
KeccakP1600_Permute_RoundConstants24:
    .quad 0x0000000000000001
    .quad 0x0000000000008082
    .quad 0x800000000000808a
    .quad 0x8000000080008000
    .quad 0x000000000000808b
    .quad 0x0000000080000001
    .quad 0x8000000080008081
    .quad 0x8000000000008009
    .quad 0x000000000000008a
    .quad 0x0000000000000088
    .quad 0x0000000080008009
    .quad 0x000000008000000a
KeccakP1600_Permute_RoundConstants12:
    .quad 0x000000008000808b
    .quad 0x800000000000008b
    .quad 0x8000000000008089
    .quad 0x8000000000008003
    .quad 0x8000000000008002
    .quad 0x8000000000000080
    .quad 0x000000000000800a
    .quad 0x800000008000000a
    .quad 0x8000000080008081
    .quad 0x8000000000008080
    .quad 0x0000000080000001
    .quad 0x8000000080008008
KeccakP1600_Permute_RoundConstants0:





.align 8
.global KeccakP1600_Initialize
KeccakP1600_Initialize:
    movi v0.2d, #0
    movi v1.2d, #0
    movi v2.2d, #0
    movi v3.2d, #0
    st4 { v0.2d, v1.2d, v2.2d, v3.2d }, [x0], #64
    st4 { v0.2d, v1.2d, v2.2d, v3.2d }, [x0], #64
    st4 { v0.2d, v1.2d, v2.2d, v3.2d }, [x0], #64
    st1 { v0.d }[0], [x0], #8
    ret






.align 8
.global KeccakP1600_AddByte
KeccakP1600_AddByte:
    ldrb w3, [x0, x2]
    eor w3, w3, w1
    strb w3, [x0, x2]
    ret






.align 8
.global KeccakP1600_AddBytes
KeccakP1600_AddBytes:
    add x0, x0, x2
    subs w4, w3, #1
    b.cc KeccakP1600_AddBytes_Exit
KeccakP1600_AddBytes_8LanesLoop:
    subs w3, w3, #64
    b.cc KeccakP1600_AddBytes_Lanes
    ld4 { v0.2d, v1.2d, v2.2d, v3.2d }, [x0]
    ld4 { v4.2d, v5.2d, v6.2d, v7.2d }, [x1], #64
    eor v0.16b, v0.16b, v4.16b
    eor v1.16b, v1.16b, v5.16b
    eor v2.16b, v2.16b, v6.16b
    eor v3.16b, v3.16b, v7.16b
    st4 { v0.2d, v1.2d, v2.2d, v3.2d }, [x0], #64
    b KeccakP1600_AddBytes_8LanesLoop
KeccakP1600_AddBytes_Lanes:
    add w3, w3, #64
KeccakP1600_AddBytes_LanesLoop:
    subs w3, w3, #8
    b.cc KeccakP1600_AddBytes_Bytes
    ld1 { v0.d }[0], [x0]
    ld1 { v4.d }[0], [x1], #8
    eor v0.8b, v0.8b, v4.8b
    st1 { v0.d }[0], [x0], #8
    b KeccakP1600_AddBytes_LanesLoop
KeccakP1600_AddBytes_Bytes:
    add w3, w3, #8
KeccakP1600_AddBytes_BytesLoop:
    subs w3, w3, #1
    b.cc KeccakP1600_AddBytes_Exit
    ldrb w4, [x0]
    ldrb w5, [x1], #1
    eor w4, w4, w5
    strb w4, [x0], #1
    b KeccakP1600_AddBytes_BytesLoop
KeccakP1600_AddBytes_Exit:
    ret





.align 8
.global KeccakP1600_OverwriteBytes
KeccakP1600_OverwriteBytes:
    add x0, x0, x2
    subs w4, w3, #1
    b.cc KeccakP1600_OverwriteBytes_Exit
KeccakP1600_OverwriteBytes_8LanesLoop:
    subs w3, w3, #64
    b.cc KeccakP1600_OverwriteBytes_Lanes
    ld4 { v0.2d, v1.2d, v2.2d, v3.2d }, [x1], #64
    st4 { v0.2d, v1.2d, v2.2d, v3.2d }, [x0], #64
    b KeccakP1600_OverwriteBytes_8LanesLoop
KeccakP1600_OverwriteBytes_Lanes:
    add w3, w3, #64
KeccakP1600_OverwriteBytes_LanesLoop:
    subs w3, w3, #8
    b.cc KeccakP1600_OverwriteBytes_Bytes
    ld1 { v0.d }[0], [x1], #8
    st1 { v0.d }[0], [x0], #8
    b KeccakP1600_OverwriteBytes_LanesLoop
KeccakP1600_OverwriteBytes_Bytes:
    add w3, w3, #8
KeccakP1600_OverwriteBytes_BytesLoop:
    subs w3, w3, #1
    b.cc KeccakP1600_OverwriteBytes_Exit
    ldrb w4, [x1], #1
    strb w4, [x0], #1
    b KeccakP1600_OverwriteBytes_BytesLoop
KeccakP1600_OverwriteBytes_Exit:
    ret






.align 8
.global KeccakP1600_OverwriteWithZeroes
KeccakP1600_OverwriteWithZeroes:
    subs w2, w1, #1
    b.cc KeccakP1600_OverwriteWithZeroes_Exit
    movi v0.2d, #0
    movi v1.2d, #0
    movi v2.2d, #0
    movi v3.2d, #0
    mov w2, #0
KeccakP1600_OverwriteWithZeroes_8LanesLoop:
    subs w1, w1, #64
    b.cc KeccakP1600_OverwriteWithZeroes_Lanes
    st4 { v0.2d, v1.2d, v2.2d, v3.2d }, [x0], #64
    b KeccakP1600_OverwriteWithZeroes_8LanesLoop
KeccakP1600_OverwriteWithZeroes_Lanes:
    add w1, w1, #64
KeccakP1600_OverwriteWithZeroes_LanesLoop:
    subs w1, w1, #8
    b.cc KeccakP1600_OverwriteWithZeroes_Bytes
    st1 { v0.d }[0], [x0], #8
    b KeccakP1600_OverwriteWithZeroes_LanesLoop
KeccakP1600_OverwriteWithZeroes_Bytes:
    add w1, w1, #8
KeccakP1600_OverwriteWithZeroes_LoopBytes:
    subs w1, w1, #1
    b.cc KeccakP1600_OverwriteWithZeroes_Exit
    strb w2, [x0], #1
    b KeccakP1600_OverwriteWithZeroes_LoopBytes
KeccakP1600_OverwriteWithZeroes_Exit:
    ret






.align 8
.global KeccakP1600_ExtractBytes
KeccakP1600_ExtractBytes:
    add x0, x0, x2
    subs w4, w3, #1
    b.cc KeccakP1600_ExtractBytes_Exit
KeccakP1600_ExtractBytes_8LanesLoop:
    subs w3, w3, #64
    b.cc KeccakP1600_ExtractBytes_Lanes
    ld4 { v0.2d, v1.2d, v2.2d, v3.2d }, [x0], #64
    st4 { v0.2d, v1.2d, v2.2d, v3.2d }, [x1], #64
    b KeccakP1600_ExtractBytes_8LanesLoop
KeccakP1600_ExtractBytes_Lanes:
    add w3, w3, #64
KeccakP1600_ExtractBytes_LanesLoop:
    subs w3, w3, #8
    b.cc KeccakP1600_ExtractBytes_Bytes
    ld1 { v0.d }[0], [x0], #8
    st1 { v0.d }[0], [x1], #8
    b KeccakP1600_ExtractBytes_LanesLoop
KeccakP1600_ExtractBytes_Bytes:
    add w3, w3, #8
KeccakP1600_ExtractBytes_BytesLoop:
    subs w3, w3, #1
    b.cc KeccakP1600_ExtractBytes_Exit
    ldrb w4, [x0], #1
    strb w4, [x1], #1
    b KeccakP1600_ExtractBytes_BytesLoop
KeccakP1600_ExtractBytes_Exit:
    ret






.align 8
.global KeccakP1600_ExtractAndAddBytes
KeccakP1600_ExtractAndAddBytes:
    add x0, x0, x3
    subs w5, w4, #1
    b.cc KeccakP1600_ExtractAndAddBytes_Exit
KeccakP1600_ExtractAndAddBytes_8LanesLoop:
    subs w4, w4, #64
    b.cc KeccakP1600_ExtractAndAddBytes_Lanes
    ld4 { v0.2d, v1.2d, v2.2d, v3.2d }, [x0], #64
    ld4 { v4.2d, v5.2d, v6.2d, v7.2d }, [x1], #64
    eor v0.16b, v0.16b, v4.16b
    eor v1.16b, v1.16b, v5.16b
    eor v2.16b, v2.16b, v6.16b
    eor v3.16b, v3.16b, v7.16b
    st4 { v0.2d, v1.2d, v2.2d, v3.2d }, [x2], #64
    b KeccakP1600_ExtractAndAddBytes_8LanesLoop
KeccakP1600_ExtractAndAddBytes_Lanes:
    add w4, w4, #64
KeccakP1600_ExtractAndAddBytes_LanesLoop:
    subs w4, w4, #8
    b.cc KeccakP1600_ExtractAndAddBytes_Bytes
    ld1 { v0.d }[0], [x0], #8
    ld1 { v4.d }[0], [x1], #8
    eor v0.8b, v0.8b, v4.8b
    st1 { v0.d }[0], [x2], #8
    b KeccakP1600_ExtractAndAddBytes_LanesLoop
KeccakP1600_ExtractAndAddBytes_Bytes:
    add w4, w4, #8
KeccakP1600_ExtractAndAddBytes_BytesLoop:
    subs w4, w4, #1
    b.cc KeccakP1600_ExtractAndAddBytes_Exit
    ldrb w5, [x0], #1
    ldrb w6, [x1], #1
    eor w5, w5, w6
    strb w5, [x2], #1
    b KeccakP1600_ExtractAndAddBytes_BytesLoop
KeccakP1600_ExtractAndAddBytes_Exit:
    ret





.align 8
.global KeccakP1600_Permute_Nrounds
KeccakP1600_Permute_Nrounds:
    mov x2, x1
    adr x1, KeccakP1600_Permute_RoundConstants0
 lsl x3, x2, #3
 sub x1, x1, x3
    b KeccakP1600_Permute





.align 8
.global KeccakP1600_Permute_12rounds
KeccakP1600_Permute_12rounds:
    adr x1, KeccakP1600_Permute_RoundConstants12
    mov x2, #12
    b KeccakP1600_Permute






.align 8
.global KeccakP1600_Permute_24rounds
KeccakP1600_Permute_24rounds:
    adr x1, KeccakP1600_Permute_RoundConstants24
    mov x2, #24
    b KeccakP1600_Permute





.align 8
.global KeccakP1600_Permute
KeccakP1600_Permute:
    LoadState
KeccakP1600_Permute_RoundLoop:
    KeccakRound
    subs w2, w2, #1
    bne KeccakP1600_Permute_RoundLoop
KeccakP1600_Permute_Exit:
    StoreState
    ret
