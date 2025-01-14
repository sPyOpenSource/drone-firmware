/* keygen.c
 *
 * Copyright (C) 2014-2016 wolfSSL Inc.
 *
 * This file is part of wolfSSH.
 *
 * wolfSSH is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * wolfSSH is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with wolfSSH.  If not, see <http://www.gnu.org/licenses/>.
 */


/*
 * The keygen module contains utility functions wrapping the wolfCrypt
 * key generation functions to product SSH friendly keys.
 */


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolf/ssl/headers/options.h>
#include <wolf/crypt/headers/random.h>
#include <wolf/crypt/headers/rsa.h>
#include <wolf/ssh/headers/error.h>
#include <wolf/ssh/headers/keygen.h>
#include <wolf/ssh/headers/log.h>

#ifdef WOLFSSH_KEYGEN

#ifdef NO_INLINE
    #include <wolfssh/misc.h>
#else
    #define WOLFSSH_MISC_INCLUDED
    #include "src/misc.c"
#endif


int wolfSSH_MakeRsaKey(byte* out, word32 outSz,
                       word32 size, word32 e)
{
    int ret = WS_SUCCESS;
    WC_RNG rng;

    WLOG(WS_LOG_DEBUG, "Entering wolfSSH_MakeRsaKey()");

    if (wc_InitRng(&rng) != 0) {
        WLOG(WS_LOG_DEBUG, "Couldn't create RNG");
        ret = WS_CRYPTO_FAILED;
    }

    if (ret == WS_SUCCESS) {
        RsaKey key;

        if (wc_InitRsaKey(&key, NULL) != 0)
            ret = WS_CRYPTO_FAILED;

        if (ret == WS_SUCCESS) {
            if (wc_MakeRsaKey(&key, size, e, &rng) != 0) {
                WLOG(WS_LOG_DEBUG, "RSA key generation failed");
                ret = WS_CRYPTO_FAILED;
            }
        }

        if (ret == WS_SUCCESS) {
            int keySz;

            keySz = wc_RsaKeyToDer(&key, out, outSz);
            if (keySz < 0) {
                WLOG(WS_LOG_DEBUG, "RSA key to DER failed");
                ret = WS_CRYPTO_FAILED;
            }
            else
                ret = keySz;
        }

        if (wc_FreeRsaKey(&key) != 0) {
            WLOG(WS_LOG_DEBUG, "RSA key free failed");
            ret = WS_CRYPTO_FAILED;
        }

        if (wc_FreeRng(&rng) != 0) {
            WLOG(WS_LOG_DEBUG, "Couldn't free RNG");
            ret = WS_CRYPTO_FAILED;
        }
    }

    WLOG(WS_LOG_DEBUG, "Leaving wolfSSH_MakeRsaKey(), ret = %d", ret);
    return ret;
}

#endif
