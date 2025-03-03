/* rabbit.c
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

#ifndef NO_RABBIT

#include <wolf/crypt/headers/rabbit.h>
#include <wolf/crypt/headers/error-crypt.h>
#include <wolf/crypt/headers/logging.h>
#ifdef NO_INLINE
    #include <wolf/crypt/headers/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolf/crypt/src/misc.c>
#endif


#ifdef BIG_ENDIAN_ORDER
    #define LITTLE32(x) ByteReverseWord32(x)
#else
    #define LITTLE32(x) (x)
#endif

#define U32V(x) ((word32)(x) & 0xFFFFFFFFU)


/* Square a 32-bit unsigned integer to obtain the 64-bit result and return */
/* the upper 32 bits XOR the lower 32 bits */
static word32 RABBIT_g_func(word32 x)
{
    /* Temporary variables */
    word32 a, b, h, l;

    /* Construct high and low argument for squaring */
    a = x&0xFFFF;
    b = x>>16;

    /* Calculate high and low result of squaring */
    h = (((U32V(a*a)>>17) + U32V(a*b))>>15) + b*b;
    l = x*x;

    /* Return high XOR low */
    return U32V(h^l);
}


/* Calculate the next internal state */
static void RABBIT_next_state(RabbitCtx* ctx)
{
    /* Temporary variables */
    word32 g[8], c_old[8], i;

    /* Save old counter values */
    for (i=0; i<8; i++)
        c_old[i] = ctx->c[i];

    /* Calculate new counter values */
    ctx->c[0] = U32V(ctx->c[0] + 0x4D34D34D + ctx->carry);
    ctx->c[1] = U32V(ctx->c[1] + 0xD34D34D3 + (ctx->c[0] < c_old[0]));
    ctx->c[2] = U32V(ctx->c[2] + 0x34D34D34 + (ctx->c[1] < c_old[1]));
    ctx->c[3] = U32V(ctx->c[3] + 0x4D34D34D + (ctx->c[2] < c_old[2]));
    ctx->c[4] = U32V(ctx->c[4] + 0xD34D34D3 + (ctx->c[3] < c_old[3]));
    ctx->c[5] = U32V(ctx->c[5] + 0x34D34D34 + (ctx->c[4] < c_old[4]));
    ctx->c[6] = U32V(ctx->c[6] + 0x4D34D34D + (ctx->c[5] < c_old[5]));
    ctx->c[7] = U32V(ctx->c[7] + 0xD34D34D3 + (ctx->c[6] < c_old[6]));
    ctx->carry = (ctx->c[7] < c_old[7]);

    /* Calculate the g-values */
    for (i=0;i<8;i++)
        g[i] = RABBIT_g_func(U32V(ctx->x[i] + ctx->c[i]));

    /* Calculate new state values */
    ctx->x[0] = U32V(g[0] + rotlFixed(g[7],16) + rotlFixed(g[6], 16));
    ctx->x[1] = U32V(g[1] + rotlFixed(g[0], 8) + g[7]);
    ctx->x[2] = U32V(g[2] + rotlFixed(g[1],16) + rotlFixed(g[0], 16));
    ctx->x[3] = U32V(g[3] + rotlFixed(g[2], 8) + g[1]);
    ctx->x[4] = U32V(g[4] + rotlFixed(g[3],16) + rotlFixed(g[2], 16));
    ctx->x[5] = U32V(g[5] + rotlFixed(g[4], 8) + g[3]);
    ctx->x[6] = U32V(g[6] + rotlFixed(g[5],16) + rotlFixed(g[4], 16));
    ctx->x[7] = U32V(g[7] + rotlFixed(g[6], 8) + g[5]);
}


