/*
 * NeonCROSS: Vectorized Implementation of the Post-Quantum Signature Algorithm CROSS
 * Copyright (c) 2025 Hanyu Wei et al.
 * Licensed under the Apache License, Version 2.0; see LICENSE for details.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stdalign.h>
#if defined(CYCLE_MAIN) || defined(CYCLE_MAIN_LOOP) || defined(CYCLE_MAIN_CONDITION)
#include <stdio.h>
#endif

#include "CROSS.h"
#include "architecture_detect.h"
#include "csprng_hash.h"
#include "merkle_tree.h"
#include "pack_unpack.h"
#include "seedtree.h"
#include "fp_arith.h"

#if defined(RSDP)
static
void expand_pk(FP_ELEM V_tr[K][N-K],
               const uint8_t seed_pk[KEYPAIR_SEED_LENGTH_BYTES]){

  /* Expansion of pk->seed, explicit domain separation for CSPRNG as in keygen */
  const uint16_t dsc_csprng_seed_pk = CSPRNG_DOMAIN_SEP_CONST + (3*T+2);

  CSPRNG_STATE_T csprng_state_mat;
  csprng_initialize(&csprng_state_mat, seed_pk, KEYPAIR_SEED_LENGTH_BYTES, dsc_csprng_seed_pk);
  csprng_fp_mat(V_tr,&csprng_state_mat);
}
#elif defined(RSDPG)
static
void expand_pk(FP_ELEM V_tr[K][N-K],
               FZ_ELEM W_mat[M][N-M],
               const uint8_t seed_pk[KEYPAIR_SEED_LENGTH_BYTES]){

  /* Expansion of pk->seed, explicit domain separation for CSPRNG as in keygen */
  const uint16_t dsc_csprng_seed_pk = CSPRNG_DOMAIN_SEP_CONST + (3*T+2);

  CSPRNG_STATE_T csprng_state_mat;
  csprng_initialize(&csprng_state_mat, seed_pk, KEYPAIR_SEED_LENGTH_BYTES, dsc_csprng_seed_pk);

  csprng_fz_mat(W_mat,&csprng_state_mat);
  csprng_fp_mat(V_tr,&csprng_state_mat);
}
#endif


#if defined(RSDP)
static
void expand_sk(FZ_ELEM e_bar[N],
               FP_ELEM V_tr[K][N-K],
               const uint8_t seed_sk[KEYPAIR_SEED_LENGTH_BYTES])
{

  uint8_t seed_e_seed_pk[2][KEYPAIR_SEED_LENGTH_BYTES];

  /* Expansion of sk->seed, explicit domain separation for CSPRNG, as in keygen */
  const uint16_t dsc_csprng_seed_sk = CSPRNG_DOMAIN_SEP_CONST + (3*T+1);

  CSPRNG_STATE_T csprng_state;
  csprng_initialize(&csprng_state, seed_sk, KEYPAIR_SEED_LENGTH_BYTES, dsc_csprng_seed_sk);
  csprng_randombytes((uint8_t *)seed_e_seed_pk,
                     2*KEYPAIR_SEED_LENGTH_BYTES,
                     &csprng_state);

  expand_pk(V_tr,seed_e_seed_pk[1]);

  /* Expansion of seede, explicit domain separation for CSPRNG as in keygen */
  const uint16_t dsc_csprng_seed_e = CSPRNG_DOMAIN_SEP_CONST + (3*T+3);

  CSPRNG_STATE_T csprng_state_e_bar;
  csprng_initialize(&csprng_state_e_bar, seed_e_seed_pk[0], KEYPAIR_SEED_LENGTH_BYTES, dsc_csprng_seed_e);
  csprng_fz_vec(e_bar,&csprng_state_e_bar);
}
#elif defined(RSDPG)
static
void expand_sk(FZ_ELEM e_bar[N],
               FZ_ELEM e_G_bar[M],
               FP_ELEM V_tr[K][N-K],
               FZ_ELEM W_mat[M][N-M],
               const uint8_t seed_sk[KEYPAIR_SEED_LENGTH_BYTES])
{

  uint8_t seed_e_seed_pk[2][KEYPAIR_SEED_LENGTH_BYTES];
  CSPRNG_STATE_T csprng_state;

  /* Expansion of sk->seed, explicit domain separation for CSPRNG, as in keygen */
  const uint16_t dsc_csprng_seed_sk = CSPRNG_DOMAIN_SEP_CONST + (3*T+1);

  csprng_initialize(&csprng_state, seed_sk, KEYPAIR_SEED_LENGTH_BYTES, dsc_csprng_seed_sk);
  csprng_randombytes((uint8_t *)seed_e_seed_pk,
                     2*KEYPAIR_SEED_LENGTH_BYTES,
                     &csprng_state);

  expand_pk(V_tr,W_mat,seed_e_seed_pk[1]);

  /* Expansion of seede, explicit domain separation for CSPRNG as in keygen */
  const uint16_t dsc_csprng_seed_e = CSPRNG_DOMAIN_SEP_CONST + (3*T+3);

  CSPRNG_STATE_T csprng_state_e_bar;
  csprng_initialize(&csprng_state_e_bar, seed_e_seed_pk[0], KEYPAIR_SEED_LENGTH_BYTES, dsc_csprng_seed_e);
  csprng_fz_inf_w(e_G_bar,&csprng_state_e_bar);
#if (defined(HIGH_PERFORMANCE_AARCH64) && defined(RSDPG) )
    FZ_DOUBLEPREC W_mat_neon[M][ROUND_UP(N-M,EPI16_PER_REG)] = {0};
    for(int i = 0; i < M; i++){
        for (int j = 0; j < N-M; j++){
            W_mat_neon[i][j] = W_mat[i][j];
        }
    }
    fz_inf_w_by_fz_matrix(e_bar,e_G_bar,W_mat_neon);
#else
  fz_inf_w_by_fz_matrix(e_bar,e_G_bar,W_mat);
#endif
  fz_dz_norm_n(e_bar);
}
#endif

unsigned char testseed[32] = {
    0x98, 0x87, 0xF1, 0xFD,
    0x85, 0x42, 0x41, 0xA3,
    0x01, 0xEE, 0x01, 0x20,
    0x64, 0x5C, 0xD8, 0xE1,
    0x19, 0xB4, 0x3F, 0x7B,
    0xEE, 0x11, 0xF7, 0x7A,
    0x83, 0x5E, 0x9A, 0xDF,
    0x51, 0x8C, 0x3A, 0x51,
};

// Algorithm 1
void CROSS_keygen(sk_t *SK,
                  pk_t *PK){

#if defined(CYCLE_MAIN)
  uint64_t a_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(a_time));
#endif

  // row 1 and 8
  //--------------------------------------------------------------------------------                  
  /* generation of random material for public and private key */
  randombytes(SK->seed_sk,KEYPAIR_SEED_LENGTH_BYTES);
  //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t b_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(b_time));
#endif

  // declaration
  //-------------------------------------------------------------------------------- 
  uint8_t seed_e_seed_pk[2][KEYPAIR_SEED_LENGTH_BYTES];

  /* Expansion of sk->seed, explicit domain separation for CSPRNG */
  const uint16_t dsc_csprng_seed_sk = CSPRNG_DOMAIN_SEP_CONST + (3*T+1);
  //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t c_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(c_time));
