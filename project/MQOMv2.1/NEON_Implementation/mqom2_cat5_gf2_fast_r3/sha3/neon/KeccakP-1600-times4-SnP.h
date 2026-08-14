/*
The Keccak-p permutations, designed by Guido Bertoni, Joan Daemen, Michaël Peeters and Gilles Van Assche.

Implementation by the XKCP contributors, hereby denoted as "the implementer".

For more information, feedback or questions, please refer to the Keccak Team website:
https://keccak.team/

Keccak-p[1600]x4 for AArch64 using the Becker-Kannwischer scalar+NEON hybrid
permutation core from mlkem-native. Four states are stored sequentially
(KeccakP1600_state states[4] == uint64_t[100]), matching the core's ABI; all
byte/lane management falls back serially on the single-state (x1) SnP backend
via PlSnP-Fallback.inc, and only the 24-round permutation and absorb fast-loop
use the parallel hybrid core.

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/

---

Please refer to PlSnP-documentation.h for more details.
*/

#ifndef _KeccakP_1600_times4_SnP_h_
#define _KeccakP_1600_times4_SnP_h_

#include <stddef.h>
#include "align.h"
#include "KeccakP-1600-SnP.h"

typedef struct {
    ALIGN(32) KeccakP1600_state states[4];
} KeccakP1600times4_states;

#define KeccakP1600times4_implementation        "ARMv8-A scalar+NEON hybrid implementation (x4, mlkem-native core)"
#define KeccakP1600times4_statesSizeInBytes     800
#define KeccakP1600times4_statesAlignment       32
#define KeccakF1600times4_FastLoop_supported
#define KeccakP1600times4_12rounds_FastLoop_supported

void KeccakP1600times4_StaticInitialize(void);
void KeccakP1600times4_InitializeAll(void *states);
void KeccakP1600times4_AddByte(void *states, unsigned int instanceIndex, unsigned char data, unsigned int offset);
void KeccakP1600times4_AddBytes(void *states, unsigned int instanceIndex, const unsigned char *data, unsigned int offset, unsigned int length);
void KeccakP1600times4_AddLanesAll(void *states, const unsigned char *data, unsigned int laneCount, unsigned int laneOffset);
void KeccakP1600times4_OverwriteBytes(void *states, unsigned int instanceIndex, const unsigned char *data, unsigned int offset, unsigned int length);
void KeccakP1600times4_OverwriteLanesAll(void *states, const unsigned char *data, unsigned int laneCount, unsigned int laneOffset);
void KeccakP1600times4_OverwriteWithZeroes(void *states, unsigned int instanceIndex, unsigned int byteCount);
void KeccakP1600times4_PermuteAll_4rounds(void *states);
void KeccakP1600times4_PermuteAll_6rounds(void *states);
void KeccakP1600times4_PermuteAll_12rounds(void *states);
void KeccakP1600times4_PermuteAll_24rounds(void *states);
void KeccakP1600times4_ExtractBytes(const void *states, unsigned int instanceIndex, unsigned char *data, unsigned int offset, unsigned int length);
void KeccakP1600times4_ExtractLanesAll(const void *states, unsigned char *data, unsigned int laneCount, unsigned int laneOffset);
void KeccakP1600times4_ExtractAndAddBytes(const void *states, unsigned int instanceIndex,  const unsigned char *input, unsigned char *output, unsigned int offset, unsigned int length);
void KeccakP1600times4_ExtractAndAddLanesAll(const void *states, const unsigned char *input, unsigned char *output, unsigned int laneCount, unsigned int laneOffset);

size_t KeccakF1600times4_FastLoop_Absorb(void *states, unsigned int laneCount, unsigned int laneOffsetParallel, unsigned int laneOffsetSerial, const unsigned char *data, size_t dataByteLen);
size_t KeccakP1600times4_12rounds_FastLoop_Absorb(void *states, unsigned int laneCount, unsigned int laneOffsetParallel, unsigned int laneOffsetSerial, const unsigned char *data, size_t dataByteLen);

#endif
