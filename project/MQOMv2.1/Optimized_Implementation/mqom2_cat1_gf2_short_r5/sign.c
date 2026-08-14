#ifdef SUPERCOP
#include "crypto_sign.h"
#else
#include "api.h"
#endif

#include <stdlib.h>
#include "common.h"
#include "fields.h"
#include "xof.h"
#include "expand_mq.h"
#include "blc.h"
#include "piop.h"
#include "benchmark.h"

#if defined(CYCLE_MAIN) || defined(CYCLE_MAIN_LOOP) || defined(CYCLE_MAIN_CONDITION)
    // to measure cycles of keygen, sign and verify
    #include "../../timing_and_stat.h"
#endif


#include <stdio.h>
extern int randombytes(unsigned char* x, unsigned long long xlen);

#if defined(USE_XOF_X4)
int SampleChallenge(const uint8_t hash[MQOM2_PARAM_DIGEST_SIZE], uint16_t i_star[MQOM2_PARAM_TAU], uint8_t nonce[4])
{
    int ret = -1;
    int e;
    xof_context_x4 xof_ctx;
    uint32_t nonce_int[4] = { 0, 1, 2, 3 };
    uint8_t _nonce[4][4];
    uint16_t _i_star[4][MQOM2_PARAM_TAU];
    unsigned int i;
    const uint8_t *constant_5[4] = { (const uint8_t*) "\x05", (const uint8_t*) "\x05", (const uint8_t*) "\x05", (const uint8_t*) "\x05" };
    const uint8_t *hash_ptr[4] = { hash, hash, hash, hash };
    uint8_t *nonce_ptr[4] = { _nonce[0], _nonce[1], _nonce[2], _nonce[3] };
   
    uint16_t val;
    uint8_t tmp[4][MQOM2_PARAM_TAU*2+2];
    uint8_t *tmp_ptr[4] = { tmp[0], tmp[1], tmp[2], tmp[3] };

    do {
        for(i = 0; i < 4; i++){
            _nonce[i][0] = nonce_int[i] & 0xff;
            _nonce[i][1] = (nonce_int[i]>> 8) & 0xff;
            _nonce[i][2] = (nonce_int[i]>>16) & 0xff;
            _nonce[i][3] = (nonce_int[i]>>24) & 0xff;
        }
        ret = xof_init_x4(&xof_ctx); ERR(ret, err);
        ret = xof_update_x4(&xof_ctx, constant_5, 1); ERR(ret, err);
        ret = xof_update_x4(&xof_ctx, hash_ptr, MQOM2_PARAM_DIGEST_SIZE); ERR(ret, err);
        ret = xof_update_x4(&xof_ctx, (const uint8_t**)nonce_ptr, 4); ERR(ret, err);
        ret = xof_squeeze_x4(&xof_ctx, tmp_ptr, MQOM2_PARAM_TAU*2+2); ERR(ret, err);
        for(i = 0; i < 4; i++){
            for(e = 0; e < MQOM2_PARAM_TAU; e++) {
                _i_star[i][e] = (tmp[i][2*e] + 256*tmp[i][2*e+1]) & ((1<<MQOM2_PARAM_NB_EVALS_LOG)-1);
            }
            val = (tmp[i][2*MQOM2_PARAM_TAU] + tmp[i][2*MQOM2_PARAM_TAU+1]*256) & ((1<<MQOM2_PARAM_W)-1);
            if(val == 0){
                goto out_loop;
            }
            nonce_int[i] += 4;
        }
    } while(1);

out_loop:
    memcpy(i_star, _i_star[i], MQOM2_PARAM_TAU * sizeof(uint16_t));
    memcpy(nonce, _nonce[i], 4);

    ret = 0;
err:
        return ret;
}
#else
int SampleChallenge(uint8_t hash[MQOM2_PARAM_DIGEST_SIZE], uint16_t i_star[MQOM2_PARAM_TAU], uint8_t nonce[4])
{
    int ret = -1;
    int e;
	xof_context xof_ctx;
    uint32_t nonce_int = 0;

    uint16_t val;
    uint8_t tmp[MQOM2_PARAM_TAU*2+2];
    do {
        nonce[0] = nonce_int & 0xff;
        nonce[1] = (nonce_int>> 8) & 0xff;
        nonce[2] = (nonce_int>>16) & 0xff;
        nonce[3] = (nonce_int>>24) & 0xff;
        ret = xof_init(&xof_ctx); ERR(ret, err);
	    ret = xof_update(&xof_ctx, (const uint8_t*) "\x05", 1); ERR(ret, err);
        ret = xof_update(&xof_ctx, hash, MQOM2_PARAM_DIGEST_SIZE); ERR(ret, err);
        ret = xof_update(&xof_ctx, nonce, 4); ERR(ret, err);
        ret = xof_squeeze(&xof_ctx, tmp, MQOM2_PARAM_TAU*2+2); ERR(ret, err);
        for(e = 0; e<MQOM2_PARAM_TAU; e++) {
            i_star[e] = (tmp[2*e] + 256*tmp[2*e+1]) & ((1<<MQOM2_PARAM_NB_EVALS_LOG)-1);
        }
        val = (tmp[2*MQOM2_PARAM_TAU] + tmp[2*MQOM2_PARAM_TAU+1]*256) & ((1<<MQOM2_PARAM_W)-1);
        nonce_int++;
    } while(val != 0);

    ret = 0;
err:
	return ret;
}
#endif

