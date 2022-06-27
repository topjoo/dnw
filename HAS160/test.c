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
Description : filename.? (CrypLib version 1.00)
			(head file)	(C-source file)

C0000 : Created by Hyo Sun Hwang (hyosun@future.co.kr) 2000/12/31

C0001 : Modified by Hyo Sun Hwang (hyosun@future.co.kr) 2001/01/01

\*------------------------------------------------------------------------*/

/*************** Header files *********************************************/
#include <stdio.h>
#include <time.h>
#include "has160.h"

/*************** Assertions ***********************************************/

/*************** Definitions / Macros  ************************************/
#define KILO	1024
#define MEGA	(KILO*KILO)

////	Prine out BYTE data in ascending order and with no '0x'(hexa type)
#undef PrintBYTE
#define PrintBYTE(pfile, msg, Data, DataLen) {			\
	int idx;											\
	fprintf(pfile, "%5s =", msg);						\
	for( idx=0; idx<(int)DataLen; idx++) {				\
		if( (idx==0) || ((idx%16)!=0) )					\
			fprintf(pfile, " %.2x", Data[idx]);			\
		else											\
			fprintf(pfile, "\n\t%.2x", Data[idx]);		\
	}													\
	fprintf(pfile, "\n");								\
}

////	한번에 'ByteLen'-bytes의 데이타를 처리하는 연산 'Oper'을
////	'Iter'번 수행하고, 그 결과를 KByte/sec단위로 출력함.
#define SPEED_TEST(str1, str2, Iter, ByteLen, Oper) {	\
	unsigned idx;										\
	clock_t start, finish;								\
	double Sec, Mbps=0.0;								\
	start = clock();									\
	for( idx=0; idx<Iter; idx++)						\
		{	Oper;	}									\
	finish = clock();									\
	Sec = (double)(finish-start) / CLOCKS_PER_SEC;		\
	if( Sec!=0 )	Mbps = 8.0*ByteLen*idx/Sec/MEGA;	\
	printf("%s%7.3fMbps(=%d*", str1, Mbps, Iter);		\
	printf("%d", ByteLen);								\
	printf("/%3.0f)%s", 1000.0*Sec, str2);				\
}

/*************** New Data Types *******************************************/

/*************** Constant (Error Code) ************************************/
#define CTR_USAGE_ERROR		0x2001

/*************** Global Variables *****************************************/
char	Help[] = "\
Usage1 : -[T/S]   (Test Value/Test Speed)\n\
Usage2 : -[H] infile\n";

/*************** Prototypes ***********************************************/

//########################################################//
//######		Error Code 관리 함수				######//
//########################################################//

/*************************************************************************/
typedef struct{
	DWORD		ErrorCode;
	BYTE		Message[32];
} ERROR_MESSAGE;

ERROR_MESSAGE	ErrorMessage[] = {
	{CTR_USAGE_ERROR,		"CTR_USAGE_ERROR"},
	{0, ""},
};

/**************************************************************************
*
*/
void	Error(
		DWORD	ErrorCode,
		char	*Message)
{
	DWORD	i;

	for( i=0; ErrorMessage[i].ErrorCode!=0; i++)
		if( ErrorMessage[i].ErrorCode==ErrorCode )	break;

	printf("ERROR(%s) :::: %s\n", ErrorMessage[i].Message, Message);
	if( ErrorCode==CTR_USAGE_ERROR )	printf("\n%s", Help);
	exit(1);
}

//########################################################//
//######		Validity Test 관련 함수				######//
//########################################################//

/*************************************************************************/
typedef struct{
	DWORD	MsgLen;
	BYTE	Messge[120];
	BYTE	DigestValue[HAS160_DIGEST_VALUELEN];
} TEST_STRUCT;

TEST_STRUCT		TestData[] = {
	////	HAS160
	{	 0, "",
		{0x30,0x79,0x64,0xEF,0x34,0x15,0x1D,0x37,0xC8,0x04,0x7A,0xDE,0xC7,0xAB,0x50,0xF4,0xFF,0x89,0x76,0x2D}	},
	{	 1, "a",
		{0x48,0x72,0xBC,0xBC,0x4C,0xD0,0xF0,0xA9,0xDC,0x7C,0x2F,0x70,0x45,0xE5,0xB4,0x3B,0x6C,0x83,0x0D,0xB8}	},
	{	 3, "abc",
		{0x97,0x5E,0x81,0x04,0x88,0xCF,0x2A,0x3D,0x49,0x83,0x84,0x78,0x12,0x4A,0xFC,0xE4,0xB1,0xC7,0x88,0x04}	},
	{	14, "message digest",
		{0x23,0x38,0xDB,0xC8,0x63,0x8D,0x31,0x22,0x5F,0x73,0x08,0x62,0x46,0xBA,0x52,0x9F,0x96,0x71,0x0B,0xC6}	},
	{	26, "abcdefghijklmnopqrstuvwxyz",
		{0x59,0x61,0x85,0xC9,0xAB,0x67,0x03,0xD0,0xD0,0xDB,0xB9,0x87,0x02,0xBC,0x0F,0x57,0x29,0xCD,0x1D,0x3C}	},
	{	62, "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
		{0xCB,0x5D,0x7E,0xFB,0xCA,0x2F,0x02,0xE0,0xFB,0x71,0x67,0xCA,0xBB,0x12,0x3A,0xF5,0x79,0x57,0x64,0xE5}	},
	{	80, "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
		{0x07,0xF0,0x5C,0x8C,0x07,0x73,0xC5,0x5C,0xA3,0xA5,0xA6,0x95,0xCE,0x6A,0xCA,0x4C,0x43,0x89,0x11,0xB5}	},
	{	1000000, "a",
		{0xD6,0xAD,0x6F,0x06,0x08,0xB8,0x78,0xDA,0x9B,0x87,0x99,0x9C,0x25,0x25,0xCC,0x84,0xF4,0xC9,0xF1,0x8D}	},
	{	(DWORD)-1},
};

