#define STRICT
#define WIN32_LEAN_AND_MEAN

#include "resource.h"

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>
#include <string.h>
#include <process.h>  /* For _beginthread() */
#include <stdlib.h>
//#include <conio.h>
//#include <stdio.h>
//#include <assert.h>


#include "def.h"
#include "dnw.h"
#include "engine.h"
#include "fileopen.h"
#include "d_box.h"

#include "rwbulk.h"
#include <stdio.h> 
#include <memory.h> 


/* 
   To compile rwbulk.c, the followings should be added.
   1. linker option: usbd.lib,setupapi.lib 
   2. additional library path: C:\NTDDK\libfre\i386
   3. additional include directories: C:\NTDDK\inc
   4. copy bulkusb.h,guid829.h from bulkusb directory.
*/
#define MAX_FILE_NUM	32
#define MAX_PATH        260

#define OUTFILENAME_LENGTH 	255  /// 2012.02.21

HANDLE hWrite = NULL;
HANDLE hRead = NULL;

static volatile char *USBtxBuf = NULL;
static volatile DWORD iUSBTxBuf = 0;
static DWORD USBTxBufSize = 0;
static DWORD fileBufSize = 0;

BOOL bUSBTxFile = FALSE; /// 2010.05.25 added.

		

DWORD downloadAddress = 0x20030000; // 2018.07.18
TCHAR szDownloadAddress[16] = TEXT("0x20030000"); // 2018.07.18 { 0, };

TCHAR szDownloadString[MAXHISTORY][FILENAMELEN] = { 0, };

TCHAR szUserBaudRate[16] = { 0, };

#if USE_HEX2BIN_CONVERT // 2016.03.04
TCHAR szH2BFullInputFile[MAX_PATH] = {0,};
TCHAR szH2BTitleInpFile[MAX_PATH] = {0,};
TCHAR szH2BInpFile[MAX_PATH] = {0,};

TCHAR szH2BFullOutputFile[MAX_PATH] = {0,};
TCHAR szH2BOutFile[MAX_PATH] = {0,};

TCHAR szH2BFullCrcOutFile[MAX_PATH] = {0,};
TCHAR szH2BCRCFile[MAX_PATH] = {0,};

DWORD dwSizeUnit = 1024; // Byte(1) or KByte (1024)
DWORD dwHex2BinMaxSize = 640; // 640 KByte
TCHAR szHex2BinMaxSize[16] = TEXT("640"); // { 0, };

int  dwPadByte = 0xff; // 0xff
TCHAR szPadByte[16] = TEXT("FF"); // 0xff
#endif



#if USE_FLOAT2HEX_VALUE // 2018.07.18
TCHAR szFlt2Hex_In1[MAX_PATH] = TEXT("0.01234");
TCHAR szFlt2Hex_In2[MAX_PATH] = TEXT("10.5273"); // 2nd input
TCHAR szFlt2Hex_In3[MAX_PATH] = TEXT("0.01"); // step

TCHAR szDbl2Hex_In1[MAX_PATH] = {0,};
TCHAR szDbl2Hex_In2[MAX_PATH] = {0,}; // 2nd input
TCHAR szDbl2Hex_In3[MAX_PATH] = {0,}; // step


TCHAR szFlt2Hex_Out[MAX_PATH] = { 0, };

int isFlt2HexStop = 0;

#endif



#if HEXA_MODE_SUPPORT	/* 2012.07.03 Hexa mode display */
DWORD ColumnNumber = 0; // 2017.04.21 -> 16;
TCHAR szColumnNumber[16] = TEXT("0"); // 2017.04.21 -> TEXT("16"); // 16 column
#endif


TCHAR szSerialTxCharData[TX_DATA_LENGTH] = { 0, }; // 2016.03.23
TCHAR szSerialTxHexaData[TX_DATA_LENGTH] = { 0, }; // 2016.03.23
TCHAR szSerialTxFileData[TX_DATA_LENGTH] = { 0, }; // 2016.03.28
TCHAR szSerialTxFileTitl[TX_DATA_LENGTH] = { 0, }; // 2016.03.28
TCHAR szSerialTxDispName[TX_DATA_LENGTH] = { 0, }; // 2016.03.28

TCHAR szSerialTxHexaXOR[TX_DATA_LENGTH] = { 0, }; // 2018.08.21



TCHAR szQuickDownloadString[MAXHISTORY][FILENAMELEN] = { 0, };
TCHAR szQuickFileName[MAXHISTORY][FILENAMELEN] = { 0, };
TCHAR szUMONDownloadString[MAXHISTORY][FILENAMELEN] = { 0, };
TCHAR szUMONSFileName[MAXHISTORY][FILENAMELEN] = { 0, };


int transmitMenuNumber = 0;
int UbootMenuNumber = 0;
int UbootMenuNumber2 = 0;
int dnUSBcount = 1;  /* 20 for AutoSend */
extern int autoSendKey;
extern int RetryUSB;


#define USB_DELAY_TIME 			5 // msec



void UsbTxFile(void *args);
extern UINT hex2int(TCHAR *str);

extern void BeepSound(int beeptype);


void MenuUsbStatus(HWND hwnd)
{
    dumpUsbConfig();
}

void AddTransmitMenu(HWND hWnd, char * str, unsigned long address)
{
	HMENU MenuBar;
	HMENU SubMenuBar;
	TCHAR tempstr[FILENAMELEN] = {0,};
	TCHAR menustr[FILENAMELEN] = {0,};
	int same = -1;
	int i;


#if 0 // 2015.11.30
	for ( i = 0; i < FILENAMELEN; i ++ )
	{
		tempstr[i] = menustr[i] = '\0';
	}
#else
	memset( tempstr, 0x00, FILENAMELEN*sizeof(TCHAR) );
	memset( menustr, 0x00, FILENAMELEN*sizeof(TCHAR) );
#endif


	strncpy(menustr,str,strlen(str));
	wsprintf(tempstr, ",0x%x", address);
	strcat(menustr, tempstr);

	for ( i = 0; i < transmitMenuNumber; i++ )
	{
		if ( _stricmp(menustr, szDownloadString[i]) == 0 )
		{
			same = i;
			break;
		}
	}

	MenuBar = GetSubMenu(GetMenu(hWnd), 1);
	SubMenuBar = GetSubMenu(MenuBar, 0);

	for ( i = 0; i < transmitMenuNumber; i++ )
	{
		DeleteMenu(SubMenuBar, HISTORYMENUID+i, MF_BYCOMMAND);
	}

	for ( i = ((same == -1) ? transmitMenuNumber:same); i > 0; i-- )
	{
		if ( i < (MAXHISTORY/3) )
			strcpy(szDownloadString[i],szDownloadString[i-1]);
	}
	strcpy(szDownloadString[0],menustr);
	if ( transmitMenuNumber < (MAXHISTORY/3) )
	{
		if ( same == -1 ) transmitMenuNumber++;
	}
	for ( i = 0; i < transmitMenuNumber; i++ )
	{
		AppendMenu(SubMenuBar, MF_STRING, HISTORYMENUID + i, szDownloadString[i]);
	}

	WriteUserConfig();
}

void AddUbootMenu(HWND hWnd, char * str)
{
	HMENU MenuBar;
	HMENU SubMenuBar;
	TCHAR tempstr[FILENAMELEN] = {0,};
	TCHAR menustr[FILENAMELEN] = {0,};
	int same = -1;
	int i;

	for ( i = 0; i < FILENAMELEN; i ++ )
	{
		tempstr[i] = menustr[i] = '\0';
	}

	strcpy(menustr,str);

	for ( i = (MAXHISTORY/3); i < (MAXHISTORY/3)+UbootMenuNumber; i++ )
	{
		if ( _stricmp(menustr, szDownloadString[i]) == 0 )
		{
			same = i;
			break;
		}
	}

	MenuBar = GetSubMenu(GetMenu(hWnd), 1);
	SubMenuBar = GetSubMenu(MenuBar, 1);

	for ( i = (MAXHISTORY/3); i < (MAXHISTORY/3)+UbootMenuNumber; i++ )
	{
		DeleteMenu(SubMenuBar, HISTORYMENUID+i, MF_BYCOMMAND);
	}

	for ( i = ((same == -1) ? UbootMenuNumber:same-(MAXHISTORY/3))+(MAXHISTORY/3); i > (MAXHISTORY/3); i-- )
	{
		if ( i < MAXHISTORY )
			strcpy(szDownloadString[i],szDownloadString[i-1]);
	}
	strcpy(szDownloadString[(MAXHISTORY/3)],menustr);
	if ( UbootMenuNumber < (MAXHISTORY/3) )
	{
		if ( same == -1 ) UbootMenuNumber++;
	}
	for ( i = 0; i < UbootMenuNumber; i++ )
	{
		AppendMenu(SubMenuBar, MF_STRING, HISTORYMENUID+(MAXHISTORY/3)+i, szDownloadString[i+(MAXHISTORY/3)]);
	}

	WriteUserConfig();
}

void AddUbootMenu2(HWND hWnd, char * str)
{
	HMENU MenuBar;
	HMENU SubMenuBar;
	TCHAR tempstr[FILENAMELEN] = {0,};
	TCHAR menustr[FILENAMELEN] = {0,};
	int same = -1;
	int i;

	for ( i = 0; i < FILENAMELEN; i ++ )
	{
		tempstr[i] = menustr[i] = '\0';
	}

	strncpy(menustr,str,strlen(str));

	for ( i = (MAXHISTORY*2/3); i < (MAXHISTORY*2/3)+UbootMenuNumber2; i++ )
	{
		if ( _stricmp(menustr, szDownloadString[i]) == 0 )
		{
			same = i;
			break;
		}
	}

	MenuBar = GetSubMenu(GetMenu(hWnd), 1);
	SubMenuBar = GetSubMenu(MenuBar, 1);

	for ( i = (MAXHISTORY*2/3); i < (MAXHISTORY*2/3)+UbootMenuNumber2; i++ )
	{
		DeleteMenu(SubMenuBar, HISTORYMENUID+i, MF_BYCOMMAND);
	}

	for ( i = ((same == -1) ? UbootMenuNumber2:same-(MAXHISTORY*2/3))+(MAXHISTORY*2/3); i > (MAXHISTORY*2/3); i-- )
	{
		if ( i < MAXHISTORY )
			strcpy(szDownloadString[i],szDownloadString[i-1]);
	}
	strcpy(szDownloadString[(MAXHISTORY*2/3)],menustr);
	if ( UbootMenuNumber2 < (MAXHISTORY*1/3) )
	{
		if ( same == -1 ) UbootMenuNumber2++;
	}
	for ( i = 0; i < UbootMenuNumber2; i++ )
	{
		AppendMenu(SubMenuBar, MF_STRING, HISTORYMENUID+(MAXHISTORY*2/3)+i, szDownloadString[i+(MAXHISTORY*2/3)]);
	}

	WriteUserConfig();
}


void MenuUsbTransmitHistory(HWND hwnd, int history, BYTE funcKey)
{
	ULONG i;
	HANDLE hFile = NULL;
	ULONG fileSize;
	USHORT cs=0;
	//unsigned long threadResult;
	uintptr_t threadResult;
	TCHAR * tocken = NULL;
	int comma;
	DWORD localdownloadAddress;
	int cnt=0;
	BOOL rdRet=FALSE;
	

	if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } 

	if( autoSendKey ) // TRUE == ~~
	{ 
		switch( RetryUSB )
		{
			case 1: /// IDC_RETRY_USB_20_TIMES
				dnUSBcount = 20; 
				break;
			case 2: /// IDC_RETRY_USB_30_TIMES
				dnUSBcount = 30; 
				break;
			case 3: /// IDC_RETRY_USB_40_TIMES
				dnUSBcount = 40; 
				break;
			case 0: /// IDC_RETRY_USB_DEFAULT
			default:
				dnUSBcount = 1; 
				break;
		}
	} /// 2012.03.06
	else { dnUSBcount = 1; } /// 2012.03.06


	cnt = 0;
	do {
		Sleep(USB_DELAY_TIME);
		hWrite = open_file( outPipe, cnt, funcKey );
		if( cnt++ > USB_OPEN_RETRY*dnUSBcount ) break;
	} while( (INVALID_HANDLE_VALUE==hWrite) || (NULL==hWrite) );

	if( (INVALID_HANDLE_VALUE==hWrite) || (NULL==hWrite) )
	{
		MessageBox(hwnd,TEXT("Can not open USB device.\n"),TEXT("TX History Error"), MB_OK | MB_ICONINFORMATION );
		return;
	}
    
//	tocken = strtok(szDownloadString[history],",");
	for ( i = 0; i < FILENAMELEN; i++ )
	{
		if ( szDownloadString[history][i] == ',' )
		{
			comma = i;
			tocken = &szDownloadString[history][i+1];
			break;
		}
	}


	memcpy(szFileName, szDownloadString[history], i);
	szFileName[i] = '\0';
//	tocken = strtok(NULL,",");
//	memcpy(szTitleName, szEBOOTTitleName, FILENAMELEN);
	localdownloadAddress = hex2int(tocken);
    
//    if(result==0) //file open fail
//    {
//	CloseHandle(hWrite);
//	return;
//    }

	hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
	{
		EB_Printf(TEXT("[dnw] can not open file in USBTx. (file:%s, L:%d, Error=%u) \n"),szFileName, __LINE__ , GetLastError() );
		if(hWrite) { CloseHandle(hWrite); hWrite=NULL; } 
		return;
	}

#if 1 // 2016.10.11, POPUP_FileName_Only
	{
		extern TCHAR szFileNameOnPopUP[FILENAMELEN];
		TCHAR szFnLen[MAX_PATH] = {0,};
		int iLen = 0, jdx=0;
		__int64  iFnSize=0L;
		HANDLE hPopUpFileNameOnly=NULL;
		
		iLen = strlen(szFileName);
		if( iLen > MAX_PATH )
		{
			EB_Printf(TEXT("\r\n[dnw] Input file length is over! length(%d) \r\n"), iLen );
			BeepSound(4);
		}
	
		for(jdx=iLen; iLen>0; jdx--) { if( szFileName[jdx] == '\\' ) break; }
	
		lstrcpy( szFileNameOnPopUP, &szFileName[jdx+1] ); // input filename

		// -----------------------------------------------------------------
		// 3. File Read and size
		// -----------------------------------------------------------------
		hPopUpFileNameOnly = CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hPopUpFileNameOnly || NULL==hPopUpFileNameOnly )
		{
			EB_Printf(TEXT("[dnw] can not open file[%s]. Err(%u)\n"), szFileName, GetLastError() );
			return;
		}
		iFnSize = GetFileExSize(hPopUpFileNameOnly);
		if(hPopUpFileNameOnly)  { CloseHandle(hPopUpFileNameOnly);  hPopUpFileNameOnly=NULL; }
	
		//wsprintf(szFnLen, TEXT(" (%I64d Byte)"), iFnSize);
		//lstrcat( szFileNameOnPopUP, szFnLen );

	}
#endif

    
	AddTransmitMenu(hwnd, szFileName, localdownloadAddress);

	fileSize=GetFileSize(hFile,NULL);

	if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
	USBtxBuf=(char *)malloc(fileSize+10); 
	if(NULL == USBtxBuf)
	{
		EB_Printf(TEXT("[dnw] can not allocate USB Tx Buffer memory. (F:%s, L:%d, Error=%u) \n"), __FUNCTION__, __LINE__ , GetLastError());
		if(hFile) CloseHandle(hFile); 
		if(hWrite) { CloseHandle(hWrite); hWrite=NULL; }; 
		return;
	}

	rdRet = ReadFile(hFile,(void *)(USBtxBuf+8),fileSize,&USBTxBufSize,NULL);
	if( (USBTxBufSize!=fileSize) || !rdRet )
	{
		if(!rdRet)
		{
			EB_Printf(TEXT("[dnw] Tx Buffer Reading error (error code=%d, L:%d, Error=%u) \n"),rdRet, __LINE__ , GetLastError());
		}
		else
		{
			EB_Printf(TEXT("[dnw] Tx Buffer size and file size not same (TxBuffer size:%d, size:%d, L:%d,Error=%u) \n"), USBTxBufSize, fileSize, __LINE__ , GetLastError());
		}

		if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
		if(hFile) { CloseHandle(hFile); hFile=NULL; } 
		if(hWrite) { CloseHandle(hWrite); hWrite=NULL; } 
		return;
	}

	*((DWORD *)USBtxBuf+0)=localdownloadAddress;
	*((DWORD *)USBtxBuf+1)=fileSize+10;   //attach filesize(n+6+4) 
	for(i=8;i<(fileSize+8);i++)
		cs+=(BYTE)(USBtxBuf[i]);

	*((WORD *)(USBtxBuf+8+fileSize))=cs;   //attach checksum 

	USBTxBufSize+=10;
	iUSBTxBuf=0;

	if(hFile) { CloseHandle(hFile); hFile=NULL; }
    
	threadResult=_beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

	if(threadResult!=-1)
	{
		Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  /// 2012.02.20, update.....	2011.12.05
		
		//Create the download progress dialogbox.
		CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
		//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
		//isn't needed because the dialog box already has WS_VISIBLE attribute.
	}
	else
	{
		EB_Printf("[dnw] can not create USB TxFile begin thread. (result=%d, L:%d, Error=%u)! \n", threadResult, __LINE__ , GetLastError());
		if(hWrite) { CloseHandle(hWrite); hWrite=NULL; }
		if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
		return; /// added.  2012.02.21
	}

    //The dialog box will be closed at the end of TxFile().
    //free(USBtxBuf) & CloseHandle(hWrite) will be done in TxFile()
}

