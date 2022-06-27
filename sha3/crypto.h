

#ifndef _CRYPTO_H_
#define _CRYPTO_H_

//#include <windows.h>
#include <stdio.h>
//#include "error.h"

//typedef unsigned char byte;
typedef unsigned short word;

typedef unsigned char uint8_t;	/* 1-byte  (8-bits)  */
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;	/* 8-bytes (64-bits) */
typedef unsigned short int uint16_t;

typedef unsigned int uint_t;
typedef unsigned char char_t;


typedef unsigned int uint16;	/* at LEAST 16 bits, maybe more */
typedef unsigned int word16;

typedef unsigned char uint8;
typedef unsigned int  uint32;
typedef unsigned long long uint64;

/* typedef a 32 bit type */
typedef unsigned long int UINT4;


typedef unsigned int error_t;

// Error
#ifndef NO_ERROR
#define NO_ERROR 						0 // success
#define ERROR_FAILURE 					1 // Generic error code
#define ERROR_INVALID_PARAMETER 		2 // Invalid parameter
#define ERROR_PARAMETER_OUT_OF_RANGE 	3 // Specified parameter is out of range
#endif

#define ERROR_OUT_OF_MEMORY 			100







//Rotate left operation
#define ROL8(a, n) (((a) << (n)) | ((a) >> (8 - (n))))
#define ROL16(a, n) (((a) << (n)) | ((a) >> (16 - (n))))
#define ROL32(a, n) (((a) << (n)) | ((a) >> (32 - (n))))
#define ROL64(a, n) (((a) << (n)) | ((a) >> (64 - (n))))

//Rotate right operation
#define ROR8(a, n) (((a) >> (n)) | ((a) << (8 - (n))))
#define ROR16(a, n) (((a) >> (n)) | ((a) << (16 - (n))))
#define ROR32(a, n) (((a) >> (n)) | ((a) << (32 - (n))))
#define ROR64(a, n) (((a) >> (n)) | ((a) << (64 - (n))))

//Shift left operation
#define SHL8(a, n) ((a) << (n))
#define SHL16(a, n) ((a) << (n))
#define SHL32(a, n) ((a) << (n))
#define SHL64(a, n) ((a) << (n))

//Shift right operation
#define SHR8(a, n) ((a) >> (n))
#define SHR16(a, n) ((a) >> (n))
#define SHR32(a, n) ((a) >> (n))
#define SHR64(a, n) ((a) >> (n))


/**
 * @brief Encryption algorithm type
 **/

typedef enum
{
   CIPHER_ALGO_TYPE_STREAM = 0,
   CIPHER_ALGO_TYPE_BLOCK  = 1
} CipherAlgoType;


/**
 * @brief Cipher operation modes
 **/

typedef enum
{
   CIPHER_MODE_NULL              = 0,
   CIPHER_MODE_STREAM            = 1,
   CIPHER_MODE_ECB               = 2,
   CIPHER_MODE_CBC               = 3,
   CIPHER_MODE_CFB               = 4,
   CIPHER_MODE_OFB               = 5,
   CIPHER_MODE_CTR               = 6,
   CIPHER_MODE_CCM               = 7,
   CIPHER_MODE_GCM               = 8,
   CIPHER_MODE_CHACHA20_POLY1305 = 9,
} CipherMode;


//Common API for hash algorithms
typedef error_t (*HashAlgoCompute)(const void *data, size_t length, uint8_t *digest);
typedef void (*HashAlgoInit)(void *context);
typedef void (*HashAlgoUpdate)(void *context, const void *data, size_t length);
typedef void (*HashAlgoFinal)(void *context, uint8_t *digest);

//Common API for encryption algorithms
typedef error_t (*CipherAlgoInit)(void *context, const uint8_t *key, size_t keyLength);
typedef void (*CipherAlgoEncryptStream)(void *context, const uint8_t *input, uint8_t *output, size_t length);
typedef void (*CipherAlgoDecryptStream)(void *context, const uint8_t *input, uint8_t *output, size_t length);
typedef void (*CipherAlgoEncryptBlock)(void *context, const uint8_t *input, uint8_t *output);
typedef void (*CipherAlgoDecryptBlock)(void *context, const uint8_t *input, uint8_t *output);

//Common API for pseudo-random number generators
typedef error_t (*PrngAlgoInit)(void *context);
typedef void (*PrngAlgoRelease)(void *context);
typedef error_t (*PrngAlgoSeed)(void *context, const uint8_t *input, size_t length);
typedef error_t (*PrngAlgoAddEntropy)(void *context, uint_t source, const uint8_t *input, size_t length, size_t entropy);
typedef error_t (*PrngAlgoRead)(void *context, uint8_t *output, size_t length);


/**
 * @brief Generic hash algorithm context
 **/

typedef struct
{
   uint8_t digest[1];
} HashContext;


/**
 * @brief Common interface for hash algorithms
 **/

typedef struct
{
   const char_t *name;
   const uint8_t *oid;
   size_t oidSize;
   size_t contextSize;
   size_t blockSize;
   size_t digestSize;
   HashAlgoCompute compute;
   HashAlgoInit init;
   HashAlgoUpdate update;
   HashAlgoFinal final;
} HashAlgo;


/**
 * @brief Common interface for encryption algorithms
 **/

typedef struct
{
   const char_t *name;
   size_t contextSize;
   CipherAlgoType type;
   size_t blockSize;
   CipherAlgoInit init;
   CipherAlgoEncryptStream encryptStream;
   CipherAlgoDecryptStream decryptStream;
   CipherAlgoEncryptBlock encryptBlock;
   CipherAlgoDecryptBlock decryptBlock;
} CipherAlgo;


/**
 * @brief Common interface for pseudo-random number generators
 **/

typedef struct
{
   const char_t *name;
   size_t contextSize;
   PrngAlgoInit init;
   PrngAlgoRelease release;
   PrngAlgoSeed seed;
   PrngAlgoAddEntropy addEntropy;
   PrngAlgoRead read;
} PrngAlgo;


#endif
