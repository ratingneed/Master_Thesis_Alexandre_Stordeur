// Ported by Alexandre Stordeur

#include "rijndael_platform.h"

#if defined(RIJNDAEL_NEON)

/* This file mainly contains an optimized implementation
 * of the AES-128 and AES-256 using AES-NI intrinsics. This
 * is the only file of the whole project that has an adherence
 * with the hardware as it expects to run on an Intel or AMD
 * platform supporting AES-NI.
 */

#include "rijndael_neon.h"

/* ======== AES-128 ================ */

#define AESE_AESMC( m, rk ) \
{ \
    /* AddRoundKey -> SubBytes -> ShiftRows */ \
    m = vaeseq_u8( m, rk ); \
    /* MixColumns */ \
    m = vaesmcq_u8( m ); \
};


#define DO_ENC_BLOCK_128_128(m,k) \
	do{\
		AESE_AESMC(m, k[ 0]); \
		AESE_AESMC(m, k[ 1]); \
		AESE_AESMC(m, k[ 2]); \
		AESE_AESMC(m, k[ 3]); \
		AESE_AESMC(m, k[ 4]); \
		AESE_AESMC(m, k[ 5]); \
		AESE_AESMC(m, k[ 6]); \
		AESE_AESMC(m, k[ 7]); \
		AESE_AESMC(m, k[ 8]); \
		m = vaeseq_u8(m, k[ 9]); \
		m = veorq_u8(m, k[10]);\
	}while(0)

#define DO_ENC_BLOCK_128_128_x2(m1,m2,k1,k2) \
	do{\
		AESE_AESMC(m1, k1[ 0]); \
		AESE_AESMC(m2, k2[ 0]); \
		AESE_AESMC(m1, k1[ 1]); \
		AESE_AESMC(m2, k2[ 1]); \
		AESE_AESMC(m1, k1[ 2]); \
		AESE_AESMC(m2, k2[ 2]); \
		AESE_AESMC(m1, k1[ 3]); \
		AESE_AESMC(m2, k2[ 3]); \
		AESE_AESMC(m1, k1[ 4]); \
		AESE_AESMC(m2, k2[ 4]); \
		AESE_AESMC(m1, k1[ 5]); \
		AESE_AESMC(m2, k2[ 5]); \
		AESE_AESMC(m1, k1[ 6]); \
		AESE_AESMC(m2, k2[ 6]); \
		AESE_AESMC(m1, k1[ 7]); \
		AESE_AESMC(m2, k2[ 7]); \
		AESE_AESMC(m1, k1[ 8]); \
		AESE_AESMC(m2, k2[ 8]); \
		m1 = vaeseq_u8(m1, k1[ 9]); \
		m2 = vaeseq_u8(m2, k2[ 9]); \
		m1 = veorq_u8(m1, k1[10]);\
		m2 = veorq_u8(m2, k2[10]);\
	}while(0)

#define DO_ENC_BLOCK_128_128_x4(m1,m2,m3,m4,k1,k2,k3,k4) \
	do{\
		AESE_AESMC(m1, k1[ 0]); \
		AESE_AESMC(m2, k2[ 0]); \
		AESE_AESMC(m3, k3[ 0]); \
		AESE_AESMC(m4, k4[ 0]); \
		AESE_AESMC(m1, k1[ 1]); \
		AESE_AESMC(m2, k2[ 1]); \
		AESE_AESMC(m3, k3[ 1]); \
		AESE_AESMC(m4, k4[ 1]); \
		AESE_AESMC(m1, k1[ 2]); \
		AESE_AESMC(m2, k2[ 2]); \
		AESE_AESMC(m3, k3[ 2]); \
		AESE_AESMC(m4, k4[ 2]); \
		AESE_AESMC(m1, k1[ 3]); \
		AESE_AESMC(m2, k2[ 3]); \
		AESE_AESMC(m3, k3[ 3]); \
		AESE_AESMC(m4, k4[ 3]); \
		AESE_AESMC(m1, k1[ 4]); \
		AESE_AESMC(m2, k2[ 4]); \
		AESE_AESMC(m3, k3[ 4]); \
		AESE_AESMC(m4, k4[ 4]); \
		AESE_AESMC(m1, k1[ 5]); \
		AESE_AESMC(m2, k2[ 5]); \
		AESE_AESMC(m3, k3[ 5]); \
		AESE_AESMC(m4, k4[ 5]); \
		AESE_AESMC(m1, k1[ 6]); \
		AESE_AESMC(m2, k2[ 6]); \
		AESE_AESMC(m3, k3[ 6]); \
		AESE_AESMC(m4, k4[ 6]); \
		AESE_AESMC(m1, k1[ 7]); \
		AESE_AESMC(m2, k2[ 7]); \
		AESE_AESMC(m3, k3[ 7]); \
		AESE_AESMC(m4, k4[ 7]); \
		AESE_AESMC(m1, k1[ 8]); \
		AESE_AESMC(m2, k2[ 8]); \
		AESE_AESMC(m3, k3[ 8]); \
		AESE_AESMC(m4, k4[ 8]); \
		m1 = vaeseq_u8(m1, k1[ 9]); \
		m2 = vaeseq_u8(m2, k2[ 9]); \
		m3 = vaeseq_u8(m3, k3[ 9]); \
		m4 = vaeseq_u8(m4, k4[ 9]); \
		m1 = veorq_u8(m1, k1[10]);\
		m2 = veorq_u8(m2, k2[10]);\
		m3 = veorq_u8(m3, k3[10]);\
		m4 = veorq_u8(m4, k4[10]);\
	}while(0)

#define DO_ENC_BLOCK_128_128_x8(m1,m2,m3,m4,m5,m6,m7,m8,k1,k2,k3,k4,k5,k6,k7,k8) \
	do{\
		AESE_AESMC(m1, k1[ 0]); \
		AESE_AESMC(m2, k2[ 0]); \
		AESE_AESMC(m3, k3[ 0]); \
		AESE_AESMC(m4, k4[ 0]); \
		AESE_AESMC(m5, k5[ 0]); \
		AESE_AESMC(m6, k6[ 0]); \
		AESE_AESMC(m7, k7[ 0]); \
		AESE_AESMC(m8, k8[ 0]); \
		\
		AESE_AESMC(m1, k1[ 1]); \
		AESE_AESMC(m2, k2[ 1]); \
		AESE_AESMC(m3, k3[ 1]); \
		AESE_AESMC(m4, k4[ 1]); \
		AESE_AESMC(m5, k5[ 1]); \
		AESE_AESMC(m6, k6[ 1]); \
		AESE_AESMC(m7, k7[ 1]); \
		AESE_AESMC(m8, k8[ 1]); \
		\
		AESE_AESMC(m1, k1[ 2]); \
		AESE_AESMC(m2, k2[ 2]); \
		AESE_AESMC(m3, k3[ 2]); \
		AESE_AESMC(m4, k4[ 2]); \
		AESE_AESMC(m5, k5[ 2]); \
		AESE_AESMC(m6, k6[ 2]); \
		AESE_AESMC(m7, k7[ 2]); \
		AESE_AESMC(m8, k8[ 2]); \
		\
		AESE_AESMC(m1, k1[ 3]); \
		AESE_AESMC(m2, k2[ 3]); \
		AESE_AESMC(m3, k3[ 3]); \
		AESE_AESMC(m4, k4[ 3]); \
		AESE_AESMC(m5, k5[ 3]); \
		AESE_AESMC(m6, k6[ 3]); \
		AESE_AESMC(m7, k7[ 3]); \
		AESE_AESMC(m8, k8[ 3]); \
		\
		AESE_AESMC(m1, k1[ 4]); \
		AESE_AESMC(m2, k2[ 4]); \
		AESE_AESMC(m3, k3[ 4]); \
		AESE_AESMC(m4, k4[ 4]); \
		AESE_AESMC(m5, k5[ 4]); \
		AESE_AESMC(m6, k6[ 4]); \
		AESE_AESMC(m7, k7[ 4]); \
		AESE_AESMC(m8, k8[ 4]); \
		\
		AESE_AESMC(m1, k1[ 5]); \
		AESE_AESMC(m2, k2[ 5]); \
		AESE_AESMC(m3, k3[ 5]); \
		AESE_AESMC(m4, k4[ 5]); \
		AESE_AESMC(m5, k5[ 5]); \
		AESE_AESMC(m6, k6[ 5]); \
		AESE_AESMC(m7, k7[ 5]); \
		AESE_AESMC(m8, k8[ 5]); \
		\
		AESE_AESMC(m1, k1[ 6]); \
		AESE_AESMC(m2, k2[ 6]); \
		AESE_AESMC(m3, k3[ 6]); \
		AESE_AESMC(m4, k4[ 6]); \
		AESE_AESMC(m5, k5[ 6]); \
		AESE_AESMC(m6, k6[ 6]); \
		AESE_AESMC(m7, k7[ 6]); \
		AESE_AESMC(m8, k8[ 6]); \
		\
		AESE_AESMC(m1, k1[ 7]); \
		AESE_AESMC(m2, k2[ 7]); \
		AESE_AESMC(m3, k3[ 7]); \
		AESE_AESMC(m4, k4[ 7]); \
		AESE_AESMC(m5, k5[ 7]); \
		AESE_AESMC(m6, k6[ 7]); \
		AESE_AESMC(m7, k7[ 7]); \
		AESE_AESMC(m8, k8[ 7]); \
		\
		AESE_AESMC(m1, k1[ 8]); \
		AESE_AESMC(m2, k2[ 8]); \
		AESE_AESMC(m3, k3[ 8]); \
		AESE_AESMC(m4, k4[ 8]); \
		AESE_AESMC(m5, k5[ 8]); \
		AESE_AESMC(m6, k6[ 8]); \
		AESE_AESMC(m7, k7[ 8]); \
		AESE_AESMC(m8, k8[ 8]); \
		\
		m1 = vaeseq_u8(m1, k1[ 9]); \
		m2 = vaeseq_u8(m2, k2[ 9]); \
		m3 = vaeseq_u8(m3, k3[ 9]); \
		m4 = vaeseq_u8(m4, k4[ 9]); \
		m5 = vaeseq_u8(m5, k5[ 9]); \
		m6 = vaeseq_u8(m6, k6[ 9]); \
		m7 = vaeseq_u8(m7, k7[ 9]); \
		m8 = vaeseq_u8(m8, k8[ 9]); \
		\
		m1 = veorq_u8(m1, k1[10]);\
		m2 = veorq_u8(m2, k2[10]);\
		m3 = veorq_u8(m3, k3[10]);\
		m4 = veorq_u8(m4, k4[10]);\
		m5 = veorq_u8(m5, k5[10]);\
		m6 = veorq_u8(m6, k6[10]);\
		m7 = veorq_u8(m7, k7[10]);\
		m8 = veorq_u8(m8, k8[10]);\
	}while(0)

/* ======== AES-256 ================ */
#define DO_ENC_BLOCK_128_256(m,k) \
	do{\
		AESE_AESMC(m, k[ 0]); \
		AESE_AESMC(m, k[ 1]); \
		AESE_AESMC(m, k[ 2]); \
		AESE_AESMC(m, k[ 3]); \
		AESE_AESMC(m, k[ 4]); \
		AESE_AESMC(m, k[ 5]); \
		AESE_AESMC(m, k[ 6]); \
		AESE_AESMC(m, k[ 7]); \
		AESE_AESMC(m, k[ 8]); \
		AESE_AESMC(m, k[ 9]); \
		AESE_AESMC(m, k[10]); \
		AESE_AESMC(m, k[11]); \
		AESE_AESMC(m, k[12]); \
		m = vaeseq_u8(m, k[13]); \
		m = veorq_u8(m, k[14]);\
	}while(0)