/* IV setup */
static void wc_RabbitSetIV(Rabbit* ctx, const byte* inIv)
{
    /* Temporary variables */
    word32 i0, i1, i2, i3, i;
    word32 iv[2];

    if (inIv)
        XMEMCPY(iv, inIv, sizeof(iv));
    else
        XMEMSET(iv,    0, sizeof(iv));

    /* Generate four subvectors */
    i0 = LITTLE32(iv[0]);
    i2 = LITTLE32(iv[1]);
    i1 = (i0>>16) | (i2&0xFFFF0000);
    i3 = (i2<<16) | (i0&0x0000FFFF);

    /* Modify counter values */
    ctx->workCtx.c[0] = ctx->masterCtx.c[0] ^ i0;
    ctx->workCtx.c[1] = ctx->masterCtx.c[1] ^ i1;
    ctx->workCtx.c[2] = ctx->masterCtx.c[2] ^ i2;
    ctx->workCtx.c[3] = ctx->masterCtx.c[3] ^ i3;
    ctx->workCtx.c[4] = ctx->masterCtx.c[4] ^ i0;
    ctx->workCtx.c[5] = ctx->masterCtx.c[5] ^ i1;
    ctx->workCtx.c[6] = ctx->masterCtx.c[6] ^ i2;
    ctx->workCtx.c[7] = ctx->masterCtx.c[7] ^ i3;

    /* Copy state variables */
    for (i=0; i<8; i++)
        ctx->workCtx.x[i] = ctx->masterCtx.x[i];
    ctx->workCtx.carry = ctx->masterCtx.carry;

    /* Iterate the system four times */
    for (i=0; i<4; i++)
        RABBIT_next_state(&(ctx->workCtx));
}


/* Key setup */
static WC_INLINE int DoKey(Rabbit* ctx, const byte* key, const byte* iv)
{
    /* Temporary variables */
    word32 k0, k1, k2, k3, i;

    /* Generate four subkeys */
    k0 = LITTLE32(*(word32*)(key+ 0));
    k1 = LITTLE32(*(word32*)(key+ 4));
    k2 = LITTLE32(*(word32*)(key+ 8));
    k3 = LITTLE32(*(word32*)(key+12));

    /* Generate initial state variables */
    ctx->masterCtx.x[0] = k0;
    ctx->masterCtx.x[2] = k1;
    ctx->masterCtx.x[4] = k2;
    ctx->masterCtx.x[6] = k3;
    ctx->masterCtx.x[1] = U32V(k3<<16) | (k2>>16);
    ctx->masterCtx.x[3] = U32V(k0<<16) | (k3>>16);
    ctx->masterCtx.x[5] = U32V(k1<<16) | (k0>>16);
    ctx->masterCtx.x[7] = U32V(k2<<16) | (k1>>16);

    /* Generate initial counter values */
    ctx->masterCtx.c[0] = rotlFixed(k2, 16);
    ctx->masterCtx.c[2] = rotlFixed(k3, 16);
    ctx->masterCtx.c[4] = rotlFixed(k0, 16);
    ctx->masterCtx.c[6] = rotlFixed(k1, 16);
    ctx->masterCtx.c[1] = (k0&0xFFFF0000) | (k1&0xFFFF);
    ctx->masterCtx.c[3] = (k1&0xFFFF0000) | (k2&0xFFFF);
    ctx->masterCtx.c[5] = (k2&0xFFFF0000) | (k3&0xFFFF);
    ctx->masterCtx.c[7] = (k3&0xFFFF0000) | (k0&0xFFFF);

    /* Clear carry bit */
    ctx->masterCtx.carry = 0;

    /* Iterate the system four times */
    for (i=0; i<4; i++)
        RABBIT_next_state(&(ctx->masterCtx));

    /* Modify the counters */
    for (i=0; i<8; i++)
        ctx->masterCtx.c[i] ^= ctx->masterCtx.x[(i+4)&0x7];

    /* Copy master instance to work instance */
    for (i=0; i<8; i++) {
        ctx->workCtx.x[i] = ctx->masterCtx.x[i];
        ctx->workCtx.c[i] = ctx->masterCtx.c[i];
    }
    ctx->workCtx.carry = ctx->masterCtx.carry;

    wc_RabbitSetIV(ctx, iv);

    return 0;
}


