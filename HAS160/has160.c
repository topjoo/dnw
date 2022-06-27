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
Description : seed.c
			(C-source file) Hash Algorithm HAS160

C0000 : Created by Hyo Sun Hwang (hyosun@future.co.kr) 2000/12/31

C0001 : Modified by Hyo Sun Hwang (hyosun@future.co.kr) 2000/00/00

\*------------------------------------------------------------------------*/

//Dependencies
#define STRICT
#define WIN32_LEAN_AND_MEAN

//#include <windows.h>
#include <stdio.h>
#include <string.h>


/*************** Header files *********************************************/
#include "has160.h"



#ifdef __cplusplus
extern "C"
{
#endif


/*************** Assertions ***********************************************/

/*************** Definitions / Macros  ************************************/

/*************** New Data Types *******************************************/

/*************** Global Variables *****************************************/

/*************** Prototypes ***********************************************/

/*************** Macros ***************************************************/
//	boolean functions
#define F(x, y, z)  ( (z)^( (x) & ((y)^(z)) ) )
#define G(x, y, z)  ( (x) ^ (y) ^ (z) )
#define H(x, y, z)  ( (y)^( (x) | ~(z) ) )

//	step operations
#define FF(a, b, c, d, e, x, s) {					\
	(e) += ROTL_DWORD(a, s) + F(b, c, d) + (x);		\
	(b) = ROTL_DWORD(b, 10);						\
}
#define GG(a, b, c, d, e, x, s) {					\
	(e) += ROTL_DWORD(a, s) + G(b, c, d) + (x) + K;	\
	(b) = ROTL_DWORD(b, 17);						\
}
#define HH(a, b, c, d, e, x, s) {					\
	(e) += ROTL_DWORD(a, s) + H(b, c, d) + (x) + K;	\
	(b) = ROTL_DWORD(b, 25);						\
}
#define II(a, b, c, d, e, x, s) {					\
	(e) += ROTL_DWORD(a, s) + G(b, c, d) + (x) + K;	\
	(b) = ROTL_DWORD(b, 30);						\
}