int Sign(const uint8_t sk[MQOM2_SK_SIZE], const uint8_t *msg, unsigned long long mlen, const uint8_t salt[MQOM2_PARAM_SALT_SIZE], const uint8_t mseed[MQOM2_PARAM_SEED_SIZE], uint8_t sig[MQOM2_SIG_SIZE]) {

#if defined(CYCLE_MAIN)
    uint64_t a_time = 0;
    uint64_t b_time = 0;
    uint64_t c_time = 0;
    uint64_t d_time = 0;
    uint64_t e_time = 0;
    uint64_t f_time = 0;
    uint64_t g_time = 0;
    uint64_t h_time = 0;
    uint64_t i_time = 0;
    uint64_t j_time = 0;
    uint64_t k_time = 0;
    uint64_t l_time = 0;
    uint64_t m_time = 0;

    a_time = x86_64_rtdsc();
#endif

    // declarations
    // -------------------------------------------------------------------------
    int ret = -1;
    int e;
    uint8_t mseed_eq[2 * MQOM2_PARAM_SEED_SIZE];
    uint8_t msg_hash[MQOM2_PARAM_DIGEST_SIZE], hash[MQOM2_PARAM_DIGEST_SIZE];
    field_base_elt x[FIELD_BASE_PACKING(MQOM2_PARAM_MQ_N)];
    xof_context xof_ctx;
    // -------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    b_time = x86_64_rtdsc();
#endif

    // row 1-2
    // -------------------------------------------------------------------------
    /* Parse the secret key */

    memcpy(mseed_eq, &sk[0], 2 * MQOM2_PARAM_SEED_SIZE);
    const uint8_t* pk = &sk[0];
    field_base_parse(&sk[(2 * MQOM2_PARAM_SEED_SIZE) + BYTE_SIZE_FIELD_EXT(MQOM2_PARAM_MQ_M/MQOM2_PARAM_MU)], MQOM2_PARAM_MQ_N, x);

    // -------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    c_time = x86_64_rtdsc();
#endif

    // row 5
    // -------------------------------------------------------------------------
    /* Hash message */

    ret = xof_init(&xof_ctx); ERR(ret, err);
    ret = xof_update(&xof_ctx, (const uint8_t*) "\x02", 1); ERR(ret, err);
    ret = xof_update(&xof_ctx, msg, mlen); ERR(ret, err);
    ret = xof_squeeze(&xof_ctx, msg_hash, MQOM2_PARAM_DIGEST_SIZE); ERR(ret, err);
    // -------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    d_time = x86_64_rtdsc();
#endif

    // signature preparation
    // -------------------------------------------------------------------------
    /* Prepare the signature */
    unsigned int pos = 0;
    memcpy(&sig[pos], salt, MQOM2_PARAM_SALT_SIZE); pos += MQOM2_PARAM_SALT_SIZE;
    uint8_t *com1 = &sig[pos]; pos += MQOM2_PARAM_DIGEST_SIZE;
    uint8_t *com2 = &sig[pos]; pos += MQOM2_PARAM_DIGEST_SIZE;
    uint8_t *serialized_alpha1 = &sig[pos]; pos += MQOM2_PARAM_TAU*BYTE_SIZE_FIELD_BASE(MQOM2_PARAM_ETA*MQOM2_PARAM_MU);
    uint8_t *opening = &sig[pos];
    uint8_t *nonce = &sig[MQOM2_SIG_SIZE-4];
    // -------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    e_time = x86_64_rtdsc();
#endif

    // row 6
    // -------------------------------------------------------------------------
    /* Commit Lines */
    blc_key_t key;
    field_ext_elt x0[MQOM2_PARAM_TAU][FIELD_EXT_PACKING(MQOM2_PARAM_MQ_N)];
    field_ext_elt u0[MQOM2_PARAM_TAU][FIELD_EXT_PACKING(MQOM2_PARAM_ETA)];
    field_ext_elt u1[MQOM2_PARAM_TAU][FIELD_EXT_PACKING(MQOM2_PARAM_ETA)];
    __BENCHMARK_START__(BS_BLC_COMMIT);
    BLC_Commit(mseed, salt, x, com1, &key, x0, u0, u1);
    __BENCHMARK_STOP__(BS_BLC_COMMIT);
    // -------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    f_time = x86_64_rtdsc();
#endif

    // row 7
    // -------------------------------------------------------------------------
    /* Compute P_alpha */
    field_ext_elt alpha0[MQOM2_PARAM_TAU][FIELD_EXT_PACKING(MQOM2_PARAM_ETA)];
    field_ext_elt alpha1[MQOM2_PARAM_TAU][FIELD_EXT_PACKING(MQOM2_PARAM_ETA)];
    __BENCHMARK_START__(BS_PIOP_COMPUTE);
    ComputePAlpha(com1, x0, u0, u1, x, mseed_eq, alpha0, alpha1);
    __BENCHMARK_STOP__(BS_PIOP_COMPUTE);
    // -------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    g_time = x86_64_rtdsc();
#endif

    // row 8
    // -------------------------------------------------------------------------
    /* Hash P_alpha and compute Fiat-Shamir hash */
    ret = xof_init(&xof_ctx); ERR(ret, err);
    ret = xof_update(&xof_ctx, (const uint8_t*) "\x03", 1); ERR(ret, err);
    uint8_t alpha[BYTE_SIZE_FIELD_BASE(MQOM2_PARAM_ETA*MQOM2_PARAM_MU)];

    for(e=0; e<MQOM2_PARAM_TAU; e++) {
        field_ext_serialize(alpha0[e], MQOM2_PARAM_ETA, alpha);
	    ret = xof_update(&xof_ctx, alpha, BYTE_SIZE_FIELD_BASE(MQOM2_PARAM_ETA*MQOM2_PARAM_MU)); ERR(ret, err);
    } // end of loop for
    // -------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    h_time = x86_64_rtdsc();
#endif

    // row 8 continued
    // -------------------------------------------------------------------------
    for(e=0; e<MQOM2_PARAM_TAU; e++) {
        uint8_t* buffer = &serialized_alpha1[e*BYTE_SIZE_FIELD_BASE(MQOM2_PARAM_ETA*MQOM2_PARAM_MU)];
        field_ext_serialize(alpha1[e], MQOM2_PARAM_ETA, buffer);
	    ret = xof_update(&xof_ctx, buffer, BYTE_SIZE_FIELD_BASE(MQOM2_PARAM_ETA*MQOM2_PARAM_MU)); ERR(ret, err);
    } // end of loop for

    ret = xof_squeeze(&xof_ctx, com2, MQOM2_PARAM_DIGEST_SIZE); ERR(ret, err);

    // -------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    i_time = x86_64_rtdsc();
#endif

    // row 9
    // -------------------------------------------------------------------------
    /* Fiat-Shamir hash */
    ret = xof_init(&xof_ctx); ERR(ret, err);
    ret = xof_update(&xof_ctx, (const uint8_t*) "\x04", 1); ERR(ret, err);
    ret = xof_update(&xof_ctx, pk, MQOM2_PK_SIZE); ERR(ret, err);
    ret = xof_update(&xof_ctx, com1, MQOM2_PARAM_DIGEST_SIZE); ERR(ret, err);
    ret = xof_update(&xof_ctx, com2, MQOM2_PARAM_DIGEST_SIZE); ERR(ret, err);
    ret = xof_update(&xof_ctx, msg_hash, MQOM2_PARAM_DIGEST_SIZE); ERR(ret, err);
    ret = xof_squeeze(&xof_ctx, hash, MQOM2_PARAM_DIGEST_SIZE); ERR(ret, err);

    // -------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    j_time = x86_64_rtdsc();
#endif

    // row 10
    // -------------------------------------------------------------------------
    /* Sample Challenge */
    uint16_t i_star[MQOM2_PARAM_TAU];
    __BENCHMARK_START__(BS_SAMPLE_CHALLENGE);
    ret = SampleChallenge(hash, i_star, nonce);
    __BENCHMARK_STOP__(BS_SAMPLE_CHALLENGE);
    // -------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    k_time = x86_64_rtdsc();
#endif

    // row 11
    // -------------------------------------------------------------------------
    /* Open Line Evaluation */
    __BENCHMARK_START__(BS_BLC_OPEN);
    BLC_Open(&key, i_star, opening);
    __BENCHMARK_STOP__(BS_BLC_OPEN);
    // -------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    l_time = x86_64_rtdsc();
#endif

    // return success
    // -------------------------------------------------------------------------

    ret = 0;
err:

    // -------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    m_time = x86_64_rtdsc();

    uint64_t t[] = {
        a_time,
        b_time,
        c_time,
        d_time,
        e_time,
        f_time,
        g_time,
        h_time,
        i_time,
        j_time,
        k_time,
        l_time,
        m_time
    };

    printf("Sign cycles");

    for (uint64_t i = 1; i < sizeof(t)/sizeof(t[0]); i++) {
        printf(", %d", (int)(t[i] - t[i-1]));
    }

    printf("\n");
#endif

    return ret;
}

