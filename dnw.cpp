// ----------------------------
// dnw.cpp
// -------------------------------

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include "resource.h"

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>
#include <string.h>
#include <process.h> /* For _beginthread() */
#include <stdlib.h>

#include <io.h>      // _findfirsti64()
#include <wchar.h>

#include "def.h"
#include "dnw.h"
#include "engine.h"
#include "fileopen.h"
#include "d_box.h"
#include "usbtxrx.h"
#include <stdio.h> 
#include <memory.h> 
#include "regmgr.h"
#include <Tlhelp32.h> // CreateToolhelp32Snapshot(TH32CS_SNAPALL,NULL);



//NOTE: _beginthread() 
// To use _beginenthread(),Activate "Project/Settings/C/C++/Categry/Code generation/
// Use Run-Time library/Multithreaded or Debug Multithreaded"

// If DDK2000 is used,
// 1. Project_Settings/CC++/Preprocessor/Additional_include_directories = C:\NTDDK\inc
// 2. Project_Settings/Link/Input/Additional_library_path=c:\NTDDK\libfre\i386

// If DDKXP is used,
// 1. Project_Settings/CC++/Preprocessor/Additional include directories = C:\WINDDK\2600\inc\w2k
// 2-1. Project_Settings/Link/Input/Additional_library_path=C:\WINDDK\2600\lib\w2k\i386
//      This causes some warning about '.rdata' section attribute.
// 2-2. Project_Settings/Link/Input/Additional_library_path==C:\WINDDK\2600\lib\i386\free
//      There's no warning.

/*
===================== REVISON HISTORY =====================
1. 2000. 3.30: V0.01 
  First release of DNW
2. 2000.10.11: V0.2
  The edit control is used for scroll, copy&paste, smooth screen update
3. 2001.1.26: V0.3
  a) The CPU usage will be less. Sleep() is inserted during TX.
  b) The filesize and checksum are transmitted together with bin file.
  c) WriteCommBlock() bug is fixed. The txEmpty flag should be changed in only DoRxTx().
4. 2001.2.24: V0.31
  a) The size of edit buffer is changed by EM_LIMITTEXT message.
     EDIT_BUF_SIZE(30000) -> MAX_EDIT_BUF_SIZE(65000)
  b) If the edit box is greater than 50000,
     the size of edit box is reduced by 10000.
  c) The horizontal scroll bar is removed for better look. 
  d) In WaitCommEvent() loop, 
     the following condition is inserted to clear the overrun condition.
    	if((dwEvtMask & EV_ERR){...}
  e) EB_Printf() have some error to process large string data.
5. 2001.3.8: V0.32
  a) EDIT_BUF_SIZE is reduced 25000 because the EM_REPLACESEL message is done very slowly
     if the size is over about 30000.
6. 2001.4.11: V0.32A
  a) Experimentally, MAX_EDIT_BUF_SIZE is set to the default value(32767). 
     //SendMessage(_hwndEdit, EM_SETLIMITTEXT, MAX_EDIT_BUF_SIZE, 0L);
     RESULT: MAX_EDIT_BUF_SIZE doesn't affect the display delay problem.
             I think that the new method for deleting the contents should be applied
	     Let's do tonight
7. 2001.5.14: V0.34
   a) I have known that the edit control isn't adequate for console program.
      So, I would give up the development of DNW using the edit control
      The last decision is as follows;
      MAX_EDIT_BUF_SIZE (65000) //up to 65535
      EDIT_BUF_SIZE (30000)   
      EDIT_BUF_DEC_SIZE (10000)
   b) If the selected text is deleted, the edit control displays the first of the text.
      In this case, to show the end of the text, dummy REPLACE_SEL is added.

8. 2001.11.23: V0.4
   a) USB download function is added.
   b) GetOverlappedResult() is used in TxFile() in order to save the cpu time more efficiently 
   c) Serial Configuration dialog box
   d) In secbulk.sys is changed to support IRP_MN_QUERY_CAPABILITIES.
      So, the surpriseRemoval is allowed. When the USB is yanked impolitely, 
      the warning dialog box won't appear in WIN2000.

9. 2001.11.24: v0.41alpha
   a) WriteFile() supports overlapped I/O to check broken pipe.
   b) progress bar is added for transmit operation.
   c) USB,serial status is printed on the window title bar
   
10. 2001.12.5: v0.42a
   a) In secbulk.sys, the maximum number of bulk packit in 1ms frame duration is changed to 16 from 4.
      So, transfer rate is increased from 220KB/S to 405KB/S 
   b) Although the fileopen was failed(or canceled), the transmit wasn't canceled. This is fixed.      
   c) When the options menu is selected, a serial port will be reconnected.
   d) The receive test menu is added. 

11. 2001.12.6: v0.43
   a) Fou USB tx operation, TX_SIZE is increased from 2KB to 16KB.
      So, transfer rate is increased from 405KB/S to 490KB/S.
      (2KB:405KB/S 4KB:450KB/S  8KB:470KB/S 16KB:490KB/S)

12. 2001.12.7: v0.44b
   a) Although a serial port is not connected, the serial port is connected 
      after Configuration/Options menu -> fixed.
   b) The name dnw.cfg for v 0.4x is changed to dnw.ini 
      in order not to confuse old dnw.cfg for ver 0.3x
      
12. 2002.01.2: v0.44c
   a) The edit box size is changed to display 80x25 characters 
      
13. 2002.02.22: v0.45
   a) In windows95, DNW doesn't search USB stack.
   b) When download, there should be a cancel button
   c) Sometimes, the progress bar is not filled although download is doing.
      I think it's because InitDownloadProgress() is executed 
      before than DownloadProgressProc().
      So, I inserted the code to wait until DownloadProgressProc() is executed as follows;
	  while(_hDlgDownloadProgress==0); //wait until the progress dialog box is ready.
   d) secbulk is optimized 
14. 2002.04.01: v0.46
   a) If DNW is start to transmit although the b/d is not ready, DNW will be hung.
      It's only solution to turn off and on the b/d.
      To solve this problem smoothly, the overlapped I/O will be used for USB transfer. 
      But, secbulk.sys may not support overlapped I/O. 
      Although I implemented the overlapped I/O, WriteFile() function didn't return before its completion.
   b) Because the overlapped I/O doens't work as my wish,
      and in order to quit the dnw.exe hung, modaless dialogue box is used for the progress bar.
   c) *.nb0

  15. 2002.04.10: v0.47
   a) I reduce the edit box size as follows;  
	#define MAX_EDIT_BUF_SIZE (0x7FFE) 
	#define EDIT_BUF_SIZE (0x6000)   
	#define EDIT_BUF_DEC_SIZE (0x1000)
      There is no good cause about why I change. I want to just reduce the edit box size.
   b) Sometimes, when transmit, there is no transmit although the transmit progrss dialog box is shown.
      I think that it's because the _enthread() fails.
      To debug this problem, the _enthread result is checked.

  16. 2002.04.19: v0.48
   a) The bug of WIN2K WINAPI is found. ->It's not fixed perfectly.
      Please let me know of the solution to avoid the memory problem of SetWindowText() API.

      The SetWindowText()(also,WM_SETTEXT message) consumes 4KB system memory every times.
      I think there is some bug in SetWindowText API.
      In my case, because SetWindowText() is called every 1 seconds, 
      the system memory used by DNW.exe is increased by 4KB every 1 seconds.
      For emergency fix, SetWindowText() will be called only when its content is changed.
      
      NOTE. This memory problem is not memory leakage problem 
            because the memory is flushed when the window is shrinked.

  17. 2002.05.03:v0.49
   a) Sometimes, when transmit, there is no transmit although the transmit progrss dialog box is shown.
      I have found the cause.
      It's because _hDlgDownloadProgress.
      If the TxFile thread is executed first than WM_INITDIALOG message,
      while(_hDlgDownloadProgress==NULL); will not exited because _hDlgDownloadProgress 
      in CPU register will be checked. So, the volatile should have been used 
      because _hDlgDownloadProgress value is changed in another thread.
      The solution is as follows;
      volatile HWND _hDlgDownloadProgress=NULL;
   b) Sometimes, the CPU usage will be always 100% if dnw.exe is being executed.
      This is because of the just above problem.
      If the problem 17-a) is occurred, the TxFile will be an obsolete thread.
      while(_hDlgDownloadProgress==NULL); will use the CPU at 100%.
      I think that this problem may be cleared because the problem 17-a) is cleared.
   c) The small icon,DNW is displayed in the window shell task bar and the window title.

  18. 2003.04.25:v0.50
   a) In case that a USB2Serial cable is used when COMPAQ Presario 1700, 
      a few last character display is postponed until the next group is received.
      -> MSDN recommends the while() for read operation, which has solved the problem.
   b) In DoRxTx(), the EV_ERR is added to SetCommMask(); -> which is more correct, I think.
   c) For overlapped WaitCommEvent,ReadFile,WriteFile, the return value should be checked.
      For ReadFile,WriteFile cases, the operation has no change.
      In previous version(V0.49), WaitCommEvent(,,NULL) should have not been used because OVERLAPPED I/O was being used.
      The combination of WaitCommEvent(,,&os) and GetOverlappedResult() will fix this problem.
   d) During developing the DNW v0.50, I had experienced some blue screen without any USB connection when I reset the SMDK2410.
      This problem was occurred when WaitCommEvent(,,&os) without GetOverlappedResult().
      This problem was not revived when is WaitCommEvent(,,NULL) although it's not correct. 
      
      From this phenomenon, I assume that the blue-screen, which is caused sometimes when resetting the SMDK2410, 
      may be caused by the mis-use of WaitCommEvent(,,NULL).

      But, it's still truth that the abnormal USBD response may cause the windows blue-screen
      ,which I experienced many times during my secbulk.sys development.
   
  19. 2003.04.29:v0.50A
   a) If the break condition is detected, "[DBG:EV_ERR]" message is displayed.
      So, the display of "[DBG:EV_ERR]" is removed.
*/

/*   
Items to be enhanced.
   - avoid SetWindowText() API problem.
   - remove debug stuff in following functions.
         void InitDownloadProgress(void);   
	 void DisplayDownloadProgress(int percent);
   - make status bar.
   - malloc() uses too much memory for large file transfer.
   - Enlarge the scroll buffer -> the edit box is not adequate.
   - file logging function
   - Ctrl+C should be work as copy function.
*/ 

/*
Edit Box Note:
- Check if the edit box is scrolled to show the text being deleted
  when the selected text is deleted in Win2000. In windows98, It's scrolled to show the deleted portion unfortunately.
*/

#define IS_OVERLAPPED_IO 			(TRUE)
#define IS_OVERLAPPED_CHECK 		1 /// 2014.03.04, if Long Time, disconneted UART...
#define IS_OVERLAPPED_WAIT 			(30*1000) /// 30sec  <-- INFINITE


#if USE_RX_TX_QUEUE
#include "queue.h"

//------------------------------
// Queue 
//------------------------------
ctQueue Rx2Tx;
#endif


int userBaudRate,idBaudRate, autoSendKey=0, msgSaveOnOff=0;

#if DISPLAY_PC_SYSTEM_TIME /*--- 2016.02.12 ------ */
int localTime=0; // NONE
int isTimeInfoOnce=0; // Time 정보를 맨 앞에 한번 표시한다. 
#endif

int cmdCRLF=0; // 2019.12.21
int LogFileType=0; // 2020.04.07


#if DNW_2ND_WINDOWS_ENABLE // 2017.4.14
extern int isStatusBarEnable; // BAR_STATUS, BAR_COMMAND
extern int OKstatusBarInitial; // 2017.4.14
#endif

int userComPort;
int TotalROMfilenum, UmonFileNum=0;
int FontType = 0;
int CPUtype=1; // 2014.04.11, SMDK6410:0, SMDKC100:1, S3C2440A:2, Other:3

// #if HEXA_MODE_SUPPORT	/* 2011.02.08 Hexa mode display */
int TextorHexaMode=0; // display Text mode or Hexa mode -> default Text mode(0)

// #endif
extern DWORD HexaEndChar; // 2016.04.12

__int64  iUARTTxFileSize=0L; // 2016.10.12 tx data in MENU
int isTxUARTDataExit=0;
int isMenuTxing = 0;


#if USE_TXDATA_TO_SERIAL
int isTxData2UART = 0; // Tx Data on/Off for UpdateWindowTitle()
int sendTxDataType=0, txDataMsgOnOff; // 2016.03.23

extern volatile HWND _hDlgTxData2UART;

extern unsigned int TxScriptCommandLines;
#endif


#if USE_FLOAT2HEX_VALUE // 2018.07.18
extern volatile HWND _hDlgFlt2Hexa;

int iChoiceFlt2Hex=0, iFlt2HexEndian=0; // F2H_LITTLE_ENDIAN
#endif



#if (RICHED_COLOR) || (TEXT_FIND_MARK) // 2016.09.23
int iColorText = 0; // 0:black, 1:Red, 2:Magenta, 3:Blue, 4:Green, 5:Yellow

int txtColorRed=0, txtColorMegenta=0, txtColorBlue=0, txtColorGreen=0, txtColorYellow=0;
int txtFindRed=0, txtFindMegenta=0, txtFindBlue=0, txtFindGreen=0, txtFindYellow=0;
#endif

#if (TEXT_FIND_MARK) // 2016.09.23
int isTxtMark=0;
#endif

#if USE_HEX2BIN_CONVERT // 2016.03.04
int userHexType=0, userHex2BinLogOnOff=0;
int userCRCtype=2; // CRC8
int userEndian=0; // little
int userHexAddrZeroForced=0; // default : read hex address
int userCRCverify=1; // ON
int userSizeUnit=0; // Unit : KByte or Byte
int userSwapWordwise=0; // Off
int userAddrAlignWord=0;
int userPadByteOnOff=1; // 2018.07.13
#endif

#if defined(COLOR_DISP2) /// 2010.05.14
int ColorType=0; // 0:Default(Gray), 1:Black, Gray1:2, Gray2:3
#endif

#ifdef ENGINEER_MODE /// 개발자용은 필요없음.
int RetryUSB = 0; /// defauilt
#else
int RetryUSB = 1; /// 20 times
#endif


#if COM_SETTINGS /// 2012.07.11
int userCOMparity=0, userCOMstop=0, userCOMdataIdx=1;
int userFlowControl=0; // 2017.08.03 None FlowControl
int userFlowCtrlVal=0; // 2017.08.04 None FlowControl

int userHexaDispVal=0; // 2018.03.22 None Hexa Display



#define LEN_COMPARITY 		6
#define LEN_COMSTOP 		4
#define LEN_COMDATABIT 		2

const TCHAR* szCOMParity[LEN_COMPARITY] = {
	TEXT("N"), // NOPARITY
	TEXT("O"), // ODDPARITY
	TEXT("E"), // EVENPARITY
	TEXT("M"), // MARKPARITY
	TEXT("S"), // SPACEPARITY
	TEXT("*"), // unknown
};


const TCHAR* szCOMStop[LEN_COMSTOP] = {
	TEXT("1S"),   // ONESTOPBIT
	TEXT("1.5S"), // ONE5STOPBITS
	TEXT("2S"),   // TWOSTOPBITS
	TEXT("*S")    // Unknown
};


const int COMportDatabit[LEN_COMDATABIT] = { 
	7,  /// index 0 : 7bit 
	8   /// index 1 : 8bit
};

#endif


#if USER_SCREEN_BUF_SIZE // 2016.02.14
// 0: Huge(100MB), 1:Latge, 2:Middle, 3:Small(2MB), 4:Smallest(1MB)
int userScrBufSizIndex=0; // huge
#endif



#if 1 /// 2015.0516 --- 
#define MAX_IN_BLOCK_SIZE 		(4*1024) // 2020.09.10 /// 2015.05.16 --- 
#define MAX_OU_BLOCK_SIZE 		(4*1024) // 2020.09.10 
#else
#define MAX_BLOCK_SIZE 		(4096)
#endif
#define RX_BUF_SIZE 			(MAX_IN_BLOCK_SIZE+20)



HANDLE idComDev = NULL;
OVERLAPPED osWrite, osRead;
volatile int isConnected=0; // 0:Disconnect, 1:Connected, 2:Try to connect
TCHAR rxBuf[RX_BUF_SIZE] = {0,};

volatile char *SerialTxBuf = NULL;
volatile DWORD idxTxBuf = 0;
DWORD TxBufSize;

volatile BOOL txEmpty=TRUE; /* int -> BOOL */

extern TCHAR szDownloadAddress[ADDR_LENGTH];
extern TCHAR szColumnNumber[16];


extern TCHAR szSerialTxCharData[TX_DATA_LENGTH]; // 2016.03.23
extern TCHAR szSerialTxHexaData[TX_DATA_LENGTH]; // 2016.03.23
extern TCHAR szSerialTxFileData[TX_DATA_LENGTH]; // 2016.03.28
extern TCHAR szSerialTxFileTitl[TX_DATA_LENGTH]; // 2016.03.28

extern TCHAR szFileNameOnPopUP[FILENAMELEN];

extern void BeepSound(int beeptype);


#if USE_WIN_OUTOF_AREA_TO_ZERO // 2016.03.31 LCD size
extern DWORD myLCDwidth, myLCDheight; 
extern DWORD WIN_XBgn, WIN_YBgn;
extern DWORD MainWidth, MainHeight;
extern DWORD WIN_XSIZE, WIN_YSIZE;

extern DWORD myVirWidth, myVirHeight; // 2017.04.20 VirtualLCD size
#endif


#if HEXA_MODE_SUPPORT	/* 2011.02.08 Hexa mode display */
extern DWORD ColumnNumber;
#endif

extern HWND _EditHwnd, _MainHwnd;
extern WORD  DownloadedBin;
extern int 	 BinFileIndex;

extern char *os_name(void);
extern TCHAR *GetProcessorName();
//extern TCHAR *GetIPaddress(void);



// get memory info...
extern int getTotalRAM(void);
extern int getAvailRAM(void);
extern int getTotalMemory(void);
extern int getAvailMemory(void);
extern __int64 XfGetTime(void);
extern int ProcessorType(void);

extern void GetComPortRegistry(void);

#if DISPLAY_MAX_EDIT_BUF_SIZE // 2016.02.12
extern unsigned int iEditCount, iLineDeleted;
#endif


void GMTTime2LocalTime(SYSTEMTIME gmtTime, SYSTEMTIME *local)
{
	// Converting UTCTime to LocalTime.
	FILETIME FileTime, LocalFileTime;
	SYSTEMTIME LocalTime;

	SystemTimeToFileTime(&gmtTime, &FileTime);
	FileTimeToLocalFileTime(&FileTime, &LocalFileTime); // to Local Time
	FileTimeToSystemTime(&LocalFileTime, &LocalTime);

	memcpy( (void*)local, &LocalTime, sizeof(SYSTEMTIME) );
}


void LocalTime2GMTTime(SYSTEMTIME locTime, SYSTEMTIME *gmt)
{
	FILETIME FileTime, GMTFileTime;
	SYSTEMTIME LocalTime;

	SystemTimeToFileTime(&locTime, &FileTime);
	LocalFileTimeToFileTime(&FileTime, &GMTFileTime); // to GMT Time
	FileTimeToSystemTime(&GMTFileTime, &LocalTime);

	memcpy( (void*)gmt, &LocalTime, sizeof(SYSTEMTIME) );
}


int SearchProcess()
{
    DWORD dwSize = 250;
    HANDLE hSnapShot=NULL;
    PROCESSENTRY32 pEntry;
    BOOL bCrrent=FALSE, hRes=FALSE;


    hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL,NULL);

	if( hSnapShot ) 
	{
	    pEntry.dwSize = sizeof(pEntry);

	    if( Process32First (hSnapShot,&pEntry) )
	    {
			do {

				EB_Printf(TEXT("[dnw] Run -----: [%s] [%d] [0x%x, 0x%x, 0x%x, 0x%x] \r\n"), 
					pEntry.szExeFile, pEntry.cntUsage,
					pEntry.th32ProcessID, pEntry.th32ModuleID, pEntry.cntThreads, pEntry.th32ParentProcessID );


		        if(!strncmp(pEntry.szExeFile,"EXC1212EL.EXE",15))
		        {
		            bCrrent = TRUE;
		        }

		        if(bCrrent)
		        {
		            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pEntry.th32ProcessID);

		            bCrrent = FALSE;
		            if(hProcess)
		            {
		                if(TerminateProcess(hProcess, 0))
		                {
		                    unsigned long nCode; //프로세스 종료 상태
		                    GetExitCodeProcess(hProcess, &nCode);
							EB_Printf(TEXT("[dnw] Kill -----: [%s] [%d] \r\n"), pEntry.szExeFile, pEntry.cntUsage );
		                }
		                CloseHandle(hProcess);
		            }
		            //break;
	        	} 

			} while( hRes = Process32Next (hSnapShot,&pEntry) );
	    }

		
		CloseHandle(hSnapShot); 
	}
	
    return 0;
}


extern TCHAR Cpu_info[MAX_PATH];
extern TCHAR Cpu_ID[MAX_PATH];

extern TCHAR ProductName[MAX_PATH];
extern TCHAR Manufacture[MAX_PATH];
extern TCHAR BIOSRelease[MAX_PATH]; // 2020.03.27,
extern TCHAR BIOSVersion[MAX_PATH];
extern TCHAR BIOSVendor_[MAX_PATH];

extern TCHAR SWProductName[MAX_PATH];
extern TCHAR SWEditionID[MAX_PATH];

extern TCHAR SWReleaseId[MAX_PATH];
extern TCHAR SWCurrentBuildNumber[MAX_PATH];

extern unsigned __int64 SWInstallDate;
//extern unsigned __int64 SWInstallTime;


#if CIPHER_RSA2048 // 2018.02.20

//#include <Windows.h> 
//#include <stdio.h> 
#include ".\RSA2048\common.h" 
#include ".\RSA2048\rsa2048.h" 


//int main(void) 
int RSA2048(void)
{ 
	HCRYPTPROV hCryptProv = 0;  // CSP handle
	HCRYPTKEY key = 0;          // Signature key pair handle
	unsigned long cLen = 0; 
	char *cipherText = NULL; 
	char *plainText = "PLAIN_TEXT_PLAIN_TEXT\0"; 
	//char *plainText = "ABC\0"; 
	unsigned char *decrypted = NULL; 
	unsigned char *publicKey = NULL; 
	unsigned char *privateKey = NULL; 


	if (!CryptoInit(&key, &hCryptProv, &publicKey, &privateKey)) 
	{ 
		//printf("Crypto initializing failed\n"); 
		EB_Printf(TEXT("[dnw] Crypto initializing failed\n") );
		return EXIT_FAILURE; 
	} 

	EB_Printf(TEXT("[dnw] RSA2048 key  : [%I64u] %I64x  \n"), key, key);
	//EB_Printf(TEXT("[dnw] publicKey  : [%s] \n"), publicKey );
	//EB_Printf(TEXT("[dnw] privateKey : [%s] \n"), privateKey );
	EB_Printf(TEXT("[dnw] Original plain text: [%s] \n"), plainText );


	if (!Encrypt(key, &cipherText, &cLen, (unsigned char *)plainText, strlen(plainText))) 
	{ 
		//printf("Encryption failed\n"); 
		EB_Printf(TEXT("[dnw] Encryption failed\n") );
		if (hCryptProv) CryptReleaseContext(hCryptProv, 0); 
		return EXIT_FAILURE; 
	} 


	//printf("Encrypted string: %s\n", cipherText); 
	EB_Printf(TEXT("[dnw] Encrypted string   : ]%s[ \n"), cipherText );


	if (!Decrypt(key, &decrypted, cipherText, cLen)) 
	{ 
		//printf("Decryption failed\n"); 
		EB_Printf(TEXT("[dnw] Decryption failed \n") );

		SAFE_FREE(cipherText); 
		if (hCryptProv) CryptReleaseContext(hCryptProv, 0); 
		return EXIT_FAILURE; 
	} 


	SAFE_FREE(cipherText); 


	//printf("Decrypted string: %s\n", decrypted); 
	EB_Printf(TEXT("[dnw] Decrypted string   : [%s] \n"), decrypted );


	SAFE_FREE(decrypted); 


	CryptoUninit(key, hCryptProv); 


	SAFE_FREE(publicKey); 
	SAFE_FREE(privateKey); 


	return EXIT_SUCCESS; 
} 
#endif // CIPHER_RSA2048


#if 0
#define SQR1(x) 		((x)*(x))
#define SQR2(x) 		(x*x)


