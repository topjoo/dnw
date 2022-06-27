//-------------------------
// checksum
#define STRICT
#define WIN32_LEAN_AND_MEAN

#include "resource.h"

#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <memory.h> 

#include "md5.h"
#include "dnw.h"



void EB_Printf(const TCHAR *fmt,...); //engine.h


#ifdef __cplusplus
extern "C"
{
#endif



#define MD5_CHECKSUM_ENCIPHER 		1
#define POLARSSL_SHA1_ENCIPHER 		1
#define SHA2_256_384_512 			1
#define MD6_CHECKSUM_ENCIPHER 		1
#define SHA2_224_CHECKSUM 			1








#if POLARSSL_SHA1_ENCIPHER

/**
 *
 *  Copyright (C) 2006-2010, Brainspark B.V.
 *
 *  This file is part of PolarSSL (http://www.polarssl.org)
 *  Lead Maintainer: Paul Bakker <polarssl_maintainer at polarssl.org>
 *
 *  All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#if 0
#define MAX_BUFFER_SIZE				4096

#define OBJECT_FILE_LIST 			0xFF000001
#define OBJECT_COMMIT_FILE 			0xFF000002

#define LOG_ERROR(format, ...) 		fprintf(stderr, format "\n", ## __VA_ARGS__)
#define LOG_INFO(format, ...)  		fprintf(stdout, format "\n", ## __VA_ARGS__)



/*20 bytes for sha so 40 bytes to put it in hex and one byte for NULL
 * A SHA is valid only if it ends with a NULL char*/
#define SHA_HASH_LENGTH				40


typedef unsigned char ShaBuffer[SHA_HASH_LENGTH+1];
#endif




/*
 * 32-bit integer manipulation macros (big endian)
 */

#ifndef GET_ULONG_BE
#define GET_ULONG_BE(n,b,i)                             \
{                                                       \
    (n) = ( (unsigned long) (b)[(i)    ] << 24 )        \
        | ( (unsigned long) (b)[(i) + 1] << 16 )        \
        | ( (unsigned long) (b)[(i) + 2] <<  8 )        \
        | ( (unsigned long) (b)[(i) + 3]       );       \
}
#endif

#ifndef PUT_ULONG_BE
#define PUT_ULONG_BE(n,b,i)                             \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

#if 0
typedef struct
{
    unsigned long total[2];     /*!< number of bytes processed  */
    unsigned long state[5];     /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
    unsigned char ipad[64];     /*!< HMAC: inner padding        */
    unsigned char opad[64];     /*!< HMAC: outer padding        */
} sha1_context;



static void sha1_starts(sha1_context *ctx);
static void sha1_update(sha1_context *ctx, const unsigned char *input, int ilen);
static void sha1_finish(sha1_context *ctx, ShaBuffer sha);
#endif

/*
 * SHA-1 context setup
 */
void sha1_starts(sha1_context *ctx)
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->state[0] = 0x67452301UL;
    ctx->state[1] = 0xEFCDAB89UL;
    ctx->state[2] = 0x98BADCFEUL;
    ctx->state[3] = 0x10325476UL;
    ctx->state[4] = 0xC3D2E1F0UL;
}

void sha1_process( sha1_context *ctx, const unsigned char data[64] )
{
    unsigned long temp, W[16], A, B, C, D, E;

    GET_ULONG_BE( W[ 0], data,  0 );
    GET_ULONG_BE( W[ 1], data,  4 );
    GET_ULONG_BE( W[ 2], data,  8 );
    GET_ULONG_BE( W[ 3], data, 12 );
    GET_ULONG_BE( W[ 4], data, 16 );
    GET_ULONG_BE( W[ 5], data, 20 );
    GET_ULONG_BE( W[ 6], data, 24 );
    GET_ULONG_BE( W[ 7], data, 28 );
    GET_ULONG_BE( W[ 8], data, 32 );
    GET_ULONG_BE( W[ 9], data, 36 );
    GET_ULONG_BE( W[10], data, 40 );
    GET_ULONG_BE( W[11], data, 44 );
    GET_ULONG_BE( W[12], data, 48 );
    GET_ULONG_BE( W[13], data, 52 );
    GET_ULONG_BE( W[14], data, 56 );
    GET_ULONG_BE( W[15], data, 60 );

#define SHA1_S(x,n) ((x << n) | ((x & 0xFFFFFFFF) >> (32 - n)))

#define SHA1_R(t)                                            \
(                                                       \
    temp = W[(t -  3) & 0x0F] ^ W[(t - 8) & 0x0F] ^     \
           W[(t - 14) & 0x0F] ^ W[ t      & 0x0F],      \
    ( W[t & 0x0F] = SHA1_S(temp,1) )                    \
)

#define SHA1_P(a,b,c,d,e,x)                                 \
{                                                           \
    e += SHA1_S(a,5) + F(b,c,d) + K + x; b = SHA1_S(b,30);  \
}

    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];
    E = ctx->state[4];

#define F(x,y,z) (z ^ (x & (y ^ z)))