#define DO_ENC_BLOCK_128_256_x2(m1,m2,k1,k2) \
	do{\
		AESE_AESMC(m1, k1[ 0]); \
		AESE_AESMC(m2, k2[ 0]); \
		AESE_AESMC(m1, k1[ 1]); \
		AESE_AESMC(m2, k2[ 1]); \
		AESE_AESMC(m1, k1[ 2]); \
		AESE_AESMC(m2, k2[ 2]); \
		AESE_AESMC(m1, k1[ 3]); \
		AESE_AESMC(m2, k2[ 3]); \
		AESE_AESMC(m1, k1[ 4]); \
		AESE_AESMC(m2, k2[ 4]); \
		AESE_AESMC(m1, k1[ 5]); \
		AESE_AESMC(m2, k2[ 5]); \
		AESE_AESMC(m1, k1[ 6]); \
		AESE_AESMC(m2, k2[ 6]); \
		AESE_AESMC(m1, k1[ 7]); \
		AESE_AESMC(m2, k2[ 7]); \
		AESE_AESMC(m1, k1[ 8]); \
		AESE_AESMC(m2, k2[ 8]); \
		AESE_AESMC(m1, k1[ 9]); \
		AESE_AESMC(m2, k2[ 9]); \
		AESE_AESMC(m1, k1[10]); \
		AESE_AESMC(m2, k2[10]); \
		AESE_AESMC(m1, k1[11]); \
		AESE_AESMC(m2, k2[11]); \
		AESE_AESMC(m1, k1[12]); \
		AESE_AESMC(m2, k2[12]); \
		m1 = vaeseq_u8(m1, k1[13]); \
		m2 = vaeseq_u8(m2, k2[13]); \
		m1 = veorq_u8(m1, k1[14]);\
		m2 = veorq_u8(m2, k2[14]);\
	}while(0)

#define DO_ENC_BLOCK_128_256_x4(m1,m2,m3,m4,k1,k2,k3,k4) \
	do{\
		AESE_AESMC(m1, k1[ 0]); \
		AESE_AESMC(m2, k2[ 0]); \
		AESE_AESMC(m3, k3[ 0]); \
		AESE_AESMC(m4, k4[ 0]); \
		AESE_AESMC(m1, k1[ 1]); \
		AESE_AESMC(m2, k2[ 1]); \
		AESE_AESMC(m3, k3[ 1]); \
		AESE_AESMC(m4, k4[ 1]); \
		AESE_AESMC(m1, k1[ 2]); \
		AESE_AESMC(m2, k2[ 2]); \
		AESE_AESMC(m3, k3[ 2]); \
		AESE_AESMC(m4, k4[ 2]); \
		AESE_AESMC(m1, k1[ 3]); \
		AESE_AESMC(m2, k2[ 3]); \
		AESE_AESMC(m3, k3[ 3]); \
		AESE_AESMC(m4, k4[ 3]); \
		AESE_AESMC(m1, k1[ 4]); \
		AESE_AESMC(m2, k2[ 4]); \
		AESE_AESMC(m3, k3[ 4]); \
		AESE_AESMC(m4, k4[ 4]); \
		AESE_AESMC(m1, k1[ 5]); \
		AESE_AESMC(m2, k2[ 5]); \
		AESE_AESMC(m3, k3[ 5]); \
		AESE_AESMC(m4, k4[ 5]); \
		AESE_AESMC(m1, k1[ 6]); \
		AESE_AESMC(m2, k2[ 6]); \
		AESE_AESMC(m3, k3[ 6]); \
		AESE_AESMC(m4, k4[ 6]); \
		AESE_AESMC(m1, k1[ 7]); \
		AESE_AESMC(m2, k2[ 7]); \
		AESE_AESMC(m3, k3[ 7]); \
		AESE_AESMC(m4, k4[ 7]); \
		AESE_AESMC(m1, k1[ 8]); \
		AESE_AESMC(m2, k2[ 8]); \
		AESE_AESMC(m3, k3[ 8]); \
		AESE_AESMC(m4, k4[ 8]); \
		AESE_AESMC(m1, k1[ 9]); \
		AESE_AESMC(m2, k2[ 9]); \
		AESE_AESMC(m3, k3[ 9]); \
		AESE_AESMC(m4, k4[ 9]); \
		AESE_AESMC(m1, k1[10]); \
		AESE_AESMC(m2, k2[10]); \
		AESE_AESMC(m3, k3[10]); \
		AESE_AESMC(m4, k4[10]); \
		AESE_AESMC(m1, k1[11]); \
		AESE_AESMC(m2, k2[11]); \
		AESE_AESMC(m3, k3[11]); \
		AESE_AESMC(m4, k4[11]); \
		AESE_AESMC(m1, k1[12]); \
		AESE_AESMC(m2, k2[12]); \
		AESE_AESMC(m3, k3[12]); \
		AESE_AESMC(m4, k4[12]); \
		m1 = vaeseq_u8(m1, k1[13]); \
		m2 = vaeseq_u8(m2, k2[13]); \
		m3 = vaeseq_u8(m3, k3[13]); \
		m4 = vaeseq_u8(m4, k4[13]); \
		m1 = veorq_u8(m1, k1[14]);\
		m2 = veorq_u8(m2, k2[14]);\
		m3 = veorq_u8(m3, k3[14]);\
		m4 = veorq_u8(m4, k4[14]);\
	}while(0)

#define DO_ENC_BLOCK_128_256_x8(m1,m2,m3,m4,m5,m6,m7,m8,k1,k2,k3,k4,k5,k6,k7,k8) \
	do{\
		AESE_AESMC(m1, k1[ 0]); \
		AESE_AESMC(m2, k2[ 0]); \
		AESE_AESMC(m3, k3[ 0]); \
		AESE_AESMC(m4, k4[ 0]); \
		AESE_AESMC(m5, k5[ 0]); \
		AESE_AESMC(m6, k6[ 0]); \
		AESE_AESMC(m7, k7[ 0]); \
		AESE_AESMC(m8, k8[ 0]); \
		\
		AESE_AESMC(m1, k1[ 1]); \
		AESE_AESMC(m2, k2[ 1]); \
		AESE_AESMC(m3, k3[ 1]); \
		AESE_AESMC(m4, k4[ 1]); \
		AESE_AESMC(m5, k5[ 1]); \
		AESE_AESMC(m6, k6[ 1]); \
		AESE_AESMC(m7, k7[ 1]); \
		AESE_AESMC(m8, k8[ 1]); \
		\
		AESE_AESMC(m1, k1[ 2]); \
		AESE_AESMC(m2, k2[ 2]); \
		AESE_AESMC(m3, k3[ 2]); \
		AESE_AESMC(m4, k4[ 2]); \
		AESE_AESMC(m5, k5[ 2]); \
		AESE_AESMC(m6, k6[ 2]); \
		AESE_AESMC(m7, k7[ 2]); \
		AESE_AESMC(m8, k8[ 2]); \
		\
		AESE_AESMC(m1, k1[ 3]); \
		AESE_AESMC(m2, k2[ 3]); \
		AESE_AESMC(m3, k3[ 3]); \
		AESE_AESMC(m4, k4[ 3]); \
		AESE_AESMC(m5, k5[ 3]); \
		AESE_AESMC(m6, k6[ 3]); \
		AESE_AESMC(m7, k7[ 3]); \
		AESE_AESMC(m8, k8[ 3]); \
		\
		AESE_AESMC(m1, k1[ 4]); \
		AESE_AESMC(m2, k2[ 4]); \
		AESE_AESMC(m3, k3[ 4]); \
		AESE_AESMC(m4, k4[ 4]); \
		AESE_AESMC(m5, k5[ 4]); \
		AESE_AESMC(m6, k6[ 4]); \
		AESE_AESMC(m7, k7[ 4]); \
		AESE_AESMC(m8, k8[ 4]); \
		\
		AESE_AESMC(m1, k1[ 5]); \
		AESE_AESMC(m2, k2[ 5]); \
		AESE_AESMC(m3, k3[ 5]); \
		AESE_AESMC(m4, k4[ 5]); \
		AESE_AESMC(m5, k5[ 5]); \
		AESE_AESMC(m6, k6[ 5]); \
		AESE_AESMC(m7, k7[ 5]); \
		AESE_AESMC(m8, k8[ 5]); \
		\
		AESE_AESMC(m1, k1[ 6]); \
		AESE_AESMC(m2, k2[ 6]); \
		AESE_AESMC(m3, k3[ 6]); \
		AESE_AESMC(m4, k4[ 6]); \
		AESE_AESMC(m5, k5[ 6]); \
		AESE_AESMC(m6, k6[ 6]); \
		AESE_AESMC(m7, k7[ 6]); \
		AESE_AESMC(m8, k8[ 6]); \
		\
		AESE_AESMC(m1, k1[ 7]); \
		AESE_AESMC(m2, k2[ 7]); \
		AESE_AESMC(m3, k3[ 7]); \
		AESE_AESMC(m4, k4[ 7]); \
		AESE_AESMC(m5, k5[ 7]); \
		AESE_AESMC(m6, k6[ 7]); \
		AESE_AESMC(m7, k7[ 7]); \
		AESE_AESMC(m8, k8[ 7]); \
		\
		AESE_AESMC(m1, k1[ 8]); \
		AESE_AESMC(m2, k2[ 8]); \
		AESE_AESMC(m3, k3[ 8]); \
		AESE_AESMC(m4, k4[ 8]); \
		AESE_AESMC(m5, k5[ 8]); \
		AESE_AESMC(m6, k6[ 8]); \
		AESE_AESMC(m7, k7[ 8]); \
		AESE_AESMC(m8, k8[ 8]); \
		\
		AESE_AESMC(m1, k1[ 9]); \
		AESE_AESMC(m2, k2[ 9]); \
		AESE_AESMC(m3, k3[ 9]); \
		AESE_AESMC(m4, k4[ 9]); \
		AESE_AESMC(m5, k5[ 9]); \
		AESE_AESMC(m6, k6[ 9]); \
		AESE_AESMC(m7, k7[ 9]); \
		AESE_AESMC(m8, k8[ 9]); \
		\
		AESE_AESMC(m1, k1[10]); \
		AESE_AESMC(m2, k2[10]); \
		AESE_AESMC(m3, k3[10]); \
		AESE_AESMC(m4, k4[10]); \
		AESE_AESMC(m5, k5[10]); \
		AESE_AESMC(m6, k6[10]); \
		AESE_AESMC(m7, k7[10]); \
		AESE_AESMC(m8, k8[10]); \
		\
		AESE_AESMC(m1, k1[11]); \
		AESE_AESMC(m2, k2[11]); \
		AESE_AESMC(m3, k3[11]); \
		AESE_AESMC(m4, k4[11]); \
		AESE_AESMC(m5, k5[11]); \
		AESE_AESMC(m6, k6[11]); \
		AESE_AESMC(m7, k7[11]); \
		AESE_AESMC(m8, k8[11]); \
		\
		AESE_AESMC(m1, k1[12]); \
		AESE_AESMC(m2, k2[12]); \
		AESE_AESMC(m3, k3[12]); \
		AESE_AESMC(m4, k4[12]); \
		AESE_AESMC(m5, k5[12]); \
		AESE_AESMC(m6, k6[12]); \
		AESE_AESMC(m7, k7[12]); \
		AESE_AESMC(m8, k8[12]); \
		\
		m1 = vaeseq_u8(m1, k1[13]); \
		m2 = vaeseq_u8(m2, k2[13]); \
		m3 = vaeseq_u8(m3, k3[13]); \
		m4 = vaeseq_u8(m4, k4[13]); \
		m5 = vaeseq_u8(m5, k5[13]); \
		m6 = vaeseq_u8(m6, k6[13]); \
		m7 = vaeseq_u8(m7, k7[13]); \
		m8 = vaeseq_u8(m8, k8[13]); \
		\
		m1 = veorq_u8(m1, k1[14]);\
		m2 = veorq_u8(m2, k2[14]);\
		m3 = veorq_u8(m3, k3[14]);\
		m4 = veorq_u8(m4, k4[14]);\
		m5 = veorq_u8(m5, k5[14]);\
		m6 = veorq_u8(m6, k6[14]);\
		m7 = veorq_u8(m7, k7[14]);\
		m8 = veorq_u8(m8, k8[14]);\
	}while(0)

