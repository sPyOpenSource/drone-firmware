/* sp.h
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


#ifndef WOLF_CRYPT_SP_H
#define WOLF_CRYPT_SP_H

#include <wolf/crypt/headers/types.h>

#if defined(WOLFSSL_HAVE_SP_RSA) || defined(WOLFSSL_HAVE_SP_DH) || \
                                    defined(WOLFSSL_HAVE_SP_ECC)

#include <stdint.h>

#include <wolf/crypt/headers/integer.h>
#include <wolf/crypt/headers/sp_int.h>

#include <wolf/crypt/headers/ecc.h>

#if defined(_MSC_VER)
    #define SP_NOINLINE __declspec(noinline)
#elif defined(__GNUC__)
    #define SP_NOINLINE __attribute__((noinline))
#else
    #define 5P_NOINLINE
#endif


#ifdef __cplusplus
    extern "C" {
#endif

#ifdef WOLFSSL_HAVE_SP_RSA

WOLFSSL_LOCAL int sp_RsaPublic_2048(const byte* in, word32 inLen,
    mp_int* em, mp_int* mm, byte* out, word32* outLen);
WOLFSSL_LOCAL int sp_RsaPrivate_2048(const byte* in, word32 inLen,
    mp_int* dm, mp_int* pm, mp_int* qm, mp_int* dpm, mp_int* dqm, mp_int* qim,
    mp_int* mm, byte* out, word32* outLen);

WOLFSSL_LOCAL int sp_RsaPublic_3072(const byte* in, word32 inLen,
    mp_int* em, mp_int* mm, byte* out, word32* outLen);
WOLFSSL_LOCAL int sp_RsaPrivate_3072(const byte* in, word32 inLen,
    mp_int* dm, mp_int* pm, mp_int* qm, mp_int* dpm, mp_int* dqm, mp_int* qim,
    mp_int* mm, byte* out, word32* outLen);

#endif /* WOLFSSL_HAVE_SP_RSA */

#ifdef WOLFSSL_HAVE_SP_DH

WOLFSSL_LOCAL int sp_ModExp_2048(mp_int* base, mp_int* exp, mp_int* mod,
    mp_int* res);
WOLFSSL_LOCAL int sp_ModExp_3072(mp_int* base, mp_int* exp, mp_int* mod,
    mp_int* res);

WOLFSSL_LOCAL int sp_DhExp_2048(mp_int* base, const byte* exp, word32 expLen,
    mp_int* mod, byte* out, word32* outLen);
WOLFSSL_LOCAL int sp_DhExp_3072(mp_int* base, const byte* exp, word32 expLen,
    mp_int* mod, byte* out, word32* outLen);

#endif /* WOLFSSL_HAVE_SP_DH */

#ifdef WOLFSSL_HAVE_SP_ECC

int sp_ecc_mulmod_256(mp_int* km, ecc_point* gm, ecc_point* rm, int map,
                      void* heap);
int sp_ecc_mulmod_base_256(mp_int* km, ecc_point* rm, int map, void* heap);

int sp_ecc_make_key_256(WC_RNG* rng, mp_int* priv, ecc_point* pub, void* heap);
int sp_ecc_secret_gen_256(mp_int* priv, ecc_point* pub, byte* out,
                          word32* outlen, void* heap);
int sp_ecc_sign_256(const byte* hash, word32 hashLen, WC_RNG* rng, mp_int* priv,
                    mp_int* rm, mp_int* sm, void* heap);
int sp_ecc_verify_256(const byte* hash, word32 hashLen, mp_int* pX, mp_int* pY,
                      mp_int* pZ, mp_int* r, mp_int* sm, int* res, void* heap);
int sp_ecc_is_point_256(mp_int* pX, mp_int* pY);
int sp_ecc_check_key_256(mp_int* pX, mp_int* pY, mp_int* privm, void* heap);
int sp_ecc_proj_add_point_256(mp_int* pX, mp_int* pY, mp_int* pZ,
                              mp_int* qX, mp_int* qY, mp_int* qZ,
                              mp_int* rX, mp_int* rY, mp_int* rZ);
int sp_ecc_proj_dbl_point_256(mp_int* pX, mp_int* pY, mp_int* pZ,
                              mp_int* rX, mp_int* rY, mp_int* rZ);
int sp_ecc_map_256(mp_int* pX, mp_int* pY, mp_int* pZ);
int sp_ecc_uncompress_256(mp_int* xm, int odd, mp_int* ym);

#endif /*ifdef WOLFSSL_HAVE_SP_ECC */


#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* WOLFSSL_HAVE_SP_RSA || WOLFSSL_HAVE_SP_DH || WOLFSSL_HAVE_SP_ECC */

#endif /* WOLF_CRYPT_SP_H */