#endif

  // row 2
  //--------------------------------------------------------------------------------
  CSPRNG_STATE_T csprng_state;
  csprng_initialize(&csprng_state, SK->seed_sk, KEYPAIR_SEED_LENGTH_BYTES, dsc_csprng_seed_sk);
  csprng_randombytes((uint8_t *)seed_e_seed_pk,
                     2*KEYPAIR_SEED_LENGTH_BYTES,
                     &csprng_state);
  memcpy(PK->seed_pk,seed_e_seed_pk[1],KEYPAIR_SEED_LENGTH_BYTES);
  //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t d_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(d_time));
#endif

  // row 3
  //--------------------------------------------------------------------------------
  /* expansion of matrix/matrices */
  FP_ELEM V_tr[K][N-K];

#if defined(RSDP)
  expand_pk(V_tr,PK->seed_pk);
#elif defined(RSDPG)
  FZ_ELEM W_mat[M][N-M];
  expand_pk(V_tr,W_mat,PK->seed_pk);
#endif
  //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t e_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(e_time));
#endif

  // row 5
  //--------------------------------------------------------------------------------
  /* expansion of secret key material */
  /* Expansion of seede, explicit domain separation for CSPRNG */
  const uint16_t dsc_csprng_seed_e = CSPRNG_DOMAIN_SEP_CONST + (3*T+3);

  CSPRNG_STATE_T csprng_state_e_bar;
  csprng_initialize(&csprng_state_e_bar, seed_e_seed_pk[0], KEYPAIR_SEED_LENGTH_BYTES, dsc_csprng_seed_e);

  FZ_ELEM e_bar[N];
#if defined(RSDP)
  csprng_fz_vec(e_bar,&csprng_state_e_bar);
#elif defined(RSDPG)
  FZ_ELEM e_G_bar[M];
  csprng_fz_inf_w(e_G_bar, &csprng_state_e_bar);
#if (defined(HIGH_PERFORMANCE_AARCH64) && defined(RSDPG) )
    FZ_DOUBLEPREC W_mat_neon[M][ROUND_UP(N-M,EPI16_PER_REG)] = {0};
    for(int i = 0; i < M; i++){
        for (int j = 0; j < N-M; j++){
            W_mat_neon[i][j] = W_mat[i][j];
        }
    }
    fz_inf_w_by_fz_matrix(e_bar,e_G_bar,W_mat_neon);
#else
  fz_inf_w_by_fz_matrix(e_bar,e_G_bar,W_mat);
#endif
  fz_dz_norm_n(e_bar);
#endif
  //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t f_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(f_time));
#endif

  // row 7 (row 4 and 6 are implicit here)
  //--------------------------------------------------------------------------------
  /* compute public syndrome */
  FP_ELEM s[N-K];
#if defined(HIGH_PERFORMANCE_AARCH64) && defined(RSDP)
    FP_DOUBLEPREC V_tr_neon[K][ROUND_UP(N-K,EPI16_PER_REG)] = {0};
    for(int i = 0; i < K; i++){
        for (int j = 0; j < N-K; j++){
            V_tr_neon[i][j] = (FP_DOUBLEPREC)V_tr[i][j];
        }
    }
    restr_vec_by_fp_matrix(s,e_bar,V_tr_neon);
#elif (defined(HIGH_PERFORMANCE_AARCH64) && defined(RSDPG) )
    FP_DOUBLEPREC V_tr_neon[K][ROUND_UP(N-K,EPI32_PER_REG)] = {0};
    for(int i = 0; i < K; i++){
        for (int j = 0; j < N-K; j++){
            V_tr_neon[i][j] = (FP_DOUBLEPREC)V_tr[i][j];
        }
    }
    restr_vec_by_fp_matrix(s,e_bar,V_tr_neon);
#else
  restr_vec_by_fp_matrix(s,e_bar,V_tr);
#endif
  //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t g_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(g_time));
#endif

  // row 9
  //--------------------------------------------------------------------------------
  fp_dz_norm_synd(s);
  pack_fp_syn(PK->s,s);
  //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t h_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(h_time));
  printf("KeyGen cycles, %d, %d, %d, %d, %d, %d, %d\n", (int)(b_time-a_time), (int)(c_time-b_time), (int)(d_time-c_time), (int)(e_time-d_time), (int)(f_time-e_time), (int)(g_time-f_time), (int)(h_time-g_time));
#endif

  // row 10 (return)
}

/* sign cannot fail */
void CROSS_sign(const sk_t *SK,
               const char *const m,
               const uint64_t mlen,
               CROSS_sig_t *sig){

#if defined(CYCLE_MAIN)
  uint64_t a_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(a_time));
#endif

    // row 28 (preparation)
    //--------------------------------------------------------------------------------            
    /* Wipe any residual information in the sig structure allocated by the 
     * caller */
    memset(sig,0,sizeof(CROSS_sig_t));
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t b_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(b_time));
#endif

    // row 1
    //--------------------------------------------------------------------------------
    /* Key material expansion */
    FP_ELEM V_tr[K][N-K];
    FZ_ELEM e_bar[N];

#if defined(RSDP)
    expand_sk(e_bar,V_tr,SK->seed_sk);
#elif defined(RSDPG)
    FZ_ELEM e_G_bar[M];
    FZ_ELEM W_mat[M][N-M];
    expand_sk(e_bar,e_G_bar,V_tr,W_mat,SK->seed_sk);
#endif
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t c_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(c_time));
#endif
    
    // neon preparation
    //--------------------------------------------------------------------------------
#if ( defined(HIGH_PERFORMANCE_AARCH64) && defined(RSDP) )
    /* Expanded */
    FP_DOUBLEPREC V_tr_neon[K][ROUND_UP(N-K,EPI16_PER_REG)] = {0};
    for(int i = 0; i < K; i++){
        for (int j = 0; j < N-K; j++){
            V_tr_neon[i][j] = (FP_DOUBLEPREC)V_tr[i][j];
        }
    }
#elif ( defined(HIGH_PERFORMANCE_AARCH64) && defined(RSDPG))
    FP_DOUBLEPREC V_tr_neon[K][ROUND_UP(N-K,EPI32_PER_REG)] = {0};
    for(int i = 0; i < K; i++){
        for (int j = 0; j < N-K; j++){
            V_tr_neon[i][j] = (FP_DOUBLEPREC)V_tr[i][j];
        }
    }
#endif
#if (defined(HIGH_PERFORMANCE_AARCH64) && defined(RSDPG))
    FZ_DOUBLEPREC W_mat_neon[M][ROUND_UP(N-M,EPI16_PER_REG)] = {0};
    for(int i = 0; i < M; i++){
        for (int j = 0; j < N-M; j++){
            W_mat_neon[i][j] = W_mat[i][j];
        }
    }
#endif
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t d_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(d_time));
#endif

    // row 2
    //--------------------------------------------------------------------------------
    uint8_t root_seed[SEED_LENGTH_BYTES];
    randombytes(root_seed,SEED_LENGTH_BYTES);
    randombytes(sig->salt,SALT_LENGTH_BYTES);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t e_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(e_time));
#endif

    // row 3
    //--------------------------------------------------------------------------------
#if defined(NO_TREES)
    unsigned char round_seeds[T*SEED_LENGTH_BYTES] = {0};
    seed_leaves(round_seeds, root_seed,sig->salt);
