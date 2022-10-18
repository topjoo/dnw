// =============================================
// Program Name: Regmgr.cpp
// =============================================

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include "resource.h"

#include <windows.h>
#include <winbase.h>
#include <mmsystem.h>
#include <winreg.h>
#include <tlhelp32.h>   // CreateToolhelp32Snapshot() 를 사용하기 위한 해더포함     

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <tchar.h>
#include <time.h>
#include <stdio.h>
#include <winuser.h>
#include <process.h> // thread!!

#include <pdh.h> // CPU usage
#include <pdhmsg.h> // CPU uage

//#include <TlHelp32.h>
//#include <Shlwapi.h>
//#pragma comment (lib, "Shlwapi.lib")


#include "engine.h"
#include "dnw.h"
#include "fileopen.h"
#include "font.h"
#include "d_box.h"
#include "usbtxrx.h"
#include "regmgr.h"



#if USE_WIN_OUTOF_AREA_TO_ZERO // 2016.03.31 LCD size
// LCD Size
DWORD 	myLCDwidth=WIN_LIMIT_XX, myLCDheight=WIN_LIMIT_YY; // 2016.03.31 LCD size
DWORD 	myVirWidth=0, myVirHeight=0; // 2017.04.20 VirtualLCD size
#endif


DWORD	WIN_XSIZE = WINDOW_XSIZE;
DWORD	WIN_YSIZE = WINDOW_YSIZE;

#if DNW_WIN_POSITION_MOVALBE // 2016.02.29
BOOL    isSaveSize = FALSE;
BOOL    isSavePosition = FALSE;
DWORD	POSave_XSIZE = WINDOW_XSIZE; 
DWORD	POSave_YSIZE = WINDOW_YSIZE; 
#endif


DWORD	editXBgn=0, editWidth=0;
DWORD	editYBgn=0, editHeight=0;

DWORD	MainXBgn=0, MainWidth=0;
DWORD	MainYBgn=0, MainHeight=0;

DWORD 	WIN_XBgn=0, WIN_YBgn=0;


extern int userComPort, idBaudRate,userBaudRate;
#ifdef COM_SETTINGS /// 2012.07.11
extern int userCOMparity, userCOMstop, userCOMdataIdx;
extern int userFlowControl; // 2017.08.03, Flow Control
extern int userFlowCtrlVal; // 2017.08.04, Flow Control Value

extern int userHexaDispVal; // 2018.03.22 Hexa display
#endif


extern BOOL isFileExist(TCHAR *file);
extern UINT str2int(TCHAR *str);
extern UINT str2hex(TCHAR *str);

extern BOOL Is64BitWindows();



#if USE_HEX2BIN_CONVERT // 2016.03.04
extern int userHexType;

extern int userHex2BinLogOnOff; // Log on/off
extern int Verbose_Flag; // Log on/off

extern int userSwapWordwise; // initial Swap Off mode
extern int Swap_Wordwise; // Swap Off mode
extern int userPadByteOnOff; // 2018.07.06


extern int userCRCtype;
extern int userEndian;

extern int userHexAddrZeroForced;	// Hex Address Zero forced
extern int Enable_HexaAddr_Zero_Forced;

extern int userCRCverify; // ON
extern int userSizeUnit;
extern int userAddrAlignWord; // Intel address word-alignment
extern int Address_Alignment_Word; // Intel address word-alignment

extern DWORD dwHex2BinMaxSize;
extern TCHAR szHex2BinMaxSize[16];

extern int Pad_Byte;

extern int  dwPadByte;
extern TCHAR szPadByte[16];

extern TCHAR szH2BFullInputFile[MAX_PATH];
extern TCHAR szH2BFullOutputFile[MAX_PATH];

extern TCHAR szH2BInpFile[MAX_PATH];
extern TCHAR szH2BOutFile[MAX_PATH];
#endif


#if USE_TXDATA_TO_SERIAL
extern int sendTxDataType; // 2016.03.23
extern int txDataMsgOnOff; // off 
extern TCHAR szSerialTxCharData[TX_DATA_LENGTH]; // 2016.03.23
extern TCHAR szSerialTxHexaData[TX_DATA_LENGTH]; // 2016.03.23
extern TCHAR szSerialTxFileData[TX_DATA_LENGTH]; // 2016.03.28
extern TCHAR szSerialTxFileTitl[TX_DATA_LENGTH]; // 2016.03.28

extern TCHAR szSerialTxHexaXOR[TX_DATA_LENGTH]; // 2018.08.21


extern TCHAR szSerialTxDispName[TX_DATA_LENGTH];
extern int iidx, jidx, isFind;
extern __int64 ulTxDataUARTFileSize;
extern __int64 GetTxDataFileSize(HANDLE hFile);
#endif



#if USE_FLOAT2HEX_VALUE // 2018.07.18
extern TCHAR szFlt2Hex_In1[MAX_PATH]; // 1st input
extern TCHAR szFlt2Hex_In2[MAX_PATH]; // 2nd input
extern TCHAR szFlt2Hex_In3[MAX_PATH]; // step
extern TCHAR szFlt2Hex_Out[MAX_PATH];

extern int isFlt2HexStop;
extern int iChoiceFlt2Hex, iFlt2HexEndian;
#endif


#if USER_SCREEN_BUF_SIZE // 2016.02.14
extern int userScrBufSizIndex;
#endif

extern DWORD downloadAddress;
extern TCHAR szDownloadAddress[ADDR_LENGTH];
extern int autoSendKey;

#if DISPLAY_PC_SYSTEM_TIME /*--- 2016.02.12 ------ */
extern int localTime;
extern int isTimeInfoOnce; // Time 정보를 맨 앞에 한번 표시한다. 
#endif

extern int cmdCRLF; // 2019.12.21
extern int LogFileType; // 2020.04.07

extern int msgSaveOnOff;
/* 2011.02.08 , Text Mode or Hexa Mode */
extern int TextorHexaMode; // 0:Text Mode, 1:Hexa Line Mode, 2:Hexa End Char
extern int CPUtype; /// SMDK6410:0, SMDKC100:1, S3C2440A:2, 

extern int FontType;
extern int RetryUSB; /// 2012.03.10


#if HEXA_MODE_SUPPORT 
extern DWORD ColumnNumber;
extern TCHAR szColumnNumber[16];
extern int uCount; // 2016.04.02 for LineFeed Count
extern DWORD HexaEndChar; // Example 0x81, or 0x83, ...
extern TCHAR szHexaEnd[16]; // 2016.04.12
#endif


#if DNW_2ND_WINDOWS_ENABLE // 2016.10.24
extern int isStatusBarEnable;
extern int OKstatusBarInitial;
#endif


#if defined(COLOR_DISP2) /// 2010.05.14
extern int ColorType; /// 2010.05.14 added
#endif


#if (RICHED_COLOR) || (TEXT_FIND_MARK) // 2016.09.23
extern int txtColorRed, txtColorMegenta, txtColorBlue, txtColorGreen, txtColorYellow;
extern int txtFindRed, txtFindMegenta, txtFindBlue, txtFindGreen, txtFindYellow;

extern int iLenDataRed, iLenDataMagenta, iLenDataBlue, iLenDataGreen, iLenDataYellow;

extern TCHAR szDataRed[MAX_PATH];
#if 0
extern TCHAR szDataMagenta[MAX_PATH];
extern TCHAR szDataBlue[MAX_PATH];
extern TCHAR szDataGreen[MAX_PATH];
extern TCHAR szDataYellow[MAX_PATH];
#endif
#endif


#if TEXT_FIND_MARK // 2016.09.23
extern void IsMarking(void);
extern long long make_hash_p(TCHAR str[], int _size);
extern long long HashRed, HashMagenta;
#endif


#if MODIFIED_JULIAN_DATE // 2017.08.31
extern double Convert2MJD(mjd_timestamp YYMMDD_Time);
extern void f2a(double number, TCHAR* string, int precision);
#endif



int isRegSave = 0; // Save finished!!



