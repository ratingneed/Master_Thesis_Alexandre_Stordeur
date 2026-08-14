// Ported by Alexandre Stordeur

#ifndef __FIELDS_NEON_H__
#define __FIELDS_NEON_H__

/* Check for NEON support */
#ifdef __ARM_NEON

#include "fields_common.h"
#include "fields_ref.h"
/* Needed for memcpy */
#include <string.h>
/* Needed for neon assembly intrinsics */
#include <arm_neon.h>

/* === GF(2) === */
/* NOTE: for atomic multiplication, using vectorization is suboptimal */
static inline uint8_t gf2_mult_neon(uint8_t a, uint8_t b)
{
        return gf2_mult_ref(a, b);
}

// ok
#define NEON_MASK 0xFFFFFFFF

static inline uint64x2_t load_incomplete_128_aligned16(const uint8_t *a, uint32_t len)
{
        uint32x4_t res;

        if(len == 4){
                /* We only keep one element */
                const uint32x4_t mask = {NEON_MASK ,0 ,0 ,0};
                uint32x4_t data = vld1q_u32((const uint32_t*)a);
                res  = vandq_u32(data, mask);
		goto out;
        }
        if(len == 8){
                /* We only keep 2 elements */
                const uint32x4_t mask = {NEON_MASK ,NEON_MASK ,0 ,0};
                uint32x4_t data = vld1q_u32((const uint32_t*)a);
                res  = vandq_u32(data, mask);
		goto out;
        }
        /* We only keep 3 elements */
        const uint32x4_t mask = {NEON_MASK ,NEON_MASK ,NEON_MASK ,0};
        uint32x4_t data = vld1q_u32((const uint32_t*)a);
        res  = vandq_u32(data, mask);

out:
        return vreinterpretq_u64_u32(res);
}

// ok
static inline uint64x2_t load_incomplete_128_unaligned16(const uint8_t *a, uint32_t len)
{
	/* Deal with the 32-bit leftover */
	uint32x4_t res, expanded_leftover;
	uint32_t leftover;
	unsigned int i;

	/* Extract the leftover */
	leftover = 0;
	for(i = 0; i < (len % 4); i++){
		leftover |= (a[(4 * (len / 4)) + i] << (8 * i));
	}
	expanded_leftover = vdupq_n_u32(leftover);

	if(len < 4){
                const uint32x4_t mask = {NEON_MASK ,0 ,0 ,0};
                res  = vandq_u32(expanded_leftover, mask);
		goto out;
	}
	if((len >= 4) && (len < 8)){
		/* We only keep one element */
		const uint32x4_t mask = {NEON_MASK ,0 ,0 ,0};
		uint32x4_t data = vld1q_u32((const uint32_t*)a);
                res  = vandq_u32(data, mask);

		const uint32x4_t mask_2 = {0, NEON_MASK, 0, 0};
                res = vbslq_u32(mask_2, expanded_leftover, res);
		goto out;
	}
	if((len >= 8) && (len < 12)){
		/* We only keep 2 elements */
		const uint32x4_t mask = {NEON_MASK ,NEON_MASK ,0 ,0};
		uint32x4_t data = vld1q_u32((const uint32_t*)a);
                res  = vandq_u32(data, mask);

		const uint32x4_t mask_2 = {0, 0, NEON_MASK, 0};
                res = vbslq_u32(mask_2, expanded_leftover, res);
		goto out;
	}
	/* We only keep 7 elements */
	const uint32x4_t mask = {NEON_MASK ,NEON_MASK ,NEON_MASK ,0};
	uint32x4_t data = vld1q_u32((const uint32_t*)a);
        res  = vandq_u32(data, mask);
	
        const uint32x4_t mask_2 = {0, 0, 0, NEON_MASK};
        res = vbslq_u32(mask_2, expanded_leftover, res);

out:
	return vreinterpretq_u64_u32(res);
}

static inline void store_incomplete_128_aligned16(uint64x2_t in, uint8_t *a, uint32_t len)
{
        uint32_t tmp[4];
        if(len == 4){
                /* We only keep one element */
                //uint32x4_t mask = {NEON_MASK, 0, 0, 0};
                //vst1q_u32(tmp, vandq_u32(mask, vreinterpretq_u32_u64(in)));
                vst1q_u32(tmp, vreinterpretq_u32_u64(in));
                memcpy((uint32_t*)a, tmp, 4);
		goto out;
        }
        if(len == 8){
                /* We only keep 2 elements */
                //uint32x4_t mask = {NEON_MASK, NEON_MASK, 0, 0};
                //vst1q_u32(tmp, vandq_u32(mask, vreinterpretq_u32_u64(in)));
                vst1q_u32(tmp, vreinterpretq_u32_u64(in));
                memcpy(a, tmp, 8);
		goto out;
        }
        /* We only keep 3 elements */
        //uint32x4_t mask = {NEON_MASK, NEON_MASK, NEON_MASK, 0};
        //vst1q_u32(tmp, vandq_u32(mask, vreinterpretq_u32_u64(in)));
        vst1q_u32(tmp, vreinterpretq_u32_u64(in));
        memcpy(a, tmp, 12);

out:
        return;
}

static inline void store_incomplete_128_unaligned16(uint64x2_t in, uint8_t *a, uint32_t len)
{
	uint8_t local_a[16];
	vst1q_u64((uint64_t*)&local_a[0], in);
	memcpy(&a[0], &local_a[0], len);

	return;
}

/* This helper tries to efficiently copy len bytes from the ymm register */
static inline void store_incomplete_128(uint64x2_t in, uint8_t *a, uint32_t len)
{
	if(len == 16){
		vst1q_u64((uint64_t*)a, in);
	}
	else if(len % 4 == 0){
		store_incomplete_128_aligned16(in, a, len);
	}
	else{
		store_incomplete_128_unaligned16(in, a, len);
	}
	return;
}


/* This helper tries to efficiently copy len bytes in the ymm register */
static inline uint64x2_t load_incomplete_128(const uint8_t *a, uint32_t len)
{
	if(len % 4 == 0){
		return load_incomplete_128_aligned16(a, len);
	}
	else{
		return load_incomplete_128_unaligned16(a, len);
	}
}

static inline uint8_t parity_neon(uint64x2_t v) {
	uint8_t res;

	res  = __builtin_popcountll(vgetq_lane_u64(v, 0));
	res ^= __builtin_popcountll(vgetq_lane_u64(v, 1));

	return (res & 1);
}