int crypto_sign_signature(uint8_t *sig,
                          unsigned long long *siglen,
                          const uint8_t *m,
                          unsigned long long mlen,
                          const uint8_t *sk)
{
    int ret = -1;

    // Sample mseed
    uint8_t mseed[MQOM2_PARAM_SEED_SIZE];
    ret = randombytes(mseed, MQOM2_PARAM_SEED_SIZE); ERR(ret, err);

    // Sample salt
    uint8_t salt[MQOM2_PARAM_SALT_SIZE];
    ret = randombytes(salt, MQOM2_PARAM_SALT_SIZE); ERR(ret, err);

    // Build the signature
    ret = Sign(sk, m, mlen, salt, mseed, sig); ERR(ret, err);
    if(siglen != NULL) {
        *siglen = (unsigned long long) MQOM2_SIG_SIZE;
    }

    ret = 0;
err:
    return ret;
}

int crypto_sign(
        unsigned char *sm, unsigned long long *smlen,
        const unsigned char *m, unsigned long long mlen,
        const unsigned char *sk)
{
    int ret = -1;

    uint8_t* message = sm;
    memmove(message, m, mlen);
    ret = crypto_sign_signature(sm+mlen, smlen, message, mlen, sk); ERR(ret, err);
    if(smlen != NULL) {
        *smlen += mlen;
    }

    ret = 0;
err:
    return ret;
}