#else
    uint8_t seed_tree[SEED_LENGTH_BYTES*NUM_NODES_SEED_TREE] = {0};
    gen_seed_tree(seed_tree,root_seed,sig->salt);
    unsigned char round_seeds[T*SEED_LENGTH_BYTES] = {0};
    seed_leaves(round_seeds, seed_tree);
#endif
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t f_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(f_time));
#endif

    // declaration
    //--------------------------------------------------------------------------------
    FZ_ELEM e_bar_prime[T][N] = {0};
    FZ_ELEM v_bar[T][N] = {0};
    FP_ELEM u_prime[T][N] = {0};
    FP_ELEM s_prime[N-K] = {0};

#if defined(RSDP)
#if defined(HIGH_PERFORMANCE_AARCH64)
    uint8_t cmt_0_i_input[2][DENSELY_PACKED_FP_SYN_SIZE+
                             DENSELY_PACKED_FZ_VEC_SIZE+
                             SALT_LENGTH_BYTES];
    uint16_t cmt_0_i_input_dsc[2];
#else
    uint8_t cmt_0_i_input[DENSELY_PACKED_FP_SYN_SIZE+
                          DENSELY_PACKED_FZ_VEC_SIZE+
                          SALT_LENGTH_BYTES];
#endif
    const int offset_salt = DENSELY_PACKED_FP_SYN_SIZE+DENSELY_PACKED_FZ_VEC_SIZE;
#elif defined(RSDPG)
    FZ_ELEM e_G_bar_prime[M];
    FZ_ELEM v_G_bar[T][M];
#if defined(HIGH_PERFORMANCE_AARCH64)
    uint8_t cmt_0_i_input[2][DENSELY_PACKED_FP_SYN_SIZE+
                         DENSELY_PACKED_FZ_RSDP_G_VEC_SIZE+
                         SALT_LENGTH_BYTES];
    uint16_t cmt_0_i_input_dsc[2];
#else
    uint8_t cmt_0_i_input[DENSELY_PACKED_FP_SYN_SIZE+
                          DENSELY_PACKED_FZ_RSDP_G_VEC_SIZE+
                          SALT_LENGTH_BYTES];
#endif
    const int offset_salt = DENSELY_PACKED_FP_SYN_SIZE+DENSELY_PACKED_FZ_RSDP_G_VEC_SIZE;
#endif

    uint8_t cmt_1_i_input[2][SEED_LENGTH_BYTES+
                            SALT_LENGTH_BYTES];
    uint16_t cmt_1_i_input_dsc[2];
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t g_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(g_time));
#endif

    // row 5 and 10 preparation
    //--------------------------------------------------------------------------------
    /* place the salt in the hash input for all parallel instances of keccak */
    for(int instance=0; instance<2; instance++) {
        /* cmt_0_i_input is syndrome|| v_bar resp. v_G_bar ||salt ; place salt at the end */
        memcpy(cmt_0_i_input[instance]+offset_salt, sig->salt, SALT_LENGTH_BYTES);
        /* cmt_1_i_input is concat(seed,salt,round index) */
        memcpy(cmt_1_i_input[instance]+SEED_LENGTH_BYTES, sig->salt, SALT_LENGTH_BYTES);
    }
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t h_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(h_time));
#endif

    // declaration 2
    //--------------------------------------------------------------------------------
    // might take some time
    uint8_t cmt_0[T][HASH_DIGEST_LENGTH] = {0};
    uint8_t cmt_1[T*HASH_DIGEST_LENGTH] = {0};

    /* enqueue the calls to hash */
    int to_hash = 0;
    int round_idx_queue[2] = {0};

    CSPRNG_STATE_T csprng_state;
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t i_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(i_time));
#endif

    // row 4
    //--------------------------------------------------------------------------------
    for(uint16_t i = 0; i<T; i++){
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_LOOP)
  uint64_t aa_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(aa_time));
#endif

    // row 5, 6 and 7
    //--------------------------------------------------------------------------------
        round_idx_queue[to_hash] = i;
        /* CSPRNG is fed with concat(seed,salt,round index) represented
         * as a 2 bytes little endian unsigned integer */
        uint8_t csprng_input[SEED_LENGTH_BYTES+SALT_LENGTH_BYTES];
        memcpy(csprng_input,round_seeds+SEED_LENGTH_BYTES*i,SEED_LENGTH_BYTES);
        memcpy(csprng_input+SEED_LENGTH_BYTES,sig->salt,SALT_LENGTH_BYTES);

        uint16_t domain_sep_csprng = CSPRNG_DOMAIN_SEP_CONST+i+(2*T-1);

        /* expand seed[i] into seed_e and seed_u */
        csprng_initialize(&csprng_state,
                          csprng_input,
                          SEED_LENGTH_BYTES+SALT_LENGTH_BYTES,
                          domain_sep_csprng);
        /* expand e_bar_prime */
#if defined(RSDP)
        csprng_fz_vec(e_bar_prime[i], &csprng_state);
#elif defined(RSDPG)
        csprng_fz_inf_w(e_G_bar_prime, &csprng_state);
        fz_vec_sub_m(v_G_bar[i], e_G_bar, e_G_bar_prime);
        fz_dz_norm_m(v_G_bar[i]);
#if defined(HIGH_PERFORMANCE_AARCH64)
        fz_inf_w_by_fz_matrix(e_bar_prime[i], e_G_bar_prime, W_mat_neon);
#else
        fz_inf_w_by_fz_matrix(e_bar_prime[i], e_G_bar_prime, W_mat);
#endif
        fz_dz_norm_n(e_bar_prime[i]);
#endif
        fz_vec_sub_n(v_bar[i], e_bar, e_bar_prime[i]);

        FP_ELEM v[N];
        convert_restr_vec_to_fp(v, v_bar[i]);
        fz_dz_norm_n(v_bar[i]);
        /* expand u_prime */
        csprng_fp_vec(u_prime[i], &csprng_state);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_LOOP)
  uint64_t ab_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(ab_time));
#endif

    // row 8
    //--------------------------------------------------------------------------------
        FP_ELEM u[N];
        fp_vec_by_fp_vec_pointwise(u, v, u_prime[i]);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_LOOP)
  uint64_t ac_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(ac_time));
#endif

    // row 9
    //--------------------------------------------------------------------------------
#if ( defined(HIGH_PERFORMANCE_AARCH64) && defined(RSDP) )
        FP_DOUBLEPREC u_neon[ROUND_UP(N,EPI16_PER_REG)] = {0};
        for(int j = 0; j < N; j++) {
            u_neon[j] = (FP_DOUBLEPREC)u[j];
        }
        fp_vec_by_fp_matrix(s_prime, u_neon, V_tr_neon);
#elif ( defined(HIGH_PERFORMANCE_AARCH64) && defined(RSDPG))
        FP_DOUBLEPREC u_neon[ROUND_UP(N,EPI32_PER_REG)] = {0};
        for(int j = 0; j < N; j++) {
            u_neon[j] = (FP_DOUBLEPREC)u[j];
        }
        fp_vec_by_fp_matrix(s_prime, u_neon, V_tr_neon);
#else
        fp_vec_by_fp_matrix(s_prime, u, V_tr);
#endif
        fp_dz_norm_synd(s_prime);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_LOOP)
  uint64_t ad_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(ad_time));
#endif

    // row 10
    //--------------------------------------------------------------------------------
        /* cmt_0_i_input contains s_prime || v_bar resp. v_G_bar || salt */
        pack_fp_syn(cmt_0_i_input[to_hash],s_prime);