void MenuUsbTransmit(HWND hwnd, BYTE funcKey)
{
	ULONG i;
	HANDLE hFile = NULL;
	ULONG fileSize;
	USHORT cs=0;
	BOOL result;
	uintptr_t threadResult;
	int cnt=0;
	BOOL rdRet=FALSE;

	if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } 

	if( autoSendKey ) // TRUE == ~~~~
	{ 
		switch( RetryUSB )
		{
			case 1: /// IDC_RETRY_USB_20_TIMES
				dnUSBcount = 20; 
				break;
			case 2: /// IDC_RETRY_USB_30_TIMES
				dnUSBcount = 30; 
				break;
			case 3: /// IDC_RETRY_USB_40_TIMES
				dnUSBcount = 40; 
				break;
			case 0: /// IDC_RETRY_USB_DEFAULT
			default:
				dnUSBcount = 1; 
				break;
		}
	} /// 2012.03.06
	else { dnUSBcount = 1; } /// 2012.03.06

	cnt = 0;
	do {
		Sleep(USB_DELAY_TIME);
		hWrite = open_file( outPipe, cnt, funcKey );
		if( cnt++ > USB_OPEN_RETRY*dnUSBcount ) break;
	} while( (INVALID_HANDLE_VALUE==hWrite) || (NULL==hWrite) );

	if( (INVALID_HANDLE_VALUE==hWrite) || (NULL==hWrite) )
	{
		MessageBox(hwnd,TEXT("Can not open USB device.\n"),TEXT("Transmit Error"), MB_OK | MB_ICONINFORMATION );
		return;
	}

	result=PopFileOpenDlg(hwnd,szEBOOTFileName,szEBOOTTitleName);
	memcpy(szFileName, szEBOOTFileName, FILENAMELEN);
	memcpy(szTitleName, szEBOOTTitleName, FILENAMELEN);

	EB_Printf(TEXT("[dnw] RAM File; [%s] \n"), szEBOOTFileName );
    
	if( 0 == result ) //file open fail
	{
		EB_Printf(TEXT("[dnw] can not open PopFileOpenDlg. (file:%s, L:%d, Error=%u) \n"),szFileName, __LINE__ , GetLastError());
		if(hWrite) { CloseHandle(hWrite); hWrite=NULL; } 
		return;
	}


#if 1 // 2016.10.11, POPUP_FileName_Only
	{
		extern TCHAR szFileNameOnPopUP[FILENAMELEN];
		TCHAR szFnLen[MAX_PATH] = {0,};
		int iLen = 0, jdx=0;
		__int64  iFnSize=0L;
		HANDLE hPopUpFileNameOnly=NULL;
		
		iLen = strlen(szFileName);
		if( iLen > MAX_PATH )
		{
			EB_Printf(TEXT("\r\n[dnw] Input file length is over! length(%d) \r\n"), iLen );
			BeepSound(4);
		}
	
		for(jdx=iLen; iLen>0; jdx--) { if( szFileName[jdx] == '\\' ) break; }
	
		lstrcpy( szFileNameOnPopUP, &szFileName[jdx+1] ); // input filename

		// -----------------------------------------------------------------
		// 3. File Read and size
		// -----------------------------------------------------------------
		hPopUpFileNameOnly = CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hPopUpFileNameOnly || NULL==hPopUpFileNameOnly )
		{
			EB_Printf(TEXT("[dnw] can not open file[%s]. Err(%u)\n"), szFileName, GetLastError() );
			return;
		}
		iFnSize = GetFileExSize(hPopUpFileNameOnly);
		if(hPopUpFileNameOnly)  { CloseHandle(hPopUpFileNameOnly);  hPopUpFileNameOnly=NULL; }
	
		//wsprintf(szFnLen, TEXT(" (%I64d Byte)"), iFnSize);
		//lstrcat( szFileNameOnPopUP, szFnLen );

	}
#endif


	hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if( INVALID_HANDLE_VALUE == hFile  || NULL==hFile )
	{
		EB_Printf(TEXT("[dnw] can not open SHARE_READ. (file:%s, L:%d, Error=%u) \n"),szFileName , __LINE__ , GetLastError());
		if(hWrite) { CloseHandle(hWrite); hWrite=NULL; } 
		return;
	}

	AddTransmitMenu(hwnd, szFileName, downloadAddress);

	fileSize=GetFileSize(hFile,NULL);

	if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
	USBtxBuf=(char *)malloc(fileSize+10); 
	if(NULL==USBtxBuf)
	{
		EB_Printf(TEXT("[dnw] can not allocate USB Tx Buffer memory. (F:%s, L:%d, Error=%u) \n"), __FUNCTION__, __LINE__ , GetLastError());
		if(hFile) { CloseHandle(hFile);  hFile=NULL; }
		if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
		return;
	}

	rdRet = ReadFile(hFile,(void *)(USBtxBuf+8),fileSize,&USBTxBufSize,NULL);
	if( (USBTxBufSize!=fileSize) || !rdRet )
	{
		if(!rdRet)
		{
			EB_Printf(TEXT("[dnw] find error of ReadFile. (error code=%d, L:%d, Error=%u)\n"),rdRet , __LINE__ , GetLastError());
		}
		else
		{
			EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Error=%u) \n"),fileSize, __LINE__ , GetLastError());
		}
		
		if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
		if(hFile) { CloseHandle(hFile); hFile=NULL; } 
		if(hWrite) { CloseHandle(hWrite); hWrite=NULL; } 
		return;
	}

	*((DWORD *)USBtxBuf+0)=downloadAddress;
	*((DWORD *)USBtxBuf+1)=fileSize+10;   //attach filesize(n+6+4) 
	for(i=8;i<(fileSize+8);i++)
		cs+=(BYTE)(USBtxBuf[i]);

	*((WORD *)(USBtxBuf+8+fileSize))=cs;   //attach checksum 

	USBTxBufSize+=10;
	iUSBTxBuf=0;

	if(hFile) CloseHandle(hFile);

	threadResult=_beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

	if(threadResult!=-1)
	{
		Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  /// 2012.02.20, update.....	2011.12.05
		
		//Create the download progress dialogbox.
		CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
		//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
    	//isn't needed because the dialog box already has WS_VISIBLE attribute.
	}
	else
	{
		EB_Printf("[dnw] can not create progress thread! (error code=%d, L:%d, Error=%u) \n", threadResult, __LINE__ , GetLastError());
		if(hWrite) { CloseHandle(hWrite); hWrite=NULL; }
		if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
		return; /// added 2012.02.21
	}

    //The dialog box will be closed at the end of TxFile().
    //free(USBtxBuf) & CloseHandle(hWrite) will be done in TxFile()
}

void MenuUBOOT(HWND hwnd)
{
	ULONG i, j;
	HANDLE hFile = NULL;
	HANDLE hOutFile = NULL;
	HANDLE hBinFile[MAX_FILE_NUM] = { NULL, };
	char * szBinFileName[MAX_FILE_NUM] = { 0, };
	char * szBinFileFullName[MAX_FILE_NUM] = { 0, };
	char * szOutFileName = NULL;

	ULONG fileSize;
	BOOL result = FALSE;
	uintptr_t threadResult;
	TCHAR	szTempFileName[3] = {0,};
	ULONG nFileNumber;
	unsigned int nCheckSum = 0;
	volatile char * tmpTxBuf = NULL;
	char *lstFileBuf = NULL;
	DWORD lstFileBufSize;
	TCHAR szDirName[FILENAMELEN] = {0,};
	int cnt=0, idel=0;
	BOOL rdRet=FALSE;
	DWORD dwState; /// added 2011.11.29

	if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } 

	if( autoSendKey ) // TRUE == ~~~~
	{ 
		switch( RetryUSB )
		{
			case 1: /// IDC_RETRY_USB_20_TIMES
				dnUSBcount = 20; 
				break;
			case 2: /// IDC_RETRY_USB_30_TIMES
				dnUSBcount = 30; 
				break;
			case 3: /// IDC_RETRY_USB_40_TIMES
				dnUSBcount = 40; 
				break;
			case 0: /// IDC_RETRY_USB_DEFAULT
			default:
				dnUSBcount = 1; 
				break;
		}
	} /// 2012.03.06
	else { dnUSBcount = 1; } /// 2012.03.06

	cnt = 0;
	do {
		Sleep(USB_DELAY_TIME);
		hWrite = open_file( outPipe, cnt, UBOOT_OTHER );
		if( cnt++ > USB_OPEN_RETRY*dnUSBcount ) break;
	} while( (INVALID_HANDLE_VALUE==hWrite) || (NULL==hWrite) );

	if( (INVALID_HANDLE_VALUE==hWrite) || (NULL==hWrite) )
	{
		MessageBox(hwnd,TEXT("Can't open USB device.\n"),TEXT("UBOOT Error"), MB_OK | MB_ICONINFORMATION );
		return;
	}

	result=PopFileOpenDlg(hwnd,szUBOOTFileName,szUBOOTTitleName);
	memcpy(szFileName, szUBOOTFileName, FILENAMELEN);
	memcpy(szTitleName, szUBOOTTitleName, FILENAMELEN);

	strncpy ( szDirName, szFileName, strlen(szFileName) - strlen(szTitleName) );
	szDirName[strlen(szFileName) - strlen(szTitleName)] = 0;
    
	if( 0 == result ) //file open fail
	{
		if(hWrite) { CloseHandle(hWrite); hWrite=NULL; }
		return;
	}



#if 1 // 2016.10.11, POPUP_FileName_Only
	{
		extern TCHAR szFileNameOnPopUP[FILENAMELEN];
		TCHAR szFnLen[MAX_PATH] = {0,};
		int iLen = 0, jdx=0;
		__int64  iFnSize=0L;
		HANDLE hPopUpFileNameOnly=NULL;
		
		iLen = strlen(szFileName);
		if( iLen > MAX_PATH )
		{
			EB_Printf(TEXT("\r\n[dnw] Input file length is over! length(%d) \r\n"), iLen );
			BeepSound(4);
		}
	
		for(jdx=iLen; iLen>0; jdx--) { if( szFileName[jdx] == '\\' ) break; }
	
		lstrcpy( szFileNameOnPopUP, &szFileName[jdx+1] ); // input filename

		// -----------------------------------------------------------------
		// 3. File Read and size
		// -----------------------------------------------------------------
		hPopUpFileNameOnly = CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hPopUpFileNameOnly || NULL==hPopUpFileNameOnly )
		{
			EB_Printf(TEXT("[dnw] can not open file[%s]. Err(%u)\n"), szFileName, GetLastError() );
			return;
		}
		iFnSize = GetFileExSize(hPopUpFileNameOnly);
		if(hPopUpFileNameOnly)  { CloseHandle(hPopUpFileNameOnly);  hPopUpFileNameOnly=NULL; }
	
		//wsprintf(szFnLen, TEXT(" (%I64d Byte)"), iFnSize);
		//lstrcat( szFileNameOnPopUP, szFnLen );

	}
