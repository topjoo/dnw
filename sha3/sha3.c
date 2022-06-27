/**
 * @file sha3_512.c
 * @brief SHA3-512 hash function (SHA-3 with 512-bit output)
 *
 * @section License
 *
 * Copyright (C) 2010-2017 Oryx Embedded SARL. All rights reserved.
 *
 * This file is part of CycloneCrypto Open.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * @section Description
 *
 * SHA-3 is a secure hash algorithm for computing a condensed representation
 * of an electronic message. Refer to FIPS 202 for more details
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 1.7.8
 **/


//Switch to the appropriate trace level
//#define TRACE_LEVEL CRYPTO_TRACE_LEVEL

//Dependencies
#define STRICT
#define WIN32_LEAN_AND_MEAN

//#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h> 

//#include "error.h"
#include "crypto.h"
#include "sha3.h"




//void EB_Printf(const TCHAR *fmt,...); //engine.h


#ifdef __cplusplus
extern "C"
{
#endif




//Check crypto library configuration
#if (SHA3_512_SUPPORT == ENABLED)

//SHA3-512 object identifier (2.16.840.1.101.3.4.2.10)
static const uint8_t sha3_512Oid[] = {0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x0A};

//Common interface for hash algorithms
const HashAlgo sha3_512HashAlgo =
{
   "SHA3-512",
   sha3_512Oid,
   sizeof(sha3_512Oid),
   sizeof(Sha3_512Context),
   SHA3_512_BLOCK_SIZE,
   SHA3_512_DIGEST_SIZE,
   (HashAlgoCompute) sha3_512Compute,
   (HashAlgoInit) sha3_512Init,
   (HashAlgoUpdate) sha3_512Update,
   (HashAlgoFinal) sha3_512Final
};


/**
 * @brief Digest a message using SHA3-512
 * @param[in] data Pointer to the message being hashed
 * @param[in] length Length of the message
 * @param[out] digest Pointer to the calculated digest
 * @return Error code
 **/

error_t sha3_512Compute(const void *data, size_t length, uint8_t *digest)
{
   Sha3_512Context *context=NULL;

   //Allocate a memory buffer to hold the SHA3-512 context

   context = (Sha3_512Context *)malloc( sizeof(Sha3_512Context) );
   //Failed to allocate memory?
   if(context == NULL)
      return ERROR_OUT_OF_MEMORY;

	memset( context, 0x00, sizeof(Sha3_512Context) ); // 2017.08.22
	
   //Initialize the SHA3-512 context
   sha3_512Init(context);
   //Digest the message
   sha3_512Update(context, data, length);
   //Finalize the SHA3-512 message digest
   sha3_512Final(context, digest);

   //Free previously allocated memory
   free( (void*) context);
   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Initialize SHA3-512 message digest context
 * @param[in] context Pointer to the SHA3-512 context to initialize
 **/

void sha3_512Init(Sha3_512Context *context)
{
   //The capacity of the sponge is twice the digest length
   keccakInit(context, 2 * 512);
}


/**
 * @brief Update the SHA3-512 context with a portion of the message being hashed
 * @param[in] context Pointer to the SHA3-512 context
 * @param[in] data Pointer to the buffer being hashed
 * @param[in] length Length of the buffer
 **/

void sha3_512Update(Sha3_512Context *context, const void *data, size_t length)
{
   //Absorb the input data
   keccakAbsorb(context, data, length);
}


/**
 * @brief Finish the SHA3-512 message digest
 * @param[in] context Pointer to the SHA3-512 context
 * @param[out] digest Calculated digest (optional parameter)
 **/

void sha3_512Final(Sha3_512Context *context, uint8_t *digest)
{
   //Finish absorbing phase (padding byte is 0x06 for SHA-3)
   keccakFinal(context, KECCAK_SHA3_PAD);
   //Extract data from the squeezing phase
   keccakSqueeze(context, digest, SHA3_512_DIGEST_SIZE);
}

#endif






/**
 * @file sha3_384.c
 * @brief SHA3-384 hash function (SHA-3 with 384-bit output)
 *
 * @section License
 *
 * Copyright (C) 2010-2017 Oryx Embedded SARL. All rights reserved.
 *
 * This file is part of CycloneCrypto Open.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * @section Description
 *
 * SHA-3 is a secure hash algorithm for computing a condensed representation
 * of an electronic message. Refer to FIPS 202 for more details
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 1.7.8
 **/


//Check crypto library configuration
#if (SHA3_384_SUPPORT == ENABLED)

//SHA3-384 object identifier (2.16.840.1.101.3.4.2.9)
static const uint8_t sha3_384Oid[] = {0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x09};

//Common interface for hash algorithms
const HashAlgo sha3_384HashAlgo =
{
   "SHA3-384",
   sha3_384Oid,
   sizeof(sha3_384Oid),
   sizeof(Sha3_384Context),
   SHA3_384_BLOCK_SIZE,
   SHA3_384_DIGEST_SIZE,
   (HashAlgoCompute) sha3_384Compute,
   (HashAlgoInit) sha3_384Init,
   (HashAlgoUpdate) sha3_384Update,
   (HashAlgoFinal) sha3_384Final
};


/**
 * @brief Digest a message using SHA3-384
 * @param[in] data Pointer to the message being hashed
 * @param[in] length Length of the message
 * @param[out] digest Pointer to the calculated digest
 * @return Error code
 **/

error_t sha3_384Compute(const void *data, size_t length, uint8_t *digest)
{
   Sha3_384Context *context=NULL;

   //Allocate a memory buffer to hold the SHA3-384 context
   context = (Sha3_384Context *)malloc( (int)sizeof(Sha3_384Context) );
   //Failed to allocate memory?
   if(context == NULL)
      return ERROR_OUT_OF_MEMORY;

	memset( context, 0x00, sizeof(Sha3_384Context) ); // 2017.08.22


   //Initialize the SHA3-384 context
   sha3_384Init(context);
   //Digest the message
   sha3_384Update(context, data, length);
   //Finalize the SHA3-384 message digest
   sha3_384Final(context, digest);

   //Free previously allocated memory
   free( (void*) context);
   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Initialize SHA3-384 message digest context
 * @param[in] context Pointer to the SHA3-384 context to initialize
 **/

void sha3_384Init(Sha3_384Context *context)
{
   //The capacity of the sponge is twice the digest length
   keccakInit(context, 2 * 384);
}


/**
 * @brief Update the SHA3-384 context with a portion of the message being hashed
 * @param[in] context Pointer to the SHA3-384 context
 * @param[in] data Pointer to the buffer being hashed
 * @param[in] length Length of the buffer
 **/

void sha3_384Update(Sha3_384Context *context, const void *data, size_t length)
{
   //Absorb the input data
   keccakAbsorb(context, data, length);
}


/**
 * @brief Finish the SHA3-384 message digest
 * @param[in] context Pointer to the SHA3-384 context
 * @param[out] digest Calculated digest (optional parameter)
 **/

void sha3_384Final(Sha3_384Context *context, uint8_t *digest)
{
   //Finish absorbing phase (padding byte is 0x06 for SHA-3)
   keccakFinal(context, KECCAK_SHA3_PAD);
   //Extract data from the squeezing phase
   keccakSqueeze(context, digest, SHA3_384_DIGEST_SIZE);
}

#endif


/**
 * @file sha3_256.c
 * @brief SHA3-256 hash function (SHA-3 with 256-bit output)
 *
 * @section License
 *
 * Copyright (C) 2010-2017 Oryx Embedded SARL. All rights reserved.
 *
 * This file is part of CycloneCrypto Open.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * @section Description
 *
 * SHA-3 is a secure hash algorithm for computing a condensed representation
 * of an electronic message. Refer to FIPS 202 for more details
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 1.7.8
 **/



//Check crypto library configuration
#if (SHA3_256_SUPPORT == ENABLED)

//SHA3-256 object identifier (2.16.840.1.101.3.4.2.8)
static const uint8_t sha3_256Oid[] = {0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x08};

//Common interface for hash algorithms
const HashAlgo sha3_256HashAlgo =
{
   "SHA3-256",
   sha3_256Oid,
   sizeof(sha3_256Oid),
   sizeof(Sha3_256Context),
   SHA3_256_BLOCK_SIZE,
   SHA3_256_DIGEST_SIZE,
   (HashAlgoCompute) sha3_256Compute,
   (HashAlgoInit) sha3_256Init,
   (HashAlgoUpdate) sha3_256Update,
   (HashAlgoFinal) sha3_256Final
};


/**
 * @brief Digest a message using SHA3-256
 * @param[in] data Pointer to the message being hashed
 * @param[in] length Length of the message
 * @param[out] digest Pointer to the calculated digest
 * @return Error code
 **/

error_t sha3_256Compute(const void *data, size_t length, uint8_t *digest)
{
   Sha3_256Context *context=NULL;

   //Allocate a memory buffer to hold the SHA3-256 context
   context = (Sha3_256Context *)malloc( sizeof(Sha3_256Context) );
   //Failed to allocate memory?
   if(context == NULL)
      return ERROR_OUT_OF_MEMORY;

   
   memset( context, 0x00, sizeof(Sha3_256Context) ); // 2017.08.22

   //Initialize the SHA3-256 context
   sha3_256Init(context);
   //Digest the message
   sha3_256Update(context, data, length);
   //Finalize the SHA3-256 message digest
   sha3_256Final(context, digest);

   //Free previously allocated memory
   free( (void*) context);
   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Initialize SHA3-256 message digest context
 * @param[in] context Pointer to the SHA3-256 context to initialize
 **/

void sha3_256Init(Sha3_256Context *context)
{
   //The capacity of the sponge is twice the digest length
   keccakInit(context, 2 * 256);
}


/**
 * @brief Update the SHA3-256 context with a portion of the message being hashed
 * @param[in] context Pointer to the SHA3-256 context
 * @param[in] data Pointer to the buffer being hashed
 * @param[in] length Length of the buffer
 **/

void sha3_256Update(Sha3_256Context *context, const void *data, size_t length)
{
   //Absorb the input data
   keccakAbsorb(context, data, length);
}


/**
 * @brief Finish the SHA3-256 message digest
 * @param[in] context Pointer to the SHA3-256 context
 * @param[out] digest Calculated digest (optional parameter)
 **/

void sha3_256Final(Sha3_256Context *context, uint8_t *digest)
{
   //Finish absorbing phase (padding byte is 0x06 for SHA-3)
   keccakFinal(context, KECCAK_SHA3_PAD);
   //Extract data from the squeezing phase
   keccakSqueeze(context, digest, SHA3_256_DIGEST_SIZE);
}

#endif







/**
 * @file sha3_224.c
 * @brief SHA3-224 hash function (SHA-3 with 224-bit output)
 *
 * @section License
 *
 * Copyright (C) 2010-2017 Oryx Embedded SARL. All rights reserved.
 *
 * This file is part of CycloneCrypto Open.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * @section Description
 *
 * SHA-3 is a secure hash algorithm for computing a condensed representation
 * of an electronic message. Refer to FIPS 202 for more details
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 1.7.8
 **/



//Check crypto library configuration
#if (SHA3_224_SUPPORT == ENABLED)

//SHA3-224 object identifier (2.16.840.1.101.3.4.2.7)
static const uint8_t sha3_224Oid[] = {0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x07};

//Common interface for hash algorithms
const HashAlgo sha3_224HashAlgo =
{
   "SHA3-224",
   sha3_224Oid,
   sizeof(sha3_224Oid),
   sizeof(Sha3_224Context),
   SHA3_224_BLOCK_SIZE,
   SHA3_224_DIGEST_SIZE,
   (HashAlgoCompute) sha3_224Compute,
   (HashAlgoInit) sha3_224Init,
   (HashAlgoUpdate) sha3_224Update,
   (HashAlgoFinal) sha3_224Final
};


/**
 * @brief Digest a message using SHA3-224
 * @param[in] data Pointer to the message being hashed
 * @param[in] length Length of the message
 * @param[out] digest Pointer to the calculated digest
 * @return Error code
 **/

error_t sha3_224Compute(const void *data, size_t length, uint8_t *digest)
{
   Sha3_224Context *context=NULL;

   //Allocate a memory buffer to hold the SHA3-224 context
   context = (Sha3_224Context *)malloc( sizeof(Sha3_224Context) );
   //Failed to allocate memory?
   if(context == NULL)
      return ERROR_OUT_OF_MEMORY;

   memset( context, 0x00, sizeof(Sha3_224Context) ); // 2017.08.22

   //Initialize the SHA3-224 context
   sha3_224Init(context);
   //Digest the message
   sha3_224Update(context, data, length);
   //Finalize the SHA3-224 message digest
   sha3_224Final(context, digest);

   //Free previously allocated memory
   free( (void*) context);
   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Initialize SHA3-224 message digest context
 * @param[in] context Pointer to the SHA3-224 context to initialize
 **/

void sha3_224Init(Sha3_224Context *context)
{
   //The capacity of the sponge is twice the digest length
   keccakInit(context, 2 * 224);
}


/**
 * @brief Update the SHA3-224 context with a portion of the message being hashed
 * @param[in] context Pointer to the SHA3-224 context
 * @param[in] data Pointer to the buffer being hashed
 * @param[in] length Length of the buffer
 **/

void sha3_224Update(Sha3_224Context *context, const void *data, size_t length)
{
   //Absorb the input data
   keccakAbsorb(context, data, length);
}


/**
 * @brief Finish the SHA3-224 message digest
 * @param[in] context Pointer to the SHA3-224 context
 * @param[out] digest Calculated digest (optional parameter)
 **/

void sha3_224Final(Sha3_224Context *context, uint8_t *digest)
{
   //Finish absorbing phase (padding byte is 0x06 for SHA-3)
   keccakFinal(context, KECCAK_SHA3_PAD);
   //Extract data from the squeezing phase
   keccakSqueeze(context, digest, SHA3_224_DIGEST_SIZE);
}

#endif



//Check crypto library configuration
#if (SHAKE256_SUPPORT == ENABLED)

//SHAKE256 object identifier (2.16.840.1.101.3.4.2.12)
static const uint8_t shake256Oid[9] = {0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x0C};


/**
 * @brief Digest a message using SHAKE256
 * @param[in] input Pointer to the input data
 * @param[in] inputLen Length of the input data
 * @param[out] output Pointer to the output data
 * @param[in] outputLen Expected length of the output data
 * @return Error code
 **/

error_t shake256Compute(const void *input, size_t inputLen,
   uint8_t *output, size_t outputLen)
{
   Shake256Context *context;

   //Allocate a memory buffer to hold the SHAKE256 context
   context = (Shake256Context *)malloc( sizeof(Shake256Context) );
   //Failed to allocate memory?
   if(context == NULL)
      return ERROR_OUT_OF_MEMORY;

   memset( context, 0x00, sizeof(Shake256Context) ); // 2017.08.22

   //Initialize the SHAKE256 context
   shake256Init(context);
   //Absorb input data
   shake256Absorb(context, input, inputLen);
   //Finish absorbing phase
   shake256Final(context);
   //Extract data from the squeezing phase
   shake256Squeeze(context, output, outputLen);

   //Free previously allocated memory
   free( (void*) context);
   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Initialize SHAKE256 context
 * @param[in] context Pointer to the SHAKE256 context to initialize
 **/

void shake256Init(Shake256Context *context)
{
   //SHAKE256 supports 256 bits of security strength
   keccakInit(context, 2 * 256);
}


/**
 * @brief Absorb data
 * @param[in] context Pointer to the SHAKE256 context
 * @param[in] input Pointer to the buffer being hashed
 * @param[in] length Length of the buffer
 **/

void shake256Absorb(Shake256Context *context, const void *input, size_t length)
{
   //Absorb the input data
   keccakAbsorb(context, input, length);
}


/**
 * @brief Finish absorbing phase
 * @param[in] context Pointer to the SHAKE256 context
 **/

void shake256Final(Shake256Context *context)
{
   //Finish absorbing phase (padding byte is 0x1F for XOFs)
   keccakFinal(context, KECCAK_XOF_PAD);
}


/**
 * @brief Extract data from the squeezing phase
 * @param[in] context Pointer to the SHAKE256 context
 * @param[out] output Output string
 * @param[in] length Desired output length, in bytes
 **/

void shake256Squeeze(Shake256Context *context, uint8_t *output, size_t length)
{
   //Extract data from the squeezing phase
   keccakSqueeze(context, output, length);
}

#endif



//Check crypto library configuration
#if (SHAKE128_SUPPORT == ENABLED)

//SHAKE128 object identifier (2.16.840.1.101.3.4.2.11)
static const uint8_t shake128Oid[9] = {0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x0B};


/**
 * @brief Digest a message using SHAKE128
 * @param[in] input Pointer to the input data
 * @param[in] inputLen Length of the input data
 * @param[out] output Pointer to the output data
 * @param[in] outputLen Expected length of the output data
 * @return Error code
 **/

error_t shake128Compute(const void *input, size_t inputLen,
   uint8_t *output, size_t outputLen)
{
   Shake128Context *context;

   //Allocate a memory buffer to hold the SHAKE128 context
   context = (Shake128Context *)malloc(sizeof(Shake128Context));
   //Failed to allocate memory?
   if(context == NULL)
      return ERROR_OUT_OF_MEMORY;

   memset( context, 0x00, sizeof(Shake128Context) ); // 2017.08.22

   //Initialize the SHAKE128 context
   shake128Init(context);
   //Absorb input data
   shake128Absorb(context, input, inputLen);
   //Finish absorbing phase
   shake128Final(context);
   //Extract data from the squeezing phase
   shake128Squeeze(context, output, outputLen);

   //Free previously allocated memory
   free( (void*) context);
   //Successful processing
   return NO_ERROR;
}


/**
 * @brief Initialize SHAKE128 context
 * @param[in] context Pointer to the SHAKE128 context to initialize
 **/

void shake128Init(Shake128Context *context)
{
   //SHAKE128 supports 128 bits of security strength
   keccakInit(context, 2 * 128);
}


/**
 * @brief Absorb data
 * @param[in] context Pointer to the SHAKE128 context
 * @param[in] input Pointer to the buffer being hashed
 * @param[in] length Length of the buffer
 **/

void shake128Absorb(Shake128Context *context, const void *input, size_t length)
{
   //Absorb the input data
   keccakAbsorb(context, input, length);
}


/**
 * @brief Finish absorbing phase
 * @param[in] context Pointer to the SHAKE128 context
 **/

void shake128Final(Shake128Context *context)
{
   //Finish absorbing phase (padding byte is 0x1F for XOFs)
   keccakFinal(context, KECCAK_XOF_PAD);
}


/**
 * @brief Extract data from the squeezing phase
 * @param[in] context Pointer to the SHAKE128 context
 * @param[out] output Output string
 * @param[in] length Desired output length, in bytes
 **/

void shake128Squeeze(Shake128Context *context, uint8_t *output, size_t length)
{
   //Extract data from the squeezing phase
   keccakSqueeze(context, output, length);
}

#endif



#ifdef __cplusplus
}
#endif