#if defined(RSDP)
        pack_fz_vec(cmt_0_i_input[to_hash] + DENSELY_PACKED_FP_SYN_SIZE, v_bar[i]);
#elif defined(RSDPG)
        pack_fz_rsdp_g_vec(cmt_0_i_input[to_hash] + DENSELY_PACKED_FP_SYN_SIZE, v_G_bar[i]);
#endif
        /* Fixed endianness marshalling of round counter */
        uint16_t domain_sep_hash = HASH_DOMAIN_SEP_CONST+i+(2*T-1);
        cmt_0_i_input_dsc[to_hash] = domain_sep_hash;

        memcpy(cmt_1_i_input[to_hash], round_seeds+SEED_LENGTH_BYTES*i, SEED_LENGTH_BYTES);

        cmt_1_i_input_dsc[to_hash++] = domain_sep_hash;

        if(to_hash == 2 || i == T-1){
            hash_par(
                to_hash,
                cmt_0[round_idx_queue[0]],
                cmt_0[round_idx_queue[1]],
                cmt_0_i_input[0],
                cmt_0_i_input[1],
                sizeof(cmt_0_i_input)/2,
                cmt_0_i_input_dsc[0],
                cmt_0_i_input_dsc[1]
            );
            hash_par(
                to_hash,
                &cmt_1[round_idx_queue[0]*HASH_DIGEST_LENGTH],
                &cmt_1[round_idx_queue[1]*HASH_DIGEST_LENGTH],
                cmt_1_i_input[0],
                cmt_1_i_input[1],
                sizeof(cmt_1_i_input)/2,
                cmt_1_i_input_dsc[0],
                cmt_1_i_input_dsc[1]
            );
            to_hash = 0;
        }
        //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_LOOP)
  uint64_t ae_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(ae_time));
#endif

#if defined(CYCLE_MAIN_LOOP)
  static uint64_t sum_ab_aa = 0, sum_ac_ab = 0, sum_ad_ac = 0, sum_ae_ad = 0;
  static uint64_t iters = 0;

  sum_ab_aa += (ab_time - aa_time);
  sum_ac_ab += (ac_time - ab_time);
  sum_ad_ac += (ad_time - ac_time);
  sum_ae_ad += (ae_time - ad_time);
  iters++;

  if(i == (T - 1)) {
      printf("Sign loop 1 cycles,over %llu iters, %llu, %llu, %llu, %llu\n",
             (unsigned long long)iters,
             (unsigned long long)(sum_ab_aa / iters),
             (unsigned long long)(sum_ac_ab / iters),
             (unsigned long long)(sum_ad_ac / iters),
             (unsigned long long)(sum_ae_ad / iters));

      sum_ab_aa = sum_ac_ab = sum_ad_ac = sum_ae_ad = 0;
      iters = 0;
  }
#endif

    } // end of loop for

#if defined(CYCLE_MAIN)
  uint64_t j_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(j_time));
#endif

  // row 11
  //--------------------------------------------------------------------------------
    /* vector containing d_0 and d_1 from spec */
    uint8_t digest_cmt0_cmt1[2*HASH_DIGEST_LENGTH];

#if defined(NO_TREES)
    tree_root(digest_cmt0_cmt1, cmt_0);
#else
    uint8_t merkle_tree_0[NUM_NODES_MERKLE_TREE * HASH_DIGEST_LENGTH];
    tree_root(digest_cmt0_cmt1, merkle_tree_0, cmt_0);
#endif
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t k_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(k_time));
#endif

    // row 12
    //--------------------------------------------------------------------------------
    hash(digest_cmt0_cmt1 + HASH_DIGEST_LENGTH, cmt_1, sizeof(cmt_1), HASH_DOMAIN_SEP_CONST);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t l_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(l_time));
#endif

    // row 13
    //--------------------------------------------------------------------------------
    hash(sig->digest_cmt, digest_cmt0_cmt1, sizeof(digest_cmt0_cmt1), HASH_DOMAIN_SEP_CONST);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t m_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(m_time));
#endif

    // row 14
    //--------------------------------------------------------------------------------
    /* first challenge extraction */
    uint8_t digest_msg_cmt_salt[2*HASH_DIGEST_LENGTH+SALT_LENGTH_BYTES];

    /* place digest_msg at the beginning of the input of the hash generating digest_chall_1 */
    hash(digest_msg_cmt_salt, (uint8_t*) m, mlen, HASH_DOMAIN_SEP_CONST);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t n_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(n_time));
#endif

    // row 15
    //--------------------------------------------------------------------------------
    memcpy(digest_msg_cmt_salt+HASH_DIGEST_LENGTH, sig->digest_cmt, HASH_DIGEST_LENGTH);
    memcpy(digest_msg_cmt_salt+2*HASH_DIGEST_LENGTH, sig->salt, SALT_LENGTH_BYTES);

    uint8_t digest_chall_1[HASH_DIGEST_LENGTH];
    hash(digest_chall_1, digest_msg_cmt_salt, sizeof(digest_msg_cmt_salt), HASH_DOMAIN_SEP_CONST);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t o_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(o_time));
#endif

    // row 16
    //--------------------------------------------------------------------------------
    // Domain separation unique for expanding chall_1
    const uint16_t dsc_csprng_chall_1 = CSPRNG_DOMAIN_SEP_CONST + (3*T-1);

    FP_ELEM chall_1[T];
    csprng_initialize(&csprng_state, digest_chall_1, sizeof(digest_chall_1), dsc_csprng_chall_1);
    csprng_fp_vec_chall_1(chall_1, &csprng_state);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t p_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(p_time));
#endif

    /* Computation of the first round of responses */
    FP_ELEM y[T][N];

    // row 17
    //--------------------------------------------------------------------------------
    for(int i = 0; i < T; i++){
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_LOOP)
  uint64_t ba_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(ba_time));
#endif

    // row 20 (row 18 and 19 are implicit here)
    //--------------------------------------------------------------------------------
        fp_vec_by_restr_vec_scaled(y[i],
                                   e_bar_prime[i],
                                   chall_1[i],
                                   u_prime[i]);
        fp_dz_norm(y[i]);
    //--------------------------------------------------------------------------------
    
#if defined(CYCLE_MAIN_LOOP)
  uint64_t bb_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(bb_time));
#endif

#if defined(CYCLE_MAIN_LOOP)
  static uint64_t sum_bb_ba = 0;
  static uint64_t iters2 = 0;

  sum_bb_ba += (bb_time - ba_time);
  iters2++;

  if(i == (T - 1)) {
      printf("Sign loop 2 cycles,over %llu iters, %llu\n",
             (unsigned long long)iters2,
             (unsigned long long)(sum_bb_ba / iters2));

      sum_bb_ba = 0;
      iters2 = 0;
  }
#endif

    } // end of loop for
    
#if defined(CYCLE_MAIN)
  uint64_t q_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(q_time));
#endif
  
    // row 21
    //--------------------------------------------------------------------------------
    /* y vectors are packed before being hashed */
    uint8_t y_digest_chall_1[T*DENSELY_PACKED_FP_VEC_SIZE+HASH_DIGEST_LENGTH];
    for(int x = 0; x < T; x++){
        pack_fp_vec(y_digest_chall_1+(x*DENSELY_PACKED_FP_VEC_SIZE),y[x]);
    }
    
    /* Second challenge extraction */
    memcpy(y_digest_chall_1+T*DENSELY_PACKED_FP_VEC_SIZE,digest_chall_1,HASH_DIGEST_LENGTH);

    hash(sig->digest_chall_2, y_digest_chall_1, sizeof(y_digest_chall_1), HASH_DOMAIN_SEP_CONST);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t r_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(r_time));