void SetRegistry(void) // void *args)
{
	HKEY hKey = NULL;
	LONG lRet;
	DWORD dwSize;
	TCHAR strBuild[32] = {0,};


	isRegSave = 1; // Start Save
	
	lRet = RegCreateKeyEx(HKEY_CURRENT_USER, REG_LOC, 0, NULL, 
					REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwSize);



	if( ERROR_SUCCESS != lRet )
	{
		WIN_XSIZE = WINDOW_XSIZE;
		WIN_YSIZE = WINDOW_YSIZE;
		autoSendKey = 0;

		msgSaveOnOff = 0; /// added.
		localTime = 0; // default Time Only
		isTimeInfoOnce = 0; // 2016.04.02

		// 2019.12.21
		cmdCRLF = 0;

		LogFileType = 0;


		userComPort  = 1;
		idBaudRate   = 9; /* 115200 bps*/
		userBaudRate = baudTable[idBaudRate];

		
	#ifdef COM_SETTINGS /// 2015.12.30
		userCOMparity  = 0;
		userCOMstop    = 0;
		userCOMdataIdx = 1;
		userFlowControl= 0; // 2017.08.03 None Flow COntrol
		userFlowCtrlVal= 0; // 2017.08.04, Flow COntrol
	#endif

		userHexaDispVal = 0; // 2018.03.22

	#if USER_SCREEN_BUF_SIZE // 2016.02.14
		userScrBufSizIndex = 0; // large
	#endif

	#if USE_HEX2BIN_CONVERT // 2016.03.04
		// Hex file type
		userHexType = 0; // intel
	#endif	


	#if USE_HEX2BIN_CONVERT // 2016.03.04
		// Log On/Off
		userHex2BinLogOnOff = 0;
		Verbose_Flag = 0;  // Log Off

		userSwapWordwise = 0; // initial Swap Off mode
		Swap_Wordwise = 0; // Swap Off mode

		userPadByteOnOff = 1; // 2018.07.06

		// CRC Type
		userCRCtype = 2; // CRC8:default
		
		// Endian
		userEndian = 0; // Little Endian

		// Hex Address Zero forced
		userHexAddrZeroForced = 0; // refer real address
		Enable_HexaAddr_Zero_Forced = 0; // Zero-forced off!
		

		// CRC Verify
		userCRCverify = 1; // On

		// Address Word-alignment
		userAddrAlignWord = 0;
		Address_Alignment_Word = 0;

		//Filesize Unit
		userSizeUnit  = 0; // KB
	#endif
	



	#if USE_TXDATA_TO_SERIAL

		// 2016.03.23 Tx Data
		sendTxDataType = 0;
		txDataMsgOnOff = 0; // off 
		lstrcpy(szSerialTxCharData,TEXT(" qUIz")); // SiRFprima2 boot menu password
		lstrcpy(szSerialTxHexaData,TEXT("0x207155497a"));
		lstrcpy(szSerialTxFileData,TEXT(""));
	#endif


	#if USE_FLOAT2HEX_VALUE // 2018.07.19
		lstrcpy(szFlt2Hex_In1,TEXT("0.01234")); // start
		lstrcpy(szFlt2Hex_In2,TEXT("10.5273")); // 2nd
		lstrcpy(szFlt2Hex_In3,TEXT("0.01"));    // step

		iFlt2HexEndian = 0;
	#endif
	

		TextorHexaMode = 0; /* Default : Text mode,   Text Mode:0, Hexa Line Mode:1, Hexa End Char */

	#if HEXA_MODE_SUPPORT	/* 2012.07.03 Hexa mode display */

		#if 1 // 2017.04.21
		ColumnNumber = 0;
		lstrcpy(szColumnNumber,TEXT("0"));
		#else
		ColumnNumber = 16;
		lstrcpy(szColumnNumber,TEXT("16"));
		#endif
		
		HexaEndChar = 0x0D0A; // Example 0x81, or 0x83, ...
		uCount = 0;
	#endif	


	#if 0
		userComPort = 1;
		idBaudRate = 9; /* 115200 bps*/
		userBaudRate = baudTable[idBaudRate];
		downloadAddress = 0x20030000;
		lstrcpy(szDownloadAddress, TEXT("0x20030000") );
	#endif

	#if (RICHED_COLOR) || (TEXT_FIND_MARK) // 2016.09.23
		txtColorRed=0;
		txtColorMegenta=0;
		txtColorBlue=0;
		txtColorGreen=0;
		txtColorYellow=0;
	#endif

	#if DNW_2ND_WINDOWS_ENABLE // 2016.10.24
		isStatusBarEnable = 0; // disable BAR_NONE
	#endif
	
	}



#if DNW_WIN_POSITION_MOVALBE // 2016.02.29

	if( isSavePosition )
	{
		lRet = RegSetValueEx(hKey, REG_X_START,		0, REG_DWORD, (const BYTE *)&WIN_XBgn, sizeof(DWORD) );
		lRet = RegSetValueEx(hKey, REG_Y_START, 	0, REG_DWORD, (const BYTE *)&WIN_YBgn, sizeof(DWORD) );
	}

	if( isSaveSize && isSavePosition ) 
	{
		// size 와 poistion이 동시에 변경되면, move 시 query 한 값으로 --
		lRet = RegSetValueEx(hKey, REG_X_TEXT,          0, REG_DWORD, (const BYTE *)&POSave_XSIZE, sizeof(DWORD) );
		lRet = RegSetValueEx(hKey, REG_Y_TEXT,          0, REG_DWORD, (const BYTE *)&POSave_YSIZE, sizeof(DWORD) );
	}
	else if( isSaveSize && (FALSE==isSavePosition) )
	{
		lRet = RegSetValueEx(hKey, REG_X_TEXT,          0, REG_DWORD, (const BYTE *)&WIN_XSIZE, sizeof(DWORD) );
		lRet = RegSetValueEx(hKey, REG_Y_TEXT,          0, REG_DWORD, (const BYTE *)&WIN_YSIZE, sizeof(DWORD) );
	}
	else // if( isSaveSize && (FALSE==isSavePosition) )
	{
		lRet = RegSetValueEx(hKey, REG_X_TEXT,          0, REG_DWORD, (const BYTE *)&WIN_XSIZE, sizeof(DWORD) );
		lRet = RegSetValueEx(hKey, REG_Y_TEXT,          0, REG_DWORD, (const BYTE *)&WIN_YSIZE, sizeof(DWORD) );
	}

	isSavePosition = FALSE;
	isSaveSize = FALSE;

#else

	lRet = RegSetValueEx(hKey, REG_X_TEXT,          0, REG_DWORD, (const BYTE *)&WIN_XSIZE, sizeof(DWORD) );
	lRet = RegSetValueEx(hKey, REG_Y_TEXT,          0, REG_DWORD, (const BYTE *)&WIN_YSIZE, sizeof(DWORD) );
	
	lRet = RegSetValueEx(hKey, REG_X_START,		0, REG_DWORD, (const BYTE *)&WIN_XBgn, sizeof(DWORD) );
	lRet = RegSetValueEx(hKey, REG_Y_START, 	0, REG_DWORD, (const BYTE *)&WIN_YBgn, sizeof(DWORD) );

#endif


	lRet = RegSetValueEx(hKey, REG_AUTO_TEXT,       0, REG_DWORD, (const BYTE *)&autoSendKey, sizeof(DWORD) );
	lRet = RegSetValueEx(hKey, REG_MSG_SAVE_TEXT,   0, REG_DWORD, (const BYTE *)&msgSaveOnOff, sizeof(DWORD) );

	lRet = RegSetValueEx(hKey, REG_FONT_TYPE,		0, REG_DWORD, (const BYTE *)&FontType, sizeof(DWORD) );


	lRet = RegSetValueEx(hKey, REG_OUT_MODE,		0, REG_DWORD, (const BYTE *)&TextorHexaMode, sizeof(DWORD) );


#if defined(COLOR_DISP2)
	lRet = RegSetValueEx(hKey, REG_COLOR_TYPE,      0, REG_DWORD, (const BYTE *)&ColorType, sizeof(DWORD) );
#endif

	lRet = RegSetValueEx(hKey, REG_RETRY_USB,		0, REG_DWORD, (const BYTE *)&RetryUSB, sizeof(DWORD) );

#if 0
	lRet = RegSetValueEx(hKey, REG_COM_Port_TEXT,   0, REG_DWORD, (const BYTE *)&userComPort, sizeof(DWORD) );
	lRet = RegSetValueEx(hKey, REG_B_RATE_IDX_TEXT, 0, REG_DWORD, (const BYTE *)&idBaudRate, sizeof(DWORD) );
	userBaudRate = baudTable[idBaudRate];

	lRet = RegSetValueEx(hKey, REG_BAUDRATE_TEXT,   0, REG_DWORD, (const BYTE *)&userBaudRate, sizeof(DWORD) );

	dwSize = sizeof(szDownloadAddress)*sizeof(TCHAR);
	lRet = RegSetValueEx(hKey, REG_DL_TEXT,         0, REG_SZ,    (PBYTE)&szDownloadAddress, dwSize );
#endif

//////////////////

#if (RICHED_COLOR) || (TEXT_FIND_MARK) // 2016.09.23
	#if 0
	lRet = RegSetValueEx(hKey, REG_COLOR_RED,       0, REG_DWORD, (const BYTE *)&txtColorRed, sizeof(DWORD) );
	lRet = RegSetValueEx(hKey, REG_COLOR_MAGENTA,   0, REG_DWORD, (const BYTE *)&txtColorMegenta, sizeof(DWORD) );
	#endif
	
	#if 0
	lRet = RegSetValueEx(hKey, REG_COLOR_BLUE,      0, REG_DWORD, (const BYTE *)&txtColorBlue, sizeof(DWORD) );
	lRet = RegSetValueEx(hKey, REG_COLOR_GREEN,     0, REG_DWORD, (const BYTE *)&txtColorGreen, sizeof(DWORD) );
	lRet = RegSetValueEx(hKey, REG_COLOR_YELLOW,    0, REG_DWORD, (const BYTE *)&txtColorYellow, sizeof(DWORD) );
	#endif

	// 2016.09.22
	dwSize = sizeof(szDataRed)*sizeof(TCHAR);
	lRet = RegSetValueEx(hKey, REG_TEXT_RED,    0, REG_SZ, (PBYTE)&szDataRed, dwSize );

	#if 0
	dwSize = sizeof(szDataMagenta)*sizeof(TCHAR);
	lRet = RegSetValueEx(hKey, REG_TEXT_MAGENTA,0, REG_SZ, (PBYTE)&szDataMagenta, dwSize );

	dwSize = sizeof(szDataBlue)*sizeof(TCHAR);
	lRet = RegSetValueEx(hKey, REG_TEXT_BLUE,   0, REG_SZ, (PBYTE)&szDataBlue, dwSize );

	dwSize = sizeof(szDataGreen)*sizeof(TCHAR);
	lRet = RegSetValueEx(hKey, REG_TEXT_GREEN,  0, REG_SZ, (PBYTE)&szDataGreen, dwSize );

	dwSize = sizeof(szDataYellow)*sizeof(TCHAR);
	lRet = RegSetValueEx(hKey, REG_TEXT_YELLOW, 0, REG_SZ, (PBYTE)&szDataYellow, dwSize );
	#endif
#endif


	lRet = RegSetValueEx(hKey, REG_MAIN_XPOS, 	0, REG_DWORD, (const BYTE *)&MainXBgn, sizeof(DWORD) );
	lRet = RegSetValueEx(hKey, REG_MAIN_YPOS, 	0, REG_DWORD, (const BYTE *)&MainYBgn, sizeof(DWORD) );
	lRet = RegSetValueEx(hKey, REG_MAIN_WID, 	0, REG_DWORD, (const BYTE *)&MainWidth, sizeof(DWORD) );
	lRet = RegSetValueEx(hKey, REG_MAIN_HEI, 	0, REG_DWORD, (const BYTE *)&MainHeight, sizeof(DWORD) );

	lRet = RegSetValueEx(hKey, REG_SAMSUNG_CPU,	0, REG_DWORD, (const BYTE *)&CPUtype, sizeof(DWORD) );

	lRet = RegSetValueEx(hKey, REG_TIME_INFO,	0, REG_DWORD, (const BYTE *)&localTime, sizeof(DWORD) ); // 2015.11.13

#if  0
	lRet = RegSetValueEx(hKey, REG_LINEFEED,	0, REG_DWORD, (const BYTE *)&cmdCRLF, sizeof(DWORD) ); // 2019.12.21
#endif

	lRet = RegSetValueEx(hKey, REG_LOGFILE_TYPE, 0, REG_DWORD, (const BYTE *)&LogFileType, sizeof(DWORD) ); // 2020.04.07


	lRet = RegSetValueEx(hKey, REG_DNW_COMPORT, 0, REG_DWORD, (const BYTE *)&userComPort, sizeof(DWORD) );
	lRet = RegSetValueEx(hKey, REG_DNW_BAUDRATE,0, REG_DWORD, (const BYTE *)&idBaudRate, sizeof(DWORD) );


#ifdef COM_SETTINGS /// 2015.12.30
	lRet = RegSetValueEx(hKey, REG_DNW_COM_PARITY,0, REG_DWORD, (const BYTE *)&userCOMparity, sizeof(DWORD) );
	lRet = RegSetValueEx(hKey, REG_DNW_COM_STOPBIT,0, REG_DWORD, (const BYTE *)&userCOMstop, sizeof(DWORD) );
	lRet = RegSetValueEx(hKey, REG_DNW_COM_DATABIT,0, REG_DWORD, (const BYTE *)&userCOMdataIdx, sizeof(DWORD) );
#endif

	lRet = RegSetValueEx(hKey, REG_DNW_COM_FLOWCTRL,0, REG_DWORD, (const BYTE *)&userFlowControl, sizeof(DWORD) ); // 2017.08.03 None Flow COntrol

	lRet = RegSetValueEx(hKey, REG_DNW_COM_FLOWCTRL_VAL,0, REG_DWORD, (const BYTE *)&userFlowCtrlVal, sizeof(DWORD) ); // 2017.08.04 None Flow COntrol


	lRet = RegSetValueEx(hKey, REG_DNW_HEXA_DISP_VAL,0, REG_DWORD, (const BYTE *)&userHexaDispVal, sizeof(DWORD) ); // 2018.03.22 Hexa Display



#if USER_SCREEN_BUF_SIZE // 2016.02.14
	lRet = RegSetValueEx(hKey, REG_DNW_SCR_BUF_SIZE,0, REG_DWORD, (const BYTE *)&userScrBufSizIndex, sizeof(DWORD) );
#endif


	if( 0xFFFF != idBaudRate )
		userBaudRate = baudTable[idBaudRate];
	lRet = RegSetValueEx(hKey, REG_DNW_USRBAUD,	0, REG_DWORD, (const BYTE *)&userBaudRate, sizeof(DWORD) );	

	dwSize = sizeof(szDownloadAddress)*sizeof(TCHAR);
	lRet = RegSetValueEx(hKey, REG_DNW_USB_STR_ADDR,0, REG_SZ,	  (PBYTE)&szDownloadAddress, dwSize );


	lRet = RegSetValueEx(hKey, REG_DNW_USB_ADDR_DWORD,0, REG_DWORD, (const BYTE *)&downloadAddress, sizeof(DWORD) );


	if( /* 3 */ (IDC_CPU_OTHER-IDC_CPU_SMDK6410) == CPUtype ) // Other
	{

		dwSize = sizeof(szDownloadAddress)*sizeof(TCHAR);
		lRet = RegSetValueEx(hKey, REG_DNW_USB_STR_O_ADDR,0, REG_SZ,	(PBYTE)&szDownloadAddress, dwSize );

		lRet = RegSetValueEx(hKey, REG_DNW_USB_ADDR_OTHER,0, REG_DWORD, (const BYTE *)&downloadAddress, sizeof(DWORD) );

		// EB_Printf("CPUType ==2222 saved !!!! \n");

	}



#if USE_TXDATA_TO_SERIAL
	// 2016.02.23 Tx Data 
	lRet = RegSetValueEx(hKey, REG_TX_DATA_MODE,	0, REG_DWORD, (const BYTE *)&sendTxDataType, sizeof(DWORD) );

	// 2015.03.23
	dwSize = sizeof(szSerialTxCharData)*sizeof(TCHAR);
	lRet = RegSetValueEx(hKey, REG_TX_CHAR_DAT,0, REG_SZ,	(PBYTE)&szSerialTxCharData, dwSize );


	dwSize = sizeof(szSerialTxHexaData)*sizeof(TCHAR);
	lRet = RegSetValueEx(hKey, REG_TX_HEXA_DAT,0, REG_SZ,	(PBYTE)&szSerialTxHexaData, dwSize );

	dwSize = sizeof(szSerialTxFileData)*sizeof(TCHAR);
	lRet = RegSetValueEx(hKey, REG_TX_FILE_DAT,0, REG_SZ,	(PBYTE)&szSerialTxFileData, dwSize );



	// 2016.02.23 Tx Data 
	lRet = RegSetValueEx(hKey, REG_TX_MSG_ONOFF,	0, REG_DWORD, (const BYTE *)&txDataMsgOnOff, sizeof(DWORD) );
#endif // USE_TXDATA_TO_SERIAL


#if USE_FLOAT2HEX_VALUE // 2018.07.19

	// 2018.07.19
	dwSize = sizeof(szFlt2Hex_In1)*sizeof(TCHAR);
	lRet = RegSetValueEx(hKey, REG_FLT2HEX_IN1,0, REG_SZ,	(PBYTE)&szFlt2Hex_In1, dwSize );

	// 2018.07.19
	dwSize = sizeof(szFlt2Hex_In2)*sizeof(TCHAR);
	lRet = RegSetValueEx(hKey, REG_FLT2HEX_IN2,0, REG_SZ,	(PBYTE)&szFlt2Hex_In2, dwSize );

	// 2018.07.19
	dwSize = sizeof(szFlt2Hex_In3)*sizeof(TCHAR);
	lRet = RegSetValueEx(hKey, REG_FLT2HEX_IN3,0, REG_SZ,	(PBYTE)&szFlt2Hex_In3, dwSize );


	// 2018.07.23
	lRet = RegSetValueEx(hKey, REG_FLT2HEX_ENDIAN,	0, REG_DWORD, (const BYTE *)&iFlt2HexEndian, sizeof(DWORD) );
#endif




#if HEXA_MODE_SUPPORT //  2016.03.24 Hexa mode display 
	dwSize = sizeof(szColumnNumber)*sizeof(TCHAR);
	lRet = RegSetValueEx(hKey, REG_HEXA_COLUMN, 0, REG_SZ,	(PBYTE)&szColumnNumber, dwSize );
	ColumnNumber=str2int(szColumnNumber);

	lRet = RegSetValueEx(hKey, REG_HEXA_ENDCHAR,	0, REG_DWORD, (const BYTE *)&HexaEndChar, sizeof(DWORD) );
	HexaEndChar = str2hex(szHexaEnd);

#endif	


#if USE_HEX2BIN_CONVERT // 2016.03.04
	// Hex file type
	lRet = RegSetValueEx(hKey, REG_HEX2BIN_HEX_TYPE,	0, REG_DWORD, (const BYTE *)&userHexType, sizeof(DWORD) );
#endif // USE_HEX2BIN_CONVERT


#if DNW_2ND_WINDOWS_ENABLE // 2016.10.24
	lRet = RegSetValueEx(hKey, REG_STATUS_BAR_EN,	0, REG_DWORD, (const BYTE *)&isStatusBarEnable, sizeof(DWORD) );
#endif

	#if 0
	// Log On/Off
	lRet = RegSetValueEx(hKey, REG_HEX2BIN_LOG,	0, REG_DWORD, (const BYTE *)&userHex2BinLogOnOff, sizeof(DWORD) );
	#endif


	#if 0
	// swap mode
	lRet = RegSetValueEx(hKey, REG_HEX2BIN_SWAP,	0, REG_DWORD, (const BYTE *)&userSwapWordwise, sizeof(DWORD) );
	#endif



	#if 0
	// Address Word-alignment
	lRet = RegSetValueEx(hKey, REG_HEX2BIN_ADDR_WORD_ALIGN,	0, REG_DWORD, (const BYTE *)&userAddrAlignWord, sizeof(DWORD) );
	#endif


#if USE_HEX2BIN_CONVERT // 2016.03.04	
	// CRC Type
	lRet = RegSetValueEx(hKey, REG_HEX2BIN_CRCTYPE,	0, REG_DWORD, (const BYTE *)&userCRCtype, sizeof(DWORD) );
		
	// Endian
	lRet = RegSetValueEx(hKey, REG_HEX2BIN_ENDIAN,	0, REG_DWORD, (const BYTE *)&userEndian, sizeof(DWORD) );

	#if 0
	// Hex Address Zero-Forced!
	lRet = RegSetValueEx(hKey, REG_HEX2BIN_ZERO_FORCECD, 0, REG_DWORD, (const BYTE *)&userHexAddrZeroForced, sizeof(DWORD) );
	#endif

	// CRC Verify
	lRet = RegSetValueEx(hKey, REG_HEX2BIN_CRCVERIFY,	0, REG_DWORD, (const BYTE *)&userCRCverify, sizeof(DWORD) );


	// File size Unit
	lRet = RegSetValueEx(hKey, REG_HEX2BIN_SIZEUNIT,	0, REG_DWORD, (const BYTE *)&userSizeUnit, sizeof(DWORD) );


	dwSize = sizeof(szHex2BinMaxSize)*sizeof(TCHAR);
	lRet = RegSetValueEx(hKey, REG_HEX2BIN_MAX_FILESIZE,0, REG_SZ,	  (PBYTE)&szHex2BinMaxSize, dwSize );

#if 1
	dwSize = sizeof(szPadByte)*sizeof(TCHAR);
	lRet = RegSetValueEx(hKey, REG_HEX2BIN_PADBYTE,0, REG_SZ,	  (PBYTE)&szPadByte, dwSize );
#endif

	lRet = RegSetValueEx(hKey, REG_HEX2BIN_PADBYTE_ON,	0, REG_DWORD, (const BYTE *)&userPadByteOnOff, sizeof(DWORD) );

	// input file
	if( isFileExist(szH2BFullInputFile) )
	{
		dwSize = sizeof(szH2BFullInputFile)*sizeof(TCHAR);
		lRet = RegSetValueEx(hKey, REG_HEX2BIN_INFILE,0, REG_SZ,	  (PBYTE)&szH2BFullInputFile, dwSize );

		// output filename full path
		dwSize = sizeof(szH2BFullOutputFile)*sizeof(TCHAR);
		lRet = RegSetValueEx(hKey, REG_HEX2BIN_OUTFILE,0, REG_SZ,	  (PBYTE)&szH2BFullOutputFile, dwSize );

		// input file short name
		dwSize = sizeof(szH2BInpFile)*sizeof(TCHAR);
		lRet = RegSetValueEx(hKey, REG_HEX2BIN_INFILE_SHORT,0, REG_SZ,	  (PBYTE)&szH2BInpFile, dwSize );

		// output file short name
		dwSize = sizeof(szH2BOutFile)*sizeof(TCHAR);
		lRet = RegSetValueEx(hKey, REG_HEX2BIN_OUTFILE_SHORT,0, REG_SZ,	  (PBYTE)&szH2BOutFile, dwSize );
	}
#endif // USE_HEX2BIN_CONVERT 




	#if 0 // 2015.12.17
	/* -------------------------------------------------------------------------------------- */
	dwSize = sizeof(REG_AUTHOR2_ID_TEXT)*sizeof(TCHAR);
	lRet = RegSetValueEx(hKey, REG_AUTHOR2_TEXT,  0, REG_SZ,    (PBYTE)&REG_AUTHOR2_ID_TEXT, dwSize );

	memset(strBuild, 0x00, sizeof(strBuild) );
	sprintf(strBuild,TEXT("Dec 09 2011,12:00:00"));
	dwSize = sizeof(strBuild)*sizeof(TCHAR);
	lRet = RegSetValueEx(hKey, REG_BUILDING2_TEXT, 0, REG_SZ,    (PBYTE)&strBuild, dwSize );
	#endif


	/* --- modify ------ */
	dwSize = sizeof(REG_AUTHOR1_ID_TEXT)*sizeof(TCHAR);
	lRet = RegSetValueEx(hKey, REG_AUTHOR1_TEXT,   0, REG_SZ,    (PBYTE)&REG_AUTHOR1_ID_TEXT, dwSize );

	memset(strBuild, 0x00, sizeof(strBuild) );
	sprintf(strBuild,TEXT("%s,%s"), __DATE__, __TIME__ );
	dwSize = sizeof(strBuild)*sizeof(TCHAR);
	lRet = RegSetValueEx(hKey, REG_BUILDING1_TEXT,   0, REG_SZ,    (PBYTE)&strBuild, dwSize );
	/* -------------------------------------------------------------------------------------- */


	if(hKey) { RegCloseKey(hKey);  hKey=NULL; }

	isRegSave = 0; // Save Finished!!!

	/* _endthread given to terminate */
	// _endthread();
}