/* ======== Rijndael-256 ================ */

static inline uint8x16_t blendv_neon(uint8x16_t a, uint8x16_t b, uint8x16_t mask)
{
    // Use a signed shift right to create a mask with the sign bit
    uint8x16_t new_mask = vreinterpretq_u8_s8(vshrq_n_s8(vreinterpretq_s8_u8(mask), 7));
    return vbslq_u8(new_mask, b, a);
}

static inline uint8x16_t shuffle_neon(uint8x16_t a, uint8x16_t idx)
{
    int8x16_t tbl = vreinterpretq_s8_u8(a);   // input a
    uint8x16_t idx_masked = vandq_u8(idx, vdupq_n_u8(0x8F));  // avoid using meaningless bits
    return vreinterpretq_u8_s8(vqtbl1q_s8(tbl, idx_masked));
}

/* 256 bits blocks: ml is left block, mr is right block */
#define RINJDAEL_COMPENSATE_SR(ml, mr, tmpl, tmpr, mblend, mshuff) \
	do{\
		/* This compensates for the AES shiftrows to */	\
		/* conform to Rijdael one */              	\
		tmpl = blendv_neon(ml, mr, mblend); \
		tmpr = blendv_neon(mr, ml, mblend); \
		tmpl = shuffle_neon(tmpl, mshuff); \
		tmpr = shuffle_neon(tmpr, mshuff); \
	}while(0)

#define ENC_256_BLOCK(ml, mr, kl, kr, mb, ms) \
	do{\
		ml = vaeseq_u8(ml, kl); 				\
		mr = vaeseq_u8(mr, kr); 				\
		\
		uint8x16_t tmpl, tmpr;  						\
		/* Compensate for the shift rows */				\
		RINJDAEL_COMPENSATE_SR(ml, mr, tmpl, tmpr, mb, ms);		\
		\
		ml = vaesmcq_u8(tmpl); 				\
		mr = vaesmcq_u8(tmpr); 				\
	}while(0)

#define ENC_256_BLOCK_LAST(ml, mr, kl, kr, mblend, mshuff) \
	do{\
		ml = vaeseq_u8(ml, kl); 			\
		mr = vaeseq_u8(mr, kr); 			\
		uint8x16_t tmpl, tmpr;  						\
		/* Compensate for the shift rows */				\
		RINJDAEL_COMPENSATE_SR(ml, mr, tmpl, tmpr, mblend, mshuff);	\
		ml = tmpl; 			\
		mr = tmpr; 			\
	}while(0)

#define DO_ENC_BLOCK_256_256(ml, mr, k) \
	do{\
		/* ShiftRows compensation constants */		\
		const uint8x16_t mblend = { 0x00,0x00,0x80,0x80,   \
									0x00,0x00,0x00,0x80,   \
									0x00,0x00,0x80,0x80,   \
									0x00,0x80,0x80,0x80 }; \
		const uint8x16_t mshuff = { 0x00,0x01,0x06,0x07,   \
									0x04,0x05,0x0a,0x0b,   \
									0x08,0x09,0x0e,0x0f,   \
									0x0c,0x0d,0x02,0x03 }; \
		/**/						\
		ENC_256_BLOCK(ml, mr, k[0], k[1]  , mblend, mshuff); \
		ENC_256_BLOCK(ml, mr, k[2], k[3]  , mblend, mshuff); \
		ENC_256_BLOCK(ml, mr, k[4], k[5]  , mblend, mshuff); \
		ENC_256_BLOCK(ml, mr, k[6], k[7]  , mblend, mshuff); \
		ENC_256_BLOCK(ml, mr, k[8], k[9]  , mblend, mshuff); \
		ENC_256_BLOCK(ml, mr, k[10], k[11], mblend, mshuff); \
		ENC_256_BLOCK(ml, mr, k[12], k[13], mblend, mshuff); \
		ENC_256_BLOCK(ml, mr, k[14], k[15], mblend, mshuff); \
		ENC_256_BLOCK(ml, mr, k[16], k[17], mblend, mshuff); \
		ENC_256_BLOCK(ml, mr, k[18], k[19], mblend, mshuff); \
		ENC_256_BLOCK(ml, mr, k[20], k[21], mblend, mshuff); \
		ENC_256_BLOCK(ml, mr, k[22], k[23], mblend, mshuff); \
		ENC_256_BLOCK(ml, mr, k[24], k[25], mblend, mshuff); \
		/**/						\
		ENC_256_BLOCK_LAST(ml, mr, k[26], k[27], mblend, mshuff); \
		/**/						\
		ml = veorq_u8(ml, k[ 28]); 		\
		mr = veorq_u8(mr, k[ 29]); 		\
		/**/						\
	}while(0)

/* X2 variant */
#define RINJDAEL_COMPENSATE_SR_x2(ml1, mr1, ml2, mr2, tmpl1, tmpr1, tmpl2, tmpr2, mblend, mshuff) \
	do{\
		/* This compensates for the AES shiftrows to */	\
		/* conform to Rijdael one */              	\
		tmpl1 = blendv_neon(ml1, mr1, mblend); \
		tmpl2 = blendv_neon(ml2, mr2, mblend); \
		tmpr1 = blendv_neon(mr1, ml1, mblend); \
		tmpr2 = blendv_neon(mr2, ml2, mblend); \
		tmpl1 = shuffle_neon(tmpl1, mshuff); \
		tmpl2 = shuffle_neon(tmpl2, mshuff); \
		tmpr1 = shuffle_neon(tmpr1, mshuff); \
		tmpr2 = shuffle_neon(tmpr2, mshuff); \
	}while(0)

#define ENC_256_BLOCK_x2(ml1, mr1, ml2, mr2, kl1, kl2, kr1, kr2, mb, ms) \
	do{\
		ml1 = vaeseq_u8(ml1, kl1); 				\
		ml2 = vaeseq_u8(ml2, kl2); 				\
		mr1 = vaeseq_u8(mr1, kr1); 				\
		mr2 = vaeseq_u8(mr2, kr2); 				\
		\
		uint8x16_t tmpl1, tmpr1, tmpl2, tmpr2;				\
		/* Compensate for the shift rows */				\
		RINJDAEL_COMPENSATE_SR_x2(ml1, mr1, ml2, mr2, tmpl1, tmpr1, tmpl2, tmpr2, mblend, mshuff); \
		\
		ml1 = vaesmcq_u8(tmpl1); 				\
		ml2 = vaesmcq_u8(tmpl2); 				\
		mr1 = vaesmcq_u8(tmpr1); 				\
		mr2 = vaesmcq_u8(tmpr2); 				\
	}while(0)

#define ENC_256_BLOCK_LAST_x2(ml1, mr1, ml2, mr2, kl1, kl2, kr1, kr2, mb, ms) \
	do{\
		ml1 = vaeseq_u8(ml1, kl1); 			\
		ml2 = vaeseq_u8(ml2, kl2); 			\
		mr1 = vaeseq_u8(mr1, kr1); 			\
		mr2 = vaeseq_u8(mr2, kr2); 			\
		uint8x16_t tmpl1, tmpr1, tmpl2, tmpr2;				\
		/* Compensate for the shift rows */				\
		RINJDAEL_COMPENSATE_SR_x2(ml1, mr1, ml2, mr2, tmpl1, tmpr1, tmpl2, tmpr2, mblend, mshuff); \
		ml1 = tmpl1; 			\
		ml2 = tmpl2; 			\
		mr1 = tmpr1; 			\
		mr2 = tmpr2; 			\
	}while(0)

#define DO_ENC_BLOCK_256_256_x2(ml1, mr1, ml2, mr2, k1, k2) \
	do{\
		/* ShiftRows compensation constants */		\
		const uint8x16_t mblend = { 0x00,0x00,0x80,0x80,   \
									0x00,0x00,0x00,0x80,   \
									0x00,0x00,0x80,0x80,   \
									0x00,0x80,0x80,0x80 }; \
		const uint8x16_t mshuff = { 0x00,0x01,0x06,0x07,   \
									0x04,0x05,0x0a,0x0b,   \
									0x08,0x09,0x0e,0x0f,   \
									0x0c,0x0d,0x02,0x03 }; \
		/**/						\
		ENC_256_BLOCK_x2(ml1, mr1, ml2, mr2, k1[0], k2[0], k1[1], k2[1], mblend, mshuff); \
		ENC_256_BLOCK_x2(ml1, mr1, ml2, mr2, k1[2], k2[2], k1[3], k2[3], mblend, mshuff); \
		ENC_256_BLOCK_x2(ml1, mr1, ml2, mr2, k1[4], k2[4], k1[5], k2[5], mblend, mshuff); \
		ENC_256_BLOCK_x2(ml1, mr1, ml2, mr2, k1[6], k2[6], k1[7], k2[7], mblend, mshuff); \
		ENC_256_BLOCK_x2(ml1, mr1, ml2, mr2, k1[8], k2[8], k1[9], k2[9], mblend, mshuff); \
		ENC_256_BLOCK_x2(ml1, mr1, ml2, mr2, k1[10], k2[10], k1[11], k2[11], mblend, mshuff); \
		ENC_256_BLOCK_x2(ml1, mr1, ml2, mr2, k1[12], k2[12], k1[13], k2[13], mblend, mshuff); \
		ENC_256_BLOCK_x2(ml1, mr1, ml2, mr2, k1[14], k2[14], k1[15], k2[15], mblend, mshuff); \
		ENC_256_BLOCK_x2(ml1, mr1, ml2, mr2, k1[16], k2[16], k1[17], k2[17], mblend, mshuff); \
		ENC_256_BLOCK_x2(ml1, mr1, ml2, mr2, k1[18], k2[18], k1[19], k2[19], mblend, mshuff); \
		ENC_256_BLOCK_x2(ml1, mr1, ml2, mr2, k1[20], k2[20], k1[21], k2[21], mblend, mshuff); \
		ENC_256_BLOCK_x2(ml1, mr1, ml2, mr2, k1[22], k2[22], k1[23], k2[23], mblend, mshuff); \
		ENC_256_BLOCK_x2(ml1, mr1, ml2, mr2, k1[24], k2[24], k1[25], k2[25], mblend, mshuff); \
		/**/						\
		ENC_256_BLOCK_LAST_x2(ml1, mr1, ml2, mr2, k1[26], k2[26], k1[27], k2[27], mblend, mshuff); \
		/**/						\
		ml1 = veorq_u8(ml1, k1[ 28]); 		\
		ml2 = veorq_u8(ml2, k2[ 28]); 		\
		mr1 = veorq_u8(mr1, k1[ 29]); 		\
		mr2 = veorq_u8(mr2, k2[ 29]); 		\
		/**/						\
	}while(0)

