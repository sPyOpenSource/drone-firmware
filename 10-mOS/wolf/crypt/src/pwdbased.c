/* pwdbased.c
 *
 * Copyright (C) 2006-2017 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolf/crypt/headers/settings.h>

#ifndef NO_PWDBASED

#include <wolf/crypt/headers/pwdbased.h>
#include <wolf/crypt/headers/hmac.h>
#include <wolf/crypt/headers/hash.h>
#include <wolf/crypt/headers/integer.h>
#include <wolf/crypt/headers/error-crypt.h>

#ifdef NO_INLINE
    #include <wolf/crypt/headers/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolf/crypt/src/misc.c>
#endif


/* PKCS#5 v1.5 with non standard extension to optionally derive the extra data (IV) */
int wc_PBKDF1_ex(byte* key, int keyLen, byte* iv, int ivLen,
    const byte* passwd, int passwdLen, const byte* salt, int saltLen,
    int iterations, int hashType, void* heap)
{
    int  err;
    int  keyLeft, ivLeft, i;
    int  digestLeft, store;
    int  keyOutput = 0;
    int  diestLen;
    byte digest[WC_MAX_DIGEST_SIZE];
#ifdef WOLFSSL_SMALL_STACK
    wc_HashAlg* hash = NULL;
#else
    wc_HashAlg  hash[1];
#endif
    enum wc_HashType hashT;

    (void)heap;

    if (key == NULL || keyLen < 0 || passwdLen < 0 || saltLen < 0 || ivLen < 0){
        return BAD_FUNC_ARG;
    }

    if (iterations <= 0)
        iterations = 1;

    hashT = wc_HashTypeConvert(hashType);
    err = wc_HashGetDigestSize(hashT);
    if (err < 0)
        return err;
    diestLen = err;

    /* initialize hash */
#ifdef WOLFSSL_SMALL_STACK
    hash = (wc_HashAlg*)XMALLOC(sizeof(wc_HashAlg), heap,
                                DYNAMIC_TYPE_HASHCTX);
    if (hash == NULL)
        return MEMORY_E;
#endif

    err = wc_HashInit(hash, hashT);
    if (err != 0) {
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(hash, heap, DYNAMIC_TYPE_HASHCTX);
    #endif
        return err;
    }

    keyLeft = keyLen;
    ivLeft  = ivLen;
    while (keyOutput < (keyLen + ivLen)) {
        digestLeft = diestLen;
        /* D_(i - 1) */
        if (keyOutput) { /* first time D_0 is empty */
            err = wc_HashUpdate(hash, hashT, digest, diestLen);
            if (err != 0) break;
        }

        /* data */
        err = wc_HashUpdate(hash, hashT, passwd, passwdLen);
        if (err != 0) break;

        /* salt */
        if (salt) {
            err = wc_HashUpdate(hash, hashT, salt, saltLen);
            if (err != 0) break;
        }

        err = wc_HashFinal(hash, hashT, digest);
        if (err != 0) break;

        /* count */
        for (i = 1; i < iterations; i++) {
            err = wc_HashUpdate(hash, hashT, digest, diestLen);
            if (err != 0) break;

            err = wc_HashFinal(hash, hashT, digest);
            if (err != 0) break;
        }

        if (keyLeft) {
            store = min(keyLeft, diestLen);
            XMEMCPY(&key[keyLen - keyLeft], digest, store);

            keyOutput  += store;
            keyLeft    -= store;
            digestLeft -= store;
        }

        if (ivLeft && digestLeft) {
            store = min(ivLeft, digestLeft);
            if (iv != NULL)
                XMEMCPY(&iv[ivLen - ivLeft],
                        &digest[diestLen - digestLeft], store);
            keyOutput += store;
            ivLeft    -= store;
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(hash, heap, DYNAMIC_TYPE_HASHCTX);
#endif

    if (err != 0)
        return err;

    if (keyOutput != (keyLen + ivLen))
        return BUFFER_E;

    return err;
}

/* PKCS#5 v1.5 */
int wc_PBKDF1(byte* output, const byte* passwd, int pLen, const byte* salt,
           int sLen, int iterations, int kLen, int hashType)
{
    return wc_PBKDF1_ex(output, kLen, NULL, 0,
        passwd, pLen, salt, sLen, iterations, hashType, NULL);
}


int wc_PBKDF2(byte* output, const byte* passwd, int pLen, const byte* salt,
           int sLen, int iterations, int kLen, int hashType)
{
    word32 i = 1;
    int    hLen;
    int    j, ret;
#ifdef WOLFSSL_SMALL_STACK
    byte*  buffer;
    Hmac*  hmac;
#else
    byte   buffer[WC_MAX_DIGEST_SIZE];
    Hmac   hmac[1];
#endif
    enum wc_HashType hashT;

    if (output == NULL || pLen < 0 || sLen < 0 || kLen < 0) {
        return BAD_FUNC_ARG;
    }

    if (iterations <= 0)
        iterations = 1;

    hashT = wc_HashTypeConvert(hashType);
    hLen = wc_HashGetDigestSize(hashT);
    if (hLen < 0)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_SMALL_STACK
    buffer = (byte*)XMALLOC(WC_MAX_DIGEST_SIZE, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (buffer == NULL)
        return MEMORY_E;
    hmac = (Hmac*)XMALLOC(sizeof(Hmac), NULL, DYNAMIC_TYPE_HMAC);
    if (buffer == NULL)
        return MEMORY_E;
#endif

    ret = wc_HmacInit(hmac, NULL, INVALID_DEVID);
    if (ret == 0) {
        /* use int hashType here, since HMAC FIPS uses the old unique value */
        ret = wc_HmacSetKey(hmac, hashType, passwd, pLen);

        while (ret == 0 && kLen) {
            int currentLen;

            ret = wc_HmacUpdate(hmac, salt, sLen);
            if (ret != 0)
                break;

            /* encode i */
            for (j = 0; j < 4; j++) {
                byte b = (byte)(i >> ((3-j) * 8));

                ret = wc_HmacUpdate(hmac, &b, 1);
                if (ret != 0)
                    break;
            }

            /* check ret from inside for loop */
            if (ret != 0)
                break;

            ret = wc_HmacFinal(hmac, buffer);
            if (ret != 0)
                break;

            currentLen = min(kLen, hLen);
            XMEMCPY(output, buffer, currentLen);

            for (j = 1; j < iterations; j++) {
                ret = wc_HmacUpdate(hmac, buffer, hLen);
                if (ret != 0)
                    break;
                ret = wc_HmacFinal(hmac, buffer);
                if (ret != 0)
                    break;
                xorbuf(output, buffer, currentLen);
            }

            /* check ret from inside for loop */
            if (ret != 0)
                break;

            output += currentLen;
            kLen   -= currentLen;
            i++;
        }
        wc_HmacFree(hmac);
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(buffer, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(hmac, NULL, DYNAMIC_TYPE_HMAC);
#endif

    return ret;
}

/* helper for PKCS12_PBKDF(), does hash operation */
static int DoPKCS12Hash(int hashType, byte* buffer, word32 totalLen,
                 byte* Ai, word32 u, int iterations)
{
    int i;
    int ret = 0;
#ifdef WOLFSSL_SMALL_STACK
    wc_HashAlg* hash = NULL;
#else
    wc_HashAlg  hash[1];
#endif
    enum wc_HashType hashT;

    if (buffer == NULL || Ai == NULL) {
        return BAD_FUNC_ARG;
    }

    hashT = wc_HashTypeConvert(hashType);

    /* initialize hash */
#ifdef WOLFSSL_SMALL_STACK
    hash = (wc_HashAlg*)XMALLOC(sizeof(wc_HashAlg), NULL,
                                DYNAMIC_TYPE_HASHCTX);
    if (hash == NULL)
        return MEMORY_E;
#endif

    ret = wc_HashInit(hash, hashT);
    if (ret != 0) {
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(hash, NULL, DYNAMIC_TYPE_HASHCTX);
    #endif
        return ret;
    }

    ret = wc_HashUpdate(hash, hashT, buffer, totalLen);

    if (ret == 0)
        ret = wc_HashFinal(hash, hashT, Ai);

    for (i = 1; i < iterations; i++) {
        if (ret == 0)
            ret = wc_HashUpdate(hash, hashT, Ai, u);
        if (ret == 0)
            ret = wc_HashFinal(hash, hashT, Ai);
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(hash, NULL, DYNAMIC_TYPE_HASHCTX);
#endif

    return ret;
}


int wc_PKCS12_PBKDF(byte* output, const byte* passwd, int passLen,
    const byte* salt, int saltLen, int iterations, int kLen, int hashType,
    int id)
{
    return wc_PKCS12_PBKDF_ex(output, passwd, passLen, salt, saltLen,
                              iterations, kLen, hashType, id, NULL);
}


/* extended API that allows a heap hint to be used */
int wc_PKCS12_PBKDF_ex(byte* output, const byte* passwd, int passLen,
                       const byte* salt, int saltLen, int iterations, int kLen,
                       int hashType, int id, void* heap)
{
    /* all in bytes instead of bits */
    word32 u, v, dLen, pLen, iLen, sLen, totalLen;
    int    dynamic = 0;
    int    ret = 0;
    int    i;
    byte   *D, *S, *P, *I;
#ifdef WOLFSSL_SMALL_STACK
    byte   staticBuffer[1]; /* force dynamic usage */
#else
    byte   staticBuffer[1024];
#endif
    byte*  buffer = staticBuffer;

#ifdef WOLFSSL_SMALL_STACK
    byte*  Ai;
    byte*  B;
#else
    byte   Ai[WC_MAX_DIGEST_SIZE];
    byte   B[WC_MAX_BLOCK_SIZE];
#endif
    enum wc_HashType hashT;

    (void)heap;

    if (output == NULL || passLen < 0 || saltLen < 0 || kLen < 0) {
        return BAD_FUNC_ARG;
    }

    if (iterations <= 0)
        iterations = 1;

    hashT = wc_HashTypeConvert(hashType);
    ret = wc_HashGetDigestSize(hashT);
    if (ret < 0)
        return ret;
    u = ret;

    ret = wc_HashGetBlockSize(hashT);
    if (ret < 0)
        return ret;
    v = ret;

#ifdef WOLFSSL_SMALL_STACK
    Ai = (byte*)XMALLOC(WC_MAX_DIGEST_SIZE, heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (Ai == NULL)
        return MEMORY_E;

    B = (byte*)XMALLOC(WC_MAX_BLOCK_SIZE, heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (B == NULL) {
        XFREE(Ai, heap, DYNAMIC_TYPE_TMP_BUFFER);
        return MEMORY_E;
    }
#endif

    XMEMSET(Ai, 0, WC_MAX_DIGEST_SIZE);
    XMEMSET(B,  0, WC_MAX_BLOCK_SIZE);

    dLen = v;
    sLen = v * ((saltLen + v - 1) / v);
    if (passLen)
        pLen = v * ((passLen + v - 1) / v);
    else
        pLen = 0;
    iLen = sLen + pLen;

    totalLen = dLen + sLen + pLen;

    if (totalLen > sizeof(staticBuffer)) {
        buffer = (byte*)XMALLOC(totalLen, heap, DYNAMIC_TYPE_KEY);
        if (buffer == NULL) {
#ifdef WOLFSSL_SMALL_STACK
            XFREE(Ai, heap, DYNAMIC_TYPE_TMP_BUFFER);
            XFREE(B,  heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif
            return MEMORY_E;
        }
        dynamic = 1;
    }

    D = buffer;
    S = D + dLen;
    P = S + sLen;
    I = S;

    XMEMSET(D, id, dLen);

    for (i = 0; i < (int)sLen; i++)
        S[i] = salt[i % saltLen];
    for (i = 0; i < (int)pLen; i++)
        P[i] = passwd[i % passLen];

    while (kLen > 0) {
        word32 currentLen;
        mp_int B1;

        ret = DoPKCS12Hash(hashType, buffer, totalLen, Ai, u, iterations);
        if (ret < 0)
            break;

        for (i = 0; i < (int)v; i++)
            B[i] = Ai[i % u];

        if (mp_init(&B1) != MP_OKAY)
            ret = MP_INIT_E;
        else if (mp_read_unsigned_bin(&B1, B, v) != MP_OKAY)
            ret = MP_READ_E;
        else if (mp_add_d(&B1, (mp_digit)1, &B1) != MP_OKAY)
            ret = MP_ADD_E;

        if (ret != 0) {
            mp_clear(&B1);
            break;
        }

        for (i = 0; i < (int)iLen; i += v) {
            int    outSz;
            mp_int i1;
            mp_int res;

            if (mp_init_multi(&i1, &res, NULL, NULL, NULL, NULL) != MP_OKAY) {
                ret = MP_INIT_E;
                break;
            }
            if (mp_read_unsigned_bin(&i1, I + i, v) != MP_OKAY)
                ret = MP_READ_E;
            else if (mp_add(&i1, &B1, &res) != MP_OKAY)
                ret = MP_ADD_E;
            else if ( (outSz = mp_unsigned_bin_size(&res)) < 0)
                ret = MP_TO_E;
            else {
                if (outSz > (int)v) {
                    /* take off MSB */
                    byte  tmp[129];
                    ret = mp_to_unsigned_bin(&res, tmp);
                    XMEMCPY(I + i, tmp + 1, v);
                }
                else if (outSz < (int)v) {
                    XMEMSET(I + i, 0, v - outSz);
                    ret = mp_to_unsigned_bin(&res, I + i + v - outSz);
                }
                else
                    ret = mp_to_unsigned_bin(&res, I + i);
            }

            mp_clear(&i1);
            mp_clear(&res);
            if (ret < 0) break;
        }

        currentLen = min(kLen, (int)u);
        XMEMCPY(output, Ai, currentLen);
        output += currentLen;
        kLen   -= currentLen;
        mp_clear(&B1);
    }

    if (dynamic) XFREE(buffer, heap, DYNAMIC_TYPE_KEY);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(Ai, heap, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(B,  heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}

#ifdef HAVE_SCRYPT
/* Rotate the 32-bit value a by b bits to the left.
 *
 * a  32-bit value.
 * b  Number of bits to rotate.
 * returns rotated value.
 */
#define R(a, b) rotlFixed(a, b)

/* One round of Salsa20/8.
 * Code taken from RFC 7914: scrypt PBKDF.
 *
 * out  Output buffer.
 * in   Input data to hash.
 */
static void scryptSalsa(word32* out, word32* in)
{
    int    i;
    word32 x[16];

#ifdef LITTLE_ENDIAN_ORDER
    for (i = 0; i < 16; ++i)
        x[i] = in[i];
#else
    for (i = 0; i < 16; i++)
        x[i] = ByteReverseWord32(in[i]);
#endif
    for (i = 8; i > 0; i -= 2) {
        x[ 4] ^= R(x[ 0] + x[12],  7);  x[ 8] ^= R(x[ 4] + x[ 0],  9);
        x[12] ^= R(x[ 8] + x[ 4], 13);  x[ 0] ^= R(x[12] + x[ 8], 18);
        x[ 9] ^= R(x[ 5] + x[ 1],  7);  x[13] ^= R(x[ 9] + x[ 5],  9);
        x[ 1] ^= R(x[13] + x[ 9], 13);  x[ 5] ^= R(x[ 1] + x[13], 18);
        x[14] ^= R(x[10] + x[ 6],  7);  x[ 2] ^= R(x[14] + x[10],  9);
        x[ 6] ^= R(x[ 2] + x[14], 13);  x[10] ^= R(x[ 6] + x[ 2], 18);
        x[ 3] ^= R(x[15] + x[11],  7);  x[ 7] ^= R(x[ 3] + x[15],  9);
        x[11] ^= R(x[ 7] + x[ 3], 13);  x[15] ^= R(x[11] + x[ 7], 18);
        x[ 1] ^= R(x[ 0] + x[ 3],  7);  x[ 2] ^= R(x[ 1] + x[ 0],  9);
        x[ 3] ^= R(x[ 2] + x[ 1], 13);  x[ 0] ^= R(x[ 3] + x[ 2], 18);
        x[ 6] ^= R(x[ 5] + x[ 4],  7);  x[ 7] ^= R(x[ 6] + x[ 5],  9);
        x[ 4] ^= R(x[ 7] + x[ 6], 13);  x[ 5] ^= R(x[ 4] + x[ 7], 18);
        x[11] ^= R(x[10] + x[ 9],  7);  x[ 8] ^= R(x[11] + x[10],  9);
        x[ 9] ^= R(x[ 8] + x[11], 13);  x[10] ^= R(x[ 9] + x[ 8], 18);
        x[12] ^= R(x[15] + x[14],  7);  x[13] ^= R(x[12] + x[15],  9);
        x[14] ^= R(x[13] + x[12], 13);  x[15] ^= R(x[14] + x[13], 18);
    }
#ifdef LITTLE_ENDIAN_ORDER
    for (i = 0; i < 16; ++i)
        out[i] = in[i] + x[i];
#else
    for (i = 0; i < 16; i++)
        out[i] = ByteReverseWord32(ByteReverseWord32(in[i]) + x[i]);
#endif
}

/* Mix a block using Salsa20/8.
 * Based on RFC 7914: scrypt PBKDF.
 *
 * b  Blocks to mix.
 * y  Temporary storage.
 * r  Size of the block.
 */
static void scryptBlockMix(byte* b, byte* y, int r)
{
    byte x[64];
#ifdef WORD64_AVAILABLE
    word64* b64 = (word64*)b;
    word64* y64 = (word64*)y;
    word64* x64 = (word64*)x;
#else
    word32* b32 = (word32*)b;
    word32* y32 = (word32*)y;
    word32* x32 = (word32*)x;
#endif
    int  i;
    int  j;

    /* Step 1. */
    XMEMCPY(x, b + (2 * r - 1) * 64, sizeof(x));
    /* Step 2. */
    for (i = 0; i < 2 * r; i++)
    {
#ifdef WORD64_AVAILABLE
        for (j = 0; j < 8; j++)
            x64[j] ^= b64[i * 8 + j];
#else
        for (j = 0; j < 16; j++)
            x32[j] ^= b32[i * 16 + j];
#endif
        scryptSalsa((word32*)x, (word32*)x);
        XMEMCPY(y + i * 64, x, sizeof(x));
    }
    /* Step 3. */
    for (i = 0; i < r; i++) {
#ifdef WORD64_AVAILABLE
        for (j = 0; j < 8; j++) {
            b64[i * 8 + j] = y64[2 * i * 8 + j];
            b64[(r + i) * 8 + j] = y64[(2 * i + 1) * 8 + j];
        }
#else
        for (j = 0; j < 16; j++) {
            b32[i * 16 + j] = y32[2 * i * 16 + j];
            b32[(r + i) * 16 + j] = y32[(2 * i + 1) * 16 + j];
        }
#endif
    }
}

/* Random oracles mix.
 * Based on RFC 7914: scrypt PBKDF.
 *
 * x  Data to mix.
 * v  Temporary buffer.
 * y  Temporary buffer for the block mix.
 * r  Block size parameter.
 * n  CPU/Memory cost parameter.
 */
static void scryptROMix(byte* x, byte* v, byte* y, int r, word32 n)
{
    word32 i;
    word32 j;
    word32 k;
    word32 bSz = 128 * r;
#ifdef WORD64_AVAILABLE
    word64* x64 = (word64*)x;
    word64* v64 = (word64*)v;
#else
    word32* x32 = (word32*)x;
    word32* v32 = (word32*)v;
#endif

    /* Step 1. X = B (B not needed therefore not implemented) */
    /* Step 2. */
    for (i = 0; i < n; i++)
    {
        XMEMCPY(v + i * bSz, x, bSz);
        scryptBlockMix(x, y, r);
    }

    /* Step 3. */
    for (i = 0; i < n; i++)
    {
#ifdef LITTLE_ENDIAN_ORDER
#ifdef WORD64_AVAILABLE
        j = *(word64*)(x + (2*r - 1) * 64) & (n-1);
#else
        j = *(word32*)(x + (2*r - 1) * 64) & (n-1);
#endif
#else
        byte* t = x + (2*r - 1) * 64;
        j = (t[0] | (t[1] << 8) | (t[2] << 16) | ((word32)t[3] << 24)) & (n-1);
#endif
#ifdef WORD64_AVAILABLE
        for (k = 0; k < bSz / 8; k++)
            x64[k] ^= v64[j * bSz / 8 + k];
#else
        for (k = 0; k < bSz / 4; k++)
            x32[k] ^= v32[j * bSz / 4 + k];
#endif
        scryptBlockMix(x, y, r);
    }
    /* Step 4. B' = X (B = X = B' so not needed, therefore not implemented) */
}

/* Generates an key derived from a password and salt using a memory hard
 * algorithm.
 * Implements RFC 7914: scrypt PBKDF.
 *
 * output     The derived key.
 * passwd     The password to derive key from.
 * passLen    The length of the password.
 * salt       The key specific data.
 * saltLen    The length of the salt data.
 * cost       The CPU/memory cost parameter. Range: 1..(128*r/8-1)
 *            (Iterations = 2^cost)
 * blockSize  The number of 128 byte octets in a working block.
 * parallel   The number of parallel mix operations to perform.
 *            (Note: this implementation does not use threads.)
 * dkLen      The length of the derived key in bytes.
 * returns BAD_FUNC_ARG when: parallel not 1, blockSize is too large for cost.
 */
int wc_scrypt(byte* output, const byte* passwd, int passLen,
              const byte* salt, int saltLen, int cost, int blockSize,
              int parallel, int dkLen)
{
    int    ret = 0;
    int    i;
    byte*  v = NULL;
    byte*  y = NULL;
    byte*  blocks = NULL;
    word32 blocksSz;
    word32 bSz;

    if (blockSize > 8)
        return BAD_FUNC_ARG;

    if (cost < 1 || cost >= 128 * blockSize / 8)
        return BAD_FUNC_ARG;

    bSz = 128 * blockSize;
    blocksSz = bSz * parallel;
    blocks = (byte*)XMALLOC(blocksSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (blocks == NULL)
        goto end;
    /* Temporary for scryptROMix. */
    v = (byte*)XMALLOC((1 << cost) * bSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (v == NULL)
        goto end;
    /* Temporary for scryptBlockMix. */
    y = (byte*)XMALLOC(blockSize * 128, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (y == NULL)
        goto end;

    /* Step 1. */
    ret = wc_PBKDF2(blocks, passwd, passLen, salt, saltLen, 1, blocksSz,
                    WC_SHA256);
    if (ret != 0)
        goto end;

    /* Step 2. */
    for (i = 0; i < parallel; i++)
        scryptROMix(blocks + i * bSz, v, y, blockSize, 1 << cost);

    /* Step 3. */
    ret = wc_PBKDF2(output, passwd, passLen, blocks, blocksSz, 1, dkLen,
                    WC_SHA256);
end:
    if (blocks != NULL)
        XFREE(blocks, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (v != NULL)
        XFREE(v, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (y != NULL)
        XFREE(y, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    return ret;
}
#endif

#undef WC_MAX_DIGEST_SIZE

#endif /* NO_PWDBASED */


