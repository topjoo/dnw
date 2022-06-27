
#ifndef __CHECKSUM_MD5_H__
#define __CHECKSUM_MD5_H__


#define false 0
#define true 1

typedef int boolean;
typedef unsigned char byte;
typedef unsigned short word;

typedef unsigned char uint8_t;	/* 1-byte  (8-bits)  */
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;	/* 8-bytes (64-bits) */
typedef unsigned short int uint16_t;

typedef unsigned int uint16;	/* at LEAST 16 bits, maybe more */
typedef unsigned int word16;

typedef unsigned char uint8;
typedef unsigned int  uint32;
typedef unsigned long long uint64;

/* typedef a 32 bit type */
typedef unsigned long int UINT4;



/* Data structure for MD5 (Message Digest) computation */
typedef struct {
  UINT4 i[2];                   /* number of _bits_ handled mod 2^64 */
  UINT4 buf[4];                                    /* scratch buffer */
  unsigned char in[64];                              /* input buffer */
  unsigned char digest[16];     /* actual digest after MD5Final call */
} MD5_CTX;


#ifdef __cplusplus
extern "C"
{
#endif


#define INIT_DATA_A (unsigned long)0x67452301UL
#define INIT_DATA_B (unsigned long)0xefcdab89UL
#define INIT_DATA_C (unsigned long)0x98badcfeUL
#define INIT_DATA_D (unsigned long)0x10325476UL


extern void MD5Init (MD5_CTX *mdContext);
extern void MD5Update (MD5_CTX *mdContext, unsigned char *inBuf, unsigned int inLen);
extern void MD5Final (MD5_CTX *mdContext);
extern void MDPrint (MD5_CTX *mdContext);


#ifdef __cplusplus
}
#endif




// SHA1
#define MAX_BUFFER_SIZE				4096

#define OBJECT_FILE_LIST 			0xFF000001
#define OBJECT_COMMIT_FILE 			0xFF000002

#define LOG_ERROR(format, ...) 		fprintf(stderr, format "\n", ## __VA_ARGS__)
#define LOG_INFO(format, ...)  		fprintf(stdout, format "\n", ## __VA_ARGS__)



/*20 bytes for sha so 40 bytes to put it in hex and one byte for NULL
 * A SHA is valid only if it ends with a NULL char*/
#define SHA_HASH_LENGTH				40


typedef unsigned char ShaBuffer[SHA_HASH_LENGTH+1];

typedef struct
{
    unsigned long total[2];     /*!< number of bytes processed  */
    unsigned long state[5];     /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
    unsigned char ipad[64];     /*!< HMAC: inner padding        */
    unsigned char opad[64];     /*!< HMAC: outer padding        */
} sha1_context;


#ifdef __cplusplus
extern "C"
{
#endif

extern void sha1_starts(sha1_context *ctx);
extern void sha1_update(sha1_context *ctx, const unsigned char *input, int ilen);
extern void sha1_finish(sha1_context *ctx, ShaBuffer sha);
//extern bool sha_buffer( const unsigned char *input, int ilen, ShaBuffer output);


#define CRC_BUFSIZE 	10*1024

extern uint64_t make_crc64(uint64_t crc, const unsigned char *s, uint64_t l);
extern uint32_t make_crc32(uint32_t crc, const void *buf, size_t size);
extern unsigned long calcCRC32(const unsigned char *mem, signed long size, unsigned long CRC) ;

#ifdef __cplusplus
}
#endif





/*
 * FILE:	sha2.c
 * AUTHOR:	Aaron D. Gifford - http://www.aarongifford.com/
 * 
 * Copyright (c) 2000-2001, Aaron D. Gifford
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 */


#define SHA2_BUFLEN 			16384