/* X4 variant */
#define RINJDAEL_COMPENSATE_SR_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, tmpl1, tmpr1, tmpl2, tmpr2, tmpl3, tmpr3, tmpl4, tmpr4, mblend, mshuff) \
	do{\
		/* This compensates for the AES shiftrows to */	\
		/* conform to Rijdael one */              	\
		tmpl1 = blendv_neon(ml1, mr1, mblend); \
		tmpl2 = blendv_neon(ml2, mr2, mblend); \
		tmpl3 = blendv_neon(ml3, mr3, mblend); \
		tmpl4 = blendv_neon(ml4, mr4, mblend); \
		tmpr1 = blendv_neon(mr1, ml1, mblend); \
		tmpr2 = blendv_neon(mr2, ml2, mblend); \
		tmpr3 = blendv_neon(mr3, ml3, mblend); \
		tmpr4 = blendv_neon(mr4, ml4, mblend); \
		tmpl1 = shuffle_neon(tmpl1, mshuff); \
		tmpl2 = shuffle_neon(tmpl2, mshuff); \
		tmpl3 = shuffle_neon(tmpl3, mshuff); \
		tmpl4 = shuffle_neon(tmpl4, mshuff); \
		tmpr1 = shuffle_neon(tmpr1, mshuff); \
		tmpr2 = shuffle_neon(tmpr2, mshuff); \
		tmpr3 = shuffle_neon(tmpr3, mshuff); \
		tmpr4 = shuffle_neon(tmpr4, mshuff); \
	}while(0)

#define ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, kl1, kl2, kl3, kl4, kr1, kr2, kr3, kr4, mb, ms) \
	do{\
		ml1 = vaeseq_u8(ml1, kl1); 				\
		ml2 = vaeseq_u8(ml2, kl2); 				\
		mr1 = vaeseq_u8(mr1, kr1); 				\
		mr2 = vaeseq_u8(mr2, kr2); 				\
		ml3 = vaeseq_u8(ml3, kl3); 				\
		ml4 = vaeseq_u8(ml4, kl4); 				\
		mr3 = vaeseq_u8(mr3, kr3); 				\
		mr4 = vaeseq_u8(mr4, kr4); 				\
		uint8x16_t tmpl1, tmpr1, tmpl2, tmpr2, tmpl3, tmpr3, tmpl4, tmpr4;	\
		/* Compensate for the shift rows */				\
		RINJDAEL_COMPENSATE_SR_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, tmpl1, tmpr1, tmpl2, tmpr2, tmpl3, tmpr3, tmpl4, tmpr4, mblend, mshuff); \
		ml1 = vaesmcq_u8(tmpl1); 				\
		ml2 = vaesmcq_u8(tmpl2); 				\
		mr1 = vaesmcq_u8(tmpr1); 				\
		mr2 = vaesmcq_u8(tmpr2); 				\
		ml3 = vaesmcq_u8(tmpl3); 				\
		ml4 = vaesmcq_u8(tmpl4); 				\
		mr3 = vaesmcq_u8(tmpr3); 				\
		mr4 = vaesmcq_u8(tmpr4); 				\
	}while(0)

#define ENC_256_BLOCK_LAST_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, kl1, kl2, kl3, kl4, kr1, kr2, kr3, kr4, mb, ms) \
	do{\
		ml1 = vaeseq_u8(ml1, kl1); 				\
		ml2 = vaeseq_u8(ml2, kl2); 				\
		mr1 = vaeseq_u8(mr1, kr1); 				\
		mr2 = vaeseq_u8(mr2, kr2); 				\
		ml3 = vaeseq_u8(ml3, kl3); 				\
		ml4 = vaeseq_u8(ml4, kl4); 				\
		mr3 = vaeseq_u8(mr3, kr3); 				\
		mr4 = vaeseq_u8(mr4, kr4); 				\
		uint8x16_t tmpl1, tmpr1, tmpl2, tmpr2, tmpl3, tmpr3, tmpl4, tmpr4;	\
		/* Compensate for the shift rows */				\
		RINJDAEL_COMPENSATE_SR_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, tmpl1, tmpr1, tmpl2, tmpr2, tmpl3, tmpr3, tmpl4, tmpr4, mblend, mshuff); \
		ml1 = tmpl1; 				\
		ml2 = tmpl2; 				\
		mr1 = tmpr1; 				\
		mr2 = tmpr2; 				\
		ml3 = tmpl3; 				\
		ml4 = tmpl4; 				\
		mr3 = tmpr3; 				\
		mr4 = tmpr4; 				\
	}while(0)

#define DO_ENC_BLOCK_256_256_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1, k2, k3, k4) \
	do{\
		/* ShiftRows compensation constants */		\
		const uint8x16_t mblend = { 0x00,0x00,0x80,0x80,   \
									0x00,0x00,0x00,0x80,   \
									0x00,0x00,0x80,0x80,   \
									0x00,0x80,0x80,0x80 }; \
		const uint8x16_t mshuff = { 0x00,0x01,0x06,0x07,   \
									0x04,0x05,0x0a,0x0b,   \
									0x08,0x09,0x0e,0x0f,   \
									0x0c,0x0d,0x02,0x03 }; \
		/**/						\
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[0], k2[0], k3[0], k4[0], k1[1], k2[1], k3[1], k4[1], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[2], k2[2], k3[2], k4[2], k1[3], k2[3], k3[3], k4[3], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[4], k2[4], k3[4], k4[4], k1[5], k2[5], k3[5], k4[5], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[6], k2[6], k3[6], k4[6], k1[7], k2[7], k3[7], k4[7], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[8], k2[8], k3[8], k4[8], k1[9], k2[9], k3[9], k4[9], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[10], k2[10], k3[10], k4[10], k1[11], k2[11], k3[11], k4[11], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[12], k2[12], k3[12], k4[12], k1[13], k2[13], k3[13], k4[13], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[14], k2[14], k3[14], k4[14], k1[15], k2[15], k3[15], k4[15], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[16], k2[16], k3[16], k4[16], k1[17], k2[17], k3[17], k4[17], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[18], k2[18], k3[18], k4[18], k1[19], k2[19], k3[19], k4[19], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[20], k2[20], k3[20], k4[20], k1[21], k2[21], k3[21], k4[21], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[22], k2[22], k3[22], k4[22], k1[23], k2[23], k3[23], k4[23], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[24], k2[24], k3[24], k4[24], k1[25], k2[25], k3[25], k4[25], mblend, mshuff); \
		/**/						\
		ENC_256_BLOCK_LAST_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[26], k2[26], k3[26], k4[26], k1[27], k2[27], k3[27], k4[27], mblend, mshuff); \
		/**/						\
		ml1 = veorq_u8(ml1, k1[ 28]); 		\
		ml2 = veorq_u8(ml2, k2[ 28]); 		\
		mr1 = veorq_u8(mr1, k1[ 29]); 		\
		mr2 = veorq_u8(mr2, k2[ 29]); 		\
		ml3 = veorq_u8(ml3, k3[ 28]); 		\
		ml4 = veorq_u8(ml4, k4[ 28]); 		\
		mr3 = veorq_u8(mr3, k3[ 29]); 		\
		mr4 = veorq_u8(mr4, k4[ 29]); 		\
		/**/						\
	}while(0)


/* X8 variant */
#define DO_ENC_BLOCK_256_256_x8(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, ml5, mr5, ml6, mr6, ml7, mr7, ml8, mr8, k1, k2, k3, k4, k5, k6, k7, k8) \
	do{\
		/* ShiftRows compensation constants */		\
		const uint8x16_t mblend = { 0x00,0x00,0x80,0x80,   \
									0x00,0x00,0x00,0x80,   \
									0x00,0x00,0x80,0x80,   \
									0x00,0x80,0x80,0x80 }; \
		const uint8x16_t mshuff = { 0x00,0x01,0x06,0x07,   \
									0x04,0x05,0x0a,0x0b,   \
									0x08,0x09,0x0e,0x0f,   \
									0x0c,0x0d,0x02,0x03 }; \
		/**/						\
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[0], k2[0], k3[0], k4[0], k1[1], k2[1], k3[1], k4[1], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml5, mr5, ml6, mr6, ml7, mr7, ml8, mr8, k5[0], k6[0], k7[0], k8[0], k5[1], k6[1], k7[1], k8[1], mblend, mshuff); \
		\
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[2], k2[2], k3[2], k4[2], k1[3], k2[3], k3[3], k4[3], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml5, mr5, ml6, mr6, ml7, mr7, ml8, mr8, k5[2], k6[2], k7[2], k8[2], k5[3], k6[3], k7[3], k8[3], mblend, mshuff); \
		\
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[4], k2[4], k3[4], k4[4], k1[5], k2[5], k3[5], k4[5], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml5, mr5, ml6, mr6, ml7, mr7, ml8, mr8, k5[4], k6[4], k7[4], k8[4], k5[5], k6[5], k7[5], k8[5], mblend, mshuff); \
		\
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[6], k2[6], k3[6], k4[6], k1[7], k2[7], k3[7], k4[7], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml5, mr5, ml6, mr6, ml7, mr7, ml8, mr8, k5[6], k6[6], k7[6], k8[6], k5[7], k6[7], k7[7], k8[7], mblend, mshuff); \
		\
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[8], k2[8], k3[8], k4[8], k1[9], k2[9], k3[9], k4[9], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml5, mr5, ml6, mr6, ml7, mr7, ml8, mr8, k5[8], k6[8], k7[8], k8[8], k5[9], k6[9], k7[9], k8[9], mblend, mshuff); \
		\
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[10], k2[10], k3[10], k4[10], k1[11], k2[11], k3[11], k4[11], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml5, mr5, ml6, mr6, ml7, mr7, ml8, mr8, k5[10], k6[10], k7[10], k8[10], k5[11], k6[11], k7[11], k8[11], mblend, mshuff); \
		\
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[12], k2[12], k3[12], k4[12], k1[13], k2[13], k3[13], k4[13], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml5, mr5, ml6, mr6, ml7, mr7, ml8, mr8, k5[12], k6[12], k7[12], k8[12], k5[13], k6[13], k7[13], k8[13], mblend, mshuff); \
		\
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[14], k2[14], k3[14], k4[14], k1[15], k2[15], k3[15], k4[15], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml5, mr5, ml6, mr6, ml7, mr7, ml8, mr8, k5[14], k6[14], k7[14], k8[14], k5[15], k6[15], k7[15], k8[15], mblend, mshuff); \
		\
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[16], k2[16], k3[16], k4[16], k1[17], k2[17], k3[17], k4[17], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml5, mr5, ml6, mr6, ml7, mr7, ml8, mr8, k5[16], k6[16], k7[16], k8[16], k5[17], k6[17], k7[17], k8[17], mblend, mshuff); \
		\
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[18], k2[18], k3[18], k4[18], k1[19], k2[19], k3[19], k4[19], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml5, mr5, ml6, mr6, ml7, mr7, ml8, mr8, k5[18], k6[18], k7[18], k8[18], k5[19], k6[19], k7[19], k8[19], mblend, mshuff); \
		\
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[20], k2[20], k3[20], k4[20], k1[21], k2[21], k3[21], k4[21], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml5, mr5, ml6, mr6, ml7, mr7, ml8, mr8, k5[20], k6[20], k7[20], k8[20], k5[21], k6[21], k7[21], k8[21], mblend, mshuff); \
		\
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[22], k2[22], k3[22], k4[22], k1[23], k2[23], k3[23], k4[23], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml5, mr5, ml6, mr6, ml7, mr7, ml8, mr8, k5[22], k6[22], k7[22], k8[22], k5[23], k6[23], k7[23], k8[23], mblend, mshuff); \
		\
		ENC_256_BLOCK_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[24], k2[24], k3[24], k4[24], k1[25], k2[25], k3[25], k4[25], mblend, mshuff); \
		ENC_256_BLOCK_x4(ml5, mr5, ml6, mr6, ml7, mr7, ml8, mr8, k5[24], k6[24], k7[24], k8[24], k5[25], k6[25], k7[25], k8[25], mblend, mshuff); \
		\
		/**/						\
		ENC_256_BLOCK_LAST_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, k1[26], k2[26], k3[26], k4[26], k1[27], k2[27], k3[27], k4[27], mblend, mshuff); \
		ENC_256_BLOCK_LAST_x4(ml5, mr5, ml6, mr6, ml7, mr7, ml8, mr8, k5[26], k6[26], k7[26], k8[26], k5[27], k6[27], k7[27], k8[27], mblend, mshuff); \
		/**/						\
		ml1 = veorq_u8(ml1, k1[28]); \
        ml2 = veorq_u8(ml2, k2[28]); \
        mr1 = veorq_u8(mr1, k1[29]); \
        mr2 = veorq_u8(mr2, k2[29]); \
        ml3 = veorq_u8(ml3, k3[28]); \
        ml4 = veorq_u8(ml4, k4[28]); \
        mr3 = veorq_u8(mr3, k3[29]); \
        mr4 = veorq_u8(mr4, k4[29]); \
        \
        ml5 = veorq_u8(ml5, k5[28]); \
        ml6 = veorq_u8(ml6, k6[28]); \
        mr5 = veorq_u8(mr5, k5[29]); \
        mr6 = veorq_u8(mr6, k6[29]); \
        ml7 = veorq_u8(ml7, k7[28]); \
        ml8 = veorq_u8(ml8, k8[28]); \
        mr7 = veorq_u8(mr7, k7[29]); \
        mr8 = veorq_u8(mr8, k8[29]); \
	}while(0)