#endif


	AddUbootMenu(hwnd, szFileName);

	szTempFileName[0] = tolower(szFileName[strlen(szFileName)-3]);
	szTempFileName[1] = tolower(szFileName[strlen(szFileName)-2]);
	szTempFileName[2] = tolower(szFileName[strlen(szFileName)-1]);

	if ( !strncmp(szTempFileName,_T("lst"),3) )
	{
		hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE == hFile || NULL==hFile )
		{
			EB_Printf(TEXT("[dnw] can not open lst file! (file:%s, L:%d, Error=%u) \n"), szFileName , __LINE__ , GetLastError());
			if(hWrite) { CloseHandle(hWrite); hWrite=NULL; } 
			if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; } 
			return;
		}
		fileSize=GetFileSize(hFile,NULL);

		lstFileBuf=(char *)malloc(fileSize+1);
		if(NULL == lstFileBuf)
		{
			EB_Printf(TEXT("[dnw] can not allocate memory in lst file. (size:%d, L:%d, Error=%u) \n"),fileSize, __LINE__ , GetLastError());
			if(hFile) CloseHandle(hFile); 
			if(hWrite) { CloseHandle(hWrite); hWrite=NULL; } 
			if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; } 
			return;
		}

		// Read chain.lst file
		rdRet = ReadFile(hFile,(void *)(lstFileBuf),fileSize,&lstFileBufSize,NULL);
		if( !rdRet ) 
		{
			EB_Printf(TEXT("[dnw] can not read ReadFile in lst file. (size=%d, L:%d, Err=%u) \n"),fileSize, __LINE__ , GetLastError());
			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite=NULL; } 
			if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
			if(lstFileBuf) { free(lstFileBuf); lstFileBuf=NULL; } 
			return;
		}
		lstFileBuf[lstFileBufSize] = 0;
		if(hFile) CloseHandle(hFile);

		szBinFileName[0] = strtok(lstFileBuf, "\t\r\n+");
		szBinFileFullName[0] = (char *)malloc(strlen(szDirName)+strlen(szBinFileName[0])+1);
		strcpy(szBinFileFullName[0], szDirName);
		strcat(szBinFileFullName[0], szBinFileName[0]);
		for ( nFileNumber = 1; nFileNumber < MAX_FILE_NUM; nFileNumber++ )
		{
			szBinFileName[nFileNumber] = strtok(NULL, "\r\n+");
			if ( szBinFileName[nFileNumber] == NULL ) break;
			szBinFileFullName[nFileNumber] = (char *)malloc(strlen(szDirName)+strlen(szBinFileName[nFileNumber])+1);
			strcpy(szBinFileFullName[nFileNumber], szDirName);
			strcat(szBinFileFullName[nFileNumber], szBinFileName[nFileNumber]);
		}

		iUSBTxBuf=0;
		
		USBTxBufSize = 7 /* X000FF\n */
					+ 4 /* check sum */
					+ 4 /* num Regions */
					+ nFileNumber*8; /* start address + length */

		for ( i = 0; i < nFileNumber; i ++ )
		{
			hBinFile[i] = CreateFile(szBinFileFullName[i],GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
			if( INVALID_HANDLE_VALUE==hBinFile[i] || NULL==hBinFile[i] )
			{
				EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Error=%u)\n"), szBinFileFullName[i], __LINE__ , GetLastError());
				for(idel=0; idel<MAX_FILE_NUM; idel++) /// added.
				{
					if(szBinFileFullName[idel]) { free((void*)szBinFileFullName[idel]); szBinFileFullName[idel]=NULL; }
				}
				if(lstFileBuf) { free( (void*)lstFileBuf );  lstFileBuf=NULL; }
				if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }  
				if(hWrite) { CloseHandle(hWrite); hWrite=NULL; }  
				return;
			}
			USBTxBufSize += GetFileSize(hBinFile[i],NULL);
		}

		if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
		USBtxBuf=(char *)malloc(USBTxBufSize);

		*((BYTE *)USBtxBuf+0)=0x58; /* X */
		*((BYTE *)USBtxBuf+1)=0x30; /* 0 */
		*((BYTE *)USBtxBuf+2)=0x30; /* 0 */
		*((BYTE *)USBtxBuf+3)=0x30; /* 0 */
		*((BYTE *)USBtxBuf+4)=0x46; /* F */
		*((BYTE *)USBtxBuf+5)=0x46; /* F */
		*((BYTE *)USBtxBuf+6)=0xa; /* \n */

		tmpTxBuf = USBtxBuf  + 7 /* X000FF\n */
						+ 4 /* check sum */
						+ 4 /* num Regions */
						+ nFileNumber*8; /* start address + length */

		for ( i = 0; i < nFileNumber; i++ )
		{
			fileSize = GetFileSize(hBinFile[i],NULL);
			rdRet = ReadFile(hBinFile[i],(void *)(tmpTxBuf),fileSize,&fileBufSize,NULL);
			if( (fileBufSize!=fileSize) || !rdRet )
			{
				if(!rdRet)
				{
					EB_Printf(TEXT("[dnw] find error of ReadFile! (F:%s, L:%d, Err=%u) \n"), __FUNCTION__ , __LINE__ , GetLastError());
				}
				else
				{
					EB_Printf(TEXT("[dnw] find error of ReadFile! (size=%d, L:%d, Err=%u) \n"),fileSize, __LINE__ , GetLastError());
				}

				if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
				if(lstFileBuf) { free( (void*)lstFileBuf ); lstFileBuf=NULL; }

				for(idel=0; idel<MAX_FILE_NUM; idel++) /// added.
				{
					if(szBinFileFullName[idel]) { free((void*)szBinFileFullName[idel]); szBinFileFullName[idel]=NULL; }
				}
				if(hWrite) { CloseHandle(hWrite); hWrite=NULL; }
				if(hBinFile[i]) { CloseHandle(hBinFile[i]); hBinFile[i]=NULL; }
				return;
			}

			for ( j = 0; j < 8; j++ )
			{
				*((BYTE *)USBtxBuf+15+(i*8+j))=(BYTE)(tmpTxBuf[7+j]);
				nCheckSum += (BYTE)(tmpTxBuf[7+j]);
			}
			tmpTxBuf += fileSize;

			CloseHandle(hBinFile[i]);
		}

		tmpTxBuf = USBtxBuf + 7;

		*((DWORD *)tmpTxBuf+0)=nCheckSum;   //checksum
		*((DWORD *)tmpTxBuf+1)=nFileNumber;   //checksum

		szOutFileName = (char *)malloc(OUTFILENAME_LENGTH);
		memset(szOutFileName, 0x00, OUTFILENAME_LENGTH ); /// added 2012.02.21

		strcpy(szOutFileName, szDirName);
		strcat(szOutFileName, TEXT("ubootimage.ubi"));

		hOutFile = CreateFile(szOutFileName,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);
		if( INVALID_HANDLE_VALUE == hOutFile || NULL==hOutFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L=%d, Err=%u) \n"), szOutFileName, __LINE__ , GetLastError());
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(lstFileBuf)  { free( (void*)lstFileBuf ); lstFileBuf=NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(szOutFileName)  { free(szOutFileName);  szOutFileName=NULL; }
			return;
		}

		dwState = WriteFile(hOutFile, (void *)(USBtxBuf), USBTxBufSize, &fileBufSize,NULL);
		if(!dwState) /// added 2011.11.29
		{
			EB_Printf(TEXT("[dnw] can not write USB Tx Buffer! (buff_size:%d, L:%d,Err=%u) \n"), USBTxBufSize , __LINE__ , GetLastError());
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
			if(lstFileBuf) { free( (void*)lstFileBuf ); lstFileBuf=NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return; /// added 2012.02.21
		}
		
		if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
		if(szOutFileName) { free((void*)szOutFileName);  szOutFileName=NULL; } 

		
		threadResult = _beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  /// 2012.02.20, update.....	2011.12.05

			//Create the download progress dialogbox.
			CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
			//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
    		//isn't needed because the dialog box already has WS_VISIBLE attribute.
		}
		else
		{
			EB_Printf("[dnw] can not create progress thread! (error code=%d, L:%d, Err=%u) \n", threadResult, __LINE__ , GetLastError());
			if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			return; /// added 2012.02.21
		}
	}
	else if ( !strncmp(szTempFileName,_T("ubi"),3) )
	{
		hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
		{
			EB_Printf(TEXT("[dnw] can not create file in ubi file. (file:%s, L:%d, Err=%u)\n"), szFileName, __LINE__ , GetLastError());
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}
		fileSize=GetFileSize(hFile,NULL);

		if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
		USBtxBuf=(char *)malloc(fileSize);

		if(NULL==USBtxBuf)
		{
			EB_Printf(TEXT("[dnw] can not allocate memory in ubi file. (size:%d, L:%d, Err=%u)\n"),fileSize, __LINE__ , GetLastError());
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; } 
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}

		// Read ubootimag.ubi file
		rdRet = ReadFile(hFile,(void *)(USBtxBuf),fileSize,&USBTxBufSize,NULL);
		if( (USBTxBufSize!=fileSize) || !rdRet )
		{
			if(!rdRet)
			{
				EB_Printf(TEXT("[dnw] can not read ReadFile in ubi file. (F:%s, L:%d, Err=%u) \n"), __FUNCTION__ , __LINE__ , GetLastError() );
			}
			else
			{
				EB_Printf(TEXT("[dnw] can not read ReadFile in ubi file. (size:%d, L:%d, Err=%u) \n"), fileSize, __LINE__ , GetLastError());
			}
			
			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return;
		}

		if(hFile) CloseHandle(hFile);

		iUSBTxBuf=0;

		threadResult=_beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  /// 2012.02.20, update.....	2011.12.05

			//Create the download progress dialogbox.
			CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
			//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
    		//isn't needed because the dialog box already has WS_VISIBLE attribute.
		}
		else
		{
			EB_Printf("[dnw] can not create progress thread! (error code=%d, L:%d, Err=%u) \n", threadResult, __LINE__ , GetLastError());
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return; /// added 2012.02.21
		}
	}
	else if ( !strncmp(szTempFileName,_T("bin"),3) )
	{
		hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
		{
			EB_Printf(TEXT("[dnw] can not open file. (file:%s, L:%d, %u)\n"), szFileName , __LINE__ , GetLastError());
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}
		fileSize=GetFileSize(hFile,NULL);

		if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
		USBtxBuf=(char *)malloc(fileSize);

		if(NULL==USBtxBuf)
		{
			EB_Printf(TEXT("[dnw] can not allocate memory! (size:%d, L:%d, Err=%u) \n"),fileSize, __LINE__ , GetLastError());
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; } 
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}

		// Read ubootimag.ubi file
		rdRet = ReadFile(hFile,(void *)(USBtxBuf),fileSize,&USBTxBufSize,NULL);
		if( (USBTxBufSize!=fileSize) || !rdRet )
		{
			if(!rdRet)
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (error code=%d, L:%d, Err=%u) \n"), rdRet, __LINE__ , GetLastError());
			}
			else
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Err=%u) \n"), fileSize, __LINE__ , GetLastError());
			}

			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(hFile) { CloseHandle(hFile); hFile=NULL; } /// added.
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}

		if(hFile) CloseHandle(hFile);

		iUSBTxBuf=0;

		threadResult=_beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  /// 2012.02.20, update.....	2011.12.05

			//Create the download progress dialogbox.
			CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
			//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
    		//isn't needed because the dialog box already has WS_VISIBLE attribute.
		}
		else
		{
			EB_Printf("[dnw] can not create progress thread! (error code=%d, L:%d, Err=%u) \n", threadResult, __LINE__ , GetLastError());
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return; /// added 2012.02.21
		}
	}
	else if ( !strncmp(szTempFileName,_T("nb0"),3) || !strncmp(szTempFileName,_T("dio"),3) )
	{
		nFileNumber = 1;

		hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u)\n"), szFileName , __LINE__ , GetLastError());
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}
		fileSize=GetFileSize(hFile,NULL);

		if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
		USBtxBuf=(char *)malloc(fileSize+7+4+4+4+4+MAX_PATH);
		if(NULL==USBtxBuf)
		{
			EB_Printf(TEXT("[dnw] can not allocate memory (size:%d, L:%d, Err=%u) \n"),fileSize, __LINE__ , GetLastError());
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; } 
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}
		memset((void *)USBtxBuf, 0, fileSize+7+4+4+4+4+MAX_PATH);

		// Read nb0 file
		rdRet = ReadFile(hFile,(void *)(USBtxBuf+7+4+4+4+4+MAX_PATH),fileSize,&USBTxBufSize,NULL);
		if( (USBTxBufSize!=fileSize) || !rdRet )
		{
			if(!rdRet)
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (error code=%d, L:%d, Err=%u) \n"), rdRet, __LINE__ , GetLastError());
			}
			else
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Err=%u) \n"), fileSize, __LINE__ , GetLastError() );
			}

			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return;
		}

		if(hFile) CloseHandle(hFile);

		tmpTxBuf = USBtxBuf+7+4+4+4+4+MAX_PATH;

		iUSBTxBuf=0;
		
		*((BYTE *)USBtxBuf+0)=0x4E;
		*((BYTE *)USBtxBuf+1)=0x30;
		*((BYTE *)USBtxBuf+2)=0x30;
		*((BYTE *)USBtxBuf+3)=0x30;
		*((BYTE *)USBtxBuf+4)=0x46;
		*((BYTE *)USBtxBuf+5)=0x46;
		*((BYTE *)USBtxBuf+6)=0xa;

		tmpTxBuf = USBtxBuf + 7 + 4 + 4;

		*((DWORD *)tmpTxBuf+0)=0;				//nb0 start address == 0
		*((DWORD *)tmpTxBuf+1)=fileSize;		//nb0 filesize



		strcpy((char *)(tmpTxBuf+8), szTitleName);

		tmpTxBuf = USBtxBuf+7;
		nCheckSum = 0;
		for ( i = 0; i < 4+4+MAX_PATH; i++ )
		{
			nCheckSum += (unsigned char)(*(tmpTxBuf+i));
		}

		*((DWORD *)tmpTxBuf+0)=nCheckSum;		//checksum
		*((DWORD *)tmpTxBuf+1)=nFileNumber;   //file number = 1

		szOutFileName = (char *)malloc(OUTFILENAME_LENGTH);
		memset(szOutFileName, 0x00, OUTFILENAME_LENGTH ); /// added 2012.02.21

		strcpy(szOutFileName, szDirName);
		strcat(szOutFileName, TEXT("ubootimage.ubi"));

		USBTxBufSize += (7+4+4+4+4+MAX_PATH);

		hOutFile = CreateFile(szOutFileName,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);
		if( INVALID_HANDLE_VALUE==hOutFile || NULL==hOutFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u) \n"), szOutFileName, __LINE__ , GetLastError());
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(szOutFileName) { free(szOutFileName);  szOutFileName=NULL; }
			return;
		}

		dwState = WriteFile(hOutFile, (void *)(USBtxBuf), USBTxBufSize, &fileBufSize,NULL);
		if( !dwState ) /// added 2011.11.29
		{
			EB_Printf("[dnw] can not write USB Tx File (size:%d, L:%d, Err=%u) \n", USBTxBufSize, __LINE__ , GetLastError());
			if(hOutFile)  { CloseHandle(hOutFile); hOutFile=NULL; } 
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(szOutFileName) { free(szOutFileName);  szOutFileName=NULL; }
			return; /// 2012.02.21
		}

		if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
		if(szOutFileName) { free((void*)szOutFileName);  szOutFileName=NULL; } 



		threadResult=_beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  /// 2012.02.20, update.....	2011.12.05

			//Create the download progress dialogbox.
			CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
			//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
    		//isn't needed because the dialog box already has WS_VISIBLE attribute.
		}
		else
		{
			EB_Printf("[dnw] can not create progress thread! (error code=%d, L:%d, Err=%u) \n", threadResult, __LINE__ , GetLastError());
			if(hWrite) { CloseHandle(hWrite); hWrite=NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			return; /// added 2012.02.21
		}
	}
	else
	{
		EB_Printf("[dnw] can not support file format! (file:%s, L:%d, Err=%u) \n", szTempFileName, __LINE__ , GetLastError() );
		if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
		return;
	}

}


void MenuUBOOT2(HWND hwnd, BYTE funcKey)
{
	ULONG i, j;
	HANDLE hFile = NULL;
	HANDLE hOutFile = NULL;
	HANDLE hBinFile[MAX_FILE_NUM] = { NULL, };
	char * szBinFileName[MAX_FILE_NUM] = {0,};
	char * szBinFileFullName[MAX_FILE_NUM] = {0,};
	char * szOutFileName = NULL;

	ULONG fileSize;
	BOOL result = FALSE;
	uintptr_t threadResult;
	TCHAR	szTempFileName[3] = {0,};
	ULONG nFileNumber;
	unsigned int nCheckSum = 0;
	volatile char * tmpTxBuf = NULL;
	char *lstFileBuf = NULL;
	DWORD lstFileBufSize;
	TCHAR szDirName[FILENAMELEN] = {0,};
	int cnt=0, idel=0;
	BOOL rdRet=FALSE;
	DWORD dwState;

	
	if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } /// added.

	if( autoSendKey ) // TRUE == ~~~~
	{ 
		switch( RetryUSB )
		{
			case 1: /// IDC_RETRY_USB_20_TIMES
				dnUSBcount = 20; 
				break;
			case 2: /// IDC_RETRY_USB_30_TIMES
				dnUSBcount = 30; 
				break;
			case 3: /// IDC_RETRY_USB_40_TIMES
				dnUSBcount = 40; 
				break;
			case 0: /// IDC_RETRY_USB_DEFAULT
			default:
				dnUSBcount = 1; 
				break;
		}
	} /// 2012.03.06
	else { dnUSBcount = 1; } /// 2012.03.06

	cnt = 0;
	do {
		Sleep(USB_DELAY_TIME);
		hWrite = open_file( outPipe, cnt, funcKey );
		if( cnt++ > USB_OPEN_RETRY*dnUSBcount ) break;
	} while( (INVALID_HANDLE_VALUE==hWrite) || (NULL==hWrite) );

	if( (INVALID_HANDLE_VALUE==hWrite) || (NULL==hWrite) )
	{
		MessageBox(hwnd,TEXT("Can't open USB device.\n"),TEXT("UBOOT2 Error"), MB_OK | MB_ICONINFORMATION );
		return;
	}

	result=PopFileOpenDlg(hwnd,szUBOOTFileName,szUBOOTTitleName);
	memcpy(szFileName, szUBOOTFileName, FILENAMELEN);
	memcpy(szTitleName, szUBOOTTitleName, FILENAMELEN);

	strncpy ( szDirName, szFileName, strlen(szFileName) - strlen(szTitleName) );
	szDirName[strlen(szFileName) - strlen(szTitleName)] = 0;

	if( 0 == result ) //file open fail
	{
		if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
		return;
	}



#if 1 // 2016.10.11, POPUP_FileName_Only
	{
		extern TCHAR szFileNameOnPopUP[FILENAMELEN];
		TCHAR szFnLen[MAX_PATH] = {0,};
		int iLen = 0, jdx=0;
		__int64  iFnSize=0L;
		HANDLE hPopUpFileNameOnly=NULL;
		
		iLen = strlen(szFileName);
		if( iLen > MAX_PATH )
		{
			EB_Printf(TEXT("\r\n[dnw] Input file length is over! length(%d) \r\n"), iLen );
			BeepSound(4);
		}
	
		for(jdx=iLen; iLen>0; jdx--) { if( szFileName[jdx] == '\\' ) break; }
	
		lstrcpy( szFileNameOnPopUP, &szFileName[jdx+1] ); // input filename

		// -----------------------------------------------------------------
		// 3. File Read and size
		// -----------------------------------------------------------------
		hPopUpFileNameOnly = CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hPopUpFileNameOnly || NULL==hPopUpFileNameOnly )
		{
			EB_Printf(TEXT("[dnw] can not open file[%s]. Err(%u)\n"), szFileName, GetLastError() );
			return;
		}
		iFnSize = GetFileExSize(hPopUpFileNameOnly);
		if(hPopUpFileNameOnly)  { CloseHandle(hPopUpFileNameOnly);  hPopUpFileNameOnly=NULL; }
	
		//wsprintf(szFnLen, TEXT(" (%I64d Byte)"), iFnSize);
		//lstrcat( szFileNameOnPopUP, szFnLen );

	}