int MISRA_test(void)
{
	int a=0, b=3;
	int bCheck = 1;
	int i, j;
	char szSrc[10] = "abcd";
	char szDes[10] = "ABCD";
	
	if( bCheck && ++a )
	{
		EB_Printf(TEXT("in if ++++++++++++  a(%d)\r\n"), a);
	}
	EB_Printf(TEXT(" outsider ... a(%d)  \r\n"), a);


	EB_Printf(TEXT("===SQR=== %d %d \r\n"), SQR1(a+b), SQR2(a+b) );


	j = strcmp( szSrc, szDes );
	if( !j ) EB_Printf(TEXT("------------ \r\n"));
	EB_Printf(TEXT("strcmp == %d %d \r\n"), j, !j);
	
	j = strcmp( szDes, szSrc );
	if( !j ) EB_Printf(TEXT("------------ \r\n"));
	EB_Printf(TEXT("strcmp == %d %d \r\n"), j, !j);
	
	j = -21;
	EB_Printf(TEXT("-----00------- (%d %d) \r\n"), j, !j);

	j = 0x16;
	EB_Printf(TEXT("-----00------- (%d %d) \r\n"), j, !j);

	
	return 0;
}
#endif

#if 0
static const char *GetExceptionString(DWORD exception) 
{ 
#define EXCEPTION(x) case EXCEPTION_##x: return (#x); 
	static char buf[16]; 

	switch (exception) 
	{ 
		EXCEPTION(ACCESS_VIOLATION) 
		EXCEPTION(DATATYPE_MISALIGNMENT) 
		EXCEPTION(BREAKPOINT) 
        EXCEPTION(SINGLE_STEP) 
        EXCEPTION(ARRAY_BOUNDS_EXCEEDED) 
        EXCEPTION(FLT_DENORMAL_OPERAND) 
        EXCEPTION(FLT_DIVIDE_BY_ZERO) 
        EXCEPTION(FLT_INEXACT_RESULT) 
        EXCEPTION(FLT_INVALID_OPERATION) 
        EXCEPTION(FLT_OVERFLOW) 
        EXCEPTION(FLT_STACK_CHECK) 
        EXCEPTION(FLT_UNDERFLOW) 
        EXCEPTION(INT_DIVIDE_BY_ZERO) 
        EXCEPTION(INT_OVERFLOW) 
        EXCEPTION(PRIV_INSTRUCTION) 
        EXCEPTION(IN_PAGE_ERROR) 
        EXCEPTION(ILLEGAL_INSTRUCTION) 
        EXCEPTION(NONCONTINUABLE_EXCEPTION) 
        EXCEPTION(STACK_OVERFLOW) 
        EXCEPTION(INVALID_DISPOSITION) 
        EXCEPTION(GUARD_PAGE) 
        EXCEPTION(INVALID_HANDLE) 
	default: 
        _snprintf_s(buf, sizeof(buf), _TRUNCATE, "0x%x", exception); 
        return buf; 
        //return "UNKNOWN_ERROR"; 
	} 


#undef EXCEPTION 
} 
#endif

/* --------------------------------------------------------------
%%	% 자체를 표시한다.
%a	요일이름을 표시한다. (일..토)
%A	완전한요일이름을 표시한다. (일요일..토요일)
%b	월이름을 표시한다. (1월..12월)
%B	완전한월 이름을 표시한다. (1월..12월)
%c	날짜와 시간을 표시한다. (2007년 11월 14일 (수) 오전 12시 50분 16초)
%C	세기를 나타낸다. (년을 100으로 나눈 몫)
%d	월일 (01..31)
%D	월일 (mm/dd/yy)
%e	월일 (1..31)
%F	%Y-%m-%d 와 같다.
%g	년도로 마지막 두자리만 표시
%G	년도로 모든자리 표시
%h	%b 와 같다.
%H	시간 (00..23)
%I	시간 (01..12)
%j	년일(001..366)
%k	시간 (0..23)
%l	시간 (1..12)
%m	월 (01..12)
%M	분 (00..59)
%n	개행문자
%N	나노초 (000000000..999999999)
%P	오전 오후
%p	오전 오후
%r	시간 (오전/오후 hh시 mm분 ss초)
%R	시간 (hh시 mm분)
%s	UTC 기준 1970-01-01 부터 지금까지 흐른 초
%t	탭문자
%T	24시간 (hh:mm:ss)
%u	주중 요일 (1..7), 1이 월요일이다
%U	1년중 몇번째 주인지, 일요일 기준 (00..53)
%V	1년중 몇번째 주인지, 월요일 기준 (00..53)
%w	주중 요일 (0..6), 0은 일요일
%x	mm/dd/yy
%X	%H:%M:%S 와 동일
%Y	년도 (1970..)
------------------------------------------------------ */

extern UINT str2hex(TCHAR *str);
extern ULONG hex2dec(TCHAR *str);


/* ------------------------------------------------------
01 : Linux_Year = 1970 -> fSUM = 31536000 -> Acc = 31536000UL 
02 : Linux_Year = 1971 -> fSUM = 31536000 -> Acc = 63072000UL 
03 : Linux_Year = 1972 -> fSUM = 31622400 -> Acc = 94694400UL <- YOON 
04 : Linux_Year = 1973 -> fSUM = 31536000 -> Acc = 126230400UL 
05 : Linux_Year = 1974 -> fSUM = 31536000 -> Acc = 157766400UL 
06 : Linux_Year = 1975 -> fSUM = 31536000 -> Acc = 189302400UL 
07 : Linux_Year = 1976 -> fSUM = 31622400 -> Acc = 220924800UL <- YOON 
08 : Linux_Year = 1977 -> fSUM = 31536000 -> Acc = 252460800UL 
09 : Linux_Year = 1978 -> fSUM = 31536000 -> Acc = 283996800UL 
10 : Linux_Year = 1979 -> fSUM = 31536000 -> Acc = 315532800UL 
11 : Linux_Year = 1980 -> fSUM = 31622400 -> Acc = 347155200UL <- YOON 
12 : Linux_Year = 1981 -> fSUM = 31536000 -> Acc = 378691200UL 
13 : Linux_Year = 1982 -> fSUM = 31536000 -> Acc = 410227200UL 
14 : Linux_Year = 1983 -> fSUM = 31536000 -> Acc = 441763200UL 
15 : Linux_Year = 1984 -> fSUM = 31622400 -> Acc = 473385600UL <- YOON 
16 : Linux_Year = 1985 -> fSUM = 31536000 -> Acc = 504921600UL 
17 : Linux_Year = 1986 -> fSUM = 31536000 -> Acc = 536457600UL 
18 : Linux_Year = 1987 -> fSUM = 31536000 -> Acc = 567993600UL 
19 : Linux_Year = 1988 -> fSUM = 31622400 -> Acc = 599616000UL <- YOON 
20 : Linux_Year = 1989 -> fSUM = 31536000 -> Acc = 631152000UL 
21 : Linux_Year = 1990 -> fSUM = 31536000 -> Acc = 662688000UL 
22 : Linux_Year = 1991 -> fSUM = 31536000 -> Acc = 694224000UL 
23 : Linux_Year = 1992 -> fSUM = 31622400 -> Acc = 725846400UL <- YOON 
24 : Linux_Year = 1993 -> fSUM = 31536000 -> Acc = 757382400UL 
25 : Linux_Year = 1994 -> fSUM = 31536000 -> Acc = 788918400UL 
26 : Linux_Year = 1995 -> fSUM = 31536000 -> Acc = 820454400UL 
27 : Linux_Year = 1996 -> fSUM = 31622400 -> Acc = 852076800UL <- YOON 
28 : Linux_Year = 1997 -> fSUM = 31536000 -> Acc = 883612800UL 
29 : Linux_Year = 1998 -> fSUM = 31536000 -> Acc = 915148800UL 
30 : Linux_Year = 1999 -> fSUM = 31536000 -> Acc = 946684800UL 
31 : Linux_Year = 2000 -> fSUM = 31622400 -> Acc = 978307200UL <- YOON 
32 : Linux_Year = 2001 -> fSUM = 31536000 -> Acc = 1009843200UL 
33 : Linux_Year = 2002 -> fSUM = 31536000 -> Acc = 1041379200UL 
34 : Linux_Year = 2003 -> fSUM = 31536000 -> Acc = 1072915200UL 
35 : Linux_Year = 2004 -> fSUM = 31622400 -> Acc = 1104537600UL <- YOON 
36 : Linux_Year = 2005 -> fSUM = 31536000 -> Acc = 1136073600UL 
37 : Linux_Year = 2006 -> fSUM = 31536000 -> Acc = 1167609600UL 
38 : Linux_Year = 2007 -> fSUM = 31536000 -> Acc = 1199145600UL 
39 : Linux_Year = 2008 -> fSUM = 31622400 -> Acc = 1230768000UL <- YOON 
40 : Linux_Year = 2009 -> fSUM = 31536000 -> Acc = 1262304000UL 
41 : Linux_Year = 2010 -> fSUM = 31536000 -> Acc = 1293840000UL 
42 : Linux_Year = 2011 -> fSUM = 31536000 -> Acc = 1325376000UL 
43 : Linux_Year = 2012 -> fSUM = 31622400 -> Acc = 1356998400UL <- YOON 
44 : Linux_Year = 2013 -> fSUM = 31536000 -> Acc = 1388534400UL 
45 : Linux_Year = 2014 -> fSUM = 31536000 -> Acc = 1420070400UL 
46 : Linux_Year = 2015 -> fSUM = 31536000 -> Acc = 1451606400UL 
47 : Linux_Year = 2016 -> fSUM = 31622400 -> Acc = 1483228800UL <- YOON 
48 : Linux_Year = 2017 -> fSUM = 31536000 -> Acc = 1514764800UL 
49 : Linux_Year = 2018 -> fSUM = 31536000 -> Acc = 1546300800UL 
50 : Linux_Year = 2019 -> fSUM = 31536000 -> Acc = 1577836800UL 
51 : Linux_Year = 2020 -> fSUM = 31622400 -> Acc = 1609459200UL <- YOON 
52 : Linux_Year = 2021 -> fSUM = 31536000 -> Acc = 1640995200UL 
53 : Linux_Year = 2022 -> fSUM = 31536000 -> Acc = 1672531200UL 
54 : Linux_Year = 2023 -> fSUM = 31536000 -> Acc = 1704067200UL 
55 : Linux_Year = 2024 -> fSUM = 31622400 -> Acc = 1735689600UL <- YOON 
56 : Linux_Year = 2025 -> fSUM = 31536000 -> Acc = 1767225600UL 
57 : Linux_Year = 2026 -> fSUM = 31536000 -> Acc = 1798761600UL 
58 : Linux_Year = 2027 -> fSUM = 31536000 -> Acc = 1830297600UL 
59 : Linux_Year = 2028 -> fSUM = 31622400 -> Acc = 1861920000UL <- YOON 
60 : Linux_Year = 2029 -> fSUM = 31536000 -> Acc = 1893456000UL 
61 : Linux_Year = 2030 -> fSUM = 31536000 -> Acc = 1924992000UL 

----------------------------------------------------------- */


#define YOON_YEAR 		(60*60*24*366)  /* aDay[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; */
#define NORM_YEAR 		(60*60*24*365)  /* aDay[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; */
#define LINUX_START_YEAR 		1970  /* reference year 1970.1.1 0:0:0 */
#define LINUX_ACC_COUNT_UNTIL_2018 		1546300800UL /* = 1970 + 1971 + ... 2018 */

int isYearOnce=1;
int isMonthOnce=1;
static unsigned int  Linux_Year = 2019; // LINUX_START_YEAR;
static unsigned __int64 LinuxCnt = LINUX_ACC_COUNT_UNTIL_2018;  /* = 1970 + 1971 + ... 2018 */
static unsigned int  Linux_Month = 0;
static unsigned __int64 dwYear=0UL, dwMonth=0UL, dwDay=0UL;


enum {
	YEAR_ERROR = 0,
	YEAR_NORMAL = 1,
	YEAR_YOON = 2,
};

int isYOONyear(unsigned int year)
{
	int isYOON = 0; // 0:Abnormal, 1:Normal year, 2:YOON year
	unsigned __int64  linuxAcc = 0UL;
	unsigned __int64  oneYearCnt = 0UL;
	
	// ++++++++++++++++++++++++++++++++++++++++
	if( (year)%400 == 0 )	  { isYOON=YEAR_YOON; linuxAcc += YOON_YEAR;	oneYearCnt = YOON_YEAR; }
	else if( (year)%4 == 0 ) 
	{
		if( (year)%100 == 0 ) { isYOON=YEAR_NORMAL; linuxAcc += NORM_YEAR;	oneYearCnt = NORM_YEAR; }
		else				  { isYOON=YEAR_YOON;   linuxAcc += YOON_YEAR;	oneYearCnt = YOON_YEAR; } 
	}
	else					  { isYOON=YEAR_NORMAL; linuxAcc += NORM_YEAR;	oneYearCnt = NORM_YEAR; }
	// ----------------------------------------------

	return isYOON;
}

