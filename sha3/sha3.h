
#ifndef __SHA3_KECCAK__
#define __SHA3_KECCAK__

//Dependencies
#include "crypto.h"
#include "keccak.h"




#ifdef __cplusplus
extern "C"
{
#endif


// Declare
#define ENABLED 		1
#define DISABLED 		0

#define SHAKE128_SUPPORT 		ENABLED
#define SHAKE256_SUPPORT 		ENABLED
#define SHA3_224_SUPPORT 		ENABLED
#define SHA3_256_SUPPORT 		ENABLED
#define SHA3_384_SUPPORT 		ENABLED
#define SHA3_512_SUPPORT 		ENABLED



#ifndef _SHA3_512_H
#define _SHA3_512_H

//SHA3-512 block size
#define SHA3_512_BLOCK_SIZE 		72
//SHA3-512 digest size
#define SHA3_512_DIGEST_SIZE 		64
//Common interface for hash algorithms
#define SHA3_512_HASH_ALGO (&sha3_512HashAlgo)


/**
 * @brief SHA3-512 algorithm context
 **/

typedef KeccakContext Sha3_512Context;


//SHA3-512 related constants
extern const HashAlgo sha3_512HashAlgo;

//SHA3-512 related functions
error_t sha3_512Compute(const void *data, size_t length, uint8_t *digest);
void sha3_512Init(Sha3_512Context *context);
void sha3_512Update(Sha3_512Context *context, const void *data, size_t length);
void sha3_512Final(Sha3_512Context *context, uint8_t *digest);

#endif


#ifndef _SHA3_384_H
#define _SHA3_384_H

//SHA3-384 block size
#define SHA3_384_BLOCK_SIZE 		104
//SHA3-384 digest size
#define SHA3_384_DIGEST_SIZE 		48
//Common interface for hash algorithms
#define SHA3_384_HASH_ALGO (&sha3_384HashAlgo)


/**
 * @brief SHA3-384 algorithm context
 **/

typedef KeccakContext Sha3_384Context;


//SHA3-384 related constants
extern const HashAlgo sha3_384HashAlgo;

//SHA3-384 related functions
error_t sha3_384Compute(const void *data, size_t length, uint8_t *digest);
void sha3_384Init(Sha3_384Context *context);
void sha3_384Update(Sha3_384Context *context, const void *data, size_t length);
void sha3_384Final(Sha3_384Context *context, uint8_t *digest);

#endif




#ifndef _SHA3_256_H
#define _SHA3_256_H

//SHA3-256 block size
#define SHA3_256_BLOCK_SIZE 		136
//SHA3-256 digest size
#define SHA3_256_DIGEST_SIZE 		32
//Common interface for hash algorithms
#define SHA3_256_HASH_ALGO (&sha3_256HashAlgo)


/**
 * @brief SHA3-256 algorithm context
 **/

typedef KeccakContext Sha3_256Context;


//SHA3-256 related constants
extern const HashAlgo sha3_256HashAlgo;

//SHA3-256 related functions
error_t sha3_256Compute(const void *data, size_t length, uint8_t *digest);
void sha3_256Init(Sha3_256Context *context);
void sha3_256Update(Sha3_256Context *context, const void *data, size_t length);
void sha3_256Final(Sha3_256Context *context, uint8_t *digest);

#endif




#ifndef _SHA3_224_H
#define _SHA3_224_H


//SHA3-224 block size
#define SHA3_224_BLOCK_SIZE 		144
//SHA3-224 digest size
#define SHA3_224_DIGEST_SIZE 		28
//Common interface for hash algorithms
#define SHA3_224_HASH_ALGO (&sha3_224HashAlgo)


/**
 * @brief SHA3-224 algorithm context
 **/

typedef KeccakContext Sha3_224Context;


//SHA3-224 related constants
extern const HashAlgo sha3_224HashAlgo;

//SHA3-224 related functions
error_t sha3_224Compute(const void *data, size_t length, uint8_t *digest);
void sha3_224Init(Sha3_224Context *context);
void sha3_224Update(Sha3_224Context *context, const void *data, size_t length);
void sha3_224Final(Sha3_224Context *context, uint8_t *digest);
#endif



#ifndef _SHAKE128_H
#define _SHAKE128_H


/**
 * @brief SHAKE128 algorithm context
 **/

typedef KeccakContext Shake128Context;


//SHAKE128 related constants
extern const uint8_t shake128Oid[9];

//SHAKE128 related functions
error_t shake128Compute(const void *input, size_t inputLen, uint8_t *output, size_t outputLen);
void shake128Init(Shake128Context *context);
void shake128Absorb(Shake128Context *context, const void *input, size_t length);
void shake128Final(Shake128Context *context);
void shake128Squeeze(Shake128Context *context, uint8_t *output, size_t length);


// SHAKE128 output length is variable
#define SHAKE128_DIGEST_DEFAULT_SIZE 		32 // (256/8) , (128/8), ...
//#define SHAKE128_DIGEST_DEFAULT_SIZE 		16 // (128/8) , (128/8), ...

#endif // _SHAKE128_H




#ifndef _SHAKE256_H
#define _SHAKE256_H

/**
 * @brief SHAKE256 algorithm context
 **/

typedef KeccakContext Shake256Context;


//SHAKE256 related constants
extern const uint8_t shake256Oid[9];

//SHAKE256 related functions
error_t shake256Compute(const void *input, size_t inputLen, uint8_t *output, size_t outputLen);
void shake256Init(Shake256Context *context);
void shake256Absorb(Shake256Context *context, const void *input, size_t length);
void shake256Final(Shake256Context *context);
void shake256Squeeze(Shake256Context *context, uint8_t *output, size_t length);


// SHAKE256 output length is variable
#define SHAKE256_DIGEST_DEFAULT_SIZE 		64 // (512/8), (256/8), (128/8), ....
//#define SHAKE256_DIGEST_DEFAULT_SIZE 		32 // (256/8), (128/8), ....
//#define SHAKE256_DIGEST_DEFAULT_SIZE 		16 // (128/8), ....

#endif // _SHAKE256_H


#ifdef __cplusplus
}
#endif


#endif // __SHA3_KECCAK__