#endif

    // row 22
    //--------------------------------------------------------------------------------
    uint8_t chall_2[T]={0};
    expand_digest_to_fixed_weight(chall_2,sig->digest_chall_2);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t s_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(s_time));
#endif

    // row 23
    //--------------------------------------------------------------------------------
    /* Computation of the second round of responses */
#if defined(NO_TREES)
    tree_proof(sig->proof,cmt_0,chall_2);
#else
    tree_proof(sig->proof,merkle_tree_0,chall_2);
#endif
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t t_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(t_time));
#endif

    // row 24
    //--------------------------------------------------------------------------------
#if defined(NO_TREES)
    seed_path(sig->path,round_seeds,chall_2);
#else
    seed_path(sig->path,seed_tree,chall_2);
#endif
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t u_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(u_time));
#endif

    int published_rsps = 0;

#if defined(CYCLE_MAIN_CONDITION)
  static uint64_t sum_aab_aaa = 0;
  static uint64_t itt_intern = 0;
#endif

    // row 25
    //--------------------------------------------------------------------------------
    for(int i = 0; i<T; i++){
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_LOOP)
  uint64_t ca_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(ca_time));
#endif

    // row 26
    //--------------------------------------------------------------------------------
        if(chall_2[i] == 0){
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_CONDITION)
  uint64_t aaa_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(aaa_time));
#endif

    // row 27
    //--------------------------------------------------------------------------------
            assert(published_rsps < T-W);
            pack_fp_vec(sig->resp_0[published_rsps].y, y[i]);

#if defined(RSDP)
            pack_fz_vec(sig->resp_0[published_rsps].v_bar, v_bar[i]);
#elif defined(RSDPG)
            pack_fz_rsdp_g_vec(sig->resp_0[published_rsps].v_G_bar, v_G_bar[i]);
#endif
            memcpy(sig->resp_1[published_rsps], &cmt_1[i*HASH_DIGEST_LENGTH], HASH_DIGEST_LENGTH);
            published_rsps++;
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_CONDITION)
  uint64_t aab_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(aab_time));
#endif

#if defined(CYCLE_MAIN_CONDITION)
  sum_aab_aaa += (aab_time - aaa_time);
  itt_intern++;
#endif

        } // end of condition if

#if defined(CYCLE_MAIN_CONDITION)
  if(i == (T - 1)) {
      printf("Sign condition 1 if cycles,over %llu iters, %llu\n",
             (unsigned long long)itt_intern,
             (unsigned long long)(sum_aab_aaa / itt_intern));

      sum_aab_aaa = 0;
      itt_intern = 0;
  }
#endif

#if defined(CYCLE_MAIN_LOOP)
  uint64_t cb_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(cb_time));
#endif

#if defined(CYCLE_MAIN_LOOP)
  static uint64_t sum_cb_ca = 0;
  static uint64_t iters3 = 0;

  sum_cb_ca += (cb_time - ca_time);
  iters3++;

  if(i == (T - 1)) {
      printf("Sign loop 3 cycles,over %llu iters, %llu\n",
             (unsigned long long)iters3,
             (unsigned long long)(sum_cb_ca / iters3));

      sum_cb_ca = 0;
      iters3 = 0;
  }
#endif

    } // end of loop for

#if defined(CYCLE_MAIN)
  uint64_t v_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(v_time));
#endif

    // row 29 (return)

#if defined(CYCLE_MAIN)
  uint64_t t[] = {a_time,b_time,c_time,d_time,e_time,f_time,g_time,h_time,i_time,j_time,k_time,l_time,m_time,n_time,o_time,p_time,q_time,r_time,s_time,t_time,u_time,v_time};
  printf("Sign cycles");
  for (int i = 1; i < sizeof(t)/sizeof(t[0]); i++) {
      printf(", %d", (int)(t[i] - t[i-1]));
  }
  printf("\n");
#endif

}

// Algorithm 3
/* verify returns 1 if signature is ok, 0 otherwise */
int CROSS_verify(const pk_t *const PK,
                 const char *const m,
                 const uint64_t mlen,
                 const CROSS_sig_t *const sig){
    CSPRNG_STATE_T csprng_state;

#if defined(CYCLE_MAIN)
  uint64_t a_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(a_time));
#endif

    // row 1
    //--------------------------------------------------------------------------------
    FP_ELEM V_tr[K][N-K];

#if defined(RSDP)
    expand_pk(V_tr,PK->seed_pk);
#elif defined(RSDPG)
    FZ_ELEM W_mat[M][N-M];
    expand_pk(V_tr,W_mat,PK->seed_pk);
#endif
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t b_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(b_time));
#endif

    // preparation neon
    //--------------------------------------------------------------------------------
#if ( defined(HIGH_PERFORMANCE_AARCH64) && defined(RSDP) )
    /* Expanded */
    FP_DOUBLEPREC V_tr_neon[K][ROUND_UP(N-K,EPI16_PER_REG)] = {0};
    for(int i = 0; i < K; i++){
        for (int j = 0; j < N-K; j++){
            V_tr_neon[i][j] = (FP_DOUBLEPREC)V_tr[i][j];
        }
    }
#elif ( defined(HIGH_PERFORMANCE_AARCH64) && defined(RSDPG))
    FP_DOUBLEPREC V_tr_neon[K][ROUND_UP(N-K,EPI32_PER_REG)] = {0};
    for(int i = 0; i < K; i++){
        for (int j = 0; j < N-K; j++){
            V_tr_neon[i][j] = (FP_DOUBLEPREC)V_tr[i][j];
        }
    }
#endif
#if (defined(HIGH_PERFORMANCE_AARCH64) && defined(RSDPG))
    FZ_DOUBLEPREC W_mat_neon[M][ROUND_UP(N-M,EPI16_PER_REG)] = {0};
    for(int i = 0; i < M; i++){
        for (int j = 0; j < N-M; j++){
            W_mat_neon[i][j] = W_mat[i][j];
        }
    }
#endif
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t c_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(c_time));
#endif

    // unpack syndrome
    //--------------------------------------------------------------------------------
    FP_ELEM s[N-K];
    uint8_t is_padd_key_ok;
    is_padd_key_ok = unpack_fp_syn(s,PK->s);
    //--------------------------------------------------------------------------------

    // row 2 and 3 are integrated in fuctions

#if defined(CYCLE_MAIN)
  uint64_t d_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(d_time));
#endif

    // row 4
    //--------------------------------------------------------------------------------
    uint8_t digest_msg_cmt_salt[2*HASH_DIGEST_LENGTH+SALT_LENGTH_BYTES];
    hash(digest_msg_cmt_salt, (uint8_t*) m, mlen, HASH_DOMAIN_SEP_CONST);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t e_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(e_time));
#endif

    // row 5
    //--------------------------------------------------------------------------------
    memcpy(digest_msg_cmt_salt+HASH_DIGEST_LENGTH, sig->digest_cmt, HASH_DIGEST_LENGTH);
    memcpy(digest_msg_cmt_salt+2*HASH_DIGEST_LENGTH, sig->salt, SALT_LENGTH_BYTES);

    uint8_t digest_chall_1[HASH_DIGEST_LENGTH];
    hash(digest_chall_1, digest_msg_cmt_salt, sizeof(digest_msg_cmt_salt), HASH_DOMAIN_SEP_CONST);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t f_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(f_time));