#define K 0x5A827999

    SHA1_P( A, B, C, D, E, W[0]  );
    SHA1_P( E, A, B, C, D, W[1]  );
    SHA1_P( D, E, A, B, C, W[2]  );
    SHA1_P( C, D, E, A, B, W[3]  );
    SHA1_P( B, C, D, E, A, W[4]  );
    SHA1_P( A, B, C, D, E, W[5]  );
    SHA1_P( E, A, B, C, D, W[6]  );
    SHA1_P( D, E, A, B, C, W[7]  );
    SHA1_P( C, D, E, A, B, W[8]  );
    SHA1_P( B, C, D, E, A, W[9]  );
    SHA1_P( A, B, C, D, E, W[10] );
    SHA1_P( E, A, B, C, D, W[11] );
    SHA1_P( D, E, A, B, C, W[12] );
    SHA1_P( C, D, E, A, B, W[13] );
    SHA1_P( B, C, D, E, A, W[14] );
    SHA1_P( A, B, C, D, E, W[15] );
    SHA1_P( E, A, B, C, D, SHA1_R(16) );
    SHA1_P( D, E, A, B, C, SHA1_R(17) );
    SHA1_P( C, D, E, A, B, SHA1_R(18) );
    SHA1_P( B, C, D, E, A, SHA1_R(19) );

#undef K
#undef F

#define F(x,y,z) (x ^ y ^ z)
#define K 0x6ED9EBA1

    SHA1_P( A, B, C, D, E, SHA1_R(20) );
    SHA1_P( E, A, B, C, D, SHA1_R(21) );
    SHA1_P( D, E, A, B, C, SHA1_R(22) );
    SHA1_P( C, D, E, A, B, SHA1_R(23) );
    SHA1_P( B, C, D, E, A, SHA1_R(24) );
    SHA1_P( A, B, C, D, E, SHA1_R(25) );
    SHA1_P( E, A, B, C, D, SHA1_R(26) );
    SHA1_P( D, E, A, B, C, SHA1_R(27) );
    SHA1_P( C, D, E, A, B, SHA1_R(28) );
    SHA1_P( B, C, D, E, A, SHA1_R(29) );
    SHA1_P( A, B, C, D, E, SHA1_R(30) );
    SHA1_P( E, A, B, C, D, SHA1_R(31) );
    SHA1_P( D, E, A, B, C, SHA1_R(32) );
    SHA1_P( C, D, E, A, B, SHA1_R(33) );
    SHA1_P( B, C, D, E, A, SHA1_R(34) );
    SHA1_P( A, B, C, D, E, SHA1_R(35) );
    SHA1_P( E, A, B, C, D, SHA1_R(36) );
    SHA1_P( D, E, A, B, C, SHA1_R(37) );
    SHA1_P( C, D, E, A, B, SHA1_R(38) );
    SHA1_P( B, C, D, E, A, SHA1_R(39) );

#undef K
#undef F

#define F(x,y,z) ((x & y) | (z & (x | y)))
#define K 0x8F1BBCDC

    SHA1_P( A, B, C, D, E, SHA1_R(40) );
    SHA1_P( E, A, B, C, D, SHA1_R(41) );
    SHA1_P( D, E, A, B, C, SHA1_R(42) );
    SHA1_P( C, D, E, A, B, SHA1_R(43) );
    SHA1_P( B, C, D, E, A, SHA1_R(44) );
    SHA1_P( A, B, C, D, E, SHA1_R(45) );
    SHA1_P( E, A, B, C, D, SHA1_R(46) );
    SHA1_P( D, E, A, B, C, SHA1_R(47) );
    SHA1_P( C, D, E, A, B, SHA1_R(48) );
    SHA1_P( B, C, D, E, A, SHA1_R(49) );
    SHA1_P( A, B, C, D, E, SHA1_R(50) );
    SHA1_P( E, A, B, C, D, SHA1_R(51) );
    SHA1_P( D, E, A, B, C, SHA1_R(52) );
    SHA1_P( C, D, E, A, B, SHA1_R(53) );
    SHA1_P( B, C, D, E, A, SHA1_R(54) );
    SHA1_P( A, B, C, D, E, SHA1_R(55) );
    SHA1_P( E, A, B, C, D, SHA1_R(56) );
    SHA1_P( D, E, A, B, C, SHA1_R(57) );
    SHA1_P( C, D, E, A, B, SHA1_R(58) );
    SHA1_P( B, C, D, E, A, SHA1_R(59) );

#undef K
#undef F