/*************** Function *************************************************
*	HAS160 core fuction
*/
static void HAS160_Transform(unsigned int *Y, unsigned int *DigestValue)
{
	unsigned int	a, b, c, d, e, T[16], K;

#if defined(HAS160_BIG_ENDIAN)
	DWORD	ZZ[16];

	ZZ[ 0] = ENDIAN_REVERSE_DWORD(Y[ 0]);
	ZZ[ 1] = ENDIAN_REVERSE_DWORD(Y[ 1]);
	ZZ[ 2] = ENDIAN_REVERSE_DWORD(Y[ 2]);
	ZZ[ 3] = ENDIAN_REVERSE_DWORD(Y[ 3]);
	ZZ[ 4] = ENDIAN_REVERSE_DWORD(Y[ 4]);
	ZZ[ 5] = ENDIAN_REVERSE_DWORD(Y[ 5]);
	ZZ[ 6] = ENDIAN_REVERSE_DWORD(Y[ 6]);
	ZZ[ 7] = ENDIAN_REVERSE_DWORD(Y[ 7]);
	ZZ[ 8] = ENDIAN_REVERSE_DWORD(Y[ 8]);
	ZZ[ 9] = ENDIAN_REVERSE_DWORD(Y[ 9]);
	ZZ[10] = ENDIAN_REVERSE_DWORD(Y[10]);
	ZZ[11] = ENDIAN_REVERSE_DWORD(Y[11]);
	ZZ[12] = ENDIAN_REVERSE_DWORD(Y[12]);
	ZZ[13] = ENDIAN_REVERSE_DWORD(Y[13]);
	ZZ[14] = ENDIAN_REVERSE_DWORD(Y[14]);
	ZZ[15] = ENDIAN_REVERSE_DWORD(Y[15]);
#else
	unsigned int	*ZZ=Y;
#endif

	T[ 0] = ZZ[ 8] ^ ZZ[ 9] ^ ZZ[10] ^ ZZ[11];
	T[ 1] = ZZ[12] ^ ZZ[13] ^ ZZ[14] ^ ZZ[15];
	T[ 2] = ZZ[ 0] ^ ZZ[ 1] ^ ZZ[ 2] ^ ZZ[ 3];
	T[ 3] = ZZ[ 4] ^ ZZ[ 5] ^ ZZ[ 6] ^ ZZ[ 7];

	T[ 4] = ZZ[11] ^ ZZ[14] ^ ZZ[ 1] ^ ZZ[ 4];
	T[ 5] = ZZ[ 7] ^ ZZ[10] ^ ZZ[13] ^ ZZ[ 0];
	T[ 6] = ZZ[ 3] ^ ZZ[ 6] ^ ZZ[ 9] ^ ZZ[12];
	T[ 7] = ZZ[15] ^ ZZ[ 2] ^ ZZ[ 5] ^ ZZ[ 8];

	T[ 8] = ZZ[ 4] ^ ZZ[13] ^ ZZ[ 6] ^ ZZ[15];
	T[ 9] = ZZ[ 8] ^ ZZ[ 1] ^ ZZ[10] ^ ZZ[ 3];
	T[10] = ZZ[12] ^ ZZ[ 5] ^ ZZ[14] ^ ZZ[ 7];
	T[11] = ZZ[ 0] ^ ZZ[ 9] ^ ZZ[ 2] ^ ZZ[11];

	T[12] = ZZ[15] ^ ZZ[10] ^ ZZ[ 5] ^ ZZ[ 0];
	T[13] = ZZ[11] ^ ZZ[ 6] ^ ZZ[ 1] ^ ZZ[12];
	T[14] = ZZ[ 7] ^ ZZ[ 2] ^ ZZ[13] ^ ZZ[ 8];
	T[15] = ZZ[ 3] ^ ZZ[14] ^ ZZ[ 9] ^ ZZ[ 4];

	//
	a=DigestValue[0];
	b=DigestValue[1];
	c=DigestValue[2];
	d=DigestValue[3];
	e=DigestValue[4];

	//	round 1
	FF(a, b, c, d, e, T[ 0],  5);
	FF(e, a, b, c, d, ZZ[ 0], 11);
	FF(d, e, a, b, c, ZZ[ 1],  7);
	FF(c, d, e, a, b, ZZ[ 2], 15);
	FF(b, c, d, e, a, ZZ[ 3],  6);
	FF(a, b, c, d, e, T[ 1], 13);
	FF(e, a, b, c, d, ZZ[ 4],  8);
	FF(d, e, a, b, c, ZZ[ 5], 14);
	FF(c, d, e, a, b, ZZ[ 6],  7);
	FF(b, c, d, e, a, ZZ[ 7], 12);
	FF(a, b, c, d, e, T[ 2],  9);
	FF(e, a, b, c, d, ZZ[ 8], 11);
	FF(d, e, a, b, c, ZZ[ 9],  8);
	FF(c, d, e, a, b, ZZ[10], 15);
	FF(b, c, d, e, a, ZZ[11],  6);
	FF(a, b, c, d, e, T[ 3], 12);
	FF(e, a, b, c, d, ZZ[12],  9);
	FF(d, e, a, b, c, ZZ[13], 14);
	FF(c, d, e, a, b, ZZ[14],  5);
	FF(b, c, d, e, a, ZZ[15], 13);

	//	round 2
	K = 0x5A827999;
	GG(a, b, c, d, e, T[ 4],  5);
	GG(e, a, b, c, d, ZZ[ 3], 11);
	GG(d, e, a, b, c, ZZ[ 6],  7);
	GG(c, d, e, a, b, ZZ[ 9], 15);
	GG(b, c, d, e, a, ZZ[12],  6);
	GG(a, b, c, d, e, T[ 5], 13);
	GG(e, a, b, c, d, ZZ[15],  8);
	GG(d, e, a, b, c, ZZ[ 2], 14);
	GG(c, d, e, a, b, ZZ[ 5],  7);
	GG(b, c, d, e, a, ZZ[ 8], 12);
	GG(a, b, c, d, e, T[ 6],  9);
	GG(e, a, b, c, d, ZZ[11], 11);
	GG(d, e, a, b, c, ZZ[14],  8);
	GG(c, d, e, a, b, ZZ[ 1], 15);
	GG(b, c, d, e, a, ZZ[ 4],  6);
	GG(a, b, c, d, e, T[ 7], 12);
	GG(e, a, b, c, d, ZZ[ 7],  9);
	GG(d, e, a, b, c, ZZ[10], 14);
	GG(c, d, e, a, b, ZZ[13],  5);
	GG(b, c, d, e, a, ZZ[ 0], 13);

	//	round 3
	K = 0x6ED9EBA1;
	HH(a, b, c, d, e, T[ 8],  5);
	HH(e, a, b, c, d, ZZ[12], 11);
	HH(d, e, a, b, c, ZZ[ 5],  7);
	HH(c, d, e, a, b, ZZ[14], 15);
	HH(b, c, d, e, a, ZZ[ 7],  6);
	HH(a, b, c, d, e, T[ 9], 13);
	HH(e, a, b, c, d, ZZ[ 0],  8);
	HH(d, e, a, b, c, ZZ[ 9], 14);
	HH(c, d, e, a, b, ZZ[ 2],  7);
	HH(b, c, d, e, a, ZZ[11], 12);
	HH(a, b, c, d, e, T[10],  9);
	HH(e, a, b, c, d, ZZ[ 4], 11);
	HH(d, e, a, b, c, ZZ[13],  8);
	HH(c, d, e, a, b, ZZ[ 6], 15);
	HH(b, c, d, e, a, ZZ[15],  6);
	HH(a, b, c, d, e, T[11], 12);
	HH(e, a, b, c, d, ZZ[ 8],  9);
	HH(d, e, a, b, c, ZZ[ 1], 14);
	HH(c, d, e, a, b, ZZ[10],  5);
	HH(b, c, d, e, a, ZZ[ 3], 13);

	//	round 4
	K = 0x8F1BBCDC;
	II(a, b, c, d, e, T[12],  5);
	II(e, a, b, c, d, ZZ[ 7], 11);
	II(d, e, a, b, c, ZZ[ 2],  7);
	II(c, d, e, a, b, ZZ[13], 15);
	II(b, c, d, e, a, ZZ[ 8],  6);
	II(a, b, c, d, e, T[13], 13);
	II(e, a, b, c, d, ZZ[ 3],  8);
	II(d, e, a, b, c, ZZ[14], 14);
	II(c, d, e, a, b, ZZ[ 9],  7);
	II(b, c, d, e, a, ZZ[ 4], 12);
	II(a, b, c, d, e, T[14],  9);
	II(e, a, b, c, d, ZZ[15], 11);
	II(d, e, a, b, c, ZZ[10],  8);
	II(c, d, e, a, b, ZZ[ 5], 15);
	II(b, c, d, e, a, ZZ[ 0],  6);
	II(a, b, c, d, e, T[15], 12);
	II(e, a, b, c, d, ZZ[11],  9);
	II(d, e, a, b, c, ZZ[ 6], 14);
	II(c, d, e, a, b, ZZ[ 1],  5);
	II(b, c, d, e, a, ZZ[12], 13);

	//	chaining variables update
	DigestValue[0] += a;
	DigestValue[1] += b;
	DigestValue[2] += c;
	DigestValue[3] += d;
	DigestValue[4] += e;
}

