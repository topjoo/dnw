/***************************************************************************
* Copyright (c) 2000-2004, Future Systems, Inc. / Seoul, Republic of Korea *
* All Rights Reserved.                                                     *
*                                                                          *
* This document contains proprietary and confidential information.  No     *
* parts of this document or the computer program it embodies may be in     *
* any way copied, duplicated, reproduced, translated into a different      *
* programming language, or distributed to any person, company, or          *
* corporation without the prior written consent of Future Systems, Inc.    *
*                              Hyo Sun Hwang                               *
*                372-2 YangJae B/D 6th Floor, Seoul, Korea                 *
*                           +82-2-578-0581 (552)                           *
***************************************************************************/

/*--------------------- [ Version/Command in detais] ---------------------*\
Description : has160.h
			(head file) head file for has160.c : Hash Algorithm HAS160

C0000 : Created by Hyo Sun Hwang (hyosun@future.co.kr) 2000/12/31

C0001 : Modified by Hyo Sun Hwang (hyosun@future.co.kr) 2000/00/00

\*------------------------------------------------------------------------*/

#ifndef _HAS160_H
#define _HAS160_H

/*************** Header files *********************************************/
#include <stdlib.h>
#include <string.h>


#ifdef __cplusplus
extern "C"
{
#endif


/*************** Assertions ***********************************************/
////////	Define the Endianness	////////
#undef HAS160_BIG_ENDIAN
#undef HAS160_LITTLE_ENDIAN

#if defined(USER_BIG_ENDIAN)
	#define HAS160_BIG_ENDIAN
#elif defined(USER_LITTLE_ENDIAN)
	#define HAS160_LITTLE_ENDIAN
#else
	#if 0
		#define HAS160_BIG_ENDIAN		//	Big-Endian machine with pointer casting
	#elif defined(_MSC_VER)
		#define HAS160_LITTLE_ENDIAN	//	Little-Endian machine with pointer casting
	#else
		#error
	#endif
#endif

/*************** Macros ***************************************************/
////////	rotate by using shift operations	////////
#if defined(_MSC_VER)
	#define ROTL_DWORD(x, n) _lrotl((x), (n))
	#define ROTR_DWORD(x, n) _lrotr((x), (n))
#else
	#define ROTL_DWORD(x, n) ( (DWORD)((x) << (n)) | (DWORD)((x) >> (32-(n))) )
	#define ROTR_DWORD(x, n) ( (DWORD)((x) >> (n)) | (DWORD)((x) << (32-(n))) )
#endif

////////	reverse the byte order of DWORD(DWORD:4-bytes integer) and WORD.
#define ENDIAN_REVERSE_DWORD(dwS)	( (ROTL_DWORD((dwS),  8) & 0x00ff00ff)	\
									 | (ROTL_DWORD((dwS), 24) & 0xff00ff00) )

////////	move DWORD type to BYTE type and BYTE type to DWORD type
#if defined(HAS160_BIG_ENDIAN)		////	Big-Endian machine
	#define BIG_B2D(B, D)		D = *(DWORD *)(B)
	#define BIG_D2B(D, B)		*(DWORD *)(B) = (DWORD)(D)
	#define LITTLE_B2D(B, D)	D = ENDIAN_REVERSE_DWORD(*(DWORD *)(B))
	#define LITTLE_D2B(D, B)	*(DWORD *)(B) = ENDIAN_REVERSE_DWORD(D)
#elif defined(HAS160_LITTLE_ENDIAN)	////	Little-Endian machine
	#define BIG_B2D(B, D)		D = ENDIAN_REVERSE_DWORD(*(unsigned int *)(B))
	#define BIG_D2B(D, B)		*(unsigned int *)(B) = ENDIAN_REVERSE_DWORD(D)
	#define LITTLE_B2D(B, D)	D = *(unsigned int *)(B)
	#define LITTLE_D2B(D, B)	*(unsigned int *)(B) = (unsigned int)(D)
#else
	#error ERROR : Invalid DataChangeType
#endif

/*************** Definitions / Macros  ************************************/
////	HAS160에 관련된 상수들
#define HAS160_DIGEST_BLOCKLEN	64		//	in bytes
#define HAS160_DIGEST_VALUELEN	20		//	in bytes

/*************** New Data Types *******************************************/

////////	Determine data types depand on the processor and compiler.
//#define BOOL	int					//	1-bit data type
//#define BYTE	unsigned char		//	unsigned 1-byte data type
//#define WORD	unsigned short int	//	unsigned 2-bytes data type
//#define DWORD	unsigned int		//	unsigned 4-bytes data type
//#define RET_VAL		DWORD			//	return values

////	HAS160..
typedef struct{
	unsigned int		ChainVar[HAS160_DIGEST_VALUELEN/4];	//	Chaining Variable 저장
	unsigned int		Count[4];							//	
	unsigned char		Buffer[HAS160_DIGEST_BLOCKLEN];		//	Buffer for unfilled block
} HAS160_ALG_INFO;

/*************** Constant (Error Code) ************************************/
////	Error Code - 정리하고, 적당히 출력해야 함.
//#define CTR_SUCCESS					0

/*************** Prototypes ***********************************************/
////	Init/Update/Final 형식을 복호화.
void	HAS160_Init(
		HAS160_ALG_INFO	*AlgInfo);
void	HAS160_Update(
		HAS160_ALG_INFO	*AlgInfo,
		unsigned char			*Message,		//	데이타가 입력됨
		unsigned int			MessageLen);
void	HAS160_Final(
		HAS160_ALG_INFO	*AlgInfo,
		unsigned char			*Digest);		//	해쉬값이 출력됨


#ifdef __cplusplus
}
#endif

/*************** END OF FILE **********************************************/
#endif	//	_HAS160_H