static inline uint8_t sum_uint8_neon(uint64x2_t accu) {
#if 0
        uint32_t i;
        uint8_t res;
        uint8_t local_c[16];
        /* Store the result */
        vst1q_u8(local_c, vreinterpretq_u8_u64(accu));
        /* Finish the xor computation byte pet byte  */
        res = 0;
        for(i = 0; i < 32; i++){
                res ^= local_c[i];
        }

        return res;
#else
	uint32_t i;
	uint8_t res;
	uint64_t a = vgetq_lane_u64(accu, 0) ^ vgetq_lane_u64(accu, 1);
	res = 0;
	for(i = 0; i < 8; i++){
		res ^= (a >> (8 * i)) & 0xff;
	}
	return res;
#endif
}

static inline uint16_t sum_uint16_neon(uint64x2_t accu) {
#if 0
        uint32_t i;
        uint16_t res;
        uint8_t local_c[16]; 
        /* Store the result */
        vst1q_u8(local_c, vreinterpretq_u8_u64(accu));
        /* Finish the xor computation byte per uint16_t  */
        res = 0;
        for(i = 0; i < 16; i++){
                res ^= local_c[i];
        }

        return res;
#else
        uint32_t i;
        uint16_t res;
        uint64_t a = vgetq_lane_u64(accu, 0) ^ vgetq_lane_u64(accu, 1);
        res = 0;
        for(i = 0; i < 4; i++){
                res ^= (a >> (16 * i)) & 0xffff;
        }
        return res;
#endif
}    

/*
 * Vector multiplied by a constant in GF(2).
 */
static inline void gf2_constant_vect_mult_neon(uint8_t b, const uint8_t *a, uint8_t *c, uint32_t len)
{
	gf2_constant_vect_mult_ref(b, a, c, len);

        return;
}

// ok
static inline uint8_t gf2_vect_mult_neon(const uint8_t *a, const uint8_t *b, uint32_t len_bits)
{
	uint32_t i;
	uint64x2_t accu, _a, _b;
	uint32_t len = (len_bits / 8);

	/* Set the accumulator to 0 */
	accu = vdupq_n_u64(0);
	for(i = 0; i < len; i += 16){ // (128 bits / 8 bits = 16 bytes)
		if((len-i) < 16){
			/* Note: if we are here, we are sure that we are 32-bit aligned */
			_a = load_incomplete_128(&a[i], len-i);
			_b = load_incomplete_128(&b[i], len-i);
		}
		else{
			/* Obvious 128-bit */
			_a = vld1q_u64((const uint64_t*)&a[i]);
			_b = vld1q_u64((const uint64_t*)&b[i]);
		}
		/* Vectorized AND of inputs and then XOR with the accumulator */
		accu ^= (_a & _b);
	}

	/* Now, we have to compute the parity bit, do it 64 bits per 64 bits */
	return parity_neon(accu);
}

/* Matrix and vector multiplication over GF(2) 
 * C = A * X, where X is a vector
 * Matrix is supposed to be square n x n, and vector n x 1
 * The output is a vector n x 1
 * */
/* XXX: TODO: this can be optimized by packing rows in zmm when n <= 256 */
static inline void gf2_mat_mult_neon(const uint8_t *A, const uint8_t *X, uint8_t *Y, uint32_t n, matrix_type mtype)
{
	GF2_MAT_MULT(A, X, Y, n, mtype, gf2_vect_mult_neon);
}

/* GF(2) matrix transposition */
static inline void gf2_mat_transpose_neon(const uint8_t *A, uint8_t *B, uint32_t n, matrix_type mtype)
{
        gf2_mat_transpose_ref(A, B, n, mtype);
}

/* === GF(256) === */
/* NOTE: for atomic multiplication, using vectorization is suboptimal */
static inline uint8_t gf256_mult_neon(uint8_t x, uint8_t y)
{
	return gf256_mult_ref(x, y);
}

// ok
static inline uint64x2_t gf256_mult_vectorized_neon(uint64x2_t _a, uint64x2_t _b)
{

        uint8x16_t va = vreinterpretq_u8_u64(_a);
        uint8x16_t vb = vreinterpretq_u8_u64(_b);

	/* Fallback to the slower implementation without GFNI */
        /* Our reduction polynomial */
        const uint8x16_t red_poly = vdupq_n_u8(0x1B);
        const uint8x16_t zero = vdupq_n_u8(0);
        uint8x16_t accu = vdupq_n_u8(0);

        uint32_t j;
        uint8x16_t mask_lsb, tmp;

        /* Compute the vectorized multiplication in GF(256) */
        for(j = 0; j < 8; j++){
                mask_lsb = vreinterpretq_u8_u64(vshlq_n_u64(vreinterpretq_u64_u8(vb), 7 - j));
                
                uint8x16_t new_mask_lsb = vreinterpretq_u8_s8(vshrq_n_s8(vreinterpretq_s8_u8(mask_lsb), 7));
                accu ^= vbslq_u8(new_mask_lsb, va, zero);
                tmp = vaddq_u8(va, va);

                uint8x16_t new_va = vreinterpretq_u8_s8(vshrq_n_s8(vreinterpretq_s8_u8(va), 7));
                va = vbslq_u8(new_va, red_poly, zero) ^ tmp;
        }
        return vreinterpretq_u64_u8(accu);
}

// ok
/*
 * Vector multiplied by a constant in GF(256).
 */
static inline void gf256_constant_vect_mult_neon(uint8_t b, const uint8_t *a, uint8_t *c, uint32_t len)
{
	uint32_t i;
	uint64x2_t _a;
        uint8x16_t _b;

	/* Load the constant byte b broadcasted in _b */
	_b = vdupq_n_u8(b);

        for(i = 0; i < len; i += 16){ // (128 bits / 8 bits = 16 bytes)
                if((len-i) < 16){
                        _a = load_incomplete_128(&a[i], len-i);
                        /* Vectorized multiplication in GF(256) */
                        store_incomplete_128(gf256_mult_vectorized_neon(_a, vreinterpretq_u64_u8(_b)), &c[i], len-i);
                }
                else{
                        /* Obvious 128-bit */
                        _a = vld1q_u64((uint64_t*)&a[i]);
                        /* Vectorized multiplication in GF(256) */
                        vst1q_u64((uint64_t*)&c[i], gf256_mult_vectorized_neon(_a, vreinterpretq_u64_u8(_b)));
                }       
        }

        return;
}