/*************** Function *************************************************
*	
*/
void	HAS160_Init(
		HAS160_ALG_INFO	*AlgInfo)
{
	//
	AlgInfo->ChainVar[0] = 0x67452301;
	AlgInfo->ChainVar[1] = 0xefcdab89;
	AlgInfo->ChainVar[2] = 0x98badcfe;
	AlgInfo->ChainVar[3] = 0x10325476;
	AlgInfo->ChainVar[4] = 0xC3D2E1F0;

	//
	AlgInfo->Count[0] = AlgInfo->Count[1] = 0;
}

/*************** Function *************************************************
*	
*/
void	HAS160_Update(
		HAS160_ALG_INFO	*AlgInfo,
		unsigned char			*Message,		//	input Message
		unsigned int			MessageLen)		//	in BYTEs
{
	unsigned int		RemainedLen, PartLen;

	//	Compute the number of hashed bytes mod HAS160_DIGEST_BLOCKLEN
	RemainedLen = (AlgInfo->Count[0] >> 3) % HAS160_DIGEST_BLOCKLEN;
	//	compute the number of bytes that can be filled up
	PartLen = HAS160_DIGEST_BLOCKLEN - RemainedLen;

	//	Update Count (number of toatl data bits)
	if( (AlgInfo->Count[0] += (MessageLen << 3)) < AlgInfo->Count[0] )
		AlgInfo->Count[1]++;
	AlgInfo->Count[1] += (MessageLen >> 29);

	//	핵심 updtae 부분
	if( MessageLen>=PartLen ) {
		memcpy(AlgInfo->Buffer+RemainedLen, Message, (int)PartLen);
		HAS160_Transform((unsigned int *)AlgInfo->Buffer, AlgInfo->ChainVar);

		Message += PartLen;
		MessageLen -= PartLen;
		RemainedLen = 0;

		while( MessageLen>=HAS160_DIGEST_BLOCKLEN ) {
			if( (((int)Message)%4)==0 ) {	//	Speed up technique
				HAS160_Transform((unsigned int *)Message, AlgInfo->ChainVar);
			}
			else {
				memcpy((unsigned char *)AlgInfo->Buffer, Message, (int)HAS160_DIGEST_BLOCKLEN);
				HAS160_Transform((unsigned int *)AlgInfo->Buffer, AlgInfo->ChainVar);
			}
			Message += HAS160_DIGEST_BLOCKLEN;
			MessageLen -= HAS160_DIGEST_BLOCKLEN;
		}
	}

	//	Buffer remaining input
	memcpy((unsigned char *)AlgInfo->Buffer+RemainedLen, Message, (int)MessageLen);
}