#define SSE2NEON_AES_SBOX(w)                                           \
    {                                                                  \
        w(0x63), w(0x7c), w(0x77), w(0x7b), w(0xf2), w(0x6b), w(0x6f), \
        w(0xc5), w(0x30), w(0x01), w(0x67), w(0x2b), w(0xfe), w(0xd7), \
        w(0xab), w(0x76), w(0xca), w(0x82), w(0xc9), w(0x7d), w(0xfa), \
        w(0x59), w(0x47), w(0xf0), w(0xad), w(0xd4), w(0xa2), w(0xaf), \
        w(0x9c), w(0xa4), w(0x72), w(0xc0), w(0xb7), w(0xfd), w(0x93), \
        w(0x26), w(0x36), w(0x3f), w(0xf7), w(0xcc), w(0x34), w(0xa5), \
        w(0xe5), w(0xf1), w(0x71), w(0xd8), w(0x31), w(0x15), w(0x04), \
        w(0xc7), w(0x23), w(0xc3), w(0x18), w(0x96), w(0x05), w(0x9a), \
        w(0x07), w(0x12), w(0x80), w(0xe2), w(0xeb), w(0x27), w(0xb2), \
        w(0x75), w(0x09), w(0x83), w(0x2c), w(0x1a), w(0x1b), w(0x6e), \
        w(0x5a), w(0xa0), w(0x52), w(0x3b), w(0xd6), w(0xb3), w(0x29), \
        w(0xe3), w(0x2f), w(0x84), w(0x53), w(0xd1), w(0x00), w(0xed), \
        w(0x20), w(0xfc), w(0xb1), w(0x5b), w(0x6a), w(0xcb), w(0xbe), \
        w(0x39), w(0x4a), w(0x4c), w(0x58), w(0xcf), w(0xd0), w(0xef), \
        w(0xaa), w(0xfb), w(0x43), w(0x4d), w(0x33), w(0x85), w(0x45), \
        w(0xf9), w(0x02), w(0x7f), w(0x50), w(0x3c), w(0x9f), w(0xa8), \
        w(0x51), w(0xa3), w(0x40), w(0x8f), w(0x92), w(0x9d), w(0x38), \
        w(0xf5), w(0xbc), w(0xb6), w(0xda), w(0x21), w(0x10), w(0xff), \
        w(0xf3), w(0xd2), w(0xcd), w(0x0c), w(0x13), w(0xec), w(0x5f), \
        w(0x97), w(0x44), w(0x17), w(0xc4), w(0xa7), w(0x7e), w(0x3d), \
        w(0x64), w(0x5d), w(0x19), w(0x73), w(0x60), w(0x81), w(0x4f), \
        w(0xdc), w(0x22), w(0x2a), w(0x90), w(0x88), w(0x46), w(0xee), \
        w(0xb8), w(0x14), w(0xde), w(0x5e), w(0x0b), w(0xdb), w(0xe0), \
        w(0x32), w(0x3a), w(0x0a), w(0x49), w(0x06), w(0x24), w(0x5c), \
        w(0xc2), w(0xd3), w(0xac), w(0x62), w(0x91), w(0x95), w(0xe4), \
        w(0x79), w(0xe7), w(0xc8), w(0x37), w(0x6d), w(0x8d), w(0xd5), \
        w(0x4e), w(0xa9), w(0x6c), w(0x56), w(0xf4), w(0xea), w(0x65), \
        w(0x7a), w(0xae), w(0x08), w(0xba), w(0x78), w(0x25), w(0x2e), \
        w(0x1c), w(0xa6), w(0xb4), w(0xc6), w(0xe8), w(0xdd), w(0x74), \
        w(0x1f), w(0x4b), w(0xbd), w(0x8b), w(0x8a), w(0x70), w(0x3e), \
        w(0xb5), w(0x66), w(0x48), w(0x03), w(0xf6), w(0x0e), w(0x61), \
        w(0x35), w(0x57), w(0xb9), w(0x86), w(0xc1), w(0x1d), w(0x9e), \
        w(0xe1), w(0xf8), w(0x98), w(0x11), w(0x69), w(0xd9), w(0x8e), \
        w(0x94), w(0x9b), w(0x1e), w(0x87), w(0xe9), w(0xce), w(0x55), \
        w(0x28), w(0xdf), w(0x8c), w(0xa1), w(0x89), w(0x0d), w(0xbf), \
        w(0xe6), w(0x42), w(0x68), w(0x41), w(0x99), w(0x2d), w(0x0f), \
        w(0xb0), w(0x54), w(0xbb), w(0x16)                             \
    }
#define SSE2NEON_AES_RSBOX(w)                                          \
    {                                                                  \
        w(0x52), w(0x09), w(0x6a), w(0xd5), w(0x30), w(0x36), w(0xa5), \
        w(0x38), w(0xbf), w(0x40), w(0xa3), w(0x9e), w(0x81), w(0xf3), \
        w(0xd7), w(0xfb), w(0x7c), w(0xe3), w(0x39), w(0x82), w(0x9b), \
        w(0x2f), w(0xff), w(0x87), w(0x34), w(0x8e), w(0x43), w(0x44), \
        w(0xc4), w(0xde), w(0xe9), w(0xcb), w(0x54), w(0x7b), w(0x94), \
        w(0x32), w(0xa6), w(0xc2), w(0x23), w(0x3d), w(0xee), w(0x4c), \
        w(0x95), w(0x0b), w(0x42), w(0xfa), w(0xc3), w(0x4e), w(0x08), \
        w(0x2e), w(0xa1), w(0x66), w(0x28), w(0xd9), w(0x24), w(0xb2), \
        w(0x76), w(0x5b), w(0xa2), w(0x49), w(0x6d), w(0x8b), w(0xd1), \
        w(0x25), w(0x72), w(0xf8), w(0xf6), w(0x64), w(0x86), w(0x68), \
        w(0x98), w(0x16), w(0xd4), w(0xa4), w(0x5c), w(0xcc), w(0x5d), \
        w(0x65), w(0xb6), w(0x92), w(0x6c), w(0x70), w(0x48), w(0x50), \
        w(0xfd), w(0xed), w(0xb9), w(0xda), w(0x5e), w(0x15), w(0x46), \
        w(0x57), w(0xa7), w(0x8d), w(0x9d), w(0x84), w(0x90), w(0xd8), \
        w(0xab), w(0x00), w(0x8c), w(0xbc), w(0xd3), w(0x0a), w(0xf7), \
        w(0xe4), w(0x58), w(0x05), w(0xb8), w(0xb3), w(0x45), w(0x06), \
        w(0xd0), w(0x2c), w(0x1e), w(0x8f), w(0xca), w(0x3f), w(0x0f), \
        w(0x02), w(0xc1), w(0xaf), w(0xbd), w(0x03), w(0x01), w(0x13), \
        w(0x8a), w(0x6b), w(0x3a), w(0x91), w(0x11), w(0x41), w(0x4f), \
        w(0x67), w(0xdc), w(0xea), w(0x97), w(0xf2), w(0xcf), w(0xce), \
        w(0xf0), w(0xb4), w(0xe6), w(0x73), w(0x96), w(0xac), w(0x74), \
        w(0x22), w(0xe7), w(0xad), w(0x35), w(0x85), w(0xe2), w(0xf9), \
        w(0x37), w(0xe8), w(0x1c), w(0x75), w(0xdf), w(0x6e), w(0x47), \
        w(0xf1), w(0x1a), w(0x71), w(0x1d), w(0x29), w(0xc5), w(0x89), \
        w(0x6f), w(0xb7), w(0x62), w(0x0e), w(0xaa), w(0x18), w(0xbe), \
        w(0x1b), w(0xfc), w(0x56), w(0x3e), w(0x4b), w(0xc6), w(0xd2), \
        w(0x79), w(0x20), w(0x9a), w(0xdb), w(0xc0), w(0xfe), w(0x78), \
        w(0xcd), w(0x5a), w(0xf4), w(0x1f), w(0xdd), w(0xa8), w(0x33), \
        w(0x88), w(0x07), w(0xc7), w(0x31), w(0xb1), w(0x12), w(0x10), \
        w(0x59), w(0x27), w(0x80), w(0xec), w(0x5f), w(0x60), w(0x51), \
        w(0x7f), w(0xa9), w(0x19), w(0xb5), w(0x4a), w(0x0d), w(0x2d), \
        w(0xe5), w(0x7a), w(0x9f), w(0x93), w(0xc9), w(0x9c), w(0xef), \
        w(0xa0), w(0xe0), w(0x3b), w(0x4d), w(0xae), w(0x2a), w(0xf5), \
        w(0xb0), w(0xc8), w(0xeb), w(0xbb), w(0x3c), w(0x83), w(0x53), \
        w(0x99), w(0x61), w(0x17), w(0x2b), w(0x04), w(0x7e), w(0xba), \
        w(0x77), w(0xd6), w(0x26), w(0xe1), w(0x69), w(0x14), w(0x63), \
        w(0x55), w(0x21), w(0x0c), w(0x7d)                             \
    }