// ok
/*
 * Vector to vector multiplication in GF(256).
 * Takes two vectors of length 'len', and returns a byte (element in GF(256))
 */
static inline uint8_t gf256_vect_mult_neon(const uint8_t *a, const uint8_t *b, uint32_t len)
{
	uint32_t i;
	uint64x2_t accu, _a, _b;

	/* Set the accumulator to 0 */
	accu = vdupq_n_u64(0);

	for(i = 0; i < len; i += 16){
		if((len-i) < 16){
			_a = load_incomplete_128(&a[i], len-i);
			_b = load_incomplete_128(&b[i], len-i);
		}
		else{
			/* Obvious 256-bit */
			_a = vld1q_u64((const uint64_t*)&a[i]);
			_b = vld1q_u64((const uint64_t*)&b[i]);
		}
		accu ^= gf256_mult_vectorized_neon(_a, _b); 
	}

	return sum_uint8_neon(accu);
}


/* Matrix and vector multiplication over GF(256) 
 * C += A * X, where X is a vector
 * Matrix is supposed to be square n x n, and vector n x 1
 * The output is a vector n x 1
 * */
static inline void gf256_mat_mult_neon(const uint8_t *A, const uint8_t *X, uint8_t *Y, uint32_t n, matrix_type mtype)
{
	GF256_MAT_MULT(A, X, Y, n, mtype, gf256_vect_mult_neon);
}

// ok
/*
 * "Hybrid" scalar multiplication of a vector in GF(2) and a vector in GF(256)
 */
static inline uint8_t gf2_gf256_vect_mult_neon(const uint8_t *a_gf2, const uint8_t *b_gf256, uint32_t len)
{
        uint32_t i;
	uint64x2_t _a, _b, accu;

        /* Set the accumulator to 0 */
        accu = vdupq_n_u64(0);

        for(i = 0; i < len; i += 16){
		if((len - i) < 16){
			uint32_t ceil_len = ((len - i) % 8 == 0) ? ((len - i) / 8) : (((len - i) / 8) + 1);
			_a = load_incomplete_128(&a_gf2[i / 8], ceil_len);
			_b = load_incomplete_128(&b_gf256[i], len - i);
		}
		else{
			/* Obvious 128-bit */
			_a = load_incomplete_128(&a_gf2[i / 8], 2); // 2 to have (2 bytes * 8 bits) = 16 bits 
			_b = vld1q_u64((const uint64_t*)&b_gf256[i]);
		}
		/* Create a selection mask from the bits in _a */
		const uint8x16_t shuff_msk = {0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1}; // not sure here <----------------------------------
		const uint8x16_t and_msk = {
                                                0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000, 0b00100000, 0b01000000, 0b10000000, 
                                                0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000, 0b00100000, 0b01000000, 0b10000000                                        
                                        };

		/* Only keep the selection bits */
                uint8x16_t idx_masked = vandq_u8(shuff_msk, vdupq_n_u8(0x8F));
		_a = vreinterpretq_u64_u8(vqtbl1q_u8(vreinterpretq_u8_u64(_a), idx_masked) & and_msk);
		/* Transform these bits to either 0 or 0xFF */
		_a = vreinterpretq_u64_u8(vceqq_u8(vreinterpretq_u8_u64(_a), and_msk));
		/* Bytes selection */
                accu ^= (_a & _b);
        }
        return sum_uint8_neon(accu);
}

/*
 * "Hybrid" multiplication of a constant in GF(2) and a vector in GF(256)
 */
static inline void gf2_gf256_constant_vect_mult_neon(uint8_t a_gf2, const uint8_t *b_gf256, uint8_t *c_gf256, uint32_t n)
{
	gf2_gf256_constant_vect_mult_ref(a_gf2, b_gf256, c_gf256, n);

        return;
}

// ok
/*
 * "Hybrid" multiplication of a constant in GF(256) and a vector in GF(2)
 */
static inline void gf256_gf2_constant_vect_mult_neon(uint8_t a_gf256, const uint8_t *b_gf2, uint8_t *c_gf256, uint32_t len)
{
	uint32_t i;
	uint64x2_t _a, _b;

        /* Broadcast the constant value */
	_a = vreinterpretq_u64_u8(vdupq_n_u8(a_gf256));

        for(i = 0; i < len; i += 16){
		uint32_t ceil_len;
		if((len - i) < 16){
			ceil_len = ((len - i) % 8 == 0) ? ((len - i) / 8) : (((len - i) / 8) + 1);
			_b = load_incomplete_128(&b_gf2[i / 8], ceil_len);
		}
		else{
			ceil_len = 2;
			/* Obvious 128-bit */
			_b = load_incomplete_128(&b_gf2[i / 8], 2);
		}
		/* Create a selection mask from the bits in _a */
		const uint8x16_t shuff_msk = {  0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1 }; // not sure hre <-----------------------------------------------------------------
		const uint8x16_t and_msk = {
                                                0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000, 0b00100000, 0b01000000, 0b10000000, 
                                                0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000, 0b00100000, 0b01000000, 0b10000000                                        
                                        };

		/* Only keep the selection bits */
		uint8x16_t idx_masked = vandq_u8(shuff_msk, vdupq_n_u8(0x8F));
		_b = vreinterpretq_u64_u8(vqtbl1q_u8(vreinterpretq_u8_u64(_b), idx_masked) & and_msk);
		/* Transform these bits to either 0 or 0xFF */
		_b = vreinterpretq_u64_u8(vceqq_u8(vreinterpretq_u8_u64(_b), and_msk));
		/* Bytes selection */
                uint64x2_t _c = (_a & _b);
		/* Store the result */
		store_incomplete_128(_c, &c_gf256[i], 8 * ceil_len);
        }

        return;
}

/*
 * "Hybrid" scalar multiplication of a vector in GF(256) and a vector in GF(2)
 */
static inline uint8_t gf256_gf2_vect_mult_neon(const uint8_t *a_gf256, const uint8_t *b_gf2, uint32_t n)
{
        return gf2_gf256_vect_mult_neon(b_gf2, a_gf256, n);
}

/* 
 * "Hybrid" matrix multiplication of a matrix in GF(2) and a vector in GF(256), resulting
 *  in a vector in GF(256)
 */                     