/*************** Function *************************************************
*	
*/
void	HAS160_Final(
		HAS160_ALG_INFO	*AlgInfo,
		unsigned char			*Digest)	//	output Hash Value
{
	unsigned int		i, dwIndex, CountL, CountH;

	//	마지박 블록 처리
	CountL = AlgInfo->Count[0];
	CountH = AlgInfo->Count[1];
	dwIndex = (CountL >> 3) % HAS160_DIGEST_BLOCKLEN;
	AlgInfo->Buffer[dwIndex++] = 0x80;

	if(dwIndex>HAS160_DIGEST_BLOCKLEN-8) {
		memset((unsigned char *)AlgInfo->Buffer + dwIndex, 0, (int)(HAS160_DIGEST_BLOCKLEN-dwIndex));

		HAS160_Transform((unsigned int *)AlgInfo->Buffer, AlgInfo->ChainVar);

		memset((unsigned char *)AlgInfo->Buffer, 0, (int)HAS160_DIGEST_BLOCKLEN-8);
	}
	else
		memset((unsigned char *)AlgInfo->Buffer+dwIndex, 0, (int)(HAS160_DIGEST_BLOCKLEN-dwIndex-8));

	//	출력
#if defined(HAS160_BIG_ENDIAN)
	CountL = ENDIAN_REVERSE_DWORD(CountL);
	CountH = ENDIAN_REVERSE_DWORD(CountH);
#endif
	((unsigned int *)AlgInfo->Buffer)[HAS160_DIGEST_BLOCKLEN/4-2] = CountL;
	((unsigned int *)AlgInfo->Buffer)[HAS160_DIGEST_BLOCKLEN/4-1] = CountH;

	HAS160_Transform((unsigned int *)AlgInfo->Buffer, AlgInfo->ChainVar);

	for( i=0; i<HAS160_DIGEST_VALUELEN; i+=4)
		LITTLE_D2B((AlgInfo->ChainVar)[i/4], &(Digest[i]));
}

/*************** END OF FILE **********************************************/


#ifdef __cplusplus
}
#endif