#endif

    // row 6
    //--------------------------------------------------------------------------------
    // Domain separation unique for expanding digest_chall_1
    const uint16_t dsc_csprng_chall_1 = CSPRNG_DOMAIN_SEP_CONST + (3*T-1);
    csprng_initialize(&csprng_state, digest_chall_1, sizeof(digest_chall_1), dsc_csprng_chall_1);

    FP_ELEM chall_1[T];
    csprng_fp_vec_chall_1(chall_1, &csprng_state);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t g_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(g_time));
#endif

    // row 7
    //--------------------------------------------------------------------------------
    uint8_t chall_2[T]={0};
    expand_digest_to_fixed_weight(chall_2,sig->digest_chall_2);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t h_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(h_time));
#endif

    // row 8
    //--------------------------------------------------------------------------------
    uint8_t is_stree_padding_ok = 0;
#if defined(NO_TREES)
    uint8_t round_seeds[T*SEED_LENGTH_BYTES] = {0};
    is_stree_padding_ok = rebuild_leaves(round_seeds, chall_2, sig->path);
#else
    uint8_t seed_tree[SEED_LENGTH_BYTES*NUM_NODES_SEED_TREE] = {0};
    is_stree_padding_ok = rebuild_tree(seed_tree, chall_2, sig->path, sig->salt);

    unsigned char round_seeds[T*SEED_LENGTH_BYTES] = {0};
    seed_leaves(round_seeds, seed_tree);
#endif
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t i_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(i_time));
#endif

    // declaration
    //--------------------------------------------------------------------------------
#if defined(RSDP)
    uint8_t cmt_0_i_input[2][DENSELY_PACKED_FP_SYN_SIZE+
                             DENSELY_PACKED_FZ_VEC_SIZE+
                             SALT_LENGTH_BYTES];
    uint16_t cmt_0_i_input_dsc[2];
    const int offset_salt = DENSELY_PACKED_FP_SYN_SIZE+DENSELY_PACKED_FZ_VEC_SIZE;
#elif defined(RSDPG)
    uint8_t cmt_0_i_input[2][DENSELY_PACKED_FP_SYN_SIZE+
                             DENSELY_PACKED_FZ_RSDP_G_VEC_SIZE+
                             SALT_LENGTH_BYTES];
    uint16_t cmt_0_i_input_dsc[2];
    const int offset_salt = DENSELY_PACKED_FP_SYN_SIZE+DENSELY_PACKED_FZ_RSDP_G_VEC_SIZE;
#endif

    uint8_t cmt_1_i_input[2][SEED_LENGTH_BYTES+SALT_LENGTH_BYTES];
    uint16_t cmt_1_i_input_dsc[2];

    /* place the salt in the hash input for all parallel instances of keccak */
    for(int instance=0; instance<2; instance++) {
        /* cmt_0_i_input is syndrome|| v_bar resp. v_G_bar ||salt */
        memcpy(cmt_0_i_input[instance]+offset_salt, sig->salt, SALT_LENGTH_BYTES);
        /* cmt_1_i_input is concat(seed,salt,round index) */
        memcpy(cmt_1_i_input[instance]+SEED_LENGTH_BYTES, sig->salt, SALT_LENGTH_BYTES);
    }

    uint8_t cmt_0[T][HASH_DIGEST_LENGTH] = {0};
    uint8_t cmt_1[T*HASH_DIGEST_LENGTH] = {0};

    FZ_ELEM e_bar_prime[N];
    FP_ELEM u_prime[N];

    FP_ELEM y_prime[N] = {0};
    FP_ELEM y_prime_H[N-K] = {0};
    FP_ELEM s_prime[N-K] = {0};

    FP_ELEM y[T][N];

    /* enqueue the calls to hash */
    int to_hash_cmt_1 = 0;
    int to_hash_cmt_0 = 0;
    int round_idx_queue_cmt_1[2] = {0};
    int round_idx_queue_cmt_0[2] = {0};

    int used_rsps = 0;
    int is_signature_ok = 1;
    uint8_t is_packed_padd_ok = 1;
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t j_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(j_time));
#endif


#if defined(CYCLE_MAIN_CONDITION)
  static uint64_t sum_aab_aaa = 0, sum_aac_aab = 0, sum_aad_aac = 0;
  static uint64_t iters = 0;
  static uint64_t sum_aaf_aae = 0, sum_aag_aaf = 0, sum_aah_aag = 0, sum_aai_aah = 0, sum_aaj_aai = 0;
  static uint64_t iters2 = 0;
  static uint64_t sum_bbb_bba = 0;
  static uint64_t iters3 = 0;
  static uint64_t sum_ccb_cca = 0;
  static uint64_t iters4 = 0;
#endif

    // row 9
    //--------------------------------------------------------------------------------
    for(uint16_t i = 0; i< T; i++){
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_LOOP)
  uint64_t aa_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(aa_time));
#endif

    // preparation
    //--------------------------------------------------------------------------------
        uint16_t domain_sep_csprng = CSPRNG_DOMAIN_SEP_CONST + i + (2*T-1);
        uint16_t domain_sep_hash = HASH_DOMAIN_SEP_CONST + i + (2*T-1);
    //--------------------------------------------------------------------------------

    // row 10
    //--------------------------------------------------------------------------------
        if(chall_2[i] == 1){
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_CONDITION)
  uint64_t aaa_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(aaa_time));
#endif

    // preparation
    //--------------------------------------------------------------------------------
            /* save the index for the hash output */
            round_idx_queue_cmt_1[to_hash_cmt_1] = i;

            memcpy(cmt_1_i_input[to_hash_cmt_1],
                   round_seeds+SEED_LENGTH_BYTES*i,
                   SEED_LENGTH_BYTES);

            cmt_1_i_input_dsc[to_hash_cmt_1++] = domain_sep_hash;
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_CONDITION)
  uint64_t aab_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(aab_time));
#endif

    // row 12
    //--------------------------------------------------------------------------------
            /* CSPRNG is fed with concat(seed,salt,round index) represented
            * as a 2 bytes little endian unsigned integer */
            const int csprng_input_length = SALT_LENGTH_BYTES+SEED_LENGTH_BYTES;
            uint8_t csprng_input[csprng_input_length];
            memcpy(csprng_input+SEED_LENGTH_BYTES,sig->salt,SALT_LENGTH_BYTES);
            memcpy(csprng_input,round_seeds+SEED_LENGTH_BYTES*i,SEED_LENGTH_BYTES);

            /* expand seed[i] into seed_e and seed_u */
            csprng_initialize(&csprng_state, csprng_input, csprng_input_length, domain_sep_csprng);
#if defined(RSDP)
            /* expand e_bar_prime */
            csprng_fz_vec(e_bar_prime, &csprng_state);
#elif defined(RSDPG)
            FZ_ELEM e_G_bar_prime[M];
            csprng_fz_inf_w(e_G_bar_prime, &csprng_state);
#if defined(HIGH_PERFORMANCE_AARCH64)
            fz_inf_w_by_fz_matrix(e_bar_prime,e_G_bar_prime,W_mat_neon);
#else
            fz_inf_w_by_fz_matrix(e_bar_prime,e_G_bar_prime,W_mat);