static inline void gf2_gf256_mat_mult_neon(const uint8_t *A, const uint8_t *X, uint8_t *Y, uint32_t n, matrix_type mtype)
{
        /* NOTE: XXX: we force a REG here as it allows for better performance */
        (void)mtype;
        GF2_GF256_MAT_MULT(A, X, Y, n, REG, gf2_gf256_vect_mult_neon);
}

/*
 * "Hybrid" matrix multiplication of a matrix in GF(256) and a vector in GF(2), resulting
 *  in a vector in GF(256)
 */
static inline void gf256_gf2_mat_mult_neon(const uint8_t *A, const uint8_t *X, uint8_t *Y, uint32_t n, matrix_type mtype)
{
        /* NOTE: XXX: we force a REG here as it allows for better performance */
        (void)mtype;
        GF256_GF2_MAT_MULT(A, X, Y, n, REG, gf256_gf2_vect_mult_neon);
}

/* GF(256) matrix transposition */
static inline void gf256_mat_transpose_neon(const uint8_t *A, uint8_t *B, uint32_t n, matrix_type mtype)
{
        gf256_mat_transpose_ref(A, B, n, mtype);
}

/*
 * "Hybrid" multiplication of a constant in GF(4) and a vector in GF(256)
 */
static inline void gf4_gf256_constant_vect_mult_neon(uint8_t a_gf4, const uint8_t *b_gf256, uint8_t *c_gf256, uint32_t n)
{
    gf4_gf256_constant_vect_mult_ref(a_gf4, b_gf256, c_gf256, n);
    return;
}

/*
 * "Hybrid" multiplication of a constant in GF(256) and a vector in GF(4)
 */
static inline void gf256_gf4_constant_vect_mult_neon(uint8_t a_gf256, const uint8_t *b_gf4, uint8_t *c_gf256, uint32_t n)
{
    gf256_gf4_constant_vect_mult_ref(a_gf256, b_gf4, c_gf256, n);
    return;
}

/*
 * "Hybrid" scalar multiplication of a vector in GF(4) and a vector in GF(256)
 */
static inline uint8_t gf4_gf256_vect_mult_neon(const uint8_t *a_gf4, const uint8_t *b_gf256, uint32_t n)
{
    return gf4_gf256_vect_mult_ref(a_gf4, b_gf256, n);
}

/*
 * "Hybrid" scalar multiplication of a vector in GF(256) and a vector in GF(4)
 */
static inline uint8_t gf256_gf4_vect_mult_neon(const uint8_t *a_gf256, const uint8_t *b_gf4, uint32_t n)
{
	return gf4_gf256_vect_mult_neon(b_gf4, a_gf256, n);
}

/*
 * "Hybrid" matrix multiplication of a matrix in GF(256) and a vector in GF(4), resulting
 *  in a vector in GF(256)
 */
static inline void gf256_gf4_mat_mult_neon(const uint8_t *A, const uint8_t *X, uint8_t *Y, uint32_t n, matrix_type mtype)
{
    GF256_GF4_MAT_MULT(A, X, Y, n, mtype, gf256_gf4_vect_mult_neon);
}

/*
 * "Hybrid" multiplication of a constant in GF(16) and a vector in GF(256)
 */
static inline void gf16_gf256_constant_vect_mult_neon(uint8_t a_gf16, const uint8_t *b_gf256, uint8_t *c_gf256, uint32_t n)
{
    uint8_t a_gf256;
    gf256_vect_lift_from_gf16_ref(&a_gf16, &a_gf256, 1);
    gf256_constant_vect_mult_neon(a_gf256, b_gf256, c_gf256, n);
    return;
}

// ok
/* Vectorized lifting from GF(16) to GF(256) */
static inline void gf256_vect_lift_from_gf16_neon(const uint8_t *b_gf16, uint8_t *c_gf256, uint32_t len)
{
        uint32_t i;
        uint64x2_t _a_gf16, _a, _b, _c;
	const uint8x16_t shuff_msk_even = {0, -1, 1, -1, 2, -1, 3, -1, 4, -1, 5, -1, 6, -1, 7, -1};
	const uint8x16_t shuff_msk_odd  = {-1, 0, -1, 1, -1, 2, -1, 3, -1, 4, -1, 5, -1, 6, -1, 7};
	const uint8x16_t lifting_lookup = {0x00, 0x01, 0xe0, 0xe1, 0x5d, 0x5c, 0xbd, 0xbc, 0xb0, 0xb1, 0x50, 0x51, 0xed, 0xec, 0x0d, 0x0c};
	const uint64x2_t nib_mask = vreinterpretq_u64_u8(vdupq_n_u8(0x0f));

	for(i = 0; i < len; i += 16){
                if((len-i) < 16){
                        _a_gf16 = load_incomplete_128((const uint8_t*)&b_gf16[i/2], (len-i+1)/2);
                }
                else{
                        /* Obvious 128-bit */
                        _a_gf16 = load_incomplete_128((const uint8_t*)&b_gf16[i/2], 8); // 8 times 16 bits 
                }

		/* Isolate the nibbles in _a_gf16 */
		_a = _a_gf16 & nib_mask;
		_b = vshrq_n_u64(_a_gf16, 4) & nib_mask;
		/* Create the nibbles mix */
                uint8x16_t idx_masked_even = vandq_u8(shuff_msk_even, vdupq_n_u8(0x8F)); 
                uint8x16_t idx_masked_odd = vandq_u8(shuff_msk_odd, vdupq_n_u8(0x8F)); 
		_c = vreinterpretq_u64_u8(vqtbl1q_u8(vreinterpretq_u8_u64(_a), idx_masked_even) | vqtbl1q_u8(vreinterpretq_u8_u64(_b), idx_masked_odd));
		/* Lift: since we are on 16 bits, we can perform a vperm lookup inside the register */
		uint8x16_t idx__c = vandq_u8(vreinterpretq_u8_u64(_c), vdupq_n_u8(0x8F)); 
                _c = vreinterpretq_u64_u8(vqtbl1q_u8(lifting_lookup, idx__c));
		/* Store the result */
		if((len-i) < 16){
			store_incomplete_128(_c, &c_gf256[i], (len-i));
		}
		else{
                        vst1q_u64((uint64_t*)&c_gf256[i], _c);
		}
	}

	return;
}

/*
 * "Hybrid" multiplication of a constant in GF(256) and a vector in GF(16)
 */
static inline void gf256_gf16_constant_vect_mult_neon(uint8_t a_gf256, const uint8_t *b_gf16, uint8_t *c_gf256, uint32_t n)
{
    gf256_vect_lift_from_gf16_neon(b_gf16, c_gf256, n);
    gf256_constant_vect_mult_neon(a_gf256, c_gf256, c_gf256, n);
    return;
}