#define SSE2NEON_AES_H0(x) (x)
const uint8_t _sse2neon_sbox[256] = SSE2NEON_AES_SBOX(SSE2NEON_AES_H0);
const uint8_t _sse2neon_rsbox[256] = SSE2NEON_AES_RSBOX(SSE2NEON_AES_H0);
#undef SSE2NEON_AES_H0

static inline uint8x16_t _aes_subbytes(uint8x16_t x)
{
    uint8x16_t v = vqtbl4q_u8(vld1q_u8_x4(_sse2neon_sbox), x);
    v = vqtbx4q_u8(v, vld1q_u8_x4(_sse2neon_sbox + 0x40),
                   vsubq_u8(x, vdupq_n_u8(0x40)));
    v = vqtbx4q_u8(v, vld1q_u8_x4(_sse2neon_sbox + 0x80),
                   vsubq_u8(x, vdupq_n_u8(0x80)));
    v = vqtbx4q_u8(v, vld1q_u8_x4(_sse2neon_sbox + 0xc0),
                   vsubq_u8(x, vdupq_n_u8(0xc0)));
    return v;
}

static inline uint8x16_t aeskeygenassist_neon(uint8x16_t a, const int rcon)
{
    // AES S-box
    uint8x16_t sub = _aes_subbytes(a);

    // view as u32x4
    uint32x4_t sub_u32 = vreinterpretq_u32_u8(sub);

    // rotate bytes inside each 32-bit lane
    uint32x4_t rot = vorrq_u32(vshrq_n_u32(sub_u32, 8), vshlq_n_u32(sub_u32, 24));

    // lane-wise xor
    uint32x4_t rcon_vec = vdupq_n_u32((uint32_t)rcon);

    // xor rcon
    uint32x4_t rot_xor = veorq_u32(rot, rcon_vec);

    // return as bytes
    return vreinterpretq_u8_u32(vtrn2q_u32(sub_u32, rot_xor));
}


#define AES_128_key_exp(k, rcon) aes_128_key_expansion(k, aeskeygenassist_neon(k, rcon))

static inline uint8x16_t aes_128_key_expansion(uint8x16_t key, uint8x16_t keygened){
	uint32x4_t tmp = vreinterpretq_u32_u8(keygened); // change view
	tmp = vdupq_laneq_u32(tmp, 3); // broadcast the 4th word
	keygened = vreinterpretq_u8_u32(tmp); // change view

	key = veorq_u8(key, vextq_u8(vdupq_n_u8(0), key, 12)); // (16-4)
	key = veorq_u8(key, vextq_u8(vdupq_n_u8(0), key, 12));
	key = veorq_u8(key, vextq_u8(vdupq_n_u8(0), key, 12));
	return veorq_u8(key, keygened);
}

static inline void KEY_256_ASSIST_1(uint8x16_t* temp1, uint8x16_t * temp2)
{
	uint8x16_t temp4;
	uint32x4_t t = vreinterpretq_u32_u8(*temp2); // change view
	t = vdupq_laneq_u32(t, 3); // broadcast the 4th word (_MM_SHUFFLE(3,3,3,3) = 0xff)
	*temp2 = vreinterpretq_u8_u32(t); // change view

	temp4 = vextq_u8(vdupq_n_u8(0), *temp1, 12);
	*temp1 = veorq_u8(*temp1, temp4);
	temp4 = vextq_u8(vdupq_n_u8(0), temp4, 12);
	*temp1 = veorq_u8(*temp1, temp4);
	temp4 = vextq_u8(vdupq_n_u8(0), temp4, 12);
	*temp1 = veorq_u8(*temp1, temp4);
	*temp1 = veorq_u8(*temp1, *temp2);
}
static inline void KEY_256_ASSIST_2(uint8x16_t* temp1, uint8x16_t * temp3)
{
	uint8x16_t temp2,temp4;
	temp4 = aeskeygenassist_neon(*temp1, 0x0);

	uint32x4_t t = vreinterpretq_u32_u8(temp4); // change view
	t = vdupq_laneq_u32(t, 2); // broadcast the 3th word (_MM_SHUFFLE(2,2,2,2) = 0xaa)
	temp2 = vreinterpretq_u8_u32(t); // change view
	
	temp4 = vextq_u8(vdupq_n_u8(0), *temp3, 12);
	*temp3 = veorq_u8(*temp3, temp4);
	temp4 = vextq_u8(vdupq_n_u8(0), temp4, 12);
	*temp3 = veorq_u8(*temp3, temp4);
	temp4 = vextq_u8(vdupq_n_u8(0), temp4, 12);
	*temp3 = veorq_u8(*temp3, temp4);
	*temp3 = veorq_u8(*temp3, temp2);
} 

/* ==== Public APIs ===== */

WEAK int aes128_aes_neon_setkey_enc(rijndael_aes_neon_ctx *ctx, const uint8_t enc_key[16])
{
	uint8x16_t *Key_Schedule = (uint8x16_t*)ctx->rk;
	/**/
	ctx->rtype = AES128;
	/**/
	Key_Schedule[0] = vld1q_u8((const uint8_t*) enc_key);
	Key_Schedule[1]  = AES_128_key_exp(Key_Schedule[0], 0x01);
	Key_Schedule[2]  = AES_128_key_exp(Key_Schedule[1], 0x02);
	Key_Schedule[3]  = AES_128_key_exp(Key_Schedule[2], 0x04);
	Key_Schedule[4]  = AES_128_key_exp(Key_Schedule[3], 0x08);
	Key_Schedule[5]  = AES_128_key_exp(Key_Schedule[4], 0x10);
	Key_Schedule[6]  = AES_128_key_exp(Key_Schedule[5], 0x20);
	Key_Schedule[7]  = AES_128_key_exp(Key_Schedule[6], 0x40);
	Key_Schedule[8]  = AES_128_key_exp(Key_Schedule[7], 0x80);
	Key_Schedule[9]  = AES_128_key_exp(Key_Schedule[8], 0x1B);
	Key_Schedule[10] = AES_128_key_exp(Key_Schedule[9], 0x36);

	return 0;
}


WEAK int aes256_aes_neon_setkey_enc(rijndael_aes_neon_ctx *ctx, const uint8_t enc_key[32])
{
	uint8x16_t temp1, temp2, temp3;
	uint8x16_t *Key_Schedule = (uint8x16_t*)ctx->rk;
	/**/
	ctx->rtype = AES256;
	/**/
	temp1 = vld1q_u8((uint8_t*)enc_key);
	temp3 = vld1q_u8((uint8_t*)(enc_key+16));
	Key_Schedule[0] = temp1;
	Key_Schedule[1] = temp3;
	temp2 = aeskeygenassist_neon(temp3,0x01);
	KEY_256_ASSIST_1(&temp1, &temp2);
	Key_Schedule[2]=temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	Key_Schedule[3]=temp3;
	temp2 = aeskeygenassist_neon(temp3,0x02);
	KEY_256_ASSIST_1(&temp1, &temp2);
	Key_Schedule[4]=temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	Key_Schedule[5]=temp3;
	temp2 = aeskeygenassist_neon(temp3,0x04);
	KEY_256_ASSIST_1(&temp1, &temp2);
	Key_Schedule[6]=temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	Key_Schedule[7]=temp3;
	temp2 = aeskeygenassist_neon(temp3,0x08);
	KEY_256_ASSIST_1(&temp1, &temp2);
	Key_Schedule[8]=temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	Key_Schedule[9]=temp3;
	temp2 = aeskeygenassist_neon(temp3,0x10);
	KEY_256_ASSIST_1(&temp1, &temp2);
	Key_Schedule[10]=temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	Key_Schedule[11]=temp3;
	temp2 = aeskeygenassist_neon(temp3,0x20);
	KEY_256_ASSIST_1(&temp1, &temp2);
	Key_Schedule[12]=temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	Key_Schedule[13]=temp3;
	temp2 = aeskeygenassist_neon(temp3,0x40);
	KEY_256_ASSIST_1(&temp1, &temp2);
	Key_Schedule[14]=temp1;

	return 0;
}
 
WEAK int rijndael256_aes_neon_setkey_enc(rijndael_aes_neon_ctx *ctx, const uint8_t enc_key[32])
{
	uint8x16_t temp1, temp2, temp3;
	uint8x16_t *Key_Schedule = (uint8x16_t*)ctx->rk;
	/**/
	ctx->rtype = RIJNDAEL_256_256;
	/**/
	temp1 = vld1q_u8((uint8_t*)enc_key);
	temp3 = vld1q_u8((uint8_t*)(enc_key+16));
	Key_Schedule[0] = temp1;
	Key_Schedule[1] = temp3;
	temp2 = aeskeygenassist_neon(temp3,0x01);
	KEY_256_ASSIST_1(&temp1, &temp2);
	Key_Schedule[2]=temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	Key_Schedule[3]=temp3;
	temp2 = aeskeygenassist_neon(temp3,0x02);
	KEY_256_ASSIST_1(&temp1, &temp2);
	Key_Schedule[4]=temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	Key_Schedule[5]=temp3;
	temp2 = aeskeygenassist_neon(temp3,0x04);
	KEY_256_ASSIST_1(&temp1, &temp2);
	Key_Schedule[6]=temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	Key_Schedule[7]=temp3;
	temp2 = aeskeygenassist_neon(temp3,0x08);
	KEY_256_ASSIST_1(&temp1, &temp2);
	Key_Schedule[8]=temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	Key_Schedule[9]=temp3;
	temp2 = aeskeygenassist_neon(temp3,0x10);
	KEY_256_ASSIST_1(&temp1, &temp2);
	Key_Schedule[10]=temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	Key_Schedule[11]=temp3;
	temp2 = aeskeygenassist_neon(temp3,0x20);
	KEY_256_ASSIST_1(&temp1, &temp2);
	Key_Schedule[12]=temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	Key_Schedule[13]=temp3;
	temp2 = aeskeygenassist_neon(temp3,0x40);
	KEY_256_ASSIST_1(&temp1, &temp2);
	Key_Schedule[14]=temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	Key_Schedule[15]=temp3;
	temp2 = aeskeygenassist_neon(temp3,0x80);
	KEY_256_ASSIST_1(&temp1, &temp2);
	Key_Schedule[16]=temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	Key_Schedule[17]=temp3;
	temp2 = aeskeygenassist_neon(temp3,0x1b);
	KEY_256_ASSIST_1(&temp1, &temp2);
	Key_Schedule[18]=temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	Key_Schedule[19]=temp3;
	temp2 = aeskeygenassist_neon(temp3,0x36);
	KEY_256_ASSIST_1(&temp1, &temp2);
	Key_Schedule[20]=temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	Key_Schedule[21]=temp3;
	temp2 = aeskeygenassist_neon(temp3,0x6c);
	KEY_256_ASSIST_1(&temp1, &temp2);
	Key_Schedule[22]=temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	Key_Schedule[23]=temp3;
	temp2 = aeskeygenassist_neon(temp3,0xd8);
	KEY_256_ASSIST_1(&temp1, &temp2);
	Key_Schedule[24]=temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	Key_Schedule[25]=temp3;
	temp2 = aeskeygenassist_neon(temp3,0xab);
	KEY_256_ASSIST_1(&temp1, &temp2);
	Key_Schedule[26]=temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	Key_Schedule[27]=temp3;
	temp2 = aeskeygenassist_neon(temp3,0x4d);
	KEY_256_ASSIST_1(&temp1, &temp2);
	Key_Schedule[28]=temp1;
	KEY_256_ASSIST_2(&temp1, &temp3);
	Key_Schedule[29]=temp3;

	return 0;

}