/*** SHA-256/384/512 Various Length Definitions ***********************/
#define SHA256_BLOCK_LENGTH			64
#define SHA256_DIGEST_LENGTH 		32
#define SHA256_DIGEST_STRING_LENGTH	(SHA256_DIGEST_LENGTH * 2 + 1)
#define SHA384_BLOCK_LENGTH			128
#define SHA384_DIGEST_LENGTH 		48
#define SHA384_DIGEST_STRING_LENGTH	(SHA384_DIGEST_LENGTH * 2 + 1)
#define SHA512_BLOCK_LENGTH			128
#define SHA512_DIGEST_LENGTH		64
#define SHA512_DIGEST_STRING_LENGTH	(SHA512_DIGEST_LENGTH * 2 + 1)


/*** SHA-256/384/512 Context Structures *******************************/
/* NOTE: If your architecture does not define either u_intXX_t types or
 * uintXX_t (from inttypes.h), you may need to define things by hand
 * for your system:
 */

typedef unsigned char      u_int8_t;	/* 1-byte  (8-bits)  */
typedef unsigned int       u_int32_t;	/* 4-bytes (32-bits) */
typedef unsigned long long u_int64_t;	/* 8-bytes (64-bits) */
typedef unsigned short     u_int16_t;

/*
 * Most BSD systems already define u_intXX_t types, as does Linux.
 * Some systems, however, like Compaq's Tru64 Unix instead can use
 * uintXX_t types defined by very recent ANSI C standards and included
 * in the file:
 *
 *   #include <inttypes.h>
 *
 * If you choose to use <inttypes.h> then please define: 
 *
 *   #define SHA2_USE_INTTYPES_H
 *
 * Or on the command line during compile:
 *
 *   cc -DSHA2_USE_INTTYPES_H ...
 */
#ifdef SHA2_USE_INTTYPES_H
typedef struct _SHA256_CTX {
	uint32_t	state[8];
	uint64_t	bitcount;
	uint8_t	buffer[SHA256_BLOCK_LENGTH];
} SHA256_CTX;
typedef struct _SHA512_CTX {
	uint64_t	state[8];
	uint64_t	bitcount[2];
	uint8_t	buffer[SHA512_BLOCK_LENGTH];
} SHA512_CTX;
#else /* SHA2_USE_INTTYPES_H */

typedef struct _SHA256_CTX {
	u_int32_t	state[8];
	u_int64_t	bitcount;
	u_int8_t	buffer[SHA256_BLOCK_LENGTH];
} SHA256_CTX;
typedef struct _SHA512_CTX {
	u_int64_t	state[8];
	u_int64_t	bitcount[2];
	u_int8_t	buffer[SHA512_BLOCK_LENGTH];
} SHA512_CTX;

#endif /* SHA2_USE_INTTYPES_H */

typedef SHA512_CTX SHA384_CTX;


/*
 * UNROLLED TRANSFORM LOOP NOTE:
 * You can define SHA2_UNROLL_TRANSFORM to use the unrolled transform
 * loop version for the hash transform rounds (defined using macros
 * later in this file).  Either define on the command line, for example:
 *
 *   cc -DSHA2_UNROLL_TRANSFORM -o sha2 sha2.c sha2prog.c
 *
 * or define below:
 *
 *   #define SHA2_UNROLL_TRANSFORM
 *
 */


#define SHA2_UNROLL_TRANSFORM 				1 /// 2014.06.30

#define LITTLE_ENDIAN 			1234
#define BIG_ENDIAN 				4321

#define BYTE_ORDER 			LITTLE_ENDIAN   /// 2014.06.30 -- OK
///#define BYTE_ORDER 			BIG_ENDIAN

#ifdef SHA2_USE_INTTYPES_H

typedef uint8_t  sha2_byte;	/* Exactly 1 byte */
typedef uint32_t sha2_word32;	/* Exactly 4 bytes */
typedef uint64_t sha2_word64;	/* Exactly 8 bytes */

#else /* SHA2_USE_INTTYPES_H */

typedef u_int8_t  sha2_byte;	/* Exactly 1 byte */
typedef u_int32_t sha2_word32;	/* Exactly 4 bytes */
typedef u_int64_t sha2_word64;	/* Exactly 8 bytes */

#endif /* SHA2_USE_INTTYPES_H */