// ok
/*
 * "Hybrid" scalar multiplication of a vector in GF(16) and a vector in GF(256)
 */
static inline uint8_t gf16_gf256_vect_mult_neon(const uint8_t *a_gf16, const uint8_t *b_gf256, uint32_t len)
{
        uint32_t i;
        uint64x2_t accu, _a, _b;
        uint64x2_t _a_gf16;

        /* Set the accumulator to 0 */
        accu = vdupq_n_u64(0);

        for(i = 0; i < len; i += 16){
                if((len-i) < 16){
                        _a_gf16 = load_incomplete_128((const uint8_t*)&a_gf16[i/2], (len-i+1)/2);
			_a = vdupq_n_u64(0);
                        gf256_vect_lift_from_gf16_neon((const uint8_t*)&_a_gf16, (uint8_t*)&_a, len-i);
                        _b = load_incomplete_128((const uint8_t*)&b_gf256[i], len-i);
                }
                else{
                        /* Obvious 256-bit */
                        _a_gf16 = load_incomplete_128((const uint8_t*)&a_gf16[i/2], 8); // 8 times 16 bits
                        gf256_vect_lift_from_gf16_neon((const uint8_t*)&_a_gf16, (uint8_t*)&_a, 16);
                        _b = vld1q_u64((uint64_t*)&b_gf256[i]);
                }
                /* Multiply in GF(256) */
                accu ^= gf256_mult_vectorized_neon(_a, _b);
        }

        return sum_uint8_neon(accu);
}

/*
 * "Hybrid" scalar multiplication of a vector in GF(256) and a vector in GF(16)
 */
static inline uint8_t gf256_gf16_vect_mult_neon(const uint8_t *a_gf256, const uint8_t *b_gf16, uint32_t n)
{
    return gf16_gf256_vect_mult_neon(b_gf16, a_gf256, n);
}

/*
 * "Hybrid" matrix multiplication of a matrix in GF(256) and a vector in GF(16), resulting
 *  in a vector in GF(256)
 */
static inline void gf256_gf16_mat_mult_neon(const uint8_t *A, const uint8_t *X, uint8_t *Y, uint32_t n, matrix_type mtype)
{
    GF256_GF16_MAT_MULT(A, X, Y, n, mtype, gf256_gf16_vect_mult_neon);
}


/* === GF(256^2) === */
/* NOTE: for atomic multiplication, using vectorization is suboptimal */
static inline uint16_t gf256to2_mult_neon(uint16_t x, uint16_t y)
{
	return gf256to2_mult_ref(x, y);
}

// ok
/* Vectorize multiplication of _a and _b in GF(256^2): the elements in the field are made of
 * 16 bits each in the lanes of the ymm */
static inline uint64x2_t gf256to2_mult_vectorized_neon(uint64x2_t _a, uint64x2_t _b)
{
        const uint8x16_t shuff_msk_crossed = {1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14};
        const uint8x16_t shuff_msk1 = {0, 0, 2, 2, 4, 4, 6, 6, 8, 8, 10, 10, 12, 12, 14, 14};
        const uint8x16_t shuff_msk2 = {1, 1, 3, 3, 5, 5, 7, 7, 9, 9, 11, 11, 13, 13, 15, 15};

        const uint64x2_t const32 = vdupq_n_u64(0x0020002000200020);

	const uint64x2_t mask_c1 = vdupq_n_u64(0x8000800080008000);
        const uint64x2_t zero = vdupq_n_u64(0);

	uint64x2_t ab = gf256_mult_vectorized_neon(_a, _b);

        uint8x16_t idx_shuff_msk1 = vandq_u8(shuff_msk1, vdupq_n_u8(0x8F));
	uint64x2_t a0b0 = vreinterpretq_u64_u8(vqtbl1q_u8(vreinterpretq_u8_u64(ab), idx_shuff_msk1));

        uint8x16_t idx_shuff_msk2 = vandq_u8(shuff_msk2, vdupq_n_u8(0x8F));
	uint64x2_t a1b1 = vreinterpretq_u64_u8(vqtbl1q_u8(vreinterpretq_u8_u64(ab), idx_shuff_msk2));

	uint64x2_t a1b1_32 = gf256_mult_vectorized_neon(a1b1, const32);
	/* */
        uint8x16_t idx_shuff_msk_crossed = vandq_u8(shuff_msk_crossed, vdupq_n_u8(0x8F));

	uint64x2_t a0_xor_a1 = _a ^ vreinterpretq_u64_u8(vqtbl1q_u8(vreinterpretq_u8_u64(_a), idx_shuff_msk_crossed));

        uint8x16_t new_mask_c1 = vreinterpretq_u8_s8(vshrq_n_s8(vreinterpretq_s8_u64(mask_c1), 7));
	uint64x2_t b0_xor_b1 = vreinterpretq_u64_u8(vbslq_u8(
                new_mask_c1, 
                vreinterpretq_u8_u64(_b ^ vreinterpretq_u64_u8(vqtbl1q_u8(vreinterpretq_u8_u64(_b), idx_shuff_msk_crossed))), 
                vreinterpretq_u8_u64(zero)));
	uint64x2_t mult_ab_xor = gf256_mult_vectorized_neon(a0_xor_a1, b0_xor_b1);
	
	/* Compute the result */
	uint64x2_t res = a0b0 ^ a1b1_32 ^ mult_ab_xor;

        return res;
}

// ok
/*
 * Vector multiplied by a constant in GF(256^2).
 */
static inline void gf256to2_constant_vect_mult_neon(uint16_t b, const uint16_t *a, uint16_t *c, uint32_t len)
{
	uint32_t i;
	uint64x2_t _a, _b;

	/* Load the constant byte b broadcasted in _b */
	_b = vreinterpretq_u64_u16(vdupq_n_u16(b));

        for(i = 0; i < (2 * len); i += 16){
                if(((2 * len)-i) < 16){
                        _a = load_incomplete_128((const uint8_t*)&a[i / 2], ((2 * len) - i));
                        /* Vectorized multiplication in GF(256) */
                        store_incomplete_128(gf256to2_mult_vectorized_neon(_a, _b), (uint8_t*)&c[i / 2], (2 * len)-i);
                }
                else{
                        /* Obvious 128-bit */
                        _a = vld1q_u64((const uint64_t*)&a[i / 2]);
                        /* Vectorized multiplication in GF(256) */
                        vst1q_u64((uint64_t*)&c[i / 2], gf256to2_mult_vectorized_neon(_a, _b));
                }       
        }

        return;
}