void GetRegistry(void)
{
	/* HKEY_CURRENT_USER\Software\DNW 값을 읽어들인다.	*/
	DWORD dwType, dwData=4;
	HKEY hKey = NULL;
	LONG lRet;
	BOOL rewrite=FALSE;
	HANDLE regFile=NULL;
	TCHAR szLenTxt[MAX_PATH] = {0,};


	/* 키를 오픈한다. */
	if ((lRet = RegOpenKeyEx(HKEY_CURRENT_USER, REG_LOC, 0, 
				KEY_READ | KEY_QUERY_VALUE , &hKey)) == ERROR_SUCCESS)	 
	{	
		if( (lRet = RegQueryValueEx(hKey, REG_X_TEXT, NULL, &dwType, (const LPBYTE)&WIN_XSIZE, &dwData))!=ERROR_SUCCESS )	
		{	
			WIN_XSIZE = WINDOW_XSIZE;
			rewrite=TRUE;
		}	

		if((lRet = RegQueryValueEx(hKey, REG_Y_TEXT, NULL, &dwType, (const LPBYTE)&WIN_YSIZE, &dwData))!=ERROR_SUCCESS )	
		{	
			WIN_YSIZE = WINDOW_YSIZE;
			rewrite=TRUE;
		}	

	#ifdef ENGINEER_MODE /// auto Send Key default Off  : 개발자용은 필요없음.
		autoSendKey = 0; /* auto key send off */
	#else
		if((lRet = RegQueryValueEx(hKey, REG_AUTO_TEXT, NULL, &dwType, (const LPBYTE)&autoSendKey, &dwData))!=ERROR_SUCCESS )	
		{	
			autoSendKey = 0; /* auto key send off */
			rewrite=TRUE;
		}	
	#endif


	#if 1 /// LOG file save : default Off,  always Log save Off mode
		msgSaveOnOff = 0; /* Save off */
	#else
		if((lRet = RegQueryValueEx(hKey, REG_MSG_SAVE_TEXT, NULL, &dwType, (const LPBYTE)&msgSaveOnOff, &dwData))!=ERROR_SUCCESS )	
		{	
			msgSaveOnOff = 0; /* Save off */
			rewrite=TRUE;
		}
	#endif

		if((lRet = RegQueryValueEx(hKey, REG_FONT_TYPE, NULL, &dwType, (const LPBYTE)&FontType, &dwData))!=ERROR_SUCCESS )	
		{	
			FontType = 3; /// index COLOR_GRAY;
			rewrite=TRUE;
		}	


		if((lRet = RegQueryValueEx(hKey, REG_OUT_MODE, NULL, &dwType, (const LPBYTE)&TextorHexaMode, &dwData))!=ERROR_SUCCESS )	
		{	
			TextorHexaMode = 0; /* Default : Text mode,   Text Mode:0, Hexa Line Mode:1, Hexa End Char */
			rewrite=TRUE;
		}	

	#if HEXA_MODE_SUPPORT 
		uCount = 0;
	#endif


	
	#if defined(COLOR_DISP2)
		if((lRet = RegQueryValueEx(hKey, REG_COLOR_TYPE, NULL, &dwType, (const LPBYTE)&ColorType, &dwData))!=ERROR_SUCCESS )	
		{	
			ColorType = 0; /// index COLOR_GRAY;
			rewrite=TRUE;
		}	
	#endif


#if (RICHED_COLOR) || (TEXT_FIND_MARK) // 2016.09.23

	#if 0
		if((lRet = RegQueryValueEx(hKey, REG_COLOR_RED, NULL, &dwType, (const LPBYTE)&txtColorRed, &dwData))!=ERROR_SUCCESS )	
		{	
			txtColorRed = 0;
			rewrite=TRUE;
		}	

		if((lRet = RegQueryValueEx(hKey, REG_COLOR_MAGENTA, NULL, &dwType, (const LPBYTE)&txtColorMegenta, &dwData))!=ERROR_SUCCESS )	
		{	
			txtColorMegenta = 0;
			rewrite=TRUE;
		}	
	#else
		txtColorRed = 0;
		txtColorMegenta = 0;
	#endif

	#if 0
		if((lRet = RegQueryValueEx(hKey, REG_COLOR_BLUE, NULL, &dwType, (const LPBYTE)&txtColorBlue, &dwData))!=ERROR_SUCCESS )	
		{	
			txtColorBlue = 0;
			rewrite=TRUE;
		}	
	
		if((lRet = RegQueryValueEx(hKey, REG_COLOR_GREEN, NULL, &dwType, (const LPBYTE)&txtColorGreen, &dwData))!=ERROR_SUCCESS ) 
		{	
			txtColorGreen = 0;
			rewrite=TRUE;
		}	

		if((lRet = RegQueryValueEx(hKey, REG_COLOR_YELLOW, NULL, &dwType, (const LPBYTE)&txtColorYellow, &dwData))!=ERROR_SUCCESS ) 
		{	
			txtColorYellow = 0;
			rewrite=TRUE;
		}	
	#endif


	#if 0 // 2017.4.12	
		// 2016.09.22
		dwData = sizeof(szDataRed);
		memset(szDataRed, 0x00, dwData );
		if((lRet = RegQueryValueEx(hKey, REG_TEXT_RED, NULL, &dwType, (const LPBYTE)szDataRed, &dwData))!=ERROR_SUCCESS )
		{	
			lstrcpy(szDataRed,TEXT(""));
			rewrite=TRUE;
		}
		iLenDataRed = lstrlen(szDataRed);
		HashRed = make_hash_p(szDataRed, iLenDataRed);
	#endif

	#if 0
		dwData = sizeof(szDataMagenta);
		memset(szDataMagenta, 0x00, dwData );
		if((lRet = RegQueryValueEx(hKey, REG_TEXT_MAGENTA, NULL, &dwType, (const LPBYTE)szDataMagenta, &dwData))!=ERROR_SUCCESS )
		{	
			lstrcpy(szDataMagenta,TEXT(""));
			rewrite=TRUE;
		}	
		iLenDataMagenta = lstrlen(szDataMagenta);
		HashMagenta = make_hash_p(szDataMagenta, iLenDataMagenta);
		

		dwData = sizeof(szDataBlue);
		memset(szDataBlue, 0x00, dwData );
		if((lRet = RegQueryValueEx(hKey, REG_TEXT_BLUE, NULL, &dwType, (const LPBYTE)szDataBlue, &dwData))!=ERROR_SUCCESS )
		{	
			lstrcpy(szDataBlue,TEXT(""));
			rewrite=TRUE;
		}
		
		iLenDataBlue = lstrlen(szDataBlue);

		dwData = sizeof(szDataGreen);
		memset(szDataGreen, 0x00, dwData );
		if((lRet = RegQueryValueEx(hKey, REG_TEXT_GREEN, NULL, &dwType, (const LPBYTE)szDataGreen, &dwData))!=ERROR_SUCCESS )
		{	
			lstrcpy(szDataGreen,TEXT(""));
			rewrite=TRUE;
		}	
		iLenDataGreen = lstrlen(szDataGreen);

		dwData = sizeof(szDataYellow);
		memset(szDataYellow, 0x00, dwData );
		if((lRet = RegQueryValueEx(hKey, REG_TEXT_YELLOW, NULL, &dwType, (const LPBYTE)szDataYellow, &dwData))!=ERROR_SUCCESS )
		{	
			lstrcpy(szDataYellow,TEXT(""));
			rewrite=TRUE;
		}	
		iLenDataYellow = lstrlen(szDataYellow);
	#endif

		IsMarking();

#endif


	#ifdef ENGINEER_MODE /// auto Send Key default Off	: 개발자용은 필요없음.
		RetryUSB = 0; 
	#else
		if((lRet = RegQueryValueEx(hKey, REG_RETRY_USB, NULL, &dwType, (const LPBYTE)&RetryUSB, &dwData))!=ERROR_SUCCESS )	
		{	
			RetryUSB = 0; /// default,  if 20, then 10*20=200,  if 20, then 40*20=800, if 
			rewrite=TRUE;
		}	
	#endif


	#if 0
		if((lRet = RegQueryValueEx(hKey, REG_COM_Port_TEXT, NULL, &dwType, (const LPBYTE)&userComPort, &dwData))!=ERROR_SUCCESS )	
		{	
			userComPort = 1;
			rewrite=TRUE;
		}	

		if((lRet = RegQueryValueEx(hKey, REG_B_RATE_IDX_TEXT, NULL, &dwType, (const LPBYTE)&idBaudRate, &dwData))!=ERROR_SUCCESS )	
		{	
			idBaudRate = 9; /* 115,200 bps*/
			rewrite=TRUE;
		}	
		userBaudRate = baudTable[idBaudRate];


		dwData = sizeof(szDownloadAddress);
		memset(szDownloadAddress, 0x00, dwData );
		if((lRet = RegQueryValueEx(hKey, REG_DL_TEXT, NULL, &dwType, (const LPBYTE)szDownloadAddress, &dwData))!=ERROR_SUCCESS )	
		{	
			lstrcpy(szDownloadAddress, TEXT("0x20030000") );
			rewrite=TRUE;
		}	
	#endif


		// ------------------------------------------------------------------
		if((lRet = RegQueryValueEx(hKey, REG_X_START, NULL, &dwType, (const LPBYTE)&WIN_XBgn, &dwData))!=ERROR_SUCCESS )	
		{	
			WIN_XBgn = 0; /// window x start
			rewrite=TRUE;
		}

	#if USE_WIN_OUTOF_AREA_TO_ZERO // 2016.03.31 LCD size
		// 2016.03.31
		myLCDwidth  = GetSystemMetrics(SM_CXSCREEN); // 2017.04.20
		myLCDheight = GetSystemMetrics(SM_CYSCREEN); // 2017.04.20

		myVirWidth	= GetSystemMetrics(SM_CXVIRTUALSCREEN);
		myVirHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

		//if( (WIN_XBgn > myLCDwidth-WIN_LIMIT_X_MARGIN) || WIN_XBgn < 0 )
		if( (WIN_XBgn > myVirWidth-WIN_LIMIT_X_MARGIN) || WIN_XBgn < 0 )   // 2017.08.01, : myLCDwidth -> myVirWidth
		{
			WIN_XBgn = 0;
		}
	#endif
		// ------------------------------------------------------------------


		// ------------------------------------------------------------------
		if((lRet = RegQueryValueEx(hKey, REG_Y_START, NULL, &dwType, (const LPBYTE)&WIN_YBgn, &dwData))!=ERROR_SUCCESS )	
		{	
			WIN_YBgn = 0; /// window y start
			rewrite=TRUE;
		}	
	#if USE_WIN_OUTOF_AREA_TO_ZERO // 2016.03.31 LCD size
		// 2016.03.31
		if( (WIN_YBgn > myLCDheight-WIN_LIMIT_Y_MARGIN) || WIN_YBgn < 0 ) 
		{
			WIN_YBgn = 0;
		}
		// ------------------------------------------------------------------
	#endif

		/* main window */
		if((lRet = RegQueryValueEx(hKey, REG_MAIN_XPOS, NULL, &dwType, (const LPBYTE)&MainXBgn, &dwData))!=ERROR_SUCCESS )	
		{	
			MainXBgn = 0; /// window y start
			rewrite=TRUE;
		}	
		if((lRet = RegQueryValueEx(hKey, REG_MAIN_YPOS, NULL, &dwType, (const LPBYTE)&MainYBgn, &dwData))!=ERROR_SUCCESS )	
		{	
			MainYBgn = 0; /// window y start
			rewrite=TRUE;
		}	
		if((lRet = RegQueryValueEx(hKey, REG_MAIN_WID, NULL, &dwType, (const LPBYTE)&MainWidth, &dwData))!=ERROR_SUCCESS )	
		{	
			MainWidth = 0; /// window y start
			rewrite=TRUE;
		}	
		if((lRet = RegQueryValueEx(hKey, REG_MAIN_HEI, NULL, &dwType, (const LPBYTE)&MainHeight, &dwData))!=ERROR_SUCCESS )	
		{	
			MainHeight = 0; /// window y start
			rewrite=TRUE;
		}	


		// 2015.11.13 added. PC Time display
		if((lRet = RegQueryValueEx(hKey, REG_TIME_INFO, NULL, &dwType, (const LPBYTE)&localTime, &dwData))!=ERROR_SUCCESS )	
		{	
			localTime = 0; // 2015.11.13
			rewrite=TRUE;
		}
		if( localTime ) isTimeInfoOnce=1; // 2016.04.02
		else isTimeInfoOnce = 0; // 2016.04.02


	#if 0 // 2020.08.14
		// 2019.12.21 added
		if((lRet = RegQueryValueEx(hKey, REG_LINEFEED, NULL, &dwType, (const LPBYTE)&cmdCRLF, &dwData))!=ERROR_SUCCESS )	
		{	
			cmdCRLF = 0; // 2019.12.21
			rewrite=TRUE;
		}
	#endif

		if((lRet = RegQueryValueEx(hKey, REG_LOGFILE_TYPE, NULL, &dwType, (const LPBYTE)&LogFileType, &dwData))!=ERROR_SUCCESS )	
		{	
			LogFileType = 0; // 2020.04.07
			rewrite=TRUE;
		}



		if((lRet = RegQueryValueEx(hKey, REG_DNW_COMPORT, NULL, &dwType, (const LPBYTE)&userComPort, &dwData))!=ERROR_SUCCESS )	
		{	
			userComPort = 1; /// COM port
			rewrite=TRUE;
		}

		if((lRet = RegQueryValueEx(hKey, REG_DNW_BAUDRATE, NULL, &dwType, (const LPBYTE)&idBaudRate, &dwData))!=ERROR_SUCCESS )	
		{	
			idBaudRate = 9; /// Baudrate Index:8 -> 115200bps
			rewrite=TRUE;
		}

		if((lRet = RegQueryValueEx(hKey, REG_DNW_USRBAUD, NULL, &dwType, (const LPBYTE)&userBaudRate, &dwData))!=ERROR_SUCCESS )	
		{	
			if( 0xFFFF != idBaudRate )
				userBaudRate = baudTable[idBaudRate]; /// Baudrate Index:8 -> 115200bps
			rewrite=TRUE;
		}

		
	#ifdef COM_SETTINGS /// 2015.12.30
		if((lRet = RegQueryValueEx(hKey, REG_DNW_COM_PARITY, NULL, &dwType, (const LPBYTE)&userCOMparity, &dwData))!=ERROR_SUCCESS ) 
		{	
			userCOMparity = 0;
			rewrite=TRUE;
		}

		if((lRet = RegQueryValueEx(hKey, REG_DNW_COM_STOPBIT, NULL, &dwType, (const LPBYTE)&userCOMstop, &dwData))!=ERROR_SUCCESS ) 
		{	
			userCOMstop = 0;
			rewrite=TRUE;
		}

		if((lRet = RegQueryValueEx(hKey, REG_DNW_COM_DATABIT, NULL, &dwType, (const LPBYTE)&userCOMdataIdx, &dwData))!=ERROR_SUCCESS ) 
		{	
			userCOMdataIdx = 1;
			rewrite=TRUE;
		}
	#endif

		// 2017.08.03 None Flow COntrol
		if((lRet = RegQueryValueEx(hKey, REG_DNW_COM_FLOWCTRL, NULL, &dwType, (const LPBYTE)&userFlowControl, &dwData))!=ERROR_SUCCESS ) 
		{	
			userFlowControl = 0;
			rewrite=TRUE;
		}


		// 2017.08.04 None Flow COntrol
		if((lRet = RegQueryValueEx(hKey, REG_DNW_COM_FLOWCTRL_VAL, NULL, &dwType, (const LPBYTE)&userFlowCtrlVal, &dwData))!=ERROR_SUCCESS ) 
		{	
			userFlowCtrlVal = 0;
			rewrite=TRUE;
		}

		// 2018.03.22 Hexa display
		if((lRet = RegQueryValueEx(hKey, REG_DNW_HEXA_DISP_VAL, NULL, &dwType, (const LPBYTE)&userHexaDispVal, &dwData))!=ERROR_SUCCESS ) 
		{	
			userHexaDispVal = 0;
			rewrite=TRUE;
		}


		

	#if USER_SCREEN_BUF_SIZE // 2016.02.14
		if((lRet = RegQueryValueEx(hKey, REG_DNW_SCR_BUF_SIZE, NULL, &dwType, (const LPBYTE)&userScrBufSizIndex, &dwData))!=ERROR_SUCCESS ) 
		{	
			userScrBufSizIndex = 0;
			rewrite=TRUE;
		}
	#endif


		if((lRet = RegQueryValueEx(hKey, REG_SAMSUNG_CPU, NULL, &dwType, (const LPBYTE)&CPUtype, &dwData))!=ERROR_SUCCESS )	
		{	
			CPUtype = 1; // SMDKC100
			rewrite=TRUE;
		}

		dwData = sizeof(szDownloadAddress);
		memset(szDownloadAddress, 0x00, dwData );
		if((lRet = RegQueryValueEx(hKey, REG_DNW_USB_STR_ADDR, NULL, &dwType, (const LPBYTE)szDownloadAddress, &dwData))!=ERROR_SUCCESS )	
		{	
			lstrcpy(szDownloadAddress, TEXT("0x20030000") );  // SMDKC100
			rewrite=TRUE;
		}	

		if((lRet = RegQueryValueEx(hKey, REG_DNW_USB_ADDR_DWORD, NULL, &dwType, (const LPBYTE)&downloadAddress, &dwData))!=ERROR_SUCCESS )	
		{	
			downloadAddress = 0x20030000; // SMDKC100
			rewrite=TRUE;
		}
		

	#if HEXA_MODE_SUPPORT //  2016.03.24 Hexa mode display 
		dwData = sizeof(szColumnNumber);
		memset(szColumnNumber, 0x00, dwData );
		if((lRet = RegQueryValueEx(hKey, REG_HEXA_COLUMN, NULL, &dwType, (const LPBYTE)szColumnNumber, &dwData))!=ERROR_SUCCESS )	
		{	
		#if 1 // 2017.04.21
			ColumnNumber = 0;
			lstrcpy(szColumnNumber, TEXT("0") );
		#else
			ColumnNumber = 16;
			lstrcpy(szColumnNumber, TEXT("16") );
		#endif
			rewrite=TRUE;
			uCount = 0;
		}
		else {
			ColumnNumber = str2int(szColumnNumber);
		}

		if((lRet = RegQueryValueEx(hKey, REG_HEXA_ENDCHAR, NULL, &dwType, (const LPBYTE)&HexaEndChar, &dwData))!=ERROR_SUCCESS ) 
		{	
			HexaEndChar = 0x0D0A;

			if( HexaEndChar > 0xFFFF ) // 2018.03.21
				wsprintf(szHexaEnd, TEXT("%06X"), HexaEndChar);
			else if( HexaEndChar > 0xFF ) // 2018.03.21
				wsprintf(szHexaEnd, TEXT("%04X"), HexaEndChar);
			else
				wsprintf(szHexaEnd, TEXT("%02X"), HexaEndChar);

			rewrite=TRUE;
		}
		else {

			if( HexaEndChar > 0xFFFF ) // 2018.03.21
				wsprintf(szHexaEnd, TEXT("%06X"), HexaEndChar);
			else if( HexaEndChar > 0xFF ) // 2018.03.21
				wsprintf(szHexaEnd, TEXT("%04X"), HexaEndChar);
			else
				wsprintf(szHexaEnd, TEXT("%02X"), HexaEndChar);

			HexaEndChar = str2hex(szHexaEnd);
		}
	#endif	


		if( /* 3 */ (IDC_CPU_OTHER-IDC_CPU_SMDK6410) == CPUtype ) // Other
		{

			dwData = sizeof(szDownloadAddress);
			memset(szDownloadAddress, 0x00, dwData );
			if((lRet = RegQueryValueEx(hKey, REG_DNW_USB_STR_O_ADDR, NULL, &dwType, (const LPBYTE)szDownloadAddress, &dwData))!=ERROR_SUCCESS )
			{	
				lstrcpy(szDownloadAddress, TEXT("0x20030000") );  // SMDKC100
				rewrite=TRUE;
			}	

			if((lRet = RegQueryValueEx(hKey, REG_DNW_USB_ADDR_OTHER, NULL, &dwType, (const LPBYTE)&downloadAddress, &dwData))!=ERROR_SUCCESS )	
			{	
				downloadAddress = 0x20030000; // SMDKC100
				rewrite=TRUE;
			}

			// EB_Printf("CPUType ==2 \n");
		}


#if USE_TXDATA_TO_SERIAL

		// 2016.02.23 Tx Data 
		if((lRet = RegQueryValueEx(hKey, REG_TX_DATA_MODE, NULL, &dwType, (const LPBYTE)&sendTxDataType, &dwData))!=ERROR_SUCCESS )	
		{	
			sendTxDataType = 0; // char 
			rewrite=TRUE;
		}

		// 2016.02.23 Tx Data 
		dwData = sizeof(szSerialTxCharData);
		memset(szSerialTxCharData, 0x00, dwData );
		if((lRet = RegQueryValueEx(hKey, REG_TX_CHAR_DAT, NULL, &dwType, (const LPBYTE)szSerialTxCharData, &dwData))!=ERROR_SUCCESS )
		{	
			lstrcpy(szSerialTxCharData,TEXT(" qUIz")); // SiRFprima2 boot menu password
			rewrite=TRUE;
		}

		dwData = sizeof(szSerialTxHexaData);
		memset(szSerialTxHexaData, 0x00, dwData );
		if((lRet = RegQueryValueEx(hKey, REG_TX_HEXA_DAT, NULL, &dwType, (const LPBYTE)szSerialTxHexaData, &dwData))!=ERROR_SUCCESS )
		{	
			lstrcpy(szSerialTxHexaData,TEXT("0x207155497a")); // "qUIz"
			rewrite=TRUE;
		}

		dwData = sizeof(szSerialTxFileData);
		memset(szSerialTxFileData, 0x00, dwData );
		if((lRet = RegQueryValueEx(hKey, REG_TX_FILE_DAT, NULL, &dwType, (const LPBYTE)szSerialTxFileData, &dwData))!=ERROR_SUCCESS )
		{	
			lstrcpy(szSerialTxFileData,TEXT(" ")); // 
			rewrite=TRUE;
		}


		iidx = strlen(szSerialTxFileData);
		if( iidx > MAX_PATH )
		{
			EB_Printf(TEXT("\r\n[dnw] Input file length is over! length(%d) \r\n"), iidx );
		}

		isFind=0;
		for(jidx=iidx; iidx>0; jidx--)
		{
			if( szSerialTxFileData[jidx] == '\\' ) { isFind=1; break; }
		}
		strcpy( szSerialTxDispName, &szSerialTxFileData[jidx+1] );

		regFile = CreateFile(szSerialTxFileData,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( regFile ) {
			ulTxDataUARTFileSize = GetTxDataFileSize(regFile);
		}
		CloseHandle(regFile);
		
		wsprintf(szLenTxt, TEXT(" == (%I64d Byte)"), ulTxDataUARTFileSize);
		strcat( szSerialTxDispName, szLenTxt );
			



		// 2016.02.23 Tx Data 
		if((lRet = RegQueryValueEx(hKey, REG_TX_MSG_ONOFF, NULL, &dwType, (const LPBYTE)&txDataMsgOnOff, &dwData))!=ERROR_SUCCESS ) 
		{	
			txDataMsgOnOff = 0; // off 
			rewrite=TRUE;
		}
#endif


	#if USE_FLOAT2HEX_VALUE // 2018.07.19

		dwData = sizeof(szFlt2Hex_In1);
		memset(szFlt2Hex_In1, 0x00, dwData );
		if((lRet = RegQueryValueEx(hKey, REG_FLT2HEX_IN1, NULL, &dwType, (const LPBYTE)szFlt2Hex_In1, &dwData))!=ERROR_SUCCESS )
		{	
			lstrcpy(szFlt2Hex_In1,TEXT("0.01234")); // start
			rewrite=TRUE;
		}

		dwData = sizeof(szFlt2Hex_In2);
		memset(szFlt2Hex_In2, 0x00, dwData );
		if((lRet = RegQueryValueEx(hKey, REG_FLT2HEX_IN2, NULL, &dwType, (const LPBYTE)szFlt2Hex_In2, &dwData))!=ERROR_SUCCESS )
		{	
			lstrcpy(szFlt2Hex_In2,TEXT("10.5273")); // 2nd
			rewrite=TRUE;
		}

		dwData = sizeof(szFlt2Hex_In3);
		memset(szFlt2Hex_In3, 0x00, dwData );
		if((lRet = RegQueryValueEx(hKey, REG_FLT2HEX_IN3, NULL, &dwType, (const LPBYTE)szFlt2Hex_In3, &dwData))!=ERROR_SUCCESS )
		{	
			lstrcpy(szFlt2Hex_In3,TEXT("0.01"));	// step
			rewrite=TRUE;
		}

		if((lRet = RegQueryValueEx(hKey, REG_FLT2HEX_ENDIAN, NULL, &dwType, (const LPBYTE)&iFlt2HexEndian, &dwData))!=ERROR_SUCCESS ) 
		{	
			iFlt2HexEndian = 0; // little endian 
			rewrite=TRUE;
		}
	#endif


	#if USE_HEX2BIN_CONVERT // 2016.03.04
		// Hex file type
		if((lRet = RegQueryValueEx(hKey, REG_HEX2BIN_HEX_TYPE, NULL, &dwType, (const LPBYTE)&userHexType, &dwData))!=ERROR_SUCCESS )	
		{	
			userHexType = 0; // Intel
			rewrite=TRUE;
		}
	#endif	


	
	#if DNW_2ND_WINDOWS_ENABLE // 2016.10.24
		if((lRet = RegQueryValueEx(hKey, REG_STATUS_BAR_EN, NULL, &dwType, (const LPBYTE)&isStatusBarEnable, &dwData))!=ERROR_SUCCESS )	
		{	
			isStatusBarEnable = BAR_NONE; // disable
			rewrite=TRUE;
		}
	#endif



#if USE_HEX2BIN_CONVERT // 2016.03.04

	#if 0
		// Log On/Off
		if((lRet = RegQueryValueEx(hKey, REG_HEX2BIN_LOG, NULL, &dwType, (const LPBYTE)&userHex2BinLogOnOff, &dwData))!=ERROR_SUCCESS )	
		{	
			userHex2BinLogOnOff = 0;
			rewrite=TRUE;
		}
	#else

		userHex2BinLogOnOff = 0; // Log off!!
		Verbose_Flag = 0;  // Log off!!
	#endif



	#if 0
		// swap mode
		if((lRet = RegQueryValueEx(hKey, REG_HEX2BIN_SWAP, NULL, &dwType, (const LPBYTE)&userSwapWordwise, &dwData))!=ERROR_SUCCESS )	
		{	
			userSwapWordwise = 0;
			rewrite=TRUE;
		}
	#else

		userSwapWordwise = 0; // initial Swap Off mode
		Swap_Wordwise = 0; // Swap Off mode
	#endif



	#if 0
		// Address Word-alignment
		if((lRet = RegQueryValueEx(hKey, REG_HEX2BIN_ADDR_WORD_ALIGN, NULL, &dwType, (const LPBYTE)&userAddrAlignWord, &dwData))!=ERROR_SUCCESS )	
		{	
			userAddrAlignWord = 0;
			rewrite=TRUE;
		}
	#else

		userAddrAlignWord = 0;
		Address_Alignment_Word = 0;
	#endif


			
		// CRC Type
		if((lRet = RegQueryValueEx(hKey, REG_HEX2BIN_CRCTYPE, NULL, &dwType, (const LPBYTE)&userCRCtype, &dwData))!=ERROR_SUCCESS )	
		{	
			userCRCtype = 2; // CRC8
			rewrite=TRUE;
		}
					
		// Endian
		if((lRet = RegQueryValueEx(hKey, REG_HEX2BIN_ENDIAN, NULL, &dwType, (const LPBYTE)&userEndian, &dwData))!=ERROR_SUCCESS )	
		{	
			userEndian = 0;
			rewrite=TRUE;
		}

		// pad-bytemode // 2018.07.06
		if((lRet = RegQueryValueEx(hKey, REG_HEX2BIN_PADBYTE_ON, NULL, &dwType, (const LPBYTE)&userPadByteOnOff, &dwData))!=ERROR_SUCCESS ) 
		{	
			userPadByteOnOff = 1;
			rewrite=TRUE;
		}


	#if 0 // 2020.07.03
		// Hex Address Zero based!!
		if((lRet = RegQueryValueEx(hKey, REG_HEX2BIN_ZERO_FORCECD, NULL, &dwType, (const LPBYTE)&userHexAddrZeroForced, &dwData))!=ERROR_SUCCESS )	
		{	
			userHexAddrZeroForced = 0;
			rewrite=TRUE;
		}
	#else
		Enable_HexaAddr_Zero_Forced = userHexAddrZeroForced = 0;
	#endif

	#if 0
		if( HEX2BIN_ZERO_FORCED==userHexAddrZeroForced ) Enable_HexaAddr_Zero_Forced = 1;
		else if( HEX2BIN_ZERO_FORCED==userHexAddrZeroForced ) Enable_HexaAddr_Zero_Forced = 2;
		else Enable_HexaAddr_Zero_Forced = 0;
	#endif


		// CRC Verify
		if((lRet = RegQueryValueEx(hKey, REG_HEX2BIN_CRCVERIFY, NULL, &dwType, (const LPBYTE)&userCRCverify, &dwData))!=ERROR_SUCCESS )	
		{	
			userCRCverify = 1; // On
			rewrite=TRUE;
		}

		// Size Unit
		if((lRet = RegQueryValueEx(hKey, REG_HEX2BIN_SIZEUNIT, NULL, &dwType, (const LPBYTE)&userSizeUnit, &dwData))!=ERROR_SUCCESS )	
		{	
			userSizeUnit = 0; // Kbyte
			rewrite=TRUE;
		}


		dwData = sizeof(szHex2BinMaxSize);
		memset(szHex2BinMaxSize, 0x00, dwData );
		if((lRet = RegQueryValueEx(hKey, REG_HEX2BIN_MAX_FILESIZE, NULL, &dwType, (const LPBYTE)szHex2BinMaxSize, &dwData))!=ERROR_SUCCESS )
		{	
			lstrcpy(szHex2BinMaxSize, TEXT("640") );  // default : Max File size : 640 Kbyte
			dwHex2BinMaxSize = 640; // KB
			rewrite=TRUE;
		}	
		else
		{
			szHex2BinMaxSize[15]='\0';
			dwHex2BinMaxSize = str2int(szHex2BinMaxSize);	
		}



	#if 1
		dwData = sizeof(szPadByte);
		memset(szPadByte, 0x00, dwData );
		if((lRet = RegQueryValueEx(hKey, REG_HEX2BIN_PADBYTE, NULL, &dwType, (const LPBYTE)szPadByte, &dwData))!=ERROR_SUCCESS )
		{	
			lstrcpy(szPadByte, TEXT("FF") );  // default : 0xff
			Pad_Byte = dwPadByte = 0xff; // KB
			rewrite=TRUE;
		}	
		else
		{
			szPadByte[15]='\0';
			Pad_Byte = dwPadByte = str2hex(szPadByte);	
		}
	#endif
		
		

		// input file
		dwData = sizeof(szH2BFullInputFile);
		memset(szH2BFullInputFile, 0x00, dwData );
		if((lRet = RegQueryValueEx(hKey, REG_HEX2BIN_INFILE, NULL, &dwType, (const LPBYTE)szH2BFullInputFile, &dwData))!=ERROR_SUCCESS )
		{	
			rewrite=TRUE;
		}	

		// output file
		dwData = sizeof(szH2BFullOutputFile);
		memset(szH2BFullOutputFile, 0x00, dwData );
		if((lRet = RegQueryValueEx(hKey, REG_HEX2BIN_OUTFILE, NULL, &dwType, (const LPBYTE)szH2BFullOutputFile, &dwData))!=ERROR_SUCCESS )
		{	
			rewrite=TRUE;
		}	

		// input file short name
		dwData = sizeof(szH2BInpFile);
		memset(szH2BInpFile, 0x00, dwData );
		if((lRet = RegQueryValueEx(hKey, REG_HEX2BIN_INFILE_SHORT, NULL, &dwType, (const LPBYTE)szH2BInpFile, &dwData))!=ERROR_SUCCESS )
		{	
			rewrite=TRUE;
		}	
		
		// output file short name
		dwData = sizeof(szH2BOutFile);
		memset(szH2BOutFile, 0x00, dwData );
		if((lRet = RegQueryValueEx(hKey, REG_HEX2BIN_OUTFILE_SHORT, NULL, &dwType, (const LPBYTE)szH2BOutFile, &dwData))!=ERROR_SUCCESS )
		{	
			rewrite=TRUE;
		}	




#endif

		RegCloseKey(hKey);	

	}
	else // ERROR
	{
		WIN_XSIZE = WINDOW_XSIZE;
		WIN_YSIZE = WINDOW_YSIZE;
		autoSendKey = 0; /* auto key send off */

		msgSaveOnOff = 0; /* Save off */
		TextorHexaMode = 0; /* 2011.02.08 Text Mode:0, Hexa Line Mode:1, Hexa End Char */
#if HEXA_MODE_SUPPORT 
		uCount = 0; // 2016.04.02
#endif


		FontType = 3; // system font
#if defined(COLOR_DISP2)
		ColorType = 0; /// index COLOR_GRAY;
#endif

	#ifdef ENGINEER_MODE /// auto Send Key default Off	: 개발자용은 필요없음.
		RetryUSB = 0;
	#else
		RetryUSB = 1;
	#endif


		CPUtype = 1; /// SMDKC100

		// 2018.07.18
		downloadAddress=0x20030000;
		memset( szDownloadAddress, 0x00, sizeof(szDownloadAddress) );
		lstrcpy(szDownloadAddress,TEXT("0x20030000"));

	

		// 2015.11.13, added
		localTime = 0; // No Time
		isTimeInfoOnce = 0; // 2016.04.02

		// 2019.12.21
		cmdCRLF = 0;

		LogFileType = 0;


		userComPort  = 1;
		idBaudRate   = 9; // -- 115200bps
		userBaudRate = baudTable[idBaudRate];

#ifdef COM_SETTINGS /// 2015.12.30
		userCOMparity  = 0;
		userCOMstop    = 0;
		userCOMdataIdx = 1;
		userFlowControl= 0; // 2017.08.03, FLow COntrol
		userFlowCtrlVal= 0; // 2017.08.04 None Flow COntrol
#endif

		userHexaDispVal = 0; // 2018.03.22
		

#if USER_SCREEN_BUF_SIZE // 2016.02.14
		userScrBufSizIndex = 0; // large
#endif

#if 0
		userComPort = 1;
		idBaudRate = 9; /* 115,200 bps*/
		userBaudRate = baudTable[idBaudRate];
		downloadAddress = 0x20030000;
		lstrcpy(szDownloadAddress, TEXT("0x20030000") );
#endif



	#if USE_HEX2BIN_CONVERT // 2016.03.04
		// Hex file type
		userHexType = 0; // intel

		// Log On/Off
		userHex2BinLogOnOff = 0; // Log off
		Verbose_Flag = 0;  // Log Off

		// CRC Type
		userCRCtype = 2; // CRC8:default

		// Endian
		userEndian = 0; // Little Endian

		// Hex Address Zero Based!!
		userHexAddrZeroForced = 0;
		Enable_HexaAddr_Zero_Forced = 0;

		// CRC Verify
		userCRCverify = 1; // On

		// Address Word-alignment
		userAddrAlignWord = 0;
		Address_Alignment_Word = 0;

		userSizeUnit = 0;
	#endif // USE_HEX2BIN_CONVERT 


	#if USE_TXDATA_TO_SERIAL
		// 2016.03.23 Tx Data
		sendTxDataType = 0;
		txDataMsgOnOff = 0; // off 
		lstrcpy(szSerialTxCharData,TEXT(" qUIz")); // SiRFprima2 boot menu password
		lstrcpy(szSerialTxHexaData,TEXT("0x207155497a"));
		lstrcpy(szSerialTxFileData,TEXT(""));
	#endif

	
	#if HEXA_MODE_SUPPORT //  2016.03.24 Hexa mode display 

		#if 1 // 2017.04.21
		ColumnNumber = 0;
		lstrcpy(szColumnNumber,TEXT("0"));
		#else
		ColumnNumber = 16;
		lstrcpy(szColumnNumber,TEXT("16"));
		#endif
		
		HexaEndChar = 0x0D0A;

		TextorHexaMode = 0; /* Default : Text mode,   Text Mode:0, Hexa Line Mode:1, Hexa End Char */
		uCount = 0;
	#endif	
	
#if (RICHED_COLOR) || (TEXT_FIND_MARK) // 2016.09.23
		txtColorRed=0;
		txtColorMegenta=0;
		txtColorBlue=0;
		txtColorGreen=0;
		txtColorYellow=0;

		txtFindRed=0;
		txtFindMegenta=0;
		txtFindBlue=0;
		txtFindGreen=0;
		txtFindYellow=0;


		// 2016.09.22
		memset(szDataRed, 0x00, sizeof(szDataRed) );
		iLenDataRed = 0; 
	
	#if 0	
		memset(szDataMagenta, 0x00, sizeof(szDataMagenta) );
		iLenDataMagenta = 0;

		memset(szDataBlue, 0x00, sizeof(szDataBlue) );
		iLenDataBlue = 0; 
	
		memset(szDataGreen, 0x00, sizeof(szDataGreen) );
		iLenDataGreen = 0; 
	
		memset(szDataYellow, 0x00, sizeof(szDataYellow) );
		iLenDataYellow = 0; 
	#endif

		IsMarking();
	
#endif


	}


#if USE_WIN_OUTOF_AREA_TO_ZERO // 2016.03.31 LCD size

	//if( (WIN_XBgn > myLCDwidth-WIN_LIMIT_X_MARGIN) || WIN_XBgn < 0 ) 
	if( (WIN_XBgn > myVirWidth-WIN_LIMIT_X_MARGIN) || WIN_XBgn < 0 )  // 2017.08.01, : myLCDwidth -> myVirWidth
	{ 
		WIN_XBgn = 0; 
		rewrite = TRUE;
	}

	if( (WIN_YBgn > myLCDheight-WIN_LIMIT_Y_MARGIN) || WIN_YBgn < 0 )
	{ 
		WIN_YBgn = 0;
		rewrite = TRUE;
	}
	
	//if( WIN_XSIZE < 10 ) { WIN_XSIZE = WINDOW_XSIZE; rewrite = TRUE; }
	//if( WIN_YSIZE < 10 ) { WIN_YSIZE = WINDOW_YSIZE; rewrite = TRUE; }
#endif



	if( rewrite ) SetRegistry(); /* Saved Last Windows size */

	if(hKey) { RegCloseKey(hKey); hKey=NULL; }

}




void GetRegistry_USBAddr(void)
{
	/* HKEY_CURRENT_USER\Software\DNW 값을 읽어들인다.	*/
	DWORD dwType, dwData=4;
	HKEY hKey = NULL;
	LONG lRet;
	BOOL rewrite=FALSE;


	/* 키를 오픈한다. */
	if ((lRet = RegOpenKeyEx(HKEY_CURRENT_USER, REG_LOC, 0, 
				KEY_READ | KEY_QUERY_VALUE , &hKey)) == ERROR_SUCCESS)	 
	{	

		if((lRet = RegQueryValueEx(hKey, REG_SAMSUNG_CPU, NULL, &dwType, (const LPBYTE)&CPUtype, &dwData))!=ERROR_SUCCESS )	
		{	
			CPUtype = 1; // SMDKC100
			rewrite=TRUE;
		}

		dwData = sizeof(szDownloadAddress);
		memset(szDownloadAddress, 0x00, dwData );
		if((lRet = RegQueryValueEx(hKey, REG_DNW_USB_STR_ADDR, NULL, &dwType, (const LPBYTE)szDownloadAddress, &dwData))!=ERROR_SUCCESS )	
		{	
			lstrcpy(szDownloadAddress, TEXT("0x20030000") );  // SMDKC100
			rewrite=TRUE;
		}	

		if((lRet = RegQueryValueEx(hKey, REG_DNW_USB_ADDR_DWORD, NULL, &dwType, (const LPBYTE)&downloadAddress, &dwData))!=ERROR_SUCCESS )	
		{	
			downloadAddress = 0x20030000; // SMDKC100
			rewrite=TRUE;
		}



		if( /* 3 */ (IDC_CPU_OTHER-IDC_CPU_SMDK6410) == CPUtype ) // Other
		{

			dwData = sizeof(szDownloadAddress);
			memset(szDownloadAddress, 0x00, dwData );
			if((lRet = RegQueryValueEx(hKey, REG_DNW_USB_STR_O_ADDR, NULL, &dwType, (const LPBYTE)szDownloadAddress, &dwData))!=ERROR_SUCCESS )	
			{	
				lstrcpy(szDownloadAddress, TEXT("0x20030000") );  // SMDKC100
				rewrite=TRUE;
			}	

			if((lRet = RegQueryValueEx(hKey, REG_DNW_USB_ADDR_OTHER, NULL, &dwType, (const LPBYTE)&downloadAddress, &dwData))!=ERROR_SUCCESS )	
			{	
				downloadAddress = 0x20030000;
				rewrite=TRUE;
			}

			//EB_Printf("CPUType ==2222 \n");
		}
		

		RegCloseKey(hKey);	

	}


#if 1 // 2016.04.03
	if( rewrite ) SetRegistry(); /* Saved Last Windows size */
#else
	if( rewrite ) ThreadSetRegistry(NULL);
#endif

	if(hKey) { RegCloseKey(hKey); hKey=NULL; }

}




// ---------------------------------
// --- [F3] key : COM Port Lists
// ---------------------------------
void GetComPortRegistry(void)
{
	DWORD dwType=REG_SZ, dwData=50;
	HKEY hKey = NULL;
	LONG lRet;
	char lpData[MAX_PATH] = {0,};
	TCHAR RegKey[MAX_PATH] = {0, };
	TCHAR RegTmp[MAX_PATH] = {0, };
	int   idx=0;
	int   mExistCount=0;
#if MODIFIED_JULIAN_DATE // 2017.08.31
	SYSTEMTIME LocTime; // NOTE PC 의 시간 정보 취득 Display에 출력한다 ---
	TCHAR szLocTime[MAX_PATH] = {0,};
	//mjd_timestamp mJDTime; 
	//TCHAR		  mJDtemp[20];
#endif



#if MODIFIED_JULIAN_DATE // 2017.08.31
	if( 3==localTime ) // 2017.08.31, case of modified Julian Date
	{
		GetLocalTime( &LocTime );
		wsprintf(&szLocTime[0], TEXT("%04d/%02d/%02d;%02d:%02d:%02d.%03d"), 
				LocTime.wYear, LocTime.wMonth, LocTime.wDay, LocTime.wHour, LocTime.wMinute, LocTime.wSecond, LocTime.wMilliseconds);

		// memcpy( &mJDTime, &LocTime, sizeof(SYSTEMTIME) );
		// f2a( Convert2MJD(mJDTime), mJDtemp, 1000000);

		EB_Printf(TEXT("[dnw] ------------------------------------------------------------------\r\n"));
		EB_Printf(TEXT("[dnw] -> [%s] The available COM ports (%s): \r\n"), szLocTime, APP_VER );
	}
	else
#endif
	{
		EB_Printf(TEXT("[dnw] ------------------------------------ \r\n"));
		EB_Printf(TEXT("[dnw] The available COM ports (%s): \r\n"), APP_VER );
	}



	/* COM Port Reg 키를 오픈한다. */
	if ((lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
							REG_COM_PORT_AVAIL, 
							0, 
							KEY_QUERY_VALUE|KEY_READ,  // KEY_ALL_ACCESS
							&hKey))== ERROR_SUCCESS )	 
	{


		for(idx=0; idx<=256; idx++)
		{
			memset( RegKey, 0x00, sizeof(RegKey) );
			memset( RegTmp, 0x00, sizeof(RegTmp) );
			memset( lpData, 0x00, sizeof(lpData) ); 


			lstrcpy( RegKey, REG_COM_PORT_KEY);

			if( 0 == idx )
				lstrcat( RegKey, _T("0") );
			else
			{
				/// lstrcat( RegKey, _itot(idx,RegKeyTmp,10) );
				wsprintf(RegTmp, _T("%d"), idx );
				lstrcat( RegKey, RegTmp );
			}
			/// EB_Printf(TEXT("[[[[ %s ]]]]  \r\n"), RegKey );

		
			dwType = REG_SZ;
			dwData = sizeof(lpData)*sizeof(TCHAR);
			if( (lRet = RegQueryValueEx(hKey, RegKey, NULL, &dwType, (LPBYTE)lpData, &dwData))==ERROR_SUCCESS )	
			{	
				if( NULL != lpData )
				{
					EB_Printf(TEXT("      [ %2d ] : %s \r\n"), mExistCount+1, lpData );
					mExistCount++;
				}
				/// if( 0==(mExistCount%5) ) EB_Printf(TEXT("\r\n") );
			}	
			else
			{
				/// EB_Printf(TEXT("[2] dwType=%d, &lpData=%s, &dwData=%d	\r\n"), dwType, lpData, dwData );
			}
		}


		if(hKey) { RegCloseKey(hKey); hKey=NULL; }

		if( mExistCount ) EB_Printf(TEXT("      Total %d COM ports are available. \r\n"), mExistCount );
		/// else EB_Printf(TEXT("      The connected COM port is NONE...\r\n") );

	}
	else
	{
		DWORD dwError = GetLastError();
		if( ERROR_SUCCESS != dwError )
			EB_Printf(TEXT("[dnw] COM Port List Error! lRet(0x%X) Err(%u) \r\n"), lRet, dwError );
	}
	if(hKey) { RegCloseKey(hKey); hKey=NULL; }



#if MODIFIED_JULIAN_DATE // 2017.08.31
	if( 3==localTime ) // 2017.08.31, case of modified Julian Date
	{
		EB_Printf(TEXT("[dnw] ------------------------------------------------------------------\r\n"));
	}
	else
#endif
	{
		EB_Printf(TEXT("[dnw] ------------------------------------ \r\n"));
	}

	
}