#endif



	AddUbootMenu2(hwnd, szFileName);

	szTempFileName[0] = tolower(szFileName[strlen(szFileName)-3]);
	szTempFileName[1] = tolower(szFileName[strlen(szFileName)-2]);
	szTempFileName[2] = tolower(szFileName[strlen(szFileName)-1]);
	if ( !strncmp(szTempFileName,_T("lst"),3) )
	{
		hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
		{
			EB_Printf(TEXT("[dnw] can not create file[%s]. (L:%d, Err=%u)\n"), szFileName , __LINE__ , GetLastError());
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}
		fileSize=GetFileSize(hFile,NULL);

		lstFileBuf=(char *)malloc(fileSize+1);
		if(NULL == lstFileBuf)
		{
			EB_Printf(TEXT("[dnw] can not allocate memory in UBOOT2 file. (size:%d, L:%d, Err=%u) \n"),fileSize, __LINE__ , GetLastError());
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}

		// Read chain.lst file
		rdRet = ReadFile(hFile,(void *)(lstFileBuf),fileSize,&lstFileBufSize,NULL);
		if( !rdRet ) /// added.
		{
			EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Err=%u) \n"), fileSize, __LINE__ , GetLastError());

			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(lstFileBuf) { free(lstFileBuf); lstFileBuf=NULL; }
			return;
		}
		lstFileBuf[lstFileBufSize] = 0;
		if(hFile) CloseHandle(hFile);

		szBinFileName[0] = strtok(lstFileBuf, "\t\r\n+");
		szBinFileFullName[0] = (char *)malloc(strlen(szDirName)+strlen(szBinFileName[0])+1);
		strcpy(szBinFileFullName[0], szDirName);
		strcat(szBinFileFullName[0], szBinFileName[0]);
		for ( nFileNumber = 1; nFileNumber < MAX_FILE_NUM; nFileNumber++ )
		{
			szBinFileName[nFileNumber] = strtok(NULL, "\r\n+");
			if ( szBinFileName[nFileNumber] == NULL ) break;
			szBinFileFullName[nFileNumber] = (char *)malloc(strlen(szDirName)+strlen(szBinFileName[nFileNumber])+1);
			strcpy(szBinFileFullName[nFileNumber], szDirName);
			strcat(szBinFileFullName[nFileNumber], szBinFileName[nFileNumber]);
		}

		iUSBTxBuf=0;
		
		USBTxBufSize = 7 /* X000FF\n */
					+ 4 /* check sum */
					+ 4 /* num Regions */
					+ nFileNumber*(8+260); /* start address + length + filename */

		for ( i = 0; i < nFileNumber; i ++ )
		{
			hBinFile[i] = CreateFile(szBinFileFullName[i],GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
			if( INVALID_HANDLE_VALUE==hBinFile[i] || NULL==hBinFile[i] )
			{
				EB_Printf(TEXT("[dnw] can not open file ! (file:%s, L:%d, Err=%u)\n"), szBinFileFullName[i] , __LINE__ , GetLastError());
				if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
				if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
				if(lstFileBuf) { free( (void*)lstFileBuf ); lstFileBuf=NULL; }

				for(idel=0; idel<MAX_FILE_NUM; idel++) /// added.
				{
					if(szBinFileFullName[idel]) { free((void*)szBinFileFullName[idel]); szBinFileFullName[idel]=NULL; }
				}
				return;
			}
			USBTxBufSize += GetFileSize(hBinFile[i],NULL);
		}

		if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
		USBtxBuf=(char *)malloc(USBTxBufSize);

		*((BYTE *)USBtxBuf+0)=0x4E;
		*((BYTE *)USBtxBuf+1)=0x30;
		*((BYTE *)USBtxBuf+2)=0x30;
		*((BYTE *)USBtxBuf+3)=0x30;
		*((BYTE *)USBtxBuf+4)=0x46;
		*((BYTE *)USBtxBuf+5)=0x46;
		*((BYTE *)USBtxBuf+6)=0xa;

		tmpTxBuf = USBtxBuf  + 7 /* X000FF\n */
						+ 4 /* check sum */
						+ 4 /* num Regions */
						+ nFileNumber*(8+260); /* start address + length */

		for ( i = 0; i < nFileNumber; i++ )
		{
			fileSize = GetFileSize(hBinFile[i],NULL);
			rdRet = ReadFile(hBinFile[i],(void *)(tmpTxBuf),fileSize,&fileBufSize,NULL);
			if( (fileBufSize!=fileSize) || !rdRet )
			{
				if(!rdRet)
				{
					EB_Printf(TEXT("[dnw] find error of ReadFile. (error code=%d, L:%d, Err=%u) \n"), rdRet, __LINE__ , GetLastError());
				}
				else
				{
					EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Err=%u) \n"), fileSize, __LINE__ , GetLastError());
				}

				if(hFile) { CloseHandle(hFile); hFile=NULL; }
				if(hBinFile[i]) { CloseHandle(hBinFile[i]); hBinFile[i]=NULL; }
				if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
				if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
				if(lstFileBuf) { free(lstFileBuf); lstFileBuf=NULL; }
				return;
			}

			for ( j = 0; j < 8 + 260; j++ )
			{
				if ( j < 8 )
				{
					*((BYTE *)USBtxBuf+15+(i*(8+260)+j))=(BYTE)(tmpTxBuf[7+j]);
					nCheckSum += (BYTE)(tmpTxBuf[7+j]);
				}
				else if ( j >= 8 && j < (8+strlen(szBinFileName[i])) )
				{
					*((BYTE *)USBtxBuf+15+(i*(8+260)+j))=(BYTE)(szBinFileName[i][j-8]);
					nCheckSum += (BYTE)(szBinFileName[i][j-8]);
				}
				else
				{
					*((BYTE *)USBtxBuf+15+(i*(8+260)+j))=0;
				}
			}

			tmpTxBuf += fileSize;

			CloseHandle(hBinFile[i]);
		}

		tmpTxBuf = USBtxBuf + 7;

		*((DWORD *)tmpTxBuf+0)=nCheckSum;   //checksum
		*((DWORD *)tmpTxBuf+1)=nFileNumber;   //checksum

		szOutFileName = (char *)malloc(OUTFILENAME_LENGTH);
		memset(szOutFileName, 0x00, OUTFILENAME_LENGTH ); /// added 2012.02.21

		strcpy(szOutFileName, szDirName);
		strcat(szOutFileName, TEXT("ubootimage.ubi"));

		hOutFile = CreateFile(szOutFileName,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);
		if( INVALID_HANDLE_VALUE==hOutFile || NULL==hOutFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u) \n"), szOutFileName, __LINE__ , GetLastError());
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(lstFileBuf) { free(lstFileBuf); lstFileBuf=NULL; }
			return;
		}

		dwState = WriteFile(hOutFile, (void *)(USBtxBuf), USBTxBufSize, &fileBufSize,NULL);
		if( !dwState ) /// added 2011.11.29
		{
			EB_Printf("[dnw] can not write USB Tx buffer! (size:%d , L:%d, Err=%u) \n", USBTxBufSize , __LINE__  , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			if(lstFileBuf) { free(lstFileBuf); lstFileBuf=NULL; }
			return; /// added 2012.02.21
		}

		if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
		if(szOutFileName) { free((void*)szOutFileName);  szOutFileName=NULL; } 

		threadResult = _beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  /// 2012.02.20, update.....	2011.12.05


			//Create the download progress dialogbox.
			CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
			//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
    		//isn't needed because the dialog box already has WS_VISIBLE attribute.
		}
		else
		{
			EB_Printf("[dnw] can not create progress thread! (error code=%d, L:%d, Err=%u) \n", threadResult, __LINE__ , GetLastError() );
			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(lstFileBuf) { free(lstFileBuf); lstFileBuf=NULL; }
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			return; /// added 2012.02.21
		}
	}
	else if ( !strncmp(szTempFileName,_T("ubi"),3) )
	{
		hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u)\n"), szFileName , __LINE__ , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}
		fileSize=GetFileSize(hFile,NULL);

		if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
		USBtxBuf=(char *)malloc(fileSize);

		if(NULL==USBtxBuf)
		{
			EB_Printf(TEXT("[dnw] can not allocate USB Tx Buffer memory. (L:%d, Err=%u) \n"), __LINE__ , GetLastError());
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}

		// Read ubootimag.ubi file
		rdRet = ReadFile(hFile,(void *)(USBtxBuf),fileSize,&USBTxBufSize,NULL);
		if( (USBTxBufSize!=fileSize) || !rdRet )
		{
			if(!rdRet)
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (Ret=%d, L:%d, Err=%u) \n"), rdRet, __LINE__ , GetLastError() );
			}
			else
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Err=%u) \n"), fileSize, __LINE__ , GetLastError() );
			}

			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return;
		}
		if(hFile) CloseHandle(hFile);

		iUSBTxBuf=0;

		threadResult=_beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  /// 2012.02.20, update.....	2011.12.05

			//Create the download progress dialogbox.
			CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
			//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
    		//isn't needed because the dialog box already has WS_VISIBLE attribute.
		}
		else
		{
			EB_Printf("[dnw] can not create progress thread! (Result=%d, L:%d, Err=%u) \n", threadResult, __LINE__ , GetLastError() );
			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return; /// added 2012.02.21
		}
	}
	else if ( !strncmp(szTempFileName,_T("bin"),3) )
	{
		hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u)\n"), szFileName , __LINE__ , GetLastError());
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}
		fileSize=GetFileSize(hFile,NULL);

		if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
		USBtxBuf=(char *)malloc(fileSize);

		if(NULL==USBtxBuf)
		{
			EB_Printf(TEXT("[dnw] can not allocate USB Tx Buffer memory. (L:%d, Err=%u) \n"), __LINE__ , GetLastError() );
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}

		// Read ubootimag.ubi file
		rdRet = ReadFile(hFile,(void *)(USBtxBuf),fileSize,&USBTxBufSize,NULL);
		if( (USBTxBufSize!=fileSize) || !rdRet )
		{
			if(!rdRet)
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (Ret=%d, L:%d, Err=%u) \n"), rdRet, __LINE__ , GetLastError() );
			}
			else
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Err=%u) \n"), fileSize, __LINE__ , GetLastError() );
			}

			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return;
		}
		if(hFile) CloseHandle(hFile);

		iUSBTxBuf=0;

		threadResult = _beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

		if( threadResult != -1 )
		{
			Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  /// 2012.02.20, update.....	2011.12.05

			//Create the download progress dialogbox.
			CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
			//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
    		//isn't needed because the dialog box already has WS_VISIBLE attribute.
		}
		else
		{
			EB_Printf("[dnw] can not create progress thread! (Result=%d, L:%d, Err=%u) \n", threadResult, __LINE__ , GetLastError() );
			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return; /// added 2012.02.21
		}
	}
	else if ( !strncmp(szTempFileName,_T("nb0"),3) || !strncmp(szTempFileName,_T("dio"),3) )
	{
		nFileNumber = 1;

		hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u) \n"), szFileName , __LINE__ , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}
		fileSize=GetFileSize(hFile,NULL);

		if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
		USBtxBuf=(char *)malloc(fileSize+7+4+4+4+4+MAX_PATH);
		if(NULL==USBtxBuf)
		{
			EB_Printf(TEXT("[dnw] can not allocate USB Tx Buffer memory. (L:%d, Err=%u) \n"), __LINE__ , GetLastError() );
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}
		memset((void *)USBtxBuf, 0, fileSize+7+4+4+4+4+MAX_PATH);

		// Read nb0 file
		rdRet = ReadFile(hFile,(void *)(USBtxBuf+7+4+4+4+4+MAX_PATH),fileSize,&USBTxBufSize,NULL);
		if( (USBTxBufSize!=fileSize) || !rdRet )
		{
			if(!rdRet)
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (Ret=%d, L:%d, Err=%u) \n"), rdRet, __LINE__ , GetLastError());
			}
			else
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Err=%u) \n"), fileSize, __LINE__ , GetLastError());
			}

			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return;
		}
		if(hFile) CloseHandle(hFile);

		tmpTxBuf = USBtxBuf+7+4+4+4+4+MAX_PATH;

		iUSBTxBuf=0;
		
		*((BYTE *)USBtxBuf+0)=0x4E;
		*((BYTE *)USBtxBuf+1)=0x30;
		*((BYTE *)USBtxBuf+2)=0x30;
		*((BYTE *)USBtxBuf+3)=0x30;
		*((BYTE *)USBtxBuf+4)=0x46;
		*((BYTE *)USBtxBuf+5)=0x46;
		*((BYTE *)USBtxBuf+6)=0xa;

		tmpTxBuf = USBtxBuf + 7 + 4 + 4;

		*((DWORD *)tmpTxBuf+0)=0;				//nb0 start address == 0
		*((DWORD *)tmpTxBuf+1)=fileSize;		//nb0 filesize



		strcpy((char *)(tmpTxBuf+8), szTitleName);

		tmpTxBuf = USBtxBuf+7;
		nCheckSum = 0;
		for ( i = 0; i < 4+4+MAX_PATH; i++ )
		{
			nCheckSum += (unsigned char)(*(tmpTxBuf+i));
		}

		*((DWORD *)tmpTxBuf+0)=nCheckSum;		//checksum
		*((DWORD *)tmpTxBuf+1)=nFileNumber;   //file number = 1

		szOutFileName = (char *)malloc(OUTFILENAME_LENGTH);
		memset(szOutFileName, 0x00, OUTFILENAME_LENGTH ); /// added 2012.02.21

		strcpy(szOutFileName, szDirName);
		strcat(szOutFileName, TEXT("ubootimage.ubi"));

		USBTxBufSize += (7+4+4+4+4+MAX_PATH);

		hOutFile = CreateFile(szOutFileName,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);
		if( INVALID_HANDLE_VALUE==hOutFile || NULL==hOutFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u) \n"), szOutFileName, __LINE__ , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			return;
		}

		dwState = WriteFile(hOutFile, (void *)(USBtxBuf), USBTxBufSize, &fileBufSize,NULL);
		if( !dwState ) /// added 2011.11.29
		{
			EB_Printf("[dnw] can not write file! (size:%d, L:%d, Err=%u) \n", USBTxBufSize, __LINE__ , GetLastError() );
			if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return; /// added 2012.02.21
		}

		if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
		if(szOutFileName) { free((void*)szOutFileName);  szOutFileName=NULL; } 


		threadResult=_beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  /// 2012.02.20, update.....	2011.12.05

			//Create the download progress dialogbox.
			CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
			//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
    		//isn't needed because the dialog box already has WS_VISIBLE attribute.
		}
		else
		{
			EB_Printf("[dnw] can not create progress thread! (Result=%d, L:%d, Err=%u) \n", threadResult, __LINE__ , GetLastError() );
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return; /// added 2012.02.21
		}
	}
	else
	{
		EB_Printf("[dnw] can not support file format in MenuUBOOT2! (file:%s, L:%d, Err=%u)  \n", szTempFileName, __LINE__ , GetLastError() );
		if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } /// added.
		return;
	}

}

void MenuUBOOTHistory(HWND hwnd, int history)
{
	ULONG i, j;
	HANDLE hFile = NULL;
	HANDLE hOutFile = NULL;
	HANDLE hBinFile[MAX_FILE_NUM] = { NULL, };
	char * szBinFileName[MAX_FILE_NUM] = {0,};
	char * szBinFileFullName[MAX_FILE_NUM] = {0,};
	char * szOutFileName = NULL;

	ULONG fileSize;
	USHORT cs=0;
	uintptr_t threadResult;
	TCHAR	szTempFileName[3] = {0,};
	ULONG nFileNumber;
	unsigned int nCheckSum = 0;
	volatile char * tmpTxBuf = NULL;
	char *lstFileBuf = NULL;
	DWORD lstFileBufSize;

	TCHAR szDirName[FILENAMELEN] = {0,};
	int lastpoint;
	TCHAR *tocken = NULL;
	int cnt=0, idel=0;
	BOOL rdRet=FALSE;
	DWORD dwState;


	if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }

	if( autoSendKey ) // TRUE == ~~~~
	{ 
		switch( RetryUSB )
		{
			case 1: /// IDC_RETRY_USB_20_TIMES
				dnUSBcount = 20; 
				break;
			case 2: /// IDC_RETRY_USB_30_TIMES
				dnUSBcount = 30; 
				break;
			case 3: /// IDC_RETRY_USB_40_TIMES
				dnUSBcount = 40; 
				break;
			case 0: /// IDC_RETRY_USB_DEFAULT
			default:
				dnUSBcount = 1; 
				break;
		}
	} /// 2012.03.06
	else { dnUSBcount = 1; } /// 2012.03.06

	cnt = 0;
	do {
		Sleep(USB_DELAY_TIME);
		hWrite = open_file( outPipe, cnt, UBOOT_OTHER );
		if( cnt++ > USB_OPEN_RETRY*dnUSBcount ) break;
	} while( (INVALID_HANDLE_VALUE==hWrite) || (NULL==hWrite) );
	
	if( (INVALID_HANDLE_VALUE==hWrite) || (NULL==hWrite) )
	{
		MessageBox(hwnd,TEXT("Can't open USB device.\n"),TEXT("Error"), MB_OK | MB_ICONINFORMATION );
		return;
	}

//    result=PopFileOpenDlg(hwnd,szUBOOTFileName,szUBOOTTitleName);
//	tocken = strtok(,",");

	memcpy(szFileName, szDownloadString[history], strlen(szDownloadString[history]));
	szFileName[strlen(szDownloadString[history])] = '\0';
	for ( i = 0; i < FILENAMELEN; i++ )
	{
		if ( szDownloadString[history][i] == '\\' )
		{
			lastpoint = i;
			tocken = &szDownloadString[history][i+1];
		}
		if ( szDownloadString[history][i] == '\0' )
			break;
	}
	memcpy(szTitleName, tocken, FILENAMELEN-i);

	strncpy ( szDirName, szFileName, strlen(szFileName) - strlen(szTitleName) );
	szDirName[strlen(szFileName) - strlen(szTitleName)] = 0;
    