// ok
/* Perform a multiplication in GF(256^2) of elements in vectors a an b */
static inline uint16_t gf256to2_vect_mult_neon(const uint16_t *a, const uint16_t *b, uint32_t len)
{
        uint32_t i;
        uint64x2_t accu, _a, _b;

        /* Set the accumulator to 0 */
        accu = vdupq_n_u64(0);

        for(i = 0; i < (2 * len); i += 16){
                if(((2 * len)-i) < 16){
                        _a = load_incomplete_128((const uint8_t*)&a[i / 2], ((2 * len) - i));
                        _b = load_incomplete_128((const uint8_t*)&b[i / 2], ((2 * len) - i));
                }
                else{
                        /* Obvious 128-bit */
                        _a = vld1q_u64((const uint64_t*)&a[i / 2]);
                        _b = vld1q_u64((const uint64_t*)&b[i / 2]);
                }
                accu ^= gf256to2_mult_vectorized_neon(_a, _b);
        }

        return sum_uint16_neon(accu);
}

/*
 * GF(2^16) matrix multiplication
 */
static inline void gf256to2_mat_mult_neon(const uint16_t *A, const uint16_t *X, uint16_t *Y, uint32_t n, matrix_type mtype)
{
        GF256to2_MAT_MULT(A, X, Y, n, mtype, gf256to2_vect_mult_neon);
}

/*
 * "Hybrid" constant multiplication of a constant in GF(2) and a vector in GF(256^2)
 */
static inline void gf2_gf256to2_constant_vect_mult_neon(uint8_t a_gf2, const uint16_t *b_gf256to2, uint16_t *c_gf256to2, uint32_t n)
{
	gf2_gf256to2_constant_vect_mult_ref(a_gf2, b_gf256to2, c_gf256to2, n);

        return;
}

// ok
/*
 * "Hybrid" constant multiplication of a constant in GF(256^2) and a vector in GF(2)
 */
static inline void gf256to2_gf2_constant_vect_mult_neon(uint16_t a_gf256to2, const uint8_t *b_gf2, uint16_t *c_gf256to2, uint32_t len)
{
        uint32_t i;
	uint64x2_t _a, _b;

        /* Broadcast the constant value */
	_a = vreinterpretq_u64_u16(vdupq_n_u16(a_gf256to2));

        for(i = 0; i < (2*len); i += 16){
		uint32_t ceil_len;
		if(((2*len) - i) < 16){
			ceil_len = (((2*len) - i) % 16 == 0) ? (((2*len) - i) / 16) : ((((2*len) - i) / 16) + 1);
			_b = load_incomplete_128(&b_gf2[i / 16], ceil_len);
		}
		else{
			/* Obvious 128-bit */
			ceil_len = 1;
			_b = load_incomplete_128(&b_gf2[i / 16], 2);
		}
		/* Create a selection mask from the bits in _a */
		//const uint8x16_t shuff_msk = {0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1}; // verify this one look tricky <----------------------------------------------
                const uint8x16_t shuff_msk = vdupq_n_u8(0); // verify this one look tricky <----------------------------------------------
		const uint8x16_t and_msk = {
                                                0b00000001, 0b00000001, 0b00000010, 0b00000010, 0b00000100, 0b00000100, 0b00001000, 0b00001000, 
                                                0b00010000, 0b00010000, 0b00100000, 0b00100000, 0b01000000, 0b01000000, 0b10000000, 0b10000000
                                        };

		/* Only keep the selection bits */
                uint8x16_t idx_shuff_msk = vandq_u8(shuff_msk, vdupq_n_u8(0x8F)); 
		_b = vreinterpretq_u64_u8(vqtbl1q_u8(vreinterpretq_u8_u64(_b), idx_shuff_msk));
		_b = _b & vreinterpretq_u64_u8(and_msk);
		/* Transform these bits to either 0 or 0xFF */
		_b = vreinterpretq_u64_u8(vceqq_u8(vreinterpretq_u8_u64(_b), and_msk));
		/* Bytes selection */
                uint64x2_t _c = (_a & _b);
		/* Store the result */
		store_incomplete_128(_c, (uint8_t*)&c_gf256to2[i / 2], 16 * ceil_len);
        }
        return;
}

/*
 * "Hybrid" constant multiplication of a constant in GF(256^2) and a vector in GF(256)
 */
static inline void gf256_gf256to2_constant_vect_mult_neon(uint8_t a_gf256, const uint16_t *b_gf256to2, uint16_t *c_gf256to2, uint32_t n)
{
	gf256_gf256to2_constant_vect_mult_ref(a_gf256, b_gf256to2, c_gf256to2, n);

	return;
}

// ok
/*
 * "Hybrid" constant multiplication of a constant in GF(256^2) and a vector in GF(256)
 */
static inline void gf256to2_gf256_constant_vect_mult_neon(uint16_t a_gf256to2, const uint8_t *b_gf256, uint16_t *c_gf256to2, uint32_t len)
{
	uint32_t i;
	uint64x2_t _a, _b;
        const uint8x16_t shuff_msk = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7};

	/* Load the constant byte b broadcasted in _b */
	_a = vreinterpretq_u64_u16(vdupq_n_u16(a_gf256to2));

        for(i = 0; i < len; i += 8){
		uint32_t to_load = (len - i) < 8 ? (len - i) : 8;
                _b = load_incomplete_128((const uint8_t*)&b_gf256[i], to_load);
                /* Duplicate elements in the register */
                uint8x16_t idx_shuff_msk = vandq_u8(shuff_msk, vdupq_n_u8(0x8F)); 
                _b = vreinterpretq_u64_u8(vqtbl1q_u8(vreinterpretq_u8_u64(_b), idx_shuff_msk));
                /* Vectorized multiplication in GF(256) */
                store_incomplete_128(gf256_mult_vectorized_neon(_a, _b), (uint8_t*)&c_gf256to2[i], 2 * to_load);
        }

        return;
}

// ok
/*
 * "Hybrid" scalar multiplication of a vector in GF(2) and a vector in GF(256^2)
 */