// get memory info...
int getTotalRAM(void)
{
	int ret = 0;
	DWORD v = GetVersion();
	DWORD major =  (DWORD)(LOBYTE(LOWORD(v)));
	DWORD minor =  (DWORD)(HIBYTE(LOWORD(v)));
	DWORD build;

	if (v < 0x80000000) build = (DWORD)(HIWORD(v));
	else build = 0;

	// because compiler static links the function...
	BOOL (__stdcall*GMSEx)(LPMEMORYSTATUSEX) = 0;

	HINSTANCE hIL = LoadLibrary("kernel32.dll");
	GMSEx = (BOOL(__stdcall*)(LPMEMORYSTATUSEX))GetProcAddress(hIL, "GlobalMemoryStatusEx");

	if(GMSEx)
	{
		MEMORYSTATUSEX m;
		m.dwLength = sizeof(m);
		if(GMSEx(&m))
		{
		ret = (int)(m.ullTotalPhys>>20); // Unit : MB
		}
	}
	else
	{
		MEMORYSTATUS m;
		m.dwLength = sizeof(m);
		GlobalMemoryStatus(&m);
		ret = (int)(m.dwTotalPhys>>20);
	}

	return ret;
}



int getAvailRAM(void)
{
	int ret = 0;
#ifdef WIN32
	DWORD v = GetVersion();
	DWORD major =  (DWORD)(LOBYTE(LOWORD(v)));
	DWORD minor =  (DWORD)(HIBYTE(LOWORD(v)));
	DWORD build;
	if (v < 0x80000000) build = (DWORD)(HIWORD(v));
	else build = 0;

	// because compiler static links the function...
	BOOL (__stdcall*GMSEx)(LPMEMORYSTATUSEX) = 0;

	HINSTANCE hIL = LoadLibrary("kernel32.dll");
	GMSEx = (BOOL(__stdcall*)(LPMEMORYSTATUSEX))GetProcAddress(hIL, "GlobalMemoryStatusEx");

	if(GMSEx)
	{
		MEMORYSTATUSEX m;
		m.dwLength = sizeof(m);
		if(GMSEx(&m))
		{
			ret = (int)(m.ullAvailPhys>>20);
		}
	}
	else
	{
		MEMORYSTATUS m;
		m.dwLength = sizeof(m);
		GlobalMemoryStatus(&m);
		ret = (int)(m.dwAvailPhys>>20);
	}
#endif

	return ret;
}