//    if(result==0) //file open fail
//    {
//		EB_Printf(TEXT("[dnw] [ERROR:File Open](%s)\n"), szFileName);
//		CloseHandle(hWrite);
//		return;
//    }

	AddUbootMenu(hwnd, szFileName);

	szTempFileName[0] = tolower(szFileName[strlen(szFileName)-3]);
	szTempFileName[1] = tolower(szFileName[strlen(szFileName)-2]);
	szTempFileName[2] = tolower(szFileName[strlen(szFileName)-1]);
	
	if ( !strncmp(szTempFileName,_T("lst"),3) )
	{
		hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u)\n"), szFileName , __LINE__ , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}
		fileSize=GetFileSize(hFile,NULL);

		lstFileBuf=(char *)malloc(fileSize+1);
		if(NULL == lstFileBuf)
		{
			EB_Printf(TEXT("[dnw] can not allocate memory in lst file. (size:%d, L:%d, Err=%u) \n"),fileSize, __LINE__ , GetLastError() );
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}

		// Read chain.lst file
		rdRet = ReadFile(hFile,(void *)(lstFileBuf),fileSize,&lstFileBufSize,NULL);
		if( !rdRet ) /// added.
		{
			EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d Err=%u) \n"), fileSize, __LINE__ , GetLastError() );

			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(lstFileBuf) { free(lstFileBuf); lstFileBuf=NULL; }
			return;
		}
		lstFileBuf[lstFileBufSize] = 0;
		CloseHandle(hFile);

		szBinFileName[0] = strtok(lstFileBuf, "\t\r\n+");
		szBinFileFullName[0] = (char *)malloc(strlen(szDirName)+strlen(szBinFileName[0])+1);
		strcpy(szBinFileFullName[0], szDirName);
		strcat(szBinFileFullName[0], szBinFileName[0]);
		for ( nFileNumber = 1; nFileNumber < MAX_FILE_NUM; nFileNumber++ )
		{
			szBinFileName[nFileNumber] = strtok(NULL, "\r\n+");
			if ( szBinFileName[nFileNumber] == NULL ) break;
			szBinFileFullName[nFileNumber] = (char *)malloc(strlen(szDirName)+strlen(szBinFileName[nFileNumber])+1);
			strcpy(szBinFileFullName[nFileNumber], szDirName);
			strcat(szBinFileFullName[nFileNumber], szBinFileName[nFileNumber]);
		}

		iUSBTxBuf=0;
		
		USBTxBufSize = 7 /* X000FF\n */
					+ 4 /* check sum */
					+ 4 /* num Regions */
					+ nFileNumber*8; /* start address + length */

		for ( i = 0; i < nFileNumber; i ++ )
		{
			hBinFile[i] = CreateFile(szBinFileFullName[i],GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
			if( INVALID_HANDLE_VALUE==hBinFile[i] || NULL==hBinFile[i] )
			{
				EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u)\n"), szBinFileFullName[i] , __LINE__ , GetLastError() );
				if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
				if(lstFileBuf) { free( (void*)lstFileBuf ); lstFileBuf=NULL; }

				for(idel=0; idel<MAX_FILE_NUM; idel++) /// added.
				{
					if(szBinFileFullName[idel]) { free((void*)szBinFileFullName[idel]); szBinFileFullName[idel]=NULL; }
				}
				return;
			}
			USBTxBufSize += GetFileSize(hBinFile[i],NULL);
		}

		if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
		USBtxBuf=(char *)malloc(USBTxBufSize);

		*((BYTE *)USBtxBuf+0)=0x58;
		*((BYTE *)USBtxBuf+1)=0x30;
		*((BYTE *)USBtxBuf+2)=0x30;
		*((BYTE *)USBtxBuf+3)=0x30;
		*((BYTE *)USBtxBuf+4)=0x46;
		*((BYTE *)USBtxBuf+5)=0x46;
		*((BYTE *)USBtxBuf+6)=0xa;

		tmpTxBuf = USBtxBuf  + 7 /* X000FF\n */
						+ 4 /* check sum */
						+ 4 /* num Regions */
						+ nFileNumber*8; /* start address + length */

		for ( i = 0; i < nFileNumber; i++ )
		{
			fileSize = GetFileSize(hBinFile[i],NULL);
			rdRet = ReadFile(hBinFile[i],(void *)(tmpTxBuf),fileSize,&fileBufSize,NULL);
			if( (fileBufSize!=fileSize) || !rdRet )
			{
				if(!rdRet)
				{
					EB_Printf(TEXT("[dnw] find error of ReadFile. (Ret=%d, L:%d, Err=%u) \n"), rdRet, __LINE__ , GetLastError());
				}
				else
				{
					EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Err=%u) \n"), fileSize, __LINE__ , GetLastError() );
				}

				if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
				if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
				if(lstFileBuf) { free( (void*)lstFileBuf ); lstFileBuf=NULL; }

				for(idel=0; idel<MAX_FILE_NUM; idel++) /// added.
				{
					if(szBinFileFullName[idel]) { free((void*)szBinFileFullName[idel]); szBinFileFullName[idel]=NULL; }
				}
				if(hBinFile[i]) { CloseHandle(hBinFile[i]); hBinFile[i]=NULL; }
				return;
			}

			for ( j = 0; j < 8; j++ )
			{
				*((BYTE *)USBtxBuf+15+(i*8+j))=(BYTE)(tmpTxBuf[7+j]);
				nCheckSum += (BYTE)(tmpTxBuf[7+j]);
			}
			tmpTxBuf += fileSize;

			CloseHandle(hBinFile[i]);
		}

		tmpTxBuf = USBtxBuf + 7;

		*((DWORD *)tmpTxBuf+0)=nCheckSum;   //checksum
		*((DWORD *)tmpTxBuf+1)=nFileNumber;   //checksum

		szOutFileName = (char *)malloc(OUTFILENAME_LENGTH);
		memset(szOutFileName, 0x00, OUTFILENAME_LENGTH ); /// added 2012.02.21

		strcpy(szOutFileName, szDirName);
		strcat(szOutFileName, TEXT("ubootimage.ubi"));

		hOutFile = CreateFile(szOutFileName,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);
		if( INVALID_HANDLE_VALUE==hOutFile || NULL==hOutFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s L:%d, Err=%u) \n"), szOutFileName, __LINE__ , GetLastError() );
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			if(lstFileBuf) { free(lstFileBuf); lstFileBuf=NULL; }
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			return;
		}

		dwState = WriteFile(hOutFile, (void *)(USBtxBuf), USBTxBufSize, &fileBufSize,NULL);
		if( !dwState ) /// added 2011.11.29
		{
			EB_Printf("[dnw] can not write USB Tx buffer! (size:%d, L:%d, Err=%u) \n", USBTxBufSize, __LINE__ , GetLastError()  );
			if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(hFile) CloseHandle(hFile);
			if(lstFileBuf) { free(lstFileBuf); lstFileBuf=NULL; }
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return;
		}

		if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
		if(szOutFileName) { free((void*)szOutFileName);  szOutFileName=NULL; } 


		threadResult=_beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  /// 2012.02.20, update.....	2011.12.05

			//Create the download progress dialogbox.
			CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
			//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
    		//isn't needed because the dialog box already has WS_VISIBLE attribute.
		}
		else
		{
			EB_Printf("[dnw] can not create progress thread! (Result=%d, L:%d, Err=%u) \n", threadResult, __LINE__ , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(lstFileBuf) { free(lstFileBuf); lstFileBuf=NULL; }
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return; /// added 2012.02.21
		}
	}
	else if ( !strncmp(szTempFileName,_T("ubi"),3) )
	{
		hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u) \n"), szFileName , __LINE__ , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}
		fileSize=GetFileSize(hFile,NULL);

		if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
		USBtxBuf=(char *)malloc(fileSize);
		if(NULL==USBtxBuf)
		{
			EB_Printf(TEXT("[dnw] can not allocate USB Tx Buffer memory. (L:%d, Err=%u) \n"), __LINE__ , GetLastError());
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}

		// Read ubootimag.ubi file
		rdRet = ReadFile(hFile,(void *)(USBtxBuf),fileSize,&USBTxBufSize,NULL);
		if( (USBTxBufSize!=fileSize) || !rdRet )
		{
			if(!rdRet)
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (Ret=%d, L:%d, Err=%u) \n"), rdRet, __LINE__ , GetLastError() );
			}
			else
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Err=%u) \n"), fileSize, __LINE__ , GetLastError() );
			}

			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return;
		}
		if(hFile) CloseHandle(hFile);

		iUSBTxBuf=0;

		threadResult=_beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  /// 2012.02.20, update.....	2011.12.05

			//Create the download progress dialogbox.
			CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
			//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
    		//isn't needed because the dialog box already has WS_VISIBLE attribute.
		}
		else
		{
			EB_Printf("[dnw] can not create progress thread! (Result=%d, L:%d, Err=%u) \n", threadResult, __LINE__ , GetLastError() );
			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite=NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return; /// added 2012.02.21
		}
	}
	else if ( !strncmp(szTempFileName,_T("bin"),3) )
	{
		hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u) \n"), szFileName , __LINE__ , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}
		fileSize=GetFileSize(hFile,NULL);

		if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
		USBtxBuf=(char *)malloc(fileSize);
		if(NULL==USBtxBuf)
		{
			EB_Printf(TEXT("[dnw] can not allocate USB Tx Buffer memory. (L:%d, Err=%u) \n"),  __LINE__ , GetLastError() );
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}

		// Read ubootimag.ubi file
		rdRet = ReadFile(hFile,(void *)(USBtxBuf),fileSize,&USBTxBufSize,NULL);
		if( (USBTxBufSize!=fileSize) || !rdRet )
		{
			if(!rdRet)
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (Ret=%d, L:%d, Err=%u) \n"), rdRet, __LINE__ , GetLastError());
			}
			else
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Err=%u) \n"), fileSize, __LINE__ , GetLastError() );
			}

			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return;
		}
		if(hFile) CloseHandle(hFile);

		iUSBTxBuf=0;

		threadResult=_beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  /// 2012.02.20, update.....	2011.12.05

			//Create the download progress dialogbox.
			CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
			//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
    		//isn't needed because the dialog box already has WS_VISIBLE attribute.
		}
		else
		{
			EB_Printf("[dnw] can not create progress thread! (Result=%d, L:%d, Err=%u) \n", threadResult, __LINE__ , GetLastError());
			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return;
		}
	}
	else if ( !strncmp(szTempFileName,_T("nb0"),3) || !strncmp(szTempFileName,_T("dio"),3) )
	{
		nFileNumber = 1;

		hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u) \n"), szFileName ,  __LINE__ , GetLastError());
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}
		fileSize=GetFileSize(hFile,NULL);

		if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
		USBtxBuf=(char *)malloc(fileSize+7+4+4+4+4+MAX_PATH);
		if(NULL==USBtxBuf)
		{
			EB_Printf(TEXT("[dnw] can not allocate USB Tx Buffer memory. (L:%d, Err=%u) \n"),  __LINE__ , GetLastError() );
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}
		memset((void *)USBtxBuf, 0, fileSize+7+4+4+4+4+MAX_PATH);

		// Read nb0 file
		rdRet = ReadFile(hFile,(void *)(USBtxBuf+7+4+4+4+4+MAX_PATH),fileSize,&USBTxBufSize,NULL);
		if( (USBTxBufSize!=fileSize) || !rdRet )
		{
			if(!rdRet)
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (Ret=%d, L:%d, Err=%u) \n"), rdRet, __LINE__ , GetLastError());
			}
			else
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Err=%u) \n"), fileSize, __LINE__ , GetLastError());
			}

			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return;
		}
		CloseHandle(hFile);

		tmpTxBuf = USBtxBuf+7+4+4+4+4+MAX_PATH;

		iUSBTxBuf=0;
		
		*((BYTE *)USBtxBuf+0)=0x4E;
		*((BYTE *)USBtxBuf+1)=0x30;
		*((BYTE *)USBtxBuf+2)=0x30;
		*((BYTE *)USBtxBuf+3)=0x30;
		*((BYTE *)USBtxBuf+4)=0x46;
		*((BYTE *)USBtxBuf+5)=0x46;
		*((BYTE *)USBtxBuf+6)=0xa;

		tmpTxBuf = USBtxBuf + 7 + 4 + 4;

		*((DWORD *)tmpTxBuf+0)=0;				//nb0 start address == 0
		*((DWORD *)tmpTxBuf+1)=fileSize;		//nb0 filesize



		strcpy((char *)(tmpTxBuf+8), szTitleName);

		tmpTxBuf = USBtxBuf+7;
		nCheckSum = 0;
		for ( i = 0; i < 4+4+MAX_PATH; i++ )
		{
			nCheckSum += (unsigned char)(*(tmpTxBuf+i));
		}

		*((DWORD *)tmpTxBuf+0)=nCheckSum;		//checksum
		*((DWORD *)tmpTxBuf+1)=nFileNumber;   //file number = 1

		szOutFileName = (char *)malloc(OUTFILENAME_LENGTH);
		memset(szOutFileName, 0x00, OUTFILENAME_LENGTH ); /// added 2012.02.21

		strcpy(szOutFileName, szDirName);
		strcat(szOutFileName, TEXT("ubootimage.ubi"));

		USBTxBufSize += (7+4+4+4+4+MAX_PATH);

		hOutFile = CreateFile(szOutFileName,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);
		if( INVALID_HANDLE_VALUE==hOutFile || NULL==hOutFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s L:%d, Err=%u) \n"), szOutFileName, __LINE__ , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return;
		}

		dwState = WriteFile(hOutFile, (void *)(USBtxBuf), USBTxBufSize, &fileBufSize,NULL);
		if( !dwState ) /// added 2011.11.29
		{
			EB_Printf("[dnw] can not write USB Tx buffer! (size:%d, L:%d, Err=%u) \n", USBTxBufSize, __LINE__ , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			return;
		}

		if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
		if(szOutFileName) { free((void*)szOutFileName);  szOutFileName=NULL; } 

		threadResult=_beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  /// 2012.02.20, update.....	2011.12.05

			//Create the download progress dialogbox.
			CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
			//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
    		//isn't needed because the dialog box already has WS_VISIBLE attribute.
		}
		else
		{
			EB_Printf("[dnw] can not create progress thread! (Result=%d, L:%d, Err=%u) \n", threadResult, __LINE__ , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			return;
		}
	}
	else
	{
		EB_Printf("[dnw] can not support file format in MenuUBOOTHistory! (file:%s, L:%d, Err=%u)  \n", szTempFileName, __LINE__ , GetLastError() );
		if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } /// added.
		return;
	}
	
}


void MenuUBOOTHistory2(HWND hwnd, int history)
{
	ULONG i, j;
	HANDLE hFile = NULL;
	HANDLE hOutFile = NULL;
	HANDLE hBinFile[MAX_FILE_NUM] = { NULL, };
	char * szBinFileName[MAX_FILE_NUM] = {0,};
	char * szBinFileFullName[MAX_FILE_NUM] = {0,};
	char * szOutFileName = NULL;

	ULONG fileSize;
	uintptr_t threadResult;
	TCHAR	szTempFileName[3] = {0,};
	ULONG nFileNumber;
	unsigned int nCheckSum = 0;
	volatile char * tmpTxBuf = NULL;
	char *lstFileBuf = NULL;
	DWORD lstFileBufSize;

	TCHAR szDirName[FILENAMELEN] = {0,};
	int lastpoint;
	TCHAR *tocken = NULL;
	int cnt=0, idel=0;
	BOOL rdRet=FALSE;
	DWORD dwState;


	if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } /// added.

	if( autoSendKey ) // TRUE == ~~~~
	{ 
		switch( RetryUSB )
		{
			case 1: /// IDC_RETRY_USB_20_TIMES
				dnUSBcount = 20; 
				break;
			case 2: /// IDC_RETRY_USB_30_TIMES
				dnUSBcount = 30; 
				break;
			case 3: /// IDC_RETRY_USB_40_TIMES
				dnUSBcount = 40; 
				break;
			case 0: /// IDC_RETRY_USB_DEFAULT
			default:
				dnUSBcount = 1; 
				break;
		}
	} /// 2012.03.06
	else { dnUSBcount = 1; } /// 2012.03.06

	cnt = 0;
	do {
		Sleep(USB_DELAY_TIME);
		hWrite = open_file( outPipe, cnt, UBOOT_OTHER  );
		if( cnt++ > USB_OPEN_RETRY*dnUSBcount ) break;
	} while( (INVALID_HANDLE_VALUE==hWrite) || (NULL==hWrite) );

	if( (INVALID_HANDLE_VALUE==hWrite) || (NULL==hWrite) )
	{
		MessageBox(hwnd,TEXT("Can't open USB device.\n"),TEXT("Error"), MB_OK | MB_ICONINFORMATION );
		return;
	}

//    result=PopFileOpenDlg(hwnd,szUBOOTFileName,szUBOOTTitleName);
//	tocken = strtok(,",");

	memcpy(szFileName, szDownloadString[history], strlen(szDownloadString[history]));
	szFileName[strlen(szDownloadString[history])] = '\0';
	for ( i = 0; i < FILENAMELEN; i++ )
	{
		if ( szDownloadString[history][i] == '\\' )
		{
			lastpoint = i;
			tocken = &szDownloadString[history][i+1];
		}
		if ( szDownloadString[history][i] == '\0' )
			break;
	}
	memcpy(szTitleName, tocken, FILENAMELEN-i);

	strncpy ( szDirName, szFileName, strlen(szFileName) - strlen(szTitleName) );
	szDirName[strlen(szFileName) - strlen(szTitleName)] = 0;
    