#define F(x,y,z) (x ^ y ^ z)
#define K 0xCA62C1D6

    SHA1_P( A, B, C, D, E, SHA1_R(60) );
    SHA1_P( E, A, B, C, D, SHA1_R(61) );
    SHA1_P( D, E, A, B, C, SHA1_R(62) );
    SHA1_P( C, D, E, A, B, SHA1_R(63) );
    SHA1_P( B, C, D, E, A, SHA1_R(64) );
    SHA1_P( A, B, C, D, E, SHA1_R(65) );
    SHA1_P( E, A, B, C, D, SHA1_R(66) );
    SHA1_P( D, E, A, B, C, SHA1_R(67) );
    SHA1_P( C, D, E, A, B, SHA1_R(68) );
    SHA1_P( B, C, D, E, A, SHA1_R(69) );
    SHA1_P( A, B, C, D, E, SHA1_R(70) );
    SHA1_P( E, A, B, C, D, SHA1_R(71) );
    SHA1_P( D, E, A, B, C, SHA1_R(72) );
    SHA1_P( C, D, E, A, B, SHA1_R(73) );
    SHA1_P( B, C, D, E, A, SHA1_R(74) );
    SHA1_P( A, B, C, D, E, SHA1_R(75) );
    SHA1_P( E, A, B, C, D, SHA1_R(76) );
    SHA1_P( D, E, A, B, C, SHA1_R(77) );
    SHA1_P( C, D, E, A, B, SHA1_R(78) );
    SHA1_P( B, C, D, E, A, SHA1_R(79) );

#undef K
#undef F

    ctx->state[0] += A;
    ctx->state[1] += B;
    ctx->state[2] += C;
    ctx->state[3] += D;
    ctx->state[4] += E;
}

/*
 * SHA-1 process buffer
 */
void sha1_update( sha1_context *ctx, const unsigned char *input, int ilen )
{
    int fill;
    unsigned long left;

    if( ilen <= 0 )
        return;

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += ilen;
    ctx->total[0] &= 0xFFFFFFFF;

    if( ctx->total[0] < (unsigned long) ilen )
        ctx->total[1]++;

    if( left && ilen >= fill )
    {
        memcpy( (void *) (ctx->buffer + left),
                (void *) input, fill );
        sha1_process( ctx, ctx->buffer );
        input += fill;
        ilen  -= fill;
        left = 0;
    }

    while( ilen >= 64 )
    {
        sha1_process( ctx, input );
        input += 64;
        ilen  -= 64;
    }

    if( ilen > 0 )
    {
        memcpy( (void *) (ctx->buffer + left),
                (void *) input, ilen );
    }
}

static const unsigned char sha1_padding[64] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * SHA-1 final digest
 */
void sha1_finish( sha1_context *ctx, ShaBuffer output)
{
	uint32_t i;
	unsigned char buffer[20];
	unsigned long last, padn;
	unsigned long high, low;
	unsigned char msglen[8];

    high = ( ctx->total[0] >> 29 )
         | ( ctx->total[1] <<  3 );
    low  = ( ctx->total[0] <<  3 );

    PUT_ULONG_BE( high, msglen, 0 );
    PUT_ULONG_BE( low,  msglen, 4 );

    last = ctx->total[0] & 0x3F;
    padn = ( last < 56 ) ? ( 56 - last ) : ( 120 - last );

    sha1_update( ctx, (unsigned char *) sha1_padding, padn );
    sha1_update( ctx, msglen, 8 );

    PUT_ULONG_BE( ctx->state[0], buffer,  0 );
    PUT_ULONG_BE( ctx->state[1], buffer,  4 );
    PUT_ULONG_BE( ctx->state[2], buffer,  8 );
    PUT_ULONG_BE( ctx->state[3], buffer, 12 );
    PUT_ULONG_BE( ctx->state[4], buffer, 16 );

	output[0] = '\0';
	for(i = 0; i < 20; i++)
	{
		char str[3+1];
		memset(str, 0x00, sizeof(str));
		//snprintf(str, 3, "%2.2x", (unsigned char)buffer[i]);
		sprintf(str, "%2.2x", (unsigned char)buffer[i]);
		strcat((char*)output, str);
	}
	output[SHA_HASH_LENGTH] = '\0';
}

/*
 * output = SHA-1( input buffer )
 */
boolean sha_buffer( const unsigned char *input, int ilen, ShaBuffer output)
{
    sha1_context ctx;
    sha1_starts( &ctx );
    sha1_update( &ctx, input, ilen );
    sha1_finish( &ctx, output );
    memset( &ctx, 0, sizeof( sha1_context ));
	return true;
}


#ifdef __NOT_USED__
/*
 * output = SHA-1( file contents )
 */
boolean sha_file( const char *path, ShaBuffer output)
{
    FILE *f;
    size_t n;
    sha1_context ctx;
    unsigned char buf[1024];


    if((f = fopen( path, "rb")) == NULL)
        return false;

    sha1_starts(&ctx);

    while((n = fread(buf, 1, sizeof(buf), f)) > 0)
        sha1_update(&ctx, buf, (int)n);

    sha1_finish(&ctx, output);

    memset(&ctx, 0, sizeof(sha1_context));

    if(ferror(f) != 0)
    {
        fclose(f);
        return(false);
    }

    fclose(f);
    return true;
}
#endif /// __NOT_USED__

boolean sha_compare(const ShaBuffer s1, const ShaBuffer s2)
{
	int i;
	for(i = 0; i < SHA_HASH_LENGTH; i++)
	{
		if(s1[i] != s2[i])
			return false;
	}
	return true;
}

void sha_reset(ShaBuffer sha)
{
	memset(sha, 0, SHA_HASH_LENGTH+1);
	return;
}
#endif /// POLARSSL_SHA1_ENCIPHER




#ifdef __cplusplus
}
#endif