int getTotalMemory(void)
{
	int ret = 0;
#ifdef WIN32
	DWORD v = GetVersion();
	DWORD major =  (DWORD)(LOBYTE(LOWORD(v)));
	DWORD minor =  (DWORD)(HIBYTE(LOWORD(v)));
	DWORD build;
	if (v < 0x80000000) build = (DWORD)(HIWORD(v));
	else build = 0;

	// because compiler static links the function...
	BOOL (__stdcall*GMSEx)(LPMEMORYSTATUSEX) = 0;

	HINSTANCE hIL = LoadLibrary("kernel32.dll");
	GMSEx = (BOOL(__stdcall*)(LPMEMORYSTATUSEX))GetProcAddress(hIL, "GlobalMemoryStatusEx");

	if(GMSEx)
	{
		MEMORYSTATUSEX m;
		m.dwLength = sizeof(m);
		if(GMSEx(&m))
		{
			ret = (int)(m.ullTotalPhys>>20) + (int)(m.ullTotalVirtual>>20);
		}
	}
	else
	{
		MEMORYSTATUS m;
		m.dwLength = sizeof(m);
		GlobalMemoryStatus(&m);
		ret = (int)(m.dwTotalPhys>>20) + (int)(m.dwTotalVirtual>>20);
	}
#endif

	return ret;
}