#endif
            fz_dz_norm_n(e_bar_prime);
#endif
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_CONDITION)
  uint64_t aac_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(aac_time));
#endif

    // row 13 and 14
    //--------------------------------------------------------------------------------
            /* expand u_prime */
            csprng_fp_vec(u_prime, &csprng_state);
            fp_vec_by_restr_vec_scaled(y[i],
                                       e_bar_prime,
                                       chall_1[i],
                                       u_prime);
            fp_dz_norm(y[i]);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_CONDITION)
  uint64_t aad_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(aad_time));
#endif

#if defined(CYCLE_MAIN_CONDITION)
  sum_aab_aaa += (aab_time - aaa_time);
  sum_aac_aab += (aac_time - aab_time);
  sum_aad_aac += (aad_time - aac_time);
  iters++;
#endif

    // row 15
    //--------------------------------------------------------------------------------        
        } else {
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_CONDITION)
  uint64_t aae_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(aae_time));
#endif

    // preparation
    //--------------------------------------------------------------------------------   
            /* save the index for the hash output */
            round_idx_queue_cmt_0[to_hash_cmt_0] = i;
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_CONDITION)
  uint64_t aaf_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(aaf_time));
#endif

    // row 17
    //--------------------------------------------------------------------------------        
            /* place y[i] in the buffer for later on hashing */
            is_packed_padd_ok = is_packed_padd_ok &&
                                unpack_fp_vec(y[i], sig->resp_0[used_rsps].y);

            FZ_ELEM v_bar[N];
#if defined(RSDP)
            /*v_bar is memcpy'ed directly into cmt_0 input buffer */
            FZ_ELEM* v_bar_ptr = cmt_0_i_input[to_hash_cmt_0]+DENSELY_PACKED_FP_SYN_SIZE;
            is_packed_padd_ok = is_packed_padd_ok &&
                                unpack_fz_vec(v_bar, sig->resp_0[used_rsps].v_bar);
            memcpy(v_bar_ptr,
                   &sig->resp_0[used_rsps].v_bar,
                   DENSELY_PACKED_FZ_VEC_SIZE);
            is_signature_ok = is_signature_ok &&
                              is_fz_vec_in_restr_group_n(v_bar);
#elif defined(RSDPG)
            /*v_G_bar is memcpy'ed directly into cmt_0 input buffer */
            FZ_ELEM* v_G_bar_ptr = cmt_0_i_input[to_hash_cmt_0]+DENSELY_PACKED_FP_SYN_SIZE;
            memcpy(v_G_bar_ptr,
                   &sig->resp_0[used_rsps].v_G_bar,
                   DENSELY_PACKED_FZ_RSDP_G_VEC_SIZE);
            FZ_ELEM v_G_bar[M];
            is_packed_padd_ok = is_packed_padd_ok &&
                                unpack_fz_rsdp_g_vec(v_G_bar, sig->resp_0[used_rsps].v_G_bar);
            is_signature_ok = is_signature_ok &&
                              is_fz_vec_in_restr_group_m(v_G_bar);
#if defined(HIGH_PERFORMANCE_AARCH64)
            fz_inf_w_by_fz_matrix(v_bar,v_G_bar,W_mat_neon);
#else
            fz_inf_w_by_fz_matrix(v_bar,v_G_bar,W_mat);
#endif
#endif
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_CONDITION)
  uint64_t aag_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(aag_time));
#endif

    // row 16
    //--------------------------------------------------------------------------------
            memcpy(&cmt_1[i*HASH_DIGEST_LENGTH], sig->resp_1[used_rsps], HASH_DIGEST_LENGTH);
            used_rsps++;
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_CONDITION)
  uint64_t aah_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(aah_time));
#endif

    // row 18, 19 and 20
    //--------------------------------------------------------------------------------
            FP_ELEM v[N];
            convert_restr_vec_to_fp(v,v_bar);
            fp_vec_by_fp_vec_pointwise(y_prime,v,y[i]);
#if ( defined(HIGH_PERFORMANCE_AARCH64) && defined(RSDP) )
            FP_DOUBLEPREC y_prime_neon[ROUND_UP(N,EPI16_PER_REG)] = {0};
            for(int j = 0; j < N; j++) {
                y_prime_neon[j] = (FP_DOUBLEPREC)y_prime[j];
            }
            fp_vec_by_fp_matrix(y_prime_H,y_prime_neon,V_tr_neon);
#elif ( defined(HIGH_PERFORMANCE_AARCH64) && defined(RSDPG))
            FP_DOUBLEPREC y_prime_neon[ROUND_UP(N,EPI32_PER_REG)] = {0};
            for(int j = 0; j < N; j++) {
                y_prime_neon[j] = (FP_DOUBLEPREC)y_prime[j];
            }
            fp_vec_by_fp_matrix(y_prime_H,y_prime_neon,V_tr_neon);
#else
            fp_vec_by_fp_matrix(y_prime_H,y_prime,V_tr);
#endif
            fp_dz_norm_synd(y_prime_H);
            fp_synd_minus_fp_vec_scaled(s_prime,
                                        y_prime_H,
                                        chall_1[i],
                                        s);
            fp_dz_norm_synd(s_prime);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_CONDITION)
  uint64_t aai_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(aai_time));
#endif

    // row 21
    //--------------------------------------------------------------------------------
            pack_fp_syn(cmt_0_i_input[to_hash_cmt_0],s_prime);
            cmt_0_i_input_dsc[to_hash_cmt_0++] = domain_sep_hash;
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_CONDITION)
  uint64_t aaj_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(aaj_time));
#endif

#if defined(CYCLE_MAIN_CONDITION)
  sum_aaf_aae += (aaf_time - aae_time);
  sum_aag_aaf += (aag_time - aaf_time);
  sum_aah_aag += (aah_time - aag_time);
  sum_aai_aah += (aai_time - aah_time);
  sum_aaj_aai += (aaj_time - aai_time);
  iters2++;
#endif


        } // end of condition else

#if defined(CYCLE_MAIN_CONDITION)
  if(i == (T - 1) && iters != 0) {
      printf("Verif condition 1 if cycles, over %llu iters, %llu, %llu, %llu\n",
             (unsigned long long)iters,
             (unsigned long long)(sum_aab_aaa / iters),
             (unsigned long long)(sum_aac_aab / iters),
             (unsigned long long)(sum_aad_aac / iters));

      sum_aab_aaa = sum_aac_aab = sum_aad_aac = 0;
      iters = 0;
  }
#endif

#if defined(CYCLE_MAIN_CONDITION)
  if(i == (T - 1) && iters2 != 0) {
      printf("Verif condition 1 else cycles, over %llu iters, %llu, %llu, %llu, %llu, %llu\n",
             (unsigned long long)iters2,
             (unsigned long long)(sum_aaf_aae / iters2),
             (unsigned long long)(sum_aag_aaf / iters2),
             (unsigned long long)(sum_aah_aag / iters2),
             (unsigned long long)(sum_aai_aah / iters2),
             (unsigned long long)(sum_aaj_aai / iters2));

      sum_aaf_aae = sum_aag_aaf = sum_aah_aag = sum_aai_aah = sum_aaj_aai = 0;
      iters2 = 0;
  }
#endif


#if defined(CYCLE_MAIN_LOOP)
  uint64_t ab_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(ab_time));
