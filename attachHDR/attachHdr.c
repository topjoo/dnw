
/************************************************************************************
 * @file  : attachHdr.c
 *          AttachHdr.exe Tool
 * @brief : 
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
 *    0.00      0.00     0.00             1     0.00     0.00  help_brief
 *
 *			 
 *
 * @version : 2.0
 * @date	: 2021/10/07
 * @author	: Yoon-Hwan Joo (tp.joo@daum.net or toppy_joo@naver.com) 
 *
 ************************************************************************************/


/*--------------------------------------------------------------------------
                     INCLUDE FILES 
---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h> /* for getopt() */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <time.h> 
#include <math.h>  
#include <assert.h>	/* assert() */
#include <unistd.h>
#include <getopt.h> /* getopt_long() */
#include <memory.h> 
#include <stdarg.h>

#include "common.h" 
#include "feature.h"
#include "attachHdr.h"

#include "hash.c"
#include "mjd.c"
#include "hex2bin.c"
#include "elfinfo.c"
#include "common.c"




/*--------------------------------------------------------------------------
                     MACRO
---------------------------------------------------------------------------*/



void beep (int frequency, int duration)
{
#if 0
#define ESC 	27

	printf("%c[10;%d]%c[11;%d]\a", ESC, frequency, ESC, duration);
#endif
}