int getAvailMemory(void)
{
	int ret = 0;
#ifdef WIN32
	DWORD v = GetVersion();
	DWORD major =  (DWORD)(LOBYTE(LOWORD(v)));
	DWORD minor =  (DWORD)(HIBYTE(LOWORD(v)));
	DWORD build;
	if (v < 0x80000000) build = (DWORD)(HIWORD(v));
	else build = 0;

	// because compiler static links the function...
	BOOL (__stdcall*GMSEx)(LPMEMORYSTATUSEX) = 0;

	HINSTANCE hIL = LoadLibrary("kernel32.dll");
	GMSEx = (BOOL(__stdcall*)(LPMEMORYSTATUSEX))GetProcAddress(hIL, "GlobalMemoryStatusEx");

	if(GMSEx)
	{
		MEMORYSTATUSEX m;
		m.dwLength = sizeof(m);
		if(GMSEx(&m))
		{
			ret = (int)(m.ullAvailPhys>>20) + (int)(m.ullAvailVirtual>>20);
		}
	}
	else
	{
		MEMORYSTATUS m;
		m.dwLength = sizeof(m);
		GlobalMemoryStatus(&m);
		ret = (int)(m.dwAvailPhys>>20) + (int)(m.dwAvailVirtual>>20);
	}
#endif
	return ret;
}