WEAK int aes128_aes_neon_enc(const rijndael_aes_neon_ctx *ctx, const uint8_t plainText[16], uint8_t cipherText[16]){
	uint8x16_t m;
	int ret = -1;

	if((ctx == NULL) || (ctx->rtype != AES128)){
		goto err;
	}

	m = vld1q_u8((uint8_t *) plainText);

	DO_ENC_BLOCK_128_128(m, ctx->rk);

	vst1q_u8((uint8_t *) cipherText, m);

	ret = 0;
err:
	return ret;
}

WEAK int aes128_aes_neon_enc_x2(const rijndael_aes_neon_ctx *ctx1, const rijndael_aes_neon_ctx *ctx2, const uint8_t plainText1[16], const uint8_t plainText2[16], uint8_t cipherText1[16], uint8_t cipherText2[16]){
	uint8x16_t m1, m2;
	int ret = -1;

	if((ctx1 == NULL) || (ctx1->rtype != AES128)){
		goto err;
	}
	if((ctx2 == NULL) || (ctx2->rtype != AES128)){
		goto err;
	}

	m1 = vld1q_u8((uint8_t *) plainText1);
	m2 = vld1q_u8((uint8_t *) plainText2);

	DO_ENC_BLOCK_128_128_x2(m1, m2, ctx1->rk, ctx2->rk);

	vst1q_u8((uint8_t *) cipherText1, m1);
	vst1q_u8((uint8_t *) cipherText2, m2);

	ret = 0;
err:
	return ret;
}

WEAK int aes128_aes_neon_enc_x4(const rijndael_aes_neon_ctx *ctx1, const rijndael_aes_neon_ctx *ctx2, const rijndael_aes_neon_ctx *ctx3, const rijndael_aes_neon_ctx *ctx4,
		const uint8_t plainText1[16], const uint8_t plainText2[16], const uint8_t plainText3[16], const uint8_t plainText4[16],
		uint8_t cipherText1[16], uint8_t cipherText2[16], uint8_t cipherText3[16], uint8_t cipherText4[16]){
	uint8x16_t m1, m2, m3, m4;
	int ret = -1;

	if((ctx1 == NULL) || (ctx1->rtype != AES128)){
		goto err;
	}
	if((ctx2 == NULL) || (ctx2->rtype != AES128)){
		goto err;
	}
	if((ctx3 == NULL) || (ctx3->rtype != AES128)){
		goto err;
	}
	if((ctx4 == NULL) || (ctx4->rtype != AES128)){
		goto err;
	}

	m1 = vld1q_u8((uint8_t *) plainText1);
	m2 = vld1q_u8((uint8_t *) plainText2);
	m3 = vld1q_u8((uint8_t *) plainText3);
	m4 = vld1q_u8((uint8_t *) plainText4);

	DO_ENC_BLOCK_128_128_x4(m1, m2, m3, m4, ctx1->rk, ctx2->rk, ctx3->rk, ctx4->rk);

	vst1q_u8((uint8_t *) cipherText1, m1);
	vst1q_u8((uint8_t *) cipherText2, m2);
	vst1q_u8((uint8_t *) cipherText3, m3);
	vst1q_u8((uint8_t *) cipherText4, m4);

	ret = 0;
err:
	return ret;
}

WEAK int aes128_aes_neon_enc_x8(const rijndael_aes_neon_ctx *ctx1, const rijndael_aes_neon_ctx *ctx2, const rijndael_aes_neon_ctx *ctx3, const rijndael_aes_neon_ctx *ctx4,
                  const rijndael_aes_neon_ctx *ctx5, const rijndael_aes_neon_ctx *ctx6, const rijndael_aes_neon_ctx *ctx7, const rijndael_aes_neon_ctx *ctx8,
                const uint8_t plainText1[16], const uint8_t plainText2[16], const uint8_t plainText3[16], const uint8_t plainText4[16],
                const uint8_t plainText5[16], const uint8_t plainText6[16], const uint8_t plainText7[16], const uint8_t plainText8[16],
                uint8_t cipherText1[16], uint8_t cipherText2[16], uint8_t cipherText3[16], uint8_t cipherText4[16],
                uint8_t cipherText5[16], uint8_t cipherText6[16], uint8_t cipherText7[16], uint8_t cipherText8[16])
{
	uint8x16_t m1, m2, m3, m4, m5, m6, m7, m8;
	int ret = -1;

	if((ctx1 == NULL) || (ctx1->rtype != AES128)){
		goto err;
	}
	if((ctx2 == NULL) || (ctx2->rtype != AES128)){
		goto err;
	}
	if((ctx3 == NULL) || (ctx3->rtype != AES128)){
		goto err;
	}
	if((ctx4 == NULL) || (ctx4->rtype != AES128)){
		goto err;
	}
	if((ctx5 == NULL) || (ctx5->rtype != AES128)){
		goto err;
	}
	if((ctx6 == NULL) || (ctx6->rtype != AES128)){
		goto err;
	}
	if((ctx7 == NULL) || (ctx7->rtype != AES128)){
		goto err;
	}
	if((ctx8 == NULL) || (ctx8->rtype != AES128)){
		goto err;
	}

	m1 = vld1q_u8((uint8_t *) plainText1);
	m2 = vld1q_u8((uint8_t *) plainText2);
	m3 = vld1q_u8((uint8_t *) plainText3);
	m4 = vld1q_u8((uint8_t *) plainText4);
	m5 = vld1q_u8((uint8_t *) plainText5);
	m6 = vld1q_u8((uint8_t *) plainText6);
	m7 = vld1q_u8((uint8_t *) plainText7);
	m8 = vld1q_u8((uint8_t *) plainText8);

	DO_ENC_BLOCK_128_128_x8(m1, m2, m3, m4, m5, m6, m7, m8,
				ctx1->rk, ctx2->rk, ctx3->rk, ctx4->rk, ctx5->rk, ctx6->rk, ctx7->rk, ctx8->rk);

	vst1q_u8((uint8_t *) cipherText1, m1);
	vst1q_u8((uint8_t *) cipherText2, m2);
	vst1q_u8((uint8_t *) cipherText3, m3);
	vst1q_u8((uint8_t *) cipherText4, m4);
	vst1q_u8((uint8_t *) cipherText5, m5);
	vst1q_u8((uint8_t *) cipherText6, m6);
	vst1q_u8((uint8_t *) cipherText7, m7);
	vst1q_u8((uint8_t *) cipherText8, m8);

	ret = 0;
err:
	return ret;
}

WEAK int aes256_aes_neon_enc(const rijndael_aes_neon_ctx *ctx, const uint8_t plainText[16], uint8_t cipherText[16]){
	uint8x16_t m;
	int ret = -1;

	if((ctx == NULL) || (ctx->rtype != AES256)){
		goto err;
	}

	m = vld1q_u8((uint8_t *) plainText);

	DO_ENC_BLOCK_128_256(m, ctx->rk);

	vst1q_u8((uint8_t *) cipherText, m);

	ret = 0;
err:
	return ret;
}

WEAK int aes256_aes_neon_enc_x2(const rijndael_aes_neon_ctx *ctx1, const rijndael_aes_neon_ctx *ctx2, const uint8_t plainText1[16], const uint8_t plainText2[16], uint8_t cipherText1[16], uint8_t cipherText2[16]){
	uint8x16_t m1, m2;
	int ret = -1;

	if((ctx1 == NULL) || (ctx1->rtype != AES256)){
		goto err;
	}
	if((ctx2 == NULL) || (ctx2->rtype != AES256)){
		goto err;
	}

	m1 = vld1q_u8((uint8_t *) plainText1);
	m2 = vld1q_u8((uint8_t *) plainText2);

	DO_ENC_BLOCK_128_256_x2(m1, m2, ctx1->rk, ctx2->rk);

	vst1q_u8((uint8_t *) cipherText1, m1);
	vst1q_u8((uint8_t *) cipherText2, m2);

	ret = 0;
err:
	return ret;
}

WEAK int aes256_aes_neon_enc_x4(const rijndael_aes_neon_ctx *ctx1, const rijndael_aes_neon_ctx *ctx2, const rijndael_aes_neon_ctx *ctx3, const rijndael_aes_neon_ctx *ctx4,
		const uint8_t plainText1[16], const uint8_t plainText2[16], const uint8_t plainText3[16], const uint8_t plainText4[16],
		uint8_t cipherText1[16], uint8_t cipherText2[16], uint8_t cipherText3[16], uint8_t cipherText4[16]){
	uint8x16_t m1, m2, m3, m4;
	int ret = -1;

	if((ctx1 == NULL) || (ctx1->rtype != AES256)){
		goto err;
	}
	if((ctx2 == NULL) || (ctx2->rtype != AES256)){
		goto err;
	}
	if((ctx3 == NULL) || (ctx3->rtype != AES256)){
		goto err;
	}
	if((ctx4 == NULL) || (ctx4->rtype != AES256)){
		goto err;
	}

	m1 = vld1q_u8((uint8_t *) plainText1);
	m2 = vld1q_u8((uint8_t *) plainText2);
	m3 = vld1q_u8((uint8_t *) plainText3);
	m4 = vld1q_u8((uint8_t *) plainText4);

	DO_ENC_BLOCK_128_256_x4(m1, m2, m3, m4, ctx1->rk, ctx2->rk, ctx3->rk, ctx4->rk);

	vst1q_u8((uint8_t *) cipherText1, m1);
	vst1q_u8((uint8_t *) cipherText2, m2);
	vst1q_u8((uint8_t *) cipherText3, m3);
	vst1q_u8((uint8_t *) cipherText4, m4);

	ret = 0;
err:
	return ret;
}