int Verify(const uint8_t pk[MQOM2_PK_SIZE], const uint8_t *msg, unsigned long long mlen, const uint8_t sig[MQOM2_SIG_SIZE]) {

#if defined(CYCLE_MAIN)
    uint64_t a_time = 0;
    uint64_t b_time = 0;
    uint64_t c_time = 0;
    uint64_t d_time = 0;
    uint64_t e_time = 0;
    uint64_t f_time = 0;
    uint64_t g_time = 0;
    uint64_t h_time = 0;
    uint64_t i_time = 0;
    uint64_t j_time = 0;
    uint64_t k_time = 0;
    uint64_t l_time = 0;
    uint64_t m_time = 0;

    a_time = x86_64_rtdsc();
#endif
    // declaration
    //--------------------------------------------------------------------------------
    int ret = -1;
    int e;
    uint8_t mseed_eq[2 * MQOM2_PARAM_SEED_SIZE];
    uint8_t msg_hash[MQOM2_PARAM_DIGEST_SIZE], hash[MQOM2_PARAM_DIGEST_SIZE], com2_[MQOM2_PARAM_DIGEST_SIZE];
    field_ext_elt y[FIELD_EXT_PACKING(MQOM2_PARAM_MQ_M/MQOM2_PARAM_MU)];
    xof_context xof_ctx;
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    b_time = x86_64_rtdsc();
#endif

    // row 1
    //--------------------------------------------------------------------------------
    /* Parse the public key */
    memcpy(mseed_eq, &pk[0], 2 * MQOM2_PARAM_SEED_SIZE);
    field_ext_parse(&pk[2 * MQOM2_PARAM_SEED_SIZE], MQOM2_PARAM_MQ_M/MQOM2_PARAM_MU, y);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    c_time = x86_64_rtdsc();
#endif

    // row 2
    //--------------------------------------------------------------------------------
    /* Parse the signature */
    unsigned int pos = 0;
    const uint8_t *salt = &sig[pos]; pos += MQOM2_PARAM_SALT_SIZE;
    const uint8_t *com1 = &sig[pos]; pos += MQOM2_PARAM_DIGEST_SIZE;
    const uint8_t *com2 = &sig[pos]; pos += MQOM2_PARAM_DIGEST_SIZE;
    const uint8_t *serialized_alpha1 = &sig[pos]; pos += MQOM2_PARAM_TAU*BYTE_SIZE_FIELD_BASE(MQOM2_PARAM_ETA*MQOM2_PARAM_MU);
    const uint8_t *opening = &sig[pos];
    const uint8_t *nonce = &sig[MQOM2_SIG_SIZE-4];
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    d_time = x86_64_rtdsc();
#endif

    // row 3
    //--------------------------------------------------------------------------------
    /* Hash message */
    ret = xof_init(&xof_ctx); ERR(ret, err);
    ret = xof_update(&xof_ctx, (const uint8_t*) "\x02", 1); ERR(ret, err);
    ret = xof_update(&xof_ctx, msg, mlen); ERR(ret, err);
    ret = xof_squeeze(&xof_ctx, msg_hash, MQOM2_PARAM_DIGEST_SIZE); ERR(ret, err);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    e_time = x86_64_rtdsc();
#endif

    // row 4
    //--------------------------------------------------------------------------------
    /* Compute Fiat-Shamir hash */
    ret = xof_init(&xof_ctx); ERR(ret, err);
    ret = xof_update(&xof_ctx, (const uint8_t*) "\x04", 1); ERR(ret, err);
    ret = xof_update(&xof_ctx, pk, MQOM2_PK_SIZE); ERR(ret, err);
    ret = xof_update(&xof_ctx, com1, MQOM2_PARAM_DIGEST_SIZE); ERR(ret, err);
    ret = xof_update(&xof_ctx, com2, MQOM2_PARAM_DIGEST_SIZE); ERR(ret, err);
    ret = xof_update(&xof_ctx, msg_hash, MQOM2_PARAM_DIGEST_SIZE); ERR(ret, err);
    ret = xof_squeeze(&xof_ctx, hash, MQOM2_PARAM_DIGEST_SIZE); ERR(ret, err);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    f_time = x86_64_rtdsc();
#endif

    // row 5
    //--------------------------------------------------------------------------------
    /* Sample Challenge */
    uint16_t i_star[MQOM2_PARAM_TAU];
    uint8_t tmp[MQOM2_PARAM_TAU*2+2];
    ret = xof_init(&xof_ctx); ERR(ret, err);
    ret = xof_update(&xof_ctx, (const uint8_t*) "\x05", 1); ERR(ret, err);
    ret = xof_update(&xof_ctx, hash, MQOM2_PARAM_DIGEST_SIZE); ERR(ret, err);
    ret = xof_update(&xof_ctx, nonce, 4); ERR(ret, err);
    ret = xof_squeeze(&xof_ctx, tmp, MQOM2_PARAM_TAU*2+2); ERR(ret, err);

    for(e = 0; e<MQOM2_PARAM_TAU; e++) {
        i_star[e] = (tmp[2*e] + 256*tmp[2*e+1]) & ((1<<MQOM2_PARAM_NB_EVALS_LOG)-1);
    }

    uint16_t val = (tmp[2*MQOM2_PARAM_TAU] + tmp[2*MQOM2_PARAM_TAU+1]*256) & ((1<<MQOM2_PARAM_W)-1);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    g_time = x86_64_rtdsc();
#endif

    // row 6
    //--------------------------------------------------------------------------------
    if(val != 0) {
        ret = -1;
        goto err;
    }
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    h_time = x86_64_rtdsc();
#endif

    // row 7
    //--------------------------------------------------------------------------------
    /* Get Opened Evaluations */
    field_ext_elt x_eval[MQOM2_PARAM_TAU][FIELD_EXT_PACKING(MQOM2_PARAM_MQ_N)];
    field_ext_elt u_eval[MQOM2_PARAM_TAU][FIELD_EXT_PACKING(MQOM2_PARAM_ETA)];
    ret = BLC_Eval(salt, com1, opening, i_star, x_eval, u_eval); ERR(ret, err);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    i_time = x86_64_rtdsc();
#endif

    // row 8 and 9
    //--------------------------------------------------------------------------------
    /* Recompute P_alpha */
    field_ext_elt alpha0[MQOM2_PARAM_TAU][FIELD_EXT_PACKING(MQOM2_PARAM_ETA)];
    field_ext_elt alpha1[MQOM2_PARAM_TAU][FIELD_EXT_PACKING(MQOM2_PARAM_ETA)];

    for(e=0; e<MQOM2_PARAM_TAU; e++) {
         field_ext_parse(&serialized_alpha1[e*BYTE_SIZE_FIELD_BASE(MQOM2_PARAM_ETA*MQOM2_PARAM_MU)], MQOM2_PARAM_ETA, alpha1[e]);
    }

    ret = RecomputePAlpha(com1, alpha1, i_star, x_eval, u_eval, mseed_eq, y, alpha0); ERR(ret, err);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    j_time = x86_64_rtdsc();
#endif

    // row 10
    //--------------------------------------------------------------------------------
    /* Hash P_alpha */
    ret = xof_init(&xof_ctx); ERR(ret, err);
    ret = xof_update(&xof_ctx, (const uint8_t*) "\x03", 1); ERR(ret, err);
    uint8_t alpha[BYTE_SIZE_FIELD_BASE(MQOM2_PARAM_ETA*MQOM2_PARAM_MU)];

    for(e=0; e<MQOM2_PARAM_TAU; e++) {
        field_ext_serialize(alpha0[e], MQOM2_PARAM_ETA, alpha);
	    ret = xof_update(&xof_ctx, alpha, BYTE_SIZE_FIELD_BASE(MQOM2_PARAM_ETA*MQOM2_PARAM_MU)); ERR(ret, err);
    }

    ret = xof_update(&xof_ctx, serialized_alpha1, MQOM2_PARAM_TAU*BYTE_SIZE_FIELD_BASE(MQOM2_PARAM_ETA*MQOM2_PARAM_MU)); ERR(ret, err);
    ret = xof_squeeze(&xof_ctx, com2_, MQOM2_PARAM_DIGEST_SIZE); ERR(ret, err);
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    k_time = x86_64_rtdsc();
#endif

    // row 11
    //--------------------------------------------------------------------------------
    if(memcmp(com2, com2_, MQOM2_PARAM_DIGEST_SIZE)) {
        ret = -1;
        goto err;
    }
    //--------------------------------------------------------------------------------

#if defined(CYCLE_MAIN)
    l_time = x86_64_rtdsc();
#endif

    // row 12
    //--------------------------------------------------------------------------------
    ret = 0;
    //--------------------------------------------------------------------------------
    
err:

#if defined(CYCLE_MAIN)
    m_time = x86_64_rtdsc();

    uint64_t t[] = {
        a_time,b_time,c_time,d_time,e_time,
        f_time,g_time,h_time,i_time,j_time,k_time,l_time,m_time
    };

    printf("Verif cycles");

    for (uint64_t i = 1; i < sizeof(t)/sizeof(t[0]); i++) {
        printf(", %d", (int)(t[i] - t[i-1]));
    }

    printf("\n");
#endif

    return ret;
}

int crypto_sign_verify(const uint8_t *sig,
                       unsigned long long siglen,
                       const uint8_t *m,
                       unsigned long long mlen,
                       const uint8_t *pk)
{
    if(siglen != (unsigned long long) MQOM2_SIG_SIZE)
        return -1;

    return Verify(pk, m, mlen, sig);
}

int crypto_sign_open(
        unsigned char *m, unsigned long long *mlen,
        const unsigned char *sm, unsigned long long smlen,
        const unsigned char *pk) {

    if(smlen < (unsigned long long) MQOM2_SIG_SIZE)
        return -1;

    const uint8_t* message = sm;
    *mlen = smlen-(unsigned long long) MQOM2_SIG_SIZE;

    int ret = crypto_sign_verify(sm+(*mlen), MQOM2_SIG_SIZE, message, *mlen, pk);
    if (ret) {
        return ret;
    }

    memmove(m, message, *mlen);
    return 0;
}
