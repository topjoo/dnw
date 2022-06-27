/* ========================================================
 * AttachHdr.exe Tool
 *
 * Yoon-Hwan Joo (tp.joo@daum.net or toppy_joo@naver.com) 
 *  - 2008/12/05 : Attached Header Created.
 *  - 2009/10/09 : Added CRC Generator
 *  - 2010/03/04 : Added Line Counter
 *  - 2010/05/31 : NK.bin Information
 *  - 2010/06/04 : NK.BIN -> NK.NB0 
 *  - 2011/05/17 : Change Title
 *  - 2012/01/27 : Hex2Bin
 *  - 2012/02/09 : Update Hex2Bin (Motorola and Intel family)
 *  - 2012/05/28 : Update help
 *  - 2012/06/16 : Convert Float number to hex-deciaml for DR_GPS Trimble packets
 *  - 2013/04/12 : Copy file to new filename.
 *  - 2013/07/09 : Convert BMP file to C text file.
 *  - 2014/06/26 : MD5 hash
 *                        MD5(Message-Digest algorithm 5) is 128bit enciption hash function. 
 *                        RFC 1321로 지정, 주로 프로그램이나 파일이 원본 그대로인지를 확인하는 무결성 검사 등에 사용. 
 *  - 2014/06/30 : SHA1 hash
 *  - 2014/06/30 : SHA256/384/512 hash
 *  - 2014/07/01 : Change AttachHdr.c to AHdr.c
 *  - 2014/07/04 : Modified Julian Date
 *  - 2014/07/08 : MJD -> Hex decimal printf
 *  - 2014/07/10 : Checking input filename and outputfile name
 *  - 2014/07/14 : Bug fix for option. check memcpy()
 *  - 2014/07/16 : Change buf name to  md5_buf, sha1_buf, sha256_buf, sha384_buf, sha512_buf
 *  - 2014/07/18 : nk.bin structure bug fix.
 *  - 2014/07/21 : nk.bin addtional information
 *  - 2014/07/23 : mot2bin, hex2bin : Version 1.0.6 -> Version 1.0.10
 *                 CRC auto checking
 *  - 2014/07/24 : version 1.0.0 fix.
 *  - 2014/07/24 : added CRC32, CRC64, CRC16CCITT, ADLER32(zlib)
 *  - 2014/07/26 : added MD4
 *  - 2014/07/28 : If Size Error in Hex2bin or Mot2bin, hex2bin is ended.
 *  - 2014/07/29 : fgets() NULL check for hex2bin and mot2bin
 *  - 2014/07/29 : multi-input bug fixed.  current dir, ./..
 *  - 2014/07/29 : MD2 added.
 *  - 2014/07/31 : MD6 hash added
 *  - 2014/08/05 : CRC is merged to checksum -M option
 *  - 2014/08/13 : elf to bin
 *  - 2015/01/20 : CRC16 unlimit file.
 *  - 2015/02/11 : Detach
 *  - 2015/09/01 : MD5 progressing display because of big file.
 *  - 2015/09/09 : MD5 progressing display update.
 *  - 2017/04/04 : Added checksum (CRC16, CRC16CCITT, CRC32, CRC64, ADLER32) to Header
 *  - 2017/04/05 : Merge from 2 bin files to 1 bin file 
 *  - 2017/04/12 : Merge bins bug fix
 *  - 2017/11/21 : "signed firmware" -> Encapsulation (JCR1110)
 *                 Version Name Length  16byte -> 32Byte
 *                 Checksum 16Bytes -> Hash Code (sha256/sha384/sha512/sha/224/md5/md6) Insertion
 *  - 2017/11/28 : File merge option fix
 *  - 2017/12/11 : File merge and 2nd file size added.
 *  - 2017/12/12 : Version Name Length  32byte -> 16Byte
 *  - 2020/05/28 : cygwin64 install
 *  - 2020/06/08 : $gcc --version
 *                  gcc (GCC) 3.4.4 (cygming special, gdc 0.12, using dmd 0.125)
 *                  Copyright (C) 2004 Free Software Foundation, Inc.
 *                  This is free software; see the source for copying conditions.  There is NO
 *                  warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *  - 2020/06/24 : upgrade intel and motorola hex to binary
 *  - 2020/07/02 : hex2bin --length option upgrade
 *  - 2020/07/06 : --fillsize option is upgraded
 *  - 2020/07/07 : --isCRCtype option is upgraded
 *  - 2020/07/14 : --szie option is deleted and --endian option is integrated
 *  - 2020/07/15 : --cinfo MD2 --cinfo MD4 is added.
 *  - 2020/07/16 : --cinfo CRC64ISC is added.
 *  - 2020/07/22 : --cinfo JOAAT hash is added.
 *  - 2020/07/30 : version 2.13 release on cafe.naver.com/topjoo.
 *  - 2020/09/08 : 2.15 building remove msg ([OUTPUT-WRITE] ELSE..)
 *  - 2020/09/09 : 2.15 --join option (hexa or 640kB or 1MB)
 *  - 2020/10/13 : 2.16 -o option -> create sub folder
 *  - 2020/10/23 : 2.17 SHA1, SHA224/256/384/512 read buffer size : 10KB -> 100KB
 *  - 2020/12/10 : 2.18 delete printf() [INPUT]...
 *  - 2021/10/07 : CAN dbc parser
 *
 *
 * How to build :
 *     1) Install cygwin
 *     2) command as below:
 *        gcc -o AttachHdr -mno-cygwin AttachHdr.c  or
 *        gcc-3  -o AttachHdr -mno-cygwin AttachHdr.c 
 *
 * How to profiling
 *     1) gcc -pg -g -o AttachHdr -mno-cygwin AttachHdr.c 
 *     2) AttachHdr  -> running and then create file, gmon.out.
 *     3) gprof -p -b AttachHdr.exe
 *     4) result is as below:
 *         Flat profile:
 *
 *       Each sample counts as 0.01 seconds. 
 *         no time accumulated  
 *
 *       %   cumulative   self              self     total            
 *    time   seconds   seconds    calls  Ts/call  Ts/call  name      
 *    0.00      0.00     0.00    3901932     0.00     0.00  sha1_process  
 *    0.00      0.00     0.00     243914     0.00     0.00  sha1_update  
 *    0.00      0.00     0.00            19     0.00     0.00  sha1_finish  
 *    0.00      0.00     0.00            19     0.00     0.00  sha1_starts  
 *    0.00      0.00     0.00              1     0.00     0.00  help_brief
 *
 * TOP.JOO
 * ========================================================
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> /// for getopt()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <time.h> 
#include <math.h>  
#include <assert.h>	/* assert() */
#include <unistd.h>
#include <getopt.h> /// getopt_long()
#include <memory.h> 
#include <stdarg.h>




#define _MOT2BIN_ 					1
#define MOT2BIN_1_0_10 				1 /// added 2014.07.23
#define _INTEL2BIN_ 				1
#define HEX2BIN_1_0_10 				1 /// added 2014.07.23
#define ERROR_HEX2BIN_EXIT 			1 /// added 2014.07.28, if Size Error, Exit
#define ELF2BIN 					1 /// 2014.08.14

#define CONVERT_BMP2C 				0 /// not supported!!
#define MD5_CHECKSUM_ENCIPHER 		1
#define MD5_MULTI_INPUT_FILES 		1

#define POLARSSL_SHA1_ENCIPHER 		1 /// 2014.06.29
#define SHA1_MULTI_INPUT_FILES 		1

#define SHA2_256_384_512 			1 /// 2014.06.30
#define SHA2_MULTI_INPUT_FILES 		1

#define SHA2_224_CHECKSUM 			1 /// 2014.07.21

#define SHA3_KECCAK_224_256_384_512 	1 // 2020.06.11



#define MD4_CHECKSUM_ENCIPHER 		1 /// 2014.07.26
#define MD2_CHECKSUM_ENCIPHER 		1 /// 2014.07.29

///#define IDEA_ENCIPHER_ALGORITHM 	1 /// 2014.07.26

///#define SHA0_OLD_ENCIPHER 			1


#define MD6_CHECKSUM_ENCIPHER 		1 /// 2014.07.31


#define MODIFIED_JULIAN_DATE 		1 /// 2014.07.04, Modified Julian Date ---
#define MJD_SAMPLE_NUMBER 			10	/// sample number ---
#define CONVERT_HEX_MSB 			11
#define CONVERT_HEX_LSB				22
#define CONVERT_HEX					CONVERT_HEX_LSB

#define CHECKSUM_PROGRESS_DISPLAY 	1 //-- 2015.09.01

#define BLAKE_224_256_384_512_HASH 	1 // 2020.07.27

#define RIPEMD160 					1 // 2020.07.23
#define RIPEMD128 					1 // 2020.07.31
#define RIPEMD320 					0

/*--------------------------------------------------------------------------
                     INCLUDE FILES 
---------------------------------------------------------------------------*/
#define CRC_BUFSIZE 			(32*1024)

#define BUFSIZE 				(1024*2)
#define SHA_READ_BUFSIZ 		(100*1024)
#define MD_HASH_BUFSIZ 			(100*1024)

/* ------------------------------------------------------------------------ */

FILE *inpfile = NULL;
FILE *outfile = NULL;
unsigned char *data_buf = NULL;

/* ------------------------------------------------------------------------ */

#if 0
#define SNDERR(...) snd_lib_error(__FILE__, __LINE__, __FUNCTION__, 0, __VA_ARGS__) /**< Shows a sound error message. */
#define SNDERR(args...) snd_lib_error(__FILE__, __LINE__, __FUNCTION__, 0, ##args) /**< Shows a sound error message. */
#endif

#if 0
printf("\033[31m"); /* RED */

printf("\033[32m"); /* GREEN */

printf("\033[34m"); /* BLUE */

printf("\033[35m"); /* ERROR */

printf("\033[36m"); /* NORMAL */

printf("\033[0m");  /* Start */

printf("\033[0m"); 
fflush(stdout); 
#endif


char Attversion[] = "2.23"; /* ver 2.21 : 2021.06.25 : Version up */
char EmailText[]  = "tp.joo@daum.net";


enum {
	MOT_HEX_S0 = 0x0001,
	MOT_HEX_S1 = 0x0002,
	MOT_HEX_S2 = 0x0004,
	MOT_HEX_S3 = 0x0008,
	MOT_HEX_S4 = 0x0010,
	MOT_HEX_S5 = 0x0020,
	MOT_HEX_S6 = 0x0040,
	MOT_HEX_S7 = 0x0080,
	MOT_HEX_S8 = 0x0100,
	MOT_HEX_S9 = 0x0200
};


#define HDR_CRC16 				0x0015
#define HDR_KSC_CRC16 			0x0016
#define HDR_CRC16CCITT 			0x0017
#define HDR_ADLER32 			0x0018 /// zlib
#define HDR_CRC32 				0x0019
#define HDR_CRC64 				0x001a
#define HDR_CRC64_ISC 			0x001b
#define HDR_JOAAT 				0x001c // 2020.07.22

#define HDR_SHA1 				0x0020 // 2020.06.10
#define HDR_SHA224 				0x0021
#define HDR_SHA256 				0x0022
#define HDR_SHA384 				0x0023
#define HDR_SHA512 				0x0024
#define HDR_SHA3_224 			0x0025
#define HDR_SHA3_256 			0x0026
#define HDR_SHA3_384 			0x0027
#define HDR_SHA3_512 			0x0028
#define HDR_SHAKE128 			0x0029
#define HDR_SHAKE256 			0x002a

#define HDR_MD2 				0x0030
#define HDR_MD4 				0x0031
#define HDR_MD5 				0x0032
#define HDR_MD6 				0x0033

#define HDR_BLAKE224 			0x0035
#define HDR_BLAKE256 			0x0036
#define HDR_BLAKE384 			0x0037
#define HDR_BLAKE512 			0x0038


#define HDR_RMD128 				0x0040
#define HDR_RMD160 				0x0041
#define HDR_RMD320 				0x0042

#define HDR_CRC_UNKNOWN 			0xffff



#define LOG_ERR(format, ...) 		fprintf(stderr, format "\n", ## __VA_ARGS__)
#define LOG_INF(format, ...)  		fprintf(stdout, format "\n", ## __VA_ARGS__)
#define LOG_V(format, ...) 			fprintf(stderr, format, ## __VA_ARGS__)


#define COMMA_BUF_SIZE  			256 /// 2014.07.23


typedef int boolean;
typedef unsigned char byte;
typedef unsigned short word;

typedef unsigned long  dword;

typedef unsigned char uint8_t;	/* 1-byte  (8-bits)  */
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;	/* 8-bytes (64-bits) */
typedef unsigned short int uint16_t;

typedef unsigned int uint16;	/* at LEAST 16 bits, maybe more */
typedef unsigned int word16;

typedef unsigned char uint8;
typedef unsigned int  uint32;
typedef unsigned long long uint64;

typedef unsigned int uint_t;
typedef unsigned char char_t;

typedef unsigned int error_t;


/* typedef a 32 bit type */
typedef unsigned long int UINT4;

typedef unsigned long ULONG;
typedef unsigned int UINT;
typedef signed short SHORT;
typedef unsigned short USHORT;
typedef void* 		PVOID;


typedef unsigned long word32;

// --------------------------------------------
// stdint.h
// 7.18.1.1 Exact-width integer types
typedef __int8            int8_t;
typedef __int16           int16_t;
typedef __int32           int32_t;
typedef __int64           int64_t;
/// typedef unsigned __int8   uint8_t;
/// typedef unsigned __int16  uint16_t;
/// typedef unsigned __int32  uint32_t;
/// typedef unsigned __int64  uint64_t;

#if 0
// 7.18.1.2 Minimum-width integer types
typedef int8_t    int_least8_t;
typedef int16_t   int_least16_t;
typedef int32_t   int_least32_t;
typedef int64_t   int_least64_t;
typedef uint8_t   uint_least8_t;
typedef uint16_t  uint_least16_t;
typedef uint32_t  uint_least32_t;
typedef uint64_t  uint_least64_t;

// 7.18.1.3 Fastest minimum-width integer types
typedef int8_t    int_fast8_t;
typedef int16_t   int_fast16_t;
typedef int32_t   int_fast32_t;
typedef int64_t   int_fast64_t;
typedef uint8_t   uint_fast8_t;
typedef uint16_t  uint_fast16_t;
typedef uint32_t  uint_fast32_t;
typedef uint64_t  uint_fast64_t;
#endif

// 7.18.1.4 Integer types capable of holding object pointers
#ifdef _WIN64 // [
   typedef __int64           intptr_t;
   typedef unsigned __int64  uintptr_t;
#else // _WIN64 ][
   typedef int               intptr_t;
   typedef unsigned int      uintptr_t;
#endif // _WIN64 ]

// 7.18.1.5 Greatest-width integer types
typedef int64_t   intmax_t;
typedef uint64_t  uintmax_t;
// --------------------------------------------




#ifndef min /* if macro not already defined */ 
#define min(a,b) ( (a)<(b) ? (a) : (b) ) 
#endif /* if min macro not already defined */ 

#ifndef max
#define max(a,b) ( (a)>(b) ? (a) : (b) ) 
#endif


#define	WORD	unsigned short
#define	DWORD	unsigned long



int verbose = 0;
int g_iUpper = -1;



#if BLAKE_224_256_384_512_HASH

#define BLOCK224 			(32*1024) // 64
#define BLAKE224_LEN 		28

#define BLOCK256 			(32*1024) // 64
#define BLAKE256_LEN 		32

#define BLOCK384 			(32*1024) // 64
#define BLAKE384_LEN 		48


#define BLOCK512 			(32*1024) // 64
#define BLAKE512_LEN 		64


#define U8TO32_BIG(p)					      \
  (((uint32_t)((p)[0]) << 24) | ((uint32_t)((p)[1]) << 16) |  \
   ((uint32_t)((p)[2]) <<  8) | ((uint32_t)((p)[3])      )) 


#define U32TO8_BIG(p, v)				        \
  (p)[0] = (uint8_t)((v) >> 24); (p)[1] = (uint8_t)((v) >> 16); \
  (p)[2] = (uint8_t)((v) >>  8); (p)[3] = (uint8_t)((v)      ); 


#define U8TO64_BIG(p) \
  (((uint64_t)U8TO32_BIG(p) << 32) | (uint64_t)U8TO32_BIG((p) + 4)) 


#define U64TO8_BIG(p, v)		      \
  U32TO8_BIG((p),     (uint32_t)((v) >> 32)); \
  U32TO8_BIG((p) + 4, (uint32_t)((v)      ));


typedef struct 
{ 
  uint32_t h[8], s[4], t[2]; 
  unsigned int buflen, nullt; 
  uint8_t  buf[64]; 
} state256; 

typedef state256 state224; 


typedef struct 
{ 
  uint64_t h[8], s[4], t[2]; 
  unsigned int buflen, nullt; 
  uint8_t buf[128]; 
} state512; 

typedef state512 state384; 


const uint8_t sigma[][16] = 
//static uint8_t sigma[][16] = 
{ 
  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }, 
  {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3 }, 
  {11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4 }, 
  { 7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8 }, 
  { 9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13 }, 
  { 2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9 }, 
  {12, 5, 1, 15, 14, 13, 4, 10, 0, 7, 6, 3, 9, 2, 8, 11 }, 
  {13, 11, 7, 14, 12, 1, 3, 9, 5, 0, 15, 4, 8, 6, 2, 10 }, 
  { 6, 15, 14, 9, 11, 3, 0, 8, 12, 2, 13, 7, 1, 4, 10, 5 }, 
  {10, 2, 8, 4, 7, 6, 1, 5, 15, 11, 9, 14, 3, 12, 13 , 0 }, 
  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }, 
  {14, 10, 4, 8, 9, 15, 13, 6, 1, 12, 0, 2, 11, 7, 5, 3 }, 
  {11, 8, 12, 0, 5, 2, 15, 13, 10, 14, 3, 6, 7, 1, 9, 4 }, 
  { 7, 9, 3, 1, 13, 12, 11, 14, 2, 6, 5, 10, 4, 0, 15, 8 }, 
  { 9, 0, 5, 7, 2, 4, 10, 15, 14, 1, 11, 12, 6, 8, 3, 13 }, 
  { 2, 12, 6, 10, 0, 11, 8, 3, 4, 13, 7, 5, 15, 14, 1, 9 } 
}; 


const uint32_t u256[16] = 
//static uint32_t u256[16] = 
{ 
  0x243f6a88, 0x85a308d3, 0x13198a2e, 0x03707344, 
  0xa4093822, 0x299f31d0, 0x082efa98, 0xec4e6c89, 
  0x452821e6, 0x38d01377, 0xbe5466cf, 0x34e90c6c, 
  0xc0ac29b7, 0xc97c50dd, 0x3f84d5b5, 0xb5470917 
}; 


const uint64_t u512[16] = 
//static uint64_t u512[16] = 
{ 
  0x243f6a8885a308d3ULL, 0x13198a2e03707344ULL,  
  0xa4093822299f31d0ULL, 0x082efa98ec4e6c89ULL, 
  0x452821e638d01377ULL, 0xbe5466cf34e90c6cULL,  
  0xc0ac29b7c97c50ddULL, 0x3f84d5b5b5470917ULL, 
  0x9216d5d98979fb1bULL, 0xd1310ba698dfb5acULL,  
  0x2ffd72dbd01adfb7ULL, 0xb8e1afed6a267e96ULL, 
  0xba7c9045f12c7f99ULL, 0x24a19947b3916cf7ULL,  
  0x0801f2e2858efc16ULL, 0x636920d871574e69ULL 
}; 




static /*const*/ uint8_t padding[129] = 
{ 
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 
}; 



void blake224_compress( state224 *S, const uint8_t *block ) 
{ 
  uint32_t v[16], m[16], i; 
#define ROT224(x,n) (((x)<<(32-n))|( (x)>>(n))) 
#define G224(a,b,c,d,e)          \
  v[a] += (m[sigma[i][e]] ^ u256[sigma[i][e+1]]) + v[b]; \
  v[d] = ROT224( v[d] ^ v[a],16);        \
  v[c] += v[d];           \
  v[b] = ROT224( v[b] ^ v[c],12);        \
  v[a] += (m[sigma[i][e+1]] ^ u256[sigma[i][e]])+v[b]; \
  v[d] = ROT224( v[d] ^ v[a], 8);        \
  v[c] += v[d];           \
  v[b] = ROT224( v[b] ^ v[c], 7); 


  for( i = 0; i < 16; ++i )  m[i] = U8TO32_BIG( block + i * 4 ); 


  for( i = 0; i < 8; ++i )  v[i] = S->h[i]; 


  v[ 8] = S->s[0] ^ u256[0]; 
  v[ 9] = S->s[1] ^ u256[1]; 
  v[10] = S->s[2] ^ u256[2]; 
  v[11] = S->s[3] ^ u256[3]; 
  v[12] = u256[4]; 
  v[13] = u256[5]; 
  v[14] = u256[6]; 
  v[15] = u256[7]; 


  /* don't xor t when the block is only padding */ 
  if ( !S->nullt ) 
  { 
    v[12] ^= S->t[0]; 
    v[13] ^= S->t[0]; 
    v[14] ^= S->t[1]; 
    v[15] ^= S->t[1]; 
  } 


  for( i = 0; i < 14; ++i ) 
  { 
    /* column step */ 
    G224( 0,  4,  8, 12,  0 ); 
    G224( 1,  5,  9, 13,  2 ); 
    G224( 2,  6, 10, 14,  4 ); 
    G224( 3,  7, 11, 15,  6 ); 
    /* diagonal step */ 
    G224( 0,  5, 10, 15,  8 ); 
    G224( 1,  6, 11, 12, 10 ); 
    G224( 2,  7,  8, 13, 12 ); 
    G224( 3,  4,  9, 14, 14 ); 
  } 


  for( i = 0; i < 16; ++i )  S->h[i % 8] ^= v[i]; 


  for( i = 0; i < 8 ; ++i )  S->h[i] ^= S->s[i % 4]; 
} 




void blake224_init( state224 *S ) 
{ 
  S->h[0] = 0xc1059ed8; 
  S->h[1] = 0x367cd507; 
  S->h[2] = 0x3070dd17; 
  S->h[3] = 0xf70e5939; 
  S->h[4] = 0xffc00b31; 
  S->h[5] = 0x68581511; 
  S->h[6] = 0x64f98fa7; 
  S->h[7] = 0xbefa4fa4; 
  S->t[0] = S->t[1] = S->buflen = S->nullt = 0; 
  S->s[0] = S->s[1] = S->s[2] = S->s[3] = 0; 
} 




void blake224_update( state224 *S, const uint8_t *in, uint64_t inlen ) 
{ 
 int left = S->buflen; 
 int fill = 64 - left; 


 /* data left and data received fill a block  */ 
 if( left && ( inlen >= fill ) ) 
 { 
   memcpy( ( void * ) ( S->buf + left ), ( void * ) in, fill ); 
   S->t[0] += 512; 


   if ( S->t[0] == 0 ) S->t[1]++; 


    blake224_compress( S, S->buf ); 
    in += fill; 
    inlen  -= fill; 
    left = 0; 
  } 


  /* compress blocks of data received */ 
  while( inlen >= 64 ) 
  { 
    S->t[0] += 512; 


    if ( S->t[0] == 0 ) S->t[1]++; 


    blake224_compress( S, in ); 
    in += 64; 
    inlen -= 64; 
  } 


  /* store any data left */ 
  if( inlen > 0 ) 
  { 
    memcpy( ( void * ) ( S->buf + left ), \
            ( void * ) in, ( size_t ) inlen ); 
    S->buflen = left + ( int )inlen; 
  } 
  else S->buflen = 0; 
} 




void blake224_final( state224 *S, uint8_t *out ) 
{ 
  uint8_t msglen[8], zz = 0x00, oz = 0x80; 
  uint32_t lo = S->t[0] + ( S->buflen << 3 ), hi = S->t[1]; 


  /* support for hashing more than 2^32 bits */ 
  if ( lo < ( S->buflen << 3 ) ) hi++; 


  U32TO8_BIG(  msglen + 0, hi ); 
  U32TO8_BIG(  msglen + 4, lo ); 


  if ( S->buflen == 55 )   /* one padding byte */ 
  { 
    S->t[0] -= 8; 
    blake224_update( S, &oz, 1 ); 
  } 
  else 
  { 
    if ( S->buflen < 55 )   /* enough space to fill the block  */ 
    { 
      if ( !S->buflen ) S->nullt = 1; 


      S->t[0] -= 440 - ( S->buflen << 3 ); 
      blake224_update( S, padding, 55 - S->buflen ); 
    } 
    else   /* need 2 compressions */ 
    { 
      S->t[0] -= 512 - ( S->buflen << 3 ); 
      blake224_update( S, padding, 64 - S->buflen ); 
      S->t[0] -= 440; 
      blake224_update( S, padding + 1, 55 ); 
      S->nullt = 1; 
    } 


    blake224_update( S, &zz, 1 ); 
    S->t[0] -= 8; 
  } 


  S->t[0] -= 64; 
  blake224_update( S, msglen, 8 ); 
  U32TO8_BIG( out + 0, S->h[0] ); 
  U32TO8_BIG( out + 4, S->h[1] ); 
  U32TO8_BIG( out + 8, S->h[2] ); 
  U32TO8_BIG( out + 12, S->h[3] ); 
  U32TO8_BIG( out + 16, S->h[4] ); 
  U32TO8_BIG( out + 20, S->h[5] ); 
  U32TO8_BIG( out + 24, S->h[6] ); 
} 




void blake224_hash( uint8_t *out, const uint8_t *in, uint64_t inlen ) 
{ 
  state224 S; 
  blake224_init( &S ); 
  blake224_update( &S, in, inlen ); 
  blake224_final( &S, out ); 
} 



void blake256_compress( state256 *S, const uint8_t *block ) 
{ 
  uint32_t v[16], m[16], i; 
#define ROT256(x,n) (((x)<<(32-n))|( (x)>>(n))) 
#define G256(a,b,c,d,e)          \
  v[a] += (m[sigma[i][e]] ^ u256[sigma[i][e+1]]) + v[b]; \
  v[d] = ROT256( v[d] ^ v[a],16);        \
  v[c] += v[d];           \
  v[b] = ROT256( v[b] ^ v[c],12);        \
  v[a] += (m[sigma[i][e+1]] ^ u256[sigma[i][e]])+v[b]; \
  v[d] = ROT256( v[d] ^ v[a], 8);        \
  v[c] += v[d];           \
  v[b] = ROT256( v[b] ^ v[c], 7); 


  for( i = 0; i < 16; ++i )  m[i] = U8TO32_BIG( block + i * 4 ); 


  for( i = 0; i < 8; ++i )  v[i] = S->h[i]; 


  v[ 8] = S->s[0] ^ u256[0]; 
  v[ 9] = S->s[1] ^ u256[1]; 
  v[10] = S->s[2] ^ u256[2]; 
  v[11] = S->s[3] ^ u256[3]; 
  v[12] = u256[4]; 
  v[13] = u256[5]; 
  v[14] = u256[6]; 
  v[15] = u256[7]; 


  /* don't xor t when the block is only padding */ 
  if ( !S->nullt ) 
  { 
    v[12] ^= S->t[0]; 
    v[13] ^= S->t[0]; 
    v[14] ^= S->t[1]; 
    v[15] ^= S->t[1]; 
  } 


  for( i = 0; i < 14; ++i ) 
  { 
    /* column step */ 
    G256( 0,  4,  8, 12,  0 ); 
    G256( 1,  5,  9, 13,  2 ); 
    G256( 2,  6, 10, 14,  4 ); 
    G256( 3,  7, 11, 15,  6 ); 
    /* diagonal step */ 
    G256( 0,  5, 10, 15,  8 ); 
    G256( 1,  6, 11, 12, 10 ); 
    G256( 2,  7,  8, 13, 12 ); 
    G256( 3,  4,  9, 14, 14 ); 
  } 


  for( i = 0; i < 16; ++i )  S->h[i % 8] ^= v[i]; 


  for( i = 0; i < 8 ; ++i )  S->h[i] ^= S->s[i % 4]; 
} 




void blake256_init( state256 *S ) 
{ 
  S->h[0] = 0x6a09e667; 
  S->h[1] = 0xbb67ae85; 
  S->h[2] = 0x3c6ef372; 
  S->h[3] = 0xa54ff53a; 
  S->h[4] = 0x510e527f; 
  S->h[5] = 0x9b05688c; 
  S->h[6] = 0x1f83d9ab; 
  S->h[7] = 0x5be0cd19; 
  S->t[0] = S->t[1] = S->buflen = S->nullt = 0; 
  S->s[0] = S->s[1] = S->s[2] = S->s[3] = 0; 
} 




void blake256_update( state256 *S, const uint8_t *in, uint64_t inlen ) 
{ 
 int left = S->buflen; 
 int fill = 64 - left; 


 /* data left and data received fill a block  */ 
 if( left && ( inlen >= fill ) ) 
 { 
   memcpy( ( void * ) ( S->buf + left ), ( void * ) in, fill ); 
   S->t[0] += 512; 


   if ( S->t[0] == 0 ) S->t[1]++; 


    blake256_compress( S, S->buf ); 
    in += fill; 
    inlen  -= fill; 
    left = 0; 
  } 


  /* compress blocks of data received */ 
  while( inlen >= 64 ) 
  { 
    S->t[0] += 512; 


    if ( S->t[0] == 0 ) S->t[1]++; 


    blake256_compress( S, in ); 
    in += 64; 
    inlen -= 64; 
  } 


  /* store any data left */ 
  if( inlen > 0 ) 
  { 
    memcpy( ( void * ) ( S->buf + left ),   \
            ( void * ) in, ( size_t ) inlen ); 
    S->buflen = left + ( int )inlen; 
  } 
  else S->buflen = 0; 
} 




void blake256_final( state256 *S, uint8_t *out ) 
{ 
  uint8_t msglen[8], zo = 0x01, oo = 0x81; 
  uint32_t lo = S->t[0] + ( S->buflen << 3 ), hi = S->t[1]; 


  /* support for hashing more than 2^32 bits */ 
  if ( lo < ( S->buflen << 3 ) ) hi++; 


  U32TO8_BIG(  msglen + 0, hi ); 
  U32TO8_BIG(  msglen + 4, lo ); 


  if ( S->buflen == 55 )   /* one padding byte */ 
  { 
    S->t[0] -= 8; 
    blake256_update( S, &oo, 1 ); 
  } 
  else 
  { 
    if ( S->buflen < 55 )   /* enough space to fill the block  */ 
    { 
      if ( !S->buflen ) S->nullt = 1; 


      S->t[0] -= 440 - ( S->buflen << 3 ); 
      blake256_update( S, padding, 55 - S->buflen ); 
    } 
    else   /* need 2 compressions */ 
    { 
      S->t[0] -= 512 - ( S->buflen << 3 ); 
      blake256_update( S, padding, 64 - S->buflen ); 
      S->t[0] -= 440; 
      blake256_update( S, padding + 1, 55 ); 
      S->nullt = 1; 
    } 


    blake256_update( S, &zo, 1 ); 
    S->t[0] -= 8; 
  } 


  S->t[0] -= 64; 
  blake256_update( S, msglen, 8 ); 
  U32TO8_BIG( out + 0, S->h[0] ); 
  U32TO8_BIG( out + 4, S->h[1] ); 
  U32TO8_BIG( out + 8, S->h[2] ); 
  U32TO8_BIG( out + 12, S->h[3] ); 
  U32TO8_BIG( out + 16, S->h[4] ); 
  U32TO8_BIG( out + 20, S->h[5] ); 
  U32TO8_BIG( out + 24, S->h[6] ); 
  U32TO8_BIG( out + 28, S->h[7] ); 
} 



void blake384_compress( state384 *S, const uint8_t *block ) 
{ 
  uint64_t v[16], m[16], i; 
#define ROT384(x,n) (((x)<<(64-n))|( (x)>>(n))) 
#define G384(a,b,c,d,e)          \
  v[a] += (m[sigma[i][e]] ^ u512[sigma[i][e+1]]) + v[b];\
  v[d] = ROT384( v[d] ^ v[a],32);        \
  v[c] += v[d];           \
  v[b] = ROT384( v[b] ^ v[c],25);        \
  v[a] += (m[sigma[i][e+1]] ^ u512[sigma[i][e]])+v[b];  \
  v[d] = ROT384( v[d] ^ v[a],16);        \
  v[c] += v[d];           \
  v[b] = ROT384( v[b] ^ v[c],11); 


  for( i = 0; i < 16; ++i )  m[i] = U8TO64_BIG( block + i * 8 ); 


  for( i = 0; i < 8; ++i )  v[i] = S->h[i]; 


  v[ 8] = S->s[0] ^ u512[0]; 
  v[ 9] = S->s[1] ^ u512[1]; 
  v[10] = S->s[2] ^ u512[2]; 
  v[11] = S->s[3] ^ u512[3]; 
  v[12] =  u512[4]; 
  v[13] =  u512[5]; 
  v[14] =  u512[6]; 
  v[15] =  u512[7]; 


  /* don't xor t when the block is only padding */ 
  if ( !S->nullt ) 
  { 
    v[12] ^= S->t[0]; 
    v[13] ^= S->t[0]; 
    v[14] ^= S->t[1]; 
    v[15] ^= S->t[1]; 
  } 


  for( i = 0; i < 16; ++i ) 
  { 
    /* column step */ 
    G384( 0, 4, 8, 12, 0 ); 
    G384( 1, 5, 9, 13, 2 ); 
    G384( 2, 6, 10, 14, 4 ); 
    G384( 3, 7, 11, 15, 6 ); 
    /* diagonal step */ 
    G384( 0, 5, 10, 15, 8 ); 
    G384( 1, 6, 11, 12, 10 ); 
    G384( 2, 7, 8, 13, 12 ); 
    G384( 3, 4, 9, 14, 14 ); 
  } 


  for( i = 0; i < 16; ++i )  S->h[i % 8] ^= v[i]; 


  for( i = 0; i < 8 ; ++i )  S->h[i] ^= S->s[i % 4]; 
} 




void blake384_init( state384 *S ) 
{ 
  S->h[0] = 0xcbbb9d5dc1059ed8ULL; 
  S->h[1] = 0x629a292a367cd507ULL; 
  S->h[2] = 0x9159015a3070dd17ULL; 
  S->h[3] = 0x152fecd8f70e5939ULL; 
  S->h[4] = 0x67332667ffc00b31ULL; 
  S->h[5] = 0x8eb44a8768581511ULL; 
  S->h[6] = 0xdb0c2e0d64f98fa7ULL; 
  S->h[7] = 0x47b5481dbefa4fa4ULL; 
  S->t[0] = S->t[1] = S->buflen = S->nullt = 0; 
  S->s[0] = S->s[1] = S->s[2] = S->s[3] = 0; 
} 




void blake384_update( state384 *S, const uint8_t *in, uint64_t inlen ) 
{ 
 int left = S->buflen; 
 int fill = 128 - left; 


 /* data left and data received fill a block  */ 
 if( left && ( inlen >= fill ) ) 
 { 
   memcpy( ( void * ) ( S->buf + left ), ( void * ) in, fill ); 
   S->t[0] += 1024; 


   if ( S->t[0] == 0 ) S->t[1]++; 


   blake384_compress( S, S->buf ); 
    in += fill; 
    inlen  -= fill; 
    left = 0; 
  } 


  /* compress blocks of data received */ 
  while( inlen >= 128 ) 
  { 
    S->t[0] += 1024; 


    if ( S->t[0] == 0 ) S->t[1]++; 


    blake384_compress( S, in ); 
    in += 128; 
    inlen -= 128; 
  } 


  /* store any data left */ 
  if( inlen > 0 ) 
  { 
    memcpy( ( void * ) ( S->buf + left ), \
            ( void * ) in, ( size_t ) inlen ); 
    S->buflen = left + ( int )inlen; 
  } 
  else S->buflen = 0; 
} 




void blake384_final( state384 *S, uint8_t *out ) 
{ 
  uint8_t msglen[16], zz = 0x00, oz = 0x80; 
  uint64_t lo = S->t[0] + ( S->buflen << 3 ), hi = S->t[1]; 


  /* support for hashing more than 2^32 bits */ 
  if ( lo < ( S->buflen << 3 ) ) hi++; 


  U64TO8_BIG(  msglen + 0, hi ); 
  U64TO8_BIG(  msglen + 8, lo ); 


  if ( S->buflen == 111 )   /* one padding byte */ 
  { 
    S->t[0] -= 8; 
    blake384_update( S, &oz, 1 ); 
  } 
  else 
  { 
    if ( S->buflen < 111 )  /* enough space to fill the block */ 
    { 
      if ( !S->buflen ) S->nullt = 1; 


      S->t[0] -= 888 - ( S->buflen << 3 ); 
      blake384_update( S, padding, 111 - S->buflen ); 
    } 
    else   /* need 2 compressions */ 
    { 
      S->t[0] -= 1024 - ( S->buflen << 3 ); 
      blake384_update( S, padding, 128 - S->buflen ); 
      S->t[0] -= 888; 
      blake384_update( S, padding + 1, 111 ); 
      S->nullt = 1; 
    } 


    blake384_update( S, &zz, 1 ); 
    S->t[0] -= 8; 
  } 


  S->t[0] -= 128; 
  blake384_update( S, msglen, 16 ); 
  U64TO8_BIG( out + 0, S->h[0] ); 
  U64TO8_BIG( out + 8, S->h[1] ); 
  U64TO8_BIG( out + 16, S->h[2] ); 
  U64TO8_BIG( out + 24, S->h[3] ); 
  U64TO8_BIG( out + 32, S->h[4] ); 
  U64TO8_BIG( out + 40, S->h[5] ); 
} 




void blake384_hash( uint8_t *out, const uint8_t *in, uint64_t inlen ) 
{ 
  state384 S; 
  blake384_init( &S ); 
  blake384_update( &S, in, inlen ); 
  blake384_final( &S, out ); 
} 



void blake512_compress( state512 *S, const uint8_t *block ) 
{ 
  uint64_t v[16], m[16], i; 
#define ROT512(x,n) (((x)<<(64-n))|( (x)>>(n))) 
#define G512(a,b,c,d,e)          \
  v[a] += (m[sigma[i][e]] ^ u512[sigma[i][e+1]]) + v[b];\
  v[d] = ROT512( v[d] ^ v[a],32);        \
  v[c] += v[d];           \
  v[b] = ROT512( v[b] ^ v[c],25);        \
  v[a] += (m[sigma[i][e+1]] ^ u512[sigma[i][e]])+v[b];  \
  v[d] = ROT512( v[d] ^ v[a],16);        \
  v[c] += v[d];           \
  v[b] = ROT512( v[b] ^ v[c],11); 


  for( i = 0; i < 16; ++i )  m[i] = U8TO64_BIG( block + i * 8 ); 


  for( i = 0; i < 8; ++i )  v[i] = S->h[i]; 


  v[ 8] = S->s[0] ^ u512[0]; 
  v[ 9] = S->s[1] ^ u512[1]; 
  v[10] = S->s[2] ^ u512[2]; 
  v[11] = S->s[3] ^ u512[3]; 
  v[12] =  u512[4]; 
  v[13] =  u512[5]; 
  v[14] =  u512[6]; 
  v[15] =  u512[7]; 


  /* don't xor t when the block is only padding */ 
  if ( !S->nullt ) 
  { 
    v[12] ^= S->t[0]; 
    v[13] ^= S->t[0]; 
    v[14] ^= S->t[1]; 
    v[15] ^= S->t[1]; 
  } 


  for( i = 0; i < 16; ++i ) 
  { 
    /* column step */ 
    G512( 0, 4, 8, 12, 0 ); 
    G512( 1, 5, 9, 13, 2 ); 
    G512( 2, 6, 10, 14, 4 ); 
    G512( 3, 7, 11, 15, 6 ); 
    /* diagonal step */ 
    G512( 0, 5, 10, 15, 8 ); 
    G512( 1, 6, 11, 12, 10 ); 
    G512( 2, 7, 8, 13, 12 ); 
    G512( 3, 4, 9, 14, 14 ); 
  } 


  for( i = 0; i < 16; ++i )  S->h[i % 8] ^= v[i]; 


  for( i = 0; i < 8 ; ++i )  S->h[i] ^= S->s[i % 4]; 
} 




void blake512_init( state512 *S ) 
{ 
  S->h[0] = 0x6a09e667f3bcc908ULL; 
  S->h[1] = 0xbb67ae8584caa73bULL; 
  S->h[2] = 0x3c6ef372fe94f82bULL; 
  S->h[3] = 0xa54ff53a5f1d36f1ULL; 
  S->h[4] = 0x510e527fade682d1ULL; 
  S->h[5] = 0x9b05688c2b3e6c1fULL; 
  S->h[6] = 0x1f83d9abfb41bd6bULL; 
  S->h[7] = 0x5be0cd19137e2179ULL; 
  S->t[0] = S->t[1] = S->buflen = S->nullt = 0; 
  S->s[0] = S->s[1] = S->s[2] = S->s[3] = 0; 
} 




void blake512_update( state512 *S, const uint8_t *in, uint64_t inlen ) 
{ 
 int left = S->buflen; 
 int fill = 128 - left; 


 /* data left and data received fill a block  */ 
 if( left && ( inlen >= fill ) ) 
 { 
   memcpy( ( void * ) ( S->buf + left ), ( void * ) in, fill ); 
   S->t[0] += 1024; 


   if ( S->t[0] == 0 ) S->t[1]++; 


   blake512_compress( S, S->buf ); 
    in += fill; 
    inlen  -= fill; 
    left = 0; 
  } 


  /* compress blocks of data received */ 
  while( inlen >= 128 ) 
  { 
    S->t[0] += 1024; 


    if ( S->t[0] == 0 ) S->t[1]++; 


    blake512_compress( S, in ); 
    in += 128; 
    inlen -= 128; 
  } 


  /* store any data left */ 
  if( inlen > 0 ) 
  { 
    memcpy( ( void * ) ( S->buf + left ),   \
            ( void * ) in, ( size_t ) inlen ); 
    S->buflen = left + ( int )inlen; 
  } 
  else S->buflen = 0; 
} 




void blake512_final( state512 *S, uint8_t *out ) 
{ 
  uint8_t msglen[16], zo = 0x01, oo = 0x81; 
  uint64_t lo = S->t[0] + ( S->buflen << 3 ), hi = S->t[1]; 


  /* support for hashing more than 2^32 bits */ 
  if ( lo < ( S->buflen << 3 ) ) hi++; 


  U64TO8_BIG(  msglen + 0, hi ); 
  U64TO8_BIG(  msglen + 8, lo ); 


  if ( S->buflen == 111 )   /* one padding byte */ 
  { 
    S->t[0] -= 8; 
    blake512_update( S, &oo, 1 ); 
  } 
  else 
  { 
    if ( S->buflen < 111 )  /* enough space to fill the block */ 
    { 
      if ( !S->buflen ) S->nullt = 1; 


      S->t[0] -= 888 - ( S->buflen << 3 ); 
      blake512_update( S, padding, 111 - S->buflen ); 
    } 
    else   /* need 2 compressions */ 
    { 
      S->t[0] -= 1024 - ( S->buflen << 3 ); 
      blake512_update( S, padding, 128 - S->buflen ); 
      S->t[0] -= 888; 
      blake512_update( S, padding + 1, 111 ); 
      S->nullt = 1; 
    } 


    blake512_update( S, &zo, 1 ); 
    S->t[0] -= 8; 
  } 


  S->t[0] -= 128; 
  blake512_update( S, msglen, 16 ); 
  U64TO8_BIG( out + 0, S->h[0] ); 
  U64TO8_BIG( out + 8, S->h[1] ); 
  U64TO8_BIG( out + 16, S->h[2] ); 
  U64TO8_BIG( out + 24, S->h[3] ); 
  U64TO8_BIG( out + 32, S->h[4] ); 
  U64TO8_BIG( out + 40, S->h[5] ); 
  U64TO8_BIG( out + 48, S->h[6] ); 
  U64TO8_BIG( out + 56, S->h[7] ); 
} 




void blake512_hash( uint8_t *out, const uint8_t *in, uint64_t inlen ) 
{ 
  state512 S; 
  blake512_init( &S ); 
  blake512_update( &S, in, inlen ); 
  blake512_final( &S, out ); 
} 

#endif // BLAKE_224_256_384_512_HASH


#if defined(RIPEMD128) || defined(RIPEMD160) || defined(RIPEMD320)
#define RMD128_DIGEST_SIZE      16 // 128/8
#define RMD128_BLOCK_SIZE       64

#define RMD160_DIGEST_SIZE      20 // 160/8
#define RMD160_BLOCK_SIZE       64

#define RMD256_DIGEST_SIZE      32 // 256/8
#define RMD256_BLOCK_SIZE       64

#define RMD320_DIGEST_SIZE      40 //320/8
#define RMD320_BLOCK_SIZE       64


/* initial values  */
#define RMD_H0  0x67452301UL
#define RMD_H1  0xefcdab89UL
#define RMD_H2  0x98badcfeUL
#define RMD_H3  0x10325476UL
#define RMD_H4  0xc3d2e1f0UL
#define RMD_H5  0x76543210UL
#define RMD_H6  0xfedcba98UL
#define RMD_H7  0x89abcdefUL
#define RMD_H8  0x01234567UL
#define RMD_H9  0x3c2d1e0fUL
/* constants */
#define RMD_K1  0x00000000UL
#define RMD_K2  0x5a827999UL
#define RMD_K3  0x6ed9eba1UL
#define RMD_K4  0x8f1bbcdcUL
#define RMD_K5  0xa953fd4eUL
#define RMD_K6  0x50a28be6UL
#define RMD_K7  0x5c4dd124UL
#define RMD_K8  0x6d703ef3UL
#define RMD_K9  0x7a6d76e9UL
#endif


#if RIPEMD160


/* if this line causes a compiler error, 
   adapt the defintion of dword above */
typedef int the_correct_size_was_chosen [sizeof (dword) == 4? 1: -1];

/********************************************************************/

/* macro definitions */

/* collect four bytes into one word: */
#define BYTES_TO_DWORD(strptr)                    \
            (((dword) *((strptr)+3) << 24) | \
             ((dword) *((strptr)+2) << 16) | \
             ((dword) *((strptr)+1) <<  8) | \
             ((dword) *(strptr)))

/* ROL(x, n) cyclically rotates x over n bits to the left */
/* x must be of an unsigned 32 bits type and 0 <= n < 32. */
#define RMD_ROL(x, n)        (((x) << (n)) | ((x) >> (32-(n))))

/* the five basic functions F(), G() and H() */
#define RMD_F(x, y, z)        ((x) ^ (y) ^ (z)) 
#define RMD_G(x, y, z)        (((x) & (y)) | (~(x) & (z))) 
#define RMD_H(x, y, z)        (((x) | ~(y)) ^ (z))
#define RMD_I(x, y, z)        (((x) & (z)) | ((y) & ~(z))) 
#define RMD_J(x, y, z)        ((x) ^ ((y) | ~(z)))
  
/* the ten basic operations FF() through III() */
#define RMD160_FF(a, b, c, d, e, x, s)        {\
      (a) += RMD_F((b), (c), (d)) + (x);\
      (a) = RMD_ROL((a), (s)) + (e);\
      (c) = RMD_ROL((c), 10);\
   }
#define RMD160_GG(a, b, c, d, e, x, s)        {\
      (a) += RMD_G((b), (c), (d)) + (x) + 0x5a827999UL;\
      (a) = RMD_ROL((a), (s)) + (e);\
      (c) = RMD_ROL((c), 10);\
   }
#define RMD160_HH(a, b, c, d, e, x, s)        {\
      (a) += RMD_H((b), (c), (d)) + (x) + 0x6ed9eba1UL;\
      (a) = RMD_ROL((a), (s)) + (e);\
      (c) = RMD_ROL((c), 10);\
   }
#define RMD160_II(a, b, c, d, e, x, s)        {\
      (a) += RMD_I((b), (c), (d)) + (x) + 0x8f1bbcdcUL;\
      (a) = RMD_ROL((a), (s)) + (e);\
      (c) = RMD_ROL((c), 10);\
   }
#define RMD160_JJ(a, b, c, d, e, x, s)        {\
      (a) += RMD_J((b), (c), (d)) + (x) + 0xa953fd4eUL;\
      (a) = RMD_ROL((a), (s)) + (e);\
      (c) = RMD_ROL((c), 10);\
   }
#define RMD160_FFF(a, b, c, d, e, x, s)        {\
      (a) += RMD_F((b), (c), (d)) + (x);\
      (a) = RMD_ROL((a), (s)) + (e);\
      (c) = RMD_ROL((c), 10);\
   }
#define RMD160_GGG(a, b, c, d, e, x, s)        {\
      (a) += RMD_G((b), (c), (d)) + (x) + 0x7a6d76e9UL;\
      (a) = RMD_ROL((a), (s)) + (e);\
      (c) = RMD_ROL((c), 10);\
   }
#define RMD160_HHH(a, b, c, d, e, x, s)        {\
      (a) += RMD_H((b), (c), (d)) + (x) + 0x6d703ef3UL;\
      (a) = RMD_ROL((a), (s)) + (e);\
      (c) = RMD_ROL((c), 10);\
   }
#define RMD160_III(a, b, c, d, e, x, s)        {\
      (a) += RMD_I((b), (c), (d)) + (x) + 0x5c4dd124UL;\
      (a) = RMD_ROL((a), (s)) + (e);\
      (c) = RMD_ROL((c), 10);\
   }
#define RMD160_JJJ(a, b, c, d, e, x, s)        {\
      (a) += RMD_J((b), (c), (d)) + (x) + 0x50a28be6UL;\
      (a) = RMD_ROL((a), (s)) + (e);\
      (c) = RMD_ROL((c), 10);\
   }


/********************************************************************/

void MD160init(dword *MDbuf)
{
   MDbuf[0] = RMD_H0; // 0x67452301UL;
   MDbuf[1] = RMD_H1; // 0xefcdab89UL;
   MDbuf[2] = RMD_H2; // 0x98badcfeUL;
   MDbuf[3] = RMD_H3; // 0x10325476UL;
   MDbuf[4] = RMD_H4; // 0xc3d2e1f0UL;

   return;
}

/********************************************************************/

void MD160compress(dword *MDbuf, dword *X)
{
   dword aa = MDbuf[0],  bb = MDbuf[1],  cc = MDbuf[2],
         dd = MDbuf[3],  ee = MDbuf[4];
   dword aaa = MDbuf[0], bbb = MDbuf[1], ccc = MDbuf[2],
         ddd = MDbuf[3], eee = MDbuf[4];

   /* round 1 */
   RMD160_FF(aa, bb, cc, dd, ee, X[ 0], 11);
   RMD160_FF(ee, aa, bb, cc, dd, X[ 1], 14);
   RMD160_FF(dd, ee, aa, bb, cc, X[ 2], 15);
   RMD160_FF(cc, dd, ee, aa, bb, X[ 3], 12);
   RMD160_FF(bb, cc, dd, ee, aa, X[ 4],  5);
   RMD160_FF(aa, bb, cc, dd, ee, X[ 5],  8);
   RMD160_FF(ee, aa, bb, cc, dd, X[ 6],  7);
   RMD160_FF(dd, ee, aa, bb, cc, X[ 7],  9);
   RMD160_FF(cc, dd, ee, aa, bb, X[ 8], 11);
   RMD160_FF(bb, cc, dd, ee, aa, X[ 9], 13);
   RMD160_FF(aa, bb, cc, dd, ee, X[10], 14);
   RMD160_FF(ee, aa, bb, cc, dd, X[11], 15);
   RMD160_FF(dd, ee, aa, bb, cc, X[12],  6);
   RMD160_FF(cc, dd, ee, aa, bb, X[13],  7);
   RMD160_FF(bb, cc, dd, ee, aa, X[14],  9);
   RMD160_FF(aa, bb, cc, dd, ee, X[15],  8);
                             
   /* round 2 */
   RMD160_GG(ee, aa, bb, cc, dd, X[ 7],  7);
   RMD160_GG(dd, ee, aa, bb, cc, X[ 4],  6);
   RMD160_GG(cc, dd, ee, aa, bb, X[13],  8);
   RMD160_GG(bb, cc, dd, ee, aa, X[ 1], 13);
   RMD160_GG(aa, bb, cc, dd, ee, X[10], 11);
   RMD160_GG(ee, aa, bb, cc, dd, X[ 6],  9);
   RMD160_GG(dd, ee, aa, bb, cc, X[15],  7);
   RMD160_GG(cc, dd, ee, aa, bb, X[ 3], 15);
   RMD160_GG(bb, cc, dd, ee, aa, X[12],  7);
   RMD160_GG(aa, bb, cc, dd, ee, X[ 0], 12);
   RMD160_GG(ee, aa, bb, cc, dd, X[ 9], 15);
   RMD160_GG(dd, ee, aa, bb, cc, X[ 5],  9);
   RMD160_GG(cc, dd, ee, aa, bb, X[ 2], 11);
   RMD160_GG(bb, cc, dd, ee, aa, X[14],  7);
   RMD160_GG(aa, bb, cc, dd, ee, X[11], 13);
   RMD160_GG(ee, aa, bb, cc, dd, X[ 8], 12);

   /* round 3 */
   RMD160_HH(dd, ee, aa, bb, cc, X[ 3], 11);
   RMD160_HH(cc, dd, ee, aa, bb, X[10], 13);
   RMD160_HH(bb, cc, dd, ee, aa, X[14],  6);
   RMD160_HH(aa, bb, cc, dd, ee, X[ 4],  7);
   RMD160_HH(ee, aa, bb, cc, dd, X[ 9], 14);
   RMD160_HH(dd, ee, aa, bb, cc, X[15],  9);
   RMD160_HH(cc, dd, ee, aa, bb, X[ 8], 13);
   RMD160_HH(bb, cc, dd, ee, aa, X[ 1], 15);
   RMD160_HH(aa, bb, cc, dd, ee, X[ 2], 14);
   RMD160_HH(ee, aa, bb, cc, dd, X[ 7],  8);
   RMD160_HH(dd, ee, aa, bb, cc, X[ 0], 13);
   RMD160_HH(cc, dd, ee, aa, bb, X[ 6],  6);
   RMD160_HH(bb, cc, dd, ee, aa, X[13],  5);
   RMD160_HH(aa, bb, cc, dd, ee, X[11], 12);
   RMD160_HH(ee, aa, bb, cc, dd, X[ 5],  7);
   RMD160_HH(dd, ee, aa, bb, cc, X[12],  5);

   /* round 4 */
   RMD160_II(cc, dd, ee, aa, bb, X[ 1], 11);
   RMD160_II(bb, cc, dd, ee, aa, X[ 9], 12);
   RMD160_II(aa, bb, cc, dd, ee, X[11], 14);
   RMD160_II(ee, aa, bb, cc, dd, X[10], 15);
   RMD160_II(dd, ee, aa, bb, cc, X[ 0], 14);
   RMD160_II(cc, dd, ee, aa, bb, X[ 8], 15);
   RMD160_II(bb, cc, dd, ee, aa, X[12],  9);
   RMD160_II(aa, bb, cc, dd, ee, X[ 4],  8);
   RMD160_II(ee, aa, bb, cc, dd, X[13],  9);
   RMD160_II(dd, ee, aa, bb, cc, X[ 3], 14);
   RMD160_II(cc, dd, ee, aa, bb, X[ 7],  5);
   RMD160_II(bb, cc, dd, ee, aa, X[15],  6);
   RMD160_II(aa, bb, cc, dd, ee, X[14],  8);
   RMD160_II(ee, aa, bb, cc, dd, X[ 5],  6);
   RMD160_II(dd, ee, aa, bb, cc, X[ 6],  5);
   RMD160_II(cc, dd, ee, aa, bb, X[ 2], 12);

   /* round 5 */
   RMD160_JJ(bb, cc, dd, ee, aa, X[ 4],  9);
   RMD160_JJ(aa, bb, cc, dd, ee, X[ 0], 15);
   RMD160_JJ(ee, aa, bb, cc, dd, X[ 5],  5);
   RMD160_JJ(dd, ee, aa, bb, cc, X[ 9], 11);
   RMD160_JJ(cc, dd, ee, aa, bb, X[ 7],  6);
   RMD160_JJ(bb, cc, dd, ee, aa, X[12],  8);
   RMD160_JJ(aa, bb, cc, dd, ee, X[ 2], 13);
   RMD160_JJ(ee, aa, bb, cc, dd, X[10], 12);
   RMD160_JJ(dd, ee, aa, bb, cc, X[14],  5);
   RMD160_JJ(cc, dd, ee, aa, bb, X[ 1], 12);
   RMD160_JJ(bb, cc, dd, ee, aa, X[ 3], 13);
   RMD160_JJ(aa, bb, cc, dd, ee, X[ 8], 14);
   RMD160_JJ(ee, aa, bb, cc, dd, X[11], 11);
   RMD160_JJ(dd, ee, aa, bb, cc, X[ 6],  8);
   RMD160_JJ(cc, dd, ee, aa, bb, X[15],  5);
   RMD160_JJ(bb, cc, dd, ee, aa, X[13],  6);

   /* parallel round 1 */
   RMD160_JJJ(aaa, bbb, ccc, ddd, eee, X[ 5],  8);
   RMD160_JJJ(eee, aaa, bbb, ccc, ddd, X[14],  9);
   RMD160_JJJ(ddd, eee, aaa, bbb, ccc, X[ 7],  9);
   RMD160_JJJ(ccc, ddd, eee, aaa, bbb, X[ 0], 11);
   RMD160_JJJ(bbb, ccc, ddd, eee, aaa, X[ 9], 13);
   RMD160_JJJ(aaa, bbb, ccc, ddd, eee, X[ 2], 15);
   RMD160_JJJ(eee, aaa, bbb, ccc, ddd, X[11], 15);
   RMD160_JJJ(ddd, eee, aaa, bbb, ccc, X[ 4],  5);
   RMD160_JJJ(ccc, ddd, eee, aaa, bbb, X[13],  7);
   RMD160_JJJ(bbb, ccc, ddd, eee, aaa, X[ 6],  7);
   RMD160_JJJ(aaa, bbb, ccc, ddd, eee, X[15],  8);
   RMD160_JJJ(eee, aaa, bbb, ccc, ddd, X[ 8], 11);
   RMD160_JJJ(ddd, eee, aaa, bbb, ccc, X[ 1], 14);
   RMD160_JJJ(ccc, ddd, eee, aaa, bbb, X[10], 14);
   RMD160_JJJ(bbb, ccc, ddd, eee, aaa, X[ 3], 12);
   RMD160_JJJ(aaa, bbb, ccc, ddd, eee, X[12],  6);

   /* parallel round 2 */
   RMD160_III(eee, aaa, bbb, ccc, ddd, X[ 6],  9); 
   RMD160_III(ddd, eee, aaa, bbb, ccc, X[11], 13);
   RMD160_III(ccc, ddd, eee, aaa, bbb, X[ 3], 15);
   RMD160_III(bbb, ccc, ddd, eee, aaa, X[ 7],  7);
   RMD160_III(aaa, bbb, ccc, ddd, eee, X[ 0], 12);
   RMD160_III(eee, aaa, bbb, ccc, ddd, X[13],  8);
   RMD160_III(ddd, eee, aaa, bbb, ccc, X[ 5],  9);
   RMD160_III(ccc, ddd, eee, aaa, bbb, X[10], 11);
   RMD160_III(bbb, ccc, ddd, eee, aaa, X[14],  7);
   RMD160_III(aaa, bbb, ccc, ddd, eee, X[15],  7);
   RMD160_III(eee, aaa, bbb, ccc, ddd, X[ 8], 12);
   RMD160_III(ddd, eee, aaa, bbb, ccc, X[12],  7);
   RMD160_III(ccc, ddd, eee, aaa, bbb, X[ 4],  6);
   RMD160_III(bbb, ccc, ddd, eee, aaa, X[ 9], 15);
   RMD160_III(aaa, bbb, ccc, ddd, eee, X[ 1], 13);
   RMD160_III(eee, aaa, bbb, ccc, ddd, X[ 2], 11);

   /* parallel round 3 */
   RMD160_HHH(ddd, eee, aaa, bbb, ccc, X[15],  9);
   RMD160_HHH(ccc, ddd, eee, aaa, bbb, X[ 5],  7);
   RMD160_HHH(bbb, ccc, ddd, eee, aaa, X[ 1], 15);
   RMD160_HHH(aaa, bbb, ccc, ddd, eee, X[ 3], 11);
   RMD160_HHH(eee, aaa, bbb, ccc, ddd, X[ 7],  8);
   RMD160_HHH(ddd, eee, aaa, bbb, ccc, X[14],  6);
   RMD160_HHH(ccc, ddd, eee, aaa, bbb, X[ 6],  6);
   RMD160_HHH(bbb, ccc, ddd, eee, aaa, X[ 9], 14);
   RMD160_HHH(aaa, bbb, ccc, ddd, eee, X[11], 12);
   RMD160_HHH(eee, aaa, bbb, ccc, ddd, X[ 8], 13);
   RMD160_HHH(ddd, eee, aaa, bbb, ccc, X[12],  5);
   RMD160_HHH(ccc, ddd, eee, aaa, bbb, X[ 2], 14);
   RMD160_HHH(bbb, ccc, ddd, eee, aaa, X[10], 13);
   RMD160_HHH(aaa, bbb, ccc, ddd, eee, X[ 0], 13);
   RMD160_HHH(eee, aaa, bbb, ccc, ddd, X[ 4],  7);
   RMD160_HHH(ddd, eee, aaa, bbb, ccc, X[13],  5);

   /* parallel round 4 */   
   RMD160_GGG(ccc, ddd, eee, aaa, bbb, X[ 8], 15);
   RMD160_GGG(bbb, ccc, ddd, eee, aaa, X[ 6],  5);
   RMD160_GGG(aaa, bbb, ccc, ddd, eee, X[ 4],  8);
   RMD160_GGG(eee, aaa, bbb, ccc, ddd, X[ 1], 11);
   RMD160_GGG(ddd, eee, aaa, bbb, ccc, X[ 3], 14);
   RMD160_GGG(ccc, ddd, eee, aaa, bbb, X[11], 14);
   RMD160_GGG(bbb, ccc, ddd, eee, aaa, X[15],  6);
   RMD160_GGG(aaa, bbb, ccc, ddd, eee, X[ 0], 14);
   RMD160_GGG(eee, aaa, bbb, ccc, ddd, X[ 5],  6);
   RMD160_GGG(ddd, eee, aaa, bbb, ccc, X[12],  9);
   RMD160_GGG(ccc, ddd, eee, aaa, bbb, X[ 2], 12);
   RMD160_GGG(bbb, ccc, ddd, eee, aaa, X[13],  9);
   RMD160_GGG(aaa, bbb, ccc, ddd, eee, X[ 9], 12);
   RMD160_GGG(eee, aaa, bbb, ccc, ddd, X[ 7],  5);
   RMD160_GGG(ddd, eee, aaa, bbb, ccc, X[10], 15);
   RMD160_GGG(ccc, ddd, eee, aaa, bbb, X[14],  8);

   /* parallel round 5 */
   RMD160_FFF(bbb, ccc, ddd, eee, aaa, X[12] ,  8);
   RMD160_FFF(aaa, bbb, ccc, ddd, eee, X[15] ,  5);
   RMD160_FFF(eee, aaa, bbb, ccc, ddd, X[10] , 12);
   RMD160_FFF(ddd, eee, aaa, bbb, ccc, X[ 4] ,  9);
   RMD160_FFF(ccc, ddd, eee, aaa, bbb, X[ 1] , 12);
   RMD160_FFF(bbb, ccc, ddd, eee, aaa, X[ 5] ,  5);
   RMD160_FFF(aaa, bbb, ccc, ddd, eee, X[ 8] , 14);
   RMD160_FFF(eee, aaa, bbb, ccc, ddd, X[ 7] ,  6);
   RMD160_FFF(ddd, eee, aaa, bbb, ccc, X[ 6] ,  8);
   RMD160_FFF(ccc, ddd, eee, aaa, bbb, X[ 2] , 13);
   RMD160_FFF(bbb, ccc, ddd, eee, aaa, X[13] ,  6);
   RMD160_FFF(aaa, bbb, ccc, ddd, eee, X[14] ,  5);
   RMD160_FFF(eee, aaa, bbb, ccc, ddd, X[ 0] , 15);
   RMD160_FFF(ddd, eee, aaa, bbb, ccc, X[ 3] , 13);
   RMD160_FFF(ccc, ddd, eee, aaa, bbb, X[ 9] , 11);
   RMD160_FFF(bbb, ccc, ddd, eee, aaa, X[11] , 11);

   /* combine results */
   ddd += cc + MDbuf[1];               /* final result for MDbuf[0] */
   MDbuf[1] = MDbuf[2] + dd + eee;
   MDbuf[2] = MDbuf[3] + ee + aaa;
   MDbuf[3] = MDbuf[4] + aa + bbb;
   MDbuf[4] = MDbuf[0] + bb + ccc;
   MDbuf[0] = ddd;

   return;
}

/********************************************************************/

void MD160finish(dword *MDbuf, byte *strptr, dword lswlen, dword mswlen)
{
   unsigned int i;                                 /* counter       */
   dword        X[16];                             /* message words */

   memset(X, 0, 16*sizeof(dword));

   /* put bytes from strptr into X */
   for (i=0; i<(lswlen&63); i++) {
      /* byte i goes into word X[i div 4] at pos.  8*(i mod 4)  */
      X[i>>2] ^= (dword) *strptr++ << (8 * (i&3));
   }

   /* append the bit m_n == 1 */
   X[(lswlen>>2)&15] ^= (dword)1 << (8*(lswlen&3) + 7);

   if ((lswlen & 63) > 55) {
      /* length goes to next block */
      MD160compress(MDbuf, X);
      memset(X, 0, 16*sizeof(dword));
   }

   /* append length in bits*/
   X[14] = lswlen << 3;
   X[15] = (lswlen >> 29) | (mswlen << 3);
   MD160compress(MDbuf, X);

   return;
}

#endif // RIPEMD160


#if RIPEMD128

#if 0
/* if this line causes a compiler error, 
   adapt the defintion of dword above */
typedef int the_correct_size_was_chosen [sizeof (dword) == 4? 1: -1];

/********************************************************************/

/* macro definitions */

/* collect four bytes into one word: */
#define BYTES_TO_DWORD(strptr)                    \
            (((dword) *((strptr)+3) << 24) | \
             ((dword) *((strptr)+2) << 16) | \
             ((dword) *((strptr)+1) <<  8) | \
             ((dword) *(strptr)))

/* ROL(x, n) cyclically rotates x over n bits to the left */
/* x must be of an unsigned 32 bits type and 0 <= n < 32. */
#define ROL(x, n)        (((x) << (n)) | ((x) >> (32-(n))))

/* the four basic functions F(), G() and H() */
#define F(x, y, z)        ((x) ^ (y) ^ (z)) 
#define G(x, y, z)        (((x) & (y)) | (~(x) & (z))) 
#define H(x, y, z)        (((x) | ~(y)) ^ (z))
#define I(x, y, z)        (((x) & (z)) | ((y) & ~(z))) 
#endif


/* the eight basic operations FF() through III() */
#define RMD128_FF(a, b, c, d, x, s)        {\
      (a) += RMD_F((b), (c), (d)) + (x);\
      (a) = RMD_ROL((a), (s));\
   }
#define RMD128_GG(a, b, c, d, x, s)        {\
      (a) += RMD_G((b), (c), (d)) + (x) + 0x5a827999UL;\
      (a) = RMD_ROL((a), (s));\
   }
#define RMD128_HH(a, b, c, d, x, s)        {\
      (a) += RMD_H((b), (c), (d)) + (x) + 0x6ed9eba1UL;\
      (a) = RMD_ROL((a), (s));\
   }
#define RMD128_II(a, b, c, d, x, s)        {\
      (a) += RMD_I((b), (c), (d)) + (x) + 0x8f1bbcdcUL;\
      (a) = RMD_ROL((a), (s));\
   }
#define RMD128_FFF(a, b, c, d, x, s)        {\
      (a) += RMD_F((b), (c), (d)) + (x);\
      (a) = RMD_ROL((a), (s));\
   }
#define RMD128_GGG(a, b, c, d, x, s)        {\
      (a) += RMD_G((b), (c), (d)) + (x) + 0x6d703ef3UL;\
      (a) = RMD_ROL((a), (s));\
   }
#define RMD128_HHH(a, b, c, d, x, s)        {\
      (a) += RMD_H((b), (c), (d)) + (x) + 0x5c4dd124UL;\
      (a) = RMD_ROL((a), (s));\
   }
#define RMD128_III(a, b, c, d, x, s)        {\
      (a) += RMD_I((b), (c), (d)) + (x) + 0x50a28be6UL;\
      (a) = RMD_ROL((a), (s));\
   }



/********************************************************************/

void MD128init(dword *MDbuf)
{
   MDbuf[0] = RMD_H0; // 0x67452301UL;
   MDbuf[1] = RMD_H1; // 0xefcdab89UL;
   MDbuf[2] = RMD_H2; // 0x98badcfeUL;
   MDbuf[3] = RMD_H3; // 0x10325476UL;

   return;
}


/********************************************************************/

void MD128compress(dword *MDbuf, dword *X)
{
   dword aa = MDbuf[0],  bb = MDbuf[1],  cc = MDbuf[2],  dd = MDbuf[3];
   dword aaa = MDbuf[0], bbb = MDbuf[1], ccc = MDbuf[2], ddd = MDbuf[3];

   /* round 1 */
   RMD128_FF(aa, bb, cc, dd, X[ 0], 11);
   RMD128_FF(dd, aa, bb, cc, X[ 1], 14);
   RMD128_FF(cc, dd, aa, bb, X[ 2], 15);
   RMD128_FF(bb, cc, dd, aa, X[ 3], 12);
   RMD128_FF(aa, bb, cc, dd, X[ 4],  5);
   RMD128_FF(dd, aa, bb, cc, X[ 5],  8);
   RMD128_FF(cc, dd, aa, bb, X[ 6],  7);
   RMD128_FF(bb, cc, dd, aa, X[ 7],  9);
   RMD128_FF(aa, bb, cc, dd, X[ 8], 11);
   RMD128_FF(dd, aa, bb, cc, X[ 9], 13);
   RMD128_FF(cc, dd, aa, bb, X[10], 14);
   RMD128_FF(bb, cc, dd, aa, X[11], 15);
   RMD128_FF(aa, bb, cc, dd, X[12],  6);
   RMD128_FF(dd, aa, bb, cc, X[13],  7);
   RMD128_FF(cc, dd, aa, bb, X[14],  9);
   RMD128_FF(bb, cc, dd, aa, X[15],  8);
                             
   /* round 2 */
   RMD128_GG(aa, bb, cc, dd, X[ 7],  7);
   RMD128_GG(dd, aa, bb, cc, X[ 4],  6);
   RMD128_GG(cc, dd, aa, bb, X[13],  8);
   RMD128_GG(bb, cc, dd, aa, X[ 1], 13);
   RMD128_GG(aa, bb, cc, dd, X[10], 11);
   RMD128_GG(dd, aa, bb, cc, X[ 6],  9);
   RMD128_GG(cc, dd, aa, bb, X[15],  7);
   RMD128_GG(bb, cc, dd, aa, X[ 3], 15);
   RMD128_GG(aa, bb, cc, dd, X[12],  7);
   RMD128_GG(dd, aa, bb, cc, X[ 0], 12);
   RMD128_GG(cc, dd, aa, bb, X[ 9], 15);
   RMD128_GG(bb, cc, dd, aa, X[ 5],  9);
   RMD128_GG(aa, bb, cc, dd, X[ 2], 11);
   RMD128_GG(dd, aa, bb, cc, X[14],  7);
   RMD128_GG(cc, dd, aa, bb, X[11], 13);
   RMD128_GG(bb, cc, dd, aa, X[ 8], 12);

   /* round 3 */
   RMD128_HH(aa, bb, cc, dd, X[ 3], 11);
   RMD128_HH(dd, aa, bb, cc, X[10], 13);
   RMD128_HH(cc, dd, aa, bb, X[14],  6);
   RMD128_HH(bb, cc, dd, aa, X[ 4],  7);
   RMD128_HH(aa, bb, cc, dd, X[ 9], 14);
   RMD128_HH(dd, aa, bb, cc, X[15],  9);
   RMD128_HH(cc, dd, aa, bb, X[ 8], 13);
   RMD128_HH(bb, cc, dd, aa, X[ 1], 15);
   RMD128_HH(aa, bb, cc, dd, X[ 2], 14);
   RMD128_HH(dd, aa, bb, cc, X[ 7],  8);
   RMD128_HH(cc, dd, aa, bb, X[ 0], 13);
   RMD128_HH(bb, cc, dd, aa, X[ 6],  6);
   RMD128_HH(aa, bb, cc, dd, X[13],  5);
   RMD128_HH(dd, aa, bb, cc, X[11], 12);
   RMD128_HH(cc, dd, aa, bb, X[ 5],  7);
   RMD128_HH(bb, cc, dd, aa, X[12],  5);

   /* round 4 */
   RMD128_II(aa, bb, cc, dd, X[ 1], 11);
   RMD128_II(dd, aa, bb, cc, X[ 9], 12);
   RMD128_II(cc, dd, aa, bb, X[11], 14);
   RMD128_II(bb, cc, dd, aa, X[10], 15);
   RMD128_II(aa, bb, cc, dd, X[ 0], 14);
   RMD128_II(dd, aa, bb, cc, X[ 8], 15);
   RMD128_II(cc, dd, aa, bb, X[12],  9);
   RMD128_II(bb, cc, dd, aa, X[ 4],  8);
   RMD128_II(aa, bb, cc, dd, X[13],  9);
   RMD128_II(dd, aa, bb, cc, X[ 3], 14);
   RMD128_II(cc, dd, aa, bb, X[ 7],  5);
   RMD128_II(bb, cc, dd, aa, X[15],  6);
   RMD128_II(aa, bb, cc, dd, X[14],  8);
   RMD128_II(dd, aa, bb, cc, X[ 5],  6);
   RMD128_II(cc, dd, aa, bb, X[ 6],  5);
   RMD128_II(bb, cc, dd, aa, X[ 2], 12);

   /* parallel round 1 */
   RMD128_III(aaa, bbb, ccc, ddd, X[ 5],  8); 
   RMD128_III(ddd, aaa, bbb, ccc, X[14],  9);
   RMD128_III(ccc, ddd, aaa, bbb, X[ 7],  9);
   RMD128_III(bbb, ccc, ddd, aaa, X[ 0], 11);
   RMD128_III(aaa, bbb, ccc, ddd, X[ 9], 13);
   RMD128_III(ddd, aaa, bbb, ccc, X[ 2], 15);
   RMD128_III(ccc, ddd, aaa, bbb, X[11], 15);
   RMD128_III(bbb, ccc, ddd, aaa, X[ 4],  5);
   RMD128_III(aaa, bbb, ccc, ddd, X[13],  7);
   RMD128_III(ddd, aaa, bbb, ccc, X[ 6],  7);
   RMD128_III(ccc, ddd, aaa, bbb, X[15],  8);
   RMD128_III(bbb, ccc, ddd, aaa, X[ 8], 11);
   RMD128_III(aaa, bbb, ccc, ddd, X[ 1], 14);
   RMD128_III(ddd, aaa, bbb, ccc, X[10], 14);
   RMD128_III(ccc, ddd, aaa, bbb, X[ 3], 12);
   RMD128_III(bbb, ccc, ddd, aaa, X[12],  6);

   /* parallel round 2 */
   RMD128_HHH(aaa, bbb, ccc, ddd, X[ 6],  9);
   RMD128_HHH(ddd, aaa, bbb, ccc, X[11], 13);
   RMD128_HHH(ccc, ddd, aaa, bbb, X[ 3], 15);
   RMD128_HHH(bbb, ccc, ddd, aaa, X[ 7],  7);
   RMD128_HHH(aaa, bbb, ccc, ddd, X[ 0], 12);
   RMD128_HHH(ddd, aaa, bbb, ccc, X[13],  8);
   RMD128_HHH(ccc, ddd, aaa, bbb, X[ 5],  9);
   RMD128_HHH(bbb, ccc, ddd, aaa, X[10], 11);
   RMD128_HHH(aaa, bbb, ccc, ddd, X[14],  7);
   RMD128_HHH(ddd, aaa, bbb, ccc, X[15],  7);
   RMD128_HHH(ccc, ddd, aaa, bbb, X[ 8], 12);
   RMD128_HHH(bbb, ccc, ddd, aaa, X[12],  7);
   RMD128_HHH(aaa, bbb, ccc, ddd, X[ 4],  6);
   RMD128_HHH(ddd, aaa, bbb, ccc, X[ 9], 15);
   RMD128_HHH(ccc, ddd, aaa, bbb, X[ 1], 13);
   RMD128_HHH(bbb, ccc, ddd, aaa, X[ 2], 11);

   /* parallel round 3 */   
   RMD128_GGG(aaa, bbb, ccc, ddd, X[15],  9);
   RMD128_GGG(ddd, aaa, bbb, ccc, X[ 5],  7);
   RMD128_GGG(ccc, ddd, aaa, bbb, X[ 1], 15);
   RMD128_GGG(bbb, ccc, ddd, aaa, X[ 3], 11);
   RMD128_GGG(aaa, bbb, ccc, ddd, X[ 7],  8);
   RMD128_GGG(ddd, aaa, bbb, ccc, X[14],  6);
   RMD128_GGG(ccc, ddd, aaa, bbb, X[ 6],  6);
   RMD128_GGG(bbb, ccc, ddd, aaa, X[ 9], 14);
   RMD128_GGG(aaa, bbb, ccc, ddd, X[11], 12);
   RMD128_GGG(ddd, aaa, bbb, ccc, X[ 8], 13);
   RMD128_GGG(ccc, ddd, aaa, bbb, X[12],  5);
   RMD128_GGG(bbb, ccc, ddd, aaa, X[ 2], 14);
   RMD128_GGG(aaa, bbb, ccc, ddd, X[10], 13);
   RMD128_GGG(ddd, aaa, bbb, ccc, X[ 0], 13);
   RMD128_GGG(ccc, ddd, aaa, bbb, X[ 4],  7);
   RMD128_GGG(bbb, ccc, ddd, aaa, X[13],  5);

   /* parallel round 4 */
   RMD128_FFF(aaa, bbb, ccc, ddd, X[ 8], 15);
   RMD128_FFF(ddd, aaa, bbb, ccc, X[ 6],  5);
   RMD128_FFF(ccc, ddd, aaa, bbb, X[ 4],  8);
   RMD128_FFF(bbb, ccc, ddd, aaa, X[ 1], 11);
   RMD128_FFF(aaa, bbb, ccc, ddd, X[ 3], 14);
   RMD128_FFF(ddd, aaa, bbb, ccc, X[11], 14);
   RMD128_FFF(ccc, ddd, aaa, bbb, X[15],  6);
   RMD128_FFF(bbb, ccc, ddd, aaa, X[ 0], 14);
   RMD128_FFF(aaa, bbb, ccc, ddd, X[ 5],  6);
   RMD128_FFF(ddd, aaa, bbb, ccc, X[12],  9);
   RMD128_FFF(ccc, ddd, aaa, bbb, X[ 2], 12);
   RMD128_FFF(bbb, ccc, ddd, aaa, X[13],  9);
   RMD128_FFF(aaa, bbb, ccc, ddd, X[ 9], 12);
   RMD128_FFF(ddd, aaa, bbb, ccc, X[ 7],  5);
   RMD128_FFF(ccc, ddd, aaa, bbb, X[10], 15);
   RMD128_FFF(bbb, ccc, ddd, aaa, X[14],  8);

   /* combine results */
   ddd += cc + MDbuf[1];               /* final result for MDbuf[0] */
   MDbuf[1] = MDbuf[2] + dd + aaa;
   MDbuf[2] = MDbuf[3] + aa + bbb;
   MDbuf[3] = MDbuf[0] + bb + ccc;
   MDbuf[0] = ddd;

   return;
}

/********************************************************************/

void MD128finish(dword *MDbuf, byte *strptr, dword lswlen, dword mswlen)
{
   unsigned int i;                                 /* counter       */
   dword        X[16];                             /* message words */

   memset(X, 0, 16*sizeof(dword));

   /* put bytes from strptr into X */
   for (i=0; i<(lswlen&63); i++) {
      /* byte i goes into word X[i div 4] at pos.  8*(i mod 4)  */
      X[i>>2] ^= (dword) *strptr++ << (8 * (i&3));
   }

   /* append the bit m_n == 1 */
   X[(lswlen>>2)&15] ^= (dword)1 << (8*(lswlen&3) + 7);

   if ((lswlen & 63) > 55) {
      /* length goes to next block */
      MD128compress(MDbuf, X);
      memset(X, 0, 16*sizeof(dword));
   }

   /* append length in bits*/
   X[14] = lswlen << 3;
   X[15] = (lswlen >> 29) | (mswlen << 3);
   MD128compress(MDbuf, X);

   return;
}

#endif // RIPEMD128



#if RIPEMD320

struct rmd320_ctx {
	uint64_t byte_count;
	uint32_t state[10];
	__le32 buffer[16];
};



#define K1  RMD_K1
#define K2  RMD_K2
#define K3  RMD_K3
#define K4  RMD_K4
#define K5  RMD_K5
#define KK1 RMD_K6
#define KK2 RMD_K7
#define KK3 RMD_K8
#define KK4 RMD_K9
#define KK5 RMD_K1
#define F1(x, y, z) (x ^ y ^ z)		/* XOR */
#define F2(x, y, z) (z ^ (x & (y ^ z)))	/* x ? y : z */
#define F3(x, y, z) ((x | ~y) ^ z)
#define F4(x, y, z) (y ^ (z & (x ^ y)))	/* z ? x : y */
#define F5(x, y, z) (x ^ (y | ~z))
#define ROUND(a, b, c, d, e, f, k, x, s)  { \
	(a) += f((b), (c), (d)) + le32_to_cpup(&(x)) + (k); \
	(a) = rol32((a), (s)) + (e); \
	(c) = rol32((c), 10); \
}
static void rmd320_transform(uint32_t *state, const __le32 *in)
{
	uint32_t aa, bb, cc, dd, ee, aaa, bbb, ccc, ddd, eee;
	/* Initialize left lane */
	aa = state[0];
	bb = state[1];
	cc = state[2];
	dd = state[3];
	ee = state[4];
	/* Initialize right lane */
	aaa = state[5];
	bbb = state[6];
	ccc = state[7];
	ddd = state[8];
	eee = state[9];
	/* round 1: left lane */
	ROUND(aa, bb, cc, dd, ee, F1, K1, in[0],  11);
	ROUND(ee, aa, bb, cc, dd, F1, K1, in[1],  14);
	ROUND(dd, ee, aa, bb, cc, F1, K1, in[2],  15);
	ROUND(cc, dd, ee, aa, bb, F1, K1, in[3],  12);
	ROUND(bb, cc, dd, ee, aa, F1, K1, in[4],   5);
	ROUND(aa, bb, cc, dd, ee, F1, K1, in[5],   8);
	ROUND(ee, aa, bb, cc, dd, F1, K1, in[6],   7);
	ROUND(dd, ee, aa, bb, cc, F1, K1, in[7],   9);
	ROUND(cc, dd, ee, aa, bb, F1, K1, in[8],  11);
	ROUND(bb, cc, dd, ee, aa, F1, K1, in[9],  13);
	ROUND(aa, bb, cc, dd, ee, F1, K1, in[10], 14);
	ROUND(ee, aa, bb, cc, dd, F1, K1, in[11], 15);
	ROUND(dd, ee, aa, bb, cc, F1, K1, in[12],  6);
	ROUND(cc, dd, ee, aa, bb, F1, K1, in[13],  7);
	ROUND(bb, cc, dd, ee, aa, F1, K1, in[14],  9);
	ROUND(aa, bb, cc, dd, ee, F1, K1, in[15],  8);
	/* round 1: right lane */
	ROUND(aaa, bbb, ccc, ddd, eee, F5, KK1, in[5],   8);
	ROUND(eee, aaa, bbb, ccc, ddd, F5, KK1, in[14],  9);
	ROUND(ddd, eee, aaa, bbb, ccc, F5, KK1, in[7],   9);
	ROUND(ccc, ddd, eee, aaa, bbb, F5, KK1, in[0],  11);
	ROUND(bbb, ccc, ddd, eee, aaa, F5, KK1, in[9],  13);
	ROUND(aaa, bbb, ccc, ddd, eee, F5, KK1, in[2],  15);
	ROUND(eee, aaa, bbb, ccc, ddd, F5, KK1, in[11], 15);
	ROUND(ddd, eee, aaa, bbb, ccc, F5, KK1, in[4],   5);
	ROUND(ccc, ddd, eee, aaa, bbb, F5, KK1, in[13],  7);
	ROUND(bbb, ccc, ddd, eee, aaa, F5, KK1, in[6],   7);
	ROUND(aaa, bbb, ccc, ddd, eee, F5, KK1, in[15],  8);
	ROUND(eee, aaa, bbb, ccc, ddd, F5, KK1, in[8],  11);
	ROUND(ddd, eee, aaa, bbb, ccc, F5, KK1, in[1],  14);
	ROUND(ccc, ddd, eee, aaa, bbb, F5, KK1, in[10], 14);
	ROUND(bbb, ccc, ddd, eee, aaa, F5, KK1, in[3],  12);
	ROUND(aaa, bbb, ccc, ddd, eee, F5, KK1, in[12],  6);
	/* Swap contents of "a" registers */
	swap(aa, aaa);
	/* round 2: left lane" */
	ROUND(ee, aa, bb, cc, dd, F2, K2, in[7],   7);
	ROUND(dd, ee, aa, bb, cc, F2, K2, in[4],   6);
	ROUND(cc, dd, ee, aa, bb, F2, K2, in[13],  8);
	ROUND(bb, cc, dd, ee, aa, F2, K2, in[1],  13);
	ROUND(aa, bb, cc, dd, ee, F2, K2, in[10], 11);
	ROUND(ee, aa, bb, cc, dd, F2, K2, in[6],   9);
	ROUND(dd, ee, aa, bb, cc, F2, K2, in[15],  7);
	ROUND(cc, dd, ee, aa, bb, F2, K2, in[3],  15);
	ROUND(bb, cc, dd, ee, aa, F2, K2, in[12],  7);
	ROUND(aa, bb, cc, dd, ee, F2, K2, in[0],  12);
	ROUND(ee, aa, bb, cc, dd, F2, K2, in[9],  15);
	ROUND(dd, ee, aa, bb, cc, F2, K2, in[5],   9);
	ROUND(cc, dd, ee, aa, bb, F2, K2, in[2],  11);
	ROUND(bb, cc, dd, ee, aa, F2, K2, in[14],  7);
	ROUND(aa, bb, cc, dd, ee, F2, K2, in[11], 13);
	ROUND(ee, aa, bb, cc, dd, F2, K2, in[8],  12);
	/* round 2: right lane */
	ROUND(eee, aaa, bbb, ccc, ddd, F4, KK2, in[6],   9);
	ROUND(ddd, eee, aaa, bbb, ccc, F4, KK2, in[11], 13);
	ROUND(ccc, ddd, eee, aaa, bbb, F4, KK2, in[3],  15);
	ROUND(bbb, ccc, ddd, eee, aaa, F4, KK2, in[7],   7);
	ROUND(aaa, bbb, ccc, ddd, eee, F4, KK2, in[0],  12);
	ROUND(eee, aaa, bbb, ccc, ddd, F4, KK2, in[13],  8);
	ROUND(ddd, eee, aaa, bbb, ccc, F4, KK2, in[5],   9);
	ROUND(ccc, ddd, eee, aaa, bbb, F4, KK2, in[10], 11);
	ROUND(bbb, ccc, ddd, eee, aaa, F4, KK2, in[14],  7);
	ROUND(aaa, bbb, ccc, ddd, eee, F4, KK2, in[15],  7);
	ROUND(eee, aaa, bbb, ccc, ddd, F4, KK2, in[8],  12);
	ROUND(ddd, eee, aaa, bbb, ccc, F4, KK2, in[12],  7);
	ROUND(ccc, ddd, eee, aaa, bbb, F4, KK2, in[4],   6);
	ROUND(bbb, ccc, ddd, eee, aaa, F4, KK2, in[9],  15);
	ROUND(aaa, bbb, ccc, ddd, eee, F4, KK2, in[1],  13);
	ROUND(eee, aaa, bbb, ccc, ddd, F4, KK2, in[2],  11);
	/* Swap contents of "b" registers */
	swap(bb, bbb);
	/* round 3: left lane" */
	ROUND(dd, ee, aa, bb, cc, F3, K3, in[3],  11);
	ROUND(cc, dd, ee, aa, bb, F3, K3, in[10], 13);
	ROUND(bb, cc, dd, ee, aa, F3, K3, in[14],  6);
	ROUND(aa, bb, cc, dd, ee, F3, K3, in[4],   7);
	ROUND(ee, aa, bb, cc, dd, F3, K3, in[9],  14);
	ROUND(dd, ee, aa, bb, cc, F3, K3, in[15],  9);
	ROUND(cc, dd, ee, aa, bb, F3, K3, in[8],  13);
	ROUND(bb, cc, dd, ee, aa, F3, K3, in[1],  15);
	ROUND(aa, bb, cc, dd, ee, F3, K3, in[2],  14);
	ROUND(ee, aa, bb, cc, dd, F3, K3, in[7],   8);
	ROUND(dd, ee, aa, bb, cc, F3, K3, in[0],  13);
	ROUND(cc, dd, ee, aa, bb, F3, K3, in[6],   6);
	ROUND(bb, cc, dd, ee, aa, F3, K3, in[13],  5);
	ROUND(aa, bb, cc, dd, ee, F3, K3, in[11], 12);
	ROUND(ee, aa, bb, cc, dd, F3, K3, in[5],   7);
	ROUND(dd, ee, aa, bb, cc, F3, K3, in[12],  5);
	/* round 3: right lane */
	ROUND(ddd, eee, aaa, bbb, ccc, F3, KK3, in[15],  9);
	ROUND(ccc, ddd, eee, aaa, bbb, F3, KK3, in[5],   7);
	ROUND(bbb, ccc, ddd, eee, aaa, F3, KK3, in[1],  15);
	ROUND(aaa, bbb, ccc, ddd, eee, F3, KK3, in[3],  11);
	ROUND(eee, aaa, bbb, ccc, ddd, F3, KK3, in[7],   8);
	ROUND(ddd, eee, aaa, bbb, ccc, F3, KK3, in[14],  6);
	ROUND(ccc, ddd, eee, aaa, bbb, F3, KK3, in[6],   6);
	ROUND(bbb, ccc, ddd, eee, aaa, F3, KK3, in[9],  14);
	ROUND(aaa, bbb, ccc, ddd, eee, F3, KK3, in[11], 12);
	ROUND(eee, aaa, bbb, ccc, ddd, F3, KK3, in[8],  13);
	ROUND(ddd, eee, aaa, bbb, ccc, F3, KK3, in[12],  5);
	ROUND(ccc, ddd, eee, aaa, bbb, F3, KK3, in[2],  14);
	ROUND(bbb, ccc, ddd, eee, aaa, F3, KK3, in[10], 13);
	ROUND(aaa, bbb, ccc, ddd, eee, F3, KK3, in[0],  13);
	ROUND(eee, aaa, bbb, ccc, ddd, F3, KK3, in[4],   7);
	ROUND(ddd, eee, aaa, bbb, ccc, F3, KK3, in[13],  5);
	/* Swap contents of "c" registers */
	swap(cc, ccc);
	/* round 4: left lane" */
	ROUND(cc, dd, ee, aa, bb, F4, K4, in[1],  11);
	ROUND(bb, cc, dd, ee, aa, F4, K4, in[9],  12);
	ROUND(aa, bb, cc, dd, ee, F4, K4, in[11], 14);
	ROUND(ee, aa, bb, cc, dd, F4, K4, in[10], 15);
	ROUND(dd, ee, aa, bb, cc, F4, K4, in[0],  14);
	ROUND(cc, dd, ee, aa, bb, F4, K4, in[8],  15);
	ROUND(bb, cc, dd, ee, aa, F4, K4, in[12],  9);
	ROUND(aa, bb, cc, dd, ee, F4, K4, in[4],   8);
	ROUND(ee, aa, bb, cc, dd, F4, K4, in[13],  9);
	ROUND(dd, ee, aa, bb, cc, F4, K4, in[3],  14);
	ROUND(cc, dd, ee, aa, bb, F4, K4, in[7],   5);
	ROUND(bb, cc, dd, ee, aa, F4, K4, in[15],  6);
	ROUND(aa, bb, cc, dd, ee, F4, K4, in[14],  8);
	ROUND(ee, aa, bb, cc, dd, F4, K4, in[5],   6);
	ROUND(dd, ee, aa, bb, cc, F4, K4, in[6],   5);
	ROUND(cc, dd, ee, aa, bb, F4, K4, in[2],  12);
	/* round 4: right lane */
	ROUND(ccc, ddd, eee, aaa, bbb, F2, KK4, in[8],  15);
	ROUND(bbb, ccc, ddd, eee, aaa, F2, KK4, in[6],   5);
	ROUND(aaa, bbb, ccc, ddd, eee, F2, KK4, in[4],   8);
	ROUND(eee, aaa, bbb, ccc, ddd, F2, KK4, in[1],  11);
	ROUND(ddd, eee, aaa, bbb, ccc, F2, KK4, in[3],  14);
	ROUND(ccc, ddd, eee, aaa, bbb, F2, KK4, in[11], 14);
	ROUND(bbb, ccc, ddd, eee, aaa, F2, KK4, in[15],  6);
	ROUND(aaa, bbb, ccc, ddd, eee, F2, KK4, in[0],  14);
	ROUND(eee, aaa, bbb, ccc, ddd, F2, KK4, in[5],   6);
	ROUND(ddd, eee, aaa, bbb, ccc, F2, KK4, in[12],  9);
	ROUND(ccc, ddd, eee, aaa, bbb, F2, KK4, in[2],  12);
	ROUND(bbb, ccc, ddd, eee, aaa, F2, KK4, in[13],  9);
	ROUND(aaa, bbb, ccc, ddd, eee, F2, KK4, in[9],  12);
	ROUND(eee, aaa, bbb, ccc, ddd, F2, KK4, in[7],   5);
	ROUND(ddd, eee, aaa, bbb, ccc, F2, KK4, in[10], 15);
	ROUND(ccc, ddd, eee, aaa, bbb, F2, KK4, in[14],  8);
	/* Swap contents of "d" registers */
	swap(dd, ddd);
	/* round 5: left lane" */
	ROUND(bb, cc, dd, ee, aa, F5, K5, in[4],   9);
	ROUND(aa, bb, cc, dd, ee, F5, K5, in[0],  15);
	ROUND(ee, aa, bb, cc, dd, F5, K5, in[5],   5);
	ROUND(dd, ee, aa, bb, cc, F5, K5, in[9],  11);
	ROUND(cc, dd, ee, aa, bb, F5, K5, in[7],   6);
	ROUND(bb, cc, dd, ee, aa, F5, K5, in[12],  8);
	ROUND(aa, bb, cc, dd, ee, F5, K5, in[2],  13);
	ROUND(ee, aa, bb, cc, dd, F5, K5, in[10], 12);
	ROUND(dd, ee, aa, bb, cc, F5, K5, in[14],  5);
	ROUND(cc, dd, ee, aa, bb, F5, K5, in[1],  12);
	ROUND(bb, cc, dd, ee, aa, F5, K5, in[3],  13);
	ROUND(aa, bb, cc, dd, ee, F5, K5, in[8],  14);
	ROUND(ee, aa, bb, cc, dd, F5, K5, in[11], 11);
	ROUND(dd, ee, aa, bb, cc, F5, K5, in[6],   8);
	ROUND(cc, dd, ee, aa, bb, F5, K5, in[15],  5);
	ROUND(bb, cc, dd, ee, aa, F5, K5, in[13],  6);
	/* round 5: right lane */
	ROUND(bbb, ccc, ddd, eee, aaa, F1, KK5, in[12],  8);
	ROUND(aaa, bbb, ccc, ddd, eee, F1, KK5, in[15],  5);
	ROUND(eee, aaa, bbb, ccc, ddd, F1, KK5, in[10], 12);
	ROUND(ddd, eee, aaa, bbb, ccc, F1, KK5, in[4],   9);
	ROUND(ccc, ddd, eee, aaa, bbb, F1, KK5, in[1],  12);
	ROUND(bbb, ccc, ddd, eee, aaa, F1, KK5, in[5],   5);
	ROUND(aaa, bbb, ccc, ddd, eee, F1, KK5, in[8],  14);
	ROUND(eee, aaa, bbb, ccc, ddd, F1, KK5, in[7],   6);
	ROUND(ddd, eee, aaa, bbb, ccc, F1, KK5, in[6],   8);
	ROUND(ccc, ddd, eee, aaa, bbb, F1, KK5, in[2],  13);
	ROUND(bbb, ccc, ddd, eee, aaa, F1, KK5, in[13],  6);
	ROUND(aaa, bbb, ccc, ddd, eee, F1, KK5, in[14],  5);
	ROUND(eee, aaa, bbb, ccc, ddd, F1, KK5, in[0],  15);
	ROUND(ddd, eee, aaa, bbb, ccc, F1, KK5, in[3],  13);
	ROUND(ccc, ddd, eee, aaa, bbb, F1, KK5, in[9],  11);
	ROUND(bbb, ccc, ddd, eee, aaa, F1, KK5, in[11], 11);
	/* Swap contents of "e" registers */
	swap(ee, eee);
	/* combine results */
	state[0] += aa;
	state[1] += bb;
	state[2] += cc;
	state[3] += dd;
	state[4] += ee;
	state[5] += aaa;
	state[6] += bbb;
	state[7] += ccc;
	state[8] += ddd;
	state[9] += eee;
}
static int rmd320_init(struct shash_desc *desc)
{
	struct rmd320_ctx *rctx = shash_desc_ctx(desc);
	rctx->byte_count = 0;
	rctx->state[0] = RMD_H0;
	rctx->state[1] = RMD_H1;
	rctx->state[2] = RMD_H2;
	rctx->state[3] = RMD_H3;
	rctx->state[4] = RMD_H4;
	rctx->state[5] = RMD_H5;
	rctx->state[6] = RMD_H6;
	rctx->state[7] = RMD_H7;
	rctx->state[8] = RMD_H8;
	rctx->state[9] = RMD_H9;
	memset(rctx->buffer, 0, sizeof(rctx->buffer));
	return 0;
}
static int rmd320_update(struct shash_desc *desc, const u8 *data,
			 unsigned int len)
{
	struct rmd320_ctx *rctx = shash_desc_ctx(desc);
	const u32 avail = sizeof(rctx->buffer) - (rctx->byte_count & 0x3f);
	rctx->byte_count += len;
	/* Enough space in buffer? If so copy and we're done */
	if (avail > len) {
		memcpy((char *)rctx->buffer + (sizeof(rctx->buffer) - avail),
		       data, len);
		goto out;
	}
	memcpy((char *)rctx->buffer + (sizeof(rctx->buffer) - avail),
	       data, avail);
	rmd320_transform(rctx->state, rctx->buffer);
	data += avail;
	len -= avail;
	while (len >= sizeof(rctx->buffer)) {
		memcpy(rctx->buffer, data, sizeof(rctx->buffer));
		rmd320_transform(rctx->state, rctx->buffer);
		data += sizeof(rctx->buffer);
		len -= sizeof(rctx->buffer);
	}
	memcpy(rctx->buffer, data, len);
out:
	return 0;
}
/* Add padding and return the message digest. */
static int rmd320_final(struct shash_desc *desc, u8 *out)
{
	struct rmd320_ctx *rctx = shash_desc_ctx(desc);
	u32 i, index, padlen;
	__le64 bits;
	__le32 *dst = (__le32 *)out;
	static const u8 padding[64] = { 0x80, };
	bits = cpu_to_le64(rctx->byte_count << 3);
	/* Pad out to 56 mod 64 */
	index = rctx->byte_count & 0x3f;
	padlen = (index < 56) ? (56 - index) : ((64+56) - index);
	rmd320_update(desc, padding, padlen);
	/* Append length */
	rmd320_update(desc, (const u8 *)&bits, sizeof(bits));
	/* Store state in digest */
	for (i = 0; i < 10; i++)
		dst[i] = cpu_to_le32p(&rctx->state[i]);
	/* Wipe context */
	memset(rctx, 0, sizeof(*rctx));
	return 0;
}

#endif




/// 7.8.1 Macros for format specifiers ------------------------
#if 1 /// !defined(__cplusplus) 
// The fprintf macros for signed integers are:
#define PRId8       "d"
#define PRIi8       "i"
#define PRIdLEAST8  "d"
#define PRIiLEAST8  "i"
#define PRIdFAST8   "d"
#define PRIiFAST8   "i"

#define PRId16       "hd"
#define PRIi16       "hi"
#define PRIdLEAST16  "hd"
#define PRIiLEAST16  "hi"
#define PRIdFAST16   "hd"
#define PRIiFAST16   "hi"

#define PRId32       "I32d"
#define PRIi32       "I32i"
#define PRIdLEAST32  "I32d"
#define PRIiLEAST32  "I32i"
#define PRIdFAST32   "I32d"
#define PRIiFAST32   "I32i"

#define PRId64       "I64d"
#define PRIi64       "I64i"
#define PRIdLEAST64  "I64d"
#define PRIiLEAST64  "I64i"
#define PRIdFAST64   "I64d"
#define PRIiFAST64   "I64i"

#define PRIdMAX     "I64d"
#define PRIiMAX     "I64i"

#define PRIdPTR     "Id"
#define PRIiPTR     "Ii"

// The fprintf macros for unsigned integers are:
#define PRIo8       "o"
#define PRIu8       "u"
#define PRIx8       "x"
#define PRIX8       "X"
#define PRIoLEAST8  "o"
#define PRIuLEAST8  "u"
#define PRIxLEAST8  "x"
#define PRIXLEAST8  "X"
#define PRIoFAST8   "o"
#define PRIuFAST8   "u"
#define PRIxFAST8   "x"
#define PRIXFAST8   "X"

#define PRIo16       "ho"
#define PRIu16       "hu"
#define PRIx16       "hx"
#define PRIX16       "hX"
#define PRIoLEAST16  "ho"
#define PRIuLEAST16  "hu"
#define PRIxLEAST16  "hx"
#define PRIXLEAST16  "hX"
#define PRIoFAST16   "ho"
#define PRIuFAST16   "hu"
#define PRIxFAST16   "hx"
#define PRIXFAST16   "hX"

#define PRIo32       "I32o"
#define PRIu32       "I32u"
#define PRIx32       "I32x"
#define PRIX32       "I32X"
#define PRIoLEAST32  "I32o"
#define PRIuLEAST32  "I32u"
#define PRIxLEAST32  "I32x"
#define PRIXLEAST32  "I32X"
#define PRIoFAST32   "I32o"
#define PRIuFAST32   "I32u"
#define PRIxFAST32   "I32x"
#define PRIXFAST32   "I32X"

#define PRIo64       "I64o"
#define PRIu64       "I64u"
#define PRIx64       "I64x"
#define PRIX64       "I64X"
#define PRIoLEAST64  "I64o"
#define PRIuLEAST64  "I64u"
#define PRIxLEAST64  "I64x"
#define PRIXLEAST64  "I64X"
#define PRIoFAST64   "I64o"
#define PRIuFAST64   "I64u"
#define PRIxFAST64   "I64x"
#define PRIXFAST64   "I64X"

#define PRIoMAX     "I64o"
#define PRIuMAX     "I64u"
#define PRIxMAX     "I64x"
#define PRIXMAX     "I64X"

#define PRIoPTR     "Io"
#define PRIuPTR     "Iu"
#define PRIxPTR     "Ix"
#define PRIXPTR     "IX"

// The fscanf macros for signed integers are:
#define SCNd8       "d"
#define SCNi8       "i"
#define SCNdLEAST8  "d"
#define SCNiLEAST8  "i"
#define SCNdFAST8   "d"
#define SCNiFAST8   "i"

#define SCNd16       "hd"
#define SCNi16       "hi"
#define SCNdLEAST16  "hd"
#define SCNiLEAST16  "hi"
#define SCNdFAST16   "hd"
#define SCNiFAST16   "hi"

#define SCNd32       "ld"
#define SCNi32       "li"
#define SCNdLEAST32  "ld"
#define SCNiLEAST32  "li"
#define SCNdFAST32   "ld"
#define SCNiFAST32   "li"

#define SCNd64       "I64d"
#define SCNi64       "I64i"
#define SCNdLEAST64  "I64d"
#define SCNiLEAST64  "I64i"
#define SCNdFAST64   "I64d"
#define SCNiFAST64   "I64i"

#define SCNdMAX     "I64d"
#define SCNiMAX     "I64i"

#ifdef _WIN64 // [
#define SCNdPTR     "I64d"
#define SCNiPTR     "I64i"
#else  // _WIN64 ][
#define SCNdPTR     "ld"
#define SCNiPTR     "li"
#endif  // _WIN64 ]

// The fscanf macros for unsigned integers are:
#define SCNo8       "o"
#define SCNu8       "u"
#define SCNx8       "x"
#define SCNX8       "X"
#define SCNoLEAST8  "o"
#define SCNuLEAST8  "u"
#define SCNxLEAST8  "x"
#define SCNXLEAST8  "X"
#define SCNoFAST8   "o"
#define SCNuFAST8   "u"
#define SCNxFAST8   "x"
#define SCNXFAST8   "X"

#define SCNo16       "ho"
#define SCNu16       "hu"
#define SCNx16       "hx"
#define SCNX16       "hX"
#define SCNoLEAST16  "ho"
#define SCNuLEAST16  "hu"
#define SCNxLEAST16  "hx"
#define SCNXLEAST16  "hX"
#define SCNoFAST16   "ho"
#define SCNuFAST16   "hu"
#define SCNxFAST16   "hx"
#define SCNXFAST16   "hX"

#define SCNo32       "lo"
#define SCNu32       "lu"
#define SCNx32       "lx"
#define SCNX32       "lX"
#define SCNoLEAST32  "lo"
#define SCNuLEAST32  "lu"
#define SCNxLEAST32  "lx"
#define SCNXLEAST32  "lX"
#define SCNoFAST32   "lo"
#define SCNuFAST32   "lu"
#define SCNxFAST32   "lx"
#define SCNXFAST32   "lX"

#define SCNo64       "I64o"
#define SCNu64       "I64u"
#define SCNx64       "I64x"
#define SCNX64       "I64X"
#define SCNoLEAST64  "I64o"
#define SCNuLEAST64  "I64u"
#define SCNxLEAST64  "I64x"
#define SCNXLEAST64  "I64X"
#define SCNoFAST64   "I64o"
#define SCNuFAST64   "I64u"
#define SCNxFAST64   "I64x"
#define SCNXFAST64   "I64X"

#define SCNoMAX     "I64o"
#define SCNuMAX     "I64u"
#define SCNxMAX     "I64x"
#define SCNXMAX     "I64X"

#ifdef _WIN64 // [
#define SCNoPTR     "I64o"
#define SCNuPTR     "I64u"
#define SCNxPTR     "I64x"
#define SCNXPTR     "I64X"
#else  // _WIN64 ][
#define SCNoPTR     "lo"
#define SCNuPTR     "lu"
#define SCNxPTR     "lx"
#define SCNXPTR     "lX"
#endif  // _WIN64 ]

#endif /// ----------------------------------





#ifndef TRUE
#define TRUE 		1
#define FALSE 		0
#endif

typedef enum
{
	false = 0,
	true = 1,
}bool;



/// --float / double to hex --------------------------
typedef union {
	float		   flt_val;		
	unsigned char  flt_bin[sizeof(float)]; 
} __float_hex_union; /// = { 0.866f };	 

typedef union { 	  
	double	       dbl_val;	   
	unsigned char  dbl_bin[sizeof(double)];    
} __double_hex_union;
/// --float / double to hex --------------------------



typedef struct _BIN_IMAGE_HEADER{	
	unsigned char sync[7];	 
	unsigned long addr;	  
	unsigned long length;	
}BIN_IMAGE_HEADER;	 
   
typedef struct _BIN_SECTION{    
	unsigned long addr;      /// 4byte
	unsigned long length;    /// 4byte
	unsigned long checksum;  /// 4byte
} BIN_SECTION;  


/// Said first TOC information the relative address 0x40 at two four-byte data, 
/// the first parity information (ECEC), the second refers clear address of the ROMHDR structure. 
/// This ROMHDR structure contains a lot of useful information, such as image file module file, 
/// there are some segments (Section) information, such as eboot is to relocate the global variable. 
		
typedef struct ROM_HEADER { 
	ULONG	   dllfirst;			/// First DLL address 
	ULONG	   dlllast; 			/// Last DLL address 
	ULONG	   physfirst;			/// First physical address 
	ULONG	   physlast;			/// Highest physical address 
	ULONG	   nummods; 			/// Number of TOCentry's 
	ULONG	   ulRamStart;			/// Start of RAM 
	ULONG	   ulRAMFree;			/// Start of RAM free space 
	ULONG	   ulRAMEnd;			/// End of RAM 
	ULONG	   ulCopyEntries;		/// Number of copy section entries 
	ULONG	   ulCopyOffset;		/// Offset to copy section 
	ULONG	   ulProfileLen;		/// Length of PROFentries RAM 
	ULONG	   ulProfileOffset; 	/// Offset to PROFentries 
	ULONG	   numfiles;			/// Number of FILES 
	ULONG	   ulKernelFlags;		/// Optional kernel flags from ROMFLAGS. Bib config option 
	ULONG	   ulFSRamPercent;		/// Percentage of RAM used for filesystem 
									// from FSRAMPERCENT .bib config option
									// byte 0 = #4K chunks/Mbyte of RAM for filesystem 0-2Mbytes 0-255
									// byte 1 = #4K chunks/Mbyte of RAM for filesystem 2-4Mbytes 0-255
									// byte 2 = #4K chunks/Mbyte of RAM for filesystem 4-6Mbytes 0-255
									// byte 3 = #4K chunks/Mbyte of RAM for filesystem > 6Mbytes 0-255

	ULONG	   ulDrivglobStart; 	/// Device driver global starting address 
	ULONG	   ulDrivglobLen;		/// Device driver global length 
	USHORT     usCPUType;			/// CPU (machine) Type 
	USHORT     usMiscFlags; 		/// Miscellaneous flags 
	PVOID	   pExtensions; 		/// Pointer to ROM Header extensions 
	ULONG	   ulTrackingStart; 	/// Tracking memory starting address 
	ULONG	   ulTrackingLen;		/// Tracking memory ending address 
} ROMHDR; 


typedef struct ROMChain_t {
    struct ROMChain_t *pNext;
    ROMHDR *pTOC;
} ROMChain_t;


//
// ROM Header extension: PID
//
#define PID_LENGTH 				10

typedef struct ROMPID {
  union{
    DWORD dwPID[PID_LENGTH];        // PID
    struct{
      char  name[(PID_LENGTH - 4) * sizeof(DWORD)];
      DWORD type;
      PVOID pdata;
      DWORD length;
      DWORD reserved;
    };
  };
  PVOID pNextExt;                 // pointer to next extension if any
} ROMPID, EXTENSION;

typedef struct _RECORD_DATA {
    DWORD dwStartAddress;
    DWORD dwLength;
    DWORD dwChecksum;
    DWORD dwFilePointer;
} RECORD_DATA, *PRECORD_DATA;

#define MAX_RECORDS 2048
RECORD_DATA g_Records[MAX_RECORDS];



#if CONVERT_BMP2C
typedef union tagPixelData
{
    struct
    {
        WORD red:4;
        WORD green:4;
        WORD blue:4;
        WORD unused:4;
    } color754xx;
    struct
    {
        WORD red:5;
        WORD green:5;
        WORD blue:5;
        WORD unused:1;
    } color555;
    struct
    {
        WORD red:5;
        WORD green:6;
        WORD blue:5;
    } color565;
    struct
    {
        DWORD red:8;
        DWORD green:8;
        DWORD blue:8;
        DWORD alpha:8;
    } color888;
    DWORD pixel32;
    WORD pixel16[2];
} PixelData;


#define TO555(pd, r, g, b) \
		pd.color555.red = (r >> 3); \
		pd.color555.green = (g >> 3); \
		pd.color555.blue = (b >> 3); \
		pd.color555.unused = 0;

#define TO565(pd, r, g, b) \
		pd.color565.red = (r >> 3); \
		pd.color565.green = (g >> 2); \
		pd.color565.blue = (b >> 3);

#define TO888(pd, r, g, b) \
		pd.color888.red = r; \
		pd.color888.green = g; \
		pd.color888.blue = b; \
		pd.color888.alpha = 0;

#define TO_lPC754XX(pd, r, g, b) \
		pd.color754xx.red = (r >> 4); \
		pd.color754xx.green = (g >> 4); \
		pd.color754xx.blue = (b >> 4); \
		pd.color754xx.unused = 0;



#pragma pack(push, 1)                // 구조체를 1바이트 크기로 정렬

typedef struct _BITMAPFILEHEADER	 // BMP 비트맵 파일 헤더 구조체
{
	unsigned short bfType;			 // BMP 파일 매직 넘버
	unsigned int   bfSize;			 // 파일 크기
	unsigned short bfReserved1; 	 // 예약
	unsigned short bfReserved2; 	 // 예약
	unsigned int   bfOffBits;		 // 비트맵 데이터의 시작 위치
} BITMAPFILEHEADER;

typedef struct _BITMAPINFOHEADER	 // BMP 비트맵 정보 헤더 구조체(DIB 헤더)
{
	unsigned int   biSize;			 // 현재 구조체의 크기
	int 		   biWidth; 		 // 비트맵 이미지의 가로 크기
	int 		   biHeight;		 // 비트맵 이미지의 세로 크기
	unsigned short biPlanes;		 // 사용하는 색상판의 수
	unsigned short biBitCount;		 // 픽셀 하나를 표현하는 비트 수
	unsigned int   biCompression;	 // 압축 방식
	unsigned int   biSizeImage; 	 // 비트맵 이미지의 픽셀 데이터 크기
	int 		   biXPelsPerMeter;  // 그림의 가로 해상도(미터당 픽셀)
	int 		   biYPelsPerMeter;  // 그림의 세로 해상도(미터당 픽셀)
	unsigned int   biClrUsed;		 // 색상 테이블에서 실제 사용되는 색상 수
	unsigned int   biClrImportant;	 // 비트맵을 표현하기 위해 필요한 색상 인덱스 수
} BITMAPINFOHEADER;

typedef struct _RGBTRIPLE			 // 24비트 비트맵 이미지의 픽셀 구조체
{
	unsigned char rgbtBlue; 		 // 파랑
	unsigned char rgbtGreen;		 // 초록
	unsigned char rgbtRed;			 // 빨강
} RGBTRIPLE;

#pragma pack(pop)


#define PIXEL_SIZE   3    // 픽셀 한 개의 크기 3바이트(24비트)
#define PIXEL_ALIGN  4    // 픽셀 데이터 가로 한 줄은 4의 배수 크기로 저장됨

#endif /// CONVERT_BMP2C



#if MODIFIED_JULIAN_DATE /// 2014.07.04
////////////////////////////////////////////
/// Modified Julian Date
/// 2013.09.14 by top.joo
////////////////////////////////////////////

/* ------------------------------------------------- */
/* --  2013.09.14 : KBS EWS (Emergency Warning Service) --- */
/* ------------------------------------------------- */
/* --- Modified Julian Date ------ */
typedef struct {
	int  m_year;
	int  m_month;
	int  m_day;
	int  m_hour;
	int  m_mins;
	int  m_secs;
	int  m_millis;
} mjd_timestamp;

#define TDMB_REFER_MJD_VALUE  			(15020)  /* Reference TDMB : 1900.1.1 (15020.0) */


/// ---------- Modified JD Examles ----------------------
/// *** -> 50000.000000  ---> [ 1995, 10, 10, 0, 0, 0 0 ] 
/// *** -> 56551.338982  ---> [ 2013, 9, 16, 8, 8, 8 6 ]  
/// *** -> 00000.000000  ---> [ 1858, 11, 17, 0, 0, 0 0 ] 
/// *** -> 40000.000000  ---> [ 1968, 5, 24, 0, 0, 0 0 ]
/// *** -> 47892.000000  ---> [ 1990, 1, 1, 0, 0, 0 0 ]  
/// *** -> 48000.000000  ---> [ 1990, 4, 19, 0, 0, 0 0 ]  
/// *** -> 15020.000000  ---> [ 1900, 1, 1, 0, 0, 0 0 ]  
/// *** -> 15021.000000  ---> [ 1900, 1, 2, 0, 0, 0 0 ]  
/// --------------------------------------------------

#define MJD_PARAM 			(2400000.5)

/// New function at 2013.09.14
/// ---- 2013.09.14
/// ---- Modified Julian Date ------
double Convert2MJD(mjd_timestamp YYMMDD_Time) 
{
	int y = YYMMDD_Time.m_year;
	int m = YYMMDD_Time.m_month;
	double d = (double) YYMMDD_Time.m_day;

	d = d + ((YYMMDD_Time.m_hour / 24.0) +
	         (YYMMDD_Time.m_mins / 1440.0) +
	         (YYMMDD_Time.m_secs / 86400.0) +
	         (YYMMDD_Time.m_millis / 86400000.0));
	
	if (m == 1 || m == 2) {
		y--;
		m = m + 12;
	}

	double a = floor(y / 100.0);
	double b = 2 - a + floor(a / 4.0);

	return (floor(365.25 * (y + 4716.0)) +
			floor(30.6001 * (m + 1)) + d + b - 1524.5) 
			- 2400000.5;  // for Julian Day omit the 2400000.5 term
}



/// ----------------------------------------------------------------------
/// Converts an Modified Julian Day Number (double) to an integer array representing
/// a timestamp (year,month,day,hours,mins,secs,millis).
/// ----------------------------------------------------------------------
mjd_timestamp Convert2Timestamp(double mjd) 
{
	mjd_timestamp ymd_hms;
	int a, b, c, d, e, z;

	/* if a JDN is passed as argument, omit the 2400000.5 term */
	double jd = mjd + 2400000.5 + 0.5;  
	double f, x;

	z = (int)floor(jd);
	f = jd - z;

	if (z >= 2299161) 
	{
		int alpha = (int)floor((z - 1867216.25) / 36524.25);
		a = z + 1 + alpha - (int)floor(alpha / 4.0);
	} 
	else 
	{
		a = z;
	}

	b = a + 1524;
	c = (int) floor((b - 122.1) / 365.25);
	d = (int) floor(365.25 * c);
	e = (int) floor((b - d) / 30.6001);

	/// Day --
	ymd_hms.m_day   = b - d - (int) floor(30.6001 * e);

	/// Month --
	ymd_hms.m_month = (e < 14) ? (e - 1) : (e - 13);

	/// Year --
	ymd_hms.m_year  = (ymd_hms.m_month > 2) ? (c - 4716) : (c - 4715);


	/// Hour --
	f = f * 24.0;
	x = floor(f);
	ymd_hms.m_hour = (int)x;
	f = f - x;

	/// Minutes --
	f = f * 60.0;
	x = floor(f);
	ymd_hms.m_mins = (int)x;
	f = f - x;

	/// seconds --
	f = f * 60.0;
	x = floor(f);
	ymd_hms.m_secs = (int)x;
	f = f - x;

	/// milli-sec ---
	f = f * 1000.0;
	x = floor(f);
	ymd_hms.m_millis= (int)x;
	f = f - x;

    return ymd_hms;
}


#define RADIX 			10 /// Decial,  2:binary

#endif /// MODIFIED_JULIAN_DATE
/////////////////////////////////////////////////////////////////




#if IDEA_ENCIPHER_ALGORITHM

/*
  Author:  Pate Williams (c) 1997

  IDEA (International Data Encryption Algorithm).
  See "Handbook of Applied Cryptography" by Alfred
  J. Menezes et al 7.6 Section pages 263 - 266.
*/


#define DEBUG

typedef unsigned long ulong;
typedef unsigned short ushort;

ushort add(long a, long b)
{
	return (ushort)((a + b) % 65536l);
}

ushort multiply(long a, long b)
{
	long ch, cl, c;

	if (a == 0) a = 65536l;
	if (b == 0) b = 65536l;
	c = a * b;
	if (c) 
	{
		ch = (c >> 16) & 65535l;
		cl = c & 65535l;
		if (cl >= ch) 
			return (ushort) (cl - ch);
		return (ushort) ((cl - ch + 65537l) & 65535l);
	}
	if (a == b) return 1;
	return 0;
}



void IDEA_encryption(ushort *X, ushort *Y, long **K)
{
	ushort a, i, r, t0, t1, t2;

	for (r = 0; r < 8; r++) 
	{
		X[0] = multiply(X[0], K[r][0]);
		X[3] = multiply(X[3], K[r][3]);
		X[1] = add(X[1], K[r][1]);
		X[2] = add(X[2], K[r][2]);
		t0 = multiply(K[r][4], X[0] ^ X[2]);
		t1 = multiply(K[r][5], add(t0, X[1] ^ X[3]));
		t2 = add(t0, t1);
		X[0] ^= t1;
		X[3] ^= t2;
		a = X[1] ^ t2;
		X[1] = X[2] ^ t1;
		X[2] = a;
#ifdef DEBUG
		printf("%u ", r + 1);
		for (i = 0; i < 6; i++)
			printf("%4x ", (ushort) K[r][i]);
		printf("%4x %4x %4x %4x\n", X[0], X[1], X[2], X[3]);
#endif
	}
	
	Y[0] = multiply(X[0], K[8][0]);
	Y[3] = multiply(X[3], K[8][3]);
	Y[1] = add(X[2], K[8][1]);
	Y[2] = add(X[1], K[8][2]);
#ifdef DEBUG
	printf("9 ");
	for (i = 0; i < 6; i++)
		printf("%4x ", (ushort) K[8][i]);
	printf("%4x %4x %4x %4x\n", Y[0], Y[1], Y[2], Y[3]);
#endif
}

ushort bits_to_ushort(ushort *bits)
{
	ushort i, value = bits[0];

	for (i = 1; i < 16; i++)
		value = (ushort) ((value << 1) + bits[i]);
	return value;
}

void ushort_to_bits(ushort number, ushort *bits)
{
	ushort i, temp[16];

	for (i = 0; i < 16; i++) 
	{
		temp[i] = (ushort) (number & 1);
		number >>= 1;
	}
	for (i = 0; i < 16; i++)
		bits[i] = temp[15 - i];
}

void cyclic_left_shift(ushort index, ushort *bits1,
                       ushort *bits2, long **K)
{
	ushort i, j;

	if (index == 0) 
	{
		for (i = 0; i < 6; i++)
			ushort_to_bits((ushort) K[0][i], bits1 + 16 * i);
		ushort_to_bits((ushort) K[1][0], bits1 + 96);
		ushort_to_bits((ushort) K[1][1], bits1 + 112);
	}

	i = 0;
	for (j = 25; j < 128; j++)
		bits2[i++] = bits1[j];

	for (j = 0; j < 25; j++)
		bits2[i++] = bits1[j];

	switch (index) 
	{
		case 0 :
			for (i = 2; i < 6; i++)
				K[1][i] = bits_to_ushort(bits2 + 16 * (i - 2));
			for (i = 0; i < 4; i++)
				K[2][i] = bits_to_ushort(bits2 + 64 + 16 * i);
			break;

		case 1 :
			K[2][4] = bits_to_ushort(bits2);
			K[2][5] = bits_to_ushort(bits2 + 16);

			for (i = 0; i < 6; i++)
				K[3][i] = bits_to_ushort(bits2 + 32 + 16 * i);
			break;

		case 2 :
			for (i = 0; i < 6; i++)
				K[4][i] = bits_to_ushort(bits2 + 16 * i);
			K[5][0] = bits_to_ushort(bits2 + 96);
			K[5][1] = bits_to_ushort(bits2 + 112);
			break;

		case 3 :
			for (i = 2; i < 6; i++)
				K[5][i] = bits_to_ushort(bits2 + 16 * (i - 2));
			for (i = 0; i < 4; i++)
				K[6][i] = bits_to_ushort(bits2 + 64 + 16 * i);
			break;

		case 4 :
			K[6][4] = bits_to_ushort(bits2);
			K[6][5] = bits_to_ushort(bits2 + 16);

			for (i = 0; i < 6; i++)
				K[7][i] = bits_to_ushort(bits2 + 32 + 16 * i);
			break;

		case 5 :
			for (i = 0; i < 4; i++)
				K[8][i] = bits_to_ushort(bits2 + 16 * i);
			break;
	}

}


void IDEA_encryption_key_schedule(ushort *key, long **K)
{
	ushort bits1[128], bits2[128], i;

	for (i = 0; i < 6; i++) K[0][i] = key[i];

	K[1][0] = key[6], K[1][1] = key[7];
	cyclic_left_shift(0, bits1, bits2, K);
	cyclic_left_shift(1, bits2, bits1, K);
	cyclic_left_shift(2, bits1, bits2, K);
	cyclic_left_shift(3, bits2, bits1, K);
	cyclic_left_shift(4, bits1, bits2, K);
	cyclic_left_shift(5, bits2, bits1, K);
}

void extended_euclidean(long a, long b, long *x, long *y, long *d)
{
	long q, r, x1, x2, y1, y2;

	if (b == 0) 
	{
		*d = a, *x = 1, *y = 0;
		return;
	}

	x2 = 1, x1 = 0, y2 = 0, y1 = 1;

	while (b > 0) 
	{
		q = a / b, r = a - q * b;
		*x = x2 - q * x1;
		*y = y2 - q * y1;
		a = b, b = r, x2 = x1, x1 = *x, y2 = y1, y1 = *y;
	}
	*d = a, *x = x2, *y = y2;
}

long inv(ushort ub)
{
	long d, a = 65537l, b = ub, x, y;

	if (ub == 0) 
		return 65536l;

	extended_euclidean(a, b, &x, &y, &d);

	if (y >= 0) 
		return (ushort) y;

	return (ushort) (y + 65537l);
}

void IDEA_decryption_key_schedule(long **K, long **L)
{
	ushort r, r8, r9;

	L[0][0] = inv((ushort) K[8][0]);
	L[0][1] = - K[8][1];
	L[0][2] = - K[8][2];
	L[0][3] = inv((ushort) K[8][3]);
	L[0][4] =  K[7][4];
	L[0][5] =  K[7][5];

	for (r = 1; r < 8; r++) 
	{
		r9 = (ushort) (8 - r);
		r8 = (ushort) (7 - r);
		L[r][0] = inv((ushort) K[r9][0]);
		L[r][1] = - K[r9][2];
		L[r][2] = - K[r9][1];
		L[r][3] = inv((ushort) K[r9][3]);
		L[r][4] = K[r8][4];
		L[r][5] = K[r8][5];
	}

	L[8][0] = inv((ushort) K[0][0]);
	L[8][1] = - K[0][1];
	L[8][2] = - K[0][2];
	L[8][3] = inv((ushort) K[0][3]);
	L[8][4] = L[8][6] = 0;
}

#endif /// IDEA_ENCIPHER_ALGORITHM




#if MD6_CHECKSUM_ENCIPHER

/* GCC */
/// #include <stdint.h>
inline uint64_t ticks() 
{
	/* read timestamp counter */
	uint32_t lo, hi;
	asm volatile (
		"xorl %%eax,%%eax \n        cpuid"
		::: "%rax", "%rbx", "%rcx", "%rdx");
	asm volatile ("rdtsc" : "=a" (lo), "=d" (hi));
	return (uint64_t)hi << 32 | lo;
} 


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

/* MD6 state.
** 
** md6_state is the main data structure for the MD6 hash function.
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



/* MD6 main interface routines
** These routines are defined in md6_mode.c
*/



/* MD6 return codes.
**
** The interface routines defined in md6_mode.c always return a
** "return code": an integer giving the status of the call.
** The codes
*/

/* SUCCESS:  */
#define MD6_SUCCESS 		0

/* ERROR CODES: */
#define MD6_FAIL 1           /* some other problem                     */
#define MD6_BADHASHLEN 2     /* hashbitlen<1 or >512 bits              */
#define MD6_NULLSTATE 3      /* null state passed to MD6               */
#define MD6_BADKEYLEN 4      /* key length is <0 or >512 bits          */
#define MD6_STATENOTINIT 5   /* state was never initialized            */
#define MD6_STACKUNDERFLOW 6 /* MD6 stack underflows (shouldn't happen)*/
#define MD6_STACKOVERFLOW 7  /* MD6 stack overflow (message too long)  */
#define MD6_NULLDATA 8       /* null data pointer                      */
#define MD6_NULL_N 9         /* compress: N is null                    */
#define MD6_NULL_B 10        /* standard compress: null B pointer      */
#define MD6_BAD_ELL 11       /* standard compress: ell not in {0,255}  */
#define MD6_BAD_p 12         /* standard compress: p<0 or p>b*w        */
#define MD6_NULL_K 13        /* standard compress: K is null           */
#define MD6_NULL_Q 14        /* standard compress: Q is null           */
#define MD6_NULL_C 15        /* standard compress: C is null           */
#define MD6_BAD_L 16         /* standard compress: L <0 or > 255       */ 
                             /* md6_init: L<0 or L>255                 */
#define MD6_BAD_r 17         /* compress: r<0 or r>255                 */
                             /* md6_init: r<0 or r>255                 */
#define MD6_OUT_OF_MEMORY 18 /* compress: storage allocation failed    */


/* compression hook, if defined, points to a function that is 
** called after each compression operation.                             
**
** compression hook must be set *after* md6_init or md6_full_init 
** is called.
*/

void (* compression_hook)(md6_word *C,
			  const md6_word *Q,
			  md6_word *K,
			  int ell,
			  int i,
			  int r,
			  int L,
			  int z,
			  int p,
			  int keylen,
			  int d,
			  md6_word *N
			  );


/// md6_nist.h
typedef unsigned char BitSequence;
typedef unsigned long long DataLength;
typedef enum { SUCCESS = 0, FAIL = 1, BAD_HASHLEN = 2 } HashReturn;
typedef md6_state hashState;
HashReturn Init( hashState *state, int hashbitlen);
HashReturn Update( hashState *state, const BitSequence *data, DataLength databitlen);
HashReturn Final( hashState *state, BitSequence *hashval );
HashReturn Hash( int hashbitlen, const BitSequence *data, DataLength databitlen, BitSequence *hashval );




/* Variables defining lengths of various values */
#define   wwww   md6_w  /* # bits in a word (64) */
#define   nnnn   md6_n  /* # words in compression input (89) */
#define   cccc   md6_c  /* # words in compression output (16) */
#define   bbbb   md6_b  /* # message words per compression input block (64) */
#define   vvvv   md6_v  /* # words in control word (1) */
#define   uuuu   md6_u  /* # words in unique nodeID (1) */
#define   kkkk   md6_k  /* # key words per compression input block (8) */
#define   qqqq   md6_q  /* # Q words per compression input block (15) */


/* "Tap positions" for feedback shift-register */

#if (nnnn==89)
#define  t0   17     /* index for linear feedback */
#define  t1   18     /* index for first input to first and */
#define  t2   21     /* index for second input to first and */
#define  t3   31     /* index for first input to second and */
#define  t4   67     /* index for second input to second and */
#define  t5   89     /* last tap */
#endif

/* Loop-unrolling setup.
**
** Define macros for loop-unrolling within compression function 
** These expand to:     loop_body(right-shift,left-shift,step)      
** These are given for word sizes 64, 32, 16, and 8, although   
** only w=64 is needed for the standard MD6 definition.         
**                                                              
** Also given for each word size are the constants S0 and Smask 
** needed in the recurrence for round constants.                
*/

#if (wwww==64)                        /* for standard word size */
#define RL00 loop_body(10,11, 0)
#define RL01 loop_body( 5,24, 1)
#define RL02 loop_body(13, 9, 2)
#define RL03 loop_body(10,16, 3)
#define RL04 loop_body(11,15, 4)
#define RL05 loop_body(12, 9, 5)
#define RL06 loop_body( 2,27, 6)
#define RL07 loop_body( 7,15, 7)
#define RL08 loop_body(14, 6, 8)
#define RL09 loop_body(15, 2, 9)
#define RL10 loop_body( 7,29,10)
#define RL11 loop_body(13, 8,11)
#define RL12 loop_body(11,15,12)
#define RL13 loop_body( 7, 5,13)
#define RL14 loop_body( 6,31,14)
#define RL15 loop_body(12, 9,15)

const md6_word S0 = (md6_word)0x0123456789abcdefULL;
const md6_word Smask = (md6_word)0x7311c2812425cfa0ULL;
#endif



/* Main compression loop. */
/*
** Perform the md6 "main compression loop" on the array A.
** This is where most of the computation occurs; it is the "heart"
** of the md6 compression algorithm.
** Input:
**     A                  input array of length t+n already set up
**                        with input in the first n words.
**     r                  number of rounds to run (178); each is c steps
** Modifies:
**     A                  A[n..r*c+n-1] filled in.
*/
void md6_main_compression_loop( md6_word* A , int r )
{ md6_word x, S;
  int i,j;

  /*
  ** main computation loop for md6 compression
  */
  S = S0;
  for (j = 0, i = nnnn; j<r*cccc; j+=cccc)
  {

/* ***************************************************************** */
#define loop_body(rs,ls,step)                                       \
      x = S;                                /* feedback constant     */ \
      x ^= A[i+step-t5];                    /* end-around feedback   */ \
      x ^= A[i+step-t0];                    /* linear feedback       */ \
      x ^= ( A[i+step-t1] & A[i+step-t2] ); /* first quadratic term  */ \
      x ^= ( A[i+step-t3] & A[i+step-t4] ); /* second quadratic term */ \
      x ^= (x >> rs);                       /* right-shift           */ \
      A[i+step] = x ^ (x << ls);            /* left-shift            */   
/* ***************************************************************** */

      /*
      ** Unroll loop c=16 times. (One "round" of computation.)
      ** Shift amounts are embedded in macros RLnn.
      */
      RL00 RL01 RL02 RL03 RL04 RL05 RL06 RL07
      RL08 RL09 RL10 RL11 RL12 RL13 RL14 RL15

      /* Advance round constant S to the next round constant. */
      S = (S << 1) ^ (S >> (wwww-1)) ^ (S & Smask);
      i += 16;
  }
}


/* ``Bare'' compression routine.
**
** Compresses n-word input to c-word output.
*/
/* Assumes n-word input array N has been fully set up.
** Input:
**	 N				 input array of n w-bit words (n=89)
**	 A				 working array of a = rc+n w-bit words
**					 A is OPTIONAL, may be given as NULL 
**					 (then md6_compress allocates and uses its own A).
**	 r				 number of rounds			 
** Modifies:
**	 C				 output array of c w-bit words (c=16)
** Returns one of the following:
**	 MD6_SUCCESS (0)	
**	 MD6_NULL_N 		
**	 MD6_NULL_C 		
**	 MD6_BAD_r			
**	 MD6_OUT_OF_MEMORY	
*/

int md6_compress( md6_word *C,
		  md6_word *N,
		  int r,
		  md6_word *A
		 )
{ md6_word* A_as_given = A;

  /* check that input is sensible */
  if ( N == NULL) return MD6_NULL_N;
  if ( C == NULL) return MD6_NULL_C;
  if ( r<0 || r > md6_max_r) return MD6_BAD_r;

  if ( A == NULL) A = calloc(r*cccc+nnnn,sizeof(md6_word));
  if ( A == NULL) return MD6_OUT_OF_MEMORY;

  memcpy( A, N, nnnn*sizeof(md6_word) );    /* copy N to front of A */

  md6_main_compression_loop( A, r );          /* do all the work */

  memcpy( C, A+(r-1)*cccc+nnnn, cccc*sizeof(md6_word) ); /* output into C */

  if ( A_as_given == NULL )           /* zero and free A if nec. */
    { memset(A,0,(r*cccc+nnnn)*sizeof(md6_word)); /* contains key info */
      free(A);           
    }

  return MD6_SUCCESS;
}


/* Control words.*/
/* Construct control word V for given inputs.
** Input:
**   r = number of rounds
**   L = mode parameter (maximum tree height)
**   z = 1 iff this is final compression operation
**   p = number of pad bits in a block to be compressed
**   keylen = number of bytes in key
**   d = desired hash output length
**   Does not check inputs for validity.
** Returns:
**   V = constructed control word
*/
md6_control_word md6_make_control_word(	int r, 
					int L, 
					int z, 
					int p, 
					int keylen, 
					int d 
					)
{ md6_control_word V;
  V = ( (((md6_control_word) 0) << 60) | /* reserved, width  4 bits */
	(((md6_control_word) r) << 48) |           /* width 12 bits */
	(((md6_control_word) L) << 40) |           /* width  8 bits */
	(((md6_control_word) z) << 36) |           /* width  4 bits */
	(((md6_control_word) p) << 20) |           /* width 16 bits */
	(((md6_control_word) keylen) << 12 ) |     /* width  8 bits */
        (((md6_control_word) d)) );                /* width 12 bits */
  return V;
}


/* Node ID's. */
/* Make "unique nodeID" U based on level ell and position i 
** within level; place it at specified destination.
** Inputs:
**    dest = address of where nodeID U should be placed
**    ell = integer level number, 1 <= ell <= ...
**    i = index within level, i = 0, 1, 2,...
** Returns
**    U = constructed nodeID
*/
md6_nodeID md6_make_nodeID( int ell,                     /* level number */
			      int i    /* index (0,1,2,...) within level */
			    )
{ md6_nodeID U;
  U = ( (((md6_nodeID) ell) << 56) | 
	((md6_nodeID) i) );
  return U;
}


/* Assembling components of compression input. */
/* Pack data before compression into n-word array N. */
void md6_pack( md6_word*N,
	       const md6_word* Q,
	       md6_word* K,
	       int ell, int i,
	       int r, int L, int z, int p, int keylen, int d,
	       md6_word* B )
{ int j;
  int ni;
  md6_nodeID U;
  md6_control_word V;    

  ni = 0;

  for (j=0;j<qqqq;j++) N[ni++] = Q[j];       /* Q: Q in words     0--14 */

  for (j=0;j<kkkk;j++) N[ni++] = K[j];       /* K: key in words  15--22 */

  U = md6_make_nodeID(ell,i);             /* U: unique node ID in 23 */
  /* The following also works for variants 
  ** in which u=0.
  */
  memcpy((unsigned char *)&N[ni],
	 &U,
	 min(uuuu*(wwww/8),sizeof(md6_nodeID)));
  ni += uuuu;

  V = md6_make_control_word(
			r,L,z,p,keylen,d);/* V: control word in   24 */
  /* The following also works for variants
  ** in which v=0.
  */
  memcpy((unsigned char *)&N[ni],
	 &V,
	 min(vvvv*(wwww/8),sizeof(md6_control_word)));
  ni += vvvv;

  memcpy(N+ni,B,bbbb*sizeof(md6_word));      /* B: data words    25--88 */
}
	       

/* Standard compress: assemble components and then compress*/
/* Perform md6 block compression using all the "standard" inputs.
** Input:
**     Q              q-word (q=15) approximation to (sqrt(6)-2)
**     K              k-word key input (k=8)
**     ell            level number
**     i              index within level
**     r              number of rounds in this compression operation
**     L              mode parameter (max tree height)
**     z              1 iff this is the very last compression
**     p              number of padding bits of input in payload B
**     keylen         number of bytes in key
**     d              desired output hash bit length
**     B              b-word (64-word) data input block (with zero padding)
** Modifies:
**     C              c-word output array (c=16)
** Returns one of the following:
**   MD6_SUCCESS (0)   MD6_BAD_p
**   MD6_NULL_B        MD6_BAD_HASHLEN
**   MD6_NULL_C        MD6_NULL_K
**   MD6_BAD_r         MD6_NULL_Q
**   MD6_BAD_ELL       MD6_OUT_OF_MEMORY
*/
int md6_standard_compress( md6_word* C,
			   const md6_word* Q,
			   md6_word* K,
			   int ell, int i,
			   int r, int L, int z, int p, int keylen, int d,
			   md6_word* B 
			   )
{ md6_word N[md6_n];
  md6_word A[5000];       /* MS VS can't handle variable size here */

  /* check that input values are sensible */
  if ( (C == NULL) ) return MD6_NULL_C;
  if ( (B == NULL) ) return MD6_NULL_B;
  if ( (r<0) | (r>md6_max_r) ) return MD6_BAD_r;
  if ( (L<0) | (L>255) ) return MD6_BAD_L;
  if ( (ell < 0) || (ell > 255) ) return MD6_BAD_ELL;
  if ( (p < 0) || (p > bbbb*wwww ) ) return MD6_BAD_p;
  if ( (d <= 0) || (d > cccc*wwww/2) ) return MD6_BADHASHLEN;
  if ( (K == NULL) ) return MD6_NULL_K;
  if ( (Q == NULL) ) return MD6_NULL_Q;

  /* pack components into N for compression */
  md6_pack(N,Q,K,ell,i,r,L,z,p,keylen,d,B);

  /* call compression hook if it is defined. */
  /* -- for testing and debugging.           */
  if (compression_hook != NULL)
    compression_hook(C,Q,K,ell,i,r,L,z,p,keylen,d,B);

  return md6_compress(C,N,r,A);
}
//// ------------------------------------------------





/* Default number of rounds                                    */
/* (as a function of digest size d and keylen                  */
int md6_default_r( int d , int keylen )
{ 
  int r;
  /* Default number of rounds is forty plus floor(d/4) */
  r = 40 + (d/4);
  /* unless keylen > 0, in which case it must be >= 80 as well */
  if (keylen>0)
    r = max(80,r);
  return r;
}


/* MD6 Constant Vector Q
** Q = initial 960 bits of fractional part of sqrt(6)
**
** Given here for w = 64, 32, 16, and 8, although only
** w = 64 is needed for the standard version of MD6.
*/

#if (wwww==64) /* for standard version */
/* 15 64-bit words */
static const md6_word Q[15] =
{
    0x7311c2812425cfa0ULL,
    0x6432286434aac8e7ULL, 
    0xb60450e9ef68b7c1ULL, 
    0xe8fb23908d9f06f1ULL, 
    0xdd2e76cba691e5bfULL, 
    0x0cd0d63b2c30bc41ULL, 
    0x1f8ccf6823058f8aULL, 
    0x54e5ed5b88e3775dULL, 
    0x4ad12aae0a6d6031ULL, 
    0x3e7f16bb88222e0dULL, 
    0x8af8671d3fb50c2cULL, 
    0x995ad1178bd25c31ULL, 
    0xc878c1dd04c4b633ULL, 
    0x3b72066c7a1552acULL, 
    0x0d6f3522631effcbULL, 
};
#endif



/* Endianness. */

/* routines for dealing with byte ordering */

int md6_byte_order = 0;    
/* md6_byte_order describes the endianness of the 
** underlying machine:
** 0 = unknown
** 1 = little-endian
** 2 = big-endian
*/

/* Macros to detect machine byte order; these
** presume that md6_byte_order has been setup by
** md6_detect_byte_order()
*/
#define MD6_LITTLE_ENDIAN (md6_byte_order == 1)
#define MD6_BIG_ENDIAN    (md6_byte_order == 2)
 

/* determine if underlying machine is little-endian or big-endian
** set global variable md6_byte_order to reflect result
** Written to work for any w.
*/
void md6_detect_byte_order( void )
{ md6_word x = 1 | (((md6_word)2)<<(wwww-8));
  unsigned char *cp = (unsigned char *)&x;
  if ( *cp == 1 )        md6_byte_order = 1;      /* little-endian */
  else if ( *cp == 2 )   md6_byte_order = 2;      /* big-endian    */
  else                   md6_byte_order = 0;      /* unknown       */
}



/* return byte-reversal of md6_word x.
** Written to work for any w, w=8,16,32,64.
*/
md6_word md6_byte_reverse( md6_word x )
{ 
#define mask8  ((md6_word)0x00ff00ff00ff00ffULL)
#define mask16 ((md6_word)0x0000ffff0000ffffULL)

#if (wwww==64)
  x = (x << 32) | (x >> 32);
#endif

#if (wwww >= 32)
  x = ((x & mask16) << 16) | ((x & ~mask16) >> 16);
#endif

#if (wwww >= 16)
  x = ((x & mask8) << 8) | ((x & ~mask8) >> 8);
#endif

  return x;
}



/* Byte-reverse words x[0...count-1] if machine is little_endian */
void md6_reverse_little_endian( md6_word *x, int count )
{
  int i;
  if (MD6_LITTLE_ENDIAN)
    for (i=0;i<count;i++)
      x[i] = md6_byte_reverse(x[i]);
}


/* Appending one bit string onto another.
*/
/* Append bit string src to the end of bit string dest
** Input:
**	   dest 		a bit string of destlen bits, starting in dest[0]
**					if destlen is not a multiple of 8, the high-order
**					bits are used first
**	   src			a bit string of srclen bits, starting in src[0]
**					if srclen is not a multiple of 8, the high-order
**					bits are used first
** Modifies:
**	   dest 		when append_bits returns, dest will be modified to
**					be a bit-string of length (destlen+srclen).
**					zeros will fill any unused bit positions in the 
**					last byte.
*/
void append_bits( unsigned char *dest, unsigned int destlen, unsigned char *src,  unsigned int srclen )
{ int i, di, accumlen;
  uint16_t accum;
  int srcbytes;

  if (srclen == 0) return;

  /* Initialize accum, accumlen, and di */
  accum = 0;    /* accumulates bits waiting to be moved, right-justified */
  accumlen = 0; /* number of bits in accumulator */
  if (destlen%8 != 0)
    { accumlen = destlen%8;
      accum = dest[destlen/8];        /* grab partial byte from dest     */
      accum = accum >> (8-accumlen);  /* right-justify it in accumulator */
    }
  di = destlen/8;        /* index of where next byte will go within dest */
  
  /* Now process each byte of src */
  srcbytes = (srclen+7)/8;   /* number of bytes (full or partial) in src */
  for (i=0;i<srcbytes;i++)
    { /* shift good bits from src[i] into accum */
      if (i != srcbytes-1) /* not last byte */
	{ accum = (accum << 8) ^ src[i];  
	  accumlen += 8;
	}
      else /* last byte */
	{ int newbits = ((srclen%8 == 0) ? 8 : (srclen%8));
	  accum = (accum << newbits) | (src[i] >> (8-newbits));
	  accumlen += newbits;
	}
      /* do as many high-order bits of accum as you can (or need to) */
      while ( ( (i != srcbytes-1) & (accumlen >= 8) ) ||
	      ( (i == srcbytes-1) & (accumlen > 0) ) )
	{ int numbits = min(8,accumlen);
	  unsigned char bits;
	  bits = accum >> (accumlen - numbits);    /* right justified */
	  bits = bits << (8-numbits);              /* left justified  */
	  bits &= (0xff00 >> numbits);             /* mask            */
	  dest[di++] = bits;                       /* save            */
	  accumlen -= numbits; 
	}
    }
}


/* State initialization. (md6_full_init, with all parameters specified)
*/
/* Initialize md6_state
** Input:
**	   st		  md6_state to be initialized
**	   d		  desired hash bit length 1 <= d <= w*(c/2)    (<=512 bits)
**	   key		  key (aka salt) for this hash computation	   (byte array)
**				  defaults to all-zero key if key==NULL or keylen==0
**	   keylen	  length of key in bytes; 0 <= keylen <= (k*8) (<=64 bytes)
**	   L		  md6 mode parameter; 0 <= L <= 255
**				  md6.h defines md6_default_L for when you want default
**	   r		  number of rounds; 0 <= r <= 255
** Output:
**	   updates components of state
**	   returns one of the following:
**		 MD6_SUCCESS
**		 MD6_NULLSTATE
**		 MD6_BADKEYLEN
**		 MD6_BADHASHLEN
*/
int md6_full_init( md6_state *st,       /* uninitialized state to use */
		   int d,                          /* hash bit length */
		   unsigned char *key,        /* key; OK to give NULL */
		   int keylen,     /* keylength (bytes); OK to give 0 */
		   int L,           /* mode; OK to give md6_default_L */
		   int r                          /* number of rounds */
		   )
{ /* check that md6_full_init input parameters make some sense */
  if (st == NULL) return MD6_NULLSTATE;
  if ( (key != NULL) && ((keylen < 0) || (keylen > kkkk*(wwww/8))) )
    return MD6_BADKEYLEN;
  if ( d < 1 || d > 512 || d > wwww*cccc/2 ) return MD6_BADHASHLEN;

  md6_detect_byte_order();
  memset(st,0,sizeof(md6_state));  /* clear state to zero */
  st->d = d;                       /* save hashbitlen */
  if (key != NULL && keylen > 0)   /* if no key given, use memset zeros*/
    { memcpy(st->K,key,keylen);    /* else save key (with zeros added) */
      st->keylen = keylen;
      /* handle endian-ness */       /* first byte went into high end */
      md6_reverse_little_endian(st->K,kkkk);
    }
  else
    st->keylen = 0;
  if ( (L<0) | (L>255) ) return MD6_BAD_L;
  st->L = L;
  if ( (r<0) | (r>255) ) return MD6_BAD_r;
  st->r = r;
  st->initialized = 1;  
  st->top = 1;
  /* if SEQ mode for level 1; use IV=0  */
  /* zero bits already there by memset; */
  /* we just need to set st->bits[1]    */
  if (L==0) st->bits[1] = cccc*wwww;     
  compression_hook = NULL;     /* just to be sure default is "not set" */
  return MD6_SUCCESS;
}


/* State initialization. (md6_init, which defaults most parameters.)
*/
/* Same as md6_full_init, but with default key, L, and r */
int md6_init( md6_state *st, int d)
{ return md6_full_init(st,
		       d,
		       NULL,
		       0,
		       md6_default_L,
		       md6_default_r(d,0)
		       );
}


/* Data structure notes.
*/

/*
Here are some notes on the data structures used (inside state).

* The variable B[] is a stack of length-b (b-64) word records,
  each corresponding to a node in the tree.  B[ell] corresponds
  to a node at level ell.  Specifically, it represents the record which,
  when compressed, will yield the value at that level. (It only
  contains the data payload, not the auxiliary information.)
  Note that B[i] is used to store the *inputs* to the computation at
  level i, not the output for the node at that level.  
  Thus, for example, the message input is stored in B[1], not B[0].

* Level 0 is not used.  The message bytes are placed into B[1].

* top is the largest ell for which B[ell] has received data,
  or is equal to 1 in case no data has been received yet at all.

* top is never greater than L+1.  If B[L+1] is
  compressed, the result is put back into B[L+1]  (this is SEQ).

* bits[ell] says how many bits have been placed into
  B[ell].  An invariant maintained is that of the bits in B[ell], 
  only the first bits[ell] may be nonzero; the following bits must be zero.

* The B nodes may have somewhat different formats, depending on the level:
  -- Level 1 node contains a variable-length bit-string, and so
     0 <= bits[1] <= b*w     is all we can say.
  -- Levels 2...top always receive data in c-word chunks (from
     children), so for them bits[ell] is between 0 and b*w,
     inclusive, but is also a multiple of cw.  We can think of these
     nodes as have (b/c) (i.e. 4) "slots" for chunks.
  -- Level L+1 is special, in that the first c words of B are dedicated
     to the "chaining variable" (or IV, for the first node on the level).

* When the hashing is over, B[top] will contain the 
  final hash value, in the first or second (if top = L+1) slot.

*/

/* Compress one block -- compress data at a node (md6_compress_block).
*/
/* compress block at level ell, and put c-word result into C.
** Input:
**	   st		  current md6 computation state
**	   ell		  0 <= ell < max_stack_height-1
**	   z		  z = 1 if this is very last compression; else 0
** Output:
**	   C		  c-word array to put result in
** Modifies:
**	   st->bits[ell]  (zeroed)
**	   st->i_for_level[ell] (incremented)  
**	   st->B[ell] (zeroed)
**	   st->compression_calls (incremented)
** Returns one of the following:
**	   MD6_SUCCESS
**	   MD6_NULLSTATE
**	   MD6_STATENOTINIT
**	   MD6_STACKUNDERFLOW
**	   MD6_STACKOVERFLOW
*/

int md6_compress_block( md6_word *C, md6_state *st, int ell, int z )
{ int p, err;

  /* check that input values are sensible */
  if ( st == NULL) return MD6_NULLSTATE;
  if ( st->initialized == 0 ) return MD6_STATENOTINIT;
  if ( ell < 0 ) return MD6_STACKUNDERFLOW;
  if ( ell >= md6_max_stack_height-1 ) return MD6_STACKOVERFLOW;

  st->compression_calls++;

  if (ell==1) /* leaf; hashing data; reverse bytes if nec. */
    { if (ell<(st->L + 1)) /* PAR (tree) node */
	md6_reverse_little_endian(&(st->B[ell][0]),bbbb);
      else /* SEQ (sequential) node; don't reverse chaining vars */
	md6_reverse_little_endian(&(st->B[ell][cccc]),bbbb-cccc);
    }

  p = bbbb*wwww - st->bits[ell];          /* number of pad bits */

  err = 
    md6_standard_compress( 
      C,                                      /* C    */
      Q,                                      /* Q    */
      st->K,                                  /* K    */
      ell, st->i_for_level[ell],              /* -> U */
      st->r, st->L, z, p, st->keylen, st->d,  /* -> V */
      st->B[ell]                              /* B    */
			   );                         
  if (err) return err; 

  st->bits[ell] = 0; /* clear bits used count this level */
  st->i_for_level[ell]++;

  memset(&(st->B[ell][0]),0,bbbb*sizeof(md6_word));     /* clear B[ell] */
  return MD6_SUCCESS;
}


/* Process (compress) a node and its compressible ancestors.
*/
int md6_process( md6_state *st, int ell, int final )
/*
** Do processing of level ell (and higher, if necessary) blocks.
** 
** Input:
**     st         md6 state that has been accumulating message bits
**                and/or intermediate results
**     ell        level number of block to process
**     final      true if this routine called from md6_final 
**                     (no more input will come)
**                false if more input will be coming
**                (This is not same notion as "final bit" (i.e. z)
**                 indicating the last compression operation.)
** Output (by side effect on state):
**     Sets st->hashval to final chaining value on final compression.
** Returns one of the following:
**     MD6_SUCCESS
**     MD6_NULLSTATE
**     MD6_STATENOTINIT
*/
{ int err, z, next_level;
  md6_word C[cccc];

  /* check that input values are sensible */
  if ( st == NULL) return MD6_NULLSTATE;
  if ( st->initialized == 0 ) return MD6_STATENOTINIT;

  if (!final) /* not final -- more input will be coming */
    { /* if not final and block on this level not full, nothing to do */
      if ( st->bits[ell] < bbbb*wwww ) 
	return MD6_SUCCESS;
      /* else fall through to compress this full block, 
      **       since more input will be coming 
      */
    }
  else /* final -- no more input will be coming */
    { if ( ell == st->top )
	{ if (ell == (st->L + 1)) /* SEQ node */
	    { if ( st->bits[ell]==cccc*wwww && st->i_for_level[ell]>0 )
		return MD6_SUCCESS;
	      /* else (bits>cw or i==0, so fall thru to compress */
	    }
           else /* st->top == ell <= st->L so we are at top tree node */
	     { if ( ell>1 && st->bits[ell]==cccc*wwww)
		 return MD6_SUCCESS;
	       /* else (ell==1 or bits>cw, so fall thru to compress */
	     }
	}
      /* else (here ell < st->top so fall through to compress */
    }

  /* compress block at this level; result goes into C */
  /* first set z to 1 iff this is the very last compression */
  z = 0; if (final && (ell == st->top)) z = 1; 
  if ((err = md6_compress_block(C,st,ell,z))) 
      return err;
  if (z==1) /* save final chaining value in st->hashval */
    { memcpy( st->hashval, C, md6_c*(wwww/8) );
      return MD6_SUCCESS;
    }
  
  /* where should result go? To "next level" */
  next_level = min(ell+1,st->L+1);
  /* Start sequential mode with IV=0 at that level if necessary 
  ** (All that is needed is to set bits[next_level] to c*w, 
  ** since the bits themselves are already zeroed, either
  ** initially, or at the end of md6_compress_block.)
  */
  if (next_level == st->L + 1 
      && st->i_for_level[next_level]==0
      && st->bits[next_level]==0 )
    st->bits[next_level] = cccc*wwww;   
  /* now copy C onto next level */
  memcpy((char *)st->B[next_level] + st->bits[next_level]/8,
	 C,
	 cccc*(wwww/8));
  st->bits[next_level] += cccc*wwww;   
  if (next_level > st->top) st->top = next_level;

  return md6_process(st,next_level,final);
}


/* Update -- incorporate data string into hash computation.
*/
/* Process input byte string data, updating state to reflect result
** Input:
**	   st				already initialized state to be updated
**	   data 			byte string of length databitlen bits 
**						to be processed (aka "M")
**	   databitlen		number of bits in string data (aka "m")
** Modifies:
**	   st				updated to reflect input of data
*/
int md6_update( md6_state *st, unsigned char *data, uint64_t databitlen )
{ 
  unsigned int j, portion_size;
  int err;

  /* check that input values are sensible */
  if ( st == NULL ) return MD6_NULLSTATE;
  if ( st->initialized == 0 ) return MD6_STATENOTINIT;
  if ( data == NULL ) return MD6_NULLDATA;
  
  j = 0; /* j = number of bits processed so far with this update */
  while (j<databitlen)
    { /* handle input string in portions (portion_size in bits)
      ** portion_size may be zero (level 1 data block might be full, 
      ** having size b*w bits) */
      portion_size = min(databitlen-j,
			 (unsigned int)(bbbb*wwww-(st->bits[1]))); 

      if ((portion_size % 8 == 0) && 
	  (st->bits[1] % 8 == 0) &&
	  (j % 8 == 0))
	{ /* use mempy to handle easy, but most common, case */
	  memcpy((char *)st->B[1] + st->bits[1]/8,
		 &(data[j/8]),                                 
		 portion_size/8);
	}
      else /* handle messy case where shifting is needed */
	{ append_bits((unsigned char *)st->B[1], /* dest */
		      st->bits[1],   /* dest current bit size */
		      &(data[j/8]),  /* src */
		      portion_size); /* src size in bits  */
	}
      j += portion_size;
      st->bits[1] += portion_size;
      st->bits_processed += portion_size;

      /* compress level-1 block if it is now full 
	 but we're not done yet */
      if (st->bits[1] == bbbb*wwww && j<databitlen)
	{ if ((err=md6_process(st,
			       1,    /* ell */
			       0     /* final */
			       ))) 
	    return err; 
	}
    } /* end of loop body handling input portion */
  return MD6_SUCCESS;
}


/* Convert hash value to hexadecimal, and store it in state.
*/
/*
** Convert hashval in st->hashval into hexadecimal, and
** save result in st->hexhashval
** This will be a zero-terminated string of length ceil(d/4).
** Assumes that hashval has already been "trimmed" to correct 
** length.
** 
** Returns one of the following:
**	  MD6_SUCCESS
**	  MD6_NULLSTATE 					 (if input state pointer was NULL)
*/
int md6_compute_hex_hashval( md6_state *st )
{ int i;
  static unsigned char hex_digits[] = "0123456789abcdef";

  /* check that input is sensible */
  if ( st == NULL ) return MD6_NULLSTATE;
  
  for (i=0;i<((st->d+7)/8);i++)
    { st->hexhashval[2*i]   
	= hex_digits[ ((st->hashval[i])>>4) & 0xf ];
      st->hexhashval[2*i+1] 
	= hex_digits[ (st->hashval[i]) & 0xf ];
    }
  
  /* insert zero string termination byte at position ceil(d/4) */
  st->hexhashval[(st->d+3)/4] = 0;
  return MD6_SUCCESS;
}


/* Extract last d bits of chaining variable as hash value.*/

void trim_hashval(md6_state *st)
{ /* trim hashval to desired length d bits by taking only last d bits */
  /* note that high-order bit of a byte is considered its *first* bit */
  int full_or_partial_bytes = (st->d+7)/8;
  int bits = st->d % 8;                 /* bits in partial byte */
  int i;

  /* move relevant bytes to the front */
  for ( i=0; i<full_or_partial_bytes; i++ )
    st->hashval[i] = st->hashval[cccc*(wwww/8)-full_or_partial_bytes+i];

  /* zero out following bytes */
  for ( i=full_or_partial_bytes; i<cccc*(wwww/8); i++ )
    st->hashval[i] = 0;

  /* shift result left by (8-bits) bit positions, per byte, if needed */
  if (bits>0)
    { for ( i=0; i<full_or_partial_bytes; i++ )
	{ st->hashval[i] = (st->hashval[i] << (8-bits));
	  if ( (i+1) < cccc*(wwww/8) )
	    st->hashval[i] |= (st->hashval[i+1] >> bits);
	}
    }
}

/* Final -- no more data; finish up and produce hash value.*/
/* Do final processing to produce md6 hash value
** Input:
**     st              md6 state that has been accumulating message bits
**                     and/or intermediate results
** Output (by side effect on state):
**     hashval         If this is non-NULL, final hash value copied here.
**                     (NULL means don't copy.)  In any case, the hash
**                     value remains in st->hashval.
**     st->hashval     this is a 64-byte array; the first st->d
**                     bits of which will be the desired hash value
**                     (with high-order bits of a byte used first), and
**                     remaining bits set to zero (same as hashval)
**     st->hexhashval  this is a 129-byte array which contains the
**                     zero-terminated hexadecimal version of the hash
** Returns one of the following:
**     MD6_SUCCESS
**     MD6_NULLSTATE
**     MD6_STATENOTINIT
*/
int md6_final( md6_state *st , unsigned char *hashval)
{ int ell, err;

  /* check that input values are sensible */
  if ( st == NULL) return MD6_NULLSTATE;
  if ( st->initialized == 0 ) return MD6_STATENOTINIT;

  /* md6_final was previously called */
  if ( st->finalized == 1 ) return MD6_SUCCESS;

  /* force any processing that needs doing */
  if (st->top == 1) ell = 1;
  else for (ell=1; ell<=st->top; ell++)
	 if (st->bits[ell]>0) break;
  /* process starting at level ell, up to root */
  err = md6_process(st,ell,1);
  if (err) return err;

  /* md6_process has saved final chaining value in st->hashval */

  md6_reverse_little_endian( (md6_word*)st->hashval, cccc );

  /* 4/15/09: Following two lines were previously out of order, which 
  **          caused errors depending on whether caller took hash output 
  **          from  st->hashval (which was correct) or from 
  **                hashval parameter (which was incorrect, since it 
  **                                   missed getting "trimmed".)
  */
  trim_hashval( st );
  if (hashval != NULL) memcpy( hashval, st->hashval, (st->d+7)/8 );

  md6_compute_hex_hashval( st );

  st->finalized = 1;
  return MD6_SUCCESS;
}

/* Routines for hashing message given "all at once".*/
int md6_full_hash( int d,                    /* hash bit length */
		   unsigned char *data,/* complete data to hash */
		   uint64_t databitlen,   /* its length in bits */
		   unsigned char *key,       /* OK to give NULL */
		   int keylen,       /* (in bytes) OK to give 0 */
		   int L,     /* mode; OK to give md6_default_L */
		   int r,                   /* number of rounds */
		   unsigned char *hashval             /* output */
		   )
{ md6_state st;
  int err;

  err = md6_full_init(&st,d,key,keylen,L,r);
  if (err) return err;
  err = md6_update(&st,data,databitlen);
  if (err) return err;
  md6_final(&st,hashval);
  if (err) return err;
  return MD6_SUCCESS;
}


/// int d: hash bit length
/// byte *data : complete data to hash
/// uint64_t databitlen : its length in bits
/// unsigned char *hashval : output
int md6_hash( int d, unsigned char *data, uint64_t databitlen, unsigned char *hashval )
{ 
  int err;

  err = md6_full_hash(d,data,databitlen,
		      NULL,0,md6_default_L,md6_default_r(d,0),hashval);
  if (err) return err;
  return MD6_SUCCESS;
}

//// end of md6_mode.c -------------------------------



HashReturn Init( hashState *state, int hashbitlen)
{ int err;
  if ((err = md6_init( (md6_state *) state, 
		       hashbitlen
		       )))
    return err;
  state->hashbitlen = hashbitlen;
  return SUCCESS;
}

HashReturn Update( hashState *state, const BitSequence *data, DataLength databitlen )
{ 
	return md6_update( (md6_state *) state, 
		     (unsigned char *)data, 
		     (uint64_t) databitlen );
}

HashReturn Final( hashState *state, BitSequence *hashval )
{ 
	return md6_final( (md6_state *) state, (unsigned char *) hashval );
}

HashReturn Hash( int hashbitlen, const BitSequence *data, DataLength databitlen, BitSequence *hashval )
{ int err;
  md6_state state;
  if ((err = Init( &state, hashbitlen ))) 
    return err;
  if ((err = Update( &state, data, databitlen ))) 
    return err;
  return Final( &state, hashval );
}


/* ------------ MD6 parameters --------------------- */
int md6_dgtLen = 256;         /* digest length */
int md6_modPar = 64;          /* mode parameter */
int md6_roundN;               /* number of rounds */
int md6_use_default_r = 1;    /* 1 if r should be set to the default, 0 if r is explicitly provided */
unsigned char md6_K[100];     /* key */
int md6_keylen = 0;           /* key length in bytes (at most 64) */
md6_state md6_st;             /* md6 computation state */

char md6_msg[5000];           /* message to be hashed (if given with -M) */
int msglenbytes;              /* message length in bytes */


/* return integer starting at s (input presumed to end with '\n')
** (It may be expressed in exponential format e.g. 1e9.)
*/
uint64_t get_int(char *s)
{ 
	long double g;
	sscanf(s,"%Lg",&g);
	return (uint64_t)g;
}

/* routines to escape/unescape filenames, in case they
** contain backslash or \n 's.
*/
/* input t, output s -- recode t so it all newlines and 
** backslashes are escaped as \n and \\ respectively.
** Also, a leading '-' is escaped to \-.
*/
void encode(char *s, char *t)
{ 
	if (*t && *t=='-')
	{ *s++ = '\\'; *s++ = '-'; t++; }
	while (*t)
    { if (*t=='\\')      { *s++ = '\\'; *s++ = '\\'; }     
      else if (*t=='\n') { *s++ = '\\'; *s++ = 'n';  }
      else               *s++ = *t;
      t++;
    }
	*s = 0;
	return;
}


/* inverse of encode -- s is unescaped version of t. */
void decode(char *s, char *t)
{ while (*t)
    { if (*t == '\\')
	{ if (*(t+1)=='\\')     { *s++ = '\\'; t+=1; }
	  else if (*(t+1)=='n') { *s++ = '\n'; t+=1; } 
	  else if (*(t+1)=='-') { *s++ = '-'; t+=1; }
	  else if (*(t+1)==0)   { *s++ = '\\'; }
	  else                  { *s++ = *t; }
	}
      else *s++ = *t;
      t++;
    }
  *s = 0;
  return;
}


/* timing variables and routines */
double start_time;
double end_time;
uint64_t start_ticks;
uint64_t end_ticks;

void start_timer()
{
  start_time = ((double)clock())/CLOCKS_PER_SEC;
  start_ticks = ticks();
}

void end_timer()
{
  end_time = ((double)clock())/CLOCKS_PER_SEC;
  end_ticks = ticks();
}

int print_times = 0;

void print_time()
{ 
#ifdef DEBUG_PRINT

  double elapsed_time = end_time - start_time;
  uint64_t elapsed_ticks = end_ticks - start_ticks;
  uint64_t bytes = md6_st.bits_processed/8;
  int bits = md6_st.bits_processed % 8;
  if (!print_times) return;
  printf("-- Length = ");
  if (md6_st.bits_processed==0) printf("0");
  if (bytes>0) printf("%g byte",(double)bytes);
  if (bytes>1) printf("s");
  if (bytes>0 && bits>0) printf(" + ");
  if (bits>0) printf("%d bit",bits);
  if (bits>1) printf("s");
  printf("\n");
  printf("-- Compression calls made = %g\n",(double)md6_st.compression_calls);
  if (elapsed_time == 0.0)
    printf("-- Elapsed time too short to measure...\n");
  else
    { printf("-- Elapsed time = %.3f seconds.\n", elapsed_time);
      printf("-- Megabytes per second = %g.\n",
	     (bytes/elapsed_time)/1000000.0);
      printf("-- Microseconds per compression function = %g.\n",
	     (elapsed_time*1.0e6 / md6_st.compression_calls ));
    }
  printf("-- Total clock ticks = %lld\n",
	 (long long int)elapsed_ticks);
  if (bytes>0)
    printf("-- Clock ticks / byte = %lld\n",
	   (long long int)(elapsed_ticks/bytes));
  printf("-- Clock ticks / compression function call = %lld\n",
	 (long long int)(elapsed_ticks/md6_st.compression_calls));

#endif
}



/* testing and debugging */

/* Global variables used by compression_hook_1 */
/// FILE *outFile = NULL;
int  print_input_output = 0;
//int  print_intermediate = 0;

void compression_hook_1(md6_word *C,
			const md6_word *Q,
			md6_word *K,
			int ell,
			int ii,
			int r,
			int L,
			int z,
			int p,
			int keylen,
			int d,
			md6_word *B
)
{ 
	int i;
	md6_word A[5000];
	time_t now;

	md6_pack(A,Q,K,ell,ii,r,L,z,p,keylen,d,B);

	md6_main_compression_loop( A, r);





#ifdef DEBUG_PRINT

	if (ell==1 && ii==0)
	{ 
		time(&now);
		printf("-- d = %6d (digest length in bits)\n",d);
		printf("-- L = %6d (number of parallel passes)\n",L);
		printf("-- r = %6d (number of rounds)\n",r);

		/* print key out as chars, since for md6sum it is essentially
		** impossible to enter non-char keys...
		*/
		printf("-- K = '");
		for (i=0;i<keylen;i++) 
			printf("%c",(int)(K[i/(wwww/8)]>>8*(7-(i%(wwww/8))))&0xff);
		printf("' (key)\n");
		printf("-- k = %6d (key length in bytes)\n",keylen);
		printf("\n");
	}

  printf("MD6 compression function computation ");
  printf("(level %d, index %d):\n",ell,ii);
  printf("Input (%d words):\n",nnnn);


  for (i=0;i<r*cccc+nnnn;i++)
  {
	if ((i<qqqq))
	{ 
	  printf("A[%4d] = " PR_MD6_WORD,i,A[i]);
	  printf(" Q[%d]\n",i);
	}
	else if ((i>=qqqq)&&(i<qqqq+kkkk))
	{ 
	  printf("A[%4d] = " PR_MD6_WORD,i,A[i]);
	  printf(" key K[%d]\n",i-qqqq);
	}
	else if ((uuuu>0)&&(i==qqqq+kkkk+uuuu-1))
	{ 
	  printf("A[%4d] = " PR_MD6_WORD,i,A[i]);
	  printf(" nodeID U = (ell,i) = (%d,%d)\n",ell,ii);
	}
	else if ((vvvv>0)&&(i==qqqq+kkkk+uuuu+vvvv-1))
	{ 
	  printf("A[%4d] = " PR_MD6_WORD,i,A[i]);
	  printf(" control word V = "
				  "(r,L,z,p,keylen,d) = "
		  "(%d,%d,%d,%d,%d,%d)\n",r,L,z,p,keylen,d);
	}
	else if ((i>=qqqq+kkkk+uuuu+vvvv)&&(i<nnnn))
	{ 
	  printf("A[%4d] = " PR_MD6_WORD,i,A[i]);
	  printf(" data B[%2d] ",i-qqqq-kkkk-uuuu-vvvv);
	  if (ell < L+1) /* PAR node */
		{ if (ell == 1)
		{ if ( (i+(p/wwww))<nnnn )
			printf("input message word %4d",
				ii*bbbb+(i-(qqqq+kkkk+uuuu+vvvv)));
		  else
			printf("padding");
		}
		  else
		if ( (i+(p/wwww))< nnnn )
		  printf(
			  "chaining from (%d,%d)",
			  ell-1,
			  4*ii+(i-(qqqq+kkkk+uuuu+vvvv))/cccc);
		else 
		  printf("padding");
		}
	  else /* SEQ node: ell == L+1 */
		{ if (i-(qqqq+kkkk+uuuu+vvvv)<cccc) /* initial portion: IV or chaining */
		{ if (ii == 0)
			printf("IV");
		  else
			printf("chaining from (%d,%d)",ell,ii-1);
		}
		  else /* data, chaining from below, or padding */
		{ if (i+(p/wwww)>=nnnn)
			printf("padding");
		  else if (ell == 1)
			printf("input message word %4d",
				ii*(bbbb-cccc)+(i-(qqqq+kkkk+uuuu+vvvv+cccc)));
		  else 
			printf(
				"chaining from (%d,%d)",
				ell-1,
				3*ii+(i-(qqqq+kkkk+uuuu+vvvv+cccc))/cccc);
		}
		}
	  printf("\n");
	}
	else if ((i>=r*cccc+nnnn-cccc))
	{ if ((i==r*cccc+nnnn-cccc))
		printf("Output (%d words of chaining values):\n",cccc);
	  printf("A[%4d] = " PR_MD6_WORD,i,A[i]);
	  printf(" output chaining value C[%d]\n",i-(r*cccc+nnnn-cccc));
	}
	  else 
	{ if (i==nnnn)
		{ if (print_intermediate)
		printf("Intermediate values:\n");
		  else
		printf(
			"Intermediate values A[%d..%d] omitted... "
			"\n",nnnn,r*cccc+nnnn-cccc-1);
		}
	  if (print_intermediate)
		printf("A[%4d] = " PR_MD6_WORD "\n",i,A[i]);
	}
  }
  printf("\n");

#endif

}


/* interface to hash routines
*/

void hash_init()
{ int err;
  start_timer();
  if ((err=md6_full_init(&md6_st, md6_dgtLen, md6_K,md6_keylen, md6_modPar, md6_roundN)))
    { printf("Bad MD6 parameters; can't initialize md6. "
	     "errcode = %d\n",err);
      return;
    }
  if (print_input_output)
    compression_hook = compression_hook_1;
}

void hash_update(char* data, 
		 uint64_t databitlen)
{ int err;
  if ((err=md6_update(&md6_st, 
		      (unsigned char *)data, 
		      databitlen)))
    { printf("MD6 update error. error code: %d\n",err);
      return;
    }
}

void hash_final()
{ int err;
  if ((err=md6_final(&md6_st,NULL)))
    { printf("MD6 finalization error. error code: %d\n",err);
      return;
    }
  end_timer();
}

void hash_filep(FILE *inFile)
{ uint64_t bytes;
  char data[1024];
  if (inFile==NULL)
    { printf("hash_filep has NULL input file pointer.\n");
      return;
    }
  hash_init();
  while ((bytes = fread (data, 1, 1024, inFile)) != 0)
    hash_update(data,bytes*8);
  hash_final();
}

void hash_stdin()
{ hash_filep(stdin);
}

void hash_file( char *filename )
{ FILE *inFile = fopen (filename, "rb");
  if ( inFile == NULL ) 
    { printf("%s can't be opened.\n", filename);
      return;
    }
  hash_filep(inFile);
  fclose(inFile);
}

void hash_b(uint64_t bitlen)
/* Hash dummy input file of length bitlen bits.
** File (hex) repeats with period 7:
**   11 22 33 44 55 66 77 11 22 33 44 55 66 77 11 22 33 ...
*/
{ int i;
  char data[700];  /* nice if length is multiple of 7 for periodicity */
  for (i=0;i<700;i++)
    data[i] =  0x11 + (char)((i % 7)*(0x11));
  hash_init();
  while (bitlen>0)
    { uint64_t part_len = min(700*8,bitlen);
      hash_update(data,part_len);
      bitlen = bitlen - part_len;
    }
  hash_final();
}

void print_hash(char *filename)
{ 
#ifdef DEBUG_PRINT
  print_tod();
  if (print_input_output == 0)
	printf("%s %s\n", md6_st.hexhashval,filename);
  else
	printf("Final hash value = %s\n", md6_st.hexhashval);
  print_time(); /* running time */
#endif
}

#endif /// MD6_CHECKSUM_ENCIPHER





#if MD2_CHECKSUM_ENCIPHER

#define MD2_DIGEST_LENGTH 16

typedef struct md2 {
    size_t len;
    unsigned char data[16]; /* stored unalligned data between Update's */
    unsigned char checksum[16];
    unsigned char state[16]; /* lower 16 bytes of X */
}MD2_CTX;

/// typedef struct md2 MD2_CTX;



/*
 * Copyright (c) 2006 Kungliga Tekniska Hogskolan
 * (Royal Institute of Technology, Stockholm, Sweden).
 * All rights reserved.
 *
 */

const unsigned char subst[256] = {
  41, 46, 67, 201, 162, 216, 124, 1, 61, 54, 84, 161, 236, 240, 6,
  19, 98, 167, 5, 243, 192, 199, 115, 140, 152, 147, 43, 217, 188,
  76, 130, 202, 30, 155, 87, 60, 253, 212, 224, 22, 103, 66, 111, 24,
  138, 23, 229, 18, 190, 78, 196, 214, 218, 158, 222, 73, 160, 251,
  245, 142, 187, 47, 238, 122, 169, 104, 121, 145, 21, 178, 7, 63,
  148, 194, 16, 137, 11, 34, 95, 33, 128, 127, 93, 154, 90, 144, 50,
  39, 53, 62, 204, 231, 191, 247, 151, 3, 255, 25, 48, 179, 72, 165,
  181, 209, 215, 94, 146, 42, 172, 86, 170, 198, 79, 184, 56, 210,
  150, 164, 125, 182, 118, 252, 107, 226, 156, 116, 4, 241, 69, 157,
  112, 89, 100, 113, 135, 32, 134, 91, 207, 101, 230, 45, 168, 2, 27,
  96, 37, 173, 174, 176, 185, 246, 28, 70, 97, 105, 52, 64, 126, 15,
  85, 71, 163, 35, 221, 81, 175, 58, 195, 92, 249, 206, 186, 197,
  234, 38, 44, 83, 13, 110, 133, 40, 132, 9, 211, 223, 205, 244, 65,
  129, 77, 82, 106, 220, 55, 200, 108, 193, 171, 250, 36, 225, 123,
  8, 12, 189, 177, 74, 120, 136, 149, 139, 227, 99, 232, 109, 233,
  203, 213, 254, 59, 0, 29, 57, 242, 239, 183, 14, 102, 88, 208, 228,
  166, 119, 114, 248, 235, 117, 75, 10, 49, 68, 80, 180, 143, 237,
  31, 26, 219, 153, 141, 51, 159, 17, 131, 20
};

void MD2_Init (MD2_CTX *m)
{
	memset(m, 0, sizeof(*m));
}


static void MD2_calc(MD2_CTX *m, const void *v)
{
    unsigned char x[48], L;
    const unsigned char *p = v;
    int i, j, t;

    L = m->checksum[15];
    for (i = 0; i < 16; i++)
		L = m->checksum[i] ^= subst[p[i] ^ L];

    for (i = 0; i < 16; i++) {
		x[i]      = m->state[i];
		x[i + 16] = p[i];
		x[i + 32] = x[i] ^ p[i];
    }

    t = 0;

    for (i = 0; i < 18; i++) {
		for (j = 0; j < 48; j++)
		    t = x[j] ^= subst[t];
		t = (t + i) & 0xff;
    }

    memcpy(m->state, x, 16);
    memset(x, 0, sizeof(x));
}


void MD2_Update (MD2_CTX *m, const void *v, size_t len)
{
    size_t idx = m->len & 0xf;
    const unsigned char *p = v;

    m->len += len;
    if (len + idx >= 16) {
		if (idx) {
		    memcpy(m->data + idx, p, 16 - idx);
		    MD2_calc(m, m->data);
		    p += 16;
		    len -= 16 - idx;
		}
		while (len >= 16) {
		    MD2_calc(m, p);
		    p += 16;
		    len -= 16;
		}
		idx = 0;
    }

    memcpy(m->data + idx, p, len);
}

void MD2_Final (void *res, MD2_CTX *m)
{
    unsigned char pad[16];
    size_t padlen;

    padlen = 16 - (m->len % 16);
    memset(pad, padlen, padlen);

    MD2_Update(m, pad, padlen);
    memcpy(pad, m->checksum, 16);
    MD2_Update(m, pad, 16);

    memcpy(res, m->state, MD2_DIGEST_LENGTH);
    memset(m, 0, sizeof(m));
}

static void MD2Print (unsigned char *MD2_digest)
{
  int i;

  for (i = 0; i < MD2_DIGEST_LENGTH; i++)
  {
    printf ("%02x", MD2_digest[i]);
	if(outfile) fprintf(outfile, "%02x", MD2_digest[i]);
  }
}
#endif /// MD2_CHECKSUM_ENCIPHER




#if MD4_CHECKSUM_ENCIPHER

#define	MD4_BLOCK_LENGTH 				64
#define	MD4_DIGEST_LENGTH 				16
#define	MD4_DIGEST_STRING_LENGTH 		(MD4_DIGEST_LENGTH * 2 + 1)

typedef struct MD4Context {
	uint32_t state[4];			/* state */
	uint32_t count[2];			/* number of bits, mod 2^64 */
	uint8_t buffer[MD4_BLOCK_LENGTH];	/* input buffer */
} MD4_CTX;


void MD4Transform(uint32_t buf[4], const unsigned char *inc);


/*	The below was retrieved from
 *	http://www.openbsd.org/cgi-bin/cvsweb/~checkout~/src/lib/libc/hash/md4.c?rev=1.2
 *	with the following changes:
 *	CVS-$OpenBSD stuff deleted
 *	#includes commented out.
 *	Support context->count as uint32_t[2] instead of uint64_t
 *	Add htole32 define from http://www.squid-cache.org/mail-archive/squid-dev/200307/0130.html
 *		(The bswap32 definition in the patch.)
 *		This is only used on BIG_ENDIAN systems, so we can always swap the bits.
 *	change BYTE_ORDER == LITTLE_ENDIAN (OpenBSD-defined) to WORDS_BIGENDIAN (autoconf-defined)
 */



/*#if BYTE_ORDER == LITTLE_ENDIAN*/
#ifndef WORDS_BIGENDIAN

#define htole32_4(buf)		/* Nothing */
#define htole32_14(buf)		/* Nothing */
#define htole32_16(buf)		/* Nothing */

#else

#define htole32(x) \
 (((((uint32_t)x) & 0xff000000) >> 24) | \
 ((((uint32_t)x) & 0x00ff0000) >> 8) | \
 ((((uint32_t)x) & 0x0000ff00) << 8) | \
 ((((uint32_t)x) & 0x000000ff) << 24)) 

#define htole32_4(buf) do {				\
	(buf)[ 0] = htole32((buf)[ 0]);		\
	(buf)[ 1] = htole32((buf)[ 1]);		\
	(buf)[ 2] = htole32((buf)[ 2]);		\
	(buf)[ 3] = htole32((buf)[ 3]);		\
} while (0)

#define htole32_14(buf) do {			\
	(buf)[ 0] = htole32((buf)[ 0]);		\
	(buf)[ 1] = htole32((buf)[ 1]);		\
	(buf)[ 2] = htole32((buf)[ 2]);		\
	(buf)[ 3] = htole32((buf)[ 3]);		\
	(buf)[ 4] = htole32((buf)[ 4]);		\
	(buf)[ 5] = htole32((buf)[ 5]);		\
	(buf)[ 6] = htole32((buf)[ 6]);		\
	(buf)[ 7] = htole32((buf)[ 7]);		\
	(buf)[ 8] = htole32((buf)[ 8]);		\
	(buf)[ 9] = htole32((buf)[ 9]);		\
	(buf)[10] = htole32((buf)[10]);		\
	(buf)[11] = htole32((buf)[11]);		\
	(buf)[12] = htole32((buf)[12]);		\
	(buf)[13] = htole32((buf)[13]);		\
} while (0)

#define htole32_16(buf) do {			\
	(buf)[ 0] = htole32((buf)[ 0]);		\
	(buf)[ 1] = htole32((buf)[ 1]);		\
	(buf)[ 2] = htole32((buf)[ 2]);		\
	(buf)[ 3] = htole32((buf)[ 3]);		\
	(buf)[ 4] = htole32((buf)[ 4]);		\
	(buf)[ 5] = htole32((buf)[ 5]);		\
	(buf)[ 6] = htole32((buf)[ 6]);		\
	(buf)[ 7] = htole32((buf)[ 7]);		\
	(buf)[ 8] = htole32((buf)[ 8]);		\
	(buf)[ 9] = htole32((buf)[ 9]);		\
	(buf)[10] = htole32((buf)[10]);		\
	(buf)[11] = htole32((buf)[11]);		\
	(buf)[12] = htole32((buf)[12]);		\
	(buf)[13] = htole32((buf)[13]);		\
	(buf)[14] = htole32((buf)[14]);		\
	(buf)[15] = htole32((buf)[15]);		\
} while (0)

#endif

/*
 * Start MD4 accumulation.
 * Set bit count to 0 and buffer to mysterious initialization constants.
 */
void MD4Init(MD4_CTX *ctx)
{
	ctx->count[0] = 0;
	ctx->count[1] = 0;
	ctx->state[0] = 0x67452301;
	ctx->state[1] = 0xefcdab89;
	ctx->state[2] = 0x98badcfe;
	ctx->state[3] = 0x10325476;
}


/*
 * Update context to reflect the concatenation of another buffer full of bytes.
 */
void MD4Update(MD4_CTX *ctx, const unsigned char *buf, size_t len)
{
	uint32_t count;

	/* Bytes already stored in ctx->buffer */
	count = (uint32_t)((ctx->count[0] >> 3) & 0x3f);

	/* Update bitcount */
/*	ctx->count += (uint64_t)len << 3;*/
	if ((ctx->count[0] += ((uint32_t)len << 3)) < (uint32_t)len) 
	{
		/* Overflowed ctx->count[0] */
		ctx->count[1]++;
	}
	ctx->count[1] += ((uint32_t)len >> 29);

	/* Handle any leading odd-sized chunks */
	if (count) 
	{
		unsigned char *p = (unsigned char *)ctx->buffer + count;

		count = MD4_BLOCK_LENGTH - count;
		if (len < count) {
			memcpy(p, buf, len);
			return;
		}
		memcpy(p, buf, count);
		htole32_16((uint32_t *)ctx->buffer);
		MD4Transform(ctx->state, ctx->buffer);
		buf += count;
		len -= count;
	}

	/* Process data in MD4_BLOCK_LENGTH-byte chunks */
	while (len >= MD4_BLOCK_LENGTH) 
	{
		memcpy(ctx->buffer, buf, MD4_BLOCK_LENGTH);
		htole32_16((uint32_t *)ctx->buffer);
		MD4Transform(ctx->state, ctx->buffer);
		buf += MD4_BLOCK_LENGTH;
		len -= MD4_BLOCK_LENGTH;
	}

	/* Handle any remaining bytes of data. */
	memcpy(ctx->buffer, buf, len);
}


/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern 
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
void MD4Final(unsigned char digest[MD4_DIGEST_LENGTH], MD4_CTX *ctx)
{
	uint32_t count;
	unsigned char *p;

	/* number of bytes mod 64 */
	count = (uint32_t)(ctx->count[0] >> 3) & 0x3f;

	/*
	 * Set the first char of padding to 0x80.
	 * This is safe since there is always at least one byte free.
	 */
	p = ctx->buffer + count;
	*p++ = 0x80;

	/* Bytes of padding needed to make 64 bytes */
	count = 64 - 1 - count;

	/* Pad out to 56 mod 64 */
	if (count < 8) {
		/* Two lots of padding:  Pad the first block to 64 bytes */
		memset(p, 0, count);
		htole32_16((uint32_t *)ctx->buffer);
		MD4Transform(ctx->state, ctx->buffer);

		/* Now fill the next block with 56 bytes */
		memset(ctx->buffer, 0, 56);
	} 
	else {
		/* Pad block to 56 bytes */
		memset(p, 0, count - 8);
	}
	htole32_14((uint32_t *)ctx->buffer);

	/* Append bit count and transform */
	((uint32_t *)ctx->buffer)[14] = ctx->count[0];
	((uint32_t *)ctx->buffer)[15] = ctx->count[1];

	MD4Transform(ctx->state, ctx->buffer);
	htole32_4(ctx->state);
	memcpy(digest, ctx->state, MD4_DIGEST_LENGTH);
	memset(ctx, 0, sizeof(*ctx));	/* in case it's sensitive */
}



/* The three core functions - F1 is optimized somewhat */

/* #define F1(x, y, z) (x & y | ~x & z) */
#define MD4_F1(x, y, z) (z ^ (x & (y ^ z)))
#define MD4_F2(x, y, z) ((x & y) | (x & z) | (y & z))
#define MD4_F3(x, y, z) (x ^ y ^ z)

/* This is the central step in the MD4 algorithm. */
#define MD4STEP(f, w, x, y, z, data, s) \
	( w += f(x, y, z) + data,  w = w<<s | w>>(32-s) )

/*
 * The core of the MD4 algorithm, this alters an existing MD4 hash to
 * reflect the addition of 16 longwords of new data.  MD4Update blocks
 * the data and converts bytes into longwords for this routine.
 */
void MD4Transform(uint32_t buf[4], const unsigned char inc[MD4_BLOCK_LENGTH])
{
	uint32_t a, b, c, d;
	const uint32_t *in = (const uint32_t *)inc;

	a = buf[0];
	b = buf[1];
	c = buf[2];
	d = buf[3];

	MD4STEP(MD4_F1, a, b, c, d, in[ 0],  3);
	MD4STEP(MD4_F1, d, a, b, c, in[ 1],  7);
	MD4STEP(MD4_F1, c, d, a, b, in[ 2], 11);
	MD4STEP(MD4_F1, b, c, d, a, in[ 3], 19);
	MD4STEP(MD4_F1, a, b, c, d, in[ 4],  3);
	MD4STEP(MD4_F1, d, a, b, c, in[ 5],  7);
	MD4STEP(MD4_F1, c, d, a, b, in[ 6], 11);
	MD4STEP(MD4_F1, b, c, d, a, in[ 7], 19);
	MD4STEP(MD4_F1, a, b, c, d, in[ 8],  3);
	MD4STEP(MD4_F1, d, a, b, c, in[ 9],  7);
	MD4STEP(MD4_F1, c, d, a, b, in[10], 11);
	MD4STEP(MD4_F1, b, c, d, a, in[11], 19);
	MD4STEP(MD4_F1, a, b, c, d, in[12],  3);
	MD4STEP(MD4_F1, d, a, b, c, in[13],  7);
	MD4STEP(MD4_F1, c, d, a, b, in[14], 11);
	MD4STEP(MD4_F1, b, c, d, a, in[15], 19);

	MD4STEP(MD4_F2, a, b, c, d, in[ 0] + 0x5a827999,  3);
	MD4STEP(MD4_F2, d, a, b, c, in[ 4] + 0x5a827999,  5);
	MD4STEP(MD4_F2, c, d, a, b, in[ 8] + 0x5a827999,  9);
	MD4STEP(MD4_F2, b, c, d, a, in[12] + 0x5a827999, 13);
	MD4STEP(MD4_F2, a, b, c, d, in[ 1] + 0x5a827999,  3);
	MD4STEP(MD4_F2, d, a, b, c, in[ 5] + 0x5a827999,  5);
	MD4STEP(MD4_F2, c, d, a, b, in[ 9] + 0x5a827999,  9);
	MD4STEP(MD4_F2, b, c, d, a, in[13] + 0x5a827999, 13);
	MD4STEP(MD4_F2, a, b, c, d, in[ 2] + 0x5a827999,  3);
	MD4STEP(MD4_F2, d, a, b, c, in[ 6] + 0x5a827999,  5);
	MD4STEP(MD4_F2, c, d, a, b, in[10] + 0x5a827999,  9);
	MD4STEP(MD4_F2, b, c, d, a, in[14] + 0x5a827999, 13);
	MD4STEP(MD4_F2, a, b, c, d, in[ 3] + 0x5a827999,  3);
	MD4STEP(MD4_F2, d, a, b, c, in[ 7] + 0x5a827999,  5);
	MD4STEP(MD4_F2, c, d, a, b, in[11] + 0x5a827999,  9);
	MD4STEP(MD4_F2, b, c, d, a, in[15] + 0x5a827999, 13);

	MD4STEP(MD4_F3, a, b, c, d, in[ 0] + 0x6ed9eba1,  3);
	MD4STEP(MD4_F3, d, a, b, c, in[ 8] + 0x6ed9eba1,  9);
	MD4STEP(MD4_F3, c, d, a, b, in[ 4] + 0x6ed9eba1, 11);
	MD4STEP(MD4_F3, b, c, d, a, in[12] + 0x6ed9eba1, 15);
	MD4STEP(MD4_F3, a, b, c, d, in[ 2] + 0x6ed9eba1,  3);
	MD4STEP(MD4_F3, d, a, b, c, in[10] + 0x6ed9eba1,  9);
	MD4STEP(MD4_F3, c, d, a, b, in[ 6] + 0x6ed9eba1, 11);
	MD4STEP(MD4_F3, b, c, d, a, in[14] + 0x6ed9eba1, 15);
	MD4STEP(MD4_F3, a, b, c, d, in[ 1] + 0x6ed9eba1,  3);
	MD4STEP(MD4_F3, d, a, b, c, in[ 9] + 0x6ed9eba1,  9);
	MD4STEP(MD4_F3, c, d, a, b, in[ 5] + 0x6ed9eba1, 11);
	MD4STEP(MD4_F3, b, c, d, a, in[13] + 0x6ed9eba1, 15);
	MD4STEP(MD4_F3, a, b, c, d, in[ 3] + 0x6ed9eba1,  3);
	MD4STEP(MD4_F3, d, a, b, c, in[11] + 0x6ed9eba1,  9);
	MD4STEP(MD4_F3, c, d, a, b, in[ 7] + 0x6ed9eba1, 11);
	MD4STEP(MD4_F3, b, c, d, a, in[15] + 0x6ed9eba1, 15);

	buf[0] += a;
	buf[1] += b;
	buf[2] += c;
	buf[3] += d;
}


static void MD4Print (unsigned char *MD4_digest)
{
  int i;

  for (i = 0; i < MD4_DIGEST_LENGTH; i++)
  {
    printf ("%02x", MD4_digest[i]);
	if(outfile) fprintf(outfile, "%02x", MD4_digest[i]);
  }
}

#ifdef EXAMPLE___
void md4_calc(unsigned char *output, const unsigned char *input, unsigned int inlen)
{
	MD4_CTX	context;

	MD4Init(&context);
	MD4Update(&context, input, inlen);
	MD4Final(output, &context);
}
#endif
#endif /// MD4_CHECKSUM_ENCIPHER





#ifdef MD5_CHECKSUM_ENCIPHER
/*
 **********************************************************************
 ** md5.h -- Header file for implementation of MD5                   **
 ** RSA Data Security, Inc. MD5 Message Digest Algorithm             **
 ** Created: 2/17/90 RLR                                             **
 ** Revised: 12/27/90 SRD,AJ,BSK,JT Reference C version              **
 ** Revised (for MD5): RLR 4/27/91                                   **
 **   -- G modified to have y&~z instead of y&z                      **
 **   -- FF, GG, HH modified to add in last register done            **
 **   -- Access pattern: round 2 works mod 5, round 3 works mod 3    **
 **   -- distinct additive constant for each step                    **
 **   -- round 4 added, working mod 7                                **
 **********************************************************************
 */

/*
 **********************************************************************
 ** Copyright (C) 1990, RSA Data Security, Inc. All rights reserved. **
 **                                                                  **
 ** License to copy and use this software is granted provided that   **
 ** it is identified as the "RSA Data Security, Inc. MD5 Message     **
 ** Digest Algorithm" in all material mentioning or referencing this **
 ** software or this function.                                       **
 **                                                                  **
 ** License is also granted to make and use derivative works         **
 ** provided that such works are identified as "derived from the RSA **
 ** Data Security, Inc. MD5 Message Digest Algorithm" in all         **
 ** material mentioning or referencing the derived work.             **
 **                                                                  **
 ** RSA Data Security, Inc. makes no representations concerning      **
 ** either the merchantability of this software or the suitability   **
 ** of this software for any particular purpose.  It is provided "as **
 ** is" without express or implied warranty of any kind.             **
 **                                                                  **
 ** These notices must be retained in any copies of any part of this **
 ** documentation and/or software.                                   **
 **********************************************************************
 */


/* Data structure for MD5 (Message Digest) computation */
typedef struct {
  UINT4 i[2];                   /* number of _bits_ handled mod 2^64 */
  UINT4 buf[4];                                    /* scratch buffer */
  unsigned char in[64];                              /* input buffer */
  unsigned char digest[16];     /* actual digest after MD5Final call */
} MD5_CTX;



/*
 **********************************************************************
 ** Copyright (C) 1990, RSA Data Security, Inc. All rights reserved. **
 **                                                                  **
 ** License to copy and use this software is granted provided that   **
 ** it is identified as the "RSA Data Security, Inc. MD5 Message     **
 ** Digest Algorithm" in all material mentioning or referencing this **
 ** software or this function.                                       **
 **                                                                  **
 ** License is also granted to make and use derivative works         **
 ** provided that such works are identified as "derived from the RSA **
 ** Data Security, Inc. MD5 Message Digest Algorithm" in all         **
 ** material mentioning or referencing the derived work.             **
 **                                                                  **
 ** RSA Data Security, Inc. makes no representations concerning      **
 ** either the merchantability of this software or the suitability   **
 ** of this software for any particular purpose.  It is provided "as **
 ** is" without express or implied warranty of any kind.             **
 **                                                                  **
 ** These notices must be retained in any copies of any part of this **
 ** documentation and/or software.                                   **
 **********************************************************************
 */

void MD5Init ();
void MD5Update ();
void MD5Final ();

/* forward declaration */
static void MD5_Transform (UINT4 *buf, UINT4 *in);

static unsigned char PADDING[64] = {
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* F, G and H are basic MD5 functions: selection, majority, parity */
#define MD5_F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define MD5_G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define MD5_H(x, y, z) ((x) ^ (y) ^ (z))
#define MD5_I(x, y, z) ((y) ^ ((x) | (~z))) 


/* ROTATE_LEFT rotates x left n bits */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4 */
/* Rotation is separate from addition to prevent recomputation */
#define FF(a, b, c, d, x, s, ac) \
  {(a) += MD5_F ((b), (c), (d)) + (x) + (UINT4)(ac); \
   (a) = ROTATE_LEFT ((a), (s)); \
   (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) \
  {(a) += MD5_G ((b), (c), (d)) + (x) + (UINT4)(ac); \
   (a) = ROTATE_LEFT ((a), (s)); \
   (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) \
  {(a) += MD5_H ((b), (c), (d)) + (x) + (UINT4)(ac); \
   (a) = ROTATE_LEFT ((a), (s)); \
   (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) \
  {(a) += MD5_I ((b), (c), (d)) + (x) + (UINT4)(ac); \
   (a) = ROTATE_LEFT ((a), (s)); \
   (a) += (b); \
  }

void MD5Init (MD5_CTX *mdContext)
{
  mdContext->i[0] = mdContext->i[1] = (UINT4)0;

  /* Load magic initialization constants.
   */
  mdContext->buf[0] = (UINT4)0x67452301UL;
  mdContext->buf[1] = (UINT4)0xefcdab89UL;
  mdContext->buf[2] = (UINT4)0x98badcfeUL;
  mdContext->buf[3] = (UINT4)0x10325476UL;
}

void MD5Update (MD5_CTX *mdContext, unsigned char *inBuf, unsigned int inLen)
{
  UINT4 in[16];
  int mdi;
  unsigned int i, ii;

  /* compute number of bytes mod 64 */
  mdi = (int)((mdContext->i[0] >> 3) & 0x3F);

  /* update number of bits */
  if ((mdContext->i[0] + ((UINT4)inLen << 3)) < mdContext->i[0])
    mdContext->i[1]++;
  mdContext->i[0] += ((UINT4)inLen << 3);
  mdContext->i[1] += ((UINT4)inLen >> 29);

  while (inLen--) {
    /* add new character to buffer, increment mdi */
    mdContext->in[mdi++] = *inBuf++;

    /* transform if necessary */
    if (mdi == 0x40) {
      for (i = 0, ii = 0; i < 16; i++, ii += 4)
        in[i] = (((UINT4)mdContext->in[ii+3]) << 24) |
                (((UINT4)mdContext->in[ii+2]) << 16) |
                (((UINT4)mdContext->in[ii+1]) << 8) |
                ((UINT4)mdContext->in[ii]);
      MD5_Transform (mdContext->buf, in);
      mdi = 0;
    }
  }
}

void MD5Final (MD5_CTX *mdContext)
{
  UINT4 in[16];
  int mdi;
  unsigned int i, ii;
  unsigned int padLen;

  /* save number of bits */
  in[14] = mdContext->i[0];
  in[15] = mdContext->i[1];

  /* compute number of bytes mod 64 */
  mdi = (int)((mdContext->i[0] >> 3) & 0x3F);

  /* pad out to 56 mod 64 */
  padLen = (mdi < 56) ? (56 - mdi) : (120 - mdi);
  MD5Update (mdContext, PADDING, padLen);

  /* append length in bits and transform */
  for (i = 0, ii = 0; i < 14; i++, ii += 4)
    in[i] = (((UINT4)mdContext->in[ii+3]) << 24) |
            (((UINT4)mdContext->in[ii+2]) << 16) |
            (((UINT4)mdContext->in[ii+1]) << 8) |
            ((UINT4)mdContext->in[ii]);
  MD5_Transform (mdContext->buf, in);

  /* store buffer in digest */
  for (i = 0, ii = 0; i < 4; i++, ii += 4) {
    mdContext->digest[ii] = (unsigned char)(mdContext->buf[i] & 0xFF);
    mdContext->digest[ii+1] =
      (unsigned char)((mdContext->buf[i] >> 8) & 0xFF);
    mdContext->digest[ii+2] =
      (unsigned char)((mdContext->buf[i] >> 16) & 0xFF);
    mdContext->digest[ii+3] =
      (unsigned char)((mdContext->buf[i] >> 24) & 0xFF);
  }
}


/* Basic MD5 step. MD5_Transform buf based on in.
 */
static void MD5_Transform (UINT4 *buf, UINT4 *in)
{
  UINT4 a = buf[0], b = buf[1], c = buf[2], d = buf[3];

  /* Round 1 */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
  FF ( a, b, c, d, in[ 0], S11, 3614090360UL); /* 1 */
  FF ( d, a, b, c, in[ 1], S12, 3905402710UL); /* 2 */
  FF ( c, d, a, b, in[ 2], S13,  606105819UL); /* 3 */
  FF ( b, c, d, a, in[ 3], S14, 3250441966UL); /* 4 */
  FF ( a, b, c, d, in[ 4], S11, 4118548399UL); /* 5 */
  FF ( d, a, b, c, in[ 5], S12, 1200080426UL); /* 6 */
  FF ( c, d, a, b, in[ 6], S13, 2821735955UL); /* 7 */
  FF ( b, c, d, a, in[ 7], S14, 4249261313UL); /* 8 */
  FF ( a, b, c, d, in[ 8], S11, 1770035416UL); /* 9 */
  FF ( d, a, b, c, in[ 9], S12, 2336552879UL); /* 10 */
  FF ( c, d, a, b, in[10], S13, 4294925233UL); /* 11 */
  FF ( b, c, d, a, in[11], S14, 2304563134UL); /* 12 */
  FF ( a, b, c, d, in[12], S11, 1804603682UL); /* 13 */
  FF ( d, a, b, c, in[13], S12, 4254626195UL); /* 14 */
  FF ( c, d, a, b, in[14], S13, 2792965006UL); /* 15 */
  FF ( b, c, d, a, in[15], S14, 1236535329UL); /* 16 */

  /* Round 2 */
#define S21 5
#define S22 9
#define S23 14
#define S24 20
  GG ( a, b, c, d, in[ 1], S21, 4129170786UL); /* 17 */
  GG ( d, a, b, c, in[ 6], S22, 3225465664UL); /* 18 */
  GG ( c, d, a, b, in[11], S23,  643717713UL); /* 19 */
  GG ( b, c, d, a, in[ 0], S24, 3921069994UL); /* 20 */
  GG ( a, b, c, d, in[ 5], S21, 3593408605UL); /* 21 */
  GG ( d, a, b, c, in[10], S22,   38016083UL); /* 22 */
  GG ( c, d, a, b, in[15], S23, 3634488961UL); /* 23 */
  GG ( b, c, d, a, in[ 4], S24, 3889429448UL); /* 24 */
  GG ( a, b, c, d, in[ 9], S21,  568446438UL); /* 25 */
  GG ( d, a, b, c, in[14], S22, 3275163606UL); /* 26 */
  GG ( c, d, a, b, in[ 3], S23, 4107603335UL); /* 27 */
  GG ( b, c, d, a, in[ 8], S24, 1163531501UL); /* 28 */
  GG ( a, b, c, d, in[13], S21, 2850285829UL); /* 29 */
  GG ( d, a, b, c, in[ 2], S22, 4243563512UL); /* 30 */
  GG ( c, d, a, b, in[ 7], S23, 1735328473UL); /* 31 */
  GG ( b, c, d, a, in[12], S24, 2368359562UL); /* 32 */

  /* Round 3 */
#define S31 4
#define S32 11
#define S33 16
#define S34 23
  HH ( a, b, c, d, in[ 5], S31, 4294588738UL); /* 33 */
  HH ( d, a, b, c, in[ 8], S32, 2272392833UL); /* 34 */
  HH ( c, d, a, b, in[11], S33, 1839030562UL); /* 35 */
  HH ( b, c, d, a, in[14], S34, 4259657740UL); /* 36 */
  HH ( a, b, c, d, in[ 1], S31, 2763975236UL); /* 37 */
  HH ( d, a, b, c, in[ 4], S32, 1272893353UL); /* 38 */
  HH ( c, d, a, b, in[ 7], S33, 4139469664UL); /* 39 */
  HH ( b, c, d, a, in[10], S34, 3200236656UL); /* 40 */
  HH ( a, b, c, d, in[13], S31,  681279174UL); /* 41 */
  HH ( d, a, b, c, in[ 0], S32, 3936430074UL); /* 42 */
  HH ( c, d, a, b, in[ 3], S33, 3572445317UL); /* 43 */
  HH ( b, c, d, a, in[ 6], S34,   76029189UL); /* 44 */
  HH ( a, b, c, d, in[ 9], S31, 3654602809UL); /* 45 */
  HH ( d, a, b, c, in[12], S32, 3873151461UL); /* 46 */
  HH ( c, d, a, b, in[15], S33,  530742520UL); /* 47 */
  HH ( b, c, d, a, in[ 2], S34, 3299628645UL); /* 48 */

  /* Round 4 */
#define S41 6
#define S42 10
#define S43 15
#define S44 21
  II ( a, b, c, d, in[ 0], S41, 4096336452UL); /* 49 */
  II ( d, a, b, c, in[ 7], S42, 1126891415UL); /* 50 */
  II ( c, d, a, b, in[14], S43, 2878612391UL); /* 51 */
  II ( b, c, d, a, in[ 5], S44, 4237533241UL); /* 52 */
  II ( a, b, c, d, in[12], S41, 1700485571UL); /* 53 */
  II ( d, a, b, c, in[ 3], S42, 2399980690UL); /* 54 */
  II ( c, d, a, b, in[10], S43, 4293915773UL); /* 55 */
  II ( b, c, d, a, in[ 1], S44, 2240044497UL); /* 56 */
  II ( a, b, c, d, in[ 8], S41, 1873313359UL); /* 57 */
  II ( d, a, b, c, in[15], S42, 4264355552UL); /* 58 */
  II ( c, d, a, b, in[ 6], S43, 2734768916UL); /* 59 */
  II ( b, c, d, a, in[13], S44, 1309151649UL); /* 60 */
  II ( a, b, c, d, in[ 4], S41, 4149444226UL); /* 61 */
  II ( d, a, b, c, in[11], S42, 3174756917UL); /* 62 */
  II ( c, d, a, b, in[ 2], S43,  718787259UL); /* 63 */
  II ( b, c, d, a, in[ 9], S44, 3951481745UL); /* 64 */

  buf[0] += a;
  buf[1] += b;
  buf[2] += c;
  buf[3] += d;
}


/* Prints message digest buffer in mdContext as 32 hexadecimal digits.
   Order is from low-order byte to high-order byte of digest.
   Each byte is printed with high-order hexadecimal digit first.
 */
static void MDPrint (MD5_CTX *mdContext)
{
  int i;

  for (i = 0; i < 16; i++)
  {
    printf ("%02x", mdContext->digest[i]);
	if(outfile) fprintf(outfile, "%02x", mdContext->digest[i]);
  }
}



/*
 **********************************************************************
 ** End of md5.c                                                     **
 ******************************* (cut) ********************************
 */

#endif /// MD5_CHECKSUM_ENCIPHER





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


#define MAX_BUFFER_SIZE				4096

#define OBJECT_FILE_LIST 			0xFF000001
#define OBJECT_COMMIT_FILE 			0xFF000002




/*20 bytes for sha so 40 bytes to put it in hex and one byte for NULL
 * A SHA is valid only if it ends with a NULL char*/
#define SHA_HASH_LENGTH				40


typedef unsigned char ShaBuffer[SHA_HASH_LENGTH+1];

///bool sha_buffer(const unsigned char *input, int ilen, ShaBuffer sha);
#ifdef __NOT_USED__
bool sha_file(const char *path, ShaBuffer sha);
#endif
///bool sha_compare(const ShaBuffer s1, const ShaBuffer s2);
/// inline void sha_reset(ShaBuffer sha);



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

static void sha1_process( sha1_context *ctx, const unsigned char data[64] )
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
		char str[3];
		snprintf(str, 3, "%2.2x", (unsigned char)buffer[i]);
		strcat((char*)output, str);
	}
	output[SHA_HASH_LENGTH] = '\0';
}

/*
 * output = SHA-1( input buffer )
 */
bool sha_buffer( const unsigned char *input, int ilen, ShaBuffer output)
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
bool sha_file( const char *path, ShaBuffer output)
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

bool sha_compare(const ShaBuffer s1, const ShaBuffer s2)
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


#if SHA2_256_384_512 /// 2014.06.30
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


#define SHA2_BUFLEN 			(100*1024) // 16384 // 16*1024



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



/*** SHA-256/384/512 Machine Architecture Definitions *****************/
/*
 * BYTE_ORDER NOTE:
 *
 * Please make sure that your system defines BYTE_ORDER.  If your
 * architecture is little-endian, make sure it also defines
 * LITTLE_ENDIAN and that the two (BYTE_ORDER and LITTLE_ENDIAN) are
 * equivilent.
 *
 * If your system does not define the above, then you can do so by
 * hand like this:
 *
 *   #define LITTLE_ENDIAN 1234
 *   #define BIG_ENDIAN    4321
 *
 * And for little-endian machines, add:
 *
 *   #define BYTE_ORDER LITTLE_ENDIAN 
 *
 * Or for big-endian machines:
 *
 *   #define BYTE_ORDER BIG_ENDIAN
 *
 * The FreeBSD machine this was written on defines BYTE_ORDER
 * appropriately by including <sys/types.h> (which in turn includes
 * <machine/endian.h> where the appropriate definitions are actually
 * made).
 */
#if !defined(BYTE_ORDER) || (BYTE_ORDER != LITTLE_ENDIAN && BYTE_ORDER != BIG_ENDIAN)
#error Define BYTE_ORDER to be equal to either LITTLE_ENDIAN or BIG_ENDIAN
#endif

/*
 * Define the followingsha2_* types to types of the correct length on
 * the native archtecture.   Most BSD systems and Linux define u_intXX_t
 * types.  Machines with very recent ANSI C headers, can use the
 * uintXX_t definintions from inttypes.h by defining SHA2_USE_INTTYPES_H
 * during compile or in the sha.h header file.
 *
 * Machines that support neither u_intXX_t nor inttypes.h's uintXX_t
 * will need to define these three typedefs below (and the appropriate
 * ones in sha.h too) by hand according to their system architecture.
 *
 * Thank you, Jun-ichiro itojun Hagino, for suggesting using u_intXX_t
 * types and pointing out recent ANSI C support for uintXX_t in inttypes.h.
 */
#ifdef SHA2_USE_INTTYPES_H

typedef uint8_t  sha2_byte;	/* Exactly 1 byte */
typedef uint32_t sha2_word32;	/* Exactly 4 bytes */
typedef uint64_t sha2_word64;	/* Exactly 8 bytes */

#else /* SHA2_USE_INTTYPES_H */

typedef u_int8_t  sha2_byte;	/* Exactly 1 byte */
typedef u_int32_t sha2_word32;	/* Exactly 4 bytes */
typedef u_int64_t sha2_word64;	/* Exactly 8 bytes */

#endif /* SHA2_USE_INTTYPES_H */


/*** SHA-256/384/512 Various Length Definitions ***********************/
/* NOTE: Most of these are in sha2.h */
#define SHA256_SHORT_BLOCK_LENGTH 			(SHA256_BLOCK_LENGTH - 8)
#define SHA384_SHORT_BLOCK_LENGTH 			(SHA384_BLOCK_LENGTH - 16)
#define SHA512_SHORT_BLOCK_LENGTH 			(SHA512_BLOCK_LENGTH - 16)


/*** ENDIAN REVERSAL MACROS *******************************************/
#if BYTE_ORDER == LITTLE_ENDIAN
#define REVERSE32(w,x)	{ \
	sha2_word32 tmp = (w); \
	tmp = (tmp >> 16) | (tmp << 16); \
	(x) = ((tmp & 0xff00ff00UL) >> 8) | ((tmp & 0x00ff00ffUL) << 8); \
}
#define REVERSE64(w,x)	{ \
	sha2_word64 tmp = (w); \
	tmp = (tmp >> 32) | (tmp << 32); \
	tmp = ((tmp & 0xff00ff00ff00ff00ULL) >> 8) | \
	      ((tmp & 0x00ff00ff00ff00ffULL) << 8); \
	(x) = ((tmp & 0xffff0000ffff0000ULL) >> 16) | \
	      ((tmp & 0x0000ffff0000ffffULL) << 16); \
}
#endif /* BYTE_ORDER == LITTLE_ENDIAN */

/*
 * Macro for incrementally adding the unsigned 64-bit integer n to the
 * unsigned 128-bit integer (represented using a two-element array of
 * 64-bit words):
 */
#define ADDINC128(w,n)	{ \
	(w)[0] += (sha2_word64)(n); \
	if ((w)[0] < (n)) { \
		(w)[1]++; \
	} \
}

/*
 * Macros for copying blocks of memory and for zeroing out ranges
 * of memory.  Using these macros makes it easy to switch from
 * using memset()/memcpy() and using bzero()/bcopy().
 *
 * Please define either SHA2_USE_MEMSET_MEMCPY or define
 * SHA2_USE_BZERO_BCOPY depending on which function set you
 * choose to use:
 */
#if !defined(SHA2_USE_MEMSET_MEMCPY) && !defined(SHA2_USE_BZERO_BCOPY)
/* Default to memset()/memcpy() if no option is specified */
#define	SHA2_USE_MEMSET_MEMCPY	1
#endif
#if defined(SHA2_USE_MEMSET_MEMCPY) && defined(SHA2_USE_BZERO_BCOPY)
/* Abort with an error if BOTH options are defined */
#error Define either SHA2_USE_MEMSET_MEMCPY or SHA2_USE_BZERO_BCOPY, not both!
#endif

#ifdef SHA2_USE_MEMSET_MEMCPY
#define MEMSET_BZERO(p,l)	memset((p), 0, (l))
#define MEMCPY_BCOPY(d,s,l)	memcpy((d), (s), (l))
#endif
#ifdef SHA2_USE_BZERO_BCOPY
#define MEMSET_BZERO(p,l)	bzero((p), (l))
#define MEMCPY_BCOPY(d,s,l)	bcopy((s), (d), (l))
#endif


/*** THE SIX LOGICAL FUNCTIONS ****************************************/
/*
 * Bit shifting and rotation (used by the six SHA-XYZ logical functions:
 *
 *   NOTE:  The naming of R and S appears backwards here (R is a SHIFT and
 *   S is a ROTATION) because the SHA-256/384/512 description document
 *   (see http://csrc.nist.gov/cryptval/shs/sha256-384-512.pdf) uses this
 *   same "backwards" definition.
 */
/* Shift-right (used in SHA-256, SHA-384, and SHA-512): */
#define SHA2_R(b,x) 		((x) >> (b))
/* 32-bit Rotate-right (used in SHA-256): */
#define SHA2_S32(b,x)	(((x) >> (b)) | ((x) << (32 - (b))))
/* 64-bit Rotate-right (used in SHA-384 and SHA-512): */
#define SHA2_S64(b,x)	(((x) >> (b)) | ((x) << (64 - (b))))

/* Two of six logical functions used in SHA-256, SHA-384, and SHA-512: */
#define Ch(x,y,z)	(((x) & (y)) ^ ((~(x)) & (z)))
#define Maj(x,y,z)	(((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

/* Four of six logical functions used in SHA-256: */
#define Sigma0_256(x)	(SHA2_S32(2,  (x)) ^ SHA2_S32(13, (x)) ^ SHA2_S32(22, (x)))
#define Sigma1_256(x)	(SHA2_S32(6,  (x)) ^ SHA2_S32(11, (x)) ^ SHA2_S32(25, (x)))
#define sigma0_256(x)	(SHA2_S32(7,  (x)) ^ SHA2_S32(18, (x)) ^ SHA2_R(3 ,   (x)))
#define sigma1_256(x)	(SHA2_S32(17, (x)) ^ SHA2_S32(19, (x)) ^ SHA2_R(10,   (x)))

/* Four of six logical functions used in SHA-384 and SHA-512: */
#define Sigma0_512(x)	(SHA2_S64(28, (x)) ^ SHA2_S64(34, (x)) ^ SHA2_S64(39, (x)))
#define Sigma1_512(x)	(SHA2_S64(14, (x)) ^ SHA2_S64(18, (x)) ^ SHA2_S64(41, (x)))
#define sigma0_512(x)	(SHA2_S64( 1, (x)) ^ SHA2_S64( 8, (x)) ^ SHA2_R(7,   (x)))
#define sigma1_512(x)	(SHA2_S64(19, (x)) ^ SHA2_S64(61, (x)) ^ SHA2_R(6,   (x)))

/*** INTERNAL FUNCTION PROTOTYPES *************************************/
/* NOTE: These should not be accessed directly from outside this
 * library -- they are intended for private internal visibility/use
 * only.
 */
void SHA512_Last(SHA512_CTX*);
void SHA256_Transform(SHA256_CTX*, const sha2_word32*);
void SHA512_Transform(SHA512_CTX*, const sha2_word64*);


/*** SHA-XYZ INITIAL HASH VALUES AND CONSTANTS ************************/
/* Hash constant words K for SHA-256: */
const static sha2_word32 K256[64] = {
	0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL,
	0x3956c25bUL, 0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL,
	0xd807aa98UL, 0x12835b01UL, 0x243185beUL, 0x550c7dc3UL,
	0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL, 0xc19bf174UL,
	0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
	0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL,
	0x983e5152UL, 0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL,
	0xc6e00bf3UL, 0xd5a79147UL, 0x06ca6351UL, 0x14292967UL,
	0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL, 0x53380d13UL,
	0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
	0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL,
	0xd192e819UL, 0xd6990624UL, 0xf40e3585UL, 0x106aa070UL,
	0x19a4c116UL, 0x1e376c08UL, 0x2748774cUL, 0x34b0bcb5UL,
	0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL, 0x682e6ff3UL,
	0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
	0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};

/* Initial hash value H for SHA-256: */
const static sha2_word32 sha256_initial_hash_value[8] = {
	0x6a09e667UL,
	0xbb67ae85UL,
	0x3c6ef372UL,
	0xa54ff53aUL,
	0x510e527fUL,
	0x9b05688cUL,
	0x1f83d9abUL,
	0x5be0cd19UL
};

/* Hash constant words K for SHA-384 and SHA-512: */
const static sha2_word64 K512[80] = {
	0x428a2f98d728ae22ULL, 0x7137449123ef65cdULL,
	0xb5c0fbcfec4d3b2fULL, 0xe9b5dba58189dbbcULL,
	0x3956c25bf348b538ULL, 0x59f111f1b605d019ULL,
	0x923f82a4af194f9bULL, 0xab1c5ed5da6d8118ULL,
	0xd807aa98a3030242ULL, 0x12835b0145706fbeULL,
	0x243185be4ee4b28cULL, 0x550c7dc3d5ffb4e2ULL,
	0x72be5d74f27b896fULL, 0x80deb1fe3b1696b1ULL,
	0x9bdc06a725c71235ULL, 0xc19bf174cf692694ULL,
	0xe49b69c19ef14ad2ULL, 0xefbe4786384f25e3ULL,
	0x0fc19dc68b8cd5b5ULL, 0x240ca1cc77ac9c65ULL,
	0x2de92c6f592b0275ULL, 0x4a7484aa6ea6e483ULL,
	0x5cb0a9dcbd41fbd4ULL, 0x76f988da831153b5ULL,
	0x983e5152ee66dfabULL, 0xa831c66d2db43210ULL,
	0xb00327c898fb213fULL, 0xbf597fc7beef0ee4ULL,
	0xc6e00bf33da88fc2ULL, 0xd5a79147930aa725ULL,
	0x06ca6351e003826fULL, 0x142929670a0e6e70ULL,
	0x27b70a8546d22ffcULL, 0x2e1b21385c26c926ULL,
	0x4d2c6dfc5ac42aedULL, 0x53380d139d95b3dfULL,
	0x650a73548baf63deULL, 0x766a0abb3c77b2a8ULL,
	0x81c2c92e47edaee6ULL, 0x92722c851482353bULL,
	0xa2bfe8a14cf10364ULL, 0xa81a664bbc423001ULL,
	0xc24b8b70d0f89791ULL, 0xc76c51a30654be30ULL,
	0xd192e819d6ef5218ULL, 0xd69906245565a910ULL,
	0xf40e35855771202aULL, 0x106aa07032bbd1b8ULL,
	0x19a4c116b8d2d0c8ULL, 0x1e376c085141ab53ULL,
	0x2748774cdf8eeb99ULL, 0x34b0bcb5e19b48a8ULL,
	0x391c0cb3c5c95a63ULL, 0x4ed8aa4ae3418acbULL,
	0x5b9cca4f7763e373ULL, 0x682e6ff3d6b2b8a3ULL,
	0x748f82ee5defb2fcULL, 0x78a5636f43172f60ULL,
	0x84c87814a1f0ab72ULL, 0x8cc702081a6439ecULL,
	0x90befffa23631e28ULL, 0xa4506cebde82bde9ULL,
	0xbef9a3f7b2c67915ULL, 0xc67178f2e372532bULL,
	0xca273eceea26619cULL, 0xd186b8c721c0c207ULL,
	0xeada7dd6cde0eb1eULL, 0xf57d4f7fee6ed178ULL,
	0x06f067aa72176fbaULL, 0x0a637dc5a2c898a6ULL,
	0x113f9804bef90daeULL, 0x1b710b35131c471bULL,
	0x28db77f523047d84ULL, 0x32caab7b40c72493ULL,
	0x3c9ebe0a15c9bebcULL, 0x431d67c49c100d4cULL,
	0x4cc5d4becb3e42b6ULL, 0x597f299cfc657e2aULL,
	0x5fcb6fab3ad6faecULL, 0x6c44198c4a475817ULL
};

/* Initial hash value H for SHA-384 */
const static sha2_word64 sha384_initial_hash_value[8] = {
	0xcbbb9d5dc1059ed8ULL,
	0x629a292a367cd507ULL,
	0x9159015a3070dd17ULL,
	0x152fecd8f70e5939ULL,
	0x67332667ffc00b31ULL,
	0x8eb44a8768581511ULL,
	0xdb0c2e0d64f98fa7ULL,
	0x47b5481dbefa4fa4ULL
};

/* Initial hash value H for SHA-512 */
const static sha2_word64 sha512_initial_hash_value[8] = {
	0x6a09e667f3bcc908ULL,
	0xbb67ae8584caa73bULL,
	0x3c6ef372fe94f82bULL,
	0xa54ff53a5f1d36f1ULL,
	0x510e527fade682d1ULL,
	0x9b05688c2b3e6c1fULL,
	0x1f83d9abfb41bd6bULL,
	0x5be0cd19137e2179ULL
};

/*
 * Constant used by SHA256/384/512_End() functions for converting the
 * digest to a readable hexadecimal character string:
 */
static const char *sha2_hex_digits = "0123456789abcdef";


/*** SHA-256: *********************************************************/
void SHA256_Init(SHA256_CTX* context) 
{
	if (context == (SHA256_CTX*)0) {
		return;
	}
	MEMCPY_BCOPY(context->state, sha256_initial_hash_value, SHA256_DIGEST_LENGTH);
	MEMSET_BZERO(context->buffer, SHA256_BLOCK_LENGTH);
	context->bitcount = 0;
}

#ifdef SHA2_UNROLL_TRANSFORM

/* Unrolled SHA-256 round macros: */

#if BYTE_ORDER == LITTLE_ENDIAN

#define ROUND256_0_TO_15(a,b,c,d,e,f,g,h)	\
	REVERSE32(*data++, W256[j]); \
	T1 = (h) + Sigma1_256(e) + Ch((e), (f), (g)) + \
             K256[j] + W256[j]; \
	(d) += T1; \
	(h) = T1 + Sigma0_256(a) + Maj((a), (b), (c)); \
	j++


#else /* BYTE_ORDER == LITTLE_ENDIAN */

#define ROUND256_0_TO_15(a,b,c,d,e,f,g,h)	\
	T1 = (h) + Sigma1_256(e) + Ch((e), (f), (g)) + \
	     K256[j] + (W256[j] = *data++); \
	(d) += T1; \
	(h) = T1 + Sigma0_256(a) + Maj((a), (b), (c)); \
	j++

#endif /* BYTE_ORDER == LITTLE_ENDIAN */

#define ROUND256(a,b,c,d,e,f,g,h)	\
	s0 = W256[(j+1)&0x0f]; \
	s0 = sigma0_256(s0); \
	s1 = W256[(j+14)&0x0f]; \
	s1 = sigma1_256(s1); \
	T1 = (h) + Sigma1_256(e) + Ch((e), (f), (g)) + K256[j] + \
	     (W256[j&0x0f] += s1 + W256[(j+9)&0x0f] + s0); \
	(d) += T1; \
	(h) = T1 + Sigma0_256(a) + Maj((a), (b), (c)); \
	j++


void SHA256_Transform(SHA256_CTX* context, const sha2_word32* data) 
{
	sha2_word32	a, b, c, d, e, f, g, h, s0, s1;
	sha2_word32	T1, *W256;
	int		j;

	W256 = (sha2_word32*)context->buffer;

	/* Initialize registers with the prev. intermediate value */
	a = context->state[0];
	b = context->state[1];
	c = context->state[2];
	d = context->state[3];
	e = context->state[4];
	f = context->state[5];
	g = context->state[6];
	h = context->state[7];

	j = 0;
	do {
		/* Rounds 0 to 15 (unrolled): */
		ROUND256_0_TO_15(a,b,c,d,e,f,g,h);
		ROUND256_0_TO_15(h,a,b,c,d,e,f,g);
		ROUND256_0_TO_15(g,h,a,b,c,d,e,f);
		ROUND256_0_TO_15(f,g,h,a,b,c,d,e);
		ROUND256_0_TO_15(e,f,g,h,a,b,c,d);
		ROUND256_0_TO_15(d,e,f,g,h,a,b,c);
		ROUND256_0_TO_15(c,d,e,f,g,h,a,b);
		ROUND256_0_TO_15(b,c,d,e,f,g,h,a);
	} while (j < 16);

	/* Now for the remaining rounds to 64: */
	do {
		ROUND256(a,b,c,d,e,f,g,h);
		ROUND256(h,a,b,c,d,e,f,g);
		ROUND256(g,h,a,b,c,d,e,f);
		ROUND256(f,g,h,a,b,c,d,e);
		ROUND256(e,f,g,h,a,b,c,d);
		ROUND256(d,e,f,g,h,a,b,c);
		ROUND256(c,d,e,f,g,h,a,b);
		ROUND256(b,c,d,e,f,g,h,a);
	} while (j < 64);

	/* Compute the current intermediate hash value */
	context->state[0] += a;
	context->state[1] += b;
	context->state[2] += c;
	context->state[3] += d;
	context->state[4] += e;
	context->state[5] += f;
	context->state[6] += g;
	context->state[7] += h;

	/* Clean up */
	a = b = c = d = e = f = g = h = T1 = 0;
}

#else /* SHA2_UNROLL_TRANSFORM */

void SHA256_Transform(SHA256_CTX* context, const sha2_word32* data) 
{
	sha2_word32	a, b, c, d, e, f, g, h, s0, s1;
	sha2_word32	T1, T2, *W256;
	int		j;

	W256 = (sha2_word32*)context->buffer;

	/* Initialize registers with the prev. intermediate value */
	a = context->state[0];
	b = context->state[1];
	c = context->state[2];
	d = context->state[3];
	e = context->state[4];
	f = context->state[5];
	g = context->state[6];
	h = context->state[7];

	j = 0;
	do {
#if BYTE_ORDER == LITTLE_ENDIAN
		/* Copy data while converting to host byte order */
		REVERSE32(*data++,W256[j]);
		/* Apply the SHA-256 compression function to update a..h */
		T1 = h + Sigma1_256(e) + Ch(e, f, g) + K256[j] + W256[j];
#else /* BYTE_ORDER == LITTLE_ENDIAN */
		/* Apply the SHA-256 compression function to update a..h with copy */
		T1 = h + Sigma1_256(e) + Ch(e, f, g) + K256[j] + (W256[j] = *data++);
#endif /* BYTE_ORDER == LITTLE_ENDIAN */
		T2 = Sigma0_256(a) + Maj(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + T1;
		d = c;
		c = b;
		b = a;
		a = T1 + T2;

		j++;
	} while (j < 16);

	do {
		/* Part of the message block expansion: */
		s0 = W256[(j+1)&0x0f];
		s0 = sigma0_256(s0);
		s1 = W256[(j+14)&0x0f];	
		s1 = sigma1_256(s1);

		/* Apply the SHA-256 compression function to update a..h */
		T1 = h + Sigma1_256(e) + Ch(e, f, g) + K256[j] + 
		     (W256[j&0x0f] += s1 + W256[(j+9)&0x0f] + s0);
		T2 = Sigma0_256(a) + Maj(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + T1;
		d = c;
		c = b;
		b = a;
		a = T1 + T2;

		j++;
	} while (j < 64);

	/* Compute the current intermediate hash value */
	context->state[0] += a;
	context->state[1] += b;
	context->state[2] += c;
	context->state[3] += d;
	context->state[4] += e;
	context->state[5] += f;
	context->state[6] += g;
	context->state[7] += h;

	/* Clean up */
	a = b = c = d = e = f = g = h = T1 = T2 = 0;
}

#endif /* SHA2_UNROLL_TRANSFORM */

void SHA256_Update(SHA256_CTX* context, const sha2_byte *data, size_t len) 
{
	unsigned int	freespace, usedspace;

	if (len == 0) {
		/* Calling with no data is valid - we do nothing */
		return;
	}

	/* Sanity check: */
	assert(context != (SHA256_CTX*)0 && data != (sha2_byte*)0);

	usedspace = (context->bitcount >> 3) % SHA256_BLOCK_LENGTH;
	if (usedspace > 0) {
		/* Calculate how much free space is available in the buffer */
		freespace = SHA256_BLOCK_LENGTH - usedspace;

		if (len >= freespace) {
			/* Fill the buffer completely and process it */
			MEMCPY_BCOPY(&context->buffer[usedspace], data, freespace);
			context->bitcount += freespace << 3;
			len -= freespace;
			data += freespace;
			SHA256_Transform(context, (sha2_word32*)context->buffer);
		} else {
			/* The buffer is not yet full */
			MEMCPY_BCOPY(&context->buffer[usedspace], data, len);
			context->bitcount += len << 3;
			/* Clean up: */
			usedspace = freespace = 0;
			return;
		}
	}
	while (len >= SHA256_BLOCK_LENGTH) {
		/* Process as many complete blocks as we can */
		SHA256_Transform(context, (sha2_word32*)data);
		context->bitcount += SHA256_BLOCK_LENGTH << 3;
		len -= SHA256_BLOCK_LENGTH;
		data += SHA256_BLOCK_LENGTH;
	}
	if (len > 0) {
		/* There's left-overs, so save 'em */
		MEMCPY_BCOPY(context->buffer, data, len);
		context->bitcount += len << 3;
	}
	/* Clean up: */
	usedspace = freespace = 0;
}

void SHA256_Final(sha2_byte digest[], SHA256_CTX* context) 
{
	sha2_word32	*d = (sha2_word32*)digest;
	unsigned int	usedspace;

	/* Sanity check: */
	assert(context != (SHA256_CTX*)0);

	/* If no digest buffer is passed, we don't bother doing this: */
	if (digest != (sha2_byte*)0) {
		usedspace = (context->bitcount >> 3) % SHA256_BLOCK_LENGTH;
#if BYTE_ORDER == LITTLE_ENDIAN
		/* Convert FROM host byte order */
		REVERSE64(context->bitcount,context->bitcount);
#endif
		if (usedspace > 0) {
			/* Begin padding with a 1 bit: */
			context->buffer[usedspace++] = 0x80;

			if (usedspace <= SHA256_SHORT_BLOCK_LENGTH) {
				/* Set-up for the last transform: */
				MEMSET_BZERO(&context->buffer[usedspace], SHA256_SHORT_BLOCK_LENGTH - usedspace);
			} else {
				if (usedspace < SHA256_BLOCK_LENGTH) {
					MEMSET_BZERO(&context->buffer[usedspace], SHA256_BLOCK_LENGTH - usedspace);
				}
				/* Do second-to-last transform: */
				SHA256_Transform(context, (sha2_word32*)context->buffer);

				/* And set-up for the last transform: */
				MEMSET_BZERO(context->buffer, SHA256_SHORT_BLOCK_LENGTH);
			}
		} else {
			/* Set-up for the last transform: */
			MEMSET_BZERO(context->buffer, SHA256_SHORT_BLOCK_LENGTH);

			/* Begin padding with a 1 bit: */
			*context->buffer = 0x80;
		}
		/* Set the bit count: */
		*(sha2_word64*)&context->buffer[SHA256_SHORT_BLOCK_LENGTH] = context->bitcount;

		/* Final transform: */
		SHA256_Transform(context, (sha2_word32*)context->buffer);

#if BYTE_ORDER == LITTLE_ENDIAN
		{
			/* Convert TO host byte order */
			int	j;
			for (j = 0; j < 8; j++) {
				REVERSE32(context->state[j],context->state[j]);
				*d++ = context->state[j];
			}
		}
#else
		MEMCPY_BCOPY(d, context->state, SHA256_DIGEST_LENGTH);
#endif
	}

	/* Clean up state data: */
	MEMSET_BZERO(context, sizeof(SHA256_CTX));
	usedspace = 0;
}

char *SHA256_End(SHA256_CTX* context, char buffer[]) 
{
	sha2_byte	digest[SHA256_DIGEST_LENGTH], *d = digest;
	int		i;

	/* Sanity check: */
	assert(context != (SHA256_CTX*)0);

	if (buffer != (char*)0) {
		SHA256_Final(digest, context);

		for (i = 0; i < SHA256_DIGEST_LENGTH; i++) {
			*buffer++ = sha2_hex_digits[(*d & 0xf0) >> 4];
			*buffer++ = sha2_hex_digits[*d & 0x0f];
			d++;
		}
		*buffer = (char)0;
	} else {
		MEMSET_BZERO(context, sizeof(SHA256_CTX));
	}
	MEMSET_BZERO(digest, SHA256_DIGEST_LENGTH);
	return buffer;
}


char* SHA256_Data(const sha2_byte* data, size_t len, char digest[SHA256_DIGEST_STRING_LENGTH]) 
{
	SHA256_CTX	context;

	SHA256_Init(&context);
	SHA256_Update(&context, data, len);
	return SHA256_End(&context, digest);
}


/*** SHA-512: *********************************************************/
void SHA512_Init(SHA512_CTX* context) {
	if (context == (SHA512_CTX*)0) {
		return;
	}
	MEMCPY_BCOPY(context->state, sha512_initial_hash_value, SHA512_DIGEST_LENGTH);
	MEMSET_BZERO(context->buffer, SHA512_BLOCK_LENGTH);
	context->bitcount[0] = context->bitcount[1] =  0;
}

#ifdef SHA2_UNROLL_TRANSFORM

/* Unrolled SHA-512 round macros: */
#if BYTE_ORDER == LITTLE_ENDIAN

#define ROUND512_0_TO_15(a,b,c,d,e,f,g,h)	\
	REVERSE64(*data++, W512[j]); \
	T1 = (h) + Sigma1_512(e) + Ch((e), (f), (g)) + \
             K512[j] + W512[j]; \
	(d) += T1, \
	(h) = T1 + Sigma0_512(a) + Maj((a), (b), (c)), \
	j++


#else /* BYTE_ORDER == LITTLE_ENDIAN */

#define ROUND512_0_TO_15(a,b,c,d,e,f,g,h)	\
	T1 = (h) + Sigma1_512(e) + Ch((e), (f), (g)) + \
             K512[j] + (W512[j] = *data++); \
	(d) += T1; \
	(h) = T1 + Sigma0_512(a) + Maj((a), (b), (c)); \
	j++

#endif /* BYTE_ORDER == LITTLE_ENDIAN */

#define ROUND512(a,b,c,d,e,f,g,h)	\
	s0 = W512[(j+1)&0x0f]; \
	s0 = sigma0_512(s0); \
	s1 = W512[(j+14)&0x0f]; \
	s1 = sigma1_512(s1); \
	T1 = (h) + Sigma1_512(e) + Ch((e), (f), (g)) + K512[j] + \
             (W512[j&0x0f] += s1 + W512[(j+9)&0x0f] + s0); \
	(d) += T1; \
	(h) = T1 + Sigma0_512(a) + Maj((a), (b), (c)); \
	j++



void SHA512_Transform(SHA512_CTX* context, const sha2_word64* data) 
{
	sha2_word64	a, b, c, d, e, f, g, h, s0, s1;
	sha2_word64	T1, *W512 = (sha2_word64*)context->buffer;
	int		j;

	/* Initialize registers with the prev. intermediate value */
	a = context->state[0];
	b = context->state[1];
	c = context->state[2];
	d = context->state[3];
	e = context->state[4];
	f = context->state[5];
	g = context->state[6];
	h = context->state[7];

	j = 0;
	do {
		ROUND512_0_TO_15(a,b,c,d,e,f,g,h);
		ROUND512_0_TO_15(h,a,b,c,d,e,f,g);
		ROUND512_0_TO_15(g,h,a,b,c,d,e,f);
		ROUND512_0_TO_15(f,g,h,a,b,c,d,e);
		ROUND512_0_TO_15(e,f,g,h,a,b,c,d);
		ROUND512_0_TO_15(d,e,f,g,h,a,b,c);
		ROUND512_0_TO_15(c,d,e,f,g,h,a,b);
		ROUND512_0_TO_15(b,c,d,e,f,g,h,a);
	} while (j < 16);

	/* Now for the remaining rounds up to 79: */
	do {
		ROUND512(a,b,c,d,e,f,g,h);
		ROUND512(h,a,b,c,d,e,f,g);
		ROUND512(g,h,a,b,c,d,e,f);
		ROUND512(f,g,h,a,b,c,d,e);
		ROUND512(e,f,g,h,a,b,c,d);
		ROUND512(d,e,f,g,h,a,b,c);
		ROUND512(c,d,e,f,g,h,a,b);
		ROUND512(b,c,d,e,f,g,h,a);
	} while (j < 80);

	/* Compute the current intermediate hash value */
	context->state[0] += a;
	context->state[1] += b;
	context->state[2] += c;
	context->state[3] += d;
	context->state[4] += e;
	context->state[5] += f;
	context->state[6] += g;
	context->state[7] += h;

	/* Clean up */
	a = b = c = d = e = f = g = h = T1 = 0;
}

#else /* SHA2_UNROLL_TRANSFORM */

void SHA512_Transform(SHA512_CTX* context, const sha2_word64* data) 
{
	sha2_word64	a, b, c, d, e, f, g, h, s0, s1;
	sha2_word64	T1, T2, *W512 = (sha2_word64*)context->buffer;
	int		j;

	/* Initialize registers with the prev. intermediate value */
	a = context->state[0];
	b = context->state[1];
	c = context->state[2];
	d = context->state[3];
	e = context->state[4];
	f = context->state[5];
	g = context->state[6];
	h = context->state[7];

	j = 0;
	do {
#if BYTE_ORDER == LITTLE_ENDIAN
		/* Convert TO host byte order */
		REVERSE64(*data++, W512[j]);
		/* Apply the SHA-512 compression function to update a..h */
		T1 = h + Sigma1_512(e) + Ch(e, f, g) + K512[j] + W512[j];
#else /* BYTE_ORDER == LITTLE_ENDIAN */
		/* Apply the SHA-512 compression function to update a..h with copy */
		T1 = h + Sigma1_512(e) + Ch(e, f, g) + K512[j] + (W512[j] = *data++);
#endif /* BYTE_ORDER == LITTLE_ENDIAN */
		T2 = Sigma0_512(a) + Maj(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + T1;
		d = c;
		c = b;
		b = a;
		a = T1 + T2;

		j++;
	} while (j < 16);

	do {
		/* Part of the message block expansion: */
		s0 = W512[(j+1)&0x0f];
		s0 = sigma0_512(s0);
		s1 = W512[(j+14)&0x0f];
		s1 =  sigma1_512(s1);

		/* Apply the SHA-512 compression function to update a..h */
		T1 = h + Sigma1_512(e) + Ch(e, f, g) + K512[j] +
		     (W512[j&0x0f] += s1 + W512[(j+9)&0x0f] + s0);
		T2 = Sigma0_512(a) + Maj(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + T1;
		d = c;
		c = b;
		b = a;
		a = T1 + T2;

		j++;
	} while (j < 80);

	/* Compute the current intermediate hash value */
	context->state[0] += a;
	context->state[1] += b;
	context->state[2] += c;
	context->state[3] += d;
	context->state[4] += e;
	context->state[5] += f;
	context->state[6] += g;
	context->state[7] += h;

	/* Clean up */
	a = b = c = d = e = f = g = h = T1 = T2 = 0;
}

#endif /* SHA2_UNROLL_TRANSFORM */

void SHA512_Update(SHA512_CTX* context, const sha2_byte *data, size_t len) 
{
	unsigned int	freespace, usedspace;

	if (len == 0) {
		/* Calling with no data is valid - we do nothing */
		return;
	}

	/* Sanity check: */
	assert(context != (SHA512_CTX*)0 && data != (sha2_byte*)0);

	usedspace = (context->bitcount[0] >> 3) % SHA512_BLOCK_LENGTH;
	if (usedspace > 0) {
		/* Calculate how much free space is available in the buffer */
		freespace = SHA512_BLOCK_LENGTH - usedspace;

		if (len >= freespace) {
			/* Fill the buffer completely and process it */
			MEMCPY_BCOPY(&context->buffer[usedspace], data, freespace);
			ADDINC128(context->bitcount, freespace << 3);
			len -= freespace;
			data += freespace;
			SHA512_Transform(context, (sha2_word64*)context->buffer);
		} else {
			/* The buffer is not yet full */
			MEMCPY_BCOPY(&context->buffer[usedspace], data, len);
			ADDINC128(context->bitcount, len << 3);
			/* Clean up: */
			usedspace = freespace = 0;
			return;
		}
	}
	while (len >= SHA512_BLOCK_LENGTH) {
		/* Process as many complete blocks as we can */
		SHA512_Transform(context, (sha2_word64*)data);
		ADDINC128(context->bitcount, SHA512_BLOCK_LENGTH << 3);
		len -= SHA512_BLOCK_LENGTH;
		data += SHA512_BLOCK_LENGTH;
	}
	if (len > 0) {
		/* There's left-overs, so save 'em */
		MEMCPY_BCOPY(context->buffer, data, len);
		ADDINC128(context->bitcount, len << 3);
	}
	/* Clean up: */
	usedspace = freespace = 0;
}

void SHA512_Last(SHA512_CTX* context) 
{
	unsigned int	usedspace;

	usedspace = (context->bitcount[0] >> 3) % SHA512_BLOCK_LENGTH;
#if BYTE_ORDER == LITTLE_ENDIAN
	/* Convert FROM host byte order */
	REVERSE64(context->bitcount[0],context->bitcount[0]);
	REVERSE64(context->bitcount[1],context->bitcount[1]);
#endif
	if (usedspace > 0) {
		/* Begin padding with a 1 bit: */
		context->buffer[usedspace++] = 0x80;

		if (usedspace <= SHA512_SHORT_BLOCK_LENGTH) {
			/* Set-up for the last transform: */
			MEMSET_BZERO(&context->buffer[usedspace], SHA512_SHORT_BLOCK_LENGTH - usedspace);
		} else {
			if (usedspace < SHA512_BLOCK_LENGTH) {
				MEMSET_BZERO(&context->buffer[usedspace], SHA512_BLOCK_LENGTH - usedspace);
			}
			/* Do second-to-last transform: */
			SHA512_Transform(context, (sha2_word64*)context->buffer);

			/* And set-up for the last transform: */
			MEMSET_BZERO(context->buffer, SHA512_BLOCK_LENGTH - 2);
		}
	} else {
		/* Prepare for final transform: */
		MEMSET_BZERO(context->buffer, SHA512_SHORT_BLOCK_LENGTH);

		/* Begin padding with a 1 bit: */
		*context->buffer = 0x80;
	}
	/* Store the length of input data (in bits): */
	*(sha2_word64*)&context->buffer[SHA512_SHORT_BLOCK_LENGTH] = context->bitcount[1];
	*(sha2_word64*)&context->buffer[SHA512_SHORT_BLOCK_LENGTH+8] = context->bitcount[0];

	/* Final transform: */
	SHA512_Transform(context, (sha2_word64*)context->buffer);
}

void SHA512_Final(sha2_byte digest[], SHA512_CTX* context) 
{
	sha2_word64	*d = (sha2_word64*)digest;

	/* Sanity check: */
	assert(context != (SHA512_CTX*)0);

	/* If no digest buffer is passed, we don't bother doing this: */
	if (digest != (sha2_byte*)0) {
		SHA512_Last(context);

		/* Save the hash data for output: */
#if BYTE_ORDER == LITTLE_ENDIAN
		{
			/* Convert TO host byte order */
			int	j;
			for (j = 0; j < 8; j++) {
				REVERSE64(context->state[j],context->state[j]);
				*d++ = context->state[j];
			}
		}
#else
		MEMCPY_BCOPY(d, context->state, SHA512_DIGEST_LENGTH);
#endif
	}

	/* Zero out state data */
	MEMSET_BZERO(context, sizeof(SHA512_CTX));
}

char *SHA512_End(SHA512_CTX* context, char buffer[]) 
{
	sha2_byte	digest[SHA512_DIGEST_LENGTH], *d = digest;
	int		i;

	/* Sanity check: */
	assert(context != (SHA512_CTX*)0);

	if (buffer != (char*)0) {
		SHA512_Final(digest, context);

		for (i = 0; i < SHA512_DIGEST_LENGTH; i++) {
			*buffer++ = sha2_hex_digits[(*d & 0xf0) >> 4];
			*buffer++ = sha2_hex_digits[*d & 0x0f];
			d++;
		}
		*buffer = (char)0;
	} else {
		MEMSET_BZERO(context, sizeof(SHA512_CTX));
	}
	MEMSET_BZERO(digest, SHA512_DIGEST_LENGTH);
	return buffer;
}

char* SHA512_Data(const sha2_byte* data, size_t len, char digest[SHA512_DIGEST_STRING_LENGTH]) 
{
	SHA512_CTX	context;

	SHA512_Init(&context);
	SHA512_Update(&context, data, len);
	return SHA512_End(&context, digest);
}


/*** SHA-384: *********************************************************/
void SHA384_Init(SHA384_CTX* context) 
{
	if (context == (SHA384_CTX*)0) {
		return;
	}
	MEMCPY_BCOPY(context->state, sha384_initial_hash_value, SHA512_DIGEST_LENGTH);
	MEMSET_BZERO(context->buffer, SHA384_BLOCK_LENGTH);
	context->bitcount[0] = context->bitcount[1] = 0;
}

void SHA384_Update(SHA384_CTX* context, const sha2_byte* data, size_t len) 
{
	SHA512_Update((SHA512_CTX*)context, data, len);
}

void SHA384_Final(sha2_byte digest[], SHA384_CTX* context) 
{
	sha2_word64	*d = (sha2_word64*)digest;

	/* Sanity check: */
	assert(context != (SHA384_CTX*)0);

	/* If no digest buffer is passed, we don't bother doing this: */
	if (digest != (sha2_byte*)0) {
		SHA512_Last((SHA512_CTX*)context);

		/* Save the hash data for output: */
#if BYTE_ORDER == LITTLE_ENDIAN
		{
			/* Convert TO host byte order */
			int	j;
			for (j = 0; j < 6; j++) {
				REVERSE64(context->state[j],context->state[j]);
				*d++ = context->state[j];
			}
		}
#else
		MEMCPY_BCOPY(d, context->state, SHA384_DIGEST_LENGTH);
#endif
	}

	/* Zero out state data */
	MEMSET_BZERO(context, sizeof(SHA384_CTX));
}

char *SHA384_End(SHA384_CTX* context, char buffer[]) 
{
	sha2_byte	digest[SHA384_DIGEST_LENGTH], *d = digest;
	int		i;

	/* Sanity check: */
	assert(context != (SHA384_CTX*)0);

	if (buffer != (char*)0) {
		SHA384_Final(digest, context);

		for (i = 0; i < SHA384_DIGEST_LENGTH; i++) {
			*buffer++ = sha2_hex_digits[(*d & 0xf0) >> 4];
			*buffer++ = sha2_hex_digits[*d & 0x0f];
			d++;
		}
		*buffer = (char)0;
	} else {
		MEMSET_BZERO(context, sizeof(SHA384_CTX));
	}
	MEMSET_BZERO(digest, SHA384_DIGEST_LENGTH);
	return buffer;
}

char* SHA384_Data(const sha2_byte* data, size_t len, char digest[SHA384_DIGEST_STRING_LENGTH]) 
{
	SHA384_CTX	context;

	SHA384_Init(&context);
	SHA384_Update(&context, data, len);
	return SHA384_End(&context, digest);
}
#endif /// SHA2_256_384_512





#if SHA2_224_CHECKSUM


#define SHFR(x, n)    (x >> n)
#define ROTR(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define ROTL(x, n)   ((x << n) | (x >> ((sizeof(x) << 3) - n)))
#define CH(x, y, z)  ((x & y) ^ (~x & z))
#define MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))

#define SHA256_F1(x) (ROTR(x,  2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define SHA256_F2(x) (ROTR(x,  6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SHA256_F3(x) (ROTR(x,  7) ^ ROTR(x, 18) ^ SHFR(x,  3))
#define SHA256_F4(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHFR(x, 10))


#define SHA256_SCR(i)                         \
{                                             \
    w[i] =  SHA256_F4(w[i -  2]) + w[i -  7]  \
          + SHA256_F3(w[i - 15]) + w[i - 16]; \
}


#define SHA256_EXP(a, b, c, d, e, f, g, h, j)               \
{                                                           \
    t1 = wv[h] + SHA256_F2(wv[e]) + CH(wv[e], wv[f], wv[g]) \
         + sha256_k[j] + w[j];                              \
    t2 = SHA256_F1(wv[a]) + MAJ(wv[a], wv[b], wv[c]);       \
    wv[d] += t1;                                            \
    wv[h] = t1 + t2;                                        \
}


#define UNPACK32(x, str)                      \
{                                             \
    *((str) + 3) = (uint8) ((x)      );       \
    *((str) + 2) = (uint8) ((x) >>  8);       \
    *((str) + 1) = (uint8) ((x) >> 16);       \
    *((str) + 0) = (uint8) ((x) >> 24);       \
}

#define PACK32(str, x)                        \
{                                             \
    *(x) =   ((uint32) *((str) + 3)      )    \
           | ((uint32) *((str) + 2) <<  8)    \
           | ((uint32) *((str) + 1) << 16)    \
           | ((uint32) *((str) + 0) << 24);   \
}

const uint32 sha224_h0[8] =
            {0xc1059ed8, 0x367cd507, 0x3070dd17, 0xf70e5939,
             0xffc00b31, 0x68581511, 0x64f98fa7, 0xbefa4fa4};

const uint32 sha256_k[64] =
            {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
             0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
             0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
             0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
             0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
             0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
             0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
             0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
             0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
             0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
             0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
             0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
             0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
             0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
             0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
             0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};


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


typedef sha256_ctx sha224_ctx;



void sha256_transf(sha256_ctx *ctx, const unsigned char *message, unsigned int block_nb)
{
    uint32 w[64];
    uint32 wv[8];
    uint32 sha224_t1, sha224_t2;
    const unsigned char *sub_block;
    int i;

#ifndef UNROLL_LOOPS
    int j;
#endif

    for (i = 0; i < (int) block_nb; i++) {
        sub_block = message + (i << 6);

#ifndef UNROLL_LOOPS
        for (j = 0; j < 16; j++) {
            PACK32(&sub_block[j << 2], &w[j]);
        }

        for (j = 16; j < 64; j++) {
            SHA256_SCR(j);
        }

        for (j = 0; j < 8; j++) {
            wv[j] = ctx->h[j];
        }

        for (j = 0; j < 64; j++) {
            sha224_t1 = wv[7] + SHA256_F2(wv[4]) + CH(wv[4], wv[5], wv[6])
                + sha256_k[j] + w[j];
            sha224_t2 = SHA256_F1(wv[0]) + MAJ(wv[0], wv[1], wv[2]);
            wv[7] = wv[6];
            wv[6] = wv[5];
            wv[5] = wv[4];
            wv[4] = wv[3] + sha224_t1;
            wv[3] = wv[2];
            wv[2] = wv[1];
            wv[1] = wv[0];
            wv[0] = sha224_t1 + sha224_t2;
        }

        for (j = 0; j < 8; j++) {
            ctx->h[j] += wv[j];
        }
#else
        PACK32(&sub_block[ 0], &w[ 0]); PACK32(&sub_block[ 4], &w[ 1]);
        PACK32(&sub_block[ 8], &w[ 2]); PACK32(&sub_block[12], &w[ 3]);
        PACK32(&sub_block[16], &w[ 4]); PACK32(&sub_block[20], &w[ 5]);
        PACK32(&sub_block[24], &w[ 6]); PACK32(&sub_block[28], &w[ 7]);
        PACK32(&sub_block[32], &w[ 8]); PACK32(&sub_block[36], &w[ 9]);
        PACK32(&sub_block[40], &w[10]); PACK32(&sub_block[44], &w[11]);
        PACK32(&sub_block[48], &w[12]); PACK32(&sub_block[52], &w[13]);
        PACK32(&sub_block[56], &w[14]); PACK32(&sub_block[60], &w[15]);

        SHA256_SCR(16); SHA256_SCR(17); SHA256_SCR(18); SHA256_SCR(19);
        SHA256_SCR(20); SHA256_SCR(21); SHA256_SCR(22); SHA256_SCR(23);
        SHA256_SCR(24); SHA256_SCR(25); SHA256_SCR(26); SHA256_SCR(27);
        SHA256_SCR(28); SHA256_SCR(29); SHA256_SCR(30); SHA256_SCR(31);
        SHA256_SCR(32); SHA256_SCR(33); SHA256_SCR(34); SHA256_SCR(35);
        SHA256_SCR(36); SHA256_SCR(37); SHA256_SCR(38); SHA256_SCR(39);
        SHA256_SCR(40); SHA256_SCR(41); SHA256_SCR(42); SHA256_SCR(43);
        SHA256_SCR(44); SHA256_SCR(45); SHA256_SCR(46); SHA256_SCR(47);
        SHA256_SCR(48); SHA256_SCR(49); SHA256_SCR(50); SHA256_SCR(51);
        SHA256_SCR(52); SHA256_SCR(53); SHA256_SCR(54); SHA256_SCR(55);
        SHA256_SCR(56); SHA256_SCR(57); SHA256_SCR(58); SHA256_SCR(59);
        SHA256_SCR(60); SHA256_SCR(61); SHA256_SCR(62); SHA256_SCR(63);

        wv[0] = ctx->h[0]; wv[1] = ctx->h[1];
        wv[2] = ctx->h[2]; wv[3] = ctx->h[3];
        wv[4] = ctx->h[4]; wv[5] = ctx->h[5];
        wv[6] = ctx->h[6]; wv[7] = ctx->h[7];

        SHA256_EXP(0,1,2,3,4,5,6,7, 0); SHA256_EXP(7,0,1,2,3,4,5,6, 1);
        SHA256_EXP(6,7,0,1,2,3,4,5, 2); SHA256_EXP(5,6,7,0,1,2,3,4, 3);
        SHA256_EXP(4,5,6,7,0,1,2,3, 4); SHA256_EXP(3,4,5,6,7,0,1,2, 5);
        SHA256_EXP(2,3,4,5,6,7,0,1, 6); SHA256_EXP(1,2,3,4,5,6,7,0, 7);
        SHA256_EXP(0,1,2,3,4,5,6,7, 8); SHA256_EXP(7,0,1,2,3,4,5,6, 9);
        SHA256_EXP(6,7,0,1,2,3,4,5,10); SHA256_EXP(5,6,7,0,1,2,3,4,11);
        SHA256_EXP(4,5,6,7,0,1,2,3,12); SHA256_EXP(3,4,5,6,7,0,1,2,13);
        SHA256_EXP(2,3,4,5,6,7,0,1,14); SHA256_EXP(1,2,3,4,5,6,7,0,15);
        SHA256_EXP(0,1,2,3,4,5,6,7,16); SHA256_EXP(7,0,1,2,3,4,5,6,17);
        SHA256_EXP(6,7,0,1,2,3,4,5,18); SHA256_EXP(5,6,7,0,1,2,3,4,19);
        SHA256_EXP(4,5,6,7,0,1,2,3,20); SHA256_EXP(3,4,5,6,7,0,1,2,21);
        SHA256_EXP(2,3,4,5,6,7,0,1,22); SHA256_EXP(1,2,3,4,5,6,7,0,23);
        SHA256_EXP(0,1,2,3,4,5,6,7,24); SHA256_EXP(7,0,1,2,3,4,5,6,25);
        SHA256_EXP(6,7,0,1,2,3,4,5,26); SHA256_EXP(5,6,7,0,1,2,3,4,27);
        SHA256_EXP(4,5,6,7,0,1,2,3,28); SHA256_EXP(3,4,5,6,7,0,1,2,29);
        SHA256_EXP(2,3,4,5,6,7,0,1,30); SHA256_EXP(1,2,3,4,5,6,7,0,31);
        SHA256_EXP(0,1,2,3,4,5,6,7,32); SHA256_EXP(7,0,1,2,3,4,5,6,33);
        SHA256_EXP(6,7,0,1,2,3,4,5,34); SHA256_EXP(5,6,7,0,1,2,3,4,35);
        SHA256_EXP(4,5,6,7,0,1,2,3,36); SHA256_EXP(3,4,5,6,7,0,1,2,37);
        SHA256_EXP(2,3,4,5,6,7,0,1,38); SHA256_EXP(1,2,3,4,5,6,7,0,39);
        SHA256_EXP(0,1,2,3,4,5,6,7,40); SHA256_EXP(7,0,1,2,3,4,5,6,41);
        SHA256_EXP(6,7,0,1,2,3,4,5,42); SHA256_EXP(5,6,7,0,1,2,3,4,43);
        SHA256_EXP(4,5,6,7,0,1,2,3,44); SHA256_EXP(3,4,5,6,7,0,1,2,45);
        SHA256_EXP(2,3,4,5,6,7,0,1,46); SHA256_EXP(1,2,3,4,5,6,7,0,47);
        SHA256_EXP(0,1,2,3,4,5,6,7,48); SHA256_EXP(7,0,1,2,3,4,5,6,49);
        SHA256_EXP(6,7,0,1,2,3,4,5,50); SHA256_EXP(5,6,7,0,1,2,3,4,51);
        SHA256_EXP(4,5,6,7,0,1,2,3,52); SHA256_EXP(3,4,5,6,7,0,1,2,53);
        SHA256_EXP(2,3,4,5,6,7,0,1,54); SHA256_EXP(1,2,3,4,5,6,7,0,55);
        SHA256_EXP(0,1,2,3,4,5,6,7,56); SHA256_EXP(7,0,1,2,3,4,5,6,57);
        SHA256_EXP(6,7,0,1,2,3,4,5,58); SHA256_EXP(5,6,7,0,1,2,3,4,59);
        SHA256_EXP(4,5,6,7,0,1,2,3,60); SHA256_EXP(3,4,5,6,7,0,1,2,61);
        SHA256_EXP(2,3,4,5,6,7,0,1,62); SHA256_EXP(1,2,3,4,5,6,7,0,63);

        ctx->h[0] += wv[0]; ctx->h[1] += wv[1];
        ctx->h[2] += wv[2]; ctx->h[3] += wv[3];
        ctx->h[4] += wv[4]; ctx->h[5] += wv[5];
        ctx->h[6] += wv[6]; ctx->h[7] += wv[7];
#endif /* !UNROLL_LOOPS */
    }
}


void sha224_init(sha224_ctx *ctx)
{
#ifndef UNROLL_LOOPS
    int i;
    for (i = 0; i < 8; i++) {
        ctx->h[i] = sha224_h0[i];
    }
#else
    ctx->h[0] = sha224_h0[0]; 
	ctx->h[1] = sha224_h0[1];
    ctx->h[2] = sha224_h0[2]; 
	ctx->h[3] = sha224_h0[3];
    ctx->h[4] = sha224_h0[4]; 
	ctx->h[5] = sha224_h0[5];
    ctx->h[6] = sha224_h0[6]; 
	ctx->h[7] = sha224_h0[7];
#endif /* !UNROLL_LOOPS */

    ctx->len = 0;
    ctx->tot_len = 0;
}

void sha224_update(sha224_ctx *ctx, const unsigned char *message, unsigned int len)
{
    unsigned int block_nb;
    unsigned int new_len, rem_len, tmp_len;
    const unsigned char *shifted_message;

    tmp_len = SHA224_BLOCK_SIZE - ctx->len;
    rem_len = len < tmp_len ? len : tmp_len;

    memcpy(&ctx->block[ctx->len], message, rem_len);

    if (ctx->len + len < SHA224_BLOCK_SIZE) {
        ctx->len += len;
        return;
    }

    new_len = len - rem_len;
    block_nb = new_len / SHA224_BLOCK_SIZE;

    shifted_message = message + rem_len;

    sha256_transf(ctx, ctx->block, 1);
    sha256_transf(ctx, shifted_message, block_nb);

    rem_len = new_len % SHA224_BLOCK_SIZE;

    memcpy(ctx->block, &shifted_message[block_nb << 6],
           rem_len);

    ctx->len = rem_len;
    ctx->tot_len += (block_nb + 1) << 6;
}

void sha224_final(sha224_ctx *ctx, unsigned char *digest)
{
    unsigned int block_nb;
    unsigned int pm_len;
    unsigned int len_b;

#ifndef UNROLL_LOOPS
    int i;
#endif

    block_nb = (1 + ((SHA224_BLOCK_SIZE - 9)
                     < (ctx->len % SHA224_BLOCK_SIZE)));

    len_b = (ctx->tot_len + ctx->len) << 3;
    pm_len = block_nb << 6;

    memset(ctx->block + ctx->len, 0, pm_len - ctx->len);
    ctx->block[ctx->len] = 0x80;
    UNPACK32(len_b, ctx->block + pm_len - 4);

    sha256_transf(ctx, ctx->block, block_nb);

#ifndef UNROLL_LOOPS
    for (i = 0 ; i < 7; i++) {
        UNPACK32(ctx->h[i], &digest[i << 2]);
    }
#else
   UNPACK32(ctx->h[0], &digest[ 0]);
   UNPACK32(ctx->h[1], &digest[ 4]);
   UNPACK32(ctx->h[2], &digest[ 8]);
   UNPACK32(ctx->h[3], &digest[12]);
   UNPACK32(ctx->h[4], &digest[16]);
   UNPACK32(ctx->h[5], &digest[20]);
   UNPACK32(ctx->h[6], &digest[24]);
#endif /* !UNROLL_LOOPS */
}


static void sha224Print (unsigned char *sha224_digest)
{
  int i;

  for (i = 0; i < SHA224_DIGEST_SIZE; i++)
  {
    printf ("%02x", sha224_digest[i]);
	if(outfile) fprintf(outfile, "%02x", sha224_digest[i]);
  }
}


/* SHA-224 functions */
void sha224(const unsigned char *message, unsigned int len, unsigned char *digest)
{
    sha224_ctx ctx;

    sha224_init(&ctx);
    sha224_update(&ctx, message, len);
    sha224_final(&ctx, digest);
}
#endif /// SHA2_224_CHECKSUM -------------



#if SHA3_KECCAK_224_256_384_512

#define KECCAK_SPONGE_BIT		1600
#define KECCAK_ROUND			24
#define KECCAK_STATE_SIZE		200

#define KECCAK_SHA3_224			224
#define KECCAK_SHA3_256			256
#define KECCAK_SHA3_384			384
#define KECCAK_SHA3_512			512

#define KECCAK_SHAKE128			128
#define KECCAK_SHAKE256			256

#define KECCAK_SHA3_SUFFIX		0x06
#define KECCAK_SHAKE_SUFFIX		0x1F


#define SHA3_224_HASH_BIT 		224
#define SHA3_256_HASH_BIT 		256
#define SHA3_384_HASH_BIT 		384
#define SHA3_512_HASH_BIT 		512

#define SHA3_OUT_LENGTH_DIV 	8

#define SHA3_OUT_224 			(SHA3_224_HASH_BIT / SHA3_OUT_LENGTH_DIV)
#define SHA3_OUT_256 			(SHA3_256_HASH_BIT / SHA3_OUT_LENGTH_DIV)
#define SHA3_OUT_384 			(SHA3_384_HASH_BIT / SHA3_OUT_LENGTH_DIV)
#define SHA3_OUT_512 			(SHA3_512_HASH_BIT / SHA3_OUT_LENGTH_DIV)

#define SHAKE_OUT_128 			(256/8)
#define SHAKE_OUT_256 			(512/8)
#define SHAKE_128_HASH_BIT 		128
#define SHAKE_256_HASH_BIT 		256

#define SHA3_OUTPUT_SIZ 		512

#define SHA3_BUFLEN 			(100*1024) // (1024*16)


typedef enum
{
	SHA3_OK = 0,
	SHA3_PARAMETER_ERROR = 1,
} SHA3_RETRUN;


typedef enum
{
	SHA3_SHAKE_NONE = 0,
	SHA3_SHAKE_USE = 1,
} SHA3_USE_SHAKE;



static unsigned int keccakRate = 0;
static unsigned int keccakCapacity = 0;
static unsigned int keccakSuffix = 0;

static uint8_t keccak_state[KECCAK_STATE_SIZE] = { 0x00, };
static int end_offset;

static const uint32_t keccakf_rndc[KECCAK_ROUND][2] =
{
	{0x00000001, 0x00000000}, {0x00008082, 0x00000000},
	{0x0000808a, 0x80000000}, {0x80008000, 0x80000000},
	{0x0000808b, 0x00000000}, {0x80000001, 0x00000000},
	{0x80008081, 0x80000000}, {0x00008009, 0x80000000},
	{0x0000008a, 0x00000000}, {0x00000088, 0x00000000},
	{0x80008009, 0x00000000}, {0x8000000a, 0x00000000},

	{0x8000808b, 0x00000000}, {0x0000008b, 0x80000000},
	{0x00008089, 0x80000000}, {0x00008003, 0x80000000},
	{0x00008002, 0x80000000}, {0x00000080, 0x80000000},
	{0x0000800a, 0x00000000}, {0x8000000a, 0x80000000},
	{0x80008081, 0x80000000}, {0x00008080, 0x80000000},
	{0x80000001, 0x00000000}, {0x80008008, 0x80000000}
};

static const unsigned keccakf_rotc[KECCAK_ROUND] =
{
	 1,  3,  6, 10, 15, 21, 28, 36, 45, 55,  2, 14,
	27, 41, 56,  8, 25, 43, 62, 18, 39, 61, 20, 44
};

static const unsigned keccakf_piln[KECCAK_ROUND] =
{
	10,  7, 11, 17, 18,  3,  5, 16,  8, 21, 24,  4,
	15, 23, 19, 13, 12,  2, 20, 14, 22,  9,  6,  1
};


void ROL64(uint32_t* in, uint32_t* out, int offset)
{
	int shift = 0;

	if (offset == 0)
	{
		out[1] = in[1];
		out[0] = in[0];
	}
	else if (offset < 32)
	{
		shift = offset;

		out[1] = (uint32_t)((in[1] << shift) ^ (in[0] >> (32 - shift)));
		out[0] = (uint32_t)((in[0] << shift) ^ (in[1] >> (32 - shift)));
	}
	else if (offset < 64)
	{
		shift = offset - 32;

		out[1] = (uint32_t)((in[0] << shift) ^ (in[1] >> (32 - shift)));
		out[0] = (uint32_t)((in[1] << shift) ^ (in[0] >> (32 - shift)));
	}
	else
	{
		out[1] = in[1];
		out[0] = in[0];
	}
}


void keccakf(uint8_t* state)
{
	uint32_t t[2], bc[5][2], s[25][2] = { 0x00, };
	int i, j, round;

	for (i = 0; i < 25; i++)
	{
		s[i][0] = (uint32_t)(state[i * 8 + 0]) |
			(uint32_t)(state[i * 8 + 1] << 8) |
			(uint32_t)(state[i * 8 + 2] << 16) |
			(uint32_t)(state[i * 8 + 3] << 24);
		s[i][1] = (uint32_t)(state[i * 8 + 4]) |
			(uint32_t)(state[i * 8 + 5] << 8) |
			(uint32_t)(state[i * 8 + 6] << 16) |
			(uint32_t)(state[i * 8 + 7] << 24);
	}

	for (round = 0; round < KECCAK_ROUND; round++)
	{
		/* Theta */
		for (i = 0; i < 5; i++)
		{
			bc[i][0] = s[i][0] ^ s[i + 5][0] ^ s[i + 10][0] ^ s[i + 15][0] ^ s[i + 20][0];
			bc[i][1] = s[i][1] ^ s[i + 5][1] ^ s[i + 10][1] ^ s[i + 15][1] ^ s[i + 20][1];
		}

		for (i = 0; i < 5; i++)
		{
			ROL64(bc[(i + 1) % 5], t, 1);

			t[0] ^= bc[(i + 4) % 5][0];
			t[1] ^= bc[(i + 4) % 5][1];

			for (j = 0; j < 25; j += 5)
			{
				s[j + i][0] ^= t[0];
				s[j + i][1] ^= t[1];
			}
		}

		/* Rho & Pi */
		t[0] = s[1][0];
		t[1] = s[1][1];

		for (i = 0; i < KECCAK_ROUND; i++)
		{
			j = keccakf_piln[i];

			bc[0][0] = s[j][0];
			bc[0][1] = s[j][1];

			ROL64(t, s[j], keccakf_rotc[i]);

			t[0] = bc[0][0];
			t[1] = bc[0][1];
		}

		/* Chi */
		for (j = 0; j < 25; j += 5)
		{
			for (i = 0; i < 5; i++)
			{
				bc[i][0] = s[j + i][0];
				bc[i][1] = s[j + i][1];
			}

			for (i = 0; i < 5; i++)
			{
				s[j + i][0] ^= (~bc[(i + 1) % 5][0]) & bc[(i + 2) % 5][0];
				s[j + i][1] ^= (~bc[(i + 1) % 5][1]) & bc[(i + 2) % 5][1];
			}
		}

		/* Iota */
		s[0][0] ^= keccakf_rndc[round][0];
		s[0][1] ^= keccakf_rndc[round][1];
	}

	for (i = 0; i < 25; i++)
	{
		state[i * 8 + 0] = (uint8_t)(s[i][0]);
		state[i * 8 + 1] = (uint8_t)(s[i][0] >> 8);
		state[i * 8 + 2] = (uint8_t)(s[i][0] >> 16);
		state[i * 8 + 3] = (uint8_t)(s[i][0] >> 24);
		state[i * 8 + 4] = (uint8_t)(s[i][1]);
		state[i * 8 + 5] = (uint8_t)(s[i][1] >> 8);
		state[i * 8 + 6] = (uint8_t)(s[i][1] >> 16);
		state[i * 8 + 7] = (uint8_t)(s[i][1] >> 24);
	}
}


int keccak_absorb(uint8_t* input, int inLen, int rate, int capacity)
{
	uint8_t* buf = input;
	int iLen = inLen;
	int rateInBytes = rate / 8;
	int blockSize = 0;
	int i = 0;

	if ((rate + capacity) != KECCAK_SPONGE_BIT)
		return SHA3_PARAMETER_ERROR;

	if (((rate % 8) != 0) || (rate < 1))
		return SHA3_PARAMETER_ERROR;

	while (iLen > 0)
	{
		if ((end_offset != 0) && (end_offset < rateInBytes))
		{
			blockSize = (((iLen + end_offset) < rateInBytes) ? (iLen + end_offset) : rateInBytes);

			for (i = end_offset; i < blockSize; i++)
				keccak_state[i] ^= buf[i - end_offset];

			buf += blockSize - end_offset;
			iLen -= blockSize - end_offset;
		}
		else
		{
			blockSize = ((iLen < rateInBytes) ? iLen : rateInBytes);

			for (i = 0; i < blockSize; i++)
				keccak_state[i] ^= buf[i];

			buf += blockSize;
			iLen -= blockSize;
		}

		if (blockSize == rateInBytes)
		{
			keccakf(keccak_state);
			blockSize = 0;
		}

		end_offset = blockSize;
	}

	return SHA3_OK;
}


int keccak_squeeze(uint8_t* output, int outLen, int rate, int suffix)
{
	uint8_t* buf = output;
	int oLen = outLen;
	int rateInBytes = rate / 8;
	int blockSize = end_offset;
	int i = 0;

	keccak_state[blockSize] ^= suffix;

	if (((suffix & 0x80) != 0) && (blockSize == (rateInBytes - 1)))
		keccakf(keccak_state);

	keccak_state[rateInBytes - 1] ^= 0x80;

	keccakf(keccak_state);

	while (oLen > 0)
	{
		blockSize = ((oLen < rateInBytes) ? oLen : rateInBytes);
		for (i = 0; i < blockSize; i++)
			buf[i] = keccak_state[i];
		buf += blockSize;
		oLen -= blockSize;

		if (oLen > 0)
			keccakf(keccak_state);
	}

	return SHA3_OK;
}


void sha3_init(int bitSize, int useSHAKE)
{
	keccakCapacity = bitSize * 2;
	keccakRate = KECCAK_SPONGE_BIT - keccakCapacity;

	if (useSHAKE)
		keccakSuffix = KECCAK_SHAKE_SUFFIX;
	else
		keccakSuffix = KECCAK_SHA3_SUFFIX;

	memset(keccak_state, 0x00, KECCAK_STATE_SIZE);

	end_offset = 0;
}


int sha3_update(uint8_t* input, int inLen)
{
	return keccak_absorb(input, inLen, keccakRate, keccakCapacity);
}


int sha3_final(uint8_t* output, int outLen)
{
	int ret = 0;

	ret = keccak_squeeze(output, outLen, keccakRate, keccakSuffix);

	keccakRate = 0;
	keccakCapacity = 0;
	keccakSuffix = 0;

	memset(keccak_state, 0x00, KECCAK_STATE_SIZE);

	return ret;
}


int sha3_hash(uint8_t* output, int outLen, uint8_t* input, int inLen, int bitSize, int useSHAKE)
{
	int ret = 0;

	if (useSHAKE == SHA3_SHAKE_USE)
	{
		if ((bitSize != KECCAK_SHAKE128) && (bitSize != KECCAK_SHAKE256))
			return SHA3_PARAMETER_ERROR;

		sha3_init(bitSize, SHA3_SHAKE_USE);
	}
	else
	{
		if ((bitSize != KECCAK_SHA3_224) && (bitSize != KECCAK_SHA3_256) &&
			(bitSize != KECCAK_SHA3_384) && (bitSize != KECCAK_SHA3_512))
			return SHA3_PARAMETER_ERROR;

		if ((bitSize / 8) != outLen)
			return SHA3_PARAMETER_ERROR;

		sha3_init(bitSize, SHA3_SHAKE_NONE);
	}

	sha3_update(input, inLen);

	ret = sha3_final(output, outLen);

	return ret;
}
#endif // SHA3_KECCAK_224_256_384_512




/*--------------------------------------------------------------------------
                     INCLUDE FILES 
---------------------------------------------------------------------------*/



//#define DEBUG
#ifdef _MOT2BIN_
/* Size in bytes.  Max length must be in powers of 2: 1,2,4,8,16,32, etc. */
#define MOT2BIN_MEMORY_SIZE 		(4*1024*1024)  /// 4MBytes
#define MOT2BIN_ADDRESS_MASK 		(MOT2BIN_MEMORY_SIZE-1)

#define MOT2BIN_MAX_LINE_SIZE 		256
#endif

#ifdef _INTEL2BIN_

#define MAX_BIN_LENGTH_SIZE 		0x800000 // Max_Length 8MByte Limit

#define HEX_MAX_LINE_SIZE 			1024
#define MAX_ERR_COUNT 				50 // error count


/* Size in bytes.  Max length must be in powers of 2: 1,2,4,8,16,32, etc. */
#define INT2BIN_MEMORY_SIZE 		(4*1024*1024) /// 4MBytes

/* This mask is for mapping the target binary inside our
binary buffer. If for example, we are generating a binary
file with records starting at FFF00000, the bytes will be
stored at the beginning of the memory buffer. */
#define INT2BIN_ADDRESS_MASK 		(INT2BIN_MEMORY_SIZE-1)

/* The data records can contain 255 bytes: this means 512 characters. */
#define INT2BIN_MAX_LINE_SIZE 		1024

#define NO_ADDRESS_TYPE_SELECTED 	0
#define LINEAR_ADDRESS 			1
#define SEGMENTED_ADDRESS 		2
#endif


/*--------------------------------------------------------------------------
                     MACRO
---------------------------------------------------------------------------*/
#define swap(a) 		(((a)<<8)|((a)>>8))     


#define ONE_KILO_BYTE 		1024.0
#define ONE_MEGA_BYTE 		1024*1024
#define HALF_MEGA_BYTE 		512*1024
#define MAX_BUF_SIZ 		(50*(ONE_MEGA_BYTE)-HALF_MEGA_BYTE) /* NK.NB0 , EBOOT.NB0 size 변경시 수정후 building */

#define RD_BUF_SIZ 			1024*10

#define MULTI_IN_FILES_CNT 			(4096)
#define LENGTH_OF_FILENAME			16 /* 16*16 -> 256 */
#define MAX_CHARS 					16
#define MAX_VERSION_LEN 			16 // 2017.12.12
#define MAX_CRC_LEN_CODE 			16 // 2017.11.21
#define MAX_HASH_CODE 				128 // 2017.11.21

#define MAX_FILENAME_LEN 			256

#define SPACE_FILL1 		'\0'
#define SPACE_FILL2 		'\0' //0xFF
#define SPACE_FILL3 		'\0'
#define SPACE_FILL4 		'\0'

#define DUMMY_FILL 		"TOPJOOhdr"
#define DUMMY_FILL_LEN 		9

#define TRUE 		1
#define FALSE 		0


#define CHECK_BIT_IN 		0x01
#define CHECK_BIT_OUT 		0x02


#define ATT_VERSION 		0x0001
#define ATT_DATEorCRC 		0x0002
#define ATT_MODEL 			0x0004
#define ATT_BOARD 			0x0008


#define MONTH_LEN 	12
typedef struct {
	unsigned char 	mon[3];
	unsigned char   amon;	
} _month_table_ ;

_month_table_ month_table[MONTH_LEN] =
{
	"Jan", 'A',
	"Feb", 'B',
	"Mar", 'C',
	"Apr", 'D',
	"May", 'E',
	"Jun", 'F',
	"Jul", 'G',
	"Aug", 'H',
	"Sep", 'I',
	"Oct", 'J',
	"Nov", 'K',
	"Dec", 'L',
};




/* Redis uses the CRC64 variant with "Jones" coefficients and init value of 0.
 *
 * Specification of this CRC64 variant follows:
 * Name: crc-64-jones
 * Width: 64 bites
 * Poly: 0xad93d23594c935a9
 * Reflected In: True
 * Xor_In: 0xffffffffffffffff
 * Reflected_Out: True
 * Xor_Out: 0x0
 * Check("123456789"): 0xe9c6d914c4b8d9ca
 *
 * Copyright (c) 2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 */


/// typedef unsigned long long uint64_t;	/* 8-bytes (64-bits) */

static const uint64_t crc64_tab[256] = {
    0x0000000000000000ULL, 0x7ad870c830358979ULL,
    0xf5b0e190606b12f2ULL, 0x8f689158505e9b8bULL,
    0xc038e5739841b68fULL, 0xbae095bba8743ff6ULL,
    0x358804e3f82aa47dULL, 0x4f50742bc81f2d04ULL,
    0xab28ecb46814fe75ULL, 0xd1f09c7c5821770cULL,
    0x5e980d24087fec87ULL, 0x24407dec384a65feULL,
    0x6b1009c7f05548faULL, 0x11c8790fc060c183ULL,
    0x9ea0e857903e5a08ULL, 0xe478989fa00bd371ULL,
    0x7d08ff3b88be6f81ULL, 0x07d08ff3b88be6f8ULL,
    0x88b81eabe8d57d73ULL, 0xf2606e63d8e0f40aULL,
    0xbd301a4810ffd90eULL, 0xc7e86a8020ca5077ULL,
    0x4880fbd87094cbfcULL, 0x32588b1040a14285ULL,
    0xd620138fe0aa91f4ULL, 0xacf86347d09f188dULL,
    0x2390f21f80c18306ULL, 0x594882d7b0f40a7fULL,
    0x1618f6fc78eb277bULL, 0x6cc0863448deae02ULL,
    0xe3a8176c18803589ULL, 0x997067a428b5bcf0ULL,
    0xfa11fe77117cdf02ULL, 0x80c98ebf2149567bULL,
    0x0fa11fe77117cdf0ULL, 0x75796f2f41224489ULL,
    0x3a291b04893d698dULL, 0x40f16bccb908e0f4ULL,
    0xcf99fa94e9567b7fULL, 0xb5418a5cd963f206ULL,
    0x513912c379682177ULL, 0x2be1620b495da80eULL,
    0xa489f35319033385ULL, 0xde51839b2936bafcULL,
    0x9101f7b0e12997f8ULL, 0xebd98778d11c1e81ULL,
    0x64b116208142850aULL, 0x1e6966e8b1770c73ULL,
    0x8719014c99c2b083ULL, 0xfdc17184a9f739faULL,
    0x72a9e0dcf9a9a271ULL, 0x08719014c99c2b08ULL,
    0x4721e43f0183060cULL, 0x3df994f731b68f75ULL,
    0xb29105af61e814feULL, 0xc849756751dd9d87ULL,
    0x2c31edf8f1d64ef6ULL, 0x56e99d30c1e3c78fULL,
    0xd9810c6891bd5c04ULL, 0xa3597ca0a188d57dULL,
    0xec09088b6997f879ULL, 0x96d1784359a27100ULL,
    0x19b9e91b09fcea8bULL, 0x636199d339c963f2ULL,
    0xdf7adabd7a6e2d6fULL, 0xa5a2aa754a5ba416ULL,
    0x2aca3b2d1a053f9dULL, 0x50124be52a30b6e4ULL,
    0x1f423fcee22f9be0ULL, 0x659a4f06d21a1299ULL,
    0xeaf2de5e82448912ULL, 0x902aae96b271006bULL,
    0x74523609127ad31aULL, 0x0e8a46c1224f5a63ULL,
    0x81e2d7997211c1e8ULL, 0xfb3aa75142244891ULL,
    0xb46ad37a8a3b6595ULL, 0xceb2a3b2ba0eececULL,
    0x41da32eaea507767ULL, 0x3b024222da65fe1eULL,
    0xa2722586f2d042eeULL, 0xd8aa554ec2e5cb97ULL,
    0x57c2c41692bb501cULL, 0x2d1ab4dea28ed965ULL,
    0x624ac0f56a91f461ULL, 0x1892b03d5aa47d18ULL,
    0x97fa21650afae693ULL, 0xed2251ad3acf6feaULL,
    0x095ac9329ac4bc9bULL, 0x7382b9faaaf135e2ULL,
    0xfcea28a2faafae69ULL, 0x8632586aca9a2710ULL,
    0xc9622c4102850a14ULL, 0xb3ba5c8932b0836dULL,
    0x3cd2cdd162ee18e6ULL, 0x460abd1952db919fULL,
    0x256b24ca6b12f26dULL, 0x5fb354025b277b14ULL,
    0xd0dbc55a0b79e09fULL, 0xaa03b5923b4c69e6ULL,
    0xe553c1b9f35344e2ULL, 0x9f8bb171c366cd9bULL,
    0x10e3202993385610ULL, 0x6a3b50e1a30ddf69ULL,
    0x8e43c87e03060c18ULL, 0xf49bb8b633338561ULL,
    0x7bf329ee636d1eeaULL, 0x012b592653589793ULL,
    0x4e7b2d0d9b47ba97ULL, 0x34a35dc5ab7233eeULL,
    0xbbcbcc9dfb2ca865ULL, 0xc113bc55cb19211cULL,
    0x5863dbf1e3ac9decULL, 0x22bbab39d3991495ULL,
    0xadd33a6183c78f1eULL, 0xd70b4aa9b3f20667ULL,
    0x985b3e827bed2b63ULL, 0xe2834e4a4bd8a21aULL,
    0x6debdf121b863991ULL, 0x1733afda2bb3b0e8ULL,
    0xf34b37458bb86399ULL, 0x8993478dbb8deae0ULL,
    0x06fbd6d5ebd3716bULL, 0x7c23a61ddbe6f812ULL,
    0x3373d23613f9d516ULL, 0x49aba2fe23cc5c6fULL,
    0xc6c333a67392c7e4ULL, 0xbc1b436e43a74e9dULL,
    0x95ac9329ac4bc9b5ULL, 0xef74e3e19c7e40ccULL,
    0x601c72b9cc20db47ULL, 0x1ac40271fc15523eULL,
    0x5594765a340a7f3aULL, 0x2f4c0692043ff643ULL,
    0xa02497ca54616dc8ULL, 0xdafce7026454e4b1ULL,
    0x3e847f9dc45f37c0ULL, 0x445c0f55f46abeb9ULL,
    0xcb349e0da4342532ULL, 0xb1eceec59401ac4bULL,
    0xfebc9aee5c1e814fULL, 0x8464ea266c2b0836ULL,
    0x0b0c7b7e3c7593bdULL, 0x71d40bb60c401ac4ULL,
    0xe8a46c1224f5a634ULL, 0x927c1cda14c02f4dULL,
    0x1d148d82449eb4c6ULL, 0x67ccfd4a74ab3dbfULL,
    0x289c8961bcb410bbULL, 0x5244f9a98c8199c2ULL,
    0xdd2c68f1dcdf0249ULL, 0xa7f41839ecea8b30ULL,
    0x438c80a64ce15841ULL, 0x3954f06e7cd4d138ULL,
    0xb63c61362c8a4ab3ULL, 0xcce411fe1cbfc3caULL,
    0x83b465d5d4a0eeceULL, 0xf96c151de49567b7ULL,
    0x76048445b4cbfc3cULL, 0x0cdcf48d84fe7545ULL,
    0x6fbd6d5ebd3716b7ULL, 0x15651d968d029fceULL,
    0x9a0d8ccedd5c0445ULL, 0xe0d5fc06ed698d3cULL,
    0xaf85882d2576a038ULL, 0xd55df8e515432941ULL,
    0x5a3569bd451db2caULL, 0x20ed197575283bb3ULL,
    0xc49581ead523e8c2ULL, 0xbe4df122e51661bbULL,
    0x3125607ab548fa30ULL, 0x4bfd10b2857d7349ULL,
    0x04ad64994d625e4dULL, 0x7e7514517d57d734ULL,
    0xf11d85092d094cbfULL, 0x8bc5f5c11d3cc5c6ULL,
    0x12b5926535897936ULL, 0x686de2ad05bcf04fULL,
    0xe70573f555e26bc4ULL, 0x9ddd033d65d7e2bdULL,
    0xd28d7716adc8cfb9ULL, 0xa85507de9dfd46c0ULL,
    0x273d9686cda3dd4bULL, 0x5de5e64efd965432ULL,
    0xb99d7ed15d9d8743ULL, 0xc3450e196da80e3aULL,
    0x4c2d9f413df695b1ULL, 0x36f5ef890dc31cc8ULL,
    0x79a59ba2c5dc31ccULL, 0x037deb6af5e9b8b5ULL,
    0x8c157a32a5b7233eULL, 0xf6cd0afa9582aa47ULL,
    0x4ad64994d625e4daULL, 0x300e395ce6106da3ULL,
    0xbf66a804b64ef628ULL, 0xc5bed8cc867b7f51ULL,
    0x8aeeace74e645255ULL, 0xf036dc2f7e51db2cULL,
    0x7f5e4d772e0f40a7ULL, 0x05863dbf1e3ac9deULL,
    0xe1fea520be311aafULL, 0x9b26d5e88e0493d6ULL,
    0x144e44b0de5a085dULL, 0x6e963478ee6f8124ULL,
    0x21c640532670ac20ULL, 0x5b1e309b16452559ULL,
    0xd476a1c3461bbed2ULL, 0xaeaed10b762e37abULL,
    0x37deb6af5e9b8b5bULL, 0x4d06c6676eae0222ULL,
    0xc26e573f3ef099a9ULL, 0xb8b627f70ec510d0ULL,
    0xf7e653dcc6da3dd4ULL, 0x8d3e2314f6efb4adULL,
    0x0256b24ca6b12f26ULL, 0x788ec2849684a65fULL,
    0x9cf65a1b368f752eULL, 0xe62e2ad306bafc57ULL,
    0x6946bb8b56e467dcULL, 0x139ecb4366d1eea5ULL,
    0x5ccebf68aecec3a1ULL, 0x2616cfa09efb4ad8ULL,
    0xa97e5ef8cea5d153ULL, 0xd3a62e30fe90582aULL,
    0xb0c7b7e3c7593bd8ULL, 0xca1fc72bf76cb2a1ULL,
    0x45775673a732292aULL, 0x3faf26bb9707a053ULL,
    0x70ff52905f188d57ULL, 0x0a2722586f2d042eULL,
    0x854fb3003f739fa5ULL, 0xff97c3c80f4616dcULL,
    0x1bef5b57af4dc5adULL, 0x61372b9f9f784cd4ULL,
    0xee5fbac7cf26d75fULL, 0x9487ca0fff135e26ULL,
    0xdbd7be24370c7322ULL, 0xa10fceec0739fa5bULL,
    0x2e675fb4576761d0ULL, 0x54bf2f7c6752e8a9ULL,
    0xcdcf48d84fe75459ULL, 0xb71738107fd2dd20ULL,
    0x387fa9482f8c46abULL, 0x42a7d9801fb9cfd2ULL,
    0x0df7adabd7a6e2d6ULL, 0x772fdd63e7936bafULL,
    0xf8474c3bb7cdf024ULL, 0x829f3cf387f8795dULL,
    0x66e7a46c27f3aa2cULL, 0x1c3fd4a417c62355ULL,
    0x935745fc4798b8deULL, 0xe98f353477ad31a7ULL,
    0xa6df411fbfb21ca3ULL, 0xdc0731d78f8795daULL,
    0x536fa08fdfd90e51ULL, 0x29b7d047efec8728ULL,
};


uint64_t make_crc64(uint64_t crc, const unsigned char *s, uint64_t l) 
{
	uint64_t j;

	for (j = 0; j < l; j++) 
	{
		uint8_t byte = s[j];
		crc = crc64_tab[(uint8_t)crc ^ byte] ^ (crc >> 8);
	}
	return crc;
}


/* Compute CRC-64 in the manner of xz, using the ECMA-182 polynomial,
   bit-reversed, with one's complement pre and post processing.  Provide a
   means to combine separately computed CRC-64's. 

   0x42F0E1EBA9EA3693 or 0xC96C5795D7870F42 (0xA17870F5D4F51B49)
   -------------------------------------------------------------------------- */

#define CRC64_POLY_ECMA_182 			(0xc96c5795d7870f42ULL)



#define CRC30_CDMA 						(0x38E74301)

#if 1
/*
 * Copyright (C) 2013  Internet Systems Consortium, Inc. ("ISC")
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */


/*%<
 * ECMA-182 CRC64 polynomial.
 */
static const uint64_t crc64_tab_isc[256] = {
	0x0000000000000000ULL, 0x42F0E1EBA9EA3693ULL, 0x85E1C3D753D46D26ULL,
	0xC711223CFA3E5BB5ULL, 0x493366450E42ECDFULL, 0x0BC387AEA7A8DA4CULL,
	0xCCD2A5925D9681F9ULL, 0x8E224479F47CB76AULL, 0x9266CC8A1C85D9BEULL,
	0xD0962D61B56FEF2DULL, 0x17870F5D4F51B498ULL, 0x5577EEB6E6BB820BULL,
	0xDB55AACF12C73561ULL, 0x99A54B24BB2D03F2ULL, 0x5EB4691841135847ULL,
	0x1C4488F3E8F96ED4ULL, 0x663D78FF90E185EFULL, 0x24CD9914390BB37CULL,
	0xE3DCBB28C335E8C9ULL, 0xA12C5AC36ADFDE5AULL, 0x2F0E1EBA9EA36930ULL,
	0x6DFEFF5137495FA3ULL, 0xAAEFDD6DCD770416ULL, 0xE81F3C86649D3285ULL,
	0xF45BB4758C645C51ULL, 0xB6AB559E258E6AC2ULL, 0x71BA77A2DFB03177ULL,
	0x334A9649765A07E4ULL, 0xBD68D2308226B08EULL, 0xFF9833DB2BCC861DULL,
	0x388911E7D1F2DDA8ULL, 0x7A79F00C7818EB3BULL, 0xCC7AF1FF21C30BDEULL,
	0x8E8A101488293D4DULL, 0x499B3228721766F8ULL, 0x0B6BD3C3DBFD506BULL,
	0x854997BA2F81E701ULL, 0xC7B97651866BD192ULL, 0x00A8546D7C558A27ULL,
	0x4258B586D5BFBCB4ULL, 0x5E1C3D753D46D260ULL, 0x1CECDC9E94ACE4F3ULL,
	0xDBFDFEA26E92BF46ULL, 0x990D1F49C77889D5ULL, 0x172F5B3033043EBFULL,
	0x55DFBADB9AEE082CULL, 0x92CE98E760D05399ULL, 0xD03E790CC93A650AULL,
	0xAA478900B1228E31ULL, 0xE8B768EB18C8B8A2ULL, 0x2FA64AD7E2F6E317ULL,
	0x6D56AB3C4B1CD584ULL, 0xE374EF45BF6062EEULL, 0xA1840EAE168A547DULL,
	0x66952C92ECB40FC8ULL, 0x2465CD79455E395BULL, 0x3821458AADA7578FULL,
	0x7AD1A461044D611CULL, 0xBDC0865DFE733AA9ULL, 0xFF3067B657990C3AULL,
	0x711223CFA3E5BB50ULL, 0x33E2C2240A0F8DC3ULL, 0xF4F3E018F031D676ULL,
	0xB60301F359DBE0E5ULL, 0xDA050215EA6C212FULL, 0x98F5E3FE438617BCULL,
	0x5FE4C1C2B9B84C09ULL, 0x1D14202910527A9AULL, 0x93366450E42ECDF0ULL,
	0xD1C685BB4DC4FB63ULL, 0x16D7A787B7FAA0D6ULL, 0x5427466C1E109645ULL,
	0x4863CE9FF6E9F891ULL, 0x0A932F745F03CE02ULL, 0xCD820D48A53D95B7ULL,
	0x8F72ECA30CD7A324ULL, 0x0150A8DAF8AB144EULL, 0x43A04931514122DDULL,
	0x84B16B0DAB7F7968ULL, 0xC6418AE602954FFBULL, 0xBC387AEA7A8DA4C0ULL,
	0xFEC89B01D3679253ULL, 0x39D9B93D2959C9E6ULL, 0x7B2958D680B3FF75ULL,
	0xF50B1CAF74CF481FULL, 0xB7FBFD44DD257E8CULL, 0x70EADF78271B2539ULL,
	0x321A3E938EF113AAULL, 0x2E5EB66066087D7EULL, 0x6CAE578BCFE24BEDULL,
	0xABBF75B735DC1058ULL, 0xE94F945C9C3626CBULL, 0x676DD025684A91A1ULL,
	0x259D31CEC1A0A732ULL, 0xE28C13F23B9EFC87ULL, 0xA07CF2199274CA14ULL,
	0x167FF3EACBAF2AF1ULL, 0x548F120162451C62ULL, 0x939E303D987B47D7ULL,
	0xD16ED1D631917144ULL, 0x5F4C95AFC5EDC62EULL, 0x1DBC74446C07F0BDULL,
	0xDAAD56789639AB08ULL, 0x985DB7933FD39D9BULL, 0x84193F60D72AF34FULL,
	0xC6E9DE8B7EC0C5DCULL, 0x01F8FCB784FE9E69ULL, 0x43081D5C2D14A8FAULL,
	0xCD2A5925D9681F90ULL, 0x8FDAB8CE70822903ULL, 0x48CB9AF28ABC72B6ULL,
	0x0A3B7B1923564425ULL, 0x70428B155B4EAF1EULL, 0x32B26AFEF2A4998DULL,
	0xF5A348C2089AC238ULL, 0xB753A929A170F4ABULL, 0x3971ED50550C43C1ULL,
	0x7B810CBBFCE67552ULL, 0xBC902E8706D82EE7ULL, 0xFE60CF6CAF321874ULL,
	0xE224479F47CB76A0ULL, 0xA0D4A674EE214033ULL, 0x67C58448141F1B86ULL,
	0x253565A3BDF52D15ULL, 0xAB1721DA49899A7FULL, 0xE9E7C031E063ACECULL,
	0x2EF6E20D1A5DF759ULL, 0x6C0603E6B3B7C1CAULL, 0xF6FAE5C07D3274CDULL,
	0xB40A042BD4D8425EULL, 0x731B26172EE619EBULL, 0x31EBC7FC870C2F78ULL,
	0xBFC9838573709812ULL, 0xFD39626EDA9AAE81ULL, 0x3A28405220A4F534ULL,
	0x78D8A1B9894EC3A7ULL, 0x649C294A61B7AD73ULL, 0x266CC8A1C85D9BE0ULL,
	0xE17DEA9D3263C055ULL, 0xA38D0B769B89F6C6ULL, 0x2DAF4F0F6FF541ACULL,
	0x6F5FAEE4C61F773FULL, 0xA84E8CD83C212C8AULL, 0xEABE6D3395CB1A19ULL,
	0x90C79D3FEDD3F122ULL, 0xD2377CD44439C7B1ULL, 0x15265EE8BE079C04ULL,
	0x57D6BF0317EDAA97ULL, 0xD9F4FB7AE3911DFDULL, 0x9B041A914A7B2B6EULL,
	0x5C1538ADB04570DBULL, 0x1EE5D94619AF4648ULL, 0x02A151B5F156289CULL,
	0x4051B05E58BC1E0FULL, 0x87409262A28245BAULL, 0xC5B073890B687329ULL,
	0x4B9237F0FF14C443ULL, 0x0962D61B56FEF2D0ULL, 0xCE73F427ACC0A965ULL,
	0x8C8315CC052A9FF6ULL, 0x3A80143F5CF17F13ULL, 0x7870F5D4F51B4980ULL,
	0xBF61D7E80F251235ULL, 0xFD913603A6CF24A6ULL, 0x73B3727A52B393CCULL,
	0x31439391FB59A55FULL, 0xF652B1AD0167FEEAULL, 0xB4A25046A88DC879ULL,
	0xA8E6D8B54074A6ADULL, 0xEA16395EE99E903EULL, 0x2D071B6213A0CB8BULL,
	0x6FF7FA89BA4AFD18ULL, 0xE1D5BEF04E364A72ULL, 0xA3255F1BE7DC7CE1ULL,
	0x64347D271DE22754ULL, 0x26C49CCCB40811C7ULL, 0x5CBD6CC0CC10FAFCULL,
	0x1E4D8D2B65FACC6FULL, 0xD95CAF179FC497DAULL, 0x9BAC4EFC362EA149ULL,
	0x158E0A85C2521623ULL, 0x577EEB6E6BB820B0ULL, 0x906FC95291867B05ULL,
	0xD29F28B9386C4D96ULL, 0xCEDBA04AD0952342ULL, 0x8C2B41A1797F15D1ULL,
	0x4B3A639D83414E64ULL, 0x09CA82762AAB78F7ULL, 0x87E8C60FDED7CF9DULL,
	0xC51827E4773DF90EULL, 0x020905D88D03A2BBULL, 0x40F9E43324E99428ULL,
	0x2CFFE7D5975E55E2ULL, 0x6E0F063E3EB46371ULL, 0xA91E2402C48A38C4ULL,
	0xEBEEC5E96D600E57ULL, 0x65CC8190991CB93DULL, 0x273C607B30F68FAEULL,
	0xE02D4247CAC8D41BULL, 0xA2DDA3AC6322E288ULL, 0xBE992B5F8BDB8C5CULL,
	0xFC69CAB42231BACFULL, 0x3B78E888D80FE17AULL, 0x7988096371E5D7E9ULL,
	0xF7AA4D1A85996083ULL, 0xB55AACF12C735610ULL, 0x724B8ECDD64D0DA5ULL,
	0x30BB6F267FA73B36ULL, 0x4AC29F2A07BFD00DULL, 0x08327EC1AE55E69EULL,
	0xCF235CFD546BBD2BULL, 0x8DD3BD16FD818BB8ULL, 0x03F1F96F09FD3CD2ULL,
	0x41011884A0170A41ULL, 0x86103AB85A2951F4ULL, 0xC4E0DB53F3C36767ULL,
	0xD8A453A01B3A09B3ULL, 0x9A54B24BB2D03F20ULL, 0x5D45907748EE6495ULL,
	0x1FB5719CE1045206ULL, 0x919735E51578E56CULL, 0xD367D40EBC92D3FFULL,
	0x1476F63246AC884AULL, 0x568617D9EF46BED9ULL, 0xE085162AB69D5E3CULL,
	0xA275F7C11F7768AFULL, 0x6564D5FDE549331AULL, 0x279434164CA30589ULL,
	0xA9B6706FB8DFB2E3ULL, 0xEB46918411358470ULL, 0x2C57B3B8EB0BDFC5ULL,
	0x6EA7525342E1E956ULL, 0x72E3DAA0AA188782ULL, 0x30133B4B03F2B111ULL,
	0xF7021977F9CCEAA4ULL, 0xB5F2F89C5026DC37ULL, 0x3BD0BCE5A45A6B5DULL,
	0x79205D0E0DB05DCEULL, 0xBE317F32F78E067BULL, 0xFCC19ED95E6430E8ULL,
	0x86B86ED5267CDBD3ULL, 0xC4488F3E8F96ED40ULL, 0x0359AD0275A8B6F5ULL,
	0x41A94CE9DC428066ULL, 0xCF8B0890283E370CULL, 0x8D7BE97B81D4019FULL,
	0x4A6ACB477BEA5A2AULL, 0x089A2AACD2006CB9ULL, 0x14DEA25F3AF9026DULL,
	0x562E43B4931334FEULL, 0x913F6188692D6F4BULL, 0xD3CF8063C0C759D8ULL,
	0x5DEDC41A34BBEEB2ULL, 0x1F1D25F19D51D821ULL,
	0xD80C07CD676F8394ULL, 0x9AFCE626CE85B507ULL
};

uint64_t isc_crc64_init() 
{
	uint64_t crc;

	crc = 0xffffffffffffffffULL;
	return crc;
}


uint64_t isc_crc64_update(uint64_t crc, const void *data, size_t len) 
{
	const unsigned char *p = data;
	int i;

	if( (NULL==data) )
	{
		printf("\n ERROR isc_crc64_update is erro ++ \n\n");
		return;
	}

	while (len-- > 0U) 
	{
		i = ((int) (crc >> 56) ^ *p++) & 0xff;
		crc = crc64_tab_isc[i] ^ (crc << 8);
	}
	return crc;
}


uint64_t isc_crc64_final(uint64_t crc) 
{
	uint64_t calcCRC;

	calcCRC = crc ^ 0xffffffffffffffffULL;

	return calcCRC;
}
#endif



#if 1
#define check_init(a, b)


#define POLY_UINT64_C 					(0xad93d23594c935a9ULL)
/******************** BEGIN GENERATED PYCRC FUNCTIONS ********************/
/**
 * Generated on Sun Dec 21 14:14:07 2014,
 * by pycrc v0.8.2, https://www.tty1.net/pycrc/
 *
 * LICENSE ON GENERATED CODE:
 * ==========================
 * As of version 0.6, pycrc is released under the terms of the MIT licence.
 * The code generated by pycrc is not considered a substantial portion of the
 * software, therefore the author of pycrc will not claim any copyright on
 * the generated code.
 * ==========================
 *
 * CRC configuration:
 *    Width        = 64
 *    Poly         = 0xad93d23594c935a9
 *    XorIn        = 0xffffffffffffffff
 *    ReflectIn    = True
 *    XorOut       = 0x0000000000000000
 *    ReflectOut   = True
 *    Algorithm    = bit-by-bit-fast
 *
 * Modifications after generation (by matt):
 *   - included finalize step in-line with update for single-pass generation
 *   - adjusted function parameters to match expected prototypes.
 *****************************************************************************/

/**
 * Reflect all bits of a \a data word of \a data_len bytes.
 *
 * \param data         The data word to be reflected.
 * \param data_len     The width of \a data expressed in number of bits.
 * \return             The reflected data.
 *****************************************************************************/
static inline uint64_t crc_reflect(uint64_t data, size_t data_len) {
    unsigned int i;
    uint64_t ret;

    ret = data & 0x01;
    for (i = 1; i < data_len; i++) {
        data >>= 1;
        ret = (ret << 1) | (data & 0x01);
    }
    return ret;
}

/**
 *  Update the crc value with new data.
 *
 * \param crc      The current crc value.
 * \param data     Pointer to a buffer of \a data_len bytes.
 * \param data_len Number of bytes in the \a data buffer.
 * \return         The updated crc value.
 ******************************************************************************/
uint64_t crc64(uint64_t crc, void *in_data, uint64_t data_len) 
{
    uint8_t *data = in_data;
    unsigned int i;
    bool bit;
    unsigned char c;

    while (data_len--) {
        c = *data++;
        for (i = 0x01; i & 0xff; i <<= 1) {
            bit = crc & 0x8000000000000000ULL;
            if (c & i) {
                bit = !bit;
            }
            crc <<= 1;
            if (bit) {
                crc ^= POLY_UINT64_C;
            }
        }
        crc &= 0xffffffffffffffffULL;
    }
    crc = crc & 0xffffffffffffffffULL;
    return crc_reflect(crc, 64) ^ 0x0000000000000000ULL;
}
/******************** END GENERATED PYCRC FUNCTIONS ********************/
#endif


/*-
 *  COPYRIGHT (C) 1986 Gary S. Brown.  You may use this program, or
 *  code or tables extracted from it, as desired without restriction.
 *
 *  First, the polynomial itself and its table of feedback terms.  The
 *  polynomial is
 *  X^32+X^26+X^23+X^22+X^16+X^12+X^11+X^10+X^8+X^7+X^5+X^4+X^2+X^1+X^0
 *
 *  Note that we take it "backwards" and put the highest-order term in
 *  the lowest-order bit.  The X^32 term is "implied"; the LSB is the
 *  X^31 term, etc.  The X^0 term (usually shown as "+1") results in
 *  the MSB being 1
 *
 * CRC32 code derived from work by Gary S. Brown.
 *
 * HDLC, ANSI X3.66, ITU-T V.42, Ethernet, Serial ATA, MPEG-2, PKZIP, Gzip, Bzip2, PNG,[14] many others
 *
 */

#define CRC32_POLYNOMIAL 			(0xEDB88320)
#define CRC32_TAB_SIZE 				256

#if 1
void makeCRCtable(unsigned long *table, unsigned long id) 
{
	unsigned long i, j, k;

	for(i = 0; i < CRC32_TAB_SIZE; ++i) 
	{
		k = i;
		for(j = 0; j < 8; ++j) 
		{
			if (k & 1) k = (k >> 1) ^ id;
			else k >>= 1;
		}
		table[i] = k;
	}
}


unsigned long calcCRC32(const unsigned char *mem, signed long size, unsigned long CRC) 
{
	unsigned long table[CRC32_TAB_SIZE];

	CRC = ~CRC;
	makeCRCtable(table, CRC32_POLYNOMIAL);

	while(size--)
		CRC = table[(CRC ^ *(mem++)) & 0xFF] ^ (CRC >> 8);

	return ~CRC;
}
#endif


const uint32_t crc32_tab[] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
	0xe963a535, 0x9e6495a3,	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
	0xf3b97148, 0x84be41de,	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,	0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5,	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,	0x35b5a8fa, 0x42b2986c,
	0xdbbbc9d6, 0xacbcf940,	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
	0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,	0x76dc4190, 0x01db7106,
	0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
	0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
	0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
	0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
	0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
	0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
	0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
	0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
	0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
	0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
	0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
	0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
	0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

void makeCRC32table(uint32_t *table, uint32_t id) 
{
	uint32_t i, j, k;

	for(i = 0; i < CRC32_TAB_SIZE; ++i) 
	{
		k = i;
		for(j = 0; j < 8; ++j) 
		{
			if (k & 1) k = (k >> 1) ^ id;
			else k >>= 1;
		}
		table[i] = k;
	}
}
//// -----------------------------------------------------
//// -----------------------------------------------------
//// -----------------------------------------------------

uint32_t make_crc32(uint32_t crc, const void *buf, size_t size)
{
	const uint8_t *p;

	p = buf;
	crc = crc ^ ~0U;

	while (size--)
		crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);

	return crc ^ ~0U;
}





/*===========================================================================
FUNCTION 
 make_crc16()
DESCRIPTION
       - Buf  : CRC를 생성하고자하는 Data + 2byte(crc)
       - size : Buf Size - 2 
       - CRC Generation (polynomial : x^16 + x^12 + x^5 + 1)
       - KS X ISO 18234-2 Appendix C
DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
#if 1
//---------------------------------------------------------------------------
// 표준 CRC 테이블
unsigned int KSCcrc16Tbl[256] = {
	0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241, 
	0xC601, 0x06C0, 0x0780,	0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440, 
	0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1,	0xCE81, 0x0E40, 
	0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841, 
	0xD801,	0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40, 
	0x1E00, 0xDEC1, 0xDF81, 0x1F40,	0xDD01, 0x1DC0, 0x1C80, 0xDC41, 
	0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680,	0xD641, 
	0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040, 
	0xF001, 0x30C0,	0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240, 
	0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501,	0x35C0, 0x3480, 0xF441, 
	0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
	0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840, 
	0x2800, 0xE8C1, 0xE981,	0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41, 
	0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1,	0xEC81, 0x2C40, 
	0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640, 
	0x2200,	0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041, 
	0xA001, 0x60C0, 0x6180, 0xA141,	0x6300, 0xA3C1, 0xA281, 0x6240, 
	0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480,	0xA441, 
	0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41, 
	0xAA01, 0x6AC0,	0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840, 
	0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01,	0x7BC0, 0x7A80, 0xBA41, 
	0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
	0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640, 
	0x7200, 0xB2C1, 0xB381,	0x7340, 0xB101, 0x71C0, 0x7080, 0xB041, 
	0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0,	0x5280, 0x9241, 
	0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440, 
	0x9C01,	0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40, 
	0x5A00, 0x9AC1, 0x9B81, 0x5B40,	0x9901, 0x59C0, 0x5880, 0x9841, 
	0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81,	0x4A40, 
	0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41, 
	0x4400, 0x84C1,	0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641, 
	0x8201, 0x42C0, 0x4380, 0x8341, 0x4100,	0x81C1, 0x8081, 0x4040
};

//===========================================================================
// CRC 체크 표준
//===========================================================================
// KS초안_KSCIEC62056-61 에 정의된 표준 CRC 16 계산 테이블 입니다.
// 유용하게 사용하세요.

//---------------------------------------------------------------------------
// 계산식
//#define SCrcCal(data) SCRC = ( (SCRC>>8) ^ KSCcrc16Tbl[ (SCRC^data) & 0xFF ] )
//U16 SCRC = 0xFFFF;
uint16_t make_ksc_crc16(uint16_t crc_seed, unsigned char *c_ptr, unsigned int len) 
{
	uint16_t crc = crc_seed;
	unsigned int index = 0;

	index = 0;
	while (len--)
	{

		crc = ( (crc>>8) ^ KSCcrc16Tbl[ (crc^c_ptr[index]) & 0xFF ] );
		index++;
	}
	return (crc);
}
#endif


unsigned int crctable[256] =
{
	0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
	0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7,
	0x0919, 0x1890, 0x2A0B, 0x3B82, 0x4F3D, 0x5EB4, 0x6C2F, 0x7DA6,
	0x8551, 0x94D8, 0xA643, 0xB7CA, 0xC375, 0xD2FC, 0xE067, 0xF1EE,
	0x1232, 0x03BB, 0x3120, 0x20A9, 0x5416, 0x459F, 0x7704, 0x668D,
	0x9E7A, 0x8FF3, 0xBD68, 0xACE1, 0xD85E, 0xC9D7, 0xFB4C, 0xEAC5,
	0x1B2B, 0x0AA2, 0x3839, 0x29B0, 0x5D0F, 0x4C86, 0x7E1D, 0x6F94,
	0x9763, 0x86EA, 0xB471, 0xA5F8, 0xD147, 0xC0CE, 0xF255, 0xE3DC,
	0x2464, 0x35ED, 0x0776, 0x16FF, 0x6240, 0x73C9, 0x4152, 0x50DB,
	0xA82C, 0xB9A5, 0x8B3E, 0x9AB7, 0xEE08, 0xFF81, 0xCD1A, 0xDC93,
	0x2D7D, 0x3CF4, 0x0E6F, 0x1FE6, 0x6B59, 0x7AD0, 0x484B, 0x59C2,
	0xA135, 0xB0BC, 0x8227, 0x93AE, 0xE711, 0xF698, 0xC403, 0xD58A,
	0x3656, 0x27DF, 0x1544, 0x04CD, 0x7072, 0x61FB, 0x5360, 0x42E9,
	0xBA1E, 0xAB97, 0x990C, 0x8885, 0xFC3A, 0xEDB3, 0xDF28, 0xCEA1,
	0x3F4F, 0x2EC6, 0x1C5D, 0x0DD4, 0x796B, 0x68E2, 0x5A79, 0x4BF0,
	0xB307, 0xA28E, 0x9015, 0x819C, 0xF523, 0xE4AA, 0xD631, 0xC7B8,
	0x48C8, 0x5941, 0x6BDA, 0x7A53, 0x0EEC, 0x1F65, 0x2DFE, 0x3C77,
	0xC480, 0xD509, 0xE792, 0xF61B, 0x82A4, 0x932D, 0xA1B6, 0xB03F,
	0x41D1, 0x5058, 0x62C3, 0x734A, 0x07F5, 0x167C, 0x24E7, 0x356E,
	0xCD99, 0xDC10, 0xEE8B, 0xFF02, 0x8BBD, 0x9A34, 0xA8AF, 0xB926,
	0x5AFA, 0x4B73, 0x79E8, 0x6861, 0x1CDE, 0x0D57, 0x3FCC, 0x2E45,
	0xD6B2, 0xC73B, 0xF5A0, 0xE429, 0x9096, 0x811F, 0xB384, 0xA20D,
	0x53E3, 0x426A, 0x70F1, 0x6178, 0x15C7, 0x044E, 0x36D5, 0x275C,
	0xDFAB, 0xCE22, 0xFCB9, 0xED30, 0x998F, 0x8806, 0xBA9D, 0xAB14,
	0x6CAC, 0x7D25, 0x4FBE, 0x5E37, 0x2A88, 0x3B01, 0x099A, 0x1813,
	0xE0E4, 0xF16D, 0xC3F6, 0xD27F, 0xA6C0, 0xB749, 0x85D2, 0x945B,
	0x65B5, 0x743C, 0x46A7, 0x572E, 0x2391, 0x3218, 0x0083, 0x110A,
	0xE9FD, 0xF874, 0xCAEF, 0xDB66, 0xAFD9, 0xBE50, 0x8CCB, 0x9D42,
	0x7E9E, 0x6F17, 0x5D8C, 0x4C05, 0x38BA, 0x2933, 0x1BA8, 0x0A21,
	0xF2D6, 0xE35F, 0xD1C4, 0xC04D, 0xB4F2, 0xA57B, 0x97E0, 0x8669,
	0x7787, 0x660E, 0x5495, 0x451C, 0x31A3, 0x202A, 0x12B1, 0x0338,
	0xFBCF, 0xEA46, 0xD8DD, 0xC954, 0xBDEB, 0xAC62, 0x9EF9, 0x8F70
};

uint16_t make_crc16(uint16_t crc_seed, unsigned char *c_ptr, unsigned int len) 
{
	uint16_t crc = crc_seed;
	unsigned int index = 0;

	index = 0;
	while (len--)
	{
		//crc = (crc << 8) ^ crctable[((crc >> 8) ^ *cdata++)];
		crc = (crc << 8) ^ crctable[( (crc >> 8) ^ c_ptr[index] )];
		index++;
		//printf("%d", crc);
	}
	return (crc);
}


#if 0	

//unsigned short crc16_init = 0xFFFF; /// initial
unsigned short make_crc16(uint16_t crcValue, unsigned char *buf, unsigned int size)
{
	unsigned short crc = crcValue;
	unsigned short crc_tmp;
	unsigned int count;
	
	if( (buf == NULL) || (size == 0)) 
	{
		printf("\n\n[++ERROR++] make_crc16 is failed.. buf is NULL. or size(%d) \n", size);
		exit(0);
		return 0;
	}
	
	for (count = 0; count < size; count++)
	{
		crc = (unsigned short) (swap(crc) ^ (unsigned short)buf[count]);
		crc ^= ((unsigned char)(crc) >> 4);
		crc = (unsigned short) (crc ^ (swap((unsigned char)(crc)) << 4) ^ ((unsigned char)(crc) << 5));
	}
	
	crc_tmp = (unsigned short)(crc ^ 0xFFFF);

//	printf("1CRC ---; 0x%X (%d) \n", crc_tmp, crc_tmp );
	return crc_tmp;
}
#endif



/*	
 * Copyright 2001-2010 Georges Menie (www.menie.org)
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 */

/* CRC16 implementation acording to CCITT standards */

const unsigned short crc16_tab_ccitt[256]= {
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
	0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
	0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
	0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
	0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
	0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
	0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
	0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
	0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
	0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
	0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
	0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
	0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
	0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
	0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
	0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
	0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
	0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
	0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
	0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
	0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
	0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
	0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
	0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
	0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
	0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
	0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
	0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};
  
unsigned short make_crc16_ccitt(unsigned short crc, const void *buf, int len)
{
	int counter;
	///unsigned short crc = 0;
	for( counter = 0; counter < len; counter++)
		crc = (crc<<8) ^ crc16_tab_ccitt[((crc>>8) ^ *(char *)buf++)&0x00FF];

	return crc;
}





/* adler32.c -- compute the Adler-32 checksum of a data stream
 * Copyright (C) 1995-1998 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

#define BASE 			65521L /* largest prime smaller than 65536 */
#define NMAX 			5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define DO2(buf,i)  {DO1(buf,i); DO1(buf,i+1);}
#define DO4(buf,i)  {DO2(buf,i); DO2(buf,i+2);}
#define DO8(buf,i)  {DO4(buf,i); DO4(buf,i+4);}
#define DO16(buf)   {DO8(buf,0); DO8(buf,8);}



unsigned int make_adler32(unsigned int adler, const byte *buf, unsigned int len)
{
	unsigned int s1 = adler & 0xffff;
	unsigned int s2 = (adler >> 16) & 0xffff;
	int k;

	if( NULL==buf ) return 1L;

    while (len > 0) 
	{
		k = len < NMAX ? len : NMAX;
		len -= k;
		while (k >= 16) 
		{
			DO16(buf);
			buf += 16;
			k -= 16;
        }
        if (k != 0) do {
			s1 += *buf++;
			s2 += s1;
		} while (--k);
		s1 %= BASE;
		s2 %= BASE;
	}
	return (s2 << 16) | s1;
}


unsigned int joaat_hash(unsigned int joaat_hash, unsigned char *key, size_t len)
{
    unsigned int hash = joaat_hash; // initial value must be 0x00
    size_t i;

    for (i = 0; i < len; i++)
    {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}



/*===========================================================================
FUNCTION 
 checkCrc()

DESCRIPTION

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
unsigned char checkCrc(unsigned char* pData, int Len)
{
	unsigned int value =0;
	int i, j, crc = 0xffff;
	unsigned short crcCalc=0, crcOrg = 0;
	unsigned char bCrcOk = 0; 
	unsigned char* DataPtr = NULL;
	
	DataPtr = pData;
	crcOrg =  *(DataPtr+Len-2) << 8 |*(DataPtr+Len-1);
	
	for(i = 0; i < Len-2; i++) 
	{  
		value = DataPtr[i];
		value <<= 8; 
		
		for (j = 0; j < 8; j++) {
			value <<= 1;
			crc <<= 1;  
			if ((crc ^ value) & 0x10000){ crc ^= 0x1021; }
		}  
	}
	
	crcCalc = ~crc & 0xffff;
	
	if(crcOrg == crcCalc) { return 1; }  
	else { return 0; }
}


#ifdef CHECKSUM_PROGRESS_DISPLAY
void ChecksumPrint(char *text, char *filename, __int64 Totfilesize, __int64 readsize, int insertCRC )
{

	if( 1==insertCRC ) return; // 2017.04.04

#if 0
	//printf("\b%s(%s) -> read : %.1f MB \r", text, filename, (float)Totfilesize/(1024.0*1024.0) );
	if( readsize>>20 )
		printf("\b%s>> (%s) -> reading : %.1fMB  \r", text, filename, (float)(Totfilesize/1024.0)/1024.0 );
	else if( readsize>>10 )
		printf("\b%s>> (%s) -> reading : %.1fkB  \r", text, filename, (float)(Totfilesize/1024.0) );
	else
#endif
		printf("\b%s>> (%s) -> reading : %lld Bytes \r", text, filename, Totfilesize );

}

void ClearScreen()
{
	#if 0
	//          1234567890123456789012345678901234567890123456789012345678901234567890
	LOG_V("                                                                                                   \r");
	#endif
}
#endif	

/* -------------------------------------------------------------------------
CRC32                 : 32-bit checksum using Cyclic Redundancy Check (CRC32). crc 
CRC64-ISO          : 64-bit checksum using the Cyclic Redundancy Check defined by ISO (CRC-64-ISO) spcrc 
CRC64-ECMA-182 : 64-bit checksum using the alternative Cyclic Redundancy Check defined in ECMA-182 (CRC-64-ECMA-182) altcrc 
CRC32-CRC64 : Combination of CRC32 and CRC64 sequence checksums with the sequence length cdigest 
MD5              : 128-bit Hash using Message Digest 5 (MD5) md 
SHA1            : 160-bit Hash using Secure Hash Algorithm 1 (SHA-1) shaa 
SHA2-224     : 224-bit Hash using Secure Hash Algorithm 2 (SHA-2) shab 
SHA2-256     : 256-bit Hash using Secure Hash Algorithm 2 (SHA-2) shac 
SHA2-384     : 384-bit Hash using Secure Hash Algorithm 2 (SHA-2) shad 
SHA2-512     : 512-bit Hash using Secure Hash Algorithm 2 (SHA-2) shae 
 ------------------------------------------------------------------------- */

uint16_t g_calcCrc16;
uint32_t g_calcCrc32;
uint32_t g_calcAdler32;
uint64_t g_calcCrc64;
uint32_t g_calcJoaat;



unsigned __int64 RunCRC16(char *infile_name, char *outfile_name, __int64 Fsize, int mIndex, int insertCRC, int iVerbosType, char *str_hash)
{ 
	uint16_t calcCrc16 = 0xFFFF; 
	unsigned __int64  TotDLen=0LL;
	size_t 	 LenRead=0;
	
	if( 0==mIndex )
		printf("CRC16>> Creating CRC16 [ KS X ISO 18234-2 ]... \n");

	if(data_buf) { free(data_buf); data_buf=NULL; }
	data_buf = (unsigned char*)malloc( MAX_BUF_SIZ*sizeof(unsigned char) );

	memset( data_buf, 0x00, MAX_BUF_SIZ*sizeof(unsigned char) );
	
	TotDLen = 0LL;
	calcCrc16 = 0xFFFF; // initial value
	while ( LenRead = fread(data_buf, sizeof(unsigned char), CRC_BUFSIZE, inpfile) )
	{
		TotDLen += LenRead;
		g_calcCrc16 = calcCrc16 = make_crc16(calcCrc16, data_buf, LenRead);
			
	#ifdef CHECKSUM_PROGRESS_DISPLAY
		ChecksumPrint("CRC16 Hashing", infile_name, TotDLen, LenRead, insertCRC );
	#endif	
	}

	ClearScreen();

	if(1 == insertCRC) 
	{
		if( g_iUpper ) // 2017.04.27
			printf("%04X   *%s*%s__(%llu) \r\n", g_calcCrc16, str_hash, infile_name, TotDLen );
		else
			printf("%04x   *%s*%s__(%llu) \r\n", g_calcCrc16, str_hash, infile_name, TotDLen );

		return TotDLen;
	}
	// ---------------------------------------	


	if( inpfile ) // 2020.07.17 && outfile ) /// && TotDLen )	
	{

		if(verbose && (2==iVerbosType || 3==iVerbosType) )
		{
			if( g_iUpper )
			{
				if(outfile) fprintf(outfile,"%04X  *%s*%s__(%llu) \r\n", calcCrc16, str_hash,infile_name, TotDLen );
			}
			else
			{
				if(outfile) fprintf(outfile,"%04x  *%s*%s__(%llu) \r\n", calcCrc16, str_hash,infile_name, TotDLen );
			}
		}
		else
		{
			if( g_iUpper )
			{
				if(outfile) fprintf(outfile,"%04X  *%s*%s \r\n", calcCrc16, str_hash,infile_name );
			}
			else
			{
				if(outfile) fprintf(outfile,"%04x  *%s*%s \r\n", calcCrc16, str_hash,infile_name );
			}
		}

		if( 0==mIndex ) /// 1 file
		{
			if(verbose && (2==iVerbosType || 3==iVerbosType) )
				printf("%04x   *%s*%s__(%llu) \r\n", calcCrc16, str_hash, infile_name, TotDLen );
			else
				printf("%04x   *%s*%s \r\n", calcCrc16, str_hash, infile_name );
		}
		else /// multi-input
		{
			if(verbose && (2==iVerbosType || 3==iVerbosType) )
				printf("%04x   *%s*%s__(%llu) \r\n", calcCrc16, str_hash, infile_name, TotDLen );
			else
				printf("%04x   *%s*%s \r\n", calcCrc16, str_hash, infile_name );
		}


	}
	else
	{
		printf("CRC16>> Can not create CRC16 for file [%s] or wrong length(%u) \r\n", infile_name, TotDLen );
	}
	
	if(data_buf) { free(data_buf); data_buf=NULL; }

	//Fsize = TotDLen; /* 인자로 값을 넘길경우,  맨마지막에 하라~~ else fclosed !!! */

	return TotDLen;
}



unsigned __int64 RunKSC_CRC16(char *infile_name, char *outfile_name, __int64 Fsize, int mIndex, int insertCRC, int iVerbosType, char *str_hash)
{ 
	uint16_t calcCrc16 = 0xFFFF; 
	unsigned __int64  TotDLen=0LL;
	size_t	 LenRead=0;
	
	if( 0==mIndex )
		printf("KSC-CRC16>> Creating KSC-CRC16 [ KSCIEC62056-61 ]... \n");

	if(data_buf) { free(data_buf); data_buf=NULL; }
	data_buf = (unsigned char*)malloc( MAX_BUF_SIZ*sizeof(unsigned char) );

	memset( data_buf, 0x00, MAX_BUF_SIZ*sizeof(unsigned char) );
	
	TotDLen = 0LL;
	calcCrc16 = 0xFFFF; // initial value
	while ( LenRead = fread(data_buf, sizeof(unsigned char), CRC_BUFSIZE, inpfile) )
	{
		TotDLen += LenRead;
		g_calcCrc16 = calcCrc16 = make_ksc_crc16(calcCrc16, data_buf, LenRead);
			
#ifdef CHECKSUM_PROGRESS_DISPLAY
		ChecksumPrint("KSC-CRC16 Hashing", infile_name, TotDLen, LenRead, insertCRC );
#endif	
	}

	ClearScreen();

	if(1 == insertCRC) 
	{
		if( g_iUpper ) // 2017.04.27
			printf("%04X   *%s*%s__(%llu) \r\n", g_calcCrc16, str_hash, infile_name, TotDLen );
		else
			printf("%04x   *%s*%s__(%llu) \r\n", g_calcCrc16, str_hash, infile_name, TotDLen );

		return TotDLen;
	}
	// ---------------------------------------	


	if( inpfile ) // 2020.07.17 && outfile ) /// && TotDLen )	
	{

		if(verbose && (2==iVerbosType || 3==iVerbosType) )
		{
			if( g_iUpper )
			{
				if(outfile) fprintf(outfile,"%04X  *%s*%s__(%llu) \r\n", calcCrc16, str_hash,infile_name, TotDLen );
			}
			else
			{
				if(outfile) fprintf(outfile,"%04x  *%s*%s__(%llu) \r\n", calcCrc16, str_hash,infile_name, TotDLen );
			}
		}
		else
		{
			if( g_iUpper )
			{
				if(outfile) fprintf(outfile,"%04X  *%s*%s \r\n", calcCrc16, str_hash,infile_name );
			}
			else
			{
				if(outfile) fprintf(outfile,"%04x  *%s*%s \r\n", calcCrc16, str_hash,infile_name );
			}
		}

		if( 0==mIndex ) /// 1 file
		{
			if(verbose && (2==iVerbosType || 3==iVerbosType) )
				printf("%04x   *%s*%s__(%llu) \r\n", calcCrc16, str_hash, infile_name, TotDLen );
			else
				printf("%04x   *%s*%s \r\n", calcCrc16, str_hash, infile_name );
		}
		else /// multi-input
		{
			if(verbose && (2==iVerbosType || 3==iVerbosType) )
				printf("%04x   *%s*%s__(%llu) \r\n", calcCrc16, str_hash, infile_name, TotDLen );
			else
				printf("%04x   *%s*%s \r\n", calcCrc16, str_hash, infile_name );
		}

	}
	else
	{
		printf("KSC-CRC16>> Can not create KSC-CRC16 for file [%s] or wrong length(%u) \r\n", infile_name, TotDLen );
	}
	
	if(data_buf) { free(data_buf); data_buf=NULL; }

	//Fsize = TotDLen; /* 인자로 값을 넘길경우,  맨마지막에 하라~~ else fclosed !!! */

	return TotDLen;
}


unsigned __int64 RunCRC16CCITT(char *infile_name, char *outfile_name, __int64 Fsize, int mIndex, int insertCRC, int iVerbosType, char *str_hash)
{ 
	uint16_t calcCrc16 = 0; 
	unsigned __int64  TotDLen=0LL;
	size_t 	 LenRead=0;

	if(0==mIndex)
		printf("CRC16CCITT>> Creating CRC16 [ CCITT standards ]..... \n");

	data_buf = (unsigned char*)malloc( MAX_BUF_SIZ*sizeof(unsigned char) );

	TotDLen = 0LL;
	calcCrc16 = 0;
	memset( data_buf, 0x00, MAX_BUF_SIZ*sizeof(unsigned char) );
	while ( LenRead = fread(data_buf, sizeof(unsigned char), CRC_BUFSIZE, inpfile) )
	{
		TotDLen += LenRead;
		g_calcCrc16 = calcCrc16 = make_crc16_ccitt( calcCrc16, data_buf, LenRead);
		
	#ifdef CHECKSUM_PROGRESS_DISPLAY
		ChecksumPrint("CRC16CCITT Hashing", infile_name, TotDLen, LenRead, insertCRC  );
	#endif		

	}
	ClearScreen();
	
	if(data_buf) { free(data_buf); data_buf=NULL; }

	if(1 == insertCRC) 
	{
	
		if( g_iUpper ) // 2017.04.27
			printf("%04X   *%s*%s__(%llu) \r\n", g_calcCrc16, str_hash, infile_name, TotDLen );
		else
			printf("%04x   *%s*%s__(%llu) \r\n", g_calcCrc16, str_hash, infile_name, TotDLen );

		return TotDLen;
	}
	// ---------------------------------------	


	if( inpfile ) // 2020.07.17 && outfile )	
	{
		//fwrite((unsigned char*)&crc_res, sizeof(unsigned char), 2 , outfile);

		if(verbose && (2==iVerbosType || 3==iVerbosType) )
		{
			if(outfile) fprintf(outfile,"%04x  *%s*%s__(%llu)  \r\n", calcCrc16, str_hash, infile_name, TotDLen);
		}
		else
		{
			if(outfile) fprintf(outfile,"%04x  *%s*%s  \r\n", calcCrc16, str_hash, infile_name);
		}

		if( 0==mIndex ) /// 1 file
		{
			if(verbose && (2==iVerbosType || 3==iVerbosType) )
			{
				printf("%04x  *%s*%s__(%llu) \r\n", calcCrc16, str_hash, infile_name, TotDLen );
			}
			else
			{
				printf("%04x  *%s*%s \r\n", calcCrc16, str_hash, infile_name );
			}
		}
		else /// multi-input files
		{
			if(verbose && (2==iVerbosType || 3==iVerbosType) )
				printf("%04x  *%s*%s__(%llu) \r\n", calcCrc16, str_hash, infile_name, TotDLen );
			else
				printf("%04x  *%s*%s \r\n", calcCrc16, str_hash, infile_name );
		}

	}
	else
	{
		printf("CRC16CCITT> Can not create CRC16CCITT for file [%s] or wrong length(%u) \r\n", infile_name, TotDLen );
	}

	//*Fsize = TotDLen; /* 인자로 값을 넘길경우,  맨마지막에 하라~~ else fclosed !!! */

	return TotDLen;
}



unsigned __int64 RunCRC32(char *infile_name, char *outfile_name, __int64 Fsize, int mIndex, int insertCRC, int iVerbosType, char *str_hash)
{
	uint32_t calcCrc32=0;
	unsigned __int64  TotDLen=0;
	size_t 	 LenRead=0;

	if(0==mIndex)
		printf("CRC32>> Creating CRC32 [ Polynomial : 0xEDB88320 ]... \n");

	data_buf = (unsigned char*)malloc( MAX_BUF_SIZ*sizeof(unsigned char) );
	memset( data_buf, 0x00, MAX_BUF_SIZ*sizeof(unsigned char) );

	TotDLen = 0LL;
	while ( LenRead = fread(data_buf, sizeof(unsigned char), CRC_BUFSIZE, inpfile) )
	{
		TotDLen += LenRead;

	#if 0 // !Choice One!!!
		g_calcCrc32 = calcCrc32 = make_crc32(calcCrc32, (void*)data_buf, LenRead);
	#else
		g_calcCrc32 = calcCrc32 = calcCRC32(data_buf, (unsigned long)LenRead, calcCrc32);
	#endif

	
	#ifdef CHECKSUM_PROGRESS_DISPLAY
		ChecksumPrint("CRC32 Hashing", infile_name, TotDLen, LenRead, insertCRC  );
	#endif

	}
	ClearScreen();
	
	if(data_buf) { free(data_buf); data_buf=NULL; }

	if(1 == insertCRC) 
	{
		if( g_iUpper ) // 2017.04.27
			printf("%08X   *%s*%s__(%llu) \r\n", g_calcCrc32, str_hash, infile_name, TotDLen );
		else	
			printf("%08x   *%s*%s__(%llu) \r\n", g_calcCrc32, str_hash, infile_name, TotDLen );

		return TotDLen;
	}
	// ---------------------------------------	

	if( inpfile ) //  2020.07.17 && outfile )
	{

		if(verbose && (2==iVerbosType || 3==iVerbosType) )
		{
			if(outfile) fprintf(outfile,"%08x  *%s*%s__(%llu) \r\n", calcCrc32, str_hash, infile_name, TotDLen );
		}
		else
		{
			if(outfile) fprintf(outfile,"%08x  *%s*%s \r\n", calcCrc32, str_hash, infile_name );
		}

		if(0==mIndex) /// 1 file
		{
			if(verbose && (2==iVerbosType || 3==iVerbosType) )
			{
				printf("%08x  *%s*%s__(%llu) \r\n", calcCrc32, str_hash, infile_name, TotDLen );
			}
			else
			{
				printf("%08x  *%s*%s \r\n", calcCrc32, str_hash, infile_name );
			}
		}
		else /// multi-input files
		{
			if(verbose && (2==iVerbosType || 3==iVerbosType) )
				printf("%08x  *%s*%s__(%llu) \r\n", calcCrc32, str_hash, infile_name, TotDLen  );
			else
				printf("%08x  *%s*%s \r\n", calcCrc32, str_hash, infile_name );
		}


	}
	else
	{
		printf("CRC32>> Can not create CRC32 for file [%s]. \r\n", infile_name );
	}


	//*Fsize = TotDLen; /* 인자로 값을 넘길경우,  맨마지막에 하라~~ else fclosed !!! */

	return TotDLen;
}


unsigned __int64 RunCRC64(char *infile_name, char *outfile_name, __int64 Fsize, int mIndex, int insertCRC, int iVerbosType, char *str_hash)
{
	uint64_t calcCrc64 = 0ULL;
	unsigned __int64  TotDLen=0LL;
	size_t 	 LenRead=0;

	if(0==mIndex)
		printf("CRC64>> Creating CRC64 [ Polynomial : 0xad93d23594c935a9 ]... \n");

	data_buf = (unsigned char*)malloc( MAX_BUF_SIZ*sizeof(unsigned char) );
	memset( data_buf, 0x00, MAX_BUF_SIZ*sizeof(unsigned char) );

	TotDLen = 0LL;
	while ( LenRead = fread(data_buf, sizeof(unsigned char), CRC_BUFSIZE, inpfile) )
	{
		TotDLen += LenRead;

	#if 1 
		g_calcCrc64 = calcCrc64 = make_crc64(calcCrc64, (void*)data_buf, LenRead);
	#else
		///g_calcCrc64 = calcCrc64 = crc64(calcCrc64, (void*)data_buf, LenRead);
	#endif
	
	#ifdef CHECKSUM_PROGRESS_DISPLAY
		ChecksumPrint("CRC64 Hashing", infile_name, TotDLen, LenRead, insertCRC  );
	#endif

	}
	ClearScreen();

	if(data_buf) { free(data_buf); data_buf=NULL; }

	if(1 == insertCRC) // in header
	{
		if( g_iUpper ) // 2017.04.27
			printf("%016llX    *%s*%s__(%llu) \r\n", g_calcCrc64, str_hash, infile_name, TotDLen );
		else	
			printf("%016llx    *%s*%s__(%llu) \r\n", g_calcCrc64, str_hash, infile_name, TotDLen );

		return TotDLen;
	}
	// --------------------------------------------------------------------	

	if( inpfile ) //  2020.07.17 && outfile )
	{
		if(verbose && (2==iVerbosType || 3==iVerbosType) )
		{
			if(outfile) fprintf(outfile,"%016llx  *%s*%s__(%llu) \r\n", calcCrc64, str_hash, infile_name, TotDLen );
		}
		else
		{
			if(outfile) fprintf(outfile,"%016llx  *%s*%s \r\n", calcCrc64, str_hash, infile_name );
		}

		if( 0==mIndex ) /// 1 files ---
		{
			if(verbose && (2==iVerbosType || 3==iVerbosType) )
			{
				printf("%016llx  *%s*%s__(%llu) \r\n", calcCrc64, str_hash, infile_name, TotDLen );
			}
			else
			{
				printf("%016llx  *%s*%s \r\n", calcCrc64, str_hash, infile_name  );
			}
		}
		else /// multi-input files
		{
			if(verbose && (2==iVerbosType || 3==iVerbosType) )
				printf("%016llx  *%s*%s__(%llu) \r\n", calcCrc64, str_hash, infile_name, TotDLen );
			else
				printf("%016llx  *%s*%s \r\n", calcCrc64, str_hash, infile_name );
		}

	}
	else
	{
		printf("CRC64>> Can not create CRC64 for file [%s]. \r\n", infile_name );
	}

	//*Fsize = TotDLen; /* 인자로 값을 넘길경우,  맨마지막에 하라~~ else fclosed !!! */

	return TotDLen;
}



unsigned __int64 RunCRC64_isc(char *infile_name, char *outfile_name, __int64 Fsize, int mIndex, int insertCRC, int iVerbosType, char *str_hash)
{
	uint64_t calcCrc64 = 0ULL;
	unsigned __int64  TotDLen=0LL;
	size_t 	 LenRead=0;

	if(0==mIndex)
		printf("CRC64>> Creating CRC64_ISC(Internet Systems Consortium)... \n");

	data_buf = (unsigned char*)malloc( MAX_BUF_SIZ*sizeof(unsigned char) );
	memset( data_buf, 0x00, MAX_BUF_SIZ*sizeof(unsigned char) );

	TotDLen = 0LL;

	isc_crc64_init(); // Internet Consorisum C
	while ( LenRead = fread(data_buf, sizeof(unsigned char), CRC_BUFSIZE, inpfile) )
	{
		TotDLen += LenRead;

	#if 0 // NEVER!!! 
		g_calcCrc64 = calcCrc64 = make_crc64(calcCrc64, (void*)data_buf, LenRead);
	#else
		g_calcCrc64 = calcCrc64 = isc_crc64_update(calcCrc64, (void*)data_buf, LenRead);
	#endif
	
	#ifdef CHECKSUM_PROGRESS_DISPLAY
		ChecksumPrint("CRC64_ISC Hashing", infile_name, TotDLen, LenRead, insertCRC  );
	#endif
	}

	g_calcCrc64 = calcCrc64 = isc_crc64_final(calcCrc64);

	
	ClearScreen();

	if(data_buf) { free(data_buf); data_buf=NULL; }

	if(1 == insertCRC) // in header
	{
		if( g_iUpper ) // 2017.04.27
			printf("%016llX    *%s*%s__(%llu) \r\n", g_calcCrc64, str_hash, infile_name, TotDLen );
		else	
			printf("%016llx    *%s*%s__(%llu) \r\n", g_calcCrc64, str_hash, infile_name, TotDLen );

		return TotDLen;
	}
	// --------------------------------------------------------------------	

	if( inpfile ) //  2020.07.17 && outfile )
	{
		if(verbose && (2==iVerbosType || 3==iVerbosType) )
		{
			if(outfile) fprintf(outfile,"%016llx  *%s*%s__(%llu) \r\n", calcCrc64, str_hash, infile_name, TotDLen );
		}
		else
		{
			if(outfile) fprintf(outfile,"%016llx  *%s*%s \r\n", calcCrc64, str_hash, infile_name );
		}

		if( 0==mIndex ) /// 1 files ---
		{
			if(verbose && (2==iVerbosType || 3==iVerbosType) )
			{
				printf("%016llx  *%s*%s__(%llu) \r\n", calcCrc64, str_hash, infile_name, TotDLen );
			}
			else
			{
				printf("%016llx  *%s*%s \r\n", calcCrc64, str_hash, infile_name  );
			}
		}
		else /// multi-input files
		{
			if(verbose && (2==iVerbosType || 3==iVerbosType) )
				printf("%016llx  *%s*%s__(%llu) \r\n", calcCrc64, str_hash, infile_name, TotDLen );
			else
				printf("%016llx  *%s*%s \r\n", calcCrc64, str_hash, infile_name );
		}

	}
	else
	{
		printf("CRC64_ISC>> Can not create CRC64ISC for file [%s]. \r\n", infile_name );
	}

	//*Fsize = TotDLen; /* 인자로 값을 넘길경우,  맨마지막에 하라~~ else fclosed !!! */

	return TotDLen;
}


unsigned __int64 RunAdler32(char *infile_name, char *outfile_name, __int64 Fsize, int mIndex, int insertCRC, int iVerbosType, char *str_hash)
{
	uint32_t calcAdler32 = 1;
	unsigned __int64  TotDLen=0;
	size_t	 LenRead=0;
	
	if(0==mIndex)
		printf("ADLER32>> Creating Adler32 [ zlib ]... \n");

	data_buf = (unsigned char*)malloc( MAX_BUF_SIZ*sizeof(unsigned char) );
	memset( data_buf, 0x00, MAX_BUF_SIZ*sizeof(unsigned char) );

	TotDLen = 0LL;
	while ( LenRead = fread(data_buf, sizeof(unsigned char), CRC_BUFSIZE, inpfile) )
	{
		TotDLen += LenRead;
		g_calcAdler32 = calcAdler32 = make_adler32(calcAdler32, data_buf, LenRead);

	#ifdef CHECKSUM_PROGRESS_DISPLAY
		ChecksumPrint("ADLER32 Hashing", infile_name, TotDLen, LenRead, insertCRC  );
	#endif

	}
	ClearScreen();

	if(data_buf) { free(data_buf); data_buf=NULL; }

	if(1 == insertCRC) 
	{
		if( g_iUpper ) // 2017.04.27
			printf("%08X     *%s*%s__(%llu) \r\n", g_calcAdler32, str_hash, infile_name, TotDLen );
		else
			printf("%08x     *%s*%s__(%llu) \r\n", g_calcAdler32, str_hash, infile_name, TotDLen );

		return TotDLen;
	}
	// ---------------------------------------	
	
	if( inpfile ) // 2020.07.17  && outfile )
	{
		if(verbose && (2==iVerbosType || 3==iVerbosType) )
		{
			if(outfile) fprintf(outfile,"%08x  *%s*%s__(%llu) \r\n", calcAdler32, str_hash, infile_name, TotDLen );
		}
		else
		{
			if(outfile) fprintf(outfile,"%08x  *%s*%s \r\n", calcAdler32, str_hash, infile_name );
		}

		if( 0==mIndex ) /// 1 files ---
		{
			if(verbose && (2==iVerbosType || 3==iVerbosType) )
			{
				printf("%08x  *%s*%s__(%llu) \r\n", calcAdler32, str_hash, infile_name, TotDLen );
			}
			else
			{
				printf("%08x  *%s*%s  \r\n", calcAdler32, str_hash, infile_name );
			}
		}
		else /// multi-input files
		{
			if(verbose && (2==iVerbosType || 3==iVerbosType) )
				printf("%08x  *%s*%s__(%llu) \r\n", calcAdler32, str_hash, infile_name, TotDLen );
			else
				printf("%08x  *%s*%s \r\n", calcAdler32, str_hash, infile_name );
		}


	}
	else
	{
		printf("ADLER32>> Can not create Adler32 for file [%s]. \r\n", infile_name );
	}


	//*Fsize = TotDLen; /* 인자로 값을 넘길경우,  맨마지막에 하라~~ else fclosed !!! */

	return TotDLen;
}



unsigned __int64 RunJoaat(char *infile_name, char *outfile_name, __int64 Fsize, int mIndex, int insertCRC, int iVerbosType, char *str_hash)
{
	uint32_t calcJoaat = 0;
	unsigned __int64  TotDLen=0;
	size_t	 LenRead=0;
	
	if(0==mIndex)
		printf("JOAAT>> Creating JOAAT Hash... \n");

	data_buf = (unsigned char*)malloc( MAX_BUF_SIZ*sizeof(unsigned char) );
	memset( data_buf, 0x00, MAX_BUF_SIZ*sizeof(unsigned char) );

	TotDLen = 0LL;
	calcJoaat = 0;
	while ( LenRead = fread(data_buf, sizeof(unsigned char), BUFSIZE, inpfile) )
	{
		TotDLen += LenRead;
		g_calcJoaat = calcJoaat = joaat_hash(calcJoaat, data_buf, LenRead);
		
#ifdef CHECKSUM_PROGRESS_DISPLAY
		ChecksumPrint("JOAAT Hashing", infile_name, TotDLen, LenRead, insertCRC  );
#endif

	}
	ClearScreen();

	if(data_buf) { free(data_buf); data_buf=NULL; }

	if(1 == insertCRC) 
	{
		if( g_iUpper ) // 2017.04.27
			printf("%08X   *%s*%s__(%llu) \r\n", g_calcJoaat, str_hash, infile_name, TotDLen );
		else
			printf("%08x   *%s*%s__(%llu) \r\n", g_calcJoaat, str_hash, infile_name, TotDLen );

		return TotDLen;
	}
	// ---------------------------------------	
	
	if( inpfile ) // 2020.07.17  && outfile )
	{
		if(verbose && (2==iVerbosType || 3==iVerbosType) )
		{
			if(outfile) fprintf(outfile,"%08x  *%s*%s__(%llu) \r\n", calcJoaat, str_hash, infile_name, TotDLen );
		}
		else
		{
			if(outfile) fprintf(outfile,"%08x  *%s*%s \r\n", calcJoaat, str_hash, infile_name );
		}

		if( 0==mIndex ) /// 1 files ---
		{
			if(verbose && (2==iVerbosType || 3==iVerbosType) )
			{
				printf("%08x  *%s*%s__(%llu) \r\n", calcJoaat, str_hash, infile_name, TotDLen );
			}
			else
			{
				printf("%08x  *%s*%s  \r\n", calcJoaat, str_hash, infile_name );
			}
		}
		else /// multi-input files
		{
			if(verbose && (2==iVerbosType || 3==iVerbosType) )
				printf("%08x  *%s*%s__(%llu) \r\n", calcJoaat, str_hash, infile_name, TotDLen );
			else
				printf("%08x  *%s*%s \r\n", calcJoaat, str_hash, infile_name );
		}
	}
	else
	{
		printf("JOAAT>> Can not create JOAAT for file [%s]. \r\n", infile_name );
	}

	//*Fsize = TotDLen; /* 인자로 값을 넘길경우,  맨마지막에 하라~~ else fclosed !!! */
	return TotDLen;
}


/* ====================================================================== */
/* ====================================================================== */
/* ====================================================================== */


void beep (int frequency, int duration)
{
#if 0
#define ESC 	27

	printf("%c[10;%d]%c[11;%d]\a", ESC, frequency, ESC, duration);
#endif
}


int 	istextfile(char *txtstr)
{
	while( (NULL != txtstr) && (*txtstr != '.') )
		txtstr++;

#if 1 /// 2013.04.08
	if( 0 == strncmp(txtstr, ".exe", 4) ) return 0; /* no text file */
	if( 0 == strncmp(txtstr, ".EXE", 4) ) return 0; /* no text file */
	if( 0 == strncmp(txtstr, ".com", 4) ) return 0; /* no text file */
	if( 0 == strncmp(txtstr, ".COM", 4) ) return 0; /* no text file */
#endif

	return 1; /* text file */
}



void 	FileCounter(char *txt)
{
	struct 	_finddata_t 	root_files;
	struct 	_finddata_t 	subd_files;
	long 	roothFile, subdhFile;
	unsigned int LineCounter = 0, AccLineCounter = 0 ;
	int 	c, subdlen, itmp;
	unsigned char sub_txt[255];
	char strtmp[255];
	
	/* Find first .c file in current directory */
	if( (roothFile = _findfirst( "*.*", &root_files )) == -1L )
	{
		printf( "No files in current directory! \n" );
	}
	else
	{

		if( 0 == strncmp(txt,"all", 3) )
		{
			//printf( "Listing of files\n\n" );
			printf( "\nR H S A d FILE                                              DATE %24c SIZE\n", ' ' );
			printf(   "- - - - - ------------------------------------------------  ------------------------ %4c ----\n", ' ' );
		}
		else if( 0 == strncmp(txt,"line", 4) )
		{
			printf( "\nFILENAME                                                 LINES\n" );
			printf(   "-------------------------------------------------------  ------\n" );
		}
		else 
		{ 
			printf("Check option...\n" );
			return; 
		}
	
		/* Find the rest of the .c files */
		do
		{
			if( 0 == strncmp(txt,"all", 3) )
			{
				printf( ( root_files.attrib & _A_RDONLY ) ? "Y " : "- " );
				printf( ( root_files.attrib & _A_SYSTEM ) ? "Y " : "- " );
				printf( ( root_files.attrib & _A_HIDDEN ) ? "Y " : "- " );
				printf( ( root_files.attrib & _A_ARCH )   ? "Y " : "- " );
				printf( ( root_files.attrib & _A_SUBDIR ) ? "Y " : "- " );

				if( !(root_files.attrib & _A_SUBDIR) )
					printf( " %-48s %.24s %9ld", root_files.name, ctime( &( root_files.time_write ) ), root_files.size );
				else
				{
					memset( strtmp, 0x00, sizeof(strtmp) );
					subdlen = (48-5-strlen(root_files.name));
					for(itmp=0; itmp<subdlen; itmp++) strtmp[itmp] = 0x20; /// fill SPACE
					
					printf( " *[%s]* %s %.24s ", root_files.name, strtmp, ctime( &( root_files.time_write ) ) );
				}
			}
			else if( 0 == strncmp(txt,"line", 4) )
			{
				//printf( " %-32s %.24s ", root_files.name, ctime( &( root_files.time_write ) ) );
				if( !(root_files.attrib & _A_SUBDIR) )
					printf( " %-48s ", root_files.name );
				else
					printf( " *[%s]* ", root_files.name );
			}
			
			
			LineCounter = 0;
			if( !(root_files.attrib & _A_SUBDIR) )
			{
				if( istextfile(root_files.name) )
				{
					if( NULL == (inpfile = fopen( root_files.name, "r")) ) 
					{
						printf("\n[++LineCounter++ERROR++] Can not open input file (%s). \n",root_files.name );
						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						_findclose( roothFile );
						beep(700,100);
						return;
					}
		
					while( EOF != (c=fgetc(inpfile)) )
					{
						if('\n' == c || '\r' == c )
							LineCounter++;
					}
					if(inpfile) { fclose(inpfile); inpfile=NULL; }
					
					AccLineCounter += LineCounter;

					printf( "   %10ld\n", LineCounter );
				}
				else
				{
					printf(" No text file \n");
				}
			}
			else
			{
		#if 1
				printf("   -> Folder\n");
		#else
				printf(" Folder. \n"); /// [%s] \n", root_files.name);
	
				subdlen = strlen(root_files.name);

				if( subdlen <= 2 && ( 0==strncmp(root_files.name,".", 1) || 0==strncmp(root_files.name,"..", 2) ) )
				{
					printf(" Folder..\n");
				} 
				else
				{
					LineCounter = 0;
					memset( sub_txt, 0x00, sizeof(sub_txt) );  
					sprintf( sub_txt, "%s\\*.*", root_files.name );

					if( (subdhFile = _findfirst( sub_txt, &subd_files )) == -1L )
					{
						printf( "No files in sub directory! \n" );
					}		

					if( !(subd_files.attrib & _A_SUBDIR) )
					{
						if( istextfile(subd_files.name) )
						{
							if( NULL == (inpfile = fopen( subd_files.name, "r")) ) 
							{
								printf("\n[++LineCounter++ERROR++] Can not open input file (%s). \n", subd_files.name );
								fclose(inpfile);
								_findclose( roothFile );
								_findclose( subdhFile );
								beep(700,100);
								return;
							}
				
							while( EOF != (c=fgetc(inpfile)) )
							{
								if('\n' == c || '\r' == c )
									LineCounter++;
							}
							fclose(inpfile);
							
							AccLineCounter += LineCounter;

							printf( "   %10ld\n", LineCounter );		
						}
						else { printf(">>>\n\n"); }
						_findclose( subdhFile );
					}
				}
		#endif
			}
			
		}while( _findnext( roothFile, &root_files ) == 0 );
		_findclose( roothFile );

		printf( "-------------------------------------------------------  ------\n" );
		printf( " Total Line Counter ===>                             %10ld\n", AccLineCounter );


	}
}



void help_int2bin_form(void)
{

 printf("Intel Hex formats \n"
        "================= \n"
        "  \n"
        "Hexadecimal values are always in uppercase. Each line is a record. \n"
        "The sum of all the bytes in each record should be 00 (modulo 256). \n"
        "  \n"
        "Record types:  \n"
        "  \n"
        "00: data records  \n"
        "01: end-of-file record  \n"
        "02: extended address record \n"
        " \n"
        "Data record \n"
        "-----------  \n"
        "	:0D011C0000000000C3E0FF0000000000C30F   \n"
        "  \n"
        ": 0D 011C 00 00000000C3E0FF0000000000C3 0F  \n"
        "|  |   |   | -------------+------------  |  \n"
        "|  |   |   |              |              +--- Checksum    \n"
        "|  |   |   |              +------------------ Data bytes  \n"
        "|  |   |   +--------------------------------- Record type  \n"
        "|  |   +------------------------------------- Address  \n"
        "|  +----------------------------------------- Number of data bytes  \n"
        "+-------------------------------------------- Start of record  \n"
        "  \n"
        "End of file record  \n"
        "------------------  \n"
        "	:00000001FE  \n"
        "  \n"
        ": 00 0000 01 FE  \n"
        "|  |   |   |  |  \n"
        "|  |   |   |  +--- Checksum  \n"
        "|  |   |   +------ Record type  \n"
        "|  |   +---------- Address  \n"
        "|  +-------------- Number of data bytes  \n"
        "+----------------- Start of record  \n"
        "  \n"
        "Extended address record  \n"
        "-----------------------  \n"
        "	:02010002E0001B  \n"
        "  \n"
        ": 02 0100 02 E000 1B  \n"
        "|  |   |   |  |    |  \n"
        "|  |   |   |  |    +--- Checksum  \n"
        "|  |   |   |  +-------- Segment address  \n"
        "|  |   |   +----------- Record type  \n"
        "|  |   +--------------- Address  \n"
        "|  +------------------- Number of data bytes  \n"
        "+---------------------- Start of record  \n"
        "  \n"
        "Following data records will start at E000:0100 or E0100  \n");

	exit(0);

}



void help_mot2bin_form(void)
{

 printf("Motorola S-Record Format \n"
        "======================== \n"
        "\n"
        "    A file in Motorola S-record format is an ASCII file. There are three different  \n"
        "    formats:   \n"
        "   \n"
        "        S19     for 16-bit address   \n"
        "        S2      for 24-bit address   \n"
        "        S3      for 32-bit address   \n"
        "   \n"
        "   \n"
        "    The files consist of optional symbol table information, data specifications    \n"
        "    for loading memory, and a terminator record.   \n"
        "   \n"
        "        [ $$ {module_record}   \n"
        "        symbol records   \n"
        "        $$ [ module_record ]   \n"
        "        symbol_records   \n"
        "        $$]   \n"
        "        header_record   \n"
        "        data_records   \n"
        "        record_count_record   \n"
        "        terminator_record   \n"
        "   \n"
        "   \n"
        "Module Record (Optional)   \n"
        "   \n"
        "    Each object file contains one record for each module that is a component of it. This   \n"
        "    record contains the name of the module. There is one module record for each relocatable    \n"
        "    object created by the assembler. The name of the relocatable object module   \n"
        "    contained in the record comes from the IDNT directive. For absolute objects created   \n"
        "    by the linker, there is one module record for each relocatable object file linked,   \n"
        "    plus an additional record whose name comes from the NAME command for the   \n"
        "    linker.   \n"
        "   \n"
        "    Example:   \n"
        "        $$ MODNAME   \n"
        "   \n"
        "Symbol Record (Optional)   \n"
        "   \n"
        "    As many symbol records as needed can be contained in the object module. Up to 4   \n"
        "    symbols per line can be used, but it is not mandatory that each line contain 4    \n"
        "    symbols. A module can contain only symbol records.   \n"
        "   \n"
        "    Example:   \n"
        "        APPLE $00000 LABEL1 $ODOC3   \n"
        "        MEM $OFFFF ZEEK $01947   \n"
        "   \n"
        "    The module name associated with the symbols can be specified in the   \n"
        "    module_record preceding the symbol records.   \n"
        "   \n"
        "    Example:   \n"
        "        $$MAIN   \n"
        "   \n"
        "    Symbols are assumed to be in the module named in the preceding module_record   \n"
        "    until another module is specified with another module_record. Symbols defined by   \n"
        "    the linker's PUBLIC command appear following the first module record, which   \n"
        "    indicates the name of the output object module specified by the linker's NAME   \n"
        "    command.   \n"
        "   \n"
        "*****************************************************************************************   \n"
        "Header Record (SO)   \n"
        "   \n"
        "    Each object module has exactly one header record with the following format:   \n"
        "   \n"
        "        S00600004844521B   \n"
        "   \n"
        "    Description:   \n"
        "        S0         Identifies the record as a header record   \n"
        "        06         The number of bytes following this one   \n"
        "        0000       The address field, which is ignored   \n"
        "        484452     The string HDR in ASCII   \n"
        "        1B         The checksum   \n"
        "   \n"
        "*****************************************************************************************   \n"
        "   \n"
        "Data Record (S1)   \n"
        "   \n"
        "    A data record specifies data bytes that are to be loaded into memory. Figure 1   \n"
        "    shows the format for such a record. The columns shown in the figure represent half   \n"
        "    of a byte (4 bits).   \n"
        "   \n"
        "                ---------------------------------------------   \n"
        "                |  1 2  3 4   5 6 7 8   9 ... 40    41 42   |   \n"
        "                |                                           |   \n"
        "                |  S ID byte   load    data...data checksum |   \n"
        "                |       count address   1      n            |   \n"
        "                ---------------------------------------------   \n"
        "            Figure 1: Data Record Formatter 16-Bit Load Address \n"
        "   \n"
        "        Column      Description   \n"
        "   \n"
        "        1           Contains the ASCII character S, which indicates the start of   \n"
        "                    a record in Motorola S-record format.   \n"
        "        2           Contains the ASCII character identifying the record type.   \n"
        "                    For data records, this character is 1.   \n"
        "        3 to 4      Contain the count of the number of bytes following this one   \n"
        "                    within the record. The count includes the checksum and the   \n"
        "                    load address bytes but not the byte count itself.   \n"
        "        5 to 8      Contain the load address. The first data byte is to be loaded   \n"
        "                    into this address and subsequent bytes into the next sequential   \n"
        "                    address. Columns 5 and 6 contain the high-order address   \n"
        "                    byte, and columns 7 and 8 contain the low-order address byte.   \n"
        "        9 to 40     Contain the specifications for up to 16 bytes of data.   \n"
        "        41 to 42    Contain a checksum for the record. To calculate this, take the   \n"
        "                    sum of the values of all bytes from the byte count up to the   \n"
        "                    last data byte, inclusive, modulo 256. Subtract this result   \n"
        "                    from 255.   \n"
        "   \n"
        "*****************************************************************************************   \n"
        "   \n"
        "Data Record (S2)   \n"
        "   \n"
        "    A data record specifies data bytes that are to be loaded into memory. Figure 2   \n"
        "    shows the format for such a record. The columns shown in the figure represent half   \n"
        "    of a byte (4 bits).   \n"
        "   \n"
        "                ----------------------------------------------------   \n"
        "                |  1 2   3 4   5 6 7 8 9 10   11 ...  42   43 44   |   \n"
        "                |                                                  |   \n"
        "                |  S ID  byte     load        data...data checksum |   \n"
        "                |        count   address       1      n            |   \n"
        "                ----------------------------------------------------   \n"
        "                Figure 2: Data Record Format for 24-Bit Load Address   \n"
        "   \n"
        "        Column      Description   \n"
        "   \n"
        "        1           Contains the ASCII character S, which indicates the start of   \n"
        "                    a record in Motorola S-record format.   \n"
        "        2           Contains the ASCII character identifying the record type.   \n"
        "                    For data records, this character is 2.   \n"
        "        3 to 4      Contain the count of the number of bytes following this one   \n"
        "                    within the record. The count includes the checksum and the   \n"
        "                    load address bytes but not the byte count itself.   \n"
        "        5 to 10     Contain the load address. The first data byte is to be loaded   \n"
        "                    into this address and subsequent bytes into the next sequential    \n"
        "                    address. Columns 5 and 6 contain the high-order address   \n"
        "                    byte, and columns 9 and 10 contain the low-order address byte.   \n"
        "        11 to 42    Contain the specifications for up to 16 bytes of data.   \n"
        "        43 to 44    Contain a checksum for the record. To calculate this, take the   \n"
        "                    sum of the values of all bytes from the byte count up to the   \n"
        "                    last data byte, inclusive, modulo 256. Subtract this result   \n"
        "                    from 255.   \n"
        "   \n"
        "*****************************************************************************************   \n"
        "   \n"
        "Data Record (S3)   \n"
        "   \n"
        "   \n"
        "    A data record specifies data bytes that are to be loaded into memory. Figure 3   \n"
        "    shows the format for such a record. The columns shown in the figure represent half   \n"
        "    of a byte (4 bits).   \n"
        "   \n"
        "                ----------------------------------------------------------   \n"
        "                |  1 2   3 4   5 6 7 8 9 10 11 12   13 ... 44    45 46   |   \n"
        "                |                                                        |   \n"
        "                |  S ID  byte        load          data...data  checksum |   \n"
        "                |        count      address         1      n             |   \n"
        "                ----------------------------------------------------------   \n"
        "                Figure 3: Data Record Format for 32-Bit Load Address   \n"
        "   \n"
        "    Column          Description   \n"
        "   \n"
        "    1               Contains the ASCII character S, which indicates the start of   \n"
        "                    a record in Motorola S-record format.   \n"
        "    2               Contains the ASCII character identifying the record type.   \n"
        "                    For data records, this digit is 3 for 32-bit addresses.   \n"
        "    3 to 4          Contain the count of the number of bytes following this one   \n"
        "                    within the record. The count includes the checksum and the   \n"
        "                    load address bytes but not the byte count itself.   \n"
        "    5 to 12         Contain the load address. The first data byte is to be loaded   \n"
        "                    into this address and subsequent bytes into the next sequential    \n"
        "                    address. Columns 5 and 6 contain the high-order address   \n"
        "                    byte, and columns 11 and 12 contain the low-order address byte.   \n"
        "    13 to 44        Contain the specifications for up to 15 bytes of data.   \n"
        "    45 to 46        Contain a checksum for the record. To calculate this, take the   \n"
        "                    sum of the values of all bytes from the byte count up to the   \n"
        "                    last data byte, inclusive, modulo 256. Subtract this result   \n"
        "                    from 255.   \n"
        "   \n"
        "*****************************************************************************************   \n"
        "   \n"
        "Record Count Record (S5)   \n"
        "   \n"
        "    The record count record verifies the number of data records preceding it. Figure 4   \n"
        "    shows the format for such a record. The columns shown in the figure represent half   \n"
        "    of a byte (4 bits).   \n"
        "   \n"
        "                --------------------------------------   \n"
        "                |  1 2   3 4      5 6 7 8     9 10   |   \n"
        "                |                                    |   \n"
        "                |  S ID  byte    # of data  checksum |   \n"
        "                |        count    records            |   \n"
        "                --------------------------------------   \n"
        "                Figure 4: Record Count Record Format   \n"
        "   \n"
        "    Column          Description   \n"
        "   \n"
        "    1               Contains the ASCII character S, which indicates the start of   \n"
        "                    a record in Motorola S-record format.   \n"
        "    2               Contains the ASCII character 5, which indicates a record   \n"
        "                    count record.   \n"
        "    3 to 4          Contain the byte count, ASCII string 03.   \n"
        "    5 to 8          Contain the number of data records in this file. The high-   \n"
        "                    order byte is in columns 5 and 6.   \n"
        "    9 to 10         Contain the checksum for the record.   \n"
        "   \n"
        "    Example:   \n"
        "        S503010DEE   \n"
        "   \n"
        "    The example above shows a record count record indicating a total of 269 records   \n"
        "    (0x010D) and a checksum of 0xEE.   \n"
        "   \n"
        "*****************************************************************************************   \n"
        "   \n"
        "Terminator Record for 32-bit address (S7)   \n"
        "   \n"
        "    A terminator record specifies the end of the data records. Figure 5 shows the   \n"
        "    format for such a record. The columns shown in the figure represent half of a byte   \n"
        "    (4 bits).   \n"
        "   \n"
        "                -------------------------------------   \n"
        "                |  1 2   3 4      5...12    13 14   |   \n"
        "                |                                   |   \n"
        "                |  S ID  byte     load     checksum |   \n"
        "                |        count   address            |   \n"
        "                -------------------------------------   \n"
        "                Figure5: Terminator Record Format for 32-Bit Load Address   \n"
        "   \n"
        "    Column          Description   \n"
        "   \n"
        "    1               Contains the ASCII character S, which indicates the start of   \n"
        "                    a record in Motorola S-record format.   \n"
        "    2               Contains the ASCII character 7, which indicates a 32-bit   \n"
        "                    load address.   \n"
        "    3 to 4          Contain the byte count, ASCII string 04.   \n"
        "    5 to 12         Contain the load address that is either set to zero or to the   \n"
        "                    starting address specified in the END directive or START   \n"
        "                    command (there are no data bytes).   \n"
        "    13 to 14        Contain the checksum for the record.   \n"
        "   \n"
        "*****************************************************************************************   \n"
        "   \n"
        "Terminator Record for 24-bit address (S8)   \n"
        "   \n"
        "    A terminator record specifies the end of the data records. Figure 6 shows the   \n"
        "    format for such a record. The columns shown in the figure represent half of a byte   \n"
        "    (4 bits).   \n"
        "   \n"
        "                ----------------------------------------   \n"
        "                |  1 2   3 4    5 6 7 8 9 10   11 12   |   \n"
        "                |                                      |   \n"
        "                |  S ID  byte       load      checksum |   \n"
        "                |        count     address             |   \n"
        "                ----------------------------------------   \n"
        "                Figure 6: Terminator Record Format for 24-Bit Load Address   \n"
        "   \n"
        "    Column          Description   \n"
        "   \n"
        "    1               Contains the ASCII character S, which indicates the start of   \n"
        "                    a record in Motorola S-record format.   \n"
        "    2               Contains the ASCII character 8, which indicates a 24-bit   \n"
        "                    load address.   \n"
        "    3 to 4          Contain the byte count, ASCII string 04.   \n"
        "    5 to 10         Contain the load address, which is either set to zero or to the   \n"
        "                    starting address specified in the END directive or START   \n"
        "                    command. There are no data bytes.   \n"
        "    11 to 12        Contain the checksum for the record.   \n"
        "   \n"
        "    Example:   \n"
        "        S804000AF0001   \n"
        "   \n"
        "    The previous example shows a terminator record with a 24-bit load address of   \n"
        "    0x000AF0 and a checksum of 0x01.   \n"
        "   \n"
        "*****************************************************************************************   \n"
        "   \n"
        "Terminator Record for 16-bit address (S9)   \n"
        "   \n"
        "   \n"
        "    A terminator record specifies the end of the data records. Figure 7 shows the   \n"
        "    format for such a record. The columns shown in the figure represent half of a byte   \n"
        "    (4 bits).   \n"
        "   \n"
        "                -------------------------------------   \n"
        "                |  1 2   3 4    5 6 7 8      9 10   |   \n"
        "                |                                   |   \n"
        "                |  S ID  byte    load      checksum |   \n"
        "                |        count  address             |   \n"
        "                -------------------------------------   \n"
        "                Figure 7: Terminator Record Format for 16-Bit Load Address   \n"
        "   \n"
        "    Column         Description   \n"
        "   \n"
        "    1               Contains the ASCII character S, which indicates the start of   \n"
        "                    a record in Motorola S-record format.   \n"
        "    2               Contains the ASCII character 9, which indicates a 16-bit   \n"
        "                    load address.   \n"
        "    3 to 4          Contain the byte count, ASCII string 04.   \n"
        "    5 to 8          Contain the load address, which is either set to zero or to the   \n"
        "                    starting address specified in the END directive or START   \n"
        "                    command (there are no data bytes).   \n"
        "    9 to 10         Contain the checksum for the record.   \n"
        "   \n"
        "*****************************************************************************************    \n" );
	exit(0);
}


void AllFilesClosed(void)
{
	if(inpfile) { fclose(inpfile);	inpfile=NULL; }
	if(outfile) { fclose(outfile);	outfile=NULL; }
	if(data_buf){ free(data_buf);	data_buf=NULL; }
}

/*===========================================================================
FUNCTION 
 main()

DESCRIPTION

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/

//++++++++++++++++ : b:PI2000
//++++++++++++++++ : m:Model Name
//++++++++++++++++ : v:Version 
//++++++++++++++++ : c:compile Date
//++++++++++++++++ :   .....
//++++++++++++++++ : i:Original OS image
//++++++++++++++++
//++++++++++++++++
//++++++++++++++++

void waiting(int cc)
{
	if( 0 == (cc%4) )
		printf("\rWaiting...-\b");
	else if( 1 == (cc%4) )
		printf("\rWaiting...\\\b");
	else if( 2 == (cc%4) )
		printf("\rWaiting...|\b");
	else if( 3 == (cc%4) )
		printf("\rWaiting.../\b");
	else
		printf("\rWaiting...+\b");

}



#if defined(_MOT2BIN_) || defined(_INTEL2BIN_)

int Pad_Byte = 0xFF;  // --join --intel --motorola
int isPadByte = 0, isPadByteAllArea=0;

boolean Enable_Checksum_Error = 1; // check checksum
boolean Status_Checksum_Error = false;
byte 	Checksum;
unsigned int Record_Nb;
	
/* This will hold binary codes translated from hex file. */
byte *Memory_Block = NULL;
unsigned int Starting_Address, Phys_Addr;
unsigned int Records_Start; // Lowest address of the records
unsigned int Max_Length = 0; // MOT2BIN_MEMORY_SIZE;
unsigned int Real_Length = 0; // 2020.06.24, real file size

unsigned int Minimum_Block_Size = 0x1000; // 4096 byte
unsigned int Floor_Address = 0x0;  
unsigned int Ceiling_Address = 0xFFFFFFFF; 
int Module;
boolean Minimum_Block_Size_Setted = 0;
boolean Starting_Address_Setted = 0;
boolean Floor_Address_Setted = 0;
boolean Ceiling_Address_Setted = 0;
boolean Max_Length_Setted = 0;
boolean Swap_Wordwise = 0;
boolean Address_Alignment_Word = 0;
boolean Batch_Mode = 0;
//boolean Verbose_Flag = 0;

int Endian = 0;  /* 0:little-endian,  1:big-endian */



// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define HEX2BIN_MOTOROLA_ZERO_FORCED 	1 // 2020.06.26
#define HEX2BIN_INTEL_ZERO_FORCED 		1 // 2020.06.30

#if 1 // HEX2BIN_MOTOROLA_ZERO_FORCED // 2020.06.24

enum {
	HEX2BIN_REAL_ADDR = 0,  // Never modified!!!
	HEX2BIN_ZERO_FORCED,    // read address is too big, so read address match to zero address!!!
	HEX2BIN_HIGH_ADDR,

	HEX2BIN_MAX = 0xff
};

int Enable_HexaAddr_Zero_Forced = HEX2BIN_REAL_ADDR; // 2016.03.10, hex file address zero-forced!!
unsigned int Phys_AddrTemp = 0x00000000; // added at 2016.03.10
unsigned int iOnceCheck = 1; // added at 2016.03.10

#endif
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#define LAST_CHECK_METHOD 5

typedef enum Crc
{
    CHK8_SUM =0,
    CHK16,
    CRC8,
    CRC16,
    CRC32,
    CHK16_8
} t_CRC;

t_CRC Cks_Type = CHK8_SUM;
unsigned int Cks_Start = 0, Cks_End = 0, Cks_Addr = 0, Cks_Value = 0;
boolean Cks_range_set = 0;
boolean Cks_Addr_set = 0;
boolean Force_Value = 0;

//char strHex2BinLen[MAX_CHARS+1];
//char strPadByte[MAX_CHARS+1];
//char strPadArea[MAX_CHARS+1];

unsigned int Crc_Poly = 0x07, Crc_Init = 0, Crc_XorOut = 0;
boolean Crc_RefIn = false;
boolean Crc_RefOut = false;


/* flag that a file was read */
boolean Fileread = 0;



/* This will hold binary codes translated from hex file. */
byte	Hex2Bin_Checksum = 0;

/* Application specific */
unsigned int 	Nb_Bytes;
unsigned int 	First_Word, Segment, Upper_Address;
unsigned int    Address, Lowest_Address, Highest_Address, Starting_Address;
unsigned int    Real_Highest_Address=0;
unsigned int 	Records_Start; // Lowest address of the records


/* This mask is for mapping the target binary inside the binary buffer. 
If for example, we are generating a binary file with records starting at FFF00000, 
the bytes will be stored at the beginning of the memory buffer.  */
unsigned int    Address_Mask = (MOT2BIN_MEMORY_SIZE-1);

unsigned int    Record_Count, Record_Checksum;

char str_address[MAX_CHARS+1];

#endif // (_MOT2BIN_) || (_INTEL2BIN_)



#if defined(_MOT2BIN_) || defined(_INTEL2BIN_)


uint8_t u16_hi(uint16_t value)
{
	return (uint8_t)((value & 0xFF00) >> 8);
}

uint8_t u16_lo(uint16_t value)
{
	return (uint8_t)(value & 0x00FF);
}

uint8_t u32_b3(uint32_t value)
{
	return (uint8_t)((value & 0xFF000000) >> 24);
}

uint8_t u32_b2(uint32_t value)
{
	return (uint8_t)((value & 0x00FF0000) >> 16);
}

uint8_t u32_b1(uint32_t value)
{
	return (uint8_t)((value & 0x0000FF00) >> 8);
}

uint8_t u32_b0(uint32_t value)
{
	return (uint8_t)(value & 0x000000FF);
}

uint8_t u64_b7(uint64_t value)
{
	return (uint8_t)((value & 0xFF00000000000000ULL) >> 56);
}

uint8_t u64_b6(uint64_t value)
{
	return (uint8_t)((value & 0x00FF000000000000ULL) >> 48);
}

uint8_t u64_b5(uint64_t value)
{
	return (uint8_t)((value & 0x0000FF0000000000ULL) >> 40);
}

uint8_t u64_b4(uint64_t value)
{
	return (uint8_t)((value & 0x000000FF00000000ULL) >> 32);
}

uint8_t u64_b3(uint64_t value)
{
	return (uint8_t)((value & 0x00000000FF000000ULL) >> 24);
}

uint8_t u64_b2(uint64_t value)
{
	return (uint8_t)((value & 0x0000000000FF0000ULL) >> 16);
}

uint8_t u64_b1(uint64_t value)
{
	return (uint8_t)((value & 0x000000000000FF00ULL) >> 8);
}

uint8_t u64_b0(uint64_t value)
{
	return (uint8_t)(value & 0x00000000000000FFULL);
}

/* Checksum/CRC conversion to ASCII */
uint8_t nibble2ascii(uint8_t value)
{
  uint8_t result = value & 0x0f;

  if (result > 9) return result + 0x41-0x0A;
  else return result + 0x30;
}

bool cs_isdecdigit(char c)
{
    return (c >= 0x30) && (c < 0x3A);
}

unsigned char tohex(unsigned char c)
{
  if ((c >= '0') && (c < '9'+1))
    return (c - '0');
  if ((c >= 'A') && (c < 'F'+1))
    return (c - 'A' + 0x0A);
  if ((c >= 'a') && (c < 'f'+1))
    return (c - 'a' + 0x0A);

  return 0;
}

unsigned char todecimal(unsigned char c)
{
  if ((c >= '0') && (c < '9'+1))
    return (c - '0');

  return 0;
}


const uint8_t Reflect8[256] = {
  0x00,0x80,0x40,0xC0,0x20,0xA0,0x60,0xE0,0x10,0x90,0x50,0xD0,0x30,0xB0,0x70,0xF0,
  0x08,0x88,0x48,0xC8,0x28,0xA8,0x68,0xE8,0x18,0x98,0x58,0xD8,0x38,0xB8,0x78,0xF8,
  0x04,0x84,0x44,0xC4,0x24,0xA4,0x64,0xE4,0x14,0x94,0x54,0xD4,0x34,0xB4,0x74,0xF4,
  0x0C,0x8C,0x4C,0xCC,0x2C,0xAC,0x6C,0xEC,0x1C,0x9C,0x5C,0xDC,0x3C,0xBC,0x7C,0xFC,
  0x02,0x82,0x42,0xC2,0x22,0xA2,0x62,0xE2,0x12,0x92,0x52,0xD2,0x32,0xB2,0x72,0xF2,
  0x0A,0x8A,0x4A,0xCA,0x2A,0xAA,0x6A,0xEA,0x1A,0x9A,0x5A,0xDA,0x3A,0xBA,0x7A,0xFA,
  0x06,0x86,0x46,0xC6,0x26,0xA6,0x66,0xE6,0x16,0x96,0x56,0xD6,0x36,0xB6,0x76,0xF6,
  0x0E,0x8E,0x4E,0xCE,0x2E,0xAE,0x6E,0xEE,0x1E,0x9E,0x5E,0xDE,0x3E,0xBE,0x7E,0xFE,
  0x01,0x81,0x41,0xC1,0x21,0xA1,0x61,0xE1,0x11,0x91,0x51,0xD1,0x31,0xB1,0x71,0xF1,
  0x09,0x89,0x49,0xC9,0x29,0xA9,0x69,0xE9,0x19,0x99,0x59,0xD9,0x39,0xB9,0x79,0xF9,
  0x05,0x85,0x45,0xC5,0x25,0xA5,0x65,0xE5,0x15,0x95,0x55,0xD5,0x35,0xB5,0x75,0xF5,
  0x0D,0x8D,0x4D,0xCD,0x2D,0xAD,0x6D,0xED,0x1D,0x9D,0x5D,0xDD,0x3D,0xBD,0x7D,0xFD,
  0x03,0x83,0x43,0xC3,0x23,0xA3,0x63,0xE3,0x13,0x93,0x53,0xD3,0x33,0xB3,0x73,0xF3,
  0x0B,0x8B,0x4B,0xCB,0x2B,0xAB,0x6B,0xEB,0x1B,0x9B,0x5B,0xDB,0x3B,0xBB,0x7B,0xFB,
  0x07,0x87,0x47,0xC7,0x27,0xA7,0x67,0xE7,0x17,0x97,0x57,0xD7,0x37,0xB7,0x77,0xF7,
  0x0F,0x8F,0x4F,0xCF,0x2F,0xAF,0x6F,0xEF,0x1F,0x9F,0x5F,0xDF,0x3F,0xBF,0x7F,0xFF,
};

uint16_t Reflect16(uint16_t Value16)
{
  return (((uint16_t) Reflect8[u16_lo(Value16)]) << 8) | ((uint16_t) Reflect8[u16_hi(Value16)]);
}

uint32_t Reflect24(uint32_t Value24)
{
  return (
		  (((uint32_t) Reflect8[u32_b0(Value24)]) << 16) |
		  (((uint32_t) Reflect8[u32_b1(Value24)]) << 8)  |
		   ((uint32_t) Reflect8[u32_b2(Value24)])
		  );
}

uint32_t Reflect32(uint32_t Value32)
{
  return (
		  (((uint32_t) Reflect8[u32_b0(Value32)]) << 24) |
		  (((uint32_t) Reflect8[u32_b1(Value32)]) << 16) |
		  (((uint32_t) Reflect8[u32_b2(Value32)]) << 8)  |
		   ((uint32_t) Reflect8[u32_b3(Value32)])
		  );
}

uint64_t Reflect40(uint64_t Value40)
{
  return (
		  (((uint64_t) Reflect8[u64_b0(Value40)]) << 32) |
		  (((uint64_t) Reflect8[u64_b1(Value40)]) << 24) |
		  (((uint64_t) Reflect8[u64_b2(Value40)]) << 16) |
		  (((uint64_t) Reflect8[u64_b3(Value40)]) << 8)  |
		   ((uint64_t) Reflect8[u64_b4(Value40)])
		  );
}

uint64_t Reflect64(uint64_t Value64)
{
  return (
		  (((uint64_t) Reflect8[u64_b0(Value64)]) << 56) |
		  (((uint64_t) Reflect8[u64_b1(Value64)]) << 48) |
		  (((uint64_t) Reflect8[u64_b2(Value64)]) << 40) |
		  (((uint64_t) Reflect8[u64_b3(Value64)]) << 32) |
		  (((uint64_t) Reflect8[u64_b4(Value64)]) << 24) |
		  (((uint64_t) Reflect8[u64_b5(Value64)]) << 16) |
		  (((uint64_t) Reflect8[u64_b6(Value64)]) << 8)  |
		   ((uint64_t) Reflect8[u64_b7(Value64)])
		  );
}



#ifndef G_GUINT64_CONSTANT
#define G_GUINT64_CONSTANT(val) (val##UL)
#endif

void *crc_table = NULL;

/* private */

void init_crc8_normal_tab(uint8_t polynom)
{
  int i, j;
  uint8_t crc;
  uint8_t *p;

  p = (uint8_t *) crc_table;

  for (i=0; i<256; i++)
    {
	  crc = (uint8_t) i;

	  for (j=0; j<8; j++)
        {
          if (crc & 0x80) crc = (crc << 1) ^ polynom;
          else            crc <<= 1;
        }
	  *p++ = crc;
    }
}

void init_crc8_reflected_tab(uint8_t polynom)
{
  int i, j;
  uint8_t crc;
  uint8_t *p;

  p = (uint8_t *) crc_table;

  for (i=0; i<256; i++)
    {
	  crc = (uint8_t) i;

	  for (j=0; j<8; j++)
        {
          if (crc & 0x01) crc = (crc >> 1) ^ polynom;
          else            crc >>= 1;
        }
	  *p++ = crc;
    }
}

/* Common routines for calculations */
void init_crc16_normal_tab(uint16_t polynom)
{
  int i, j;
  uint16_t crc;
  uint16_t *p;

  p = (uint16_t *) crc_table;

  for (i=0; i<256; i++)
    {
	  crc = ((uint16_t) i) << 8;

	  for (j=0; j<8; j++)
        {
		  if ( crc & 0x8000 ) crc = ( crc << 1 ) ^ polynom;
		  else                crc <<= 1;
        }
	  *p++ = crc;
    }
}

void init_crc16_reflected_tab(uint16_t polynom)
{
  int i, j;
  uint16_t crc;
  uint16_t *p;

  p = (uint16_t *) crc_table;

  for (i=0; i<256; i++)
    {
	  crc   = (uint16_t) i;

	  for (j=0; j<8; j++)
        {
		  if ( crc & 0x0001 ) crc = ( crc >> 1 ) ^ polynom;
		  else                crc >>= 1;
        }
	  *p++ = crc;
    }
}

void init_crc32_normal_tab(uint32_t polynom)
{
  int i, j;
  uint32_t crc;
  uint32_t *p;

  p = (uint32_t *) crc_table;

  for (i=0; i<256; i++)
    {
	  crc = ((uint32_t) i) << 24;

	  for (j=0; j<8; j++)
        {
		  if ( crc & 0x80000000L ) crc = ( crc << 1 ) ^ polynom;
		  else                     crc <<= 1;
        }
	  *p++ = crc;
    }
}

void init_crc32_reflected_tab(uint32_t polynom)
{
  int i, j;
  uint32_t crc;
  uint32_t *p;

  p = (uint32_t *) crc_table;

  for (i=0; i<256; i++)
    {
	  crc = (uint32_t) i;

	  for (j=0; j<8; j++)
        {
		  if ( crc & 0x00000001L ) crc = ( crc >> 1 ) ^ polynom;
		  else                     crc >>= 1;
        }
	  *p++ = crc;
    }
}

/* Common routines for calculations */

uint8_t update_crc8(uint8_t crc, uint8_t c)
{
  return (((uint8_t *) crc_table)[crc ^ c]);
}

uint16_t update_crc16_normal(uint16_t crc, char c )
{
  uint16_t short_c;

  short_c  = 0x00ff & (uint16_t) c;

  /* Normal form */
  return (crc << 8) ^ ((uint16_t *) crc_table)[(crc >> 8) ^ short_c];
}

uint16_t update_crc16_reflected(uint16_t crc, char c )
{
  uint16_t short_c;

  short_c  = 0x00ff & (uint16_t) c;

  /* Reflected form */
  return (crc >> 8) ^ ((uint16_t *) crc_table)[(crc ^ short_c) & 0xff];
}

uint32_t update_crc32_normal(uint32_t crc, char c )
{
  uint32_t long_c;

  long_c = 0x000000ffL & (uint32_t) c;

  return (crc << 8) ^ ((uint32_t *) crc_table)[((crc >> 24) ^ long_c) & 0xff];
}

uint32_t update_crc32_reflected(uint32_t crc, char c )
{
  uint32_t long_c;

  long_c = 0x000000ffL & (uint32_t) c;

  return (crc >> 8) ^ ((uint32_t *) crc_table)[(crc ^ long_c) & 0xff];
}



#ifdef AAAAAAAA
void usage(void)
{
    fprintf (stderr,
             "\n"
             "usage: %s [OPTIONS] filename\n"
             "Options:\n"
             "  -a            Address Alignment Word (hex2bin only)\n"
             "  -b            Batch mode: exits if specified file doesn't exist\n"
             "  -c            Enable record checksum verification\n"
             "  -C [Poly][Init][RefIn][RefOut][XorOut]\n                CRC parameters\n"
             "  -e [ext]      Output filename extension (without the dot)\n"
             "  -E [0|1]      Endian for checksum/CRC, 0: little, 1: big\n"
             "  -f [address]  Address of check result to write\n"
             "  -F [address] [value]\n                Address and value to force\n"
             "  -k [0-5]      Select check method (checksum or CRC) and size\n"
             "  -d            display list of check methods/value size\n"
             "  -l [length]   Maximal Length (Starting address + Length -1 is Max Address)\n"
             "                File will be filled with Pattern until Max Address is reached\n"
             "  -m [size]     Minimum Block Size\n"
             "                File Size Dimension will be a multiple of Minimum block size\n"
             "                File will be filled with Pattern\n"
             "                Length must be a power of 2 in hexadecimal [see -l option]\n"
             "                Attention this option is STRONGER than Maximal Length  \n"
             "  -p [value]    Pad-byte value in hex (default: %x)\n"
             "  -r [start] [end]\n"
             "                Range to compute checksum over (default is min and max addresses)\n"
             "  -s [address]  Starting address in hex for binary file (default: 0)\n"
             "                ex.: if the first record is :nn010000ddddd...\n"
             "                the data supposed to be stored at 0100 will start at 0000\n"
             "                in the binary file.\n"
             "                Specifying this starting address will put pad bytes in the\n"
             "                binary file so that the data supposed to be stored at 0100\n"
             "                will start at the same address in the binary file.\n"
             "  -t [address]  Floor address in hex (hex2bin only)\n"
             "  -T [address]  Ceiling address in hex (hex2bin only)\n"
             "  -v            Verbose messages for debugging purposes\n"
             "  -w            Swap wordwise (low <-> high)\n\n",
             Pgm_Name,Pad_Byte);
    exit(1);
} /* procedure USAGE */

void DisplayCheckMethods(void)
{
    fprintf (stderr,
             "Check methods/value size:\n"
             "0:  Checksum  8-bit\n"
             "1:  Checksum 16-bit (adds 16-bit words into a 16-bit sum, data and result BE or LE)\n"
             "2:  CRC8\n"
             "3:  CRC16\n"
             "4:  CRC32\n"
             "5:  Checksum 16-bit (adds bytes into a 16-bit sum, result BE or LE)\n");
    exit(1);
}
#endif


void *NoFailMalloc (size_t size)
{
    void *result = NULL;

    if ((result = malloc (size)) == NULL)
    {
        fprintf (stderr,"Can't allocate memory. size(%d) \n", size);
        exit(1);
    }
    return (result);
}


// 0 or 1
int GetBin(const char *str)
{
    int result;
    unsigned int value;

    result = sscanf(str,"%u",&value);

    if (result == 1) return value & 1;
    else
    {
        fprintf(stderr,"GetBin: some error occurred.\n");
        exit (1);
    }
}

int GetDec(const char *str)
{
    int result;
    unsigned int value;

    result = sscanf(str,"%u",&value);

    if (result == 1) return value;
    else
    {
        fprintf(stderr,"GetDec: some error occurred. \n");
        exit (1);
    }
}

int GetHex(const char *str)
{
    int result;
    unsigned int value;

    result = sscanf(str,"%x",&value);

    if (result == 1) return value;
    else
    {
        fprintf(stderr,"GetHex: some error occurred. \n");
        exit (1);
    }
}


// Char t/T: true f/F: false
bool GetBoolean(const char *str)
{
    int result;
    unsigned char value, temp;

    result = sscanf(str,"%c",&value);
    temp = tolower(value);

    if ((result == 1) && ((temp == 't') || (temp == 'f')))
    {
        return (temp == 't');
    }
    else
    {
        fprintf(stderr,"GetBoolean: some error occurred when parsing options.\n");
        exit (1);
    }
}


void VerifyChecksumValue(void)
{
    if ((Checksum != 0) && Enable_Checksum_Error)
	{
		fprintf(stderr,"\n[++ERROR++]Checksum error in record %d: should be %#02x \n",
			Record_Nb, (256 - Checksum) & 0xFF);
		Status_Checksum_Error = true;
	}
}

/* Check if are set Floor and Ceiling Address and range is coherent*/
void VerifyRangeFloorCeil(void)
{
    if (Floor_Address_Setted && Ceiling_Address_Setted && (Floor_Address >= Ceiling_Address))
    {
        printf("Floor address %#08x higher than Ceiling address %#08x \n",Floor_Address,Ceiling_Address);
        exit(1);
    }
}

void CrcParamsCheck(void)
{
    switch (Cks_Type)
    {
    case CRC8:
        Crc_Poly &= 0xFF;
        Crc_Init &= 0xFF;
        Crc_XorOut &= 0xFF;
        break;
    case CRC16:
        Crc_Poly &= 0xFFFF;
        Crc_Init &= 0xFFFF;
        Crc_XorOut &= 0xFFFF;
        break;
    case CRC32:
        break;
    default:
        fprintf (stderr,"See file CRC list.txt for parameters\n");
        exit(1);
    }
}

void WriteMemBlock16(uint16_t Value)
{
    if (Endian == 1) /* 1: Big Endian */
    {
        Memory_Block[Cks_Addr - Lowest_Address]    = u16_hi(Value);
        Memory_Block[Cks_Addr - Lowest_Address +1] = u16_lo(Value);
    }
    else
    {
        Memory_Block[Cks_Addr - Lowest_Address +1] = u16_hi(Value);
        Memory_Block[Cks_Addr - Lowest_Address]    = u16_lo(Value);
    }
}

void WriteMemBlock32(uint32_t Value)
{
    if (Endian == 1) /* 1: Big Endian */
    {
        Memory_Block[Cks_Addr - Lowest_Address]    = u32_b3(Value);
        Memory_Block[Cks_Addr - Lowest_Address +1] = u32_b2(Value);
        Memory_Block[Cks_Addr - Lowest_Address +2] = u32_b1(Value);
        Memory_Block[Cks_Addr - Lowest_Address +3] = u32_b0(Value);
    }
    else
    {
        Memory_Block[Cks_Addr - Lowest_Address +3] = u32_b3(Value);
        Memory_Block[Cks_Addr - Lowest_Address +2] = u32_b2(Value);
        Memory_Block[Cks_Addr - Lowest_Address +1] = u32_b1(Value);
        Memory_Block[Cks_Addr - Lowest_Address]    = u32_b0(Value);
    }
}

void WriteMemory(void)
{
	unsigned int i;

    if ((Cks_Addr >= Lowest_Address) && (Cks_Addr < Highest_Address))
    {
        if(Force_Value)
        {
            switch (Cks_Type)
            {
                case 0:
                    Memory_Block[Cks_Addr - Lowest_Address] = Cks_Value;
                    fprintf(stdout,"Addr %#08x set to %#02x \n",Cks_Addr, Cks_Value);
                    break;
                case 1:
                    WriteMemBlock16(Cks_Value);
                    fprintf(stdout,"Addr %#08x set to %#04x \n",Cks_Addr, Cks_Value);
                    break;
                case 2:
                    WriteMemBlock32(Cks_Value);
                    fprintf(stdout,"Addr %#08x set to %#08x \n",Cks_Addr, Cks_Value);
                    break;
                default:;
            }
        }
        else if (Cks_Addr_set)
        {
            /* Add a checksum to the binary file */
            if (!Cks_range_set)
            {
                Cks_Start = Lowest_Address;
                Cks_End = Highest_Address;
            }
            /* checksum range MUST BE in the array bounds */

            if (Cks_Start < Lowest_Address)
            {
                fprintf(stdout,"Modifying range start from %X to %X\n",Cks_Start,Lowest_Address);
                Cks_Start = Lowest_Address;
            }
            if (Cks_End > Highest_Address)
            {
                fprintf(stdout,"Modifying range end from %X to %X\n",Cks_End,Highest_Address);
                Cks_End = Highest_Address;
            }

            crc_table = NULL;

            switch (Cks_Type)
            {
            case CHK8_SUM:
            {
                uint8_t wCKS = 0;

                for (i=Cks_Start; i<=Cks_End; i++)
                {
                    wCKS += Memory_Block[i - Lowest_Address];
                }

                fprintf(stdout,"8-bit Checksum = %#02x\n",wCKS & 0xff);
                Memory_Block[Cks_Addr - Lowest_Address] = wCKS;
                fprintf(stdout,"Addr %#08x set to %#02x\n",Cks_Addr, wCKS);
            }
            break;

            case CHK16:
            {
                uint16_t wCKS, w;

                wCKS = 0;

                if (Endian == 1) /* 1: Big Endian */
                {
                    for (i=Cks_Start; i<=Cks_End; i+=2)
                    {
                        w =  Memory_Block[i - Lowest_Address +1] | ((word)Memory_Block[i - Lowest_Address] << 8);
                        wCKS += w;
                    }
                }
                else
                {
                    for (i=Cks_Start; i<=Cks_End; i+=2)
                    {
                        w =  Memory_Block[i - Lowest_Address] | ((word)Memory_Block[i - Lowest_Address +1] << 8);
                        wCKS += w;
                    }
                }
                fprintf(stdout,"16-bit Checksum = %#04x\n",wCKS);
                WriteMemBlock16(wCKS);
                fprintf(stdout,"Addr %08X set to %04X\n",Cks_Addr, wCKS);
            }
            break;

            case CHK16_8:
            {
                uint16_t wCKS;

                wCKS = 0;

                for (i=Cks_Start; i<=Cks_End; i++)
                {
                    wCKS += Memory_Block[i - Lowest_Address];
                }

                fprintf(stdout,"16-bit Checksum = %04X\n",wCKS);
                WriteMemBlock16(wCKS);
                fprintf(stdout,"Addr %08X set to %04X\n",Cks_Addr, wCKS);
            }
            break;

            case CRC8:
            {
                uint8_t CRC8;
                crc_table = NoFailMalloc(256);

                if (Crc_RefIn)
                {
                    init_crc8_reflected_tab(Reflect8[Crc_Poly]);
                    CRC8 = Reflect8[Crc_Init];
                }
                else
                {
                    init_crc8_normal_tab(Crc_Poly);
                    CRC8 = Crc_Init;
                }

                for (i=Cks_Start; i<=Cks_End; i++)
                {
                    CRC8 = update_crc8(CRC8,Memory_Block[i - Lowest_Address]);
                }

                CRC8 = (CRC8 ^ Crc_XorOut) & 0xff;
                Memory_Block[Cks_Addr - Lowest_Address] = CRC8;
                fprintf(stdout,"Addr %08X set to %02X\n",Cks_Addr, CRC8);
            }
            break;

            case CRC16:
            {
                uint16_t CRC16;
                crc_table = NoFailMalloc(256 * 2);

                if (Crc_RefIn)
                {
                    init_crc16_reflected_tab(Reflect16(Crc_Poly));
                    CRC16 = Reflect16(Crc_Init);

                    for (i=Cks_Start; i<=Cks_End; i++)
                    {
                        CRC16 = update_crc16_reflected(CRC16,Memory_Block[i - Lowest_Address]);
                    }
                }
                else
                {
                    init_crc16_normal_tab(Crc_Poly);
                    CRC16 = Crc_Init;


                    for (i=Cks_Start; i<=Cks_End; i++)
                    {
                        CRC16 = update_crc16_normal(CRC16,Memory_Block[i - Lowest_Address]);
                    }
                }

                CRC16 = (CRC16 ^ Crc_XorOut) & 0xffff;
                WriteMemBlock16(CRC16);
                fprintf(stdout,"Addr %08X set to %04X\n",Cks_Addr, CRC16);
            }
            break;

            case CRC32:
            {
                uint32_t CRC32;

                crc_table = NoFailMalloc(256 * 4);
                if (Crc_RefIn)
                {
                    init_crc32_reflected_tab(Reflect32(Crc_Poly));
                    CRC32 = Reflect32(Crc_Init);

                    for (i=Cks_Start; i<=Cks_End; i++)
                    {
                        CRC32 = update_crc32_reflected(CRC32,Memory_Block[i - Lowest_Address]);
                    }
                }
                else
                {
                    init_crc32_normal_tab(Crc_Poly);
                    CRC32 = Crc_Init;

                    for (i=Cks_Start; i<=Cks_End; i++)
                    {
                        CRC32 = update_crc32_normal(CRC32,Memory_Block[i - Lowest_Address]);
                    }
                }

                CRC32 ^= Crc_XorOut;
                WriteMemBlock32(CRC32);
                fprintf(stdout,"Addr %#08x set to %#08x\n",Cks_Addr, CRC32);
            }
            break;

            default:
                ;
            }

            if (crc_table != NULL) free(crc_table);
        }
    }
    else
    {
        if(Force_Value || Cks_Addr_set)
        {
            fprintf (stderr,"Force/Check address outside of memory range\n");
        }
    }

    /* write binary file */
	if(outfile)
	{
	    fwrite (Memory_Block,
	            Max_Length,
	            1,
	            outfile);
	}

    free (Memory_Block);


    // Minimum_Block_Size is set; the memory buffer is multiple of this?
    if ( Minimum_Block_Size_Setted==1 )
    {
        Module = Max_Length % Minimum_Block_Size;
        if (Module)
        {
            Module = Minimum_Block_Size - Module;
            Memory_Block = (byte *) NoFailMalloc(Module);
	#if 0 // 2020.06.24
            memset (Memory_Block,Pad_Byte,Module);
	#else
			if( 1==isPadByteAllArea ) // original version 2.5
				memset (Memory_Block,Pad_Byte,Module);
			else
				memset (Memory_Block,0xff /*Pad_Byte*/,Module);
	#endif
	
			if(outfile)
			{
	            fwrite (Memory_Block,
	                    Module,
	                    1,
	                    outfile);
			}

            if(Memory_Block) free (Memory_Block);

            if (Max_Length_Setted==1)
                fprintf(stdout,"Attention Max Length changed by Minimum Block Size\n");

            // extended 
            Max_Length += Module;
            Highest_Address += Module;
            fprintf(stdout,"Extended\nHighest address:  %#08x\n",Highest_Address);
            fprintf(stdout,"Max Length:       %u\n\n",Max_Length);

        }
    }
}


void Allocate_Memory_And_Rewind(void)
{

    Records_Start = Lowest_Address;

    if( (Starting_Address_Setted ==1) )
    {
        Lowest_Address = Starting_Address;
    }
    else
    {
        Starting_Address = Lowest_Address;
    }

    if (Max_Length_Setted == 0)
    {
        Max_Length = Highest_Address - Lowest_Address + 1;
    }
    else
    {
        Highest_Address = Lowest_Address + Max_Length - 1;

	#if 1 // HEX2BIN_MOTOROLA_ZERO_FORCED // 2020.06.24
		if( (Real_Length+1 > Max_Length) || (Real_Length > Highest_Address) )
		{
			// Real_Length(0xD34B) Max_Length(0xC800) Highest_Address (0xC7FF)
			printf("\n\nMax. length (%#x) is too small (%#x != %#x). \n", Max_Length, Real_Length, Highest_Address );

			if( HEX2BIN_HIGH_ADDR != Enable_HexaAddr_Zero_Forced ) // 2018.07.05
			{
				Highest_Address = Real_Length;
				Max_Length = Real_Length+1;
			}
		}
	#endif

    }
	printf("\n");
	printf("--------------------------------------------------------\n");
	printf("Converting hexa to binary \n");
	printf("--------------------------------------------------------\n");
	printf("Lowest address    : %#010x \n",Lowest_Address);
	printf("Highest address   : %#010x \n",Highest_Address);
	printf("Starting address  : %#010x \n",Starting_Address);
	printf("Bin file length   : %#010x (%u) -> %.2fkB \n",(Real_Length+1), (Real_Length+1), (float)(Real_Length+1)/1024.0 ); // bescause of 0,1..
	printf("Max Length        : %#010x (%u) -> %.2fkB \n",Max_Length, Max_Length, (float)(Max_Length/1024.0) );



#if defined(HEX2BIN_MOTOROLA_ZERO_FORCED) || defined(HEX2BIN_INTEL_ZERO_FORCED)// 2016.03.10
	// ----------------------------------------
	// Hex Address Zero based converted 
	// ----------------------------------------
	if( HEX2BIN_ZERO_FORCED == Enable_HexaAddr_Zero_Forced )
	{
		if( Phys_AddrTemp )
		{
			printf("------ [ Zero-Address Forced Logic ] -------------------\n");
			printf("Hex file real start addr : 0x%08X \n", Lowest_Address + Phys_AddrTemp );
			printf("Hex real Highest address : 0x%08X <- %.2fkB \n", Highest_Address + Phys_AddrTemp, (float)(Highest_Address + Phys_AddrTemp)/1024.0 );
			printf("Hex real starting addr.  : 0x%08X \n", Starting_Address + Phys_AddrTemp );
			printf("--------------------------------------------------------\n");
		}
	}
#endif



	if( Max_Length > MAX_BIN_LENGTH_SIZE ) // 2016.03.07
	{
		printf("\n");
		printf("The parsed binary length is %.2fMB. Maximum binary limit is %.2fMBytes!! \n", (float)Max_Length/(1024.0*1024.0), MAX_BIN_LENGTH_SIZE/1024.0/1024.0 );
		printf("Use the Zero-Address Forced option!!! ( --zeroforced ) \n\n"); 

		AllFilesClosed();		
		exit(0);
		return;
	}
	else
	{

    /* Now that we know the buffer size, we can allocate it. */
    /* allocate a buffer */
    Memory_Block = (byte *) NoFailMalloc(Max_Length);

	}
    /* For EPROM or FLASH memory types, fill unused bytes with FF or the value specified by the p option */

#if 0 // 2020.06.24
    memset (Memory_Block,Pad_Byte,Max_Length);
#else

	if( 1==isPadByteAllArea ) // original version 2.5
	{
		memset (Memory_Block, Pad_Byte, Max_Length);
	}
	else
	{
		memset (Memory_Block,0xff /*Pad_Byte*/,Max_Length);
		if( Max_Length-1 > Real_Length )
		memset (&Memory_Block[Real_Length+1],Pad_Byte, (Max_Length-Real_Length-1) );
	}
#endif

    rewind(inpfile);
}



char *ReadDataBytes(char *p)
{
	unsigned int i,temp2;
	int result;

	/* Read the Data bytes. */
	/* Bytes are written in the Memory block even if checksum is wrong. */
	i = Nb_Bytes;

	do
	{
		result = sscanf (p, "%2x",&temp2);
		if (result != 1) fprintf(stderr,"Error in line %d of hex file. \n", Record_Nb);
		p += 2;

		/* Check that the physical address stays in the buffer's range. */

		if (Phys_Addr < Max_Length)
		{
			/* Overlapping record will erase the pad bytes */
			if (Swap_Wordwise)
			{
				if (Memory_Block[Phys_Addr ^ 1] != Pad_Byte) fprintf(stderr,"\bOverlapped record detected (%d) Pad(%#08x -> %#x) \r", Swap_Wordwise, Phys_Addr, Pad_Byte);
				Memory_Block[Phys_Addr++ ^ 1] = temp2;
			}
			else
			{
				// ~if (Memory_Block[Phys_Addr] != Pad_Byte) fprintf(stderr,"\bOverlapped record detected Pad(%#08x -> %#x) \r", Phys_Addr, Pad_Byte);
				Memory_Block[Phys_Addr++] = temp2;
			}

			Checksum = (Checksum + temp2) & 0xFF;
		}
	}
	while (--i != 0);

	return p;
}

#ifdef BBBBBB
void ParseOptions(int argc, char *argv[])
{
int Param;
char *p;

    Starting_Address = 0;

    /* Parse options on the command line
    variables:
    use p for parsing arguments
    use i for number of parameters to skip
    use c for the current option
    */
    for (Param = 1; Param < argc; Param++)
    {
        int i = 0;
        char c;

        p = argv[Param];
        c = *(p+1); /* Get option character */

		if ( _IS_OPTION_(*p) )
        {
            // test for no space between option and parameter
            if (strlen(p) != 2) usage();

            switch(c)
            {
            //case 'a':
            //    Address_Alignment_Word = true;
                i = 0;
                break;
			case 'b':
				Batch_Mode = true;
				i = 0;
				break;
            //case 'c':
            //    Enable_Checksum_Error = true;
                i = 0;
                break;
            case 'd':
                DisplayCheckMethods();
            //case 'e':
            //    GetExtension(argv[Param + 1],Extension);
            //    i = 1; /* add 1 to Param */
            //    break;
            case 'E':
                Endian = GetBin(argv[Param + 1]);
                i = 1; /* add 1 to Param */
                break;
            case 'f':
                Cks_Addr = GetHex(argv[Param + 1]);
                Cks_Addr_set = true;
                i = 1; /* add 1 to Param */
                break;
            case 'F':
                Cks_Addr = GetHex(argv[Param + 1]);
                Cks_Value = GetHex(argv[Param + 2]);
                Force_Value = true;
                i = 2; /* add 2 to Param */
                break;
           // case 'k':
           //     Cks_Type = GetHex(argv[Param + 1]);
           //     {
           //         if (Cks_Type > LAST_CHECK_METHOD) usage();
           //     }
           //     i = 1; /* add 1 to Param */
           //     break;
           // case 'l':
           //     Max_Length = GetHex(argv[Param + 1]);
			////	if (Max_Length > 0x800000)
			//	{
			//		fprintf(stderr,"Max_Length = %u\n", Max_Length);
					exit(1);
			//	}
           //     Max_Length_Setted = true;
           ////     i = 1; /* add 1 to Param */
           //     break;
            case 'm':
                Minimum_Block_Size = GetHex(argv[Param + 1]);
                Minimum_Block_Size_Setted = true;
                i = 1; /* add 1 to Param */
                break;
            case 'p':
                Pad_Byte = GetHex(argv[Param + 1]);
                i = 1; /* add 1 to Param */
                break;
            case 'r':
                Cks_Start = GetHex(argv[Param + 1]);
                Cks_End = GetHex(argv[Param + 2]);
                Cks_range_set = true;
                i = 2; /* add 2 to Param */
                break;
           // case 's':
           //     Starting_Address = GetHex(argv[Param + 1]);
           //     Starting_Address_Setted = true;
           //     i = 1; /* add 1 to Param */
           //     break;
            //case 'v':
            //    Verbose_Flag = true;
            //    i = 0;
           //     break;
      		case 't':
                Floor_Address = GetHex(argv[Param + 1]);
                Floor_Address_Setted = true;
                i = 1; /* add 1 to Param */
                break;
            case 'T':
                Ceiling_Address = GetHex(argv[Param + 1]);
                Ceiling_Address_Setted = true;
                i = 1; /* add 1 to Param */
                break;
            case 'w':
                Swap_Wordwise = true;
                i = 0;
                break;
            case 'C':
                Crc_Poly = GetHex(argv[Param + 1]);
                Crc_Init = GetHex(argv[Param + 2]);
                Crc_RefIn = GetBoolean(argv[Param + 3]);
                Crc_RefOut = GetBoolean(argv[Param + 4]);
                Crc_XorOut = GetHex(argv[Param + 5]);
                CrcParamsCheck();
                i = 5; /* add 5 to Param */
                break;

            } /* switch */


        }
        else
            break;
        /* if option */
    } /* for Param */
}
#endif


UINT str2hex(char *str)
{
	int i;
	UINT number=0; 
	int order=1;
	char ch;

	for(i=strlen(str)-1; i>=0; i--)
	{
		ch=str[i];

		if(ch=='x' || ch=='X') break;

		if(ch=='.' || ch=='-') return 0; 

		if(ch>='0' && ch<='9')
		{
		    number+=order*(ch-'0');
		    order*=16;
		}
	
		if(ch>='A' && ch<='F')
		{
		    number+=order*(ch-'A'+10);
		    order*=16;
		}
		if(ch>='a' && ch<='f')
		{
		    number+=order*(ch-'a'+10);
		    order*=16;
		}

	}
	return number;
}


UINT str2int(char *str)
{
	int i;
	UINT number=0; 
	int order=1;
	char ch;

	for(i=strlen(str)-1;i>=0;i--)
	{
		ch=str[i];
#if 0
		if(ch=='x' || ch=='X')break;
#else
		if(ch=='.' || ch=='-') return 0; 
#endif

		if(ch>='0' && ch<='9')
		{
		    number+=order*(ch-'0');
		    order*=10;
		}

	}
	return number;
}


#endif /// _MOT2BIN_ || _INTEL2BIN_





#if ELF2BIN
/*  Copyright (c) 1992-2005 CodeGen, Inc.  All rights reserved. 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met: 
*  1. Redistributions of source code must retain the above copyright 
*     notice, this list of conditions and the following disclaimer. 
*  2. Redistributions in binary form must reproduce the above copyright? 
*     notice, this list of conditions and the following disclaimer in the 
*     documentation and/or other materials provided with the distribution. 
*  3. Redistributions in any form must be accompanied by information on 
*     how to obtain complete source code for the CodeGen software and any 
*     accompanying software that uses the CodeGen software.  The source code 
*     must either be included in the distribution or be available for no 
*     more than the cost of distribution plus a nominal fee, and must be 
*     freely redistributable under reasonable conditions.  For an 
*     executable file, complete source code means the source code for all 
*     modules it contains.  It does not include source code for modules or 
*     files that typically accompany the major components of the operating 
*     system on which the executable file runs.  It does not include 
*     source code generated as output by a CodeGen compiler. 
* 
*  THIS SOFTWARE IS PROVIDED BY CODEGEN AS IS AND ANY EXPRESS OR IMPLIED 
*  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
*  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
*  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, 
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
*  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
*  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
* 
*  (Commercial source/binary licensing and support is also available. 
*   Please contact CodeGen for details. http://www.codegen.com/) 
*/ 
 
/* (c) Copyright 1997-2003 by CodeGen, Inc.  All Rights Reserved. */ 
 
/* Program to dump out TekHEX from an ELF binary to aid in building 
   a PROM image for Peritek AGP. 
   ELF definitions copied from OpenBSD.  Original copyrights follow: 
 */ 
 
/*  $OpenBSD: exec_elf.h,v 1.10 1996/12/11 05:55:33 imp Exp $   */ 
/* 
 * Copyright (c) 1995, 1996 Erik Theisen.  All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. The name of the author may not be used to endorse or promote products 
 *    derived from this software without specific prior written permission 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */ 
 
 
///#include <stdio.h> 
///#include <stdlib.h> 
///#include <sys/types.h> 
///#include <sys/stat.h> 

#define ELF_LITTLE_ENDIAN 			1

#define	ELFCLASSNONE		0	/* Invalid class */
#define	ELFCLASS32	      	1   /* 32 bit objects */
#define	ELFCLASS64	       	2   /* 64 bit objects */
#define	ELFCLASSNUM			3   /* ELF class number */


#define ET_NONE  	0    /* 파일의 타입이 없음 */
#define ET_REL    	1    /* 재배치 가능 파일 */
#define ET_EXEC   	2    /* 실행 화일 */
#define ET_DYN    	3    /* 공유 object 파일 */
#define ET_CORE   	4    /* core 파일 */
#define ET_LOPROC 	0xff00 /* 프로세서에 의존적인 파일 */


#define EM_NONE  		0    /* 특정 machine을 구분하지 않음 */
#define EM_M32   		1     /* AT&T WE32100 */
#define EM_SPARC 		2     /* SPARC */
#define EM_386   		3      /* Intel 80386 */
#define EM_68K   		4      /* Motorola 68000 */
#define EM_88K   		5      /* Motorola 88000 */
#define EM_486   		6      /* 사용되지 않음 */ 
#define EM_860   		7      /* Intel 80860 */

#define EM_MIPS_RS4_BE 10	/* MIPS R4000 big-endian */
#define EM_PARISC      15	/* HPPA */
#define EM_SPARC32PLUS 18/* Sun's "v8plus" */
#define EM_PPC	   20	/* PowerPC */
#define EM_SH	   42	/* SuperH */
#define EM_SPARCV9   43	/* SPARC v9 64-bit */
#define EM_IA_64	   50	/* HP/Intel IA-64 */
#define EM_X8664       62  /* AMD x86-64 */
#define EM_ALPHA	   0x9026 /* Alpha */
#define EM_S390        0xA390 /* IBM S390 */


/* 
 * These typedefs need to be handled better - 
 *  doesn't work on 64-bit machines.  Note: 
 *  there currently isn't a 64-bit ABI. 
 */ 
typedef u_int32_t   Elf32_Addr; /* Unsigned program address */ 
typedef u_int32_t   Elf32_Off;  /* Unsigned file offset */ 
typedef int32_t     Elf32_Sword;    /* Signed large integer */ 
typedef u_int32_t   Elf32_Word; /* Unsigned large integer */ 
typedef u_int16_t   Elf32_Half; /* Unsigned medium integer */ 
 
/* e_ident[] identification indexes */ 
#define EI_MAG0     0       /* file ID */ 
#define EI_MAG1     1       /* file ID */ 
#define EI_MAG2     2       /* file ID */ 
#define EI_MAG3     3       /* file ID */ 
#define EI_CLASS    4       /* file class */ 
#define EI_DATA     5       /* data encoding */ 
#define EI_VERSION  6       /* ELF header version */ 
#define EI_PAD      7       /* start of pad bytes */ 
#define EI_NIDENT   16      /* Size of e_ident[] */ 
 
/* e_ident[] magic number */ 
#define ELFMAG0     0x7f        /* e_ident[EI_MAG0] */ 
#define ELFMAG1     'E'     /* e_ident[EI_MAG1] */ 
#define ELFMAG2     'L'     /* e_ident[EI_MAG2] */ 
#define ELFMAG3     'F'     /* e_ident[EI_MAG3] */ 
#define ELFMAG      "\177ELF"   /* magic */ 
#define SELFMAG     4       /* size of magic */ 
 
/* e_ident[] file class */ 
#define ELFCLASSNONE    0       /* invalid */ 
#define ELFCLASS32  1       /* 32-bit objs */ 
#define ELFCLASS64  2       /* 64-bit objs */ 
#define ELFCLASSNUM 3       /* number of classes */ 
 
/* e_ident[] data encoding */ 
#define ELFDATANONE 0       /* invalid */ 
#define ELFDATA2LSB 1       /* Little-Endian */ 
#define ELFDATA2MSB 2       /* Big-Endian */ 
#define ELFDATANUM  3       /* number of data encode defines */ 
 
/* e_ident */ 
#define IS_ELF(ehdr) ((ehdr).e_ident[EI_MAG0] == ELFMAG0 && \
                      (ehdr).e_ident[EI_MAG1] == ELFMAG1 && \
                      (ehdr).e_ident[EI_MAG2] == ELFMAG2 && \
                      (ehdr).e_ident[EI_MAG3] == ELFMAG3)


/* ELF Header */ 
typedef struct elfhdr 
{ 
    unsigned char e_ident[EI_NIDENT];   /* ELF Identification */ 
    Elf32_Half e_type;      /* object file type */ 
    Elf32_Half e_machine;   /* machine */ 
    Elf32_Word e_version;   /* object file version */ 
    Elf32_Addr e_entry;     /* virtual entry point */ 
    Elf32_Off e_phoff;      /* program header table offset */ 
    Elf32_Off e_shoff;      /* section header table offset */ 
    Elf32_Word e_flags;     /* processor-specific flags */ 
    Elf32_Half e_ehsize;    /* ELF header size */ 
    Elf32_Half e_phentsize; /* program header entry size */ 
    Elf32_Half e_phnum;     /* number of program header entries */ 
    Elf32_Half e_shentsize; /* section header entry size */ 
    Elf32_Half e_shnum;     /* number of section header entries */ 
    Elf32_Half e_shstrndx;  /* section header table's "section header 
                   string table" entry offset */ 
} Elf32_Ehdr; 
 
/* e_type */ 
#define ET_NONE     0       /* No file type */ 
#define ET_REL      1       /* relocatable file */ 
#define ET_EXEC     2       /* executable file */ 
#define ET_DYN      3       /* shared object file */ 
#define ET_CORE     4       /* core file */ 
#define ET_NUM      5       /* number of types */ 
#define ET_LOPROC   0xff00      /* reserved range for processor */ 
#define ET_HIPROC   0xffff      /*  specific e_type */ 
 
/* e_machine */ 
#define EM_NONE     0       /* No Machine */ 
#define EM_M32      1       /* AT&T WE 32100 */ 
#define EM_SPARC    2       /* SPARC */ 
#define EM_386      3       /* Intel 80386 */ 
#define EM_68K      4       /* Motorola 68000 */ 
#define EM_88K      5       /* Motorola 88000 */ 
#define EM_486      6       /* Intel 80486 - unused? */ 
#define EM_860      7       /* Intel 80860 */ 
#define EM_MIPS     8       /* MIPS R3000 Big-Endian only */ 
/*  
 * Don't know if EM_MIPS_RS4_BE, 
 * EM_SPARC64, EM_PARISC, 
 * or EM_PPC are ABI compliant 
 */ 
#define EM_MIPS_RS4_BE  10      /* MIPS R4000 Big-Endian */ 
#define EM_SPARC64  11      /* SPARC v9 64-bit unoffical */ 
#define EM_PARISC   15      /* HPPA */ 
#define EM_PPC      20      /* PowerPC */ 
#define EM_NUM      13      /* number of machine types */ 
 
/* Version */ 
#define EV_NONE     0       /* Invalid */ 
#define EV_CURRENT  1       /* Current */ 
#define EV_NUM      2       /* number of versions */ 
 
/* Section Header */ 
typedef struct 
{ 
    Elf32_Word sh_name;     /* name - index into section header string 
                   table section */ 
    Elf32_Word sh_type;     /* type */ 
    Elf32_Word sh_flags;    /* flags */ 
    Elf32_Addr sh_addr;     /* address */ 
    Elf32_Off sh_offset;    /* file offset */ 
    Elf32_Word sh_size;     /* section size */ 
    Elf32_Word sh_link;     /* section header table index link */ 
    Elf32_Word sh_info;     /* extra information */ 
    Elf32_Word sh_addralign;    /* address alignment */ 
    Elf32_Word sh_entsize;  /* section entry size */ 
} Elf32_Shdr; 
 
/* Special Section Indexes */ 
#define SHN_UNDEF   0       /* undefined */ 
#define SHN_LORESERVE   0xff00      /* lower bounds of reserved indexes */ 
#define SHN_LOPROC  0xff00      /* reserved range for processor */ 
#define SHN_HIPROC  0xff1f      /*   specific section indexes */ 
#define SHN_ABS     0xfff1      /* absolute value */ 
#define SHN_COMMON  0xfff2      /* common symbol */ 
#define SHN_HIRESERVE   0xffff      /* upper bounds of reserved indexes */ 
 
/* sh_type */ 
#define SHT_NULL    0       /* inactive */ 
#define SHT_PROGBITS    1       /* program defined information */ 
#define SHT_SYMTAB  2       /* symbol table section */ 
#define SHT_STRTAB  3       /* string table section */ 
#define SHT_RELA    4       /* relocation section with addends*/ 
#define SHT_HASH    5       /* symbol hash table section */ 
#define SHT_DYNAMIC 6       /* dynamic section */ 
#define SHT_NOTE    7       /* note section */ 
#define SHT_NOBITS  8       /* no space section */ 
#define SHT_REL     9       /* relation section without addends */ 
#define SHT_SHLIB   10      /* reserved - purpose unknown */ 
#define SHT_DYNSYM  11      /* dynamic symbol table section */ 
#define SHT_NUM     12      /* number of section types */ 
#define SHT_LOPROC  0x70000000  /* reserved range for processor */ 
#define SHT_HIPROC  0x7fffffff  /*  specific section header types */ 
#define SHT_LOUSER  0x80000000  /* reserved range for application */ 
#define SHT_HIUSER  0xffffffff  /*  specific indexes */ 
 
/* Section names */ 
#define ELF_BSS         ".bss"      /* uninitialized data */ 
#define ELF_DATA        ".data"     /* initialized data */ 
#define ELF_DEBUG       ".debug"    /* debug */ 
#define ELF_DYNAMIC     ".dynamic"  /* dynamic linking information */ 
#define ELF_DYNSTR      ".dynstr"   /* dynamic string table */ 
#define ELF_DYNSYM      ".dynsym"   /* dynamic symbol table */ 
#define ELF_FINI        ".fini"     /* termination code */ 
#define ELF_GOT         ".got"      /* global offset table */ 
#define ELF_HASH        ".hash"     /* symbol hash table */ 
#define ELF_INIT        ".init"     /* initialization code */ 
#define ELF_REL_DATA    ".rel.data" /* relocation data */ 
#define ELF_REL_FINI    ".rel.fini" /* relocation termination code */ 
#define ELF_REL_INIT    ".rel.init" /* relocation initialization code */ 
#define ELF_REL_DYN     ".rel.dyn"  /* relocaltion dynamic link info */ 
#define ELF_REL_RODATA  ".rel.rodata"   /* relocation read-only data */ 
#define ELF_REL_TEXT    ".rel.text" /* relocation code */ 
#define ELF_RODATA      ".rodata"   /* read-only data */ 
#define ELF_SHSTRTAB    ".shstrtab" /* section header string table */ 
#define ELF_STRTAB      ".strtab"   /* string table */ 
#define ELF_SYMTAB      ".symtab"   /* symbol table */ 
#define ELF_TEXT        ".text"     /* code */ 
 
 
/* Section Attribute Flags - sh_flags */ 
#define SHF_WRITE   	0x1     /* Writable */ 
#define SHF_ALLOC   	0x2     /* occupies memory */ 
#define SHF_EXECINSTR   0x4     /* executable */ 
#define SHF_MASKPROC    0xf0000000  /* reserved bits for processor */ 
                    /*  specific section attributes */ 
 
/* Symbol Table Entry */ 
typedef struct elf32_sym 
{ 
    Elf32_Word st_name;     /* name - index into string table */ 
    Elf32_Addr st_value;    /* symbol value */ 
    Elf32_Word st_size;     /* symbol size */ 
    unsigned char st_info;  /* type and binding */ 
    unsigned char st_other; /* 0 - no defined meaning */ 
    Elf32_Half st_shndx;    /* section header index */ 
} Elf32_Sym; 
 
/* Symbol table index */ 
#define STN_UNDEF   0       /* undefined */ 
 
/* Extract symbol info - st_info */ 
#define ELF32_ST_BIND(x)    ((x) >> 4) 
#define ELF32_ST_TYPE(x)    (((unsigned int) x) & 0xf) 
#define ELF32_ST_INFO(b,t)  (((b) << 4) + ((t) & 0xf)) 
 
/* Symbol Binding - ELF32_ST_BIND - st_info */ 
#define STB_LOCAL   0       /* Local symbol */ 
#define STB_GLOBAL  1       /* Global symbol */ 
#define STB_WEAK    2       /* like global - lower precedence */ 
#define STB_NUM     3       /* number of symbol bindings */ 
#define STB_LOPROC  13      /* reserved range for processor */ 
#define STB_HIPROC  15      /*  specific symbol bindings */ 
 
/* Symbol type - ELF32_ST_TYPE - st_info */ 
#define STT_NOTYPE  0       /* not specified */ 
#define STT_OBJECT  1       /* data object */ 
#define STT_FUNC    2       /* function */ 
#define STT_SECTION 3       /* section */ 
#define STT_FILE    4       /* file */ 
#define STT_NUM     5       /* number of symbol types */ 
#define STT_LOPROC  13      /* reserved range for processor */ 
#define STT_HIPROC  15      /*  specific symbol types */ 
 
/* Relocation entry with implicit addend */ 
typedef struct  
{ 
    Elf32_Addr r_offset;    /* offset of relocation */ 
    Elf32_Word r_info;      /* symbol table index and type */ 
} Elf32_Rel; 
 
/* Relocation entry with explicit addend */ 
typedef struct  
{ 
    Elf32_Addr r_offset;    /* offset of relocation */ 
    Elf32_Word r_info;      /* symbol table index and type */ 
    Elf32_Sword r_addend; 
} Elf32_Rela; 
 
/* Extract relocation info - r_info */ 
#define ELF32_R_SYM(i)      ((i) >> 8) 
#define ELF32_R_TYPE(i)     ((unsigned char) (i)) 
#define ELF32_R_INFO(s,t)   (((s) << 8) + (unsigned char)(t)) 
 
/* Program Header */ 
typedef struct 
{ 
    Elf32_Word p_type;      /* segment type */ 
    Elf32_Off p_offset;     /* segment offset */ 
    Elf32_Addr p_vaddr;     /* virtual address of segment */ 
    Elf32_Addr p_paddr;     /* physical address - ignored? */ 
    Elf32_Word p_filesz;    /* number of bytes in file for seg. */ 
    Elf32_Word p_memsz;     /* number of bytes in mem. for seg. */ 
    Elf32_Word p_flags;     /* flags */ 
    Elf32_Word p_align;     /* memory alignment */ 
} Elf32_Phdr; 
 
/* Segment types - p_type */ 
#define PT_NULL     0       /* unused */ 
#define PT_LOAD     1       /* loadable segment */ 
#define PT_DYNAMIC  2       /* dynamic linking section */ 
#define PT_INTERP   3       /* the RTLD */ 
#define PT_NOTE     4       /* auxiliary information */ 
#define PT_SHLIB    5       /* reserved - purpose undefined */ 
#define PT_PHDR     6       /* program header */ 
#define PT_NUM      7       /* Number of segment types */ 
#define PT_LOPROC   0x70000000  /* reserved range for processor */ 
#define PT_HIPROC   0x7fffffff  /*  specific segment types */ 
 
/* Segment flags - p_flags */ 
#define PF_X        0x1     /* Executable */ 
#define PF_W        0x2     /* Writable */ 
#define PF_R        0x4     /* Readable */ 
#define PF_MASKPROC 0xf0000000  /* reserved bits for processor */ 
                    /*  specific segment flags */ 
 
/* Dynamic structure */ 
typedef struct  
{ 
    Elf32_Sword d_tag;      /* controls meaning of d_val */ 
    union 
    { 
    Elf32_Word d_val;   /* Multiple meanings - see d_tag */ 
    Elf32_Addr d_ptr;   /* program virtual address */ 
    } d_un; 
} Elf32_Dyn; 
 
extern Elf32_Dyn    _DYNAMIC[]; 
 
/* Dynamic Array Tags - d_tag */ 
#define DT_NULL     0       /* marks end of _DYNAMIC array */ 
#define DT_NEEDED   1       /* string table offset of needed lib */ 
#define DT_PLTRELSZ 2       /* size of relocation entries in PLT */ 
#define DT_PLTGOT   3       /* address PLT/GOT */ 
#define DT_HASH     4       /* address of symbol hash table */ 
#define DT_STRTAB   5       /* address of string table */ 
#define DT_SYMTAB   6       /* address of symbol table */ 
#define DT_RELA     7       /* address of relocation table */ 
#define DT_RELASZ   8       /* size of relocation table */ 
#define DT_RELAENT  9       /* size of relocation entry */ 
#define DT_STRSZ    10      /* size of string table */ 
#define DT_SYMENT   11      /* size of symbol table entry */ 
#define DT_INIT     12      /* address of initialization func. */ 
#define DT_FINI     13      /* address of termination function */ 
#define DT_SONAME   14      /* string table offset of shared obj */ 
#define DT_RPATH    15      /* string table offset of library 
                      search path */ 
#define DT_SYMBOLIC 16      /* start sym search in shared obj. */ 
#define DT_REL      17      /* address of rel. tbl. w addends */ 
#define DT_RELSZ    18      /* size of DT_REL relocation table */ 
#define DT_RELENT   19      /* size of DT_REL relocation entry */ 
#define DT_PLTREL   20      /* PLT referenced relocation entry */ 
#define DT_DEBUG    21      /* bugger */ 
#define DT_TEXTREL  22      /* Allow rel. mod. to unwritable seg */ 
#define DT_JMPREL   23      /* add. of PLT's relocation entries */ 
#define DT_NUM      24      /* Number used. */ 
#define DT_LOPROC   0x70000000  /* reserved range for processor */ 
#define DT_HIPROC   0x7fffffff  /*  specific dynamic array tags */ 
 
/* Standard ELF hashing function */ 
unsigned long elf_hash(const unsigned char *name); 
 
#define ELF_TARG_VER    1   /* The ver for which this code is intended */ 
 
int do_swap = 0; 
 
static void elf_swap(char *mem, int len) 
{ 
	int t, i; 
 
    for (i = 0; i < len / 2; i++) 
    { 
        t = mem[i]; 
        mem[i] = mem[len - i - 1]; 
        mem[len - i - 1] = t; 
    } 
} 
 
void elf_dump_bin(char *data, unsigned long size) 
{ 
// fprintf(stderr, "dump: data=%p size=%#lx\n", data, size); 
	fwrite(data, sizeof *data, size, outfile); 

	#if 0
	if(verbose)
		fwrite(data, sizeof *data, size, stdout); 
	#endif
} 
 
/* dump a bunch of zero bytes to make sure that segments like bss 
  are initialized correctly 
*/ 
void elf_dump_zero(unsigned long size) 
{ 
// fprintf(stderr, "zero: size=%#lx\n", size); 
 
    while (size-- > 0) 
   	{
   	#if 0
		if(verbose)
	        putc('\0', stdout); 
	#endif
	
		fputc('\0', outfile); 
   	}
} 
 
void elf_swap_ehdr(Elf32_Ehdr *hdr) 
{ 
    if (do_swap) 
    { 
        elf_swap((char *)&hdr->e_type, sizeof hdr->e_type); 
        elf_swap((char *)&hdr->e_machine, sizeof hdr->e_machine); 
        elf_swap((char *)&hdr->e_version, sizeof hdr->e_version); 
        elf_swap((char *)&hdr->e_entry, sizeof hdr->e_entry); 
        elf_swap((char *)&hdr->e_phoff, sizeof hdr->e_phoff); 
        elf_swap((char *)&hdr->e_shoff, sizeof hdr->e_shoff); 
        elf_swap((char *)&hdr->e_flags, sizeof hdr->e_flags); 
        elf_swap((char *)&hdr->e_ehsize, sizeof hdr->e_ehsize); 
        elf_swap((char *)&hdr->e_phentsize, sizeof hdr->e_phentsize); 
        elf_swap((char *)&hdr->e_phnum, sizeof hdr->e_phnum); 
        elf_swap((char *)&hdr->e_shentsize, sizeof hdr->e_shentsize); 
        elf_swap((char *)&hdr->e_shnum, sizeof hdr->e_shnum); 
        elf_swap((char *)&hdr->e_shstrndx, sizeof hdr->e_shstrndx); 
    } 	


	if (verbose) 
	{ 
		printf("ELF2BIN>>  elf_swap_ehdr =============== \n");
		printf("e_type      : %u (0x%X) ", hdr->e_type, hdr->e_type ); 
		switch( hdr->e_type )
		{
			case ET_NONE: /* 파일의 타입이 없음 */
				printf("None type! \n");
				break;
			case ET_REL: /* 재배치 가능 파일 */
				printf("Relocation type! \n");
				break;
			case ET_EXEC: /* 실행 화일 */
				printf("Executable type! \n");
				break;
			case ET_DYN: /* 공유 object 파일 */
				printf("Sharing object type! \n");
				break;
			case ET_CORE: /* core 파일 */
				printf("Core file type! \n");
				break;
			case ET_LOPROC: /* 프로세서에 의존적인 파일 */
				printf("CPU depenenable type! \n");
				break;
			default:
				printf("unknown type! \n");
				break;
		}
		
		printf("e_machine   : %u (0x%X)\n", hdr->e_machine, hdr->e_machine ); 
		switch( hdr->e_machine )
		{
			case EM_NONE:
				break;
		}
		
		printf("e_version   : %u (0x%X)\n", hdr->e_version, hdr->e_version ); 
		printf("e_entry     : %u (0x%X)\n", hdr->e_entry, hdr->e_entry ); 
		printf("e_phoff     : %u (0x%X)\n", hdr->e_phoff, hdr->e_phoff ); 
		printf("e_shoff     : %u (0x%X)\n", hdr->e_shoff, hdr->e_shoff ); 
		printf("e_flags     : %u (0x%X)\n", hdr->e_flags, hdr->e_flags ); 
		printf("e_ehsize    : %u (0x%X)\n", hdr->e_ehsize, hdr->e_ehsize ); 
		printf("e_phentsize : %u (0x%X)\n", hdr->e_phentsize, hdr->e_phentsize ); 
		printf("e_phnum     : %u (0x%X)\n", hdr->e_phnum, hdr->e_phnum ); 
		printf("e_shentsize : %u (0x%X)\n", hdr->e_shentsize, hdr->e_shentsize ); 
		printf("e_shnum     : %u (0x%X)\n", hdr->e_shnum, hdr->e_shnum ); 
		printf("e_shstrndx  : %u (0x%X)\n", hdr->e_shstrndx, hdr->e_shstrndx ); 
	} 


} 
 
void elf_swap_phdr(Elf32_Phdr *p) 
{ 
    if (do_swap) 
    { 
        elf_swap((char *)&p->p_type, sizeof p->p_type); 
        elf_swap((char *)&p->p_offset, sizeof p->p_offset); 
        elf_swap((char *)&p->p_vaddr, sizeof p->p_vaddr); 
        elf_swap((char *)&p->p_paddr, sizeof p->p_paddr); 
        elf_swap((char *)&p->p_filesz, sizeof p->p_filesz); 
        elf_swap((char *)&p->p_memsz, sizeof p->p_memsz); 
        elf_swap((char *)&p->p_flags, sizeof p->p_flags); 
        elf_swap((char *)&p->p_align, sizeof p->p_align); 

		if (verbose) 
		{ 
			printf("ELF2BIN>>  elf_swap_phdr =============== \n");
			printf("p_type   : %u (0x%X)\n", p->p_type, p->p_type ); 
			printf("p_offset : %u (0x%X)\n", p->p_offset, p->p_offset ); 
			printf("p_vaddr  : %u (0x%X)\n", p->p_vaddr, p->p_vaddr ); 
			printf("p_paddr  : %u (0x%X)\n", p->p_paddr, p->p_paddr ); 
			printf("p_filesz : %u (0x%X)\n", p->p_filesz, p->p_filesz ); 
			printf("p_memsz  : %u (0x%X)\n", p->p_memsz, p->p_memsz ); 
			printf("p_flags  : %u (0x%X)\n", p->p_flags, p->p_flags ); 
			printf("p_align	 : %u (0x%X)\n", p->p_align, p->p_align ); 
		} 

    } 
} 
 
void elf_swap_shdr(Elf32_Shdr *s) 
{ 
    if (do_swap) 
    { 
        elf_swap((char *)&s->sh_name, sizeof s->sh_name); 
        elf_swap((char *)&s->sh_type, sizeof s->sh_type); 
        elf_swap((char *)&s->sh_flags, sizeof s->sh_flags); 
        elf_swap((char *)&s->sh_addr, sizeof s->sh_addr); 
        elf_swap((char *)&s->sh_offset, sizeof s->sh_offset); 
        elf_swap((char *)&s->sh_size, sizeof s->sh_size); 
        elf_swap((char *)&s->sh_link, sizeof s->sh_link); 
        elf_swap((char *)&s->sh_info, sizeof s->sh_info); 
        elf_swap((char *)&s->sh_addralign, sizeof s->sh_addralign); 
        elf_swap((char *)&s->sh_entsize, sizeof s->sh_entsize); 

		if (verbose) 
		{ 
			printf("ELF2BIN>>  elf_swap_shdr =============== \n");
			printf("sh_name      : %u (0x%X)\n", s->sh_name, s->sh_name ); 
			printf("sh_type      : %u (0x%X)\n", s->sh_type, s->sh_type ); 
			printf("sh_flags     : %u (0x%X)\n", s->sh_flags, s->sh_flags ); 
			printf("sh_addr      : %u (0x%X)\n", s->sh_addr, s->sh_addr ); 
			printf("sh_offset    : %u (0x%X)\n", s->sh_offset, s->sh_offset ); 
			printf("sh_size      : %u (0x%X)\n", s->sh_size, s->sh_size ); 
			printf("sh_link      : %u (0x%X)\n", s->sh_link, s->sh_link ); 
			printf("sh_info      : %u (0x%X)\n", s->sh_info, s->sh_info ); 
			printf("sh_addralign : %u (0x%X)\n", s->sh_addralign, s->sh_addralign ); 
			printf("sh_entsize   : %u (0x%X)\n", s->sh_entsize, s->sh_entsize ); 
		} 


    } 
} 
 
int elf_dumpimage(char *image, int imagelen) 
{ 
    Elf32_Ehdr *hdr = (Elf32_Ehdr *)image; 
    Elf32_Shdr *scnhdr; 
    Elf32_Phdr *proghdr, *p; 
    Elf32_Sym *syms; 
    int i, l; 
    unsigned long last = 0; 
    Elf32_Ehdr ehdr; 
    Elf32_Shdr shdr; 
    Elf32_Shdr *symtab = NULL; 
    Elf32_Shdr *strtab = NULL; 
    Elf32_Shdr *shstrtab = NULL; 
 
    if (!IS_ELF(*hdr)) 
    { 
        printf("ELF2BIN>> It is not ELF binary.\n"); 

		if(data_buf){ free(data_buf);	data_buf=NULL; } // 2020.07.17
        return 10; 
    } 
 	else
	{
 		if(verbose)
		{
			printf("ELF2BIN>> file ID       :[0x%X %X %X %X] \n", 
				hdr->e_ident[EI_MAG0], hdr->e_ident[EI_MAG1], hdr->e_ident[EI_MAG2], hdr->e_ident[EI_MAG3] ); 	
			printf("ELF2BIN>> file class    :[0x%X] ", hdr->e_ident[EI_CLASS] );	
			switch( hdr->e_ident[EI_CLASS] )
			{
				case ELFCLASSNONE: /* Invalid class */
					printf(" Invalid class \n" );	
					break;
				case ELFCLASS32: /* 32 bit objects */
					printf(" 32bit objects\n" );	
					break;
				case ELFCLASS64: /* 64 bit objects */
					printf(" 64bit objects\n" );	
					break;
				case ELFCLASSNUM: /* ELF class number */
					printf(" ELF class number \n" );	
					break;
				default:
					printf(" unknown \n" );	
					break;
			}
			printf("ELF2BIN>> data encoding :[0x%X] (%s) \n", hdr->e_ident[EI_DATA],
					( ELFDATA2LSB==hdr->e_ident[EI_DATA]? "Little-endian" : 
						(ELFDATA2MSB==hdr->e_ident[EI_DATA]? "Big-endian" : "invalid") )  );
			printf("ELF2BIN>> hdr version   :[0x%X] \n", hdr->e_ident[EI_VERSION] );	
		}
	}

	
#ifdef ELF_LITTLE_ENDIAN 
    do_swap = (hdr->e_ident[EI_DATA] == ELFDATA2MSB); 
#else 
    do_swap = (hdr->e_ident[EI_DATA] == ELFDATA2LSB); 
#endif 
 
    elf_swap_ehdr(hdr); 
    proghdr = (Elf32_Phdr *)(image + hdr->e_phoff); 
 
    for (i = 0; i < hdr->e_phnum; i++) 
    { 
        p = &proghdr[i]; 
        elf_swap_phdr(p); 
 
        if (p->p_type != PT_LOAD) 
            continue; 

 		if(verbose)
			printf("ELF2BIN>> %02d: vaddr=%#x memsz=%#x filesz=%#x\n", i, p->p_vaddr, p->p_memsz, p->p_filesz); 
 
        if (last > 0 && last < p->p_vaddr) 
            elf_dump_zero(p->p_vaddr - last); 
 
        elf_dump_bin(image + p->p_offset, p->p_filesz); 
 
        if (p->p_memsz > p->p_filesz) 
            elf_dump_zero(p->p_memsz - p->p_filesz); 
 
        last = p->p_vaddr + p->p_memsz; 
    } 
 
    scnhdr = (Elf32_Shdr *)(image + hdr->e_shoff); 
 
    for (i = 0; i < hdr->e_shnum; i++) 
        elf_swap_shdr(&scnhdr[i]); 
 
    shstrtab = &scnhdr[hdr->e_shstrndx]; 
 
    for (i = 0; i < hdr->e_shnum; i++) 
    { 
        if (scnhdr[i].sh_type != SHT_SYMTAB || scnhdr[i].sh_offset == 0) 
            continue; 
 
        symtab = &scnhdr[i]; 
        strtab = &scnhdr[scnhdr[i].sh_link]; 
 
        if (strtab->sh_type != SHT_STRTAB) 
            fprintf(stderr, "ELF2BIN>> section %d should be strtab?!?\n", 
                    scnhdr[i].sh_link); 
 
        break; 
    } 
 
    if (symtab == NULL || strtab == NULL || shstrtab == NULL || 
            symtab->sh_entsize != sizeof *syms) 
    { 
        printf("ELF2BIN>> no symbol table?!?\n"); 

		if(data_buf){ free(data_buf);	data_buf=NULL; } // 2020.07.17
        return 0; 
    } 
 
    ehdr = *hdr; 
    ehdr.e_phnum = 0; 
    ehdr.e_phoff = 0; 
    ehdr.e_shnum = 4; 
    ehdr.e_shoff = sizeof (*hdr); 
    ehdr.e_shstrndx = 3; 

	if(verbose)
		printf("ELF2BIN>> dump syms at file offset %#x...\n", ftell(outfile)); 
	
    elf_swap_ehdr(&ehdr); 
    elf_dump_bin((char *)&ehdr, sizeof ehdr); 
    elf_swap_ehdr(&ehdr); 
 
    memset(&shdr, 0, sizeof shdr); 
    elf_swap_shdr(&shdr); 
    elf_dump_bin((char *)&shdr, sizeof shdr); 
 
    i = ehdr.e_shoff + ehdr.e_shentsize * ehdr.e_shnum; 
    shdr = *symtab; 
    shdr.sh_offset = i; 
    shdr.sh_link = 2; 
    elf_swap_shdr(&shdr); 
    elf_dump_bin((char *)&shdr, sizeof shdr); 
 
    shdr = *strtab; 
    shdr.sh_offset = i + symtab->sh_size; 
    elf_swap_shdr(&shdr); 
    elf_dump_bin((char *)&shdr, sizeof shdr); 
 
    shdr = *shstrtab; 
    shdr.sh_offset = i + symtab->sh_size + strtab->sh_size; 
    elf_swap_shdr(&shdr); 
    elf_dump_bin((char *)&shdr, sizeof shdr); 
 
    elf_dump_bin(image + symtab->sh_offset, symtab->sh_size); 
    elf_dump_bin(image + strtab->sh_offset, strtab->sh_size); 
    elf_dump_bin(image + shstrtab->sh_offset, shstrtab->sh_size); 
 
    return 0; 
} 
 
 
int tt_main(int argc, char *argv[]) 
{ 
     FILE *fp; 
     char *image; 
     int imagelen; 
     struct stat statbuf; 
  
     if (argc < 2) 
     { 
         fprintf(stderr, "usage: %s file\n", argv[0]); 
         return -1; 
     } 
  
     fp = fopen(argv[1], "r"); 
  
     if (fp == NULL) 
     { 
         fprintf(stderr, "Cannot open %s!\n", argv[1]); 
         return 1; 
     } 
  
     if (fstat(fileno(fp), &statbuf) < 0) 
     { 
         fprintf(stderr, "Cannot stat %s!\n", argv[1]); 
         return 2; 
     } 
  
     imagelen = statbuf.st_size; 
     image = (char *)malloc(imagelen); 
	 fprintf(stderr, "image=%p\n", image); 
  
     if (image == NULL) 
     { 
         fprintf(stderr, "Cannot allocate space for %s!\n", argv[1]); 
         return 3; 
     } 
  
     if (fread(image, sizeof (char), imagelen, fp) != imagelen) 
     { 
         fprintf(stderr, "Cannot read %s!\n", argv[1]); 
         return 2; 
     } 
  
     return elf_dumpimage(image, imagelen); 
} 
#endif /// ELF2BIN






/// double   val    : Number to be converted
/// int        round : Number of digits after decimal point
/// int        *dp   : Pointer to decimal-point position
/// int        *sign : Pointer to stored sign indicator
char *commify(double val, char *buf, int round) 
{
	static char *result;
	char *nmr;
	int dp, sign;

	result = buf;

	/* Be sure round-off is positive  */
	if (round < 0)
		round = -round;

	nmr = malloc( COMMA_BUF_SIZE*sizeof(unsigned char) );

	/* Convert number to a string */
	nmr = fcvt(val, round, &dp, &sign); 

	/* Prefix minus sign if negative  */
	if (sign)
		*buf++ = '-';

	/* Check if number is less than 1 */
	if (dp <= 0)
	{
		/* Set dp to max(dp, -round) */
		if (dp < -round)
			dp = -round;

		/* Prefix with "0." */
		*buf++ = '0';
		*buf++ = '.';

		/* Write zeros following decimal  */
		while (dp++)
		{
			/* point */
			*buf++ = '0';
		}
	}
	else 
	{
		/* Number is >= 1, commify it */
		while (dp--)
		{
			*buf++ = *nmr++;
			if (dp % 3 == 0)
			{
				/// *buf++ = dp ? ',' : '.';
				*buf++ = dp ? ',' : (round ? '.' : 0);
			}
		}
	}

	/* Append rest of digits */
	strcpy(buf, nmr);
	
	if(nmr) free(nmr);
	return result;
}




#define MATH_PI 3.1415926535897932384626433832795F



void Make_batchfile(void)
{

}


void help_brief(void)
{

	printf("\033[32m"); /* GREEN */
	//printf("\033[34m"); /* BLUE */

	printf("-------------------------------------------------------------\n");
	printf(" ah.exe %s by TOP.JOO (%s)/ %s \n", Attversion, EmailText, __DATE__ );
	printf("-------------------------------------------------------------\n");

	printf("\033[0m"); 
	fflush(stdout); 


}


void help(void)
{
    printf("\nUsage: ah.exe [Version %s, Build:%s] by TOP.JOO. \r\n"
           "--[ Helps ]----------------------------- -------------------------------------------------------------------------\n"
           "  -h or --help [intel|motorola]          intel    : Intel hex format help.  \n"
           "                                         motorola : Motorola hex format help. \n"
           " Ex) ah.exe --help motorola \n"
           "     ah.exe -h intel \n"
           "\n"
           "--[ Input / Output filename ]----------- -------------------------------------------------------------------------\n"
		   "  -i or --input [FILE]                   Input file. \n"
		   "  -o or --output [FILE]                  Output file. (new create) \n"
		   "  -a or --append [FILE]                  Output file. (add to the specific file) \n"
		   "\n"
		   "--[ Detach Header ]--------------------- -------------------------------------------------------------------------\n"
		   "  -d or --detach [byte number(in decimal)]  \n"
		   "        --detach 0|default               Delete the default size (16Bytes X 4Lines) Bytes. \n"
		   "        --detach 54                      Delete the specific bytes number, 54 Bytes in decimal. \n"
		   "        --detach 2048 | 16384 | 1048576  Delete the specific bytes number, 2048/ 16384/ 1048576 Bytes. in decimal.\n"
		   "        --detach 2kB | 16kB | 1MB        Delete the specific bytes number, 2kB, 16kB, 1MB. \n"
		   "\n"
		   " Ex) ah.exe -i LAN1234_nk.LGE -o nk.bin -d 0 \n"
		   "     ah.exe --input LAN1234_nk.LGE --output nk.bin --detach 128 \n"
		   "     ah.exe --input LAN1234_nk.LGE --output nk.bin --detach 2kB \n"
           "\n"
		   "--[ Attach Header and Order ]----------- -------------------------------------------------------------------------\n"
		   "  -b or --board [CPU/PCB NAME]           CPU/PCB/Module Name (Max. 16 bytes)\n"	 /* PI2000, Max 16 Characters */
		   "  -m or --model [MODEL NAME]             Model Name (Max. 16 bytes)\n"	 /* GMPT43, Max 16 Characters  */
           "  -v or --version [VERSION NAME]         Version Name (Max. 16 bytes) \n"
		   "  -c or --cinfo date                     Building Date/Time(size is 16bytes)\n" /* Building Date, Max 128 Characters  */
		   "        --cinfo crc16|crc16c|crc32       Insert checksum-lowercase in front of output file. (Max. 16Bytes) \n"  
		   "        --cinfo crc16ksc                 Insert checksum-lowercase in front of output file. (Max. 16Bytes) \n"  
		   "        --cinfo crc64|adler32|joaat      Insert checksum-lowercase in front of output file. (Max. 16Bytes) \n"  
		   "        --cinfo CRC16|CRC16C|CRC32       Insert checksum-uppercase in front of output file. (Max. 16Bytes) \n"  
		   "        --cinfo CRC64|ADLER32            Insert checksum-uppercase in front of output file. (Max. 16Bytes) \n"  
		   "        --cinfo CRC64ISC                 Insert CRC64 ISC(Internet Systems Consortium) (Max. 16Bytes) \n"  
		   "        --cinfo sha224|SHA224            Insert SHA2 (lower/uppercase) in front of output file. (Size is 56Bytes)\n"
		   "        --cinfo md5|md6                  Insert Message Digest5/6 in front of output file. (Size is 32/64Bytes)\n"  
		   "        --cinfo md2|md4                  Insert Message Digest2/4 in front of output file. (Size is 32/32Bytes)\n"  
           "        --cinfo sha1                     Insert SHA1 in front of output file. (Size is 40Bytes)\n"  
           "        --cinfo sha224|sha256            Insert SHA2 in front of output file. (Size is 56/64Bytes)\n"	
           "        --cinfo sha384|sha512            Insert SHA2 in front of output file. (Size is 96/128Bytes)\n" 
           "        --cinfo sha3-224|SHA3-256        Insert SHA3 in front of output file. (Size is 56/64Bytes)\n" 
           "        --cinfo sha3-384|SHA3-512        Insert SHA3 in front of output file. (Size is 96/128Bytes)\n" 
           "        --cinfo shake128|SHAKE256        Insert Shake in front of output file. (Size is 64/128Bytes)\n" 
           "        --cinfo RMD128|RMD160            Insert RipeMD128/RipeMD160in front of output file. (Size is 32/40Bytes)\n" 
           "\n"
           " Ex) ah.exe -b AU1354 -m LAN1234 -c date  -i nk.bin -o LAN1234_nk.JOO --version 5.0.00(13A) \n"
           "     ah.exe -b AU1354 -m LAN1234 --cinfo crc16  -i nk.bin -o LAN1234_nk.JOO -v 5.0.00(13A) \n"
           "     ah.exe -b AU1354 -m LAN1234 --cinfo crc64  --input nk.bin --output LAN1234_nk.LGE -v 5.0.00(13A) \n"
           "     ah.exe -b AU1354 -m LAN1234 --cinfo sha256 -i nk.bin      -o LAN1234_nk.LGE  -v 5.0.00(13E) \n"
           "     ah.exe -b AU1354 -m LAN1234 --cinfo md5    -i nk.bin      -o LAN1234_nk.LGE  --version 5.0.00(13E) \n"
           "     ah.exe -b AU1354 -m LAN1234 --cinfo shake128 -i nk.bin    -o LAN1234_nk.MTX  -v 5.0.00(13A) \n"
           "\n"
		   "--[ Convert Float number to Hexa ]------ -------------------------------------------------------------------------\n"
		   "  -g both -f or --ignore both --float [0.8543]     -g both -f 0.866 \n" 
		   "  -g both -f or --ignore both --float [sin|cos|tan 60]  -g both -f sin 60 \n" 
		   "  --ignore both --float [float number]        --ignore both --float 0.866 \n" 
		   "  --ignore both --float [sin|cos|tan 60]      --ignore both --float sin 60 \n" 
		   "  -i -o -f file access                   -i [input] -o [output] -f f2h \n" 
		   "  --input --output --float file access   --input [in-file] --output [out-file] --float f2h \n" 
		   " Ex) ah.exe -g both --float 0.8543 \n"
		   "     ah.exe -g both --float sin 60 \n"
		   "     ah.exe --input ff.dat --output ff.txt --float f2h \n"
		   "\n"
           "--[ Hex2Bin ]--------------------------- -------------------------------------------------------------------------\n"
		   "  -L or --intel                          Convert INTEL hexa format to Bin. (Max Size:%u MB) \n"
		   "  -A or --motorola                       Convert MOTOROLA hexa format to Bin. (Max Size:%u MB) \n"
		   "                                           . S19-style (16-bit) addressing format. \n"
		   "                                           . S28-style (24-bit) addressing format. \n"
		   "                                           . S37-style (32-bit) addressing format. \n"
		   "                                           . checksum \n"
           "  -S or --startaddr                      Starting address in hexa value. (with -A or -L option) \n"
	#if 0
           "  -k or --crctype [0|1|2|3|4|5]          Select checksum type (default:Checksum 8-bit) \n"
           "                                           0: Checksum 8-bit \n"
           "                                           1: Checksum 16-bit (adds 16-bit words into a 16-bit sum, data and result BE or LE)\n"
           "                                           2: CRC8 \n"
           "                                           3: CRC16 \n"
           "                                           4: CRC32 \n"
           "                                           5: Checksum 16-bit (adds bytes into a 16-bit sum, result BE or LE) \n"
	#endif
           "  -l or --length [1000]                  0x1000 in hexa value for bin's file length . (ex: 8000 for 0x8000)\n"
           "        --length [a0000]                 0xa0000 in hexa value for bin's file length 640kByte \n"
           "        --length [640kB]                 For bin's file length 640kBytes \n"
           "        --length [1MB]                   For bin's file length 1MBytes \n"
           "  -n or --alignword                      Address alignment word (--intel option only) \n"
           "  -E or --endian [little|big]            Endian for checksum/CRC, 0: little, 1: big \n"
           "  -P or --padbyte [hexa-value]           Pad-byte value in hex, example -P ee (default: 0xff)\n"
           "  -k or --allpadarea                     Padding byte for ALL empty area. --padbyte ab option \n"
           "  -Z or --zeroforced                     Because the real address of hexa file is very large, so \n"
           "                                           it is created binary a based on zero address.\n"
           "\n"
           " Ex) ah.exe -i [hexa-file] -o [out bin file] -A or -L \n"
           "     ah.exe -i [hexa-file] -o [out bin file] -L --length 1000 \n"
           "     ah.exe -i [hexa-file] -o [out bin file] --intel \n"
           "     ah.exe --input [hexa-file] --output [out bin file] -A or -L -S A00 \n"
           "     ah.exe --input romp.hex -o BIN\\ROMP.bin --intel --startaddr A00 \n"
           "     ah.exe --input romp.hex --output BIN\\ROMP.bin --intel --length A0000 \n"
           "     ah.exe --input romp.hex --output BIN\\ROMP.bin --motorola \n"
           "     ah.exe --input romp.hex --output BIN\\ROMP.bin --motorola \n"
           "     ah.exe --input romp.hex --output BIN\\ROMP.bin --motorola --length 10000 \n"
           "     ah.exe --input romp.hex --output BIN\\ROMP.bin --motorola --length 10000 --padbyte ab \n"
           "\n"
           "--[ WinCE Record ]---------------------- -------------------------------------------------------------------------\n"
           "  -N or --nk [nb0|none]                  NK.BIN (WinCE Kernel) Record information \n"
           " Ex) ah.exe -i nk.bin -o NK.LOG -N none \n"
           "     ah.exe --input nk.bin --output NK.LOG -nk nb0 \n"
           "\n"
           "--[ Files Merge/Join ]------------------ -------------------------------------------------------------------------\n"
           "  -j or --join [hexa value]              Specify the 1st file's total size in hexa. \n"
           "        --join [640kB/320kB]             Specify the 1st file's total size 640kB/320kB in size unit no-space. \n"
           "        --join [1MB/2MB]                 Specify the 1st file's total size 1MB/2MB in size unit no-space. \n"
           "  -E or --endian [little|big]            Calculate the 2nd file size in hexa (4byte) \n"
           "  -P or --padbyte [hexa value]           Pad byte in hexa (1byte). default is 0xFF \n"
           "\n"
           " Ex) ah.exe -j 3000       -i [1st-file] [2nd-file] -o [merged output file] \n"
           "     ah.exe --join 0x8000 -i [1st-file] [2nd-file] -o [merged output file] \n"
           "     ah.exe --join 2800   -i STM_mcu_Boot.bin  STM_mcu_302K8_App.bin -o  STM_mcu_TotalQD21.bin --padbyte ab \n"
           "     ah.exe --join 10kB   -i STM_mcu_Boot.bin  STM_mcu_302K8_App.bin -o  STM_mcu_TotalQD21.bin --padbyte ab \n"
           "     ah.exe --join 0x2800 -E little -i STM_mcu_Boot.bin	STM_mcu_302K8_App.bin -o STM_mcu_TotalQD21.bin \n"
           "     ah.exe --join 2800   --endian little -i STM_mcu_Boot.bin  STM_mcu_302K8_App.bin -o STM_mcu_TotalQD21.bin \n"
           "     ah.exe --join 0x2800 --endian big    -i STM_mcu_Boot.bin  STM_mcu_302K8_App.bin -o STM_mcu_TotalQD21.bin \n"
           "\n"

	#if MODIFIED_JULIAN_DATE 
           "--[ MJD (Modified Julian Date) ]-------- -------------------------------------------------------------------------\n"
           "  -J or --mjd test                        Modified JD test and create test-file.\n" 
           "        --mjd current                     Convert the current System Time to MJD \n" 
           "        --mjd mjd                         Convert input DATE file(-i) to MJD \n" 
           "        --mjd date                        Convert input MJD file(-i) to DATE  \n" 
           "\n"
           " Ex) ah.exe --mjd test \n"
           "     ah.exe --mjd current \n"
           "     ah.exe --input number.mjd --output out.txt --mjd date \n"
           "     ah.exe --input date.mjd   --append out.txt --mjd mjd \n"
           "\n"
	#endif

           "--[ Others ]---------------------------- -------------------------------------------------------------------------\n"
           "  -I or --fileinform [all|line]          Searching all files. -I all , -I lines \n"
           "  Ex) ah.exe -I all \n"
           "\n"
           "  -F or --fillsize [size in hexa]        Fill Pad Byte. (default:0xFF). The fillsize means total file length. \n"
           "        --fillsize [a0000]               hexa value 0xa0000 for length 640kByte \n"
           "        --fillsize [640kB]               For total length 640kBytes \n"
           "        --fillsize [1MB]                 For example, output file total length is 1MBytes \n"
           "  -z or --verbose 0|1|2|3|4              display message\n"
           "        --verbose date|size|datesize     display message\n"
           "\n"
           " Ex) ah.exe --input STM32.bin --output mcuTOT_STM32.bin --fillsize a0000  \n"
           "     ah.exe --input STM32.bin --output mcuTOT_STM32.bin --fillsize 640kB --padbyte CC  \n"
           "\n"
           "--[ Message Digest / SHA / Checksum ]--- -------------------------------------------------------------------------\n"
           "  -M or --checksum [crc16|crc16c|crc16ksc Generate CRC16/CRC16CCITT/KSC-CRC16  \n" 
           "                    crc32|adler32|joaat            CRC32/ADLER32  \n"
           "                    crc64|crc64isc                 CRC64/CRC64 ISC(Internet Systems Consortium)  \n"
           "        --checksum [MD2|MD4|MD5|MD6]      Generate MD2 (32Bytes) Message Digest Algorithm-2 \n" 
           "                                                   MD4 (32Bytes) Message Digest Algorithm-4 \n" 
           "                                                   MD5 (32Bytes) Message Digest Algorithm-5 \n" 
           "                                                   MD6 (64Bytes) Message Digest Algorithm-6 \n" 
           "        --checksum [SHA1|SHA224|          Generate SHA1 (40Bytes) Secure Hash Algorithm \n" 
           "                    SHA256|SHA384|SHA512]          SHA2-224 (56Bytes) Secure Hash Algorithm \n" 
           "                                                   SHA2-256 (64Bytes) Secure Hash Algorithm \n" 
           "                                                   SHA2-384 (96Bytes) Secure Hash Algorithm \n" 
           "                                                   SHA2-512 (128Bytes) Secure Hash Algorithm \n" 
           "        --checksum [SHA3-224|SHA3-256|    Generate SHA3-Keccak-224 (56Bytes) Secure Hash Algorithm \n" 
           "                    SHA3-384|SHA3-512|             SHA3-Keccak-256 (64Bytes) Secure Hash Algorithm \n" 
           "                    SHAKE128|SHAKE256]             SHA3-Keccak-384 (96Bytes) Secure Hash Algorithm \n" 
           "                                                   SHA3-Keccak-512 (128Bytes) Secure Hash Algorithm \n" 
           "                                                   SHAKE128 (64Bytes) Secure Hash Algorithm \n" 
           "                                                   SHAKE256 (128Bytes) Secure Hash Algorithm \n" 
           "        --checksum [RMD128|RMD160]        Generate RipeMD128/RipeMD160 (32/40Bytes) Hash Algorithm \n" 
	#if MD5_MULTI_INPUT_FILES 
           " \n"
           " Ex) ah.exe -i @.zip         -a MD5sum.txt        --checksum MD5 --verbose datesize \n"
           "     ah.exe -i $.zip         -o SHA384sum.txt     -M sha224      -z size \n"
           "     ah.exe --input @.zip    --output SHA2sum.txt -M SHA512      --verbose date \n"
           "     ah.exe --input @.@      --append CRCsum.txt  --checksum CRC16C \n"
           "     ah.exe --input @.zip    --append SHA2sum.txt --checksum SHA256 --verbose date \n"
           "     ah.exe --input Mem@.zip --append SHA1sum.txt --checksum SHA1   --verbose datesize \n"
           "     ah.exe --input @.zip    --checksum sha256 \n"
           "     ah.exe --input $.zip    --checksum rmd160 \n"
    #endif

	#if CONVERT_BMP2C
           "--[ Convert bmp image to C text ]------- -------------------------------------------------------------------------\n"
           "  -B or --bmp                             Convert BMP file to C text file. \n" 
           "      888                RGB 888 \n" 
           "      444                RGB 444 \n" 
           "      555                RGB 555 \n"
           "      565                RGB 565 \n"
	#endif /// CONVERT_BMP2C
           "------------------------------------------------------------------------------------------------------------------\n", 
           		Attversion, __DATE__ , /* __TIME__ , */ (INT2BIN_MEMORY_SIZE>>20), (MOT2BIN_MEMORY_SIZE>>20) );




	/* ================================= */
	Make_batchfile();

#if 0
    exit(0);
#else
	printf("Press [ENTER]...\r");
#endif
}



#define ENDIAN_LITTLE 		1
#define ENDIAN_BIG 			2

enum {
	ASTERISK_UNKNOWN = -1,
	ASTERISK_INIT = 0,
	ASTERISK_STEP1 = 1,
	ASTERISK_FOUND = 2,

	ASTERISK_MAX = 9
};


#if 0
int arg_sum(int num_args, ...)
{
	int arg=0;
	int result=0;
	int i=0;
	
	va_list ap;
	va_start(ap, num_args);

	//for(i=0; i<num_args; i++)
	result = 0;
	for(;;)
	{
		arg = va_arg(ap, int);
		if(0==arg) break;
		result += arg;
	}
	va_end(ap);
	return result;
}
//printf(" ==== %d  ==== ", arg_sum(3,3,3,3,3) );
#endif



int isFileExist(const char *file, int disp)
{
	struct	_finddatai64_t fexist;
	long ret = 0L;
	int  retValue = 0;

	if(0==strcmp(file,"") || NULL==file) return 0;

	if( (ret = _findfirsti64( file, &fexist )) == -1L )
	{
		if(disp) printf(" ------ [ %s ] is Non file ", file);
		retValue = 0;
	}
	else
	{
		if(disp) printf(" FILE : [ %s ] ", file);
		retValue = 1;
	}

	_findclose( ret );

	if(disp) printf(" RETURN VALUE : %d  \n", retValue );
	return retValue;
}


struct tm *current_time(void)
{
	time_t	cur_time;
	struct  tm* pLocalTime;

	time( &cur_time );
    pLocalTime = localtime(&cur_time);

	#if 0
    fprintf(stdout, "+++++ %04d/%02d/%02d %02d:%02d:%02d ++++ \n", 
			pLocalTime->tm_year + 1900, 
			pLocalTime->tm_mon + 1, 
			pLocalTime->tm_mday, 
			pLocalTime->tm_hour, pLocalTime->tm_min, pLocalTime->tm_sec);
	#endif

	pLocalTime->tm_year += 1900;
	pLocalTime->tm_mon  += 1;

	return pLocalTime;
}



int main(int argc, char *argv[])
{ 
	struct stat file_statbuf; 
	int elf_size = 0;

	struct	_finddatai64_t iFiles; // c_file;
	struct	_finddatai64_t fexist; // 2020.06.18

	unsigned __int64     iTotSize = 0ULL;
	__int64     iFileNum = 0ULL;
	int     iFirst=-1, iLast=-1;
	int     isAsteMode = ASTERISK_UNKNOWN; /* initial value must be -1 */
	int  	isAsteminus = 0;
	long	retFiles;
	int 	multifileindex = 0, idx=0;
	int 	olen=0;
	//__int64 isize;
	unsigned int titlen=0;
	int 	isLength = 0;

	uint32_t crc32_ctab[CRC32_TAB_SIZE] = {0,};

	const char WeekTXT[][3+1] = {
			"Sun", // wDayOfWeek 0
			"Mon", // wDayOfWeek 1
			"Tue", // wDayOfWeek 2
			"Wed", // wDayOfWeek 3
			"Thu", // wDayOfWeek 4
			"Fri", // wDayOfWeek 5
			"Sat", // wDayOfWeek 6
			"***", // NEVER
			0x00
		};

	WORD    mot_style = 0x0;
	int ret_scan = 0, ret_bdscan=0;
	int opt;
	byte opt_ok = 0x0;
	int ii;
	int isJoin=0; // --join option

	int isFileMerge=0; // 2017.12.11
	int is2ndFileSize=0;
	int is2ndEndian=0; // 1:little, 2:big endian
	int isAttach = 0;
	int isDelHdr = 0, isFillSize = 0; /* 2009.10.09, CRC Generator */
	int isCRC = 0, isCRCtype = 0;
	int insertCRC=0; // 2017.04.04
	int iUpper = -1;
	int isAppend = 0; /// over-write
	int isNKinform = 0, isCreateNB0=0;
	int isMot2bin = 0;
	int isIntel2bin = 0;
	int isElf2Bin = 0;
	int isRandomNum = 0;
	int iRanMaxi=0;
	int isTitle = 0;
	unsigned int total_bin_size = 0;
	int c;
	int count=0;
	///int verbose=FALSE;
	size_t 	fr_size;
	int isFloat2Hex = 0;
	int isIgnoreBothFile = 0, isIgnoreInFile=0, isIgnoreOuFile=0;
	int isConvertBMP2C = 0, iRGB = 565;
	int isHash=0;
	int isMD2 = 0; 
	int isMD4 = 0;
	int isMD5 = 0;
	int isSHA1 = 0;
	int isSHA256 = 0; /// SHA2, 2014.06.30
	int isSHA384 = 0; /// SHA2, 2014.06.30
	int isSHA512 = 0; /// SHA2, 2014.06.30
	int isSHA224 = 0; /// 2014.07.31
	int isMD6    = 0; /// 2014.07.31
//#if SHA3_KECCAK_224_256_384_512
	int isSHA3_KECCAK_224 =0, isSHA3_KECCAK_256=0, isSHA3_KECCAK_384=0, isSHA3_KECCAK_512=0;
	int isShake128=0, isShake256=0;
	int isBlake224=0, isBlake256=0, isBlake384=0, isBlake512=0;
	int isRipeMD128 = 0, isRipeMD160 = 0;
//#endif
	int isIDEA = 0;
	int isMJD = 0; /// 2014.07.04
	int isBMPreverse = 0;
	unsigned int ibmpIndex = 0;
	
	unsigned int i_readCount = 0;
	int len_board_name = 0;
	int len_module_name = 0;
	int len_version_name = 0;
	int len_build_date = 0;
	int len_attach_hdr = 0; // 2020.07.07
	int iloop=0;
	int temp_index = 0;
#if MODIFIED_JULIAN_DATE 
	double	MJDvalue;
	__double_hex_union	cHex;
	mjd_timestamp ymd_hms; /// To Modified Julian Date
	int iTMP=0;
#endif
	char str_cmBuf[COMMA_BUF_SIZE] = {0,};

#define NB0_EXT 			"nb0"

	char extfile_name[MAX_CHARS*LENGTH_OF_FILENAME+1];
	//char mulfile_name[MULTI_IN_FILES_CNT][MAX_CHARS*LENGTH_OF_FILENAME+1];
	//__int64 mulfile_size[MULTI_IN_FILES_CNT]; /// 2014.08.06
	char ttlfilename[MAX_FILENAME_LEN];
	char infile_name[MAX_CHARS*LENGTH_OF_FILENAME+1];
	char sefile_name[MAX_CHARS*LENGTH_OF_FILENAME+1]; // -i option 2nd file name
	__int64  infile_size; /// 2014.08.06
	__int64  sefile_size; /// 2017.04.05
	char outfile_name[MAX_CHARS*LENGTH_OF_FILENAME+1];

	char str_boardName[MAX_CHARS+1]; /* PI2000 */
	char str_moduleName[MAX_CHARS+1];
	char str_versionName[MAX_VERSION_LEN+1]; // 2017.12.12
	char str_buildDate[MAX_CHARS*2];
	char str_abuild_date[MAX_CHARS+1];
	char str_fillSiz[MAX_CHARS+1];
	char str_inform[MAX_CHARS+1];
	char str_nb0[MAX_CHARS+1];
	char str_help[MAX_CHARS+1];
	char str_BMPType[MAX_CHARS+1];
	char str_hash[MAX_CHARS+1];
	unsigned char str_float[MAX_CHARS*LENGTH_OF_FILENAME+1];
#if MODIFIED_JULIAN_DATE 
	char str_mjd[MAX_CHARS+1];
	char str_cur_date[MAX_CHARS+1];
#endif /// MODIFIED_JULIAN_DATE 
	
#ifdef _MOT2BIN_
	/* line inputted from file */
	char 	Mot2bin_Line[MOT2BIN_MAX_LINE_SIZE];
	byte 	Mot2Bin_Data_Str[MOT2BIN_MAX_LINE_SIZE];
	char    TmpHexBuf[10];
#endif
	char strZeroForced[MAX_CHARS+1];
	char str_dummy[MAX_CHARS+1];
	double	 f2h_float;

	char str_Verbos[MAX_CHARS+1];
	unsigned int iVerbosType = 0;

	char str_DelHdr[MAX_CHARS+1];
	char str_crcAdd[MAX_CHARS+1]; // 2017.04.04
	char str_FileMax[MAX_CHARS+1]; // 2017.04.04
	char str_SecFileSiz[MAX_CHARS+1]; // 2017.12.11
	char strHexType[MAX_CHARS+1]; 

	char str_ignore[MAX_CHARS+1]; 
	char strHex2BinLen[MAX_CHARS+1]; // 2020.07.07
	char strPadByte[MAX_CHARS+1];
//	char strPadArea[MAX_CHARS+1];

	int result;
	unsigned int iFileLimit=0;
	unsigned int i2ndFileSize=0;
	unsigned int iHdr_len=0;
	unsigned int iEachPacketSize=0; // Hexa decimal in Byte.
	/* -------------------------------------------------------------------- */
	/* -- getopt_long stores the option index here. -- */
	int option_index = 0;	
	static int verbose_flag;

	//static char strOpt[] = "gALz:h:b:m:v:i:o:a:cd:F:I:N:S:k:f:B:M:J:l:e";
	static char strOpt[] = "g:ALz:h:b:m:v:i:o:a:c:d:F:I:N:S:f:B:M:J:l:ej:nE:P:kZR:"; // 2020.06.26

	static struct option long_options[] =
	{
		/* These options set a flag. */
		{"brief",		no_argument,	   &verbose_flag, 0},
		/* These options don't set a flag.	We distinguish them by their indices. */
		{"append",		required_argument, 0, 'a'}, /// Output (append) filename
		{"board",		required_argument, 0, 'b'}, /// Attach Header : Board Name(16byte)
		{"cinfo",		required_argument, 0, 'c'}, /// create current system date or insert checksum
		{"detach",		required_argument, 0, 'd'}, /// detach Header (16byte * 4 lines)
		{"elf",			no_argument,       0, 'e'}, /// elf
		{"float",		required_argument, 0, 'f'}, /// float number to hex-decimal
		{"ignore",		required_argument, 0, 'g'}, /// ignore input & output-file
		{"help",		required_argument, 0, 'h'}, /// help
		{"input",		required_argument, 0, 'i'}, /// input file name
		{"join",		required_argument, 0, 'j'}, // file merged!!  2017.04.05
		{"allpadarea", 	no_argument,       0, 'k'}, /// checksum type for hex2bin
		{"length",		required_argument, 0, 'l'}, /// Mot2bin, hex2bin : MaxSize
		{"model",		required_argument, 0, 'm'}, /// Attach Header : Module Name (16byte)
		{"alignword",	no_argument,       0, 'n'}, // intel family address alignment word ENABLE
		{"output",		required_argument, 0, 'o'}, /// output filename
	//	{"file",		required_argument, 0, 'p'},
	//	{"file",		required_argument, 0, 'q'},
	//	{"reverse",		no_argument,       0, 'r'}, /// bmp reverse
	//	{"size",		required_argument, 0, 's'}, // 2nd file size
	//	{"title",		required_argument, 0, 't'},
	//	{"file",		required_argument, 0, 'u'},
		{"version", 	required_argument, 0, 'v'}, /// Attach Header : Version Name (16byte)
	//	{"file",		required_argument, 0, 'w'},
	//	{"file",		required_argument, 0, 'x'},
	//	{"file",		required_argument, 0, 'y'},
		{"verbose",		required_argument, 0, 'z'}, /// verbos --
	
		/// ------------------------------------------------------------------------------
		{"motorola",	no_argument,	   0, 'A'}, /// convert Motorola to binary
		{"bmp", 		required_argument, 0, 'B'}, /// bmp file
	//	{"file",		required_argument, 0, 'C'},
	//	{"dummy",		required_argument, 0, 'D'}, 
		{"endian",		required_argument, 0, 'E'}, /* 0:little-endian,  1:big-endian */
		{"fillsize",	required_argument, 0, 'F'}, /// Fill Data (0xFF)
	//	{"CRC", 		required_argument, 0, 'G'}, 
	//	{"file",		required_argument, 0, 'H'},
		{"fileinform",	required_argument, 0, 'I'}, /// File Information in PC
		{"mjd",	        required_argument, 0, 'J'}, /// Modified Julian Date---
	//	{"file",		required_argument, 0, 'K'},
		{"intel",		no_argument,	   0, 'L'}, /// convert Intel Format to binary
		{"checksum",	required_argument, 0, 'M'}, /// checksum MD5, SHA1, SHA256, SHA384, SHA512--
		{"nk",			required_argument, 0, 'N'}, /// WinCE OS Kernel, nk.bin information
	//	{"file",		required_argument, 0, 'O'}, 
		{"padbyte",		required_argument, 0, 'P'}, // Padbyte 0xff
	//	{"file",		required_argument, 0, 'Q'},
		{"random",		required_argument, 0, 'R'},
		{"startaddr",	required_argument, 0, 'S'}, /// start address Hex2bin--
	//	{"file",		required_argument, 0, 'T'},
	//	{"file",		required_argument, 0, 'U'},
	//	{"file",		required_argument, 0, 'V'},
	//	{"file",		required_argument, 0, 'W'},
	//	{"file",		required_argument, 0, 'X'},
	//	{"file",		required_argument, 0, 'Y'},
		{"zeroforced",	no_argument,       0, 'Z'},
	
		{0, 0, 0, 0}
	};
	/* -------------------------------------------------------------------- */



	/// -------------------------------------------
	/// --- buffer initialized -------------------------
	/// -------------------------------------------
	memset(extfile_name, 0x00, (MAX_CHARS*LENGTH_OF_FILENAME)+1 );
	//memset(mulfile_name, 0x00, (MULTI_IN_FILES_CNT*(MAX_CHARS*LENGTH_OF_FILENAME))+1 );
	//memset(mulfile_size, 0x00, sizeof(mulfile_size) );

	memset(ttlfilename, 0x00, MAX_FILENAME_LEN*sizeof(char));
	
	memset(infile_name,  0x00, (MAX_CHARS*LENGTH_OF_FILENAME)+1 );
	infile_size = 0ULL;
	memset(sefile_name,  0x00, (MAX_CHARS*LENGTH_OF_FILENAME)+1 );
	sefile_size = 0ULL;
	isJoin = 0;
	memset(outfile_name, 0x00, (MAX_CHARS*LENGTH_OF_FILENAME)+1 );
	memset(str_boardName,   0x00, (MAX_CHARS+1) );
	memset(str_moduleName,  0x00, (MAX_CHARS+1) );
	memset(str_versionName, 0x00, sizeof(str_versionName) );
	memset(str_buildDate,   0x00, (MAX_CHARS*2) );
	memset(str_abuild_date, 0x00, (MAX_CHARS+1) );
	memset(str_fillSiz,  0x00, (MAX_CHARS+1) ); /// 2014.07.04
	memset(str_inform,   0x00, (MAX_CHARS+1) );
	memset(str_nb0,      0x00, (MAX_CHARS+1) );
	memset(str_address,  0x00, (MAX_CHARS+1) );
	memset(str_help,     0x00, (MAX_CHARS+1) ); /// 2014.07.04

	memset(str_Verbos,   0x00, (MAX_CHARS+1) );
	memset(str_float,    0x00, (MAX_CHARS*LENGTH_OF_FILENAME)+1 );
	memset(str_BMPType,  0x00, (MAX_CHARS+1) ); /// 2013.07.16
	memset(str_hash,     0x00, (MAX_CHARS+1) ); /// 2014.06.26
#if MODIFIED_JULIAN_DATE 
	memset(str_mjd,      0x00, (MAX_CHARS+1) ); /// 2014.07.04
	memset(str_cur_date, 0x00, (MAX_CHARS+1) ); /// 2014.07.04
#endif /// MODIFIED_JULIAN_DATE 
	

#ifdef _MOT2BIN_
	memset(Mot2bin_Line,     0x00, sizeof(Mot2bin_Line) ); /// 2014.06.26
	memset(Mot2Bin_Data_Str, 0x00, sizeof(Mot2Bin_Data_Str) ); /// 2014.06.26
#endif


	memset( str_cmBuf, 0x00, sizeof(str_cmBuf) );
	memset( strHex2BinLen, 0x00, sizeof(strHex2BinLen) );


	memset( &cHex, 0x00, sizeof(__double_hex_union) );
	memset( &ymd_hms, 0x00, sizeof(mjd_timestamp) );

	memset( crc32_ctab, 0x00, sizeof(crc32_ctab) );
	memset( str_DelHdr, 0x00, sizeof(str_DelHdr) );

	memset( str_crcAdd, 0x00, sizeof(str_crcAdd) );
	memset( str_FileMax, 0x00, sizeof(str_FileMax) );

	memset( str_SecFileSiz, 0x00, sizeof(str_SecFileSiz) ); // 2017.12.11
	is2ndFileSize = 0;
	is2ndEndian = 0;

	memset( strHexType, 0x00, sizeof(strHexType) ); // 2020.06.20

	memset( strZeroForced, 0x00, sizeof(strZeroForced) ); // 2020.06.20

	memset( str_dummy, 0x00, sizeof(str_dummy) );
	memset( str_ignore, 0x00, sizeof(str_ignore) );
	
	

	/// -----------------------------------------------

	is2ndFileSize = 0;
	is2ndEndian = 0;

	/// -----------------------------------------------
	/// --- parameters initialized -------------------------
	/// -----------------------------------------------
	opt_ok          = 0; /* clear */
	isAttach        = 0; 
	isDelHdr        = 0; /* default : Adding Header */
	isIgnoreBothFile= 0; /// ignore input & output
	isIgnoreInFile  = isIgnoreOuFile = 0; // 2020.06.12

	isCRC           = 0; /* CRC Generator */
	isCRCtype       = HDR_CRC_UNKNOWN;
	insertCRC       = 0;
	iUpper          = 0;

	isFillSize      = 0;
	isNKinform      = 0; /* NK information */
	isCreateNB0     = 0;
	isMot2bin       = 0;
	isIntel2bin     = 0;
	isElf2Bin       = 0;
	isAppend        = 0; /// output file : over-write
	isFloat2Hex     = 0;
	isConvertBMP2C  = 0;

	isHash          = 0;
	isMD2           = 0; /// MD2 off
	isMD4           = 0; /// MD4 off
	isMD5           = 0;
	isSHA1          = 0;
	isSHA256        = 0; /// SHA2, 2014.06.30
	isSHA384        = 0; /// SHA2, 2014.06.30
	isSHA512        = 0; /// SHA2, 2014.06.30
	isMD6           = 0; /// 2014.07.31
    isSHA224        = 0; /// 2014.07.31
	isIDEA          = 0; /// IDEA, 2014.07.25

#if SHA3_KECCAK_224_256_384_512
	isSHA3_KECCAK_224 = isSHA3_KECCAK_256 = isSHA3_KECCAK_384 = isSHA3_KECCAK_512 = 0;
	isShake128 = isShake256 = 0;
#endif

	isRipeMD128 = isRipeMD160 = 0;

	isBlake224 = isBlake256 = isBlake384 = isBlake512 = 0;
	isRandomNum = 0;

	isMJD           = 0; /// 2014.07.04
	olen            = 0; /// 2014.07.25
	isBMPreverse    = 0; /// 2015.02.12

	verbose                 = FALSE;
	Starting_Address_Setted = FALSE;
	multifileindex          = 0;

	Minimum_Block_Size_Setted = 0;
	Starting_Address_Setted   = 0;
	Floor_Address_Setted      = 0;
	Ceiling_Address_Setted    = 0;
	Max_Length_Setted         = 0;
	Swap_Wordwise             = 0;
	Address_Alignment_Word    = 0;
	Batch_Mode                = 0;
	Enable_Checksum_Error     = 1; // Enable checksum always ON!!

	Cks_range_set             = 0;
	Cks_Addr_set              = 0;
	Force_Value               = 0;


	iFirst = iLast = -1; /* initial value must be -1 */
	isAsteMode     = ASTERISK_UNKNOWN; /* initial value must be -1 */
	isAsteminus    = 0;
	isPadByte = isPadByteAllArea = 0;
	Enable_HexaAddr_Zero_Forced  = HEX2BIN_REAL_ADDR; // CASE of default & no option

	/// -----------------------------------------------


	help_brief();


	/* ---------------------------------------------------------------------
	A (MOTOROLA) : convert hex to Motorola bin
	L (INTEL)    : convert hex to Intel bin
	------------------------------------------------------------------------*/


#if 1 
	while( EOF != (opt = getopt_long(argc, argv, strOpt, long_options, &option_index)) ) 
#else
	while( EOF != (opt = getopt(argc, argv, strOpt) ) ) 
#endif
	{
     
		switch(opt) 
		{ 

		#if MODIFIED_JULIAN_DATE 
			case 'J' : /// 2014.06.27,  Modified Julian Date--

				if(optarg) 
				{
					memcpy(str_mjd, optarg, MAX_CHARS);
					olen = strlen(str_mjd);

					if( 0==strcasecmp(str_mjd, "test" )  )
					{
						printf("\n---------------- MJD (Modified Julian Date) ----------------- \n");
						

						/// --- Test ------
						/// --- test #1
						cHex.dbl_val = MJDvalue = 50000.000000;
						ymd_hms = Convert2Timestamp( MJDvalue );

						printf("MJD:[ %s ] [", commify(MJDvalue, str_cmBuf, 6)  );
					#if CONVERT_HEX==CONVERT_HEX_MSB
						for(iTMP=0; iTMP<sizeof(double); iTMP++)
						{
							printf("%02X", cHex.dbl_bin[iTMP]);
							if(iTMP<sizeof(double)-1) printf("-");
						}
					#elif CONVERT_HEX==CONVERT_HEX_LSB
						for(iTMP=sizeof(double)-1; iTMP>=0; iTMP--)
						{
							printf("%02X", cHex.dbl_bin[iTMP]);
							if(iTMP>0) printf("-");
						}
					#endif /// CONVERT_HEX_MSB

						printf("] -> DATE:[ %d/%02d/%02d, %02d:%02d:%02d:%03d ] \n", 
									ymd_hms.m_year, ymd_hms.m_month, ymd_hms.m_day, 
									ymd_hms.m_hour, ymd_hms.m_mins,  ymd_hms.m_secs, ymd_hms.m_millis );

						/// --- test #2
						cHex.dbl_val = MJDvalue = 56551.338982;
						ymd_hms = Convert2Timestamp( MJDvalue );

						printf("MJD:[ %s ] [", commify(MJDvalue, str_cmBuf, 6)  );
					#if CONVERT_HEX==CONVERT_HEX_MSB
						for(iTMP=0; iTMP<sizeof(double); iTMP++)
						{
							printf("%02X", cHex.dbl_bin[iTMP]);
							if(iTMP<sizeof(double)-1) printf("-");
						}
					#elif CONVERT_HEX==CONVERT_HEX_LSB
						for(iTMP=sizeof(double)-1; iTMP>=0; iTMP--)
						{
							printf("%02X", cHex.dbl_bin[iTMP]);
							if(iTMP>0) printf("-");
						}
					#endif /// CONVERT_HEX_MSB

						printf("] -> DATE:[ %d/%02d/%02d, %02d:%02d:%02d:%03d ] \n", 
									ymd_hms.m_year, ymd_hms.m_month, ymd_hms.m_day, 
									ymd_hms.m_hour, ymd_hms.m_mins,  ymd_hms.m_secs, ymd_hms.m_millis );

						/// --- test #3
						cHex.dbl_val = MJDvalue = 15020.000000;
						ymd_hms = Convert2Timestamp( MJDvalue );

						printf("MJD:[ %s ] [", commify(MJDvalue, str_cmBuf, 6)  );

					#if CONVERT_HEX==CONVERT_HEX_MSB
						for(iTMP=0; iTMP<sizeof(double); iTMP++)
						{
							printf("%02X", cHex.dbl_bin[iTMP]);
							if(iTMP<sizeof(double)-1) printf("-");
						}
					#elif CONVERT_HEX==CONVERT_HEX_LSB
						for(iTMP=sizeof(double)-1; iTMP>=0; iTMP--)
						{
							printf("%02X", cHex.dbl_bin[iTMP]);
							if(iTMP>0) printf("-");
						}
					#endif /// CONVERT_HEX_MSB

						printf("] -> DATE:[ %d/%02d/%02d, %02d:%02d:%02d:%03d ] \n", 
									ymd_hms.m_year, ymd_hms.m_month, ymd_hms.m_day, 
									ymd_hms.m_hour, ymd_hms.m_mins,  ymd_hms.m_secs, ymd_hms.m_millis );


						if( (outfile = fopen( "number.mjd", "a+")) == NULL )  /// test sample
						{
							beep(700,100);
							printf("\n\n[++ERROR++] Can not create output file (number.mjd). \n" );

							AllFilesClosed();
							exit(0); /// help();
							return 0;
						}

						if( outfile )
						{
							srand( (unsigned)time(NULL)+(unsigned)getpid() );
							for ( idx = 0; idx < MJD_SAMPLE_NUMBER; idx++)
							{
								if(outfile) fprintf(outfile, "%lf \n", (float)( rand()+ (float)(rand()/MATH_PI)) );
							}
							printf("\nThe sample file (number.mjd) is created.. \n" );
						}

						beep(700,100);
						AllFilesClosed();
						
						exit(0);
						return 0;

					}
					else if( 0==strcasecmp(str_mjd, "current" ) )  
					{
						time_t	MJD_t;
						struct tm *mjdt;

						printf("\n---------------- MJD (Modified Julian Date) ----------------- \n");
			

						time( &MJD_t );
						mjdt = localtime( &MJD_t ); // time_t	형식으로 변환합니다.
						
						ymd_hms.m_year   = mjdt->tm_year+1900;
						ymd_hms.m_month  = mjdt->tm_mon+1;
						ymd_hms.m_day    = mjdt->tm_mday;
						ymd_hms.m_hour   = mjdt->tm_hour;
						ymd_hms.m_mins   = mjdt->tm_min;
						ymd_hms.m_secs   = mjdt->tm_sec;
						ymd_hms.m_millis = 0;
						
						cHex.dbl_val = MJDvalue = Convert2MJD( ymd_hms );

						printf("DATE:[ %d/%02d/%02d, %02d:%02d:%02d:%03d ] -> MJD:[ %s ] [", 
								ymd_hms.m_year, ymd_hms.m_month, ymd_hms.m_day, 
								ymd_hms.m_hour, ymd_hms.m_mins,  ymd_hms.m_secs, ymd_hms.m_millis, commify(MJDvalue, str_cmBuf, 6)  );

					#if CONVERT_HEX==CONVERT_HEX_MSB
						for(iTMP=0; iTMP<sizeof(double); iTMP++)
						{
							printf("%02x", cHex.dbl_bin[iTMP]);
							if(iTMP<sizeof(double)-1) printf("-");
						}
					#elif CONVERT_HEX==CONVERT_HEX_LSB
						for(iTMP=sizeof(double)-1; iTMP>=0; iTMP--)
						{
							printf("%02x", cHex.dbl_bin[iTMP]);
							if(iTMP>0) printf("-");
						}
					#endif /// CONVERT_HEX_MSB	
						printf("]\n");
					
						if( (outfile = fopen( "date.mjd", "a+")) == NULL )  /// test sample
						{
							beep(700,100);
							printf("\n\n[++ERROR++] Can not create output file (date.mjd). \n" );
							AllFilesClosed();

							exit(0); /// help();
							return 0;
						}
					

						if( outfile )
						{
							int iYear;
							srand( (unsigned)time(NULL)+(unsigned)getpid() );
							for ( idx = 0; idx < MJD_SAMPLE_NUMBER;  )
							{
								/// *** -> 00000.000000  ---> [ 1858, 11, 17, 0, 0, 0 0 ] 
								iYear = rand()%10000;
								if(iYear >= 1858 )
								{
									if(outfile) fprintf(outfile, "%12d %02d %02d %02d %02d %02d \n", 
												iYear, rand()%13, rand()%32, rand()%24, rand()%60, rand()%60  );
									idx++;
								}
								else
								{
									/// idx --;
								}
							}
							printf("\nThe sample file (date.mjd) is created.. (refer:1858/00/00)\n" );
						}

						beep(700,100);
						AllFilesClosed();

						exit(0);
						return 0;
										
					}
					else if( 0==strcasecmp(str_mjd, "date" ) )  
					{
						isMJD = 1; /// to date
					}
					else if( 0==strcasecmp(str_mjd, "mjd" ) )  
					{
						isMJD = 2; /// to MJD
					}
					else
					{
						printf("\nMJD>> WARNING:Wrong option. --mjd [date|mjd]. Check option.\n");

						beep(700,100);
						AllFilesClosed();

						exit(0);
						return 0;
					}
				}
				else
				{
					printf("\nMJD>> WARNING:option error. check option --mjd [date|mjd]. \r\n");

					beep(700,100);
					AllFilesClosed();

					exit(0);
					return 0;
				}
				break;
		#endif /// MODIFIED_JULIAN_DATE -------------

		#if CONVERT_BMP2C
			case 'B' : /// Convert BMP file to C text file
				if(optarg) 
				{
					olen = 0;
					isConvertBMP2C = 1;
					memcpy(str_BMPType, optarg, MAX_CHARS);
					olen = strlen(str_BMPType);
					
					if( 0 == strncmp(str_BMPType, "888", olen ) )
						iRGB = 888;
					else if( 0 == strncmp(str_BMPType, "444", olen ) )
						iRGB = 444;
					else if( 0 == strncmp(str_BMPType, "555", olen ) )
						iRGB = 555;
					else if( 0 == strncmp(str_BMPType, "565", olen ) )
						iRGB = 565;
					else
					{
						printf("RGB Type : wrong option [%s] \n", str_BMPType);

						beep(700,100);
						AllFilesClosed(); // 2020.07.10
						exit(0); /// 2017.11.21

						return 0;
					}
						
				}
				else
				{
					printf("RGB Type ??? [%s] \n", str_BMPType);

					beep(700,100);
					AllFilesClosed();

					exit(0);
					return 0;
				}
				break;
		#endif /// CONVERT_BMP2C


		#if MD5_CHECKSUM_ENCIPHER 
			case 'M' : /// 2014.06.27, MD5/SHA1/SHA256/SHA384/SHA512 Checksum
				if(optarg) 
				{
					isHash    = 1;

					// initialized
					isMD2	  = 0; /// MD2 off
					isMD4	  = 0; /// MD4 off
					isMD5	  = 0; ///  MD5 on
					isMD6	  = 0;				
					isSHA1	  = 0; /// SHA1 off
					isSHA256  = 0; /// SHA2, 256 off
					isSHA384  = 0; /// SHA2, 384 off 
					isSHA512  = 0; /// SHA2, 512 off 
					isSHA224  = 0; /// 2014.07.31
					isIDEA	  = 0; /// 2014.07.26
					isCRC	  = 0;
					isCRCtype = 0;
					
					isSHA3_KECCAK_224 = 0;
					isSHA3_KECCAK_256 = 0;
					isSHA3_KECCAK_384 = 0;
					isSHA3_KECCAK_512 = 0; // 2020.06.11
					isShake128 = 0;
					isShake256 = 0;
					isRipeMD128 = 0;
					isRipeMD160 = 0;

					isBlake224 = 0; // 2020.0721
					isBlake256 = 0;
					isBlake384 = 0;
					isBlake512 = 0;

					memcpy(str_hash, optarg, MAX_CHARS);
					olen = strlen(str_hash);

					if( 0==strcasecmp(str_hash, "MD5") )
					{
						isMD5    = 1; ///  MD5 on
						iUpper   = isupper(str_hash[0]); // 2017.04.27
					}
					else if( 0==strcasecmp(str_hash, "SHA1") )
					{
						isSHA1   = 1; /// SHA1 on
						iUpper   = isupper(str_hash[0]); // 2017.04.27
					}
					else if( 0==strcasecmp(str_hash, "SHA224") )
					{
						isSHA224 = 1; /// 2014.07.31
						iUpper   = isupper(str_hash[0]); // 2017.04.27
					}
					else if( 0==strcasecmp(str_hash, "SHA256") )
					{
						isSHA256 = 1; /// SHA2, 256 on
						iUpper   = isupper(str_hash[0]); // 2017.04.27
					}
					else if( 0==strcasecmp(str_hash, "SHA384") )
					{
						isSHA384 = 1; /// SHA2, 384 on
						iUpper   = isupper(str_hash[0]); // 2017.04.27
					}
					else if( 0==strcasecmp(str_hash, "SHA512") )
					{
						isSHA512 = 1; /// SHA2, 512 on
						iUpper   = isupper(str_hash[0]); // 2017.04.27
					}
					else if( 0==strcasecmp(str_hash, "MD4") )
					{
						isMD4    = 1; /// MD4 on
						iUpper   = isupper(str_hash[0]); // 2017.04.27
					}
					else if( 0==strcasecmp(str_hash, "MD2") )
					{
						isMD2    = 1; /// MD2 on
						iUpper   = isupper(str_hash[0]); // 2017.04.27
					}
					else if( 0==strcasecmp(str_hash, "MD6") )
					{
						isMD6    = 1; /// MD6 on
						iUpper   = isupper(str_hash[0]); // 2017.04.27
					}
					else if( 0==strcasecmp(str_hash, "SHA3-224") )
					{
						isSHA3_KECCAK_224 = 1;
						iUpper   = isupper(str_hash[0]); // 2017.04.27
					}
					else if( 0==strcasecmp(str_hash, "SHA3-256") )
					{
						isSHA3_KECCAK_256 = 1;
						iUpper   = isupper(str_hash[0]); // 2017.04.27
					}
					else if( 0==strcasecmp(str_hash, "SHA3-384") )
					{
						isSHA3_KECCAK_384 = 1;
						iUpper   = isupper(str_hash[0]); // 2017.04.27
					}
					else if( 0==strcasecmp(str_hash, "SHA3-512") )
					{
						isSHA3_KECCAK_512 = 1;
						iUpper   = isupper(str_hash[0]); // 2017.04.27
					}
					else if( 0==strcasecmp(str_hash, "SHAKE128") )
					{
						isShake128 = 1;
						iUpper   = isupper(str_hash[0]); // 2017.04.27
					}
					else if( 0==strcasecmp(str_hash, "SHAKE256") )
					{
						isShake256 = 1;
						iUpper   = isupper(str_hash[0]); // 2017.04.27
					}
				#if defined(RIPEMD160) || defined(RIPEMD128)	
					else if( 0==strcasecmp(str_hash, "RipeMD128") || (0==strcasecmp(str_hash, "RMD128")) )
					{
						isRipeMD128 = 1;
						iUpper   = isupper(str_hash[0]);
					}
					else if( 0==strcasecmp(str_hash, "RipeMD160") || (0==strcasecmp(str_hash, "RMD160")) )
					{
						isRipeMD160 = 1;
						iUpper   = isupper(str_hash[0]);
					}
				#endif
					else if( 0==strcasecmp(str_hash, "crc16") )
					{
						isCRC	  = 1;
						isCRCtype = HDR_CRC16;
						iUpper    = isupper(str_hash[0]); // 2017.04.27
					}
					else if( 0==strcasecmp(str_hash, "crc16ksc") )
					{
						isCRC	  = 1;
						isCRCtype = HDR_KSC_CRC16;
						iUpper    = isupper(str_hash[0]); // 2017.04.27
					}
					else if( 0==strcasecmp(str_hash, "crc16c") )
					{
						isCRC	  = 1;
						isCRCtype = HDR_CRC16CCITT;
						iUpper    = isupper(str_hash[0]); // 2017.04.27
					}
					else if( 0==strcasecmp(str_hash, "crc32") )
					{
						isCRC	  = 1;
						isCRCtype = HDR_CRC32;
						iUpper    = isupper(str_hash[0]); // 2017.04.27
					}
					else if( 0==strcasecmp(str_hash, "crc64") )
					{
						isCRC	  = 1;
						isCRCtype = HDR_CRC64;
						iUpper    = isupper(str_hash[0]); // 2017.04.27
					}
					else if( 0==strcasecmp(str_hash, "crc64isc") )
					{
						isCRC	  = 1;
						isCRCtype = HDR_CRC64_ISC;
						iUpper	  = isupper(str_hash[0]); // 2020.07.16
					}
					else if( 0==strcasecmp(str_hash, "adler32") ) 
					{
						isCRC	  = 1;
						isCRCtype = HDR_ADLER32;
						iUpper    = isupper(str_hash[0]); // 2017.04.27
					}
					else if( 0==strcasecmp(str_hash, "joaat") ) 
					{
						isCRC	  = 1;
						isCRCtype = HDR_JOAAT;
						iUpper    = isupper(str_hash[0]);
					}

				#if BLAKE_224_256_384_512_HASH // 2020.07.23
					else if( 0==strcasecmp(str_hash, "blake224") )
					{
						isBlake224 = 1;
						iUpper   = isupper(str_hash[0]);
					}
					else if( 0==strcasecmp(str_hash, "blake256") )
					{
						isBlake256 = 1;
						iUpper   = isupper(str_hash[0]);
					}
					else if( 0==strcasecmp(str_hash, "blake384") )
					{
						isBlake384 = 1;
						iUpper   = isupper(str_hash[0]);
					}
					else if( 0==strcasecmp(str_hash, "blake512") )
					{
						isBlake512 = 1;
						iUpper   = isupper(str_hash[0]); 
					}
				#endif

					else
					{
						multifileindex = 0; /// only one input file---
						isMD2    = 0; /// MD2 off
						isMD4	 = 0; /// MD4 off
						isMD5	 = 0; /// MD5 off
						isMD6    = 0;
						isSHA1	 = 0; /// SHA1 off
						isSHA224 = 0; /// 2014.07.31
						isSHA256 = 0; /// SHA2, 256 off
						isSHA384 = 0; /// SHA2, 384 off 
						isSHA512 = 0; /// SHA2, 512 off 
						isIDEA	 = 0; /// 2014.07.26

						isCRC	  = 0;
						isCRCtype = HDR_CRC_UNKNOWN;
						iUpper    = 0; // 2017.04.27
						isSHA3_KECCAK_224 = isSHA3_KECCAK_256 = isSHA3_KECCAK_384 = isSHA3_KECCAK_512 = 0; // 2020.06.11
						isShake128 = isShake256 = 0;
						isBlake224 = isBlake256 = isBlake384 = isBlake512 = 0;
						isRipeMD128 = 0;
						isRipeMD160 = 0;

						printf("\n\nWarning: -M or --checksum MD2|MD4|MD5|MD6 \n");
						printf("                          SHA1|SHA224|SHA256|SHA384|SHA512 \n");
						printf("                          SHA3-224|SHA3-256|SHA3-384|SHA3-512|SHAKE128|SHAKE256 \n");
						printf("                          RipeMD128|RipeMD160 \n");
						printf("                          crc16|crc16c|crc32|crc64|adler32 \n");
						printf("                          CRC16|CRC16C|CRC32|CRC64|ADLER32 \n");

						beep(700,100);

						AllFilesClosed();
						exit(0);

						return 0;
					
					}

				}
				else
				{
					multifileindex = 0; /// only one input file---
					isMD2 = isMD4 = isMD5 = isMD6 = 0;
					isSHA1   = 0; /// SHA1 off
					isSHA224 = isSHA256 = isSHA384 = isSHA512 = 0; /// SHA2 OFF
					isIDEA	 = 0; /// 2014.07.26
					isSHA3_KECCAK_224 = isSHA3_KECCAK_256 = isSHA3_KECCAK_384 = isSHA3_KECCAK_512 = 0; // 2020.06.11
					isShake128 = isShake256 = 0;
					isBlake224 = isBlake256 = isBlake384 = isBlake512 = 0;
					isRipeMD128 = 0;
					isRipeMD160 = 0;

					isCRC     = 0;
					isCRCtype = HDR_CRC_UNKNOWN;
					iUpper	  = 0; // 2017.04.27

					printf("\n\nWarning: -M or --checksum MD2|MD4|MD5|MD6 \n");
					printf("                          SHA1|SHA224|SHA256|SHA384|SHA512 \n");
					printf("                          SHA3-224|SHA3-256|SHA3-384|SHA3-512|SHAKE128|SHAKE256 \n");
					printf("                          RipeMD128|RipeMD160 \n");
					printf("                          crc16|crc16c|crc32|crc64|adler32 \n");
					printf("                          CRC16|CRC16C|CRC32|CRC64|ADLER32 \n");

					beep(700,100);

					AllFilesClosed();

					exit(0);
					return 0;

				}
				break;
		#endif /// MD5_CHECKSUM_ENCIPHER -------------


			case 'g' : /* ignore inputfile and output file */

				if(optarg) 
				{
					memcpy(str_ignore, optarg, sizeof(str_ignore) );

					if( 0 == strcasecmp (str_ignore, "both" ) )
						isIgnoreBothFile = 1;
					else if( 0 == strcasecmp (str_ignore, "input" ) )
						isIgnoreInFile = 1;
					else if( 0 == strcasecmp (str_ignore, "output" ) )
						isIgnoreOuFile = 1;
				}

				break;


			case 'f' : /* convert float number to Hex-decial for using DR_GPS Trimble packet */

				olen = 0;
				isFloat2Hex = 1;
				//printf("str_float=[%s] argc=%d, argv[3]=%s, argv[4]=%s, argv[5]=%s argv[6]=%s \n", 
				//	str_float, argc, argv[3], argv[4], argv[5], argv[6] );

				if(optarg) 
				{
					memcpy(str_float, optarg, MAX_CHARS*LENGTH_OF_FILENAME );
					f2h_float = atof(str_float);
				}
				else
				{
					printf("\n\n WARNING:wrong option --float [float number]. \r\n");
			
					beep(700,100);
					AllFilesClosed(); // 2020.07.10
					exit(0); /// 2017.11.21
			
					return 0;
				}

				#if 0
				switch(argc)
				{
					case 5:
						if(optarg) 
						{
							memcpy(str_float, optarg, MAX_CHARS*LENGTH_OF_FILENAME );
							f2h_float = atof(str_float);
						}
						else
						{
							printf("\n\n WARNING:wrong option --float [float number]. \r\n");

							beep(700,100);
							AllFilesClosed(); // 2020.07.10
							exit(0); /// 2017.11.21

							return 0;
						}
						break;

					case 6:
						if(argv[3]) 
						{
							memcpy(str_float, argv[3], MAX_CHARS*LENGTH_OF_FILENAME );
							olen = strlen(str_float);
						}

						if( 0 == strcasecmp (str_float, "sin" ) )
						{
							//printf("  sin(%s) ", argv[4] );
							f2h_float = atof( argv[4] );
							f2h_float = (f2h_float*MATH_PI)/180.0;
							f2h_float = sin(f2h_float);
						}
						else if( 0 == strcasecmp (str_float, "cos") )
						{
							//printf("  cos(%s) ", argv[4] );
							f2h_float = atof( argv[4] );
							f2h_float = (f2h_float*MATH_PI)/180.0;
							f2h_float = cos(f2h_float);
						}
						else if( 0 == strcasecmp (str_float, "tan") )
						{
							//printf("  tan(%s) ", argv[4] );
							f2h_float = atof( argv[4] );
							f2h_float = (f2h_float*MATH_PI)/180.0;
							f2h_float = tan(f2h_float);
						}
						else
						{
							printf("\n WARNING:float2hex: invalid format! [%s] \n", str_float );

							beep(700,100);
							AllFilesClosed(); // 2020.07.10
							exit(0); /// 2017.11.21

							return 0;
						}
						break;

					default:
						break;
				}
				#endif
				break;

		    case 'h' : /* help */
				if(optarg)
				{
					memcpy(str_help, optarg, MAX_CHARS);
					olen = strlen(str_help);
					
					if( 0 == strcasecmp(str_help, "intel" ) )
						help_int2bin_form();
					else if( 0 == strcasecmp(str_help, "motorola" ) )
						help_mot2bin_form();
					else
					{
						isIgnoreBothFile = 1; // 2020.06.08
						help();
					}
				}
				else
				{
					help();
				}


				AllFilesClosed(); // 2020.07.10
				exit(0);
				return 0;

			case 'S': /* --startaddr : Starting Address hex2bin */

				printf("\n");
				if( (isMot2bin!=1) && (isIntel2bin!=1) )
				{
					printf("[++ERROR Starting Address++] Need option --intel or --motorola \n");

					beep(700,100);
					AllFilesClosed(); // 2020.07.10

					exit(0); /// 2017.11.21

					return 0;					
					break;
				}

				if(optarg) 
				{
					memcpy(str_address, optarg, MAX_CHARS);
					olen = strlen(str_address);

					if( olen > MAX_CHARS )
					{
						printf("\n\n[++ERROR++] Start Address(Hex) is too long (%d).. Max:16 \n\n", olen );
					}
					
					ret_scan = sscanf(str_address,"%x",&Starting_Address);
					if( 1!=ret_scan )
					{
						printf("\n\n[++ERROR++] str_address is wrong. It must be Hex Value \n\n" );
					}

					Starting_Address_Setted = TRUE;

			        printf("\nHEX2BIN>> Start address : %s (0x%X) \n", str_address, Starting_Address);
				}
				else
				{
					printf("\nHEX2BIN>> WARNING:wrong option --start [hexa value]. check option\r\n");

					beep(700,100);
					AllFilesClosed(); // 2020.07.10
					exit(0); /// 2017.11.21

					return 0;
				}
				break;

			case 'e': /// elf 2 bin
				isElf2Bin = 1;
				break;

		    case 'A': /* convert mot2bin -> Motorola FORMAT family --- */
				printf("\n");
				printf(">>Hex family type : MOTOROLA family");

				if( 1==isIntel2bin ) // error
				{
					printf("\n");
					printf("[++ERROR Motorola++] Choice one --motorola or --intel option \n");
				
					beep(700,100);
					AllFilesClosed(); // 2020.07.10
					exit(0); /// 2017.11.21

					return 0;
					break;
				}

				isMot2bin=1;

				break;
				
		    case 'L': /* convert intel2bin -> Intel FORMAT family --- */
				printf("\n");
				printf(">>Hex family type : Intel family");

				if( 1==isMot2bin ) // Error
				{
					printf("\n");
					printf("[++ERROR Intel++] Choice one --intel or --motorola option \n");
				
					beep(700,100);
					AllFilesClosed(); // 2020.07.10
					exit(0); /// 2017.11.21

					return 0;
					break;
				}

				isIntel2bin=1;
				break;

		    case 'n': /* --alignword : Address Alignment Word -> Intel family only --- */
				if( 1==isMot2bin ) 
				{
					printf("\n");
					printf("[++ERROR ALIGNWORD++] Need option --intel option only \n");
				
					beep(700,100);
					AllFilesClosed(); // 2020.07.10
					exit(0); /// 2017.11.21

					return 0;
					break;
				}

				if( 1==isIntel2bin )
				{
					printf(">>Addr Alignment  : Enable address alignment word in Intel family");
					Address_Alignment_Word = 1; // true;
				}
				break;
				
			case 'P': // --padbyte

				printf("\n");
				if( (isMot2bin!=1) && (isIntel2bin!=1) && (1!=isFileMerge) && (1!=isFillSize) )
				{
					printf("[++ERROR PADBYTE++] Need option --intel or --motorola or --fillsize [size] \n");
				

					beep(700,100);
					AllFilesClosed(); // 2020.07.10
					exit(0); /// 2017.11.21
					return 0;					
					break;
				}

				Pad_Byte = 0xff;
				if(optarg) 
				{
					memset( strPadByte, 0x00, sizeof(strPadByte) );
					memcpy( strPadByte, optarg, MAX_CHARS);
					olen = strlen(strPadByte);

					isPadByte = 1;
					Pad_Byte = GetHex( strPadByte );

					printf(">>Padding Byte    : 0x%x (default:ff) ", Pad_Byte);
				}
				else
				{
					printf("\n\n WARNING:wrong option --padbyte [hexa value]. check option [%s] \r\n", strPadByte);
				
					beep(700,100);
					AllFilesClosed(); // 2020.07.10
					exit(0); /// 2017.11.21
					return 0;
				}
				break;

			case 'E': // --endian

				if( (isMot2bin!=1) && (isIntel2bin!=1) && (isFileMerge!=1) )
				{
					printf("\n");
					printf("[++ERROR Endian++] Need option --intel or --motorola or --join \n");
					beep(700,100);
					AllFilesClosed(); // 2020.07.10
					exit(0); /// 2017.11.21

					return 0;
					break;
				}

				if(optarg) 
				{
					Endian = 0; // default little for INTEL or MOTOROLA family

					is2ndFileSize = 0;
					is2ndEndian = 0;

					memset(str_SecFileSiz, 0x00, sizeof(str_SecFileSiz) );
					memcpy(str_SecFileSiz, optarg, MAX_CHARS);

					printf("\n");
					if( 0==strcasecmp( str_SecFileSiz, "little" ) ) 
					{
						if(isFileMerge)
						{
							is2ndFileSize = 1;
							is2ndEndian = ENDIAN_LITTLE; // little
							printf(">>Endian          : Little endian");
						}
						else // For INTEL Hex2bin or MOTOROLA hex2bni
						{
							printf(">>Endian for CRC  : little endian");
							Endian = 0; // little
						}
					}
					else if( 0==strcasecmp( str_SecFileSiz, "big" ) ) 
					{
						if(isFileMerge)
						{
							is2ndFileSize = 1;
							is2ndEndian = ENDIAN_BIG; // big
							printf(">>Endian          : BIG endian");
						}
						else // For INTEL Hex2bin or MOTOROLA hex2bni
						{
							printf(">>Endian for CRC  : Big endian");
							Endian = 1; // big
						}

					}
					else
					{
						printf("[++ERROR Endian++] Need option --intel or --motorola or --join \n");
					
						beep(700,100);
						AllFilesClosed(); // 2020.07.10
						exit(0); /// 2017.11.21

						return 0;
						break;
					}
				}
				else
				{
					printf("\n\n WARNING:wrong option --endian [little|big]. check option\r\n");
			
					beep(700,100);
					AllFilesClosed(); // 2020.07.10
					exit(0); /// 2017.11.21

					return 0;
				}
				break;

		    case 'k':  /* --allpadarea : fill Padbye in all empty area */

				printf("\n");
				if( (isMot2bin!=1) && (isIntel2bin!=1) )
				{
					printf("\n[++ERROR PADAREA++] Need option --intel or --motorola \n");

					beep(700,100);
					AllFilesClosed(); // 2020.07.10
					exit(0); /// 2017.11.21

					return 0;					
					break;
				}

				printf(">>Fill Pad Byte   : Pad byte(0x%X) in empty ALL area of binary \n", Pad_Byte);

				isPadByteAllArea = 1;  // The specified char (Pad_Byte) is filled in all area.
				break;

			case 'Z': // --zeroforced

				printf("\n");
				if( (isMot2bin!=1) && (isIntel2bin!=1) )
				{
					printf("\n");
					printf("[++ERROR ZERO_FORCED++] Need option --intel or --motorola \n");

					beep(700,100);
					AllFilesClosed(); // 2020.07.10
					exit(0); /// 2017.11.21

					return 0;
					break;
				}

				Enable_HexaAddr_Zero_Forced = HEX2BIN_ZERO_FORCED;
				printf(">>Address Forced  : Using ZERO addressing forced. \n");

				break;

		    case 'l': /* Hex2bin Max size - length */

			#if 0 // 2021.12.27
				if( (isMot2bin!=1) && (isIntel2bin!=1) )
				{
					printf("\n");
					printf("\n[++ERROR++] Need option --intel or --motorola \n");

					beep(700,100);
					AllFilesClosed(); // 2020.07.10
					exit(0); /// 2017.11.21

					return 0;
					break;
				}
			#endif

			
				if(optarg) 
				{
					olen = 0;
			        memcpy(strHex2BinLen, optarg, MAX_CHARS);
					olen = strlen(strHex2BinLen);

					if( strcasecmp( &strHex2BinLen[olen-2], "kB" ) == 0 )
					{
						Max_Length_Setted = 1;
						strHex2BinLen[olen-2] = 0x00;
						strHex2BinLen[olen-1] = 0x00;
						strHex2BinLen[olen-0] = 0x00;
						Max_Length = str2int(strHex2BinLen);
						Max_Length *= 1024; // Because of KBytes
					}
					else if( strcasecmp( &strHex2BinLen[olen-2], "MB" ) == 0 )
					{
						Max_Length_Setted = 1;
						strHex2BinLen[olen-2] = 0x00;
						strHex2BinLen[olen-1] = 0x00;
						strHex2BinLen[olen-0] = 0x00;
						Max_Length = str2int(strHex2BinLen);
						Max_Length *= (1024*1024); // Because of MBytes
					}
					else
					{
						Max_Length_Setted = 1;
						Max_Length = GetHex( strHex2BinLen );

						isLength = 1;
						titlen = GetDec( strHex2BinLen ); // 2021.12.27
					}

				#if 0
					if (Max_Length > 0x800000)
					{
						printf("Hex2bin Max Length Error = %u (Max Length = 0x800000) \n", Max_Length);
						Max_Length_Setted = 0;
						exit(1);
					}
				#endif
					
				}
				else
				{
					printf("\n WARNING:wrong --length [hexa value] in converting --intel or --motorola. check option [%s] \n", strHex2BinLen);

					beep(700,100);
					AllFilesClosed(); // 2020.07.10
					exit(0); /// 2017.11.21

					return 0;
				}
				break;

			case 'R':

				if(optarg) 
				{
			        memcpy(str_dummy, optarg, MAX_CHARS);
					olen = strlen(str_dummy);

					isRandomNum = 1;

					iRanMaxi = str2int(str_dummy);
				}
				break;
				
		    case 'N': /* infotmation */
				if(optarg) 
				{
			    	isNKinform  = 1;
			        memcpy(str_nb0, optarg, MAX_CHARS);
					olen = strlen(str_nb0);

					if( 0==strncmp(NB0_EXT, str_nb0, olen) )
						isCreateNB0 = 1; /// Create *.nb0 file.
					else
						isCreateNB0 = 0;
					
				}
				else
				{
					printf("\n\n WARNING:wrong option --nk [nb0 | none]. check option\r\n");

					beep(700,100);
					AllFilesClosed(); // 2020.07.10
					exit(0); /// 2017.11.21

					return 0;
				}
				break;
			
		    case 'I': /* infotmation */
				if(optarg) 
				{
			        memcpy(str_inform, optarg, MAX_CHARS);
			        FileCounter(str_inform);
				}

				AllFilesClosed(); // 2020.07.16
		        exit(0);
		    	break;

		    case 'F':

				if(optarg) 
				{
			    	isFillSize = 1; /* Fill 0xFF */

					memcpy(str_fillSiz, optarg, MAX_CHARS);
					olen = strlen(str_fillSiz);

			        printf("\n>>Total file size : %s ", str_fillSiz );
				
					if( 0 == strcasecmp( &str_fillSiz[olen-2], "kB" ) )
					{
						str_fillSiz[olen-2] = 0x00;
						str_fillSiz[olen-1] = 0x00;
						str_fillSiz[olen-0] = 0x00;
						total_bin_size = str2int(str_fillSiz);
						total_bin_size *= 1024; // Because of KBytes
						printf(" (%#x) ", total_bin_size );
					}
					else if( 0 == strcasecmp( &str_fillSiz[olen-2], "MB" ) )
					{
						str_fillSiz[olen-2] = 0x00;
						str_fillSiz[olen-1] = 0x00;
						str_fillSiz[olen-0] = 0x00;
						total_bin_size = str2int(str_fillSiz);
						total_bin_size *= (1024*1024); // Because of MBytes
						printf(" (%#x) ", total_bin_size );
					}
					else
					{
						total_bin_size = GetHex( str_fillSiz );
						printf(" (0x%x) %dBytes ", total_bin_size, total_bin_size );
					}

				}
				else
				{
					printf("\n\n WARNING:wrong option --fillsize [value]. check option\r\n");

					beep(700,100);
					AllFilesClosed(); // 2020.07.10
					exit(0); /// 2017.11.21

					return 0;
				}

		    	break;

			case 'j': /* 2017.04.05, File merge */

				if(optarg) 
				{
				
				#if 0
					isFileMerge = 1;
					memcpy(str_FileMax, optarg, MAX_CHARS);
					result = sscanf(str_FileMax,"%x",&iFileLimit);
					if( result==1 ) 
					{
						// OK
						printf("\n>>2nd file pos    : %#x (%u)", iFileLimit, iFileLimit );
					}
					else
					{
						// Hex read error
						printf("\n>>2nd file pos    : ++ERROR++ (%s)" , str_FileMax );
					}

				#else
				
					olen = 0;
					memcpy(str_FileMax, optarg, MAX_CHARS);
					olen = strlen(str_FileMax);
					isFileMerge = 1;
					
					if( strcasecmp( &str_FileMax[olen-2], "kB" ) == 0 )
					{
						str_FileMax[olen-2] = 0x00;
						str_FileMax[olen-1] = 0x00;
						str_FileMax[olen-0] = 0x00;
						iFileLimit = str2int(str_FileMax);
						iFileLimit *= 1024; // Because of KBytes

						printf("\n>>2nd file pos    : %#x (%skB)", iFileLimit, str_FileMax );
					}
					else if( strcasecmp( &str_FileMax[olen-2], "MB" ) == 0 )
					{
						str_FileMax[olen-2] = 0x00;
						str_FileMax[olen-1] = 0x00;
						str_FileMax[olen-0] = 0x00;
						iFileLimit = str2int(str_FileMax);
						iFileLimit *= (1024*1024); // Because of MBytes

						printf("\n>>2nd file pos    : %#x (%sMB)", iFileLimit, str_FileMax );
					}
					else
					{
						iFileLimit = GetHex( str_FileMax );
						printf("\n>>2nd file pos    : %#x (%u Bytes)", iFileLimit, iFileLimit );
					}

					//printf("\n>>2nd file pos    : %#x (%u)", iFileLimit, iFileLimit );
				#endif

				}
				else
				{
					printf("\n\n WARNING:wrong option --join [in hexa]. check option\r\n");

					beep(700,100);
					AllFilesClosed(); // 2020.07.10
					exit(0); /// 2017.11.21

					return 0;
				}

				break;

			#if 0
			case 's':
				if(isFileMerge)
				{
					is2ndFileSize = 1;
					is2ndEndian   = 0; // initial value none
					memcpy(str_SecFileSiz, optarg, MAX_CHARS);

					if( 0==strcasecmp( str_SecFileSiz, "little" ) ) 
					{
						is2ndEndian = ENDIAN_LITTLE; // little
					}
					else if( 0==strcasecmp( str_SecFileSiz, "big" ) ) 
					{
						is2ndEndian = ENDIAN_BIG; // big
					}

						
					//result = sscanf(str_SecFileSiz,"%x",&i2ndFileSize);
					//printf("\n>>Join Address : 0x%X \r\n", iFileLimit);				
				}
				else
				{
					is2ndFileSize = 0;
					is2ndEndian = 0;
					memset(str_SecFileSiz, 0x00, MAX_CHARS);

					printf("\n>>Do use --join option!! \n");
				}

				break;
			#endif


		    case 'd': /* detach Header */
				if(optarg) 
				{
			    	isDelHdr = 1; /* detach Header */
				    memcpy(str_DelHdr, optarg, MAX_CHARS);
					olen = strlen(str_DelHdr);

			    	printf("\n>>Delete HDR size : ");

					if( 0 == strcasecmp( &str_DelHdr[olen-2], "kB" ) )
					{
						str_DelHdr[olen-2] = 0x00;
						str_DelHdr[olen-1] = 0x00;
						str_DelHdr[olen-0] = 0x00;

						iHdr_len = str2int(str_DelHdr);
						printf(" (%dkB) 0x%x \n", iHdr_len, iHdr_len );
					
						iHdr_len *= 1024; // Because of KBytes
					}
					else if( 0 == strcasecmp( &str_DelHdr[olen-2], "MB" ) )
					{
						str_DelHdr[olen-2] = 0x00;
						str_DelHdr[olen-1] = 0x00;
						str_DelHdr[olen-0] = 0x00;
						iHdr_len = str2int(str_DelHdr);
						printf(" (%dMB) 0x%x \n", iHdr_len, iHdr_len );
					
						iHdr_len *= (1024*1024); // Because of MBytes
					}
					else if( 0 == strcasecmp( str_DelHdr, "default" ) )
					{
						iHdr_len=16*4; // default size
						printf("default (%dBytes) \n", iHdr_len );
					}
					else
					{
						iHdr_len = str2int(str_DelHdr);
						if( iHdr_len == 0 ) 
						{
							iHdr_len=16*4; // default size
							printf("default (%dBytes) 0x%x \n", iHdr_len, iHdr_len );
						}
						else
							printf(" (%dBytes) 0x%x \n", iHdr_len, iHdr_len );
					}

				}
				else
				{
					printf("\n\n WARNING:wrong option --detach [decimal value]. check option\r\n");

					beep(700,100);
					AllFilesClosed(); // 2020.07.10
					exit(0); /// 2017.11.21

					return 0;
				}				

		        break;

	#ifdef  __ING___
			case 'r': /* BMP reverse */
				isBMPreverse = 1;
				break;
	#endif

	
		    case 'b': /* Attach Header : 16 characters */
				if(optarg) 
				{
					olen = 0;
					isAttach |= ATT_BOARD;

			        memcpy(str_boardName, optarg, MAX_CHARS);
					olen = strlen(str_boardName);

			        printf("\n>>Board Name      : %s", str_boardName);

					if( olen > MAX_CHARS )
					{
						printf("\n\n[++ERROR++] Board Name length is too long (%d Chars).. Max:16 Bytes\n\n", olen );
					}
				}
				else
				{
					printf("\n\n WARNING:wrong option --board [string]. check option\r\n");

					beep(700,100);
					AllFilesClosed(); // 2020.07.10
					exit(0); /// 2017.11.21

					return 0;
				}
		        break;

				
		    case 'm': /* Attach Header : 16 characters */
				if(optarg) 
				{
					olen = 0;
					isAttach |= ATT_MODEL;

					if(titlen)
					{
						printf("\n\n[length] %d  \r\n", titlen );
					}
					else
					{
				        memcpy(str_moduleName, optarg, MAX_CHARS);
						olen = strlen(str_moduleName);

				        printf("\n>>Model Name      : %s", str_moduleName);

						if( olen > MAX_CHARS )
						{
							printf("\n\n[++ERROR++] Module Name length is too long (%d Chars).. Max:16 Bytes\n\n", olen );
						}
					}

				}
				else
				{
					printf("\n\n WARNING:wrong option --model [string]. check option\r\n");

					beep(700,100);
					AllFilesClosed(); // 2020.07.10
					exit(0); /// 2017.11.21

					return 0;
				}
		        break;
		
		    case 'c': /* Attach Header : 16 characters ; date / crc16/crc32/crc64/adler32 */

				isMD2 = isMD4 = isMD5 = isMD6 = isSHA1 = isSHA224 = isSHA256 = isSHA384 = isSHA512 = isIDEA = 0;
				isSHA3_KECCAK_224 = isSHA3_KECCAK_256 = isSHA3_KECCAK_384 = isSHA3_KECCAK_512 = 0; // 2020.06.11
				isShake128 = isShake256 = 0;
				isBlake224 = isBlake256 = isBlake384 = isBlake512 = 0;
				isCRC = 0;
				isRipeMD128 = 0;
				isRipeMD160 = 0;
				
				if(optarg) 
				{
					memcpy(str_crcAdd, optarg, MAX_CHARS);
					isCRC	  = 0;
					insertCRC = 0;
					iUpper    = 0;
					olen = strlen(str_crcAdd);

					printf("\n>>Security cinfo  : %s", str_crcAdd);

					strcpy(str_hash, str_crcAdd);

					if( 0==strcasecmp( str_crcAdd, "date" ) ) // inserted date
				    {
				    	int i;
				    	time_t 	sys_t;

						isAttach |= ATT_DATEorCRC;

				    	time( &sys_t );
				    	strcpy( str_buildDate, ctime( &sys_t ) );
				        /*Mon Dec  8 16:41:19 2008*/
						for(i=0; i<MAX_CHARS*2; i++)
						{ if( '\n' == str_buildDate[i] ) str_buildDate[i] = '\0'; }


						/* uppercase */
						for(i=1; i<3; i++) { str_buildDate[i] = str_buildDate[i]^0x20; }
				        
				        /* -> 2008L08MON164119*/
				        /* step1. year */
						memcpy(&str_abuild_date[0], &str_buildDate[20], 4);

				        /* step2. month */
						for( iloop=0; iloop<MONTH_LEN; iloop++)
						{
							if( 0==strncmp(&str_buildDate[4], month_table[iloop].mon, 3 ) )
								break;
						}
						memcpy(&str_abuild_date[4], &month_table[iloop].amon, 1);

				        /* step3. date */
						memcpy(&str_abuild_date[5], &str_buildDate[8], 2);

				        /* step4. week */
						memcpy(&str_abuild_date[7], &str_buildDate[0], 3);

				        /* step5. hour */
						memcpy(&str_abuild_date[10], &str_buildDate[11], 2);

				        /* step5. minute */
						memcpy(&str_abuild_date[12], &str_buildDate[14], 2);

				        /* step6. second */
						memcpy(&str_abuild_date[14], &str_buildDate[17], 2);

						#ifdef DEBUG
						printf("\nABuild Date>>%s<<", str_abuild_date);
						#endif

						printf("\n>>Make curr today : %s", str_buildDate);
				        break;
						
				    }   
					else if( 0==strcasecmp(str_crcAdd, "crc16") )
					{
						isCRC	  = 0;
						insertCRC = 1;
						isCRCtype = HDR_CRC16;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]); // 2017.04.27
						printf(" is inserted as signed image.");
					}
					else if( 0==strcasecmp(str_crcAdd, "crc16ksc") )
					{
						isCRC	  = 0;
						insertCRC = 1;
						isCRCtype = HDR_KSC_CRC16;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]); // 2017.04.27
						printf(" is inserted as signed image.");
					}
					else if( 0==strcasecmp(str_crcAdd, "crc16c") )
					{
						isCRC	  = 0;
						insertCRC = 1;
						isCRCtype = HDR_CRC16CCITT;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]); // 2017.04.27
						printf(" is inserted as signed image.");
					}
					else if( 0==strcasecmp(str_crcAdd, "crc32") )
					{
						isCRC	  = 0;
						insertCRC = 1;
						isCRCtype = HDR_CRC32;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]); // 2017.04.27
						printf(" is inserted as signed image.");
					}
					else if( 0==strcasecmp(str_crcAdd, "crc64") )
					{
						isCRC	  = 0;
						insertCRC = 1;
						isCRCtype = HDR_CRC64;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]); // 2017.04.27
						printf(" is inserted as signed image.");
					}
					else if( 0==strcasecmp(str_crcAdd, "crc64isc") )
					{
						isCRC	  = 0;
						insertCRC = 1;
						isCRCtype = HDR_CRC64_ISC;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]); // 2017.04.27
						printf(" is inserted as signed image.");
					}
					else if( 0==strcasecmp(str_crcAdd, "adler32") ) 
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_ADLER32;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]); // 2017.04.27
						printf(" is inserted as signed image.");
					}	
					else if( 0==strcasecmp(str_crcAdd, "joaat") ) 
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_JOAAT;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]);
						printf(" is inserted as signed image.");
					}
					else if( 0==strcasecmp(str_crcAdd, "sha1") )	// SHA1
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_SHA1;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]);  
						printf(" is inserted as signed image.");
					}	
					else if( 0==strcasecmp(str_crcAdd, "sha224") )	// SHA2-224
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_SHA224;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]); // 2017.11.21
						printf(" is inserted as signed image.");
					}	
					else if( 0==strcasecmp(str_crcAdd, "sha256") )  // SHA2-256
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_SHA256;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]); // 2017.11.21
						printf(" is inserted as signed image.");
					}	
					else if( 0==strcasecmp(str_crcAdd, "sha384") ) // SHA2-384
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_SHA384;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]); // 2017.11.21
						printf(" is inserted as signed image.");
					}	
					else if( 0==strcasecmp(str_crcAdd, "sha512") ) // SHA2-512
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_SHA512;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]); // 2017.11.21
						printf(" is inserted as signed image.");
					}	
					else if( 0==strcasecmp(str_crcAdd, "sha3-224") ) 
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_SHA3_224;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]);
						printf(" is inserted as signed image.");
					}	
					else if( 0==strcasecmp(str_crcAdd, "sha3-256") ) 
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_SHA3_256;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]);
						printf(" is inserted as signed image.");
					}	
					else if( 0==strcasecmp(str_crcAdd, "sha3-384") ) 
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_SHA3_384;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]);
						printf(" is inserted as signed image.");
					}	
					else if( 0==strcasecmp(str_crcAdd, "sha3-512") ) 
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_SHA3_512;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]);
						printf(" is inserted as signed image.");
					}	
					else if( 0==strcasecmp(str_crcAdd, "shake128") ) 
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_SHAKE128;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]);
						printf(" is inserted as signed image.");
					}	
					else if( 0==strcasecmp(str_crcAdd, "shake256") ) 
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_SHAKE256;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]);
						printf(" is inserted as signed image.");
					}	
					else if( 0==strcasecmp(str_crcAdd, "md5") ) // MD5
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_MD5;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]); // 2017.11.21
						printf(" is inserted as signed image.");
					}	
					else if( 0==strcasecmp(str_crcAdd, "md6") ) // MD6
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_MD6;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]); // 2017.11.21
						printf(" is inserted as signed image.");
					}	
					else if( 0==strcasecmp(str_crcAdd, "md2") ) // MD2
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_MD2; // 2020.07.15
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]); // 2017.11.21
						printf(" is inserted as signed image.");
					}	
					else if( 0==strcasecmp(str_crcAdd, "md4") ) // MD4
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_MD4; // 2020.07.15
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]); // 2017.11.21
						printf(" is inserted as signed image.");
					}	
				#if BLAKE_224_256_384_512_HASH
					else if( 0==strcasecmp(str_crcAdd, "blake224") ) 
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_BLAKE224;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]);
						printf(" is inserted as signed image.");
					}	
					else if( 0==strcasecmp(str_crcAdd, "blake256") ) 
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_BLAKE256;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]);
						printf(" is inserted as signed image.");
					}	
					else if( 0==strcasecmp(str_crcAdd, "blake384") ) 
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_BLAKE384;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]);
						printf(" is inserted as signed image.");
					}	
					else if( 0==strcasecmp(str_crcAdd, "blake512") ) 
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_BLAKE512;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]);
						printf(" is inserted as signed image.");
					}	
				#endif

				#if defined(RIPEMD160) || defined(RIPEMD128)	
					else if( 0==strcasecmp(str_crcAdd, "RipeMD128") || (0==strcasecmp(str_crcAdd, "RMD128")) )
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_RMD128;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]);
						printf(" is inserted as signed image.");
					}	
					else if( 0==strcasecmp(str_crcAdd, "RipeMD160") || (0==strcasecmp(str_crcAdd, "RMD160")) )
					{
						isCRC	  = 0;
						insertCRC = 1; 
						isCRCtype = HDR_RMD160;
						isAttach |= ATT_DATEorCRC;
						iUpper = isupper(str_crcAdd[0]);
						printf(" is inserted as signed image.");
					}	
				#endif		
					else
					{
						printf("\n\n WARNING:wrong option --cinfo [string]. check option (%s) \r\n", str_crcAdd);

						beep(700,100);
						AllFilesClosed();

						exit(0);
						return 0;
					}
				}
				else
				{
					printf("\n\n WARNING:wrong option --cinfo [string]. check option\r\n");
					beep(700,100);
					AllFilesClosed();
					exit(0);
					return 0;
				}
		    	break;


		    case 'v': /* Attach Header : 16 characters */

				if(optarg) 
				{
					olen = 0;
					isAttach |= ATT_VERSION;

					memcpy(str_versionName, optarg, MAX_VERSION_LEN);
					olen = strlen(str_versionName);
					
					printf("\n>>Version Name    : %s", str_versionName);

					if( olen > MAX_VERSION_LEN )
					{
						printf("\n\n[++ERROR++] Version Name length is too long (%d Chars).. Max:%d Bytes\n\n", olen, MAX_VERSION_LEN );
					}
				}
				else
				{
					printf("\n\n WARNING:wrong option --version [string]. check option\r\n");

					beep(700,100);
					AllFilesClosed(); // 2020.07.10
					exit(0); /// 2017.11.21

					return 0;
				}

				break;
		
		    case 'i': /* input file name : 32 characters */
				olen = 0;
				multifileindex = 0;
				iTotSize = 0ULL;

			#if 1
				iFileNum = 0;
				for(ii=1; ii<argc ; ii++)
				{
					if( (retFiles = _findfirsti64( argv[ii], &fexist )) == -1L )
					{
						if( ii>3 && (argv[ii][0] == '-') ) 
						{
							if( (iFileNum>2) && (-1!=iFirst) && (-1==iLast) ) 
							{
								if( (iFileNum>=2) && (ASTERISK_STEP1==isAsteMode) ) 
								{
									//printf("---++--(%d) (%d) (%d) \n", iFileNum, ii, argc); 
									isAsteMode = ASTERISK_FOUND;
								}
								iLast = ii+1;
							}
							else break;
						}
						//printf(" %d : [%s] ++++++ -> %d %d ", ii, argv[ii], iFirst, iLast );
					}
					else
					{
						if( (-1==iFirst) && (-1==iLast) )  
						{
							isAsteMode = ASTERISK_STEP1;
							iFirst = ii;
						}
						iFileNum ++;
						//printf(" %d : [%s] in current directory!  -> %d %d [%d] ", ii, argv[ii], iFirst, iLast, iFileNum );
					}
					_findclose( retFiles );

					//printf(" -> AsteMode %d \n", isAsteMode);
					if( ASTERISK_FOUND==isAsteMode ) break;
				}
				if( retFiles ) _findclose( retFiles );

			#endif		


			#if 1 // 2017.04.05
				isJoin=0;
				for(ii=0; ii<argc; ii++)
				{
					if( 0==strcmp(argv[ii], "-j") || 0==strcmp(argv[ii], "--join") )
					{
						isJoin = 1;
						break;
					}
					else isJoin=0;
				}

				if(isJoin)
				{
					for(ii=0; ii<argc; ii++)
					{
						if( isJoin && (0==strcmp(argv[ii], "-i") || 0==strcmp(argv[ii], "--input")) )
						{
							memset(sefile_name, 0x00, MAX_CHARS*LENGTH_OF_FILENAME);
							strcpy(sefile_name, argv[ii+2] );
							//printf("2nd input=[[ %s ]] \n", sefile_name);	
							isJoin = 2; // OK
							break;
						}
					}
				}

				if( 2==isJoin && (sefile_name[0] != 0x00) && (sefile_name[0] != '-') ) isJoin=3;

			#endif


				if(optarg) 
				{
					memcpy(infile_name, optarg, MAX_CHARS*LENGTH_OF_FILENAME);
					olen = strlen(infile_name);
				}
				else
				{
					printf("\n\n[++ERROR++] Input file is NULL.  check option --input [filename]. \n\n" );
					
					beep(700,100);

					AllFilesClosed();

					exit(0);
					return 0;
				}

				
			#if MD5_MULTI_INPUT_FILES
				if( NULL != strstr(infile_name, "@." )  /// NOT olen 
					||  NULL != strstr(infile_name, "$." ) 
					)
				{
					int iIdx=0;
					int iLenFile = 0;
					iLenFile = strlen(infile_name);
					
					for(iIdx=0; iIdx<iLenFile; iIdx++)
					{
						     if( 0==strncmp( &infile_name[iIdx], "@.", 2) ) infile_name[iIdx] = '*';
						else if( 0==strncmp( &infile_name[iIdx], ".@", 2) ) infile_name[iIdx+1] = '*';
						else if( 0==strncmp( &infile_name[iIdx], "$.", 2) ) infile_name[iIdx] = '*';
						else if( 0==strncmp( &infile_name[iIdx], ".$", 2) ) infile_name[iIdx+1] = '*';
						else if( 0==strncmp( &infile_name[iIdx], "$", 1) ) infile_name[iIdx] = '*';
						else if( 0==strncmp( &infile_name[iIdx], "@", 1) ) infile_name[iIdx] = '*';
						else if( 0==strncmp( &infile_name[iIdx], "*.", 2) ) infile_name[iIdx] = '*';
						else if( 0==strncmp( &infile_name[iIdx], ".*", 2) ) infile_name[iIdx+1] = '*';
					}

					multifileindex = 1;
					memcpy( &extfile_name[0], infile_name, iLenFile );

					printf("\n>>Input files     : @.@ or $.$ ");

					/// opt_ok += CHECK_BIT_IN;
				}
				else if( ASTERISK_FOUND==isAsteMode )
				{
					// no action!!
					// case ah.exe --input *.* ( case of 2 more files )
					//printf(" -i or --input *.* isAsteMode(%d) \n", isAsteMode);
					printf("\n>>Input files     : *.* ");
				}
				else
			#endif /// MD5_MULTI_INPUT_FILES
				{
					if( (retFiles = _findfirsti64( infile_name, &iFiles )) == -1L )
					{
						printf("\n\nNo input file [%s] \n", infile_name );
						_findclose( retFiles );

						beep(700,100);
						AllFilesClosed();

						exit(0);
						return 0;
					}
					_findclose( retFiles );

					infile_size = iFiles.size; /// file size

				#if 0
					if( iFiles.size>>20 )
						printf("\n>>Input file   : %s (%.3f MB)", infile_name, (iFiles.size/1024.0)/1024.0 );
					else if( iFiles.size>>10 )
						printf("\n>>Input file   : %s (%.3f kB)", infile_name, (iFiles.size/1024.0) );
					else 
				#endif
						printf("\n>>Input file      : %s (%lu Bytes)", infile_name, iFiles.size );


				#if 1 // 2017.04.05, 2nd file information
					if( 3==isJoin ) // --join option
					{
						if( (retFiles = _findfirsti64( sefile_name, &iFiles )) == -1L )
						{
							printf("\r\nNo 2nd file [%s]. Check it! \n", sefile_name ); // 2nd input file check
							_findclose( retFiles );
						
							beep(700,100);
							AllFilesClosed();

							exit(0);

							return 0;						
						}
						_findclose( retFiles );
						
						sefile_size = iFiles.size; /// file size

						printf("\n>>2nd input file  : %s (%lu Bytes)", sefile_name, sefile_size );

					}
				#endif

					opt_ok += CHECK_BIT_IN;

					if( strlen(optarg) >= (MAX_CHARS*LENGTH_OF_FILENAME) )
					{
						printf("\n\n[++ERROR++] Input file name length is too long (%d Chars).. Max:%d Bytes\n\n", strlen(optarg), (MAX_CHARS*LENGTH_OF_FILENAME) );

						beep(700,100);
						AllFilesClosed();

						exit(0);

						return 0;
					}
				}

				break;
		

			case 'a': /* output file name : 32 characters -- append mode */
				isAppend = 1; /// File Append--

		    case 'o': /* 32 characters  -- write mode */
				olen = 0;

				if(optarg) 
				{
					memcpy(outfile_name, optarg, MAX_CHARS*LENGTH_OF_FILENAME);
					olen = strlen(outfile_name);

					printf("\n>>Output file     : %s (%s)", outfile_name, (isAppend==1)? "append":"new create");
					opt_ok += CHECK_BIT_OUT;

					if( strlen(optarg) >= (MAX_CHARS*LENGTH_OF_FILENAME) )
					{
						printf("\n\n[++ERROR++] Output file name length is too long (%d Chars).. Max:%d Bytes \n\n", strlen(optarg), (MAX_CHARS*LENGTH_OF_FILENAME) );

						beep(700,100);
						AllFilesClosed();
						
						exit(0);
						return 0;
					}
				}
				else
				{
					printf("\n\n[++ERROR++] Output file is NULL.  check option --output|--append [filename]. \n\n" );

					beep(700,100);
					AllFilesClosed();

					exit(0);
					return 0;
				}				
				break;
		

		    case 'z': /// printf, verbos ---

				printf("\n");
				memcpy(str_Verbos, optarg, MAX_CHARS);
				olen = strlen(str_Verbos);

				if( 0==strcasecmp( str_Verbos, "date" ) ) // inserted date
				{
					iVerbosType = 1;
			    	verbose=TRUE;
				}
				else if( 0==strcasecmp( str_Verbos, "size" ) ) // inserted date
				{
					iVerbosType = 2;
			    	verbose=TRUE;
				}
				else if( 0==strcasecmp( str_Verbos, "datesize" ) || 0==strcasecmp( str_Verbos, "sizedate" )) // inserted date
				{
					iVerbosType = 3;
			    	verbose=TRUE;
				}
				else
				{
					ret_scan = sscanf(str_Verbos,"%u",&iVerbosType);
					if( 1==ret_scan )
					{
			    		verbose=TRUE; // OK
					}
					else
					{
						printf("\n\n[++ERROR++] str_Verbos parser is wrong [%s]-(%d) \n\n", str_Verbos, iVerbosType );
						printf("  --verbose date or size or datesize or 1|2|3|4 \n");

						beep(700,100);
						AllFilesClosed(); // 2020.07.10
						exit(0); /// 2017.11.21

						return 0;
					}

				}
		    	break;

			default:
				//printf("\n\nWARNING : Unknwon option [%c]. Check options.... \r\n", opt);
				printf("\n\n");
				beep(700,100);

				AllFilesClosed();

				exit(0);
				return 0;
				break;
		}


#if 0 //////////////////////
		if( 'i' == opt )
		for (temp_index = optind; temp_index < argc; temp_index++)
			printf ("opt(%c;%d)argc(%d) => argv(%d)[%s] : %d, %c \n", opt,opt, argc, temp_index, argv[temp_index], optind, optopt);
#endif //////////////////////

	}



	g_iUpper = iUpper; // uppercase for crc16/crc32/crc64/crc16c or CRC16/CRC32/CRC64/CRC16C

	//printf("OPTIND  ============ %d \n", optind);


	if( argc < 2 )
	{
		char ch;
		
		help();
		ch = getch();
		if( ch=='q' || ch=='Q' || ch==27) ClearScreen();

		beep(700,100);
		AllFilesClosed();

		exit(0);
		return 0;
	}


	
	/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
	/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
	/* ========== INPUT FILE ================= */

#if MD5_MULTI_INPUT_FILES
	if(1==isIgnoreBothFile)
	{
		// input file and output files are ignored
		if(verbose==TRUE) LOG_ERR("\n\n[INPUT] Input & output files are ignored.");
	}
	else if(1==isIgnoreInFile)
	{
		// input file is ignored
		if(verbose==TRUE) LOG_ERR("\n\n[INPUT] Input file is ignored.");
	}	
	else if( multifileindex > 0 || (ASTERISK_FOUND==isAsteMode) || (1==isIgnoreBothFile && 1==isFloat2Hex) ) /// input files---
	{
		/// MD5 / SHA1 Checksum 에서만 사용한다...
		/// printf("There are input file...\n");
		/// if(verbose==TRUE) LOG_ERR("\n\n[INPUT] Input files are multi.. %d/ %d/ %d/ %d/ %d", multifileindex, isAsteMode, isIgnoreBothFile, isIgnoreInFile, isIgnoreOuFile );
	}
	else
#endif /// MD5_MULTI_INPUT_FILES
	{

	#if 0
		if( (0x00==infile_name[0]) )  
		{ 
			isIgnoreBothFile =1; 
			isIgnoreInFile = 1; 
		}
		
		if( (0x00==outfile_name[0]) ) 
		{ 
			//printf("\n\n[++ERROR++] output file file [%s]. \n", outfile_name);
			isIgnoreBothFile =1; 
			isIgnoreOuFile = 1; 
		}
	#endif

		/* ========== INPUT FILE ================= */
		if( NULL == (inpfile = fopen( infile_name, "rb")) ) 
		{
			beep(700,100);
			printf("\n\n[++ERROR++] Can not open input file[%s]. \n",infile_name);

			AllFilesClosed();

			exit(0); /// help();
			return 0;
		}
		else
		{
			/// OK ---
			if (fstat(fileno(inpfile), &file_statbuf) < 0) 
			{ 
				printf("\n\n[++ERROR++]Cannot stat [%s]\n", infile_name ); 

				AllFilesClosed(); // 2020.07.10
				exit(0);
				return 2; 
			} 	 
		}
		/* ========== INPUT FILE ================= */
	}



	/* ========== OUTPUT FILE ================= */
	if( (1==isIgnoreBothFile && 1==isFloat2Hex) )
	{
		// 2020.06.23, ignore input file and output file
		if(verbose==TRUE) LOG_ERR("\n\n[OUTPUT] Output files are multi.. %d/ %d/ %d/ %d", multifileindex, isIgnoreBothFile, isIgnoreInFile, isIgnoreOuFile );
	}
	else if(1==isIgnoreBothFile)
	{
		if(verbose==TRUE) LOG_ERR("\n\n[OUTPUT] isIgnoreBothFile is TRUE ");
		// input and output BOTH files are ignored
	}
	else if(1==isIgnoreOuFile)
	{
		if(verbose==TRUE) LOG_ERR("\n\n[OUTPUT] isIgnoreOuFile is TRUE ");
		// output file is ignored
	}
	else if(1==isAppend)
	{
		/// CHECKING input = output
		if( 0==strcasecmp(infile_name, outfile_name) )
		{
			printf("\n\n[++ERROR++] DO check input[%s] and output[%s]. filenames are same!!! \n",infile_name, outfile_name );

			beep(700,100);
			AllFilesClosed();

			exit(0); /// help();
			return 0;
		} 
		else if( (outfile = fopen( outfile_name, "ab")) == NULL )  /// append
		{
			int i=0, iLen, imkFolder=0;
			int nResult;
			char szDir[255] = {0,};

			if(outfile) { fclose(outfile); outfile=NULL; }

			// ~~if(inpfile) fclose(inpfile);
			// ~~if(data_buf) free(data_buf);

			
			iLen = strlen(outfile_name);
			memset( szDir, 0x00, sizeof(szDir) );
			for(i=0; i<iLen; i++)
			{
				szDir[i] = outfile_name[i];
				if( '\\' == outfile_name[i]  )
				{
					szDir[i+1] = 0x00;
			
					nResult = mkdir( szDir );
					if( nResult == 0 )
					{
						imkFolder = 1;
						// OK
					}
					else if( nResult == -1 )
					{
						imkFolder = 0;
						// mkdir ERROR
						if(outfile) { fclose(outfile);	outfile=NULL; }
					
						printf("\n\nCan not create folder [%s] - (append mode) \n", szDir);
					}
				}
			}
			
			if( 1==imkFolder )
			{
				// mkdir OK
				if( NULL == (outfile = fopen( outfile_name, "wb"))	)	
				{
					// FAIL
					printf("\n\nCan not create folder & output file. [%s / %s] - (append mode) \n", szDir, outfile_name);
					if( NULL == outfile_name || outfile_name[0] == 0x00 )
						printf("Need an output file option (--output or --append) for saving output.\n" );
			
					isIgnoreBothFile = 1; 
					isIgnoreOuFile = 1; 

				}
			}
			else
			{
				printf("\n\nCan not create output file. [%s] - (append mode) \n", outfile_name);
				if( NULL == outfile_name || outfile_name[0] == 0x00 )
					printf("Need an output file option (--output or --append) for saving output. \n" );

				isIgnoreBothFile = 1; 
				isIgnoreOuFile = 1; 

			}
			
			//~~~~ exit(0); /// help();
			//~~~~ return 0;

		}	
		else
		{
			/// OK---
			/// if(verbose==TRUE) LOG_ERR("\n\n[OUTPUT-APPEND] ELSE...  " );
		}
		
	}
	else
	{
		
		/// --- CHECKING input = output ----
		if( 1==isIgnoreBothFile || 1==isIgnoreOuFile )
		{
			//
			if(verbose==TRUE) LOG_ERR("\n\n[OUTPUT] isIgnoreBothFile(%d) or isIgnoreOuFile(%d) is TRUE ", isIgnoreBothFile, isIgnoreOuFile);
		}
		else if( 0==strcasecmp(infile_name, outfile_name) )
		{
			beep(700,100);
			printf("\n\n[++ERROR++] DO check input[%s] and output[%s]. Filenames are same!! \n",infile_name, outfile_name );

			AllFilesClosed();

			exit(0); /// help();
			return 0;
		} 
		else if( NULL == (outfile = fopen( outfile_name, "wb"))  ) /// write
		{
			int i=0, iLen, imkFolder=0;
			int nResult;
			char szDir[255] = {0,};
						

			if(outfile) { fclose(outfile);  outfile=NULL; }

			//~~ if(inpfile) fclose(inpfile);
			//~~ if(data_buf) free(data_buf);

			iLen = strlen(outfile_name);
			memset( szDir, 0x00, sizeof(szDir) );
			for(i=0; i<iLen; i++)
			{
				szDir[i] = outfile_name[i];
				if( '\\' == outfile_name[i]  )
				{
					szDir[i+1] = 0x00;

					nResult = mkdir( szDir );
					if( nResult == 0 )
					{
						imkFolder = 1;
						// OK
					}
					else if( nResult == -1 )
					{
						imkFolder = 0;
						// mkdir ERROR
						if(outfile) { fclose(outfile);	outfile=NULL; }
					
						printf("\n\nCan not create folder [%s] \n", szDir);
					}
				}
			}

			if( 1==imkFolder )
			{
				// mkdir OK
				if( NULL == (outfile = fopen( outfile_name, "wb"))  )	
				{
					// FAIL
					printf("\n\nCan not create folder & output file. [%s / %s] - (write mode) \n", szDir, outfile_name);
					if( NULL == outfile_name || outfile_name[0] == 0x00 )
						printf("Need an output file option (--output or --append) for saving output.\n" );

				}
			}
			else
			{
				printf("\n\nCan not create output file. [%s] - (write mode) \n", outfile_name);
				if( NULL == outfile_name || outfile_name[0] == 0x00 )
					printf("Need an output file option (--output or --append) for saving output.\n" );
			}
			//isIgnoreBothFile = 1; 
			//isIgnoreOuFile = 1; 

			//~~~~ exit(0); /// help();
			//~~~~ return 0;
		}
		else
		{
			//if(verbose==TRUE) LOG_ERR("\n\n[OUTPUT-WRITE] ELSE...  " );
			/// OK---
		}
	}


	#if 0
	if( 1==isAppend )
		printf("\nWaiting... Output file : append \n");
	else
		printf("\nWaiting... Output file : new create \n");
	#endif
		
		/* ========== OUTPUT FILE ================= */
	

	if( (0==isHash) && (1==isIgnoreOuFile) )
	{

		beep(700,100);
		AllFilesClosed();

		printf("\n\n[++ERROR++]Can not create output file[%s]!!! please check!!! \n\n", outfile_name);

		exit(0); /// help();
		return 0;	
	}


	// --------------------------------------------------------------------------------
	// --------------------------------------------------------------------------------
	// --------------------------------------------------------------------------------
	// --------------------------------------------------------------------------------
	// --------------------------------------------------------------------------------
	// --------------------------------------------------------------------------------



	/* 1. Header (Version) 붙이기 */
	if(    (0 == isDelHdr) 
		&& ( (ATT_VERSION|ATT_DATEorCRC|ATT_MODEL|ATT_BOARD)== isAttach ) /// 2014.07.25
		&& (0 == isIgnoreBothFile) /// 2014.07.14
		&& (0 == isCRC) 
		&& (0 == isFillSize) 
		&& (0 == isNKinform) 
		&& (0 == isMot2bin) 
		&& (0 == isIntel2bin) 
		&& (0 == isElf2Bin)
		&& (0 == isFloat2Hex) 
		&& (0 == isBMPreverse) /// 2015.02.12
		&& (0 == isFileMerge) // 2017.04.05
#if CONVERT_BMP2C
		&& (0 == isConvertBMP2C) /// 2013.07.09
#endif /// CONVERT_BMP2C

#if MD2_CHECKSUM_ENCIPHER 
		&& (0 == isMD2) /// 2014.07.29
#endif /// MD5_CHECKSUM_ENCIPHER

#if MD4_CHECKSUM_ENCIPHER 
		&& (0 == isMD4) /// 2014.07.26
#endif /// MD5_CHECKSUM_ENCIPHER

#if MD5_CHECKSUM_ENCIPHER 
		&& (0 == isMD5) /// 2014.06.26
#endif /// MD5_CHECKSUM_ENCIPHER

#if POLARSSL_SHA1_ENCIPHER
		&& (0 == isSHA1) /// 2014.06.29
#endif /// POLARSSL_SHA1_ENCIPHER

#if SHA2_256_384_512
		&& (0 == isSHA256) /// 2014.06.30
		&& (0 == isSHA384) /// 2014.06.30
		&& (0 == isSHA512) /// 2014.06.30
#endif /// SHA2_256_384_512

#if MD6_CHECKSUM_ENCIPHER
		&& (0 == isMD6 ) /// 2014.07.31
#endif

#if SHA2_224_CHECKSUM
		&& (0 == isSHA224 ) /// 2014.07.31
#endif

#if SHA3_KECCAK_224_256_384_512 // 2020.06.11
		&& (0 == isSHA3_KECCAK_224)
		&& (0 == isSHA3_KECCAK_256)
		&& (0 == isSHA3_KECCAK_384)
		&& (0 == isSHA3_KECCAK_512) 
		&& (0 == isShake128)
		&& (0 == isShake256)
#endif

#if defined(RIPEMD160) || defined(RIPEMD128)	
		&& (0==isRipeMD128)
		&& (0==isRipeMD160)
#endif

#if BLAKE_224_256_384_512_HASH
		&& (0 == isBlake224)
		&& (0 == isBlake256)
		&& (0 == isBlake384)
		&& (0 == isBlake512)
#endif // BLAKE_224_256_384_512_HASH

#if IDEA_ENCIPHER_ALGORITHM 
		&& (0 == isIDEA) /// 2014.07.26
#endif

		&& (0 == isRandomNum) // 2020.0731

#if MODIFIED_JULIAN_DATE
		&& (0 == isMJD) /// 2014.07.04
#endif /// MODIFIED_JULIAN_DATE
	   

	   )
	{
		len_attach_hdr = 0; // 2020.07.07, Total Header Length


		if( 0x00 == str_boardName[0] )
	        printf("\n>>Board Name (default)  : %s", DUMMY_FILL);
		if( 0x00 == str_moduleName[0] )
	        printf("\n>>Model Name (default)  : %s", DUMMY_FILL);


		/* ++++++++ BOARD NAME +++++++++++++++++++++++++++ */
		/* step1 : board name (16 characters) : ex) PI2000 */
		/* +++++++++++++++++++++++++++++++++++++++++++++++ */
		len_board_name = strlen(str_boardName);
		if( len_board_name < MAX_CHARS )
		{
			if( len_board_name == 0 ) 
			{ 
				memcpy(str_boardName, DUMMY_FILL, DUMMY_FILL_LEN); 
				len_board_name=DUMMY_FILL_LEN; 
			}
			if(outfile) fprintf(outfile,"%s", str_boardName);
			while( len_board_name < MAX_CHARS )
			{
				if(outfile) fprintf(outfile,"%c",SPACE_FILL1);
				len_board_name++;
			}
		}
		else
		{
			count=0;
			while( count < MAX_CHARS )
			{
				if(outfile) fprintf(outfile,"%c",str_boardName[count] );
				count++;
			}
		}
		len_attach_hdr += MAX_CHARS; // for Board Name
		
		/* ++++++++ MODEL NAME +++++++++++++++++++++++++++ */
		/* step2 : Model name (16 characters) */
		/* +++++++++++++++++++++++++++++++++++++++++++++++ */
		len_module_name = strlen(str_moduleName);
		if( len_module_name < MAX_CHARS )
		{
			if( len_module_name == 0 ) 
			{ 
				memcpy(str_moduleName, DUMMY_FILL, DUMMY_FILL_LEN); 
				len_module_name=DUMMY_FILL_LEN; 
			}
			if(outfile) fprintf(outfile,"%s", str_moduleName);
			while( len_module_name < MAX_CHARS )
			{
				if(outfile) fprintf(outfile,"%c",SPACE_FILL2);
				len_module_name++;
			}
		}
		else
		{
			count=0;
			while( count < MAX_CHARS )
			{
				if(outfile) fprintf(outfile,"%c",str_moduleName[count] );
				count++;
			}
		}
		len_attach_hdr += MAX_CHARS; // for Module Name


		// --------------------------------------------------
		/* ++++++++ VERSION NAME +++++++++++++++++++++++++++ */
		/* step4 : Version name (16 characters) -> 
		   step3 : Version name 32 Bytes : 2017.11.21 */
		/* +++++++++++++++++++++++++++++++++++++++++++++++ */
		len_version_name = strlen(str_versionName);
		if( len_version_name < MAX_VERSION_LEN )
		{
			if( len_version_name == 0 ) 
			{ 
				memcpy(str_versionName, DUMMY_FILL, DUMMY_FILL_LEN); 
				len_version_name = DUMMY_FILL_LEN; 
			}
			if(outfile) fprintf(outfile,"%s", str_versionName);
			while( len_version_name < MAX_VERSION_LEN )
			{
				if(outfile) fprintf(outfile,"%c",SPACE_FILL4);
				len_version_name++;
			}
		}
		else
		{
			count=0;
			while( count < MAX_VERSION_LEN )
			{
				if(outfile) fprintf(outfile,"%c",str_versionName[count] );
				count++;
			}
		}

		len_attach_hdr += MAX_VERSION_LEN; // for Version Name
	


		/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
		/* HEADER INSERTION                  +++++++++++++++++++++++++ */
		/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
		if( (1 == insertCRC) )
		{
			unsigned __int64  kll = 0UL;
		
			LOG_V("\n");
			if( HDR_CRC16 == isCRCtype )
			{
				kll = RunCRC16(infile_name, NULL, 0, 0, insertCRC, iVerbosType, str_hash);

				if( iUpper ) // 2017.04.27
					fprintf(outfile,"%04X", g_calcCrc16);
				else
					fprintf(outfile,"%04x", g_calcCrc16);

				len_build_date = 4;
				while( len_build_date < MAX_CRC_LEN_CODE )
				{
					fprintf(outfile,"%c",SPACE_FILL3);
					len_build_date++;
				}

				len_attach_hdr += MAX_CRC_LEN_CODE; // 2020.07.07, for CRC16 Name

			} 	
			else if( HDR_KSC_CRC16 == isCRCtype )
			{
				kll = RunKSC_CRC16(infile_name, NULL, 0, 0, insertCRC, iVerbosType, str_hash);

				if( iUpper ) // 2017.04.27
					fprintf(outfile,"%04X", g_calcCrc16);
				else
					fprintf(outfile,"%04x", g_calcCrc16);

				len_build_date = 4;
				while( len_build_date < MAX_CRC_LEN_CODE )
				{
					fprintf(outfile,"%c",SPACE_FILL3);
					len_build_date++;
				}

				len_attach_hdr += MAX_CRC_LEN_CODE; // 2020.07.07, for CRC16 Name

			} 	
			else if( HDR_CRC16CCITT == isCRCtype )
			{
 				kll = RunCRC16CCITT(infile_name, NULL, 0, 0, insertCRC, iVerbosType, str_hash);

				if( iUpper ) // 2017.04.27
					fprintf(outfile,"%04X", g_calcCrc16);
				else
					fprintf(outfile,"%04x", g_calcCrc16);

				len_build_date = 4;
				while( len_build_date < MAX_CRC_LEN_CODE )
				{
					fprintf(outfile,"%c",SPACE_FILL3);
					len_build_date++;
				}

				len_attach_hdr += MAX_CRC_LEN_CODE; // 2020.07.07, for CRC16 Name

			}
			else if( HDR_CRC32 == isCRCtype  )
			{
 				kll = RunCRC32(infile_name, NULL, 0, 0, insertCRC, iVerbosType, str_hash);

				if( iUpper ) // 2017.04.27
					fprintf(outfile,"%08X", g_calcCrc32);
				else
					fprintf(outfile,"%08x", g_calcCrc32);

				len_build_date = 8;
				while( len_build_date < MAX_CRC_LEN_CODE )
				{
					fprintf(outfile,"%c",SPACE_FILL3);
					len_build_date++;
				}

				len_attach_hdr += MAX_CRC_LEN_CODE; // 2020.07.07, for CRC32 Name

			}
 			else if( HDR_CRC64 == isCRCtype  )
			{
				kll = RunCRC64(infile_name, NULL, 0, 0, insertCRC, iVerbosType, str_hash);

				if( iUpper ) // 2017.04.27
					fprintf(outfile,"%016llX", g_calcCrc64);
				else
					fprintf(outfile,"%016llx", g_calcCrc64);

				len_build_date = 16;
				while( len_build_date < MAX_CRC_LEN_CODE )
				{
					fprintf(outfile,"%c",SPACE_FILL3);
					len_build_date++;
				}

				len_attach_hdr += MAX_CRC_LEN_CODE; // 2020.07.07, for CRC64 Name

			}
			else if( HDR_CRC64_ISC==isCRCtype )
			{
				kll = RunCRC64_isc(infile_name, NULL, 0, 0, insertCRC, iVerbosType, str_hash);

				if( iUpper ) // 2020.07.16
					fprintf(outfile,"%016llX", g_calcCrc64);
				else
					fprintf(outfile,"%016llx", g_calcCrc64);

				len_build_date = 16;
				while( len_build_date < MAX_CRC_LEN_CODE )
				{
					fprintf(outfile,"%c",SPACE_FILL3);
					len_build_date++;
				}

				len_attach_hdr += MAX_CRC_LEN_CODE; // 2020.07.07, for CRC64 Name
			}
			else if( HDR_ADLER32 == isCRCtype  ) 
			{
 				kll = RunAdler32(infile_name, NULL, 0, 0, insertCRC, iVerbosType, str_hash);

				if( iUpper ) // 2017.04.27
					fprintf(outfile,"%08X", g_calcAdler32);
				else
					fprintf(outfile,"%08x", g_calcAdler32);

				len_build_date = 8;
				while( len_build_date < MAX_CRC_LEN_CODE )
				{
					fprintf(outfile,"%c",SPACE_FILL3);
					len_build_date++;
				}

				len_attach_hdr += MAX_CRC_LEN_CODE; // 2020.07.07, for CRC64 Name
			}
			else if( HDR_JOAAT == isCRCtype  ) 
			{
 				kll = RunJoaat(infile_name, NULL, 0, 0, insertCRC, iVerbosType, str_hash);

				if( iUpper )
					fprintf(outfile,"%08X", g_calcJoaat);
				else
					fprintf(outfile,"%08x", g_calcJoaat);

				len_build_date = 8;
				while( len_build_date < MAX_CRC_LEN_CODE )
				{
					fprintf(outfile,"%c", SPACE_FILL3);
					len_build_date++;
				}

				len_attach_hdr += MAX_CRC_LEN_CODE;
			}

		#if 1 // 2017.11.21
			else if(HDR_SHA1 == isCRCtype )
			{
				ShaBuffer SHA1output;
				size_t nBytes;
				sha1_context ctx;
				unsigned char sha1_buf[SHA_READ_BUFSIZ]; /// NERVER modified!!!!
				unsigned __int64 	kll=0UL, ll=0UL;
				//unsigned char *sha1_buf;

				//printf("SHA1>> SHA1 hashing... \n");

				//sha1_buf = (unsigned char*)malloc( SHA_READ_BUFSIZ*sizeof(unsigned char) );

				memset( &ctx, 0x00, sizeof(sha1_context) );
				memset( sha1_buf, 0x00, SHA_READ_BUFSIZ );
	
				/// SHA1 initial ----
				sha1_starts(&ctx);
	
				kll = 0UL;
				while((nBytes = fread(sha1_buf, 1, sizeof(sha1_buf), inpfile)) > 0)
				{
					kll += nBytes;
					sha1_update(&ctx, sha1_buf, (int)nBytes);
				}
				
				sha1_finish(&ctx, SHA1output);
	
				if(outfile) fprintf(outfile, "%s", SHA1output);
				printf("%s  *%s*%s__(%llu) \r\n", SHA1output, str_hash, infile_name, infile_size );

				len_attach_hdr += SHA_HASH_LENGTH; // 2020.07.07, for SHA1 Hash 

				//free(sha1_buf);
				
				//printf("\nSHA1>> Calculated SHA1 Hash Value - OK");
			}
			else if(HDR_SHA224 == isCRCtype )
			{
				unsigned __int64 	kll=0UL, ll=0UL;
				sha224_ctx		ctx224; 	
				unsigned char	sha224_buf[SHA_READ_BUFSIZ];
				int i;
			
			
				// printf("SHA2>> SHA-224 hashing... \n");
		
				/// initial ----
				memset( &ctx224, 0x00, sizeof(sha224_ctx) );
				memset( sha224_buf, 0x00, sizeof(sha224_buf) );
	
	
				/// SHA2 Calculate ----
				sha224_init(&ctx224);
						
				kll = 0UL;
				while((ll = fread(sha224_buf, 1, sizeof(sha224_buf), inpfile)) > 0) 
				{
					kll += ll;
					sha224_update(&ctx224, sha224_buf, ll);
				}
				
				sha224_final(&ctx224, sha224_buf);
	
				//sha224Print (sha224_buf);

				if(iUpper) // upper-case
				{
					for (i = 0; i < SHA224_DIGEST_SIZE; i++)
					{
						if(outfile) fprintf(outfile, "%02X", sha224_buf[i]);
						printf("%02X", sha224_buf[i]);
					}
				}
				else
				{
					for (i = 0; i < SHA224_DIGEST_SIZE; i++)
					{
						if(outfile) fprintf(outfile, "%02x", sha224_buf[i]);
						printf("%02x", sha224_buf[i]);
					}
				}
				
				printf("  *%s*%s__(%llu) \r\n", str_hash, infile_name, infile_size );
				//printf("  *SHA224*%s__(%llu) \r\n", infile_name, infile_size );

				//fprintf(outfile,"  *SHA224*%s__(%llu) \r\n", infile_name, infile_size );
				//fprintf(outfile,"%s", sha256_buf);

				len_attach_hdr += (SHA224_DIGEST_SIZE*2); // 2020.07.07, for SHA1 Hash 
			
			}
			else if(HDR_SHA256 == isCRCtype )
			{
				unsigned __int64 	kll=0, ll=0;
				SHA256_CTX 		ctx256;
				unsigned char	sha256_buf[SHA2_BUFLEN];

		
				//printf("SHA2>> SHA-256 hashing... \n");

				/// initial
				memset( &ctx256, 0x00, sizeof(SHA256_CTX) );
				memset( sha256_buf, 0x00, sizeof(sha256_buf) );

				/// SHA2-256 Calculate ----
				SHA256_Init(&ctx256);

				kll = 0UL;
				while((ll = fread(sha256_buf, 1, SHA2_BUFLEN, inpfile)) > 0) 
				{
					kll += ll;
					SHA256_Update(&ctx256, (unsigned char*)sha256_buf, ll);

					//printf("\bSHA-256 Hashing(%s) -> read : %.1f MB \r", infile_name, (float)kll/(1024.0*1024.0) );
				}

				SHA256_End(&ctx256, sha256_buf);

				if(outfile) fprintf(outfile,"%s", sha256_buf);

				printf("%s	*%s*%s__(%llu) \r\n", sha256_buf, str_hash, infile_name, infile_size );

				len_attach_hdr += (SHA256_DIGEST_SIZE*2); // 2020.07.07, for SHA1 Hash 

			}				
			else if(HDR_SHA384 == isCRCtype )
			{

				unsigned __int64 	kll=0, ll=0;
				SHA384_CTX 		ctx384;
				unsigned char	sha384_buf[SHA2_BUFLEN];

				//printf("SHA2>> SHA-384 hashing... \n");

				/// initial ---
				memset( &ctx384, 0x00, sizeof(SHA384_CTX) );
				memset( sha384_buf, 0x00, sizeof(sha384_buf) );

				/// SHA2-384 Calculate ----
				SHA384_Init(&ctx384);

				kll = 0UL;
				while((ll = fread(sha384_buf, 1, SHA2_BUFLEN, inpfile)) > 0) 
				{
					kll += ll;
					SHA384_Update(&ctx384, (unsigned char*)sha384_buf, ll);
				}

				SHA384_End(&ctx384, sha384_buf);

				if(outfile) fprintf(outfile,"%s", sha384_buf);

				printf("%s  *%s*%s__(%llu) \r\n", sha384_buf, str_hash, infile_name, infile_size );
				//fprintf(outfile,"%s  *SHA384*%s__(%llu) \r\n", sha384_buf, infile_name, infile_size );

				len_attach_hdr += (SHA384_DIGEST_SIZE*2); // 2020.07.07, for SHA1 Hash 

			}
			else if(HDR_SHA512 == isCRCtype )
			{
				unsigned __int64 	kll=0, ll=0;
				SHA512_CTX 		ctx512;
				unsigned char	sha512_buf[SHA2_BUFLEN];

				//printf("SHA2>> SHA2-512 hashing... \n");


				/// initial
				memset( &ctx512, 0x00, sizeof(SHA512_CTX) );
				memset( sha512_buf, 0x00, sizeof(sha512_buf) );


				/// SHA2-512 Calculate ----
				SHA512_Init(&ctx512);

				kll = 0UL;
				while((ll = fread(sha512_buf, 1, SHA2_BUFLEN, inpfile)) > 0) 
				{
					kll += ll;
					SHA512_Update(&ctx512, (unsigned char*)sha512_buf, ll);
				}

				SHA512_End(&ctx512, sha512_buf);

				if(outfile) fprintf(outfile,"%s", sha512_buf);

				printf("%s  *%s*%s__(%llu) \r\n", sha512_buf, str_hash, infile_name, infile_size );
				//fprintf(outfile,"%s  *SHA512*%s__(%llu) \r\n", sha512_buf, infile_name, infile_size );

				len_attach_hdr += (SHA512_DIGEST_SIZE*2); // 2020.07.07, for SHA1 Hash 

			}
			else if(HDR_SHA3_224 == isCRCtype )
			{
				char	sha3Buf[SHA3_BUFLEN] = {0,};
				char	sha3digestTxt[SHA3_OUTPUT_SIZ] = {0,};
				uint8_t sha3out[SHA3_OUTPUT_SIZ] = { 0, };
				
				unsigned __int64 	kll=0;
				size_t		ll=0;
				int 		ret;

				//printf("SHA3-KECCAK>> SHA3-224 hashing... \n");

				memset( sha3Buf, 0x00, sizeof(sha3Buf) );
				memset( sha3out, 0x00, sizeof(sha3out) );
				memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );

				// Initialize the SHA3-256 context
				sha3_init(SHA3_224_HASH_BIT, SHA3_SHAKE_NONE);	

				kll = 0UL;
				while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
				{
					kll += ll;
					sha3_update(sha3Buf, ll);
				}
				
				ret = sha3_final(sha3out, SHA3_OUT_224);
				
				for (ii = 0; ii < SHA3_OUT_224; ii++)
				{
					if( iUpper )
						sprintf(&sha3digestTxt[ii*2], "%02X", sha3out[ii]);
					else
						sprintf(&sha3digestTxt[ii*2], "%02x", sha3out[ii]);
				}

				if(outfile) fprintf(outfile,"%s", sha3digestTxt);
				printf("%s  *%s*%s__(%llu) \r\n", sha3digestTxt, str_hash, infile_name, infile_size   );

				len_attach_hdr += (SHA3_OUT_224*2); // 2020.07.07, for SHA1 Hash 
					
			}
			else if(HDR_SHA3_256 == isCRCtype )
			{
				char	sha3Buf[SHA3_BUFLEN] = {0,};
				char	sha3digestTxt[SHA3_OUTPUT_SIZ] = {0,};
				uint8_t sha3out[SHA3_OUTPUT_SIZ] = { 0, };
				
				unsigned __int64 	kll=0;
				size_t		ll=0;
				int 		ret;

				memset( sha3Buf, 0x00, sizeof(sha3Buf) );
				memset( sha3out, 0x00, sizeof(sha3out) );
				memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
				
				// Initialize the SHA3-256 context
				sha3_init(SHA3_256_HASH_BIT, SHA3_SHAKE_NONE);	

				kll = 0UL;
				while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
				{
					kll += ll;
					sha3_update(sha3Buf, ll);
				}
				
				ret = sha3_final(sha3out, SHA3_OUT_256);
				
				for (ii = 0; ii < SHA3_OUT_256; ii++)
				{
					if( iUpper )
						sprintf(&sha3digestTxt[ii*2], "%02X", sha3out[ii]);
					else
						sprintf(&sha3digestTxt[ii*2], "%02x", sha3out[ii]);
				}

				if(outfile) fprintf(outfile,"%s", sha3digestTxt);
				printf("%s  *%s*%s__(%llu) \r\n", sha3digestTxt, str_hash, infile_name, infile_size   );

				len_attach_hdr += (SHA3_OUT_256*2); // 2020.07.07, for SHA1 Hash 

			}
			else if(HDR_SHA3_384 == isCRCtype )
			{
				char	sha3Buf[SHA3_BUFLEN] = {0,};
				char	sha3digestTxt[SHA3_OUTPUT_SIZ] = {0,};
				uint8_t sha3out[SHA3_OUTPUT_SIZ] = { 0, };
				
				unsigned __int64 	kll=0;
				size_t		ll=0;
				int 		ret;

				memset( sha3Buf, 0x00, sizeof(sha3Buf) );
				memset( sha3out, 0x00, sizeof(sha3out) );
				memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
				
				// Initialize the SHA3-384 context
				sha3_init(SHA3_384_HASH_BIT, SHA3_SHAKE_NONE);	

				kll = 0UL;
				while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
				{
					kll += ll;
					sha3_update(sha3Buf, ll);
				}
				
				ret = sha3_final(sha3out, SHA3_OUT_384);
				
				for (ii = 0; ii < SHA3_OUT_384; ii++)
				{
					if( iUpper )
						sprintf(&sha3digestTxt[ii*2], "%02X", sha3out[ii]);
					else
						sprintf(&sha3digestTxt[ii*2], "%02x", sha3out[ii]);
				}
				
				if(outfile) fprintf(outfile,"%s", sha3digestTxt);
				printf("%s  *%s*%s__(%llu) \r\n", sha3digestTxt, str_hash, infile_name, infile_size   );

				len_attach_hdr += (SHA3_OUT_384*2); // 2020.07.07, for SHA1 Hash 

			}
			else if(HDR_SHA3_512 == isCRCtype )
			{
				char	sha3Buf[SHA3_BUFLEN] = {0,};
				char	sha3digestTxt[SHA3_OUTPUT_SIZ] = {0,};
				uint8_t sha3out[SHA3_OUTPUT_SIZ] = { 0, };
				
				unsigned __int64 	kll=0;
				size_t		ll=0;
				int 		ret;

				memset( sha3Buf, 0x00, sizeof(sha3Buf) );
				memset( sha3out, 0x00, sizeof(sha3out) );
				memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
				
				// Initialize the SHA3-512 context
				sha3_init(SHA3_512_HASH_BIT, SHA3_SHAKE_NONE);	

				kll = 0UL;
				while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
				{
					kll += ll;
					sha3_update(sha3Buf, ll);
				}
				
				ret = sha3_final(sha3out, SHA3_OUT_512);
				
				for (ii = 0; ii < SHA3_OUT_512; ii++)
				{
					if( iUpper )
						sprintf(&sha3digestTxt[ii*2], "%02X", sha3out[ii]);
					else
						sprintf(&sha3digestTxt[ii*2], "%02x", sha3out[ii]);
				}
				

				if(outfile) fprintf(outfile,"%s", sha3digestTxt);
				printf("%s  *%s*%s__(%llu) \r\n", sha3digestTxt, str_hash, infile_name, infile_size   );

				len_attach_hdr += (SHA3_OUT_512*2); // 2020.07.07, for SHA1 Hash 

			}
			else if( HDR_SHAKE128 == isCRCtype )
			{
				char	sha3Buf[SHA3_BUFLEN] = {0,};
				char	sha3digestTxt[SHA3_OUTPUT_SIZ] = {0,};
				uint8_t sha3out[SHA3_OUTPUT_SIZ] = { 0, };
				
				unsigned __int64 	kll=0;
				size_t		ll=0;
				int 		ret;

				memset( sha3Buf, 0x00, sizeof(sha3Buf) );
				memset( sha3out, 0x00, sizeof(sha3out) );
				memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
				
				// Initialize the SHAKE128 context
				sha3_init(SHAKE_128_HASH_BIT, SHA3_SHAKE_USE);	

				kll = 0UL;
				while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
				{
					kll += ll;
					sha3_update(sha3Buf, ll);
				}
				
				ret = sha3_final(sha3out, SHAKE_OUT_128);
				
				for (ii = 0; ii < SHAKE_OUT_128; ii++)
				{
					if( iUpper )
						sprintf(&sha3digestTxt[ii*2], "%02X", sha3out[ii]);
					else
						sprintf(&sha3digestTxt[ii*2], "%02x", sha3out[ii]);
				}
					

				if(outfile) fprintf(outfile,"%s", sha3digestTxt);
				printf("%s  *%s*%s__(%llu) \r\n", sha3digestTxt, str_hash, infile_name, infile_size   );

				len_attach_hdr += (SHAKE_OUT_128*2); // 2020.07.07, for SHA1 Hash 

			}
			else if( HDR_SHAKE256 == isCRCtype )
			{
				char	sha3Buf[SHA3_BUFLEN] = {0,};
				char	sha3digestTxt[SHA3_OUTPUT_SIZ] = {0,};
				uint8_t sha3out[SHA3_OUTPUT_SIZ] = { 0, };
				
				unsigned __int64 	kll=0;
				size_t		ll=0;
				int 		ret;
				
				memset( sha3Buf, 0x00, sizeof(sha3Buf) );
				memset( sha3out, 0x00, sizeof(sha3out) );
				memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
				
				// Initialize the SHAKE128 context
				sha3_init(SHAKE_256_HASH_BIT, SHA3_SHAKE_USE);	

				kll = 0UL;
				while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
				{
					kll += ll;
					sha3_update(sha3Buf, ll);
				}
				
				ret = sha3_final(sha3out, SHAKE_OUT_256);
				
				for (ii = 0; ii < SHAKE_OUT_256; ii++)
				{
					if( iUpper )
						sprintf(&sha3digestTxt[ii*2], "%02X", sha3out[ii]);
					else
						sprintf(&sha3digestTxt[ii*2], "%02x", sha3out[ii]);
				}
					

				if(outfile) fprintf(outfile,"%s", sha3digestTxt);
				printf("%s  *%s*%s__(%llu) \r\n", sha3digestTxt, str_hash, infile_name, infile_size   );

				len_attach_hdr += (SHAKE_OUT_256*2); // 2020.07.07, for SHA1 Hash 

			}
			else if(HDR_MD5 == isCRCtype )
			{
				/* Computes the message digest for a specified file.
				Prints out message digest, a space, the file name, and a carriage return.
				*/
				MD5_CTX mdContext;
				unsigned char md5_data[MD_HASH_BUFSIZ];
				unsigned __int64  nBytes = 0UL, kll = 0UL;

				//printf("MD5>> MD5 hashing... \n");
	
				/// initial
				memset( &mdContext, 0x00, sizeof(MD5_CTX) );
				memset( &md5_data, 0x00, sizeof(md5_data) );
	
	
				MD5Init (&mdContext);
	
				kll = 0UL;
				while ((nBytes = fread (md5_data, 1, sizeof(md5_data), inpfile)) != 0)
				{
					kll += nBytes;				
					MD5Update (&mdContext, md5_data, nBytes);
				}

				MD5Final(&mdContext);
				MDPrint(&mdContext);
	
				printf ("  *MD5*%s__(%llu) \r\n", infile_name, infile_size);
				//fprintf(outfile,"  *MD5*%s__(%llu) \r\n", infile_name, infile_size );

			#if 0
				len_build_date = 32;
				while( len_build_date < MAX_HASH_CODE )
				{
					fprintf(outfile,"%c",SPACE_FILL3);
					len_build_date++;
				}
			#endif

				len_attach_hdr += (16*2); // 2020.07.07, for SHA1 Hash 

			}
			else if(HDR_MD6 == isCRCtype )
			{

				uint64_t nBytes = 0UL, kll = 0UL;
				unsigned char md6_data[MD_HASH_BUFSIZ];
				double elapsed_time = end_time - start_time;
				uint64_t elapsed_ticks = end_ticks - start_ticks;


				/* -------------------------------------------------- */
				/* ------ MD6 : set default md6 parameter settings ------ */
				md6_dgtLen = 256;           /* digest length */
				md6_keylen = 0;             /* key length in bytes (at most 64) */
				md6_modPar = 64;            /* mode parameter */
				md6_roundN = md6_default_r(md6_dgtLen,md6_keylen);  /* number of rounds */
				md6_use_default_r = 1;     /* 1 if r should be set to the default, 0 if r is explicitly provided */
				/* -------------------------------------------------- */

		
				//printf("MD6>> MD6 hashing... \n");

				/// initial
				nBytes = 0UL;
				memset( md6_data, 0x00, sizeof(md6_data) );

				/// MD6 initial ----
				hash_init();

				kll = 0UL;
				while((nBytes = fread(md6_data, 1, sizeof(md6_data), inpfile)) > 0) 
				{
					kll += nBytes;
					hash_update(md6_data,nBytes*8);
				}

				hash_final();

				if(outfile) fprintf(outfile,"%s", md6_st.hexhashval);
				
				printf("%s  *MD6*%s__(%llu)  \n", md6_st.hexhashval, infile_name, infile_size  );
				//fprintf(outfile,"%s  *MD6*%s__(%llu) \r\n", md6_st.hexhashval, infile_name, infile_size );


			#if 0
				len_build_date = 64;
				while( len_build_date < MAX_HASH_CODE )
				{
					fprintf(outfile,"%c",SPACE_FILL3);
					len_build_date++;
				}
			#endif

				len_attach_hdr += (32*2); // 2020.07.07, for SHA1 Hash 

			}	
			else if(HDR_MD2 == isCRCtype )
			{
				MD2_CTX mdContext;
				size_t nBytes;
				unsigned char md2_data[MD_HASH_BUFSIZ];
				unsigned char md2_output[MD2_DIGEST_LENGTH] = {0,};
				
				//printf("MD2>> MD2 hashing... \n");
	
				/// initial
				memset( &mdContext, 0x00, sizeof(MD2_CTX) );
				memset( &md2_data, 0x00, sizeof(md2_data) );
				memset( &md2_output, 0x00, sizeof(md2_output) );
	
				MD2_Init(&mdContext);
	
				kll = 0UL;				
				while ((nBytes = fread (md2_data, 1, sizeof(md2_data), inpfile)) != 0)
				{
					kll += nBytes;
					MD2_Update(&mdContext, md2_data, nBytes);
	
					//printf("\bMD2 Hashing(%s) -> read : %.1f MB \r", infile_name, (float)(nBytes*(isize++))/(1024.0*1024.0) );
				}

				MD2_Final( md2_output, &mdContext );
				MD2Print( md2_output );
	
				printf ("  *MD2*%s__(%llu) \n", infile_name, infile_size );
	
				len_attach_hdr += (MD2_DIGEST_LENGTH*2); // 2020.07.15, for MD2 Hash 
	
			}
			else if(HDR_MD4 == isCRCtype )
			{
				MD4_CTX mdContext;
				int nBytes;
				unsigned char md4_data[MD_HASH_BUFSIZ];
				unsigned char md4_output[MD4_DIGEST_LENGTH] = {0,};
				
				//printf("MD4>> MD4 hashing... \n");
	
				/// initial
				memset( &mdContext, 0x00, sizeof(MD4_CTX) );
				memset( &md4_data, 0x00, sizeof(md4_data) );
				memset( &md4_output, 0x00, sizeof(md4_output) );
	
	
				MD4Init (&mdContext);
	
				kll = 0UL;
				while ((nBytes = fread (md4_data, 1, sizeof(md4_data) /*1024*/, inpfile)) != 0)
				{
					kll += nBytes;
					MD4Update (&mdContext, md4_data, nBytes);

					//printf("\bMD4 Hashing(%s) -> read : %.1f MB \r", infile_name, (float)(nBytes*(isize++))/(1024.0*1024.0) );
				}

				MD4Final(md4_output, &mdContext);
				MD4Print(md4_output);
	
				printf ("  *MD4*%s__(%llu) \r\n", infile_name, infile_size);

				len_attach_hdr += (MD4_DIGEST_LENGTH*2); // 2020.07.15, for MD4 Hash 

			}

		#if BLAKE_224_256_384_512_HASH
			else if( HDR_BLAKE224 == isCRCtype )
			{
				uint8_t inbuf[BLOCK224], out[BLAKE224_LEN]={0,}; 
				char outTxt[BLAKE224_LEN*2]={0,}; 
				state224 blakeS; 
				unsigned __int64 kll = 0UL;
				size_t		ll=0;
				
				memset( inbuf, 0x00, sizeof(inbuf) );
				memset( out, 0x00, sizeof(out) );
				
				//Initialize the BLAKE-224 context
				blake224_init( &blakeS ); 
				
				kll = 0ULL;
				while((ll = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
				{
					kll += ll;
					//Absorb input data
					blake224_update( &blakeS, inbuf, ll ); 				
				}
				
				//Finish absorbing phase
				blake224_final( &blakeS, out ); 
				
				
				memset( outTxt, 0x00, sizeof(outTxt) );
				for (ii = 0; ii < BLAKE224_LEN; ii++) 
				{
					if( iUpper )
						sprintf(&outTxt[ii*2], (char*)"%02X", out[ii]);
					else
						sprintf(&outTxt[ii*2], (char*)"%02x", out[ii]);
				}
				
				if(outfile) fprintf(outfile,"%s", outTxt);
				printf("%s  *%s*%s__(%llu) \r\n", outTxt, str_hash, infile_name, infile_size   );
			
				len_attach_hdr += (BLAKE224_LEN*2);

			}
			else if( HDR_BLAKE256 == isCRCtype )
			{
				uint8_t inbuf[BLOCK256], out[BLAKE256_LEN]={0,}; 
				char outTxt[BLAKE256_LEN*2]={0,}; 
				state256 blakeS;	
				unsigned __int64 kll = 0UL;
				size_t		ll=0;
				
				memset( inbuf, 0x00, sizeof(inbuf) );
				memset( out, 0x00, sizeof(out) );

				//Initialize the BLAKE-256 context
				blake256_init( &blakeS ); 
				
				kll = 0ULL;
				while((ll = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
				{
					kll += ll;
					//Absorb input data
					blake256_update( &blakeS, inbuf, ll ); 
				}
			
				//Finish absorbing phase
				blake256_final( &blakeS, out ); 
				
				memset( outTxt, 0x00, sizeof(outTxt) );
				for (ii = 0; ii < BLAKE256_LEN; ii++)
				{
					if( iUpper )
						sprintf(&outTxt[ii*2], (char*)"%02X", out[ii]);
					else
						sprintf(&outTxt[ii*2], (char*)"%02x", out[ii]);
				}

				if(outfile) fprintf(outfile,"%s", outTxt);
				printf("%s  *%s*%s__(%llu) \r\n", outTxt, str_hash, infile_name, infile_size   );
			
				len_attach_hdr += (BLAKE256_LEN*2);

			}
			else if( HDR_BLAKE384 == isCRCtype )
			{
				uint8_t inbuf[BLOCK384], out[BLAKE384_LEN] ={0,}; 
				char outTxt[BLAKE384_LEN*2]={0,}; 
				state384 blakeS;
				unsigned __int64 kll = 0UL;
				size_t		ll=0;
				
				memset( inbuf, 0x00, sizeof(inbuf) );
				memset( out, 0x00, sizeof(out) );
				
				//Initialize the BLAKE-384 context
				blake384_init( &blakeS ); 
				
				kll = 0ULL;
				while((ll = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
				{
					kll += ll;				
					//Absorb input data
					blake384_update( &blakeS, inbuf, ll ); 
				}
				
				//Finish absorbing phase
				blake384_final( &blakeS, out ); 

				memset( outTxt, 0x00, sizeof(outTxt) );
				for (ii = 0; ii < BLAKE384_LEN; ii++)
				{
					if( iUpper )
						sprintf(&outTxt[ii*2], (char*)"%02X", out[ii]);
					else
						sprintf(&outTxt[ii*2], (char*)"%02x", out[ii]);
				}

				if(outfile) fprintf(outfile,"%s", outTxt);
				printf("%s  *%s*%s__(%llu) \r\n", outTxt, str_hash, infile_name, infile_size   );
			
				len_attach_hdr += (BLAKE384_LEN*2);

			}
			else if( HDR_BLAKE512 == isCRCtype )
			{
				uint8_t inbuf[BLOCK512], out[BLAKE512_LEN]={0,}; 
				char outTxt[BLAKE512_LEN*2]={0,}; 
				state512 blakeS; 
				unsigned __int64 kll = 0UL;
				size_t		ll=0;
				
				memset( inbuf, 0x00, sizeof(inbuf) );
				memset( out, 0x00, sizeof(out) );
				
				//Initialize the BLAKE512 context
				blake512_init( &blakeS ); 
				
				kll = 0ULL;
				while((ll = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
				{
					kll += ll;				
					//Absorb input data
					blake512_update( &blakeS, inbuf, ll ); 
				}

				//Finish absorbing phase
				blake512_final( &blakeS, out ); 

				memset( outTxt, 0x00, sizeof(outTxt) );
				for (ii = 0; ii < BLAKE512_LEN; ii++)
				{
					if( iUpper )
						sprintf(&outTxt[ii*2], (char*)"%02X", out[ii]);
					else
						sprintf(&outTxt[ii*2], (char*)"%02x", out[ii]);
				}

				if(outfile) fprintf(outfile,"%s", outTxt);
				printf("%s  *%s*%s__(%llu) \r\n", outTxt, str_hash, infile_name, infile_size   );
			
				len_attach_hdr += (BLAKE512_LEN*2);

			}

		#endif

		#if defined(RIPEMD160) || defined(RIPEMD128)	
			else if(HDR_RMD128 == isCRCtype )
			{
#define RMD128_INBUF_SIZ 		(1024)  // NOT NOT NEVER modify~~~
#define RMD128_BUF_LEN 			5 // 160/32
	
				uint8_t inbuf[RMD128_INBUF_SIZ];
				char outTxt[RMD128_INBUF_SIZ*2]={0,}; 
				
				word32		  MDbuf[RMD128_BUF_LEN];   /* contains (A, B, C, D(, E))   */
				unsigned char hashcode[RMD128_DIGEST_SIZE]; /* for final hash-value		   */


				unsigned __int64	kll;
				size_t		nbytes; // ll;
				word32		  X[16];		  /* current 16-word chunk	   */
				unsigned int  i, j; 		  /* counters					   */
				word32		  length[2];	  /* length in bytes of message   */
				word32		  offset;		  /* # of unprocessed bytes at	  */
	
	
				memset( inbuf, 0x00, sizeof(inbuf) ); // 2018.06.15
					
				//Initialize the RIPEMD-160 context
				MD128init(MDbuf);
				length[0] = 0;
				length[1] = 0;
	
				kll = 0UL;
				while((nbytes = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
				{
					kll += nbytes;
			
					/* process all complete blocks */
					for (i=0; i<(nbytes>>6); i++) {
					   for (j=0; j<16; j++)
						  X[j] = BYTES_TO_DWORD(inbuf+64*i+4*j);
					   MD128compress(MDbuf, X);
					}
					/* update length[] */
					if (length[0] + nbytes < length[0])
					   length[1]++; 				 /* overflow to msb of length */
					length[0] += nbytes;
	
				}

				//Finish absorbing phase		
				/* finish: */
				offset = length[0] & 0x3C0;   /* extract bytes 6 to 10 inclusive */
				MD128finish(MDbuf, inbuf+offset, length[0], length[1]);
	
				for (ii=0; ii<RMD128_DIGEST_SIZE; ii+=4) 
				{
					hashcode[ii]   = (unsigned char)(MDbuf[ii>>2]);
					hashcode[ii+1] = (unsigned char)(MDbuf[ii>>2] >>  8);
					hashcode[ii+2] = (unsigned char)(MDbuf[ii>>2] >> 16);
					hashcode[ii+3] = (unsigned char)(MDbuf[ii>>2] >> 24);
				}
			
				memset( outTxt, 0x00, sizeof(outTxt) );
				for (ii = 0; ii < RMD128_DIGEST_SIZE; ii++)
				{
					sprintf(&outTxt[ii*2], (char*)"%02x", hashcode[ii]);
				}
				

				printf("%s  *RMD128*%s__(%llu) \r\n", outTxt, infile_name, infile_size );
				if(outfile) fprintf(outfile, "%s", outTxt);

				len_attach_hdr += (RMD128_DIGEST_SIZE*2);
	
			}
			else if(HDR_RMD160 == isCRCtype )
			{
						
#define RMD160_INBUF_SIZ 		(1024) // NOT NOT NEVER modify~~~
#define RMD160_BUF_LEN 			5 // 160/32


				uint8_t inbuf[RMD160_INBUF_SIZ];
				char outTxt[RMD160_INBUF_SIZ*2]={0,}; 
				
				word32		  MDbuf[RMD160_BUF_LEN];   /* contains (A, B, C, D(, E))   */
				unsigned char hashcode[RMD160_DIGEST_SIZE]; /* for final hash-value		   */
				
				unsigned __int64	kll;
				size_t		nbytes; // ll;
				word32		  X[16];		  /* current 16-word chunk	   */
				unsigned int  i, j; 		  /* counters					   */
				word32		  length[2];	  /* length in bytes of message   */
				word32		  offset;		  /* # of unprocessed bytes at	  */


				memset( inbuf, 0x00, sizeof(inbuf) ); // 2018.06.15
					
				//Initialize the RIPEMD-160 context
				MD160init(MDbuf);
				length[0] = 0;
				length[1] = 0;

				kll = 0UL;
				while((nbytes = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
				{
					kll += nbytes;
			
					/* process all complete blocks */
					for (i=0; i<(nbytes>>6); i++) {
					   for (j=0; j<16; j++)
						  X[j] = BYTES_TO_DWORD(inbuf+64*i+4*j);
					   MD160compress(MDbuf, X);
					}
					/* update length[] */
					if (length[0] + nbytes < length[0])
					   length[1]++; 				 /* overflow to msb of length */
					length[0] += nbytes;

				}

				//Finish absorbing phase		
				/* finish: */
				offset = length[0] & 0x3C0;   /* extract bytes 6 to 10 inclusive */
				MD160finish(MDbuf, inbuf+offset, length[0], length[1]);

				for (ii=0; ii<RMD160_DIGEST_SIZE; ii+=4) 
				{
					hashcode[ii]   = (unsigned char)(MDbuf[ii>>2]);
					hashcode[ii+1] = (unsigned char)(MDbuf[ii>>2] >>  8);
					hashcode[ii+2] = (unsigned char)(MDbuf[ii>>2] >> 16);
					hashcode[ii+3] = (unsigned char)(MDbuf[ii>>2] >> 24);
				}
			
				memset( outTxt, 0x00, sizeof(outTxt) );
				for (ii = 0; ii <RMD160_DIGEST_SIZE; ii++)
				{
					sprintf(&outTxt[ii*2], (char*)"%02x", hashcode[ii]);
				}
				
				printf("%s  *RMD160*%s__(%llu) \r\n", outTxt, infile_name, infile_size );
				if(outfile) fprintf(outfile, "%s", outTxt);
				
				len_attach_hdr += (RMD160_DIGEST_SIZE*2);

			}	
		#endif		

			else
			{
				printf("\n[++CRC ADD_HEADER ERROR++] Unknown isCRCtype = %d \n\n", isCRCtype);
			}
		#endif // 2017.11.21


		/////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////


			if(inpfile) { fclose(inpfile); inpfile=NULL; } // file close for checksum


			// ==========================================
			// ==========================================
			/* ========== INPUT FILE ================= */
			if( NULL == (inpfile = fopen( infile_name, "rb")) ) 
			{
				beep(700,100);
				printf("\n\n[++ERROR++] Can not open input file[%s]. \n",infile_name);
				if( NULL == infile_name || infile_name[0] == 0x00)
					printf("[++ERROR++] Must be input file with -i or --input option. \n" );
			
				AllFilesClosed();

				exit(0); /// help();
				return 0;
			}
			else
			{
				/// OK ---
				if (fstat(fileno(inpfile), &file_statbuf) < 0) 
				{ 
					printf("\n\n[++ERROR++]Cannot stat [%s]\n", infile_name ); 

					AllFilesClosed(); // 2020.07.10
					exit(0);
					return 2; 
				}	 
			}
			/* ========== INPUT FILE ================= */
			// ==========================================
			// ==========================================

		}
		else if( 0==strcasecmp( str_crcAdd, "date" ) ) // inserted date
		{
			// --- Insert date ---
			/* step3 : Compile Date (32 characters) */
			len_build_date = strlen(str_abuild_date);
			if( len_build_date < MAX_CHARS )
			{
				if( len_build_date == 0 ) 
				{ 
					len_build_date = sizeof("2017D04TUE123456");
					memcpy(str_abuild_date, "2017D04TUE123456", len_build_date);
				}

				if(outfile) fprintf(outfile,"%s", str_abuild_date);

				while( len_build_date < MAX_CHARS )
				{
					fprintf(outfile,"%c",SPACE_FILL3);
					len_build_date++;
				}
			}
			else
			{
				count=0;
				while( count < MAX_CHARS )
				{
					fprintf(outfile,"%c", str_abuild_date[count] );
					count++;
				}
			}

			len_attach_hdr += MAX_CHARS; // 2020.07.07, for date 

		}
		/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
		/* HEADER INSERTION */
		/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
		

		/// Attached Header --- 2014.07.15
		if(inpfile && outfile)
		{
			unsigned char read_buf[RD_BUF_SIZ] = {0,};

			//printf("\nHDR>> Attaching Header... ");
			memset( read_buf, 0x00, sizeof(read_buf) ); /// initial
			
			while( fr_size = fread(read_buf, sizeof(unsigned char), (RD_BUF_SIZ), inpfile ) )
			{
				fwrite(read_buf, sizeof(unsigned char), fr_size, outfile); 
				//printf("\rReading size = %d      ", fr_size);
				//waiting(count);
				//count++;
			}

			printf("\nHDR>> Attached Header!! (%dBytes) - OK", len_attach_hdr);
		}
		else
		{
			printf("\nHDR>> Can not open input file[%s] or output file[%s] \n", infile_name, outfile_name);
		}

		AllFilesClosed();

		//// -------------------------------------------------------------



	} /* 0 == isDelHdr && 0==isCRC */
	/* 2. Header 삭제하기 */
	else if( 1 == isDelHdr )
	{
		/* 16*4 byte 이동 */
		///printf("\n>>Moved Position 16*4 ");

		if( 0==iHdr_len ) 
			iHdr_len=16*4; // default

		if(inpfile) fseek(inpfile, iHdr_len, SEEK_SET);  /* -- (16 Bytes * 4 Lines) : SEEK_CUR, SEEK_END */


		/// 2014.07.15
		if(inpfile && outfile)
		{
			unsigned char read_buf[BUFSIZE];

			printf("\nHDR>> Deleting Header size: %d Bytes", iHdr_len);
			memset( read_buf, 0x00, sizeof(read_buf) ); /// initial
			while( fr_size = fread(read_buf, sizeof(unsigned char), BUFSIZE, inpfile ) )
			{
				fwrite(read_buf, sizeof(unsigned char), fr_size, outfile); 
				//printf("\rReading size = %d      ", fr_size);
				//waiting(count);
				//count++;
			}

			printf("\nHDR>> Deleted Header!! - OK");
		}
		else
		{
			printf("\nHDR>> Can not open output file[%s] or input file[%s] \n", outfile_name, infile_name );
		}

		AllFilesClosed();

		//// -------------------------------------------------------------

	}
	else if( 1 == isBMPreverse )
	{

#ifdef  __ING___

	#define WIDTH 		800
	#define HIEGHT 		480
	#define BMP_RESOL 	(WIDTH*HIEGHT)*4 /// 32bit
	
		unsigned char read_buf[BMP_RESOL+800*480];
		unsigned int idx=1;

		printf("ibmpIndex = %d \n", ibmpIndex);

		fr_size = fread(read_buf, sizeof(unsigned char), 54, inpfile );
		fwrite(read_buf, sizeof(unsigned char), fr_size, outfile); 

		if(inpfile && outfile)
		{
			memset( read_buf, 0xff, sizeof(read_buf) );
			for( idx=1; idx<HIEGHT; idx+=1 ) 
			{
				ibmpIndex=WIDTH*(HIEGHT-idx)*4;
				printf("ibmpIndex = %d, idx=%d \n", ibmpIndex, idx);
				fseek(inpfile, ibmpIndex, SEEK_SET);
				fr_size = fread(read_buf, sizeof(unsigned char), BMP_RESOL, inpfile );
				fwrite(read_buf, sizeof(unsigned char), fr_size, outfile); 
			}
			printf("\nBMP Reverse>> Convert BMP!!! - OK");

		}
		else
		{
			printf("\nBMP Reverse>> Can not convert bmp \n");
		}

		AllFilesClosed();

		//// -------------------------------------------------------------
#endif
	

	}
	
	/* 3. CRC 생성하기 */
	else if( 1 == isCRC )
	{
		unsigned int index = 0;
		unsigned int iloop = 0;
		int iLenSub=0;

	#if 1
		char cdate[30];
		time_t	sys_t;
		int ilen;
		struct	tm *pTime;

		pTime = current_time();

		time( &sys_t );
		
		memset( cdate, 0x00, sizeof(cdate) );
		strcpy( cdate, ctime( &sys_t ) );
		ilen = strlen(cdate);
		cdate[ilen-0] = '\0';
		cdate[ilen-1] = '\0';
		/// cdate = Sun Jun 29 12:09:12 2014		
	#endif
		
		LOG_V("\n");

	
	#if 1 /// multi-input files
		if( HDR_CRC16==isCRCtype )
		{
			printf("CRC16>> ");

			if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile ) /// 2014.08.05
			{
				fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
				fprintf(outfile, "CRC16 checksum is created at %s by %s \n", cdate, EmailText );
				fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			}
		}
		else if( HDR_KSC_CRC16==isCRCtype )
		{
			printf("KSC-CRC16>> ");

			if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile ) /// 2014.08.05
			{
				fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
				fprintf(outfile, "KSC-CRC16 checksum is created at %s by %s \n", cdate, EmailText );
				fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			}
		}
		else if( HDR_CRC16CCITT==isCRCtype )
		{
			printf("CRC16CCITT>> ");
			if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile ) /// 2014.08.05
			{
				fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
				fprintf(outfile, "CRC16CCITT checksum is created at %s by %s \n", cdate, EmailText );
				fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			}
		}
		else if( HDR_CRC32==isCRCtype )
		{
			printf("CRC32>> ");
			if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile ) /// 2014.08.05
			{
				fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
				fprintf(outfile, "CRC32 checksum is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
				fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			}
		}
		else if( HDR_CRC64==isCRCtype )
		{
			printf("CRC64>> ");
			if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile ) /// 2014.08.05
			{
				fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
				fprintf(outfile, "CRC64 checksum is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
				fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			}
		}
		else if( HDR_CRC64_ISC==isCRCtype )
		{
			printf("CRC64ISC>> ");
			if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile ) /// 2014.08.05
			{
				fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
				fprintf(outfile, "CRC64ISC checksum is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
				fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			}
		}
		else if( HDR_ADLER32==isCRCtype )
		{
			printf("ADLER32>> ");
			if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile ) /// 2014.08.05
			{
				fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
				fprintf(outfile, "ADLER32 checksum is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
				fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			}
		}
		else if( HDR_JOAAT==isCRCtype )
		{
			printf("JOAAT>> ");
			if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile )  
			{
				fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
				fprintf(outfile, "JOAAT checksum is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
				fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			}
		}
		else					
		{
			printf("CRC**>> ");
		}


		if( ASTERISK_FOUND == isAsteMode ) /* For ah.exe --input *.* */
		{
			int iIdx=0, iCount=0;
			unsigned __int64 	kll = 0UL;
			unsigned __int64 	totSize = 0UL;

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			printf("CRC>> Calculating for input files* \n");
			printf("------------------------------------------------------\r\n" );

			iTotSize = 0LL;
			totSize = 0LL;

			for(ii=iFirst; ii<=(argc-isAsteminus); ii++)
			{
				if( NULL != (inpfile = fopen( argv[ii], "rb")) ) 
				{
			
					if( HDR_CRC16==isCRCtype )
					{
						kll = RunCRC16(argv[ii], outfile_name, totSize, multifileindex, 0, iVerbosType, str_hash);
					}
					else if( HDR_KSC_CRC16==isCRCtype )
					{
						kll = RunKSC_CRC16(argv[ii], outfile_name, totSize, multifileindex, 0, iVerbosType, str_hash);
					}
					else if( HDR_CRC16CCITT==isCRCtype )
					{ 
						kll = RunCRC16CCITT(argv[ii], outfile_name, totSize, multifileindex, 0, iVerbosType, str_hash);	
					}
					else if( HDR_CRC32==isCRCtype )
					{
						kll = RunCRC32(argv[ii], outfile_name, totSize, multifileindex, 0, iVerbosType, str_hash);
					}
					else if( HDR_CRC64==isCRCtype )
					{
						kll = RunCRC64(argv[ii], outfile_name, totSize, multifileindex, 0, iVerbosType, str_hash);
					}
					else if( HDR_CRC64_ISC==isCRCtype )
					{
						kll = RunCRC64_isc(argv[ii], outfile_name, totSize, multifileindex, 0, iVerbosType, str_hash);
					}
					else if( HDR_ADLER32==isCRCtype )
					{
						kll = RunAdler32(argv[ii], outfile_name, totSize, multifileindex, 0, iVerbosType, str_hash);
					}
					else if( HDR_JOAAT==isCRCtype )
					{
						kll = RunJoaat(argv[ii], outfile_name, totSize, multifileindex, 0, iVerbosType, str_hash);
					}
					else
					{
						printf("CRC>> Check CRC unknown type (%d) .... \r\n", isCRCtype );
					}
	
					if(inpfile) { fclose(inpfile); inpfile=NULL; }
					if(data_buf){ free(data_buf); data_buf=NULL; }

					iTotSize += kll;
					multifileindex ++;
				}
			}
			printf("\nCRC>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			AllFilesClosed();
	
		}
		else if( multifileindex > 0 )
		{
			unsigned __int64  kll = 0UL;

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }


			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("CRC>>No file [%s] in current directory!!! [%s] \n", extfile_name, infile_name );
			}
			else
			{

				printf("CRC calculating for input files... \n");
				printf("------------------------------------------------------\r\n" );

				do {

					iLenSub = strlen(iFiles.name);

					if( iFiles.attrib & _A_SUBDIR )
					{

					}
					else
					{
						if( NULL == (inpfile = fopen( iFiles.name, "rb")) ) 
						{
							beep(700,100);
							printf("\n[CRC++ERROR++] Can not open multi-input file (%s). \n", iFiles.name );

							AllFilesClosed();

							exit(0);
							return 0;
						}
				
						if( HDR_CRC16==isCRCtype )
						{
							kll = RunCRC16(iFiles.name, outfile_name, iFiles.size, multifileindex, 0, iVerbosType, str_hash);
						}
						else if( HDR_KSC_CRC16==isCRCtype )
						{
							kll = RunKSC_CRC16(iFiles.name, outfile_name, iFiles.size, multifileindex, 0, iVerbosType, str_hash);
						}
						else if( HDR_CRC16CCITT==isCRCtype )
						{ 
							kll = RunCRC16CCITT(iFiles.name, outfile_name, iFiles.size, multifileindex, 0, iVerbosType, str_hash);	
						}
						else if( HDR_CRC32==isCRCtype )
						{
							kll = RunCRC32(iFiles.name, outfile_name, iFiles.size, multifileindex, 0, iVerbosType, str_hash);
						}
						else if( HDR_CRC64==isCRCtype )
						{
							kll = RunCRC64(iFiles.name, outfile_name, iFiles.size, multifileindex, 0, iVerbosType, str_hash);
						}
						else if( HDR_CRC64_ISC==isCRCtype )
						{
							kll = RunCRC64_isc(iFiles.name, outfile_name, iFiles.size, multifileindex, 0, iVerbosType, str_hash);
						}
						else if( HDR_ADLER32==isCRCtype )
						{
							kll = RunAdler32(iFiles.name, outfile_name, iFiles.size, multifileindex, 0, iVerbosType, str_hash);
						}
						else if( HDR_JOAAT==isCRCtype )
						{
							kll = RunJoaat(iFiles.name, outfile_name, iFiles.size, multifileindex, 0, iVerbosType, str_hash);
						}
						else
						{
							printf("CRC>> Check CRC unknown type .... \r\n" );
						}
		
						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }

						/* file length checking */
						if( iFiles.size != kll ) printf("CRC>> file length is wrong!! (%lld) (%ld)... \r\n", kll, iFiles.size );

						iTotSize += kll;
						multifileindex ++;
					}

				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );

				multifileindex--;

				printf("\nCRC>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			}

		}
		else
	#endif /// MULTI_INPUT_FILES
		{
			unsigned __int64 kll=0UL;

			if(data_buf){ free(data_buf); data_buf=NULL; }

			if( HDR_CRC16==isCRCtype )
			{ 
				kll = RunCRC16(infile_name, outfile_name, infile_size, 0, 0, iVerbosType, str_hash);
			}
			else if( HDR_KSC_CRC16==isCRCtype )
			{ 
				kll = RunKSC_CRC16(infile_name, outfile_name, infile_size, 0, 0, iVerbosType, str_hash);
			}
			else if( HDR_CRC16CCITT==isCRCtype )
			{ 
				kll = RunCRC16CCITT(infile_name, outfile_name, infile_size, 0, 0, iVerbosType, str_hash);
			}
			else if( HDR_CRC32==isCRCtype )
			{
				kll = RunCRC32(infile_name, outfile_name, infile_size, 0, 0, iVerbosType, str_hash);
			}
			else if( HDR_CRC64==isCRCtype )
			{
				kll = RunCRC64(infile_name, outfile_name, infile_size, 0, 0, iVerbosType, str_hash);
			}
			else if( HDR_CRC64_ISC==isCRCtype )
			{
				kll = RunCRC64_isc(infile_name, outfile_name, infile_size, 0, 0, iVerbosType, str_hash);
			}
			else if( HDR_ADLER32==isCRCtype )
			{
				kll = RunAdler32(infile_name, outfile_name, infile_size, 0, 0, iVerbosType, str_hash);
			}
			else if( HDR_JOAAT==isCRCtype )
			{
				kll = RunJoaat(infile_name, outfile_name, infile_size, 0, 0, iVerbosType, str_hash);
			}
			else if( HDR_CRC_UNKNOWN==isCRCtype )
			{
				printf("CRC>> Check CRC type : [%d]. \n", isCRCtype );
			}
			else
			{
				printf("CRC>> Check CRC unknown type : [%d]... \n", isCRCtype );
			}
		}

		AllFilesClosed();

		opt_ok = 0; /* 2014.07.16 : NOTE !!! clear!!!  */

	}
	else if( 1 == isFillSize )
	{

		unsigned int index = 0;
		int ii;

		printf("\n");
		printf("FILL>> Filling pad byte(0x%02x) at end of input file. \n", Pad_Byte);
	
		data_buf = (unsigned char*)malloc( MAX_BUF_SIZ*sizeof(unsigned char) );

		while( EOF != (c=fgetc(inpfile)) )
		{
			if(outfile) fprintf(outfile,"%c", c);
			//data_buf[index] = c;
	
			index++;
	
			if( MAX_BUF_SIZ < index ) {
				beep(700,100);

				AllFilesClosed();

				printf("\n\nFILL>> [++ERROR++] MAX:%u MB. Too bigger file... \n",MAX_BUF_SIZ/(ONE_MEGA_BYTE) );
				exit(0);
				return 0;
			}
		}
	
		if( total_bin_size < index )
		{
			printf("FILL>> Checking file size... %s (= %dKB). Check please... \r\n\n", str_fillSiz, index/1024);
		}
		else
		{
			for(ii=0; ii<(total_bin_size-index); ii++)
			{
				if(outfile) fprintf(outfile,"%c", Pad_Byte); // 2020.07.03 0xFF);
			}
		
			printf("FILL>> Total file size : %d Bytes. (%#x)\n", total_bin_size, total_bin_size );
			printf("FILL>> Fill pad byte (%#x) -  OK \n", Pad_Byte);
			
		}

		AllFilesClosed();

	}
	else if( 1 == isNKinform )
	{
		int idx, ii=0;
		unsigned int load_addr;
		unsigned int image_size;
		unsigned int record_addr, record_size, record_crc, tot_record_size=0;
		unsigned int rom_header_ind = 0, rom_header_addr = 0;
		unsigned int nk_calc_crc=0, rcl=0;
		char rom_header_txt[5] = {0,};
		static int 	prev_record_addr = -1, prev_record_size = -1;
		int	is_BIN_OK = 1;
		FILE *BIN_nb0 = NULL;
		ROMHDR   rom_hdr_s;


/* -- nk.bin structure ---- */
#define NK_BIN_ID 				7	/* 7 bytes */
#define NK_BIN_LAUNCH_ADDR 		4	/* 4bytes */
#define NK_BIN_IMAGE_SIZE 		4	/* 4bytes */

#define NK_BIN_RECORD_ADDR		4
#define NK_BIN_RECORD_SIZE		4
#define NK_BIN_RECORD_CRC		4

#define pTOC_OFFSET_ADDR 		0x44
   
   
#define BIN_SYNC 				'B'  
#define ERR_OPENFILE  			1
#define ERR_READFILE  			2
#define ERR_FILETYPE  			3
#define ERR_ALLOCMEM  			4
#define ERR_NOERR 				5

#define IMAGE_FILE_MACHINE_SH3          0x01a2
#define IMAGE_FILE_MACHINE_SH3E         0x01a4
#define IMAGE_FILE_MACHINE_SH3DSP       0x01a3
#define IMAGE_FILE_MACHINE_SH3E         0x01a4
#define IMAGE_FILE_MACHINE_SH4          0x01a6
#define IMAGE_FILE_MACHINE_I386         0x014c
#define IMAGE_FILE_MACHINE_THUMB        0x01c2
#define IMAGE_FILE_MACHINE_POWERPC      0x01F0
#define IMAGE_FILE_MACHINE_ARM          0x01c0
#define IMAGE_FILE_MACHINE_THUMB        0x01c2
#define IMAGE_FILE_MACHINE_MIPS16       0x0266
#define IMAGE_FILE_MACHINE_ALPHA64      0x0284
#define IMAGE_FILE_MACHINE_MIPSFPU16    0x0466
#define IMAGE_FILE_MACHINE_R4000        0x0166
#define IMAGE_FILE_MACHINE_R10000       0x0168
#define IMAGE_FILE_MACHINE_WCEMIPSV2    0x0169
#define IMAGE_FILE_MACHINE_MIPSFPU      0x0366


		if( 1==isCreateNB0 )
		{
			/* ========== OUTPUT FILE ================= */
			if( (BIN_nb0 = fopen( "CE_nk.nb0", "wb")) == NULL ) 
			{
				printf("\n\n[++ERROR++] Can not create output file (NK.NB0). \n");

				beep(700,100);
				AllFilesClosed();

				exit(0); /// help();
				return 0;
			}
		}

		printf("WinCE BIN Information...... FileName : %s \r\n", infile_name);
		printf("------------------------------------------------------------------------- \r\n");
		fprintf(outfile, "WinCE BIN Information...... FileName : %s \r\n", infile_name);
		fprintf(outfile,"------------------------------------------------------------------------- \r\n");


		data_buf = (unsigned char*)malloc( MAX_BUF_SIZ*sizeof(unsigned char) );

		memset(data_buf, 0x00, sizeof(data_buf));
		fr_size = fread(data_buf, sizeof(unsigned char), NK_BIN_ID, inpfile );
		if( (0==strncmp(data_buf, (char*)"B000FF", NK_BIN_ID-1)) && (0x0A==data_buf[NK_BIN_ID-1]) )
		{
			data_buf[NK_BIN_ID-1] = '\0';
			printf("%s Inform> Standard BIN Format              : %s+0x0A \r\n", infile_name, data_buf);
			fprintf(outfile,"%s Inform> Standard BIN Format              : %s+0x0A \r\n", infile_name, data_buf);
			is_BIN_OK = 1;	
		}
		else if( (0==strncmp(data_buf, (char*)"X000FF", NK_BIN_ID-1)) && (0x0A==data_buf[NK_BIN_ID-1]) )
		{
			data_buf[NK_BIN_ID-1] = '\0';
			printf("%s Inform> Extended BIN Format              : %s+0x0A \r\n", infile_name, data_buf);
			fprintf(outfile,"%s Inform> Extended BIN Format              : %s+0x0A \r\n", infile_name, data_buf);
			is_BIN_OK = 1;	
		}
		else
		{
			printf("%s Inform> Unknown BIN Format               :( ", infile_name);
			fprintf(outfile,"%s Inform> Unknown BIN Format               :( ", infile_name);
			for(idx=0; idx<NK_BIN_ID; idx++)
			{
				printf("%c", data_buf[idx] );
				fprintf(outfile,"%c", data_buf[idx] );
			}
			printf(") \n");
			fprintf(outfile,") \r\n");
			is_BIN_OK = 0;	
		}


		if( is_BIN_OK )
		{

			/* ------ Load Address to SDRAM ---------------- */
			memset(data_buf, 0x00, sizeof(data_buf));
			fr_size = fread(data_buf, sizeof(unsigned char), NK_BIN_LAUNCH_ADDR, inpfile );
			load_addr = data_buf[3]<<24 | data_buf[2]<<16 | data_buf[1]<<8 | data_buf[0];
			printf("%s Inform> Load Address (RAM start address) : 0x%X \r\n", infile_name, load_addr);
			fprintf(outfile,"%s Inform> Load Address (RAM start address) : 0x%X \r\n", infile_name, load_addr);
			
			printf("%s Inform> pTOC                             : 0x%X \r\n", infile_name, (load_addr+pTOC_OFFSET_ADDR) );
			fprintf(outfile,"%s Inform> pTOC                             : 0x%X \r\n", infile_name, (load_addr+pTOC_OFFSET_ADDR) );


			/* ------ bin image Size ---------------- */
			memset(data_buf, 0x00, sizeof(data_buf));
			fr_size = fread(data_buf, sizeof(unsigned char), NK_BIN_IMAGE_SIZE, inpfile );
			image_size = data_buf[3]<<24 | data_buf[2]<<16 | data_buf[1]<<8 | data_buf[0];
			printf("%s Inform> BIN Image size                   : 0x%08X (%.3fKB) \r\n", infile_name, image_size, (image_size/1024.0));
			fprintf(outfile,"%s Inform> BIN Image size                   : 0x%08X (%.3fKB) \r\n", infile_name, image_size, (image_size/1024.0));
	
			/* Record Start */
			prev_record_addr = -1;
			prev_record_size = -1;

			printf("------------------------------------------------------------------------- \r\n");
			fprintf(outfile,"------------------------------------------------------------------------- \r\n");
			for(idx=1; is_BIN_OK ; idx++)
			{
				/* ------ Record 1 ~ N ---------------- */
				memset(data_buf, 0x00, sizeof(data_buf));
				fr_size = fread(data_buf, sizeof(unsigned char), 
								(NK_BIN_RECORD_ADDR+NK_BIN_RECORD_SIZE+NK_BIN_RECORD_CRC), 
								inpfile );

				record_addr = data_buf[3]<<24 | data_buf[2]<<16 | data_buf[1]<<8 | data_buf[0];
				record_size = data_buf[3+4]<<24 | data_buf[2+4]<<16 | data_buf[1+4]<<8 | data_buf[0+4];
				record_crc  = data_buf[3+4+4]<<24 | data_buf[2+4+4]<<16 | data_buf[1+4+4]<<8 | data_buf[0+4+4];
				if( (0==record_addr) && (0==record_crc) ) { /*printf("***"); */break; }
				tot_record_size += record_size;
				
				printf("    Record_%04d> Launch Address       : 0x%X ", idx, record_addr);
				fprintf(outfile,"    Record_%04d> Launch Address       : 0x%X ", idx, record_addr);

				if( rom_header_addr==record_addr )
				{
					printf(" <-- *ROM Header Address* \r\n");
					fprintf(outfile," <-- *ROM Header Address* \r\n");
				}
				else
				{
					printf("\r\n");
					fprintf(outfile,"\r\n");
				}

				printf("    Record_%04d> Payload Size         : (0x%-6X) %d Bytes (%.1f KB) \r\n", idx, record_size, record_size, record_size/1024.0 );
				fprintf(outfile,"    Record_%04d> Payload Size         : (0x%-6X) %d Bytes (%.1f KB) \r\n", idx, record_size, record_size, record_size/1024.0 );

				printf("    Record_%04d> CheckSum             : 0x%08X ", idx, record_crc);
				fprintf(outfile,"    Record_%04d> CheckSum             : 0x%08X ", idx, record_crc);


			#if 1
				if( (1==isCreateNB0) && (-1!=prev_record_addr) && (-1!=prev_record_size) )
				{
					if( (prev_record_addr + prev_record_size) < record_addr )
					{
						for(ii=0; ii < (record_addr - (prev_record_addr + prev_record_size)) ; ii++)
						{
							fprintf(BIN_nb0,"%c", 0x00);
						}
					}
				}
			#endif


				/* --------- payload -------- */
				memset(data_buf, 0x00, sizeof(data_buf));
				fr_size = fread(data_buf, sizeof(unsigned char), record_size, inpfile ); /* size 만큰 payload reading.. */
				/* --------- payload -------- */
				if( fr_size <= 0) break;

				if( 1==isCreateNB0 )
				{
					fwrite(data_buf, sizeof(unsigned char), record_size, BIN_nb0 );
				}
				/// -----Calc CRC for all Record-----------
				nk_calc_crc = 0UL;
				for(rcl=0; rcl<record_size; rcl++)
					nk_calc_crc += data_buf[rcl];
				printf(" (Calc_CRC: 0x%08X", nk_calc_crc);
				fprintf(outfile," (Calc_CRC: 0x%08X", nk_calc_crc);

				if(nk_calc_crc==record_crc) 
				{
					printf(" - CRC OK) \n");
					fprintf(outfile," - CRC OK) \r\n");
				}
				else
				{
					printf(" - CRC Failed) \n");
					fprintf(outfile," - CRC Failed) \r\n");
				}
				/// -----------------------------------------

			#if 1 /// 2014.07.19, new--
			
				if( rom_header_addr==record_addr )
				{
					memset( &rom_hdr_s, 0x00, sizeof(ROMHDR) );
					memcpy( (void*)&rom_hdr_s, data_buf, sizeof(ROMHDR) );

					printf("     ------------ ROMHDR ---------------\n");
					printf("      DLL First           : 0x%08X \n", rom_hdr_s.dllfirst);
					printf("      DLL Last            : 0x%08X \n", rom_hdr_s.dlllast);
					printf("      Physical First      : 0x%08X \n", rom_hdr_s.physfirst);
					printf("      Physical Last       : 0x%08X \n", rom_hdr_s.physlast);
					printf("      Num Modules         : %10d \n", rom_hdr_s.nummods);
					printf("      RAM Start           : 0x%08X \n", rom_hdr_s.ulRamStart);
					printf("      RAM Free            : 0x%08X \n", rom_hdr_s.ulRAMFree);
					printf("      RAM End             : 0x%08X \n", rom_hdr_s.ulRAMEnd);
					printf("      Num Copy Entries    : %10d \n", rom_hdr_s.ulCopyEntries);
					printf("      Copy Entries Offset : 0x%08X \n", rom_hdr_s.ulCopyOffset);
					printf("      Length of profile   : %10u \n", rom_hdr_s.ulProfileLen);
					printf("      Prof Symbol Offset  : 0x%08X \n", rom_hdr_s.ulProfileOffset);
					printf("      Num Files           : %10d \n", rom_hdr_s.numfiles);
					printf("      Kernel flags        : 0x%08X \n", rom_hdr_s.ulKernelFlags);
					printf("      MiscFlags           : 0x%08X \n", rom_hdr_s.usMiscFlags);
					printf("      CPU                 : 0x%04x ", rom_hdr_s.usCPUType);

					fprintf(outfile,"     ------------ ROMHDR --------------- \r\n");
					fprintf(outfile,"      DLL First           : 0x%08X \r\n", rom_hdr_s.dllfirst);
					fprintf(outfile,"      DLL Last            : 0x%08X \r\n", rom_hdr_s.dlllast);
					fprintf(outfile,"      Physical First      : 0x%08X \r\n", rom_hdr_s.physfirst);
					fprintf(outfile,"      Physical Last       : 0x%08X \r\n", rom_hdr_s.physlast);
					fprintf(outfile,"      Num Modules         : %10d \r\n", rom_hdr_s.nummods);
					fprintf(outfile,"      RAM Start           : 0x%08X \r\n", rom_hdr_s.ulRamStart);
					fprintf(outfile,"      RAM Free            : 0x%08X \r\n", rom_hdr_s.ulRAMFree);
					fprintf(outfile,"      RAM End             : 0x%08X \r\n", rom_hdr_s.ulRAMEnd);
					fprintf(outfile,"      Num Copy Entries    : %10d \r\n", rom_hdr_s.ulCopyEntries);
					fprintf(outfile,"      Copy Entries Offset : 0x%08X \r\n", rom_hdr_s.ulCopyOffset);
					fprintf(outfile,"      Length of profile   : %10u \r\n", rom_hdr_s.ulProfileLen);
					fprintf(outfile,"      Prof Symbol Offset  : 0x%08X \r\n", rom_hdr_s.ulProfileOffset);
					fprintf(outfile,"      Num Files           : %10d \r\n", rom_hdr_s.numfiles);
					fprintf(outfile,"      Kernel flags        : 0x%08X \r\n", rom_hdr_s.ulKernelFlags);
					fprintf(outfile,"      MiscFlags           : 0x%08X \r\n", rom_hdr_s.usMiscFlags);
					fprintf(outfile,"      CPU                 : 0x%04x ", rom_hdr_s.usCPUType);

					switch(rom_hdr_s.usCPUType) 
					{
					case IMAGE_FILE_MACHINE_SH3:
					  printf("(SH3)\n");
					  fprintf(outfile,"(SH3) \r\n");
					  break;
					case IMAGE_FILE_MACHINE_SH3E:
					  printf("(SH3e)\n");
					  fprintf(outfile,"(SH3e)\r\n");
					  break;
					case IMAGE_FILE_MACHINE_SH3DSP:
					  printf("(SH3-DSP)\n");
					  fprintf(outfile,"(SH3-DSP)\r\n");
					  break;
					case IMAGE_FILE_MACHINE_SH4:
					  printf("(SH4)\n");
					  fprintf(outfile,"(SH4)\r\n");
					  break;
					case IMAGE_FILE_MACHINE_I386:
					  printf("(x86)\n");
					  fprintf(outfile,"(x86)\r\n");
					  break;
					case IMAGE_FILE_MACHINE_THUMB:
					  printf("(Thumb)\n");
					  fprintf(outfile,"(Thumb)\r\n");
					  break;
					case IMAGE_FILE_MACHINE_ARM:
					  printf("(ARM)\n");
					  fprintf(outfile,"(ARM)\r\n");
					  break;
					case IMAGE_FILE_MACHINE_POWERPC:
					  printf("(PPC)\n");
					  fprintf(outfile,"(PPC)\r\n");
					  break;
					case IMAGE_FILE_MACHINE_R4000:
					  printf("(R4000)\n");
					  fprintf(outfile,"(R4000)\r\n");
					  break;
					case IMAGE_FILE_MACHINE_MIPS16:
					  printf("(MIPS16)\n");
					  fprintf(outfile,"(MIPS16)\r\n");
					  break;
					case IMAGE_FILE_MACHINE_MIPSFPU:
					  printf("(MIPSFPU)\n");
					  fprintf(outfile,"(MIPSFPU)\r\n");
					  break;
					case IMAGE_FILE_MACHINE_MIPSFPU16:
					  printf("(MIPSFPU16)\n");
					  fprintf(outfile,"(MIPSFPU16)\r\n");
					  break; 
					default:
					  printf("(Unknown)\r\n");
					  fprintf(outfile,"(Unknown)\r\n");
					  break;
					}


					printf("      RAM -- FSRAMPERCENT : 0x%08X \n", rom_hdr_s.ulFSRamPercent);
					printf("      Device Start addr   : 0x%08X \n", rom_hdr_s.ulDrivglobStart);
					printf("      Device length       : 0x%08X \n", rom_hdr_s.ulDrivglobLen);
					printf("      ROM Header ext.     : 0x%08X \n", rom_hdr_s.pExtensions);
					printf("      Tracking MEM start  : 0x%08X \n", rom_hdr_s.ulTrackingStart); /// Start Address
					printf("      Tracking MEM end    : 0x%08X \n",rom_hdr_s.ulTrackingLen);

					fprintf(outfile,"      RAM -- FSRAMPERCENT : 0x%08X \r\n", rom_hdr_s.ulFSRamPercent);
					fprintf(outfile,"      Device Start addr   : 0x%08X \r\n", rom_hdr_s.ulDrivglobStart);
					fprintf(outfile,"      Device length       : 0x%08X \r\n", rom_hdr_s.ulDrivglobLen);

					fprintf(outfile,"      ROM Header ext.     : 0x%08X \r\n", rom_hdr_s.pExtensions);
					fprintf(outfile,"      Tracking MEM start  : 0x%08X \r\n", rom_hdr_s.ulTrackingStart); /// Start Address
					fprintf(outfile,"      Tracking MEM end    : 0x%08X \r\n",rom_hdr_s.ulTrackingLen);

				
					printf("      Extensions          : 0x%08X \n", rom_hdr_s.pExtensions );
					fprintf(outfile,"      Extensions          : 0x%08X \r\n", rom_hdr_s.pExtensions );

			
					if (rom_hdr_s.pExtensions) 
					{
				#if 0
						printf("     -------- ROMHDR Extensions ---------\n");


						printf("     ---------- COPY SECTIONS -----------\n");


						printf("     ------------- MODULES --------------\n");
				#endif
					}



					printf("     ----------------------------------- \r\n");
					fprintf(outfile,"     ----------------------------------- \r\n");

				}

				if(2==idx) /// ROM Header Indicator , idx=2
				{
					sprintf(rom_header_txt,"%c%c%c%c", data_buf[0], data_buf[1], data_buf[2], data_buf[3]);
					rom_header_ind  = (data_buf[0]<<24 | data_buf[1]<<16 | data_buf[2]<<8 | data_buf[3]);
					rom_header_addr = (data_buf[7]<<24 | data_buf[6]<<16 | data_buf[5]<<8 | data_buf[4]);
					printf("    Record_%04d> ROM Header Indicator : %s \r\n", idx, rom_header_txt );
					printf("    Record_%04d> ROM Header Address   : 0x%08X \r\n", idx, rom_header_addr);

					fprintf(outfile,"    Record_%04d> ROM Header Indicator : %s \r\n", idx, rom_header_txt);
					fprintf(outfile,"    Record_%04d> ROM Header Address   : 0x%08X \r\n", idx, rom_header_addr);
				}

				printf(" \r\n");
				fprintf(outfile," \r\n");
			#endif

				prev_record_addr = record_addr;
				prev_record_size = record_size;
				
				
			}

			memset(data_buf, 0x00, sizeof(data_buf));

			printf("WinCE BIN (%s) payload size = %.3f MB (0x%X)\r\n", infile_name, (tot_record_size/1024.0)/1024.0,  tot_record_size);
			fprintf(outfile,"WinCE BIN (%s) payload size = %.3f MB (0x%X) \r\n", infile_name, (tot_record_size/1024.0)/1024.0,  tot_record_size);


			if( 1==isCreateNB0 )
			{
				if(BIN_nb0) fclose(BIN_nb0);
				printf("\nWinCE OS Kernel(NK.BIN) Information!!! Created CE_nk.nb0 - OK");
				fprintf(outfile,"\r\nWinCE OS Kernel(NK.BIN) Information!!! Created CE_nk.nb0 - OK");
			}
			else
			{
				printf("\nWinCE OS Kernel(NK.BIN) Information!!! - OK");
			}

		}
		else
		{
			printf("\nThis file[%s] is not WinCE OS Kernel(NK.BIN)... Check file.. \n\n", infile_name );
		}

		AllFilesClosed();

		opt_ok = 0; /* NOTE !!! clear!!!  */

	}

#if ELF2BIN	
	else if( 1 == isElf2Bin )
	{

		if (fstat(fileno(inpfile), &file_statbuf) < 0) 
		{ 
			printf("\n\n[++ERROR++]Cannot stat [%s]\n", infile_name ); 

			AllFilesClosed(); // 2020.07.10
			exit(0);
			return 2; 
		}	 
		
		
		elf_size = file_statbuf.st_size; 
		printf("ELF2BIN>> ELF size      : %u Bytes\n", elf_size ); 

		data_buf = (unsigned char*)malloc( elf_size );
		memset( data_buf, 0x00, elf_size );
		
		if( NULL==data_buf ) 
		{ 
			printf("ELF2BIN>> Cannot allocate space for %s!\n", infile_name ); 
		} 
		else
		{
			if (fread(data_buf, sizeof (char), elf_size, inpfile) != elf_size) 
			{ 
				printf("ELF2BIN>> Cannot read %s!\n", infile_name ); 
			} 
			
			elf_dumpimage(data_buf, elf_size); 
		}

		AllFilesClosed();

	}
#endif

	else if( 1 == isMot2bin )
	{
		unsigned int iErrCount = 0;
		
		/* line inputted from file */
		char HexaLine[HEX_MAX_LINE_SIZE];
		
		/* flag that a file was read */
		bool Fileread;
	
		/* cmd-line parameter # */
		char *pBin = NULL;
	
		int result;
	
		/* Application specific */
		unsigned int First_Word, Address;
		unsigned int Type;
		unsigned int Exec_Address;
		unsigned int temp;
		unsigned int Record_Count, Record_Checksum;
	
		byte	Data_Str[HEX_MAX_LINE_SIZE];


		Phys_AddrTemp = 0; // added at 2016.03.10
		iErrCount = 0; // clear
	

		memset( HexaLine, 0x00, sizeof(HexaLine) );
		memset( Data_Str, 0x00, sizeof(Data_Str) );


		Fileread = 1;
	
		/* When the hex file is opened, the program will read it in 2 passes.
		The first pass gets the highest and lowest addresses so that we can allocate
		the right size.
		The second pass processes the hex data. */
	
		/* To begin, assume the lowest address is at the end of the memory.
		 While reading each records, subsequent addresses will lower this number.
		 At the end of the input file, this value will be the lowest address.
	
		 A similar assumption is made for highest address. It starts at the
		 beginning of memory. While reading each records, subsequent addresses will raise this number.
		 At the end of the input file, this value will be the highest address. 
		----------------------------------------------------------------------- */

		Lowest_Address = (unsigned int)-1;
		Highest_Address = 0;
		Records_Start = 0;
		Record_Nb = 0;
		First_Word = 0;
	
		/* get highest and lowest addresses so that we can allocate the right size */
		do
		{
			unsigned int i;
	
			/* Read a line from input file. */
			memset( HexaLine, 0x00, sizeof(HexaLine) );
	
			if( NULL == fgets( HexaLine, HEX_MAX_LINE_SIZE, inpfile ) )
			{
				break;
			}
			
			Record_Nb++;
	
			/* Remove carriage return/line feed at the end of line. */
			i = strlen(HexaLine);

		#if 0 // 2016.03.09_initial
			if (--i != 0)
		#else
			if (--i > 0)
		#endif
			{
				if (HexaLine[i] == '\n') HexaLine[i] = '\0';
				if (HexaLine[i] == '\r') HexaLine[i] = '\0';
	
				pBin = (char *) Data_Str;
	
				switch(HexaLine[1])
				{
				case '0':
					Nb_Bytes = 1; /* This is to fix the Highest_Address set to -1 when Nb_Bytes = 0 */
					break;
	
				/* 16 bits address */
				// S1 Record. The type of record field is 'S1' (0x5331).
				// The address field is intrepreted as a 2-byte address. 
				// The data field is composed of memory loadable data.
				case '1':
					result = sscanf (HexaLine,"S%1x%2x%4x",&Type,&Nb_Bytes,&First_Word);
		            if (result != 3 && (i>0) ) 
		            {
						// fprintf(stderr,"S1:Error in line %d of hex file.\n", Record_Nb);
						printf("Line%6d :S1-Error in hex file. (%d) \n", Record_Nb, result );
						iErrCount ++; // 2016.03.05

						// ---------------------------------
						if( iErrCount > MAX_ERR_COUNT*2 ) // 2016.03.05
						{
							printf("\n\n Too many parsing error.  Check Motorola hexa family type!! \n");
						
							iErrCount = 0; // clear
							AllFilesClosed();
						
							exit(0);
							return 1;
						}
						// ---------------------------------

		            }
				
					/* Adjust Nb_Bytes for the number of data bytes */
					Nb_Bytes = Nb_Bytes - 3;
					break;
	
				/* 24 bits address */
				// S2 Record. The type of record field is 'S2' (0x5332). 
				// The address field is intrepreted as a 3-byte address. 
				// The data field is composed of memory loadable data.
				case '2':
					result = sscanf (HexaLine,"S%1x%2x%6x",&Type,&Nb_Bytes,&First_Word);
		            if (result != 3 && (i>0) ) 
		            {
						//fprintf(stderr,"S2:Error in line %d of hex file.\n", Record_Nb);
						printf("Line%6d :S2-Error in hex file. (%d) \n", Record_Nb, result );
						iErrCount ++; // 2016.03.05
		            }
				
					/* Adjust Nb_Bytes for the number of data bytes */
					Nb_Bytes = Nb_Bytes - 4;
					break;
	
				/* 32 bits address */
				// S3 Record. The type of record field is 'S3' (0x5333). 
				// The address field is intrepreted as a 4-byte address. 
				// The data field is composed of memory loadable data.
				case '3':
					result = sscanf (HexaLine,"S%1x%2x%8x",&Type,&Nb_Bytes,&First_Word);
		            if (result != 3 && (i>0) ) 
	            	{
						//fprintf(stderr,"S3:Error in line %d of hex file.\n", Record_Nb);
						printf("Line%6d :S3-Error in hex file. (%d) \n", Record_Nb, result );
						iErrCount ++; // 2016.03.05
		            }
				
					/* Adjust Nb_Bytes for the number of data bytes */
					Nb_Bytes = Nb_Bytes - 5;
					break;
				}
	
				Phys_Addr = First_Word;

			#if HEX2BIN_MOTOROLA_ZERO_FORCED // 2020.06.24 Zero-Forced!!
				// ----------------------------------------
				// Hex Address Zero based converted 
				// ----------------------------------------
				if( HEX2BIN_ZERO_FORCED==Enable_HexaAddr_Zero_Forced )
				{
					// S 3 0D 00000000 FFFFFFFF8007E80B7C
					// S 3 15 08000000 000C0020F99C0208799102087B910208ED
					// S 3 15 9FC00000 00B4083C2D20093C40002935201009AD7D
			
					if( iOnceCheck && (HexaLine[1]=='1' || HexaLine[1]=='2' || HexaLine[1]=='3') )
					{
						iOnceCheck = 0;
						if( Phys_Addr>0x00000000 )
						{
							Phys_AddrTemp = Phys_Addr;

							printf("\n");
							printf("--------------------------------------------------------\n");
							printf("Line%6d :S%c-Record: Phys_Addr = 0x%08X. \n", Record_Nb, HexaLine[1], Phys_Addr );
						}
					}
					
					if( Phys_AddrTemp && (0==iOnceCheck) )
					{
						Phys_Addr -= Phys_AddrTemp;
						//if(verbose) printf("S%c-rrecord: Phys_Addr = 0x%08X \n", HexaLine[1], Phys_Addr );
					}
				}
			#endif

	
				/* Set the lowest address as base pointer. */
				if (Phys_Addr < Lowest_Address)
					Lowest_Address = Phys_Addr;
	
				/* Same for the top address. */
				temp = Phys_Addr + Nb_Bytes -1;
	
				if (temp > Highest_Address)
				{
					Highest_Address = temp;

					Real_Length = Highest_Address; // real size 2016.03.05
					if (verbose) 
					{
						if(iVerbosType<2) printf("Line%6d :Highest Address : %#08x  Read Byte:%#02x \r", Record_Nb, Highest_Address, Nb_Bytes );
						else printf("Line%6d :Highest Address : %#08x  Read Byte:%#02x \n", Record_Nb, Highest_Address, Nb_Bytes );
					}

				}

			}
		}
		while (!feof (inpfile));


		// ---------------------------------
		if( iErrCount > MAX_ERR_COUNT ) // 2016.03.05
		{
			printf("\n\nToo many MOTOROLA parsing error.  Check Motorola hexa type!!! \n");

			iErrCount = 0; // clear

			AllFilesClosed();

			exit(0);
			return 1;
		}
		// ---------------------------------


		
		Allocate_Memory_And_Rewind();
	
		Record_Nb = 0;
	
		/* Read the file & process the lines. */
		do /* repeat until EOF(Filin) */
		{
			int i;
	
			Checksum = 0;
	
			/* Read a line from input file. */
			memset( HexaLine, 0x00, sizeof(HexaLine) );
			if( NULL == fgets( HexaLine, HEX_MAX_LINE_SIZE, inpfile ) )
			{
				break;
			}
			
			Record_Nb++;
	
			/* Remove carriage return/line feed at the end of line. */
			i = strlen(HexaLine);

		#if 0 // 2016.03.09_initial
			if (--i != 0)
		#else
			if (--i > 0)
		#endif
			{
				if (HexaLine[i] == '\n') HexaLine[i] = '\0';
				if (HexaLine[i] == '\r') HexaLine[i] = '\0';
	
				/* Scan starting address and nb of bytes. */
				/* Look at the record type after the 'S' */
				Type = 0;
	
				switch(HexaLine[1])
				{
				case '0':
					result = sscanf (HexaLine,"S0%2x0000484452%2x",&Nb_Bytes,&Record_Checksum);

					//if (result != 2) fprintf(stderr,"S0:Error in line %d of hex file\n", Record_Nb);
		            if (result != 2 && (i>0) ) 
		            {
						// ------------------------------------------------------------
						// -- [S0 17 52656C65617365204275696C645C726F6D702E6F7574 77] 
						// ------------------------------------------------------------
						char TempC[3];
						int  iidx=2, wasChar=0;
						unsigned char TempVal=0, TempCRCSumVal=0, TempLenVal=0, TempCRC=0, iCount=0;
						char szS0tmp[5]={0,}, szS0rectxt[1024]={0,};

						memset( szS0rectxt, 0x00, sizeof(szS0rectxt) );

						strcpy( szS0rectxt, "S0-record         : [" );
						for(iidx=2; HexaLine[iidx] != 0x00 && iidx<1024; iidx+=2 )
						{
							memset( TempC, 0x00, sizeof(TempC) );
							strncpy( (char*)TempC, (char *)&HexaLine[iidx], 2);
							TempVal = str2hex(TempC);
							iCount ++;
							if( 2==iidx ) 
							{
								TempLenVal = TempVal; // length once saved!
							}
							else
							{
								TempCRCSumVal += TempVal;
								TempCRCSumVal &= 0xFF;

								if( iCount <= TempLenVal )
								{
									memset( szS0tmp, 0x00, sizeof(szS0tmp) );
									if( TempVal >= 0x20 && TempVal <= 0x7F )
									{
										sprintf(szS0tmp, "%c", TempVal);
										wasChar=1;
									}
									else
									{
										if(wasChar) 
										{ 
											wasChar=0;  
											sprintf(szS0tmp, " %02x ", TempVal);
										}
										else 
										{
											sprintf(szS0tmp, "%02x ", TempVal);
										}
									}

									strcat( szS0rectxt, szS0tmp );
								}
							}
						}
						strcat( szS0rectxt, "]");
						printf("%s -> ", szS0rectxt );

						
						TempCRC    = TempVal;  // minus last Checksum 
						TempCRCSumVal = TempCRCSumVal - TempVal; // minus last Checksum 
						TempCRCSumVal = TempCRCSumVal + Nb_Bytes;

						// checksum confirm!!
						if( ((255-TempCRCSumVal)&0xFF) == (TempVal&0xFF) )
						{
							// OK !!!!
							//Checksum = Nb_Bytes + 0x52 + 0x65 + 0x6C + 0x65 + 0x61 + 0x73 + 0x65 + 0x20 + 0x42 + 0x75 + 0x69 + 0x6C + 0x64 + 0x5C + 0x72 + 0x6F + 0x6D + 0x70 + 0x2E + 0x6F + 0x75 + 0x74;
							//printf("S0 Checksum = %#02x -- OK \n", (255-Checksum) );
							printf("S0 Checksum is OK \n");
						}
						else
						{
							//fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
							printf("Line%6d :S0-Error in selected hex file. checksum(%x) \n", Record_Nb, TempCRC );
							//printf("[%s]\n", HexaLine );
						}

						// read checksum ---
						Checksum = TempCRCSumVal;
						Record_Checksum = TempCRC;
		            }
					else // 0000484452
					{
		                Checksum = Nb_Bytes + 0x48 + 0x44 + 0x52;
						printf("Line%6d :S0-record(default): [%c%c%c] \n", Record_Nb, 0x48, 0x44, 0x52 );
					}

	
					/* Adjust Nb_Bytes for the number of data bytes */
					Nb_Bytes = 0;
					break;
	
				/* 16 bits address */
				case '1':
					result = sscanf (HexaLine,"S%1x%2x%4x%s",&Type,&Nb_Bytes,&Address,Data_Str);
					if (result != 4) fprintf(stderr,"Line%6d :S1:Error in hex file\n", Record_Nb);
					Checksum = Nb_Bytes + (Address >> 8) + (Address & 0xFF);
	
					/* Adjust Nb_Bytes for the number of data bytes */
					Nb_Bytes = Nb_Bytes - 3;
					break;
	
				/* 24 bits address */
				case '2':
					result = sscanf (HexaLine,"S%1x%2x%6x%s",&Type,&Nb_Bytes,&Address,Data_Str);
					if (result != 4) fprintf(stderr,"Line%6d :S2:Error in hex file\n", Record_Nb);
					Checksum = Nb_Bytes + (Address >> 16) + (Address >> 8) + (Address & 0xFF);
	
					/* Adjust Nb_Bytes for the number of data bytes */
					Nb_Bytes = Nb_Bytes - 4;
					break;
	
				/* 32 bits address */
				case '3':
					result = sscanf (HexaLine,"S%1x%2x%8x%s",&Type,&Nb_Bytes,&Address,Data_Str);
					if (result != 4) fprintf(stderr,"Line%6d :S3:Error in hex file\n", Record_Nb);
					Checksum = Nb_Bytes + (Address >> 24) + (Address >> 16) + (Address >> 8) + (Address & 0xFF);
	
					/* Adjust Nb_Bytes for the number of data bytes */
					Nb_Bytes = Nb_Bytes - 5;
					break;
	
				case '5':
					result = sscanf (HexaLine,"S%1x%2x%4x%2x",&Type,&Nb_Bytes,&Record_Count,&Record_Checksum);
					if (result != 4) fprintf(stderr,"Line%6d :S5:Error in hex file\n", Record_Nb);
					Checksum = Nb_Bytes + (Record_Count >> 8) + (Record_Count & 0xFF);
	
					/* Adjust Nb_Bytes for the number of data bytes */
					Nb_Bytes = 0;
					break;
	
				case '7':
					result = sscanf (HexaLine,"S%1x%2x%8x%2x",&Type,&Nb_Bytes,&Exec_Address,&Record_Checksum);
					if (result != 4) fprintf(stderr,"Line%6d :S7:Error in hex file\n", Record_Nb);
					Checksum = Nb_Bytes + (Exec_Address >> 24) + (Exec_Address >> 16) + (Exec_Address >> 8) + (Exec_Address & 0xFF);
					Nb_Bytes = 0;
					break;
	
				case '8':
					result = sscanf (HexaLine,"S%1x%2x%6x%2x",&Type,&Nb_Bytes,&Exec_Address,&Record_Checksum);
					if (result != 4) fprintf(stderr,"Line%6d :S8:Error in hex file\n", Record_Nb);
					Checksum = Nb_Bytes + (Exec_Address >> 16) + (Exec_Address >> 8) + (Exec_Address & 0xFF);
					Nb_Bytes = 0;
					break;
				case '9':
					result = sscanf (HexaLine,"S%1x%2x%4x%2x",&Type,&Nb_Bytes,&Exec_Address,&Record_Checksum);
					if (result != 4) fprintf(stderr,"Line%6d :S9:Error in hex file\n", Record_Nb);
					Checksum = Nb_Bytes + (Exec_Address >> 8) + (Exec_Address & 0xFF);
					Nb_Bytes = 0;
					break;
				}
	
				pBin = (char *) Data_Str;
	
				/* If we're reading the last record, ignore it. */
				switch (Type)
				{
				/* Data record */
				case 1:
				case 2:
				case 3:
					if (Nb_Bytes == 0)
					{
						fprintf(stderr,"0 byte length Data record ignored\n");
						break;
					}
	
					Phys_Addr = Address;

				#if HEX2BIN_MOTOROLA_ZERO_FORCED // 2020.06.24 Zero-Forced!!
					// ----------------------------------------
					// Hex Address Zero based converted 
					// ----------------------------------------
					if( HEX2BIN_ZERO_FORCED==Enable_HexaAddr_Zero_Forced )
					{
						// S 3 0D 00000000 FFFFFFFF8007E80B7C
						// S 3 15 08000000 000C0020F99C0208799102087B910208ED
						// S 3 15 9FC00000 00B4083C2D20093C40002935201009AD7D
				
						if( Phys_AddrTemp )
						{
							if( Phys_Addr >= Phys_AddrTemp )
							{
								Phys_Addr -= Phys_AddrTemp;
								//EB_Printf(TEXT("[dnw] S%d-Record: Phys_Addr = 0x%08X \r\n"), Type, Phys_Addr );
							}
							else
							{
								printf("S%d-Record: Zero-forced Error!!! Phys_Addr: %#08X, Phys_Addr_Base: %#08X \n", Type, Phys_Addr, Phys_AddrTemp );
							}
						}
					}
				#endif


					pBin = ReadDataBytes(pBin);
	
					/* Read the Checksum value. */
					result = sscanf (pBin, "%2x",&Record_Checksum);
					if (result != 1) fprintf(stderr,"Line%6d ::Error in hex file\n", Record_Nb);
					break;
	
				case 5:
					if( verbose ) fprintf(stderr,"Line%6d ::S5:Record total: %d\n",Record_Nb, Record_Count);
					break;
	
				case 7:
					if( verbose ) fprintf(stderr,"Line%6d ::S7:Execution Address (unused): %#08x\n",Record_Nb, Exec_Address);
					break;
	
				case 8:
					if( verbose ) fprintf(stderr,"Line%6d ::S8:Execution Address (unused): %#06x\n",Record_Nb, Exec_Address);
					break;
	
				case 9:
					if( verbose ) fprintf(stderr,"Line%6d ::S9:Execution Address (unused): %#04x\n",Record_Nb, Exec_Address);
					break;
	
				/* Ignore all other records */
				default: // S4, S6
					if( verbose )
					{
						printf("Line%6d ::S%d-Record: Phys_Addr: %#08X \n", Record_Nb, Type, Phys_Addr );
						//printf("[%s] \r\n", HexaLine );
					}
					// ignore -- break;

				}
	
				Record_Checksum &= 0xFF;
	
				/* Verify Checksum value. */
				if (((Record_Checksum + Checksum) != 0xFF) && Enable_Checksum_Error)
				{
					printf("Line%6d ::Checksum error: should be 0x%02x, not be 0x%02x \n",Record_Nb, 255-Checksum, Checksum); ///, HexaLine);
					Status_Checksum_Error = true;
				}
			}
		}
		while (!feof (inpfile));
		/*-----------------------------------------------------------------------------*/
		
		printf("Binary file start : %#010x \n", Lowest_Address);
		printf("Records start     : %#010x \n", Records_Start);
		printf("Highest address   : %#010x \n", Highest_Address /*, (float)(Highest_Address/1024.0) */ );
		printf("Pad byte(def:FF)  : 0x%02X \n", Pad_Byte);


	#if HEX2BIN_MOTOROLA_ZERO_FORCED // 2016.03.10
		// ----------------------------------------
		// Hex Address Zero based converted 
		// ----------------------------------------
		if( HEX2BIN_ZERO_FORCED==Enable_HexaAddr_Zero_Forced )
		{
			if( Phys_AddrTemp )
			{
				printf("------ [ Zero-Address Forced Logic ] -------------------\n");
				printf("Physical Base address    : 0x%08X \n", Phys_AddrTemp );
				printf("Hex file real start addr : 0x%08X \n", Lowest_Address + Phys_AddrTemp );
				printf("Hex real Highest address : 0x%08X <- %.2fkB\n", Highest_Address + Phys_AddrTemp, (float)((Highest_Address + Phys_AddrTemp)/1024.0) );
				printf("--------------------------------------------------------\r\n");
				printf("Motorola S-Record address Zero-forced!!! \n");
			}

		}
	#endif


		WriteMemory();

		if (Status_Checksum_Error && Enable_Checksum_Error)
		{
			printf("\n");
			printf("MOTOROLA family hexa: Checksum error detected !!!\n");

			AllFilesClosed();

			exit(0);
			return 1;
		}
		else
		{
			printf("--------------------------------------------------------\n");
			printf("Parserd line number of hex record : %d lines \n", Record_Nb);
			printf("Converted the MOTOROLA hexa to binary. -- OK \n");

			AllFilesClosed();

			exit(0);
			return 1;
		}

	}
	else if( 1 == isIntel2bin )
	{
#define INTEL_BASE_ADDR_CMP 		0x0010 

		unsigned int iErrCount = 0;
		
		/* line inputted from file */
		char HexaLine[HEX_MAX_LINE_SIZE];
	
		/* flag that a file was read */
		bool Fileread;
	
		/* cmd-line parameter # */
		char *pBin = NULL;
	
		int result;
	
		/* Application specific */
		unsigned int First_Word, Address, Segment, Upper_Address;
		unsigned int Type;
		unsigned int Offset = 0x00;
		unsigned int temp;
	
		/* We will assume that when one type of addressing is selected, it will be valid for all the
		 current file. Records for the other type will be ignored. */
		unsigned int Seg_Lin_Select = NO_ADDRESS_TYPE_SELECTED;
	
		unsigned int temp2;
	
		byte	Data_Str[HEX_MAX_LINE_SIZE];
	

		Phys_AddrTemp = 0; // added at 2016.03.10

		iErrCount = 0; // added at 2016.03.07, Error count clear
	
		memset(HexaLine, 0x00, sizeof(HexaLine) ); /// 2014.06.26
		memset(Data_Str, 0x00, sizeof(Data_Str) ); /// 2014.06.26
	

		/* allocate a buffer */
		Memory_Block = (byte *)NoFailMalloc(Max_Length); 
		
		/* For EPROM or FLASH memory types, fill unused bytes with FF or the value specified by the p option */
		//memset (Memory_Block, 0xff, Max_Length);
	

		Fileread = 1;
	
		/* When the hex file is opened, the program will read it in 2 passes.
		The first pass gets the highest and lowest addresses so that we can allocate
		the right size.
		The second pass processes the hex data. */
	
		/* To begin, assume the lowest address is at the end of the memory.
		 While reading each records, subsequent addresses will lower this number.
		 At the end of the input file, this value will be the lowest address.
	
		 A similar assumption is made for highest address. It starts at the
		 beginning of memory. While reading each records, subsequent addresses will raise this number.
		 At the end of the input file, this value will be the highest address. */

		Lowest_Address = (unsigned int)-1;
		Highest_Address = 0;
		Records_Start = 0;
		Segment = 0;
		Upper_Address = 0;
		Record_Nb = 0;	  // Used for reporting errors
		First_Word = 0;

		printf("\n");
		
		/* Check if are set Floor and Ceiling Address and range is coherent*/
		VerifyRangeFloorCeil();
	
		/* get highest and lowest addresses so that we can allocate the rintervallo incoerenteight size */
		do
		{
			unsigned int i;
	

			/* Read a line from input file. */
			memset( HexaLine, 0x00, sizeof(HexaLine) );
	
			if( NULL == fgets( HexaLine, HEX_MAX_LINE_SIZE, inpfile ) )
			{
				break;
			}
			
			Record_Nb++;

	
			/* Remove carriage return/line feed at the end of line. */
			i = strlen(HexaLine);

		#if 0 // 2016.03.09_initial
			if (--i != 0)
		#else
			if (--i > 0)
		#endif
			{
				if (HexaLine[i] == '\n') HexaLine[i] = '\0';
				if (HexaLine[i] == '\r') HexaLine[i] = '\0'; // added by 2016.03.10
	
				/* Scan the first two bytes and nb of bytes.
				   The two bytes are read in First_Word since its use depend on the
				   record type: if it's an extended address record or a data record.
				   */
				result = sscanf (HexaLine, ":%2x%4x%2x%s",&Nb_Bytes,&First_Word,&Type,Data_Str);
				if (result != 4 && (result!=-1 && i>0) ) 
				{
					// fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
					printf("Line%6d :0-Error in hex file. result(%d) Length:%d \n", Record_Nb, result, i );

					iErrCount ++; // 2016.03.05


					// ---------------------------------
					if( iErrCount > MAX_ERR_COUNT*2 ) // 2020.06.30
					{
						printf("\n\n Too many parsing error.  Check Intel hexa family type!! \n");
					
						iErrCount = 0; // clear

						AllFilesClosed();

						exit(0);
						return 1;
					}
					// ---------------------------------

				}
			
				pBin = (char *) Data_Str;
	
				/* If we're reading the last record, ignore it. */
				switch (Type)
				{
				/* Data record */
				case 0: /* 16/20/32bit address offset */
					if (Nb_Bytes == 0) // Length 
						break;
	
					Address = First_Word;
	
					if (Seg_Lin_Select == SEGMENTED_ADDRESS)
					{
						Phys_Addr = (Segment << 4) + Address;
					}
					else
					{

				#if HEX2BIN_INTEL_ZERO_FORCED // 2020.06.29
						// ----------------------------------------
						// Hex Address Zero based converted 
						// ----------------------------------------
						if( HEX2BIN_ZERO_FORCED==Enable_HexaAddr_Zero_Forced )
						{
							if( Upper_Address>INTEL_BASE_ADDR_CMP )
							{
								Phys_AddrTemp = Upper_Address;
								Upper_Address = 0x0000;
								
								if(verbose)
								printf("Line%6d :%d: Upper_Addr: 0x%X to zero-based Addr: 0x%X \n", Record_Nb, Type, Phys_AddrTemp, Upper_Address );
							}
						}
				#endif

						/* LINEAR_ADDRESS or NO_ADDRESS_TYPE_SELECTED
						   Upper_Address = 0 as specified in the Intel spec. until an extended address
						   record is read. */
						Phys_Addr = ((Upper_Address << 16) + Address);
					}
	
					if (verbose) fprintf(stderr,"Line%6d :0:Physical Address: %#08x  ",Record_Nb, Phys_Addr);
	
					/* Floor address */
					if (Floor_Address_Setted)
					{
						/* Discard if lower than Floor_Address */
					  if (Phys_Addr < (Floor_Address - Starting_Address)) {
						if (verbose) fprintf(stderr,"Line%6d :0:Discard physical address less than %#08x \n",Record_Nb, Floor_Address - Starting_Address);
						break; 
					  }
					}
					/* Set the lowest address as base pointer. */
					if (Phys_Addr < Lowest_Address)
						Lowest_Address = Phys_Addr;
	
					/* Same for the top address. */
					temp = Phys_Addr + Nb_Bytes -1;
	
					/* Ceiling address */
					if (Ceiling_Address_Setted)
					{
						/* Discard if higher than Ceiling_Address */
						if (temp  > (Ceiling_Address +	Starting_Address)) {
						  if (verbose) fprintf(stderr,"Line%6d :0:Discard physical address more than %#08x \n", Record_Nb, Ceiling_Address + Starting_Address);
						  break;
						}
					}
					
					if (temp > Highest_Address)
					{
						Highest_Address = temp;
						
						Real_Length = Highest_Address; // added at 2016.03.05, for real file size
					}

					if (verbose) 
					{
						if(iVerbosType<2) fprintf(stderr,"Line%6d :0:Highest_Address: %#08x  \r", Record_Nb, Highest_Address);
						else fprintf(stderr,"Line%6d :0:Highest_Address: %#08x  \n", Record_Nb, Highest_Address);
					}
					break;
		
				case 1:
					if (verbose) 
					{
						ClearScreen();
						fprintf(stderr,"Line%6d :1:End of File record\n", Record_Nb);
					}
					break;
	
				case 2: // 20bit address
					/* First_Word contains the offset. It's supposed to be 0000 so
					   we ignore it. */
	
					/* First extended segment address record ? */
					if (Seg_Lin_Select == NO_ADDRESS_TYPE_SELECTED)
						Seg_Lin_Select = SEGMENTED_ADDRESS;
	
					/* Then ignore subsequent extended linear address records */
					if (Seg_Lin_Select == SEGMENTED_ADDRESS)
					{
						result = sscanf (pBin, "%4x%2x",&Segment,&temp2);
						if (result != 2) 
						{
							fprintf(stderr,"Line%6d :2:Error in hex file\n", Record_Nb);
							iErrCount ++; // 2016.03.05
						}
						
						if (verbose) 
						{
							ClearScreen();
							fprintf(stderr,"Line%6d :2:Extended Segment Address record: %#04x \n",Record_Nb, Segment);
						}
						
						/* Update the current address. */
						Phys_Addr = (Segment << 4);
					}
					else
					{
						fprintf(stderr,"Line%6d :2:Ignored extended linear address record\n", Record_Nb);
					}
					break;
	
				case 3: // 20bit address
					if (verbose) 
					{
						ClearScreen();
						fprintf(stderr,"Line%6d :3:Start Segment Address record: ignored\n", Record_Nb);
					}
					break;
	
				case 4: // 32bit address
					/* First_Word contains the offset. It's supposed to be 0000 so
					   we ignore it. */
	
					/* First extended linear address record ? */
					if (Seg_Lin_Select == NO_ADDRESS_TYPE_SELECTED)
						Seg_Lin_Select = LINEAR_ADDRESS;
	
					/* Then ignore subsequent extended segment address records */
					if (Seg_Lin_Select == LINEAR_ADDRESS)
					{
						result = sscanf (pBin, "%4x%2x",&Upper_Address,&temp2);
						if (result != 2) 
						{
							fprintf(stderr,"Line%6d :4:Error in hex file. \n", Record_Nb);
							iErrCount ++; // 2016.03.05
						}
						
						if (verbose) 
						{
							ClearScreen();
							printf("Line%6d :4:Extended Linear Address record: %#04x \n",Record_Nb, Upper_Address);
						}

						/* Update the current address. */
						Phys_Addr = (Upper_Address << 16);

					#if HEX2BIN_INTEL_ZERO_FORCED // 2020.06.29
						// ----------------------------------------
						// Hex Address Zero based converted 
						// ----------------------------------------
						if( HEX2BIN_ZERO_FORCED==Enable_HexaAddr_Zero_Forced )
						{
							
							//[2020/06/29;08:49:52.916] [dnw] :4: Extended Linear Address record: Upper_Addr:0x0800, Segment:0x0000 
							//[2020/06/29;08:49:52.932] [dnw] :4: Physical Address: 0x08000000 
				
							// :02 0000 04 0030 CA -> Extended Linear Address Record (32bit address format)
							// :02 0000 05	   -> Extended Linear Address Record (32bit address format)
							//
							// :02 0000 02	   -> Extended Linear Address Record (20bit address format)
							// :02 0000 03	   -> Extended Linear Address Record (20bit address format)
							//
							// :0E 0000 00 FF730332FFA480FF0F800FA00F409C
							// :00 0000 01 FF -> end-of-file recoed
				
							if( Upper_Address>INTEL_BASE_ADDR_CMP )
							{
								if( iOnceCheck )
								{
									iOnceCheck = 0;
									Phys_AddrTemp = Phys_Addr;
									Upper_Address = 0x0000;
									
									printf("Line%6d :%d:Upper_Address: 0x%X to zero-based addr: 0x%X \n", Record_Nb, Type, Phys_AddrTemp, Upper_Address );
								}
							}

							if( Phys_AddrTemp && (0==iOnceCheck) )
							{
								Phys_Addr -= Phys_AddrTemp;
							}

						}
					#endif

						if (verbose) 
						{
							ClearScreen();
							fprintf(stderr,"Line%6d :4:Physical Address              : %#08x \n", Record_Nb, Phys_Addr);
						}
					}
					else
					{
						ClearScreen();
						fprintf(stderr,"Line%6d :4:Ignored extended segment address record. \n", Record_Nb);
					}
					break;
	
				case 5: // 32bit address
					if (verbose) 
					{
						ClearScreen();
						fprintf(stderr,"Line%6d :5:Start Linear Address record   : ignored \n", Record_Nb );
					}
					break;
	
				default:
					if (verbose) 
					{
						ClearScreen();
						fprintf(stderr,"Line%6d :%d:Unknown Intel record type:%d \n", Record_Nb, Type, Type );
					}
					break;
				}
			}
		}
		while (!feof (inpfile));

		
		// ---------------------------------
		if( iErrCount > MAX_ERR_COUNT ) // 2016.03.05
		{
			printf("\n\nToo many Intel parsing error.  Check Intel hexa family type!!! \n");

			iErrCount = 0; // clear

			AllFilesClosed();

			exit(0);
			return 1;
		}
		// ---------------------------------

	
		if (Address_Alignment_Word)
			Highest_Address += (Highest_Address - Lowest_Address) + 1;
	
		Allocate_Memory_And_Rewind();
	
		Segment = 0;
		Upper_Address = 0;
		Record_Nb = 0;
	
		/* Read the file & process the lines. */
		do /* repeat until EOF(Filin) */
		{
			unsigned int i;
	

			memset( HexaLine, 0x00, sizeof(HexaLine) );
	
			/* Read a line from input file. */
			if( NULL == fgets( HexaLine, HEX_MAX_LINE_SIZE, inpfile ) )
			{
				break;
			}

			Record_Nb++;
	
			/* Remove carriage return/line feed at the end of line. */
			i = strlen(HexaLine);


			//fprintf(stderr,"Record: %d; length: %d\n", Record_Nb, i);
		#if 0
			if (--i != 0)
		#else
			if (--i > 0)
		#endif
			{
				if (HexaLine[i] == '\n') { HexaLine[i] = '\0'; }
				if (HexaLine[i] == '\r') { HexaLine[i] = '\0'; }
	
				/* Scan the first two bytes and nb of bytes.
				   The two bytes are read in First_Word since its use depend on the
				   record type: if it's an extended address record or a data record.
				*/
				memset( Data_Str, 0x00, sizeof(Data_Str) ); // added at 2016.03.09

				result = sscanf (HexaLine, ":%2x%4x%2x%s",&Nb_Bytes,&First_Word,&Type,Data_Str);
				if (result != 4) 
				{ 
					printf("Intel family : %4d line error. (%d) \n", Record_Nb, result );
				}
	
				Checksum = Nb_Bytes + (First_Word >> 8) + (First_Word & 0xFF) + Type;
	
				pBin = (char *) Data_Str;
	
				/* If we're reading the last record, ignore it. */
				switch (Type)
				{
				/* Data record */
				case 0:
					if (Nb_Bytes == 0)
					{
						//printf("0 byte length Data record ignored\n");
						break;
					}
	
					Address = First_Word;
	
					if (Seg_Lin_Select == SEGMENTED_ADDRESS)
						Phys_Addr = (Segment << 4) + Address;
					else
					{

				#if HEX2BIN_INTEL_ZERO_FORCED // 2020.06.29
						// ----------------------------------------
						// Hex Address Zero based converted 
						// ----------------------------------------
						if( HEX2BIN_ZERO_FORCED==Enable_HexaAddr_Zero_Forced )
						{
							if( Upper_Address>INTEL_BASE_ADDR_CMP )
							{
								Phys_AddrTemp = Upper_Address;
								Upper_Address = 0x0000;
								
								if(verbose)
								printf("Line%6d :%d: Upper_Addr:: 0x%X to zero-based Addr:: 0x%X \n", Record_Nb, Type, Phys_AddrTemp, Upper_Address );
							}
						}
				#endif
            	
						/* LINEAR_ADDRESS or NO_ADDRESS_TYPE_SELECTED
						   Upper_Address = 0 as specified in the Intel spec. until an extended address
						   record is read. */
						if (Address_Alignment_Word)
							Phys_Addr = ((Upper_Address << 16) + (Address << 1)) + Offset;
						else
							Phys_Addr = ((Upper_Address << 16) + Address);
					}

					/* Check that the physical address stays in the buffer's range. */
					if ((Phys_Addr >= Lowest_Address) && (Phys_Addr <= Highest_Address))
					{
						/* The memory block begins at Lowest_Address */
						Phys_Addr -= Lowest_Address;
	
						pBin = ReadDataBytes(pBin);
	
						/* Read the Checksum value. */
						result = sscanf (pBin, "%2x",&temp2);
						if (result != 1) fprintf(stderr,":0:Error in line %d of hex file\n", Record_Nb);
	
						/* Verify Checksum value. */
						Checksum = (Checksum + temp2) & 0xFF;
						VerifyChecksumValue();
					}
					else
					{
						if (Seg_Lin_Select == SEGMENTED_ADDRESS)
							fprintf(stderr,":0:Data record skipped at %#6x:%4x\n",Segment,Address);
						else
							fprintf(stderr,":0:Data record skipped at %#10x\n",Phys_Addr);
					}
	
					break;
	
				/* End of file record */
				case 1:
					/* Simply ignore checksum errors in this line. */
					break;
	
				/* Extended segment address record */
				case 2:
					/* First_Word contains the offset. It's supposed to be 0000 so
					   we ignore it. */
	
					/* First extended segment address record ? */
					if (Seg_Lin_Select == NO_ADDRESS_TYPE_SELECTED)
						Seg_Lin_Select = SEGMENTED_ADDRESS;
	
					/* Then ignore subsequent extended linear address records */
					if (Seg_Lin_Select == SEGMENTED_ADDRESS)
					{
						result = sscanf (pBin, "%4x%2x",&Segment,&temp2);
						if (result != 2) fprintf(stderr,":2:Error in line %d of hex file +++\n", Record_Nb);
	
						/* Update the current address. */
						Phys_Addr = (Segment << 4);
	
						/* Verify Checksum value. */
						Checksum = (Checksum + (Segment >> 8) + (Segment & 0xFF) + temp2) & 0xFF;
						VerifyChecksumValue();
					}
					break;
	
				/* Start segment address record */
				case 3:
					/* Nothing to be done since it's for specifying the starting address for
					   execution of the binary code */
					break;
	
				/* Extended linear address record */
				case 4:
					/* First_Word contains the offset. It's supposed to be 0000 so
					   we ignore it. */
	
					if (Address_Alignment_Word)
					{
						sscanf (pBin, "%4x",&Offset);
						Offset = Offset << 16;
						Offset -= Lowest_Address;
	
					}
					/* First extended linear address record ? */
					if (Seg_Lin_Select == NO_ADDRESS_TYPE_SELECTED)
						Seg_Lin_Select = LINEAR_ADDRESS;
	
					/* Then ignore subsequent extended segment address records */
					if (Seg_Lin_Select == LINEAR_ADDRESS)
					{
						result = sscanf (pBin, "%4x%2x",&Upper_Address,&temp2);
						if (result != 2) printf(":4:Error in line %d of hex file (result:%d) \n", Record_Nb, result);
	

					#if HEX2BIN_INTEL_ZERO_FORCED  // 2020.06.29
						// ----------------------------------------
						// Hex Address Zero based converted 
						// ----------------------------------------
						if( HEX2BIN_ZERO_FORCED==Enable_HexaAddr_Zero_Forced )
						{
							if( Upper_Address>INTEL_BASE_ADDR_CMP )
							{
								Phys_AddrTemp = Upper_Address;
								Upper_Address = 0x0000;
								
								if(verbose)
								printf("Line%6d :%d: Upper_Addr:*: 0x%X to zero-based Addr:*: 0x%X \n", Record_Nb, Type, Phys_AddrTemp, Upper_Address );
							}
						}
					#endif

						/* Update the current address. */
						Phys_Addr = (Upper_Address << 16);
	
						/* Verify Checksum value. */
						Checksum = (Checksum + (Upper_Address >> 8) + (Upper_Address & 0xFF) + temp2) & 0xFF;

					#if HEX2BIN_INTEL_ZERO_FORCED // 2020.06.29
						// ----------------------------------------
						// Hex Address Zero based converted 
						// ----------------------------------------
						if( HEX2BIN_ZERO_FORCED==Enable_HexaAddr_Zero_Forced )
						{
							Checksum = (Checksum + (Phys_AddrTemp >> 8) + (Phys_AddrTemp & 0xFF) ) & 0xFF;							
						}
					#endif
					
						VerifyChecksumValue();
					}
					break;
	
				/* Start linear address record */
				case 5:
					/* Nothing to be done since it's for specifying the starting address for
					   execution of the binary code */
					break;
				default:
					if (verbose) 
					{
						fprintf(stderr,"Intel Unknown record type (%d) \n", Type);
					}
					break;
				}
			}
		}
		while (!feof (inpfile));
		/*-----------------------------------------------------------------------------*/
	
		printf("Binary file start : 0x%08x \n", Lowest_Address);
		printf("Records start     : 0x%08x \n", Records_Start);
		printf("Highest address   : 0x%08x \n", Highest_Address);
		printf("Pad byte(def:0xff): 0x%02X \n", Pad_Byte);
	

	#if HEX2BIN_INTEL_ZERO_FORCED // 2016.03.10
		// ----------------------------------------
		// Hex Address Zero based converted 
		// ----------------------------------------
		if( HEX2BIN_ZERO_FORCED==Enable_HexaAddr_Zero_Forced )
		{
			if( Phys_AddrTemp )
			{
				printf("-------------------------------------------------------- \n");
				printf("Hex file real start addr : 0x%08X \n", Lowest_Address + Phys_AddrTemp );
				printf("Hex real Highest address : 0x%08X \n", Highest_Address + Phys_AddrTemp );
				printf("Hex real starting addr.  : 0x%08X \n", Starting_Address + Phys_AddrTemp );
				printf("-------------------------------------------------------- \n");
			}
		}
	#endif


		WriteMemory();
	
		if (Status_Checksum_Error && Enable_Checksum_Error)
		{
			printf("\n");
			printf("INTEL family haxa: Checksum error detected !!!\n");

			AllFilesClosed();

			exit(0);
			return 1;
		}
		else
		{
			printf("--------------------------------------------------------\n");
			printf("Parserd line number of hex record : %d lines \n", Record_Nb);
			printf("Converted the Intel hexa to binary. -- OK \n");

			AllFilesClosed();

			exit(0);
			return 1;
		}

	}
	
	else if(1 == isFloat2Hex)
	{
		size_t i;	 
		__float_hex_union fh_v;


		if( inpfile && outfile )
		{
			fprintf(outfile,"-------float_number-----------LSB_format---------MSB_format----- \r\n");

			i_readCount = 0;
			while( EOF != (c=fscanf(inpfile,"%s", &str_float)) )
			{
				i_readCount ++;

				f2h_float = atof( str_float );

				if( (TRUE==verbose) && 1==iVerbosType )
					printf("reading:[%-20s]   ->  using data:[%15.6f] \n", str_float , f2h_float );
		
				fprintf(outfile,"%15.6f  ->         ", f2h_float);
				fh_v.flt_val = (float)f2h_float;
				
				/* LSB */
				fprintf(outfile, " %02X-%02X-%02X-%02X        ", fh_v.flt_bin[3], fh_v.flt_bin[2], fh_v.flt_bin[1], fh_v.flt_bin[0] ); 
				/* MSB */
				fprintf(outfile, " %02X-%02X-%02X-%02X \r\n", fh_v.flt_bin[0], fh_v.flt_bin[1], fh_v.flt_bin[2], fh_v.flt_bin[3] ); 

			}

			AllFilesClosed();

		}	
		else
		{

			printf("-------float_number-----------LSB_format---------MSB_format----- \n");
			fh_v.flt_val = (float)f2h_float;


			printf("  %16.6f       -> ", f2h_float);

			/* LSB */
			printf(" %02X-%02X-%02X-%02X        ", fh_v.flt_bin[3], fh_v.flt_bin[2], fh_v.flt_bin[1], fh_v.flt_bin[0] ); 
			/* MSB */
			for ( i = 0; i < sizeof(fh_v.flt_bin); ++i )
			{		
				printf("%02X%c", fh_v.flt_bin[i],  i<(sizeof(fh_v.flt_bin)-1) ? '-' : '\n'); 
			}

		#if __NOT_USED___
			{
				double	pit_tmp, rol_tmp1, rol_tmp2;
				
				pit_tmp = asin( fh_v.flt_val ) * 180.0f/MATH_PI;
				//rol_tmp1 = asin( Cal_Yup.f_val / cos(ang_pitch) );
				//rol_tmp2 = acos( Cal_Zup.f_val / cos(ang_pitch) );
			
				printf("re-degree	 %.2f	  \n",  pit_tmp );
			}	
		#endif


		}
		

		opt_ok = 0; /* 2014.07.16 : NOTE !!! clear!!!  */

		if(i_readCount)
			printf("\nFLT>> Converted float number to hex-decimal!! Count:%d - OK", i_readCount);
		else			
			printf("\nFLT>> Converted float number to hex-decimal!! - OK");


	}

#if CONVERT_BMP2C
	else if(1 == isConvertBMP2C)
	{
    BITMAPFILEHEADER bf;
    BITMAPINFO bmi;

	PixelData *databuffer = NULL;
	DWORD bytes_read = 0, bmpsize, fillindex, nextlinei, linei, i, offset;
	int swaprgb = 0;
	int binout = 0;


		bmpsize = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight;
		databuffer = (PixelData *) malloc(sizeof(PixelData) * bmpsize);
		if(NULL == databuffer)
		{
			printf("Error allocating temporary data buffer, is image too big?\n");
			fclose(infile);
			exit(-1);
		}



		switch(iRGB)
		{
			case 888:
				break;

			case 444:
				break;

			case 555:
				break;

			case 565:
				break;


			default:
				break;
		}

		if(databuffer) free(databuffer);

		opt_ok = 0; /* 2014.07.16 : NOTE !!! clear!!!  */

		printf("\nBMP>> Converted BMP file to C text file - OK" );	


	}

	
	{  

		BITMAPFILEHEADER fileHeader;	// 비트맵 파일 헤더 구조체 변수
		BITMAPINFOHEADER infoHeader;	// 비트맵 정보 헤더 구조체 변수
	
		unsigned char *image;	 // 픽셀 데이터 포인터
		int size;				 // 픽셀 데이터 크기
		int width, height;		 // 비트맵 이미지의 가로, 세로 크기
		int padding;			 // 픽셀 데이터의 가로 크기가 4의 배수가 아닐 때 남는 공간의 크기
	
		// 각 픽셀을 표현할 ASCII 문자. 인덱스가 높을 수록 밝아지는 것을 표현
		char ascii[] = { '#', '#', '@', '%', '=', '+', '*', ':', '-', '.', ' ' };	// 11개
	
		fpBmp = fopen("Peppers80x80.bmp", "rb");	// 비트맵 파일을 바이너리 모드로 열기
		if (fpBmp == NULL)	  // 파일 열기에 실패하면
			return 0;		  // 프로그램 종료
	
		// 비트맵 파일 헤더 읽기. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료
		if (fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, inpfile) < 1)
		{
			fclose(inpfile);
			return 0;
		}
	
		// 매직 넘버가 MB가 맞는지 확인(2바이트 크기의 BM을 리틀 엔디언으로 읽었으므로 MB가 됨)
		// 매직 넘버가 맞지 않으면 프로그램 종료
		if (fileHeader.bfType != 'MB')
		{
			fclose(inpfile);
			return 0;
		}
	
		// 비트맵 정보 헤더 읽기. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료
		if (fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, inpfile) < 1)
		{
			fclose(inpfile);
			return 0;
		}
	
		// 24비트 비트맵이 아니면 프로그램 종료
		if (infoHeader.biBitCount != 24)
		{
			fclose(inpfile);
			return 0;
		}
	
		size = infoHeader.biSizeImage;	  // 픽셀 데이터 크기
		width = infoHeader.biWidth; 	  // 비트맵 이미지의 가로 크기
		height = infoHeader.biHeight;	  // 비트맵 이미지의 세로 크기
	
		// 이미지의 가로 크기에 픽셀 크기를 곱하여 가로 한 줄의 크기를 구하고 4로 나머지를 구함
		// 그리고 4에서 나머지를 빼주면 남는 공간을 구할 수 있음.
		// 만약 남는 공간이 0이라면 최종 결과가 4가 되므로 여기서 다시 4로 나머지를 구함
		padding = (PIXEL_ALIGN - ((width * PIXEL_SIZE) % PIXEL_ALIGN)) % PIXEL_ALIGN;
	
		if (size == 0)	  // 픽셀 데이터 크기가 0이라면
		{
			// 이미지의 가로 크기 * 픽셀 크기에 남는 공간을 더해주면 완전한 가로 한 줄 크기가 나옴
			// 여기에 이미지의 세로 크기를 곱해주면 픽셀 데이터의 크기를 구할 수 있음
			size = (width * PIXEL_SIZE + padding) * height;
		}
	
		image = malloc(size);	 // 픽셀 데이터의 크기만큼 동적 메모리 할당
	
		// 파일 포인터를 픽셀 데이터의 시작 위치로 이동
		fseek(inpfile, fileHeader.bfOffBits, SEEK_SET);
	
		// 파일에서 픽셀 데이터 크기만큼 읽음. 읽기에 실패하면 파일 포인터를 닫고 프로그램 종료
		if (fread(image, size, 1, inpfile) < 1)
		{
			fclose(inpfile);
			return 0;
		}
	
		fclose(inpfile);	  // 비트맵 파일 닫기
	
		outfile = fopen("ascii.txt", "w");	// 결과 출력용 텍스트 파일 열기
		if (outfile == NULL)	  // 파일 열기에 실패하면
		{
			free(image);	  // 픽셀 데이터를 저장한 동적 메모리 해제
			return 0;		  // 프로그램 종료
		}
	
		// 픽셀 데이터는 아래 위가 뒤집혀서 저장되므로 아래쪽부터 반복
		// 세로 크기만큼 반복
		for (int y = height - 1; y >= 0; y--)
		{
			// 가로 크기만큼 반복
			for (int x = 0; x < width; x++)
			{
				// 일렬로 된 배열에 접근하기 위해 인덱스를 계산
				// (x * 픽셀 크기)는 픽셀의 가로 위치
				// (y * (세로 크기 * 픽셀 크기))는 픽셀이 몇 번째 줄인지 계산
				// 남는 공간 * y는 줄별로 누적된 남는 공간
				int index = (x * PIXEL_SIZE) + (y * (width * PIXEL_SIZE)) + (padding * y);
	
				// 현재 픽셀의 주소를 RGBTRIPLE 포인터로 변환하여 RGBTRIPLE 포인터에 저장
				RGBTRIPLE *pixel = (RGBTRIPLE *)&image[index];
	
				// RGBTRIPLE 구조체로 파랑, 초록, 빨강값을 가져옴
				unsigned char blue = pixel->rgbtBlue;
				unsigned char green = pixel->rgbtGreen;
				unsigned char red = pixel->rgbtRed;
	
				// 파랑, 초록, 빨강값의 평균을 구하면 흑백 이미지를 얻을 수 있음
				unsigned char gray = (red + green + blue) / PIXEL_SIZE;
	
				// 흑백값에 ASCII 문자의 개수를 곱한 뒤 256으로 나누면 흑백값에 따라 
				// ASCII 문자의 인덱스를 얻을 수 있음
				char c = ascii[gray * sizeof(ascii) / 256];
	
				// 비트맵 이미지에서는 픽셀의 가로, 세로 크기가 똑같지만
				// 보통 ASCII 문자는 세로로 길쭉한 형태이므로 정사각형 모양과 비슷하게 보여주기 위해
				// 같은 문자를 두 번 저장해줌
				fprintf(outfile, "%c%c", c, c);	 // 텍스트 파일에 문자 출력
			}
	
			fprintf(outfile, "\n");	 // 가로 픽셀 저장이 끝났으면 줄바꿈을 해줌
		}
	
		fclose(outfile);	  // 텍스트 파일 닫기
	
		free(image);	  // 픽셀 데이터를 저장한 동적 메모리 해제
	
		return 0;
	}

#endif /// CONVERT_BMP2C


#if MD5_CHECKSUM_ENCIPHER /// 2014.06.27, MD5 Checksum
	else if(1 == isMD5)
	{
		/* Computes the message digest for a specified file.
		Prints out message digest, a space, the file name, and a carriage return.
		*/
		MD5_CTX mdContext;
		int nBytes;
		unsigned char md5_data[MD_HASH_BUFSIZ]; // 1024*10];
		struct	tm *pTime;

		pTime = current_time();

		if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile) /// 2014.08.01
		{
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "MD5 Hash is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
		}

		LOG_V("\n");

	#if MD5_MULTI_INPUT_FILES
		if( ASTERISK_FOUND == isAsteMode ) /* For ah.exe --input *.* */
		{
			int iIdx=0, iCount=0;
			unsigned __int64 	kll = 0UL;


			printf("MD5>> MD5 hashing for input files* \n");
			for(ii=0; ii<16*2; ii++) printf("-");  printf(" %d --\r\n", 16*2 );

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }


		#if 1 // 
			for(ii=iFirst; ii<=(argc-isAsteminus); ii++)
			{
				if( NULL != (inpfile = fopen( argv[ii], "rb")) ) 
				{
					/// initial ----
					memset( &mdContext, 0x00, sizeof(MD5_CTX) );
					memset( &md5_data, 0x00, sizeof(md5_data) );

					/// MD5 Calculate ----
					MD5Init (&mdContext);

					kll = 0UL;
					while ((nBytes = fread (md5_data, 1, sizeof(md5_data), inpfile)) > 0)
					{
						kll += nBytes;
						MD5Update (&mdContext, md5_data, nBytes);

						// LOG_V("\bMD5 Hashing for (%s) -> read : %lld Bytes \r", argv[ii], kll );
					}
					ClearScreen(); 
					
					MD5Final(&mdContext);
					MDPrint(&mdContext);

					if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
					{
						//printf ("  *%s*%s__(%llu) \r\n", str_hash, argv[ii], kll );
						printf ("  *%s*%s__(%s) \n", str_hash, argv[ii], commify(kll, str_cmBuf, 0) );
						if(outfile) fprintf(outfile,"  *%s*%s__(%llu) \n", str_hash, argv[ii], kll );
					}
					else
					{
						printf ("  *%s*%s \n", str_hash, argv[ii] );
						if(outfile) fprintf(outfile,"  *%s*%s \n", str_hash, argv[ii] );
					}

					iTotSize += kll;
					multifileindex ++;
				}
				
				if(inpfile) { fclose(inpfile); inpfile=NULL; }
				if(data_buf){ free(data_buf); data_buf=NULL; }

			}

			printf("\nMD5>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			AllFilesClosed();

		#endif
	
		}
		else if( multifileindex > 0 )
		{

			int iLenFile = 0;
			int iLenSub=0;
			iLenFile = strlen(infile_name);
			int iIdx=0, iCount=0;
			unsigned __int64 	kll = 0UL;


			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }


			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("MD5>>No file [%s] in current directory!!! [%s] \n", extfile_name, infile_name );
			}
			else
			{
				printf("MD5>> MD5 hashing for input files... \n");
				for(ii=0; ii<16*2; ii++) printf("-");  printf(" %d --\r\n", 16*2 );

				do {

					iLenSub = strlen(iFiles.name);

					memset( ttlfilename, 0x00, MAX_FILENAME_LEN );
					memcpy( ttlfilename, iFiles.name, iLenSub );
					

					if( iFiles.attrib & _A_SUBDIR ) /* Subdirectory.  Value: 0x10. */
					{

				#if 0
						printf("subdir -- [%s] ---- \r\n", iFiles.name);
						if( 0!=strcmp(iFiles.name,"..") || 0!=strcmp(iFiles.name,".") )
						{
							std::string s2 = dn;
							s2 += "\\";
							s2 += iFiles.name;
							r += insertDirectory(s2.c_str(),list);
						}
				#endif

					}
				#if 0
					else if( iFiles.attrib & _A_SYSTEM ) /* System file.  Not normally seen with the DIR command, unless the /A or /A:S option is used.  Value: 0x04. */
					{
						printf("SYSTEM ---- [%s] \r\n", iFiles.name);
					}
					else if( iFiles.attrib & _A_RDONLY ) /* Read-only.  File cannot be opened for writing, and a file with the same name cannot be created. Value: 0x01. */
					{
						printf("READONLY -- [%s] \r\n", iFiles.name);
					}
					else if( iFiles.attrib & _A_NORMAL ) /* Normal.  File can be read or written to without restriction. Value: 0x00. */
					{
						printf("NORMAL ---- [%s] \r\n", iFiles.name);
					}
					else if( iFiles.attrib & _A_HIDDEN ) /* Hidden file.  Not normally seen with the DIR command, unless the /AH option is used.  Returns information about normal files as well as files with this attribute.  Value: 0x02. */
					{
						printf("HIDDEN ---- [%s] \r\n", iFiles.name);
					}
					else if( iFiles.attrib & _A_ARCH )  /* Archive.  Set whenever the file is changed, and cleared by the BACKUP command. Value: 0x20. */
				#endif
					else
					{
						/// FileName
						// memcpy( mulfile_name[multifileindex], iFiles.name, iLenSub );
						/// File Size
						/// mulfile_size[multifileindex] = iFiles.size;

					#if 0 // 2020.06.08
						if( iFiles.size>>20 )
							printf("\n>>Input Files  : %s (%.3f MB)", mulfile_name[multifileindex], (iFiles.size/1024.0)/1024.0 );
						else if( iFiles.size>>10 )
							printf("\n>>Input Files  : %s (%.3f kB)", mulfile_name[multifileindex], (iFiles.size/1024.0) );
						else  
							printf("\n>>Input Files  : %s (%lu Byte)", mulfile_name[multifileindex], iFiles.size );
					#endif


						if( NULL == (inpfile = fopen( ttlfilename, "rb")) ) 
						{
							beep(700,100);
							printf("\n\n[++ERROR++] Can not open multi-input file (%s) \n", ttlfilename );

							AllFilesClosed();

							exit(0);
							return 0;
						}

						/// initial ----
						memset( &mdContext, 0x00, sizeof(MD5_CTX) );
						memset( &md5_data, 0x00, sizeof(md5_data) );

						/// MD5 Calculate ----
						MD5Init (&mdContext);

						kll = 0UL;
						while ((nBytes = fread (md5_data, 1, sizeof(md5_data), inpfile)) != 0)
						{
							kll += nBytes;
						
							MD5Update (&mdContext, md5_data, nBytes);

							// LOG_V("\bMD5 Hashing for (%s) -> read : %lld Bytes \r", ttlfilename, kll );
						}
						ClearScreen(); 
						
						MD5Final(&mdContext);
						MDPrint(&mdContext);


						if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
						{
							printf ("  *%s*%s__(%llu) \r\n", str_hash, iFiles.name, iFiles.size );
							if(outfile) fprintf(outfile,"  *%s*%s__(%llu) \r\n", str_hash, iFiles.name, iFiles.size );
						}
						else
						{
							printf ("  *%s*%s \r\n", str_hash, iFiles.name );
							if(outfile) fprintf(outfile,"  *%s*%s \r\n", str_hash, iFiles.name );
						}

						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }

						iTotSize += iFiles.size;

						multifileindex ++;
					}


					//if( multifileindex >= MULTI_IN_FILES_CNT )
					//{
					//	printf("\n\n>>Too many of Input files(%d).	Max number is %u. ", multifileindex, MULTI_IN_FILES_CNT );
					//	break;
					//}
				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );

				multifileindex--;

				if( iTotSize>>20 )
					printf("\nMD5>> Number of Input files : %d files (Total size: %.3f MB) ---", multifileindex, (iTotSize/1024.0)/1024.0 );
				else if( iTotSize>>10 )
					printf("\nMD5>> Number of Input files : %d files (Total size: %.3f kB) ---", multifileindex, (iTotSize/1024.0) );
				else
					printf("\nMD5>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			}

	
		}
		else
	#endif /// MD5_MULTI_INPUT_FILES
		{

			unsigned __int64 	kll = 0UL;

			printf("MD5>> MD5 hashing... \n");

			/// initial
			memset( &mdContext, 0x00, sizeof(MD5_CTX) );
			memset( &md5_data, 0x00, sizeof(md5_data) );


			MD5Init (&mdContext);

			kll = 0UL;
			while ((nBytes = fread (md5_data, 1, sizeof(md5_data), inpfile)) != 0)
			{
				kll += nBytes;
			
				MD5Update (&mdContext, md5_data, nBytes);

				// LOG_V("\bMD5 Hashing for (%s) -> read : %lld Bytes \r", infile_name, kll );
			}
			ClearScreen(); 
			
			MD5Final(&mdContext);
			MDPrint(&mdContext);

			if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
			{
				printf ("  *%s*%s__(%llu) \r\n", str_hash, infile_name, infile_size);
				if(outfile) fprintf(outfile,"  *%s*%s__(%llu) \r\n", str_hash, infile_name, infile_size );
			}
			else
			{
				printf ("  *%s*%s \r\n", str_hash, infile_name );
				if(outfile) fprintf(outfile,"  *%s*%s \r\n", str_hash, infile_name );
			}

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
			printf("\nMD5>> Calculated MD5 Hash Value - OK" );

		}

		opt_ok = 0; /* 2014.07.16 : NOTE !!! clear!!!  */

	}
#endif /// MD5_CHECKSUM_ENCIPHER


#if MD4_CHECKSUM_ENCIPHER /// 2014.07.26, MD4 Checksum
	else if(1 == isMD4)
	{
		/* Computes the message digest for a specified file.
		Prints out message digest, a space, the file name, and a carriage return.
		*/
		MD4_CTX mdContext;
		int nBytes;
		unsigned char md4_data[MD_HASH_BUFSIZ]; // 1024*16];
		unsigned char md4_output[MD4_DIGEST_LENGTH] = {0,};
		struct	tm *pTime; // 2020.07.15
	
		pTime = current_time();
		

		if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile ) /// 2014.08.01
		{
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "MD4 Hash is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
		}


		LOG_V("\n");

		if( ASTERISK_FOUND == isAsteMode ) /* For ah.exe --input *.* */
		{
			int iIdx=0, iCount=0;
			unsigned __int64 	kll = 0UL;


			printf("MD4>> MD4 hashing for input files* \n");
			for(ii=0; ii<MD4_DIGEST_LENGTH*2; ii++) printf("-");  printf(" %d --\r\n", MD4_DIGEST_LENGTH*2 );

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			for(ii=iFirst; ii<=(argc-isAsteminus); ii++)
			{
				if( NULL != (inpfile = fopen( argv[ii], "rb")) ) 
				{
					/// initial ----
					memset( &mdContext, 0x00, sizeof(MD4_CTX) );
					memset( &md4_data, 0x00, sizeof(md4_data) );
					memset( &md4_output, 0x00, sizeof(md4_output) );

					/// MD4 Calculate ----
					MD4Init (&mdContext);
					
					kll = 0UL;
					while ((nBytes = fread (md4_data, 1, sizeof(md4_data) /*1024*/, inpfile))> 0 )
					{
						kll += nBytes;
						MD4Update (&mdContext, md4_data, nBytes);

						// LOG_V("\bMD4 Hashing for (%s) -> read : %lld Bytes \r", argv[ii], kll );
					}
					ClearScreen(); 
					
					MD4Final(md4_output, &mdContext);
					MD4Print(md4_output);

					if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
					{
						printf ("  *%s*%s__(%llu) \r\n", str_hash, argv[ii], kll	);
						if(outfile) fprintf(outfile,"  *%s*%s__(%llu) \r\n", str_hash, argv[ii], kll	);
					}
					else
					{
						printf ("  *%s*%s \r\n", str_hash, argv[ii] );
						if(outfile) fprintf(outfile,"  *%s*%s \r\n", str_hash, argv[ii] );
					}
					
					iTotSize += kll;
				
					multifileindex ++;
				}
				
				if(inpfile) { fclose(inpfile); inpfile=NULL; }
				if(data_buf){ free(data_buf); data_buf=NULL; }

			}

			printf("\nMD4>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			AllFilesClosed();

		}
		else if( multifileindex > 0 )
		{
			int iLenFile = 0;
			int iLenSub=0;
			iLenFile = strlen(infile_name);
			int iIdx=0, iCount=0;
			unsigned __int64 	kll = 0UL;


			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("MD4>>No file [%s] in current directory!!! [%s] \n", extfile_name, infile_name );
			}
			else
			{
			
				printf("MD4>> MD4 hashing for input files... \n");
				for(ii=0; ii<MD4_DIGEST_LENGTH*2; ii++) printf("-");  printf(" %d --\r\n", MD4_DIGEST_LENGTH*2 );

				do {

					iLenSub = strlen(iFiles.name);
			
					if( iFiles.attrib & _A_SUBDIR )
					{

					}
					else
					{

						if( NULL == (inpfile = fopen( iFiles.name, "rb")) ) 
						{
							beep(700,100);
							printf("\n\nMD4>> [++ERROR++] Can not open multi-input file (%s) \n", iFiles.name );

							AllFilesClosed();

							exit(0);
							return 0;
						}

						/// initial ----
						memset( &mdContext, 0x00, sizeof(MD4_CTX) );
						memset( &md4_data, 0x00, sizeof(md4_data) );
						memset( &md4_output, 0x00, sizeof(md4_output) );

						/// MD4 Calculate ----
						MD4Init (&mdContext);

						kll = 0UL;
						while ((nBytes = fread (md4_data, 1, sizeof(md4_data) /*1024*/, inpfile)) != 0)
						{
							kll += nBytes;
						
							MD4Update (&mdContext, md4_data, nBytes);

							// LOG_V("\bMD4 Hashing for (%s) -> read : %lld Bytes \r", iFiles.name, kll );
						}
						ClearScreen(); 

						MD4Final(md4_output, &mdContext);
						MD4Print(md4_output);

						if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
						{
							printf ("  *%s*%s__(%llu) \r\n", str_hash, iFiles.name, iFiles.size	);
							if(outfile) fprintf(outfile,"  *%s*%s__(%llu) \r\n", str_hash, iFiles.name, iFiles.size	);
						}
						else
						{
							printf ("  *%s*%s \r\n", str_hash, iFiles.name );
							if(outfile) fprintf(outfile,"  *%s*%s \r\n", str_hash, iFiles.name );
						}
						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }

						iTotSize += iFiles.size;

						multifileindex ++;
					}

				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );

				multifileindex--;

				printf("\nMD4>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );
			
			}

		}
		else
		{
		
			unsigned __int64	kll = 0UL;

			printf("MD4>> MD4 hashing... \n");

			/// initial
			memset( &mdContext, 0x00, sizeof(MD4_CTX) );
			memset( &md4_data, 0x00, sizeof(md4_data) );
			memset( &md4_output, 0x00, sizeof(md4_output) );


			MD4Init (&mdContext);

			kll = 0UL;
			while ((nBytes = fread (md4_data, 1, sizeof(md4_data) /*1024*/, inpfile)) != 0)
			{
				kll += nBytes;

				MD4Update (&mdContext, md4_data, nBytes);

				// LOG_V("\bMD4 Hashing for (%s) -> read : %lld Bytes \r", infile_name, kll );
			}
			ClearScreen(); 

			MD4Final(md4_output, &mdContext);
			MD4Print(md4_output);

			if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
			{
				printf ("  *%s*%s__(%llu) \r\n", str_hash, infile_name, infile_size);
				if(outfile) fprintf(outfile,"  *%s*%s__(%llu) \r\n", str_hash, infile_name, infile_size);
			}
			else
			{
				printf ("  *%s*%s \r\n", str_hash, infile_name );
				if(outfile) fprintf(outfile,"  *%s*%s \r\n", str_hash, infile_name );
			}
			
			if(inpfile) { fclose(inpfile);	inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
			printf("\nMD4>> Calculated MD4 Hash Value - OK" );

		}

		opt_ok = 0; /* 2014.07.16 : NOTE !!! clear!!!  */

	}
#endif /// MD4_CHECKSUM_ENCIPHER


#if MD2_CHECKSUM_ENCIPHER /// 2014.07.29
	else if(1 == isMD2)
	{
		/* Computes the message digest for a specified file.
		Prints out message digest, a space, the file name, and a carriage return.
		*/
		MD2_CTX mdContext;
		size_t nBytes;
		unsigned char md2_data[MD_HASH_BUFSIZ]; // 1024*16];
		unsigned char md2_output[MD2_DIGEST_LENGTH] = {0,};
		int iLenSub=0;
		struct	tm *pTime;  // 2020.07.15
	
		pTime = current_time();	
		
		if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile ) /// 2014.08.01
		{
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "MD2 Hash is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
		}


		LOG_V("\n");
		if( ASTERISK_FOUND == isAsteMode ) /* For ah.exe --input *.* */
		{
			int iIdx=0, iCount=0;
			unsigned __int64 	kll = 0UL;

			printf("MD2>> MD2 hashing for input files* \n");
			for(ii=0; ii<MD2_DIGEST_LENGTH*2; ii++) printf("-");  printf(" %d --\r\n", MD2_DIGEST_LENGTH*2 );

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			for(ii=iFirst; ii<=(argc-isAsteminus); ii++)
			{
				if( NULL != (inpfile = fopen( argv[ii], "rb")) ) 
				{
					/// initial ----
					memset( &mdContext, 0x00, sizeof(MD2_CTX) );
					memset( &md2_data, 0x00, sizeof(md2_data) );
					memset( &md2_output, 0x00, sizeof(md2_output) );

					/// MD2 Calculate ----
					MD2_Init(&mdContext);
					
					kll = 0UL;
					while ((nBytes = fread (md2_data, 1, sizeof(md2_data), inpfile)) > 0)
					{
						kll += nBytes;
						MD2_Update(&mdContext, md2_data, nBytes);

						// LOG_V("\bMD2 Hashing for (%s) -> read : %lld Bytes \r", argv[ii], kll );
					}
					ClearScreen(); 
					
					MD2_Final( md2_output, &mdContext );
					MD2Print( md2_output );

					if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
					{
						printf ("  *%s*%s__(%llu) \n", str_hash, argv[ii], kll );
						if(outfile) fprintf(outfile,"  *%s*%s__(%llu) \n", str_hash, argv[ii], kll );
					}
					else
					{
						printf ("  *%s*%s \n", str_hash, argv[ii] );
						if(outfile) fprintf(outfile,"  *%s*%s \n", str_hash, argv[ii] );
					}
					
					if(inpfile) { fclose(inpfile); inpfile=NULL; }
					if(data_buf){ free(data_buf); data_buf=NULL; }

					iTotSize += kll;
					multifileindex ++;
				}
			}

			printf("\nMD2>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			AllFilesClosed();

		}
		else if( multifileindex > 0 )
		{
			unsigned __int64 	kll = 0UL;


			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("MD2>>No file [%s] in current directory!!! [%s] \n", extfile_name, infile_name );
			}
			else
			{

				printf("MD2>> MD2 hashing for input files... \n");
				//printf("--------------------------------------------------------\r\n" );
				for(ii=0; ii<MD2_DIGEST_LENGTH*2; ii++) printf("-");  printf(" %d --\r\n", MD2_DIGEST_LENGTH*2 );

				do {

					iLenSub = strlen(iFiles.name);

					if( iFiles.attrib & _A_SUBDIR )
					{

					}
					else
					{

						if( NULL == (inpfile = fopen( iFiles.name, "rb")) ) 
						{
							beep(700,100);
							printf("\n\nMD2>>[++ERROR++] Can not open multi-input file (%s) \n", iFiles.name );

							AllFilesClosed();

							exit(0);
							return 0;
						}
		
						/// initial ----
						memset( &mdContext, 0x00, sizeof(MD2_CTX) );
						memset( &md2_data, 0x00, sizeof(md2_data) );
						memset( &md2_output, 0x00, sizeof(md2_output) );
		
						/// MD2 Calculate ----
						MD2_Init(&mdContext);
						
						kll = 0UL;
						while ((nBytes = fread (md2_data, 1, sizeof(md2_data), inpfile)) != 0)
						{
							kll += nBytes;
							MD2_Update(&mdContext, md2_data, nBytes);

							// LOG_V("\bMD2 Hashing for (%s) -> read : %lld \r", iFiles.name, kll );
						}
						ClearScreen(); 

						MD2_Final( md2_output, &mdContext );
						MD2Print( md2_output );
		
						if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
						{
							printf ("  *%s*%s__(%llu) \n", str_hash, iFiles.name, iFiles.size );
							if(outfile) fprintf(outfile,"  *%s*%s__(%llu) \n", str_hash, iFiles.name, iFiles.size );
						}
						else
						{
							printf ("  *%s*%s \n", str_hash, iFiles.name );
							if(outfile) fprintf(outfile,"  *%s*%s \n", str_hash, iFiles.name );
						}
						
						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }

						iTotSize += iFiles.size;
						multifileindex ++;
					}

				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );

				multifileindex--;

				printf("\nMD2>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			}

		}
		else
		{

			unsigned __int64 	kll = 0UL;

			printf("MD2>> MD2 hashing... \n");

			/// initial
			memset( &mdContext, 0x00, sizeof(MD2_CTX) );
			memset( &md2_data, 0x00, sizeof(md2_data) );
			memset( &md2_output, 0x00, sizeof(md2_output) );

			MD2_Init(&mdContext);


			kll = 0UL;
			while ((nBytes = fread (md2_data, 1, sizeof(md2_data), inpfile)) != 0)
			{
				kll += nBytes;

				MD2_Update(&mdContext, md2_data, nBytes);

				// LOG_V("\bMD2 Hashing for (%s) -> read : %lld Bytes \r", infile_name, kll );
			}
			ClearScreen(); 

			MD2_Final( md2_output, &mdContext );
			MD2Print( md2_output );

			if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
			{
				printf ("  *%s*%s__(%llu) \r\n", str_hash, infile_name, infile_size );
				if(outfile) fprintf(outfile,"  *%s*%s__(%llu) \r\n", str_hash, infile_name, infile_size );
			}
			else
			{
				printf ("  *%s*%s  \r\n", str_hash, infile_name );
				if(outfile) fprintf(outfile,"  *%s*%s \r\n", str_hash, infile_name );
			}

			if(inpfile) { fclose(inpfile);	inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
			printf("\nMD2>> Calculated MD2 Hash Value - OK" );

		}

		opt_ok = 0; /* 2014.07.16 : NOTE !!! clear!!!  */

	}
#endif /// MD2_CHECKSUM_ENCIPHER

#if POLARSSL_SHA1_ENCIPHER
	else if(1 == isSHA1)
	{
		ShaBuffer SHA1output;
	    size_t nBytes;
	    sha1_context ctx;
		unsigned char sha1_buf[SHA_READ_BUFSIZ]; /// NERVER modified!!!!
		int iLenSub=0;
		struct	tm *pTime;  // 2020.07.15

		//unsigned char *sha1_buf;
		//sha1_buf = (unsigned char*)malloc( SHA_READ_BUFSIZ*sizeof(unsigned char) );
			
		pTime = current_time();

		if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile ) /// 2014.08.01
		{
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "SHA1 Hash is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
		}

		LOG_V("\n");
		
	#if SHA1_MULTI_INPUT_FILES
		if( ASTERISK_FOUND == isAsteMode ) /* For ah.exe --input *.* */
		{
			int iIdx=0, iCount=0;
			unsigned __int64 	kll = 0UL;


			printf("SHA1>> SHA1 hashing for input files* (%d) \n", argc);
			for(ii=0; ii<20*2; ii++) printf("-");  printf(" %d --\r\n", 20*2 );

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			for(ii=iFirst; ii<=(argc-isAsteminus); ii++)
			{
				if( NULL != (inpfile = fopen( argv[ii], "rb")) ) 
				{
					/// initial ----
					memset( &ctx, 0x00, sizeof(sha1_context) );
					memset( sha1_buf, 0x00, sizeof(sha1_buf) );

					/// SHA1 Calculate ----
					sha1_starts(&ctx);
	
					kll = 0UL;
					while((nBytes = fread(sha1_buf, 1, sizeof(sha1_buf), inpfile)) > 0)
					{
						kll += nBytes;
						sha1_update(&ctx, sha1_buf, (int)nBytes);

						// LOG_V("\bSHA1 Hashing for (%s) -> read : %lld Bytes \r", argv[ii], kll );
					}
					ClearScreen(); 

					sha1_finish(&ctx, SHA1output);
	
					memset(&ctx, 0, sizeof(sha1_context));

					if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
					{
						printf("%s  *%s*%s__(%llu) \r\n", SHA1output, str_hash, argv[ii], kll );
						if(outfile) fprintf(outfile,"%s  *%s*%s__(%llu) \r\n", SHA1output, str_hash, argv[ii], kll );
					}
					else
					{
						printf("%s  *%s*%s \r\n", SHA1output, str_hash, argv[ii] );
						if(outfile) fprintf(outfile,"%s  *%s*%s \r\n", SHA1output, str_hash, argv[ii] );
					}

					if(inpfile) { fclose(inpfile); inpfile=NULL; }
					if(data_buf){ free(data_buf); data_buf=NULL; }

					//if(sha1_buf) free(sha1_buf);

					iTotSize += kll;
					multifileindex ++;
				}
			}

			printf("\nSHA1>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			AllFilesClosed();

		}
		else if( multifileindex > 0 )
		{
			unsigned __int64 	kll = 0UL;
	
	
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

		#if 1 // 
			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("SHA1>>No file [%s] in current directory!!! [%s] \n", extfile_name, infile_name );
			}
			else
			{

				printf("SHA1>> SHA1 hashing for input files... \n");
				//printf("----------------------------------------------------------\r\n" );
				for(ii=0; ii<20*2; ii++) printf("-");  printf(" %d --\r\n", 20*2 );


				do {

					iLenSub = strlen(iFiles.name);

					if( iFiles.attrib & _A_SUBDIR )
					{

					}
					else
					{

						if( NULL == (inpfile = fopen( iFiles.name, "rb")) ) 
						{
							beep(700,100);
							printf("\n\nSHA1>>[++ERROR++] Can not open multi-input file (%s) \n", iFiles.name );

							AllFilesClosed();
							//if(sha1_buf) free(sha1_buf);

							exit(0);
							return 0;
						}
			
						/// initial ----
						memset( &ctx, 0x00, sizeof(sha1_context) );
						memset( sha1_buf, 0x00, sizeof(sha1_buf) );
		
		
						/// SHA1 Calculate ----
						sha1_starts(&ctx);
		
						while((nBytes = fread(sha1_buf, 1, sizeof(sha1_buf), inpfile)) > 0)
						{
							kll += nBytes;

							sha1_update(&ctx, sha1_buf, (int)nBytes);

							// LOG_V("\bSHA1 Hashing for (%s) -> read : %lld Bytes \r", iFiles.name, kll );
						}
						ClearScreen(); 

						sha1_finish(&ctx, SHA1output);
		
						memset(&ctx, 0, sizeof(sha1_context));

						if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
						{
							printf("%s  *%s*%s__(%llu) \r\n", SHA1output, str_hash, iFiles.name, iFiles.size  );
							if(outfile) fprintf(outfile,"%s  *%s*%s__(%llu) \r\n", SHA1output, str_hash, iFiles.name, iFiles.size  );
						}
						else
						{
							printf("%s  *%s*%s \r\n", SHA1output, str_hash, iFiles.name );
							if(outfile) fprintf(outfile,"%s  *%s*%s \r\n", SHA1output, str_hash, iFiles.name );
						}

						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }
						//if(sha1_buf) free(sha1_buf);

						iTotSize += iFiles.size;
						multifileindex ++;
					}

				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );
				// if(sha1_buf) free(sha1_buf);

				multifileindex--;

				printf("\nSHA1>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			}
		#endif

		}
		else
	#endif /// MD5_MULTI_INPUT_FILES
		{
			unsigned __int64	kll = 0UL;

			printf("SHA1>> SHA1 hashing... \n");
			
			/// initial
			memset( &ctx, 0x00, sizeof(sha1_context) );
			memset( sha1_buf, 0x00, sizeof(sha1_buf) );

			/// SHA1 Calculate ----
		    sha1_starts(&ctx);

			kll = 0UL;
		    while((nBytes = fread(sha1_buf, 1, sizeof(sha1_buf), inpfile)) > 0)
		    {
				kll += nBytes;

		        sha1_update(&ctx, sha1_buf, (int)nBytes);

				// LOG_V("\bSHA1 Hashing for (%s) -> read : %lld Bytes \r", infile_name, kll );
		    }
			ClearScreen(); 

		    sha1_finish(&ctx, SHA1output);

		    memset(&ctx, 0, sizeof(sha1_context));

			if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
			{
				printf("%s  *%s*%s__(%llu) \r\n", SHA1output, str_hash, infile_name, infile_size );
				if(outfile) fprintf(outfile,"%s  *%s*%s__(%llu) \r\n", SHA1output, str_hash, infile_name, infile_size );
			}
			else
			{
				printf("%s  *%s*%s \r\n", SHA1output, str_hash, infile_name );
				if(outfile) fprintf(outfile,"%s  *%s*%s \r\n", SHA1output, str_hash, infile_name );
			}

			if(inpfile) { fclose(inpfile);	inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
			//if(sha1_buf) free(sha1_buf);

			printf("\nSHA1>> Calculated SHA1 Hash Value - OK");

		}

		opt_ok = 0; /* 2014.07.16 : NOTE !!! clear!!!  */


	}
#endif /// POLARSSL_SHA1_ENCIPHER


#if SHA2_256_384_512
	else if(1 == isSHA256)
	{
		unsigned __int64 	kll=0UL, ll=0UL;
		SHA256_CTX 		ctx256;
		unsigned char	sha256_buf[SHA2_BUFLEN];
		int iLenSub=0;
		struct	tm *pTime;  // 2020.07.15
	
		pTime = current_time();

		if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile ) /// 2014.08.01
		{
	#if (BYTE_ORDER==LITTLE_ENDIAN)
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "SHA-256 Hash is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
	#else
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "SHA-256 Hash(BIG endian) is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
	#endif
		}


		LOG_V("\n");

	#if SHA2_MULTI_INPUT_FILES
		if( ASTERISK_FOUND == isAsteMode )  /* For ah.exe --input *.* */
		{
			int iIdx=0, iCount=0;
			unsigned __int64 	kll = 0UL;

			printf("SHA2>> SHA256 hashing for input files* \n");
			for(ii=0; ii<SHA256_DIGEST_LENGTH*2; ii++) printf("-");  printf(" %d --\r\n", SHA256_DIGEST_LENGTH*2 );

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			for(ii=iFirst; ii<=(argc-isAsteminus); ii++)
			{
				if( NULL != (inpfile = fopen( argv[ii], "rb")) ) 
				{
					/// initial ----
					memset( &ctx256, 0x00, sizeof(SHA256_CTX) );
					memset( sha256_buf, 0x00, sizeof(sha256_buf) );
					
					/// SHA2 Calculate ----
					SHA256_Init(&ctx256);
					
					kll = 0UL;
					while((ll = fread(sha256_buf, 1, sizeof(sha256_buf) /*SHA2_BUFLEN*/, inpfile)) > 0) 
					{
						kll += ll;
						SHA256_Update(&ctx256, (unsigned char*)sha256_buf, ll);

						// LOG_V("\bSHA-256 Hashing for (%s) -> read : %lld \r", argv[ii], kll );
					}
					ClearScreen(); 
					
					SHA256_End(&ctx256, sha256_buf);

					if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
					{
						printf("%s  *%s*%s__(%llu) \r\n", sha256_buf, str_hash, argv[ii], kll );
						if(outfile) fprintf(outfile,"%s  *%s*%s__(%llu) \r\n", sha256_buf, str_hash, argv[ii], kll );	
					}
					else
					{
						printf("%s  *%s*%s \r\n", sha256_buf, str_hash, argv[ii] );
						if(outfile) fprintf(outfile,"%s  *%s*%s \r\n", sha256_buf, str_hash, argv[ii] );	
					}
					
					if(inpfile) { fclose(inpfile); inpfile=NULL; }
					if(data_buf){ free(data_buf); data_buf=NULL; }
	
					iTotSize += kll;
					multifileindex ++;
				}
			}
			printf("\nSHA2>> SHA-256 Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			AllFilesClosed();

		}
		else if( multifileindex > 0 )
		{

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

	#if 1 // 
			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("SHA2>>No file [%s] in current directory!!! [%s] \n", extfile_name, infile_name );
			}
			else
			{

				printf("SHA2>> SHA-256 hashing for input files... \n");
				for(ii=0; ii<SHA256_DIGEST_LENGTH*2; ii++) printf("-");  printf(" %d --\r\n", SHA256_DIGEST_LENGTH*2 );

				do {

					iLenSub = strlen(iFiles.name);

					if( iFiles.attrib & _A_SUBDIR )
					{

					}
					else
					{

						if( NULL == (inpfile = fopen( iFiles.name, "rb")) ) 
						{
							beep(700,100);
							printf("\n\n[++ERROR++] Can not open multi-input file (%s) in SHA2-256. \n", iFiles.name );

							AllFilesClosed();
	
							exit(0);
							return 0;
						}
			
						/// initial ----
						memset( &ctx256, 0x00, sizeof(SHA256_CTX) );
						memset( sha256_buf, 0x00, sizeof(sha256_buf) );
						
						/// SHA2 Calculate ----
						SHA256_Init(&ctx256);
						
						kll = 0UL;
						while((ll = fread(sha256_buf, 1, SHA2_BUFLEN, inpfile)) > 0) 
						{
							kll += ll;
							SHA256_Update(&ctx256, (unsigned char*)sha256_buf, ll);

							// LOG_V("\bSHA-256 Hashing for (%s) -> read : %lld \r", iFiles.name, kll );
						}
						ClearScreen(); 

						SHA256_End(&ctx256, sha256_buf);

						if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
						{
							printf("%s  *SHA256*%s__(%llu) \r\n", sha256_buf, iFiles.name, iFiles.size );
							if(outfile) fprintf(outfile,"%s  *SHA256*%s__(%llu) \r\n", sha256_buf, iFiles.name, iFiles.size );	
						}
						else
						{
							printf("%s  *SHA256*%s \r\n", sha256_buf, iFiles.name );
							if(outfile) fprintf(outfile,"%s  *SHA256*%s \r\n", sha256_buf, iFiles.name );	
						}
						
						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }

						iTotSize += iFiles.size;
						multifileindex ++;
					}

				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );

				multifileindex--;

				if( iTotSize>>20 )
					printf("\nSHA2>> Number of Input files : %d files (Total size: %.3f MB) ---", multifileindex, (iTotSize/1024.0)/1024.0 );
				else if( iTotSize>>10 )
					printf("\nSHA2>> Number of Input files : %d files (Total size: %.3f kB) ---", multifileindex, (iTotSize/1024.0) );
				else
					printf("\nSHA2>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			}
	#endif

		}
		else
	#endif /// SHA2_MULTI_INPUT_FILES
		{
			printf("SHA2>> SHA-256 hashing... \n");


			/// initial
			memset( &ctx256, 0x00, sizeof(SHA256_CTX) );
			memset( sha256_buf, 0x00, sizeof(sha256_buf) );

			/// SHA2 Calculate ----
			SHA256_Init(&ctx256);

			kll = 0UL;
			while((ll = fread(sha256_buf, 1, SHA2_BUFLEN, inpfile)) > 0) 
			{
				kll += ll;
				SHA256_Update(&ctx256, (unsigned char*)sha256_buf, ll);

				// LOG_V("\bSHA-256 Hashing for (%s) -> read : %lld  \r", infile_name, kll );
			}
			ClearScreen(); 

			SHA256_End(&ctx256, sha256_buf);


			if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
			{
				printf("%s  *SHA256*%s__(%llu) \r\n", sha256_buf, infile_name, infile_size );
				if(outfile) fprintf(outfile,"%s  *SHA256*%s__(%llu) \r\n", sha256_buf, infile_name, infile_size );
			}
			else
			{
				printf("%s  *SHA256*%s \r\n", sha256_buf, infile_name );
				if(outfile) fprintf(outfile,"%s  *SHA256*%s \r\n", sha256_buf, infile_name );
			}
			
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
			printf("\nSHA2>> Calculated SHA-256 Hash Value - OK" );

		}	

		opt_ok = 0; /* 2014.07.16 : NOTE !!! clear!!!  */

	}
	else if(1 == isSHA384)
	{
		unsigned __int64 	kll=0, ll=0;
		SHA384_CTX 		ctx384;
		unsigned char	sha384_buf[SHA2_BUFLEN];
		int iLenSub=0;
		struct	tm *pTime;
	
		pTime = current_time();

		if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile) /// 2014.08.01
		{
	#if (BYTE_ORDER==LITTLE_ENDIAN)
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "SHA-384 Hash is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
	#else
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "SHA-384 Hash(BIG endian) is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
	#endif
		}	


		LOG_V("\n");

	#if SHA2_MULTI_INPUT_FILES
		if( ASTERISK_FOUND == isAsteMode ) /* For ah.exe --input *.* */
		{
			int iIdx=0, iCount=0;
			unsigned __int64 	kll = 0UL;


			printf("SHA2>> SHA-384 hashing for input files* \n");
			for(ii=0; ii<SHA384_DIGEST_LENGTH*2; ii++) printf("-");  printf(" %d --\r\n", SHA384_DIGEST_LENGTH*2 );

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			for(ii=iFirst; ii<=(argc-isAsteminus); ii++)
			{
				if( NULL != (inpfile = fopen( argv[ii], "rb")) ) 
				{
					/// initial ----
					memset( &ctx384, 0x00, sizeof(SHA384_CTX) );
					memset( sha384_buf, 0x00, sizeof(sha384_buf) );
					
					/// SHA2 Calculate ----
					SHA384_Init(&ctx384);
					
					kll = 0UL;
					while((ll = fread(sha384_buf, 1, sizeof(sha384_buf) /*SHA2_BUFLEN*/, inpfile)) > 0) 
					{
						kll += ll;
						SHA384_Update(&ctx384, (unsigned char*)sha384_buf, ll);

						// LOG_V("\bSHA-384 Hashing for (%s) -> read : %lld Bytes \r", argv[ii], kll );
					}
					ClearScreen(); 

					SHA384_End(&ctx384, sha384_buf);

					if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
					{
						printf("%s  *SHA384*%s__(%llu) \r\n", sha384_buf, argv[ii], kll );
						if(outfile) fprintf(outfile,"%s  *SHA384*%s__(%llu) \r\n", sha384_buf, argv[ii], kll );
					}
					else
					{
						printf("%s  *SHA384*%s \r\n", sha384_buf, argv[ii] );
						if(outfile) fprintf(outfile,"%s  *SHA384*%s \r\n", sha384_buf, argv[ii] );
					}

					if(inpfile) { fclose(inpfile); inpfile=NULL; }
					if(data_buf){ free(data_buf); data_buf=NULL; }
		
					iTotSize += kll;
					multifileindex ++;
				}
			}

			printf("\nSHA2>> SHA-384 Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			AllFilesClosed();
	
		}
		else if( multifileindex > 0 )
		{

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }


			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("SHA2>>No file [%s] in current directory!!! [%s] in SHA-384\n", extfile_name, infile_name );
			}
			else
			{

				printf("SHA2>> SHA-384 hashing for input files... \n");
				//printf("-------------------------------------------------------------\r\n" );
				for(ii=0; ii<SHA384_DIGEST_LENGTH*2; ii++) printf("-");  printf(" %d --\r\n", SHA384_DIGEST_LENGTH*2 );

				do {

					iLenSub = strlen(iFiles.name);

					if( iFiles.attrib & _A_SUBDIR )
					{

					}
					else
					{

						if( NULL == (inpfile = fopen( iFiles.name, "rb")) ) 
						{
							beep(700,100);
							printf("\n\n[++ERROR++] Can not open multi-input file (%s) in SHA2-384. \n", iFiles.name );

							AllFilesClosed();
	
							exit(0);
							return 0;
						}
			
						/// initial ----
						memset( &ctx384, 0x00, sizeof(SHA384_CTX) );
						memset( sha384_buf, 0x00, sizeof(sha384_buf) );
						
						/// SHA2 Calculate ----
						SHA384_Init(&ctx384);
						
						kll = 0;
						while((ll = fread(sha384_buf, 1, SHA2_BUFLEN, inpfile)) > 0) 
						{
							kll += ll;
							SHA384_Update(&ctx384, (unsigned char*)sha384_buf, ll);

							// LOG_V("\bSHA-384 Hashing for (%s) -> read : %lld \r", iFiles.name, kll );
						}
						ClearScreen(); 

						SHA384_End(&ctx384, sha384_buf);

						if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
						{
							printf("%s  *SHA384*%s__(%llu) \r\n", sha384_buf, iFiles.name, iFiles.size );
							if(outfile) fprintf(outfile,"%s  *SHA384*%s__(%llu) \r\n", sha384_buf, iFiles.name, iFiles.size );
						}
						else
						{
							printf("%s  *SHA384*%s \r\n", sha384_buf, iFiles.name );
							if(outfile) fprintf(outfile,"%s  *SHA384*%s \r\n", sha384_buf, iFiles.name );
						}
						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }
			
						iTotSize += iFiles.size;
						multifileindex ++;
					}

				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );

				multifileindex--;

				if( iTotSize>>20 )
					printf("\nSHA2>> Number of Input files : %d files (Total size: %.3f MB) ---", multifileindex, (iTotSize/1024.0)/1024.0 );
				else if( iTotSize>>10 )
					printf("\nSHA2>> Number of Input files : %d files (Total size: %.3f kB) ---", multifileindex, (iTotSize/1024.0) );
				else
					printf("\nSHA2>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			}

		}
		else
	#endif /// SHA2_MULTI_INPUT_FILES
		{
			printf("SHA2>> SHA-384 hashing... \n");


			/// initial ---
			memset( &ctx384, 0x00, sizeof(SHA384_CTX) );
			memset( sha384_buf, 0x00, sizeof(sha384_buf) );

			/// SHA2 Calculate ----
			SHA384_Init(&ctx384);

			kll = 0;
			while((ll = fread(sha384_buf, 1, SHA2_BUFLEN, inpfile)) > 0) 
			{
				kll += ll;
				SHA384_Update(&ctx384, (unsigned char*)sha384_buf, ll);

				// LOG_V("\bSHA-384 Hashing for (%s) -> read : %lld \r", infile_name, kll );
			}
			ClearScreen(); 

			SHA384_End(&ctx384, sha384_buf);

			if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
			{
				printf("%s  *SHA384*%s__(%llu) \r\n", sha384_buf, infile_name, infile_size );
				if(outfile) fprintf(outfile,"%s  *SHA384*%s__(%llu) \r\n", sha384_buf, infile_name, infile_size );
			}
			else
			{
				printf("%s  *SHA384*%s \r\n", sha384_buf, infile_name );
				if(outfile) fprintf(outfile,"%s  *SHA384*%s \r\n", sha384_buf, infile_name );
			}

			if(inpfile) { fclose(inpfile);	inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
			printf("\nSHA2>> Calculated SHA-384 Hash Value - OK" );

		}	

		opt_ok = 0; /* 2014.07.16 : NOTE !!! clear!!!  */

	}
	else if(1 == isSHA512)
	{
		unsigned __int64 	kll=0, ll=0;
		SHA512_CTX 		ctx512;
		unsigned char	sha512_buf[SHA2_BUFLEN];
		int iLenSub=0;
		struct	tm *pTime;  // 2020.07.15
	
		pTime = current_time();

	
		if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile) /// 2014.08.01
		{
	#if (BYTE_ORDER==LITTLE_ENDIAN)
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "SHA-512 Hash is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
	#else
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "SHA-512 Hash(BIG endian) is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
	#endif
		}


		LOG_V("\n");

	#if SHA2_MULTI_INPUT_FILES
		if( ASTERISK_FOUND == isAsteMode )  /* For ah.exe --input *.* */
		{
			int iIdx=0, iCount=0;
			unsigned __int64 	kll = 0UL;


			printf("SHA2>> SHA-512 hashing for input files* \n");
			for(ii=0; ii<SHA512_DIGEST_LENGTH*2; ii++) printf("-");  printf(" %d --\r\n", SHA512_DIGEST_LENGTH*2 );

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			for(ii=iFirst; ii<=(argc-isAsteminus); ii++)
			{
				if( NULL != (inpfile = fopen( argv[ii], "rb")) ) 
				{
					/// initial ----
					memset( &ctx512, 0x00, sizeof(SHA512_CTX) );
					memset( sha512_buf, 0x00, sizeof(sha512_buf) );
					
					/// SHA2 Calculate ----
					SHA512_Init(&ctx512);
					
					kll = 0;
					while((ll = fread(sha512_buf, 1, sizeof(sha512_buf) /*SHA2_BUFLEN*/, inpfile)) > 0) 
					{
						kll += ll;
						SHA512_Update(&ctx512, (unsigned char*)sha512_buf, ll);

						// LOG_V("\bSHA-512 Hashing for (%s) -> read : %lld Bytes \r", argv[ii], kll );
					}
					ClearScreen(); 

					SHA512_End(&ctx512, sha512_buf);

					if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
					{
						printf("%s  *SHA512*%s__(%llu) \r\n", sha512_buf, argv[ii], kll );
						if(outfile) fprintf(outfile,"%s  *SHA512*%s__(%llu) \r\n", sha512_buf, argv[ii], kll );
					}
					else
					{
						printf("%s  *SHA512*%s \r\n", sha512_buf, argv[ii] );
						if(outfile) fprintf(outfile,"%s  *SHA512*%s \r\n", sha512_buf, argv[ii] );
					}
					
					if(inpfile) { fclose(inpfile); inpfile=NULL; }
					if(data_buf){ free(data_buf); data_buf=NULL; }

					iTotSize += kll;
					multifileindex ++;
				}
			}

			printf("\nSHA2>> SHA-512 Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			AllFilesClosed();

		}
		else if( multifileindex > 0 )
		{
	
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("SHA2>>No file [%s] in current directory!!! [%s] in SHA2-512 \n", extfile_name, infile_name );
			}
			else
			{

				printf("SHA2>> SHA-512 hashing for input files... \n");
				//printf("-------------------------------------------------------------\r\n" );
				for(ii=0; ii<SHA512_DIGEST_LENGTH*2; ii++) printf("-");  printf(" %d --\r\n", SHA512_DIGEST_LENGTH*2 );
				
				do {

					iLenSub = strlen(iFiles.name);

					if( iFiles.attrib & _A_SUBDIR )
					{

					}
					else
					{

						if( NULL == (inpfile = fopen( iFiles.name, "rb")) ) 
						{
							beep(700,100);
							printf("\n\nSHA2>>[++ERROR++] Can not open multi-input file (%s) in SHA2-512. \n", iFiles.name );

							AllFilesClosed();
	
							exit(0);
							return 0;
						}
			
						/// initial ----
						memset( &ctx512, 0x00, sizeof(SHA512_CTX) );
						memset( sha512_buf, 0x00, sizeof(sha512_buf) );
						
						/// SHA2 Calculate ----
						SHA512_Init(&ctx512);
						
						kll = 0;
						while((ll = fread(sha512_buf, 1, SHA2_BUFLEN, inpfile)) > 0) 
						{
							kll += ll;
							SHA512_Update(&ctx512, (unsigned char*)sha512_buf, ll);

							// LOG_V("\bSHA-512 Hashing for (%s) -> read : %lld \r", iFiles.name, kll );
						}
						ClearScreen(); 

						SHA512_End(&ctx512, sha512_buf);

						if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
						{
							printf("%s  *SHA512*%s__(%llu) \r\n", sha512_buf, iFiles.name, iFiles.size );
							if(outfile) fprintf(outfile,"%s  *SHA512*%s__(%llu) \r\n", sha512_buf, iFiles.name, iFiles.size );
						}
						else
						{
							printf("%s  *SHA512*%s \r\n", sha512_buf, iFiles.name );
							if(outfile) fprintf(outfile,"%s  *SHA512*%s \r\n", sha512_buf, iFiles.name );
						}
						
						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }


						iTotSize += iFiles.size;
						multifileindex ++;
					}

				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );

				multifileindex--;

				if( iTotSize>>20 )
					printf("\nSHA2>> Number of Input files : %d files (Total size: %.3f MB) ---", multifileindex, (iTotSize/1024.0)/1024.0 );
				else if( iTotSize>>10 )
					printf("\nSHA2>> Number of Input files : %d files (Total size: %.3f kB) ---", multifileindex, (iTotSize/1024.0) );
				else
					printf("\nSHA2>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			}

		}
		else
	#endif /// SHA2_MULTI_INPUT_FILES
		{
			printf("SHA2>> SHA-512 hashing... \n");


			/// initial
			memset( &ctx512, 0x00, sizeof(SHA512_CTX) );
			memset( sha512_buf, 0x00, sizeof(sha512_buf) );


			/// SHA2 Calculate ----
			SHA512_Init(&ctx512);

			kll = 0;
			while((ll = fread(sha512_buf, 1, SHA2_BUFLEN, inpfile)) > 0) 
			{
				kll += ll;
				SHA512_Update(&ctx512, (unsigned char*)sha512_buf, ll);

				// LOG_V("\bSHA-512 Hashing for (%s) -> read : %lld \r", infile_name,  kll  );
			}
			ClearScreen(); 

			SHA512_End(&ctx512, sha512_buf);

			if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
			{
				printf("%s  *SHA512*%s__(%llu) \r\n", sha512_buf, infile_name, infile_size );
				if(outfile) fprintf(outfile,"%s  *SHA512*%s__(%llu) \r\n", sha512_buf, infile_name, infile_size );
			}
			else
			{
				printf("%s  *SHA512*%s \r\n", sha512_buf, infile_name  );
				if(outfile) fprintf(outfile,"%s  *SHA512*%s \r\n", sha512_buf, infile_name );
			}

			if(inpfile) { fclose(inpfile);	inpfile=NULL; } //			
			if(data_buf){ free(data_buf); data_buf=NULL; }
			printf("\nSHA2>> Calculated SHA-512 Hash Value - OK" );

		}	

		opt_ok = 0; /* 2014.07.16 : NOTE !!! clear!!!  */

	}
#endif /// SHA2_256_384_512

#if SHA2_224_CHECKSUM
	else if(1 == isSHA224)
	{
		unsigned __int64 	kll=0, ll=0;
		sha224_ctx      ctx224;		
		unsigned char	sha224_buf[SHA2_BUFLEN]; // 1024*10];
		int iLenSub=0;
		struct	tm *pTime;
	
		pTime = current_time();

		if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile ) /// 2014.08.01
		{
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "SHA-224 Hash is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
		}


		LOG_V("\n");

	#if SHA2_MULTI_INPUT_FILES
		if( ASTERISK_FOUND == isAsteMode )  /* For ah.exe --input *.* */
		{
			int iIdx=0, iCount=0;
			unsigned __int64 	kll = 0UL;


			printf("SHA2>> SHA-224 hashing for input files* \n");
			for(ii=0; ii<SHA224_DIGEST_SIZE*2; ii++) printf("-");  printf(" %d --\r\n", SHA224_DIGEST_SIZE*2 );

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			for(ii=iFirst; ii<=(argc-isAsteminus); ii++)
			{
				if( NULL != (inpfile = fopen( argv[ii], "rb")) ) 
				{
					/// initial ----
					memset( &ctx224, 0x00, sizeof(sha224_ctx) );
					memset( sha224_buf, 0x00, sizeof(sha224_buf) );
					
					/// SHA2 Calculate ----
					sha224_init(&ctx224);
							
					kll = 0UL;
					while((ll = fread(sha224_buf, 1, sizeof(sha224_buf), inpfile)) > 0) 
					{
						kll += ll;
						sha224_update(&ctx224, sha224_buf, ll);

						// LOG_V("\bSHA-224 Hashing for (%s) -> read : %lld Bytes \r", argv[ii], kll );
					}
					ClearScreen(); 

					sha224_final(&ctx224, sha224_buf);
					sha224Print (sha224_buf);

					if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
					{
						printf("  *SHA224*%s__(%llu) \r\n", argv[ii], kll );
						if(outfile) fprintf(outfile,"  *SHA224*%s__(%llu) \r\n", argv[ii], kll );
					}
					else
					{
						printf("  *SHA224*%s \r\n", argv[ii] );
						if(outfile) fprintf(outfile,"  *SHA224*%s \r\n", argv[ii] );
					}

					if(inpfile) { fclose(inpfile); inpfile=NULL; }
					if(data_buf){ free(data_buf); data_buf=NULL; }
	
					iTotSize += kll;
					multifileindex ++;
				}
			}
			printf("\nSHA2>> SHA-224 Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			AllFilesClosed();

		}
		else if( multifileindex > 0 )
		{
	
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

		#if 1 // 
			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("SHA2>>No file [%s] in current directory!!! [%s] in SHA-224 \n", extfile_name, infile_name );
			}
			else
			{
				printf("SHA2>> SHA-224 hashing for input files... \n");
				//printf("-------------------------------------------------------------\r\n" );
				for(ii=0; ii<SHA224_DIGEST_SIZE*2; ii++) printf("-");  printf(" %d --\r\n", SHA224_DIGEST_SIZE*2 );
				
				do {
					iLenSub = strlen(iFiles.name);

					if( iFiles.attrib & _A_SUBDIR )
					{

					}
					else
					{
						if( NULL == (inpfile = fopen( iFiles.name, "rb")) ) 
						{
							beep(700,100);
							printf("\n\nSHA2>> [++ERROR++] Can not open multi-input file (%s) in SHA2-224. \n", iFiles.name );

							AllFilesClosed();
	
							exit(0);
							return 0;
						}
			
						/// initial ----
						memset( &ctx224, 0x00, sizeof(sha224_ctx) );
						memset( sha224_buf, 0x00, sizeof(sha224_buf) );
						
						/// SHA2 Calculate ----
						sha224_init(&ctx224);
								
						kll = 0;
						while((ll = fread(sha224_buf, 1, sizeof(sha224_buf), inpfile)) > 0) 
						{
							kll += ll;
							sha224_update(&ctx224, sha224_buf, ll);

							// LOG_V("\bSHA-224 Hashing for (%s) -> read : %lld \r", iFiles.name, kll  );
						}
						ClearScreen(); 

						sha224_final(&ctx224, sha224_buf);
						sha224Print (sha224_buf);


						if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
						{
							printf("  *SHA224*%s__(%llu) \r\n", iFiles.name, iFiles.size );
							if(outfile) fprintf(outfile,"  *SHA224*%s__(%llu) \r\n", iFiles.name, iFiles.size );
						}
						else
						{
							printf("  *SHA224*%s \r\n", iFiles.name );
							if(outfile) fprintf(outfile,"  *SHA224*%s \r\n", iFiles.name );
						}

						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }
		
						iTotSize += iFiles.size;
						multifileindex ++;
					}

				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );

				multifileindex--;

				if( iTotSize>>20 )
					printf("\nSHA2>> Number of Input files : %d files (Total size: %.3f MB) ---", multifileindex, (iTotSize/1024.0)/1024.0 );
				else if( iTotSize>>10 )
					printf("\nSHA2>> Number of Input files : %d files (Total size: %.3f kB) ---", multifileindex, (iTotSize/1024.0) );
				else
					printf("\nSHA2>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			}
		#endif	

		}
		else
	#endif /// SHA2_MULTI_INPUT_FILES
		{
			printf("SHA2>> SHA-224 hashing... \n");


			/// initial ----
			memset( &ctx224, 0x00, sizeof(sha224_ctx) );
			memset( sha224_buf, 0x00, sizeof(sha224_buf) );


			/// SHA2 Calculate ----
			sha224_init(&ctx224);
					
			kll = 0;
			while((ll = fread(sha224_buf, 1, sizeof(sha224_buf), inpfile)) > 0) 
			{
				kll += ll;
				sha224_update(&ctx224, sha224_buf, ll);

				// LOG_V("\bSHA-224 Hashing for (%s) -> read : %lld \r", infile_name, kll );
			}
			ClearScreen(); 

			sha224_final(&ctx224, sha224_buf);
			sha224Print (sha224_buf);


			if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
			{
				printf("  *SHA224*%s__(%llu) \r\n", infile_name, infile_size );
				if(outfile) fprintf(outfile,"  *SHA224*%s__(%llu) \r\n", infile_name, infile_size );
			}			
			else
			{
				printf("  *SHA224*%s \r\n", infile_name );
				if(outfile) fprintf(outfile,"  *SHA224*%s \r\n", infile_name );
			}
			
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
			printf("\nSHA2>> Calculated SHA-224 Hash Value - OK" );

		}	
		
		opt_ok = 0; /* 2014.07.16 : NOTE !!! clear!!!  */

	}		
#endif


#if MD6_CHECKSUM_ENCIPHER
	else if(1 == isMD6)
	{
		uint64_t nBytes = 0UL, kll = 0UL;
		unsigned char md6_data[1024*10]; // MD_HASH_BUFSIZ
		double elapsed_time = end_time - start_time;
		uint64_t elapsed_ticks = end_ticks - start_ticks;
		int iLenSub=0;
		struct	tm *pTime;

		pTime = current_time();
		
		if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile ) /// 2014.08.01
		{
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "MD6 Hash is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
		}


		/* -------------------------------------------------- */
		/* ------ MD6 : set default md6 parameter settings ------ */
		md6_dgtLen = 256;           /* digest length */
		md6_keylen = 0;             /* key length in bytes (at most 64) */
		md6_modPar = 64;            /* mode parameter */
		md6_roundN = md6_default_r(md6_dgtLen,md6_keylen);  /* number of rounds */
		md6_use_default_r = 1;     /* 1 if r should be set to the default, 0 if r is explicitly provided */
		/* -------------------------------------------------- */

		LOG_V("\n");

		if( ASTERISK_FOUND == isAsteMode ) /* For ah.exe --input *.* */
		{
			int iIdx=0, iCount=0;
			unsigned __int64 	kll = 0UL;

			printf("MD6>> MD6 hashing for input files* \n");
			for(ii=0; ii<32*2; ii++) printf("-");  printf(" %d --\r\n", 32*2 );

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			for(ii=iFirst; ii<=(argc-isAsteminus); ii++)
			{
				if( NULL != (inpfile = fopen( argv[ii], "rb")) ) 
				{
					/// initial ----
					nBytes = 0UL;
					memset( md6_data, 0x00, sizeof(md6_data) );
	
					/// MD6 Calculate ----
					hash_init();
					
					kll = 0UL;
					while((nBytes = fread(md6_data, 1, sizeof(md6_data), inpfile)) > 0) 
					{
						kll += nBytes;
						hash_update(md6_data,nBytes*8);

						// LOG_V("\bMD6 Hashing for (%s) -> read : %lld Bytes \r", argv[ii], kll );
					}
					ClearScreen(); 

					hash_final();

					if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
					{
						printf("%s  *MD6*%s__(%llu) \r\n", md6_st.hexhashval, argv[ii], kll );
						if(outfile) fprintf(outfile,"%s  *MD6*%s__(%llu) \r\n", md6_st.hexhashval, argv[ii], kll );
					}		
					else
					{
						printf("%s  *MD6*%s \r\n", md6_st.hexhashval, argv[ii] );
						if(outfile) fprintf(outfile,"%s  *MD6*%s \r\n", md6_st.hexhashval, argv[ii] );
					}
					
					if(inpfile) { fclose(inpfile); inpfile=NULL; }
					if(data_buf){ free(data_buf); data_buf=NULL; }

					iTotSize += kll;
					multifileindex ++;
				}
			}
			printf("\nMD6>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			AllFilesClosed();

		}
		else if( multifileindex > 0 )
		{


			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

		#if 1 // 
			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("MD6>>No file [%s] in current directory!!! [%s] \n", extfile_name, infile_name );
			}
			else
			{

				printf("MD6>> MD6 hashing for input files... \n");
				//printf("--------------------------------------------------------\r\n" );
				for(ii=0; ii<32*2; ii++) printf("-");  printf(" %d --\r\n", 32*2 );

				do {

					iLenSub = strlen(iFiles.name);

					if( iFiles.attrib & _A_SUBDIR )
					{
					}
					else
					{

 						if( NULL == (inpfile = fopen( iFiles.name, "rb")) ) 
						{
							beep(700,100);
							printf("\n\n[++ERROR++] Can not open multi-input file (%s) in SHA2-512. \n", iFiles.name );

							AllFilesClosed();

							exit(0);
							return 0;
						}
		
						/// initial ----
						nBytes = 0UL;
						memset( md6_data, 0x00, sizeof(md6_data) );
		
		
						/// MD6 Calculate ----
						hash_init();
						
						kll = 0UL;
						while((nBytes = fread(md6_data, 1, sizeof(md6_data), inpfile)) > 0) 
						{
							kll += nBytes;
							hash_update(md6_data,nBytes*8);

							//LOG_V("\bMD6 Hashing for (%s) -> read : %lld Bytes \r", iFiles.name, kll );
						}
						//ClearScreen(); 

						hash_final();
		
				#if 0
						elapsed_time = end_time - start_time;
						printf("-- Elapsed time = %.3f seconds\n",elapsed_time);
						elapsed_ticks = end_ticks - start_ticks;
						printf("-- Total clock ticks = %lld\n",(long long int)elapsed_ticks);
				#endif
					
						/// encode(encfilename,argv[i]);
						/// print_hash(infile_name);


						if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
						{
							printf("%s  *MD6*%s__(%llu) \r\n", md6_st.hexhashval, iFiles.name, iFiles.size );
							if(outfile) fprintf(outfile,"%s  *MD6*%s__(%llu) \r\n", md6_st.hexhashval, iFiles.name, iFiles.size );
						}		
						else
						{
							printf("%s  *MD6*%s \r\n", md6_st.hexhashval, iFiles.name );
							if(outfile) fprintf(outfile,"%s  *MD6*%s \r\n", md6_st.hexhashval, iFiles.name );
						}
						
						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }
	
						iTotSize += iFiles.size;
						multifileindex ++;
					}

				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );

				multifileindex--;

				if( iTotSize>>20 )
					printf("\nMD6>> Number of Input files : %d files (Total size: %.3f MB) ---", multifileindex, (iTotSize/1024.0)/1024.0 );
				else if( iTotSize>>10 )
					printf("\nMD6>> Number of Input files : %d files (Total size: %.3f kB) ---", multifileindex, (iTotSize/1024.0) );
				else
					printf("\nMD6>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			}
		#endif

		}
		else
		{
			printf("MD6>> MD6 hashing... \n");

			/// initial
			nBytes = 0UL;
			memset( md6_data, 0x00, sizeof(md6_data) );


			/// MD6 Calculate ----
			hash_init();

			kll = 0UL;
			while((nBytes = fread(md6_data, 1, sizeof(md6_data), inpfile)) > 0) 
			{
				kll += nBytes;
				hash_update(md6_data,nBytes*8);

				// LOG_V("\bMD6 Hashing for (%s) -> read : %lld Bytes \r", infile_name, kll );
			}
			ClearScreen(); 

			hash_final();


			if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
			{
				printf("%s	*MD6*%s__(%llu)  \r\n", md6_st.hexhashval, infile_name, infile_size  );
				if(outfile) fprintf(outfile,"%s  *MD6*%s__(%llu) \r\n", md6_st.hexhashval, infile_name, infile_size );
			}
			else
			{
				printf("%s  *MD6*%s \r\n", md6_st.hexhashval, infile_name  );
				if(outfile) fprintf(outfile,"%s  *MD6*%s \r\n", md6_st.hexhashval, infile_name );
			}

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			printf("\nMD6>> Calculated MD6 Hash Value - OK" );

		}	

		opt_ok = 0; /* 2014.07.16 : NOTE !!! clear!!!  */

	}
#endif

#if SHA3_KECCAK_224_256_384_512 // SHA3, 2017.08.22
	else if(1 == isSHA3_KECCAK_224)
	{
		char	sha3Buf[SHA3_BUFLEN] = {0,};
		char	sha3digestTxt[SHA3_OUTPUT_SIZ] = {0,};
		uint8_t sha3out[SHA3_OUTPUT_SIZ] = { 0, };
		
		unsigned __int64 	kll=0;
		size_t		ll=0;
		int 		ret;
		int iLenSub=0;
	#if 1
		struct	tm *pTime;

		pTime = current_time();


		if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile) /// 2014.08.01
		{
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "SHA3-224 Hash is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
		}
	#endif

		LOG_V("\n");

		if( ASTERISK_FOUND == isAsteMode ) /* For ah.exe --input *.* */
		{
			int iIdx=0;
			unsigned __int64 	kll = 0UL;

			printf("HA3-KECCAK>> SHA3-224 hashing for input files* \n");
			for(ii=0; ii<SHA3_OUT_224*2; ii++) printf("-");  printf(" %d --\r\n", SHA3_OUT_224*2 );

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			for(ii=iFirst; ii<=(argc-isAsteminus); ii++)
			{
				if( NULL != (inpfile = fopen( argv[ii], "rb")) ) 
				{
					memset( sha3Buf, 0x00, sizeof(sha3Buf) );
					memset( sha3out, 0x00, sizeof(sha3out) );
					memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
					
					// Initialize the SHA3-224 context
					sha3_init(SHA3_224_HASH_BIT, SHA3_SHAKE_NONE);				
	
					kll = 0UL;
					while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
					{
						kll += ll;
						sha3_update(sha3Buf, ll);
						
						//Digest the message
						// LOG_V("\bSHA3-224 Hashing for (%s) -> read : %lld Bytes \r", argv[ii], kll );
					}
					ClearScreen(); 

					ret = sha3_final(sha3out, SHA3_OUT_224);
	
					for (iIdx = 0; iIdx < SHA3_OUT_224; iIdx++)
					{
						sprintf(&sha3digestTxt[iIdx*2], "%02x", sha3out[iIdx]);
					}

					if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
					{
						printf("%s  *SHA3-224*%s__(%llu) \r\n", sha3digestTxt, argv[ii], kll );
						if(outfile) fprintf(outfile, "%s  *SHA3-224*%s__(%llu) \r\n", sha3digestTxt, argv[ii], kll );
					}
					else
					{
						printf("%s  *SHA3-224*%s \r\n", sha3digestTxt, argv[ii] );
						if(outfile) fprintf(outfile, "%s  *SHA3-224*%s \r\n", sha3digestTxt, argv[ii] );
					}
					
					if(inpfile) { fclose(inpfile); inpfile=NULL; }
					if(data_buf){ free(data_buf); data_buf=NULL; }

					iTotSize += kll;
					multifileindex ++;
				}
			}
			
			printf("\nHA3-KECCAK>> SHA3-224 Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			AllFilesClosed();

		}
		else if( multifileindex > 0 )
		{
		

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

		#if 1 // 
			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("SHA3-KECCAK>>No file [%s] in current directory!!! [%s] in SHA3-224 \n", extfile_name, infile_name );
			}
			else
			{

				printf("SHA3-KECCAK>> SHA3-224 hashing for input files... \n");
				//printf("-------------------------------------------------------------\r\n" );
				for(ii=0; ii<SHA3_OUT_224*2; ii++) printf("-");  printf(" %d --\r\n", SHA3_OUT_224*2 );

				do {
					iLenSub = strlen(iFiles.name);

					if( iFiles.attrib & _A_SUBDIR )
					{

					}
					else
					{
						if( NULL == (inpfile = fopen( iFiles.name, "rb")) ) 
						{
							beep(700,100);
							printf("\n\n[++ERROR++] Can not open multi-input file (%s) in SHA3-224. \n", iFiles.name );

							AllFilesClosed();
	
							exit(0);
							return 0;
						}

						memset( sha3Buf, 0x00, sizeof(sha3Buf) );
						memset( sha3out, 0x00, sizeof(sha3out) );
						memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
						
						// Initialize the SHA3-224 context
						sha3_init(SHA3_224_HASH_BIT, SHA3_SHAKE_NONE);				
		
						kll = 0;
						while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
						{
							kll += ll;
		
							sha3_update(sha3Buf, ll);
							
							//Digest the message
							// LOG_V("\bSHA3-224 Hashing for (%s) -> read : %lld \r", iFiles.name,  kll );
						}
						ClearScreen(); 

						ret = sha3_final(sha3out, SHA3_OUT_224);
		
						for (ii = 0; ii < SHA3_OUT_224; ii++)
						{
							//printf("%02x", sha3out[ii]);
							sprintf(&sha3digestTxt[ii*2], "%02x", sha3out[ii]);
						}


						if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
						{
							printf("%s  *SHA3-224*%s__(%llu) \r\n", sha3digestTxt, iFiles.name, iFiles.size );
							if(outfile) fprintf(outfile, "%s  *SHA3-224*%s__(%llu) \r\n", sha3digestTxt, iFiles.name, iFiles.size );
						}
						else
						{
							printf("%s  *SHA3-224*%s \r\n", sha3digestTxt, iFiles.name  );
							if(outfile) fprintf(outfile, "%s  *SHA3-224*%s \r\n", sha3digestTxt, iFiles.name  );
						}
						
						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }

						iTotSize += iFiles.size;
						multifileindex ++;
					}

				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );

				multifileindex--;

				if( iTotSize>>20 )
					printf("\nSHA3-KECCAK>> Number of Input files : %d files (Total size: %.3f MB) ---", multifileindex, (iTotSize/1024.0)/1024.0 );
				else if( iTotSize>>10 )
					printf("\nSHA3-KECCAK>> Number of Input files : %d files (Total size: %.3f kB) ---", multifileindex, (iTotSize/1024.0) );
				else
					printf("\nSHA3-KECCAK>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			}
		#endif
		
		}
		else
		{
			printf("SHA3-KECCAK>> SHA3-224 hashing... \n");
		
		
			memset( sha3Buf, 0x00, sizeof(sha3Buf) );
			memset( sha3out, 0x00, sizeof(sha3out) );
			memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
			
			// Initialize the SHA3-256 context
			sha3_init(SHA3_224_HASH_BIT, SHA3_SHAKE_NONE);	

			kll = 0;
			while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
			{
				kll += ll;
			
				sha3_update(sha3Buf, ll);
				
				//Digest the message
				// LOG_V("\bSHA3-224 Hashing for (%s) -> read : %lld \r", infile_name, kll );
			}
			ClearScreen(); 

			ret = sha3_final(sha3out, SHA3_OUT_224);
			
			for (ii = 0; ii < SHA3_OUT_224; ii++)
			{
				sprintf(&sha3digestTxt[ii*2], "%02x", sha3out[ii]);
			}
			

			if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
			{
				printf("%s  *SHA3-224*%s__(%llu) \r\n", sha3digestTxt, infile_name, infile_size   );
				if(outfile) fprintf(outfile, "%s  *SHA3-224*%s__(%llu) \r\n", sha3digestTxt, infile_name, infile_size );
			}			
			else
			{
				printf("%s  *SHA3-224*%s \r\n", sha3digestTxt, infile_name );
				if(outfile) fprintf(outfile, "%s  *SHA3-224*%s \r\n", sha3digestTxt, infile_name );
			}

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
			printf("\nSHA3-KECCAK>> Calculated SHA3-224 Hash Value - OK" );
				
		}	
		
		opt_ok = 0; /* 2014.07.16 : NOTE !!! clear!!!  */

	}
	else if(1 == isSHA3_KECCAK_256)
	{
		char	sha3Buf[SHA3_BUFLEN] = {0,};
		char	sha3digestTxt[SHA3_OUTPUT_SIZ] = {0,};
		uint8_t sha3out[SHA3_OUTPUT_SIZ] = { 0, };
		
		unsigned __int64 	kll=0;
		size_t		ll=0;
		int 		ret;
		int iLenSub=0;
	#if 1
		struct	tm *pTime;

		pTime = current_time();

		if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile) /// 2014.08.01
		{
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "SHA3-256 Hash is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
		}		
	#endif

		LOG_V("\n");
	
		if( ASTERISK_FOUND == isAsteMode )  /* For ah.exe --input *.* */
		{
			int iIdx=0;
			unsigned __int64 	kll = 0UL;


			printf("SHA3-KECCAK>> SHA3-256 hashing for input files* \n");
			for(ii=0; ii<SHA3_OUT_256*2; ii++) printf("-");  printf(" %d --\r\n", SHA3_OUT_256*2 );

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			for(ii=iFirst; ii<=(argc-isAsteminus); ii++)
			{
				if( NULL != (inpfile = fopen( argv[ii], "rb")) ) 
				{
					memset( sha3Buf, 0x00, sizeof(sha3Buf) );
					memset( sha3out, 0x00, sizeof(sha3out) );
					memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
					
					// Initialize the SHA3-256 context
					sha3_init(SHA3_256_HASH_BIT, SHA3_SHAKE_NONE);				
	
					kll = 0UL;
					while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
					{
						kll += ll;
						sha3_update(sha3Buf, ll);
						
						//Digest the message
						// LOG_V("\bSHA3-256 Hashing for (%s) -> read : %lld Bytes \r", argv[ii], kll );
					}
					ClearScreen(); 

					ret = sha3_final(sha3out, SHA3_OUT_256);
	
					for (iIdx = 0; iIdx < SHA3_OUT_256; iIdx++)
					{
						//printf("%02x", sha3out[ii]);
						sprintf(&sha3digestTxt[iIdx*2], "%02x", sha3out[iIdx]);
					}
	
					if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
					{
						printf("%s  *SHA3-256*%s__(%llu) \r\n", sha3digestTxt, argv[ii], kll );
						if(outfile) fprintf(outfile, "%s  *SHA3-256*%s__(%llu) \r\n", sha3digestTxt, argv[ii], kll );
					}
					else
					{
						printf("%s  *SHA3-256*%s \r\n", sha3digestTxt, argv[ii]);
						if(outfile) fprintf(outfile, "%s  *SHA3-256*%s \r\n", sha3digestTxt, argv[ii] );
					}

					if(inpfile) { fclose(inpfile); inpfile=NULL; }
					if(data_buf){ free(data_buf); data_buf=NULL; }
				
					iTotSize += kll;
					multifileindex ++;
				}
			}
			printf("\nSHA3-KECCAK>> SHA3-256 Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			AllFilesClosed();
	
		}
		else if( multifileindex > 0 )
		{	
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

		#if 1 // 
			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("SHA3-KECCAK>>No file [%s] in current directory!!! [%s] in SHA3-256 \n", extfile_name, infile_name );
			}
			else
			{

				printf("SHA3-KECCAK>> SHA3-256 hashing for input files... \n");
				//printf("-------------------------------------------------------------\r\n" );
				for(ii=0; ii<SHA3_OUT_256*2; ii++) printf("-");  printf(" %d --\r\n", SHA3_OUT_256*2 );
				
				do {

					iLenSub = strlen(iFiles.name);

					if( iFiles.attrib & _A_SUBDIR )
					{

					}
					else
					{
						if( NULL == (inpfile = fopen( iFiles.name, "rb")) ) 
						{
							beep(700,100);
							printf("\n\n[++ERROR++] Can not open multi-input file (%s) in SHA3-256. \n", iFiles.name );

							AllFilesClosed();
	
							exit(0);
							return 0;
						}

						memset( sha3Buf, 0x00, sizeof(sha3Buf) );
						memset( sha3out, 0x00, sizeof(sha3out) );
						memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
						
						// Initialize the SHA3-256 context
						sha3_init(SHA3_256_HASH_BIT, SHA3_SHAKE_NONE);				
		
						kll = 0;
						while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
						{
							kll += ll;
		
							sha3_update(sha3Buf, ll);
							
							//Digest the message
							// LOG_V("\bSHA3-256 Hashing for (%s) -> read : %lld  \r", iFiles.name, kll );
						}
						ClearScreen(); 

						ret = sha3_final(sha3out, SHA3_OUT_256);
		
						for (ii = 0; ii < SHA3_OUT_256; ii++)
						{
							//printf("%02x", sha3out[ii]);
							sprintf(&sha3digestTxt[ii*2], "%02x", sha3out[ii]);
						}
		
						if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
						{
							printf("%s  *SHA3-256*%s__(%llu) \r\n", sha3digestTxt, iFiles.name, iFiles.size );
							if(outfile) fprintf(outfile, "%s  *SHA3-256*%s__(%llu) \r\n", sha3digestTxt, iFiles.name, iFiles.size );
						}
						else
						{
							printf("%s  *SHA3-256*%s \r\n", sha3digestTxt, iFiles.name );
							if(outfile) fprintf(outfile, "%s  *SHA3-256*%s \r\n", sha3digestTxt, iFiles.name  );
						}

						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }
					
						iTotSize += iFiles.size;
						multifileindex ++;
					}

				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );

				multifileindex--;

				if( iTotSize>>20 )
					printf("\nSHA3-KECCAK>> Number of Input files : %d files (Total size: %.3f MB) ---", multifileindex, (iTotSize/1024.0)/1024.0 );
				else if( iTotSize>>10 )
					printf("\nSHA3-KECCAK>> Number of Input files : %d files (Total size: %.3f kB) ---", multifileindex, (iTotSize/1024.0) );
				else
					printf("\nSHA3-KECCAK>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			}
		#endif
		
		}
		else
		{
			printf("SHA3-KECCAK>> SHA3-256 hashing... \n");
		
		
			memset( sha3Buf, 0x00, sizeof(sha3Buf) );
			memset( sha3out, 0x00, sizeof(sha3out) );
			memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
			
			// Initialize the SHA3-256 context
			sha3_init(SHA3_256_HASH_BIT, SHA3_SHAKE_NONE);	

			kll = 0;
			while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
			{
				kll += ll;
			
				sha3_update(sha3Buf, ll);
				
				//Digest the message
				// LOG_V("\bSHA3-256 Hashing for (%s) -> read : %lld  \r", infile_name, kll );
			}
			ClearScreen(); 

			ret = sha3_final(sha3out, SHA3_OUT_256);
			
			for (ii = 0; ii < SHA3_OUT_256; ii++)
			{
				sprintf(&sha3digestTxt[ii*2], "%02x", sha3out[ii]);
			}


			if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
			{
				printf("%s  *SHA3-256*%s__(%llu) \r\n", sha3digestTxt, infile_name, infile_size   );
				if(outfile) fprintf(outfile, "%s  *SHA3-256*%s__(%llu) \r\n", sha3digestTxt, infile_name, infile_size );
			}
			else
			{
				printf("%s  *SHA3-256*%s \r\n", sha3digestTxt, infile_name );
				if(outfile) fprintf(outfile, "%s  *SHA3-256*%s \r\n", sha3digestTxt, infile_name );
			}
			
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
			printf("\nSHA3-KECCAK>> Calculated SHA3-256 Hash Value - OK" );
				
		}	
		
		opt_ok = 0; /* 2014.07.16 : NOTE !!! clear!!!  */

	}
	else if(1 == isSHA3_KECCAK_384)
	{
		char	sha3Buf[SHA3_BUFLEN] = {0,};
		char	sha3digestTxt[SHA3_OUTPUT_SIZ] = {0,};
		uint8_t sha3out[SHA3_OUTPUT_SIZ] = { 0, };
		
		unsigned __int64 	kll=0;
		size_t		ll=0;
		int 		ret;
		int iLenSub=0;
		struct	tm *pTime;
	
		pTime = current_time();

		if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile) /// 2014.08.01
		{
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "SHA3-384 Hash is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
		}		

		LOG_V("\n");

		if( ASTERISK_FOUND == isAsteMode )  /* For ah.exe --input *.* */
		{
			int iIdx=0;
			unsigned __int64 	kll = 0UL;

			printf("SHA3-KECCAK>> SHA3-384  hashing for input files* \n");
			for(ii=0; ii<SHA3_OUT_384*2; ii++) printf("-");  printf(" %d --\r\n", SHA3_OUT_384*2 );

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			for(ii=iFirst; ii<=(argc-isAsteminus); ii++)
			{
				if( NULL != (inpfile = fopen( argv[ii], "rb")) ) 
				{
					memset( sha3Buf, 0x00, sizeof(sha3Buf) );
					memset( sha3out, 0x00, sizeof(sha3out) );
					memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
					
					// Initialize the SHA3-256 context
					sha3_init(SHA3_384_HASH_BIT, SHA3_SHAKE_NONE);				
	
					kll = 0UL;
					while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
					{
						kll += ll;	
						sha3_update(sha3Buf, ll);

						//Digest the message
						// LOG_V("\bSHA3-384 Hashing for (%s) -> read : %lld Bytes \r", argv[ii], kll );
					}
					ClearScreen(); 

					ret = sha3_final(sha3out, SHA3_OUT_384);
	
					for (iIdx = 0; iIdx < SHA3_OUT_384; iIdx++)
					{
						sprintf(&sha3digestTxt[iIdx*2], "%02x", sha3out[iIdx]);
					}

					if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
					{
						printf("%s  *SHA3-384*%s__(%llu) \r\n", sha3digestTxt, argv[ii], kll );
						if(outfile) fprintf(outfile, "%s  *SHA3-384*%s__(%llu) \r\n", sha3digestTxt, argv[ii], kll );
					}
					else
					{
						printf("%s  *SHA3-384*%s \r\n", sha3digestTxt, argv[ii] );
						if(outfile) fprintf(outfile, "%s  *SHA3-384*%s \r\n", sha3digestTxt, argv[ii] );
					}

					if(inpfile) { fclose(inpfile); inpfile=NULL; }
					if(data_buf){ free(data_buf); data_buf=NULL; }

					iTotSize += kll;
					multifileindex ++;
				}
			}
			
			printf("\nSHA3-KECCAK>> SHA3-384 Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			AllFilesClosed();
	
		}
		else if( multifileindex > 0 )
		{
		
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

#if 1 // 
			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("SHA3-KECCAK>>No file [%s] in current directory!!! [%s] in SHA3-384 \n", extfile_name, infile_name );
			}
			else
			{

				printf("SHA3-KECCAK>> SHA3-384 hashing for input files... \n");
				//printf("-------------------------------------------------------------\r\n" );
				for(ii=0; ii<SHA3_OUT_384*2; ii++) printf("-");  printf(" %d --\r\n", SHA3_OUT_384*2 );
				
				do {

					iLenSub = strlen(iFiles.name);

					if( iFiles.attrib & _A_SUBDIR )
					{

					}
					else
					{
						if( NULL == (inpfile = fopen( iFiles.name, "rb")) ) 
						{
							beep(700,100);
							printf("\n\n[++ERROR++] Can not open multi-input file (%s) in SHA3-384. \n", iFiles.name );

							AllFilesClosed();

							exit(0);
							return 0;
						}

						memset( sha3Buf, 0x00, sizeof(sha3Buf) );
						memset( sha3out, 0x00, sizeof(sha3out) );
						memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
						
						// Initialize the SHA3-256 context
						sha3_init(SHA3_384_HASH_BIT, SHA3_SHAKE_NONE);				
		
						kll = 0;
						while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
						{
							kll += ll;
		
							sha3_update(sha3Buf, ll);
							
							//Digest the message
							// LOG_V("\bSHA3-384 Hashing for (%s) -> read : %lld \r", iFiles.name, kll  );
						}
						ClearScreen(); 

						ret = sha3_final(sha3out, SHA3_OUT_384);
		
						for (ii = 0; ii < SHA3_OUT_384; ii++)
						{
							//printf("%02x", sha3out[ii]);
							sprintf(&sha3digestTxt[ii*2], "%02x", sha3out[ii]);
						}

						if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
						{
							printf("%s  *SHA3-384*%s__(%llu) \r\n", sha3digestTxt, iFiles.name, iFiles.size   );
							if(outfile) fprintf(outfile, "%s  *SHA3-384*%s__(%llu) \r\n", sha3digestTxt, iFiles.name, iFiles.size	);
						}
						else
						{
							printf("%s  *SHA3-384*%s \r\n", sha3digestTxt, iFiles.name );
							if(outfile) fprintf(outfile, "%s  *SHA3-384*%s \r\n", sha3digestTxt, iFiles.name );
						}

						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }

						iTotSize += iFiles.size;
						multifileindex ++;
					}

				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );

				multifileindex--;

				if( iTotSize>>20 )
					printf("\nSHA3-KECCAK>> Number of Input files : %d files (Total size: %.3f MB) ---", multifileindex, (iTotSize/1024.0)/1024.0 );
				else if( iTotSize>>10 )
					printf("\nSHA3-KECCAK>> Number of Input files : %d files (Total size: %.3f kB) ---", multifileindex, (iTotSize/1024.0) );
				else
					printf("\nSHA3-KECCAK>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			}
#endif
		
		}
		else
		{
			printf("SHA3-KECCAK>> SHA3-384 hashing... \n");
		
		
			memset( sha3Buf, 0x00, sizeof(sha3Buf) );
			memset( sha3out, 0x00, sizeof(sha3out) );
			memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
			
			// Initialize the SHA3-384 context
			sha3_init(SHA3_384_HASH_BIT, SHA3_SHAKE_NONE);	

			kll = 0;
			while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
			{
				kll += ll;
			
				sha3_update(sha3Buf, ll);
				
				//Digest the message
				// LOG_V("\bSHA3-384 Hashing for (%s) -> read : %lld \r", infile_name, kll );
			}
			ClearScreen(); 

			ret = sha3_final(sha3out, SHA3_OUT_384);
			
			for (ii = 0; ii < SHA3_OUT_384; ii++)
			{
				sprintf(&sha3digestTxt[ii*2], "%02x", sha3out[ii]);
			}
			

			if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
			{
				printf("%s  *SHA3-384*%s__(%llu) \r\n", sha3digestTxt, infile_name, infile_size   );
				if(outfile) fprintf(outfile, "%s  *SHA3-384*%s__(%llu) \r\n", sha3digestTxt, infile_name, infile_size );
			}			
			else
			{
				printf("%s  *SHA3-384*%s \r\n", sha3digestTxt, infile_name );
				if(outfile) fprintf(outfile, "%s  *SHA3-384*%s \r\n", sha3digestTxt, infile_name );
			}
			
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
			printf("\nSHA3-KECCAK>> Calculated SHA3-384 Hash Value - OK" );
				
		}	
		
		opt_ok = 0; /* 2014.07.16 : NOTE !!! clear!!!  */

	}
	else if(1 == isSHA3_KECCAK_512)
	{
		char	sha3Buf[SHA3_BUFLEN] = {0,};
		char	sha3digestTxt[SHA3_OUTPUT_SIZ] = {0,};
		uint8_t sha3out[SHA3_OUTPUT_SIZ] = { 0, };
		
		unsigned __int64 	kll=0;
		size_t		ll=0;
		int 		ret;
		int iLenSub=0;
		struct	tm *pTime;
	
		pTime = current_time();

		if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile) /// 2014.08.01
		{
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "SHA3-512 Hash is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
		}		

		LOG_V("\n");

		if( ASTERISK_FOUND == isAsteMode )  /* For ah.exe --input *.* */
		{
			int iIdx=0;
			unsigned __int64 	kll = 0UL;


			printf("SHA3-KECCAK>> SHA3-512 hashing for input files* \n");
			for(ii=0; ii<SHA3_OUT_512*2; ii++) printf("-");  printf(" %d --\r\n", SHA3_OUT_512*2 );

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			for(ii=iFirst; ii<=(argc-isAsteminus); ii++)
			{
				if( NULL != (inpfile = fopen( argv[ii], "rb")) ) 
				{
					memset( sha3Buf, 0x00, sizeof(sha3Buf) );
					memset( sha3out, 0x00, sizeof(sha3out) );
					memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
					
					// Initialize the SHA3-512 context
					sha3_init(SHA3_512_HASH_BIT, SHA3_SHAKE_NONE);				
	
					kll = 0UL;
					while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
					{
						kll += ll;
	
						sha3_update(sha3Buf, ll);
						
						//Digest the message
						// LOG_V("\bSHA3-512 Hashing for (%s) -> read : %lld Bytes \r", argv[ii], kll );
					}
					ClearScreen(); 

					ret = sha3_final(sha3out, SHA3_OUT_512);
	
					for (iIdx = 0; iIdx < SHA3_OUT_512; iIdx++)
					{
						sprintf(&sha3digestTxt[iIdx*2], "%02x", sha3out[iIdx]);
					}

					if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
					{
						printf("%s  *SHA3-512*%s__(%llu) \r\n", sha3digestTxt, argv[ii], kll );
						if(outfile) fprintf(outfile, "%s  *SHA3-512*%s__(%llu) \r\n", sha3digestTxt, argv[ii], kll);
					}
					else
					{
						printf("%s  *SHA3-512*%s \r\n", sha3digestTxt, argv[ii] );
						if(outfile) fprintf(outfile, "%s  *SHA3-512*%s \r\n", sha3digestTxt, argv[ii]);
					}

					if(inpfile) { fclose(inpfile); inpfile=NULL; }
					if(data_buf){ free(data_buf); data_buf=NULL; }

					iTotSize += kll;
					multifileindex ++;
				}
			}
			printf("\nSHA3-KECCAK>> SHA3-512 Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			AllFilesClosed();

		}
		else if( multifileindex > 0 )
		{
		
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

		#if 1 // 
			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("SHA3-KECCAK>>No file [%s] in current directory!!! [%s] in SHA3-512 \n", extfile_name, infile_name );
			}
			else
			{

				printf("SHA3-KECCAK>> SHA3-512 hashing for input files... \n");
				//printf("-------------------------------------------------------------\r\n" );
				for(ii=0; ii<SHA3_OUT_512*2; ii++) printf("-");  printf(" %d --\r\n", SHA3_OUT_512*2 );
				
				do {

					iLenSub = strlen(iFiles.name);

					if( iFiles.attrib & _A_SUBDIR )
					{

					}
					else
					{
						if( NULL == (inpfile = fopen( iFiles.name, "rb")) ) 
						{
							beep(700,100);
							printf("\n\n[++ERROR++] Can not open multi-input file (%s) in SHA3-512. \n", iFiles.name );

							AllFilesClosed();
	
							exit(0);
							return 0;
						}

						memset( sha3Buf, 0x00, sizeof(sha3Buf) );
						memset( sha3out, 0x00, sizeof(sha3out) );
						memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
						
						// Initialize the SHA3-512 context
						sha3_init(SHA3_512_HASH_BIT, SHA3_SHAKE_NONE);				
		
						kll = 0;
						while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
						{
							kll += ll;
		
							sha3_update(sha3Buf, ll);
							
							//Digest the message
							// LOG_V("\bSHA3-512 Hashing for (%s) -> read : %lld \r", iFiles.name, kll );
						}
						ClearScreen(); 

						ret = sha3_final(sha3out, SHA3_OUT_512);
		
						for (ii = 0; ii < SHA3_OUT_512; ii++)
						{
							sprintf(&sha3digestTxt[ii*2], "%02x", sha3out[ii]);
						}

						
						if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
						{
							printf("%s  *SHA3-512*%s__(%llu) \r\n", sha3digestTxt, iFiles.name, iFiles.size  );
							if(outfile) fprintf(outfile, "%s  *SHA3-512*%s__(%llu) \r\n", sha3digestTxt, iFiles.name, iFiles.size );
						}
						else
						{
							printf("%s  *SHA3-512*%s \r\n", sha3digestTxt, iFiles.name );
							if(outfile) fprintf(outfile, "%s  *SHA3-512*%s \r\n", sha3digestTxt, iFiles.name );
						}

						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }

						iTotSize += iFiles.size;
						multifileindex ++;
					}

				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );

				multifileindex--;

				if( iTotSize>>20 )
					printf("\nSHA3-KECCAK>> Number of Input files : %d files (Total size: %.3f MB) ---", multifileindex, (iTotSize/1024.0)/1024.0 );
				else if( iTotSize>>10 )
					printf("\nSHA3-KECCAK>> Number of Input files : %d files (Total size: %.3f kB) ---", multifileindex, (iTotSize/1024.0) );
				else
					printf("\nSHA3-KECCAK>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			}
		#endif
		
		}
		else
		{
			printf("SHA3-KECCAK>> SHA3-512 hashing... \n");
		
		
			memset( sha3Buf, 0x00, sizeof(sha3Buf) );
			memset( sha3out, 0x00, sizeof(sha3out) );
			memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
			
			// Initialize the SHA3-512 context
			sha3_init(SHA3_512_HASH_BIT, SHA3_SHAKE_NONE);	

			kll = 0;
			while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
			{
				kll += ll;
			
				sha3_update(sha3Buf, ll);
				
				//Digest the message
				// LOG_V("\bSHA3-512 Hashing for (%s) -> read : %lld \r", infile_name, kll );
			}
			ClearScreen(); 

			ret = sha3_final(sha3out, SHA3_OUT_512);
			
			for (ii = 0; ii < SHA3_OUT_512; ii++)
			{
				sprintf(&sha3digestTxt[ii*2], "%02x", sha3out[ii]);
			}
			

			if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
			{
				printf("%s  *SHA3-512*%s__(%llu) \r\n", sha3digestTxt, infile_name, infile_size   );
				if(outfile) fprintf(outfile, "%s  *SHA3-512*%s__(%llu) \r\n", sha3digestTxt, infile_name, infile_size );
			}			
			else
			{
				printf("%s  *SHA3-512*%s \r\n", sha3digestTxt, infile_name );
				if(outfile) fprintf(outfile, "%s  *SHA3-512*%s \r\n", sha3digestTxt, infile_name );
			}

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
			printf("\nSHA3-KECCAK>> Calculated SHA3-512 Hash Value - OK" );
				
		}	
		
		opt_ok = 0; /* 2014.07.16 : NOTE !!! clear!!!  */

	}
	else if(1 == isShake128)
	{
		char	sha3Buf[SHA3_BUFLEN] = {0,};
		char	sha3digestTxt[SHA3_OUTPUT_SIZ] = {0,};
		uint8_t sha3out[SHA3_OUTPUT_SIZ] = { 0, };
		
		unsigned __int64 	kll=0;
		size_t		ll=0;
		int 		ret;
		int iLenSub=0;
	#if 1
		struct	tm *pTime;
	
		pTime = current_time();
		
		if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile) /// 2014.08.01
		{
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "SHAKE128 Hash is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
		}		
	#endif

		LOG_V("\n");

		if( ASTERISK_FOUND == isAsteMode )  /* For ah.exe --input *.* */
		{
			int iIdx=0;
			unsigned __int64 	kll = 0UL;


			printf("SHAKE128>> SHAKE128 hashing for input files* \n");
			for(ii=0; ii<SHAKE_OUT_128*2; ii++) printf("-");  printf(" %d --\r\n", SHAKE_OUT_128*2 );

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			for(ii=iFirst; ii<=(argc-isAsteminus); ii++)
			{
				if( NULL != (inpfile = fopen( argv[ii], "rb")) ) 
				{
					memset( sha3Buf, 0x00, sizeof(sha3Buf) );
					memset( sha3out, 0x00, sizeof(sha3out) );
					memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
					
					// Initialize the Shake128 context
					sha3_init(SHAKE_128_HASH_BIT, SHA3_SHAKE_USE);	
	
					kll = 0UL;
					while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
					{
						kll += ll;
						sha3_update(sha3Buf, ll);
						
						//Digest the message
						// LOG_V("\bSHAKE128 Hashing for (%s) -> read : %lld Bytes \r", argv[ii], kll );
					}
					ClearScreen(); 

					ret = sha3_final(sha3out, SHAKE_OUT_128);
					
					for (iIdx = 0; iIdx < SHAKE_OUT_128; iIdx++)
					{
						sprintf(&sha3digestTxt[iIdx*2], "%02x", sha3out[iIdx]);
					}

					if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
					{
						printf("%s  *SHAKE128*%s__(%llu) \r\n", sha3digestTxt, argv[ii], kll );
						if(outfile) fprintf(outfile, "%s  *SHAKE128*%s__(%llu) \r\n", sha3digestTxt, argv[ii], kll );
					}
					else
					{
						printf("%s  *SHAKE128*%s \r\n", sha3digestTxt, argv[ii] );
						if(outfile) fprintf(outfile, "%s  *SHAKE128*%s \r\n", sha3digestTxt, argv[ii] );
					}

					if(inpfile) { fclose(inpfile); inpfile=NULL; }
					if(data_buf){ free(data_buf); data_buf=NULL; }

					iTotSize += kll;
					multifileindex ++;
				}
			}
			printf("\nSHAKE128>> SHAKE128 Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			AllFilesClosed();
	
		}
		else if( multifileindex > 0 )
		{
		
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

	#if 1 // 
			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("SHAKE128>>No file [%s] in current directory!!! [%s] \n", extfile_name, infile_name );
			}
			else
			{

				printf("SHAKE128>> SHAKE128 hashing for input files... \n");
				//printf("-------------------------------------------------------------\r\n" );
				for(ii=0; ii<SHAKE_OUT_128*2; ii++) printf("-");  printf(" %d --\r\n", SHAKE_OUT_128*2 );
				
				do {
					iLenSub = strlen(iFiles.name);

					if( iFiles.attrib & _A_SUBDIR )
					{

					}
					else
					{
						if( NULL == (inpfile = fopen( iFiles.name, "rb")) ) 
						{
							beep(700,100);
							printf("\n\n[++ERROR++] Can not open multi-input file (%s) in SHAKE128. \n", iFiles.name );

							AllFilesClosed();
	
							exit(0);
							return 0;
						}
		
						memset( sha3Buf, 0x00, sizeof(sha3Buf) );
						memset( sha3out, 0x00, sizeof(sha3out) );
						memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
						
						// Initialize the Shake128 context
						sha3_init(SHAKE_128_HASH_BIT, SHA3_SHAKE_USE);	
		
						kll = 0;
						while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
						{
							kll += ll;
		
							sha3_update(sha3Buf, ll);
							
							//Digest the message
							// LOG_V("\bSHAKE128 Hashing for (%s) -> read : %lld \r", iFiles.name, kll );
						}
						ClearScreen(); 

						ret = sha3_final(sha3out, SHAKE_OUT_128);
						
						for (ii = 0; ii < SHAKE_OUT_128; ii++)
						{
							sprintf(&sha3digestTxt[ii*2], "%02x", sha3out[ii]);
						}

						
						if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
						{
							printf("%s  *SHAKE128*%s__(%llu) \r\n", sha3digestTxt, iFiles.name, iFiles.size );
							if(outfile) fprintf(outfile, "%s  *SHAKE128*%s__(%llu) \r\n", sha3digestTxt, iFiles.name, iFiles.size );
						}
						else
						{
							printf("%s  *SHAKE128*%s \r\n", sha3digestTxt, iFiles.name );
							if(outfile) fprintf(outfile, "%s  *SHAKE128*%s \r\n", sha3digestTxt, iFiles.name );
						}

						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }

						iTotSize += iFiles.size;
						multifileindex ++;
					}

				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );

				multifileindex--;

				if( iTotSize>>20 )
					printf("\nSHAKE128>> Number of Input files : %d files (Total size: %.3f MB) ---", multifileindex, (iTotSize/1024.0)/1024.0 );
				else if( iTotSize>>10 )
					printf("\nSHAKE128>> Number of Input files : %d files (Total size: %.3f kB) ---", multifileindex, (iTotSize/1024.0) );
				else
					printf("\nSHAKE128>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			}
	#endif
		
		}
		else
		{
			printf("SHAKE128>> SHAKE128 hashing... \n");
		
		
			memset( sha3Buf, 0x00, sizeof(sha3Buf) );
			memset( sha3out, 0x00, sizeof(sha3out) );
			memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
			
			// Initialize the SHAKE128 context
			sha3_init(SHAKE_128_HASH_BIT, SHA3_SHAKE_USE);	

			kll = 0;
			while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
			{
				kll += ll;
			
				sha3_update(sha3Buf, ll);
				
				//Digest the message
				// LOG_V("\bSHAKE128 Hashing for (%s) -> read : %lld \r", infile_name, kll );
			}
			ClearScreen(); 

			ret = sha3_final(sha3out, SHAKE_OUT_128);
			
			for (ii = 0; ii < SHAKE_OUT_128; ii++)
			{
				sprintf(&sha3digestTxt[ii*2], "%02x", sha3out[ii]);
			}
				

			if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
			{
				printf("%s  *SHAKE128*%s__(%llu) \r\n", sha3digestTxt, infile_name, infile_size   );
				if(outfile) fprintf(outfile, "%s  *SHAKE128*%s__(%llu) \r\n", sha3digestTxt, infile_name, infile_size );
			}
			else 
			{
				printf("%s  *SHAKE128*%s \r\n", sha3digestTxt, infile_name );
				if(outfile) fprintf(outfile, "%s  *SHAKE128*%s \r\n", sha3digestTxt, infile_name );
			}

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
			printf("\nSHAKE128>> Calculated SHAKE128 Hash Value - OK" );
			
		}
	}
	else if(1 == isShake256)
	{
		char	sha3Buf[SHA3_BUFLEN] = {0,};
		char	sha3digestTxt[SHA3_OUTPUT_SIZ] = {0,};
		uint8_t sha3out[SHA3_OUTPUT_SIZ] = { 0, };
		
		unsigned __int64 	kll=0;
		size_t		ll=0;
		int 		ret;
		int iLenSub=0;
	#if 1
		struct	tm *pTime;
	
		pTime = current_time();

		if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile) /// 2014.08.01
		{
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "SHAKE256 Hash is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
		}		
	#endif

		LOG_V("\n");

		if( ASTERISK_FOUND == isAsteMode )  /* For ah.exe --input *.* */
		{
			int iIdx=0;
			unsigned __int64 	kll = 0UL;


			printf("SHAKE256>> SHAKE256 hashing for input files* \n");
			for(ii=0; ii<SHAKE_OUT_256*2; ii++) printf("-");  printf(" %d --\r\n", SHAKE_OUT_256*2 );

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			for(ii=iFirst; ii<=(argc-isAsteminus); ii++)
			{
				if( NULL != (inpfile = fopen( argv[ii], "rb")) ) 
				{
					memset( sha3Buf, 0x00, sizeof(sha3Buf) );
					memset( sha3out, 0x00, sizeof(sha3out) );
					memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
					
					// Initialize the Shake128 context
					sha3_init(SHAKE_256_HASH_BIT, SHA3_SHAKE_USE);	
	
					kll = 0UL;
					while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
					{
						kll += ll;
						sha3_update(sha3Buf, ll);
						
						//Digest the message
						// LOG_V("\bSHAKE256 Hashing for (%s) -> read : %lld Bytes \r", argv[ii], kll );
					}
					ClearScreen(); 

					ret = sha3_final(sha3out, SHAKE_OUT_256);
					
					for (iIdx = 0; iIdx < SHAKE_OUT_256; iIdx++)
					{
						sprintf(&sha3digestTxt[iIdx*2], "%02x", sha3out[iIdx]);
					}

					if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
					{
						printf("%s  *SHAKE256*%s__(%llu) \r\n", sha3digestTxt, argv[ii], kll );
						if(outfile) fprintf(outfile, "%s  *SHAKE128*%s__(%llu) \r\n", sha3digestTxt, argv[ii], kll );
					}
					else
					{
						printf("%s  *SHAKE256*%s \r\n", sha3digestTxt, argv[ii] );
						if(outfile) fprintf(outfile, "%s  *SHAKE128*%s \r\n", sha3digestTxt, argv[ii] );
					}

					if(inpfile) { fclose(inpfile); inpfile=NULL; }
					if(data_buf){ free(data_buf); data_buf=NULL; }

					iTotSize += kll;
					multifileindex ++;
				}
			}
			printf("\nSHAKE256>> SHAKE256 Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			AllFilesClosed();
	
		}
		else if( multifileindex > 0 )
		{
		
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

		#if 1 // 
			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("SHAKE256>>No file [%s] in current directory!!! [%s] \n", extfile_name, infile_name );
			}
			else
			{

				printf("SHAKE256>> SHAKE256 hashing for input files... \n");
				//printf("-------------------------------------------------------------\r\n" );
				for(ii=0; ii<SHAKE_OUT_256*2; ii++) printf("-");  printf(" %d --\r\n", SHAKE_OUT_256*2 );
				
				do {

					iLenSub = strlen(iFiles.name);

					if( iFiles.attrib & _A_SUBDIR )
					{

					}
					else
					{
						if( NULL == (inpfile = fopen( iFiles.name, "rb")) ) 
						{
							beep(700,100);
							printf("\n\n[++ERROR++] Can not open multi-input file (%s) in SHAKE256. \n", iFiles.name );

							AllFilesClosed();
	
							exit(0);
							return 0;
						}

						memset( sha3Buf, 0x00, sizeof(sha3Buf) );
						memset( sha3out, 0x00, sizeof(sha3out) );
						memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
						
						// Initialize the Shake128 context
						sha3_init(SHAKE_256_HASH_BIT, SHA3_SHAKE_USE);	
		
						kll = 0UL;
						while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
						{
							kll += ll;
							sha3_update(sha3Buf, ll);
							
							//Digest the message
							// LOG_V("\bSHAKE256 Hashing for (%s) -> read : %lld Bytes \r", iFiles.name, kll );
						}
						ClearScreen(); 

						ret = sha3_final(sha3out, SHAKE_OUT_256);
						
						for (ii = 0; ii < SHAKE_OUT_256; ii++)
						{
							sprintf(&sha3digestTxt[ii*2], "%02x", sha3out[ii]);
						}

						if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
						{
							printf("%s  *SHAKE256*%s__(%llu) \r\n", sha3digestTxt, iFiles.name, iFiles.size  );
							if(outfile) fprintf(outfile, "%s  *SHAKE128*%s__(%llu) \r\n", sha3digestTxt, iFiles.name, iFiles.size	);
						}
						else
						{
							printf("%s  *SHAKE256*%s \r\n", sha3digestTxt, iFiles.name );
							if(outfile) fprintf(outfile, "%s  *SHAKE128*%s \r\n", sha3digestTxt, iFiles.name );
						}

						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }

						iTotSize += iFiles.size;
						multifileindex ++;
					}

				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );

				multifileindex--;

				if( iTotSize>>20 )
					printf("\nSHAKE256>> Number of Input files : %d files (Total size: %.3f MB) ---", multifileindex, (iTotSize/1024.0)/1024.0 );
				else if( iTotSize>>10 )
					printf("\nSHAKE256>> Number of Input files : %d files (Total size: %.3f kB) ---", multifileindex, (iTotSize/1024.0) );
				else
					printf("\nSHAKE256>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			}
		#endif
		
		}
		else
		{
			printf("SHAKE256>> SHAKE256 hashing... \n");

			memset( sha3Buf, 0x00, sizeof(sha3Buf) );
			memset( sha3out, 0x00, sizeof(sha3out) );
			memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );
			
			// Initialize the SHAKE128 context
			sha3_init(SHAKE_256_HASH_BIT, SHA3_SHAKE_USE);	

			kll = 0;
			while((ll = fread(sha3Buf, 1, sizeof(sha3Buf), inpfile)) > 0) 
			{
				kll += ll;
			
				sha3_update(sha3Buf, ll);
				
				//Digest the message
				// LOG_V("\bSHAKE256 Hashing for (%s) -> read : %lld Bytes \r", infile_name, kll );
			}
			ClearScreen(); 

			ret = sha3_final(sha3out, SHAKE_OUT_256);
			
			for (ii = 0; ii < SHAKE_OUT_256; ii++)
			{
				sprintf(&sha3digestTxt[ii*2], "%02x", sha3out[ii]);
			}

			if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
			{
				printf("%s  *SHAKE256*%s__(%llu) \r\n", sha3digestTxt, infile_name, infile_size   );
				if(outfile) fprintf(outfile, "%s  *SHAKE256*%s__(%llu) \r\n", sha3digestTxt, infile_name, infile_size );
			}
			else
			{
				printf("%s  *SHAKE256*%s \r\n", sha3digestTxt, infile_name );
				if(outfile) fprintf(outfile, "%s  *SHAKE256*%s \r\n", sha3digestTxt, infile_name );
			}

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
			printf("\nSHAKE256>> Calculated SHAKE256 Hash Value - OK" );
			
		}
	}
#endif // SHA3_KECCAK_224_256_384_512

	else if(1 == isRipeMD128)
	{
		
#define RMD128_INBUF_SIZ 		(1024)  // NOT NOT NEVER modify~~~
#define RMD128_BUF_LEN 			5 // 160/32
#define RMD128_LENGTH 			16 // 160/8

		uint8_t inbuf[RMD128_INBUF_SIZ];
		char outTxt[RMD128_INBUF_SIZ*2]={0,}; 
		
		word32		  MDbuf[RMD128_BUF_LEN];   /* contains (A, B, C, D(, E))   */
		unsigned char hashcode[RMD128_LENGTH]; /* for final hash-value		   */
		
		unsigned __int64	kll;
		size_t		nbytes; // ll;
		word32		  X[16];		  /* current 16-word chunk	   */
		unsigned int  i, j; 		  /* counters					   */
		word32		  length[2];	  /* length in bytes of message   */
		word32		  offset;		  /* # of unprocessed bytes at	  */
		int 		  iLenSub=0;
		
	#if 1
		struct	tm *pTime;
	
		pTime = current_time();

		if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile) /// 2014.08.01
		{
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "RipeMD128 Hash is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
		}		
	#endif

		LOG_V("\n");

		if( ASTERISK_FOUND == isAsteMode )  /* For ah.exe --input *.* */
		{
			int iIdx=0;
			unsigned __int64 	kll = 0UL;


			printf("RMD128>> RMD128 hashing for input files* \n");
			for(ii=0; ii<RMD128_LENGTH*2; ii++) printf("-");  printf(" %d --\r\n", RMD128_LENGTH*2 );

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			for(ii=iFirst; ii<=(argc-isAsteminus); ii++)
			{
				if( NULL != (inpfile = fopen( argv[ii], "rb")) ) 
				{
					
					memset( inbuf, 0x00, sizeof(inbuf) ); // 2018.06.15
						
					//Initialize the RIPEMD-160 context
					MD128init(MDbuf);
					length[0] = 0;
					length[1] = 0;
					
					kll = 0UL;
					while((nbytes = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
					{
						kll += nbytes;
					
						/* process all complete blocks */
						for (i=0; i<(nbytes>>6); i++) {
						   for (j=0; j<16; j++)
							  X[j] = BYTES_TO_DWORD(inbuf+64*i+4*j);
						   MD128compress(MDbuf, X);
						}
						/* update length[] */
						if (length[0] + nbytes < length[0])
						   length[1]++; 				 /* overflow to msb of length */
						length[0] += nbytes;
					
						//Digest the message
						// LOG_V("\bRMD128 Hashing for (%s) -> read : %lld Bytes \r", argv[ii], kll );
					}
					ClearScreen(); 
					
					//Finish absorbing phase		
					/* finish: */
					offset = length[0] & 0x3C0;   /* extract bytes 6 to 10 inclusive */
					MD128finish(MDbuf, inbuf+offset, length[0], length[1]);
					
					for (ii=0; ii<RMD128_LENGTH; ii+=4) 
					{
						hashcode[ii]   = (unsigned char)(MDbuf[ii>>2]);
						hashcode[ii+1] = (unsigned char)(MDbuf[ii>>2] >>  8);
						hashcode[ii+2] = (unsigned char)(MDbuf[ii>>2] >> 16);
						hashcode[ii+3] = (unsigned char)(MDbuf[ii>>2] >> 24);
					}
					
					memset( outTxt, 0x00, sizeof(outTxt) );
					for (ii = 0; ii < RMD128_LENGTH; ii++)
					{
						sprintf(&outTxt[ii*2], (char*)"%02x", hashcode[ii]);
					}
	
					if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
					{
						printf("%s  *RMD128*%s__(%llu) \r\n", outTxt, argv[ii], kll );
						if(outfile) fprintf(outfile, "%s  *RMD128*%s__(%llu) \r\n", outTxt, argv[ii], kll );
					}
					else
					{
						printf("%s  *RMD128*%s \r\n", outTxt, argv[ii] );
						if(outfile) fprintf(outfile, "%s  *RMD128*%s \r\n", outTxt, argv[ii] );
					}

					if(inpfile) { fclose(inpfile); inpfile=NULL; }
					if(data_buf){ free(data_buf); data_buf=NULL; }

					iTotSize += kll;
					multifileindex ++;
				}
			}
			printf("\nRMD128>> RMD128 Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			AllFilesClosed();
	
		}
		else if( multifileindex > 0 )
		{
		
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("RMD128>>No file [%s] in current directory!!! [%s] \n", extfile_name, infile_name );
			}
			else
			{

				printf("RMD128>> RMD128 hashing for input files... \n");
				//printf("-------------------------------------------------------------\r\n" );
				for(ii=0; ii<RMD128_LENGTH*2; ii++) printf("-");  printf(" %d --\r\n", RMD128_LENGTH*2 );
				
				do {

					iLenSub = strlen(iFiles.name);

					if( iFiles.attrib & _A_SUBDIR )
					{

					}
					else
					{
						if( NULL == (inpfile = fopen( iFiles.name, "rb")) ) 
						{
							beep(700,100);
							printf("\n\n[++ERROR++] Can not open multi-input file (%s) in RMD128. \n", iFiles.name );

							AllFilesClosed();
	
							exit(0);
							return 0;
						}

						memset( inbuf, 0x00, sizeof(inbuf) ); // 2018.06.15
							
						//Initialize the RIPEMD-160 context
						MD128init(MDbuf);
						length[0] = 0;
						length[1] = 0;
						
						kll = 0UL;
						while((nbytes = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
						{
							kll += nbytes;
						
							/* process all complete blocks */
							for (i=0; i<(nbytes>>6); i++) {
							   for (j=0; j<16; j++)
								  X[j] = BYTES_TO_DWORD(inbuf+64*i+4*j);
							   MD128compress(MDbuf, X);
							}
							/* update length[] */
							if (length[0] + nbytes < length[0])
							   length[1]++; 				 /* overflow to msb of length */
							length[0] += nbytes;
						
							//Digest the message
							// LOG_V("\bRMD128 Hashing for (%s) -> read : %lld Bytes \r", iFiles.name, kll );
						}
						ClearScreen(); 
						
						//Finish absorbing phase		
						/* finish: */
						offset = length[0] & 0x3C0;   /* extract bytes 6 to 10 inclusive */
						MD128finish(MDbuf, inbuf+offset, length[0], length[1]);
						
						for (ii=0; ii<RMD128_LENGTH; ii+=4) 
						{
							hashcode[ii]   = (unsigned char)(MDbuf[ii>>2]);
							hashcode[ii+1] = (unsigned char)(MDbuf[ii>>2] >>  8);
							hashcode[ii+2] = (unsigned char)(MDbuf[ii>>2] >> 16);
							hashcode[ii+3] = (unsigned char)(MDbuf[ii>>2] >> 24);
						}
						
						memset( outTxt, 0x00, sizeof(outTxt) );
						for (ii = 0; ii < RMD128_LENGTH; ii++)
						{
							sprintf(&outTxt[ii*2], (char*)"%02x", hashcode[ii]);
						}

						if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
						{
							printf("%s  *RMD128*%s__(%llu) \r\n", outTxt, iFiles.name, iFiles.size  );
							if(outfile) fprintf(outfile, "%s  *RMD128*%s__(%llu) \r\n", outTxt, iFiles.name, iFiles.size	);
						}
						else
						{
							printf("%s  *RMD128*%s \r\n", outTxt, iFiles.name );
							if(outfile) fprintf(outfile, "%s  *RMD128*%s \r\n", outTxt, iFiles.name );
						}

						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }

						iTotSize += iFiles.size;
						multifileindex ++;
					}

				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );

				multifileindex--;

				printf("\nRMD128>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			}

		}
		else
		{

			printf("RMD128>> RMD128 hashing... \n");

			memset( inbuf, 0x00, sizeof(inbuf) ); // 2018.06.15
				
			//Initialize the RIPEMD-160 context
			MD128init(MDbuf);
			length[0] = 0;
			length[1] = 0;

			kll = 0UL;
			while((nbytes = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
			{
				kll += nbytes;
		
				/* process all complete blocks */
				for (i=0; i<(nbytes>>6); i++) {
				   for (j=0; j<16; j++)
					  X[j] = BYTES_TO_DWORD(inbuf+64*i+4*j);
				   MD128compress(MDbuf, X);
				}
				/* update length[] */
				if (length[0] + nbytes < length[0])
				   length[1]++; 				 /* overflow to msb of length */
				length[0] += nbytes;

				//Digest the message
				// LOG_V("\bRMD128 Hashing for (%s) -> read : %lld Bytes \r", infile_name, kll );
			}
			ClearScreen(); 

			//Finish absorbing phase		
			/* finish: */
			offset = length[0] & 0x3C0;   /* extract bytes 6 to 10 inclusive */
			MD128finish(MDbuf, inbuf+offset, length[0], length[1]);

			for (ii=0; ii<RMD128_LENGTH; ii+=4) 
			{
				hashcode[ii]   = (unsigned char)(MDbuf[ii>>2]);
				hashcode[ii+1] = (unsigned char)(MDbuf[ii>>2] >>  8);
				hashcode[ii+2] = (unsigned char)(MDbuf[ii>>2] >> 16);
				hashcode[ii+3] = (unsigned char)(MDbuf[ii>>2] >> 24);
			}
		
			memset( outTxt, 0x00, sizeof(outTxt) );
			for (ii = 0; ii < RMD128_LENGTH; ii++)
			{
				sprintf(&outTxt[ii*2], (char*)"%02x", hashcode[ii]);
			}
			
			if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
			{
				printf("%s  *RMD128*%s__(%llu) \r\n", outTxt, infile_name, infile_size   );
				if(outfile) fprintf(outfile, "%s  *RMD128*%s__(%llu) \r\n", outTxt, infile_name, infile_size );
			}
			else
			{
				printf("%s  *RMD128*%s \r\n", outTxt, infile_name );
				if(outfile) fprintf(outfile, "%s  *RMD128*%s \r\n", outTxt, infile_name );
			}
			
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
			printf("\nRMD128>> Calculated RMD128 Hash Value - OK" );

		}
	}
	else if(1 == isRipeMD160)
	{
		
#define RMD160_INBUF_SIZ 		(1024) // NOT NOT NEVER modify~~~
#define RMD160_BUF_LEN 			5 // 160/32
#define RMD160_LENGTH 			20 // 160/8

		uint8_t inbuf[RMD160_INBUF_SIZ];
		char outTxt[RMD160_INBUF_SIZ*2]={0,}; 
		
		word32		  MDbuf[RMD160_BUF_LEN];   /* contains (A, B, C, D(, E))   */
		unsigned char hashcode[RMD160_LENGTH]; /* for final hash-value		   */
		
		unsigned __int64	kll;
		size_t		nbytes; // ll;
		word32		  X[16];		  /* current 16-word chunk	   */
		unsigned int  i, j; 		  /* counters					   */
		word32		  length[2];	  /* length in bytes of message   */
		word32		  offset;		  /* # of unprocessed bytes at	  */
		int 		  iLenSub=0;
		
	#if 1
		struct	tm *pTime;
	
		pTime = current_time();

		if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile) /// 2014.08.01
		{
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "RipeMD160 Hash is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
		}		
	#endif

		LOG_V("\n");

		if( ASTERISK_FOUND == isAsteMode )  /* For ah.exe --input *.* */
		{
			int iIdx=0;
			unsigned __int64 	kll = 0UL;


			printf("RipeMD160>> RipeMD160 hashing for input files* \n");
			for(ii=0; ii<RMD160_LENGTH*2; ii++) printf("-");  printf(" %d --\r\n", RMD160_LENGTH*2 );

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			for(ii=iFirst; ii<=(argc-isAsteminus); ii++)
			{
				if( NULL != (inpfile = fopen( argv[ii], "rb")) ) 
				{
					
					memset( inbuf, 0x00, sizeof(inbuf) ); // 2018.06.15
						
					//Initialize the RIPEMD-160 context
					MD160init(MDbuf);
					length[0] = 0;
					length[1] = 0;
					
					kll = 0UL;
					while((nbytes = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
					{
						kll += nbytes;
					
						/* process all complete blocks */
						for (i=0; i<(nbytes>>6); i++) {
						   for (j=0; j<16; j++)
							  X[j] = BYTES_TO_DWORD(inbuf+64*i+4*j);
						   MD160compress(MDbuf, X);
						}
						/* update length[] */
						if (length[0] + nbytes < length[0])
						   length[1]++; 				 /* overflow to msb of length */
						length[0] += nbytes;
					
						//Digest the message
						// LOG_V("\bRipeMD160 Hashing for (%s) -> read : %lld Bytes \r", argv[ii], kll );
					}
					ClearScreen(); 
					
					//Finish absorbing phase		
					/* finish: */
					offset = length[0] & 0x3C0;   /* extract bytes 6 to 10 inclusive */
					MD160finish(MDbuf, inbuf+offset, length[0], length[1]);
					
					for (ii=0; ii<RMD160_LENGTH; ii+=4) 
					{
						hashcode[ii]   = (unsigned char)(MDbuf[ii>>2]);
						hashcode[ii+1] = (unsigned char)(MDbuf[ii>>2] >>  8);
						hashcode[ii+2] = (unsigned char)(MDbuf[ii>>2] >> 16);
						hashcode[ii+3] = (unsigned char)(MDbuf[ii>>2] >> 24);
					}
					
					memset( outTxt, 0x00, sizeof(outTxt) );
					for (ii = 0; ii < RMD160_LENGTH; ii++)
					{
						sprintf(&outTxt[ii*2], (char*)"%02x", hashcode[ii]);
					}
	
					if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
					{
						printf("%s  *RMD160*%s__(%llu) \r\n", outTxt, argv[ii], kll );
						if(outfile) fprintf(outfile, "%s  *RMD160*%s__(%llu) \r\n", outTxt, argv[ii], kll );
					}
					else
					{
						printf("%s  *RMD160*%s \r\n", outTxt, argv[ii] );
						if(outfile) fprintf(outfile, "%s  *RMD160*%s \r\n", outTxt, argv[ii] );
					}

					if(inpfile) { fclose(inpfile); inpfile=NULL; }
					if(data_buf){ free(data_buf); data_buf=NULL; }

					iTotSize += kll;
					multifileindex ++;
				}
			}
			printf("\nRipeMD160>> RipeMD160 Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			AllFilesClosed();
	
		}
		else if( multifileindex > 0 )
		{
		
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("RipeMD160>>No file [%s] in current directory!!! [%s] \n", extfile_name, infile_name );
			}
			else
			{

				printf("RipeMD160>> RipeMD160 hashing for input files... \n");
				//printf("-------------------------------------------------------------\r\n" );
				for(ii=0; ii<RMD160_LENGTH*2; ii++) printf("-");  printf(" %d --\r\n", RMD160_LENGTH*2 );
				
				do {

					iLenSub = strlen(iFiles.name);

					if( iFiles.attrib & _A_SUBDIR )
					{

					}
					else
					{
						if( NULL == (inpfile = fopen( iFiles.name, "rb")) ) 
						{
							beep(700,100);
							printf("\n\n[++ERROR++] Can not open multi-input file (%s) in SHAKE256. \n", iFiles.name );

							AllFilesClosed();
	
							exit(0);
							return 0;
						}

						memset( inbuf, 0x00, sizeof(inbuf) ); // 2018.06.15
							
						//Initialize the RIPEMD-160 context
						MD160init(MDbuf);
						length[0] = 0;
						length[1] = 0;
						
						kll = 0UL;
						while((nbytes = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
						{
							kll += nbytes;
						
							/* process all complete blocks */
							for (i=0; i<(nbytes>>6); i++) {
							   for (j=0; j<16; j++)
								  X[j] = BYTES_TO_DWORD(inbuf+64*i+4*j);
							   MD160compress(MDbuf, X);
							}
							/* update length[] */
							if (length[0] + nbytes < length[0])
							   length[1]++; 				 /* overflow to msb of length */
							length[0] += nbytes;
						
							//Digest the message
							// LOG_V("\bRipeMD160 Hashing for (%s) -> read : %lld Bytes \r", iFiles.name, kll );
						}
						ClearScreen(); 
						
						//Finish absorbing phase		
						/* finish: */
						offset = length[0] & 0x3C0;   /* extract bytes 6 to 10 inclusive */
						MD160finish(MDbuf, inbuf+offset, length[0], length[1]);
						
						for (ii=0; ii<RMD160_LENGTH; ii+=4) 
						{
							hashcode[ii]   = (unsigned char)(MDbuf[ii>>2]);
							hashcode[ii+1] = (unsigned char)(MDbuf[ii>>2] >>  8);
							hashcode[ii+2] = (unsigned char)(MDbuf[ii>>2] >> 16);
							hashcode[ii+3] = (unsigned char)(MDbuf[ii>>2] >> 24);
						}
						
						memset( outTxt, 0x00, sizeof(outTxt) );
						for (ii = 0; ii < RMD160_LENGTH; ii++)
						{
							sprintf(&outTxt[ii*2], (char*)"%02x", hashcode[ii]);
						}

						if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
						{
							printf("%s  *RMD160*%s__(%llu) \r\n", outTxt, iFiles.name, iFiles.size  );
							if(outfile) fprintf(outfile, "%s  *RMD160*%s__(%llu) \r\n", outTxt, iFiles.name, iFiles.size	);
						}
						else
						{
							printf("%s  *RMD160*%s \r\n", outTxt, iFiles.name );
							if(outfile) fprintf(outfile, "%s  *RMD160*%s \r\n", outTxt, iFiles.name );
						}

						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }

						iTotSize += iFiles.size;
						multifileindex ++;
					}

				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );

				multifileindex--;

				printf("\nRipeMD160>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			}

		}
		else
		{

			printf("RipeMD160>> RipeMD160 hashing... \n");

			memset( inbuf, 0x00, sizeof(inbuf) ); // 2018.06.15
				
			//Initialize the RIPEMD-160 context
			MD160init(MDbuf);
			length[0] = 0;
			length[1] = 0;

			kll = 0UL;
			while((nbytes = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
			{
				kll += nbytes;
		
				/* process all complete blocks */
				for (i=0; i<(nbytes>>6); i++) {
				   for (j=0; j<16; j++)
					  X[j] = BYTES_TO_DWORD(inbuf+64*i+4*j);
				   MD160compress(MDbuf, X);
				}
				/* update length[] */
				if (length[0] + nbytes < length[0])
				   length[1]++; 				 /* overflow to msb of length */
				length[0] += nbytes;

				//Digest the message
				// LOG_V("\bRipeMD160 Hashing for (%s) -> read : %lld Bytes \r", infile_name, kll );
			}
			ClearScreen(); 

			//Finish absorbing phase		
			/* finish: */
			offset = length[0] & 0x3C0;   /* extract bytes 6 to 10 inclusive */
			MD160finish(MDbuf, inbuf+offset, length[0], length[1]);

			for (ii=0; ii<RMD160_LENGTH; ii+=4) 
			{
				hashcode[ii]   = (unsigned char)(MDbuf[ii>>2]);
				hashcode[ii+1] = (unsigned char)(MDbuf[ii>>2] >>  8);
				hashcode[ii+2] = (unsigned char)(MDbuf[ii>>2] >> 16);
				hashcode[ii+3] = (unsigned char)(MDbuf[ii>>2] >> 24);
			}
		
			memset( outTxt, 0x00, sizeof(outTxt) );
			for (ii = 0; ii < RMD160_LENGTH; ii++)
			{
				sprintf(&outTxt[ii*2], (char*)"%02x", hashcode[ii]);
			}
			
			if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
			{
				printf("%s  *RMD160*%s__(%llu) \r\n", outTxt, infile_name, infile_size   );
				if(outfile) fprintf(outfile, "%s  *RMD160*%s__(%llu) \r\n", outTxt, infile_name, infile_size );
			}
			else
			{
				printf("%s  *RMD160*%s \r\n", outTxt, infile_name );
				if(outfile) fprintf(outfile, "%s  *RMD160*%s \r\n", outTxt, infile_name );
			}
			
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
			printf("\nRipeMD160>> Calculated RipeMD160 Hash Value - OK" );

		}
	}

#if BLAKE_224_256_384_512_HASH
	else if(1 == isBlake224)
	{
		uint8_t inbuf[BLOCK224], out[BLAKE224_LEN]={0,}; 
		char outTxt[BLAKE224_LEN*2]={0,}; 
		state224 blakeS; 
		unsigned __int64 kll;
		size_t		ll=0;
		int iLenSub=0;
		struct	tm *pTime = NULL;
	
		pTime = current_time();

		if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile) /// 2014.08.01
		{
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "BLAKE224 Hash is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
		}		

		LOG_V("\n");

		if( ASTERISK_FOUND == isAsteMode )	/* For ah.exe --input *.* */
		{
			unsigned __int64 	kll = 0UL;

			printf("BLAKE224>> BLAKE224 hashing for input files* (%d) \n", argc );

		#if 0
			//for(ii=0; ii<=(argc-isAsteminus); ii++) printf("%3d : [%s] (%d:%d)\n", ii, argv[ii], isAsteminus, iFirst );

					
			for(ii=0; ii<BLAKE224_LEN*2; ii++) printf("-");  printf(" %d --\r\n", BLAKE224_LEN*2 );

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			for(ii=iFirst; ii<=(argc-isAsteminus) ; ii++)
			{
				if( NULL != (inpfile = fopen( argv[ii], "rb")) ) 
				{
					memset( inbuf, 0x00, sizeof(inbuf) );
					memset( out, 0x00, sizeof(out) );
					
					//Initialize the BLAKE-224 context
					blake224_init( &blakeS ); 

					kll = 0ULL;
					while((ll = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
					{
						kll += ll;
					
						//Absorb input data
						blake224_update( &blakeS, inbuf, ll ); 
					
						//Digest the message
						printf("\bBLAKE224 Hashing for (%s) -> read : %lld Bytes \r", argv[ii], kll );
					}
					ClearScreen(); 
					
					//Finish absorbing phase
					blake224_final( &blakeS, out ); 

					memset( outTxt, 0x00, sizeof(outTxt) );
					
					for (ii = 0; ii < BLAKE224_LEN; ii++) 
					{
						if( iUpper )
							sprintf(&outTxt[ii*2], (char*)"%02X", out[ii]);
						else
							sprintf(&outTxt[ii*2], (char*)"%02x", out[ii]);
					}

					if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
					{
						printf("%s  *BLAKE224*%s__(%llu) \r\n", outTxt, argv[ii], kll );
						if(outfile) fprintf(outfile, "%s  *BLAKE224*%s__(%llu) \r\n", outTxt, argv[ii], kll );
					}
					else
					{
						printf("%s  *BLAKE224*%s \r\n", outTxt, argv[ii] );
						if(outfile) fprintf(outfile, "%s  *BLAKE224*%s \r\n", outTxt, argv[ii] );
					}
					
					if(inpfile) { fclose(inpfile); inpfile=NULL; }
					if(data_buf){ free(data_buf); data_buf=NULL; }

					iTotSize += kll;
					multifileindex ++;
				}
				else
				{
					printf("==NOT-OPEN== [%s] === \n", argv[ii] );
				}
			}
			printf("\nBLAKE224>> BLAKE224 Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

		#endif
		
			AllFilesClosed();
	
		}
		else if( multifileindex > 0 )
		{

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("BLAKE224>>No file [%s] in current directory!!! [%s] \n", extfile_name, infile_name );
			}
			else
			{

				printf("BLAKE224>> BLAKE224 hashing for input files... \n");
				//printf("-------------------------------------------------------------\r\n" );
				for(ii=0; ii<BLAKE224_LEN*2; ii++) printf("-");  printf(" %d --\r\n", BLAKE224_LEN*2 );

				do {

					iLenSub = strlen(iFiles.name);

					if( iFiles.attrib & _A_SUBDIR )
					{

					}
					else
					{
						if( NULL == (inpfile = fopen( iFiles.name, "rb")) ) 
						{
							beep(700,100);
							printf("\n\n[++ERROR++] Can not open multi-input file (%s) in BLAKE224. \n", iFiles.name );

							AllFilesClosed();
	
							exit(0);
							return 0;
						}

						memset( inbuf, 0x00, sizeof(inbuf) );
						memset( out, 0x00, sizeof(out) );
						
						//Initialize the BLAKE-224 context
						blake224_init( &blakeS ); 
						
						kll = 0ULL;
						while((ll = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
						{
							kll += ll;
						
							//Absorb input data
							blake224_update( &blakeS, inbuf, ll ); 
						
							//Digest the message
							// LOG_V("\bBLAKE224 Hashing for (%s) -> read : %lld Bytes \r", iFiles.name, kll );
						}
						ClearScreen(); 
						
						//Finish absorbing phase
						blake224_final( &blakeS, out ); 
						
						memset( outTxt, 0x00, sizeof(outTxt) );
						
						for (ii = 0; ii < BLAKE224_LEN; ii++) 
						{
							if( iUpper )
								sprintf(&outTxt[ii*2], (char*)"%02X", out[ii]);
							else
								sprintf(&outTxt[ii*2], (char*)"%02x", out[ii]);
						}

						
						if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
						{
							printf("%s  *%s*%s__(%llu) \r\n", outTxt, str_hash, iFiles.name, iFiles.size   );
							if(outfile) fprintf(outfile, "%s  *%s*%s__(%llu) \r\n", outTxt, str_hash, iFiles.name, iFiles.size );
						}
						else
						{
							printf("%s  *%s*%s \r\n", outTxt, str_hash, iFiles.name );
							if(outfile) fprintf(outfile, "%s  *%s*%s \r\n", outTxt, str_hash, iFiles.name );
						}
						
						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }


						iTotSize += iFiles.size;
						multifileindex ++;
					}

				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );

				multifileindex--;

				printf("\nBLAKE224>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			}
		
		}
		else
		{
			//uint8_t inbuf[BLOCK224], out[BLAKE224_LEN]={0,}; 
			//char outTxt[BLAKE224_LEN*2]={0,}; 
			//state224 S; 
			//unsigned __int64 kll;
			//size_t 		ll;


			printf("BLAKE224>> BLAKE224 hashing... \n");

			memset( inbuf, 0x00, sizeof(inbuf) );
			memset( out, 0x00, sizeof(out) );

			//Initialize the BLAKE-224 context
			blake224_init( &blakeS ); 

			kll = 0ULL;
			while((ll = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
			{
				kll += ll;

				//Absorb input data
				blake224_update( &blakeS, inbuf, ll ); 

				//Digest the message
				// LOG_V("\bBLAKE224 Hashing for (%s) -> read : %lld Bytes \r", infile_name, kll );
			}
			ClearScreen(); 

			//Finish absorbing phase
			blake224_final( &blakeS, out ); 


			memset( outTxt, 0x00, sizeof(outTxt) );
			for (ii = 0; ii < BLAKE224_LEN; ii++) 
			{
				if( iUpper )
					sprintf(&outTxt[ii*2], (char*)"%02X", out[ii]);
				else
					sprintf(&outTxt[ii*2], (char*)"%02x", out[ii]);
			}
			
			if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
			{
				printf("%s  *%s*%s__(%llu) \r\n", outTxt, str_hash, infile_name, infile_size   );
				if(outfile) fprintf(outfile, "%s  *%s*%s__(%llu) \r\n", outTxt, str_hash, infile_name, infile_size );
			}
			else
			{
				printf("%s  *%s*%s \r\n", outTxt, str_hash, infile_name );
				if(outfile) fprintf(outfile, "%s  *%s*%s \r\n", outTxt, str_hash, infile_name );
			}
			
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
			printf("\nBLAKE224>> Calculated BLAKE224 Hash Value - OK" );

		}
	}
	else if(1 == isBlake256)
	{
		uint8_t inbuf[BLOCK256], out[BLAKE256_LEN]={0,}; 
		char outTxt[BLAKE256_LEN*2]={0,}; 
		state256 blakeS; 	
		unsigned __int64  kll;
		size_t		ll;

		int iLenSub=0;
		struct	tm *pTime = NULL;
	
		pTime = current_time();

		if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile) /// 2014.08.01
		{
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "BLAKE256 Hash is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
		}		

		LOG_V("\n");

		if( ASTERISK_FOUND == isAsteMode )	/* For ah.exe --input *.* */
		{
			unsigned __int64 	kll = 0UL;

			printf("BLAKE256>> BLAKE256 hashing for input files* (%d) \n", argc );


		#if 0
			//for(ii=0; ii<=(argc-isAsteminus); ii++) printf("%3d : [%s] (%d:%d)\n", ii, argv[ii], isAsteminus, iFirst );

					
			for(ii=0; ii<BLAKE256_LEN*2; ii++) printf("-");  printf(" %d --\r\n", BLAKE256_LEN*2 );

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			for(ii=iFirst; ii<=(argc-isAsteminus) ; ii++)
			{
				//if( 0==isFileExist( (const char *)argv[ii], 0 ) ) continue;
				if( NULL != (inpfile = fopen( argv[ii], "rb")) ) 
				{

					memset( inbuf, 0x00, sizeof(inbuf) );
					memset( out, 0x00, sizeof(out) );

					//Initialize the BLAKE-256 context
					blake256_init( &blakeS ); 
					
					kll = 0ULL;
					while((ll = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
					{
						kll += ll;
					
						//Absorb input data
						blake256_update( &blakeS, inbuf, ll ); 
					
						//Digest the message
						printf("\bBLAKE256 Hashing for (%s) -> read : %lld Bytes \r", argv[ii], kll );
					}
					ClearScreen(); 
					
					//Finish absorbing phase
					blake256_final( &blakeS, out ); 

					memset( outTxt, 0x00, sizeof(outTxt) );
					for (ii = 0; ii < BLAKE256_LEN; ii++)
					{
						if( iUpper )
							sprintf(&outTxt[ii*2], (char*)"%02X", out[ii]);
						else
							sprintf(&outTxt[ii*2], (char*)"%02x", out[ii]);
					}

					if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
					{
						printf("%s  *BLAKE256*%s__(%llu) \r\n", outTxt, argv[ii], kll );
						if(outfile) fprintf(outfile, "%s  *BLAKE256*%s__(%llu) \r\n", outTxt, argv[ii], kll );
					}
					else
					{
						printf("%s  *BLAKE256*%s \r\n", outTxt, argv[ii] );
						if(outfile) fprintf(outfile, "%s  *BLAKE256*%s \r\n", outTxt, argv[ii] );
					}
					
					if(inpfile) { fclose(inpfile); inpfile=NULL; }
					if(data_buf){ free(data_buf); data_buf=NULL; }
					
					iTotSize += iFiles.size;
					multifileindex ++;

				}
				else
				{
					printf("==NOT-OPEN== [%s] === \n", argv[ii] );
				}
			}
			printf("\nBLAKE224>> BLAKE256 Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );
		#endif
			AllFilesClosed();
	
		}
		else if( multifileindex > 0 )
		{

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }

			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("BLAKE224>>No file [%s] in current directory!!! [%s] \n", extfile_name, infile_name );
			}
			else
			{

				printf("BLAKE256>> BLAKE256 hashing for input files... \n");
				//printf("-------------------------------------------------------------\r\n" );
				for(ii=0; ii<BLAKE256_LEN*2; ii++) printf("-");  printf(" %d --\r\n", BLAKE256_LEN*2 );
				
				do {

					iLenSub = strlen(iFiles.name);

					if( iFiles.attrib & _A_SUBDIR )
					{

					}
					else
					{
						if( NULL == (inpfile = fopen( iFiles.name, "rb")) ) 
						{
							beep(700,100);
							printf("\n\n[++ERROR++] Can not open multi-input file (%s) in BLAKE256. \n", iFiles.name );

							AllFilesClosed();
	
							exit(0);
							return 0;
						}

						memset( inbuf, 0x00, sizeof(inbuf) );
						memset( out, 0x00, sizeof(out) );


						//Initialize the BLAKE-256 context
					    blake256_init( &blakeS ); 
						
						kll = 0ULL;
						while((ll = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
						{
							kll += ll;

							//Absorb input data
							blake256_update( &blakeS, inbuf, ll ); 

							//Digest the message
							// LOG_V("\bBLAKE256 Hashing for (%s) -> read : %lld Bytes \r", iFiles.name, kll );
						}
						ClearScreen(); 

						//Finish absorbing phase
					    blake256_final( &blakeS, out ); 
						

						memset( outTxt, 0x00, sizeof(outTxt) );
						for (ii = 0; ii < BLAKE256_LEN; ii++)
						{
							if( iUpper )
								sprintf(&outTxt[ii*2], (char*)"%02X", out[ii]);
							else
								sprintf(&outTxt[ii*2], (char*)"%02x", out[ii]);
						}
	
						
						if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
						{
							printf("%s  *%s*%s__(%llu) \r\n", outTxt, str_hash, iFiles.name, iFiles.size   );
							if(outfile) fprintf(outfile, "%s  *%s*%s__(%llu) \r\n", outTxt, str_hash, iFiles.name, iFiles.size );
						}
						else
						{
							printf("%s  *%s*%s \r\n", outTxt, str_hash, iFiles.name );
							if(outfile) fprintf(outfile, "%s  *%s*%s \r\n", outTxt, str_hash, iFiles.name );
						}
						
						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }


						iTotSize += iFiles.size;
						multifileindex ++;
					}

				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );

				multifileindex--;

				printf("\nBLAKE256>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );

			}
		
		}
		else
		{

			printf("BLAKE256>> BLAKE256 hashing... \n");

			memset( inbuf, 0x00, sizeof(inbuf) );
			memset( out, 0x00, sizeof(out) );
			
			
			//Initialize the BLAKE-256 context
			blake256_init( &blakeS ); 
			
			kll = 0ULL;
			while((ll = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
			{
				kll += ll;
			
				//Absorb input data
				blake256_update( &blakeS, inbuf, ll ); 
			
				//Digest the message
				// LOG_V("\bBLAKE256 Hashing for (%s) -> read : %lld Bytes \r", iFiles.name, kll );
			}
			ClearScreen(); 

			//Finish absorbing phase
			blake256_final( &blakeS, out ); 

			memset( outTxt, 0x00, sizeof(outTxt) );
			for (ii = 0; ii < BLAKE256_LEN; ii++)
			{
				if( iUpper )
					sprintf(&outTxt[ii*2], (char*)"%02X", out[ii]);
				else
					sprintf(&outTxt[ii*2], (char*)"%02x", out[ii]);
			}

			if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
			{
				printf("%s  *%s*%s__(%llu) \r\n", outTxt, str_hash, infile_name, infile_size );
				if(outfile) fprintf(outfile, "%s  *BLAKE256*%s__(%llu) \r\n", outTxt, str_hash, infile_name, infile_size );
			}
			else
			{
				printf("%s  *%s*%s \r\n", outTxt, str_hash, infile_name );
				if(outfile) fprintf(outfile, "%s  *%s*%s \r\n", outTxt, str_hash, infile_name );
			}

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
			printf("\nBLAKE256>> Calculated BLAKE256 Hash Value - OK" );

		}
	}
	else if(1 == isBlake384)
	{
		uint8_t inbuf[BLOCK384], out[BLAKE384_LEN] ={0,}; 
		char outTxt[BLAKE384_LEN*2]={0,}; 
		state384 blakeS; 		
		unsigned __int64  kll;
		size_t		ll;

		int iLenSub=0;
		struct	tm *pTime = NULL;
	
		pTime = current_time();
	
		if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile) /// 2014.08.01
		{
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "BLAKE384 Hash is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
		}		
	
		LOG_V("\n");
	
		if( ASTERISK_FOUND == isAsteMode )	/* For ah.exe --input *.* */
		{
			unsigned __int64	kll = 0UL;
	
			printf("BLAKE384>> BLAKE384 hashing for input files* (%d) \n", argc );

		#if 0
			//for(ii=0; ii<=(argc-isAsteminus); ii++) printf("%3d : [%s] (%d:%d)\n", ii, argv[ii], isAsteminus, iFirst );
	
					
			for(ii=0; ii<BLAKE384_LEN*2; ii++) printf("-");  printf(" %d --\r\n", BLAKE384_LEN*2 );
	
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
	
			for(ii=iFirst; ii<=(argc-isAsteminus) ; ii++)
			{

				printf("FILE - [%s] : ", argv[ii] );
				if( isFileExist( argv[ii] ) ) printf(" -- OK \n");
				else printf(" --- NONE ----------------- \n");


				if( (inpfile = fopen( argv[ii], "rb")) != NULL ) 
				{
					
					memset( inbuf, 0x00, sizeof(inbuf) );
					memset( out, 0x00, sizeof(out) );
					
					//Initialize the BLAKE-384 context
					blake384_init( &blakeS ); 
					
					kll = 0ULL;
					while((ll = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
					{
						kll += ll;
					
						//Absorb input data
						blake384_update( &blakeS, inbuf, ll ); 
					
						//Digest the message
						printf("\bBLAKE384 Hashing for (%s) -> read : %lld Bytes \r", argv[ii], kll );
					}
					ClearScreen(); 
					
					//Finish absorbing phase
					blake384_final( &blakeS, out ); 
					
					
					memset( outTxt, 0x00, sizeof(outTxt) );
					for (ii = 0; ii < BLAKE384_LEN; ii++)
					{
						if( iUpper )
							sprintf(&outTxt[ii*2], (char*)"%02X", out[ii]);
						else
							sprintf(&outTxt[ii*2], (char*)"%02x", out[ii]);
					}
					
					
					if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
					{
						printf("%s  *BLAKE384*%s__(%llu) \r\n", outTxt, argv[ii], kll );
						if(outfile) fprintf(outfile, "%s  *BLAKE384*%s__(%llu) \r\n", outTxt, argv[ii], kll );
					}
					else
					{
						printf("%s  *BLAKE384*%s \r\n", outTxt, argv[ii] );
						if(outfile) fprintf(outfile, "%s  *BLAKE384*%s \r\n", outTxt, argv[ii] );
					}
					
					if(inpfile) { fclose(inpfile); inpfile=NULL; }
					if(data_buf){ free(data_buf); data_buf=NULL; }
					
					iTotSize += iFiles.size;
					multifileindex ++;
	
				}

				
			}
			printf("\nBLAKE384>> BLAKE384 Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );
		#endif
			AllFilesClosed();
	
		}
		else if( multifileindex > 0 )
		{

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
	
			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("BLAKE384>>No file [%s] in current directory!!! [%s] \n", extfile_name, infile_name );
			}
			else
			{
	
				printf("BLAKE384>> BLAKE384 hashing for input files... \n");
				//printf("-------------------------------------------------------------\r\n" );
				for(ii=0; ii<BLAKE384_LEN*2; ii++) printf("-");  printf(" %d --\r\n", BLAKE384_LEN*2 );
				
				do {
	
					iLenSub = strlen(iFiles.name);
	
					if( iFiles.attrib & _A_SUBDIR )
					{
	
					}
					else
					{
						if( NULL == (inpfile = fopen( iFiles.name, "rb")) ) 
						{
							beep(700,100);
							printf("\n\n[++ERROR++] Can not open multi-input file (%s) in BLAKE384. \n", iFiles.name );
	
							AllFilesClosed();
	
							exit(0);
							return 0;
						}

						memset( inbuf, 0x00, sizeof(inbuf) );
						memset( out, 0x00, sizeof(out) );
						
						//Initialize the BLAKE-384 context
						blake384_init( &blakeS ); 
						
						kll = 0ULL;
						while((ll = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
						{
							kll += ll;
						
							//Absorb input data
							blake384_update( &blakeS, inbuf, ll ); 
						
							//Digest the message
							// LOG_V("\bBLAKE384 Hashing for (%s) -> read : %lld Bytes \r", iFiles.name, kll );
						}
						ClearScreen(); 
						
						//Finish absorbing phase
						blake384_final( &blakeS, out ); 

						memset( outTxt, 0x00, sizeof(outTxt) );
						for (ii = 0; ii < BLAKE384_LEN; ii++)
						{
							if( iUpper )
								sprintf(&outTxt[ii*2], (char*)"%02X", out[ii]);
							else
								sprintf(&outTxt[ii*2], (char*)"%02x", out[ii]);
						}
						
						
						if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
						{
							printf("%s  *%s*%s__(%llu) \r\n", outTxt, str_hash, iFiles.name, iFiles.size   );
							if(outfile) fprintf(outfile, "%s  *%s*%s__(%llu) \r\n", outTxt, str_hash, iFiles.name, iFiles.size );
						}
						else
						{
							printf("%s  *%s*%s \r\n", outTxt, str_hash, iFiles.name );
							if(outfile) fprintf(outfile, "%s  *%s*%s \r\n", outTxt, str_hash, iFiles.name );
						}

						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }
	
	
						iTotSize += iFiles.size;
						multifileindex ++;
					}
	
				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );
	
				multifileindex--;
	
				printf("\nBLAKE384>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );
	
			}
		
		}
		else
		{

			printf("BLAKE384>> BLAKE384 hashing... \n");

			memset( inbuf, 0x00, sizeof(inbuf) );
			memset( out, 0x00, sizeof(out) );

			//Initialize the BLAKE-384 context
		    blake384_init( &blakeS ); 

			kll = 0ULL;
			while((ll = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
			{
				kll += ll;

				//Absorb input data
				blake384_update( &blakeS, inbuf, ll ); 

				//Digest the message
				// LOG_V("\bBLAKE384 Hashing for (%s) -> read : %lld Bytes \r", iFiles.name, kll );
			}
			ClearScreen(); 

			//Finish absorbing phase
		    blake384_final( &blakeS, out ); 


			memset( outTxt, 0x00, sizeof(outTxt) );
			for (ii = 0; ii < BLAKE384_LEN; ii++)
			{
				if( iUpper )
					sprintf(&outTxt[ii*2], (char*)"%02X", out[ii]);
				else
					sprintf(&outTxt[ii*2], (char*)"%02x", out[ii]);
			}

			if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
			{
				printf("%s  *%s*%s__(%llu) \r\n", outTxt, str_hash, infile_name, infile_size  );
				if(outfile) fprintf(outfile, "%s  *%s*%s__(%llu) \r\n", outTxt, str_hash, infile_name, infile_size  );
			}
			else
			{
				printf("%s  *%s*%s \r\n", outTxt, str_hash, infile_name );
				if(outfile) fprintf(outfile, "%s  *%s*%s \r\n", outTxt, str_hash, infile_name );
			}
	
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
			printf("\nBLAKE384>> Calculated BLAKE384 Hash Value - OK" );
	
		}
	}
	else if(1 == isBlake512)
	{
		uint8_t inbuf[BLOCK512], out[BLAKE512_LEN]={0,}; 
		char outTxt[BLAKE512_LEN*2]={0,}; 
		state512 blakeS; 
		unsigned __int64 kll;
		size_t		ll;

		int iLenSub=0;
		struct	tm *pTime = NULL;
	
		pTime = current_time();
	
		if( (1==iVerbosType || 3==iVerbosType) && (TRUE == verbose) && outfile) /// 2014.08.01
		{
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
			fprintf(outfile, "BLAKE512 Hash is created at %04d/%02d/%02d/%s %02d:%02d:%02d by %s \n", pTime->tm_year, pTime->tm_mon, pTime->tm_mday,  
													WeekTXT[pTime->tm_wday], pTime->tm_hour, pTime->tm_min, pTime->tm_sec, EmailText );
			fprintf(outfile, "----------------------------------------------------------------------------------\r\n" );
		}		
	
		LOG_V("\n");
	
		if( ASTERISK_FOUND == isAsteMode )	/* For ah.exe --input *.* */
		{
			unsigned __int64	kll = 0UL;
	
			printf("BLAKE512>> BLAKE512 hashing for input files* (%d) \n", argc );

#if 0
			//for(ii=0; ii<=(argc-isAsteminus); ii++) printf("%3d : [%s] (%d:%d)\n", ii, argv[ii], isAsteminus, iFirst );
	
					
			for(ii=0; ii<BLAKE512_LEN*2; ii++) printf("-");  printf(" %d --\r\n", BLAKE512_LEN*2 );
	
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
	
			for(ii=iFirst; ii<=(argc-isAsteminus) ; ii++)
			{

				printf("FILE - [%s] : ", argv[ii] );
				if( isFileExist( argv[ii] ) ) printf(" -- OK \n");
				else printf(" --- NONE ----------------- \n");


				if( (inpfile = fopen( argv[ii], "rb")) != NULL ) 
				{
					
					memset( inbuf, 0x00, sizeof(inbuf) );
					memset( out, 0x00, sizeof(out) );
					
					//Initialize the BLAKE-384 context
					blake384_init( &blakeS ); 
					
					kll = 0ULL;
					while((ll = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
					{
						kll += ll;
					
						//Absorb input data
						blake384_update( &blakeS, inbuf, ll ); 
					
						//Digest the message
						printf("\bBLAKE384 Hashing for (%s) -> read : %lld Bytes \r", argv[ii], kll );
					}
					ClearScreen(); 
					
					//Finish absorbing phase
					blake384_final( &blakeS, out ); 
					
					
					memset( outTxt, 0x00, sizeof(outTxt) );
					for (ii = 0; ii < BLAKE384_LEN; ii++)
					{
						if( iUpper )
							sprintf(&outTxt[ii*2], (char*)"%02X", out[ii]);
						else
							sprintf(&outTxt[ii*2], (char*)"%02x", out[ii]);
					}
					
					
					if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
					{
						printf("%s	*BLAKE384*%s__(%llu) \r\n", outTxt, argv[ii], kll );
						if(outfile) fprintf(outfile, "%s  *BLAKE384*%s__(%llu) \r\n", outTxt, argv[ii], kll );
					}
					else
					{
						printf("%s	*BLAKE384*%s \r\n", outTxt, argv[ii] );
						if(outfile) fprintf(outfile, "%s  *BLAKE384*%s \r\n", outTxt, argv[ii] );
					}
					
					if(inpfile) { fclose(inpfile); inpfile=NULL; }
					if(data_buf){ free(data_buf); data_buf=NULL; }
					
					iTotSize += iFiles.size;
					multifileindex ++;
	
				}

				
			}
			printf("\nBLAKE384>> BLAKE384 Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );
#endif
			AllFilesClosed();
	
		}
		else if( multifileindex > 0 )
		{
		
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
	
			/* Find first .c file in current directory */
			if( (retFiles = _findfirsti64( extfile_name, &iFiles )) == -1L )
			{
				printf("BLAKE512>>No file [%s] in current directory!!! [%s] \n", extfile_name, infile_name );
			}
			else
			{
	
				printf("BLAKE512>> BLAKE512 hashing for input files... \n");
				//printf("-------------------------------------------------------------\r\n" );
				for(ii=0; ii<BLAKE512_LEN*2; ii++) printf("-");  printf(" %d --\r\n", BLAKE512_LEN*2 );

				do {
	
					iLenSub = strlen(iFiles.name);
	
					if( iFiles.attrib & _A_SUBDIR )
					{
	
					}
					else
					{
						if( NULL == (inpfile = fopen( iFiles.name, "rb")) ) 
						{
							beep(700,100);
							printf("\n\n[++ERROR++] Can not open multi-input file (%s) in BLAKE512 \n", iFiles.name );
	
							AllFilesClosed();
	
							exit(0);
							return 0;
						}

						memset( inbuf, 0x00, sizeof(inbuf) );
						memset( out, 0x00, sizeof(out) );
						
						//Initialize the BLAKE512 context
						blake512_init( &blakeS ); 
						
						kll = 0ULL;
						while((ll = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
						{
							kll += ll;
						
							//Absorb input data
							blake512_update( &blakeS, inbuf, ll ); 
						
							//Digest the message
							// LOG_V("\bBLAKE512 Hashing for (%s) -> read : %lld Bytes \r", iFiles.name, kll );
						}
						ClearScreen(); 
						
						//Finish absorbing phase
						blake512_final( &blakeS, out ); 
						
						
						memset( outTxt, 0x00, sizeof(outTxt) );
						for (ii = 0; ii < BLAKE512_LEN; ii++)
						{
							if( iUpper )
								sprintf(&outTxt[ii*2], (char*)"%02X", out[ii]);
							else
								sprintf(&outTxt[ii*2], (char*)"%02x", out[ii]);
						}
						
						if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
						{
							printf("%s  *%s*%s__(%llu) \r\n", outTxt, str_hash, iFiles.name, iFiles.size );
							if(outfile) fprintf(outfile, "%s  *%s*%s__(%llu) \r\n", outTxt, str_hash, iFiles.name, iFiles.size );
						}
						else
						{
							printf("%s  *%s*%s \r\n", outTxt, str_hash, iFiles.name  );
							if(outfile) fprintf(outfile, "%s  *%s*%s \r\n", outTxt, str_hash, iFiles.name );
						}
						
						if(inpfile) { fclose(inpfile); inpfile=NULL; }
						if(data_buf){ free(data_buf); data_buf=NULL; }
	
	
						iTotSize += iFiles.size;
						multifileindex ++;
					}
	
				} while( 0 == _findnexti64( retFiles, &iFiles ) );
				_findclose( retFiles );
	
				multifileindex--;
	
				printf("\nBLAKE512>> Number of Input files : %d files (Total size: %llu Byte) ---", multifileindex, iTotSize );
	
			}
		
		}
		else
		{

			printf("BLAKE512>> BLAKE512 hashing... \n");

			memset( inbuf, 0x00, sizeof(inbuf) );
			memset( out, 0x00, sizeof(out) );

			//Initialize the BLAKE512 context
			blake512_init( &blakeS ); 

			kll = 0ULL;
			while((ll = fread(inbuf, 1, sizeof(inbuf), inpfile)) > 0) 
			{
				kll += ll;

				//Absorb input data
				blake512_update( &blakeS, inbuf, ll ); 

				//Digest the message
				// LOG_V("\bBLAKE512 Hashing for (%s) -> read : %lld Bytes \r", iFiles.name, kll );
			}
			ClearScreen(); 

			//Finish absorbing phase
			blake512_final( &blakeS, out ); 
			
			memset( outTxt, 0x00, sizeof(outTxt) );
			for (ii = 0; ii < BLAKE512_LEN; ii++)
			{
				if( iUpper )
					sprintf(&outTxt[ii*2], (char*)"%02X", out[ii]);
				else
					sprintf(&outTxt[ii*2], (char*)"%02x", out[ii]);
			}

			if( (2==iVerbosType || 3==iVerbosType) && (TRUE == verbose) )
			{
				printf("%s  *%s*%s__(%llu) \r\n", outTxt, str_hash, infile_name, infile_size  );
				if(outfile) fprintf(outfile, "%s  *%s*%s__(%llu) \r\n", outTxt, str_hash, infile_name, infile_size	);
			}
			else
			{
				printf("%s  *%s*%s \r\n", outTxt, str_hash, infile_name );
				if(outfile) fprintf(outfile, "%s  *%s*%s \r\n", outTxt, str_hash, infile_name );
			}
	
			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }
			printf("\nBLAKE512>> Calculated BLAKE512 Hash Value - OK" );
	
		}
	}
#endif // BLAKE_224_256_384_512_HASH



#if IDEA_ENCIPHER_ALGORITHM 
	else if( 1==isIDEA ) /// 2014.07.26
	{
		long **K, **L;
		ushort i, j, key[8] = {1, 2, 3, 4, 5, 6, 7, 8};
		ushort X[4] = {0, 1, 2, 3}, Y[4];

		K = calloc(9, sizeof(long *));
		L = calloc(9, sizeof(long *));

		for (i = 0; i < 9; i++) 
		{
			K[i] = calloc(6, sizeof(long));
			L[i] = calloc(6, sizeof(long));

			for (j = 0; j < 6; j++) K[i][j] = L[i][j] = 0;
		}

		IDEA_encryption_key_schedule(key, K);
		IDEA_encryption(X, Y, K);
		IDEA_decryption_key_schedule(K, L);
		IDEA_encryption(Y, X, L);

		for (i = 0; i < 9; i++) 
		{
			free(K[i]);
			free(L[i]);
		}
		free(K);
		free(L);
	}
#endif /// IDEA_ENCIPHER_ALGORITHM 



#if MODIFIED_JULIAN_DATE
	else if(1 == isMJD) /// to date
	{
		printf("\n---------------- MJD (Modified Julian Date) ----------------- \n");
		
		printf("MJD>> Processing Modified Julian Date to DATE.. \n");	
		while( EOF != (c=fscanf(inpfile,"%lf", &MJDvalue)) )
		{
			ymd_hms = Convert2Timestamp( MJDvalue );
			cHex.dbl_val = MJDvalue;

			printf("MJD:[ %17s ] [",  commify(MJDvalue, str_cmBuf, 6) );
		#if CONVERT_HEX==CONVERT_HEX_MSB
			for(iTMP=0; iTMP<sizeof(double); iTMP++)
			{
				printf("%02X", cHex.dbl_bin[iTMP]);
				if(iTMP<sizeof(double)-1) printf("-");
			}
		#elif CONVERT_HEX==CONVERT_HEX_LSB
			for(iTMP=sizeof(double)-1; iTMP>=0; iTMP--)
			{
				printf("%02X", cHex.dbl_bin[iTMP]);
				if(iTMP>0) printf("-");
			}
		#endif /// CONVERT_HEX_MSB	

			printf("] -> DATE:[ %d/%02d/%02d, %02d:%02d:%02d:%03d ] \n", 
						ymd_hms.m_year, ymd_hms.m_month, ymd_hms.m_day, 
						ymd_hms.m_hour, ymd_hms.m_mins,  ymd_hms.m_secs, ymd_hms.m_millis );

			if(outfile) 
				fprintf(outfile, "MJD:[ %17s ] -> DATE:[ %d/%02d/%02d, %02d:%02d:%02d:%03d ] \r\n", 
						commify(MJDvalue, str_cmBuf, 6), ymd_hms.m_year, ymd_hms.m_month, ymd_hms.m_day, 
						ymd_hms.m_hour, ymd_hms.m_mins,  ymd_hms.m_secs, ymd_hms.m_millis );
		}


		opt_ok = 0; /* 2014.07.16 : NOTE !!! clear!!!  */

		AllFilesClosed();

		printf("\nMJD>> Converted MJD(Modified Julian Date) to DATE - OK" );


	}
	else if(2 == isMJD) /// to MJD
	{

		printf("\n---------------- MJD (Modified Julian Date) ----------------- \n");
		
		printf("MJD>> Processing Modified Julian Date to MJD number.. \n");	

		ymd_hms.m_millis = 0;
		while( EOF != (c=fscanf(inpfile,"%d %d %d %d %d %d", 
				&ymd_hms.m_year, &ymd_hms.m_month, &ymd_hms.m_day, 
				&ymd_hms.m_hour, &ymd_hms.m_mins, &ymd_hms.m_secs )) )
		{
			MJDvalue = Convert2MJD( ymd_hms );
			cHex.dbl_val = MJDvalue;

			printf("DATE:[ %d/%02d/%02d, %02d:%02d:%02d:%03d ] -> MJD:[ %17s ] [", 
						ymd_hms.m_year, ymd_hms.m_month, ymd_hms.m_day, 
						ymd_hms.m_hour, ymd_hms.m_mins,  ymd_hms.m_secs, ymd_hms.m_millis, commify(MJDvalue, str_cmBuf, 6) );

		#if CONVERT_HEX==CONVERT_HEX_MSB
			for(iTMP=0; iTMP<sizeof(double); iTMP++)
			{
				printf("%02X", cHex.dbl_bin[iTMP]);
				if(iTMP<sizeof(double)-1) printf("-");
			}
		#elif CONVERT_HEX==CONVERT_HEX_LSB
			for(iTMP=sizeof(double)-1; iTMP>=0; iTMP--)
			{
				printf("%02X", cHex.dbl_bin[iTMP]);
				if(iTMP>0) printf("-");
			}
		#endif /// CONVERT_HEX_MSB	
			printf("]\n");

			fprintf(outfile, "DATE:[ %d/%02d/%02d, %02d:%02d:%02d:%03d ] -> MJD:[ %17s ] \r\n", 
						ymd_hms.m_year, ymd_hms.m_month, ymd_hms.m_day, 
						ymd_hms.m_hour, ymd_hms.m_mins,  ymd_hms.m_secs, ymd_hms.m_millis, commify(MJDvalue, str_cmBuf, 6) );
		}


		opt_ok = 0; /* 2014.07.16 : NOTE !!! clear!!!  */

		AllFilesClosed();

		printf("\nMJD>> Converted DATE to MJD(Modified Julian Date) - OK" );


	}
#endif /// MODIFIED_JULIAN_DATE

	else if( (1==isFileMerge) && (3==isJoin) ) // 2017.04.05
	{

		if( 0==strcasecmp(sefile_name, outfile_name) )
		{
			beep(700,100);
			printf("\n\n[++ERROR++] DO check 2nd input[%s] and output[%s]. Same!!! \n",sefile_name, outfile_name  );

			AllFilesClosed();
		
			exit(0); /// help();
			return 0;
		} 


		/// Attached Header --- 2014.07.15
		if(inpfile && outfile)
		{
			unsigned char Siz2ndTmp[4+1] = {0,};
			unsigned char Siz2ndBuf[4+1] = {0,};
			unsigned char read_buf[BUFSIZE] = {0,};
			unsigned __int64       first_file_tot_size = 0UL;
			unsigned int  sec_file_tot_size = 0;
			unsigned int  ipad_byte_cnt = 0;
			int aa=0, isum=0, totsum=0, iago=0, iidx=0;


			memset( Siz2ndTmp, 0x00, sizeof(Siz2ndTmp) );  
			memset( Siz2ndBuf, 0x00, sizeof(Siz2ndBuf) );  
			memset( read_buf, 0x00, sizeof(read_buf) ); /// initial

			// --- 1st file created
			first_file_tot_size = 0LL;
			while( fr_size = fread(read_buf, sizeof(unsigned char), BUFSIZE, inpfile ) )
			{
				fwrite(read_buf, sizeof(unsigned char), fr_size, outfile); 
				first_file_tot_size += fr_size;
			}

			if(inpfile) { fclose(inpfile); inpfile=NULL; }
			if(data_buf){ free(data_buf); data_buf=NULL; }


			// ----------------------------------------------------------
			// 2nd file의 size을 구하여 1st file의 마지막에 붙여라...
			// ----------------------------------------------------------
			if(is2ndFileSize)
			{

				// --------------------------------------------------
				// --------------------------------------------------
				// --- 2nd file is read because of 2nd file size -----------
				/* ========== INPUT FILE ================= */
				if( NULL == (inpfile = fopen( sefile_name, "rb")) ) 
				{
					beep(700,100);
					printf("\n\n[++ERROR++] Can not open 2nd input file[%s]. \n",sefile_name);

					AllFilesClosed();

					exit(0); /// help();
					return 0;
				}
				else
				{
					/// OK ---
					if (fstat(fileno(inpfile), &file_statbuf) < 0) 
					{ 
						printf("\n\n[++ERROR++]Can not stat 2nd file:[%s]\n", sefile_name ); 

						AllFilesClosed(); // 2020.07.10

						exit(0); /// 2017.11.21
						return 2; 
					} 	 
				}
				/* ========== INPUT FILE ================= */


				// -----------------------------------------------
				// 2nd file size ? 
				// -----------------------------------------------
				sec_file_tot_size = 0;
				while( fr_size = fread(read_buf, sizeof(unsigned char), BUFSIZE, inpfile ) )
				{
					sec_file_tot_size += fr_size;
				}

				//printf("\n>>2nd file size : 0x%08x (%d Byte)", sec_file_tot_size, sec_file_tot_size );
				if(inpfile) { fclose(inpfile); inpfile=NULL; }
				if(data_buf){ free(data_buf); data_buf=NULL; }
				// --------------------------------------------------
				// --------------------------------------------------



				// ----------------------------------------
				// 1) 0xFF padding : 4Byte 는 2nd file size를 위해 pad에서 제외...
				// ----------------------------------------
				ipad_byte_cnt = 0; // initial
				while( first_file_tot_size < iFileLimit - 4 ) // j option limit size (hexa)
				{
					fprintf(outfile,"%c", Pad_Byte); // 2020.06.23 <- 0xFF);
					first_file_tot_size++;
					ipad_byte_cnt ++;
				}

				if( ipad_byte_cnt > 0 )
				{
					if( ipad_byte_cnt>>20 )
						printf("\n>>Fill Pad byte   : (0x%02X) Size: %.3f MB (%#x)", Pad_Byte, (ipad_byte_cnt/1024.0)/1024.0, ipad_byte_cnt );
					else if( ipad_byte_cnt>>10 )
						printf("\n>>Fill Pad byte   : (0x%02X) Size: %.3f kB (%#x)", Pad_Byte, (ipad_byte_cnt/1024.0), ipad_byte_cnt );
					else 
						printf("\n>>Fill Pad byte   : (0x%02X) Size: %lu Byte (%#x)", Pad_Byte, ipad_byte_cnt, ipad_byte_cnt );
				}
				

				// ----------------------------------------
				// 2) size : bottom 4 byte, 마지막 4byte는 2nd file의 file size
				// itoa(int val, char * buf, int radix);
				// ----------------------------------------
				//itoa(sec_file_tot_size, Siz2ndBuf, 16);
				//fprintf(outfile,"%s",Siz2ndBuf );
				
				sprintf(Siz2ndBuf,"%08x",sec_file_tot_size );
				printf("\n>>2nd file size   : %#04x (%d Byte)", sec_file_tot_size, sec_file_tot_size); // , Siz2ndBuf );

		#if 1
				iago = 0;
				totsum = 0;

				for( aa=0; aa<8;aa++)
				{
					if(Siz2ndBuf[aa]>='0' && Siz2ndBuf[aa]<='9')
					{
						isum |= Siz2ndBuf[aa]-0x30;
						iago ++;
					}
					else if(Siz2ndBuf[aa]>='A' && Siz2ndBuf[aa]<='F') 
					{
						//fprintf(outfile,"%c",Siz2ndBuf[aa]-0x41+10 );
						isum |= Siz2ndBuf[aa]-0x41+10;
						iago ++;
					}
					else if(Siz2ndBuf[aa]>='a' && Siz2ndBuf[aa]<='f') 
					{
						//fprintf(outfile,"%c",Siz2ndBuf[aa]-0x61+10 );
						isum |= Siz2ndBuf[aa]-0x61+10;
						iago ++;
					}
					else 
					{
						printf("\n>>2nd file size : Error check for out of range!!\n");
						fprintf(outfile,"%c",'Z' );
					}

					if( iago == 2 )
					{
						if( ENDIAN_BIG == is2ndEndian ) // Big Endian
							fprintf(outfile,"%c",isum );
						else if( ENDIAN_LITTLE == is2ndEndian ) // Little Endian
							sprintf(&Siz2ndTmp[iidx],"%c",isum );


						totsum += isum;

						//printf("\n>>[%c] [%x] %d totsum:[0x%x]\n", isum, isum, isum, totsum);
						iidx ++;

						isum = 0;
						iago = 0;
					}
					else
					{
						isum <<= 4;
					}

				}


				//if( iidx > 4 )
				//{
				//	printf("\n>>++ERROR++ 2nd file size field (4Byte) is over!!! [0x%x] \n", totsum);
				//}

				if( ENDIAN_LITTLE == is2ndEndian ) // Little Endian
				{
					for( aa=3; aa>=0;aa--)
					{
						fprintf(outfile,"%c",Siz2ndTmp[aa] );
					}
				}
		#else
				fprintf(outfile,"%s",Siz2ndBuf );

		#endif


			}
			else
			{
				ipad_byte_cnt = 0; // initial
				while( first_file_tot_size < iFileLimit ) // j option limit size (hexa)
				{
					fprintf(outfile,"%c", Pad_Byte); // <-- 2020.06.23 0xFF);
					first_file_tot_size++;
					ipad_byte_cnt ++;
				}


				if( ipad_byte_cnt > 0 )
				{
					if( ipad_byte_cnt>>20 )
						printf("\n>>Fill Pad byte   : (0x%02X) size: %.3f MB (%#x)", Pad_Byte, (ipad_byte_cnt/1024.0)/1024.0, ipad_byte_cnt );
					else if( ipad_byte_cnt>>10 )
						printf("\n>>Fill Pad byte   : (0x%02X) size: %.3f kB (%#x)", Pad_Byte, (ipad_byte_cnt/1024.0), ipad_byte_cnt );
					else 
						printf("\n>>Fill Pad byte   : (0x%02X) size: %lu Byte (%#x)", Pad_Byte, ipad_byte_cnt, ipad_byte_cnt );
				
				}
			}


			// --- 2nd file is added -----------
			/* ========== INPUT FILE ================= */
			if( NULL == (inpfile = fopen( sefile_name, "rb")) ) 
			{
				beep(700,100);
				printf("\n\n[++ERROR++] Can not open input file[%s]. \n",sefile_name);

				AllFilesClosed();

				exit(0); /// help();
				return 0;
			}
			else
			{
				/// OK ---
				if (fstat(fileno(inpfile), &file_statbuf) < 0) 
				{ 
					printf("\n\n[++ERROR++]Cannot stat 2nd file:[%s]\n", sefile_name ); 

					AllFilesClosed(); // 2020.07.10
					exit(0); /// 2017.11.21
					return 2; 
				} 	 
			}
			/* ========== INPUT FILE ================= */


			// -----------------------------------------------
			// append 1st file to 2nd-file
			// -----------------------------------------------
			first_file_tot_size = 0UL;
			while( fr_size = fread(read_buf, sizeof(unsigned char), BUFSIZE, inpfile ) )
			{
				fwrite(read_buf, sizeof(unsigned char), fr_size, outfile); 
				first_file_tot_size += fr_size;
			}

			printf("\n>>Merged Total file size : %lld (0x%x) Bytes", first_file_tot_size, first_file_tot_size );


		}
		else
		{
			printf("\nHDR>> Can not open input file[%s] or output file[%s] \n", infile_name, outfile_name);
		}

	}
	else if( (1==isFileMerge) && (3!=isJoin) ) // 2017.11.28
	{
		printf("\n\n[++ERROR++] DO check 2nd input[%s] for merging file!! \n",sefile_name );
	}
	else if(1==isRandomNum)
	{
		int ran;
		int caa=0, cbb=0, cdd=0, ccc=0, cee=0, cff=0;
		int c00=0, c11=0, c22=0, c77=0, c88=0, c99=0;

		srand( (unsigned int)time(NULL)+(unsigned int)getpid() );

		fprintf(stderr,"\n\n== Random Generator == (0x00~0xff: Count:%d) SEED:%d %d\n", iRanMaxi, time(NULL), getpid());
		if(outfile) fprintf(outfile,"\n\n== Random Generator == (0x00~0xff: Count:%d) SEED:%d %d\n", iRanMaxi, time(NULL), getpid());

		for ( idx = 1; idx <= iRanMaxi; idx++ )
		{
			ran = rand()%0x100;

			fprintf(stderr, "%02x ",  ran );
			if(outfile) fprintf(outfile,"%02x ", ran);

			if( 0xff == ran ) cff++;
			if( 0xee == ran ) cee++;
			if( 0xdd == ran ) cdd++;
			if( 0xcc == ran ) ccc++;
			if( 0xbb == ran ) cbb++;
			if( 0xaa == ran ) caa++;

			if( 0x10 == ran ) c00++;
			if( 0x11 == ran ) c11++;
			if( 0x22 == ran ) c22++;
			if( 0x77 == ran ) c77++;
			if( 0x88 == ran ) c88++;
			if( 0x99 == ran ) c99++;

			if( (0==idx%20) ) 
			{
				fprintf(stderr,"\n");
				if(outfile) fprintf(outfile,"\n");
			}
		}

		AllFilesClosed();
		
		fprintf(stderr,"\n\n");
		fprintf(stderr,"10:%d, 11:%d, 22:%d, 77:%d, 88:%d, 99:%d \n", c00, c11, c22, c77, c88, c99);
		fprintf(stderr,"aa:%d, bb:%d, cc:%d, dd:%d, ee:%d, ff:%d \n", caa, cbb, ccc, cdd, cee, cff);
	}
	else 
	{
		//printf("\n\n[++ERROR++] DO check -option");
	}
	printf("\r\n");




	/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
	/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

	AllFilesClosed();
	exit(0);

	/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
	/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

	return(0);
}