int istextfile(char *txtstr)
{
	while( (NULL != txtstr) && (*txtstr != '.') )
		txtstr++;

	if( 0 == strncmp(txtstr, ".exe", 4) ) return 0; /* no text file */
	if( 0 == strncmp(txtstr, ".EXE", 4) ) return 0; /* no text file */
	if( 0 == strncmp(txtstr, ".com", 4) ) return 0; /* no text file */
	if( 0 == strncmp(txtstr, ".COM", 4) ) return 0; /* no text file */

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



/* -----------------------------------------------------------------
 * double   val   : Number to be converted
 * int      round : Number of digits after decimal point
 * int      *dp   : Pointer to decimal-point position
 * int      *sign : Pointer to stored sign indicator
 * -----------------------------------------------------------------
 */
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



void help_brief(void)
{

//	printf("\033[32m"); /* GREEN */
//	//printf("\033[34m"); /* BLUE */

	printf("-------------------------------------------------------------\n");
	printf(" ah.exe %s by TOP.JOO (%s)/ %s \n", Attversion, EmailText, __DATE__ );
	printf("-------------------------------------------------------------\n");

//	printf("\033[0m"); 
//	fflush(stdout); 

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

           "--[ Files Encapsulation/Extract ]------- -------------------------------------------------------------------------\n"
           "  -y or --merge [1st file 2nd file .....] Specify the file lists to be merging to 1 file. Max count is 100 files.\n"
           "  -x or --extract [filename]              Specify the merged file name (upto 100 files). \n"
           "\n"
           " Ex) ah.exe --merge a100.bin a200.bin b300.bin c400.bin c600.bin --model BLTNCAM --version CI02-02 --output total.bin \n"
           "     ah.exe --model BLTNCAM --version CI02-02 --merge a100.bin a200.bin b300.bin c400.bin c600.bin --output total.bin \n"
           "     ah.exe --merge a100.bin a200.bin b300.bin c400.bin c600.bin --version CI02-02 --output total.bin \n"
           "     ah.exe --merge a100.bin a200.bin b300.bin c400.bin c600.bin --output total.bin \n"
           "\n"
           "     ah.exe --extract total.bin \n"
           "     ah.exe -x total.bin \n"
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




#if 0
    exit(0);
#else
	printf("Press [ENTER]...\r");
#endif
}




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



/* ---------------------------------------------------------------------------------------
 * main() function
 *
 * ---------------------------------------------------------------------------------------
 */

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

	// index file merged
	int istartIdx=-1, iEndIdx=0, iTotcnt=0;

	//char mergefile[50][128];
#define MERGE_TOTAL_CNT 			16
#define MERGE_INDEX_SIZ 			6 // 16
#define MERGE_DATE_SIZ 				26 // 32
#define MERGE_FILE_SIZ 				16
#define MERGE_FILENAME_LENGTH 		64
#define MERGE_SHA256_SIZ 			64
#define MERGE_MAX_FILES 			101

	typedef struct {
		int  mergeIndex;
		char mergeDate[MERGE_DATE_SIZ];
		char mergeFileName[MERGE_FILENAME_LENGTH];
		int  mergeSize;
		char mergeSHA256[MERGE_SHA256_SIZ];
	} _mergeFiles ;

	char mergeTotIndex[MERGE_TOTAL_CNT];
	char mergeTxtIndex[MERGE_INDEX_SIZ];
	char mergeTxtSize[MERGE_FILE_SIZ];
	char extractFile[MAX_FILENAME_LEN];
	
	_mergeFiles mFile[MERGE_MAX_FILES];
	struct _finddata_t mergeInfo;

// ======================================================================
#pragma pack(push, 1)                /* 구조체를 1바이트 크기로 정렬 */

	typedef struct {
		char mergeTxtIndex[MERGE_INDEX_SIZ];
		char mergeDate[MERGE_DATE_SIZ];
		char mergeFileName[MERGE_FILENAME_LENGTH];
		char mergeTxtSize[MERGE_FILE_SIZ];
		char mergeSHA256[MERGE_SHA256_SIZ+1];
	} _extractFiles ;

	_extractFiles  exFileInfo[MERGE_MAX_FILES];

#pragma pack(pop)

	char strExtract[1024+1];
// ======================================================================


	
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

	int ismultiFilesMerge=0; // 2022.09.02
	int ismultiFilesExtract=0; // 2022.09.02
	
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
	static char strOpt[] = "g:ALz:h:b:m:v:i:o:a:c:d:F:I:N:S:f:B:M:J:l:ej:nE:P:kZR:y:x:"; // 2022.09.04

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
		{"extract",		required_argument, 0, 'x'},
		{"merge",		required_argument, 0, 'y'}, /// file merged
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
							myerror("\n\n[++ERROR++] Can not create output file (number.mjd). \n");

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
							myerror("\n\n[++ERROR++] Can not create output file (date.mjd). \n");
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
						myerror("\nMJD>> WARNING:Wrong option. --mjd [date|mjd]. Check option.\n");

						beep(700,100);
						AllFilesClosed();

						exit(0);
						return 0;
					}
				}
				else
				{
					myerror("\nMJD>> WARNING:option error. check option --mjd [date|mjd]. \r\n");

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
						myerror("RGB Type : wrong option [%s] \n", str_BMPType);

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
					myerror("\n\n WARNING:wrong option --float [float number]. \r\n");

					beep(700,100);
					AllFilesClosed();
					exit(0);
			
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
					myerror("[++ERROR Starting Address++] Need option --intel or --motorola \n");

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
					myerror("[++ERROR PADBYTE++] Need option --intel or --motorola or --fillsize [size] \n");

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
					myerror("\n\n WARNING:wrong option --padbyte [hexa value]. check option [%s] \r\n", strPadByte);

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

			case 'y':
				{
				int ii=0, reidx=0;
				
					// --------------------------------------------------------------------
					iTotcnt = argc;
					do {
						if( strcmp( argv[ii], "--merge" ) == 0 )
						{
							//++printf("\n>> ii    : (%s) [%d] iTotcnt=%d \n", argv[ii], ii, iTotcnt );
							istartIdx = ii;
							break;
						}	
					} while( ii++ < iTotcnt);

					if( istartIdx == -1 ) 
					{
						printf(">> check please --merge option!!\n" );
						break;
					}

					ii++;
					do {
						if( (strncmp( argv[ii], "--", 2 ) == 0) || (strncmp( argv[ii], "-", 1 ) == 0) )
						{
							//++printf(">> iEndIdx    : (%s) [%d] \n", argv[ii], ii );
							iEndIdx = ii;
							break;
						}						
					} while( ii++ < iTotcnt);

					
					memset( mFile, 0x00, sizeof(mFile) );
					//memset(mergefile, 0x00, sizeof(mergefile) );

					printf("\n>>Merging input file lists: \n");
					for(reidx=1, ii=istartIdx+1; ii<iEndIdx; ii++, reidx++)
					{
						memset( mFile[reidx].mergeFileName, 0x00, sizeof(mFile[reidx].mergeFileName) );
						strcpy( mFile[reidx].mergeFileName, argv[ii] );
						printf("%2d -> filename: [%s] \n", reidx, mFile[reidx].mergeFileName );
					}

					ismultiFilesMerge = 1;
					// --------------------------------------------------------------------

				}
				break;

			// extract =============================
			case 'x':
				{

					if(optarg) 
					{
	
						memcpy(extractFile, optarg, MAX_FILENAME_LEN);
						olen = strlen(extractFile);
	
						printf("\n>>Merged filename : %s ", extractFile );

						if( NULL == (inpfile = fopen( extractFile, "rb")) ) 
						{
							printf("\n--extract option error, files not found!! [%s] \n", extractFile );
							AllFilesClosed();
							exit(0); /// help();
							return 0;
						}
						fclose(inpfile);
	
						ismultiFilesExtract = 1;
					}
				
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
	else if( multifileindex > 0 || (ASTERISK_FOUND==isAsteMode) || (1==isIgnoreBothFile && 1==isFloat2Hex) || (1==ismultiFilesMerge) || (1==ismultiFilesExtract) ) /// input files ignored---
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
		if(verbose==TRUE) LOG_ERR("\n\n[OUTPUT] isIgnoreBothFile is TRUE. ");
		// input and output BOTH files are ignored
	}
	else if(1==isIgnoreOuFile)
	{
		if(verbose==TRUE) LOG_ERR("\n\n[OUTPUT] isIgnoreOuFile is TRUE. ");
		// output file is ignored
	}
	else if(1==ismultiFilesExtract)
	{
		if(verbose==TRUE) LOG_ERR("\n\n[OUTPUT] ismultiFilesExtract is TRUE. so this do extract files --merge option file.. \n");
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
					printf("\n\n[++ERROR++]Can NOT create folder & output file. [%s / %s] - (write mode) \n", szDir, outfile_name);
					if( NULL == outfile_name || outfile_name[0] == 0x00 )
						printf("Need an output file option (--output or --append) for saving output.\n" );

				}
			}
			else
			{
				printf("\n\n[++ERROR++]Can NOT create output file. [%s] - (write mode) \n", outfile_name);
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

#if SHA1_HASH_ENCIPHER
		&& (0 == isSHA1) /// 2014.06.29
#endif /// SHA1_HASH_ENCIPHER

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
				printf("\n\n[++ERROR++] Can not open input file[%s] \n",infile_name);
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
		unsigned int hexFamily=2; // Intel Family:1, Motorola Family : 2
	
		byte	Data_Str[HEX_MAX_LINE_SIZE];


		Phys_AddrTemp = 0; // added at 2016.03.10
		iErrCount = 0; // clear
		checksum_err_cnt = 0;	// 2022.08.03

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
						printf("\nLine%6d :S1-Error in hex file. ", Record_Nb );
						if( result<=0 ) printf(" Check INTEL family hexa type!!!");


						iErrCount ++; // 2016.03.05
						// ---------------------------------
						if( iErrCount > MAX_ERR_COUNT*2 ) // 2016.03.05
						{
							printf("\n\nCheck Motorola hexa family type!! Maybe INTEL family hex type!\n");
						
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

						// ---------------------------------
						if( iErrCount > MAX_ERR_COUNT*2 ) // 2022.08.05
						{
							printf("\n\nCheck Motorola hexa family type!! Maybe INTEL family hexa type!!\n");
						
							iErrCount = 0; // clear
							AllFilesClosed();
						
							exit(0);
							return 1;
						}
						// ---------------------------------

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

						// ---------------------------------
						if( iErrCount > MAX_ERR_COUNT*2 ) // 2022.08.05
						{
							printf("\n\nCheck Motorola hexa family type!! Maybe INTEL family hexa type!!!\n");
						
							iErrCount = 0; // clear
							AllFilesClosed();
						
							exit(0);
							return 1;
						}
						// ---------------------------------

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
			printf("\n\nCheck Motorola hexa type!!! Maybe INTEL family hexa type!! \n");

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
						for(iidx=2; HexaLine[iidx] != 0x00 && iidx<(Nb_Bytes+1)*2; iidx+=2 )
						{
							memset( TempC, 0x00, sizeof(TempC) );
							strncpy( (char*)TempC, (char *)&HexaLine[iidx], 2);
							TempVal = str2hex(TempC);
							iCount ++;
							if( 2==iidx ) 
							{
								TempLenVal = TempVal; // length once saved!
							}

							/* ========== checksum loginc ========= */
							{
								TempCRCSumVal += TempVal;
								TempCRCSumVal &= 0xFF;

								//printf("idx(%d) -> %02x -> %02x \n", iidx, TempVal, TempCRCSumVal );

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


						strncpy( (char*)TempC, (char *)&HexaLine[iidx], 2);
						TempVal  = str2hex(TempC);
						TempCRC  = TempVal;


						// ========= checksum confirm!!
						if( ((255-TempCRCSumVal)&0xFF) == (TempCRC&0xFF) )
						{
							// OK !!!!
							//Checksum = Nb_Bytes + 0x52 + 0x65 + 0x6C + 0x65 + 0x61 + 0x73 + 0x65 + 0x20 + 0x42 + 0x75 + 0x69 + 0x6C + 0x64 + 0x5C + 0x72 + 0x6F + 0x6D + 0x70 + 0x2E + 0x6F + 0x75 + 0x74;
							//printf("S0 Checksum = %#02x -- OK \n", (255-Checksum) );
							printf("Line #%d :S0 Checksum is OK \n", Record_Nb );
						}
						else
						{
							//fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
							printf("Line #%d :S0-Error in selected hex file. This checksum should be 0x%02x \n", Record_Nb, (255-TempCRCSumVal)&0xFF );
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
						fprintf(stderr,"zero byte length Data record is ignored.\n");
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
					if (result != 1) fprintf(stderr,"Line%6d ::Error in hex file (result%d) \n", Record_Nb, result);
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
			#if 1
				VerifyChecksumValue(Record_Checksum, hexFamily); /* hexFamily:2 MOTOROLA family*/
			#else
				if (((Record_Checksum + Checksum) != 0xFF) && Enable_Checksum_Error)
				{
					printf("Line%6d ::Checksum error: should be 0x%02x, not be 0x%02x. \n",Record_Nb, 255-Checksum, Record_Checksum); ///, HexaLine);
					Status_Checksum_Error = true;
				}
			#endif
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
			printf("MOTOROLA family hexa: Checksum error (%d) is detected !!!\n", checksum_err_cnt);

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
		const unsigned int hexFamily=1; // Intel Family:1, Motorola Family : 2
		
		/* We will assume that when one type of addressing is selected, it will be valid for all the
		 current file. Records for the other type will be ignored. */
		unsigned int Seg_Lin_Select = NO_ADDRESS_TYPE_SELECTED;
	
		unsigned int temp2;
	
		byte	Data_Str[HEX_MAX_LINE_SIZE];
	
		checksum_err_cnt = 0;	// 2022.08.03

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
					if( iErrCount > MAX_ERR_COUNT ) // 2020.06.30
					{
						printf("\n\nCheck Intel hexa family type!!!  Maybe MOTOROLA family in this hex file!! \n");
					
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
									
									printf("Line%6d :%06d:Upper_Address: 0x%X to zero-based addr: 0x%X \n", Record_Nb, Type, Phys_AddrTemp, Upper_Address );
									printf("                   Parsig on the rebased address: 0x%X from 0x%X.\n", Upper_Address, Phys_AddrTemp);
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
						fprintf(stderr,"Line%6d :%d:Unknown Intel record type.  Maybe MOTOROLA family type!! \n", Record_Nb, Type );
					}
					break;
				}
			}
		}
		while (!feof (inpfile));

		
		// ---------------------------------
		if( iErrCount > MAX_ERR_COUNT ) // 2016.03.05
		{
			printf("\n\nCheck Intel hexa family type!!! Maybe MOTOROLA family hex type!!\n");

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
						if (result != 1) fprintf(stderr,":0:Error in line %d of hex file. \n", Record_Nb);
	
						/* Verify Checksum value. */
						Checksum = (Checksum + temp2) & 0xFF;

						VerifyChecksumValue(temp2, hexFamily); /* hexFamily:1 INTEL family*/
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
						if (result != 2) fprintf(stderr,":2:Error in line %d of hex file (result:%d) \n", Record_Nb, result);
	
						/* Update the current address. */
						Phys_Addr = (Segment << 4);
	
						/* Verify Checksum value. */
						Checksum = (Checksum + (Segment >> 8) + (Segment & 0xFF) + temp2) & 0xFF;

						VerifyChecksumValue((Segment >> 8) + (Segment & 0xFF) + temp2, hexFamily);  /* hexFamily:1 INTEL family*/

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

						//VerifyChecksumValue( temp2, hexFamily); /* hexFamily:1 INTEL family*/

					#if HEX2BIN_INTEL_ZERO_FORCED // 2020.06.29
						// ----------------------------------------
						// Hex Address Zero based converted 
						// ----------------------------------------
						if( HEX2BIN_ZERO_FORCED==Enable_HexaAddr_Zero_Forced )
						{
							Checksum = (Checksum + (Phys_AddrTemp >> 8) + (Phys_AddrTemp & 0xFF) ) & 0xFF;							

							//fprintf(stderr,"[+FORCED+1+]");
							//VerifyChecksumValue( (Phys_AddrTemp >> 8) + (Phys_AddrTemp & 0xFF), hexFamily); /* hexFamily:1 INTEL family*/
							VerifyChecksumValue( Checksum, hexFamily); /* hexFamily:1 INTEL family*/
						}
						else
						{
							VerifyChecksumValue( temp2, hexFamily); /* hexFamily:1 INTEL family*/
						}
					#else

						VerifyChecksumValue( temp2, hexFamily); /* hexFamily:1 INTEL family*/

					#endif



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
			printf("INTEL family haxa: Checksum error (%d) detected !!!\n", checksum_err_cnt);

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

#if SHA1_HASH_ENCIPHER
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
#endif /// SHA1_HASH_ENCIPHER


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


	else if( (1==ismultiFilesExtract) )
	{
		unsigned __int64	kll=0UL, ll=0UL;
		int  ii=0, iiTot=0, hashOK=0;
		unsigned __int64	fSize=0UL, fUsize=0UL, fRead=0UL;

		
		SHA256_CTX		ctx256;
		unsigned char	sha256_buf[SHA2_BUFLEN];
		int iLenSub=0;
		

		if( NULL != (inpfile = fopen( extractFile, "rb")) ) 
		{
			printf("\n");
			memset(mergeTotIndex, 0x00, sizeof(mergeTotIndex) );
			ll = fread(mergeTotIndex, 1, MERGE_TOTAL_CNT, inpfile); 
			kll += ll;
			iiTot = atoi(mergeTotIndex);
			printf(">>Merged total cnt: %s (%d)\n", mergeTotIndex, iiTot);

			memset(str_moduleName, 0x00, sizeof(str_moduleName) );
			ll = fread(str_moduleName, 1, MAX_VERSION_LEN, inpfile); 
			kll += ll;
			printf(">>Model Name      : %s\n", str_moduleName);


			memset(str_versionName, 0x00, sizeof(str_versionName) );
			ll = fread(str_versionName, 1, MAX_VERSION_LEN, inpfile); 
			kll += ll;
			printf(">>Version Name    : %s\n", str_versionName);

			memset(exFileInfo, 0x00, sizeof(exFileInfo) );
			for(ii=0; ii<iiTot; ii++)
			{
				ll = fread((char *)&exFileInfo[ii], 1, sizeof(exFileInfo[0]), inpfile);
				kll += ll;
				printf("Index ---------: [%s] len:%d \n", exFileInfo[ii].mergeTxtIndex, strlen(exFileInfo[ii].mergeTxtIndex) );
				printf("  Date --------: [%s] len:%d \n", exFileInfo[ii].mergeDate, strlen(exFileInfo[ii].mergeDate) );
				printf("  FileName ----: [%s] len:%d \n", exFileInfo[ii].mergeFileName, strlen(exFileInfo[ii].mergeFileName) );
				printf("  FileSize ----: [%s] len:%d \n", exFileInfo[ii].mergeTxtSize, strlen(exFileInfo[ii].mergeTxtSize)  );
				exFileInfo[ii].mergeSHA256[MERGE_SHA256_SIZ] = 0x00; 
				printf("  File SHA256 -: [%s] len:%d \n", exFileInfo[ii].mergeSHA256, strlen(exFileInfo[ii].mergeSHA256)  );

				if(inpfile) fseek(inpfile, -1, SEEK_CUR);  /* because of SHA256 - 64bytes, SEEK_CUR, SEEK_END */

			}


			// === create output file			
			printf("\n");
			for(ii=0; ii<iiTot; ii++)
			{
				if( NULL == (outfile = fopen( exFileInfo[ii].mergeFileName, "wb"))	)	
				{
					printf("[++ERROR++]Can NOT create extract file... \n", exFileInfo[ii].mergeFileName );
				}
				else
				{
				
					fSize = atoi(exFileInfo[ii].mergeTxtSize);
					fRead = 1024;
					fUsize = 0UL;
					memset(strExtract, 0x00, sizeof(strExtract) );

					printf(">>%2d -> Extracting --: %s / %d \n", ii+1, exFileInfo[ii].mergeFileName, fSize );
					while((ll = fread(strExtract, 1, fRead, inpfile)) > 0) 
					{
						kll += ll;

						fwrite(strExtract, sizeof(unsigned char), ll, outfile); 
						fUsize += ll;

						if( (fSize-fUsize) < 1024 ) fRead=(fSize-fUsize);
					}					
					fclose(outfile);

				}
			}

		}
		fclose(inpfile);


		// ==================================================================
		// === SHA256 Hashing
		printf("\n");
		hashOK = 0; // 0 for OK
		for(ii=0; ii<iiTot; ii++)
		{
		
			if( NULL == (inpfile = fopen( exFileInfo[ii].mergeFileName, "rb")) ) 
			{
				printf("Files not found for SHA256!! [%s] \n", exFileInfo[ii].mergeFileName );
			}
			else
			{
				/// initial
				memset( &ctx256, 0x00, sizeof(SHA256_CTX) );
				memset( sha256_buf, 0x00, sizeof(sha256_buf) );
				
				/// SHA2 Calculate ----
				SHA256_Init(&ctx256);

				kll = 0UL;
				printf(">>%2d -> Read *SHA256*%s   %s \n", ii+1, exFileInfo[ii].mergeSHA256, exFileInfo[ii].mergeFileName );
				while((ll = fread(sha256_buf, 1, SHA2_BUFLEN, inpfile)) > 0) 
				{
					kll += ll;
					SHA256_Update(&ctx256, (unsigned char*)sha256_buf, ll);
				}
				SHA256_End(&ctx256, sha256_buf);
				
				if( 0 == strncmp(exFileInfo[ii].mergeSHA256, sha256_buf, MERGE_SHA256_SIZ ) )
				{
					printf("     SHA256 Hash OK... \n" );
				}
				else
				{
					printf("  SHA256 Hash NG ---:%s -> Not same file. Check plz.. \n", sha256_buf );
					hashOK ++;
				}
			}
			fclose(inpfile);				
		}
		// ==================================================================
		if( 0 == hashOK )
			printf("\nAll extracted files are trusted files... \n");
		else
			printf("\n[++ERROR++]An untrusted file has been created... check plz...\n");

			
	}
	else if( (1==ismultiFilesMerge) )
	{
	int fileNum = iEndIdx-istartIdx;
	int ii=1, i=0, length=0;
	long 	attFile;

	unsigned __int64	kll=0UL, ll=0UL;
	SHA256_CTX		ctx256;
	unsigned char	sha256_buf[SHA2_BUFLEN];
	int iLenSub=0;


		//==1== Total Counter
		printf("\n>>Total merged file count : %d \n",  fileNum-1) ;
		memset(mergeTotIndex, 0x00, sizeof(mergeTotIndex) );
		sprintf(mergeTotIndex,"%d",fileNum-1 );
		
		for(i=0; i<MERGE_TOTAL_CNT; i++)
		{ if( ('\n' == mergeTotIndex[i]) || ('\r' == mergeTotIndex[i]) || (0x20 == mergeTotIndex[i]) ) mergeTotIndex[i] = 0x00; }

		// ----------------------------------------------------
		// sub store-0 : Total File Counter		
		if(outfile) 
		{
			fprintf(outfile,"%s", mergeTotIndex );
		}
		length = strlen(mergeTotIndex);
		while( length < MERGE_TOTAL_CNT )
		{
			if(outfile) fprintf(outfile,"%c",SPACE_FILL1);
			length++;
		}		
		// ----------------------------------------------------


		// ================= MODEL NAME ====================================
		if( ATT_MODEL == (isAttach&ATT_MODEL) )
		{
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
		}


		// ================= VERSION NAME ====================================
		if( ATT_VERSION == (isAttach&ATT_VERSION) )
		{

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

		}


		

		//==2== File List-up (Date/Size) and Index
		for(ii=1; ii<fileNum; ii++)
		{

			if( (attFile = _findfirst( mFile[ii].mergeFileName, &mergeInfo )) == -1L )
			{
				printf( "No files in current directory!! (%d)-[%s] \n", ii, mFile[ii].mergeFileName );
			}
			else
			{
				if( !(mergeInfo.attrib & _A_SUBDIR) )
				{
					mFile[ii].mergeIndex = ii;
					mFile[ii].mergeSize = mergeInfo.size;
					memset(mFile[ii].mergeDate, 0x00, sizeof(mFile[ii].mergeDate) );
					strncpy(mFile[ii].mergeDate, ctime( &( mergeInfo.time_write )), 24 );

					//printf( "[%d] => [%s] [%s] [%s], Size: %ld bytes \n", ii, mFile[ii].mergeFileName, mergeInfo.name, mFile[ii].mergeDate, mergeInfo.size );
				}
			}

			_findclose( attFile );
		}


		//==3== SHA2 ----
		printf("SHA2>> SHA-256 hashing for encapsulating files... \n");
		for(ii=1; ii<fileNum; ii++)
		{

			if( NULL == (inpfile = fopen( mFile[ii].mergeFileName, "rb")) ) 
			{
				printf("--merge option error, files not found!! [%s] \n", mFile[ii].mergeFileName );
				//AllFilesClosed();
				//exit(0); /// help();
				//return 0;
			}

			/// initial
			memset( &ctx256, 0x00, sizeof(SHA256_CTX) );
			memset( sha256_buf, 0x00, sizeof(sha256_buf) );

			/// SHA2 Calculate ----
			SHA256_Init(&ctx256);

			kll = 0UL;
			printf("Waiting for SHA256**[%s]... \r", mFile[ii].mergeFileName);
			while((ll = fread(sha256_buf, 1, SHA2_BUFLEN, inpfile)) > 0) 
			{
				kll += ll;
				SHA256_Update(&ctx256, (unsigned char*)sha256_buf, ll);
			}
			SHA256_End(&ctx256, sha256_buf);

			memset(mFile[ii].mergeSHA256, 0x00, sizeof(mFile[ii].mergeSHA256) );
			strcpy(mFile[ii].mergeSHA256, sha256_buf );

			printf("\r");
			printf("%2d -> %s  *SHA256*%s [%s] / %ld Bytes \n", ii, mFile[ii].mergeSHA256, mFile[ii].mergeFileName, mFile[ii].mergeDate, mFile[ii].mergeSize );


			if(outfile) 
			{

				// --------------------------------------------------------
				// sub store-1: Index Number
				memset(mergeTxtIndex, 0x00, sizeof(mergeTxtIndex) );
				sprintf(mergeTxtIndex,"%d",ii );
				fprintf(outfile,"%s", mergeTxtIndex);
				
				length = strlen(mergeTxtIndex);
				while( length < MERGE_INDEX_SIZ )
				{
					if(outfile) fprintf(outfile,"%c",SPACE_FILL1);
					length++;
				}
				// --------------------------------------------------------

				// --------------------------------------------------------
				// sub store-2: File Date
				fprintf(outfile,"%s", mFile[ii].mergeDate );
				length = strlen(mFile[ii].mergeDate);
				while( length < MERGE_DATE_SIZ )
				{
					if(outfile) fprintf(outfile,"%c",SPACE_FILL1);
					length++;
				}


				// --------------------------------------------------------
				// sub store-3: File Name
				fprintf(outfile,"%s", mFile[ii].mergeFileName );
				length = strlen(mFile[ii].mergeFileName);
				while( length < MERGE_FILENAME_LENGTH )
				{
					if(outfile) fprintf(outfile,"%c",SPACE_FILL1);
					length++;
				}
				// --------------------------------------------------------


				// --------------------------------------------------------
				// sub store-4: File Size
				memset(mergeTxtSize, 0x00, sizeof(mergeTxtSize) );
				itoa(mFile[ii].mergeSize, mergeTxtSize, 10 );
				fprintf(outfile,"%s", mergeTxtSize );
				length = strlen(mergeTxtSize);
				while( length < MERGE_FILE_SIZ )
				{
					if(outfile) fprintf(outfile,"%c",SPACE_FILL1);
					length++;
				}
				// --------------------------------------------------------

				// sub store-5: Hash Code SHA256 (64bytes) for specific file
				fprintf(outfile,"%s", mFile[ii].mergeSHA256 );
				length = strlen(mFile[ii].mergeSHA256);
				while( length < MERGE_SHA256_SIZ )
				{
					if(outfile) fprintf(outfile,"%c",SPACE_FILL1);
					length++;
				}
				// --------------------------------------------------------


			}

			fclose(inpfile);


		}


		//==4 step== FILE Merging ----
		if(outfile) 
		{
			size_t	fr_size;
			size_t	tot_size = 0L;
			unsigned char read_buf[RD_BUF_SIZ] = {0,};
			

			printf("Encapulating...\n");
			for(ii=1; ii<fileNum; ii++)
			{
			
				printf("%2d -> [%s / %ld Bytes] to output [%s / ", ii, mFile[ii].mergeFileName, mFile[ii].mergeSize, outfile_name );
				if( NULL == (inpfile = fopen( mFile[ii].mergeFileName, "rb")) ) 
				{
					printf("--merge option error2, files not found!! [%s] \n", mFile[ii].mergeFileName );
					//AllFilesClosed();
					//exit(0); /// help();
					//return 0;
				}

				memset( read_buf, 0x00, sizeof(read_buf) ); /// initial
				while( fr_size = fread(read_buf, sizeof(unsigned char), BUFSIZE, inpfile ) )
				{
					fwrite(read_buf, sizeof(unsigned char), fr_size, outfile); 
					tot_size += fr_size;
				}

				printf("%ld Bytes] ... \n", tot_size );

				if(inpfile) { fclose(inpfile); inpfile=NULL; }

			}

			if(outfile) fclose(outfile);
			printf("\n>>Output file     : %s (%s)", outfile_name, (isAppend==1)? "append":"new create");
			printf("\n>>Merge Completed...\n");

		}
		else
		{
			printf("\n\n[++ERROR++] Can NOT Encapulating... check plz.....\n\n");
		}

		if(outfile) fclose(outfile);

		
	}

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
				printf("\n\n[++ERROR++] Can not open input file [%s]. \n",sefile_name);

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
	AllFilesClosed();
	exit(0);
	/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
	return(0);
}