static inline uint16_t gf2_gf256to2_vect_mult_neon(const uint8_t *a_gf2, const uint16_t *b_gf256to2, uint32_t len)
{
        uint32_t i;
	uint64x2_t _a, _b;

        /* Set the accumulator to 0 */
        uint64x2_t accu = vdupq_n_u64(0);

        for(i = 0; i < (2*len); i += 16){
		if(((2*len) - i) < 16){
			uint32_t ceil_len = (((2*len) - i) % 16 == 0) ? (((2*len) - i) / 16) : ((((2*len) - i) / 16) + 1);
			_a = load_incomplete_128(&a_gf2[i / 16], ceil_len);
			_b = load_incomplete_128((const uint8_t*)&b_gf256to2[i / 2], (2*len) - i);
		}
		else{
			/* Obvious 128-bit */
			_a = load_incomplete_128(&a_gf2[i / 16], 2);
			_b = vld1q_u64((const uint64_t*)&b_gf256to2[i / 2]);
		}
		/* Create a selection mask from the bits in _a */
		//const uint8x16_t shuff_msk = {0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1}; // verify this one look tricky <----------------------------------------------
                const uint8x16_t shuff_msk = vdupq_n_u8(0); // verify this one look tricky <----------------------------------------------
		const uint8x16_t and_msk = {
						0b00000001, 0b00000001, 0b00000010, 0b00000010, 0b00000100, 0b00000100, 0b00001000, 0b00001000, 
                                                0b00010000, 0b00010000, 0b00100000, 0b00100000, 0b01000000, 0b01000000, 0b10000000, 0b10000000
                                        };

		/* Only keep the selection bits */
                uint8x16_t idx_shuff_msk = vandq_u8(shuff_msk, vdupq_n_u8(0x8F)); 
		_a = vreinterpretq_u64_u8(vqtbl1q_u8(vreinterpretq_u8_u64(_a), idx_shuff_msk) & and_msk);
		/* Transform these bits to either 0 or 0xFF */
		_a = vreinterpretq_u64_u8(vceqq_u8(vreinterpretq_u8_u64(_a), and_msk));
		/* Bytes selection */
                accu ^= (_a & _b);
        }
        return sum_uint16_neon(accu);
}

/*
 * "Hybrid" scalar multiplication of a vector in GF(256^2) and a vector in GF(256)
 */
static inline uint16_t gf256to2_gf2_vect_mult_neon(const uint16_t *a_gf256to2, const uint8_t *b_gf2, uint32_t n)
{
        return gf2_gf256to2_vect_mult_neon(b_gf2, a_gf256to2, n);
}

// ok
/*
 * "Hybrid" scalar multiplication of a vector in GF(256) and a vector in GF(256^2)
 */
static inline uint16_t gf256_gf256to2_vect_mult_neon(const uint8_t *a_gf256, const uint16_t *b_gf256to2, uint32_t len)
{
        /* Note: the multiplication of an element in GF(256) and an element in GF(256^2)
         * simply consists in two multiplications in GF(256) (this is multiplying a constant by a degree 1 polynomial)
         * */
        uint32_t i;
        uint64x2_t accu, _a, _b;

        /* Set the accumulator to 0 */
        accu = vdupq_n_u64(0);

        for(i = 0; i < (2 * len); i += 16){
                const uint8x16_t shuff_msk = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7};
                if(((2 * len)-i) < 16){
                        _a = load_incomplete_128((const uint8_t*)&a_gf256[i / 2], ((2 * len)-i) / 2);
                        _b = load_incomplete_128((const uint8_t*)&b_gf256to2[i / 2], (2 * len)-i);
                }
                else{
                        /* Obvious 128-bit */
                        _a = load_incomplete_128((const uint8_t*)&a_gf256[i / 2], 8);
                        _b = vld1q_u64((const uint64_t*)&b_gf256to2[i / 2]);
                }

                /* Only keep the selection bits */
                uint8x16_t idx_shuff_msk = vandq_u8(shuff_msk, vdupq_n_u8(0x8F)); 
                _a = vreinterpretq_u64_u8(vqtbl1q_u8(vreinterpretq_u8_u64(_a), idx_shuff_msk));
                /* Multiply in GF(256) */
                accu ^= gf256_mult_vectorized_neon(_a, _b);
        }

        return sum_uint16_neon(accu);
}

/*
 * "Hybrid" scalar multiplication of a vector in GF(256^2) and a vector in GF(256)
 */
static inline uint16_t gf256to2_gf256_vect_mult_neon(const uint16_t *a_gf256to2, const uint8_t *b_gf256, uint32_t n)
{
        return gf256_gf256to2_vect_mult_neon(b_gf256, a_gf256to2, n);
}

/*
 * "Hybrid" matrix multiplication of a matrix in GF(2) and a vector in GF(256^2), resulting
 *  in a vector in GF(256^2)
 */
static inline void gf2_gf256to2_mat_mult_neon(const uint8_t *A, const uint16_t *X, uint16_t *Y, uint32_t n, matrix_type mtype)
{
        GF2_GF256to2_MAT_MULT(A, X, Y, n, mtype, gf2_gf256to2_vect_mult_neon);
}

/*
 * "Hybrid" matrix multiplication of a matrix in GF(256^2) and a vector in GF(2), resulting
 *  in a vector in GF(256^2)
 */
static inline void gf256to2_gf2_mat_mult_neon(const uint16_t *A, const uint8_t *X, uint16_t *Y, uint32_t n, matrix_type mtype)
{
        GF256to2_GF2_MAT_MULT(A, X, Y, n, mtype, gf256to2_gf2_vect_mult_neon);
}

/*
 * "Hybrid" matrix multiplication of a matrix in GF(256) and a vector in GF(256^2), resulting
 *  in a vector in GF(256^2)
 */
static inline void gf256_gf256to2_mat_mult_neon(const uint8_t *A, const uint16_t *X, uint16_t *Y, uint32_t n, matrix_type mtype)
{
        GF256to2_MAT_MULT(A, X, Y, n, mtype, gf256_gf256to2_vect_mult_neon);
}

/*
 * "Hybrid" matrix multiplication of a matrix in GF(256^2) and a vector in GF(256), resulting
 *  in a vector in GF(256^2)
 */
static inline void gf256to2_gf256_mat_mult_neon(const uint16_t *A, const uint8_t *X, uint16_t *Y, uint32_t n, matrix_type mtype)
{
        GF256to2_MAT_MULT(A, X, Y, n, mtype, gf256to2_gf256_vect_mult_neon);
}