//    if(result==0) //file open fail
//    {
//		EB_Printf(TEXT("[dnw] [ERROR:File Open](%s)\n"), szFileName);
//		CloseHandle(hWrite);
//		return;
//    }

	AddUbootMenu2(hwnd, szFileName);

	szTempFileName[0] = tolower(szFileName[strlen(szFileName)-3]);
	szTempFileName[1] = tolower(szFileName[strlen(szFileName)-2]);
	szTempFileName[2] = tolower(szFileName[strlen(szFileName)-1]);
	if ( !strncmp(szTempFileName,_T("lst"),3) )
	{
		hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u)\n"), szFileName , __LINE__ , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}
		fileSize=GetFileSize(hFile,NULL);

		lstFileBuf=(char *)malloc(fileSize+1);
		if(NULL == lstFileBuf)
		{
			EB_Printf(TEXT("[dnw] can not allocate memory in lst file. (size:%d, L:%d, Err=%u) \n"),fileSize, __LINE__ , GetLastError() );
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}

		// Read chain.lst file
		rdRet = ReadFile(hFile,(void *)(lstFileBuf),fileSize,&lstFileBufSize,NULL);
		if( !rdRet ) /// added.
		{
			EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Err=%u) \n"), fileSize, __LINE__ , GetLastError());

			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(lstFileBuf) { free(lstFileBuf); lstFileBuf=NULL; }
			return;
		}
		lstFileBuf[lstFileBufSize] = 0;
		if(hFile) CloseHandle(hFile);

		szBinFileName[0] = strtok(lstFileBuf, "\t\r\n+");
		szBinFileFullName[0] = (char *)malloc(strlen(szDirName)+strlen(szBinFileName[0])+1);
		strcpy(szBinFileFullName[0], szDirName);
		strcat(szBinFileFullName[0], szBinFileName[0]);
		for ( nFileNumber = 1; nFileNumber < MAX_FILE_NUM; nFileNumber++ )
		{
			szBinFileName[nFileNumber] = strtok(NULL, "\r\n+");
			if ( szBinFileName[nFileNumber] == NULL ) break;
			szBinFileFullName[nFileNumber] = (char *)malloc(strlen(szDirName)+strlen(szBinFileName[nFileNumber])+1);
			strcpy(szBinFileFullName[nFileNumber], szDirName);
			strcat(szBinFileFullName[nFileNumber], szBinFileName[nFileNumber]);
		}

		iUSBTxBuf=0;
		
		USBTxBufSize = 7 /* X000FF\n */
					+ 4 /* check sum */
					+ 4 /* num Regions */
					+ nFileNumber*(8+260); /* start address + length + filename */

		for ( i = 0; i < nFileNumber; i ++ )
		{
			hBinFile[i] = CreateFile(szBinFileFullName[i],GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
			if( INVALID_HANDLE_VALUE==hBinFile[i] || NULL==hBinFile[i] )
			{
				EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u) \n"), szBinFileFullName[i], __LINE__  , GetLastError() );
				if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
				if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
				if(lstFileBuf) { free( (void*)lstFileBuf );  lstFileBuf=NULL; }
				
				for(idel=0; idel<MAX_FILE_NUM; idel++) /// added.
				{
					if(szBinFileFullName[idel]) { free((void*)szBinFileFullName[idel]); szBinFileFullName[idel]=NULL; }
				}
				return;
			}
			USBTxBufSize += GetFileSize(hBinFile[i],NULL);
		}

		if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
		USBtxBuf=(char *)malloc(USBTxBufSize);

		*((BYTE *)USBtxBuf+0)=0x4E;
		*((BYTE *)USBtxBuf+1)=0x30;
		*((BYTE *)USBtxBuf+2)=0x30;
		*((BYTE *)USBtxBuf+3)=0x30;
		*((BYTE *)USBtxBuf+4)=0x46;
		*((BYTE *)USBtxBuf+5)=0x46;
		*((BYTE *)USBtxBuf+6)=0xa;

		tmpTxBuf = USBtxBuf  + 7 /* X000FF\n */
						+ 4 /* check sum */
						+ 4 /* num Regions */
						+ nFileNumber*(8+260); /* start address + length */

		for ( i = 0; i < nFileNumber; i++ )
		{
			fileSize = GetFileSize(hBinFile[i],NULL);
			rdRet = ReadFile(hBinFile[i],(void *)(tmpTxBuf),fileSize,&fileBufSize,NULL);
			if( (fileBufSize!=fileSize) || !rdRet )
			{
				if(!rdRet)
				{
					EB_Printf(TEXT("[dnw] find error of ReadFile. (Ret=%d, L:%d, Err=%u) \n"), rdRet, __LINE__ , GetLastError());
				}
				else
				{
					EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Err=%u) \n"), fileSize, __LINE__ , GetLastError() );
				}

				if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
				if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
				if(lstFileBuf) { free( (void*)lstFileBuf ); lstFileBuf=NULL; }

				for(idel=0; idel<MAX_FILE_NUM; idel++) /// added.
				{
					if(szBinFileFullName[idel]) { free((void*)szBinFileFullName[idel]); szBinFileFullName[idel]=NULL; }
				}
				if(hBinFile[i]) { CloseHandle(hBinFile[i]); hBinFile[i]=NULL; }
				return;
			}

			for ( j = 0; j < 8 + 260; j++ )
			{
				if ( j < 8 )
				{
					*((BYTE *)USBtxBuf+15+(i*(8+260)+j))=(BYTE)(tmpTxBuf[7+j]);
					nCheckSum += (BYTE)(tmpTxBuf[7+j]);
				}
				else if ( j >= 8 && j < (8+strlen(szBinFileName[i])) )
				{
					*((BYTE *)USBtxBuf+15+(i*(8+260)+j))=(BYTE)(szBinFileName[i][j-8]);
					nCheckSum += (BYTE)(szBinFileName[i][j-8]);
				}
				else
				{
					*((BYTE *)USBtxBuf+15+(i*(8+260)+j))=0;
				}
			}

			tmpTxBuf += fileSize;

			CloseHandle(hBinFile[i]);
		}

		tmpTxBuf = USBtxBuf + 7;

		*((DWORD *)tmpTxBuf+0)=nCheckSum;   //checksum
		*((DWORD *)tmpTxBuf+1)=nFileNumber;   //checksum

		szOutFileName = (char *)malloc(OUTFILENAME_LENGTH);
		memset(szOutFileName, 0x00, OUTFILENAME_LENGTH ); /// added 2012.02.21

		strcpy(szOutFileName, szDirName);
		strcat(szOutFileName, TEXT("ubootimage.ubi"));

		hOutFile = CreateFile(szOutFileName,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);
		if( INVALID_HANDLE_VALUE==hOutFile || NULL==hOutFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u) \n"), szOutFileName, __LINE__ , GetLastError() );
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(lstFileBuf) { free( (void*)lstFileBuf ); lstFileBuf=NULL; }
			return;
		}

		dwState = WriteFile(hOutFile, (void *)(USBtxBuf), USBTxBufSize, &fileBufSize,NULL);
		if( !dwState ) /// added 2011.11.29
		{
			EB_Printf("[dnw] can not write USB Tx buffer! (size:%d, L:%d, Err=%u) \n", USBTxBufSize, __LINE__ , GetLastError() );
			if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(lstFileBuf) { free( (void*)lstFileBuf ); lstFileBuf=NULL; }
			return; /// added 2012.02.21
		}

		if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
		if(szOutFileName) { free((void*)szOutFileName);  szOutFileName=NULL; } 

		threadResult=_beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  /// 2012.02.20, update.....	2011.12.05

			//Create the download progress dialogbox.
			CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
			//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
    		//isn't needed because the dialog box already has WS_VISIBLE attribute.
		}
		else
		{
			EB_Printf("[dnw] can not create progress thread! (Result=%d, L:%d, Err=%u) \n", threadResult, __LINE__ , GetLastError() );
			if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(lstFileBuf) { free( (void*)lstFileBuf ); lstFileBuf=NULL; }
			return; /// added 2012.02.21
		}
	}
	else if ( !strncmp(szTempFileName,_T("ubi"),3) )
	{
		hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u)\n"), szFileName , __LINE__ , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}
		fileSize=GetFileSize(hFile,NULL);

		if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
		USBtxBuf=(char *)malloc(fileSize);
		if(NULL==USBtxBuf)
		{
			EB_Printf(TEXT("[dnw] can not allocate USB Tx Buffer memory. (L:%d, Err=%u) \n"),  __LINE__ , GetLastError() );
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}

		// Read ubootimag.ubi file
		rdRet = ReadFile(hFile,(void *)(USBtxBuf),fileSize,&USBTxBufSize,NULL);
		if( (USBTxBufSize!=fileSize) || !rdRet )
		{
			if(!rdRet)
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (Ret=%d, L:%d, Err=%u) \n"), rdRet, __LINE__ , GetLastError() );
			}
			else
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Err=%u) \n"), fileSize, __LINE__ , GetLastError() );
			}

			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return;
		}
		if(hFile) CloseHandle(hFile);

		iUSBTxBuf=0;

		threadResult=_beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  /// 2012.02.20, update.....	2011.12.05

			//Create the download progress dialogbox.
			CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
			//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
    		//isn't needed because the dialog box already has WS_VISIBLE attribute.
		}
		else
		{
			EB_Printf("[dnw] can not create progress thread! (Result=%d, L:%d, Err=%u) \n", threadResult, __LINE__ , GetLastError() );

			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return; /// added 2012.02.21
		}
	}
	else if ( !strncmp(szTempFileName,_T("bin"),3) )
	{
		hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u) \n"), szFileName , __LINE__ , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}
		fileSize=GetFileSize(hFile,NULL);

		if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
		USBtxBuf=(char *)malloc(fileSize);
		if(NULL==USBtxBuf)
		{
			EB_Printf(TEXT("[dnw] can not allocate USB Tx Buffer memory. (L:%d, Err=%u) \n"),  __LINE__ , GetLastError() );
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}

		// Read ubootimag.ubi file
		rdRet = ReadFile(hFile,(void *)(USBtxBuf),fileSize,&USBTxBufSize,NULL);
		if( (USBTxBufSize!=fileSize) || !rdRet )
		{
			if(!rdRet)
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (Ret=%d, L:%d, Err=%u) \n"), rdRet, __LINE__ , GetLastError() );
			}
			else
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Err=%u) \n"), fileSize, __LINE__ , GetLastError() );
			}

			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return;
		}
		if(hFile) CloseHandle(hFile);

		iUSBTxBuf=0;

		threadResult=_beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  /// 2012.02.20, update.....	2011.12.05

			//Create the download progress dialogbox.
			CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
			//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
    		//isn't needed because the dialog box already has WS_VISIBLE attribute.
		}
		else
		{
			EB_Printf("[dnw] can not create progress thread! (Result=%d, L:%d, Err=%u) \n", threadResult, __LINE__ , GetLastError() );

			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite=NULL; } /// added.
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return; /// added 2012.02.21
		}
	}
	else if ( !strncmp(szTempFileName,_T("nb0"),3) || !strncmp(szTempFileName,_T("dio"),3) )
	{
		nFileNumber = 1;

		hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u) \n"), szFileName , __LINE__ , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return;
		}
		fileSize=GetFileSize(hFile,NULL);

		if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
		USBtxBuf=(char *)malloc(fileSize+7+4+4+4+4+MAX_PATH);
		if(NULL==USBtxBuf)
		{
			EB_Printf(TEXT("[dnw] can not allocate USB Tx Buffer memory. (L:%d, Err=%u) \n"),  __LINE__ , GetLastError() );
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite=NULL; } /// added.
			return;
		}
		memset((void *)USBtxBuf, 0, fileSize+7+4+4+4+4+MAX_PATH);

		// Read nb0 file
		rdRet = ReadFile(hFile,(void *)(USBtxBuf+7+4+4+4+4+MAX_PATH),fileSize,&USBTxBufSize,NULL);
		if( (USBTxBufSize!=fileSize) || !rdRet )
		{
			if(!rdRet)
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (Ret=%d, L:%d, Err=%u) \n"), rdRet, __LINE__ , GetLastError() );
			}
			else
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Err=%u) \n"), fileSize, __LINE__ , GetLastError() );
			}

			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return;
		}
		CloseHandle(hFile);

		tmpTxBuf = USBtxBuf+7+4+4+4+4+MAX_PATH;

		iUSBTxBuf=0;
		
		*((BYTE *)USBtxBuf+0)=0x4E;
		*((BYTE *)USBtxBuf+1)=0x30;
		*((BYTE *)USBtxBuf+2)=0x30;
		*((BYTE *)USBtxBuf+3)=0x30;
		*((BYTE *)USBtxBuf+4)=0x46;
		*((BYTE *)USBtxBuf+5)=0x46;
		*((BYTE *)USBtxBuf+6)=0xa;

		tmpTxBuf = USBtxBuf + 7 + 4 + 4;

		*((DWORD *)tmpTxBuf+0)=0;				//nb0 start address == 0
		*((DWORD *)tmpTxBuf+1)=fileSize;		//nb0 filesize



		strcpy((char *)(tmpTxBuf+8), szTitleName);

		tmpTxBuf = USBtxBuf+7;
		nCheckSum = 0;
		for ( i = 0; i < 4+4+MAX_PATH; i++ )
		{
			nCheckSum += (unsigned char)(*(tmpTxBuf+i));
		}

		*((DWORD *)tmpTxBuf+0)=nCheckSum;		//checksum
		*((DWORD *)tmpTxBuf+1)=nFileNumber;   //file number = 1

		szOutFileName = (char *)malloc(OUTFILENAME_LENGTH);
		memset(szOutFileName, 0x00, OUTFILENAME_LENGTH ); /// added 2012.02.21

		strcpy(szOutFileName, szDirName);
		strcat(szOutFileName, TEXT("ubootimage.ubi"));

		USBTxBufSize += (7+4+4+4+4+MAX_PATH);

		hOutFile = CreateFile(szOutFileName,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);
		if( INVALID_HANDLE_VALUE==hOutFile || NULL==hOutFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s L:%d, Err=%u) \n"), szOutFileName, __LINE__ , GetLastError());
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			return;
		}

		dwState = WriteFile(hOutFile, (void *)(USBtxBuf), USBTxBufSize, &fileBufSize,NULL);
		if( !dwState ) /// added 2011.11.29
		{
			EB_Printf("[dnw] can not write USB Tx buffer! (size:%d, L:%d) \n", USBTxBufSize, __LINE__ );
			if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			return; /// added 2012.02.21
		}

		if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
		if(szOutFileName) { free((void*)szOutFileName);  szOutFileName=NULL; } 

		threadResult=_beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  /// 2012.02.20, update.....	2011.12.05

			//Create the download progress dialogbox.
			CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
			//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
    		//isn't needed because the dialog box already has WS_VISIBLE attribute.
		}
		else
		{
			EB_Printf("[dnw] can not create progress thread! (Result=%d, L:%d, Err=%u) \n", threadResult, __LINE__ , GetLastError() );
			if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return; /// added 2012.02.21
		}
	}
	else
	{
		EB_Printf("[dnw] can not support file format in MenuUBOOTHistory2! (file:%s, L:%d, Err=%u)  \n", szTempFileName, __LINE__ , GetLastError() );
		if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } /// added.
		return;
	}

}


#define USB_OVERLAPPED_IO FALSE  //should be FALSE becasue USB overlapped IO doesn't work well.

//#define USB_TX_SIZE (4096*4*16)
//#define USB_RX_SIZE (4096*4*16) 
//#define TX_SIZE (4096*4)
//#define RX_SIZE (4096*4) 

#ifdef _NOT_USED__
VOID CALLBACK UsbWriteCompleteRoutine(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
    //nothing to do
}
#endif



void UsbTxFile(void *args)
{
	void *txBlk = NULL;
	ULONG txBlkSize;
	DWORD nBytesWrite;
	DWORD txBufSize100;
	DWORD dwState; /// added 2011.11.29

#if USB_OVERLAPPED_IO
	//NOTE: hWrite should have FILE_FLAG_OVERLAPPED attribute.
	OVERLAPPED os;

	memset(&os,0,sizeof(OVERLAPPED));

	os.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	if( (INVALID_HANDLE_VALUE == os.hEvent) || (NULL==os.hEvent) )
	{
		if(USBtxBuf) { free((void *)USBtxBuf);  USBtxBuf = NULL; }
		if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
		_endthread();

		EB_Printf(TEXT("[dnw] can not create event in USBTxFile! (L:%d, Err=%u) \n"), __LINE__ , GetLastError() );

		return; // 2015.12.02
	}
#endif


	InitDownloadProgress();

	txBufSize100=USBTxBufSize/100;
	if(txBufSize100==0)txBufSize100=1;
	while(1)
	{
		if((USBTxBufSize-iUSBTxBuf) > USB_TX_SIZE)
			txBlkSize=USB_TX_SIZE;
		else
			txBlkSize=USBTxBufSize-iUSBTxBuf;
	
		txBlk=(void *)(USBtxBuf+iUSBTxBuf);

		bUSBTxFile = TRUE; /// added.

    #if USB_OVERLAPPED_IO
		dwState = WriteFile(hWrite,txBlk,txBlkSize,&nBytesWrite,&os);

	#if 0 /// def IS_OVERLAPPED_CHECK /// 2014.03.04 ----
		if( HasOverlappedIoCompleted(&os) )
		{
			/// EB_Printf(TEXT("Completed") );
		}
		else
	#endif /// -------------------------

		//...
		if( !dwState ) /// added 2011.11.29
		{
			if(GetLastError()==ERROR_IO_PENDING)
			{

		#if 0 /// def IS_OVERLAPPED_CHECK /// 2014.03.04 ----
				// I/O가 완료될 때 가지 기다린다.
				WaitForSingleObject(os.hEvent, IS_OVERLAPPED_WAIT);
		#endif /// -----------------------------
	
				if( ! GetOverlappedResult(hWrite,&os,&nBytesWrite,FALSE) )
				{
					//more efficient in order to save the cpu time
					if(ERROR_IO_INCOMPLETE != GetLastError() )  /// 2011.11.29
					{
						memset(&comStat, 0x00, sizeof(COMSTAT) );
						ClearCommError(hWrite, &dwErrorFlags, &comStat);
					}
				}
		#if 1 /// added 2012.07.04
				else
				{
					memset(&comStat, 0x00, sizeof(COMSTAT) );
					ClearCommError(hWrite, &dwErrorFlags, &comStat);
				}
		#endif

			}
		}

		//...   
		//Why doesn't work GetOverlappedResult().
		//It may seem that secbulk.sys doesn't support OverlappedIO.
    #else
		dwState = WriteFile(hWrite,txBlk,txBlkSize,&nBytesWrite,NULL);
		if( !dwState ) /// added 2011.11.29
		{
			SendMessage(_hDlgDownloadProgress,WM_CLOSE,0,0);  /// added 2012.02.22
			/// _hDlgDownloadProgress=NULL;	/// added 2012.02.22

			EB_Printf("[dnw] find error for USBTx txBuffer! (txBlkSize=%d, Err=%u) \n", txBlkSize, GetLastError());
			break; /// added 2012.02.17
		}
    #endif

		//assert(nBytesWrite == WriteLen);

		iUSBTxBuf+=USB_TX_SIZE;
		
		if( ((iUSBTxBuf/txBufSize100)%2)==1 )
		{
		    DisplayDownloadProgress(iUSBTxBuf/txBufSize100);
		}

		if(downloadCanceled==1)break;	//download is canceled by user.

		if(iUSBTxBuf>=USBTxBufSize)break;


	}

	CloseDownloadProgress();

	if(USBtxBuf) { free((void *)USBtxBuf); USBtxBuf=NULL; }

	if(hWrite) { CloseHandle(hWrite);  hWrite=NULL; }

	iUSBTxBuf = 0;
	USBTxBufSize = 0;
	bUSBTxFile = FALSE; /// 2010.05.25 added


#if USB_OVERLAPPED_IO    
	CloseHandle(os);
	if(os.hEvent) CloseHandle(os.hEvent); /// added.
#endif

	_endthread();

}