#endif

    // row 11
    //--------------------------------------------------------------------------------
        /* hash commitment 1 in batches of 4 (or less on the last round) */
        if(to_hash_cmt_1 == 2 || i == T-1){

#if defined(CYCLE_MAIN_CONDITION)
  uint64_t bba_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(bba_time));
#endif

            hash_par(
                to_hash_cmt_1,
                &cmt_1[round_idx_queue_cmt_1[0]*HASH_DIGEST_LENGTH],
                &cmt_1[round_idx_queue_cmt_1[1]*HASH_DIGEST_LENGTH],
                cmt_1_i_input[0],
                cmt_1_i_input[1],
                sizeof(cmt_1_i_input)/2,
                cmt_1_i_input_dsc[0],
                cmt_1_i_input_dsc[1]
            );
            to_hash_cmt_1 = 0;

#if defined(CYCLE_MAIN_CONDITION)
  uint64_t bbb_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(bbb_time));
#endif

#if defined(CYCLE_MAIN_CONDITION)
  sum_bbb_bba += (bbb_time - bba_time);
  iters3++;
#endif

        } // end of condition if
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_CONDITION)
  if(i == (T - 1) && iters3 != 0) {
      printf("Verif condition 2 if cycles, over %llu iters, %llu\n",
             (unsigned long long)iters3,
             (unsigned long long)(sum_bbb_bba / iters3));

      sum_bbb_bba = 0;
      iters3 = 0;
  }
#endif


#if defined(CYCLE_MAIN_LOOP)
  uint64_t ac_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(ac_time));
#endif

    // row 21
    //--------------------------------------------------------------------------------
        /* hash commitment 0 in batches of 4 (or less on the last round) */
        if(to_hash_cmt_0 == 2 || i == T-1){

#if defined(CYCLE_MAIN_CONDITION)
  uint64_t cca_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(cca_time));
#endif

            hash_par(
                to_hash_cmt_0,
                cmt_0[round_idx_queue_cmt_0[0]],
                cmt_0[round_idx_queue_cmt_0[1]],
                cmt_0_i_input[0],
                cmt_0_i_input[1],
                sizeof(cmt_0_i_input)/2,
                cmt_0_i_input_dsc[0],
                cmt_0_i_input_dsc[1]
            );
            to_hash_cmt_0 = 0;

#if defined(CYCLE_MAIN_CONDITION)
  uint64_t ccb_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(ccb_time));
#endif

#if defined(CYCLE_MAIN_CONDITION)
  sum_ccb_cca += (ccb_time - cca_time);
  iters4++;
#endif

        } // end of condition if
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN_CONDITION)
  if(i == (T - 1) && iters4 != 0) {
      printf("Verif condition 3 if cycles, over %llu iters, %llu\n",
             (unsigned long long)iters4,
             (unsigned long long)(sum_ccb_cca / iters4));

      sum_ccb_cca = 0;
      iters4 = 0;
  }
#endif

#if defined(CYCLE_MAIN_LOOP)
  uint64_t ad_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(ad_time));
#endif

#if defined(CYCLE_MAIN_LOOP)
  static uint64_t sum_ab_aa = 0, sum_ac_ab = 0, sum_ad_ac = 0, iters = 0;

  sum_ab_aa += (ab_time - aa_time);
  sum_ac_ab += (ac_time - ab_time);
  sum_ad_ac += (ad_time - ac_time);
  iters++;

  if(i == (T - 1)) {
      printf("Verif loop 1 cycles, over %llu iters, %llu, %llu, %llu\n",
             (unsigned long long)iters,
             (unsigned long long)(sum_ab_aa / iters),
             (unsigned long long)(sum_ac_ab / iters),
             (unsigned long long)(sum_ad_ac / iters));

      sum_ab_aa = sum_ac_ab = sum_ad_ac = 0;
      iters = 0;
  }
#endif

    } // end of loop for
    
    /* end for iterating on ZKID iterations */

#if defined(CYCLE_MAIN)
  uint64_t k_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(k_time));
#endif

    // row 22
    //--------------------------------------------------------------------------------
    #ifndef SKIP_ASSERT
    assert(is_signature_ok);
    #endif

    uint8_t digest_cmt0_cmt1[2*HASH_DIGEST_LENGTH];

    uint8_t is_mtree_padding_ok = recompute_root(digest_cmt0_cmt1,
                                                 cmt_0,
                                                 sig->proof,
                                                 chall_2);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t l_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(l_time));
#endif

    // row 23
    //--------------------------------------------------------------------------------
    hash(digest_cmt0_cmt1 + HASH_DIGEST_LENGTH, cmt_1, sizeof(cmt_1), HASH_DOMAIN_SEP_CONST);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t m_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(m_time));
#endif

    // row 24
    //--------------------------------------------------------------------------------
    uint8_t digest_cmt_prime[HASH_DIGEST_LENGTH];
    hash(digest_cmt_prime, digest_cmt0_cmt1 ,sizeof(digest_cmt0_cmt1), HASH_DOMAIN_SEP_CONST);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t n_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(n_time));
#endif

    // row 25
    //--------------------------------------------------------------------------------
    uint8_t y_digest_chall_1[T*DENSELY_PACKED_FP_VEC_SIZE+HASH_DIGEST_LENGTH];

    for(int x = 0; x < T; x++){
        pack_fp_vec(y_digest_chall_1+(x*DENSELY_PACKED_FP_VEC_SIZE), y[x]);
    }
    memcpy(y_digest_chall_1+T*DENSELY_PACKED_FP_VEC_SIZE, digest_chall_1, HASH_DIGEST_LENGTH);

    uint8_t digest_chall_2_prime[HASH_DIGEST_LENGTH];
    hash(digest_chall_2_prime, y_digest_chall_1, sizeof(y_digest_chall_1), HASH_DOMAIN_SEP_CONST);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t o_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(o_time));
#endif

    // row 26
    //--------------------------------------------------------------------------------
    int does_digest_cmt_match = ( memcmp(digest_cmt_prime,
                                        sig->digest_cmt,
                                        HASH_DIGEST_LENGTH) == 0);

    #ifndef SKIP_ASSERT
    assert(does_digest_cmt_match);
    #endif

    int does_digest_chall_2_match = ( memcmp(digest_chall_2_prime,
                                        sig->digest_chall_2,
                                        HASH_DIGEST_LENGTH) == 0);
    #ifndef SKIP_ASSERT
    assert(does_digest_chall_2_match);
    #endif

    is_signature_ok = is_signature_ok &&
                      does_digest_cmt_match &&
                      does_digest_chall_2_match &&
                      is_mtree_padding_ok &&
                      is_stree_padding_ok &&
                      is_padd_key_ok &&
                      is_packed_padd_ok;
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
  uint64_t p_time;
  asm volatile("mrs %0, cntvct_el0" : "=r"(p_time));
#endif

#if defined(CYCLE_MAIN)
  uint64_t t[] = {a_time,b_time,c_time,d_time,e_time,f_time,g_time,h_time,i_time,j_time,k_time,l_time,m_time,n_time,o_time,p_time};
  printf("Verif cycles");
  for (int i = 1; i < sizeof(t)/sizeof(t[0]); i++) {
      printf(", %d", (int)(t[i] - t[i-1]));
  }
  printf("\n");
#endif

    // row 27 and 28
    //--------------------------------------------------------------------------------
    return is_signature_ok;
    //--------------------------------------------------------------------------------
}