/**************************************************************************
*
*/
void	ValidityTest()
{
	BYTE	DigestValue[HAS160_DIGEST_VALUELEN];
	DWORD	i, tt;
	HAS160_ALG_INFO	AlgInfo;

	//
	for( i=0; TestData[i].MsgLen!=(DWORD)-1; i++) {
		//
		if( TestData[i].MsgLen!=1000000 ) {
			HAS160_Init(&AlgInfo);
			HAS160_Update(&AlgInfo, TestData[i].Messge, TestData[i].MsgLen);
			HAS160_Final(&AlgInfo, DigestValue);

			//
			fprintf(stdout, "\n==== test %d\n", i);
			PrintBYTE(stdout, "Msg", TestData[i].Messge, TestData[i].MsgLen);
			PrintBYTE(stdout, "DV", DigestValue, HAS160_DIGEST_VALUELEN);
		}
		else {
			HAS160_Init(&AlgInfo);
			for( tt=0; tt<1000000; tt++)
				HAS160_Update(&AlgInfo, TestData[i].Messge, 1);
			HAS160_Final(&AlgInfo, DigestValue);

			//
			fprintf(stdout, "\n==== test %d : 1000000-times 'a'\n", i);
			PrintBYTE(stdout, "DV", DigestValue, HAS160_DIGEST_VALUELEN);
		}
	}
}

/**************************************************************************
*
*/
void	SpeedTest()
{
#define DATA_LEN	1024
	BYTE	Data[DATA_LEN];
	BYTE	DigestValue[HAS160_DIGEST_VALUELEN];
	DWORD	i, tt, Iter;
	HAS160_ALG_INFO	AlgInfo;

	//
	for( tt=0; tt<DATA_LEN; tt++)	Data[tt] = rand() >> 3;

	//
	for( i=0; i<3; i++) {
		fprintf(stdout, "==== test %d : ", i);
		Iter = 2 * 42500;

		//
		SPEED_TEST("HAS160(1Kbyte data) : ", "\n", Iter, DATA_LEN,
			{	HAS160_Init(&AlgInfo);
				HAS160_Update(&AlgInfo, Data, DATA_LEN);
				HAS160_Final(&AlgInfo, DigestValue);		}	);
	}
#undef DATA_LEN
}

/**************************************************************************
*
*/
void	GeneralTest(
		char	*infile)
{
	FILE	*pIn;
	BYTE	DigestValue[HAS160_DIGEST_VALUELEN];
	BYTE	SrcData[1024+32];
	DWORD	SrcLen;
	HAS160_ALG_INFO	AlgInfo;

	//
	if( (pIn=fopen(infile, "rb"))==NULL ) {
		printf( "The file '%s' was not opened\n", infile);
		Error(0, "File(infile) Open Error");
	}

	//
	HAS160_Init(&AlgInfo);

	for(  ;  ;  ) {
		SrcLen = fread(SrcData, sizeof(BYTE), 1024, pIn);
		if( SrcLen==0 )	break;

		HAS160_Update(&AlgInfo, SrcData, SrcLen);
	}

	HAS160_Final(&AlgInfo, DigestValue);
	PrintBYTE(stdout, "DV", DigestValue, HAS160_DIGEST_VALUELEN);

	//
	fclose(pIn);
}

/**************************************************************************
*
*/
void	ChangeToCapital(char *String)
{
	DWORD	i;

	if( String[0]=='-' ) {
		for( i=1;  ; i++) {
			if( String[i]=='\0' )	break;
			if( (String[i]>='a') && (String[i]<='z') )
				String[i] += 'A' - 'a';
		}
	}
}

/**************************************************************************
*
*/
void	main(int argc, char **argv)
{
	DWORD	i;

	//
	for( i=1; i<(DWORD)argc; i++)
		ChangeToCapital(argv[i]);

	//
	if( argc==2 ) {
		if( strcmp(argv[1], "-T")==0 )		ValidityTest();
		else if( strcmp(argv[1], "-S")==0 )	SpeedTest();
		else	Error(CTR_USAGE_ERROR, "Invalid Use of Argument");
	}
	else if( argc==3 ) {
		DWORD	EncType=0, ModeType=0, PadType=0;

		//
		if( strcmp(argv[1], "-H")==0 );
		else	Error(CTR_USAGE_ERROR, "Invalid Use of Argument");

		GeneralTest(argv[2]);
	}
	else
		Error(CTR_USAGE_ERROR, "Invalid Number of Argument");
}

/*************** END OF FILE **********************************************/
