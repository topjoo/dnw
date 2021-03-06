
#ifndef _KECCAK_H
#define _KECCAK_H

//Dependencies
#include "crypto.h"

//The binary logarithm of the lane size
#ifndef KECCAK_L
   #define KECCAK_L 6
#endif

//Check lane size
#if (KECCAK_L == 3)
   //Base type that represents a lane
   typedef uint8_t keccak_lane_t;
   //Rotate left operation
   #define KECCAK_ROL(a, n) ROL8(a, n)
   //Host byte order to little-endian byte order
   #define KECCAK_HTOLE(a) (a)
   //Little-endian byte order to host byte order
   #define KECCAK_LETOH(a) (a)
#elif (KECCAK_L == 4)
   //Base type that represents a lane
   #define keccak_lane_t uint16_t
   //Rotate left operation
   #define KECCAK_ROL(a, n) ROL16(a, n)
   //Host byte order to little-endian byte order
   #define KECCAK_HTOLE(a) htole16(a)
   //Little-endian byte order to host byte order
   #define KECCAK_LETOH(a) letoh16(a)
#elif (KECCAK_L == 5)
   //Base type that represents a lane
   #define keccak_lane_t uint32_t
   //Rotate left operation
   #define KECCAK_ROL(a, n) ROL32(a, n)
   //Host byte order to little-endian byte order
   #define KECCAK_HTOLE(a) htole32(a)
   //Little-endian byte order to host byte order
   #define KECCAK_LETOH(a) letoh32(a)
#elif (KECCAK_L == 6)
   //Base type that represents a lane
   #define keccak_lane_t uint64_t
   //Rotate left operation
   #define KECCAK_ROL(a, n) ROL64(a, n)
   //Host byte order to little-endian byte order
   #define KECCAK_HTOLE(a) htole64(a)
   //Little-endian byte order to host byte order
   #define KECCAK_LETOH(a) letoh64(a)
#else
   #error KECCAK_L parameter is not valid
#endif

//The lane size of a Keccak-p permutation in bits
#define KECCAK_W (1 << KECCAK_L)
//The width of a Keccak-p permutation
#define KECCAK_B (KECCAK_W * 25)
//The number of rounds for a Keccak-p permutation
#define KECCAK_NR (12 + 2 * KECCAK_L)

//Keccak padding byte
#define KECCAK_PAD 0x01
//SHA-3 padding byte
#define KECCAK_SHA3_PAD 0x06
//XOF padding byte
#define KECCAK_XOF_PAD 0x1F


/**
 * @brief Keccak context
 **/

typedef struct
{
   union
   {
      keccak_lane_t a[5][5];
      uint8_t digest[1];
   };
   union
   {
      keccak_lane_t block[24];
      uint8_t buffer[1];
   };
   uint_t blockSize;
   size_t length;
} KeccakContext;


//Keccak related functions
error_t keccakInit(KeccakContext *context, uint_t capacity);
void keccakAbsorb(KeccakContext *context, const void *input, size_t length);
void keccakFinal(KeccakContext *context, uint8_t pad);
void keccakSqueeze(KeccakContext *context, uint8_t *output, size_t length);
void keccakPermutBlock(KeccakContext *context);

#endif