WEAK int aes256_aes_neon_enc_x8(const rijndael_aes_neon_ctx *ctx1, const rijndael_aes_neon_ctx *ctx2, const rijndael_aes_neon_ctx *ctx3, const rijndael_aes_neon_ctx *ctx4,
                  const rijndael_aes_neon_ctx *ctx5, const rijndael_aes_neon_ctx *ctx6, const rijndael_aes_neon_ctx *ctx7, const rijndael_aes_neon_ctx *ctx8,
                const uint8_t plainText1[16], const uint8_t plainText2[16], const uint8_t plainText3[16], const uint8_t plainText4[16],
                const uint8_t plainText5[16], const uint8_t plainText6[16], const uint8_t plainText7[16], const uint8_t plainText8[16],
                uint8_t cipherText1[16], uint8_t cipherText2[16], uint8_t cipherText3[16], uint8_t cipherText4[16],
                uint8_t cipherText5[16], uint8_t cipherText6[16], uint8_t cipherText7[16], uint8_t cipherText8[16])
{
	uint8x16_t m1, m2, m3, m4, m5, m6, m7, m8;
	int ret = -1;

	if((ctx1 == NULL) || (ctx1->rtype != AES256)){
		goto err;
	}
	if((ctx2 == NULL) || (ctx2->rtype != AES256)){
		goto err;
	}
	if((ctx3 == NULL) || (ctx3->rtype != AES256)){
		goto err;
	}
	if((ctx4 == NULL) || (ctx4->rtype != AES256)){
		goto err;
	}
	if((ctx5 == NULL) || (ctx5->rtype != AES256)){
		goto err;
	}
	if((ctx6 == NULL) || (ctx6->rtype != AES256)){
		goto err;
	}
	if((ctx7 == NULL) || (ctx7->rtype != AES256)){
		goto err;
	}
	if((ctx8 == NULL) || (ctx8->rtype != AES256)){
		goto err;
	}

	m1 = vld1q_u8((uint8_t *) plainText1);
	m2 = vld1q_u8((uint8_t *) plainText2);
	m3 = vld1q_u8((uint8_t *) plainText3);
	m4 = vld1q_u8((uint8_t *) plainText4);
	m5 = vld1q_u8((uint8_t *) plainText5);
	m6 = vld1q_u8((uint8_t *) plainText6);
	m7 = vld1q_u8((uint8_t *) plainText7);
	m8 = vld1q_u8((uint8_t *) plainText8);

	DO_ENC_BLOCK_128_256_x8(m1, m2, m3, m4, m5, m6, m7, m8,
				ctx1->rk, ctx2->rk, ctx3->rk, ctx4->rk, ctx5->rk, ctx6->rk, ctx7->rk, ctx8->rk);

	vst1q_u8((uint8_t *) cipherText1, m1);
	vst1q_u8((uint8_t *) cipherText2, m2);
	vst1q_u8((uint8_t *) cipherText3, m3);
	vst1q_u8((uint8_t *) cipherText4, m4);
	vst1q_u8((uint8_t *) cipherText5, m5);
	vst1q_u8((uint8_t *) cipherText6, m6);
	vst1q_u8((uint8_t *) cipherText7, m7);
	vst1q_u8((uint8_t *) cipherText8, m8);

	ret = 0;
err:
	return ret;
}

WEAK int rijndael256_aes_neon_enc(const rijndael_aes_neon_ctx *ctx, const uint8_t plainText[32], uint8_t cipherText[32]){
	uint8x16_t ml, mr;
	int ret = -1;

	if((ctx == NULL) || (ctx->rtype != RIJNDAEL_256_256)){
		goto err;
	}

	ml = vld1q_u8((uint8_t *) &plainText[0]);
	mr = vld1q_u8((uint8_t *) &plainText[16]);

	DO_ENC_BLOCK_256_256(ml, mr, ctx->rk);

	vst1q_u8((uint8_t *) &cipherText[0], ml);
	vst1q_u8((uint8_t *) &cipherText[16], mr);

	ret = 0;
err:
	return ret;
}

WEAK int rijndael256_aes_neon_enc_x2(const rijndael_aes_neon_ctx *ctx1, const rijndael_aes_neon_ctx *ctx2,
			const uint8_t plainText1[32], const uint8_t plainText2[32],
			uint8_t cipherText1[32], uint8_t cipherText2[32]){
	uint8x16_t ml1, mr1, ml2, mr2;
	int ret = -1;

	if((ctx1 == NULL) || (ctx1->rtype != RIJNDAEL_256_256)){
		goto err;
	}
	if((ctx2 == NULL) || (ctx2->rtype != RIJNDAEL_256_256)){
		goto err;
	}

	ml1 = vld1q_u8((uint8_t *) &plainText1[0]);
	mr1 = vld1q_u8((uint8_t *) &plainText1[16]);
	ml2 = vld1q_u8((uint8_t *) &plainText2[0]);
	mr2 = vld1q_u8((uint8_t *) &plainText2[16]);

	DO_ENC_BLOCK_256_256_x2(ml1, mr1, ml2, mr2, ctx1->rk, ctx2->rk);

	vst1q_u8((uint8_t *) &cipherText1[0], ml1);
	vst1q_u8((uint8_t *) &cipherText1[16], mr1);
	vst1q_u8((uint8_t *) &cipherText2[0], ml2);
	vst1q_u8((uint8_t *) &cipherText2[16], mr2);

	ret = 0;
err:
	return ret;
}

WEAK int rijndael256_aes_neon_enc_x4(const rijndael_aes_neon_ctx *ctx1, const rijndael_aes_neon_ctx *ctx2, const rijndael_aes_neon_ctx *ctx3, const rijndael_aes_neon_ctx *ctx4,
		const uint8_t plainText1[32], const uint8_t plainText2[32], const uint8_t plainText3[32], const uint8_t plainText4[32],
		uint8_t cipherText1[32], uint8_t cipherText2[32], uint8_t cipherText3[32], uint8_t cipherText4[32]){
	uint8x16_t ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4;
	int ret = -1;

	if((ctx1 == NULL) || (ctx1->rtype != RIJNDAEL_256_256)){
		goto err;
	}
	if((ctx2 == NULL) || (ctx2->rtype != RIJNDAEL_256_256)){
		goto err;
	}
	if((ctx3 == NULL) || (ctx3->rtype != RIJNDAEL_256_256)){
		goto err;
	}
	if((ctx4 == NULL) || (ctx4->rtype != RIJNDAEL_256_256)){
		goto err;
	}

	ml1 = vld1q_u8((uint8_t *) &plainText1[0]);
	mr1 = vld1q_u8((uint8_t *) &plainText1[16]);
	ml2 = vld1q_u8((uint8_t *) &plainText2[0]);
	mr2 = vld1q_u8((uint8_t *) &plainText2[16]);
	ml3 = vld1q_u8((uint8_t *) &plainText3[0]);
	mr3 = vld1q_u8((uint8_t *) &plainText3[16]);
	ml4 = vld1q_u8((uint8_t *) &plainText4[0]);
	mr4 = vld1q_u8((uint8_t *) &plainText4[16]);

	DO_ENC_BLOCK_256_256_x4(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, ctx1->rk, ctx2->rk, ctx3->rk, ctx4->rk);

	vst1q_u8((uint8_t *) &cipherText1[0], ml1);
	vst1q_u8((uint8_t *) &cipherText1[16], mr1);
	vst1q_u8((uint8_t *) &cipherText2[0], ml2);
	vst1q_u8((uint8_t *) &cipherText2[16], mr2);
	vst1q_u8((uint8_t *) &cipherText3[0], ml3);
	vst1q_u8((uint8_t *) &cipherText3[16], mr3);
	vst1q_u8((uint8_t *) &cipherText4[0], ml4);
	vst1q_u8((uint8_t *) &cipherText4[16], mr4);

	ret = 0;
err:
	return ret;
}

WEAK int rijndael256_aes_neon_enc_x8(const rijndael_aes_neon_ctx *ctx1, const rijndael_aes_neon_ctx *ctx2, const rijndael_aes_neon_ctx *ctx3, const rijndael_aes_neon_ctx *ctx4,
                  const rijndael_aes_neon_ctx *ctx5, const rijndael_aes_neon_ctx *ctx6, const rijndael_aes_neon_ctx *ctx7, const rijndael_aes_neon_ctx *ctx8,
                const uint8_t plainText1[32], const uint8_t plainText2[32], const uint8_t plainText3[32], const uint8_t plainText4[32],
                const uint8_t plainText5[32], const uint8_t plainText6[32], const uint8_t plainText7[32], const uint8_t plainText8[32],
                uint8_t cipherText1[32], uint8_t cipherText2[32], uint8_t cipherText3[32], uint8_t cipherText4[32],
                uint8_t cipherText5[32], uint8_t cipherText6[32], uint8_t cipherText7[32], uint8_t cipherText8[32])
{
	uint8x16_t ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, ml5, mr5, ml6, mr6, ml7, mr7, ml8, mr8;
	int ret = -1;

	if((ctx1 == NULL) || (ctx1->rtype != RIJNDAEL_256_256)){
		goto err;
	}
	if((ctx2 == NULL) || (ctx2->rtype != RIJNDAEL_256_256)){
		goto err;
	}
	if((ctx3 == NULL) || (ctx3->rtype != RIJNDAEL_256_256)){
		goto err;
	}
	if((ctx4 == NULL) || (ctx4->rtype != RIJNDAEL_256_256)){
		goto err;
	}
	if((ctx5 == NULL) || (ctx5->rtype != RIJNDAEL_256_256)){
		goto err;
	}
	if((ctx6 == NULL) || (ctx6->rtype != RIJNDAEL_256_256)){
		goto err;
	}
	if((ctx7 == NULL) || (ctx7->rtype != RIJNDAEL_256_256)){
		goto err;
	}
	if((ctx8 == NULL) || (ctx8->rtype != RIJNDAEL_256_256)){
		goto err;
	}

	ml1 = vld1q_u8((uint8_t *) &plainText1[0]);
	mr1 = vld1q_u8((uint8_t *) &plainText1[16]);
	ml2 = vld1q_u8((uint8_t *) &plainText2[0]);
	mr2 = vld1q_u8((uint8_t *) &plainText2[16]);
	ml3 = vld1q_u8((uint8_t *) &plainText3[0]);
	mr3 = vld1q_u8((uint8_t *) &plainText3[16]);
	ml4 = vld1q_u8((uint8_t *) &plainText4[0]);
	mr4 = vld1q_u8((uint8_t *) &plainText4[16]);
	ml5 = vld1q_u8((uint8_t *) &plainText5[0]);
	mr5 = vld1q_u8((uint8_t *) &plainText5[16]);
	ml6 = vld1q_u8((uint8_t *) &plainText6[0]);
	mr6 = vld1q_u8((uint8_t *) &plainText6[16]);
	ml7 = vld1q_u8((uint8_t *) &plainText7[0]);
	mr7 = vld1q_u8((uint8_t *) &plainText7[16]);
	ml8 = vld1q_u8((uint8_t *) &plainText8[0]);
	mr8 = vld1q_u8((uint8_t *) &plainText8[16]);

	DO_ENC_BLOCK_256_256_x8(ml1, mr1, ml2, mr2, ml3, mr3, ml4, mr4, ml5, mr5, ml6, mr6, ml7, mr7, ml8, mr8,
				ctx1->rk, ctx2->rk, ctx3->rk, ctx4->rk, ctx5->rk, ctx6->rk, ctx7->rk, ctx8->rk);

	vst1q_u8((uint8_t *) &cipherText1[0], ml1);
	vst1q_u8((uint8_t *) &cipherText1[16], mr1);
	vst1q_u8((uint8_t *) &cipherText2[0], ml2);
	vst1q_u8((uint8_t *) &cipherText2[16], mr2);
	vst1q_u8((uint8_t *) &cipherText3[0], ml3);
	vst1q_u8((uint8_t *) &cipherText3[16], mr3);
	vst1q_u8((uint8_t *) &cipherText4[0], ml4);
	vst1q_u8((uint8_t *) &cipherText4[16], mr4);
	vst1q_u8((uint8_t *) &cipherText5[0], ml5);
	vst1q_u8((uint8_t *) &cipherText5[16], mr5);
	vst1q_u8((uint8_t *) &cipherText6[0], ml6);
	vst1q_u8((uint8_t *) &cipherText6[16], mr6);
	vst1q_u8((uint8_t *) &cipherText7[0], ml7);
	vst1q_u8((uint8_t *) &cipherText7[16], mr7);
	vst1q_u8((uint8_t *) &cipherText8[0], ml8);
	vst1q_u8((uint8_t *) &cipherText8[16], mr8);

	ret = 0;
err:
	return ret;
}

#else /* !RIJNDAEL_AES_NEON */
/*
 * Dummy definition to avoid the empty translation unit ISO C warning
 */
typedef int dummy;
#endif