#if 1
// sha224
#define SHA224_DIGEST_SIZE ( 224 / 8)
#define SHA256_DIGEST_SIZE ( 256 / 8)
#define SHA384_DIGEST_SIZE ( 384 / 8)
#define SHA512_DIGEST_SIZE ( 512 / 8)

#define SHA256_BLOCK_SIZE  ( 512 / 8)
#define SHA512_BLOCK_SIZE  (1024 / 8)
#define SHA384_BLOCK_SIZE  SHA512_BLOCK_SIZE
#define SHA224_BLOCK_SIZE  SHA256_BLOCK_SIZE


typedef struct {
    unsigned int tot_len;
    unsigned int len;
    unsigned char block[2 * SHA256_BLOCK_SIZE];
    uint32 h[8];
} sha256_ctx;

typedef sha256_ctx 		sha224_ctx;

#endif





#ifdef __cplusplus
extern "C"
{
#endif

extern void SHA256_Init(SHA256_CTX* context);
extern void SHA256_Update(SHA256_CTX* context, const sha2_byte *data, size_t len) ;
extern void SHA256_Final(sha2_byte digest[], SHA256_CTX* context) ;
extern char *SHA256_End(SHA256_CTX* context, char buffer[]);


extern void SHA384_Init(SHA384_CTX* context);
extern void SHA384_Update(SHA384_CTX* context, const sha2_byte* data, size_t len);
extern void SHA384_Final(sha2_byte digest[], SHA384_CTX* context);
extern char *SHA384_End(SHA384_CTX* context, char buffer[]);


extern void SHA512_Init(SHA512_CTX* context);
extern void SHA512_Update(SHA512_CTX* context, const sha2_byte *data, size_t len);
extern void SHA512_Final(sha2_byte digest[], SHA512_CTX* context);
extern char *SHA512_End(SHA512_CTX* context, char buffer[]);


extern void sha224_init(sha224_ctx *ctx);
extern void sha224_update(sha224_ctx *ctx, const unsigned char *message, unsigned int len);
extern void sha224_final(sha224_ctx *ctx, unsigned char *digest);
extern void sha224Print(unsigned char *sha224_digest);







// ----------------------------------------
// ----------------------------------------
// ----------------------------------------
// ----------------------------------------
/* ------------ MD6 --------------------- */
// ----------------------------------------

#define   md6_w    64

/* Define "md6_word" appropriately for given value of md6_w.
** Also define PR_MD6_WORD to be the appropriate hex format string,
** using the format strings from inttypes.h .
** The term `word' in comments means an `md6_word'.
*/

#if (md6_w==64)                    /* standard md6 */
typedef uint64_t md6_word;
#define PR_MD6_WORD "%.16" PRIx64
#endif


/* MD6 compression function constants  */
#define md6_n      89    /* size of compression input block, in words  */
#define md6_c      16    /* size of compression output, in words       */
                         /* a c-word block is also called a "chunk"    */
#define md6_max_r 255    /* max allowable value for number r of rounds */

/* Compression function routines                                
** These are ``internal'' routines that need not be called for  
** ordinary md6 usage.
*/


typedef uint64_t md6_control_word;                      /* (r,L,z,p,d) */
typedef uint64_t md6_nodeID; /* (ell,i) */


/* MD6 mode of operation.
** MD6 mode of operation is defined in file md6_mode.c 
*/

/* MD6 constants related to standard mode of operation                 */
/* These five values give lengths of the components of compression     */
/* input block; they should sum to md6_n.                              */

#define md6_q 15         /* # Q words in compression block (>=0)       */
#define md6_k  8         /* # key words per compression block (>=0)    */
#define md6_u (64/md6_w) /* # words for unique node ID (0 or 64/w)     */
#define md6_v (64/md6_w) /* # words for control word (0 or 64/w)       */
#define md6_b 64         /* # data words per compression block (>0)    */

#define md6_default_L 64    /* large so that MD6 is fully hierarchical */

#define md6_max_stack_height 29
    /* max_stack_height determines the maximum number of bits that
    ** can be processed by this implementation (with default L) to be:
    **    (b*w) * ((b/c) ** (max_stack_height-3)
    **    = 2 ** 64  for b = 64, w = 64, c = 16, and  max_stack_height = 29
    ** (We lose three off the height since level 0 is unused,
    ** level 1 contains the input data, and C has 0-origin indexing.)
    ** The smallest workable value for md6_max_stack_height is 3.
    ** (To avoid stack overflow for non-default L values, 
    ** we should have max_stack_height >= L + 2.)
    ** (One level of storage could be saved by letting st->N[] use
    ** 1-origin indexing, since st->N[0] is now unused.)
    */
    


typedef struct {

  int d;           /* desired hash bit length. 1 <= d <= 512.      */
  int hashbitlen;  /* hashbitlen is the same as d; for NIST API    */

  unsigned char hashval[ md6_c*(md6_w/8) ];
      /* e.g. unsigned char hashval[128]                           */
      /* contains hashval after call to md6_final                  */
      /* hashval appears in first floor(d/8) bytes, with           */
      /* remaining (d mod 8) bits (if any) appearing in            */
      /* high-order bit positions of hashval[1+floor(d/8)].        */

  unsigned char hexhashval[(md6_c*(md6_w/8))+1];
      /* e.g. unsigned char hexhashval[129];                       */
      /* zero-terminated string representing hex value of hashval  */

  int initialized;         /* zero, then one after md6_init called */
  uint64_t bits_processed;                /* bits processed so far */
  uint64_t compression_calls;    /* compression function calls made*/
  int finalized;          /* zero, then one after md6_final called */

  md6_word K[ md6_k ];  
      /* k-word (8 word) key (aka "salt") for this instance of md6 */
  int keylen;
      /* number of bytes in key K. 0<=keylen<=k*(w/8)              */

  int L;
      /* md6 mode specification parameter. 0 <= L <= 255           */
      /* L == 0 means purely sequential (Merkle-Damgaard)          */
      /* L >= 29 means purely tree-based                           */
      /* Default is md6_default_L = 64 (hierarchical)              */

  int r;
      /* Number of rounds. 0 <= r <= 255                           */

  int top;
      /* index of block corresponding to top of stack              */

  md6_word B[ md6_max_stack_height ][ md6_b ];
      /* md6_word B[29][64]                                        */
      /* stack of 29 64-word partial blocks waiting to be          */
      /* completed and compressed.                                 */
      /* B[1] is for compressing text data (input);                */
      /* B[ell] corresponds to node at level ell in the tree.      */

  unsigned int bits[ md6_max_stack_height ];    
      /* bits[ell] =                                               */
      /*    number of bits already placed in B[ell]                */
      /*    for 1 <= ell < max_stack_height                        */
      /* 0 <= bits[ell] <= b*w                                     */

  uint64_t i_for_level[ md6_max_stack_height ];
      /* i_for_level[ell] =                                        */
      /*    index of the node B[ ell ] on this level (0,1,...)     */
      /* when it is output   */

} md6_state;



/* ------------ MD6 parameters --------------------- */
extern int md6_dgtLen; // = 256;         /* digest length */
extern int md6_modPar; // = 64;          /* mode parameter */
extern int md6_roundN;               /* number of rounds */
extern int md6_use_default_r; // = 1;    /* 1 if r should be set to the default, 0 if r is explicitly provided */
extern unsigned char md6_K[100];     /* key */
extern int md6_keylen; // = 0;           /* key length in bytes (at most 64) */
extern md6_state md6_st;             /* md6 computation state */

extern char md6_msg[5000];           /* message to be hashed (if given with -M) */
extern int msglenbytes;              /* message length in bytes */


extern int md6_default_r( int d , int keylen );
extern void hash_init();
extern void hash_update(char* data, uint64_t databitlen);
extern void hash_final();






#ifdef __cplusplus
}
#endif


#endif // __CHECKSUM_MD5_H__