/*
void UsbTxFile(void *args)
{
    void *txBlk;
    ULONG txBlkSize;
    DWORD nBytesWrite;
    DWORD txBufSize100;

#if USB_OVERLAPPED_IO
    //NOTE: hWrite should have FILE_FLAG_OVERLAPPED attribute.
    OVERLAPPED os;
    memset(&os,0,sizeof(OVERLAPPED));

    os.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
    if(os.hEvent==NULL)
    {
	EB_Printf(TEXT("[ERROR:DoRxTx os.hEvent]\n"));
        free((void *)USBtxBuf);
        CloseHandle(hWrite);    
	_endthread();
    }
#endif
       
    InitDownloadProgress();

    txBufSize100=USBTxBufSize/100;
    if(txBufSize100==0)txBufSize100=1;
    while(1)
    {
        if((USBTxBufSize-iUSBTxBuf) > TX_SIZE)
	    txBlkSize=TX_SIZE;
	else
	    txBlkSize=USBTxBufSize-iUSBTxBuf;
	
	txBlk=(void *)(USBtxBuf+iUSBTxBuf);
    #if USB_OVERLAPPED_IO
	WriteFile(hWrite,txBlk,txBlkSize,&nBytesWrite,&os);
        //...
	GetOverlappedResult(hWrite,&os,&temp,FALSE); 	
	//...   
	//Why doesn't work GetOverlappedResult().
	//It may seem that secbulk.sys doesn't support OverlappedIO.
    #else
	WriteFile(hWrite,txBlk,txBlkSize,&nBytesWrite,NULL);
    #endif

	//assert(nBytesWrite == WriteLen);

	iUSBTxBuf+=TX_SIZE;
	
	if( ((iUSBTxBuf/txBufSize100)%2)==1 )
	    DisplayDownloadProgress(iUSBTxBuf/txBufSize100);
	
	if(downloadCanceled==1)break;	//download is canceled by user.

	if(iUSBTxBuf>=USBTxBufSize)break;
    }

    CloseDownloadProgress();

    free((void *)USBtxBuf);

    CloseHandle(hWrite);    

#if USB_OVERLAPPED_IO    
    CloseHandle(os);    
#endif

    _endthread();
}
*/



void MenuUsbReceive(HWND hwnd)
{
/*  int nBytesRead, nBytes;
    ULONG i, j;    
    UINT success;


    if (fRead)
    {
	//ReadLen=64;
        hRead = open_file( inPipe);
	pinBuf = malloc(ReadLen);

        if (pinBuf) 
	{
	    success = ReadFile(hRead,
	    	              pinBuf,
                              ReadLen,
	                      &nBytesRead,
	                      NULL);
            printf("<%s> R (%04.4d) : request %06.6d bytes -- %06.6d bytes read\n",
                    inPipe, i, ReadLen, nBytesRead);
        }
	if (pinBuf)
	{
	    free(pinBuf);
	}
		
	// close devices if needed
	if(hRead != INVALID_HANDLE_VALUE)
	    CloseHandle(hRead);
    }
*/

#define RX_BUF_LENGTH 			128

	ULONG i;    
	UINT success;
	UINT rxLength=RX_BUF_LENGTH;
	ULONG nBytesRead;
	char *USBRxBuf = NULL;
	int cnt = 0;

	
	if(hRead) { CloseHandle(hRead); hRead = NULL; } /// 2010.5.10 added.

	if( autoSendKey ) // TRUE == ~~~~
	{ 
		switch( RetryUSB )
		{
			case 1: /// IDC_RETRY_USB_20_TIMES
				dnUSBcount = 20; 
				break;
			case 2: /// IDC_RETRY_USB_30_TIMES
				dnUSBcount = 30; 
				break;
			case 3: /// IDC_RETRY_USB_40_TIMES
				dnUSBcount = 40; 
				break;
			case 0: /// IDC_RETRY_USB_DEFAULT
			default:
				dnUSBcount = 1; 
				break;
		}
	} /// 2012.03.06
	else { dnUSBcount = 1; } /// 2012.03.06

#if 0 /// LSI original
	hRead = open_file(inPipe);
#else

	cnt=0;
	do {
		Sleep(USB_DELAY_TIME);
		hRead = open_file( inPipe, cnt, UBOOT_OTHER  );
		if( cnt++ > USB_OPEN_RETRY ) break;
	} while( INVALID_HANDLE_VALUE==hRead || NULL==hRead );

	if( INVALID_HANDLE_VALUE==hRead || NULL==hRead )
	{
		if (USBRxBuf) { free(USBRxBuf); USBRxBuf=NULL; }
		MessageBox(hwnd,TEXT("Can not open USB port. Check USB port!!\n"),TEXT("USB Error"), MB_OK | MB_ICONINFORMATION );
		return;
	}
#endif




	USBRxBuf = (char *)malloc(rxLength);
	if( NULL==USBRxBuf ) 
	{
		EB_Printf("[dnw] MenuUsbReceive: USBRxBuf is NULL!! Error(%u) \r\n", GetLastError() );
		return; 
	}


	EB_Printf("[dnw] =============== USB Receive Test ==============\n");
	memset(USBRxBuf, 0x00, sizeof(UINT)*RX_BUF_LENGTH ); //// added.

	success = ReadFile(hRead, USBRxBuf, rxLength, &nBytesRead, NULL);
	EB_Printf("[dnw] ====== rxLength=%d, BytesRead = %u, success=%d \n", rxLength, nBytesRead, success);
	if(success)
	{
		for(i=0;i<rxLength;i++)
		{
			EB_Printf("0x%02X ",USBRxBuf[i]);
			if(i%16==15)EB_Printf("\n");
		}
	}
	else
	{
		EB_Printf("[dnw] Error: can't receive data from the USBD. Err=%u \n",  GetLastError() );
	}

#if 0
	//This part is only for S3C2410 USB signal quality test
    EB_Printf("============ 4096*128byte Read Test ===========\n");

    for(i=0;i<4096;i++)
    {
    	success = ReadFile(hRead,
     	               USBRxBuf,
                       rxLength,
	               &nBytesRead,
	               NULL);
    	if(success)
    	{
	    EB_Printf("%2x,",(unsigned char)USBRxBuf[0]);
    	}
    	else
    	{
	    EB_Printf("Er,");
    	}
    	if(i%16==15)EB_Printf("\n");
    }
#endif   

	EB_Printf("[dnw] ===============================================\n");

	if(USBRxBuf) { free(USBRxBuf); USBRxBuf=NULL; }

	// close devices if needed
	if(hRead) { CloseHandle(hRead); hRead = NULL; }
}


int IsUsbConnected(void)
{
	HANDLE hDEV = open_dev();

	if( (INVALID_HANDLE_VALUE==hDEV) || (NULL==hDEV) )
	{
		return 0; /* FAIL */
	}
	else
	{
		CloseHandle(hDEV);
		hDEV = NULL;
		return 1; /* OK */
	}
}


