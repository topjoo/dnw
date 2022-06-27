/**
 * @file keccak.c
 * @brief Keccak sponge function
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
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 1.7.8
 **/

//Switch to the appropriate trace level
//#define TRACE_LEVEL CRYPTO_TRACE_LEVEL

//Dependencies
#define STRICT
#define WIN32_LEAN_AND_MEAN

//Dependencies
//#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "crypto.h"
#include "keccak.h"
#include "endian.h" // 2017.08.22




//void EB_Printf(const TCHAR *fmt,...); //engine.h


#ifdef __cplusplus
extern "C"
{
#endif



#ifndef MIN /* if macro not already defined */ 
#define MIN(a,b) ( (a)<(b) ? (a) : (b) ) 
#endif /* if min macro not already defined */ 

#ifndef MAX
#define MAX(a,b) ( (a)>(b) ? (a) : (b) ) 
#endif




#define ENABLED 				1
#define KECCAK_SUPPORT 			ENABLED


//Check crypto library configuration
#if (KECCAK_SUPPORT == ENABLED)

//Keccak round constants
static const keccak_lane_t rc[KECCAK_NR] =
{
   (keccak_lane_t) 0x0000000000000001ULL,
   (keccak_lane_t) 0x0000000000008082ULL,
   (keccak_lane_t) 0x800000000000808AULL,
   (keccak_lane_t) 0x8000000080008000ULL,
   (keccak_lane_t) 0x000000000000808BULL,
   (keccak_lane_t) 0x0000000080000001ULL,
   (keccak_lane_t) 0x8000000080008081ULL,
   (keccak_lane_t) 0x8000000000008009ULL,
   (keccak_lane_t) 0x000000000000008AULL,
   (keccak_lane_t) 0x0000000000000088ULL,
   (keccak_lane_t) 0x0000000080008009ULL,
   (keccak_lane_t) 0x000000008000000AULL,
   (keccak_lane_t) 0x000000008000808BULL,
   (keccak_lane_t) 0x800000000000008BULL,
   (keccak_lane_t) 0x8000000000008089ULL,
   (keccak_lane_t) 0x8000000000008003ULL,
   (keccak_lane_t) 0x8000000000008002ULL,
   (keccak_lane_t) 0x8000000000000080ULL,
#if (KECCAK_L >= 4)
   (keccak_lane_t) 0x000000000000800AULL,
   (keccak_lane_t) 0x800000008000000AULL,
#endif
#if (KECCAK_L >= 5)
   (keccak_lane_t) 0x8000000080008081ULL,
   (keccak_lane_t) 0x8000000000008080ULL,
#endif
#if (KECCAK_L >= 6)
   (keccak_lane_t) 0x0000000080000001ULL,
   (keccak_lane_t) 0x8000000080008008ULL
#endif
};


/**
 * @brief Apply theta transformation
 * @param[in,out] a State array
 **/

static void theta(keccak_lane_t aa[5][5])
{
   keccak_lane_t cc[5];
   keccak_lane_t dd[5];

   //The effect of the theta transformation is to XOR each bit in the
   //state with the parities of two columns in the array
   cc[0] = aa[0][0] ^ aa[1][0] ^ aa[2][0] ^ aa[3][0] ^ aa[4][0];
   cc[1] = aa[0][1] ^ aa[1][1] ^ aa[2][1] ^ aa[3][1] ^ aa[4][1];
   cc[2] = aa[0][2] ^ aa[1][2] ^ aa[2][2] ^ aa[3][2] ^ aa[4][2];
   cc[3] = aa[0][3] ^ aa[1][3] ^ aa[2][3] ^ aa[3][3] ^ aa[4][3];
   cc[4] = aa[0][4] ^ aa[1][4] ^ aa[2][4] ^ aa[3][4] ^ aa[4][4];

   dd[0] = cc[4] ^ KECCAK_ROL(cc[1], 1);
   dd[1] = cc[0] ^ KECCAK_ROL(cc[2], 1);
   dd[2] = cc[1] ^ KECCAK_ROL(cc[3], 1);
   dd[3] = cc[2] ^ KECCAK_ROL(cc[4], 1);
   dd[4] = cc[3] ^ KECCAK_ROL(cc[0], 1);

   aa[0][0] ^= dd[0];
   aa[1][0] ^= dd[0];
   aa[2][0] ^= dd[0];
   aa[3][0] ^= dd[0];
   aa[4][0] ^= dd[0];

   aa[0][1] ^= dd[1];
   aa[1][1] ^= dd[1];
   aa[2][1] ^= dd[1];
   aa[3][1] ^= dd[1];
   aa[4][1] ^= dd[1];

   aa[0][2] ^= dd[2];
   aa[1][2] ^= dd[2];
   aa[2][2] ^= dd[2];
   aa[3][2] ^= dd[2];
   aa[4][2] ^= dd[2];

   aa[0][3] ^= dd[3];
   aa[1][3] ^= dd[3];
   aa[2][3] ^= dd[3];
   aa[3][3] ^= dd[3];
   aa[4][3] ^= dd[3];

   aa[0][4] ^= dd[4];
   aa[1][4] ^= dd[4];
   aa[2][4] ^= dd[4];
   aa[3][4] ^= dd[4];
   aa[4][4] ^= dd[4];
}


/**
 * @brief Apply rho transformation
 * @param[in,out] a State array
 **/

static void rho(keccak_lane_t aa[5][5])
{
   //The effect of the rho transformation is to rotate the bits of each lane by
   //an offset, which depends on the fixed x and y coordinates of the lane
   aa[0][1] = KECCAK_ROL(aa[0][1], 1   % KECCAK_W);
   aa[0][2] = KECCAK_ROL(aa[0][2], 190 % KECCAK_W);
   aa[0][3] = KECCAK_ROL(aa[0][3], 28  % KECCAK_W);
   aa[0][4] = KECCAK_ROL(aa[0][4], 91  % KECCAK_W);

   aa[1][0] = KECCAK_ROL(aa[1][0], 36  % KECCAK_W);
   aa[1][1] = KECCAK_ROL(aa[1][1], 300 % KECCAK_W);
   aa[1][2] = KECCAK_ROL(aa[1][2], 6   % KECCAK_W);
   aa[1][3] = KECCAK_ROL(aa[1][3], 55  % KECCAK_W);
   aa[1][4] = KECCAK_ROL(aa[1][4], 276 % KECCAK_W);

   aa[2][0] = KECCAK_ROL(aa[2][0], 3   % KECCAK_W);
   aa[2][1] = KECCAK_ROL(aa[2][1], 10  % KECCAK_W);
   aa[2][2] = KECCAK_ROL(aa[2][2], 171 % KECCAK_W);
   aa[2][3] = KECCAK_ROL(aa[2][3], 153 % KECCAK_W);
   aa[2][4] = KECCAK_ROL(aa[2][4], 231 % KECCAK_W);

   aa[3][0] = KECCAK_ROL(aa[3][0], 105 % KECCAK_W);
   aa[3][1] = KECCAK_ROL(aa[3][1], 45  % KECCAK_W);
   aa[3][2] = KECCAK_ROL(aa[3][2], 15  % KECCAK_W);
   aa[3][3] = KECCAK_ROL(aa[3][3], 21  % KECCAK_W);
   aa[3][4] = KECCAK_ROL(aa[3][4], 136 % KECCAK_W);

   aa[4][0] = KECCAK_ROL(aa[4][0], 210 % KECCAK_W);
   aa[4][1] = KECCAK_ROL(aa[4][1], 66  % KECCAK_W);
   aa[4][2] = KECCAK_ROL(aa[4][2], 253 % KECCAK_W);
   aa[4][3] = KECCAK_ROL(aa[4][3], 120 % KECCAK_W);
   aa[4][4] = KECCAK_ROL(aa[4][4], 78  % KECCAK_W);
}


/**
 * @brief Apply pi transformation
 * @param[in,out] a State array
 **/

static void pi(keccak_lane_t aa[5][5])
{
   keccak_lane_t temp;

   //The effect of the pi transformation is to rearrange the
   //positions of the lanes
   temp = aa[0][1];
   aa[0][1] = aa[1][1];
   aa[1][1] = aa[1][4];
   aa[1][4] = aa[4][2];
   aa[4][2] = aa[2][4];
   aa[2][4] = aa[4][0];
   aa[4][0] = aa[0][2];
   aa[0][2] = aa[2][2];
   aa[2][2] = aa[2][3];
   aa[2][3] = aa[3][4];
   aa[3][4] = aa[4][3];
   aa[4][3] = aa[3][0];
   aa[3][0] = aa[0][4];
   aa[0][4] = aa[4][4];
   aa[4][4] = aa[4][1];
   aa[4][1] = aa[1][3];
   aa[1][3] = aa[3][1];
   aa[3][1] = aa[1][0];
   aa[1][0] = aa[0][3];
   aa[0][3] = aa[3][3];
   aa[3][3] = aa[3][2];
   aa[3][2] = aa[2][1];
   aa[2][1] = aa[1][2];
   aa[1][2] = aa[2][0];
   aa[2][0] = temp;
}


/**
 * @brief Apply chi transformation
 * @param[in,out] a State array
 **/

static void chi(keccak_lane_t aa[5][5])
{
   keccak_lane_t temp1;
   keccak_lane_t temp2;

   //The effect of the chi transformation is to XOR each bit with
   //a non linear function of two other bits in its row
   temp1 = aa[0][0];
   temp2 = aa[0][1];
   aa[0][0] ^= ~aa[0][1] & aa[0][2];
   aa[0][1] ^= ~aa[0][2] & aa[0][3];
   aa[0][2] ^= ~aa[0][3] & aa[0][4];
   aa[0][3] ^= ~aa[0][4] & temp1;
   aa[0][4] ^= ~temp1 & temp2;

   temp1 = aa[1][0];
   temp2 = aa[1][1];
   aa[1][0] ^= ~aa[1][1] & aa[1][2];
   aa[1][1] ^= ~aa[1][2] & aa[1][3];
   aa[1][2] ^= ~aa[1][3] & aa[1][4];
   aa[1][3] ^= ~aa[1][4] & temp1;
   aa[1][4] ^= ~temp1 & temp2;

   temp1 = aa[2][0];
   temp2 = aa[2][1];
   aa[2][0] ^= ~aa[2][1] & aa[2][2];
   aa[2][1] ^= ~aa[2][2] & aa[2][3];
   aa[2][2] ^= ~aa[2][3] & aa[2][4];
   aa[2][3] ^= ~aa[2][4] & temp1;
   aa[2][4] ^= ~temp1 & temp2;

   temp1 = aa[3][0];
   temp2 = aa[3][1];
   aa[3][0] ^= ~aa[3][1] & aa[3][2];
   aa[3][1] ^= ~aa[3][2] & aa[3][3];
   aa[3][2] ^= ~aa[3][3] & aa[3][4];
   aa[3][3] ^= ~aa[3][4] & temp1;
   aa[3][4] ^= ~temp1 & temp2;

   temp1 = aa[4][0];
   temp2 = aa[4][1];
   aa[4][0] ^= ~aa[4][1] & aa[4][2];
   aa[4][1] ^= ~aa[4][2] & aa[4][3];
   aa[4][2] ^= ~aa[4][3] & aa[4][4];
   aa[4][3] ^= ~aa[4][4] & temp1;
   aa[4][4] ^= ~temp1 & temp2;
}


/**
 * @brief Apply iota transformation
 * @param[in,out] a State array
 * @param[index] round Round index
 **/

static void iota(keccak_lane_t aa[5][5], uint_t index)
{
   //The iota transformation is parameterized by the round index
   aa[0][0] ^= rc[index];
}


/**
 * @brief Initialize Keccak context
 * @param[in] context Pointer to the Keccak context to initialize
 * @param[in] capacity Capacity of the sponge function
 **/

error_t keccakInit(KeccakContext *context, uint_t capacity)
{
   uint_t rate;

   //Clear Keccak context
   memset(context, 0, sizeof(KeccakContext));

   //The capacity cannot exceed the width of a Keccak-p permutation
   if(capacity > KECCAK_B)
      return ERROR_INVALID_PARAMETER;

   //The rate depends on the capacity of the sponge function
   rate = KECCAK_B - capacity;

   //The rate must be multiple of the lane size
   if((rate % KECCAK_W) != 0)
      return ERROR_INVALID_PARAMETER;

   //Save the block size, in bytes
   context->blockSize = rate / 8;

   //Successful initialization
   return NO_ERROR;
}


/**
 * @brief Absorb data
 * @param[in] context Pointer to the Keccak context
 * @param[in] input Pointer to the buffer being hashed
 * @param[in] length Length of the buffer
 **/

void keccakAbsorb(KeccakContext *context, const void *input, size_t length)
{
   uint_t i;
   size_t n;
   keccak_lane_t *a;

   //Point to the state array
   a = (keccak_lane_t *) context->a;

   //Absorbing phase
   while(length > 0)
   {
      //Limit the number of bytes to process at a time
      n = MIN(length, context->blockSize - context->length);

      //Copy the data to the buffer
      memcpy(context->buffer + context->length, input, n);

      //Number of data bytes that have been buffered
      context->length += n;

      //Advance the data pointer
      input = (uint8_t *) input + n;
      //Remaining bytes to process
      length -= n;

      //Absorb the message block by block
      if(context->length == context->blockSize)
      {
         //Absorb the current block
         for(i = 0; i < context->blockSize / sizeof(keccak_lane_t); i++)
            a[i] ^= KECCAK_LETOH(context->block[i]);

         //Apply block permutation function
         keccakPermutBlock(context);

         //The input buffer is empty
         context->length = 0;
      }
   }
}


/**
 * @brief Finish absorbing phase
 * @param[in] context Pointer to the Keccak context
 * @param[in] pad Value of the padding byte (0x01 for Keccak, 0x06 for SHA-3 and 0x1F for XOF)
 **/

void keccakFinal(KeccakContext *context, uint8_t pad)
{
   uint_t i;
   size_t q;
   keccak_lane_t *a;

   //Point to the state array
   a = (keccak_lane_t *) context->a;

   //Compute the number of padding bytes
   q = context->blockSize - context->length;

   //Append padding
   memset(context->buffer + context->length, 0, q);
   context->buffer[context->length] |= pad;
   context->buffer[context->blockSize - 1] |= 0x80;

   //Absorb the final block
   for(i = 0; i < context->blockSize / sizeof(keccak_lane_t); i++)
      a[i] ^= KECCAK_LETOH(context->block[i]);

   //Apply block permutation function
   keccakPermutBlock(context);

   //Convert lanes to little-endian byte order
   for(i = 0; i < context->blockSize / sizeof(keccak_lane_t); i++)
      a[i] = KECCAK_HTOLE(a[i]);

   //Number of bytes available in the output buffer
   context->length = context->blockSize;
}


/**
 * @brief Extract data from the squeezing phase
 * @param[in] context Pointer to the Keccak context
 * @param[out] output Output string
 * @param[in] length Desired output length, in bytes
 **/

void keccakSqueeze(KeccakContext *context, uint8_t *output, size_t length)
{
   uint_t i;
   size_t n;
   keccak_lane_t *a=NULL;

   //Point to the state array
   a = (keccak_lane_t *) context->a;

   //An arbitrary number of output bits can be squeezed out of the state
   while(length > 0)
   {
      //Check whether more data is required
      if(context->length == 0)
      {
         //Convert lanes to host byte order
         for(i = 0; i < context->blockSize / sizeof(keccak_lane_t); i++)
            a[i] = KECCAK_LETOH(a[i]);

         //Apply block permutation function
         keccakPermutBlock(context);

         //Convert lanes to little-endian byte order
         for(i = 0; i < context->blockSize / sizeof(keccak_lane_t); i++)
            a[i] = KECCAK_HTOLE(a[i]);

         //Number of bytes available in the output buffer
         context->length = context->blockSize;
      }

      //Compute the number of bytes to process at a time
      n = MIN(length, context->length);

      //Copy the output string
      if(output != NULL)
         memcpy(output, context->digest + context->blockSize - context->length, n);

      //Number of bytes available in the output buffer
      context->length -= n;

      //Advance the data pointer
      output = (uint8_t *) output + n;
      //Number of bytes that remains to be written
      length -= n;
   }
}


/**
 * @brief Block permutation
 * @param[in] context Pointer to the Keccak context
 **/

void keccakPermutBlock(KeccakContext *context)
{
   uint_t i;

   //Each round consists of a sequence of five transformations,
   //which are called the step mappings
   for(i = 0; i < KECCAK_NR; i++)
   {
      //Apply theta step mapping
      theta(context->a);
      //Apply rho step mapping
      rho(context->a);
      //Apply pi step mapping
      pi(context->a);
      //Apply chi step mapping
      chi(context->a);
      //Apply iota step mapping
      iota(context->a, i);
   }
}

#endif


#ifdef __cplusplus
}
#endif