int wc_Rabbit_SetHeap(Rabbit* ctx, void* heap)
{
    if (ctx == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef XSTREAM_ALIGN
    ctx->heap = heap;
#endif

    (void)heap;
    return 0;
}


/* Key setup */
int wc_RabbitSetKey(Rabbit* ctx, const byte* key, const byte* iv)
{
    if (ctx == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef XSTREAM_ALIGN
    /* default heap to NULL or heap test value */
    #ifdef WOLFSSL_HEAP_TEST
        ctx->heap = (void*)WOLFSSL_HEAP_TEST;
    #else
        ctx->heap = NULL;
    #endif /* WOLFSSL_HEAP_TEST */

    if ((wolfssl_word)key % 4) {
        int alignKey[4];

        /* iv aligned in SetIV */
        WOLFSSL_MSG("wc_RabbitSetKey unaligned key");

        XMEMCPY(alignKey, key, sizeof(alignKey));

        return DoKey(ctx, (const byte*)alignKey, iv);
    }
#endif /* XSTREAM_ALIGN */

    return DoKey(ctx, key, iv);
}


/* Encrypt/decrypt a message of any size */
static WC_INLINE int DoProcess(Rabbit* ctx, byte* output, const byte* input,
                            word32 msglen)
{
    /* Encrypt/decrypt all full blocks */
    while (msglen >= 16) {
        /* Iterate the system */
        RABBIT_next_state(&(ctx->workCtx));

        /* Encrypt/decrypt 16 bytes of data */
        *(word32*)(output+ 0) = *(word32*)(input+ 0) ^
                   LITTLE32(ctx->workCtx.x[0] ^ (ctx->workCtx.x[5]>>16) ^
                   U32V(ctx->workCtx.x[3]<<16));
        *(word32*)(output+ 4) = *(word32*)(input+ 4) ^
                   LITTLE32(ctx->workCtx.x[2] ^ (ctx->workCtx.x[7]>>16) ^
                   U32V(ctx->workCtx.x[5]<<16));
        *(word32*)(output+ 8) = *(word32*)(input+ 8) ^
                   LITTLE32(ctx->workCtx.x[4] ^ (ctx->workCtx.x[1]>>16) ^
                   U32V(ctx->workCtx.x[7]<<16));
        *(word32*)(output+12) = *(word32*)(input+12) ^
                   LITTLE32(ctx->workCtx.x[6] ^ (ctx->workCtx.x[3]>>16) ^
                   U32V(ctx->workCtx.x[1]<<16));

        /* Increment pointers and decrement length */
        input  += 16;
        output += 16;
        msglen -= 16;
    }

    /* Encrypt/decrypt remaining data */
    if (msglen) {

        word32 i;
        word32 tmp[4];
        byte*  buffer = (byte*)tmp;

        XMEMSET(tmp, 0, sizeof(tmp));   /* help static analysis */

        /* Iterate the system */
        RABBIT_next_state(&(ctx->workCtx));

        /* Generate 16 bytes of pseudo-random data */
        tmp[0] = LITTLE32(ctx->workCtx.x[0] ^
                  (ctx->workCtx.x[5]>>16) ^ U32V(ctx->workCtx.x[3]<<16));
        tmp[1] = LITTLE32(ctx->workCtx.x[2] ^
                  (ctx->workCtx.x[7]>>16) ^ U32V(ctx->workCtx.x[5]<<16));
        tmp[2] = LITTLE32(ctx->workCtx.x[4] ^
                  (ctx->workCtx.x[1]>>16) ^ U32V(ctx->workCtx.x[7]<<16));
        tmp[3] = LITTLE32(ctx->workCtx.x[6] ^
                  (ctx->workCtx.x[3]>>16) ^ U32V(ctx->workCtx.x[1]<<16));

        /* Encrypt/decrypt the data */
        for (i=0; i<msglen; i++)
            output[i] = input[i] ^ buffer[i];
    }

    return 0;
}


/* Encrypt/decrypt a message of any size */
int wc_RabbitProcess(Rabbit* ctx, byte* output, const byte* input, word32 msglen)
{
    if (ctx == NULL || output == NULL || input == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef XSTREAM_ALIGN
    if ((wolfssl_word)input % 4 || (wolfssl_word)output % 4) {
        #ifndef NO_WOLFSSL_ALLOC_ALIGN
            byte* tmp;
            WOLFSSL_MSG("wc_RabbitProcess unaligned");

            tmp = (byte*)XMALLOC(msglen, ctx->heap, DYNAMIC_TYPE_TMP_BUFFER);
            if (tmp == NULL) return MEMORY_E;

            XMEMCPY(tmp, input, msglen);
            DoProcess(ctx, tmp, tmp, msglen);
            XMEMCPY(output, tmp, msglen);

            XFREE(tmp, ctx->heap, DYNAMIC_TYPE_TMP_BUFFER);

            return 0;
        #else
            return BAD_ALIGN_E;
        #endif
    }
#endif /* XSTREAM_ALIGN */

    return DoProcess(ctx, output, input, msglen);
}


#endif /* NO_RABBIT */