#if 0
BOOL ThreadSetRegistry(void *args)
{
	uintptr_t threadResult;


	threadResult = _beginthread( (void (*)(void *))SetRegistry, THREAD_STACK_SIZE, (void *)NULL );
	if(threadResult!=-1)
	{
		//Sleep(10);
	}
	else
	{
		EB_Printf("[dnw] can not create ThreadSetRegistry thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
		return FALSE;
	}
	return TRUE;
}
#endif


#if 0
// Put name of your process here!!!!
//CONST PWSTR COUNTER_PATH[100] = L"\\Process(a_program_name)\\% Processor Time";
#define COUNTER_PATH  			"\\Process(a_program_name)\\% Processor Time" // % of cpu in use
#define CNTR_DISKAVIL_MB  		"\\Local Disk\\Free Space"



void CPUPercent(void)
{
	PDH_HQUERY hquery;
	PDH_HCOUNTER hcountercpu;
	PDH_STATUS status;
	LPSTR pMessage;
	PDH_FMT_COUNTERVALUE countervalcpu;

	if((status=PdhOpenQuery(NULL, 0, &hquery))!=ERROR_SUCCESS)
	{
		EB_Printf(TEXT("[dnw] PdhOpenQuery %lx\n"), status);
		goto ENDED;
    }

    if((status=PdhAddCounter(hquery,COUNTER_PATH,0, &hcountercpu))!=ERROR_SUCCESS){
		EB_Printf(TEXT("[dnw] PdhAddCounter (cpu) 0x%lx\n"), status);
		goto ENDED;
    }

    /*Start outside the loop as CPU requires difference 
    between two PdhCollectQueryData s*/
    if((status=PdhCollectQueryData(hquery))!=ERROR_SUCCESS){
		EB_Printf(TEXT("[dnw] PdhCollectQueryData 0x%lx\n"), status);
        goto ENDED;
    }

    // while(true)
	{
        if((status=PdhCollectQueryData(hquery))!=ERROR_SUCCESS){
			EB_Printf(TEXT("[dnw] PdhCollectQueryData 0x%lx\n"), status);
            goto ENDED;
        }

        if((status=PdhGetFormattedCounterValue(hcountercpu, PDH_FMT_LONG | PDH_FMT_NOCAP100, 0, &countervalcpu))!=ERROR_SUCCESS){
			EB_Printf(TEXT("[dnw] PdhGetFormattedCounterValue(cpu) 0x%lx\n"), status);
			goto ENDED;
        }

		EB_Printf(TEXT("[dnw] cpu %3d%% \n"), countervalcpu.longValue );

        Sleep(1000);

    }
ENDED:
    ;
   // Close the query and the log file.
   status = PdhCloseQuery(hquery);	
}
#endif


#if 0
#include <winsock2.h>
#include <ws2tcpip.h>

TCHAR 	*GetIPaddress(void)
{
	char 	chName[MAX_PATH] = {0,};
	TCHAR 	sAddress[MAX_PATH] = {0,};
	PHOSTENT 	pHostEntry;
	IN_ADDR 	inAddr;

	if( 0 != gethostname(chName, MAX_PATH) ) return TEXT("aaa");
	else
	{
		if( ( pHostEntry = gethostbyname( chName ) ) == NULL ) return TEXT("bbb");
		else
		{
			memcpy( &inAddr, pHostEntry->h_addr, 4);
			wsprintf( sAddress, TEXT("%s"), inet_ntoa( inAddr ) );
			return sAddress;
		}
	}
}
#endif


__int64 XfGetTime(void)
{
	LARGE_INTEGER liStartCounter, liEndCounter, liFrequency;
	__int64 ms_interval;


    // Start 시점의 진동수.
	QueryPerformanceCounter(&liStartCounter);

    // 고해상도 타이머의 주기를 얻는다.
	QueryPerformanceFrequency(&liFrequency);


	// End 시점의 진동수
	QueryPerformanceCounter(&liEndCounter);

	// 끝 시점의 진동수에서 시작 시점의 진동수를 뺀 후 진동수에서 1000으로 나눈 값을 나눈다.
	// 1초 기준의 주기를 1000 으로 나누었기 때문에 1밀리초 동안 발생하는 진동수로 나눈 셈이다.
	ms_interval = ( (liEndCounter.QuadPart /*- liEndCounter.QuadPart*/) / liFrequency.QuadPart ) * 1000;

	return ms_interval;
} 


int ProcessorType(void)   
{
	int dwDNWrunCount = 0;
	HANDLE   hProcess = NULL;   
	PROCESSENTRY32 pe32 = {0};   
	hProcess = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );   

	pe32.dwSize = sizeof( PROCESSENTRY32 );

	dwDNWrunCount = 0;
	if( Process32First( hProcess, &pe32 ) )
	{   
		do { 
			// EB_Printf(TEXT("[dnw] %40s   Thread:%-10d  ID:%u \r\n"), pe32.szExeFile, pe32.cntThreads, pe32.th32ProcessID);

			if( 0==lstrcmp( TEXT("dnw.exe"), pe32.szExeFile ) || 0==lstrcmp( TEXT("DNW.EXE"), pe32.szExeFile) )
			{
				EB_Printf(TEXT("[dnw] %s  Thread:%-10d  ID:%u \r\n"), pe32.szExeFile, pe32.cntThreads, pe32.th32ProcessID);
				dwDNWrunCount++;
			}

		} while ( Process32Next( hProcess, &pe32 ) );   
	}   

	if(hProcess) { CloseHandle(hProcess); hProcess=NULL; }

	return dwDNWrunCount;   
}  



char *os_name(void)
{

	//OSVERSIONINFOEX infoEx; 	 // infoEx.wProductType == VER_NT_WORKSTATION
	OSVERSIONINFO osInfo;
	static char OSbuf[128] = {0,};

	if(OSbuf[0]) { return OSbuf; }

	memset(&osInfo, 0, sizeof(osInfo));
	osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if(0 == GetVersionEx(&osInfo) )
	{
		EB_Printf(TEXT("[dnw] os_name is invaild.. Err(%u) \r\n"), GetLastError() );
		//exit(-1);
		return NULL;
	}

#if 0
{
	TCHAR szText[2048] = {0,};
		
	wsprintf(szText, TEXT("[dnw]  %d.%d.%d (%s) \r\n"), osInfo.dwMajorVersion, osInfo.dwMinorVersion, osInfo.dwBuildNumber&0xFFFF, osInfo.szCSDVersion );

	MessageBox(NULL, szText, TEXT("About DNW"), MB_OK | MB_ICONINFORMATION );

}
#endif



	if(osInfo.dwMajorVersion <=  4)	   /// { strcpy(OSbuf, "Windows NT"); }
	{
		sprintf(OSbuf, "Windows NT %d.%d.%d", osInfo.dwMajorVersion , osInfo.dwMinorVersion, osInfo.dwBuildNumber&0xFFFF);
	}
	else if(5 == osInfo.dwMajorVersion)
	{

		switch(osInfo.dwMinorVersion)
		{
			case 0: /// 5.0
				strcpy(OSbuf, "Windows 2K");
				break;
			case 1: /// 5.1
				strcpy(OSbuf, "Windows XP"); 
				break;
			case 2: /// 5.1
				strcpy(OSbuf, "Windows 2003"); 
				break;
			default: /// 5.x
				strcpy(OSbuf, "Windows XP*"); 
				break;
		}
	}
	else if(6 == osInfo.dwMajorVersion)
	{

		switch( osInfo.dwMinorVersion )
		{
			case 0: /// 6.0
				strcpy(OSbuf, "Windows Vista"); 
				break;

			case 1: /// 6.1
				strcpy(OSbuf, "Windows7");
				break;

			case 2: /// 6.2
				//if ( osInfo.wProductType != VER_NT_WORKSTATION ) strcpy(OSbuf, "Windows Server 2012");
				//else // if( osInfo.wProductType == VER_NT_WORKSTATION)
				strcpy(OSbuf, "Windows8");
				break;

			case 3: /// 6.3
				//if ( osInfo.wProductType != VER_NT_WORKSTATION ) strcpy(OSbuf, "Windows Server 2012 R2");
				//else 
				strcpy(OSbuf, "Windows8.1");
				break;

			default:
				//sprintf(OSbuf, "Windows(8) %d.%d %s", osInfo.dwMajorVersion , osInfo.dwMinorVersion, osInfo.szCSDVersion);
				sprintf(OSbuf, "Windows8.x");
				break;
		}
	}
	else if(10 == osInfo.dwMajorVersion)
	{
		switch( osInfo.dwMinorVersion )
		{
			case 0: // 10.0
				//if ( osInfo.wProductType != VER_NT_WORKSTATION )  strcpy(OSbuf, "Windows Server 2016"); 
				//else 	
				strcpy(OSbuf, "Windows 10"); 
				break;

			default:
				//sprintf(OSbuf, "Windows(10) %d.%d %s", osInfo.dwMajorVersion , osInfo.dwMinorVersion, osInfo.szCSDVersion);
				sprintf(OSbuf, "Windows 10.x" );
				break;
		}
	}
	else
	{
		sprintf(OSbuf, "Windows(%d)", osInfo.dwMajorVersion );
	}


	sprintf(OSbuf + strlen(OSbuf), " [%d.%d.%d - %s]", osInfo.dwMajorVersion,
									osInfo.dwMinorVersion,
									osInfo.dwBuildNumber&0xFFFF, osInfo.szCSDVersion );
	return OSbuf;

}




//#pragma comment(lib, "user32.lib")

TCHAR Cpu_info[MAX_PATH] = {0,};	
TCHAR Cpu_ID[MAX_PATH] = {0,};	

TCHAR ProductName[MAX_PATH] = {0,};	
TCHAR Manufacture[MAX_PATH] = {0,};	
TCHAR BIOSRelease[MAX_PATH] = {0,};	
TCHAR BIOSVersion[MAX_PATH] = {0,};	
TCHAR BIOSVendor_[MAX_PATH] = {0,};	


TCHAR SWProductName[MAX_PATH] = {0,};	
TCHAR SWEditionID[MAX_PATH] = {0,};	
TCHAR SWReleaseId[MAX_PATH] = {0,};	
TCHAR SWCurrentBuildNumber[MAX_PATH] = {0,};
unsigned __int64 SWInstallDate = 0L;
//unsigned __int64 SWInstallTime = 0L;