/* GF(256^2) matrix transposition */
static inline void gf256to2_mat_transpose_neon(const uint16_t *A, uint16_t *B, uint32_t n, matrix_type mtype)
{
        gf256to2_mat_transpose_ref(A, B, n, mtype);
}

/*
 * "Hybrid" constant multiplication of a constant in GF(4) and a vector in GF(256^2)
 */
static inline void gf4_gf256to2_constant_vect_mult_neon(uint8_t a_gf4, const uint16_t *b_gf256to2, uint16_t *c_gf256to2, uint32_t n)
{
    gf4_gf256to2_constant_vect_mult_ref(a_gf4, b_gf256to2, c_gf256to2, n);
    return;
}

/*
 * "Hybrid" constant multiplication of a constant in GF(256^2) and a vector in GF(4)
 */
static inline void gf256to2_gf4_constant_vect_mult_neon(uint16_t a_gf256to2, const uint8_t *b_gf4, uint16_t *c_gf256to2, uint32_t n)
{
    gf256to2_gf4_constant_vect_mult_ref(a_gf256to2, b_gf4, c_gf256to2, n);
    return;
}

/*
 * "Hybrid" scalar multiplication of a vector in GF(4) and a vector in GF(256^2)
 */
static inline uint16_t gf4_gf256to2_vect_mult_neon(const uint8_t *a_gf4, const uint16_t *b_gf256to2, uint32_t n)
{
    return gf4_gf256to2_vect_mult_ref(a_gf4, b_gf256to2, n);
}

/*
 * "Hybrid" scalar multiplication of a vector in GF(256^2) and a vector in GF(4)
 */
static inline uint16_t gf256to2_gf4_vect_mult_neon(const uint16_t *a_gf256to2, const uint8_t *b_gf4, uint32_t n)
{
    return gf4_gf256to2_vect_mult_neon(b_gf4, a_gf256to2, n);
}

/*
 * "Hybrid" matrix multiplication of a matrix in GF(256^2) and a vector in GF(4), resulting
 *  in a vector in GF(256^2)
 */
static inline void gf256to2_gf4_mat_mult_neon(const uint16_t *A, const uint8_t *X, uint16_t *Y, uint32_t n, matrix_type mtype)
{
    GF256to2_GF4_MAT_MULT(A, X, Y, n, mtype, gf256to2_gf4_vect_mult_neon);
}

/*
 * "Hybrid" constant multiplication of a constant in GF(16) and a vector in GF(256^2)
 */
static inline void gf16_gf256to2_constant_vect_mult_neon(uint8_t a_gf16, const uint16_t *b_gf256to2, uint16_t *c_gf256to2, uint32_t n)
{
    uint8_t a_gf256;
    gf256_vect_lift_from_gf16_ref(&a_gf16, &a_gf256, 1);
    gf256_gf256to2_constant_vect_mult_neon(a_gf256, b_gf256to2, c_gf256to2, n);
    return;
}

/*
 * "Hybrid" constant multiplication of a constant in GF(256^2) and a vector in GF(16)
 */
static inline void gf256to2_gf16_constant_vect_mult_neon(uint16_t a_gf256to2, const uint8_t *b_gf16, uint16_t *c_gf256to2, uint32_t n)
{
    uint8_t* buf = ((uint8_t*) c_gf256to2) + n;
    gf256_vect_lift_from_gf16_neon(b_gf16, buf, n);
    gf256to2_gf256_constant_vect_mult_neon(a_gf256to2, buf, c_gf256to2, n);
    return;
}

// TODO port neon current
/*
 * "Hybrid" scalar multiplication of a vector in GF(16) and a vector in GF(256^2)
 */
static inline uint16_t gf16_gf256to2_vect_mult_neon(const uint8_t *a_gf16, const uint16_t *b_gf256to2, uint32_t len)
{
        uint32_t i;
        uint64x2_t accu, _b;
        uint64x2_t _a = vdupq_n_u64(0);
        uint64x2_t _a_gf16;

        /* Set the accumulator to 0 */
        accu = vdupq_n_u64(0);

        for(i = 0; i < (2 * len); i += 16){
                const uint8x16_t shuff_msk = {0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7};
                if(((2 * len)-i) < 16){
                        _a_gf16 = load_incomplete_128((const uint8_t*)&a_gf16[i / 4], (((2 * len)-i) + 3)/ 4);
                        _a = vdupq_n_u64(0);
                        gf256_vect_lift_from_gf16_neon((const uint8_t*)&_a_gf16, (uint8_t*)&_a, ((2 * len)-i+1)/2);
                        _b = load_incomplete_128((const uint8_t*)&b_gf256to2[i / 2], (2 * len)-i);
                }
                else{
                        /* Obvious 256-bit */
                        _a_gf16 = load_incomplete_128((const uint8_t*)&a_gf16[i / 4], 4);
                        gf256_vect_lift_from_gf16_neon((const uint8_t*)&_a_gf16, (uint8_t*)&_a, 8);
                        _b = vld1q_u64((const uint64_t*)&b_gf256to2[i / 2]);
                }

                /* Only keep the selection bits */
                uint8x16_t idx_shuff_msk = vandq_u8(shuff_msk, vdupq_n_u8(0x8F)); 
                _a = vreinterpretq_u64_u8(vqtbl1q_u8(vreinterpretq_u8_u64(_a), idx_shuff_msk));
                /* Multiply in GF(256) */
                accu ^= gf256_mult_vectorized_neon(_a, _b);
        }

        return sum_uint16_neon(accu);
}

/*
 * "Hybrid" scalar multiplication of a vector in GF(256^2) and a vector in GF(16)
 */
static inline uint16_t gf256to2_gf16_vect_mult_neon(const uint16_t *a_gf256to2, const uint8_t *b_gf16, uint32_t n)
{
    return gf16_gf256to2_vect_mult_neon(b_gf16, a_gf256to2, n);
}

/*
 * "Hybrid" matrix multiplication of a matrix in GF(256^2) and a vector in GF(16), resulting
 *  in a vector in GF(256^2)
 */
static inline void gf256to2_gf16_mat_mult_neon(const uint16_t *A, const uint8_t *X, uint16_t *Y, uint32_t n, matrix_type mtype)
{
    GF256to2_GF16_MAT_MULT(A, X, Y, n, mtype, gf256to2_gf16_vect_mult_neon);
}

#endif /* __ARM_NEON */

#endif /* __FIELDS_NEON_H__ */