// F7, F8, F9
WORD ROM_MenuUBOOT2(HWND hwnd, int Fileindex, BYTE funcKey)
{
	ULONG i, j;
	HANDLE hFile = NULL;
	HANDLE hOutFile = NULL;
	HANDLE hBinFile[MAX_FILE_NUM] = { NULL, };
	char * szBinFileName[MAX_FILE_NUM] = {0,};
	char * szBinFileFullName[MAX_FILE_NUM] = {0,};
	char * szOutFileName = NULL;

	ULONG fileSize;
	uintptr_t threadResult;
	TCHAR	szTempFileName[3] = {0,};
	ULONG nFileNumber;
	unsigned int nCheckSum = 0;
	volatile char * tmpTxBuf = NULL;
	char *lstFileBuf = NULL;
	DWORD lstFileBufSize;
	TCHAR szDirName[FILENAMELEN] = {0,};
	char tmpFileName[FILENAMELEN] = {0,};
	WORD  MaxLen=0;
	int   idel=0;
	int   cnt=0;
	BOOL rdRet=FALSE;
	DWORD dwState;

	if(hWrite) { CloseHandle(hWrite);  hWrite = NULL; } /// added.

	if( autoSendKey ) // TRUE == ~~~~
	{ 
		switch( RetryUSB )
		{
			case 1: /// IDC_RETRY_USB_20_TIMES
				dnUSBcount = 20; 
				break;
			case 2: /// IDC_RETRY_USB_30_TIMES
				dnUSBcount = 30; 
				break;
			case 3: /// IDC_RETRY_USB_40_TIMES
				dnUSBcount = 40; 
				break;
			case 0: /// IDC_RETRY_USB_DEFAULT
			default:
				dnUSBcount = 1; 
				break;
		}
	} /// 2012.03.06
	else { dnUSBcount = 1; } /// 2012.03.06

	cnt = 0;
	do {
		Sleep(USB_DELAY_TIME);
		hWrite = open_file( outPipe, cnt, funcKey );
		if( cnt++ > USB_OPEN_RETRY*dnUSBcount ) { break; } /* NOTE : <-- retry count fix while automatic download */
	} while( (INVALID_HANDLE_VALUE==hWrite) || (NULL==hWrite) );

	
	if( (INVALID_HANDLE_VALUE==hWrite) || (NULL==hWrite) )
	{

	#if 0 // 2016.03.25
		DWORD error_code = GetLastError();
		switch(error_code)
		{
			case ERROR_INVALID_HANDLE: // 6 (0x6)
				EB_Printf("[dnw] can not open USB device in ROM_UBOOT2. The handle is invalid. \r\n");
				break;
			default:
				EB_Printf("[dnw] can not open USB device in ROM_UBOOT2. Check USB port! (retry:%d, Err=%u) \r\n", dnUSBcount, error_code );
				break;
		}
	#endif

		//BinFileIndex = 0;
		return 0;
	}

	//EB_Printf(TEXT("\nROM File = %s [%s]"), szQuickDownloadString[Fileindex], szQuickFileName[Fileindex] );
	//EB_Printf(TEXT("\r\n[dnw] >> ROM File => %s... Waiting.... \n"), szQuickDownloadString[Fileindex]);

	memset(szFileName, 0x00, sizeof(szFileName));
	memset(szTitleName, 0x00, sizeof(szTitleName));

	memcpy(szFileName, szQuickDownloadString[Fileindex], FILENAMELEN);
	memcpy(szTitleName, szQuickFileName[Fileindex], FILENAMELEN);

	strncpy ( szDirName, szFileName, strlen(szFileName) - strlen(szTitleName) );
	szDirName[strlen(szFileName) - strlen(szTitleName)] = 0;
    
	if(NULL == szQuickDownloadString[Fileindex] || NULL == szQuickFileName[Fileindex] ) //file open fail
	{

		MessageBox(hwnd,TEXT("File Not found.. check ini file\n"),TEXT("Error"), MB_OK | MB_ICONINFORMATION );
		if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } 
		return 0;
	}

	///////////AddUbootMenu2(hwnd, szFileName);

	szTempFileName[0] = tolower(szFileName[strlen(szFileName)-3]);
	szTempFileName[1] = tolower(szFileName[strlen(szFileName)-2]);
	szTempFileName[2] = tolower(szFileName[strlen(szFileName)-1]);
	if ( !strncmp(szTempFileName,_T("lst"),3) )
	{

		hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u) \n"), szFileName , __LINE__ , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } 
			return 0;
		}
		fileSize=GetFileSize(hFile,NULL);

		lstFileBuf=(char *)malloc(fileSize+1);
		if(NULL == lstFileBuf)
		{
			EB_Printf(TEXT("[dnw] can not allocate memory in lst file. (size:%d, L:%d, Err=%u) \n"),fileSize, __LINE__ , GetLastError() );
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } 
			return 0;
		}

		// Read chain.lst file
		rdRet = ReadFile(hFile,(void *)(lstFileBuf),fileSize,&lstFileBufSize,NULL);
		if( !rdRet ) /// added.
		{
			EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Err=%u) \n"), fileSize, __LINE__ , GetLastError() );

			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(lstFileBuf) { free(lstFileBuf); lstFileBuf=NULL; }
			return 0;
		}
		lstFileBuf[lstFileBufSize] = 0;
		if(hFile) CloseHandle(hFile);

		szBinFileName[0] = strtok(lstFileBuf, "\t\r\n+");
		szBinFileFullName[0] = (char *)malloc(strlen(szDirName)+strlen(szBinFileName[0])+1);
		strcpy(szBinFileFullName[0], szDirName);
		strcat(szBinFileFullName[0], szBinFileName[0]);
		for ( nFileNumber = 1; nFileNumber < MAX_FILE_NUM; nFileNumber++ )
		{
			szBinFileName[nFileNumber] = strtok(NULL, "\r\n+");
			if ( szBinFileName[nFileNumber] == NULL ) break;
			szBinFileFullName[nFileNumber] = (char *)malloc(strlen(szDirName)+strlen(szBinFileName[nFileNumber])+1);
			strcpy(szBinFileFullName[nFileNumber], szDirName);
			strcat(szBinFileFullName[nFileNumber], szBinFileName[nFileNumber]);
		}

		iUSBTxBuf=0;
		
		USBTxBufSize = 7 /* X000FF\n */
					+ 4 /* check sum */
					+ 4 /* num Regions */
					+ nFileNumber*(4+4+MAX_PATH); /* start address + length + filename */

		for ( i = 0; i < nFileNumber; i ++ )
		{
			hBinFile[i] = CreateFile(szBinFileFullName[i],GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
			if( INVALID_HANDLE_VALUE==hBinFile[i] || NULL==hBinFile[i] )
			{
				EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u) \n"), szBinFileFullName[i] , __LINE__ , GetLastError() );
				if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } 
				if(lstFileBuf) { free( (void*)lstFileBuf ); lstFileBuf=NULL; } 
				for(idel=0; idel<MAX_FILE_NUM; idel++) /// added.
				{
					if(szBinFileFullName[idel]) { free((void*)szBinFileFullName[idel]); szBinFileFullName[idel]=NULL; }
				}
				return 0;
			}
			USBTxBufSize += GetFileSize(hBinFile[i],NULL);
		}

		if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
		USBtxBuf=(char *)malloc(USBTxBufSize);

		*((BYTE *)USBtxBuf+0)=0x4E;
		*((BYTE *)USBtxBuf+1)=0x30;
		*((BYTE *)USBtxBuf+2)=0x30;
		*((BYTE *)USBtxBuf+3)=0x30;
		*((BYTE *)USBtxBuf+4)=0x46;
		*((BYTE *)USBtxBuf+5)=0x46;
		*((BYTE *)USBtxBuf+6)=0xa;

		tmpTxBuf = USBtxBuf  + 7 /* X000FF\n */
						+ 4 /* check sum */
						+ 4 /* num Regions */
						+ nFileNumber*(4+4+MAX_PATH); /* start address + length */

		for ( i = 0; i < nFileNumber; i++ )
		{
			fileSize = GetFileSize(hBinFile[i],NULL);
			rdRet = ReadFile(hBinFile[i],(void *)(tmpTxBuf),fileSize,&fileBufSize,NULL);
			if( (fileBufSize!=fileSize) || !rdRet )
			{
				if(!rdRet)
				{
					EB_Printf(TEXT("[dnw] find error of ReadFile. (Ret=%d, L:%d, Error=%u) \n"), rdRet, __LINE__ , GetLastError());
				}
				else
				{
					EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Err=%u) \n"), fileSize, __LINE__ , GetLastError());
				}

				if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } 
				if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
				if(lstFileBuf) { free( (void*)lstFileBuf ); lstFileBuf=NULL; }

				for(idel=0; idel<MAX_FILE_NUM; idel++) /// added.
				{
					if(szBinFileFullName[idel]) { free((void*)szBinFileFullName[idel]); szBinFileFullName[idel]=NULL; }
				}
				if(hBinFile[i]) { CloseHandle(hBinFile[i]); hBinFile[i]=NULL; }
				return 0;
			}

			for ( j = 0; j < (4+4+MAX_PATH); j++ )
			{
				if ( j < 8 )
				{
					*((BYTE *)USBtxBuf+15+(i*(8+MAX_PATH)+j))=(BYTE)(tmpTxBuf[7+j]);
					nCheckSum += (BYTE)(tmpTxBuf[7+j]);
				}
				else if ( j >= 8 && j < (8+strlen(szBinFileName[i])) )
				{
					*((BYTE *)USBtxBuf+15+(i*(8+MAX_PATH)+j))=(BYTE)(szBinFileName[i][j-8]);
					nCheckSum += (BYTE)(szBinFileName[i][j-8]);
				}
				else
				{
					*((BYTE *)USBtxBuf+15+(i*(8+MAX_PATH)+j))=0;
				}
			}

			tmpTxBuf += fileSize;

			CloseHandle(hBinFile[i]);
		}

		tmpTxBuf = USBtxBuf + 7;

		*((DWORD *)tmpTxBuf+0)=nCheckSum;   //checksum
		*((DWORD *)tmpTxBuf+1)=nFileNumber;   //checksum

		szOutFileName = (char *)malloc(OUTFILENAME_LENGTH);
		memset(szOutFileName, 0x00, OUTFILENAME_LENGTH ); /// added 2012.02.21

		strcpy(szOutFileName, szDirName);
		strcat(szOutFileName, TEXT("ubootimage.ubi"));

		hOutFile = CreateFile(szOutFileName,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);
		if( INVALID_HANDLE_VALUE==hOutFile || NULL==hOutFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u) \n"), szOutFileName, __LINE__ , GetLastError() );
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(lstFileBuf) { free(lstFileBuf); lstFileBuf=NULL; }
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			return 0;
		}

		dwState = WriteFile(hOutFile, (void *)(USBtxBuf), USBTxBufSize, &fileBufSize,NULL);
		if( !dwState ) /// added 2011.11.29
		{
			EB_Printf("[dnw] can not write USB Tx buffer! (size:%d, L:%d, Err=%u)\n", USBTxBufSize, __LINE__ , GetLastError() );
			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } 
			if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(lstFileBuf) { free(lstFileBuf); lstFileBuf=NULL; }
			return 0;
		}

		if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
		if(szOutFileName) { free((void*)szOutFileName);  szOutFileName=NULL; } 


		threadResult=_beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  


			//Create the download progress dialogbox.
			CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
			//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
    		//isn't needed because the dialog box already has WS_VISIBLE attribute.
		}
		else
		{
			EB_Printf("[dnw] can not create progress thread! (Ret=%d, L:%d, Err=%u) \n", threadResult, __LINE__ , GetLastError() );

			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } 
			if(lstFileBuf) { free(lstFileBuf); lstFileBuf=NULL; }
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return 0;
		}
	}
	else if ( !strncmp(szTempFileName,_T("ubi"),3) )
	{

		hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u) \n"), szFileName , __LINE__ , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } 
			return 0;
		}
		fileSize=GetFileSize(hFile,NULL);


		if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
		USBtxBuf=(char *)malloc(fileSize);
		if(NULL==USBtxBuf)
		{
			EB_Printf(TEXT("[dnw] can not allocate USB Tx Buffer memory. (L:%d, Err=%u) \n"),  __LINE__ , GetLastError() );
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } 
			return 0;
		}

		// Read ubootimag.ubi file
		rdRet = ReadFile(hFile,(void *)(USBtxBuf),fileSize,&USBTxBufSize,NULL);
		if( (USBTxBufSize!=fileSize) || !rdRet )
		{
			if(!rdRet)
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (Ret=%d, L:%d, Err=%u) \n"), rdRet, __LINE__ , GetLastError());
			}
			else
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Error=%u) \n"), fileSize, __LINE__ , GetLastError());
			}

			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } 
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return 0;
		}
		if(hFile) CloseHandle(hFile);

		iUSBTxBuf=0;

		threadResult=_beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  
	
			//Create the download progress dialogbox.
			CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
			//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
    		//isn't needed because the dialog box already has WS_VISIBLE attribute.
		}
		else
		{
			EB_Printf("[dnw] can not create progress thread! (Ret=%d, L:%d, Err=%u) \n", threadResult, __LINE__ , GetLastError());
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } /// added 2011.12.05
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(lstFileBuf) { free(lstFileBuf); lstFileBuf=NULL; }
			return 0; /// added 2012.02.21
		}
	}
	else if ( !strncmp(szTempFileName,_T("bin"),3) )
	{

		hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u) \n"), szFileName , __LINE__ , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } /// added.
			return 0;
		}
		fileSize=GetFileSize(hFile,NULL);

		if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
		USBtxBuf=(char *)malloc(fileSize);
		if(NULL==USBtxBuf)
		{
			EB_Printf(TEXT("[dnw] can not allocate USB Tx Buffer memory. (L:%d, Err=%u) \n"),  __LINE__ , GetLastError() );
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } /// added.
			return 0;
		}

		// Read ubootimag.ubi file
		rdRet = ReadFile(hFile,(void *)(USBtxBuf),fileSize,&USBTxBufSize,NULL);
		if( (USBTxBufSize!=fileSize) || !rdRet )
		{
			if(!rdRet)
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (Ret=%d, L:%d, Err=%u) \n"), rdRet, __LINE__ , GetLastError());
			}
			else
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Error=%u) \n"), fileSize, __LINE__ , GetLastError() );
			}

			if(hFile) { CloseHandle(hFile); hFile=NULL; } /// added.
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } /// added.
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return 0;
		}
		if(hFile) CloseHandle(hFile);

		iUSBTxBuf=0;

		threadResult=_beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  /// 2012.02.20, update.....	2011.12.05
			EB_Printf("[dnw] Type:BIN USBTX in ROM_MenuUBOOT2... \n" ); ///  2014.08.31, to void progressbar

			//Create the download progress dialogbox.
			CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
			//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
    		//isn't needed because the dialog box already has WS_VISIBLE attribute.
		}
		else
		{
			EB_Printf("[dnw] can not create progress thread! (Ret=%d, L:%d, Err=%u) \n", threadResult, __LINE__ , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } /// added 2011.12.05
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(lstFileBuf) { free(lstFileBuf); lstFileBuf=NULL; }
			return 0; /// added 2012.02.21
		}
	}
	else if ( !strncmp(szTempFileName,_T("nb0"),3) || !strncmp(szTempFileName,_T("dio"),3) )
	{

		nFileNumber = 1;

		hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u) \n"), szFileName , __LINE__ , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } /// added.
			return 0;
		}
		fileSize=GetFileSize(hFile,NULL);

		if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 

		USBtxBuf=(char *)malloc(fileSize+7+4+4+4+4+MAX_PATH);
		if(NULL==USBtxBuf)
		{
			EB_Printf(TEXT("[dnw] can not allocate USB Tx Buffer memory. (L:%d, Err=%u) \n"),  __LINE__ , GetLastError() );
			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } /// added.
			return 0;
		}
		memset((void *)USBtxBuf, 0, fileSize+7+4+4+4+4+MAX_PATH);

		// Read nb0 file
		rdRet = ReadFile(hFile,(void *)(USBtxBuf+7+4+4+4+4+MAX_PATH),fileSize,&USBTxBufSize,NULL);
		if( (USBTxBufSize!=fileSize) || !rdRet )
		{
			if(!rdRet)
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (Ret=%d, L:%d, Err=%u) \n"), rdRet, __LINE__ , GetLastError());
			}
			else
			{
				EB_Printf(TEXT("[dnw] find error of ReadFile. (size=%d, L:%d, Error=%u) \n"), fileSize, __LINE__ , GetLastError() );
			}

			if(hFile) { CloseHandle(hFile); hFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } /// added.
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			return 0;
		}
		CloseHandle(hFile);

		tmpTxBuf = USBtxBuf+7+4+4+4+4+MAX_PATH;

		iUSBTxBuf=0;
		
		*((BYTE *)USBtxBuf+0)=0x4E;
		*((BYTE *)USBtxBuf+1)=0x30;
		*((BYTE *)USBtxBuf+2)=0x30;
		*((BYTE *)USBtxBuf+3)=0x30;
		*((BYTE *)USBtxBuf+4)=0x46;
		*((BYTE *)USBtxBuf+5)=0x46;
		*((BYTE *)USBtxBuf+6)=0xa;

		tmpTxBuf = USBtxBuf + 7 + 4 + 4;

		*((DWORD *)tmpTxBuf+0)=0;				//nb0 start address == 0
		*((DWORD *)tmpTxBuf+1)=fileSize;		//nb0 filesize



		strcpy((char *)(tmpTxBuf+8), szTitleName);

		tmpTxBuf = USBtxBuf+7;
		nCheckSum = 0;
		for ( i = 0; i < 4+4+MAX_PATH; i++ )
		{
			nCheckSum += (unsigned char)(*(tmpTxBuf+i));
		}

		*((DWORD *)tmpTxBuf+0)=nCheckSum;		//checksum
		*((DWORD *)tmpTxBuf+1)=nFileNumber;   //file number = 1

		szOutFileName = (char *)malloc(OUTFILENAME_LENGTH);
		memset(szOutFileName, 0x00, OUTFILENAME_LENGTH ); /// added 2012.02.21

		strcpy(szOutFileName, szDirName);
		strcat(szOutFileName, TEXT("ubootimage.ubi"));

		USBTxBufSize += (7+4+4+4+4+MAX_PATH);

		hOutFile = CreateFile(szOutFileName,GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);
		if( INVALID_HANDLE_VALUE==hOutFile || NULL==hOutFile )
		{
			EB_Printf(TEXT("[dnw] can not open file! (file:%s, L:%d, Err=%u) \n"), szOutFileName , __LINE__ , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; }
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 

			return 0;
		}

		dwState = WriteFile(hOutFile, (void *)(USBtxBuf), USBTxBufSize, &fileBufSize,NULL);
		if( !dwState ) /// added 2011.11.29
		{
			EB_Printf("[dnw] can not write USB Tx file! (size:%d, L:%d, Err=%u) \n", USBTxBufSize , __LINE__ , GetLastError() );
			if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } /// added 2011.12.05
			if(szOutFileName) { free((void*)szOutFileName); szOutFileName=NULL; }
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(lstFileBuf) { free(lstFileBuf); lstFileBuf=NULL; }

			return 0; /// added 2012.02.21
		}


		if(hOutFile) { CloseHandle(hOutFile); hOutFile=NULL; }
		if(szOutFileName) { free((void*)szOutFileName);  szOutFileName=NULL; } 


		threadResult=_beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25,  /// 2012.02.20, update.....	2011.12.05
			EB_Printf("[dnw] Type:NB0 USBTX in ROM_MenuUBOOT2... \n" ); ///  2014.08.31, to void progressbar

			//Create the download progress dialogbox.
			CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
			//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
    		//isn't needed because the dialog box already has WS_VISIBLE attribute.
		}
		else
		{
			EB_Printf("[dnw] can not create progress thread! (Ret=%d, L:%d, Err=%u) \n", threadResult, __LINE__ , GetLastError() );
			if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } /// added 2011.12.05
			if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
			if(lstFileBuf) { free(lstFileBuf); lstFileBuf=NULL; }
			return 0; /// added 2012.02.21
		}
	}
	else
	{
		EB_Printf("[dnw] can not support file format in ROM_MenuUBOOT2! (file:%s, L:%d, Err=%u) \n", szTempFileName , __LINE__ , GetLastError() );
		if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } 
		if(USBtxBuf) { free( (void*)USBtxBuf ); USBtxBuf=NULL; } 
		if(lstFileBuf) { free(lstFileBuf); lstFileBuf=NULL; }
		return 0;
	}



	/* --------------- uppercase ------------------------------------- */
	MaxLen = strlen((char *)szTitleName);
	memset(tmpFileName, 0x00, sizeof(tmpFileName) );
	for(i=0; i<MaxLen; i++) 
	{ 
		if(szTitleName[i] >= 'a' && szTitleName[i] <= 'z' )
			tmpFileName[i] = szTitleName[i]^0x20; 
		else
			tmpFileName[i] = szTitleName[i];
	}
	/* -------------------------------------------------------------- */
	//////// EB_Printf("[dnw] ROM Writing binary name: %s \r\n", tmpFileName);

	if( NULL != strstr((const char *)tmpFileName, (char *)"NK" ) )
	{
		return DNW_NK;
	}
	else if( NULL != strstr( (const char *)tmpFileName, (char *)"BOOT" ) ) /* EBOOT */
	{
		return DNW_EBOOT;
	}
	else if( NULL != strstr( (const char *)tmpFileName, (char *)"STEPLDR" ) )
	{
		return DNW_STEP;
	}
	else
	{
		return DNW_OTHER;
	}

}



BOOL RAM_MenuUsbTransmit(HWND hwnd, int Fileindex, BYTE funcKey)
{
	ULONG i;
	HANDLE hFile = NULL;
	ULONG fileSize;
	USHORT cs=0;
	uintptr_t threadResult;
	TCHAR * tocken = NULL;
	int comma;
	DWORD localdownloadAddress;
	int cnt=0;
	BOOL rdRet=FALSE;



	if(hWrite) { CloseHandle(hWrite); hWrite = NULL; } 


	if( autoSendKey ) // TRUE == ~~~~
	{ 
		switch( RetryUSB )
		{
			case 1: /// IDC_RETRY_USB_20_TIMES
				dnUSBcount = 20; 
				break;
			case 2: /// IDC_RETRY_USB_30_TIMES
				dnUSBcount = 30; 
				break;
			case 3: /// IDC_RETRY_USB_40_TIMES
				dnUSBcount = 40; 
				break;
			case 0: /// IDC_RETRY_USB_DEFAULT
			default:
				dnUSBcount = 1; 
				break;
		}
	} /// 2012.03.06
	else { dnUSBcount = 1; } /// 2012.03.06

	cnt = 0;
	do {
		Sleep(USB_DELAY_TIME);
		hWrite = open_file( outPipe, cnt, funcKey );
		if( cnt++ > USB_OPEN_RETRY*dnUSBcount ) break;
	} while( (INVALID_HANDLE_VALUE==hWrite) || (NULL==hWrite) );
	
	if( (INVALID_HANDLE_VALUE==hWrite) || (NULL==hWrite) )
	{
		///MessageBox(hwnd,TEXT("Can't open USB device.\n"),TEXT("Error"), MB_OK | MB_ICONINFORMATION );
		EB_Printf(TEXT("[dnw] can not open USB device for transmitting USB RAM Tx file! (Error=%u) \n"), GetLastError() );
		return FALSE;
	}

	
    //////////////
	/// EB_Printf(TEXT("[dnw] RAM File; [%s] \n"), szUMONDownloadString[Fileindex] );
//	tocken = strtok(szDownloadString[Fileindex],",");
	for ( i = 0; i < FILENAMELEN; i++ )
	{
		if ( szUMONDownloadString[Fileindex][i] == ',' )
		{
			comma = i;
			tocken = &szUMONDownloadString[Fileindex][i+1];
			break;
		}
	}

	memcpy(szUmonFileName, szUMONDownloadString[Fileindex], i);
	szUmonFileName[i] = '\0';
//	tocken = strtok(NULL,",");
//	memcpy(szTitleName, szEBOOTTitleName, FILENAMELEN);
	localdownloadAddress = hex2int(tocken);

	EB_Printf(TEXT("[dnw] RAM File(%s) & Addr(0x%X)\n"), szUMONDownloadString[Fileindex], localdownloadAddress );
    
//    if(result==0) //file open fail
//    {
//	CloseHandle(hWrite);
//	return;
//    }

	hFile=CreateFile(szUmonFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
	{
		EB_Printf(TEXT("[dnw] can not create file[%s]. [L:%d, Err=%u] \n"), szUmonFileName , __LINE__ , GetLastError() );
		if(hWrite) { CloseHandle(hWrite); hWrite=NULL; } /// added.
		return FALSE;
	}

	///AddTransmitMenu(hwnd, szUmonFileName, localdownloadAddress);

	fileSize=GetFileSize(hFile,NULL);


	if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
	USBtxBuf=(char *)malloc(fileSize+10); 
	if(NULL==USBtxBuf)
	{
		EB_Printf(TEXT("[dnw] can not allocate memory! (size:%d, L:%d, Err=%u) \n"),fileSize+6 , __LINE__ , GetLastError() );
		if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
		if(hWrite) { CloseHandle(hWrite); hWrite=NULL; } 
		return FALSE;
	}

	rdRet = ReadFile(hFile,(void *)(USBtxBuf+8),fileSize,&USBTxBufSize,NULL);
	if( (USBTxBufSize!=fileSize) || !rdRet )
	{
		EB_Printf(TEXT("[dnw] find error of ReadFile! (size=%d, L:%d, Err=%u) \n"), fileSize , __LINE__ , GetLastError() );

		if(hFile) { CloseHandle(hFile); hFile=NULL; } 
		if(hWrite) { CloseHandle(hWrite); hWrite=NULL; }
		if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
		return FALSE;
	}

	*((DWORD *)USBtxBuf+0)=localdownloadAddress;
	*((DWORD *)USBtxBuf+1)=fileSize+10;   //attach filesize(n+6+4) 
	for(i=8;i<(fileSize+8);i++)
	cs+=(BYTE)(USBtxBuf[i]);
	*((WORD *)(USBtxBuf+8+fileSize))=cs;   //attach checksum 

	USBTxBufSize+=10;
	iUSBTxBuf=0;

	if(hFile) CloseHandle(hFile);

	threadResult = _beginthread( (void (*)(void *))UsbTxFile, THREAD_STACK_SIZE,(void *)0);

	if(threadResult!=-1)
	{
		Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25, 


		//Create the download progress dialogbox.
		CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG1),hwnd,DownloadProgressProc); //modaless
		//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
		//isn't needed because the dialog box already has WS_VISIBLE attribute.
	}
	else
	{
		EB_Printf("[dnw] can not create progress thread! (Ret=%d, L:%d, Err=%u) \n", threadResult, __LINE__ , GetLastError() );
		if(hFile) { CloseHandle(hFile); hFile=NULL; } 
		if(hWrite) { CloseHandle(hWrite); hWrite=NULL; }
		if(USBtxBuf) { free((void*)USBtxBuf); USBtxBuf=NULL; }
		return FALSE;
	}

	//The dialog box will be closed at the end of TxFile().
	//free(USBtxBuf) & CloseHandle(hWrite) will be done in TxFile()

	return TRUE;

}