/*
REG_BINARY : 어떤 형태의 이진 데이터.

 REG_DWORD : 32 비트 숫자.

 REG_DWORD_LITTLE_ENDIAN : 리틀 엔디안 형식의 32 비트 숫자.  창은 리틀 엔디안 컴퓨터 아키텍처에서 실행되도록 설계되었습니다
 따라서,이 값은 Windows 헤더 파일에있는 REG_DWORD로 정의됩니  다.

 REG_DWORD_BIG_ENDIAN :  빅 엔디안 형식의 32 비트 숫자.  일부 UNIX 시스템은 빅 엔디안 아키텍처를 지원합니다.

 REG_EXPAND_SZ :  환경 변수 (예를 들어, "% 경로 %")로 확장되지 않은 참조를 포함 null로 끝나는 문자열입니다. 
 그것은 당신이 유니 코드 또는 ANSI 함수를 사용 하는지 여부에 따라 유니 코드 또는 ANSI 문자열이됩니다. 환경 변수
참조를 확장하려면 사용ExpandEnvironmentStrings의 기능을.

 REG_LINK :  호출에 의해 생성 된 심볼릭 링크의 대상 경로를 포함하는 null로 끝나는 유니 코드 문자열 RegCreateKeyEx REG_OPTION_CREATE_LINK과 기능을

 REG_MULTI_SZ :  빈 문자열 (\ 0)로 종료 null로 끝나는 문자열의 순서

 REG_NONE :  이 정의 값을 입력합니다.

 REG_QWORD :  64 비트 숫자입니다.

 REG_QWORD_LITTLE_ENDIAN : 리틀 엔디안 형식의 64 비트 숫자입니다. 

 REG_SZ : 
 */


TCHAR *GetProcessorName()
{
	TCHAR num[30] = {0,};
	HKEY hKey = NULL;
	DWORD c_size = sizeof(Cpu_info);  
	SYSTEM_INFO systemInfo;
	DWORD dwType, dwData=4;
	LONG lRet;

	memset( Cpu_info, 0x00, sizeof(Cpu_info) );
	memset( Cpu_ID, 0x00, sizeof(Cpu_ID) );


	/* 키를 오픈한다. */
	// --- 레지스트리를 조사하여 프로세서의 모델명을 얻어냅니다.
	if ((lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Hardware\\Description\\System\\CentralProcessor\\0"), 0, 
				KEY_READ | KEY_QUERY_VALUE, &hKey) ) == ERROR_SUCCESS)	 
	{
		c_size = sizeof(Cpu_info);	
		RegQueryValueEx(hKey, TEXT("ProcessorNameString"), NULL, NULL, (const LPBYTE)Cpu_info, &c_size);  

		c_size = sizeof(Cpu_ID);	
		RegQueryValueEx(hKey, TEXT("Identifier"), NULL, NULL, (const LPBYTE)Cpu_ID, &c_size);  
	}
	RegCloseKey(hKey);


	// -------------
	memset( Manufacture, 0x00, sizeof(Manufacture) );
	memset( ProductName, 0x00, sizeof(ProductName) );	
	if ((lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("Hardware\\Description\\System\\BIOS"), 0, 
				KEY_READ | KEY_QUERY_VALUE, &hKey))== ERROR_SUCCESS)
	{

		c_size = sizeof(Manufacture);	
		RegQueryValueEx(hKey, TEXT("SystemManufacturer"), NULL, NULL, (const LPBYTE)Manufacture, &c_size);  

		c_size = sizeof(ProductName);	
		RegQueryValueEx(hKey, TEXT("SystemProductName"), NULL, NULL, (const LPBYTE)ProductName, &c_size);  

		c_size = sizeof(BIOSRelease);	
		RegQueryValueEx(hKey, TEXT("BIOSReleaseDate"), NULL, NULL, (const LPBYTE)BIOSRelease, &c_size);  

		c_size = sizeof(BIOSVersion);	
		RegQueryValueEx(hKey, TEXT("BIOSVersion"), NULL, NULL, (const LPBYTE)BIOSVersion, &c_size);  

		c_size = sizeof(BIOSVendor_);	
		RegQueryValueEx(hKey, TEXT("BIOSVendor"), NULL, NULL, (const LPBYTE)BIOSVendor_, &c_size);  
	}

	RegCloseKey(hKey);



	if ((lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"), 0, 
					KEY_WOW64_64KEY|KEY_EXECUTE|KEY_READ|KEY_QUERY_VALUE, &hKey))== ERROR_SUCCESS)
	{

		c_size = sizeof(SWProductName);	
		RegQueryValueEx(hKey, TEXT("ProductName"), NULL, NULL, (const LPBYTE)SWProductName, &c_size);  

		c_size = sizeof(SWEditionID); 
		RegQueryValueEx(hKey, TEXT("EditionID"), NULL, NULL, (const LPBYTE)SWEditionID, &c_size);  

		c_size = sizeof(SWReleaseId);	
		RegQueryValueEx(hKey, TEXT("ReleaseId"), NULL, NULL, (const LPBYTE)SWReleaseId, &c_size);  

		c_size = sizeof(SWCurrentBuildNumber);	
		RegQueryValueEx(hKey, TEXT("CurrentBuildNumber"), NULL, NULL, (const LPBYTE)SWCurrentBuildNumber, &c_size);  

		// OS Intallation Date ++++
		dwData = sizeof(REG_DWORD); 
		lRet = RegQueryValueEx(hKey, TEXT("InstallDate"), 0, &dwType, (const LPBYTE)&SWInstallDate, &dwData);
		if( ERROR_SUCCESS != lRet )
		{
			//MessageBox(NULL, TEXT("InstallDate Register Open Error"), TEXT(" +++ Exception Error +++ "), MB_OK);
		}

	#if 0
		dwData = sizeof(REG_QWORD); 
		lRet = RegQueryValueEx(hKey, TEXT("InstallTime"), 0, &dwType, (const LPBYTE)&SWInstallTime, &dwData);
		if( ERROR_SUCCESS != lRet )
		{
			//MessageBox(NULL, TEXT("InstallTime Register Open Error"), TEXT(" +++ Exception Error +++ "), MB_OK);
		}
	#endif
	
	}	  

	RegCloseKey(hKey);


	// GetSystemInfo 함수를 이용해 논리적 코어 개수를 얻어냅니다.
	memset( &systemInfo, 0x00, sizeof(SYSTEM_INFO) );
	GetSystemInfo(&systemInfo);
	//sprintf(num, "Rev:%d * %d", systemInfo.wProcessorLevel, systemInfo.dwNumberOfProcessors);

	if( Is64BitWindows() )
	{
		sprintf(num, " * %d (64bit)", systemInfo.dwNumberOfProcessors);
	}
	else
	{
		sprintf(num, " * %d (32bit)", systemInfo.dwNumberOfProcessors);
	}
	
	lstrcat(Cpu_info, num);
	return Cpu_info;
}



char mss[128] = {0,}; 
char *GetMemoryInfo()
{
	//GlobalMemoryStatusEX 함수를 이용해 물리적 메모리양을 조사합니다.
    MEMORYSTATUSEX ms;

    ms.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&ms);

	memset( mss, 0x00, sizeof(mss) );
    sprintf_s(mss, 128, "%lld MB", ms.ullTotalPhys>>20 );
    return mss;
}


typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
LPFN_ISWOW64PROCESS fnIsWow64Process;

BOOL Is64BitWindows()
{
	BOOL bIsWow64 = FALSE;

	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "IsWow64Process");

	if(fnIsWow64Process != NULL)
	{
		if( !fnIsWow64Process(GetCurrentProcess(), &bIsWow64) )
		{
			// handle error
		}
	}

	return bIsWow64;
}




#if 0
HANDLE  RetrieveProcessHandle(LPCTSTR lpszExeFileName);
__int32 RetrieveCpuUsage(HANDLE hProcess, INT nProcessorCount);
__int32 RetrieveProcessorCount();
DWORD CountSetBits(ULONG_PTR bitMask);


DWORD RetrieveCpuUsage(HANDLE hProcess, INT nProcessorCount)
{
	__int64  nCreationTime = 0;
	__int64  nExitTime = 0;
	__int64  nKernelTimeOld = 0, nKernelTimeNew = 0;
	__int64  nUserTimeOld = 0, nUserTimeNew = 0;

	__int64  nTickCountOld = 0, nTickCountNew = 0;
	DWORD  nCpuUsagePercent = 0;

	nTickCountOld = GetTickCount();
	if (!GetProcessTimes(hProcess, (LPFILETIME)&nCreationTime, (LPFILETIME)&nExitTime, (LPFILETIME)&nKernelTimeOld, (LPFILETIME)&nUserTimeOld))
		return -1;

	Sleep(5000);

	nTickCountNew = GetTickCount();
	if (!GetProcessTimes(hProcess,	(LPFILETIME)&nCreationTime, (LPFILETIME)&nExitTime, (LPFILETIME)&nKernelTimeNew, (LPFILETIME)&nUserTimeNew))
		return -1;

	nCpuUsagePercent = (DWORD)((nKernelTimeNew - nKernelTimeOld + nUserTimeNew - nUserTimeOld) / (100 * (nProcessorCount * (nTickCountNew - nTickCountOld))));

	return nCpuUsagePercent;
}




int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE hProcess = NULL;
	INT nUsage = 0;
	INT nProcessorCount = 0;

	nProcessorCount = RetrieveProcessorCount();

	hProcess = RetrieveProcessHandle(_T("MsMpEng.exe"));
	nUsage = RetrieveCpuUsage(hProcess, nProcessorCount);

	CloseHandle(hProcess);
	return 0;
}

HANDLE RetrieveProcessHandle(LPCTSTR lpszExeFileName)
{
	HANDLE hProcessSnap = NULL;
	HANDLE hProcess = NULL;
	PROCESSENTRY32 pe32 = {0};

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hProcessSnap == INVALID_HANDLE_VALUE)
		goto CLEAR_RESOURCE;

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if(!Process32First(hProcessSnap, &pe32))
		goto CLEAR_RESOURCE;

	while(Process32Next(hProcessSnap, &pe32))
	{
		if (StrCmpI(lpszExeFileName, pe32.szExeFile) == 0) 
		{
			hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
			break;
		}
	};

CLEAR_RESOURCE:

	if (hProcessSnap != NULL)
		CloseHandle(hProcessSnap);
	
	return hProcess;
}

__int32 RetrieveCpuUsage(HANDLE hProcess, INT nProcessorCount)
{
	__int64  nCreationTime = 0;
	__int64  nExitTime = 0;
	__int64  nKernelTimeOld = 0, nKernelTimeNew = 0;
	__int64  nUserTimeOld = 0, nUserTimeNew = 0;

	__int64  nTickCountOld = 0, nTickCountNew = 0;
	__int32  nCpuUsagePercent = 0;

	nTickCountOld = GetTickCount64();
	if (!GetProcessTimes(hProcess, (LPFILETIME)&nCreationTime, (LPFILETIME)&nExitTime, (LPFILETIME)&nKernelTimeOld, (LPFILETIME)&nUserTimeOld))
		return -1;

	Sleep(5000);

	nTickCountNew = GetTickCount64();
	if (!GetProcessTimes(hProcess,  (LPFILETIME)&nCreationTime, (LPFILETIME)&nExitTime, (LPFILETIME)&nKernelTimeNew, (LPFILETIME)&nUserTimeNew))
		return -1;

	nCpuUsagePercent = (__int32)((nKernelTimeNew - nKernelTimeOld + nUserTimeNew - nUserTimeOld) / (100 * (nProcessorCount * (nTickCountNew - nTickCountOld))));

	return nCpuUsagePercent;
}

__int32 RetrieveProcessorCount()
{
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION lpSysProcInfo = NULL;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptrSysProcInfo = NULL;

	DWORD dwReturnLength = 0;
	DWORD dwByteOffSet = 0;

	INT   nProcessCoreCount = 0;
	INT   nLogicalProcessorCount = 0;

	if (!GetLogicalProcessorInformation(lpSysProcInfo, &dwReturnLength))
	{
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) 
		{
			if (lpSysProcInfo != NULL)
				LocalFree((HLOCAL)lpSysProcInfo);

			lpSysProcInfo = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)LocalAlloc(LPTR, dwReturnLength);

			if (!GetLogicalProcessorInformation(lpSysProcInfo, &dwReturnLength)) {
				LocalFree((HLOCAL)lpSysProcInfo);
				return 1;
			}
		}

		ptrSysProcInfo = lpSysProcInfo;
		while (dwByteOffSet + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= dwReturnLength) 
		{
			switch (ptrSysProcInfo->Relationship) 
			{
			case RelationProcessorCore:
				nProcessCoreCount++;
				nLogicalProcessorCount += CountSetBits(ptrSysProcInfo->ProcessorMask);
				break;
			}
			dwByteOffSet += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
			++ptrSysProcInfo;
		}
	}
	LocalFree((HLOCAL)lpSysProcInfo);

	return nLogicalProcessorCount;
}

DWORD CountSetBits(ULONG_PTR bitMask)
{
    DWORD LSHIFT = sizeof(ULONG_PTR)*8 - 1;
    DWORD bitSetCount = 0;
    ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;    
    DWORD i;
    
    for (i = 0; i <= LSHIFT; ++i)
    {
        bitSetCount += ((bitMask & bitTest)?1:0);
        bitTest/=2;
    }

    return bitSetCount;
}
#endif