unsigned __int64 LinuxDate2Number( mjd_timestamp LTime )
{
								 /*   1   2   3   4   5   6   7   8   9  10  11  12 */
	const unsigned short	yDay[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; 
	const unsigned short	aDay[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; 

	unsigned int  dwHour=0, dwMin=0, dwSec=0;
	int  ii=0;
	
	unsigned int iCheck = 0;
	unsigned int fSUM = 0;
	
	/* ---------------------------------------------------
	2. 년도 계산 (평년과 윤년 확인하여야 한다)
	15026/365 = 41
	기준일이 1970년 1월 1일 00:00:00  로  (UNIX 또는 Linux)
	1970 + 41 = 2011년을 위미
	
	3. 일자계산
	15026 - (41*365) = 61일
	날짜를 계산하면
	1월이 31일, 2월이 28일로
	61일째 날은 3월 2일임을 계산할 수 있음
	
	즉 변경일은 2011년 3월 2일임.
	
	1593565932 -> 2020-7-1 10:12:12 
	------------------------------------------------------ */


	// ++ Year ++++++++++++++++++++++++++++++++++++++
	//LinuxCnt = 1546300800UL; /* = 1970 + 1971 + ... 2018 */
	//Linux_Year = 2019;

	if( !dwYear || (LTime.m_year != Linux_Year) ) isYearOnce=1;
	if( isYearOnce )
	{
		isYearOnce = 0;

		do {
			iCheck = 0;

			// ++++++++++++++++++++++++++++++++++++++++
			if( (Linux_Year)%400 == 0 )	  { LinuxCnt += YOON_YEAR;  fSUM = YOON_YEAR;  iCheck = 2; }
			else if( (Linux_Year)%4 == 0 ) 
			{
				if( (Linux_Year)%100 == 0 ) { LinuxCnt += NORM_YEAR;  fSUM = NORM_YEAR;  iCheck = 1; }
				else					    { LinuxCnt += YOON_YEAR;  fSUM = YOON_YEAR;  iCheck = 2; } 
			}
			else						    { LinuxCnt += NORM_YEAR;  fSUM = NORM_YEAR;  iCheck = 1; }
			// ----------------------------------------
			
			dwYear++;
			Linux_Year++;

		} while( Linux_Year < LTime.m_year );
		//EB_Printf(TEXT("[dnw] %u  %u\r\n"), LTime.m_year , Linux_Year);

		dwYear = LinuxCnt;


		//EB_Printf(TEXT("[dnw] %04d/%02d/%02d %02d:%02d:%02d \r\n"), LTime.m_year, LTime.m_month, LTime.m_day, LTime.m_hour, LTime.m_mins, LTime.m_secs);
		//EB_Printf(TEXT("[dnw] %I64u  \r\n"), dwYear);
		//EB_Printf(TEXT("[dnw] %lu  \r\n"), dwYear);
	}

	// ++ Month ++++++++++++++++++++++++++++++++++++++
	if( !dwMonth || (Linux_Month != LTime.m_month) ) isMonthOnce=1;
	if( isMonthOnce )
	{
		Linux_Month = LTime.m_month;

		dwMonth = 0UL;
		isMonthOnce = 0;

	#if 0
		if( (LTime.m_year)%400 == 0 )	  { iCheck = 2; } // YOON_YEAR
		else if( (LTime.m_year)%4 == 0 ) 
		{
			if( (LTime.m_year)%100 == 0 ) { iCheck = 1; } // NORMAL_YEAR
			else				  { iCheck = 2; } // YOON_YEAR
		}
		else					 { iCheck = 1; } // NORMAL_YEAR
	#else
		iCheck = isYOONyear(LTime.m_year);
	#endif
	
		if( YEAR_YOON==iCheck ) // YOON_DAL
		{
			for(ii=0; ii<LTime.m_month-1; ii++)
				dwMonth  += yDay[ii]*24*60*60;
		}
		else if( YEAR_NORMAL==iCheck ) // NORMAL_DAL
		{
			for(ii=0; ii<LTime.m_month-1; ii++)
				dwMonth  += aDay[ii]*24*60*60;
		}
		else
		{
			//EB_Printf(TEXT("[dnw] LINUX Time -> Min ERROR iCheck=(%d) \r\n"), iCheck );
			// Error
		}
	}
	//EB_Printf(TEXT("[dnw] dwMonth %ul \r\n"), dwMonth );


	// ++ Day ++++++++++++++++++++++++++++++++++++++++
	dwDay    = (LTime.m_day)*24*60*60;

	// ++ Hour ++++++++++++++++++++++++++++++++++++++++
	dwHour   = (LTime.m_hour)*60*60; 

	// ++ Minute ++++++++++++++++++++++++++++++++++++++
	dwMin    = (LTime.m_mins)*60;

	// ++ Second ++++++++++++++++++++++++++++++++++++++
	dwSec    = LTime.m_secs;

	// ++++++++++ SUM +++++++++++++++++++++++++++++++++
	LinuxCnt = dwYear + dwMonth + dwDay + dwHour + dwMin + dwSec;


	LinuxCnt -= (24*60*60); // -1Day because of Reference
	LinuxCnt -= (9*60*60); // -9Hour because of Korean TimeZone

	//EB_Printf(TEXT("[dnw] LinuxCnt = %lld  \r\n"), LinuxCnt  );

	return LinuxCnt;
}


void LinuxCount2Date(unsigned __int64 linuxCount, unsigned int *year, unsigned int *month, unsigned int *day, unsigned int *hour, unsigned int *min, unsigned int *sec)
{
								 /*   1   2   3   4   5   6   7   8   9  10  11  12 */
	const unsigned short	yDay[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; // YOON-year
	const unsigned short	aDay[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; // Normal year
	unsigned int  dwyear=0, dwmonth=0, dwday=0;
	unsigned int  dwhour=0, dwmin=0, dwsec=0;
	unsigned int  accDay=0;
	unsigned int  iMonCnt = 0;
	unsigned int  Linux_Year = LINUX_START_YEAR;
	unsigned __int64 linuxCnt = 0UL;
	unsigned int iCheck = 0;
	unsigned int fSUM = 0;
	unsigned int tmpDay=0, ValueDay=0;
	unsigned int iAbnormal = 0; // OK
	/* ---------------------------------------------------
	2. 년도 계산 (평년과 윤년 확인하여야 한다)
	15026/365 = 41
	기준일이 1970년 1월 1일 00:00:00  로  (UNIX 또는 Linux)
	1970 + 41 = 2011년을 위미
	
	3. 일자계산
	15026 - (41*365) = 61일
	날짜를 계산하면
	1월이 31일, 2월이 28일로
	61일째 날은 3월 2일임을 계산할 수 있음
	
	즉 변경일은 2011년 3월 2일임.
	------------------------------------------------------ */


#if 0
	//linuxCount = 1593565932UL; // -> 2020-7-1 10:12:12 
	//linuxCount = 1593566111UL; // -> 2020-7-1 10:15:11 
	//linuxCount = 1593569724UL; // -> 2020-7-1 11:15:24 
	//linuxCount = 1596257795UL; // -> 2020-8-1 13:56:35
	//linuxCount = 1583038595UL; // -> 2020-3-1 13:56:35
	//linuxCount = 1577860190UL; // -> 2020-1-1 15:29:50
	//linuxCount = 1609396190UL; // -> 2020-12-31 15:29:50
	//linuxCount = 1704004190UL; // -> 2023-12-31 15:29:50
	//linuxCount = 1672554590UL; // -> 2023-1-1 15:29:50 
	linuxCount = 1677652190UL; // -> 2023-3-1 15:29:50 
#else

	linuxCnt = LINUX_ACC_COUNT_UNTIL_2018;
	dwyear = 49; // Number : to 2018 from 1970
	Linux_Year = 2019;


	//if( linuxCnt < linuxCount  )
	//EB_Printf(TEXT("[dnw] : %I64d, %I64d \r\n"), linuxCnt , linuxCount );

#endif

	do {
		iCheck = 0;

		// ++++++++++++++++++++++++++++++++++++++++
		if( (Linux_Year)%400 == 0 )	  { linuxCnt += YOON_YEAR;  fSUM = YOON_YEAR;  iCheck = 2; }
		else if( (Linux_Year)%4 == 0 ) 
		{
			if( (Linux_Year)%100 == 0 ) { linuxCnt += NORM_YEAR;  fSUM = NORM_YEAR;  iCheck = 1; }
			else					    { linuxCnt += YOON_YEAR;  fSUM = YOON_YEAR;  iCheck = 2; } 
		}
		else						    { linuxCnt += NORM_YEAR;  fSUM = NORM_YEAR;  iCheck = 1; }
		// ++++++++++++++++++++++++++++++++++++++++
		
		dwyear++;
		Linux_Year++;

	} while( linuxCnt < (linuxCount-YOON_YEAR) );


	//dwyear  = (unsigned int) linuxCount/(60*60*24*365);
	dwday   = (unsigned int)( (linuxCount - linuxCnt)/(60*60*24) );
	dwhour  = (unsigned int)( (linuxCount - linuxCnt)%(60*60*24) );

	dwmin   = dwhour % (60*60);
	dwhour  = dwhour / (60*60);

	dwsec   = dwmin % 60;
	dwmin   = dwmin / 60;

	//dwmsec  = dwsec % 1000;


	//EB_Printf(TEXT("[dnw] : %04d - %04d/%02d/%02d %02d:%02d:%02d \r\n"), Linux_Year, dwyear, dwmonth, dwday, dwhour, dwmin, dwsec );

	iCheck = isYOONyear(Linux_Year);

	iMonCnt = 0;
	accDay = 0;
	do {
		if( YEAR_NORMAL==iCheck ) accDay += aDay[iMonCnt];
		else if( YEAR_YOON==iCheck ) accDay += yDay[iMonCnt];

		if( accDay < dwday ) {
			iMonCnt ++;

			if( iMonCnt >= 12 ) 
			{
				iMonCnt = 0;
				Linux_Year++;

				// +++++++++++++++++++++++++++++++++
				// abnormal +++++++++++++
				if(Linux_Year>2200) { iAbnormal=1; break; }
				// abnormal +++++++++++++
				// +++++++++++++++++++++++++++++++++
				
				iCheck = isYOONyear(Linux_Year);
			}
		}
		else {
			if( YEAR_NORMAL==iCheck ) accDay -= aDay[iMonCnt];
			else if( YEAR_YOON==iCheck ) accDay -= yDay[iMonCnt];

			dwday = dwday - accDay;
			break;
		}
	} while( 1 );

	//EB_Printf(TEXT("[dnw] : %I64d, %I64d  +++++++ 5 iAbnormal=%d \r\n"), linuxCnt , linuxCount, iAbnormal );

	dwmonth = iMonCnt+1;

	//EB_Printf(TEXT("[dnw] :2: %04d - %04d/%02d/%02d %02d:%02d:%02d \r\n"), Linux_Year, dwyear, dwmonth, dwday, dwhour, dwmin, dwsec );

	if( YEAR_NORMAL==iCheck ) 
	{
		if( dwday+1 > aDay[dwmonth-1] ) 
		{ 
			dwday -= aDay[dwmonth-1]; 
			dwmonth++; 
		}
	}
	else if( YEAR_YOON==iCheck )  
	{
		if( dwday+1 > yDay[dwmonth-1] ) 
		{ 
			dwday -= yDay[dwmonth-1]; 
			dwmonth++; 
		}
	}
	/* 윤년 계산 => 2월: 29Day */

	//EB_Printf(TEXT("[dnw] : %I64d, %I64d  +++++++ 6 \r\n"), linuxCnt , linuxCount );

	//EB_Printf(TEXT("[dnw] :3: %04d - %04d/%02d/%02d %02d:%02d:%02d \r\n"), Linux_Year, dwyear, dwmonth, dwday, dwhour, dwmin, dwsec );
	if( dwmonth > 12 ) // 2020.07.03
	{
		dwmonth    -= 12;
		Linux_Year += 1;
	}
		
	*year  = Linux_Year;
	*month = dwmonth;

	*day   = 1+dwday;

	*hour  = 9+dwhour; // Because of Korean TimeZone
	*min   = dwmin;
	*sec   = dwsec;

	//EB_Printf(TEXT("[dnw] : %I64d, %I64d  +++++++ 7 \r\n"), linuxCnt , linuxCount );

}


void MenuAbout(HWND hwnd)
{
	int dwTotal, dwFree, dwUse;
	int dwOnce = 1;
	ULONG xSWInstallDate = 0L;
	TCHAR szLenTxt[MAX_PATH] = {0,};
	//SYSTEMTIME OSInstallDate;
	unsigned int  dwyear=0, dwmonth=0, dwday=0;
	unsigned int  dwhour=0, dwmin=0, dwsec=0;	
	// __int64 dCPURate;
#define INFO_BUFFER_SIZE 		MAX_PATH // 32767
	
	TCHAR  infoBuf[INFO_BUFFER_SIZE] = {0,};
	DWORD  bufCharCount = INFO_BUFFER_SIZE;

	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n") );
	//MISRA_test();

	GetProcessorName();
	EB_Printf(TEXT("[dnw] Processor Name ----: %s \r\n"), Cpu_info );
	EB_Printf(TEXT("[dnw] CPU Identifier ----: %s \r\n"), Cpu_ID );
	EB_Printf(TEXT("[dnw] Product Name ------: %s (%s) \r\n"), ProductName, Manufacture );

	EB_Printf(TEXT("[dnw] OS Info. ----------: %s (%s) \r\n"), SWProductName, SWEditionID );
	EB_Printf(TEXT("[dnw] OS Release ID -----: %s (Build#:%s) \r\n"), SWReleaseId, SWCurrentBuildNumber );


	if( dwOnce )
	{
		LinuxCount2Date(SWInstallDate, &dwyear, &dwmonth, &dwday, &dwhour, &dwmin, &dwsec);
		dwOnce = 0;
	}
	
	EB_Printf(TEXT("[dnw] OS Install Date ---: %04d/%02d/%02d %02d:%02d:%02d (Linux type:%I64u 0x%x) \r\n"), dwyear, dwmonth, dwday, dwhour, dwmin, dwsec, SWInstallDate, SWInstallDate );
//	EB_Printf(TEXT("[dnw] OS Install Time ---: %I64u 0x%x \r\n"), SWInstallTime, SWInstallTime );


	EB_Printf(TEXT("[dnw] BIOS Version ------: %s \r\n"), BIOSVersion );
	EB_Printf(TEXT("[dnw] BIOS Vendor -------: %s \r\n"), BIOSVendor_ );
	EB_Printf(TEXT("[dnw] BIOS Release Date -: %s \r\n"), BIOSRelease );

	//EB_Printf(TEXT("[dnw] OS Info. -2--------: %s \r\n"), os_name() );

//	EB_Printf(TEXT("[dnw] IP Address --------: %s \r\n"), GetIPaddress() );


#if 0
	PostMessage( GetDesktopWindow(), WM_SYSCOMMAND, SC_MONITORPOWER, 2 ); // power shut off
	PostMessage( GetDesktopWindow(), WM_SYSCOMMAND, SC_MONITORPOWER, -1 ); // power recover
	PostMessage( GetDesktopWindow(), WM_SYSCOMMAND, SC_MONITORPOWER, 1 ); // power save mode
#endif	


	dwTotal = getTotalRAM();
	dwFree  = getAvailRAM();
	dwUse   = dwTotal - dwFree;
	EB_Printf(TEXT("[dnw] RAM (MB) ----------: Total(%d) / Free(%d) / Used(%d) \r\n"), dwTotal, dwFree, dwUse );
	//EB_Printf(TEXT("[dnw] Total RAM: %u / %u / %u / %u \n"), getTotalRAM(), getAvailRAM(), getTotalMemory(), getAvailMemory() );

	#if USE_WIN_OUTOF_AREA_TO_ZERO // 2016.03.31 LCD size
	myLCDwidth	= GetSystemMetrics(SM_CXSCREEN);
	myLCDheight = GetSystemMetrics(SM_CYSCREEN);


	myVirWidth	= GetSystemMetrics(SM_CXVIRTUALSCREEN);
	myVirHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	EB_Printf(TEXT("[dnw] LCD size ----------: %dx%d, Cur(%dx%d; %dx%d) Vir[%dx%d] \r\n"), myLCDwidth, myLCDheight, WIN_XBgn, WIN_YBgn, WIN_XSIZE, WIN_YSIZE, myVirWidth, myVirHeight );
	#endif

	// -----------------------------------------------------
	// Get and display the name of the computer. 
	bufCharCount = INFO_BUFFER_SIZE;
	GetComputerName( infoBuf, &bufCharCount );
	EB_Printf(TEXT("[dnw] Computer Name -----: %s \r\n"), infoBuf );

	// Get and display the user name. 
	bufCharCount = INFO_BUFFER_SIZE;
	GetUserName( infoBuf, &bufCharCount );
	EB_Printf(TEXT("[dnw] User Name ---------: %s \r\n"), infoBuf );

	// Get and display the system directory. 
	GetSystemDirectory( infoBuf, INFO_BUFFER_SIZE );
	EB_Printf(TEXT("[dnw] System Folder -----: %s \r\n"), infoBuf );

	// Get and display the Windows directory. 
	//GetWindowsDirectory( infoBuf, INFO_BUFFER_SIZE );
	//EB_Printf(TEXT("[dnw] Windows Folder : %s \r\n"), infoBuf );
	// -----------------------------------------------------


	#if 0
	EB_Printf(TEXT("[dnw] sizeof(char) = %d \r\n"), sizeof(char) );
	EB_Printf(TEXT("[dnw] sizeof(TCHAR) = %d \r\n"), sizeof(TCHAR) );
	EB_Printf(TEXT("[dnw] sizeof(wchar_t) = %d \r\n"), sizeof(wchar_t) );
	EB_Printf(TEXT("[dnw] sizeof(long) = %d \r\n"), sizeof(long) );
	EB_Printf(TEXT("[dnw] sizeof(__int64) = %d \r\n"), sizeof(__int64) );
	EB_Printf(TEXT("[dnw] sizeof(ULONG) = %d \r\n"), sizeof(ULONG) );
	EB_Printf(TEXT("[dnw] sizeof(long long) = %d \r\n"), sizeof(long long) );
	#endif
	
	EB_Printf(TEXT("[dnw] Version  ----------: %s [Build: %s, %s]%s(%d) \r\n"), APPNAME, __DATE__ , __TIME__ , BUILD_MSC_VER, _MSC_VER ); // , _MSC_VER);
	EB_Printf(TEXT("[dnw] Email addr. -------: %s \r\n"), REG_AUTHOR1_ID_TEXT );
	EB_Printf(TEXT("[dnw] NAVER Cafe --------: https://cafe.naver.com/topjoo \r\n") );
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n") );





#if 0
/* 
 (1) %*d - 숫자가 출력될 너비를 런타임에서 조절
     %2d나 %5d와 같은 너비 크기 조절을 런타임에서 하고자 할때, *를 넣으면 된다.
     %*d를 쓰면 인자를 두개를 사용한다. 처음 인자가 너비이고 두번째 인자는 원래 수이다.

 (2) %.*s - substring!
     str함수를 이용해야 하는 문자열 잘라내기를 printf내에서 할 수 있다!
     그리고, 길이만 안다면 NULL로 끝나지 않는 문자열에 대해서도 잘라낼 수 있다.

     printf("%.*s\n", sublen, str) 
     이렇게 하면, str의 왼쪽에서 sublen만큼만 출력할 수 있다.

     s앞에 .을 붙이면, .뒤에 숫자를 붙임으로써 문자열이 출력되는 갯수를 정할 수 있다.
     (예: %.5s는 문자열이 몇개가 오든 5개만 출력한다는 소리)
     그런데 s앞에 *도 붙이면, .의 뒤에 와야할 숫자를 printf의 인자로 줄 수 있다.

     그래서 마치 printf를 substring함수와 같이 쓸 수 있는 것이다.
     추가로, printf의 인자로 직접 정한 숫자보다 더 작은 문자열이 들어오면, 인자로 준 숫자는 그냥 무시된다.

  (3) %.0d - 0일때 출력 안하기
      값이 0일때는 출력을 안하고싶은 때가 있을거다.
      그럴때 if문으로 분기해서 일일이 처리했다면, 이것이 반가울것이다.
      바로 %와 d사이에 .을 붙임으로 해결된다.

      printf ("%.d", 0);

  (4) %#x - 0x를 붙여주는 hex표현
      0x를 앞에다 출력하고 싶으면, %x 대신에 %#x를 쓰면 된다.

*/


{
	int i;
	char* str = "1234567890";
	int dectest = 1234;

	for (i = 0; i <= 10; i++)
	{
		EB_Printf(TEXT("[dnw] %*d \n"), i,dectest );	
	}

	EB_Printf(TEXT("[dnw] \r\n\r\n") );	

	for (i = 0; i <= 10; i++)
	{
		EB_Printf(TEXT("[dnw] %.*sㅗ"), 10-i,str+i );	
		EB_Printf(TEXT("%.*s\n"), i,str );	

		//printf ("%.*sㅗ", 10-i,str+i);
		//printf ("%.*s\n", i,str);
	}

	EB_Printf(TEXT("[dnw] \r\n\r\n") ); 

	for (i = -5; i <= 5; i++)
	{
		EB_Printf(TEXT("[dnw] %.d \r\n"), i );	
	}
	EB_Printf(TEXT("[dnw] \r\n\r\n") ); 

	for (i = 0; i <= 20; i++)
	{
		EB_Printf(TEXT("[dnw] hex: %02x - %02X - %#x - %#X \r\n"), i, i, i, i );	
	}
	
}

#endif



#if CIPHER_RSA2048 // 2018.02.20

	RSA2048();
#endif

#if 0 /// 2015.05.11
	GetComPortRegistry();

	EB_Printf(TEXT("\r\n") );
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------------- \n") );
#endif
	


//	if( GetAsyncKeyState(VK_F1) & 0x8000 )
//		EB_Printf(TEXT("[dnw] --------------------------------------------------- \n") );
		
#if 0
    MessageBox(hwnd,TEXT("Serial Console with Serial/USB Download\n\n")
		    TEXT("1. e-mail: mobilesol.cs@samsung.com\n")
	  	    TEXT("2. USB Tx format: addr(4)+size(4)+data(n)+cs(2)   \n")
		    TEXT("3. Serial Tx format: size(4)+data(n)+cs(2)\n\n")
		    TEXT("For WinCE Version, WinCE 5.0, WinCE 6.0, \n")
			TEXT("Windows Mobile 5.0, Windows Mobile 6.0 UBOOT is supported\n")
			TEXT("About ")APPNAME TEXT(" @Build:")__DATE__ , MB_OK | MB_ICONINFORMATION );

#endif


#if 0
	int dwTotal, dwFree, dwUse;

	TCHAR szText[2048] = {0,};
	TCHAR szTemp[MAX_PATH] = {0,};
	
	wsprintf(szText, TEXT("OS Info. : %s \r\n"), os_name() );

	dwTotal = getTotalRAM();
	dwFree  = getAvailRAM();
	dwUse   = dwTotal - dwFree;
	wsprintf(szTemp, TEXT("RAM (MB) : Total(%d)/ Free(%d)/ Used(%d) \r\n"), dwTotal, dwFree, dwUse );
	lstrcat(szText, szTemp);

#if USE_WIN_OUTOF_AREA_TO_ZERO // 2016.03.31 LCD size
	myLCDwidth	= GetSystemMetrics(SM_CXSCREEN);
	myLCDheight = GetSystemMetrics(SM_CYSCREEN);

	wsprintf(szTemp, TEXT("LCD size : %d x %d, Curr Pos(%d x %d) \r\n"), myLCDwidth, myLCDheight, WIN_XBgn, WIN_YBgn );
	lstrcat(szText, szTemp);
#endif

	wsprintf(szTemp, TEXT("Version : %s [Build: %s, %s] \r\n"), APPNAME, __DATE__ , __TIME__ );
	lstrcat(szText, szTemp);

	wsprintf(szTemp, TEXT("Email : %s \r\n"), REG_AUTHOR1_ID_TEXT );
	lstrcat(szText, szTemp);

	MessageBox(hwnd, szText, TEXT("About DNW"), MB_OK | MB_ICONINFORMATION );


#endif
}




int GetIsConnect(void)
{
	return isConnected;
}

void Quit(HWND hwnd)
{
    CloseComPort();
}


// ------------------------
// Setting ----
// ------------------------
BOOL MenuConnect(HWND hwnd)
{
	BOOL 	retVal = FALSE;
	
	if( 1 == isConnected ) 
	{
		CloseComPort();
	}

#ifdef SERIAL_CONNECTING_DISPLAY /// 2012.07.05, connecting 
	isConnected = 2; /// Try to connecting
	UpdateWindowTitle();
#endif

	retVal = OpenComPort(userComPort, MSG_OFF);
	if( FALSE == retVal )
	{
		// EB_Printf(TEXT("[dnw] can not connect COM%d port in MenuConnect. (L:%d, V:%d) \n"), userComPort, __LINE__ , isConnected );

		if(1==isConnected) /// TRUE
		{
			CloseComPort();
		}

#ifdef SERIAL_CONNECTING_DISPLAY /// 2012.07.05, connecting 
		isConnected = 2; /// Try to connecting
		UpdateWindowTitle();
#endif

		Sleep(10);
		retVal = OpenComPort(userComPort, MSG_ON);
		if( FALSE == retVal )
		{
			UpdateWindowTitle();
		}
	}
	return retVal;

}


void MenuDisconnect(HWND hwnd)
{
	if( 1==isConnected ) /// TRUE
	{
		CloseComPort();
	}
}


/* F2 key */
BOOL MenuConnectDisconnect(HWND hwnd)
{
	BOOL 	retVal = TRUE;

	if( 1==isConnected ) 
	{
		CloseComPort();
		retVal = TRUE;
		return TRUE;
	}
	else if( 0==isConnected ) 
	{
#ifdef SERIAL_CONNECTING_DISPLAY /// 2012.07.05, connecting 
		isConnected = 2; /// Try to connecting
		UpdateWindowTitle();
#endif
		retVal = OpenComPort(userComPort, MSG_OFF);
		if( TRUE == retVal )
		{
			UpdateWindowTitle();
		}
		else if( FALSE==retVal )
		{
#ifdef SERIAL_CONNECTING_DISPLAY /// 2012.07.05, connecting 
			isConnected = 2; /// Try to connecting
			UpdateWindowTitle();
#endif
			Sleep(50);
			retVal = OpenComPort(userComPort, MSG_ON);
			if( TRUE == retVal )
			{
				UpdateWindowTitle();
			}
			else if( FALSE == retVal )
			{
#ifndef SERIAL_CONNECTING_DISPLAY /// 2012.07.05, connecting 
				DWORD dwError = GetLastError();
				EB_Printf(TEXT("[dnw] can not connect COM%d port in MenuConnectDisconnect. (%s) (Error=%u) \r\n"), userComPort, APP_VER, dwError );
#endif
			}
		}
	}
	return retVal;
}





/* 1000 msec 마다 checking & Title update */
void UpdateWindowTitle(void)
{
    TCHAR title[FILENAMELEN] = {0,};
    TCHAR tch[MAX_PATH] = {0,};  // 100 -> MAX_PATH
    
    static int prevComPort=0xff;
    static int prevBaudRate=0xff;
    static int prevUsbAlive=0xff;
    static int prevIsConnected=0xff;
	static int prevautoSendKey=0xff; /// added.
	static int prevmsgSaveOnOff=0xff; /// 2011.12.05
	static TCHAR prevDownAddr[16] = {0,};
    int usbAlive=0;
	static int prevuserCOMparity=0xff, prevuserCOMstop=0xff, prevuserCOMdata=0xff, prevLocalTime=0xff;
	static int prevFlowControl=0xff, prevFlowCtrlVal=0xff; // 2017.08.03 Flow Control
	static int prevCPUtype=0xff; /// 2014.04.11, C100, 6410 USB Address.
#if DISPLAY_MAX_EDIT_BUF_SIZE // 2016.02.12
	static unsigned int prevEditCount=0xff;
#endif
#if USER_SCREEN_BUF_SIZE // 2016.02.14
	static int prevuserScrBufSizIndex=0xff;
#endif

#if USE_TXDATA_TO_SERIAL
	static int prevuserisTxData2UART=0xff;
#endif


#if 0
	DWORD ModemStat; // 2017.08.03
	static DWORD prevModemStat=0xffff;


	GetCommModemStatus(idComDev, &ModemStat); // 2017.08.03
#endif
	


	///////////////// USB port : One more /////////////////
	if(IsUsbConnected())
	{
		///lstrcat(title,TEXT(" [USB:OK]"));
		usbAlive=1;
	}
	else
	{
		///lstrcat(title,TEXT(" [USB:x]"));
		usbAlive=0;
	}

#if 0
	EB_Printf(TEXT("[dnw] %d %d %d %d %d %d  \r\n"), userComPort, prevComPort, userBaudRate, prevBaudRate, usbAlive, prevUsbAlive );
	EB_Printf(TEXT("[dnw] %d %d %d %d %d %d  \r\n"), isConnected, prevIsConnected, autoSendKey, prevautoSendKey, msgSaveOnOff , prevmsgSaveOnOff );
	EB_Printf(TEXT("[dnw] %d %d %d %d %d %d  \r\n"), autoSendKey, prevautoSendKey , msgSaveOnOff , prevmsgSaveOnOff, userCOMparity , prevuserCOMparity );
	EB_Printf(TEXT("[dnw] %d %d %d %d %d %d  \r\n"), userCOMstop , prevuserCOMstop , userCOMdataIdx , prevuserCOMdata	, CPUtype , prevCPUtype );
	EB_Printf(TEXT("[dnw] %d %d %d %d %d %d  \r\n"), userFlowControl , prevFlowControl , userFlowCtrlVal , prevFlowCtrlVal, userScrBufSizIndex ,  prevuserScrBufSizIndex );
	EB_Printf(TEXT("[dnw] %d %d   \r\n"), isTxData2UART , prevuserisTxData2UART);
#endif


    if(userComPort!=prevComPort || userBaudRate!=prevBaudRate || usbAlive!=prevUsbAlive 
		|| isConnected!=prevIsConnected 
		|| (0 != lstrcmp(prevDownAddr,szDownloadAddress)) // 2017.08.04, Bug Fix, Speed up!!!
		|| autoSendKey !=prevautoSendKey || msgSaveOnOff != prevmsgSaveOnOff
		|| userCOMparity != prevuserCOMparity
		|| userCOMstop != prevuserCOMstop
		|| userCOMdataIdx != prevuserCOMdata	
		|| CPUtype != prevCPUtype /// 2014.04.11
		|| userFlowControl != prevFlowControl // 2017.08.03
		|| userFlowCtrlVal != prevFlowCtrlVal // 2017.08.04
		
	#if DISPLAY_MAX_EDIT_BUF_SIZE // 2016.02.12
		|| iEditCount != prevEditCount 
	#endif

	#if USER_SCREEN_BUF_SIZE // 2016.02.14
		|| userScrBufSizIndex != prevuserScrBufSizIndex
	#endif

	#if USE_TXDATA_TO_SERIAL
		|| isTxData2UART != prevuserisTxData2UART
	#endif
      )
    {

		// --------------------------------------
		// --------- COM port -------------------
		// --------------------------------------
		
		lstrcpy(title,APPNAME);
		// lstrcat(title,TEXT("  [COM"));
	
		if(1 == isConnected ) // TRUE, serial connected : O.K.
		{
	
	#if COM_SETTINGS /// 2012.07.11

			wsprintf(tch, TEXT("  [COM%d,%dbps,%d-%s-%s,"), 
				userComPort, userBaudRate, COMportDatabit[userCOMdataIdx], szCOMParity[userCOMparity], szCOMStop[userCOMstop] );
			lstrcat(title, tch);
	#else

			wsprintf(tch, TEXT("  [COM%d,%dbps]"), userComPort, userBaudRate );
			lstrcat(title, tch);
	#endif

	#if 1 // 2017.08.04 None FlowControl
			//if( MS_CTS_ON==(ModemStat & MS_CTS_ON) ) lstrcat(title,TEXT("-[CTS]"));
			//if( MS_DSR_ON==(ModemStat & MS_DSR_ON) ) lstrcat(title,TEXT("-[DSR]"));
			//if( MS_RING_ON==(ModemStat & MS_RING_ON) ) lstrcat(title,TEXT("-[RING]"));
			//if( MS_RLSD_ON==(ModemStat & MS_RLSD_ON) ) lstrcat(title,TEXT("-[RLSD]"));

			//EB_Printf(TEXT("[dnw] userFlowControl [%x  %x] \r\n"), userFlowControl, userFlowCtrlVal );
			if( userFlowControl && userFlowCtrlVal )
			{
				if( FLOWC_DTR_DSR == (userFlowCtrlVal & FLOWC_DTR_DSR) )
				{
					if( (FLOWC_RTS_CTS==(userFlowCtrlVal & FLOWC_RTS_CTS)) || FLOWC_XON_XOFF==(userFlowCtrlVal & FLOWC_XON_XOFF) ) 
						lstrcat(title,TEXT("DTR/"));
					else
						lstrcat(title,TEXT("DTR"));
				}

				if( FLOWC_RTS_CTS == (userFlowCtrlVal & FLOWC_RTS_CTS) ) 
				{
					if( FLOWC_XON_XOFF==(userFlowCtrlVal & FLOWC_XON_XOFF) ) 
						lstrcat(title,TEXT("RTS/"));
					else
						lstrcat(title,TEXT("RTS"));
				}

				if( FLOWC_XON_XOFF == (userFlowCtrlVal & FLOWC_XON_XOFF) ) 
					lstrcat(title,TEXT("Xon"));

			}
			else
			{
				lstrcat(title,TEXT("None"));
			}

			lstrcat(title,TEXT("]"));

			prevFlowControl 	  = userFlowControl; // 2017.08.04
			prevFlowCtrlVal       = userFlowCtrlVal;
	#endif


		}
	#ifdef SERIAL_CONNECTING_DISPLAY /// 2012.07.05, connecting 
		else if(2 == isConnected)
		{
		#if 0
			lstrcat(title,TEXT("  [COM"));
			if(userComPort<10)
			{
				tch[0]='0'+userComPort;
				tch[1]='\0';
			}
			else
			{	/* : ; < = > ? @ */
				tch[0]='0'+(userComPort/10);
				tch[1]='0'+(userComPort%10);
				tch[2]='\0';
			}
	
			lstrcat(title,tch);
			lstrcat(title,TEXT(" -> connecting...]"));

		#else
			wsprintf(tch, TEXT("  [COM%d,%dbps -> connecting...]"), userComPort, userBaudRate );
			lstrcat(title, tch);
		#endif
		}
	#endif
		else
		{
			lstrcat(title,TEXT("  [COM:x]"));
		}



		///////////////// USB port : One more /////////////////
		if(IsUsbConnected())
		{
			lstrcat(title,TEXT(" [USB:OK]"));
			usbAlive=1;

		#if 1 // 2015.12.16. Move to here
			////////////// RAM address ///////////////////
			memset( tch, 0x00, sizeof(tch) );
			wsprintf(tch, TEXT(" [Addr:%s]"), szDownloadAddress );
			lstrcat(title, tch);
		#endif
		
		}
		else
		{
			lstrcat(title,TEXT(" [USB:x]"));
			usbAlive=0;
		}


	#if 0 // 2015.12.16 - USB OK
		////////////// RAM address ///////////////////
		lstrcat(title,TEXT(" [Addr:"));
		lstrcat(title,szDownloadAddress );	 //_itot :TCHAR version of itoa 
		lstrcat(title,TEXT("]"));
	#endif


		//The bug of WIN2K WINAPI is found.
		//The SetWindowText()(also,WM_SETTEXT message) consumes 4KB system memory every times.
		//I think there is some bug in SetWindowText API.
		//In my case, because SetWindowText() is called every 1 seconds, 
		//the system memory used by DNW.exe is increased by 4KB every 1 seconds.
		//For emergency fix, SetWindowText will called only when its content is changed.
		//NOTE. This memory leakage is flushed when window is shrinked.


	#if DISPLAY_MAX_EDIT_BUF_SIZE // 2016.02.12
		if( iEditCount > 0 )
		{
			memset( tch, 0x00, sizeof(tch) );
			wsprintf(tch, TEXT(" (%d*%d)"), iEditCount, iLineDeleted );
			lstrcat(title, tch);
		}
	#endif


	#if USER_SCREEN_BUF_SIZE // 2016.02.14
		switch(userScrBufSizIndex)
		{
			case 0:
				lstrcat(title, TEXT("-[Huge]") );
				break;
			case 1:
				lstrcat(title, TEXT("-[Large]") );
				break;
			case 2:
				lstrcat(title, TEXT("-[Middle]") );
				break;
			case 3:
				lstrcat(title, TEXT("-[Small]") );
				break;
			case 4:
				lstrcat(title, TEXT("-[Smallest]") );
				break;
			default:
				lstrcat(title, TEXT("-[Huge]") );
				break;
		}
	#endif


	#if HEXA_MODE_SUPPORT	/* 2011.02.08 Hexa mode display */
		if(1==TextorHexaMode)
		{
			if(!ColumnNumber)
			{
				lstrcat(title,TEXT("-[Hexa]"));
			}
			else
			{
			#if 0
				memset( tch, 0x00, sizeof(tch) );
				lstrcat(title,TEXT("("));
				lstrcat(title,_itot(ColumnNumber,tch,10) );   //_itot :TCHAR version of itoa 
				lstrcat(title,TEXT(")]"));
			#else
				memset( tch, 0x00, sizeof(tch) );
				wsprintf(tch, TEXT("-[Hexa Col(%d)]"), ColumnNumber );
				lstrcat(title, tch);
			#endif
			}
		}
		else if(2==TextorHexaMode) // 2016.04.12, End char
		{

	#if 1 // 2018.3.26
			memset( tch, 0x00, sizeof(tch) );

			// Hexa End Char Mode
			if( HexaEndChar > 0xFFFF ) // 2018.03.26
				wsprintf(tch, TEXT("-[Hexa LF(0x%06x)]"), HexaEndChar );
			else if( HexaEndChar > 0xFF ) // 2018.03.26
				wsprintf(tch, TEXT("-[Hexa LF(0x%04x)]"), HexaEndChar );
			else
				wsprintf(tch, TEXT("-[Hexa LF(0x%02x)]"), HexaEndChar );

			lstrcat(title, tch);
	#else
			memset( tch, 0x00, sizeof(tch) );
			wsprintf(tch, TEXT("-[Hexa EndCh(0x%02X)]"), HexaEndChar );
			lstrcat(title, tch);
	#endif

		}
	#endif



	#if USE_TXDATA_TO_SERIAL
		if( isTxData2UART ) {
			     if( TX_TYPE_CHAR==sendTxDataType ) lstrcat(title, TEXT("-[TxData-Char] >>>"));
			else if( TX_TYPE_HEXA==sendTxDataType ) lstrcat(title, TEXT("-[TxData-Hexa] >>>"));
			else if( TX_TYPE_FILE==sendTxDataType ) lstrcat(title, TEXT("-[TxData-File] >>>"));
			else if( TX_TYPE_SCRIPT==sendTxDataType ) {
				memset( tch, 0x00, sizeof(tch) );
				wsprintf(tch, TEXT("-[TxData-Script:%u] >>>"), TxScriptCommandLines );
				lstrcat(title, tch);
			}
			else if( TX_TYPE_RANDOM==sendTxDataType ) lstrcat(title, TEXT("-[TxData-Rand] >>>")); // 2018.03.23
			else if( TX_TYPE_SEQUENTIAL==sendTxDataType ) lstrcat(title, TEXT("-[TxData-Seq] >>>")); // 2018.03.26
			
			else lstrcat(title,TEXT("-[TxData] >>>"));
		}
	#endif


		// ---------------------------------------------
		// automatic flag 
		if( autoSendKey )
			lstrcat(title,TEXT("-[AutoSend]"));

		// ---------------------------------------------
		// Log save		
		if( msgSaveOnOff )
			lstrcat(title,TEXT("-[Logging]"));

	
		SetWindowText(_hwnd, title); // update title


		prevComPort        = userComPort;
		prevBaudRate       = userBaudRate;
		prevUsbAlive       = usbAlive;
		prevIsConnected    = isConnected;
		lstrcpy(prevDownAddr, szDownloadAddress);  /* strcpy() -> lstrcpy() */
		prevautoSendKey    = autoSendKey;
		prevmsgSaveOnOff   = msgSaveOnOff;

		prevuserCOMparity  = userCOMparity;
		prevuserCOMstop    = userCOMstop;
		prevuserCOMdata    = userCOMdataIdx;
		prevLocalTime      = localTime;
		prevCPUtype        = CPUtype;

	#if DISPLAY_MAX_EDIT_BUF_SIZE // 2016.02.12
		prevEditCount      = iEditCount; // 2016.02.12
	#endif

	#if USER_SCREEN_BUF_SIZE // 2016.02.14
		prevuserScrBufSizIndex = userScrBufSizIndex;
	#endif
	
	#if USE_TXDATA_TO_SERIAL // 2016.03.29 - File Tx --
		prevuserisTxData2UART = isTxData2UART;
	#endif


    }

}



void MenuTransmit(HWND hwnd)
{
	HANDLE hFile = NULL;
	// DWORD fileSize=0, i=0;
	unsigned short cs=0;
	BOOL result = FALSE;
	uintptr_t threadResult;
	BOOL rdRet = FALSE;


	if( 0 == isConnected ) // 2016.10.11, If disconected!!
		SendMessage( _hwnd, WM_COMMAND, CM_CONNECT, 0 ); // try to connect to COM!

	
	if(!isConnected)
	{
		EB_Printf(TEXT("[dnw] COM%d port can not open on MenuTransmit! \r\n"), userComPort );
		return;
	}

	if( (1==isMenuTxing) && (NULL != _hDlgDownloadProgressUART) )
	{
		// EB_Printf(TEXT("[dnw] Now File[%s] is transfering to COM%d port. Wait!!! \r\n"), szFileNameOnPopUP, userComPort );
		ShowWindow(_hDlgDownloadProgressUART, SW_SHOWNORMAL); // SW_MINIMIZE); // SW_SHOW|SW_RESTORE); // SW_MAXIMIZE); 
		//EB_Printf(TEXT("[dnw] Window is already running... \r\n") );

		return;
	}


	memset( szFileName, 0x00, sizeof(szFileName) );
	memset( szTitleName, 0x00, sizeof(szTitleName) );
	result = PopFileOpenDlg(hwnd,szFileName,szTitleName);

	if( 0 == result ) //file open fail
	{
		DWORD dwError = GetLastError();
		EB_Printf(TEXT("[dnw] can not open PopFileOpenDlg! FileName:[%s],TitleName:[%s], Error:%u \r\n"), szFileName, szTitleName, dwError );
		return;
	}


#if 1 // 2016.10.11, POPUP_FileName_Only
	{
		extern TCHAR szFileNameOnPopUP[FILENAMELEN];
		TCHAR szFnLen[MAX_PATH] = {0,};
		int iLen = 0, jdx=0;
		HANDLE hPopUpFileNameOnly=NULL;
		
		iLen = strlen(szFileName);
		if( iLen > MAX_PATH )
		{
			EB_Printf(TEXT("\r\n[dnw] Input file length is over! length(%d) \r\n"), iLen );
			BeepSound(4);
		}
	
		for(jdx=iLen; iLen>0; jdx--) { if( '\\' == szFileName[jdx] ) break; }
	
		lstrcpy( szFileNameOnPopUP, &szFileName[jdx+1] ); // input filename

		// -----------------------------------------------------------------
		// 3. File Read and size
		// -----------------------------------------------------------------
		hPopUpFileNameOnly = CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hPopUpFileNameOnly || NULL==hPopUpFileNameOnly )
		{
			EB_Printf(TEXT("[dnw] can not open file[%s]. Err(%u) \r\n"), szFileName, GetLastError() );
			return;
		}
		iUARTTxFileSize = GetFileExSize(hPopUpFileNameOnly);
		if(hPopUpFileNameOnly)  { CloseHandle(hPopUpFileNameOnly);  hPopUpFileNameOnly=NULL; }
	
		//wsprintf(szFnLen, TEXT(" (%I64d Byte)"), iUARTTxFileSize);
		//lstrcat( szFileNameOnPopUP, szFnLen );

	}
#endif


#if 0 // 2016.10.19
	hFile=CreateFile(szFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);

	if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
	{
		DWORD dwError = GetLastError();
		EB_Printf(TEXT("[dnw] can not create File. FileName:[%s], Error:%u \n"), szFileName, dwError );
		return;
	}

	fileSize = GetFileSize(hFile,NULL);

	if(SerialTxBuf) { free((void*)SerialTxBuf); SerialTxBuf=NULL; } 

	SerialTxBuf=(char *)malloc(fileSize+6);
	if( NULL==SerialTxBuf )
	{
		DWORD dwError = GetLastError();
		EB_Printf(TEXT("[dnw] can not allocate the serial Tx buffer memory. (Size:%d, Error:%u) \n"), fileSize+6,  dwError  );
		if(hFile) { CloseHandle(hFile); hFile=NULL; } 
		return;
	}

	rdRet = ReadFile(hFile,(void *)(SerialTxBuf+4),fileSize,&TxBufSize,NULL);
	if( (TxBufSize!=fileSize) || !rdRet )
	{
		if( !rdRet )
		{
			DWORD dwError = GetLastError();
			EB_Printf(TEXT("[dnw] find some error while ReadFile. (%s) (rdRet:%d, Error:%u) \n"), APP_VER, rdRet,  dwError );
		}
		else
		{
			EB_Printf(TEXT("[dnw] find some error while ReadFile. (%s) (Size:%d) \n"), APP_VER, fileSize );
		}
		
		if(hFile) { CloseHandle(hFile); hFile=NULL; }
		if(SerialTxBuf) { free((void*)SerialTxBuf); SerialTxBuf=NULL; }
		return;
	}

	*((DWORD *)SerialTxBuf)=fileSize+6;   //attach filesize(n+6) 

	for(i=4;i<(fileSize+4);i++)
		cs+=(BYTE)(SerialTxBuf[i]);

	*((WORD *)(SerialTxBuf+4+fileSize))=cs;   //attach checksum 

	TxBufSize+=6;
	idxTxBuf=0;

	if(hFile) { CloseHandle(hFile);  hFile=NULL; }
#endif



	isTxUARTDataExit = 0; // Start Tx Data in MENU..


#if 0 // 2016.10.12
	threadResult = _beginthread( (void (*)(void *))TxFile,  /* void( __cdecl *start_addr)(void*) */
							THREAD_STACK_SIZE,		   /* stack size */
							(void *)NULL			   /* arglist */
						  );

#else
	//EB_Printf(TEXT("[dnw] cs=%x\n"),cs);
	threadResult = _beginthread( (void (*)(void *))TxFileNew,  /* void( __cdecl *start_addr)(void*) */
    							THREAD_STACK_SIZE,         /* stack size */
    							(void *)NULL               /* arglist */
    						  );
#endif


	// -------------------------------------------------------------------
	// _beginthread 함수의 경우 성공일 때 0, 실패일 때 -1을 리턴 ---
	// -------------------------------------------------------------------
	if( -1 != threadResult )
	{
		// Sleep(WAIT_TIME_USB_DOWNLOAD); // 2013.03.25 
		
		//Create the download progress dialogbox.

		CreateDialog(_hInstance,MAKEINTRESOURCE(IDD_DIALOG6), hwnd,  DownloadProgressProcUART ); //modaless

		//ShowWindow(_hDlgDownloadProgress,SW_SHOW); 
		//isn't needed because the dialog box already has WS_VISIBLE attribute.
	}
	else
	{
		DWORD dwError = GetLastError();
		if(hFile) { CloseHandle(hFile); hFile=NULL; }
		if(SerialTxBuf) { free((void*)SerialTxBuf); SerialTxBuf=NULL; }
		EB_Printf(TEXT("[dnw] can not create TxFile MenuTransmit thread. (Error:%u), threadResult:%ld \n"), dwError, threadResult );
	}
	//The dialog box will be closed at the end of TxFile().
	//free(SerialTxBuf) & CloseHandle(hWrite) will be done in TxFile()
}



void MenuOptions(HWND hwnd)
{
	int result = 0;

	//Create the download progress dialogbox.
	result = DialogBox(_hInstance,MAKEINTRESOURCE(IDD_DIALOG2),_hwnd, OptionsProc); //modal
	if( (1==result) ) // && (1!=isConnected) ) 
	{
		MenuConnect(hwnd); //reconfig the serial port.
		//EB_Printf(TEXT("[dnw] MenuOptions is connected! \r\n") );  // refer -> EndDialog(hDlg,1)
	}
	else if( 0 == result )
	{
		// Cancel or ESC 
		//EB_Printf(TEXT("[dnw] MenuOptions is error or already connected!! (%d, %d) \r\n"), result, GetLastError() );
	}

}


#if USE_HEX2BIN_CONVERT // 2016.03.04
int MenuHex2Bin(HWND hwnd)
{
	int result = 0;


	//Create the download progress dialogbox.
	result = DialogBox(_hInstance,MAKEINTRESOURCE(IDD_DIALOG3),_hwnd, cbHex2BinProc);
	if( 1 == result )
	{
		// OK --
	}
	else if( 0 == result )
	{
		// Cancel or ESC 
		// EB_Printf(TEXT("[dnw] MenuHex2Bin is error or already connected!! (%d, %d) \r\n"), result, GetLastError() );
	}
	return result;


}
#endif



#if USE_TXDATA_TO_SERIAL // 2016.03.27
int MenuTxData2UART(HWND hwnd)
{

	int result = 0;

	//Create the download progress dialogbox.
	result = DialogBox(_hInstance,MAKEINTRESOURCE(IDD_DIALOG4),_hwnd, cbTxData2UART );
	if( 1 == result ) // &&  (1!=isConnected) )
	{
		// OK --
	}
	else if( 0 == result )
	{
		// Cancel or ESC 
		// EB_Printf(TEXT("[dnw] MenuTxData2UART is error or already connected!! (%d, %d) \r\n"), result, GetLastError() );
	}

	return result;

}
#endif



#if USE_FLOAT2HEX_VALUE // 2018.07.18
int MenuFloat2Hexa(HWND hwnd)
{

	int result = 0;

	//Create the download progress dialogbox.
	result = DialogBox(_hInstance,MAKEINTRESOURCE(IDD_DIALOG7),_hwnd, cbFloat2Hexa );
	if( 1 == result ) // &&  (1!=isConnected) )
	{
		// OK --
	}
	else if( 0 == result )
	{
		// Cancel or ESC 
		// EB_Printf(TEXT("[dnw] cbFloat2Hexa is error or already connected!! (%d, %d) \r\n"), result, GetLastError() );
	}

	return result;

}

#endif





#if (RICHED_COLOR) || (TEXT_FIND_MARK) // 2016.09.23
int MenuLogTextColor(HWND hwnd)
{
	int result = 0;

	//Create the download progress dialogbox.
	result = DialogBox(_hInstance,MAKEINTRESOURCE(IDD_DIALOG5),_hwnd, cbLogTxtColor );
	if( 1 == result ) // &&  (1!=isConnected) )
	{
		// OK --
	}
	else if( 0 == result )
	{
		// Cancel or ESC 
	}

	return result;
}
#endif



void SerialTimeOut(void)
{
	COMMTIMEOUTS commTimeOuts;
	COMSTAT comStat;
	DWORD dwErrorFlags;


	memset( &commTimeOuts, 0x00, sizeof(COMMTIMEOUTS) );
	if( FALSE == GetCommTimeouts (idComDev, &commTimeOuts) )
	{
		memset( &comStat, 0x00, sizeof(COMSTAT) );
		ClearCommError(idComDev,&dwErrorFlags,&comStat); // 2016.03.01

		//EB_Printf(TEXT("[dnw] GetCommTimeouts COM%d port. (Err:%u) \r\n"), userComPort, dwError  );
	}

#if COM_PORT_TIMEOUT /// 2014.08.12
	commTimeOuts.ReadIntervalTimeout         = MAXDWORD; /* msec  MAXDWORD */
	commTimeOuts.ReadTotalTimeoutMultiplier  = 0;
	commTimeOuts.ReadTotalTimeoutConstant    = 0; /* msec */
	commTimeOuts.WriteTotalTimeoutMultiplier = 0;
	commTimeOuts.WriteTotalTimeoutConstant   = 0;
	if( !SetCommTimeouts(idComDev,&commTimeOuts) ) /* 지정한 통신 디바이스의 읽기쓰기 타임아웃 설정 */				
	{
		//EB_Printf(TEXT("[dnw] can not set COM%d port in SerialTimeOut. Retry it!! (Error:%u) \r\n"), userComPort, GetLastError() );
	}

#else
	commTimeOuts.ReadIntervalTimeout         = 0;
	commTimeOuts.ReadTotalTimeoutMultiplier  = 0;
	commTimeOuts.ReadTotalTimeoutConstant    = 500; /* msec */
	commTimeOuts.WriteTotalTimeoutMultiplier = 0;
	commTimeOuts.WriteTotalTimeoutConstant   = 500;
	SetCommTimeouts(idComDev,&commTimeOuts); /* 지정한 통신 디바이스의 읽기쓰기 타임아웃 설정 */				
#endif /// 2014.08.12

}



/*****************************
 *                           *
 * serial communication part *
 *                           *
 *****************************/
BOOL OpenComPort(int port, int msg_onoff)
{
const  TCHAR *textCom[]= { 
		TEXT("COM0"),TEXT("COM1"),TEXT("COM2"),TEXT("COM3"),TEXT("COM4"),
		TEXT("COM5"),TEXT("COM6"),TEXT("COM7"),TEXT("COM8"),TEXT("COM9"), 
		TEXT("\\\\.\\COM10"),TEXT("\\\\.\\COM11"),TEXT("\\\\.\\COM12"),TEXT("\\\\.\\COM13"),TEXT("\\\\.\\COM14"),
		TEXT("\\\\.\\COM15"),TEXT("\\\\.\\COM16"),TEXT("\\\\.\\COM17"),TEXT("\\\\.\\COM18"),TEXT("\\\\.\\COM19"),
		TEXT("\\\\.\\COM20"),TEXT("\\\\.\\COM21"),TEXT("\\\\.\\COM22"),TEXT("\\\\.\\COM23"),TEXT("\\\\.\\COM24"),
		TEXT("\\\\.\\COM25"),TEXT("\\\\.\\COM26"),TEXT("\\\\.\\COM27"),TEXT("\\\\.\\COM28"),TEXT("\\\\.\\COM29"),
		TEXT("\\\\.\\COM30"),TEXT("\\\\.\\COM31"),TEXT("\\\\.\\COM32"),TEXT("\\\\.\\COM33"),TEXT("\\\\.\\COM34"),
		TEXT("\\\\.\\COM35"),TEXT("\\\\.\\COM36"),TEXT("\\\\.\\COM37"),TEXT("\\\\.\\COM38"),TEXT("\\\\.\\COM39"),
		TEXT("\\\\.\\COM40"),TEXT("\\\\.\\COM41"),TEXT("\\\\.\\COM42"),TEXT("\\\\.\\COM43"),TEXT("\\\\.\\COM44"),
		TEXT("\\\\.\\COM45"),TEXT("\\\\.\\COM46"),TEXT("\\\\.\\COM47"),TEXT("\\\\.\\COM48"),TEXT("\\\\.\\COM49"),
		TEXT("\\\\.\\COM50"),TEXT("\\\\.\\COM51"),TEXT("\\\\.\\COM52"),TEXT("\\\\.\\COM53"),TEXT("\\\\.\\COM54"),
		TEXT("\\\\.\\COM55"),TEXT("\\\\.\\COM56"),TEXT("\\\\.\\COM57"),TEXT("\\\\.\\COM58"),TEXT("\\\\.\\COM59"),
		TEXT("\\\\.\\COM60"),TEXT("\\\\.\\COM61"),TEXT("\\\\.\\COM62"),TEXT("\\\\.\\COM63"),TEXT("\\\\.\\COM64"),
		TEXT("\\\\.\\COM65"),TEXT("\\\\.\\COM66"),TEXT("\\\\.\\COM67"),TEXT("\\\\.\\COM68"),TEXT("\\\\.\\COM69"),
		TEXT("\\\\.\\COM70"),TEXT("\\\\.\\COM71"),TEXT("\\\\.\\COM72"),TEXT("\\\\.\\COM73"),TEXT("\\\\.\\COM74"),
		TEXT("\\\\.\\COM75"),TEXT("\\\\.\\COM76"),TEXT("\\\\.\\COM77"),TEXT("\\\\.\\COM78"),TEXT("\\\\.\\COM79"),
		TEXT("\\\\.\\COM80"),TEXT("\\\\.\\COM81"),TEXT("\\\\.\\COM82"),TEXT("\\\\.\\COM83"),TEXT("\\\\.\\COM84"),
		TEXT("\\\\.\\COM85"),TEXT("\\\\.\\COM86"),TEXT("\\\\.\\COM87"),TEXT("\\\\.\\COM88"),TEXT("\\\\.\\COM89"),
		TEXT("\\\\.\\COM90"),TEXT("\\\\.\\COM91"),TEXT("\\\\.\\COM92"),TEXT("\\\\.\\COM93"),TEXT("\\\\.\\COM94"),
		TEXT("\\\\.\\COM95"),TEXT("\\\\.\\COM96"),TEXT("\\\\.\\COM97"),TEXT("\\\\.\\COM98"),TEXT("\\\\.\\COM99"),
		TEXT("\\\\.\\COM100"),TEXT("\\\\.\\COM101"),TEXT("\\\\.\\COM102"),TEXT("\\\\.\\COM103"),TEXT("\\\\.\\COM104"),
		TEXT("\\\\.\\COM105"),TEXT("\\\\.\\COM106"),TEXT("\\\\.\\COM107"),TEXT("\\\\.\\COM108"),TEXT("\\\\.\\COM109"),
		TEXT("\\\\.\\COM110"),TEXT("\\\\.\\COM111"),TEXT("\\\\.\\COM112"),TEXT("\\\\.\\COM113"),TEXT("\\\\.\\COM114"),
		TEXT("\\\\.\\COM115"),TEXT("\\\\.\\COM116"),TEXT("\\\\.\\COM117"),TEXT("\\\\.\\COM118"),TEXT("\\\\.\\COM119"),

		TEXT("\\\\.\\COM120"),TEXT("\\\\.\\COM121"),TEXT("\\\\.\\COM122"),TEXT("\\\\.\\COM123"),TEXT("\\\\.\\COM124"), 
		TEXT("\\\\.\\COM125"),TEXT("\\\\.\\COM126"),TEXT("\\\\.\\COM127"),TEXT("\\\\.\\COM128"),TEXT("\\\\.\\COM129"),
		TEXT("\\\\.\\COM130"),TEXT("\\\\.\\COM131"),TEXT("\\\\.\\COM132"),TEXT("\\\\.\\COM133"),TEXT("\\\\.\\COM134"),
		TEXT("\\\\.\\COM135"),TEXT("\\\\.\\COM136"),TEXT("\\\\.\\COM137"),TEXT("\\\\.\\COM138"),TEXT("\\\\.\\COM139"),
		TEXT("\\\\.\\COM140"),TEXT("\\\\.\\COM141"),TEXT("\\\\.\\COM142"),TEXT("\\\\.\\COM143"),TEXT("\\\\.\\COM144"),
		TEXT("\\\\.\\COM145"),TEXT("\\\\.\\COM146"),TEXT("\\\\.\\COM147"),TEXT("\\\\.\\COM148"),TEXT("\\\\.\\COM149"),
		TEXT("\\\\.\\COM150"),TEXT("\\\\.\\COM151"),TEXT("\\\\.\\COM152"),TEXT("\\\\.\\COM153"),TEXT("\\\\.\\COM154"),
		TEXT("\\\\.\\COM155"),TEXT("\\\\.\\COM156"),TEXT("\\\\.\\COM157"),TEXT("\\\\.\\COM158"),TEXT("\\\\.\\COM159"),
		TEXT("\\\\.\\COM160"),TEXT("\\\\.\\COM161"),TEXT("\\\\.\\COM162"),TEXT("\\\\.\\COM163"),TEXT("\\\\.\\COM164"),
		TEXT("\\\\.\\COM165"),TEXT("\\\\.\\COM166"),TEXT("\\\\.\\COM167"),TEXT("\\\\.\\COM168"),TEXT("\\\\.\\COM169"),
		TEXT("\\\\.\\COM170"),TEXT("\\\\.\\COM171"),TEXT("\\\\.\\COM172"),TEXT("\\\\.\\COM173"),TEXT("\\\\.\\COM174"),
		TEXT("\\\\.\\COM175"),TEXT("\\\\.\\COM176"),TEXT("\\\\.\\COM177"),TEXT("\\\\.\\COM178"),TEXT("\\\\.\\COM179"),
		TEXT("\\\\.\\COM180"),TEXT("\\\\.\\COM181"),TEXT("\\\\.\\COM182"),TEXT("\\\\.\\COM183"),TEXT("\\\\.\\COM184"),
		TEXT("\\\\.\\COM185"),TEXT("\\\\.\\COM186"),TEXT("\\\\.\\COM187"),TEXT("\\\\.\\COM188"),TEXT("\\\\.\\COM189"),  
		TEXT("\\\\.\\COM190"),TEXT("\\\\.\\COM191"),TEXT("\\\\.\\COM192"),TEXT("\\\\.\\COM193"),TEXT("\\\\.\\COM194"),
		TEXT("\\\\.\\COM195"),TEXT("\\\\.\\COM196"),TEXT("\\\\.\\COM197"),TEXT("\\\\.\\COM198"),TEXT("\\\\.\\COM199"), // IDC_RADIOCOM197
		TEXT("\\\\.\\COM200"),TEXT("\\\\.\\COM201"),TEXT("\\\\.\\COM202"),TEXT("\\\\.\\COM203"),TEXT("\\\\.\\COM204"),
		TEXT("\\\\.\\COM205"),TEXT("\\\\.\\COM206"),TEXT("\\\\.\\COM207"),TEXT("\\\\.\\COM208"),TEXT("\\\\.\\COM209"),

		TEXT("\\\\.\\COM210"),TEXT("\\\\.\\COM211"),TEXT("\\\\.\\COM212"),TEXT("\\\\.\\COM213"),TEXT("\\\\.\\COM214"),
		TEXT("\\\\.\\COM215"),TEXT("\\\\.\\COM216"),TEXT("\\\\.\\COM217"),TEXT("\\\\.\\COM218"),TEXT("\\\\.\\COM219"),
		TEXT("\\\\.\\COM220"),TEXT("\\\\.\\COM221"),TEXT("\\\\.\\COM222"),TEXT("\\\\.\\COM223"),TEXT("\\\\.\\COM224"),
		TEXT("\\\\.\\COM225"),TEXT("\\\\.\\COM226"),TEXT("\\\\.\\COM227"),TEXT("\\\\.\\COM228"),TEXT("\\\\.\\COM229"),
		TEXT("\\\\.\\COM230"),TEXT("\\\\.\\COM231"),TEXT("\\\\.\\COM232"),TEXT("\\\\.\\COM233"),TEXT("\\\\.\\COM234"),
		TEXT("\\\\.\\COM235"),TEXT("\\\\.\\COM236"),TEXT("\\\\.\\COM237"),TEXT("\\\\.\\COM238"),TEXT("\\\\.\\COM239"),
		TEXT("\\\\.\\COM240"),TEXT("\\\\.\\COM241"),TEXT("\\\\.\\COM242"),TEXT("\\\\.\\COM243"),TEXT("\\\\.\\COM244"),
		TEXT("\\\\.\\COM245"),TEXT("\\\\.\\COM246"),TEXT("\\\\.\\COM247"),TEXT("\\\\.\\COM248"),TEXT("\\\\.\\COM249"),
		TEXT("\\\\.\\COM250"),TEXT("\\\\.\\COM251"),TEXT("\\\\.\\COM252"),TEXT("\\\\.\\COM253"),TEXT("\\\\.\\COM254"),
		TEXT("\\\\.\\COM255"),TEXT("\\\\.\\COM256"),TEXT("\\\\.\\COM257"),TEXT("\\\\.\\COM258"),TEXT("\\\\.\\COM259"),

	};



	/* COMM device를 파일 형식으로 연결한다.
	   nPort Number >=10 and then,   szPort.Format("\\\\.\\COM%d", nPort); */


	DCB dcb;
	COMMTIMEOUTS commTimeOuts;
	uintptr_t tThread;
	COMSTAT comStat;
	DWORD dwErrorFlags;
	int  retry_cnt = 0;
#define MAX_RETRY_CNT 		5

//	SetUnhandledExceptionFilter(UnhandledExceptionHandler);   // 2016.10.15
	


	//====================================================
	isConnected = 2; /// Try to connecting.....
	//====================================================


	if( idComDev ) { CloseHandle(idComDev); idComDev=NULL; }
	if( osRead.hEvent ) { CloseHandle(osRead.hEvent); osRead.hEvent=NULL; }
	if( osWrite.hEvent ) { CloseHandle(osWrite.hEvent); osWrite.hEvent=NULL; }


	// --------------------------------------------
	// Initialized
	// --------------------------------------------
#if 0
	if( NULL == textCom[port] )
	{
		EB_Printf(TEXT("[dnw] COM port array is abnormal.. (%s) \r\n"), port );
	}
#endif


	//====================================================
	//== 1. Open COM port
	//====================================================
	retry_cnt = 0;
	do {
	    idComDev=CreateFile(textCom[port /*-1*/],
							GENERIC_READ|GENERIC_WRITE,
							0, //exclusive access
							NULL, /* no security attrs  */
							OPEN_EXISTING,
		    #if 1 // IS_OVERLAPPED_IO
							FILE_FLAG_OVERLAPPED|FILE_ATTRIBUTE_NORMAL,
		    #else
							/*FILE_ATTRIBUTE_NORMAL,*/ 0,
		    #endif	
							NULL);

		if(retry_cnt ++ > MAX_RETRY_CNT) break;
		if(INVALID_HANDLE_VALUE==idComDev || NULL==idComDev) Sleep(10);
	} while( (INVALID_HANDLE_VALUE==idComDev) || (NULL==idComDev) );


	if( (INVALID_HANDLE_VALUE==idComDev) || (NULL==idComDev) )
	{
		DWORD dwError = GetLastError();

		if( msg_onoff )
		{
			if( ERROR_FILE_NOT_FOUND==dwError ) 
				EB_Printf(TEXT("[dnw] COM%d port can not connect. (%s) \r\n"), port, APP_VER );
			else if( ERROR_ACCESS_DENIED==dwError ) /// 5
				EB_Printf(TEXT("[dnw] COM%d port access is denied! (%s) \r\n"), port, APP_VER );
			else if( ERROR_ALREADY_ASSIGNED==dwError) // 85 (0x55)
				EB_Printf(TEXT("[dnw] COM%d port is already in use. (%s) \r\n"), port, APP_VER );
			else 
				EB_Printf(TEXT("[dnw] COM%d port error is occurred. dwError(%u) (%s) \r\n"), port, dwError, APP_VER );
		}
		memset(&comStat, 0x00, sizeof(COMSTAT) );
		ClearCommError(idComDev,&dwErrorFlags,&comStat); // 2016.03.01

		CloseComPort();
		isConnected = 0; /// 2012.07.04
		if( idComDev ) { CloseHandle(idComDev); idComDev = NULL; }
		return FALSE;
	}


	//====================================================
	//== 2. Overlapped
	//====================================================

	memset( &osRead, 0x00,sizeof(OVERLAPPED) );
	memset( &osWrite, 0x00,sizeof(OVERLAPPED) );

	osRead.Offset      = 0;
	osRead.OffsetHigh  = 0;
	osRead.hEvent      = NULL;

	osWrite.Offset     = 0;
	osWrite.OffsetHigh = 0;
	osWrite.hEvent     = NULL;


	//manual reset event object should be used. 
	//So, system can make the event objecte nonsignalled.
	//osRead.hEvent & osWrite.hEvent may be used to check the completion of 
	// WriteFile() & ReadFile(). But, the DNW doesn't use this feature.

	// --------------------------------------------------------
	// 1. Event : create I/O event used for overlapped read
	// --------------------------------------------------------

	osRead.hEvent = CreateEvent(NULL, // no security
								TRUE/*bManualReset*/, // explicit reset req
								FALSE, // initial event reset
								NULL); // no name

	if( (INVALID_HANDLE_VALUE==osRead.hEvent ) || (NULL==osRead.hEvent) )
	{
		EB_Printf(TEXT("[dnw] can not create osRead Event. (Error:%u) \r\n\r\n"), GetLastError() );

		if( osRead.hEvent ) { CloseHandle(osRead.hEvent); osRead.hEvent=NULL; } 
		if( osWrite.hEvent ) { CloseHandle(osWrite.hEvent); osWrite.hEvent=NULL; }
		if( idComDev ) { CloseHandle(idComDev); idComDev = NULL; }

		CloseComPort();
		isConnected = 0;

		return FALSE;
	}

	osWrite.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	if( (INVALID_HANDLE_VALUE==osWrite.hEvent) || (NULL==osWrite.hEvent) )
	{
		EB_Printf(TEXT("[dnw] can not create osWrite Event. (Error:%u) \r\n\r\n"), GetLastError() );

		CloseComPort();
		isConnected = 0;
		if( osRead.hEvent ) { CloseHandle(osRead.hEvent); osRead.hEvent=NULL; } 
		if( osWrite.hEvent ) { CloseHandle(osWrite.hEvent); osWrite.hEvent=NULL; }
		if( idComDev ) { CloseHandle(idComDev); idComDev = NULL; }
		return FALSE;
	}



	//====================================================
	//== 2. SetupComm - 
	//====================================================

	if( FALSE == SetupComm(idComDev, MAX_IN_BLOCK_SIZE, MAX_OU_BLOCK_SIZE) ) /* input, output buffer size : 4096 */
	{
		EB_Printf(TEXT("[dnw] can not set COM%d port in SetupComm. (Error:%u) \r\n\r\n"), userComPort, GetLastError()  );

		CloseComPort();
		isConnected = 0;

		if( osRead.hEvent ) { CloseHandle(osRead.hEvent); osRead.hEvent=NULL; } 
		if( osWrite.hEvent ) { CloseHandle(osWrite.hEvent); osWrite.hEvent=NULL; }
		if( idComDev ) { CloseHandle(idComDev); idComDev = NULL; }
		return FALSE;
	}

	memset(&comStat, 0x00, sizeof(COMSTAT) );
	ClearCommError(idComDev, &dwErrorFlags, &comStat);


	//====================================================
	//== 3. PurgeComm - 
	//====================================================

	if( FALSE == PurgeComm(idComDev,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR) )
	{
		EB_Printf(TEXT("[dnw] can not set COM%d port in PurgeComm. (Error:%u) \r\n\r\n"), userComPort, GetLastError() );


		CloseComPort();
		isConnected = 0;

		if( osRead.hEvent ) { CloseHandle(osRead.hEvent); osRead.hEvent=NULL; } 
		if( osWrite.hEvent ) { CloseHandle(osWrite.hEvent); osWrite.hEvent=NULL; }
		if( idComDev ) { CloseHandle(idComDev); idComDev = NULL; }
		return FALSE;
	}


	//====================================================
	//== 6. GetCommState - 
	//====================================================
	memset(&dcb,0,sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);
	if( FALSE == GetCommState(idComDev,&dcb) )  /* 지정한 통신 디바이스의 현재 DCB 설정 얻기 */
	{
		EB_Printf(TEXT("[dnw] can not get COM%d port in GetCommState. (Error:%u) \r\n\r\n"), userComPort, GetLastError() );

		CloseComPort();
		isConnected = 0;

		if( osRead.hEvent ) { CloseHandle(osRead.hEvent); osRead.hEvent=NULL; } 
		if( osWrite.hEvent ) { CloseHandle(osWrite.hEvent); osWrite.hEvent=NULL; }
		if( idComDev ) { CloseHandle(idComDev); idComDev = NULL; }
		return FALSE;
	}


	dcb.DCBlength		  = sizeof(DCB);
	

	/* ---------------------------
			0 : NOPARITY
			1 : ODDPARITY
			2 : EVENPARITY
			3 : MARKPARITY
			4 : SPACEPARITY
	------------------------------ */ 
	dcb.BaudRate		  = userBaudRate;		 /* 115200; 통신 속도 */
	dcb.ByteSize		  = 8;					 /* 8: 8Bit, 7: 7Bit, 6:6Bit, 5:5Bit : 1바이트의 비트수 */


	dcb.Parity			  = 0;                   /* Parity Bit: 0~4 (NOPARITY, ODDPARITY, EVENPARITY, MARK, SPACE) */
	dcb.StopBits		  = 0;					 /* Stop Bit  : 0,1,2가 각각1,1.5,2를 의미 */
	dcb.fBinary 		  = TRUE;				 /* Binary mode,  EOF 체크 안함 */

	dcb.fParity 		  = NOPARITY;	         /* Parity bit 사용 여부 */

#if COM_SETTINGS /// 2012.07.11

	if( userCOMparity > LEN_COMPARITY-1 ) userCOMparity = 0;
	if( userCOMdataIdx > LEN_COMDATABIT-1 ) userCOMdataIdx=1;
	if( userCOMstop > LEN_COMSTOP-1 ) userCOMstop = 0;

	dcb.fParity 		  = userCOMparity;

	dcb.ByteSize		  = (BYTE)COMportDatabit[userCOMdataIdx];
	dcb.StopBits		  = (BYTE)userCOMstop;   /* Stop Bit  : 0,1,2가 각각1,1.5,2를 의미 */

	dcb.Parity			  = userCOMparity;		 /* Parity Bit: 0~4 (NOPARITY, ODDPARITY, EVENPARITY, MARK, SPACE) */
	dcb.fParity 		  = userCOMparity ? TRUE : FALSE; // NOPARITY; /* Parity bit 사용 여부 */
#endif


	 /* ---------------------------
			0 : ONESTOPBIT (1 stop bit)
			1 : ONE5STOPBITS (1.5 stop bits.)
			2 : TWOSTOPBITS (2 stop bit)
	------------------------------ */ 

	// 일단 기본 flow control 옵션을 설정한다.
	// 뒤에서 flow control 옵션에 따라 필요한 항목을 Enable 한다.
	dcb.fOutxCtsFlow	  = FALSE;			  /* CTS 출력 흐름제어 사용 여부. fOutxCtsFlow  송신 흐름 제어를 위하여 CTS신호를 주시할 것을 지정한다 */
											  /* 이 항목이 TRUE이고 CTS가 저위이면 고위가 될 때까지 송신은 억제된다. */
	dcb.fOutxDsrFlow      = FALSE; 			  /* DSR 출력 흐름제어 사용 여부. fOutxDsrFlow	송신 흐름 제어를 위하여 DSR (data-set-ready) 신호를 주시할 것을 지정한다.  */
											  /* 이 항목이 TRUE이고 DSR이 저위일 경 우 고위가 될 때까지 송신은 억제된다 */

	dcb.fDtrControl 	  = DTR_CONTROL_DISABLE; /* DTR 흐름제어 형태 , RTS_CONTROL_ENABLE */
	dcb.fRtsControl 	  = RTS_CONTROL_DISABLE; /* RTS 흐름제어 , DTR_CONTROL_ENABLE */

	dcb.fOutX			  = FALSE;			  /* XON/XOFF 출력 흐름제어 사용 여부 */
	dcb.fInX			  = FALSE;			  /* XON/XOFF 입력 흐름제어 사용 여부 */


#ifdef __TEST__
	switch (_flowControl) {
		case FC_XONXOFF:	// Software Handshaking 
			dcb.fOutX = true;
			dcb.fInX = true;
			dcb.XonChar = 0x11; 		// Tx and Rx X-ON character 	 
			dcb.XoffChar = 0x13;		// Tx and Rx X-OFF character   
			dcb.XoffLim = 100;			// Transmit X-ON threshold 
			dcb.XonLim = 100;			// Transmit X-OFF threshold    
			break;
		case FC_RTSCTS:
			dcb.fOutxCtsFlow = true;
			dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
			break;
		case FC_DTRDSR:
			dcb.fOutxDsrFlow = true;
			dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
			break;
		case FC_FULLHARDWARE: // Hardware Handshaking
			dcb.fOutxCtsFlow = true; 
			dcb.fOutxDsrFlow = true; 
			dcb.fDtrControl = DTR_CONTROL_HANDSHAKE; 
			dcb.fRtsControl = RTS_CONTROL_HANDSHAKE; 
			break;
	}
#endif


	dcb.fDsrSensitivity   = FALSE;	/* DSR 감도, DSR Sensitivity */
	dcb.fTXContinueOnXoff = FALSE;	/* XOFF continues Tx, Continue TX when Xoff sent */
	dcb.fErrorChar		  = FALSE;	/* Disable error replacement  */
	dcb.ErrorChar         = 0;      /* Error replacement char */
	dcb.fNull			  = FALSE;	/* Disable null stripping. 0번 문자 없애기 */
	dcb.fAbortOnError	  = FALSE;	/* 에러 무시. 오류가 발생하면 읽기와 쓰기 작업이 종료될 것인가를 정한다.  */
									/* 이 항목이 TRUE이면 오류(ERROR_IO_ABORTED)가 발생하 였을 때 제어기는 읽기와 쓰기 작업을 중지하고 오류 상태와 함께 종료된다.	*/
									/* 응용프로그램에서 ClearCommError() 로 오류가 발생한 상황을 알아챌 때 까지 모든 통신 작업을 중지한다	*/

	dcb.EofChar           = 0; // End of Input character
	dcb.EvtChar           = 0; // Received Event character


	// --------------------------------------------------------------
	// added 2017.08.04 None FlowControl
	dcb.XoffChar          = 0;
	dcb.XoffLim           = 0; // Transmit X-OFF threshold 
	dcb.XonChar           = 0;
	dcb.XonLim            = 0; // Transmit X-ON threshold 


	// ----------------------------------
	// --- Flow Control 2017.08.03
	// ----------------------------------
#define ASCII_XON       0x11 /* 수싞버퍼의 여유가 있음. */
#define ASCII_XOFF      0x13 /* 수싞버퍼의 여유가 없으니 데이터 송싞 금지 */

	if( userFlowControl )
	{
		if( FLOWC_DTR_DSR == (userFlowCtrlVal & FLOWC_DTR_DSR) )
		{
			//EB_Printf(TEXT("[dnw] FlowControl::DTR/DSR (%u) \n"), userFlowControl );

			// set XON/XOFF
			//dcb.fOutX        = FALSE;
			//dcb.fInX         = FALSE;

			// set RTSCTS
			//dcb.fOutxCtsFlow = FALSE;
			//dcb.fRtsControl  = RTS_CONTROL_DISABLE; 

			// set DSRDTR
			dcb.fOutxDsrFlow = TRUE;
			dcb.fDtrControl  = DTR_CONTROL_HANDSHAKE; 
		}

		if( FLOWC_RTS_CTS == (userFlowCtrlVal & FLOWC_RTS_CTS) ) 
		{
			//EB_Printf(TEXT("[dnw] FlowControl::RTS/CTS (%u) \n"), userFlowControl );

			// set XON/XOFF
			//dcb.fOutX        = FALSE;
			//dcb.fInX         = FALSE;

			// set RTSCTS
			dcb.fOutxCtsFlow = TRUE;
			dcb.fRtsControl  = RTS_CONTROL_HANDSHAKE; 

			// set DSRDTR
			//dcb.fOutxDsrFlow = FALSE;
			//dcb.fDtrControl  = DTR_CONTROL_DISABLE; 
		}

		if( FLOWC_XON_XOFF == (userFlowCtrlVal & FLOWC_XON_XOFF) ) 
		{
			//EB_Printf(TEXT("[dnw] FlowControl::XON/XOFF (%u) \n"), userFlowControl );

			// set XON/XOFF
			dcb.fOutX        = TRUE;
			dcb.fInX         = TRUE;
			dcb.fTXContinueOnXoff = TRUE;
			dcb.XoffChar     = ASCII_XOFF;
			dcb.XoffLim      = MAX_IN_BLOCK_SIZE - (MAX_IN_BLOCK_SIZE / 4); // Transmit X-OFF threshold 
			dcb.XonChar      = ASCII_XON;
			dcb.XonLim       = MAX_IN_BLOCK_SIZE - (MAX_IN_BLOCK_SIZE / 2); // Transmit X-ON threshold 

			// set RTSCTS
			//dcb.fOutxCtsFlow = FALSE;
			//dcb.fRtsControl  = RTS_CONTROL_DISABLE; 

			// set DSRDTR
			//dcb.fOutxDsrFlow = FALSE;
			//dcb.fDtrControl  = DTR_CONTROL_DISABLE;
		}

	}
	else
	{
		// None FlowControl, NOPROTOCOL
		//EB_Printf(TEXT("[dnw] FlowControl::NONE (%u) \n"), userFlowControl );

		// set XON/XOFF
		dcb.fTXContinueOnXoff = FALSE;	/* XOFF continues Tx, Continue TX when Xoff sent */

		dcb.fOutX        = FALSE;
		dcb.fInX         = FALSE;
		// set RTSCTS
		dcb.fOutxCtsFlow = FALSE;
		dcb.fRtsControl  = RTS_CONTROL_DISABLE; 
		// set DSRDTR
		dcb.fOutxDsrFlow = FALSE;
		dcb.fDtrControl  = DTR_CONTROL_DISABLE; 
	}

	// ----------------------------------
	// --- Flow Control 2017.08.03
	// ----------------------------------





	//====================================================
	//== 5. SetCommState - 
	//====================================================

	memset(&comStat, 0x00, sizeof(COMSTAT) );
	ClearCommError(idComDev, &dwErrorFlags, &comStat);

	/* --- 하드웨어와 제어 설정 초기화 --- */
	if( FALSE == SetCommState(idComDev,&dcb) )
	{
		DWORD dwError = GetLastError();
		EB_Printf(TEXT("[dnw] can not set COM%d port in SetCommState. (Error:%u) \n"), userComPort, dwError );

		memset(&comStat, 0x00, sizeof(COMSTAT) );
		ClearCommError(idComDev,&dwErrorFlags,&comStat);
		CloseComPort();
		isConnected = 0;

		if( osRead.hEvent ) { CloseHandle(osRead.hEvent); osRead.hEvent=NULL; } 
		if( osWrite.hEvent ) { CloseHandle(osWrite.hEvent); osWrite.hEvent=NULL; }
		if( idComDev ) { CloseHandle(idComDev); idComDev = NULL; }
		return FALSE;
	}


	//====================================================
	//== 6. PurgeComm - 
	//====================================================
	if( FALSE == PurgeComm(idComDev, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR) ) // overlapped
	{
		DWORD dwError = GetLastError();
		EB_Printf(TEXT("[dnw] can not set COM%d port in PurgeComm... (Error:%u) \r\n"), userComPort, dwError );
		CloseComPort();
		isConnected = 0;

		if( osRead.hEvent ) { CloseHandle(osRead.hEvent); osRead.hEvent=NULL; } 
		if( osWrite.hEvent ) { CloseHandle(osWrite.hEvent); osWrite.hEvent=NULL; }
		if( idComDev ) { CloseHandle(idComDev); idComDev = NULL; }
		return FALSE;
	}

	//====================================================
	//== 7. GetCommTimeouts - 
	//====================================================
	memset( &commTimeOuts, 0x00, sizeof(COMMTIMEOUTS) ); /// OK -> initial
	if( FALSE == GetCommTimeouts (idComDev, &commTimeOuts) )
	{
		DWORD dwError = GetLastError();
	    EB_Printf(TEXT("[dnw] can not get COM%d port in GetCommTimeouts... (Error:%u) \r\n"), userComPort, dwError  );
	}
	else
	{
#if 0
	    EB_Printf(TEXT("[dnw] ReadIntervalTimeout         : 0x%X\n" 
			           "      ReadTotalTimeoutMultiplier  : %u\n"
			           "      ReadTotalTimeoutConstant    : %u\n" 
			           "      WriteTotalTimeoutMultiplier : %u\n" 
			           "      WriteTotalTimeoutConstant   : %u\n"),
								commTimeOuts.ReadIntervalTimeout,
								commTimeOuts.ReadTotalTimeoutMultiplier,
								commTimeOuts.ReadTotalTimeoutConstant,
								commTimeOuts.WriteTotalTimeoutMultiplier,
								commTimeOuts.WriteTotalTimeoutConstant );
#endif
	}


	//====================================================
	//== 8. SetCommTimeouts - 
	//====================================================

	// set up for overlapped I/O
	// CBR_9600 is approximately 1byte/ms. 
	// For our purposes, allow double the expected time per character for a fudge factor.
	commTimeOuts.ReadIntervalTimeout         = MAXDWORD; /* msec  MAXDWORD */
	commTimeOuts.ReadTotalTimeoutMultiplier  = 0;
	commTimeOuts.ReadTotalTimeoutConstant    = 0; /* msec */
	commTimeOuts.WriteTotalTimeoutMultiplier = 0; // 2*CBR_9600/dwBaudRate; /* CBR_9600 기준 ms당 바이트를 두배까지 설정 */
	commTimeOuts.WriteTotalTimeoutConstant   = 0; /* msec */

	if( !SetCommTimeouts(idComDev,&commTimeOuts) ) /* 지정한 통신 디바이스의 읽기쓰기 타임아웃 설정 */
	{
		DWORD dwError = GetLastError();
		EB_Printf(TEXT("[dnw] can not set COM%d port in SetCommTimeouts. Retry it!! (Error:%u) \r\n"), userComPort, dwError );

		memset(&comStat, 0x00, sizeof(COMSTAT) );
		ClearCommError(idComDev,&dwErrorFlags,&comStat); /// added 2011.1.29
		PurgeComm(idComDev,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR); /// added. 2012.02.17

		CloseComPort();
		isConnected = 0;

		if( osRead.hEvent ) { CloseHandle(osRead.hEvent); osRead.hEvent=NULL; } 
		if( osWrite.hEvent ) { CloseHandle(osWrite.hEvent); osWrite.hEvent=NULL; }
		if( idComDev ) { CloseHandle(idComDev); idComDev = NULL; }
		return FALSE;
	}


	//====================================================
	//== 9. SetCommMask - 
	//====================================================
	memset(&comStat, 0x00, sizeof(COMSTAT) );
	ClearCommError(idComDev, &dwErrorFlags, &comStat);


	SetCommMask(idComDev, EV_RXCHAR ); /// 2012.07.04


	tThread = _beginthread( 
					(void (*)(void *))DoRxTx,  	// void( __cdecl *start_address )( void * )
					THREAD_STACK_SIZE, 		// unsigned stack_size
					(void *)NULL ); 			// void *arglist 

	if( -1 == tThread )
	{
		/// error
		DWORD dwError = GetLastError();
		EB_Printf(TEXT("[dnw] can not create DoRxTx beginthread. (tThread=%d, Error:%u)\n"), tThread, dwError );

		memset(&comStat, 0x00, sizeof(COMSTAT) );
		ClearCommError(idComDev,&dwErrorFlags,&comStat);
		PurgeComm(idComDev,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
		CloseComPort();
		isConnected=0;

		if( osRead.hEvent ) { CloseHandle(osRead.hEvent); osRead.hEvent=NULL; } 
		if( osWrite.hEvent ) { CloseHandle(osWrite.hEvent); osWrite.hEvent=NULL; }
		if( idComDev ) { CloseHandle(idComDev); idComDev = NULL; }
		return FALSE; /// 2012.02.17
	}
	isConnected=1; /// 1 : connected!!!!! TRUE;


#if DNW_2ND_WINDOWS_ENABLE // 2016.10.24
	if( OKstatusBarInitial )
	{
		if( BAR_STATUS==isStatusBarEnable ) 
		{
			StatusBarText();
		}
	}
#endif


	BeepSound(3); // 2016.03.26 connection OK
	return TRUE;


}



void CloseComPort(void)
{
	DWORD dwErrorFlags;
	COMSTAT comStat;

	SerialTimeOut();
	
	if( 1==isConnected ) /// 2013.04.12 deleted,.... bug fix || (2==isConnected) )
	{
		isConnected = 0;

		if( osRead.hEvent ) { CloseHandle(osRead.hEvent); osRead.hEvent=NULL; } 
		if( osWrite.hEvent ) { CloseHandle(osWrite.hEvent); osWrite.hEvent=NULL; }
		if( idComDev ) { CloseHandle(idComDev); idComDev = NULL; }

		memset(&comStat, 0x00, sizeof(COMSTAT) );
		ClearCommError(idComDev, &dwErrorFlags, &comStat);

		SetCommMask(idComDev, 0);
		//disable event notification and wait for thread to halt
		EscapeCommFunction(idComDev,CLRDTR);
		PurgeComm(idComDev,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
		Sleep(50);
	}
	isConnected = 0;


	if( osRead.hEvent ) { CloseHandle(osRead.hEvent); osRead.hEvent=NULL; } 
	if( osWrite.hEvent ) { CloseHandle(osWrite.hEvent); osWrite.hEvent=NULL; }
	if( idComDev ) { CloseHandle(idComDev); idComDev=NULL; }

 
#if DNW_2ND_WINDOWS_ENABLE // 2016.10.24
	if( OKstatusBarInitial )
	{
		if( BAR_STATUS==isStatusBarEnable ) 
		{
			StatusBarText();
		}
	}
#endif

    //wait until CloseComPort() effective.
    //If not, OpenComPort()::CreateFile(...) will fail.

}


int SerialGetModemStatus(DWORD Mask)
{
// The value for Mask must be one of the following definitions:
// MS_CTS_ON
// MS_DSR_ON
// MS_RING_ON
	DWORD ModemStat;

	GetCommModemStatus(idComDev, &ModemStat);

	switch( Mask ) {
	case MS_CTS_ON: // The CTS (clear-to-send) signal is on.
	case MS_DSR_ON: // The DSR (data-set-ready) signal is on.
	case MS_RING_ON: // The ring indicator signal is on.
	case MS_RLSD_ON: // The RLSD (receive-line-signal-detect) signal is on.
		if((ModemStat & Mask) != 0) {
			return 1;
		} else {
			return 0;
		}
	default:
		return -1;
	}
}


WORD 	CheckMsg(int ReVLength)
{

	if( !autoSendKey ) return 0;

	if( ReVLength < MSG_DNW_BEGIN_LEN ) return 0;

	if( NULL != strstr( rxBuf, MSG_DNW_BEGIN_TXT ) )
	{
		return DNW_BGN;
	}
	else if( NULL != strstr( rxBuf, MSG_DNW_END_TXT ) )
	{
		return DNW_END;
	}
	else if( NULL != strstr( rxBuf, MSG_DNW_SCR_CLEAR_TXT ) )
	{
		return DNW_CLR;
	}
/*
	else if( NULL != strstr( rxBuf, TEXT("Waiting....")) )
	{
		return DNW_ING;
	}
	else if( NULL != strnstr( rxBuf, MSG_DNW_NK_FIN_TXT, MSG_DNW_NK_FIN_LEN) )
	{
		return DNW_NK;
	}
	else if( NULL != strnstr( rxBuf, MSG_DNW_EBOOT_FIN_TXT, MSG_DNW_EBOOT_FIN_LEN) )
	{
		return DNW_EBOOT;
	}
	else if( NULL != strnstr( rxBuf, MSG_DNW_STEPLDR_FIN_TXT, MSG_DNW_STEPLDR_FIN_LEN) )
	{
		return DNW_STEP;
	}
*/
	else
	{
		return 0; /* NOT found !! */
	}
	
}

//////////////////////////////////////

void ResetCmdToDNW(void)
{
	memset(rxBuf, 0x00, sizeof(rxBuf));
}


void uSecDelay(int nTime)
{
	LARGE_INTEGER litmp; 
	LONGLONG QPartBegin, QPartEnd;
	double dfMinus, dfFreq, dfTim;

	QueryPerformanceFrequency(&litmp);
	dfFreq = (double)litmp.QuadPart;
	QueryPerformanceCounter(&litmp);
	QPartBegin = litmp.QuadPart;
	///EB_Printf(TEXT("[%u]\n"), QPartBegin);

	do
	{
		QueryPerformanceCounter(&litmp);
		QPartEnd = litmp.QuadPart;
		dfMinus = (double)(QPartEnd-QPartBegin);
		dfTim = dfMinus / dfFreq;
	} while(dfTim<0.001*nTime);
	
}


void DoRxTx(void *args)
{
	OVERLAPPED os;
	DWORD dwEvtMask;
	int nLength;
	BOOL fStat;
	DWORD dwWritten;
	WORD  MsgType=0;
	COMSTAT comStat;
	DWORD dwErrorFlags = 0;
	extern BOOL bUSBTxFile;
	DWORD dwError;
#if USE_RX_TX_QUEUE
	TCHAR *DeQBuf = NULL;
#endif


	memset(&os, 0x00, sizeof(OVERLAPPED));

	os.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	if( (INVALID_HANDLE_VALUE == os.hEvent) || (NULL == os.hEvent) )
	{
		DWORD dwError = GetLastError();
		EB_Printf(TEXT("[dnw] can not create the os.hEvent event. (Error:%u) \r\n\r\n"), dwError );

		memset(&comStat, 0x00, sizeof(COMSTAT) );
		ClearCommError(idComDev,&dwErrorFlags,&comStat);

		CloseComPort();
		isConnected = 0;

		/* _endthread given to terminate */
		_endthread();
		return;
	}


/* ----------------------------------------------------------------------------------
   EV_RXCHAR (0x01)  : 문자가 수신되었음, 입력 버퍼에 하나의 문자가 수신된 경우 
   EV_RXFLAG  (0x02) : DCB구조체의 EvtChar 의 문자가 수신되어 입력버퍼에 저장되었음
   EV_TXEMPTY (0x04) : 출력 버퍼로 마지막 문자가 전송되었음, 출력 버퍼에서 마지막 문자가 보내지면 
   EV_CTS (0x08)     : CTS 신호가 변경되었음, CTS(clear to send) 신호가 오는 경우
   EV_DSR (0x10)     : DSR 신호가 변경되었음, DSR(data set ready)신호가 오는 경우
   EV_BREAK (0x40)     : 입력시에 갑자기 중단이 되었음, 입력에서 BREAK신호가 오는 경우
   EV_ERR       (0x80) : 에러가 감지 되었음, 라인이 에러가 발생한 경우
   EV_RING     (0x100) : 전화벨이 울렸음, 전화가 걸려온 경우
   EV_RLSD             : RLSD(receive line signal detect) 신호가 오는 경우 
----------------------------------------------------------------------------------- */


	PurgeComm(idComDev, PURGE_TXCLEAR|PURGE_RXCLEAR|PURGE_TXABORT|PURGE_RXABORT);
	memset(&comStat, 0x00, sizeof(COMSTAT) );
	ClearCommError(idComDev,&dwErrorFlags,&comStat); // 2016.03.26


	if( !SetCommMask(idComDev, EV_RXCHAR|EV_TXEMPTY) ) /// ~~~~ |EV_ERR|EV_RXFLAG|EV_BREAK))
	{
		SetCommMask(idComDev, 0);

		memset(&comStat, 0x00, sizeof(COMSTAT) );
		ClearCommError(idComDev,&dwErrorFlags,&comStat);
		//disable event notification and wait for thread to halt
		EscapeCommFunction(idComDev,CLRDTR);
		PurgeComm(idComDev,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

		EB_Printf(TEXT("[dnw] can not set COM%d port in SetCommMask! (Error:%u) \r\n\r\n"), userComPort, GetLastError() );
		if( os.hEvent ) { CloseHandle(os.hEvent); os.hEvent=NULL; }
		if( osRead.hEvent ) { CloseHandle(osRead.hEvent); osRead.hEvent=NULL; } 
		if( osWrite.hEvent ) { CloseHandle(osWrite.hEvent); osWrite.hEvent=NULL; }
		if( idComDev ) { CloseHandle(idComDev); idComDev=NULL; }

		CloseComPort();
		isConnected = 0;

		/* _endthread given to terminate */
		_endthread();
		return; /// added.
	}



	while( (1==isConnected) ) // ~~~ && idComDev )
	{

		dwEvtMask=0;

    #if IS_OVERLAPPED_IO
		fStat = WaitCommEvent(idComDev,&dwEvtMask,&os);   

	#ifdef IS_OVERLAPPED_CHECK /// 2014.03.04 ----
		if( HasOverlappedIoCompleted(&os) )
		{
			// Returns TRUE if the I/O operation has completed, and FALSE otherwise.
			// EB_Printf(TEXT("Completed") );
		}
		else
	#else /// 2014.07.23
		//Apr.28.2003: fStat should be checked for the cpu time saving. 
		if(!fStat)   //Apr.28.2003:GetOverlappedResult() is needed.  
	#endif // -------------------------
		{
			// -----------------------------------------------------------------------------------------------------------------------
			// To cancel all pending asynchronous I/O operations, use the CancelIo function. 
			// The CancelIo function only cancels operations issued by the calling thread for the specified file handle. 
			// I/O operations that are canceled complete with the error ERROR_OPERATION_ABORTED.
			// To get more details about a completed I/O operation, call the GetOverlappedResult or GetQueuedCompletionStatus function.
			// -----------------------------------------------------------------------------------------------------------------------

		    //By experiment. Only when there was no signalled event, the following was executed.
		    //EB_Printf(TEXT("[dnw] [WaitCommEvent=false]\n") ); 
		    ///--> 아직 전송할 문자가 남았을 경우..
			// -----------------------------------------------------------------------------------------------------------------------
		    if( ERROR_IO_PENDING == GetLastError() )
		    {

		#ifdef IS_OVERLAPPED_CHECK /// 2014.03.04 ----
				// I/O가 완료될 때 가지 기다린다.
				WaitForSingleObject(os.hEvent, IS_OVERLAPPED_WAIT);
		#endif /// -----------------------------
	
		    	/* 비동기 입출력 상태를 조사하는 방법 */
				while( ! GetOverlappedResult(idComDev, &os, &dwWritten, TRUE) ) /// 2014.01.04 /// if() -> while()
				{
					dwError = GetLastError();
					if(ERROR_IO_INCOMPLETE == dwError ) 
					{
						// ----------------------------------------
						// normal result if not finished
						// 정상 ----
						// ----------------------------------------
						continue;
					}
					else if(ERROR_HANDLE_EOF == dwError ) // 2014.07.23
					{
						/// End of file reached.
						memset(&comStat, 0x00, sizeof(COMSTAT) );
						ClearCommError(idComDev,&dwErrorFlags,&comStat); // 2016.03.01
						EB_Printf(TEXT("\r\n[dnw] COM%d port End of file reached! (%s) \r\n\r\n"), userComPort, APP_VER );
						break;
					}
					else
					{
						/// 2013.12.24 --- dwWritten = 0;
						memset(&comStat, 0x00, sizeof(COMSTAT) );
						ClearCommError(idComDev, &dwErrorFlags, &comStat);
						// ReadNum = comStat.cbInQue;
						break;
					}
				}

			}
			else
			{
				dwError = GetLastError();

				///state := WaitForSingleObject(ovlap.hEvent,10);
				dwWritten = 0; /// 2013.09.30, added.

				memset(&comStat, 0x00, sizeof(COMSTAT) );
				ClearCommError(idComDev, &dwErrorFlags, &comStat); // 2016.02.26

			#if 0 // 2016.02.03
				if( CE_BREAK == (dwErrorFlags & CE_BREAK) ) /// 0x0010 The hardware detected a break condition.
				{
					EB_Printf(TEXT("\r\n[dnw] Line error detected a break condition. COM%d: dwError(%u) \r\n"), userComPort, dwError );
				}
			
				if( CE_FRAME == (dwErrorFlags & CE_FRAME) ) /// 0x0008 The hardware detected a framing error.
				{
					EB_Printf(TEXT("\r\n[dnw] Line error detected a framing error. COM%d: dwError(%u) \r\n"), userComPort, dwError  );
				}
			
				if( CE_RXPARITY == (dwErrorFlags & CE_RXPARITY) ) /// 0x0004 The hardware detected a parity error.
				{
					EB_Printf(TEXT("\r\n[dnw] Line error detected a parity error. COM%d: dwError(%u) \r\n"), userComPort, dwError  );
				}

				if( CE_OVERRUN == (dwErrorFlags & CE_OVERRUN) ) /// 0x0002 A character-buffer overrun has occurred. The next character is lost.
				{
					EB_Printf(TEXT("\r\n[dnw] The overrun has occurred. The next character is lost. [inQ(%d), outQ(%d)] \r\n"), comStat.cbInQue, comStat.cbOutQue  );
				}
			
				if( CE_RXOVER == (dwErrorFlags & CE_RXOVER) ) /// 0x0001 An input buffer overflow has occurred. There is either no room in the input buffer, or a character was received after the end-of-file (EOF) character.
				{
					EB_Printf(TEXT("\r\n[dnw] An input buffer overflow has occurred. [inQ(%d), outQ(%d)] \r\n"), comStat.cbInQue, comStat.cbOutQue	);
				}
			#endif



			#if USE_TXDATA_TO_SERIAL // 2016.10.07
				if( NULL != _hDlgTxData2UART )
					SendMessage( _hDlgTxData2UART, WM_COMMAND, IDCANCEL, 0 ); // Delete the popup when disconnected!!
			#endif	



			#if 1 // 2016.10.12
				if( NULL != _hDlgDownloadProgressUART )
					SendMessage( _hDlgDownloadProgressUART, WM_COMMAND, IDCANCEL, 0 ); // Delete the popup when disconnected!!
			#endif


			#if USE_FLOAT2HEX_VALUE // 2018.07.18
				if( NULL != _hDlgFlt2Hexa )
					SendMessage( _hDlgFlt2Hexa, WM_COMMAND, IDCANCEL, 0 ); // Delete the popup when disconnected!!
			#endif


				if( ERROR_ACCESS_DENIED == dwError )
				{
					EB_Printf(TEXT("\r\n\r\n[dnw] COM%d port is disconnected. Access is denied! (%s) \r\n"), 
						userComPort, APP_VER);
				}
				else
				{
					EB_Printf(TEXT("\r\n\r\n[dnw] COM%d port is disconnected! (Err:%u) (flag:0x%X) [%d,%d] (%s) \r\n"), 
						userComPort, dwError, dwErrorFlags, comStat.cbInQue, comStat.cbOutQue, APP_VER );
				}



				// -----------------------------------------
				// COM port is closed..
				// -----------------------------------------

				// ~~ if( ERROR_ACCESS_DENIED==dwError || (0==comStat.cbInQue && 0==comStat.cbOutQue) )
				{
					CloseComPort();
					isConnected = 0;

			#ifdef IS_OVERLAPPED_CHECK /// 2014.03.05 ----
					// I/O가 완료될 때 가지 기다린다. -> HandleClose
					if(osRead.hEvent) { CloseHandle(osRead.hEvent); osRead.hEvent = NULL; }
					if(osWrite.hEvent) { CloseHandle(osWrite.hEvent); osWrite.hEvent = NULL; }
					if(os.hEvent) { CloseHandle(os.hEvent); os.hEvent = NULL; }
			#endif /// -----------------------------


					if( idComDev ) { CloseHandle(idComDev); idComDev=NULL; }

					memset(rxBuf, 0x00, sizeof(rxBuf));

					break;
				}
		
			}

		}
    #else
		WaitCommEvent(idComDev,&dwEvtMask,NULL);  //wait until any event is occurred.
    #endif


		if( EV_TXEMPTY == (dwEvtMask & EV_TXEMPTY) )
		{
			/// inform about the transmission of the last character from the back-end buffer.

			txEmpty = TRUE;
		}


		if( EV_RXCHAR == (dwEvtMask & EV_RXCHAR) ) /// A character was received and placed in the input buffer.
		{
			/// inform about the character reception and place the character into the front-end buffer.
			/// 
			do   //Apr.28.2003:The caveat on MSDN,"Serial Communications in Win32" recommends while();
			{


				//ReadNum = comStat.cbInQue; //++11++deleted
				// if( nLength = ReadCommBlock( rxBuf, MAX_IN_BLOCK_SIZE, ReadNum) )  

				nLength = ReadCommBlock( rxBuf, MAX_IN_BLOCK_SIZE );
				if( nLength ) 
				{
					if(nLength >= MAX_IN_BLOCK_SIZE /*for debug*/)
					{
						memset(rxBuf, 0x00, sizeof(rxBuf) ); /* 2015.11.21 initial added. */
						//EB_Printf(TEXT("\n\n[dnw] find error overrun (length=%d) \n"), nLength );

				#if 1 // 2015.11.21
						/// some other error occurred
						memset(&comStat, 0x00, sizeof(COMSTAT) );
						ClearCommError(idComDev, &dwErrorFlags, &comStat);
						PurgeComm(idComDev,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

						EB_Printf(TEXT("\n\n[dnw] find error overrun-length=%d, Ver[%s] ClearComm(InQ:%d, OutQ:%d) \r\n"), nLength, APP_VER, comStat.cbInQue, comStat.cbOutQue );
				#endif

					}
					else
					{

						rxBuf[nLength]='\0';


				
						if( 0==TextorHexaMode )  // Text Mode
						{
							//OutputDebugString(rxBuf);
							EB_Printf(rxBuf);
							// EB_PrintfText(nLength, rxBuf);
						}
						else // if( 1==TextorHexaMode || 2==TextorHexaMode ) // Hexa mode
						{
							EB_PrintfHexa(nLength, rxBuf); // Hexa display
						}



						// -----------------------------------------------------
						// ------ [F4] key -- automatic added ----------------------
						// -----------------------------------------------------
				#if 1
						if( autoSendKey ) // condition for [F4] Key
						{
							///WriteCommBlock('/');  /* DNW to 6410 or C100 CPU */

							MsgType = CheckMsg( nLength );

							if( (DNW_END == MsgType) )
							{
								ResetCmdToDNW();
								nLength = 0;
								////WriteCommBlock(',');  /* ENDED....	*/

								BinFileIndex = 0;
								DownloadedBin = 0x00;
							}
							else if( DNW_BGN == MsgType )
							{
								ResetCmdToDNW();
								nLength = 0;

								if( TOTAL_BIN_OK != DownloadedBin ) /* 이미 3개 파일 모두 처리했다. */
								{
									Sleep(1500); /// (1000); /* For CloseDownloadProgress()	*/
								 	while( TRUE == bUSBTxFile ) 
									{ 
										Sleep(1); 
									} /// 2010.05.26 added.

									SendMessage(_MainHwnd, WM_COMMAND, CM_ROM_ALL, 0 );
								}

								if( TOTAL_BIN_OK == DownloadedBin )
								{
									BinFileIndex = 0;
									DownloadedBin = 0x00;
								}

								///////////CloseDownloadProgress(); /// added.
								///////////Sleep(1);
								/////////// break;
							}
							else if( (DNW_CLR == MsgType) )
							{
								ResetCmdToDNW();
								nLength = 0;
								
								////WriteCommBlock(',');  /* ENDED....	*/
						#if 0
								Sleep(100); /* For CloseDownloadProgress()	*/
 								SendMessage(_MainHwnd, WM_COMMAND, CM_CLEAR_BUF, 0 );

								BinFileIndex = 0;
								DownloadedBin = 0x00;
						#endif

							}

						#if 0
							else if( (DNW_ING == MsgType) )
							{
								ResetCmdToDNW();
							}
						#endif
						
						}	

				#endif /////////////////////////////////////////////////////////
				////////////////// [F4]-key automatic added... ///////////////////////


					}
				}

			} while(nLength);  
		}



		if( EV_ERR == (dwEvtMask & EV_ERR) ) /// A line-status error occurred. Line-status errors are CE_FRAME, CE_OVERRUN, and CE_RXPARITY.
		{
			/// inform about line-status errors. Line-status errors are CE_FRAME, CE_OVERRUN, and CE_RXPARITY.
			///
			/// 
			// Clear OVERRUN condition.
			// If OVERRUN error is occurred,the tx/rx will be locked.
		  	//// ---- EB_Printf(TEXT("[dnw] [EV_ERR=0x%X] \n"),dwErrorFlags);
			/// 2013.12.24 --- dwWritten = 0; /// 2013.09.30, added.
			/// --------------
			/// Retrieves information about a communications error and reports the current status of a communications device. 
			/// The function is called when a communications error occurs, 
			/// and it clears the device's error flag to enable additional input and output (I/O) operations.

			// EB_Printf(TEXT("\r\n[dnw] Line error detected.. Err(%u) \r\n"), GetLastError() ); 

			//PurgeComm(idComDev,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR); // added. 2016.03.29

			memset(&comStat, 0x00, sizeof(COMSTAT) );
			if( ClearCommError(idComDev,&dwErrorFlags,&comStat) )
			{
				// BYTE errreg = 0;

#define LSR_RX_DATA_READY_MASK 			0x01 
#define LSR_OVERRUN_ERROR_MASK 			0x02 
#define LSR_PARITY_ERROR_MASK 			0x04 
#define LSR_FRAMING_ERROR_MASK 			0x08 
#define LSR_RX_BREAK_MASK 				0x10 
#define LSR_TX_HOLDING_EMPTY_MASK 		0x20 
#define LSR_TX_EMPTY_MASK 				0x40 


			#if 0 /// __DEBUG__
				if( CE_BREAK == (dwErrorFlags&CE_BREAK) ) /// 0x0010  [inQ(%d), outQ(%d)]") );
				{
					EB_Printf(TEXT("\r\n[dnw] Line error detected a break condition.\r\n")); /// , comStat.cbInQue, comStat.cbOutQue  );
					errreg |= LSR_RX_BREAK_MASK;
				}

				if( CE_FRAME == (dwErrorFlags&CE_FRAME) ) /// 0x0008 
				{
					EB_Printf(TEXT("\r\n[dnw] Line error detected a framing error.\r\n")); /// , comStat.cbInQue, comStat.cbOutQue  );
					errreg |= LSR_FRAMING_ERROR_MASK;
				}

				if( CE_RXPARITY == (dwErrorFlags&CE_RXPARITY) ) /// 0x0004 
				{
					EB_Printf(TEXT("\r\n[dnw] Line error detected a parity error.\r\n")); /// , comStat.cbInQue, comStat.cbOutQue  );
					errreg |= LSR_PARITY_ERROR_MASK;
				}

				#if 0
				if( CE_OVERRUN == (dwErrorFlags&CE_OVERRUN) ) /// 0x0002 
				{
					EB_Printf(TEXT("\r\n[dnw] The overrun has occurred. The next character is lost. [inQ(%d), outQ(%d)] \r"), comStat.cbInQue, comStat.cbOutQue  );
					errreg |= LSR_OVERRUN_ERROR_MASK;
				}

				if( CE_RXOVER == (dwErrorFlags&CE_RXOVER) ) /// 0x0001 
				{
					EB_Printf(TEXT("\r\n[dnw] An input buffer overflow has occurred. [inQ(%d), outQ(%d)] \r"), comStat.cbInQue, comStat.cbOutQue  );
				}
				#endif
				
			#endif //////////////////////

			}

			/// ------------------------------------
			/// Discards all characters from the output or input buffer of a specified communications resource. 
			/// It can also terminate pending read or write operations on the resource.


			// ~~~~~~~ CloseComPort();
			// ~~~~~~~ isConnected = 0;

			// EB_Printf(TEXT("[dnw] COM%d port errors occurred. dwErrorFlags(0x%X), inQ/outQ(%d/%d) \r\n\r\n"), userComPort, dwErrorFlags, comStat.cbInQue, comStat.cbOutQue  );
			//--- memset(rxBuf, 0x00, sizeof(rxBuf));
			//--- PurgeComm(idComDev,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR); /// 2014.08.13
			
		}


	#ifdef __TEST__
		if( EV_BREAK == (dwEvtMask & EV_BREAK) )  /// A break was detected on input.
		{
			memset(&comStat, 0x00, sizeof(COMSTAT) );
			ClearCommError(idComDev, &dwErrorFlags, &comStat);

			PurgeComm(idComDev,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR); /// 2014.08.13
		}

		if( EV_RXFLAG == (dwEvtMask & EV_RXFLAG) ) 
		{
			aaaa
		}
	#endif
	

	#ifdef __TEST__ //////////////////////
		if( EV_CTS == (dwEvtMask & EV_CTS) )  /// The CTS (clear-to-send) signal changed state.
		{
			/// inform about changes of CTS (clear-to-send) signal.
			EB_Printf(TEXT("\r\n[dnw] EV_CTS signal!!! \r\n"));
		}

		if( EV_DSR == (dwEvtMask & EV_DSR) ) /// The DSR (data-set-ready) signal changed state.
		{
			/// inform about changes of DSR (data-set-ready)signal.
			EB_Printf(TEXT("\r\n[dnw] EV_DSR signal!!! \r\n"));
		}

		if( EV_RING == (dwEvtMask & EV_RING) )  /// A ring indicator was detected.
		{
			/// A ring indicator was detected.
			EB_Printf(TEXT("\r\n[dnw] EV_RING signal!!! \r\n"));
		}

		if( EV_RLSD == (dwEvtMask & EV_RLSD) )  /// The RLSD (receive-line-signal-detect) signal changed state.
		{
			/// inform about changes of RLSD (receive-line-signal-detect) signal.
			EB_Printf(TEXT("\r\n[dnw] EV_RLSD signal!!! \r\n"));
		}

		if( EV_RX80FULL == (dwEvtMask & EV_RX80FULL) )
		{
			/// inform when the buffer is 80 percent full.
			EB_Printf(TEXT("\r\n[dnw] EV_RX80FULL signal!!! \r\n"));
		}	
	#endif


	}	


	memset(&comStat, 0x00, sizeof(COMSTAT) );
	ClearCommError(idComDev,&dwErrorFlags,&comStat); // 2016.03.01

	if( osRead.hEvent ) { CloseHandle(osRead.hEvent); osRead.hEvent = NULL; }
	if( osWrite.hEvent ) { CloseHandle(osWrite.hEvent); osWrite.hEvent = NULL; }
	if( os.hEvent ) { CloseHandle(os.hEvent); os.hEvent=NULL; }

 	if( idComDev ) { CloseHandle(idComDev); idComDev=NULL; }


	
	/* _endthread given to terminate */
	_endthread();

}


// int ReadCommBlock(char *buf,int maxLen) // 2016.10.14
int ReadCommBlock(TCHAR *buf, int maxLen) 
{
	BOOL fReadStat = FALSE;
	COMSTAT comStat;
	DWORD dwErrorFlags = 0;
	DWORD dwLength = 0, dwLengthRead=0;
	DWORD dwError = 0;



	memset( &comStat, 0x00, sizeof(COMSTAT) ); // 2015.11.23

	/// only try to read number of bytes in queue
	ClearCommError(idComDev,&dwErrorFlags,&comStat);
	dwLength = min((DWORD)maxLen,comStat.cbInQue);


	if(dwLength>0)
	{

#if IS_OVERLAPPED_IO

		fReadStat = ReadFile(idComDev, buf, dwLength, &dwLengthRead, &osRead);
		
	#ifdef IS_OVERLAPPED_CHECK /// 2014.03.04 ----
		if( HasOverlappedIoCompleted(&osRead) )
		{
			// EB_Printf(TEXT("Completed") );
		}
		else
	#endif /// -------------------------
		if(!fReadStat) 
		{
			// failed : by experiment, fReadStat was always TRUE, of course, and the following was never executed.
			//--> 아직 전송할 문자가 남았을 경우..
			if( ERROR_IO_PENDING == GetLastError() )
			{

		#ifdef IS_OVERLAPPED_CHECK /// 2014.03.04 ----
				// I/O가 완료될 때 가지 기다린다.
				WaitForSingleObject(osRead.hEvent, IS_OVERLAPPED_WAIT);
		#endif /// -----------------------------
	

				// 읽을 문자가 남아 있거나 전송할 문자가 남아 있을 경우 Overapped IO의 특성에 따라 ERROR_IO_PENDING 에러 메시지가 전달된다.
				// timeouts에 정해준 시간만큼 기다려준다.
				// dwLength : 실제 전송이 완료된 바이트 크기를 얻기 위한 변수.
				// TRUE : I/O 연산이 끝날때까지 기다린다.
				// FALSE : I/O 연산이 끝나지 않아도 반환한다.
				while( !GetOverlappedResult(idComDev,&osRead,&dwLength,TRUE) ) /// 2012.07.03 /// if() -> while()
				{
					dwError = GetLastError();
					if(ERROR_IO_INCOMPLETE == dwError ) 
					{
						// -------------------------------------
						// normal result if not finished .
						// 정상 -- I/O 가 완료되지 않았음. 
						// -------------------------------------
						continue;
					}
					else if(ERROR_HANDLE_EOF == dwError ) // 2016.02.05
					{
						/// End of file reached.
						memset(&comStat, 0x00, sizeof(COMSTAT) );
						ClearCommError(idComDev,&dwErrorFlags,&comStat); // 2016.03.01
						EB_Printf(TEXT("\r\n[dnw] COM%d port End of file reached! (%s) \r\n\r\n"), userComPort, APP_VER );
						break;
					}
					else
					{
						// an error occurred, try to recover
						memset(&comStat, 0x00, sizeof(COMSTAT) );
						ClearCommError(idComDev, &dwErrorFlags, &comStat); // 2016.03.01
						break;
					}
				}
			}
			else
			{

				DWORD dwError = GetLastError();
				EB_Printf(TEXT("[dnw] find COM%d port Rx error. (%s) (Len:%d, Error:%u) \n"), userComPort, APP_VER, dwLength, dwError );

				// 2015.11.21 added
				memset(rxBuf, 0x00, sizeof(rxBuf));
				CloseComPort();
				isConnected = 0;

		#ifdef IS_OVERLAPPED_CHECK // 2015.11.21 ----
				// I/O가 완료될 때 가지 기다린다. -> HandleClose
				if(osRead.hEvent) { CloseHandle(osRead.hEvent); osRead.hEvent = NULL; }
				if(osWrite.hEvent) { CloseHandle(osWrite.hEvent);  osWrite.hEvent = NULL; }
		#endif /// -----------------------------

				if( idComDev ) { CloseHandle(idComDev); idComDev=NULL; }

				/// some other error occurred
				memset(&comStat, 0x00, sizeof(COMSTAT) );
				ClearCommError(idComDev, &dwErrorFlags, &comStat); /// 2012.07.03
				dwLength = 0; // 2016.03.01
				PurgeComm(idComDev,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR); /// added. 2014.07.08

			}
		}
#else
		fReadStat=ReadFile(idComDev,buf,dwLength,&dwLength,NULL);
		if(!fReadStat)
		{
			DWORD dwError = GetLastError();
			EB_Printf(TEXT("[dnw] find COM%d port Rx error. (Err:%u) \r\n\r\n"), userComPort, dwError );
		}
#endif
	}


	return dwLength;

}




// #define TX_SIZE 2048
// TX_SIZE must be less than 4096
void TxFile(void *args)
{
	void *txBlk = NULL;
	DWORD txBlkSize;
	DWORD fWriteStat;
	DWORD temp;
	COMSTAT comStat; /// 2011.11.29
	DWORD dwErrorFlags = 0;
	DWORD dwError;



	memset(&comStat, 0x00, sizeof(COMSTAT) );

	InitDownloadProgress();
	while( FALSE==txEmpty )
	{
		;
	}
	
	if( 0==TxBufSize ) TxBufSize=1; // 2010.05.25 critical error fix.


	while(1)
	{
		if((TxBufSize-idxTxBuf) > TX_SIZE)
		{
			txBlkSize = TX_SIZE;
		}
		else
		{
			txBlkSize = TxBufSize-idxTxBuf;
		}

		txBlk=(void *)(SerialTxBuf+idxTxBuf);

		txEmpty = FALSE;


    #if IS_OVERLAPPED_IO
		fWriteStat = WriteFile(idComDev,txBlk,txBlkSize,&temp,&osWrite);


	#ifdef IS_OVERLAPPED_CHECK /// 2014.03.04 ----
		if( HasOverlappedIoCompleted(&osWrite) )
		{
			/// EB_Printf(TEXT("Completed") );
		}
		else
	#endif /// -------------------------
		if(!fWriteStat)   //Apr.28.2003:GetOverlappedResult() may be needed.
		{
			if( ERROR_IO_PENDING==GetLastError() )
			{

		#ifdef IS_OVERLAPPED_CHECK /// 2014.03.04 ----
				// I/O가 완료될 때 가지 기다린다.
				WaitForSingleObject(osWrite.hEvent, IS_OVERLAPPED_WAIT);
		#endif /// -----------------------------
	
				while( ! GetOverlappedResult(idComDev,&osWrite,&temp,TRUE) ) /// 2014.01.04 /// if() -> while()
				{
					//more efficient in order to save the cpu time
					dwError = GetLastError();
					if(ERROR_IO_INCOMPLETE == dwError ) 
					{
						// normal result if not finished .
						// I/O 가 완료되지 않았음. 정상 ----
						continue;
					}
					else if(ERROR_HANDLE_EOF == dwError ) // 2016.02.05
					{
						// End of file reached.
						memset(&comStat, 0x00, sizeof(COMSTAT) );
						ClearCommError(idComDev, &dwErrorFlags, &comStat); // 2016.03.01
						EB_Printf(TEXT("\r\n[dnw] COM%d port End of file reached in TxFile! (%s) \r\n\r\n"), userComPort, APP_VER );
						break;
					}
					else
					{
						/// 2013.12.24 --- temp = 0;
						memset(&comStat, 0x00, sizeof(COMSTAT) );
						ClearCommError(idComDev, &dwErrorFlags, &comStat);
						break;
					}
				}
			}
			else
			{
				DWORD dwError = GetLastError();
				EB_Printf(TEXT("[dnw] find error in TxFile. (%s) (Err:%u) \r\n\r\n"),  APP_VER, dwError );

				/// 2013.12.24 --- temp = 0;
				memset(&comStat, 0x00, sizeof(COMSTAT) );
				ClearCommError(idComDev, &dwErrorFlags, &comStat); /// 2012.07.03
				/// 2013.12.26, PurgeComm(idComDev,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR); /// added. 2012.02.17
				///// break; /////////////// added.
			}
		}
		else
		{
		    //By experiment, never executed.
		    //EB_Printf(TEXT("\n[dnw] [TX_NO_WR_WAIT]\n") ); 
		}
    #else
		WriteFile(idComDev,txBlk,txBlkSize,&temp,NULL);
		Sleep(TX_SIZE*1000/11520-10); //to save cpu time.
		while(txEmpty==FALSE);
    #endif

		idxTxBuf+=TX_SIZE;

		if(TxBufSize>0)
			DisplayDownloadProgress(idxTxBuf*100/TxBufSize);
		else
			EB_Printf(TEXT("[dnw] TxBufSize is none. (%s) (TxBufSize:%d) \r\n\r\n"), APP_VER, TxBufSize );

		if(downloadCanceled==1)break;	//download is canceled by user.

		if(idxTxBuf>=TxBufSize)break;
	}


	if(SerialTxBuf) { free((void *)SerialTxBuf);  SerialTxBuf = NULL; }

	CloseDownloadProgress();

	_endthread();
}



void TxFileNew(void *args)
{
#define TX_UART_DATA_LEN 		2048 // 16

	FILE *fpTxMenuFile=NULL;
	char cch[TX_UART_DATA_LEN]= {0,};
	unsigned int rdnBytes = 0; //, ilp=0;
	__int64 TotalBytes = 0UL;
	//TCHAR szcch[TX_UART_DATA_LEN*4+TX_UART_DATA_LEN] = {0,};


	//EB_Printf(TEXT("\r\n[dnw] Tx File:[%s] \r\n"), szFileName ); 

	// input file check
	if( FALSE == isFileExist(szFileName) )
	{
		isTxUARTDataExit = 0; // Tx Data for UpdateWindowTitle()

		isMenuTxing = 0;

		/* _endthread given to terminate */
		_endthread();

		EB_Printf( TEXT("\r\n[dnw] input file in TxFileNew not found! \r\n") );
		BeepSound(4); // Error
	
		return;
	}


	fpTxMenuFile = fopen( (const char*)szFileName, "rb");
	if( NULL == fpTxMenuFile )
	{
		EB_Printf(TEXT("[dnw] can not open input [%s]. Err(%u)\n"), szFileName, GetLastError() );
		fclose(fpTxMenuFile);

		isTxUARTDataExit = 0; // Tx Data for UpdateWindowTitle()

		isMenuTxing = 0;
		
		/* _endthread given to terminate */
		_endthread();
		return;
	}


	InitDownloadProgressUART(); // UART port closed

	isMenuTxing = 1;
	/*-----------------------------------------------------------------------------*/
	// ilp = 0;
	TotalBytes = 0;
	isTxUARTDataExit = 0;

	do {
		if( isTxUARTDataExit ) break;


		memset( cch, 0x00, sizeof(cch) );

		rdnBytes = fread ( cch, 1, TX_UART_DATA_LEN /*sizeof(cch)*/, fpTxMenuFile);
		TotalBytes += rdnBytes;

	#if 1
		WriteCommBlockArray(cch, rdnBytes);
	#else
		for(ilp=0; ilp<TX_UART_DATA_LEN && ilp< rdnBytes; ilp++)
		{
			cch[ilp] = cch[ilp] & 0x00FF;

			WriteCommBlock( cch[ilp] );
			//if( txDataMsgOnOff ) wsprintf( &szcch[ilp*3], TEXT("%02X "), cch[ilp] );
		}
	#endif

		//if( txDataMsgOnOff ) EB_Printf( TEXT("[dnw] %s \r\n"), szcch );
		if(iUARTTxFileSize) 
			DisplayDownloadProgressUART( TotalBytes, iUARTTxFileSize );

	} while( !feof (fpTxMenuFile) );

	fclose(fpTxMenuFile);
	fpTxMenuFile = NULL;

	//iloop = TotalBytes;
	isTxUARTDataExit = 0; // Tx Data for UpdateWindowTitle()

	/*-----------------------------------------------------------------------------*/
	CloseDownloadProgressUART(); // UART port closed
	isMenuTxing = 0;

	/* _endthread given to terminate */
	_endthread();
}




void WriteCommBlock(char c)
{
	void *txBlk = NULL;
	DWORD txBlkSize;
	static char _c;
	DWORD temp = 0;
	DWORD fWriteStat;  // 2011.11.29
	COMSTAT comStat;  // 2011.11.29
	DWORD dwErrorFlags = 0;
	DWORD dwError = 0;


	memset(&comStat, 0x00, sizeof(COMSTAT) );
	ClearCommError(idComDev,&dwErrorFlags,&comStat); // 2016.03.26


	_c=c;


	while(txEmpty==FALSE)
	{
		;
	}



	txBlk=&_c;
	txBlkSize=1;

	//txEmpty=FALSE; why needed??? this line should be removed.
#if IS_OVERLAPPED_IO

	fWriteStat = WriteFile(idComDev,txBlk,txBlkSize,&temp,&osWrite);

#ifdef IS_OVERLAPPED_CHECK /// 2014.03.04 ----
	if( HasOverlappedIoCompleted(&osWrite) )
	{
		/// EB_Printf(TEXT("Completed") );
	}
	else
#endif /// -------------------------

	if(!fWriteStat)   //Apr.28.2003:GetOverlappedResult() may be needed.
	{
		if( ERROR_IO_PENDING==GetLastError() )
		{

	#ifdef IS_OVERLAPPED_CHECK /// 2014.03.04 ----
			// I/O가 완료될 때 가지 기다린다.
			WaitForSingleObject(osWrite.hEvent, IS_OVERLAPPED_WAIT);
	#endif /// -----------------------------


			while( ! GetOverlappedResult(idComDev,&osWrite,&temp,TRUE) ) /// 2014.01.04 /// if() -> while()
			{
				//more efficient in order to save the cpu time
				dwError = GetLastError();
				if(ERROR_IO_INCOMPLETE == dwError ) 
				{
					// --------------------------------------
					// normal result if not finished .
					// 정상 -- I/O 가 완료되지 않았음. 
					// --------------------------------------
					continue;
				}
				else if(ERROR_HANDLE_EOF == dwError ) // 2016.02.05
				{
					/// End of file reached.
					memset(&comStat, 0x00, sizeof(COMSTAT) );
					ClearCommError(idComDev, &dwErrorFlags, &comStat); // 2016.03.01
					EB_Printf(TEXT("\r\n[dnw] COM%d port End of file reached in WriteCommBlock! (%s) \r\n\r\n"), userComPort, APP_VER );
					break;
				}
				else
				{
					/// 2013.12.24 --- temp = 0; /// 2013.09.30, added
					memset(&comStat, 0x00, sizeof(COMSTAT) );
					ClearCommError(idComDev, &dwErrorFlags, &comStat);
					break;
				}
			}
		}
		else
		{
			/// 2013.12.24 --- temp = 0; /// 2013.09.30, added
			memset(&comStat, 0x00, sizeof(COMSTAT) );
			ClearCommError(idComDev, &dwErrorFlags, &comStat); /// 2012.07.04
			/// 2013.12.26, PurgeComm(idComDev,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR); /// added. 2012.02.17
			///EB_Printf(TEXT("[dnw] ERROR:WriteCommBlock error!! \r\n") );
			///// break; /////////////// added.
		}
	}

#else
	WriteFile(idComDev,txBlk,txBlkSize,&temp,NULL);
#endif

	while(txEmpty==FALSE)
	{
		;
	}

}
    

// 2016.10.13
void WriteCommBlockArray(char *szC, DWORD szSize)
{
	void *txBlk = NULL;
	DWORD txBlkSize;
	//static char _c;
	DWORD temp = 0;
	DWORD fWriteStat;  // 2011.11.29
	COMSTAT comStat;  // 2011.11.29
	DWORD dwErrorFlags = 0;
	DWORD dwError = 0;


	memset(&comStat, 0x00, sizeof(COMSTAT) );
	ClearCommError(idComDev,&dwErrorFlags,&comStat); // 2016.03.26


//	_c=c;


	while(txEmpty==FALSE)
	{
		;
	}



	txBlk     = szC;
	txBlkSize = szSize;

	//txEmpty=FALSE; why needed??? this line should be removed.
#if IS_OVERLAPPED_IO

	fWriteStat = WriteFile(idComDev,txBlk,txBlkSize,&temp,&osWrite);

#ifdef IS_OVERLAPPED_CHECK /// 2014.03.04 ----
	if( HasOverlappedIoCompleted(&osWrite) )
	{
		/// EB_Printf(TEXT("Completed") );
	}
	else
#endif /// -------------------------

	if(!fWriteStat)   //Apr.28.2003:GetOverlappedResult() may be needed.
	{
		if( ERROR_IO_PENDING==GetLastError() )
		{

	#ifdef IS_OVERLAPPED_CHECK /// 2014.03.04 ----
			// I/O가 완료될 때 가지 기다린다.
			WaitForSingleObject(osWrite.hEvent, IS_OVERLAPPED_WAIT);
	#endif /// -----------------------------


			while( ! GetOverlappedResult(idComDev,&osWrite,&temp,TRUE) ) /// 2014.01.04 /// if() -> while()
			{
				//more efficient in order to save the cpu time
				dwError = GetLastError();
				if(ERROR_IO_INCOMPLETE == dwError ) 
				{
					// --------------------------------------
					// normal result if not finished .
					// 정상 -- I/O 가 완료되지 않았음. 
					// --------------------------------------
					continue;
				}
				else if(ERROR_HANDLE_EOF == dwError ) // 2016.02.05
				{
					/// End of file reached.
					memset(&comStat, 0x00, sizeof(COMSTAT) );
					ClearCommError(idComDev, &dwErrorFlags, &comStat); // 2016.03.01
					EB_Printf(TEXT("\r\n[dnw] COM%d port End of file reached in WriteCommBlock! (%s) \r\n\r\n"), userComPort, APP_VER );
					break;
				}
				else
				{
					/// 2013.12.24 --- temp = 0; /// 2013.09.30, added
					memset(&comStat, 0x00, sizeof(COMSTAT) );
					ClearCommError(idComDev, &dwErrorFlags, &comStat);
					break;
				}
			}
		}
		else
		{
			/// 2013.12.24 --- temp = 0; /// 2013.09.30, added
			memset(&comStat, 0x00, sizeof(COMSTAT) );
			ClearCommError(idComDev, &dwErrorFlags, &comStat); /// 2012.07.04
			/// 2013.12.26, PurgeComm(idComDev,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR); /// added. 2012.02.17
			///EB_Printf(TEXT("[dnw] ERROR:WriteCommBlock error!! \r\n") );
			///// break; /////////////// added.
		}
	}

#else
	WriteFile(idComDev,txBlk,txBlkSize,&temp,NULL);
#endif

	while(txEmpty==FALSE)
	{
		;
	}

}
    

#ifdef APPRUN_FILES_TO_ENCIPHER // 2016.05.27 by top.joo


#define STRICT_TYPED_ITEMIDS    // Better type safety for IDLists
#include <shtypes.h>
#include <shlobj.h>             // Typical Shell header file

LPITEMIDLIST PidlBrowse(HWND hwnd, int nCSIDL, LPSTR pszDisplayName)
{
    LPITEMIDLIST pidlRoot = NULL;
    LPITEMIDLIST pidlSelected = NULL;
    BROWSEINFO bi = {0};

    if(nCSIDL)
    {
        ::SHGetFolderLocation(hwnd, nCSIDL, NULL, NULL, &pidlRoot);
    }
    else
    {
        pidlRoot = NULL;
    }

    bi.hwndOwner = hwnd;
    bi.pidlRoot = pidlRoot;
    bi.pszDisplayName = pszDisplayName;
    bi.lpszTitle = "Choose a folder";
    bi.ulFlags = 0;
    bi.lpfn = NULL;
    bi.lParam = 0;

    pidlSelected = ::SHBrowseForFolder(&bi);

    if(pidlRoot)
    {
        CoTaskMemFree(pidlRoot);
    }

    return pidlSelected;
}



///////////////////////////////////////////////////
// 디렉토리를 선택하기 위한 shell 사용한 예
///////////////////////////////////////////////////
#include <Shlobj.h>

void GetFolderName(HWND hwnd) 
{
#if 0

	char m_dirFolderPath[MAX_PATH] = {0,}; // 선택된 디렉토리의 path명 까지 포함한 디렉토리  예: C:\tmp\test
	char m_dirName[MAX_PATH] = {0,};	// 선택된 디렉토릴 저장   예:test
	BROWSEINFO   bi; 
	LPITEMIDLIST  idl;

	ZeroMemory(&bi,sizeof(BROWSEINFO)); 

	bi.hwndOwner      = hwnd; // this->m_hWnd; 
	bi.pszDisplayName = m_dirName;  // 선택된 디렉토리명 저장 
	bi.lpszTitle      = TEXT("Select Folder"); 
	bi.ulFlags        =  BIF_EDITBOX    // 선택된 디렉토리명을 표시하고 편집할 수 있는 창
						| 0x0040        // 새로운 디렉토리를 만들수 있는 버튼 추가 
						; 

	idl = SHBrowseForFolder(&bi); 

	if (idl) { 
		SHGetPathFromIDList(idl, m_dirFolderPath); // 전체 PATH을 포함한 디렉토리명

    }
#endif


	TCHAR cpath[MAX_PATH] = _T("");
	LPITEMIDLIST pDirList;
	BROWSEINFO browseInfo;

	ZeroMemory(&browseInfo,sizeof(BROWSEINFO)); 

	browseInfo.hwndOwner = hwnd; // NULL;

	browseInfo.pidlRoot = NULL;

	browseInfo.lpszTitle = _T("Select Folder");

	browseInfo.pszDisplayName = cpath;

	browseInfo.ulFlags = BIF_RETURNONLYFSDIRS
			| BIF_EDITBOX    // 선택된 디렉토리명을 표시하고 편집할 수 있는 창
			| 0x0040        // 새로운 디렉토리를 만들수 있는 버튼 추가 
			; 

	browseInfo.lpfn = NULL;

	browseInfo.lParam = 0;

	pDirList = SHBrowseForFolder(&browseInfo);

	if(pDirList == NULL) return;

	SHGetPathFromIDList(pDirList, cpath);

	AfxMessageBox(cpath);


}



CString m_initialFolder = TEXT("");


INT CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
{
	TCHAR szDir[MAX_PATH] = {0,};

	switch(uMsg)
	{
		case BFFM_INITIALIZED:

			if (!m_initialFolder.IsEmpty())
			{
				// WParam is TRUE since you are passing a path
				// it would be FALSE if you were passing a pidl
				::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCSTR)m_initialFolder.GetBuffer(0));
			}

			break;

		case BFFM_SELCHANGED:

			// set the status window to the currently selected path
			if (SHGetPathFromIDList((LPITEMIDLIST)lp, szDir))
			{
				::SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szDir);
			}

			break;
	}

	return 0;
}


int SelectDirectory(TCHAR* pFolder)
{
	int iRet=IDCANCEL;
//	m_selectedFolder.Empty();

	// A thread must call CoInitializeEx or CoInitialize before calling any other COM library function
	// except the CoGetMalloc function and other memory allocation calls. (here: BIF_NEWDIALOGSTYLE)
	HRESULT result = CoInitializeEx(0, COINIT_APARTMENTTHREADED);

	// check result
	if ((result == S_OK) || (result == S_FALSE))
	{
		// interface pointer to the memory allocator
		LPMALLOC pMalloc;

		// Retrieves a pointer to the default OLE task memory allocator
		// (which supports the system implementation of the IMalloc interface)
		// so applications can call its methods to manage memory.
		if (CoGetMalloc(1, &pMalloc) == S_OK)
		{
			TCHAR buffer[MAX_PATH] = {0,};

			// initialize the BROWSEINFO struct
			BROWSEINFO bi;
			bi.hwndOwner      = this->m_hWnd;
			bi.pidlRoot       = 0;
			bi.pszDisplayName = buffer;
			bi.lpszTitle      = L"Select Folder";
			bi.ulFlags        = (BIF_RETURNONLYFSDIRS | BIF_RETURNFSANCESTORS | BIF_NEWDIALOGSTYLE);
			bi.lpfn           = BrowseCallbackProc;
			bi.lParam         = 0;

			// hide button if necessary
			//if (m_hideNewFolderButton)
			{
			//	bi.ulFlags |= BIF_NONEWFOLDERBUTTON;
			}

			// Displays a dialog box enabling the user to select a Shell folder.
			// You must initialize Component Object Model (COM) using CoInitializeEx with the
			// COINIT_APARTMENTTHREADED flag set in the dwCoInit parameter prior to calling SHBrowseForFolder.
			LPITEMIDLIST pidl = ::SHBrowseForFolder(&bi);

			// cancel will return 0
			if (pidl != 0)
			{
				// Converts an item identifier list to a file system path.
				if (::SHGetPathFromIDList(pidl, buffer))
				{
					memcpy(pFolder, buffer,wcslen(buffer)*sizeof(TCHAR));
					iRet=IDOK;
				}

				// Frees a previously allocated block of memory. (IMalloc::Free)
				pMalloc->Free(pidl);
			}

			// Decrements the reference count. (IUnknown::Release)
			pMalloc->Release();
		}

		// To close the COM library gracefully on a thread, each successful call to
		// CoInitialize or CoInitializeEx, including any call that returns S_FALSE,
		// must be balanced by a corresponding call to CoUninitialize.
		CoUninitialize();
	}

	// check selected folder
	return iRet;
}

CString m_strOpenPath;
CString m_strCurPath;
TCHAR m_strMultiBinDir[MAX_PATH];

void OnBnClickedBtnOpenOsDir()
{
    // TODO: Add your control notification handler code here
	memset(m_strMultiBinDir,0x00,sizeof(m_strMultiBinDir));

	if(m_strOpenPath.GetLength()>0)
	 	m_initialFolder=m_strOpenPath;
	else	 m_initialFolder=m_strCurPath;
	if(SelectDirectory(m_strMultiBinDir)==IDOK)
	{
		m_edt_os_dir.SetWindowText(m_strMultiBinDir);
		m_strOpenPath = m_strMultiBinDir;  
	}
}


BOOL ChecknMakeFolder()
{
	BOOL bRet = TRUE;
	
	if( FALSE==isFileExist(APP_SECOND_DIR) ) {
		if( FALSE == CreateDirectory( APP_SECOND_DIR, NULL) ) {
			LOGMSG(DEBUG_LEVEL_ERR,TEXT("CreateDirectory %s Fail Error:%d\r\n"),APP_SECOND_DIR,GetLastError());
			bRet = FALSE;
		}
	}
	
	if( FALSE==isFileExist(APP_FIRST_DIR) ) {
		if( FALSE == CreateDirectory( APP_FIRST_DIR, NULL) ) {
			LOGMSG(DEBUG_LEVEL_ERR,TEXT("CreateDirectory %s Fail Error:%d\r\n"),APP_FIRST_DIR,GetLastError());
			bRet = FALSE;
		}
	}

	return bRet;
}


BOOL SetFileToCurrentTime(HANDLE hFile, SYSTEMTIME st)
{
	FILETIME ft, ft_utc;
	BOOL bRet = TRUE;

	memset( &ft, 0x00, sizeof(FILETIME) );
	memset( &ft_utc, 0x00, sizeof(FILETIME) );

	// Converts the current system time to file time format
    SystemTimeToFileTime( &st, &ft );
    LocalFileTimeToFileTime( &ft, &ft_utc );
    FileTimeToSystemTime( &ft_utc, &st );
    SystemTimeToFileTime( &st, &ft );

	bRet = SetFileTime(hFile, (LPFILETIME) NULL, (LPFILETIME) NULL, &ft);	 
	
	return bRet;
}


DWORD ReadMergeFile(FILE *pFile, PBYTE pBuffer,DWORD dwSize)
{

#define DATEPACKET_SIZE  4096

	DWORD dwLen=0, dwReadLen=0;

	while(dwSize){
		dwLen = dwSize>DATEPACKET_SIZE ? DATEPACKET_SIZE: dwSize;

		if(fread(pBuffer,1,dwLen,pFile) != dwLen){
			//TRACE(_T("CUpdateDlg::ReadMergeFile failed dwReadLen(%u), dwSize(%u) \r\n"),dwReadLen,dwSize);
			LOGMSG(DEBUG_LEVEL_ERR,_T("CUpdateDlg::ReadMergeFile failed+++++ dwReadLen(%u), dwSize(%u) \r\n"), dwReadLen, dwSize );
			break;
		}

		dwReadLen += dwLen;
		pBuffer += dwLen;
		dwSize -= dwLen; 
	}

	return dwReadLen;
}


BOOL ExtractAppRunFiles()
{
	FILE *LGE1File = NULL, *exFile=NULL;
	DWORD iNum=0, dwFileAttr=0; // , iCount=0; 
	BYTE* pMem = NULL;
	BOOL bRet = TRUE;
	TCHAR  szExtractFile[MAX_PATH] = {0,}, szSourceFile[MAX_PATH]={0,};
	HANDLE hFileTime=NULL;
	SYSTEMTIME localsysTime; 

	APPRUN_ROOT_INFO   AppRunRootInfo;
	APPRUN_MERGED_INFO AppRunFileInfo[TOTAL_SEARCH_FILE_NUM];


	if( !IsValidBinByMD5(MEUDISK_UPDATE_MERGED_APPRUN_LGE) ) {
		LOGMSG(DEBUG_LEVEL_ERR,_T("%s IsValidBinByMD5 in MEUDisk5 is invaild Error ++++++ \r\n"), MEUDISK_UPDATE_MERGED_APPRUN_LGE );
		return FALSE;
	}

	//wsprintf(szSourceFile, TEXT("%s"), MEUDISK_UPDATE_MERGED_APPRUN_LGE );
	if( APPRUN_SDMMC==m_AppRunPath)	
		wsprintf(szSourceFile, TEXT("%s%s"), SD_UPDATE_MEU_APP_DIR, MEUDISK_UPDATE_APPRUN_LGEFILE );
	else 
		wsprintf(szSourceFile, TEXT("%s%s"), MEUDISK_UPDATE_MEU_APP_DIR, MEUDISK_UPDATE_APPRUN_LGEFILE );


	LOGMSG(DEBUG_LEVEL_ERR,TEXT("ExtractAppRunFiles::[%s]  \r\n"), szSourceFile );

	LGE1File = _wfopen(szSourceFile, TEXT("rb"));

	if( NULL==LGE1File ) {
		LOGMSG(DEBUG_LEVEL_ERR,TEXT("ExtractAppRunFiles:: Can not read LGE file Error:%d ++ERROR++ \r\n"), GetLastError());
		return FALSE;
	}
	// -----------------------------------------------------------------------
	// Merged 된 AppRun ROOT Information (TOtal Count, Model, Version 정보
	// -----------------------------------------------------------------------
	memset( &AppRunRootInfo, 0x00, sizeof(APPRUN_ROOT_INFO) );
	if(fread(&AppRunRootInfo,1,sizeof(APPRUN_ROOT_INFO),LGE1File) != sizeof(APPRUN_ROOT_INFO)) {
		fclose(LGE1File);
		LOGMSG(DEBUG_LEVEL_ERR,TEXT("ExtractAppRunFiles:: ROOT_INFO Error:%d ++ERROR++ \r\n"), GetLastError());
		return FALSE;
	}

	LOGMSG(DEBUG_LEVEL_ERR,TEXT("ExtractAppRunFiles::Totoal File Count:%u Model:[%S], Vesion:[%S]\r\n"), 
		AppRunRootInfo.mergedTotalNum, AppRunRootInfo.mergedModel, AppRunRootInfo.mergedVersion );


	// -----------------------------------------------------------------------
	// Total Count 만큼 각 파일 정보  
	// -----------------------------------------------------------------------
	memset( &AppRunFileInfo, 0x00, sizeof(AppRunFileInfo) );
	for(iNum=0; iNum < AppRunRootInfo.mergedTotalNum && iNum<TOTAL_SEARCH_FILE_NUM; iNum++) {

		if(fread(&AppRunFileInfo[iNum],1,sizeof(APPRUN_MERGED_INFO),LGE1File) != sizeof(APPRUN_MERGED_INFO))
		{
			fclose(LGE1File);
			LOGMSG(DEBUG_LEVEL_ERR,TEXT("ExtractAppRunFiles:: APPRUN FILE INFO Error:%d ++ERROR++ \r\n"), GetLastError());
			return FALSE;
		}

		LOGMSG(DEBUG_LEVEL_ERR,TEXT("ExtractAppRunFiles::AppRunIndex:%2u File:[%S]-[%u]\r\n"), 
			AppRunFileInfo[iNum].mergedIndex, AppRunFileInfo[iNum].mergedFileName, AppRunFileInfo[iNum].mergedFileSize );
	}

	// -----------------------------------------------------------------------
	// FileName, Filesize 정보를 이용하여 Files Extract 
	// -----------------------------------------------------------------------
	for(iNum=0; iNum < AppRunRootInfo.mergedTotalNum && iNum<TOTAL_SEARCH_FILE_NUM ; iNum++) {

		// Extract 하고자 하는 경로 지정 --
		if( APPRUN_FIRST==m_SystemRunPath) 
			wsprintf(szExtractFile, TEXT("%s%S"), APP_SECOND_DIR, AppRunFileInfo[iNum].mergedFileName );
		else 
			wsprintf(szExtractFile, TEXT("%s%S"), APP_FIRST_DIR, AppRunFileInfo[iNum].mergedFileName );

		LOGMSG(DEBUG_LEVEL_ERR,TEXT("ExtractAppRunFiles::(%2d):[%s] = [%u]\r\n"), iNum, szExtractFile, AppRunFileInfo[iNum].mergedFileSize );

		ChecknMakeFolder();

		if( isFileExist(szExtractFile) ) { // Read File Attribute -> if it is READONLY attribute, removed it.

			dwFileAttr = GetFileAttributes(szExtractFile);
			dwFileAttr = dwFileAttr^FILE_ATTRIBUTE_READONLY;
			
			bRet = SetFileAttributes(szExtractFile, dwFileAttr);
			if( FALSE == bRet) {
				LOGMSG(DEBUG_LEVEL_ERR, TEXT("ExtractAppRunFiles::[%s] dwAttr:0x%x, err:(%u) ++ERROR++\r\n"), szExtractFile, dwFileAttr, GetLastError() );
			}

		#if 0
			do{
				if( !DeleteFile(szExtractFile) ) {
					LOGMSG(DEBUG_LEVEL_ERR,TEXT("ExtractAppRunFiles::DeleteFile %s Error:%d\r\n"), szExtractFile,GetLastError());
				}
				else {
					LOGMSG(DEBUG_LEVEL_01,TEXT("ExtractAppRunFiles::DeleteFile %s Success iCount:%d\r\n"), szExtractFile,iCount);
					break;
				}
				Sleep(1);
				iCount++;
			}while(iCount < DELETE_RETRY_COUNT); 
		#endif
		
		}
		
		exFile = _wfopen(szExtractFile, TEXT("wb"));
		if( exFile ) {
			pMem  = (LPBYTE)LocalAlloc(LPTR, AppRunFileInfo[iNum].mergedFileSize);

			if( pMem ) {
				if(ReadMergeFile(LGE1File, pMem, AppRunFileInfo[iNum].mergedFileSize) != AppRunFileInfo[iNum].mergedFileSize)
				{
					LOGMSG(DEBUG_LEVEL_ERR,TEXT("ExtractAppRunFiles:: ReadMergeFile iNum(%d)[%s] Error:%d ++ERROR++ \r\n"), iNum, szExtractFile, GetLastError());
					bRet = FALSE;
					break; // Error -> break
				}

				fwrite(pMem, AppRunFileInfo[iNum].mergedFileSize, 1, exFile); // Create File
			}
			else {
				LOGMSG(DEBUG_LEVEL_ERR,TEXT("ExtractAppRunFiles:: LocalAlloc iNum(%d)[%s] Error:%d ++ERROR++ \r\n"), iNum, szExtractFile, GetLastError());
				bRet = FALSE;	
				break; // Error -> break
			}

			if(pMem) { LocalFree(pMem); pMem = NULL; }
	
		}
		else {
			LOGMSG(DEBUG_LEVEL_ERR,TEXT("ExtractAppRunFiles::[%s] Error(%u) ++ERROR++ \r\n"), szExtractFile, GetLastError());
			bRet = FALSE;	
			break; // Error -> break
		}

		if(exFile) { fclose(exFile); exFile = NULL; }

		// --Set File Write Date/Time-------------------
		memset( &localsysTime, 0x00, sizeof(SYSTEMTIME) );
		memcpy( (void*)&localsysTime, (void*)&AppRunFileInfo[iNum].mergedSysTime, sizeof(SYSTEMTIME) );

		hFileTime = CreateFile(szExtractFile, GENERIC_READ|GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if( hFileTime ) {

			LOGMSG(DEBUG_LEVEL_ERR,TEXT("ExtractAppRunFiles::(%2d):[%s] = [%04d/%02d/%02d %02d:%02d:%02d] \r\n"), iNum, szExtractFile, 
				localsysTime.wYear, localsysTime.wMonth, localsysTime.wDay, localsysTime.wHour, localsysTime.wMinute, localsysTime.wSecond );

			SetFileToCurrentTime(hFileTime, localsysTime);
			CloseHandle(hFileTime);
		}

		dwFileAttr = GetFileAttributes(szExtractFile);
		dwFileAttr |= FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_ARCHIVE; // fdata.dwFileAttributes|FILE_ATTRIBUTE_SYSTEM|
		
		if( FALSE == SetFileAttributes(szExtractFile, dwFileAttr) ) {
			LOGMSG(DEBUG_LEVEL_ERR,TEXT("ExtractAppRunFiles::szFullDestPath[%s] FileAttr:[0x%X], dwError(%d) \r\n\r\n"),szExtractFile, dwFileAttr, GetLastError() );
		}
		// --------------------------------------------
		
	}
	
	if(LGE1File) { fclose(LGE1File); LGE1File = NULL; }

	if(pMem) { LocalFree(pMem); pMem = NULL; } 
	if(exFile) { fclose(exFile); exFile = NULL; } 

	return bRet;
}

#endif


char isTxtMode(void)
{
	if( (1==TextorHexaMode) || (2==TextorHexaMode) ) return HEXA_OUT_MODE; // Hexa-mode
	return TEXT_OUT_MODE; // Text mode
}



LONG CALLBACK UnhandledExceptionHandler(EXCEPTION_POINTERS *lpExceptionInfo)  
{  
	TCHAR szText[MAX_PATH] = {0,};

	wsprintf(szText, TEXT("Exception code : 0x%08X\r\nException Flag : 0x%08X\r\nNumber Parameter : 0x%08X\r\nException Addr : 0x%08p\r\n"), 
		lpExceptionInfo->ExceptionRecord->ExceptionCode,
		lpExceptionInfo->ExceptionRecord->ExceptionFlags,
		lpExceptionInfo->ExceptionRecord->NumberParameters,
		lpExceptionInfo->ExceptionRecord->ExceptionAddress );
	
	MessageBox(NULL, szText, TEXT(" +++ Exception Error +++ "), MB_OK);


	//EB_Printf(TEXT("[dnw] Exception code : 0x%08X\r\n"), lpExceptionInfo->ExceptionRecord->ExceptionCode );
	//EB_Printf(TEXT("[dnw] Exception Addr : 0x%08p\r\n"), lpExceptionInfo->ExceptionRecord->ExceptionAddress );

	// 0xC0000005
	// 0x77AA3129
	
	return EXCEPTION_EXECUTE_HANDLER;  
}  



#if __UNICODE_UTF8__
#include <Windows.h>
#include <stdlib.h> 
#include <math.h>

///ANSI 문자열에서 유니코드로 변환
LPWSTR AnsiToUnicode(LPSTR szAnsi)
{
	LPWSTR szUniStr;

	// 유니코드로 변환 전 return 되는 길이얻기
	int nLen = MultiByteToWideChar(CP_ACP, 0, szAnsi, -1, NULL, NULL);
	nLen = nLen * sizeof(WCHAR);

	szUniStr = (LPWSTR)malloc(nLen+1);// 메모리를 할당한다.
	memset(szUniStr,0,nLen+1);

	// 이제 변환을 수행한다.
	MultiByteToWideChar(CP_ACP, 0, szAnsi, -1, szUniStr, nLen);

	return szUniStr;
}

//유니코드에서 ANSI 문자열로 변환
LPSTR UnicodeToAnsi(LPWSTR szUniStr)
{
	LPSTR szAnsi;
	int nLen = WideCharToMultiByte(CP_ACP, 0, szUniStr, -1, NULL, 0, NULL, NULL);

	szAnsi = (LPSTR) malloc(nLen+1);
	memset(szAnsi,0,nLen+1);

	WideCharToMultiByte(CP_ACP, 0, szUniStr, -1, szAnsi, nLen, NULL, NULL);

	return szAnsi;
}

//유니코드에서 UTF8 문자열로 변환
LPWSTR UnicodeToUTF8(LPWSTR szUniStr)
{
	int idx=0;
	WCHAR uc;
	LPWSTR szUtf8=NULL;

	int nLen = sizeof(WCHAR) * wcslen(szUniStr);

	nLen = nLen*3+sizeof(WCHAR); //최대 값
	szUtf8 = (LPWSTR) malloc(nLen);

	memset(szUtf8,0,nLen);
	LPSTR pszUTF8 = (LPSTR)szUtf8;

	for(int i=0;szUniStr[i];i++)
	{
		uc = szUniStr[i];

		if (uc <= 0x7f)
		{
			pszUTF8[idx++] = (char) uc;
		}
		else if (uc <= 0x7ff)
		{
			pszUTF8[idx++] = (char) 0xc0 + uc / (wchar_t) pow(2, 6);
			pszUTF8[idx++] = (char) 0x80 + uc % (wchar_t) pow(2, 6);
		}
		else if (uc <= 0xffff)
		{
			pszUTF8[idx++] = (char) 0xe0 + uc / (wchar_t) pow(2, 12);
			pszUTF8[idx++] = (char) 0x80 + uc / (wchar_t) pow(2, 6) % (wchar_t) pow(2, 6);
			pszUTF8[idx++] = (char) 0x80 + uc % (wchar_t) pow(2, 6);
		}
	}

	return szUtf8;

/** Win95에서만 안되는 코드
http://support.microsoft.com/default.aspx?scid=kb%3Bko%3B601368
LPWSTR szUtf8;
int nLen = WideCharToMultiByte(CP_UTF8, 0, szUniStr, -1, NULL, 0, NULL, NULL);
szUtf8 = (LPWSTR) malloc(nLen+1);
memset(szUtf8,0,nLen+1);
WideCharToMultiByte(CP_UTF8, 0, szUniStr, -1, (LPSTR)szUtf8, nLen, NULL, NULL);

return szUtf8;
**/
}

//유니코드에서 UTF8 문자열로 변환
LPWSTR UTF8ToUnicode(LPWSTR szUtf8)
{
	LPWSTR szUniStr;
	int nLen = sizeof(WCHAR) * wcslen(szUtf8);

	nLen = nLen+sizeof(WCHAR);
	szUniStr = (LPWSTR) malloc(nLen);
	memset(szUniStr,0,nLen);

	LPSTR src = (LPSTR)szUtf8;

	int p = 0;
	int q = 0;
	WCHAR d;

	while( src[p] ) 
	{
		if( 0xE0==(src[p]&0xE0) )
		{
			d = ((src[p]&0x0f)<<12) | ((src[p+1]&0x3F)<<6) | (src[p+2]&0x3F);
			p+=3;
		} 
		else if( 0xC0==(src[p]&0xC0) ) 
		{
			d = ((src[p]&0x1F)<<6) | (src[p+1]&0x3F);
			p+=2;
		} 
		else
		{
			d = src[p]&0x7F;
			p++;
		}

		szUniStr[q] = d;
		q++;
	}

	szUniStr[q] = NULL;
	return szUniStr;
}

int main()
{
  LPSTR s="한글";
  LPWSTR szUniStr = AnsiToUnicode(s);
  LPWSTR szUtf8 = UnicodeToUTF8(szUniStr);
  LPWSTR szUniStr2 = UTF8ToUnicode(szUtf8);
  LPSTR szAnsi = UnicodeToAnsi(szUniStr);

  free(szUniStr);
  free(szUtf8);
  free(szUniStr2);
  free(szAnsi);

  return 0;
}


// 1. TCHAR --> CString
CStirng sSring;
TCHAR sTString = _T("가나다");
sString.Format(_T(%s), sTString);


// 2. CString --> TCHAR

TCHAR * des = new TCHAR[sString.GetLength() + 1]; // 1은 NULL을 위한 여유공간_tcscpy(des, NowSaving.GetBuffer(0));



// 3. String --> UTF - 8 
char buffer[20];WCHAR Unicode[20];
char UTF8code[20];int nUnicodeSize = MultiByteToWideChar(CP_ACP, 0, lpID, strlen(lpID), Unicode, sizeof(Unicode));
int nUTF8codeSize = WideCharToMultiByte(CP_UTF8, 0, Unicode, nUnicodeSize, UTF8code, sizeofUTF8code), NULL, NULL);
nUnicodeSize = MultiByteToWideChar(CP_UTF8, 0, UTF8code, nUTF8codeSize, Unicode, sizeof(Unicode));
memcpy(buffer, UTF8code, nUTF8codeSize);

// 이렇게 하면 buffer에 UTF-8로 변환


// 4. UTF-8 --> Unicode 
void CCPimFONEDevice::UTF82UNICODE(char * strSrc, wchar_t *strTarget) 
{  
    int nSize = MultiByteToWideChar(CP_UTF8, 0, strSrc, -1, 0, 0);  
    MultiByteToWideChar(CP_UTF8, 0, strSrc ,  -1 , strTarget, nSize);
}

// 5. char --> TCHAR 
char charBuff[]="test";
TCHAR szUniCode[256]={0,};
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, charBuff, strlen(charBuff), szUniCode, 256);


// 6. CString --> char*

//EVC는 기본적으로 유니코드를 사용하기 때문에 CString을 char*로 변환할 수 없음..
nToWrite = lpByte.GetLength();
char *pAnsiString = new char[nToWrite+1];memset(pAnsiString,0,nToWrite+1);
wcstombs(pAnsiString, lpByte, nToWrite+1); // wchar_t배열을 multibyte 문자열로 변환한다.

#endif



