
// =============================================
// Program Name: engine
// =============================================

#define STRICT
#define WIN32_LEAN_AND_MEAN


#define REMOVED_AFTER_PRESS_CTRL_C 		0 // FEATURing 2019.07.12


#include "resource.h"
#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>
#include <winreg.h>
#include <windowsx.h>
#include <CommCtrl.h>  /// 2010.05.28 added.
#include <process.h>  /* 2016.02.19 : For _beginthread() */

#include <wchar.h>

//#include <crtdbg.h> // memory leakage check

#if  0
#pragma comment(lib, "comctl32.lib") /// 2013.05.22
#endif



#pragma hdrstop

#include <stdio.h> //for _vsntprintf()
#include <stdarg.h> // for va_list, va_start
#include <string.h> // for memset
#include <stdlib.h>
#include <tchar.h>
#include <stddef.h>
#include <conio.h>
#include <time.h>
#include <winuser.h>
#include <math.h> // 2017.08.30
#if RICHED_COLOR // 2016.09.19 +++++++++
#include <richedit.h>
#endif

#include "engine.h"
#include "dnw.h"
#include "fileopen.h"
#include "font.h"
#include "d_box.h"
#include "usbtxrx.h"
#include "regmgr.h"  // added.
#include "md5.h"

#include ".\sha3\sha3.h" // 2017.08.22
#include ".\has160\has160.h" // 2018.06.15
#include ".\BLAKE\blake.h" // 2018.06.15


#if USE_RX_TX_QUEUE
#include "queue.h"

//------------------------------
// Queue 
//------------------------------
extern ctQueue Rx2Tx;
#endif


extern BOOL isFileExist(TCHAR *file);
extern BOOL ExecProgram (LPCTSTR lpAppName, LPTSTR lpCmdLine);


void CheckSum_MD5(void *args);
void CheckSum_SHA1(void *args);
void CheckSum_CRC32(void *args);
void CheckSum_CRC64(void *args);
void CheckSum_SHA2_256(void *args);
void CheckSum_SHA2_384(void *args);
void CheckSum_SHA2_512(void *args);
void CheckSum_SHA2_224(void *args);
void CheckSum_MD6(void *args);

void CheckSum_SHA3_KECCAK_224(void *args); // 2017.08.22
void CheckSum_SHA3_KECCAK_256(void *args); // 2017.08.22
void CheckSum_SHA3_KECCAK_384(void *args); // 2017.08.22
void CheckSum_SHA3_KECCAK_512(void *args); // 2017.08.22

void CheckSum_SHAKE128(void *args); // 2017.08.29
void CheckSum_SHAKE256(void *args);

void CheckSum_Blake224(void *args); // 2018.06.15
void CheckSum_Blake256(void *args);
void CheckSum_Blake384(void *args);
void CheckSum_Blake512(void *args);

void CheckSum_KR_HAS160(void *args); // 2018.06.15
void CheckSum_Tiger1995(void *args);

void CheckSum_RIPEMD160(void *args); // 2018.06.22


void ThreadLogSaveFunc(void *args);
void DoBeforeExit(HWND hwnd); // 2017.05.11

void MenuCheckSum(HWND hwnd, int checksum_type);
void MenuSaveLog(HWND hwnd); // 2016.02.23

extern void BeepSound(int beeptype);
extern void LOGFile_close(void);
extern BOOL LOGFile_open(void);
extern int ProcessorType(void);
extern unsigned __int64 LinuxDate2Number( mjd_timestamp LTime );


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

void InitFindMark(COLORREF tColor);
void IsMarking(void);

#define MARK_RED 			TEXT("***")
#define MARK_NULL 			TEXT("   ")
#endif


#if RICHED_COLOR// 2016.09.22
void RichEditInit(void);
void RichTxtColor(DWORD xx, DWORD yy, DWORD line, COLORREF tColor);

static HMODULE hDll = NULL; /// added.

CHARFORMAT cf;
#endif

#if (TEXT_FIND_MARK) // 2016.09.23
extern int isTxtMark;
extern long long HashRed, HashMagenta;
#endif



#if LOG_SAVE_BY_THREAD // 2016.02.25
#define FILE_WRITE_QUEUE_SIZE 		2000
TCHAR buf2write[FILE_WRITE_QUEUE_SIZE][STRING_LEN+4096] = {0,};
int qindex = 0; // initial
#endif


unsigned __int64 ulCRCFileSize = 0UL; // 2017.08.28
unsigned __int64 ulCRCFileRead = 0UL; // 2017.08.28 __int64 -> unsigned __int64
TCHAR gszCRCFileName[FILENAMELEN] = {0,};
TCHAR szCRCTitleName[FILENAMELEN] = {0,};  
TCHAR gszPrtFileName[FILENAMELEN] = {0,};
int iidx=0, jidx=0, isFind=0;

int g_ChecksumCalcing = CHECKSUM_UNKNOWN;


const char* szCRCtype[CHECKSUM_MAX+1] = {
	NULL,
	"MD5",
	"SHA1",
	"CRC32",
	"CRC64",
	"SHA2-256",
	"SHA2-384",
	"SHA2-512",
	"SHA2-224",
	"MD6",

	"SHA3-224 (Keccak)", // 2017.08.22
	"SHA3-256 (Keccak)", // 2017.08.22
	"SHA3-384 (Keccak)", // 2017.08.22
	"SHA3-512 (Keccak)", // 2017.08.22

	"Shake128 (256bit)",  // SHAKE128_DIGEST_DEFAULT_SIZE CHECKSUM_SHAKE_128
	"Shake256 (512bit)",  // SHAKE256_DIGEST_DEFAULT_SIZE CHECKSUM_SHAKE_256

	"BLAKE224", // 2018.06.15
	"BLAKE256",
	"BLAKE384",
	"BLAKE512",

	"KR HAS-160", // Korea HAS160 CHECKSUM_KR_HAS160
	"Tiger-1995", // CHECKSUM_TIGER1995
	"RMD-160", // CHECKSUM_RIPEMD160
	
	"unknown",
};

SYSTEMTIME mCRCSysTime; // checksum system time



#pragma warning (disable : 4100)
#pragma warning (disable : 4068)

TCHAR szAppName[] = APPNAME;

HINSTANCE _hInstance;
HWND _hwnd, _MainHwnd, _EditHwnd;
static HWND _hwndEdit;

//static HWND _hwndBott;

#if 1 // 2017.4.25
extern volatile HWND _hDlgTxData2UART;
extern HWND _hTxData2UART;

extern volatile HWND _hDlgHex2Binary;
extern HWND _hHex2Binary;

extern volatile HWND _hDlgDownloadProgress;
extern HWND _hProgressControl;

extern volatile HWND _hDlgDownloadProgressUART;
extern HWND _hProgressControlUART;

extern volatile HWND _hDlgSerialSettings;
#endif

#if USE_FLOAT2HEX_VALUE // 2018.07.18
extern volatile HWND _hDlgFlt2Hexa;
#endif

extern int LogFileType; // 2020.04.07

#if DNW_2ND_WINDOWS_ENABLE // 2016.10.24
static HWND hWndStatusBar;
static HWND hWndCommandBar;

//const int iStatusWidths[NUM_STATUS_BAR] = {80, 170, 250, 290, 350, -1};
const int iStatusWidths[NUM_STATUS_BAR] = {70, 160, 240, 280, 340, -1};

const DWORD WinStatus_YInit = 24;
const DWORD	WinStatus_YSize = 20;
int isStatusBarEnable = BAR_NONE;
int OKstatusBarInitial = 0;

extern int userComPort, userBaudRate;
extern volatile int isConnected; // 0:Disconnect, 1:Connected, 2:Try to connect


void MenuStatusClear(HWND hwnd);


#define ARR_KEY_UP 		0x01
#define ARR_KEY_DN 		0x02
#define ARR_KEY_LT 		0x04
#define ARR_KEY_RT 		0x08
#define ARR_KEY_PGUP 	0x10
#define ARR_KEY_PGDN 	0x20


#define MAX_AT_COMMAND_NUM 		100 // 100 // dnw_cmd_history.txt



TCHAR szTempKeyIn[MAX_PATH] = {0,}; // key-in

#if KEYIN_HISTORY_SAVE // 2017.4.25
TCHAR szKeyAll[MAX_AT_COMMAND_NUM][MAX_PATH] = {0,}; // key-in
TCHAR szTmpTxt[MAX_PATH] = {0,}; // display in Status bar
int idxCurrKey = 0;
int idxKeyTot = 0; // 2017.08.28, MAX_AT_COMMAND_NUM;

unsigned char arrowKeyTouched = 0;
int idxRepNum = 0;

int 	TxDataOnStatusBar(void); // 2017.05.10
void 	CmdDisplayOnStatusBar(void); // 2017.05.15
int 	CheckTxDataFileOnStatusBar(void); // 2017.08.28
#endif



int key_index = 0; // unsigned int -> int
unsigned int isPressKey = 0;

#else
const DWORD WinStatus_YInit = 0;
const DWORD	WinStatus_YSize = 0;
#endif



char dnwCurrPath[MAX_PATH] = {0,};
char dnwDefaultPath[MAX_PATH] = {0,};



#if DISPLAY_PC_SYSTEM_TIME /*--- 2016.02.12 ------ */
SYSTEMTIME LocalSysTime; // NOTE PC 의 시간 정보 취득 Display에 출력한다 ---
#endif

const char WeekTXT[][3+1] = {
		"Sun", // wDayOfWeek 0
		"Mon", // wDayOfWeek 1
		"Tue", // wDayOfWeek 2
		"Wed", // wDayOfWeek 3
		"Thu", // wDayOfWeek 4
		"Fri", // wDayOfWeek 5
		"Sat", // wDayOfWeek 6
		"***", // NEVER
		NULL
	};


extern int FontType;
#if defined(COLOR_DISP2)
HBRUSH m_Brush = NULL; /// background color
COLORREF BGColor=RGB(255,255,255);

extern int ColorType; /// 2010.05.14 added
#endif ////////


#define UMON_IRAM_BL1_FILE_INDEX 	0
#define UMON_EBOOT_FILE_INDEX 		1


#if HEXA_MODE_SUPPORT	/* 2012.07.03 Hexa mode display */
extern DWORD ColumnNumber;
extern TCHAR szColumnNumber[16];
int uCount = 0; // 2016.04.02 for LineFeed Count

DWORD HexaEndChar = 0x0D0A; // Example 0x81, or 0x83, ...
extern TCHAR szHexaEnd[16];
#endif

extern int TextorHexaMode; /* Text display or Hexa display */
extern int userFlowControl; // 2017.08.03, Flow COntrol
extern int userFlowCtrlVal; // 2017.08.04, Flow Control Value
extern int userHexaDispVal; // 2018.03.22, Hexa display



extern UINT str2int(TCHAR *str);


extern int sendTxDataType; // 2016.03.23
extern int txDataMsgOnOff; // 2016.03.23
extern TCHAR szSerialTxCharData[TX_DATA_LENGTH];
extern TCHAR szSerialTxHexaData[TX_DATA_LENGTH];
extern TCHAR szSerialTxFileData[TX_DATA_LENGTH]; // 2016.03.28
extern TCHAR szSerialTxFileTitl[TX_DATA_LENGTH]; // 2016.03.28


extern unsigned int WriteTXComm(void *args);
extern BOOL ThreadWriteTXComm(void *args);
extern int isTxData2UART; // Tx Data on/Off for UpdateWindowTitle()

#if USE_WIN_OUTOF_AREA_TO_ZERO // 2016.03.31 LCD size
extern DWORD myLCDwidth, myLCDheight; 

extern DWORD myVirWidth, myVirHeight; // 2017.08.01 VirtualLCD size
#endif


// const DWORD 	WIx_START = 0;
// const DWORD 	WIy_START = 0;

extern DWORD	WIN_XSIZE;
extern DWORD	WIN_YSIZE;

#if DNW_WIN_POSITION_MOVALBE // 2016.02.29
extern BOOL    	isSaveSize;
extern BOOL 	isSavePosition;
extern DWORD	POSave_XSIZE;
extern DWORD	POSave_YSIZE;
#endif


extern DWORD	editXBgn, editWidth;
extern DWORD	editYBgn, editHeight;

extern DWORD	MainXBgn, MainWidth;
extern DWORD	MainYBgn, MainHeight;

extern DWORD 	WIN_XBgn, WIN_YBgn;

extern int autoSendKey;
extern int msgSaveOnOff;

#if DISPLAY_PC_SYSTEM_TIME /*--- 2016.02.12 ------ */
extern int localTime;
extern int isTimeInfoOnce; // Time 정보를 맨 앞에 한번 표시한다. 
#endif

extern int cmdCRLF; // 2019.12.21



static BOOL 	is_resize = FALSE;
static RECT 	MainRect, rect;

DWORD 	W_dif = X_DIFF_CAL; /* 8 */
DWORD 	H_dif = Y_DIFF_CAL; /* 50 */

BOOL 	is_auto_download_Key = FALSE;
WORD 	DownloadedBin = 0x00;
int 	BinFileIndex = 0; /* start = 0*/


static BOOL is_ram_usbtransmit_bl1 = FALSE;

extern FILE 	*fFileWrite;
extern unsigned char buff_fin[DATE_LEN];

#define DNW_WIN_NAME            TEXT("DNW")


extern void LOGFile_close(void);



#define EDITID     1
#define STATUSID   2
#define TOOLBAR_ID 3
#define HEXAID     4
#define CMDKEY_ID  5



// --------------------------------------------------------------------------------------------------
// ------------- SCREEN BUFFER SIZE -----------------------------------------------------------------
// --------------------------------------------------------------------------------------------------

#if 0 // dnw original
#define MAX_EDIT_BUF_SIZE 		(0x7FFE) // 31K
#define EDIT_BUF_SIZE 			(0x6000) // 24K
#define EDIT_BUF_DEC_SIZE 		(0x2000) /* 8KB - 맨 위 8KB 단위 (약 81 line) 로 삭제한다 -- */
#else

#if USER_SCREEN_BUF_SIZE // 2016.02.14
// 0: Large(100MB), 1:Normal, 2:Small(20MB), 3:Smallest(2MB)

#define MAX_EDIT_BUF_SIZE 			(0x7FFFFFE) /* 128 MB */ /* Max : 0x6FFFFFE */

#define HUGE_EDIT_BUF_SIZE 					(0x6400000)  /* 100MB */
#define HUGE_EDIT_BUF_DEC_SIZE 				(0x1E00000)  /* 30MB - 맨 위 30MB 단위로 삭제하여 scroll 처리한다 -- */

#define LARGE_EDIT_BUF_SIZE 				(0x3C00000)  /* 60MB */
#define LARGE_EDIT_BUF_DEC_SIZE 			(0x1400000)  /* 20MB - 맨 위 20MB 단위로 삭제하여 scroll 처리한다 -- */

#define MIDDLE_EDIT_BUF_SIZE 				(0x1400000)  /* 20 MB */
#define MIDDLE_EDIT_BUF_DEC_SIZE 			(0x0500000)  /* 5MB - 맨 위 5MB 단위로 삭제하여 scroll 처리한다 -- */

#define SMALL_EDIT_BUF_SIZE 				(0x0500000)  /* 5 MB */
#define SMALL_EDIT_BUF_DEC_SIZE 			(0x0100000)  /* 1MB - 맨 위 1MB 단위로 삭제하여 scroll 처리한다 -- */

#define SMALLEST_EDIT_BUF_SIZE 				(0x0200000)  /* 2 MB */
#define SMALLEST_EDIT_BUF_DEC_SIZE 			(0x0080000)  /* 512KB - 맨 위 512KB 단위로 삭제하여 scroll 처리한다 -- */

#else

#define MAX_EDIT_BUF_SIZE 			(0x7FFFFFE) /* 128MB */ /* Max : 0x7FFFFFFE */
//#define EDIT_BUF_SIZE 				(0x7F00000) /* 127MB */
//#define EDIT_BUF_DEC_SIZE 			(0x3F80000)  /* 64MB - 맨 위 64MB 단위로 삭제하여 scroll 처리한다 -- */
//#define EDIT_BUF_DEC_SIZE 			(0x3200000)  /* 50MB - 맨 위 50MB 단위로 삭제하여 scroll 처리한다 -- */
#define EDIT_BUF_SIZE 				(0x6400000) /* 100MB */
#define EDIT_BUF_DEC_SIZE 			(0x1E00000)  /* 30MB - 맨 위 30MB 단위로 삭제하여 scroll 처리한다 -- */

#endif // USER_SCREEN_BUF_SIZE
#endif

#define MIN_EDIT_BUF_SIZE 			(0x10)



#if USER_SCREEN_BUF_SIZE // 2016.02.14
DWORD dwBufferCheckSize = HUGE_EDIT_BUF_SIZE; // Huge (100MB)
DWORD dwBufferEraseSize = HUGE_EDIT_BUF_DEC_SIZE; // Huge (30MB)

extern int userScrBufSizIndex;
#endif


// --------------------------------------------------------------------------------------------------




//1) Why program doesn't work when EDIT_BUF_SIZE=50000? 
//   If the data size of the edit box is over about 30000, 
//   EM_REPLACESEL message needs very long time.
//   So,EDIT_BUF_SIZE is limited to 30000. 
//   If the size is bigger than 30000, the size will be decreased by 5000. 
//2) WM_CLEAR to the edit box doesn't work. only, EM_REPLACESEL message works.

// ==============================================
// INITIALIZATION
// ==============================================

//////////////////////////////////////
// The WinMain function is the program entry point.
//////////////////////////////////////
#pragma argsused
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance,
                   LPSTR  lpszCmdParam, int nCmdShow)
{
	MSG  Msg;
	HACCEL hAccel;


	SetUnhandledExceptionFilter(UnhandledExceptionHandler);  // 2016.10.15



	_hInstance = hInst;

	//OutputDebugString("");




// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if 0
	int result = 0;

	GetRegistry(); // 2017.4.13

	//Create the download progress dialogbox.
	result = DialogBox(_hInstance, MAKEINTRESOURCE(IDD_DIALOG2), NULL , OptionsProc); //modal
	if( (1==result) ) // && (1!=isConnected) ) 
	{
		/// MenuConnect(hwnd); //reconfig the serial port.
		//EB_Printf(TEXT("[dnw] MenuOptions is connected! \r\n") );  // refer -> EndDialog(hDlg,1)
	}
	else if( 0 == result )
	{
		// Cancel or ESC 
		//EB_Printf(TEXT("[dnw] MenuOptions is error or already connected!! (%d, %d) \r\n"), result, GetLastError() );
	}

	SetRegistry();

	return FALSE; ///
#endif
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#if 0
	int result = 0;


	//Create the download progress dialogbox.
	result = DialogBox(_hInstance, MAKEINTRESOURCE(IDD_DIALOG3), NULL, cbHex2BinProc);
	if( 1 == result )
	{
		// OK --
	}
	else if( 0 == result )
	{
		// Cancel or ESC 
		// EB_Printf(TEXT("[dnw] MenuHex2Bin is error or already connected!! (%d, %d) \r\n"), result, GetLastError() );
	}
	//return FALSE;

#endif





	if (!Register(hInst))
		return FALSE;

	if (!Create(hInst, nCmdShow))
		return FALSE;






#if 0 // #include <crtdbg.h> // memory leakage check

//	_CrtSetBreakAlloc(64); 

//	_CrtMemDumpAllObjectsSince(0); 
	
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

#endif





	// ----------------------------------------------
	// HOT key
	// ----------------------------------------------
	hAccel = LoadAccelerators(hInst,MAKEINTRESOURCE(IDR_ACCELERATOR1));

	while (GetMessage(&Msg, NULL, 0, 0))
	{
		//To throw the message to dialog box procedure
		if( _hDlgDownloadProgress==0 || !IsDialogMessage(_hDlgDownloadProgress,&Msg) 
			|| _hDlgDownloadProgressUART==0 || !IsDialogMessage(_hDlgDownloadProgressUART,&Msg) ) 
		{
			/* --------------------------------------------------
		    TranslateMessage(&Msg);

		    //To intercept key-board input instead of edit control.
		    if(Msg.message==WM_CHAR) 
			SendMessage(_hwnd,WM_CHAR,Msg.wParam,Msg.lParam);
		    else	//2000.1.26
			DispatchMessage(&Msg);
		    //EB_Printf("."); //for debug
			-------------------------------------------------- */

			if (!TranslateAccelerator(_hwnd,hAccel,&Msg)) 
			{
				TranslateMessage(&Msg);
				if(WM_CHAR == Msg.message) 
					SendMessage(_hwnd, WM_CHAR, Msg.wParam, Msg.lParam);
				else	//2000.1.26
					DispatchMessage(&Msg);

				// EB_Printf("[0x%x] [%x %x] \r\n", Msg.message, Msg.wParam, Msg.lParam ); //for debug

			}

		}
		
	}
	return Msg.wParam;
}



//////////////////////////////////////
// Register Window
//////////////////////////////////////
BOOL Register(HINSTANCE hInst)
{

	GetRegistry(); // 2017.4.13



	WNDCLASSEX WndClass = { 0 }; // main

#if DNW_2ND_WINDOWS_ENABLE // 2016.10.24
	WNDCLASSEX WndChild = { 0 }; // child
#endif


	WndClass.cbSize	        = sizeof(WNDCLASSEX);
	WndClass.style          = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc    = WndProc;
	WndClass.cbClsExtra     = 0;
	WndClass.cbWndExtra     = 0;
	WndClass.hInstance      = hInst;
	WndClass.hIcon          = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON1)); /// IDI_APPLICATION
	WndClass.hCursor        = LoadCursor(hInst,IDC_ARROW);
	WndClass.hbrBackground  = (HBRUSH)(COLOR_BLACK);
	//WndClass.hbrBackground  = (HBRUSH)(COLOR_BTNFACE+1); /// 2018.12.28

#ifdef __TEST__
	IDC_APPSTARTING // App Start Used to show that something undetermined is going on or the application is not stable 
	IDC_ARROW // Arrow This standard arrow is the most commonly used cursor 
	IDC_CROSS  // The crosshair cursor is used in various circumstances such as drawing 
	IDC_HAND  // The Hand is standard only in Windows 2000. If you are using a previous operating system and need this cursor, you may have to create your own. 
	IDC_HELP  // The combined arrow and question mark cursor is used when providing help on a specific item on a window object 
	IDC_IBEAM  // The I-beam cursor is used on text-based object to show the position of the caret 
	IDC_ICON  // This cursor is not used anymore 
	IDC_NO	// This cursor can be used to indicate an unstable situation 
	IDC_SIZE  // This cursor is not used anymore 
	IDC_SIZEALL  // The four arrow cursor pointing north, south, east, and west is highly used to indicate that an object is selected or that it is ready to be moved 
	IDC_SIZENESW  // The northeast and southwest arrow cursor can be used when resizing an object on both the length and the height 
	IDC_SIZENS	// The  north - south arrow pointing cursor can be used when shrinking or heightening an object 
	IDC_SIZENWSE  // The northwest - southeast arrow pointing cursor can be used when resizing an object on both the length and the height 
	IDC_SIZEWE	// The west - east arrow pointing cursor can be used when narrowing or enlarging an object 
	IDC_UPARROW  // The vertical arrow cursor can be used to indicate the presence of the mouse or the caret 
	IDC_WAIT  // The Hourglass cursor is usually used to indicate that a window or the application is not ready. 
#endif

	
	//WndClass.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	//WndClass.hbrBackground  = (HBRUSH)(COLOR_BLACK);
//2017.4.12,	WndClass.hbrBackground  = (HBRUSH)CreateSolidBrush( COLOR_GRAY7 );
	//WndClass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName   = MAKEINTRESOURCE(IDR_MENU1);
	WndClass.lpszClassName  = szAppName;
	WndClass.hIconSm	    = LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON2));


	
	
#if DNW_2ND_WINDOWS_ENABLE // 2016.10.24

#if DNW_2ND_COMMAND // 2017.4.22
	if( BAR_STATUS==isStatusBarEnable || BAR_COMMAND==isStatusBarEnable ) 
#else
	if( BAR_STATUS==isStatusBarEnable ) 
#endif
	{
		WndChild = WndClass;
		WndChild.lpszClassName = "h2";
		WndChild.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		WndChild.lpfnWndProc   = WndProc;


		//RegisterClassEx(&WndClass);
		//RegisterClassEx(&WndChild);

		if( !RegisterClassEx(&WndClass) ) 
		{
			MessageBox(NULL, TEXT("Failed to create the WinClass!1!"), TEXT("Error1"), MB_OK | MB_ICONERROR);
			return 0;
		}

		if( !RegisterClassEx(&WndChild) ) 
		{
			MessageBox(NULL, TEXT("Failed to create the WndChild!!2!!"), TEXT("Error2"), MB_OK | MB_ICONERROR);
			return 0;
		}

		return 1;

	}
	else
	{
		return (RegisterClassEx(&WndClass) != 0);
	}

#else

	return (RegisterClassEx(&WndClass) != 0);

#endif


}




//////////////////////////////////////
// Create the window and show it.
//////////////////////////////////////
BOOL Create(HINSTANCE hInst, int nCmdShow)
{

//~~ 2017.04.13	GetRegistry();


	#if DNW_WIN_POSITION_MOVALBE // 2016.02.29

	HWND hwnd = CreateWindow(szAppName, szAppName,
							WS_OVERLAPPEDWINDOW /*&(~(WS_SIZEBOX|WS_MAXIMIZEBOX|WS_MINIMIZEBOX))*/,
							WIN_XBgn, /* 0 */ /* CW_USEDEFAULT */
							WIN_YBgn, /* 0 */ /* CW_USEDEFAULT */
							WIN_XSIZE, 
							WIN_YSIZE, /*WINDOW_XSIZE, WINDOW_YSIZE, */
							NULL, NULL, hInst, NULL);

	#else

	HWND hwnd = CreateWindow(szAppName, szAppName,
							WS_OVERLAPPEDWINDOW /*&(~(WS_SIZEBOX|WS_MAXIMIZEBOX|WS_MINIMIZEBOX))*/,
							0, // WIN_XBgn, /* 0 */ /* CW_USEDEFAULT */
							0, // WIN_YBgn, /* 0 */ /* CW_USEDEFAULT */
							WIN_XSIZE, 
							WIN_YSIZE, /*WINDOW_XSIZE, WINDOW_YSIZE, */
							NULL, NULL, hInst, NULL);

	#endif // DNW_WIN_POSITION_MOVALBE! -----------------------------------------
  
	if ( NULL==hwnd )
	{
		return FALSE;
	}

	//_hwnd=hwnd; 





	GetWindowRect(hwnd,&MainRect);
	_MainHwnd = hwnd;

	///////////////////////////////////////////////////////////////////////////////



	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd); /* 윈도우 프로시저로 WM_PAINT 메시지를 보내 작업영역을 강제로 그리도록(즉시 다시그림)  */





#if USE_RX_TX_QUEUE
	
	// intialized Circular Queue for Rx2Tx
	InitCQueue(&Rx2Tx, MAX_QUEUE_SIZE );
	
#endif

	return TRUE;
}


//==============================================
// IMPLEMENTATION
//==============================================
LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	int 		wmId, wmEvent;
	WORD 		BinType = 0;

#if DNW_2ND_WINDOWS_ENABLE // 2016.10.24
	RECT 		rcClient; // 2017.04.21
#endif


#if  0
    //static HINSTANCE hInst;
	//HCURSOR 	hcur1;
	static 		HWND hSB;
	PTSTR 		pstr = TEXT("Owner draw Text");
	BOOL 		fResult=FALSE;
#endif



	///HWND hwndRichedit = (HWND)GetWindowLong(hwnd, 0 ); /// added.

    switch(Message)
    {
    case WM_CREATE:

		if( FALSE == isFileExist( TEXT(DNW_FOLDER_NAME1) ) )
		{
			if( FALSE == CreateDirectory( TEXT(DNW_FOLDER_NAME2), NULL) )
			{
				DWORD dwError = GetLastError();
				if( ERROR_ALREADY_EXISTS == dwError ) /// The specified directory already exists ---
				{
					// EB_Printf(TEXT("[dnw] Folder [%s] already exists. \r\n"), TEXT(DNW_FOLDER_NAME1) );
				}
				else
				{
					EB_Printf(TEXT("[dnw] Can not create the dnw folfer!!! Err(%u) \r\n"), dwError );
				}
			}
			else
			{
				//EB_Printf(TEXT("[dnw] dnwconfig!!! --- OK \r\n"));
			}
		}
		else
		{
			///EB_Printf(TEXT("[dnw] found!!!! === \r\n"));
		}


#if DISPLAY_PC_SYSTEM_TIME /*--- 2016.02.12 ------ */
		memset(&LocalSysTime,0x00,sizeof(SYSTEMTIME));
#endif


		memset( dnwCurrPath, 0x00, sizeof(dnwCurrPath) );
		memset( dnwDefaultPath, 0x00, sizeof(dnwDefaultPath) );
		if( !GetCurrentDirectory(MAX_PATH, dnwCurrPath) )
		{
			EB_Printf(TEXT("[dnw] currDir is [%s]. Err(%u) \r\n"), dnwCurrPath, GetLastError() );
		}

		strcpy(dnwDefaultPath,dnwCurrPath);
		strcat(dnwDefaultPath,"\\");


		//PlaySound(TEXT("d:\\windows\\media\\chimes.wav"),NULL,SND_FILENAME|SND_ASYNC);

		_hwnd=hwnd;



		

		// ~~~ 2016.04.13 ~~~ GetRegistry();  // added.


	#if USE_WIN_OUTOF_AREA_TO_ZERO // 2016.03.31 LCD size
		myLCDwidth  = GetSystemMetrics(SM_CXSCREEN);
		myLCDheight = GetSystemMetrics(SM_CYSCREEN);

		myVirWidth	= GetSystemMetrics(SM_CXVIRTUALSCREEN);
		myVirHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);


		if( myLCDwidth <= 0 )  myLCDwidth  = WIN_LIMIT_XX;
		if( myLCDheight <= 0 ) myLCDheight = WIN_LIMIT_YY;


		//if( (WIN_XBgn > myLCDwidth-WIN_LIMIT_X_MARGIN) || WIN_XBgn < 0 ) 
		if( (WIN_XBgn > myVirWidth-WIN_LIMIT_X_MARGIN) || WIN_XBgn < 0 ) // 2017.08.01, : myLCDwidth -> myVirWidth
		{ 
			WIN_XBgn = 0; 
		}

		if( (WIN_YBgn > myLCDheight-WIN_LIMIT_Y_MARGIN) || WIN_YBgn < 0 ) 
		{ 
			WIN_YBgn = 0;
		}

		// EB_Printf(TEXT("[dnw] ++++++++++  ==> X:Y[%d, %d] \n"), LCDwidth, LCDheight );
	#endif



		SetTimer(hwnd,TIMER_ID_WIN_UPDATE, 1000, NULL); /* dnw title - 1sec 마다 update */


	#if RICHED_COLOR // 2016.09.19 +++++++++

		if( !(hDll = LoadLibrary("RichEd20.dll")) )   
		{
			if( !(hDll = LoadLibrary("riched32.dll")) )	 
			{
				MessageBox(NULL, TEXT("RICHED_COLOR DLL can not loaded!"), TEXT("Error"), MB_OK);
			}
		}


		// Create the edit control child window
		_hwndEdit = CreateWindowEx (0, RICHEDIT_CLASS,
					TEXT("edit"),  /*The name of the progress class*/
					//NULL,   /*Caption Text*/
					WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | /* WS_CAPTION | WS_HSCROLL |*/ 
					WS_BORDER | ES_LEFT | ES_MULTILINE | ES_READONLY /* | TBSTYLE_TOOLTIPS  */
					/* | ES_SUNKEN | ES_NOHIDESEL|*/ /*ES_AUTOVSCROLL*/ /*|ES_AUTOHSCROLL*/ ,  /*Styles*/
					0,  /*X co-ordinates*/
					0,  /*Y co-ordinates*/  /* CW_USEDEFAULT, CW_USEDEFAULT,*/
					WIN_XSIZE,   /*Width*/
					WIN_YSIZE,   /*Height*/  /*SCREEN_X, SCREEN_Y, */
					hwnd,  /*Parent HWND*/
					(HMENU)EDITID,  /*The Progress Bar's ID*/
					((LPCREATESTRUCT)lParam)->hInstance,  /*The HINSTANCE of your program*/ 
					NULL); /*Parameters for main window*/		


		RichEditInit(); // RICHEDIT Initialized


	#else // ~~~~~~~~~~~~~~~~~~~~~~~


		// Create the edit control child window
		_hwndEdit = CreateWindow (
					TEXT("edit"),  /*The name of the progress class*/
					NULL,   /*Caption Text*/
					WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL /* | WS_CAPTION | WS_HSCROLL |*/ 
					| WS_BORDER | ES_LEFT | ES_MULTILINE | ES_READONLY /* | TBSTYLE_TOOLTIPS  */
					| WS_TABSTOP /* | ES_SUNKEN | ES_NOHIDESEL|*/ /*ES_AUTOVSCROLL*/ /*|ES_AUTOHSCROLL*/ ,  /*Styles*/
					0,  /*X co-ordinates*/
					0,  /*Y co-ordinates*/  /* CW_USEDEFAULT, CW_USEDEFAULT,*/
					WIN_XSIZE,   /*Width*/
					WIN_YSIZE - STATUS_BAR_HEIGHT, // WinStatus_YInit,   /*Height*/  /*SCREEN_X, SCREEN_Y, */
					hwnd,  /*Parent HWND*/
					(HMENU)EDITID,  /*The Progress Bar's ID*/
					((LPCREATESTRUCT)lParam)->hInstance,  /*The HINSTANCE of your program*/ 
					NULL); /*Parameters for main window*/


		// _hwndBott = CreateDialog (((LPCREATESTRUCT)lParam)->hInstance, (LPCTSTR)IDD_DIAGLOG_BOTTOM, hwnd, (DLGPROC)(WNDPROC) ChildBottomProc);

	#endif // ----------------------------------------




		SetWindowLong( _hwndEdit, 
						GWL_EXSTYLE|GWL_USERDATA|DWL_USER|GWL_STYLE /*|GWL_WNDPROC|GWL_ID|DWL_DLGPROC|DWL_MSGRESULT*/,
						TRUE); /// added.



		//hcur1 = LoadCursor( NULL, IDC_CROSS );
		//hcur1 = LoadCursorFromFile(TEXT("icon1.ico"));


#if DNW_2ND_WINDOWS_ENABLE // 2016.10.24

		OKstatusBarInitial = 0;  // None Status Bar

		if( BAR_STATUS==isStatusBarEnable ) // status bar
		{
			//InitCommonControls();

			//hWndStatusBar = CreateStatusWindow(WS_CHILD | WS_VISIBLE, "", hWnd,0);

			// Create the status bar.
			hWndStatusBar = CreateWindowEx(
							0,
							STATUSCLASSNAME,
							(PCTSTR) NULL,
							WS_OVERLAPPED | WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, //  | CCS_TOP, // includes a sizing grip
							0,
							0,
							WIN_XSIZE,
							STATUS_BAR_HEIGHT, // WinStatus_YSize
							hwnd,              // handle to parent window
							(HMENU)STATUSID,
							(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE),
							NULL);

			// Get the coordinates of the parent window's client area.
			GetClientRect(hwnd, &rcClient);
			//EB_Printf(TEXT("[dnw] STATUS_BAR ==> edit:[%d, %d, %d, %d] \n"), rcClient.left, rcClient.top, rcClient.right, rcClient.bottom );

			if (!hWndStatusBar)
			{
				MessageBox(NULL, TEXT("Failed To Create The Status Bar"), TEXT("Error"), MB_OK | MB_ICONERROR);
				EB_Printf(TEXT("[dnw] Failed to create the Status Bar.. Error(%u) \n"), GetLastError() );
				return 0;
			}

			// Tell the status bar to create the window parts.
			SendMessage(hWndStatusBar, SB_SETPARTS, (WPARAM)NUM_STATUS_BAR, (LPARAM)iStatusWidths);
			SendMessage(hWndStatusBar, SB_SETMINHEIGHT, STATUS_BAR_HEIGHT,0);


			StatusBarText();

			OKstatusBarInitial = 1;
			//ShowWindow(hWndStatusBar, SW_SHOW);

			//SendMessage(hWndStatusBar,WM_SIZE,0,0); // 2016.10.25

		}
	#if DNW_2ND_COMMAND // 2017.4.22
		else if( BAR_COMMAND==isStatusBarEnable )
		{

			//Create controls of the Childwindows 
			hWndCommandBar = CreateWindowEx(
							WS_EX_CLIENTEDGE, 
							TEXT("edit"), 
							(PCTSTR)NULL, 
							WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL | WS_BORDER | ES_LEFT,
							0,
							WIN_YBgn-STATUS_BAR_HEIGHT,
							WIN_XSIZE,
							STATUS_BAR_HEIGHT, // WinStatus_YSize
							hwnd,  // handle to parent window
							(HMENU)(CMDKEY_ID),
							(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
							NULL);

			if (!hWndCommandBar) 
			{
				MessageBox(NULL, TEXT("Failed To Create The Command Bar"), TEXT("Error"), MB_OK | MB_ICONERROR);
				return 0;
			}

			OKstatusBarInitial = 1;

			ShowWindow(hWndCommandBar, SW_SHOW);

		}
	#endif // ------------------
#endif



#if defined(COLOR_DISP2)
		if( 1 == ColorType )
		{
			BGColor = COLOR_BLACK;
			m_Brush = CreateSolidBrush( (COLORREF)COLOR_BLACK );
		}
		else if( 2 == ColorType )
		{
			BGColor = COLOR_GRAY0;
			m_Brush = CreateSolidBrush( (COLORREF)COLOR_GRAY0 );
		}
		else if( 3 == ColorType )
		{
			BGColor = COLOR_GRAY9;
			m_Brush = CreateSolidBrush( (COLORREF)COLOR_GRAY9 );
		}
#endif



		SetFont(_hwndEdit);



#if 0 /// 2013.05.22
		hSB = CreateStatusWindow(WS_CHILD|WS_VISIBLE, TEXT("This is a test"), _hwnd, 0);
		if( NULL == hSB )
		{
			MessageBox(NULL, TEXT("CreateStatusWindow failed!"), TEXT("Error"), MB_OK);
		}

		fResult = SendMessage(hSB, SB_SETTEXT, SBT_OWNERDRAW, (LPARAM)pstr);
		if( FALSE == fResult )
		{
			MessageBox(NULL, TEXT("SB_SETTEXT failed!"), TEXT("Error"), MB_OK);
		}
#endif


		//?Single-line edit controls are limited to a maximum of 32K (32,767 bytes) of text and, of course, to a single line. 
		// By default, single-line edit controls are limited to 30,000 characters. 
		// An application can change this limit to a maximum of 32,767 characters with the EM_LIMITTEXT message described in 
		// "Edit Control Messages," later in this article.

		//?Multiple-line edit controls are limited to a maximum of 64K (65,535 bytes) of text. 
		// Whether a multiple-line edit control is actually able to approach this limit is based on how the edit control uses memory. 
		// Techniques to control the way edit controls use memory are described in the next section, 
		// "Edit Controls and Memory." Multiple-line edit controls are also limited to the following: ?characters per line of text
		//?lines of text
		//?pixels per line of text
		

	#if USER_SCREEN_BUF_SIZE // 2016.02.14
		SendMessage(_hwndEdit, EM_SETLIMITTEXT, MAX_EDIT_BUF_SIZE, 0L);
	#else

		// Now limit the text to the maximum possible amount.
		//
		SendMessage(_hwndEdit, EM_SETLIMITTEXT, MAX_EDIT_BUF_SIZE, 0L);
	#endif


		ReadUserConfig(hwnd); //To get serial settings

		if( FALSE == ReadQuickUserConfig(hwnd) ) /* NAND files */
		{
			//~~ ReWriteQuickUserConfig(); /// re-arrange : EBOOT -> STEPLDR -> NK
			ReadQuickUserConfig(hwnd);
		}

		ReadQuickUMONConfig(hwnd); /* Umon Files */

		MenuConnect(hwnd);

		GetWindowRect(_hwndEdit, &rect);
		_EditHwnd = _hwndEdit;


		//EB_Printf(TEXT("[dnw] currDir[%s]  GetEditBufferSize(0x%X) \n"), dnwCurrPath, GetEditBufferSize(hwnd) );

		// ~~ EB_Printf("WIN_XBgn == %d WIN_YBgn == %d [%5d, %5d] \n", WIN_XBgn, WIN_YBgn, WIN_XSIZE, WIN_YSIZE  );


#if	KEYIN_HISTORY_SAVE // 2017.8.28
		CheckTxDataFileOnStatusBar();
#endif // KEYIN_HISTORY_SAVE // 2017.4.25



		PopFileInitialize(hwnd);

		PopFileSaveInitialize (hwnd); // 2020.04.07
		

		return 0; 





    case WM_COMMAND:
		wmEvent = HIWORD(wParam);
		wmId    = LOWORD(wParam); 

		if( 0==lParam ) //is menu command?
		{
			switch(wmId) ///LOWORD(wParam))
			{
			case CM_ESCAPE_BTN:
				//EB_Printf(TEXT("[dnw] ESCAPE +++ \n"));

				if( _hDlgSerialSettings ) {  SendMessage( _hDlgSerialSettings, WM_COMMAND, IDCANCEL, 0 ); } 
		

		#if 0 // USE_TXDATA_TO_SERIAL // 2016.10.07
				{
				extern volatile HWND _hDlgTxData2UART;
				if( NULL != _hDlgTxData2UART ) SendMessage( _hDlgTxData2UART, WM_COMMAND, IDCANCEL, 0 );
				}
		#endif

				
		#if 0 // USE_HEX2BIN_CONVERT // 2016.03.04
				{
				extern volatile HWND _hDlgHex2Binary;
				if( NULL != _hDlgHex2Binary ) SendMessage( _hDlgHex2Binary, WM_COMMAND, IDCANCEL, 0 );
				}
		#endif
		
				break;



			case IDM_EXIT: /* Shift + ESC key */
			case CM_EXIT:

				DoBeforeExit(hwnd); // 2017.5.12

				PostQuitMessage(0);
				return 0;

				break;

			case CM_CONNECT:
				MenuConnect(hwnd);
				break;

			case CM_DISCONNECT:
				MenuDisconnect(hwnd);
				break;

			case IDM_CONNECT_DISCONNECT: /* function key, F2 toggle  */
				MenuConnectDisconnect(hwnd);
				break;

			case CM_TRANSMIT:
				MenuTransmit(hwnd);
				break;

			case IDM_OPTIONS: /* F12 key */
			case CM_OPTIONS:
				MenuOptions(hwnd);
				break;


		#if USE_HEX2BIN_CONVERT // 2016.03.04
			case IDM_HEX2BIN: /* Shift + F12 key */
			case CM_HEX2BIN:
				MenuHex2Bin(hwnd);
				break;
		#endif



		#if USE_FLOAT2HEX_VALUE // 2018.07.18
			case CM_FLOAT2HEXA:
				MenuFloat2Hexa(hwnd);
				break;
		#endif


		// ------------------------------------------------------------
		#if USE_TXDATA_TO_SERIAL // 2016.03.27

			case IDM_TXDATA2UART: /* Shift + F6 key */
			case CM_TXDATA2UART:
				MenuTxData2UART(hwnd);
				break;

	
			case IDM_SPECIAL_KEY_SEND: // 2016.03.23 [F6]-key Tx Data (char and hexa only)
				// WriteTXComm(NULL);
				ThreadWriteTXComm(TXDATA_BY_F6Key);
				break;


			case CM_EDIT_TXDATA_SCRIPT_FILE:
				if( isFileExist( TEXT("C:\\Windows\\notepad.exe") ) )
				{
					TCHAR  szScriptFile[MAX_PATH] = {0,};
			
					memset(szScriptFile, 0x00, sizeof(szScriptFile) );
					strcpy(szScriptFile, dnwDefaultPath);
					strcat(szScriptFile, DNW_TX_SCRIPT_FUNC_FILE); // 2018.04.02
			
					if( FALSE==isFileExist( szScriptFile ) )
					{
						EB_Printf(TEXT("[dnw] There is no TxData script file. [%s] \r\n"), szScriptFile );
					}
					else
					{
						memset( szScriptFile, 0x00, sizeof(szScriptFile) );
						strcpy(szScriptFile, "C:\\Windows\\notepad.exe  ");
						strcat(szScriptFile, dnwDefaultPath);
						strcat(szScriptFile, DNW_TX_SCRIPT_FUNC_FILE); // 2018.03.30

						ExecProgram( NULL , szScriptFile );

					}
				}
				else
				{
					EB_Printf(TEXT("[dnw] There is no notepad.exe... \r\n"));
				}
				break;

		#endif
		// ------------------------------------------------------------
		

		#if (RICHED_COLOR) || (TEXT_FIND_MARK) // 2016.09.23
			case CM_LOG_TEXTCOLOR:
				MenuLogTextColor(hwnd);
				break;
		#endif

		
		#if HEXA_MODE_SUPPORT	/* 2011.02.08 Hexa mode display */
			case IDM_TEXT_HEXA_MODE: /* Shift+[F5] ; text mode or Hexa mode */
				TextorHexaMode ++;
				TextorHexaMode = TextorHexaMode % 3;  // to Text Mode (0,1,2)

				if(1==TextorHexaMode) // Hexa Line mode
				{
					ColumnNumber = str2int(szColumnNumber);
					uCount = 0; // 2016.04.02
				}


			#if DNW_2ND_WINDOWS_ENABLE // 2016.11.18
				if( OKstatusBarInitial )
				{
					if( BAR_STATUS==isStatusBarEnable ) 
					{

						if(0==TextorHexaMode)
							SendMessage(hWndStatusBar, SB_SETTEXT, STATUS_IDX3,(LPARAM)STA_TX_TEXT);
						else
							SendMessage(hWndStatusBar, SB_SETTEXT, STATUS_IDX3,(LPARAM)STA_TX_HEXA);

						ShowWindow(hWndStatusBar, SW_SHOW);
					}

				#if DNW_2ND_COMMAND // 2017.4.22
				#endif

				}
			#endif // DNW_2ND_WINDOWS_ENABLE
			
				break;

		#endif


		#if 0 // DNW_2ND_WINDOWS_ENABLE

			case IDM_STATUS_BAR_ENABLE:
				isStatusBarEnable++;
				isStatusBarEnable = isStatusBarEnable % 2;

				if(	isStatusBarEnable ) 
				{
					WinStatus_YInit = 24;
					WinStatus_YSize = 20;
					ShowWindow(hWndStatusBar, SW_SHOW);
				}
				else
				{
					WinStatus_YInit = 0;
					WinStatus_YSize = 0;
					ShowWindow(hWndStatusBar, SW_HIDE);
				}
				
				SendMessage(hWndStatusBar,WM_SIZE,0,0); // 2016.10.27
				SendMessage(_hwndEdit,WM_SIZE,0,0); // 2016.10.27


				SetRegistry(); /* Saved Last Windows size */

				break;

		#endif

		
			case IDM_CLEAR_BUFFER: /* function key, F11 */
			case CM_CLEAR_BUF:
				ScrClear(hwnd);
				break;


			case IDM_USBTRANSMIT: /* Shift + F10 */
			case CM_USBTRANSMIT:
				MenuUsbTransmit(hwnd, UBOOT_SHIFT_F10 );
				break;


			case IDM_UMON_TRANSMIT_TO_RAM: /* F4 */
			case CM_RAM_USBTRANSMIT_BL1:
				if( autoSendKey ) // TRUE
				{
					ScrClear(hwnd);

					KillTimer(hwnd, TIMER_ID_AFTER_BL1);
					KillTimer(hwnd, TIMER_ID_AUTO_DOWNLOAD_KEY_SEND);

					is_ram_usbtransmit_bl1 = FALSE;
					if( TRUE == RAM_MenuUsbTransmit(hwnd, UMON_IRAM_BL1_FILE_INDEX, UBOOT_SHIFT_ALL) ) /* 4th file in dnw_usbboot.ini ... bl1 */
					{
						EB_Printf(TEXT("[dnw] L2 >> iRAM USB Booter, bl1 TX OK.  2sec Waiting... \n"));
						is_ram_usbtransmit_bl1 = TRUE;
						SetTimer(hwnd,TIMER_ID_AFTER_BL1,  2000,NULL);  /// <- 3000
						/* Do Timer, TIMER_ID_AFTER_BL1  -> Write EBOOT to RAM 
						   That is, call function,  RAM_MenuUsbTransmit(hwnd, UMON_EBOOT_FILE_INDEX);
						*/
					}
				}
				else
				{
					KillTimer(hwnd, TIMER_ID_AFTER_BL1);
					KillTimer(hwnd, TIMER_ID_AUTO_DOWNLOAD_KEY_SEND);
					EB_Printf(TEXT("[dnw] Check [auto download] of Option menu[F12]. (%s) \r\n"), APP_VER);
				}
				break;


			case CM_ROM_ALL:
				if( BinFileIndex >= 3 ) 
				{
					BinFileIndex = 0;
					DownloadedBin = 0x00;
					break;
				}

				BinType = ROM_MenuUBOOT2(hwnd, BinFileIndex, UBOOT_SHIFT_ALL );
				DownloadedBin |= BinType;
				if( BinType )  /* 1st file in dnw_usbboot.ini ... bl1 */
				{
					BinFileIndex ++;
				}
				break;
				
			case IDM_NK_TO_NAND: /* Shift + F7 => in dnw_usbboot.ini */
			case CM_ROM_UBOOT2_1ST: 

		#if 0
				BinFileIndex = 0;  /* F7 initila */
				DownloadedBin = 0x00;

				if( DNW_EBOOT == (DownloadedBin & DNW_EBOOT) )
				{
					EB_Printf(TEXT("[dnw] EBOOT already downloaded..... one more ++++++ \n"));
				}
		#endif

				BinType = ROM_MenuUBOOT2(hwnd, 0, UBOOT_SHIFT_F7);
				if( BinType )
				{
					DownloadedBin |= DNW_EBOOT;
				}
				break;

			case IDM_EBOOT_TO_NAND: /* Shift + F8 => in dnw_usbboot.ini */
			case CM_ROM_UBOOT2_2ND: 
		#if 0
				if( DNW_NK == (DownloadedBin & DNW_NK) )
				{
					EB_Printf(TEXT("[dnw] NK.bin already downloaded..... one more +++++ \n"));
				}
		#endif


				BinType = ROM_MenuUBOOT2(hwnd, 1, UBOOT_SHIFT_F8);
				if( BinType )
				{
					DownloadedBin |= DNW_NK;
				}
				break;

			case IDM_STEPLDR_TO_NAND: /* Shift+F9 => in dnw_usbboot.ini */
			case CM_ROM_UBOOT2_3RD: 
		#if 0
				if( DNW_STEP == (DownloadedBin & DNW_STEP) )
				{
					EB_Printf(TEXT("[dnw] StepLdr.nb0 already downloaded..... one more +++++  \n"));
				}
		#endif


				BinType = ROM_MenuUBOOT2(hwnd, 2, UBOOT_SHIFT_F9);
				if( BinType )
				{
					DownloadedBin |= DNW_STEP;
				}
		#if 0
				if( TOTAL_BIN_OK == DownloadedBin )
				{
					DownloadedBin = 0x00; /* initial */
				}
		#endif
				break;

			
			case CM_UBOOT:
				MenuUBOOT(hwnd);
				break;

			case IDM_UBOOT2: /* Shift + F11 */
			case CM_UBOOT2:
				MenuUBOOT2(hwnd, UBOOT_SHIFT_F11);
				break;

			case CM_USBRECEIVE:
				MenuUsbReceive(hwnd);
				break;      

			case CM_USBSTATUS:
				MenuUsbStatus(hwnd);
				break;

	#if DNW_2ND_WINDOWS_ENABLE // 2016.11.18
			case IDM_STATUS_BAR_KEY_CLEAR: /* F9 */
				if( OKstatusBarInitial && (BAR_STATUS==isStatusBarEnable) /* && isFileExist( TEXT(USER_TXDATA_FILE) ) */ ) 
				{
					EB_Printf(TEXT("[dnw] The history command on StatusBar is cleared. \r\n") );
					MenuStatusClear(hwnd);

					isPressKey = 1; // press
				}

			#if 0 // 2018.12.28
				//EB_Printf(TEXT("[dnw] WM_LBUTTONDOWN.. 0x%X\n"), wParam );
				if (SendMessage(hWndStatusBar, SB_ISSIMPLE,0,0)) {
					SendMessage(hWndStatusBar,SB_SIMPLE,(WPARAM)FALSE,0);
				} 
				else {
					SendMessage(hWndStatusBar,SB_SIMPLE,(WPARAM)TRUE,0);
				}
				return 0;		
			#endif
			

			#if DNW_2ND_COMMAND // 2017.4.22
			#endif

				break;


		#if KEYIN_HISTORY_SAVE // 2017.4.25

			case IDM_KEY_PGDN_HIST_OUT: /* PAGE_DN -> VK_NEXT */
				if( OKstatusBarInitial && idxKeyTot )
				{
					if( BAR_STATUS==isStatusBarEnable ) 
					{
						arrowKeyTouched = ARR_KEY_PGDN;

						isPressKey = 1; // No display for initial guide 

						idxCurrKey -= 5;
						if( idxCurrKey < 0 ) { idxCurrKey = idxKeyTot-1; } // MAX_AT_COMMAND_NUM-1; DOWN -> decrease

						//EB_Printf(TEXT("[dnw] [VK_PGDN] %02x -> %2d:[%s] \r\n"), arrowKeyTouched, idxCurrKey, szKeyAll[idxCurrKey] );

						CmdDisplayOnStatusBar();
					}
				}
				break;

	
			case IDM_KEY_PGUP_HIST_OUT: /* PAGE_UP -> VK_PRIOR */
				if( OKstatusBarInitial && idxKeyTot )
				{
					if( BAR_STATUS==isStatusBarEnable ) 
					{
						arrowKeyTouched = ARR_KEY_PGUP;

						isPressKey = 1; // No display for initial guide 

						if( idxKeyTot>0 )
						{
							idxCurrKey += 5;
							idxCurrKey %= idxKeyTot; // MAX_AT_COMMAND_NUM; UP -> increase
						}
						//EB_Printf(TEXT("[dnw] [VK_PGUP] %02x -> %2d:[%s] \r\n"), arrowKeyTouched, idxCurrKey, szKeyAll[idxCurrKey] );

						CmdDisplayOnStatusBar();
					}
				}
				break;
				
			case IDM_KEY_UP_HISTORY_OUT: /* VK_UP / VK_DOWN  */

				if( OKstatusBarInitial && idxKeyTot )
				{
					if( BAR_STATUS==isStatusBarEnable ) 
					{
						arrowKeyTouched = ARR_KEY_UP;

						isPressKey = 1; // No display for initial guide 

						if( idxKeyTot>0 )
						{
							idxCurrKey ++;
							idxCurrKey %= idxKeyTot; // MAX_AT_COMMAND_NUM; UP -> increase
						}
						// EB_Printf(TEXT("[dnw] [VK_UP] %02x -> %2d:[%s] \r\n"), arrowKeyTouched, idxCurrKey, szKeyAll[idxCurrKey] );

						CmdDisplayOnStatusBar();

					}
				}
				break;
			
			case IDM_KEY_DN_HISTORY_OUT:

				if( OKstatusBarInitial && idxKeyTot )
				{
					if( BAR_STATUS==isStatusBarEnable ) 
					{
						arrowKeyTouched = ARR_KEY_DN;

						isPressKey = 1; // No display for initial guide 

						idxCurrKey --;
						if( idxCurrKey < 0 ) { idxCurrKey = idxKeyTot-1; } // MAX_AT_COMMAND_NUM-1; DOWN -> decrease

						// EB_Printf(TEXT("[dnw] [VK_DN] %02x -> %2d:[%s] \r\n"), arrowKeyTouched, idxCurrKey, szKeyAll[idxCurrKey] );

						CmdDisplayOnStatusBar();

					}
				}
				break;

			case IDM_KEY_LT_HISTORY_OUT:

				if( OKstatusBarInitial && idxKeyTot )
				{
					if( BAR_STATUS==isStatusBarEnable ) 
					{
						arrowKeyTouched = ARR_KEY_LT;
						
						isPressKey = 1; // No display for initial guide 

						// EB_Printf(TEXT("[dnw] [VK_LEFT] %02x -> %2d:[%s] \r\n"), arrowKeyTouched, idxCurrKey, szKeyAll[idxCurrKey] );
						idxCurrKey = 0; // initial

						CmdDisplayOnStatusBar();

					}
				}
				break;

			case IDM_KEY_RT_HISTORY_OUT:

				if( OKstatusBarInitial && idxKeyTot )
				{
					if( BAR_STATUS==isStatusBarEnable ) 
					{
						arrowKeyTouched = ARR_KEY_RT;
						
						isPressKey = 1; // No display for initial guide 

						// EB_Printf(TEXT("[dnw] [VK_RIGHT] %02x -> %2d:[%s] \r\n"), arrowKeyTouched, idxCurrKey, szKeyAll[idxCurrKey] );
						
						CmdDisplayOnStatusBar(); // 2017.05.12
				
					}
				}
				break;


		#endif // KEYIN_HISTORY_SAVE ---------------------

	#endif // DNW_2ND_WINDOWS_ENABLE ---------------------


		#if 0 // DNW_2ND_WINDOWS_ENABLE // 2017.04.22
		
			case IDM_STATUS_DISABLE: // [F1]
				if ( SendMessage(hWndStatusBar, SB_ISSIMPLE,0,0) ) 
				{
					SendMessage(hWndStatusBar,SB_SIMPLE,(WPARAM)FALSE,0);
					// ~~ShowWindow(hWndCommandBar, SW_HIDE);
				} 
				else 
				{
					TCHAR szBuf[MAX_PATH] = {0,};

					// disabled--
					SendMessage(hWndStatusBar,SB_SIMPLE,(WPARAM)TRUE,0);


				#if 0
					// command windows
					hWndCommandBar = CreateWindow(TEXT("edit"),NULL, 
									WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER | ES_AUTOHSCROLL,
									0,	/*X co-ordinates*/
									WIN_YSIZE - STATUS_BAR_HEIGHT,	/*Y co-ordinates*/	/* CW_USEDEFAULT, CW_USEDEFAULT,*/
									WIN_XSIZE,	 /*Width*/
									STATUS_BAR_HEIGHT, // WinStatus_YInit,	 /*Height*/  /*SCREEN_X, SCREEN_Y, */
									hwnd,
									(HMENU)(CMDKEY_ID),
									(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
									NULL);



					

					GetWindowText(hWndCommandBar,szBuf,MAX_PATH);
					SetWindowText(hwnd,szBuf);

					//ShowWindow(hWndCommandBar, SW_SHOW);
				#endif

				}
				break;


		#endif // DNW_2ND_WINDOWS_ENABLE



			case IDM_ABOUT: /* F10 */
			case CM_ABOUT:
				MenuAbout(hwnd);
				break;

			case IDM_COMPORT_LIST: /* F3 */
				GetComPortRegistry();
				break;
				
			case IDM_EDIT_COPY: /* CTRL+C */
				// wParam : This parameter is not used and must be zero. 
				// lParam : This parameter is not used and must be zero. 
				SendMessage(_hwndEdit, WM_COPY, 0, 0);
#if REMOVED_AFTER_PRESS_CTRL_C
				Sleep(100); // 2016.02.15
				SendMessage(_hwndEdit, EM_SETSEL, -1, 0);  // all text is deselected.
#endif
				break;

			case IDM_EDIT_ALL: /* CTRL + A */
				// If the start is 0 and the end is -1, all the text in the edit control is selected. 
				// If the start is -1, any current selection is deselected. 
				SendMessage(_hwndEdit, EM_SETSEL, 0, -1); // all the text is selected. 
#if REMOVED_AFTER_PRESS_CTRL_C
				Sleep(100); // 2015.12.14
#endif
				break;

			case IDM_EDIT_COPYALL: /* CTRL+SPACE */
				SendMessage(_hwndEdit, EM_SETSEL, 0, -1); // all the text is selected. 
				SendMessage(_hwndEdit, WM_COPY, 0, 0);
#if REMOVED_AFTER_PRESS_CTRL_C
				Sleep(100); // 2016.02.15
				SendMessage(_hwndEdit, EM_SETSEL, -1, 0);  // all text is deselected.
#endif
				break;


			case IDM_EDIT_PASTE: /* CTRL+V */
#if 0	
			{
				char szPasteBuf[1024+1] = {0,};
				
				//SendMessage(_hwndEdit, WM_COPY, 0, 0)
				GetWindowText(_hwndEdit, szPasteBuf, 1024);

				//SendMessage(_hwndEdit, WM_PASTE, 0, 0); // Does not paste anything in textbox.
				SendMessage(_hwndEdit, EM_REPLACESEL, FALSE, (LPARAM)szPasteBuf); 
				EB_Printf(TEXT("[dnw] [%s] \r\n"), szPasteBuf );

			}
#endif

				break;

		// ---------------------------------
		// --- Checksum --------------------
		// ---------------------------------
		#if SHELL_COMMAND // 2016.02.18
			case CM_CHECKSUM_SHELL_CMD:
			case IDM_CHECKSUM_SHELL_CMD:
				{
					TCHAR  runFile1[256] = {0,};
					TCHAR  runFile2[256] = {0,};
			
					strcat(runFile1, dnwDefaultPath);
					strcat(runFile1, "MD5.bat");

					strcat(runFile2, dnwDefaultPath);
					strcat(runFile2, "AttachHdr.exe");

					if( isFileExist(runFile1) && isFileExist(runFile2) )
					{
						ExecProgram (runFile1, NULL); /// TEXT(".\\dnw_checksum.txt") );
					}
					else
					{

					}
				}
				break;
		#endif


			case CM_CHECKSUM_MD5:
			case IDM_CHECKSUM_MD5:
				MenuCheckSum(hwnd, CHECKSUM_MD5);
				break;

			case CM_CHECKSUM_SHA1:
			case IDM_CHECKSUM_SHA1:
				MenuCheckSum(hwnd, CHECKSUM_SHA1);
				break;

			case CM_CHECKSUM_SHA2_256:
			case IDM_CHECKSUM_SHA2_256:
				MenuCheckSum(hwnd, CHECKSUM_SHA2_256);
				break;

			case CM_CHECKSUM_SHA2_384:
			case IDM_CHECKSUM_SHA2_384:
				MenuCheckSum(hwnd, CHECKSUM_SHA2_384);
				break;

			case CM_CHECKSUM_SHA2_512:
			case IDM_CHECKSUM_SHA2_512:
				MenuCheckSum(hwnd, CHECKSUM_SHA2_512);
				break;

			case CM_CHECKSUM_SHA2_224:
			case IDM_CHECKSUM_SHA2_224:
				MenuCheckSum(hwnd, CHECKSUM_SHA2_224);
				break;
				
			case CM_CHECKSUM_MD6:
			case IDM_CHECKSUM_MD6:
				MenuCheckSum(hwnd, CHECKSUM_MD6);
				break;

	#if 1 // 2017.08.22

			case CM_CHECKSUM_SHA3_KECCAK_224: // 2017.08.22
			case IDM_CHECKSUM_SHA3_KECCAK_224: // 2017.08.22
				MenuCheckSum(hwnd, CHECKSUM_SHA3_KECCAK_224); // 2017.08.22
				break;
				
			case CM_CHECKSUM_SHA3_KECCAK_256:
			case IDM_CHECKSUM_SHA3_KECCAK_256:
				MenuCheckSum(hwnd, CHECKSUM_SHA3_KECCAK_256); 
				break;
				
			case CM_CHECKSUM_SHA3_KECCAK_384:
			case IDM_CHECKSUM_SHA3_KECCAK_384:
				MenuCheckSum(hwnd, CHECKSUM_SHA3_KECCAK_384); 
				break;

			case CM_CHECKSUM_SHA3_KECCAK_512:
			case IDM_CHECKSUM_SHA3_KECCAK_512:
				MenuCheckSum(hwnd, CHECKSUM_SHA3_KECCAK_512);
				break;


			case CM_CHECKSUM_SHAKE128:
			case IDM_CHECKSUM_SHAKE128:
				MenuCheckSum(hwnd, CHECKSUM_SHAKE_128); 
				break;

			case CM_CHECKSUM_SHAKE256:
			case IDM_CHECKSUM_SHAKE256:
				MenuCheckSum(hwnd, CHECKSUM_SHAKE_256); 
				break;

			case CM_CHECKSUM_BLAKE_224:
			case IDM_CHECKSUM_BLAKE_224:
				MenuCheckSum(hwnd, CHECKSUM_BLAKE224); 
				break;

			case CM_CHECKSUM_BLAKE_256:
			case IDM_CHECKSUM_BLAKE_256:
				MenuCheckSum(hwnd, CHECKSUM_BLAKE256); 
				break;

			case CM_CHECKSUM_BLAKE_384:
			case IDM_CHECKSUM_BLAKE_384:
				MenuCheckSum(hwnd, CHECKSUM_BLAKE384); 
				break;

			case CM_CHECKSUM_BLAKE_512:
			case IDM_CHECKSUM_BLAKE_512:
				MenuCheckSum(hwnd, CHECKSUM_BLAKE512); 
				break;

			case CM_CHECKSUM_KR_HAS160:
			case IDM_CHECKSUM_KR_HAS160:
				MenuCheckSum(hwnd, CHECKSUM_KR_HAS160); 
				break;


			case CM_CHECKSUM_TIGER1995:
			case IDM_CHECKSUM_TIGER1995:
				MenuCheckSum(hwnd, CHECKSUM_TIGER1995); 
				break;

			case CM_CHECKSUM_RIPEMD160:
			case IDM_CHECKSUM_RIPEMD160:
				MenuCheckSum(hwnd, CHECKSUM_RIPEMD160); 
				break;
				
	#endif

	
	
			case CM_CHECKSUM_CRC32:
			case IDM_CHECKSUM_CRC32:
				MenuCheckSum(hwnd, CHECKSUM_CRC32);
				break;

			case CM_CHECKSUM_CRC64:
			case IDM_CHECKSUM_CRC64:
				MenuCheckSum(hwnd, CHECKSUM_CRC64);
				break;

	#if BASE64_ENCODE_AND_DECODE
			case CM_BASE64_ENCODE:
				break;

			case CM_BASE64_DECODE:
				break;
	#endif


			case CM_CHECKSUM_VIEW_CHECKSUM_FILE:
				if( isFileExist( TEXT("C:\\Windows\\notepad.exe") ) )
				{
					TCHAR  MD5File[MAX_PATH] = {0,};
			
					memset(MD5File, 0x00, sizeof(MD5File) );
					strcpy(MD5File, dnwDefaultPath);
					strcat(MD5File, CRC_VIEW_FILE);
			
					if( FALSE==isFileExist( MD5File ) )
					{
						//EB_Printf(TEXT("[dnw] There is no checksum file. [%s] \r\n"), MD5File );
						EB_Printf(TEXT("[dnw] There is no checksum file.\r\n"));
					}
					else
					{
						memset( MD5File, 0x00, sizeof(MD5File) );
						strcpy(MD5File, "C:\\Windows\\notepad.exe  ");
						strcat(MD5File, dnwDefaultPath);
						strcat(MD5File, CRC_VIEW_FILE);
			
						//EB_Printf(TEXT("[dnw] MD5File [%s] \n"), MD5File );
						ExecProgram( NULL /* TEXT("C:\\Windows\\notepad.exe") */, MD5File );
					}
				}
				else
				{
					EB_Printf(TEXT("[dnw] There is no notepad.exe in the Windows. \r\n"));
				}
				break;

		// ---------------------------------
		// --- Checksum --------------------
		// ---------------------------------
		
			case IDM_LOG_SAVE_ON_OFF:
			case CM_LOG_SAVE_ON_OFF: /* Ctrl + ALT + L */
				MenuSaveLog(hwnd);
				//EB_Printf(TEXT("[dnw] Log save On/Off. (%d) \r\n"), msgSaveOnOff );
				break;


	#if USE_WIN_OUTOF_AREA_TO_ZERO // 2017.08.01
			case CM_DISPLAY_ENABLE_INIT: /* CTRL + ALT + I -> Display position initialized to X,Y(0,0) */
			{
				static int iOnceWr=1;

				WIN_XBgn = 0; // Forced to display position X to 0
				WIN_YBgn = 0; // Forced to display position Y to 0
		
				if( iOnceWr ) 
				{
					//EB_Printf(TEXT("[dnw] Presses CTRL+ALT+I \r\n") );
					SetRegistry(); /* Saved Last Windows size */
				}
				iOnceWr = 0;
			}

				break;				
	#endif

			// 2016.03.23 - TimeStamp Toggle
			case IDC_TIMESTAMP_HOT_TOGGLE:
			{
				localTime++;

		#if MODIFIED_JULIAN_DATE // 2017.08.30
				localTime = localTime % (IDC_LOCALTIME_LINUX_HEX - IDC_LOCALTIME_NONE + 1);
		#else
				localTime = localTime % (IDC_LOCALTIME_DATETIME - IDC_LOCALTIME_NONE + 1);
		#endif

				if( localTime ) isTimeInfoOnce=1; // 2016.04.02
				else isTimeInfoOnce=0;

		#if MODIFIED_JULIAN_DATE // 2017.08.30
				CheckRadioButton(_hDlgSerialSettings,IDC_LOCALTIME_NONE, IDC_LOCALTIME_LINUX_HEX, IDC_LOCALTIME_NONE+localTime);
		#else
				CheckRadioButton(_hDlgSerialSettings,IDC_LOCALTIME_NONE, IDC_LOCALTIME_DATETIME, IDC_LOCALTIME_NONE+localTime);
		#endif
			}
				break;



			default:
				//EB_Printf(TEXT("[dnw] WndProc:: lParam=0x%X  wParam=0x%X\n"), lParam, wParam );
				if (LOWORD(wParam) >= HISTORYMENUID && LOWORD(wParam) < (HISTORYMENUID+MAXHISTORY/3))
				{
					MenuUsbTransmitHistory(hwnd, LOWORD(wParam)-HISTORYMENUID, UBOOT_OTHER );
				}
				if (LOWORD(wParam) >= (HISTORYMENUID+MAXHISTORY/3) && LOWORD(wParam) < (HISTORYMENUID+(MAXHISTORY*2)/3))
				{
					MenuUBOOTHistory(hwnd, LOWORD(wParam)-HISTORYMENUID);
				}
				if (LOWORD(wParam) >= (HISTORYMENUID+(MAXHISTORY*2)/3) && LOWORD(wParam) < (HISTORYMENUID+MAXHISTORY))
				{
					MenuUBOOTHistory2(hwnd, LOWORD(wParam)-HISTORYMENUID);
				}
				break;
			}
			return 0;
		}
		else
		{
			///EB_Printf(TEXT("[dnw] WndProc:WM_COMMAND: lParam=0x%X  wParam=0x%X\n"), lParam, wParam );
			return 0;
		}

		return 0; // 2017.04.22
		break;



    // WM_PAINT should be processed on DefWindowProc(). 
    // If there is WM_PAINT, there should be UpdateWindow(_hwndEdit). 


#if 0 /* tp.joo_2020_0318 */
	//case WM_DEADCHAR:
	case WM_SYSKEYDOWN:
		if (wParam==VK_F10) {
			KeyDownProc(hwnd, wParam, lParam);
		}
		else {
			return (DefWindowProc(hwnd, Message, wParam, lParam));
		}
		break;
	case WM_SYSKEYUP:
		if (wParam==VK_F10) {
			KeyUpProc(hwnd, wParam, lParam);
		}
		else {
			return (DefWindowProc(hwnd, Message, wParam, lParam));
		}
		break;

	case WM_KEYUP:
		KeyUpProc(hwnd, wParam, lParam);
		break;

	case WM_KEYDOWN:
		KeyDownProc(hwnd, wParam, lParam);
		break;

	case WM_PAINT:
		//PaintProc(hwnd);
		UpdateWindow(_hwndEdit);
		return 0;

#endif /* KEYDOWN_PROC */


	case WM_SETFOCUS:
		SetFocus( _hwndEdit );

		return 0; // 2017.04.22
		break;



#if 0
	case WM_SETCURSOR:
		if (LOWORD(lParam) == HTCLIENT)
		{
			SetCursor(hcur1);
			return 0;
		}
		break;
#endif


/* ----------------------------------------------------------------------------------------------------------------

	WM_CLOSE
	
	윈도우가 닫히기 전에 이 메시지가 전달되며 메인 윈도우인 경우는 응용 프로그램이 종료된다는 신호이다.
	이 메시지를 처리하지 않고 DefWindowProc으로 보내면 DestroyWindow 함수를 호출하여 윈도우를 파괴하도록 한다.
	이 메시지가 전달되었을 때는 아직 윈도우가 파괴된 것이 아니므로 윈도우가 파괴되는 것을 중간에 차단할 수 있다.
	미저장 파일이 있거나 프로그램을 종료할 상황이 되지 않을 때 사용자에게 메시지 박스를 통해 종료 사실을 확인시킬
	수 있으며 이 메시지를 가로채서 단순히 return하면 DestroyWindow가 호출되지 않도록 할 수 있다.
	예를 들어 프로그램을 사용자가 닫을 때 "저장할까요?"라는 확인이 필요한 경우 등에 사용할 수 있다.

	WM_NCDESTROY
	비작업 영역이 파괴될 때 보내진다.
	윈도우와 그 차일드들이 먼저 파괴된 후에 비작업 영역이 파괴되므로 이 메시지는 윈도우가 가장 마지막으로 받는 메시지이다.
	WM_DESTROY보다 뒤에 발생되며 이 메시지를 받았을 때는 모든 차일드가 이미 파괴된 후이다.
	반면 WM_DESTROY 메시지는 차일드가 아직 파괴되기 전이다.
	종료 처리가 필요할 경우는 일반적으로 WM_DESTROY 메시지에 코드를 작성하므로 이 메시지는 실용적인 가치가 거의 없는
	셈이며 처리하는 경우가 극히 드물다. 

----------------------------------------------------------------------------------------------------------------
*/


	//case WM_NCDESTROY:
	case WM_CLOSE: /* Message: 16, 윈도우가 닫히기 전에 이 메시지가 전달되며 메인 윈도우인 경우는 응용 프로그램이 종료된다는 신호이다. */
	case WM_DESTROY: /* Message : 2 윈도우가 파괴될 때 이 메시지가 전달된다. 사용자가 Alt+F4 또는 닫기 버튼을 누를 경우 WM_CLOSE 메시지가 전달되며  */
	case WM_QUIT:

#if 0
{
	TCHAR szTxt[MAX_PATH] = {0,};
	wsprintf( szTxt, TEXT("WM_CLOSE, [Message:0x%x]"), Message );
	MessageBox(_hwndEdit, szTxt, TEXT("Message \n"), MB_OK | MB_ICONINFORMATION );			
}
#endif


		DoBeforeExit(hwnd); // 2017.5.12

		PostQuitMessage(0);
		return 0;
		break;


	case WM_TIMER:
		switch(wParam)
		{
			case TIMER_ID_WIN_UPDATE:
				UpdateWindowTitle();
				break;

			/* --------------- UMON RAM Files --------------------- */
			case TIMER_ID_AFTER_BL1:
				KillTimer(hwnd, TIMER_ID_AFTER_BL1);
				if( autoSendKey ) // TRUE ==  ~~ /// if( TRUE == is_ram_usbtransmit_bl1 )
				{
					if( TRUE == RAM_MenuUsbTransmit(hwnd, UMON_EBOOT_FILE_INDEX, UBOOT_TIMER) ) /* 5th file in dnw_usbboot.ini,	*/
					{
						//// KillTimer(hwnd, TIMER_ID_AFTER_BL1);
						is_ram_usbtransmit_bl1 = FALSE;
						is_auto_download_Key = FALSE;

						SetTimer(hwnd, TIMER_ID_AUTO_DOWNLOAD_KEY_SEND, 500, NULL);  /* 400ms FIX : SPACE BAR sending */
						EB_Printf(TEXT("[dnw] L2 >> Auto download ; Eboot TX OK...\n"));

					}
					else
					{
						EB_Printf(TEXT("[dnw] Auto retry... 3sec wait..  or DO power off \n"));
					}
				}
				break;

			case TIMER_ID_AUTO_DOWNLOAD_KEY_SEND:

				KillTimer(hwnd, TIMER_ID_AUTO_DOWNLOAD_KEY_SEND );
				is_auto_download_Key = TRUE;
				WriteCommBlock(0x20); /* SPACE BAR send for downloading eboot menu */
				WriteCommBlock(0x20); /* retry  */
				WriteCommBlock(0x20); /* retry  */

				EB_Printf(TEXT("[dnw] L2 >> Send SPACE...\n")); // 2015.11.8

#if 0 /// def SLC_NAND
{
volatile int delayi = 0;
				for(delayi=0; delayi < 1000; delayi ++) { ; }
				WriteCommBlock('F'); /* SLC NAND Format send */

				for(delayi=0; delayi < 5*1000; delayi ++) { ; }
}
#endif
				break;

			case TIMER_ID_MSG_WRITE_TO_FILE:
				if( msgSaveOnOff )
				{
					if( fFileWrite  /*&& GetIsConnect() */ )
					{
						fclose(fFileWrite);
						fFileWrite = NULL;

						fFileWrite = fopen( (const char*)buff_fin, "a"); // "a+"
						if( NULL == fFileWrite )
						{
							KillTimer(hwnd, TIMER_ID_MSG_WRITE_TO_FILE );
							/// error
							EB_Printf(TEXT("[dnw] Can not write LOG file. Error(%u) type(%d) \r\n"), GetLastError(), LogFileType );
						}
					}
				}
				else
				{
					KillTimer(hwnd, TIMER_ID_MSG_WRITE_TO_FILE );
				}
				break;
				
			default:
				break;
		}

		return 0;



#if 0
	case WM_MOVING:
		{
		int xPos, yPos;
		GetWindowRect(_hwndEdit, &rect);

		xPos = (int)(short) LOWORD(lParam)&0x0ffff;   // horizontal position 
		yPos = (int)(short) HIWORD(lParam)&0x0ffff;   // vertical position 

	    EB_Printf(TEXT("[dnw] WM_MOVE ==> [%d, %d] \n"), xPos, yPos );
	    EB_Printf(TEXT("[dnw] WM_MOVE ==> [%d, %d, %d, %d] \n"), rect.left, rect.top, rect.right, rect.bottom );
		}
	    return 0;
#endif


#if DNW_WIN_POSITION_MOVALBE
	case WM_MOVE:
		{
		//int  xpos, ypos;
		//xpos =  LOWORD(lParam)&0x0ffff; /* x cal */  /// added.
		//ypos =  HIWORD(lParam)&0x0ffff; /* y cal */  /// added.

		GetWindowRect(_MainHwnd, &MainRect);
		//_EditHwnd = _hwndEdit;

		WIN_XBgn = MainRect.left;
		WIN_YBgn = MainRect.top;

		WIN_XSIZE = MainRect.right - MainRect.left;
		WIN_YSIZE = MainRect.bottom - MainRect.top;

	#if DNW_WIN_POSITION_MOVALBE // 2016.02.29
		// 2016.02.29
		POSave_XSIZE = WIN_XSIZE; 
		POSave_YSIZE = WIN_YSIZE; 


		isSavePosition = TRUE;	// Position 변경 -- registry save!!!
	#endif


		//EB_Printf("MOVE==> WIN_XBgn == %d WIN_YBgn == %d [%5d, %5d] \n", WIN_XBgn, WIN_YBgn, WIN_XSIZE, WIN_YSIZE  );
	
	#if 0 // 2017.08.01, USE_WIN_OUTOF_AREA_TO_ZERO // 2016.03.31 LCD size
		//EB_Printf(TEXT("[dnw] +1+ :[%d, %d] (%d, %d)=(%d,%d) \r\n"), WIN_XBgn, WIN_YBgn, myLCDwidth, myLCDheight, MainWidth, MainHeight );

		if( ((WIN_XBgn > myLCDwidth-WIN_LIMIT_X_MARGIN) || WIN_XBgn < 0) || ((WIN_YBgn > myLCDheight-WIN_LIMIT_Y_MARGIN) || WIN_YBgn < 0) ) 
		{ 
			WIN_XBgn = 0; 
			WIN_YBgn = 0;
			//WIN_XSIZE = WINDOW_XSIZE;
			//WIN_YSIZE = WINDOW_YSIZE;
		}
	#endif
		

	#if 0
		//MoveWindow (_MainHwnd, xpos, ypos, (WIN_XSIZE), (WIN_YSIZE), TRUE);

		//MoveWindow (_hwndEdit, xpos, ypos, (WIN_XSIZE-W_dif), (WIN_YSIZE-H_dif), TRUE);


		//EB_Printf(TEXT("[dnw] WM_MOVE:[%d, %d, %d, %d] \n"), MainXBgn, MainYBgn, MainWidth, MainHeight);
		//EB_Printf(TEXT("[dnw] WM_MOVE:[%d, %d, %d, %d] [%d,%d]\n"), editXBgn, editYBgn, editWidth, editHeight, WIN_XBgn, WIN_YBgn);

	    EB_Printf(TEXT("[dnw] WM_MOVE ==> [%d, %d] \n"), WIN_XBgn, WIN_YBgn );
	    EB_Printf(TEXT("[dnw] WM_MOVE ==> main:[%d, %d, %d, %d] \n"), MainRect.left, MainRect.top, MainRect.right, MainRect.bottom );
	    EB_Printf(TEXT("[dnw] WM_MOVE ==> size:[%d, %d] \n"), WIN_XSIZE, WIN_YSIZE );
	    //EB_Printf(TEXT("[dnw] WM_MOVE ==> edit:[%d, %d, %d, %d] \n"), rect.left, rect.top, rect.right, rect.bottom );
	#endif

	    return 0;
		}
		break;
#endif // DNW_WIN_POSITION_MOVALBE





	case WM_SIZE:
		/* main window */
		MainXBgn   = MainRect.left;
		MainYBgn   = MainRect.top;
		MainWidth  = MainRect.right;
		MainHeight = MainRect.bottom;

		/* edit  window */
		editXBgn   = rect.left;
		editYBgn   = rect.top;
		editWidth  = rect.right;
		editHeight = rect.bottom;

		/* main frame 과 edit frame의 차이로 인하여 windows 크기가 다르다... 보상.... */
		W_dif = abs( (int)(editXBgn-MainXBgn) ) + abs( (int)(editWidth-MainWidth) );
		H_dif = abs( (int)(editYBgn-MainYBgn) ) + 4;

	#if 0
		EB_Printf(TEXT("[dnw] WM_SIZE ==> Main:[%d, %d, %d, %d] \n"), MainXBgn, MainYBgn, MainWidth, MainHeight);
		EB_Printf(TEXT("[dnw] WM_SIZE ==> edit:[%d, %d, %d, %d] [%d,%d]\n"), editXBgn, editYBgn, editWidth, editHeight, WIN_XBgn, WIN_YBgn);
		//EB_Printf(TEXT("[dnw] [%d, %d, %d, %d] \n"), 
		//	rect.left-MainRect.left, rect.top-MainRect.top, rect.right-MainRect.right, rect.bottom-MainRect.bottom 	);
		EB_Printf(TEXT("[dnw] WM_SIZE ==> pos:[%d, %d] X,Y[%d, %d]\n"), W_dif, H_dif, LOWORD(lParam), HIWORD(lParam) );
	#endif


	#if DNW_WIN_POSITION_MOVALBE // 2016.02.29
		isSaveSize = TRUE;	// 2016.02.29, size 변경 -- registry save!!!
	#endif
		
		if( FALSE == is_resize ) 
		{
			is_resize = TRUE;

	#if DNW_WIN_POSITION_MOVALBE // 2016.02.29

			WIN_XSIZE = WIN_XSIZE - W_dif;
			WIN_YSIZE = WIN_YSIZE - H_dif;

		#if DNW_2ND_WINDOWS_ENABLE // 2016.10.25
			if( OKstatusBarInitial )
			{
				if( BAR_STATUS==isStatusBarEnable  ) 
				{

					MoveWindow (_hwndEdit, 0, 0, WIN_XSIZE, WIN_YSIZE-WinStatus_YInit, FALSE);

					SendMessage(hWndStatusBar, WM_SIZE, 0, 0);
				}

			#if DNW_2ND_COMMAND // 2017.4.22
			#endif


			}
			else
			{
				MoveWindow (_hwndEdit, 0, 0, WIN_XSIZE, WIN_YSIZE, FALSE);
			}

		#else

			MoveWindow (_hwndEdit, 0, 0, WIN_XSIZE, WIN_YSIZE, FALSE);

		#endif
		
	#else

			MoveWindow (_hwndEdit, 0, 0, (WIN_XSIZE-W_dif), (WIN_YSIZE-H_dif), FALSE);
	#endif

			// ~~ EB_Printf("SIZE0==> WIN_XBgn == %d WIN_YBgn == %d [%5d, %5d] \n", WIN_XBgn, WIN_YBgn, WIN_XSIZE, WIN_YSIZE  );

			return 0;
		}

	#if DNW_2ND_WINDOWS_ENABLE // 2016.10.25
		if( OKstatusBarInitial )
		{
			if( BAR_STATUS==isStatusBarEnable ) 
			{
				MoveWindow (_hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam)-WinStatus_YSize, TRUE);

				SendMessage(hWndStatusBar, WM_SIZE, 0, 0);
			}

		#if DNW_2ND_COMMAND // 2017.4.22
		#endif

		}
		else
		{
			MoveWindow (_hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		}
	#else
		MoveWindow (_hwndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
	#endif
	

	#if DNW_WIN_POSITION_MOVALBE // 2016.02.29

		GetWindowRect(_MainHwnd, &MainRect);

		WIN_XSIZE = MainRect.right - MainRect.left;
		WIN_YSIZE = MainRect.bottom - MainRect.top;
	#else

		WIN_XSIZE = LOWORD(lParam)+W_dif; /* x cal */  // added.
		WIN_YSIZE = HIWORD(lParam)+H_dif; /* y cal */  // added.

	#endif



		// ~~ EB_Printf("SIZE1==> WIN_XBgn == %d WIN_YBgn == %d [%5d, %5d] \n", WIN_XBgn, WIN_YBgn, WIN_XSIZE, WIN_YSIZE  );
		//EB_Printf(TEXT("[dnw] +SIZE+ :[%d, %d] (%d, %d)=(%d,%d) \r\n"), WIN_XBgn, WIN_YBgn, myLCDwidth, myLCDheight, MainWidth, MainHeight );

        return 0;



	//If the size of the parent window is not adjustable,
	//WM_SIZE message is not received.
	//If the size of the parent window is adjustable,
	//WM_SIZE message is received although window size is not changed.
	//MoveWindow(,,,,,TRUE) will make WM_PAINT.
	
	//If DefWindowProc() doesn't process WM_PAINT and WM_PAINT is processed by user, 
	//UpdateWindow(_hwndEdit) is should be executed inide WM_PAINT case,
	//which update the child window.




	case WM_CHAR:

#if KEYIN_HISTORY_SAVE // 2017.4.25

		if( (arrowKeyTouched & ARR_KEY_PGDN) && (idxKeyTot > 0) && (ENTER_KEY==(char)wParam) )
		{
			arrowKeyTouched &= ~ARR_KEY_PGDN;

			TxDataOnStatusBar(); // at [ENTER]-key
		}
		else if( (arrowKeyTouched & ARR_KEY_PGUP) && (idxKeyTot > 0) && (ENTER_KEY==(char)wParam) )
		{
			arrowKeyTouched &= ~ARR_KEY_PGUP;

			TxDataOnStatusBar(); // at [ENTER]-key
		}
		else if( (arrowKeyTouched & ARR_KEY_UP) && (idxKeyTot > 0) && (ENTER_KEY==(char)wParam) )
		{
			arrowKeyTouched &= ~ARR_KEY_UP;

			TxDataOnStatusBar(); // at [ENTER]-key
		}
		else if( (arrowKeyTouched & ARR_KEY_DN) && (idxKeyTot > 0) && (ENTER_KEY==(char)wParam) )
		{
			arrowKeyTouched &= ~ARR_KEY_DN;

			TxDataOnStatusBar(); // at [ENTER]-key

		}
		else if( (arrowKeyTouched & ARR_KEY_LT) && (idxKeyTot > 0) && (ENTER_KEY==(char)wParam) )
		{
			arrowKeyTouched &= ~ARR_KEY_LT;

			TxDataOnStatusBar(); // at [ENTER]-key
		}
		else if( (arrowKeyTouched & ARR_KEY_RT) && (idxKeyTot > 0) && (ENTER_KEY==(char)wParam) )
		{
			arrowKeyTouched &= ~ARR_KEY_RT;

			TxDataOnStatusBar(); // at [ENTER]-key
		}
		else if( OKstatusBarInitial && (BAR_STATUS==isStatusBarEnable) && (DEL_KEY==(char)wParam) ) // 2017.05.15 For delete!
		{
			key_index--;
			if(key_index<=0) key_index=0;
			szTempKeyIn[key_index] = 0x00;

			//EB_Printf("WM_CHAR-1- [%d] [%u] [%c] 0x%x \r\n", key_index, key_index, (char)wParam, (char)wParam );
		}
		else
#endif // KEYIN_HISTORY_SAVE ---------------------------
		{
		
		WriteCommBlock((char)wParam);    //UNICODE problem!!!

	#if KEYIN_HISTORY_SAVE // 2017.4.25
		arrowKeyTouched = 0; // ignore arrow key
	#endif
	
		}



#if DNW_2ND_WINDOWS_ENABLE // 2016.10.24
		if( OKstatusBarInitial )
		{
			if( BAR_STATUS==isStatusBarEnable ) 
			{

				// EB_Printf("WM_CHAR-2- [%c] 0x%x \r\n", (char)wParam, (char)wParam );

				if( DEL_KEY!=(char)wParam ) // 2017.05.15 For delete!
				{
					wsprintf(&szTempKeyIn[key_index], TEXT("%c"), (char)wParam );
					key_index ++;
				}

				if( ENTER_KEY == (char)wParam || key_index>= MAX_PATH ) 
				{ // Enter or Max Length


					if(key_index>= MAX_PATH) szTempKeyIn[key_index] = ENTER_KEY;

			#if KEYIN_HISTORY_SAVE // 2017.4.25
					if( ENTER_KEY==(char)wParam  && (ENTER_KEY!=szTempKeyIn[0]) && szTempKeyIn[0] ) // saving
					{

						szTempKeyIn[key_index] = 0x00;
						
						if( idxKeyTot < MAX_AT_COMMAND_NUM ) 
						{
							memset( szKeyAll[idxKeyTot], 0x00, MAX_PATH );
							strncpy( szKeyAll[idxKeyTot], szTempKeyIn, key_index );
						}
						else if(idxRepNum>=0 && (idxRepNum < MAX_AT_COMMAND_NUM) )
						{
							memset( szKeyAll[idxRepNum], 0x00, MAX_PATH );
							strncpy( szKeyAll[idxRepNum], szTempKeyIn, key_index );
							idxRepNum ++;
							if( idxRepNum >= MAX_AT_COMMAND_NUM ) idxRepNum=0;
						}
						else
							EB_Printf(TEXT("[dnw] History Error (%d, %d) %d[%s] \r\n\r\n"), idxKeyTot, idxRepNum, key_index, szTempKeyIn );


						//EB_Printf(TEXT("[dnw] key_index(%3d) idxKeyTot(%d) idxRepNum(%d) \r\n"), key_index, idxKeyTot,idxRepNum );

						//EB_Printf(TEXT("[dnw] ENTER-1- %d(end:%d) -> (%s) \n"), idxCurrKey, idxKeyTot, szTempKeyIn );
						//EB_Printf(TEXT("[dnw] %s \n\n"), szKeyAll[idxKeyTot] );
						//EB_Printf(TEXT("[dnw] OK \n") );
						
						idxKeyTot++;
						if( idxKeyTot >= MAX_AT_COMMAND_NUM ) idxKeyTot = MAX_AT_COMMAND_NUM;

					}
			#endif // KEYIN_HISTORY_SAVE --------------

			
					memset(szTempKeyIn, 0x00, sizeof(szTempKeyIn) );
					key_index = 0;
					isPressKey = 1;

				}

				SendMessage(hWndStatusBar, SB_SETTEXT, /*SBT_POPOUT|*/STATUS_IDX5,(LPARAM)szTempKeyIn);
				//SendMessage(hWndStatusBar, SB_SETBKCOLOR, 0, (LPARAM)RGB(219,227,250));
				ShowWindow(hWndStatusBar, SW_SHOW);

			}


		#if DNW_2ND_COMMAND // 2017.4.22
		#endif


		}		
#endif // DNW_2ND_WINDOWS_ENABLE

		return 0;



#if 0 // 2016.04.06 comment ~~~
	case WM_ACTIVATEAPP: // 2016.04.04
	{
		int txtRepEnd, lineIdx, txtSelEnd;
		DWORD dwThreadID = (DWORD)lParam; // thread identifier 
		BOOL  bActive = (BOOL) wParam; // activation flag 

		//TCHAR szTxt[MAX_PATH] = {0,};
		//static DWORD dwCurrThreadID;

		//memset( szTxt, 0x20, sizeof(szTxt) ); 
		//szTxt[MAX_PATH-1] = '\0';
		
		if( FALSE == bActive )
		{
			//DWORD dwTick1 = GetTickCount();
			//DWORD dwTick2=0, dwTick3=0, dwTick4=0;

			txtRepEnd = SendMessage(_hwndEdit,WM_GETTEXTLENGTH,0x0,0x0); // -1;

			// dwTick2 = GetTickCount() - dwTick1;
			//lineIdx=SendMessage(_hwndEdit,EM_LINEFROMCHAR,dwBufferEraseSize,0x0);
			//EB_Printf(TEXT("[dnw] WM_ACTIVATEAPP +++ bActive (%d) txtRepEnd(%u) lineIdx(%u) \r\n"), bActive, txtRepEnd, lineIdx );

			if( txtRepEnd > (int)dwBufferCheckSize )
			{
				//dwTick3 = GetTickCount() - dwTick1;

				lineIdx=SendMessage(_hwndEdit,EM_LINEFROMCHAR,dwBufferEraseSize,0x0);
			
				txtSelEnd=SendMessage(_hwndEdit,EM_LINEINDEX,lineIdx,0x0)-1;
			
				SendMessage(_hwndEdit,EM_SETSEL,0,txtSelEnd+1);
				SendMessage(_hwndEdit,EM_REPLACESEL,0,(LPARAM)"");
			
				//make the end of the text shown.
				txtRepEnd=SendMessage(_hwndEdit,WM_GETTEXTLENGTH,0x0,0x0)-1;
			
			
				SendMessage(_hwndEdit,EM_SETSEL,txtRepEnd+1,txtRepEnd+2); 
				SendMessage(_hwndEdit,EM_REPLACESEL,0,(LPARAM)"");

				//dwTick4 = GetTickCount() - dwTick1;

			}

			//wsprintf( szTxt, TEXT("dwThreadID=0x%X, [[%u,  %u, %u ]]"), dwThreadID, dwTick2, dwTick3, dwTick4 );
			//MessageBox(_hwndEdit, szTxt, TEXT("Message \n"), MB_OK | MB_ICONINFORMATION );			
			
		}
	}

		break; 
#endif



#if defined(COLOR_DISP2) /// 2010.05.14
	/*
	  * Read-only or disabled edit controls do not send the WM_CTLCOLOREDIT message; 
	  * instead, they send the WM_CTLCOLORSTATIC 
	  */


	//case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLOREDIT: //for changing FONT & COLOR
		//MessageBox(_hwndEdit, TEXT("WM_CTLCOLOREDIT \n"), TEXT("WM_CTLCOLOREDIT \n"), MB_OK | MB_ICONINFORMATION );
		{  
			HDC hdc=(HDC)wParam;  
			SetTextColor(hdc,COLOR_RED); /*red text*/  
			return (LRESULT)hdc; /*this brush used by system to paint control background */ 
		}
		return FALSE;

	case WM_CTLCOLORSTATIC:
		//MessageBox(_hwndEdit, TEXT("WM_CTLCOLORSTATIC \n"), TEXT("WM_CTLCOLORSTATIC \n"), MB_OK | MB_ICONINFORMATION );

		if( 1 == ColorType ) /* 0:default, 1:Black style */
		{
			//MessageBox(_hwndEdit, TEXT("WM_CTLCOLORSTATIC+++ \n"), TEXT("WM_CTLCOLORSTATIC+++ \n"), MB_OK | MB_ICONINFORMATION );
			SetBkColor( (HDC)wParam, COLOR_BLACK );
			SetTextColor( (HDC)wParam, COLOR_WHITE );
			return (LRESULT)m_Brush;  
		}
		else if( 2 == ColorType ) /* 0:default, 1:Black style */
		{
			//MessageBox(_hwndEdit, TEXT("WM_CTLCOLORSTATIC+++ \n"), TEXT("WM_CTLCOLORSTATIC+++ \n"), MB_OK | MB_ICONINFORMATION );
			SetBkColor( (HDC)wParam, COLOR_GRAY0 );
			SetTextColor( (HDC)wParam, COLOR_WHITE );
			return (LRESULT)m_Brush;  
		}
		else if( 3 == ColorType ) /* 0:default, 1:Black style */
		{
			//MessageBox(_hwndEdit, TEXT("WM_CTLCOLORSTATIC+++ \n"), TEXT("WM_CTLCOLORSTATIC+++ \n"), MB_OK | MB_ICONINFORMATION );
			SetBkColor( (HDC)wParam, COLOR_GRAY9 );
			SetTextColor( (HDC)wParam, COLOR_BLACK );
			return (LRESULT)m_Brush;  
		}

#if 0
		else {
			SetBkColor((HDC)wParam, COLOR_GRAY1 );
			SetTextColor((HDC)wParam, COLOR_GRAY6 );
			return (LRESULT)m_Brush;  
		}
#endif

		// ~~ return (LRESULT)m_Brush;  
		// ~~ return GetStockObject(LTGRAY_BRUSH);
		// ~~ return (HBRUSH)m_Brush.GetSafeHandle();
		break;



	

#if 0
	case WM_MOUSEACTIVATE:
		EB_Printf(TEXT("[dnw] WM_MOUSEACTIVATE  \n"));
		break;
		
	case WM_MOUSEMOVE:
		EB_Printf(TEXT("[dnw] WM_MOUSEMOVE  \n"));
		break;

	case WM_MOUSEHWHEEL:
		EB_Printf(TEXT("[dnw] WM_MOUSEHWHEEL  \n"));
		break;

	case WM_LBUTTONDOWN:
		EB_Printf(TEXT("[dnw] WM_LBUTTONDOWN  \n"));

		break;

#endif


#if 0	  
    case WM_CTLCOLOR: //for changing FONT & COLOR
    case WM_CTLCOLOREDIT: //for changing FONT & COLOR
	    EB_Printf(TEXT("[dnw] WM_CTLCOLOR  \n"));
		if ( HIWORD(lParam) == CTLCOLOR_EDIT ) 
		{
			EB_Printf(TEXT("[dnw] WM_CTLCOLOREDIT  \n"));
			// Set the text background color.
			SetBkColor((HDC)wParam,  RGB(128,128,128) );

			// Set the text foreground color.
			///SetTextColor(wParam, RGB(255, 255, 255) );
			// Return the control background brush.
			return 0; /// GetStockObject(LTGRAY_BRUSH);
		}
		else
			return 0; /// GetStockObject(WHITE_BRUSH);
#endif
#endif /////////////////////////////// COLOR_DISP /// 2010.05.14

//////////////////////////////////////////////////////////////////////////////
#if 0 /// refer 
	//case WM_SETFOCUS:
	//case WM_COMMAND:
	//case WM_NULL:
	case WM_ACTIVATE:
		{
		TCHAR aaa[256];
		EB_Printf(TEXT("[dnw] Message WM_ACTIVATE (0x%08X) ---) \n"),  Message );
		//wsprintf(aaa, TEXT("[dnw] Message (0x%08X) ---) \n"), Message );
		//MessageBox(_hwndEdit, aaa, TEXT("Message \n"), MB_OK | MB_ICONINFORMATION );
		}
		break; 
		//return 0;
	WM_ACTIVATE : LOWORD(wParam) : 윈도우가 활성화되었는지 비활성화 되었는지를 표현한다. 
	어떤시점에서 활성화 상태의 윈도우는 오직 하나만 존재할 수 있다. 활성화되어 있다는 것은 현재 사용자가 사용하고 있는 윈도우라는 뜻이다. 
	탑 레벨(오버랩드, 팝업)윈도우만 활성화 될 수 있으며 차일드 윈도우는 활성화될 수 없다. 활성화된 부모 윈도우에 속한 차일드 윈도우는 포커스를 가질 수 있다. 
	이 메시지는 윈도우의 활성화 상태가 변경될 때 보내지는데 새로 활성화되는 윈도우와 활성 상태를 잃는 윈도우에게 동시에 보내진다. 
	예를 들어 A윈도우가 활성화된 상태에서 사용자가 B윈도우를 선택했다면 A윈도우에게는 비활성화된다는 메시지가 전달되며 B윈도우에게는 새로 활성화 된다는 메시지가 전달된다.
	2윈도우가 같은 스레드에 소속되어 있으면 즉, 동일한 메시지 큐를 사용하고 있으면 메시지는 동기적으로 전달된다. 
	비활성화되는 윈도우에게 메시지가 먼저 전달되고 이 메시지가 처리된 후 활성화된 윈도우에게 메시지가 이어서 전달된다. 
	따라서 비활성화되는 윈도우가 이 메시지에 늦게 응답하면 활성화되는 윈도우도 그만큼 늦게 활성화된다. 
	두 윈도우가 다른 메시지 큐를 사용하고 있으면 이 메시지는 비동기적으로 전달되므로 
	비활성화되는 윈도우의 응답 여부에 상관없이 새로 활성화 되는 윈도우는 즉시 사용 가능한 상태가 된다. 
	DefWindowProc 은 최소화되지 않은 윈도우가 활성화될 때 키보드 포커스를 전달한다. 
	사용자가 마우스로 윈도우를 클릭하여 활성화했다면 WM_MOUSEACTIVATE 메시지가 먼저 전달되고 이 메시지의 응답 결과에 따라 활성화 여부가 결정된다. 

	case WM_MOVING:
	case WM_MOVE:
	case WM_KILLFOCUS:
	case WM_ENABLE:
	case WM_SETREDRAW:
	case WM_SETTEXT:
	case WM_GETTEXT:
	case WM_GETTEXTLENGTH:
	case WM_PAINT:
	case WM_ERASEBKGND:
	case WM_SYSCOLORCHANGE:
	case WM_SHOWWINDOW:
	case WM_WININICHANGE:
	case WM_DEVMODECHANGE:
	case WM_FONTCHANGE:
	case WM_TIMECHANGE:
	case WM_CANCELMODE:
	case WM_SETCURSOR:
	case WM_MOUSEACTIVATE:
	case WM_CHILDACTIVATE:
	case WM_QUEUESYNC:
	case WM_GETMINMAXINFO:
		//PMINMAXINFO p = (PMINMAXINFO)lParam;
	case WM_PAINTICON:
	case WM_ICONERASEBKGND:
	case WM_NEXTDLGCTL:
	case WM_SPOOLERSTATUS:
	case WM_DRAWITEM:
	case WM_MEASUREITEM:
	case WM_DELETEITEM:
	case WM_VKEYTOITEM:
	case WM_CHARTOITEM:
	case WM_SETFONT:
	case WM_GETFONT:
	case WM_SETHOTKEY:
	case WM_GETHOTKEY:
	case WM_QUERYDRAGICON:
	case WM_COMPAREITEM:
	case WM_GETOBJECT:
	case WM_COMPACTING:
	case WM_COMMNOTIFY:
	case WM_WINDOWPOSCHANGING:
		//PWINDOWPOS p = (PWINDOWPOS) lParam;
	case WM_WINDOWPOSCHANGED:
	case WM_POWER:
	case WM_COPYDATA:
	case WM_CANCELJOURNAL:
	case WM_CONTEXTMENU:
	case WM_STYLECHANGING:
	case WM_STYLECHANGED:
	case WM_GETICON:
	case WM_NCCREATE:
	case WM_NCCALCSIZE:
	case WM_NCHITTEST:
	case WM_NCPAINT:
	case WM_NCACTIVATE:
	case WM_NCMOUSEMOVE:
	case WM_NCLBUTTONDOWN:
	case WM_NCLBUTTONUP:
	case WM_NCLBUTTONDBLCLK:
	case WM_NCRBUTTONDOWN:
	case WM_NCRBUTTONUP:
	case WM_NCRBUTTONDBLCLK:
	case WM_NCMBUTTONDOWN:
	case WM_NCMBUTTONUP:
	case WM_NCMBUTTONDBLCLK:
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_DEADCHAR:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_SYSCHAR:
	case WM_SYSDEADCHAR:
	//case WM_UNICHAR:
	case WM_IME_STARTCOMPOSITION:
	case WM_IME_ENDCOMPOSITION:
	case WM_IME_COMPOSITION:
	case WM_SYSCOMMAND:
	case WM_HSCROLL:
	case WM_VSCROLL:
	case WM_INITMENU:
	case WM_INITMENUPOPUP:
	case WM_MENUSELECT:
	case WM_MENUCHAR:
	case WM_ENTERIDLE:
	case WM_UNINITMENUPOPUP:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
	//case WM_MOUSEWHEEL:
	//case WM_XBUTTONDOWN:
	//case WM_XBUTTONUP:
	//case WM_XBUTTONDBLCLK:
	//case WM_MOUSEHWHEEL:
	case WM_ENTERMENULOOP:
	case WM_EXITMENULOOP:
	case WM_SIZING:
	case WM_CAPTURECHANGED:
	case WM_ENTERSIZEMOVE:
	case WM_EXITSIZEMOVE:
	case WM_IME_SETCONTEXT:
	case WM_IME_NOTIFY:
	case WM_IME_CONTROL:
	case WM_IME_COMPOSITIONFULL:
	case WM_IME_SELECT:
	case WM_IME_CHAR:
	case WM_IME_KEYDOWN:
	case WM_NCMOUSELEAVE:



#endif
//////////////////////////////////////////////////////////////////////////////

    }


    return DefWindowProc(hwnd, Message, wParam, lParam); 

}




// ------------------------------------------------------------
//8: \b
//13: \r

// 1)about EM_SETSEL message arguments
//   start_position,end_position
//   The selected character is from start_position to end_position-1;
//   So, We should do +1 to the position of last selected character.
// 2)The char index start from 0.
// 3)WM_CLEAR,WM_CUT doesn't operate on EM_SETSEL chars.
// ------------------------------------------------------------


#if 0
void KeyDownProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if ((wParam==VK_SHIFT) ||
	    (wParam==VK_CONTROL) ||
	    (wParam==VK_MENU)) {
		return;
	}

	Scan = HIWORD(lParam) & 0x1ff;
	if ((GetKeyState(VK_SHIFT) & 0x80) != 0) {
		Scan = Scan | 0x200;
	}
	if ((GetKeyState(VK_CONTROL) & 0x80) != 0) {
		Scan = Scan | 0x400;
	}
	if ((GetKeyState(VK_MENU) & 0x80) != 0) {
		Scan = Scan | 0x800;
	}

	if (! KeyDown) {
		KeyDown = TRUE;
		Short = TRUE;

		// _snprintf_s(OutStr,sizeof(OutStr),_TRUNCATE,"Key code is %u",Scan);
		EB_Printf(TEXT("[dnw] Key code is %u \r\n"), Scan );
				
		//SetTimer(hWnd,1,10,NULL);
		//InvalidateRect(hWnd,NULL,TRUE);
	}
}

void KeyUpProc(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	(void)wParam;
	(void)lParam;
	if (! KeyDown) {
		return;
	}
	if (Short) {
		//SetTimer(hWnd,2,500,NULL);
	}
	else {
		KeyDown = FALSE;
		//InvalidateRect(hWnd,NULL,TRUE);
	}
}

#if 0
void PaintProc(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hDC;
	char OutStr[30];

	hDC = BeginPaint(hWnd, &ps);

	if (KeyDown) {
		_snprintf_s(OutStr,sizeof(OutStr),_TRUNCATE,"Key code is %u",Scan);
		TextOutA(hDC,10,10,OutStr, (int)strlen(OutStr));
	}
	else {
		TextOutA(hDC,10,10,"Push any key.",13);
	}

	EndPaint(hWnd, &ps);
}
#endif
#endif





volatile static int isUsed=0;


#if 1 /* 2011.02.08 , Text Mode or Hexa Mode */
const TCHAR HexEncodeTable[16+1] = TEXT("0123456789ABCDEF"); 
const TCHAR HexEncodeLower[16+1] = TEXT("0123456789abcdef"); // 2018.03.22
#endif

#if DISPLAY_MAX_EDIT_BUF_SIZE // 2016.02.12
unsigned int iEditCount = 0, iLineDeleted=0;
#endif



#if RICHED_COLOR // 2016.09.22
void RichEditInit(void)
{
	
	memset( &cf, 0x00, sizeof(cf) );		
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR|CFM_BOLD; // |CFM_FACE|CFM_CHARSET|CFM_UNDERLINE; /// CFM_ALL2|CFM_COLOR; /// |CFM_BOLD; // CFM_UNDERLINETYPE | CFM_UNDERLINE;
	
	//CFM_BOLD // The CFE_BOLD value of the dwEffects member is valid 
	//CFM_CHARSET // The bCharSet member is valid. 
	//CFM_COLOR // The crTextColor member and the CFE_AUTOCOLOR value of the dwEffects member are valid 
	//CFM_FACE // The szFaceName member is valid. 
	//CFM_ITALIC // The CFE_ITALIC value of the dwEffects member is valid 
	//CFM_OFFSET // The yOffset member is valid 
	//CFM_PROTECTED // The CFE_PROTECTED value of the dwEffects member is valid 
	//CFM_SIZE // The yHeight member is valid 
	//CFM_STRIKEOUT // The CFE_STRIKEOUT value of the dwEffects member is valid. 
	//CFM_UNDERLINE // The	
	
	//cf.dwEffects = CFE_AUTOCOLOR; // |CFE_BOLD; // CFE_UNDERLINE
	//CFE_AUTOCOLOR // Use the system text color 
	//CFE_BOLD //Characters are bold 
	//CFE_ITALIC //Characters are italic 
	//CFE_STRIKEOUT //Characters are struck. 
	//CFE_UNDERLINE //Characters are underlined 
	//CFE_PROTECTED //Characters are protected; an attempt to modify them will cause an EN_PROTECTED notification message.	
	
	cf.crTextColor = COLOR_BLACK;
	
	//	cf.bUnderlineType = CFU_UNDERLINEWAVE;
	//	cf.bUnderlineColor = 0x05;
	//	cf.yHeight = ;
	//	cf.yOffset = ;
	//	cf.bCharSet = ;
	//	cf.bPitchAndFamily = ;
	//	cf.szFaceName[LF_FACESIZE];
}


void RichTxtColor(DWORD xx, DWORD yy, DWORD line, COLORREF tColor)
{
	// crStop = SendMessage(_hwndEdit, WM_GETTEXTLENGTH, 0, 0);

	SendMessage(_hwndEdit, EM_SETSEL, xx, yy);
	cf.crTextColor = tColor;
	SendMessage(_hwndEdit, EM_SETCHARFORMAT, (WPARAM)SCF_SELECTION, (LPARAM) &cf);
	SendMessage(_hwndEdit, EM_SETSEL, -1, -1);

	cf.crTextColor = COLOR_BLACK;

	InitFindMark(tColor);

}
#endif


#if (RICHED_COLOR) || (TEXT_FIND_MARK) // 2016.09.23

void InitFindMark(COLORREF tColor)
{
	if( COLOR_RED == tColor )
		txtFindRed = 0;

	if( COLOR_MAGENTA == tColor )
		txtFindMegenta = 0;

#if 0
	if( COLOR_BLUE == tColor )
		txtFindBlue = 0;

	if( COLOR_GREEN2 == tColor )
		txtFindGreen = 0;
		
	if( COLOR_YELLOW2 == tColor ) 
		txtFindYellow = 0;
#endif
}

void IsMarking(void)
{
	if( 0==txtColorRed /* && 0==txtColorMegenta && 0==txtColorBlue && 0==txtColorGreen && 0==txtColorYellow*/ )
		isTxtMark = 0;
	else
		isTxtMark = 1;
}



void RichTxtFind(TCHAR *string2, int txtRepStart, int txtRepEnd, DWORD *ColorStart, DWORD *ColorEnded)
{
	int newLineCount = 0;


	if( txtColorRed && iLenDataRed>=TEXT_LEN_MIN_SIZ) {
		if( NULL != _tcsstr( string2, szDataRed ) )
		{
			newLineCount = SendMessage(_hwndEdit,EM_LINEFROMCHAR,dwBufferEraseSize,0x0);
			//if( newLineCount > 0 ) 
				txtFindRed = 1;
			
			*ColorStart = txtRepStart - newLineCount;
			*ColorEnded = txtRepEnd - newLineCount; // - 1;
		}
	}


#if 0
	if( txtColorMegenta && iLenDataMagenta>=TEXT_LEN_MIN_SIZ ) {
		if( NULL != _tcsstr( string2, szDataMagenta ) )
		{
			newLineCount = SendMessage(_hwndEdit,EM_LINEFROMCHAR,dwBufferEraseSize,0x0);
			//if( newLineCount > 0 ) 
				txtFindMegenta = 1;

			*ColorStart = txtRepStart - newLineCount;
			*ColorEnded = txtRepEnd - newLineCount; // - 1;
		}
	}

	if( txtColorBlue && iLenDataBlue>=TEXT_LEN_MIN_SIZ ) {
		if( NULL != _tcsstr( string2, szDataBlue ) )
		{
			newLineCount = SendMessage(_hwndEdit,EM_LINEFROMCHAR,dwBufferEraseSize,0x0);
			//if( newLineCount > 0 ) 
				txtFindBlue = 1;

			*ColorStart = txtRepStart - newLineCount;
			*ColorEnded = txtRepEnd - newLineCount; // - 1;
		}
	}

	if( txtColorGreen && iLenDataGreen>=TEXT_LEN_MIN_SIZ ) {
		if( NULL != _tcsstr( string2, szDataGreen ) )
		{
			newLineCount = SendMessage(_hwndEdit,EM_LINEFROMCHAR,dwBufferEraseSize,0x0);
			//if( newLineCount > 0 ) 
				txtFindGreen = 1;

			*ColorStart = txtRepStart - newLineCount;
			*ColorEnded = txtRepEnd - newLineCount; // - 1;
		}
	}

	if( txtColorYellow && iLenDataYellow>=TEXT_LEN_MIN_SIZ ) {
		if( NULL != _tcsstr( string2, szDataYellow ) )
		{
			newLineCount = SendMessage(_hwndEdit,EM_LINEFROMCHAR,dwBufferEraseSize,0x0);
			//if( newLineCount > 0 ) 
				txtFindYellow = 1;

			*ColorStart = txtRepStart - newLineCount;
			*ColorEnded = txtRepEnd - newLineCount; // - 1;
		}
	}
#endif
}


#define HASH_MOD 1000000009
long long make_hash_p(TCHAR str[], int _size) 
{
	int i;
	long long hash_p = 0;
	for (i = 0; i < _size; i++) {
		hash_p *= 26;
		hash_p += str[i];
		hash_p %= HASH_MOD;
	}
	return hash_p;
}


int match_s(TCHAR str[], int _size, int _size_p, long long hash_p) 
{
	int i;
	long long hash_s = 0;
	long long last = 1;

	if( _size < _size_p ) return 0;

	for (i = 0; i < _size_p; i++) {
		hash_s *= 26;
		hash_s += str[i];
		hash_s %= HASH_MOD;

		if (i == _size_p - 1) continue;
		last *= 26;
		last %= HASH_MOD;
	}
	for (i = _size_p; i <= _size; i++) {
		if (i >= _size_p) {
			if (hash_s == hash_p) return 1; /// OK printf("match %d\n", i - _size_p + 1);
			if (i == _size) break;

			hash_s -= last * str[i - _size_p];
			hash_s *= 26;
			hash_s += str[i];
			hash_s %= HASH_MOD;
			if (hash_s < 0) hash_s += HASH_MOD; //빼는 과정에서 해시값이 음수가 되는 경우를 방지
		}
	}
	return 0;
}


void RichTxtFindMarking(TCHAR *string2, int txtRepStart, int txtRepEnd, int fPos, int sPos)
{
//	int newLineCount = 0;
	int iloop=0;

	txtFindRed=0;
	txtFindMegenta=0;
	txtFindBlue=0;
	txtFindGreen=0;
	txtFindYellow=0;


	if( txtColorRed && iLenDataRed>=TEXT_LEN_MIN_SIZ) {

		#if 0

		for(iloop=0; iloop<length-iLenDataRed; iloop++) {
			if( 0==strncmp( &string2[sPos+iloop], szDataRed, iLenDataRed ) ) {
				txtFindRed = 1;
				string2[fPos+0] = TEXT('*');
				string2[fPos+1] = TEXT('*');
				string2[fPos+2] = TEXT('*');
				break;
			}
		}
		
		#else

		if( NULL != _tcsstr( &string2[sPos], szDataRed ) )
		{
			//newLineCount = SendMessage(_hwndEdit,EM_LINEFROMCHAR,dwBufferEraseSize,0x0);
			//if( newLineCount > 0 ) 
			txtFindRed = 1;
			string2[fPos+0] = TEXT('*');
			string2[fPos+1] = TEXT('*');
			string2[fPos+2] = TEXT('*');
		}

		#endif
	}


#if 0

	if( txtColorMegenta && iLenDataMagenta>=TEXT_LEN_MIN_SIZ ) {
		if( NULL != _tcsstr( &string2[sPos], szDataMagenta ) )
		{
			//newLineCount = SendMessage(_hwndEdit,EM_LINEFROMCHAR,dwBufferEraseSize,0x0);
			//if( newLineCount > 0 ) 
			txtFindMegenta = 1;

			//string2[0] = TEXT('>');
			//string2[1] = TEXT('>');
			//string2[2] = TEXT('>');

//{
//TCHAR aaa[250];
//wsprintf(aaa, TEXT("len = %u "), iLenDataMagenta);
//MessageBox(_hwndEdit,aaa,TEXT("RichTxtFindMarking !000!! \n"), MB_OK | MB_ICONINFORMATION );
//}


		}
		else txtFindMegenta = 0;

	}


	if( txtColorBlue && iLenDataBlue>=TEXT_LEN_MIN_SIZ ) {
		if( NULL != _tcsstr( &string2[sPos], szDataBlue ) )
		{
			//newLineCount = SendMessage(_hwndEdit,EM_LINEFROMCHAR,dwBufferEraseSize,0x0);
			//if( newLineCount > 0 ) 
				txtFindBlue = 1;

			string2[0] = TEXT('/');
			string2[1] = TEXT('/');
			string2[2] = TEXT('/');
		}
	}

	if( txtColorGreen && iLenDataGreen>=TEXT_LEN_MIN_SIZ ) {
		if( NULL != _tcsstr( &string2[sPos], szDataGreen ) )
		{
			//newLineCount = SendMessage(_hwndEdit,EM_LINEFROMCHAR,dwBufferEraseSize,0x0);
			//if( newLineCount > 0 ) 
				txtFindGreen = 1;

			string2[0] = TEXT('<');
			string2[1] = TEXT('<');
			string2[2] = TEXT('<');
		}
	}

	if( txtColorYellow && iLenDataYellow>=TEXT_LEN_MIN_SIZ ) {
		if( NULL != _tcsstr( &string2[sPos], szDataYellow ) )
		{
			//newLineCount = SendMessage(_hwndEdit,EM_LINEFROMCHAR,dwBufferEraseSize,0x0);
			//if( newLineCount > 0 ) 
				txtFindYellow = 1;

			string2[0] = TEXT('$');
			string2[1] = TEXT('$');
			string2[2] = TEXT('$');
		}
	}
#endif
}

#endif



#if MODIFIED_JULIAN_DATE /// 2014.07.04
#if 0 // 2017.08.31. move to engine.h
////////////////////////////////////////////
/// Modified Julian Date
/// 2013.09.14 by top.joo
////////////////////////////////////////////

/* ------------------------------------------------- */
/* --  2013.09.14 : KBS EWS (Emergency Warning Service) --- */
/* ------------------------------------------------- */
/* --- Modified Julian Date ------ */
typedef struct {
	WORD  m_year;
	WORD  m_month;
	WORD  wDayOfWeek;  
	WORD  m_day;
	WORD  m_hour;
	WORD  m_mins;
	WORD  m_secs;
	WORD  m_millis;
} mjd_timestamp;


#define PRECISION_LEVEL 				1000000
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
#endif


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

	double a = floor( (long double)(y / 100.0) );
	double b = 2 - a + floor( (long double)(a / 4.0) );

	return (floor( (long double)( 365.25 * (y + 4716.0)) ) +
			floor( (long double)(30.6001 * (m + 1)) ) + d + b - 1524.5) 
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




void ScrClear(HWND hwnd)
{
#ifdef __TEST__
	HWND hEdit = GetDlgItem (hDlg, ID_EDIT);
	int ndx = GetWindowTextLength (hEdit);

	SetFocus (hEdit);

	SendMessage (hEdit, EM_SETSEL, (WPARAM)ndx, (LPARAM)ndx);
	SendMessage (hEdit, EM_SETSEL, 0, MAKELONG (ndx, ndx));
#endif


#if RICHED_COLOR // 2016.09.21 +++++++++
	RichEditInit();
#endif /* RICHED_COLOR */

    /// memset(string, 0x00, sizeof(TCHAR)*(STRING_LEN+4096) );
    /// memset( string2, 0x00, sizeof(string2)*sizeof(TCHAR) );

	
	// ** EM_SETSEL message
	// Selects a range of characters in an edit control. You can send this message to either an edit control or a rich edit control.
	// Parameters
	// wParam  : The starting character position of the selection. 
	// lParam  : The ending character position of the selection. 
	// Return value: This message does not return a value. 
	// Remarks
	// The start value can be greater than the end value. 
	// The lower of the two values specifies the character position of the first character in the selection. 
	// The higher value specifies the position of the first character beyond the selection. 
	// The start value is the anchor point of the selection, and the end value is the active end. 
	// If the user uses the SHIFT key to adjust the size of the selection, the active end can move but the anchor point remains the same.
	// 
	// If the start is 0 and the end is -1, all the text in the edit control is selected. 
	// If the start is -1, any current selection is deselected. 

	// ** EM_REPLACESEL message
	// Replaces the selected text in an edit control or a rich edit control with the specified text.
	// Parameters
	// wParam  : Specifies whether the replacement operation can be undone. 
	//           If this is TRUE, the operation can be undone. If this is FALSE , the operation cannot be undone. 
	// lParam  : A pointer to a null-terminated string containing the replacement text. 
	// Return value : This message does not return a value. 

#if RICHED_COLOR // 2016.09.21 +++++++++
	SendMessage(_hwndEdit, EM_SETSEL, 0, -1); // all text is selected.
	///SendMessage(_hwndEdit,WM_CLEAR, -1, 0); // all text is deselected.
	SendMessage(_hwndEdit, EM_REPLACESEL, 0, (LPARAM)" ");	// If this is FALSE , the operation cannot be undone. 

#else
	SendMessage(_hwndEdit, EM_SETSEL, 0, -1); // all text is selected.
	///SendMessage(_hwndEdit,WM_CLEAR, -1, 0); // all text is deselected.
	SendMessage(_hwndEdit, EM_REPLACESEL, 0, (LPARAM)"");  // If this is FALSE , the operation cannot be undone. 
#endif


#if DISPLAY_MAX_EDIT_BUF_SIZE // 2016.02.12
	iEditCount = 0;
	iLineDeleted = 0;
#endif


#if HEXA_MODE_SUPPORT	
	uCount = 0;
#endif
}



void EB_PrintfHexa(int iLength, TCHAR *fmtData) // ,...)
{
//	va_list ap;

	int i,slen;
	int lineIdx;
	int txtRepStart,txtRepEnd,txtSelEnd;
	static int wasCr=0; //should be static type.
	static TCHAR string[STRING_LEN +1 /*+4096*/] = {0,}; //margin for '\b'
	static TCHAR string2[STRING_LEN +1 /*+4096*/]= {0,}; //margin for '\n'->'\r\n'
	static int prevBSCnt=0;

#if (RICHED_COLOR) || (TEXT_FIND_MARK) // 2016.09.23
	DWORD ColorStart=0, ColorEnded=0;
	int startPos=0;
	int FrontPos=0;
	int iMatchedRed=0;
#endif

	int str2Pt=0;
	
#if HEXA_MODE_SUPPORT
	static int tmpTextorHexaMode=0xff;
#endif
	static int tempScrBufOnce=0xff;



#ifdef _DELETE_FUNC_ /* -- 2013.04.20 -- */
	while( 1 == isUsed )
	{
		; //EB_Printf can be called multiplely  //KIW
	}
#endif



#if 0 // 2017.03.27, HEXA_MODE_SUPPORT
	if( tmpTextorHexaMode != TextorHexaMode ) // Hexa mode (1 or 2) <-> Text Mode (0)
	{
		if( tmpTextorHexaMode && TextorHexaMode ) 
		{
			// No Screen clear
		}
		else 
			ScrClear(_hwndEdit);

		tmpTextorHexaMode = TextorHexaMode;
		memset( string, 0x00, sizeof(string)*sizeof(TCHAR) );
		memset( string2, 0x00, sizeof(string2)*sizeof(TCHAR) );

	}
#endif


	// ** WM_GETTEXTLENGTH
	// Determines the length, in characters, of the text associated with a window. 
	// wParam  : This parameter is not used and must be zero. 
	// lParam  : 	This parameter is not used and must be zero. 
	// Return value : LRESULT
	//    The return value is the length of the text in characters, not including the terminating null character.
	txtRepStart = SendMessage(_hwndEdit,WM_GETTEXTLENGTH,0x0,0x0);
	txtRepEnd   = txtRepStart-1;


#if 0 // USE_RX_TX_QUEUE
	
	// DeQueue 하기 전에 Queue empty 확인 ---
	if( EmptyCQueue(&Rx2Tx) )
	{
		isUsed=0;
		return;
	}
	else
	{
		fmt = DeCQueue(&Rx2Tx);
	}
#endif





#if 0 // 2016.10.21
	va_start(ap,fmt);
	_vsntprintf(string2,STRING_LEN-1,fmt,ap);
	va_end(ap);
#else

	if( iLength )
		memcpy(string2, fmtData, iLength);
#endif

	string2[STRING_LEN-1]='\0';







#if HEXA_MODE_SUPPORT	/* 2011.02.08 Hexa mode display */
	if( 1==TextorHexaMode || 2==TextorHexaMode ) /* Hexa mode all (including special chars)  ==> type : FF */
	{
		string[0 /*prevBSCnt*/]='\0';
		// 2016.10.20, lstrcat(string,string2);
		memcpy(string, string2, iLength);
		string2[0]='\0';

		slen=lstrlen(string);	
	} 
#endif
	


    if(/*slen==0 || */ 0==iLength)
	{
		isUsed=0;
		return;
	}



	for(i=0;i<iLength /* slen */;i++)
	{

	#if HEXA_MODE_SUPPORT	/* 2011.02.08 Hexa mode display */
		if( 1==TextorHexaMode || 2==TextorHexaMode ) /* Hexa mode all (including special chars)  ==> type : FF */
		{

			if( 2==TextorHexaMode ) // Hexa mode -- End Char is 0x81, 0x0D
			{

				// 2018.03.23 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				if( HEXA_DISP_ADD0X == (userHexaDispVal & HEXA_DISP_ADD0X) ) // 2018.03.23
				{
					string2[str2Pt++] = TEXT('0');
					string2[str2Pt++] = TEXT('x');
					txtRepEnd += 2; 		
				}
				// 2018.03.23 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				

				if( HEXA_DISP_LOWERCASE == (userHexaDispVal & HEXA_DISP_LOWERCASE) ) // 2018.03.22
				{
					string2[str2Pt++] = HexEncodeLower[ (string[i] & 0xF0) >> 4 ]; // Lower-case
					string2[str2Pt++] = HexEncodeLower[ (string[i] & 0x0F) >> 0 ]; // Lower-case
					txtRepEnd += 2;
				}
				else
				{				
					string2[str2Pt++] = HexEncodeTable[ (string[i] & 0xF0) >> 4 ];  // uppercase
					string2[str2Pt++] = HexEncodeTable[ (string[i] & 0x0F) >> 0 ]; 
					txtRepEnd += 2;
				}

				if( HEXA_DISP_COMMA == (userHexaDispVal & HEXA_DISP_COMMA) )
				{
					string2[str2Pt++] = TEXT(','); 
					txtRepEnd += 1;
				}
				else
				{
					string2[str2Pt++] = TEXT(' '); 
					txtRepEnd += 1;
				}	
				// ----------------------------------------------------
				

				if( HexaEndChar > 0xFFFF ) // 2018.03.21, over 5-hexa 
				{
					if( i>1 )
					{
						if( ((HexaEndChar>>16 & 0x00FF) == string[i-2]) && ((HexaEndChar>>8 & 0x00FF) == string[i-1]) && ((HexaEndChar & 0x00FF) == string[i]) )
						{
							string2[str2Pt++]='\r';
							string2[str2Pt++]='\n';
							txtRepEnd += 2;
						}
					}
				}
				else if( HexaEndChar > 0xFF ) // 2018.03.21
				{
					if( i>0 )
					{
						if( ((HexaEndChar>>8 & 0x00FF) == string[i-1]) && ((HexaEndChar & 0x00FF) == string[i]) )
						{
							string2[str2Pt++]='\r';
							string2[str2Pt++]='\n';
							txtRepEnd += 2;
						}
					}
				}
				else
				{
					if( (HexaEndChar & 0x00FF) == string[i] ) 
					{
						string2[str2Pt++]='\r';
						string2[str2Pt++]='\n';
						txtRepEnd += 2;
					}
				}
				
			}
			else if( 1==TextorHexaMode ) // Hexa mode - Hexa Column Number
			{
				if( ColumnNumber && (0==uCount) ) // 2016.04.02
				{
					string2[str2Pt++]='\r';
					string2[str2Pt++]='\n';
					txtRepEnd += 2;
				}


				// 2018.03.23 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				if( HEXA_DISP_ADD0X == (userHexaDispVal & HEXA_DISP_ADD0X) ) // 2018.03.23
				{
					string2[str2Pt++] = TEXT('0');
					string2[str2Pt++] = TEXT('x');
					txtRepEnd += 2; 		
				}
				// 2018.03.23 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++

				if( HEXA_DISP_LOWERCASE == (userHexaDispVal & HEXA_DISP_LOWERCASE) ) // 2018.03.22
				{
					string2[str2Pt++] = HexEncodeLower[ (string[i] & 0xF0) >> 4 ]; 
					string2[str2Pt++] = HexEncodeLower[ (string[i] & 0x0F) >> 0 ]; 
					txtRepEnd += 2;
				}
				else
				{
					string2[str2Pt++] = HexEncodeTable[ (string[i] & 0xF0) >> 4 ]; 
					string2[str2Pt++] = HexEncodeTable[ (string[i] & 0x0F) >> 0 ]; 
					txtRepEnd += 2;
				}			

				if( HEXA_DISP_COMMA == (userHexaDispVal & HEXA_DISP_COMMA) )
				{
					string2[str2Pt++] = TEXT(','); 
					txtRepEnd += 1;
				}
				else
				{
					string2[str2Pt++] = TEXT(' '); 
					txtRepEnd += 1;
				}
				// ---------------------------------------------------------------


				if( ColumnNumber>0 )
				{
					uCount++;
					uCount = uCount % ColumnNumber;
				}
			}
		
			// 2015.11.13, System Time Info display. added
			// 2017.04.21, isTimeInfoOnce = 0; // 2015.11.13
		}
	#endif // ----------------------------
		
		// if(str2Pt>FILENAMELEN-3)break; //why needed? 2001.1.26
	}



#if 1 // 2017.04.21
	if( (0==ColumnNumber) && (1==TextorHexaMode) ) // column numer==0
	{
		string2[str2Pt++]='\r';
		string2[str2Pt++]='\n';
		txtRepEnd += 2;
	}
#endif


	string2[str2Pt]='\0';

//#if (RICHED_COLOR) || (TEXT_FIND_MARK) // 2016.09.23
//	lstrcpy(string0, string2);
//	string0[str2Pt]='\0';
//#endif

	if(str2Pt>0)
	{


		/* EditBox에 텍스트 이어서 추가하기  , EM_SETTEXTEX*/
		SendMessage(_hwndEdit,EM_SETSEL,	 (WPARAM)txtRepStart, (LPARAM)txtRepEnd+1);
		SendMessage(_hwndEdit,EM_REPLACESEL, (WPARAM)FALSE, 	  (LPARAM)((LPSTR)string2) ); 


		/* log file save */
		if( msgSaveOnOff )
		{
			if( fFileWrite ) // && buff_fin )
			{
				// SetFilePointer(hFile, 0, NULL, FILE_END);			
				// fseek(fFileWrite, 0L, SEEK_END); // 2016.02.28 end of file pointer 

				// original
				fwrite(string2, sizeof(TCHAR), str2Pt, fFileWrite); 

		#if !LOG_SAVE_BY_TIMER  // 2016.02.24,  
				/* 2012.07.07 fflush() 사용으로 변경.... 출력버퍼는 즉시 처리가 된다... flclose() 대신 사용 가능 */
				/// NEVER use-- 2016.02.26 -- fflush(fFileWrite); 
		#endif
			}
		}



	}
	else
	{

		if(txtRepStart<=txtRepEnd)
		{

			SendMessage(_hwndEdit,EM_SETSEL,     (WPARAM)txtRepStart, (LPARAM)txtRepEnd+1);
			SendMessage(_hwndEdit,EM_REPLACESEL, (WPARAM)0,           (LPARAM)"");

		}
	}




#if USER_SCREEN_BUF_SIZE // 2016.02.14

	if( tempScrBufOnce != userScrBufSizIndex )
	{
		tempScrBufOnce = userScrBufSizIndex;

		switch( userScrBufSizIndex )
		{
			case 0: 
				dwBufferCheckSize = HUGE_EDIT_BUF_SIZE; 
				dwBufferEraseSize = HUGE_EDIT_BUF_DEC_SIZE;
				//MessageBox(_hwndEdit,TEXT("PAUSE !000!! \n"),TEXT("PAUSE !000!! \n"), MB_OK | MB_ICONINFORMATION );
				break;
			case 1: 
				dwBufferCheckSize = LARGE_EDIT_BUF_SIZE; 
				dwBufferEraseSize = LARGE_EDIT_BUF_DEC_SIZE;
				//MessageBox(_hwndEdit,TEXT("PAUSE !111!! \n"),TEXT("PAUSE !111!! \n"), MB_OK | MB_ICONINFORMATION );
				break;
			case 2: 
				dwBufferCheckSize = MIDDLE_EDIT_BUF_SIZE; 
				dwBufferEraseSize = MIDDLE_EDIT_BUF_DEC_SIZE;
				//MessageBox(_hwndEdit,TEXT("PAUSE !121!! \n"),TEXT("PAUSE !121!! \n"), MB_OK | MB_ICONINFORMATION );
				break;
			case 3: 
				dwBufferCheckSize = SMALL_EDIT_BUF_SIZE; 
				dwBufferEraseSize = SMALL_EDIT_BUF_DEC_SIZE;
				//MessageBox(_hwndEdit,TEXT("PAUSE !222!! \n"),TEXT("PAUSE !222!! \n"), MB_OK | MB_ICONINFORMATION );
				break;
			case 4: 
				dwBufferCheckSize = SMALLEST_EDIT_BUF_SIZE; 
				dwBufferEraseSize = SMALLEST_EDIT_BUF_DEC_SIZE;
				//MessageBox(_hwndEdit,TEXT("PAUSE !333!! \n"),TEXT("PAUSE !333!! \n"), MB_OK | MB_ICONINFORMATION );
				break; // Smallest
			default: 
				dwBufferCheckSize = HUGE_EDIT_BUF_SIZE; 
				dwBufferEraseSize = HUGE_EDIT_BUF_DEC_SIZE;
				//MessageBox(_hwndEdit,TEXT("PAUSE !DEFAULT !! \n"),TEXT("PAUSE !DEFAULT!! \n"), MB_OK | MB_ICONINFORMATION );
				break;
		}
	}



	if( txtRepEnd > (int)dwBufferCheckSize )

#else

	//If edit buffer is over EDIT_BUF_SIZE,
	//the size of buffer must be decreased by EDIT_BUF_DEC_SIZE.
	if(txtRepEnd > EDIT_BUF_SIZE)
#endif
	{


	#if USER_SCREEN_BUF_SIZE // 2016.02.14
		lineIdx=SendMessage(_hwndEdit,EM_LINEFROMCHAR,dwBufferEraseSize,0x0);
	#else
		lineIdx=SendMessage(_hwndEdit,EM_LINEFROMCHAR,EDIT_BUF_DEC_SIZE,0x0);
	#endif

		//lineIdx=SendMessage(_hwndEdit,EM_LINEFROMCHAR,txtRepEnd-txtRepStart+1,0x0); //for debug
		txtSelEnd=SendMessage(_hwndEdit,EM_LINEINDEX,lineIdx,0x0)-1;

		SendMessage(_hwndEdit,EM_SETSEL,0,txtSelEnd+1);
		SendMessage(_hwndEdit,EM_REPLACESEL,0,(LPARAM)"");

		//SendMessage(_hwndEdit,WM_CLEAR,0,0); //WM_CLEAR doesn't work? Why?
		//SendMessage(_hwndEdit,WM_CUT,0,0); //WM_CUT doesn't work? Why?

		#if 0 // def __TEST_VIEW__
		//EB_Printf(TEXT("[dnw] lineIdx(%d) ---txtSelEnd(%d) \n"),  lineIdx, txtSelEnd );
		MessageBox(_hwndEdit,TEXT("PAUSE !212!! \n"),TEXT("PAUSE !212!! \n"), MB_OK | MB_ICONINFORMATION );
		#endif


		//make the end of the text shown.
		txtRepEnd=SendMessage(_hwndEdit,WM_GETTEXTLENGTH,0x0,0x0)-1;

	#if 0 // 2016.02.12
		SendMessage(_hwndEdit,EM_SETSEL,txtRepEnd+1,txtRepEnd+2); 
		SendMessage(_hwndEdit,EM_REPLACESEL,0,(LPARAM)" ");
	#endif

		SendMessage(_hwndEdit,EM_SETSEL,txtRepEnd+1,txtRepEnd+2); 
		SendMessage(_hwndEdit,EM_REPLACESEL,0,(LPARAM)"");


	#if DISPLAY_MAX_EDIT_BUF_SIZE // 2016.02.12
		iEditCount++;
		iLineDeleted = lineIdx;
	#endif


	}

	isUsed=0;

}


// COnvert float to string
// 2017.08.30
void f2a(double number, TCHAR* string, int precision)
{
	wsprintf (string, TEXT("%06d.%06u"), (int)number, (int) ((number - (int) number ) * precision) );
}//end ftoa()



void EB_Printf(const TCHAR *fmt,...)
{
	va_list ap;
	int i,slen;
	int lineIdx;
	int txtRepStart,txtRepEnd,txtSelEnd;
	static int wasCr=0; //should be static type.
	static TCHAR string[STRING_LEN +1 /*+4096*/] = {0,}; //margin for '\b'
	static TCHAR string2[STRING_LEN +1 /*+4096*/]= {0,}; //margin for '\n'->'\r\n'
	static int prevBSCnt=0;

#if (RICHED_COLOR) || (TEXT_FIND_MARK) // 2016.09.23
	DWORD ColorStart=0, ColorEnded=0;
	int startPos=0;
	int FrontPos=0;
	int iMatchedRed=0;
#endif

#define LINE_MAX 			2000 /* 2020.10.28 Fix */
	int str2Pt=0;
	int inewline[LINE_MAX] = { 0, }, ilinefeed[LINE_MAX] = {0,};
	int lnidx = 0, fdidx=0;
	
#if HEXA_MODE_SUPPORT
	static int tmpTextorHexaMode=0xff;
#endif
	static int tempScrBufOnce=0xff;

#if MODIFIED_JULIAN_DATE // 2017.08.30
	mjd_timestamp mJDTime; 
	TCHAR         mJDtemp[20] = {0,};
	int           imJDLen=0; // 2018.07.06
#endif

	mjd_timestamp LinTime;  // 2020.06.30

#ifdef _DELETE_FUNC_ /* -- 2013.04.20 -- */
	while( 1 == isUsed )
	{
		; //EB_Printf can be called multiplely  //KIW
	}
#endif



#if 0 // 2017.03.27, -- deleted --- HEXA_MODE_SUPPORT

	if( tmpTextorHexaMode != TextorHexaMode ) // Hexa mode (1 or 2) <-> Text Mode (0)
	{
		if( tmpTextorHexaMode && TextorHexaMode ) 
		{
			// No Screen clear
		}
		else 
			ScrClear(_hwndEdit);

		tmpTextorHexaMode = TextorHexaMode;
		memset( string, 0x00, sizeof(string)*sizeof(TCHAR) );
		memset( string2, 0x00, sizeof(string2)*sizeof(TCHAR) );

	}

	// ~~~~~  if( 0!=TextorHexaMode ) return; // EB_PrintfHexa() >>>>>> 2016.10.21 +++++++++++++++++++++++++++++
#endif


	memset( inewline, 0x00, sizeof(inewline) );
	memset( ilinefeed, 0x00, sizeof(ilinefeed) );

	// ** WM_GETTEXTLENGTH
	// Determines the length, in characters, of the text associated with a window. 
	// wParam  : This parameter is not used and must be zero. 
	// lParam  : 	This parameter is not used and must be zero. 
	// Return value : LRESULT
	//    The return value is the length of the text in characters, not including the terminating null character.
	txtRepStart = SendMessage(_hwndEdit,WM_GETTEXTLENGTH,0x0,0x0);
	txtRepEnd   = txtRepStart-1;



	va_start(ap,fmt);
	_vsntprintf(string2,STRING_LEN-1,fmt,ap);
	va_end(ap); //va_end 메크로는 ap 를 단순히 0으로 설정

	string2[STRING_LEN-1]='\0';



#if 0 // HEXA_MODE_SUPPORT	/* 2011.02.08 Hexa mode display */
	if( 1==TextorHexaMode || 2==TextorHexaMode ) /* Hexa mode all (including special chars)  ==> type : FF */
	{
		string[0 /*prevBSCnt*/]='\0';
		lstrcat(string,string2);
		string2[0]='\0';

		slen=lstrlen(string);	
	} 
	else // Text line
#endif
	{

	// -------------------------------------------------
	// -------------------------------------------------
		//for better look of BS(backspace) char.,
		//the BS in the end of the string will be processed next time.
		for(i=0; i<prevBSCnt; i++) //process the previous BS char.
			string[i]='\b';

		string[prevBSCnt]='\0';
		lstrcat(string,string2);
		string2[0]='\0';

		slen=lstrlen(string);
		for(i=0;i<slen;i++)
			if(string[slen-i-1]!='\b')break;

		prevBSCnt=i; // These BSs will be processed next time.
		slen=slen-prevBSCnt;
	// -------------------------------------------------
	// -------------------------------------------------

	}


    if(slen==0)
	{
		isUsed=0;
		return;
	}


#if (TEXT_FIND_MARK) // 2016.09.27
	if( isTxtMark && iLenDataRed>TEXT_LEN_MIN_SIZ ) {
		iMatchedRed = match_s(string2, slen, iLenDataRed, HashRed);
	}
#endif

	for(i=0;i<slen;i++)
	{

	#if 0 // HEXA_MODE_SUPPORT	 ---- /* 2011.02.08 Hexa mode display */
		if( 1==TextorHexaMode || 2==TextorHexaMode ) /* Hexa mode all (including special chars)  ==> type : FF */
		{

			if( 2==TextorHexaMode ) // End Char is 0x81, 0x0D
			{
				string2[str2Pt++] = HexEncodeTable[ (string[i] & 0xF0) >> 4 ]; 
				string2[str2Pt++] = HexEncodeTable[ (string[i] & 0x0F) >> 0 ]; 
				string2[str2Pt++] = TEXT(' '); 
				txtRepEnd += 3;

				// if( szHexaEnd[0] == string[i] )
				if( (HexaEndChar&0x00FF) == string[i] )
				{
					string2[str2Pt++]='\r';
					string2[str2Pt++]='\n';
					txtRepEnd += 2;
				}
				
			}
			else if( 1==TextorHexaMode ) // Hexa Line Number
			{
				if( ColumnNumber && (0==uCount) ) // 2016.04.02
				{
					string2[str2Pt++]='\r';
					string2[str2Pt++]='\n';
					txtRepEnd += 2;
				}

				string2[str2Pt++] = HexEncodeTable[ (string[i] & 0xF0) >> 4 ]; 
				string2[str2Pt++] = HexEncodeTable[ (string[i] & 0x0F) >> 0 ]; 
				string2[str2Pt++] = TEXT(' '); 
				txtRepEnd += 3;
			
				if( ColumnNumber>0 )
				{
					uCount++;
					uCount = uCount % ColumnNumber;
				}
			}
		
			// 2015.11.13, System Time Info display. added
			isTimeInfoOnce = 0; // 2015.11.13
		}
		else /// if (0 == TextorHexaMode) /* Text Mode 2011.02.08 */
	#endif // ----------------------------
		{


		#if 0 // 2020.08.14
			if(1==cmdCRLF) // CR
			{

				if( (string[i]=='\n') || (string[i]=='\r') )
				{
					string2[str2Pt++]=string[i]; txtRepEnd++;
					string2[str2Pt++]='\r';      txtRepEnd++;

					//-- MessageBox( _MainHwnd, TEXT("cmdCRLF!!! - cmdCRLF  1 \n"), "cmdCRLF",MB_OK);

					isTimeInfoOnce = 1; // PC Time 정보를 맨앞에 1번 표시한다. ---
					continue;


				}
			}
			else if(2==cmdCRLF) // LF
			{

				if( (string[i]=='\n') || (string[i]=='\r') )
				{
					string2[str2Pt++]=string[i]; txtRepEnd++;
					string2[str2Pt++]='\n';      txtRepEnd++;

					//-- MessageBox( _MainHwnd, TEXT("cmdCRLF!!! - cmdCRLF - 2\n"), "cmdCRLF",MB_OK);

					isTimeInfoOnce = 1; // PC Time 정보를 맨앞에 1번 표시한다. ---
					continue;


				}
			}
			else
		#endif
			{

				if( (string[i]=='\n'))
				{
					string2[str2Pt++]='\r';txtRepEnd++; inewline[lnidx++] = str2Pt;
					string2[str2Pt++]='\n';txtRepEnd++; 
					wasCr=0;

					// 2015.11.13, System Time Info display. added
					isTimeInfoOnce = 1; // 2015.11.13, PC Time 정보를 맨앞에 1번 표시한다. ---
					continue;
				}

				if( (string[i]!='\n') && (wasCr==1) )
				{
					string2[str2Pt++]='\r';txtRepEnd++; ilinefeed[fdidx++] = str2Pt;
					string2[str2Pt++]='\n';txtRepEnd++; 
					wasCr=0;

					// 2015.11.13, System Time Info display. added
					isTimeInfoOnce = 1; // 2015.11.13, PC Time 정보를 맨앞에 1번 표시한다. ---
				}

				if(string[i]=='\r')
				{
					wasCr=1;
					continue;
				}

			}




			if( string[i]=='\b' ) 
			{
				//flush string2
				if(str2Pt>0)
				{
					string2[--str2Pt]='\0';
					txtRepEnd--;
					continue;
				}
				//str2Pt==0;	    
				if(txtRepStart>0)
				{
					txtRepStart--;		
				}
				continue;
			}



		// -------------------------------------------------------------
		/*--- 2015.11.13, Aging 시 PC의 시간을 알 필요가 있다.	------ */
		// -------------------------------------------------------------
		#if DISPLAY_PC_SYSTEM_TIME /*--- 2016.02.12 ------ */
			if(0==localTime)
			{

		#if (TEXT_FIND_MARK) // 2016.09.23

				if(isTxtMark && isTimeInfoOnce) {
					if(iMatchedRed)
						wsprintf(&string2[str2Pt], MARK_RED );
					else
						wsprintf(&string2[str2Pt], MARK_NULL );

					txtRepEnd += 3; // date and time and char [] length
					str2Pt += 3;

					startPos = 3;
					FrontPos = str2Pt;

				}
		#endif
		
				// 2015.11.13, System Time Info display. added
				isTimeInfoOnce = 0; // Time Inform OFF because of Once
				/// None ---

			}
			else if( isTimeInfoOnce && ( (1==localTime) /*&& (0==TextorHexaMode)*/ ) ) // Time only. 2015..11.13
			{
				isTimeInfoOnce = 0; // Time Inform OFF because of Once

				GetLocalTime( &LocalSysTime );


		#if (TEXT_FIND_MARK) // 2016.09.23
				if(isTxtMark) {
					if(iMatchedRed) {
						wsprintf(&string2[str2Pt], MARK_RED TEXT("[%02d:%02d:%02d.%03d] "), 
							LocalSysTime.wHour, LocalSysTime.wMinute, LocalSysTime.wSecond, LocalSysTime.wMilliseconds);
					}
					else {
						wsprintf(&string2[str2Pt], MARK_NULL TEXT("[%02d:%02d:%02d.%03d] "), 
							LocalSysTime.wHour, LocalSysTime.wMinute, LocalSysTime.wSecond, LocalSysTime.wMilliseconds);
					}
					txtRepEnd += 18; // Time length
					str2Pt += 18;

					startPos = 18;
					FrontPos = str2Pt;

				}
				else 
		#endif
				{
					wsprintf(&string2[str2Pt], TEXT("[%02d:%02d:%02d.%03d] "), 
						LocalSysTime.wHour, LocalSysTime.wMinute, LocalSysTime.wSecond, LocalSysTime.wMilliseconds);

					txtRepEnd += 15; // Time length
					str2Pt += 15;
				}
			}
			else if( isTimeInfoOnce && ( (2==localTime) /* && (0==TextorHexaMode) */ ) ) // Date & Time. 2015..11.13
			{
				isTimeInfoOnce = 0; // Time Inform OFF because of Once
				
				GetLocalTime( &LocalSysTime );

		#if (TEXT_FIND_MARK) // 2016.09.23
				if(isTxtMark) {
					if(iMatchedRed) {
						wsprintf(&string2[str2Pt], MARK_RED TEXT("[%04d/%02d/%02d %02d:%02d:%02d.%03d] "), 
							LocalSysTime.wYear, LocalSysTime.wMonth, LocalSysTime.wDay, LocalSysTime.wHour, 
							LocalSysTime.wMinute, LocalSysTime.wSecond, LocalSysTime.wMilliseconds);
					}
					else {
						wsprintf(&string2[str2Pt], MARK_NULL TEXT("[%04d/%02d/%02d %02d:%02d:%02d.%03d] "), 
							LocalSysTime.wYear, LocalSysTime.wMonth, LocalSysTime.wDay, LocalSysTime.wHour, 
							LocalSysTime.wMinute, LocalSysTime.wSecond, LocalSysTime.wMilliseconds);
					}
					
					txtRepEnd += 29; // date and time and char [] length
					str2Pt += 29;

					startPos = 29;
					FrontPos = str2Pt;

				}
				else 
		#endif
				{
					wsprintf(&string2[str2Pt], TEXT("[%04d/%02d/%02d %02d:%02d:%02d.%03d] "), 
						LocalSysTime.wYear, LocalSysTime.wMonth, LocalSysTime.wDay, LocalSysTime.wHour, 
						LocalSysTime.wMinute, LocalSysTime.wSecond, LocalSysTime.wMilliseconds);

					txtRepEnd += 26; // year/date and time and char [] length
					str2Pt += 26;
				}
			}

		#if MODIFIED_JULIAN_DATE // 2017.08.30
			else if( isTimeInfoOnce && ( (3==localTime) /* && (0==TextorHexaMode) */ ) ) // Modified Julian Date 2017.08.30
			{
				isTimeInfoOnce = 0; // Time Inform OFF because of Once
				
				GetLocalTime( &LocalSysTime );
				memcpy( &mJDTime, &LocalSysTime, sizeof(SYSTEMTIME) );

/* ---------------------------------------------------------- 
				mJDTime.m_year     = 5000; // 2013;
				mJDTime.m_month    = 10; // 9;
				mJDTime.wDayOfWeek = 0;  
				mJDTime.m_day      = 10; // 16;
				mJDTime.m_hour     = 0; // 8;
				mJDTime.m_mins     = 0; // 8;
				mJDTime.m_secs     = 0; // 8;
				mJDTime.m_millis   = 0; // 6;
 ------------------------------------------------------------- */

				f2a( Convert2MJD(mJDTime), mJDtemp, 1000000);

				wsprintf(&string2[str2Pt], TEXT("[%s] "), mJDtemp );

			#if 1
				txtRepEnd += 16; // date and time and char [] length
				str2Pt += 16;
			#else
				imJDLen = lstrlen(mJDtemp); // 2018.07.13

				txtRepEnd += (imJDLen + 3); // modified JD and [ and ] and space 
				str2Pt += (imJDLen + 3);
			#endif
			
				
			}
		#endif // MODIFIED_JULIAN_DATE
			else if( isTimeInfoOnce && ( (4==localTime) /* && (0==TextorHexaMode) */ ) ) // Date & Time & Week
			{
				isTimeInfoOnce = 0; // Time Inform OFF because of Once
				
				GetLocalTime( &LocalSysTime );
				{
					wsprintf(&string2[str2Pt], TEXT("[%04d/%02d/%02d/%s %02d:%02d:%02d.%03d] "), 
						LocalSysTime.wYear, LocalSysTime.wMonth, LocalSysTime.wDay, WeekTXT[LocalSysTime.wDayOfWeek], LocalSysTime.wHour, 
						LocalSysTime.wMinute, LocalSysTime.wSecond, LocalSysTime.wMilliseconds);

					txtRepEnd += 30; // year/date and time and char [] length
					str2Pt += 30;
				}
			}
			else if( isTimeInfoOnce && ( (5==localTime) /* && (0==TextorHexaMode) */ ) ) // LINUX Time Decimal
			{
				unsigned __int64 linuxTime=0UL;
			
				isTimeInfoOnce = 0; // Time Inform OFF because of Once
				
				GetLocalTime( &LocalSysTime );
				memcpy( &LinTime, &LocalSysTime, sizeof(SYSTEMTIME) );

				linuxTime = LinuxDate2Number(LinTime);

				{
					//[   1580630107] [dnw] OS Install Date ---: 2020/06/25 23:01:05 (Linux type:1593180065 0x5ef5ffa1)
					wsprintf(&string2[str2Pt], TEXT("[%11lu] "), linuxTime );

					txtRepEnd += 14; // year/date and time and char [] length
					str2Pt += 14;
				}
			}
			else if( isTimeInfoOnce && ( (6==localTime) /* && (0==TextorHexaMode) */ ) ) // LINUX Time HEXA type
			{
				unsigned __int64 linuxTime=0UL;

				isTimeInfoOnce = 0; // Time Inform OFF because of Once
				
				GetLocalTime( &LocalSysTime );
				memcpy( &LinTime, &LocalSysTime, sizeof(SYSTEMTIME) );

				linuxTime = LinuxDate2Number(LinTime);

				{
					//[   1580630107] [dnw] OS Install Date ---: 2020/06/25 23:01:05 (Linux type:1593180065 0x5ef5ffa1)
					wsprintf(&string2[str2Pt], TEXT("[%10x] "), linuxTime );

					txtRepEnd += 13; // year/date and time and char [] length
					str2Pt += 13;
				}
			}

		#endif /////////////////////////
			// -----------------------------------------------------
			// -----------------------------------------------------


			// Insert string to Buffer for dnw display ---
			string2[str2Pt++]=string[i];
			txtRepEnd++;

		}
		// if(str2Pt>FILENAMELEN-3)break; //why needed? 2001.1.26
	}


	string2[str2Pt]='\0';

//#if (RICHED_COLOR) || (TEXT_FIND_MARK) // 2016.09.23
//	lstrcpy(string0, string2);
//	string0[str2Pt]='\0';
//#endif

	if(str2Pt>0)
	{

#if RICHED_COLOR // 2016.09.19 +++++++++

		RichTxtFind(string2, txtRepStart, txtRepEnd, &ColorStart, &ColorEnded );

		/* EditBox에 텍스트 이어서 추가하기  , EM_SETTEXTEX*/
		SendMessage(_hwndEdit,EM_SETSEL,	 (WPARAM)txtRepStart, (LPARAM)txtRepEnd+1);
		SendMessage(_hwndEdit,EM_REPLACESEL, (WPARAM)FALSE, 	  (LPARAM)((LPSTR)string2) ); 


		if( txtColorRed && txtFindRed ) {
			RichTxtColor(ColorStart, ColorEnded, 111, COLOR_RED);
		}

		if( txtColorMegenta && txtFindMegenta ) {
			RichTxtColor(ColorStart, ColorEnded, 111, COLOR_MAGENTA);
		}

		if( txtColorBlue && txtFindBlue ) {
			RichTxtColor(ColorStart, ColorEnded, 111, COLOR_BLUE);
		}
		

		if( txtColorGreen && txtFindGreen ) {
			RichTxtColor(ColorStart, ColorEnded, 111, COLOR_GREEN2);
		}

		if( txtColorYellow && txtFindYellow ) {
			RichTxtColor(ColorStart, ColorEnded, 111, COLOR_YELLOW2);
		}

#else ///////////////////////////////////////

	#if TEXT_FIND_MARK // 2016.09.23

//		if(isTxtMark) {
//			RichTxtFindMarking(string2, txtRepStart, txtRepEnd, 0/*FrontPos*/, startPos);
//		}

	#endif

		/* EditBox에 텍스트 이어서 추가하기  , EM_SETTEXTEX*/
		SendMessage(_hwndEdit,EM_SETSEL,	 (WPARAM)txtRepStart, (LPARAM)txtRepEnd+1);
		SendMessage(_hwndEdit,EM_REPLACESEL, (WPARAM)FALSE, 	  (LPARAM)((LPSTR)string2) ); 


#endif


	#if LOG_SAVE_BY_THREAD // 2016.02.25
		/* log file save - thread을 이용한 성능 개선 -- */
		if( msgSaveOnOff )
		{
			if( fFileWrite ) 
			{
				uintptr_t threadResult;

				qindex = (qindex+1) % FILE_WRITE_QUEUE_SIZE;
				lstrcpy(buf2write[qindex], string2);
				buf2write[qindex][str2Pt]='\0';

				threadResult = _beginthread( (void (*)(void *))ThreadLogSaveFunc, THREAD_STACK_SIZE, buf2write[qindex] );
				if(threadResult != -1)
				{
					// OK
					// avoid loss the ThreadLogSaveFunc() function and buf2write[qindex] buffer.
					Sleep(10); 
				}
				else
				{
					EB_Printf("[dnw] can not create ThreadLogSaveFunc thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
				}	
		
			}
		}
	
	#else // ------------------------------------------
	
		/* log file save */
		if( msgSaveOnOff )
		{
			if( fFileWrite ) // && buff_fin )
			{
				int ii=0;
				// SetFilePointer(hFile, 0, NULL, FILE_END);			
				// fseek(fFileWrite, 0L, SEEK_END); // 2016.02.28 end of file pointer 

				// original, 2020.08.13
				if(lnidx>=LINE_MAX) EB_Printf("[dnw] newline over!! (%d) \r\n", lnidx);
				else
				{
					for( ii=lnidx-1; ii>=0; ii-- ) 
					{
						if( inewline[ii] && (ii>=0 && ii<LINE_MAX) ) string2[ inewline[ii]-1 ] = ' ';
					}
				}
			
				fwrite(string2, sizeof(TCHAR), str2Pt, fFileWrite); 

		#if !LOG_SAVE_BY_TIMER  // 2016.02.24,  
				/* 2012.07.07 fflush() 사용으로 변경.... 출력버퍼는 즉시 처리가 된다... flclose() 대신 사용 가능 */
				/// NEVER use-- 2016.02.26 -- fflush(fFileWrite); 
		#endif
			}
		}

	#endif // LOG_SAVE_BY_THREAD !! ------------------------

	}
	else
	{

		if(txtRepStart<=txtRepEnd)
		{

			SendMessage(_hwndEdit,EM_SETSEL,     (WPARAM)txtRepStart, (LPARAM)txtRepEnd+1);
			SendMessage(_hwndEdit,EM_REPLACESEL, (WPARAM)0,           (LPARAM)"");

		}
	}




#if USER_SCREEN_BUF_SIZE // 2016.02.14

	if( tempScrBufOnce != userScrBufSizIndex )
	{
		tempScrBufOnce = userScrBufSizIndex;

		switch( userScrBufSizIndex )
		{
			case 0: 
				dwBufferCheckSize = HUGE_EDIT_BUF_SIZE; 
				dwBufferEraseSize = HUGE_EDIT_BUF_DEC_SIZE;
				//MessageBox(_hwndEdit,TEXT("PAUSE !000!! \n"),TEXT("PAUSE !000!! \n"), MB_OK | MB_ICONINFORMATION );
				break;
			case 1: 
				dwBufferCheckSize = LARGE_EDIT_BUF_SIZE; 
				dwBufferEraseSize = LARGE_EDIT_BUF_DEC_SIZE;
				//MessageBox(_hwndEdit,TEXT("PAUSE !111!! \n"),TEXT("PAUSE !111!! \n"), MB_OK | MB_ICONINFORMATION );
				break;
			case 2: 
				dwBufferCheckSize = MIDDLE_EDIT_BUF_SIZE; 
				dwBufferEraseSize = MIDDLE_EDIT_BUF_DEC_SIZE;
				//MessageBox(_hwndEdit,TEXT("PAUSE !121!! \n"),TEXT("PAUSE !121!! \n"), MB_OK | MB_ICONINFORMATION );
				break;
			case 3: 
				dwBufferCheckSize = SMALL_EDIT_BUF_SIZE; 
				dwBufferEraseSize = SMALL_EDIT_BUF_DEC_SIZE;
				//MessageBox(_hwndEdit,TEXT("PAUSE !222!! \n"),TEXT("PAUSE !222!! \n"), MB_OK | MB_ICONINFORMATION );
				break;
			case 4: 
				dwBufferCheckSize = SMALLEST_EDIT_BUF_SIZE; 
				dwBufferEraseSize = SMALLEST_EDIT_BUF_DEC_SIZE;
				//MessageBox(_hwndEdit,TEXT("PAUSE !333!! \n"),TEXT("PAUSE !333!! \n"), MB_OK | MB_ICONINFORMATION );
				break; // Smallest
			default: 
				dwBufferCheckSize = HUGE_EDIT_BUF_SIZE; 
				dwBufferEraseSize = HUGE_EDIT_BUF_DEC_SIZE;
				//MessageBox(_hwndEdit,TEXT("PAUSE !DEFAULT !! \n"),TEXT("PAUSE !DEFAULT!! \n"), MB_OK | MB_ICONINFORMATION );
				break;
		}
	}



	if( txtRepEnd > (int)dwBufferCheckSize )

#else

	//If edit buffer is over EDIT_BUF_SIZE,
	//the size of buffer must be decreased by EDIT_BUF_DEC_SIZE.
	if(txtRepEnd > EDIT_BUF_SIZE)
#endif
	{

		// ** EM_LINEFROMCHAR 
		// Gets the index of the line that contains the specified character index in a multiline edit control. 
		// A character index is the zero-based index of the character from the beginning of the edit control. 
		// You can send this message to either an edit control or a rich edit control.
		// Parameters
		// wParam : The character index of the character contained in the line whose number is to be retrieved. 
		//           If this parameter is -1, EM_LINEFROMCHAR retrieves either the line number of the current line 
		//			(the line containing the caret) or, 
		//           if there is a selection, the line number of the line containing the beginning of the selection. 
		// lParam  : 	This parameter is not used. 
		// The return value is the zero-based line number of the line containing the character index specified by wParam. 
		//
		// ** EM_LINEINDEX message
		// Gets the character index of the first character of a specified line in a multiline edit control. 
		// A character index is the zero-based index of the character from the beginning of the edit control. 
		// You can send this message to either an edit control or a rich edit control.
		// Parameters
		// wParam : The zero-based line number. 
		//               A value of -1 specifies the current line number (the line that contains the caret). 
		// lParam  : 	This parameter is not used. 
		// The return value is the character index of the line specified in the wParam parameter, 
		// or it is -1 if the specified line number is greater than the number of lines in the edit control.


	#if USER_SCREEN_BUF_SIZE // 2016.02.14
		lineIdx=SendMessage(_hwndEdit,EM_LINEFROMCHAR,dwBufferEraseSize,0x0);
	#else
		lineIdx=SendMessage(_hwndEdit,EM_LINEFROMCHAR,EDIT_BUF_DEC_SIZE,0x0);
	#endif

		//lineIdx=SendMessage(_hwndEdit,EM_LINEFROMCHAR,txtRepEnd-txtRepStart+1,0x0); //for debug
		txtSelEnd=SendMessage(_hwndEdit,EM_LINEINDEX,lineIdx,0x0)-1;

		SendMessage(_hwndEdit,EM_SETSEL,0,txtSelEnd+1);
		SendMessage(_hwndEdit,EM_REPLACESEL,0,(LPARAM)"");

		//SendMessage(_hwndEdit,WM_CLEAR,0,0); //WM_CLEAR doesn't work? Why?
		//SendMessage(_hwndEdit,WM_CUT,0,0); //WM_CUT doesn't work? Why?

		#if 0 // def __TEST_VIEW__
		//EB_Printf(TEXT("[dnw] lineIdx(%d) ---txtSelEnd(%d) \n"),  lineIdx, txtSelEnd );
		MessageBox(_hwndEdit,TEXT("PAUSE !212!! \n"),TEXT("PAUSE !212!! \n"), MB_OK | MB_ICONINFORMATION );
		#endif


		//make the end of the text shown.
		txtRepEnd=SendMessage(_hwndEdit,WM_GETTEXTLENGTH,0x0,0x0)-1;

	#if 0 // 2016.02.12
		SendMessage(_hwndEdit,EM_SETSEL,txtRepEnd+1,txtRepEnd+2); 
		SendMessage(_hwndEdit,EM_REPLACESEL,0,(LPARAM)" ");
	#endif

		SendMessage(_hwndEdit,EM_SETSEL,txtRepEnd+1,txtRepEnd+2); 
		SendMessage(_hwndEdit,EM_REPLACESEL,0,(LPARAM)"");


	#if DISPLAY_MAX_EDIT_BUF_SIZE // 2016.02.12
		iEditCount++;
		iLineDeleted = lineIdx;
	#endif


	}

	isUsed=0;


	//memset( string, 0x00, sizeof(string)*sizeof(TCHAR) ); // added 2016.09.23 
	//memset( string2, 0x00, sizeof(string2)*sizeof(TCHAR) ); // added 2016.09.23 

}



void EB_PrintfText(int iLength, TCHAR *fmtData) // ,...)
{
	// va_list ap;
	int i,slen;
	int lineIdx;
	int txtRepStart,txtRepEnd,txtSelEnd;
	static int wasCr=0; //should be static type.
	static TCHAR string[STRING_LEN +1 /*+4096*/] = {0,}; //margin for '\b'
	static TCHAR string2[STRING_LEN +1 /*+4096*/]= {0,}; //margin for '\n'->'\r\n'
	static int prevBSCnt=0;

#if (RICHED_COLOR) || (TEXT_FIND_MARK) // 2016.09.23
	DWORD ColorStart=0, ColorEnded=0;
	int startPos=0;
	int FrontPos=0;
	int iMatchedRed=0;
#endif

	int str2Pt=0;
	
#if HEXA_MODE_SUPPORT
	static int tmpTextorHexaMode=0xff;
#endif
	static int tempScrBufOnce=0xff;

#if MODIFIED_JULIAN_DATE // 2017.08.30
	mjd_timestamp mJDTime; 
	TCHAR         mJDtemp[20];
	int 		  imJDLen=0; // 2018.07.13
#endif


#ifdef _DELETE_FUNC_ /* -- 2013.04.20 -- */
	while( 1 == isUsed )
	{
		; //EB_Printf can be called multiplely  //KIW
	}
#endif


#if 0 // 2017.03.27, HEXA_MODE_SUPPORT

	if( tmpTextorHexaMode != TextorHexaMode ) // Hexa mode (1 or 2) <-> Text Mode (0)
	{
		if( tmpTextorHexaMode && TextorHexaMode ) 
		{
			// No Screen clear
		}
		else 
			ScrClear(_hwndEdit);

		tmpTextorHexaMode = TextorHexaMode;
		memset( string, 0x00, sizeof(string)*sizeof(TCHAR) );
		memset( string2, 0x00, sizeof(string2)*sizeof(TCHAR) );

	}

#endif

	// ** WM_GETTEXTLENGTH
	// Determines the length, in characters, of the text associated with a window. 
	// wParam  : This parameter is not used and must be zero. 
	// lParam  : 	This parameter is not used and must be zero. 
	// Return value : LRESULT
	//    The return value is the length of the text in characters, not including the terminating null character.
	txtRepStart = SendMessage(_hwndEdit,WM_GETTEXTLENGTH,0x0,0x0);
	txtRepEnd   = txtRepStart-1;


#if 0 // USE_RX_TX_QUEUE
	
	// DeQueue 하기 전에 Queue empty 확인 ---
	if( EmptyCQueue(&Rx2Tx) )
	{
		isUsed=0;
		return;
	}
	else
	{
		fmt = DeCQueue(&Rx2Tx);
	}
#endif


#if 0 // 2016.10.24
	va_start(ap,fmt);
	_vsntprintf(string2,STRING_LEN-1,fmt,ap);
	va_end(ap); //va_end 메크로는 ap 를 단순히 0으로 설정

#else

	if( iLength )
		memcpy(string2, fmtData, iLength);
#endif


	string2[STRING_LEN-1]='\0';



#if 0 // HEXA_MODE_SUPPORT	/* 2011.02.08 Hexa mode display */
	if( 1==TextorHexaMode || 2==TextorHexaMode ) /* Hexa mode all (including special chars)  ==> type : FF */
	{
		string[0 /*prevBSCnt*/]='\0';
		lstrcat(string,string2);
		string2[0]='\0';

		slen=lstrlen(string);	
	} 
	else // Text line
#endif
	{

	// -------------------------------------------------
	// -------------------------------------------------
		//for better look of BS(backspace) char.,
		//the BS in the end of the string will be processed next time.
		for(i=0; i<prevBSCnt; i++) //process the previous BS char.
			string[i]='\b';

		string[prevBSCnt]='\0';

	#if 0 // 2016.10.24
		lstrcat(string,string2);
	#else
		memcpy(&string[prevBSCnt], string2, iLength); // 2016.10.24
	#endif
	
		string2[0]='\0';

		slen=lstrlen(string);
		for(i=0;i<slen;i++)
			if(string[slen-i-1]!='\b')break;

		prevBSCnt=i; // These BSs will be processed next time.
		slen=slen-prevBSCnt;
	// -------------------------------------------------
	// -------------------------------------------------

	}


    if(/*slen==0 ||*/ 0==iLength)
	{
		isUsed=0;
		return;
	}


#if (TEXT_FIND_MARK) // 2016.09.27
	if( isTxtMark && iLenDataRed>TEXT_LEN_MIN_SIZ ) {
		iMatchedRed = match_s(string2, slen, iLenDataRed, HashRed);
	}
#endif

	for(i=0; /*i<slen*/ i<iLength; i++)
	{

	#if 0 // HEXA_MODE_SUPPORT	/* 2011.02.08 Hexa mode display */
		if( 1==TextorHexaMode || 2==TextorHexaMode ) /* Hexa mode all (including special chars)  ==> type : FF */
		{

			if( 2==TextorHexaMode ) // End Char is 0x81, 0x0D
			{
				string2[str2Pt++] = HexEncodeTable[ (string[i] & 0xF0) >> 4 ]; 
				string2[str2Pt++] = HexEncodeTable[ (string[i] & 0x0F) >> 0 ]; 
				string2[str2Pt++] = TEXT(' '); 
				txtRepEnd += 3;

				// if( szHexaEnd[0] == string[i] )
				if( (HexaEndChar&0x00FF) == string[i] )
				{
					string2[str2Pt++]='\r';
					string2[str2Pt++]='\n';
					txtRepEnd += 2;
				}
				
			}
			else if( 1==TextorHexaMode ) // Hexa Line Number
			{
				if( ColumnNumber && (0==uCount) ) // 2016.04.02
				{
					string2[str2Pt++]='\r';
					string2[str2Pt++]='\n';
					txtRepEnd += 2;
				}

				string2[str2Pt++] = HexEncodeTable[ (string[i] & 0xF0) >> 4 ]; 
				string2[str2Pt++] = HexEncodeTable[ (string[i] & 0x0F) >> 0 ]; 
				string2[str2Pt++] = TEXT(' '); 
				txtRepEnd += 3;
			
				if( ColumnNumber>0 )
				{
					uCount++;
					uCount = uCount % ColumnNumber;
				}
			}
		
			// 2015.11.13, System Time Info display. added
			isTimeInfoOnce = 0; // 2015.11.13
		}
		else /// if (0 == TextorHexaMode) /* Text Mode 2011.02.08 */
	#endif // ----------------------------
		{

			if( (string[i]=='\n') || (string[i]=='\0') )
			{
				string2[str2Pt++]='\r';txtRepEnd++;
				string2[str2Pt++]='\n';txtRepEnd++;
				wasCr=0;

				// 2015.11.13, System Time Info display. added
				isTimeInfoOnce = 1; // 2015.11.13, PC Time 정보를 맨앞에 1번 표시한다. ---
				continue;
			}

			if( (string[i]!='\n') && (wasCr==1) )
			{
				string2[str2Pt++]='\r';txtRepEnd++;
				string2[str2Pt++]='\n';txtRepEnd++;
				wasCr=0;

				// 2015.11.13, System Time Info display. added
				isTimeInfoOnce = 1; // 2015.11.13, PC Time 정보를 맨앞에 1번 표시한다. ---
			}

			if(string[i]=='\r')
			{
				wasCr=1;
				continue;
			}

			if( string[i]=='\b' ) 
			{
				//flush string2
				if(str2Pt>0)
				{
					string2[--str2Pt]='\0';
					txtRepEnd--;
					continue;
				}
				//str2Pt==0;	    
				if(txtRepStart>0)
				{
					txtRepStart--;		
				}
				continue;
			}



		// -------------------------------------------------------------
		/*--- 2015.11.13, Aging 시 PC의 시간을 알 필요가 있다.	------ */
		// -------------------------------------------------------------
		#if DISPLAY_PC_SYSTEM_TIME /*--- 2016.02.12 ------ */
			if(0==localTime)
			{

		#if (TEXT_FIND_MARK) // 2016.09.23

				if(isTxtMark && isTimeInfoOnce) {
					if(iMatchedRed)
						wsprintf(&string2[str2Pt], MARK_RED );
					else
						wsprintf(&string2[str2Pt], MARK_NULL );

					txtRepEnd += 3; // date and time and char [] length
					str2Pt += 3;

					startPos = 3;
					FrontPos = str2Pt;

				}
		#endif
		
				// 2015.11.13, System Time Info display. added
				isTimeInfoOnce = 0; // Time Inform OFF because of Once
				/// None ---

			}
			else if( isTimeInfoOnce && ( (1==localTime) && (0==TextorHexaMode) ) ) // Time only. 2015..11.13
			{
				isTimeInfoOnce = 0; // Time Inform OFF because of Once

				GetLocalTime( &LocalSysTime );


		#if (TEXT_FIND_MARK) // 2016.09.23
				if(isTxtMark) {
					if(iMatchedRed) {
						wsprintf(&string2[str2Pt], MARK_RED TEXT("[%02d:%02d:%02d.%03d] "), 
							LocalSysTime.wHour, LocalSysTime.wMinute, LocalSysTime.wSecond, LocalSysTime.wMilliseconds);
					}
					else {
						wsprintf(&string2[str2Pt], MARK_NULL TEXT("[%02d:%02d:%02d.%03d] "), 
							LocalSysTime.wHour, LocalSysTime.wMinute, LocalSysTime.wSecond, LocalSysTime.wMilliseconds);
					}
					txtRepEnd += 18; // Time length
					str2Pt += 18;

					startPos = 18;
					FrontPos = str2Pt;

				}
				else 
		#endif
				{
					wsprintf(&string2[str2Pt], TEXT("[%02d:%02d:%02d.%03d] "), 
						LocalSysTime.wHour, LocalSysTime.wMinute, LocalSysTime.wSecond, LocalSysTime.wMilliseconds);

					txtRepEnd += 15; // Time length
					str2Pt += 15;
				}
			}
			else if( isTimeInfoOnce && ( (2==localTime) && (0==TextorHexaMode) ) ) // Date & Time. 2015..11.13
			{
				isTimeInfoOnce = 0; // Time Inform OFF because of Once
				
				GetLocalTime( &LocalSysTime );

		#if (TEXT_FIND_MARK) // 2016.09.23
				if(isTxtMark) {
					if(iMatchedRed) {
						wsprintf(&string2[str2Pt], MARK_RED TEXT("[%04d/%02d/%02d %02d:%02d:%02d.%03d] "), 
							LocalSysTime.wYear, LocalSysTime.wMonth, LocalSysTime.wDay, LocalSysTime.wHour, 
							LocalSysTime.wMinute, LocalSysTime.wSecond, LocalSysTime.wMilliseconds);
					}
					else {
						wsprintf(&string2[str2Pt], MARK_NULL TEXT("[%04d/%02d/%02d %02d:%02d:%02d.%03d] "), 
							LocalSysTime.wYear, LocalSysTime.wMonth, LocalSysTime.wDay, LocalSysTime.wHour, 
							LocalSysTime.wMinute, LocalSysTime.wSecond, LocalSysTime.wMilliseconds);
					}
					
					txtRepEnd += 29; // date and time and char [] length
					str2Pt += 29;

					startPos = 29;
					FrontPos = str2Pt;

				}
				else 
		#endif
				{
					wsprintf(&string2[str2Pt], TEXT("[%04d/%02d/%02d %02d:%02d:%02d.%03d] "), 
						LocalSysTime.wYear, LocalSysTime.wMonth, LocalSysTime.wDay, LocalSysTime.wHour, 
						LocalSysTime.wMinute, LocalSysTime.wSecond, LocalSysTime.wMilliseconds);

					txtRepEnd += 26; // date and time and char [] length
					str2Pt += 26;
				}
			}
		#if MODIFIED_JULIAN_DATE // 2017.08.30
			else if( isTimeInfoOnce && ( (3==localTime) && (0==TextorHexaMode) ) ) // Modified Julian Date 2017.09.01
			{
				isTimeInfoOnce = 0; // Time Inform OFF because of Once
				
				GetLocalTime( &LocalSysTime );
				memcpy( &mJDTime, &LocalSysTime, sizeof(SYSTEMTIME) );
				f2a( Convert2MJD(mJDTime), mJDtemp, 1000000);

				imJDLen = lstrlen(mJDtemp); // 2018.07.13

				wsprintf(&string2[str2Pt], TEXT("[%s] "), mJDtemp );
				txtRepEnd += 16; // date and time and char [] length
				str2Pt += 16;
				
			}
		#endif // MODIFIED_JULIAN_DATE

		#endif /////////////////////////
			// -----------------------------------------------------
			// -----------------------------------------------------


			// Insert string to Buffer for dnw display ---
			string2[str2Pt++]=string[i];
			txtRepEnd++;

		}
		// if(str2Pt>FILENAMELEN-3)break; //why needed? 2001.1.26
	}


	string2[str2Pt]='\0';

//#if (RICHED_COLOR) || (TEXT_FIND_MARK) // 2016.09.23
//	lstrcpy(string0, string2);
//	string0[str2Pt]='\0';
//#endif

	if(str2Pt>0)
	{

#if RICHED_COLOR // 2016.09.19 +++++++++

		RichTxtFind(string2, txtRepStart, txtRepEnd, &ColorStart, &ColorEnded );

		/* EditBox에 텍스트 이어서 추가하기  , EM_SETTEXTEX*/
		SendMessage(_hwndEdit,EM_SETSEL,	 (WPARAM)txtRepStart, (LPARAM)txtRepEnd+1);
		SendMessage(_hwndEdit,EM_REPLACESEL, (WPARAM)FALSE, 	  (LPARAM)((LPSTR)string2) ); 


		if( txtColorRed && txtFindRed ) {
			RichTxtColor(ColorStart, ColorEnded, 111, COLOR_RED);
		}

		if( txtColorMegenta && txtFindMegenta ) {
			RichTxtColor(ColorStart, ColorEnded, 111, COLOR_MAGENTA);
		}

		if( txtColorBlue && txtFindBlue ) {
			RichTxtColor(ColorStart, ColorEnded, 111, COLOR_BLUE);
		}
		

		if( txtColorGreen && txtFindGreen ) {
			RichTxtColor(ColorStart, ColorEnded, 111, COLOR_GREEN2);
		}

		if( txtColorYellow && txtFindYellow ) {
			RichTxtColor(ColorStart, ColorEnded, 111, COLOR_YELLOW2);
		}

#else ///////////////////////////////////////

	#if TEXT_FIND_MARK // 2016.09.23

//		if(isTxtMark) {
//			RichTxtFindMarking(string2, txtRepStart, txtRepEnd, 0/*FrontPos*/, startPos);
//		}

	#endif

		/* EditBox에 텍스트 이어서 추가하기  , EM_SETTEXTEX*/
		SendMessage(_hwndEdit,EM_SETSEL,	 (WPARAM)txtRepStart, (LPARAM)txtRepEnd+1);
		SendMessage(_hwndEdit,EM_REPLACESEL, (WPARAM)FALSE, 	  (LPARAM)((LPSTR)string2) ); 


#endif


	#if LOG_SAVE_BY_THREAD // 2016.02.25
		/* log file save - thread을 이용한 성능 개선 -- */
		if( msgSaveOnOff )
		{
			if( fFileWrite ) 
			{
				uintptr_t threadResult;

				qindex = (qindex+1) % FILE_WRITE_QUEUE_SIZE;
				lstrcpy(buf2write[qindex], string2);
				buf2write[qindex][str2Pt]='\0';

				threadResult = _beginthread( (void (*)(void *))ThreadLogSaveFunc, THREAD_STACK_SIZE, buf2write[qindex] );
				if(threadResult != -1)
				{
					// OK
					// avoid loss the ThreadLogSaveFunc() function and buf2write[qindex] buffer.
					Sleep(10); 
				}
				else
				{
					EB_Printf("[dnw] can not create ThreadLogSaveFunc thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
				}	
		
			}
		}
	
	#else // ------------------------------------------
	
		/* log file save */
		if( msgSaveOnOff )
		{
			if( fFileWrite ) // && buff_fin )
			{
				// SetFilePointer(hFile, 0, NULL, FILE_END);			
				// fseek(fFileWrite, 0L, SEEK_END); // 2016.02.28 end of file pointer 

				// original
				fwrite(string2, sizeof(TCHAR), str2Pt, fFileWrite); 

		#if !LOG_SAVE_BY_TIMER  // 2016.02.24,  
				/* 2012.07.07 fflush() 사용으로 변경.... 출력버퍼는 즉시 처리가 된다... flclose() 대신 사용 가능 */
				/// NEVER use-- 2016.02.26 -- fflush(fFileWrite); 
		#endif
			}
		}

	#endif // LOG_SAVE_BY_THREAD !! ------------------------

	}
	else
	{

		if(txtRepStart<=txtRepEnd)
		{

			SendMessage(_hwndEdit,EM_SETSEL,     (WPARAM)txtRepStart, (LPARAM)txtRepEnd+1);
			SendMessage(_hwndEdit,EM_REPLACESEL, (WPARAM)0,           (LPARAM)"");

		}
	}




#if USER_SCREEN_BUF_SIZE // 2016.02.14

	if( tempScrBufOnce != userScrBufSizIndex )
	{
		tempScrBufOnce = userScrBufSizIndex;

		switch( userScrBufSizIndex )
		{
			case 0: 
				dwBufferCheckSize = HUGE_EDIT_BUF_SIZE; 
				dwBufferEraseSize = HUGE_EDIT_BUF_DEC_SIZE;
				//MessageBox(_hwndEdit,TEXT("PAUSE !000!! \n"),TEXT("PAUSE !000!! \n"), MB_OK | MB_ICONINFORMATION );
				break;
			case 1: 
				dwBufferCheckSize = LARGE_EDIT_BUF_SIZE; 
				dwBufferEraseSize = LARGE_EDIT_BUF_DEC_SIZE;
				//MessageBox(_hwndEdit,TEXT("PAUSE !111!! \n"),TEXT("PAUSE !111!! \n"), MB_OK | MB_ICONINFORMATION );
				break;
			case 2: 
				dwBufferCheckSize = MIDDLE_EDIT_BUF_SIZE; 
				dwBufferEraseSize = MIDDLE_EDIT_BUF_DEC_SIZE;
				//MessageBox(_hwndEdit,TEXT("PAUSE !121!! \n"),TEXT("PAUSE !121!! \n"), MB_OK | MB_ICONINFORMATION );
				break;
			case 3: 
				dwBufferCheckSize = SMALL_EDIT_BUF_SIZE; 
				dwBufferEraseSize = SMALL_EDIT_BUF_DEC_SIZE;
				//MessageBox(_hwndEdit,TEXT("PAUSE !222!! \n"),TEXT("PAUSE !222!! \n"), MB_OK | MB_ICONINFORMATION );
				break;
			case 4: 
				dwBufferCheckSize = SMALLEST_EDIT_BUF_SIZE; 
				dwBufferEraseSize = SMALLEST_EDIT_BUF_DEC_SIZE;
				//MessageBox(_hwndEdit,TEXT("PAUSE !333!! \n"),TEXT("PAUSE !333!! \n"), MB_OK | MB_ICONINFORMATION );
				break; // Smallest
			default: 
				dwBufferCheckSize = HUGE_EDIT_BUF_SIZE; 
				dwBufferEraseSize = HUGE_EDIT_BUF_DEC_SIZE;
				//MessageBox(_hwndEdit,TEXT("PAUSE !DEFAULT !! \n"),TEXT("PAUSE !DEFAULT!! \n"), MB_OK | MB_ICONINFORMATION );
				break;
		}
	}



	if( txtRepEnd > (int)dwBufferCheckSize )

#else

	//If edit buffer is over EDIT_BUF_SIZE,
	//the size of buffer must be decreased by EDIT_BUF_DEC_SIZE.
	if(txtRepEnd > EDIT_BUF_SIZE)
#endif
	{

		// ** EM_LINEFROMCHAR 
		// Gets the index of the line that contains the specified character index in a multiline edit control. 
		// A character index is the zero-based index of the character from the beginning of the edit control. 
		// You can send this message to either an edit control or a rich edit control.
		// Parameters
		// wParam : The character index of the character contained in the line whose number is to be retrieved. 
		//           If this parameter is -1, EM_LINEFROMCHAR retrieves either the line number of the current line 
		//			(the line containing the caret) or, 
		//           if there is a selection, the line number of the line containing the beginning of the selection. 
		// lParam  : 	This parameter is not used. 
		// The return value is the zero-based line number of the line containing the character index specified by wParam. 
		//
		// ** EM_LINEINDEX message
		// Gets the character index of the first character of a specified line in a multiline edit control. 
		// A character index is the zero-based index of the character from the beginning of the edit control. 
		// You can send this message to either an edit control or a rich edit control.
		// Parameters
		// wParam : The zero-based line number. 
		//               A value of -1 specifies the current line number (the line that contains the caret). 
		// lParam  : 	This parameter is not used. 
		// The return value is the character index of the line specified in the wParam parameter, 
		// or it is -1 if the specified line number is greater than the number of lines in the edit control.


	#if USER_SCREEN_BUF_SIZE // 2016.02.14
		lineIdx=SendMessage(_hwndEdit,EM_LINEFROMCHAR,dwBufferEraseSize,0x0);
	#else
		lineIdx=SendMessage(_hwndEdit,EM_LINEFROMCHAR,EDIT_BUF_DEC_SIZE,0x0);
	#endif

		//lineIdx=SendMessage(_hwndEdit,EM_LINEFROMCHAR,txtRepEnd-txtRepStart+1,0x0); //for debug
		txtSelEnd=SendMessage(_hwndEdit,EM_LINEINDEX,lineIdx,0x0)-1;

		SendMessage(_hwndEdit,EM_SETSEL,0,txtSelEnd+1);
		SendMessage(_hwndEdit,EM_REPLACESEL,0,(LPARAM)"");

		//SendMessage(_hwndEdit,WM_CLEAR,0,0); //WM_CLEAR doesn't work? Why?
		//SendMessage(_hwndEdit,WM_CUT,0,0); //WM_CUT doesn't work? Why?

		#if 0 // def __TEST_VIEW__
		//EB_Printf(TEXT("[dnw] lineIdx(%d) ---txtSelEnd(%d) \n"),  lineIdx, txtSelEnd );
		MessageBox(_hwndEdit,TEXT("PAUSE !212!! \n"),TEXT("PAUSE !212!! \n"), MB_OK | MB_ICONINFORMATION );
		#endif


		//make the end of the text shown.
		txtRepEnd=SendMessage(_hwndEdit,WM_GETTEXTLENGTH,0x0,0x0)-1;

	#if 0 // 2016.02.12
		SendMessage(_hwndEdit,EM_SETSEL,txtRepEnd+1,txtRepEnd+2); 
		SendMessage(_hwndEdit,EM_REPLACESEL,0,(LPARAM)" ");
	#endif

		SendMessage(_hwndEdit,EM_SETSEL,txtRepEnd+1,txtRepEnd+2); 
		SendMessage(_hwndEdit,EM_REPLACESEL,0,(LPARAM)"");


	#if DISPLAY_MAX_EDIT_BUF_SIZE // 2016.02.12
		iEditCount++;
		iLineDeleted = lineIdx;
	#endif


	}

	isUsed=0;


	//memset( string, 0x00, sizeof(string)*sizeof(TCHAR) ); // added 2016.09.23 
	//memset( string2, 0x00, sizeof(string2)*sizeof(TCHAR) ); // added 2016.09.23 

}



void EditBufferClear(HWND hwnd)
{
	// ** EM_SETLIMITTEXT message
	// Sets the text limit of an edit control. 
	// The text limit is the maximum amount of text, in TCHARs, that the user can type into the edit control. 
	// You can send this message to either an edit control or a rich edit control.
	// For edit controls and Microsoft Rich Edit 1.0, bytes are used. For Microsoft Rich Edit 2.0 and later, characters are used.
	// The EM_SETLIMITTEXT message is identical to the EM_LIMITTEXT message. 
	// Parameters
	// wParam  : The maximum number of TCHARs the user can enter. 
	//  For ANSI text, this is the number of bytes; for Unicode text, this is the number of characters. 
	//  This number does not include the terminating null character. 
	//  Rich edit controls: If this parameter is zero, the text length is set to 64,000 characters. 
	//  If this parameter is zero, the text length is set to 0x7FFFFFFE characters for single-line edit controls 
	//     or  -1 for multiline edit controls. 
	// lParam  : 	This parameter is not used. 
	// Return value : This message does not return a value. 


	SendMessage(_hwndEdit, EM_SETLIMITTEXT, MIN_EDIT_BUF_SIZE, 0L);
}



LRESULT GetEditBufferSize(HWND hwnd)
{
	// ** EM_GETLIMITTEXT message
	// Gets the current text limit for an edit control. You can send this message to either an edit control or a rich edit control.
	// Parameters
	// wParam  : Not used; must be zero. 
	// lParam  : Not used; must be zero. 
	// Return value
	// The return value is the text limit.
	LRESULT iTextSize;

	iTextSize = SendMessage(_hwndEdit, EM_GETLIMITTEXT, 0L, 0L);

	// char *szText = new char[iTextSize];
	// SendMessage(hwndText, WM_GETTEXT, iTextSize, (LPARAM)szText);
	return iTextSize;
}




char pDecimal[MAX_PATH] = {0,};

//----------------------------------------------
//----------------------------------------------
char *PRTComma(long long deci)
{
	memset( pDecimal, 0x00, sizeof(pDecimal)*sizeof(char) );
	
	if(deci<1000UL) 
	{
		sprintf(pDecimal, "%d", (int)deci );
	}
	else if(deci<1000000UL) 
	{
		sprintf(pDecimal, "%ld,%03d", (int)(deci/1000), (int)(deci%1000) );
	}
	else if(deci<1000000000UL) 
	{
		sprintf(pDecimal, "%ld,%03d,%03d", (int)deci/1000000, (int)(deci%1000000)/1000, (int)(deci%1000000)%1000 );
	}
	else if(deci<1000000000000UL) 
	{
		sprintf(pDecimal, "%ld,%03ld,%03d,%03d", 
			(int)(deci/1000000000UL), (int)((deci%1000000000UL)/1000000), (int)((deci%1000000)/1000), (deci%1000000)%1000 );
	}
	else if(deci<1000000000000000UL) 
	{
		sprintf(pDecimal, "%ld,%03d,%03d,%03d,%03d", 
			(int)(deci/1000000000000UL), (int)(deci%1000000000000UL)/1000000000UL, (int)((deci%1000000000UL)/1000000), (int)((deci%1000000)/1000), (deci%1000000)%1000 );
	}
	else
	{
		sprintf(pDecimal, "%I64d", deci );
	}

	return pDecimal;
}





//----------------------------------------------
//----------------------------------------------
// -- Checksum 
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------
//----------------------------------------------

FILE *outFileCRC=NULL;
#define CRC_FILENAME 			"\\dnw\\dnw_checksum.txt"



void CheckSum_MD5(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};
	int ii=0;
	
	memset( logfile, 0x00, sizeof(logfile) );


	{
	MD5_CTX mdContext;
	int nBytes;
	__int64 isize = 1L;
	unsigned char md5_data[1024]; // The size is NERVER modified!!!!

	/// initial
	memset( &mdContext, 0x00, sizeof(MD5_CTX) );
	memset( &md5_data, 0x00, sizeof(md5_data) );

	// 3. open File and Calcuate MD5
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open MD5 file[%s]. Err(%u)\n"), gszCRCFileName, GetLastError() );
		fclose(fiCRC);
		_endthread();
		return;
	}


#if 0
	// Display Filename ---
	ii = strlen(gszCRCFileName);
	isFind=0;
	for(j=ii; ii>0; j--)
	{
		if( gszCRCFileName[j] == '\\' ) { isFind=1; break; }
	}
	strcpy( gszPrtFileName, &gszCRCFileName[j+1] );
#endif


	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] MD5>> [%s] is hashing... \r\n"), gszPrtFileName );


	// --------------------------------------
	// Starting Checksum
	// --------------------------------------
	g_ChecksumCalcing = CHECKSUM_MD5;

	MD5Init (&mdContext);
	isize = 1L;
	ulCRCFileRead = 0UL;
	while ((nBytes = fread (md5_data, 1, sizeof(md5_data) /*1024*/, fiCRC)) != 0)
	{
		ulCRCFileRead += nBytes;
		MD5Update (&mdContext, md5_data, nBytes);
	}
	MD5Final(&mdContext);
	MDPrint(&mdContext);

	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }


	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("  [%s] \r\n"), logfile ); // file name only

	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s hash is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");


	for (ii = 0; ii < 16; ii++)
	{
		fprintf(outFileCRC, "%02x", mdContext.digest[ii]);
	}


	if(1==isFind)
	{
		EB_Printf(TEXT("   *MD5*%s (%I64u Byte) \r\n"), gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "   *MD5*%s (%I64u Byte) \r\n", gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("   *MD5*%s (%I64u Byte) \r\n"), gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "   *MD5*%s (%I64u Byte) \r\n", gszCRCFileName, ulCRCFileSize );
	}
	// Display Filename ---
	//fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);

	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	

	//EB_Printf(TEXT("[dnw] ------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);
	/* _endthread given to terminate */
	_endthread();
}




void CheckSum_SHA1(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};

	memset( logfile, 0x00, sizeof(logfile) );


	{
	ShaBuffer SHA1output;
	size_t nBytes;
	__int64 isize = 1L;	
	sha1_context ctx;
	unsigned char sha1_buf[1024]; // The size is NERVER modified!!!!

	
	/// initial
	memset( &ctx, 0x00, sizeof(sha1_context) );
	memset( sha1_buf, 0x00, sizeof(sha1_buf) );

	// 3. open File and Calcuate SHA1
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open SHA1 file[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		fclose(fiCRC);
		_endthread();
		return;
	}

	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] SHA1>> [%s] is hashing... \r\n"), gszPrtFileName );

	// --------------------------------------
	// Starting Checksum
	// --------------------------------------
	g_ChecksumCalcing = CHECKSUM_SHA1;

	/// SHA1 Calculate ----
	sha1_starts(&ctx);
	isize = 1L;
	ulCRCFileRead = 0L;
	while((nBytes = fread(sha1_buf, 1, sizeof(sha1_buf), fiCRC)) > 0)
	{
		ulCRCFileRead += nBytes;
		sha1_update(&ctx, sha1_buf, (int)nBytes);
	}
	sha1_finish(&ctx, SHA1output);
	memset(&ctx, 0, sizeof(sha1_context));
	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }


	//EB_Printf(TEXT("\r\n") );

	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("  [%s] \r\n"), logfile ); // file name only


	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s hash is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	

	if(1==isFind)
	{
		EB_Printf(TEXT("[dnw] %s    *SHA1*%s (%I64u Byte)\r\n"), SHA1output, gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "%s    *SHA1*%s (%I64u Byte)\r\n", SHA1output, gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("[dnw] %s    *SHA1*%s (%I64u Byte)\r\n"), SHA1output, gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "%s    *SHA1*%s (%I64u Byte)\r\n", SHA1output, gszCRCFileName, ulCRCFileSize );
	}
	// Display Filename ---
	//fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));


	//EB_Printf(TEXT("[dnw] ------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);
	/* _endthread given to terminate */
	_endthread();
}



void CheckSum_CRC32(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};
	unsigned char data_buf[CRC_BUFSIZE+1];

	memset( data_buf, 0x00, sizeof(data_buf) );
	memset( logfile, 0x00, sizeof(logfile) );


	{
	uint32_t calcCrc32=0;
	__int64  TotDLen=0;
	size_t 	 LenRead=0;


	// 3. open File and Calcuate CRC32
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open CRC32 file[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		fclose(fiCRC);
		_endthread();
		return;
	}

	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] CRC32>> [%s] is calculating... \r\n"), gszPrtFileName );


	// --------------------------------------
	// Starting Checksum
	// --------------------------------------
	g_ChecksumCalcing = CHECKSUM_CRC32;


	TotDLen = 0;
	ulCRCFileRead = 0L;
	while ( LenRead = fread(data_buf, 1 /*sizeof(unsigned char)*/, CRC_BUFSIZE, fiCRC) )
	{
		ulCRCFileRead += LenRead;
		TotDLen += LenRead;

	#if 1 // 2020.07.16, Choice One!!
		calcCrc32 = make_crc32(calcCrc32, (void*)data_buf, LenRead);
	#else
		calcCrc32 = calcCRC32(data_buf, LenRead, calcCrc32);
	#endif
	
	}

	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }	



	//EB_Printf(TEXT("\r\n") );


	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("  [%s] \r\n"), logfile ); // file name only


	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s checksum is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");


	if(1==isFind)
	{
		EB_Printf(TEXT("[dnw] %08x    *CRC32*%s (%I64u Byte)\r\n"), calcCrc32, gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "%08x    *CRC32*%s (%I64u Byte)\r\n", calcCrc32, gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("[dnw] %08x    *CRC32*%s (%I64u Byte)\r\n"), calcCrc32, gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "%08x    *CRC32*%s (%I64u Byte)\r\n", calcCrc32, gszCRCFileName, ulCRCFileSize );
	}
	// Display Filename ---
	//fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));

	//EB_Printf(TEXT("[dnw] ------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);
	/* _endthread given to terminate */
	_endthread();
}


void CheckSum_CRC64(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};
	unsigned char data_buf[CRC_BUFSIZE+1];

	memset( data_buf, 0x00, sizeof(data_buf) );
	memset( logfile, 0x00, sizeof(logfile) );


	{
	uint64_t calcCrc64 = 0ULL;
	__int64  TotDLen=0;
	size_t 	 LenRead=0;

	// 3. open File and Calcuate CRC64
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open file CRC64[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		fclose(fiCRC);
		_endthread();
		return;
	}


	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] CRC64>> [%s] is calculating... \r\n"), gszPrtFileName );


	// --------------------------------------
	// Starting Checksum
	// --------------------------------------
	g_ChecksumCalcing = CHECKSUM_CRC64;


	TotDLen = 0;
	ulCRCFileRead = 0L;
	while ( LenRead = fread(data_buf, 1 /*sizeof(unsigned char)*/, CRC_BUFSIZE, fiCRC) )
	{
		ulCRCFileRead += LenRead;
		TotDLen += LenRead;
		calcCrc64 = make_crc64(calcCrc64, data_buf, LenRead);
	}

	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }	



	//EB_Printf(TEXT("\r\n") );


	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("  [%s] \r\n"), logfile ); // file name only


	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s checksum is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	


	if(1==isFind)
	{
		EB_Printf(TEXT("[dnw] %016llx    *CRC64*%s (%I64u Byte)\r\n"), calcCrc64, gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "%016llx    *CRC64*%s (%I64u Byte)\r\n", calcCrc64, gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("[dnw] %016llx    *CRC64*%s (%I64u Byte)\r\n"), calcCrc64, gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "%016llx    *CRC64*%s (%I64u Byte)\r\n", calcCrc64, gszCRCFileName, ulCRCFileSize );
	}
	// Display Filename ---
	//fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));

	//EB_Printf(TEXT("[dnw] ------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);
	/* _endthread given to terminate */
	_endthread();
}




void CheckSum_SHA2_256(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};

	memset( logfile, 0x00, sizeof(logfile) );


	{
	__int64 	kll;
	size_t		ll;
	SHA256_CTX		ctx256;
	char	sha256_buf[SHA2_BUFLEN];


	/// initial
	memset( &ctx256, 0x00, sizeof(SHA256_CTX) );
	memset( sha256_buf, 0x00, sizeof(sha256_buf) );

	// 3. open File and Calcuate CRC64
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open file SHA256[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		fclose(fiCRC);
		_endthread();
		return;
	}


	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] SHA256>> [%s] is hashing... \r\n"), gszPrtFileName );


	// --------------------------------------
	// Starting Checksum
	// --------------------------------------
	g_ChecksumCalcing = CHECKSUM_SHA2_256;


	/// SHA2 Calculate ----
	SHA256_Init(&ctx256);

	kll = 0;
	ulCRCFileRead = 0L;
	while((ll = fread(sha256_buf, 1, SHA2_BUFLEN, fiCRC)) > 0) 
	{
		ulCRCFileRead += ll;
		kll += ll;
		SHA256_Update(&ctx256, (unsigned char*)sha256_buf, ll);
	}
	SHA256_End(&ctx256, sha256_buf);

	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }	

	


	//EB_Printf(TEXT("\r\n") );


	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("  [%s] \r\n"), logfile ); // file name only


	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s hash is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");


	if(1==isFind)
	{
		EB_Printf(TEXT("[dnw] %s  *SHA256*%s (%I64u Byte) \r\n"), sha256_buf, gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "%s  *SHA256*%s (%I64u Byte) \r\n", sha256_buf, gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("[dnw] %s  *SHA256*%s (%I64u Byte) \r\n"), sha256_buf, gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "%s  *SHA256*%s (%I64u Byte) \r\n", sha256_buf, gszCRCFileName, ulCRCFileSize );
	}
	// Display Filename ---
	//fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));


	//EB_Printf(TEXT("[dnw] ------------------------------------------------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);
	/* _endthread given to terminate */
	_endthread();
}



void CheckSum_SHA2_384(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};

	memset( logfile, 0x00, sizeof(logfile) );


	{
	__int64 	kll;
	size_t		ll;
	SHA384_CTX		ctx384;
	char	sha384_buf[SHA2_BUFLEN];


	/// initial ---
	memset( &ctx384, 0x00, sizeof(SHA384_CTX) );
	memset( sha384_buf, 0x00, sizeof(sha384_buf) );

	// 3. open File and Calcuate CRC64
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open file SHA384[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		fclose(fiCRC);
		_endthread();
		return;
	}


	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] SHA384>> [%s] is hashing... \r\n"), gszPrtFileName );


	// --------------------------------------
	// Starting Checksum
	// --------------------------------------
	g_ChecksumCalcing = CHECKSUM_SHA2_384;


	/// SHA2 Calculate ----
	SHA384_Init(&ctx384);

	kll = 0;
	ulCRCFileRead = 0L;
	while((ll = fread(sha384_buf, 1, SHA2_BUFLEN, fiCRC)) > 0) 
	{
		ulCRCFileRead += ll;
		kll += ll;
		SHA384_Update(&ctx384, (unsigned char*)sha384_buf, ll);
	}
	SHA384_End(&ctx384, sha384_buf);

	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }	



	//EB_Printf(TEXT("\r\n") );


	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("  [%s] \r\n"), logfile ); // file name only


	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s hash is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");


	
	if(1==isFind)
	{
		EB_Printf(TEXT("[dnw] %s  *SHA384*%s (%I64u Byte) \r\n"), sha384_buf, gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "%s  *SHA384*%s (%I64u Byte) \r\n", sha384_buf, gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("[dnw] %s  *SHA384*%s (%I64u Byte) \r\n"), sha384_buf, gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "%s	 *SHA384*%s (%I64u Byte) \r\n", sha384_buf, gszCRCFileName, ulCRCFileSize );
	}
	// Display Filename ---
	//fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));


	//EB_Printf(TEXT("[dnw] ------------------------------------------------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);
	/* _endthread given to terminate */
	_endthread();
}



void CheckSum_SHA2_512(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};

	memset( logfile, 0x00, sizeof(logfile) );


	{
	__int64 	kll;
	size_t 		ll;
	SHA512_CTX 		ctx512;
	char	sha512_buf[SHA2_BUFLEN];


	/// initial
	memset( &ctx512, 0x00, sizeof(SHA512_CTX) );
	memset( sha512_buf, 0x00, sizeof(sha512_buf) );


	// 3. open File and Calcuate CRC64
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open file SHA512[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		fclose(fiCRC);
		_endthread();
		return;
	}


	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] SHA512>> [%s] is hashing... \r\n"), gszPrtFileName );

	// --------------------------------------
	// Starting Checksum
	// --------------------------------------	
	g_ChecksumCalcing = CHECKSUM_SHA2_512;


	/// SHA2 Calculate ----
	SHA512_Init(&ctx512);

	kll = 0;
	ulCRCFileRead = 0;
	while((ll = fread(sha512_buf, 1, SHA2_BUFLEN, fiCRC)) > 0) 
	{
		ulCRCFileRead += ll;
		kll += ll;
		SHA512_Update(&ctx512, (unsigned char*)sha512_buf, ll);
	}
	SHA512_End(&ctx512, sha512_buf);

	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }	



	//EB_Printf(TEXT("\r\n") );


	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("  [%s] \r\n"), logfile ); // file name only


	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s hash is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");


	
	if(1==isFind)
	{
		EB_Printf(TEXT("[dnw] %s  *SHA512*%s (%I64u Byte) \r\n"), sha512_buf, gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "%s  *SHA512*%s (%I64u Byte) \r\n", sha512_buf, gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("[dnw] %s  *SHA512*%s (%I64u Byte) \r\n"), sha512_buf, gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "%s  *SHA512*%s (%I64u Byte) \r\n", sha512_buf, gszCRCFileName, ulCRCFileSize );
	}

	// Display Filename ---
	//fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));


	//EB_Printf(TEXT("[dnw] ------------------------------------------------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);
	/* _endthread given to terminate */
	_endthread();
}





void CheckSum_SHA2_224(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};
	int ii=0;

	memset( logfile, 0x00, sizeof(logfile) );


	{
	__int64 	kll;
	size_t 		ll;
	sha224_ctx      ctx224;		
	unsigned char	sha224_buf[1024];



	/// initial ----
	memset( &ctx224, 0x00, sizeof(sha224_ctx) );
	memset( sha224_buf, 0x00, sizeof(sha224_buf) );

	// 3. open File and Calcuate CRC64
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open file SHA224[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		fclose(fiCRC);
		_endthread();
		return;
	}


	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] SHA224>> [%s] is hashing... \r\n"), gszPrtFileName );


	// --------------------------------------
	// Starting Checksum
	// --------------------------------------
	g_ChecksumCalcing = CHECKSUM_SHA2_224;

	/// SHA2 Calculate ----
	sha224_init(&ctx224);
			
	kll = 0;
	ulCRCFileRead = 0L;
	while((ll = fread(sha224_buf, 1, sizeof(sha224_buf), fiCRC)) > 0) 
	{
		ulCRCFileRead += ll;
		kll += ll;
		sha224_update(&ctx224, sha224_buf, ll);
	}
	
	sha224_final(&ctx224, sha224_buf);

	//sha224Print (sha224_buf);

	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }	
	


	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("	[%s] \r\n"), logfile ); // file name only


	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s hash is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");


#if 0
	EB_Printf(TEXT("\r\n[dnw] "));
#else
	EB_Printf(TEXT("[dnw] "));
#endif

	for (ii = 0; ii < SHA224_DIGEST_SIZE; ii++)
	{
		EB_Printf(TEXT("%02x"), sha224_buf[ii] );
		fprintf(outFileCRC, "%02x", sha224_buf[ii]);
	}


	if(1==isFind)
	{
		EB_Printf(TEXT("   *SHA224*%s (%I64u Byte) \r\n"), gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "   *SHA224*%s (%I64u Byte) \r\n", gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("   *SHA224*%s (%I64u Byte) \r\n"), gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "   *SHA224*%s (%I64u Byte) \r\n", gszCRCFileName, ulCRCFileSize );
	}

	// Display Filename ---
	//fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));


	//EB_Printf(TEXT("[dnw] ------------------------------------------------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);
	/* _endthread given to terminate */
	_endthread();
}




void CheckSum_MD6(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};

	memset( logfile, 0x00, sizeof(logfile) );


	{
	uint64_t nBytes = 0UL, kll = 0UL;
	char md6_data[1024];


	/* ------------------------------------------------------ */
	/* ------ MD6 : set default md6 parameter settings ------ */
	md6_dgtLen = 256;           /* digest length */
	md6_keylen = 0;             /* key length in bytes (at most 64) */
	md6_modPar = 64;            /* mode parameter */
	md6_roundN = md6_default_r(md6_dgtLen,md6_keylen);  /* number of rounds */
	md6_use_default_r = 1;     /* 1 if r should be set to the default, 0 if r is explicitly provided */
	/* ------------------------------------------------------ */


	/// initial
	nBytes = 0UL;
	memset( md6_data, 0x00, sizeof(md6_data) );


	// 3. open File and Calcuate CRC64
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open file MD6[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		fclose(fiCRC);
		/* _endthread given to terminate */
		_endthread();
		return;
	}


	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] MD6>> [%s] is hashing... \r\n"), gszPrtFileName );

	// --------------------------------------
	// Starting Checksum
	// --------------------------------------
	g_ChecksumCalcing = CHECKSUM_MD6;

	/// MD6 Calculate ----
	hash_init();

	kll = 0UL;
	ulCRCFileRead = 0L;
	while( (nBytes = fread(md6_data, 1, sizeof(md6_data), fiCRC)) > 0 ) 
	{

		ulCRCFileRead += kll;
		kll += nBytes;
		hash_update(md6_data,nBytes*8);
	}
	hash_final();


	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }	




	//EB_Printf(TEXT("\r\n") );


	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("  [%s] \r\n"), logfile ); // file name only


	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s hash is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");


	
	if(1==isFind)
	{
		EB_Printf(TEXT("[dnw] %s  *MD6*%s (%I64u Byte) \r\n"), md6_st.hexhashval, gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "%s  *MD6*%s (%I64u Byte) \r\n", md6_st.hexhashval, gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("[dnw] %s  *MD6*%s (%I64u Byte) \r\n"), md6_st.hexhashval, gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "%s  *MD6*%s (%I64u Byte) \r\n", md6_st.hexhashval, gszCRCFileName, ulCRCFileSize );
	}

	// Display Filename ---
	//fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));


	//EB_Printf(TEXT("[dnw] ------------------------------------------------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);

	/* _endthread given to terminate */
	_endthread();
}




#if 1 // SHA3, 2017.08.22
// -------------------------------------------------------------
// Secure Hash Algorithm : 2017.08.22
// -------------------------------------------------------------

#define SHA3_BUFLEN 			16384 // 16*1024



void CheckSum_SHA3_KECCAK_224(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};
	int ii=0;


	memset( logfile, 0x00, sizeof(logfile) );

	{
	Sha3_224Context *sha3context=NULL;
	char	sha3_buf[SHA3_BUFLEN] = {0,};
	uint8_t sha3digest[SHA3_BUFLEN] = {0,};
	char    sha3digestTxt[SHA3_BUFLEN] = {0,};
	
	__int64 	kll;
	size_t 		ll;

	//Allocate a memory buffer to hold the SHA3-224 context
	sha3context = (Sha3_224Context *)malloc( sizeof(Sha3_224Context) );

	if(NULL == sha3context ) 
	{
		EB_Printf(TEXT("[dnw] can not open file KECCAK SHA3-224[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		/* _endthread given to terminate */
		_endthread();
		return;
	}
	
	memset( sha3context, 0x00, sizeof(Sha3_224Context) ); // 2017.08.22



	// 3. open File and Calcuate SHA3-224
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open file KECCAK SHA3-224[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		free( (void*) sha3context);	// 2020.06.11
		fclose(fiCRC);
		_endthread();
		return;
	}


	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] SHA3-224>> [%s] is hashing... \r\n"), gszPrtFileName );


	// --------------------------------------
	// Starting Checksum
	// --------------------------------------
	g_ChecksumCalcing = CHECKSUM_SHA3_KECCAK_224;


	//Initialize the SHA3-224 context
	sha3_224Init(sha3context);

	kll = 0;
	ulCRCFileRead = 0L;
	while((ll = fread(sha3_buf, 1, sizeof(sha3_buf), fiCRC)) > 0) 
	{
		ulCRCFileRead += ll;
		kll += ll;

		//Digest the message
		sha3_224Update(sha3context, sha3_buf, ll);
	}
	
	//Finalize the SHA3-224 message digest
	sha3_224Final(sha3context, sha3digest);
	

	
	//Free previously allocated memory
	free( (void*) sha3context);


	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }	
	


	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("	[%s] \r\n"), logfile ); // file name only


	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s hash is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");


	EB_Printf(TEXT("[dnw] "));

#if 1 // 2017.09.13
	memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );

	for (ii = 0; ii < SHA3_224_DIGEST_SIZE; ii++)
	{
		sprintf(&sha3digestTxt[ii*2], (char*)"%02x", sha3digest[ii]);
	}

	EB_Printf(TEXT("%s"), sha3digestTxt);
	fprintf(outFileCRC, "%s", sha3digestTxt);

#else

	for (ii = 0; ii < SHA3_224_DIGEST_SIZE; ii++)
	{
		EB_Printf(TEXT("%02x"), sha3digest[ii] );
		fprintf(outFileCRC, "%02x", sha3digest[ii]);
	}
#endif


	if(1==isFind)
	{
		EB_Printf(TEXT("  *SHA3-224*%s (%I64u Byte) \r\n"), gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "  *SHA3-224*%s (%I64u Byte) \r\n", gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("  *SHA3-224*%s (%I64u Byte) \r\n"), gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "  *SHA3-224*%s (%I64u Byte) \r\n", gszCRCFileName, ulCRCFileSize );
	}

	// Display Filename ---
	//fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));


	//EB_Printf(TEXT("[dnw] ------------------------------------------------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);
	/* _endthread given to terminate */
	_endthread();
}




void CheckSum_SHA3_KECCAK_256(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};
	int ii=0;

	memset( logfile, 0x00, sizeof(logfile) );

	{
	Sha3_256Context *sha3context=NULL;
	char	sha3_buf[SHA3_BUFLEN] = {0,};
	uint8_t sha3digest[SHA3_BUFLEN] = {0,};
	char    sha3digestTxt[SHA3_BUFLEN] = {0,};
	
	__int64 	kll;
	size_t 		ll;

	//Allocate a memory buffer to hold the SHA3-384 context
	sha3context = (Sha3_256Context *)malloc( sizeof(Sha3_256Context) );
	//Failed to allocate memory?
	if(NULL == sha3context ) 
	{
		EB_Printf(TEXT("[dnw] can not open file KECCAK SHA3-256[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		/* _endthread given to terminate */
		_endthread();
		return;
	}
	
	memset( sha3context, 0x00, sizeof(Sha3_256Context) ); // 2017.08.22



	// 3. open File and Calcuate SHA3-256
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open file KECCAK SHA3-256[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		free( (void*) sha3context);	// 2020.06.11
		fclose(fiCRC);
		_endthread();
		return;
	}


	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] SHA3-256>> [%s] is hashing... \r\n"), gszPrtFileName );


	// --------------------------------------
	// Starting Checksum
	// --------------------------------------
	g_ChecksumCalcing = CHECKSUM_SHA3_KECCAK_256;


	//Initialize the SHA3-256 context
	sha3_256Init(sha3context);


	kll = 0;
	ulCRCFileRead = 0L;
	while((ll = fread(sha3_buf, 1, sizeof(sha3_buf), fiCRC)) > 0) 
	{
		ulCRCFileRead += ll;
		kll += ll;

		//Digest the message
		sha3_256Update(sha3context, sha3_buf, ll);
	}
	
	//Finalize the SHA3-256 message digest
	sha3_256Final(sha3context, sha3digest);

	
	//Free previously allocated memory
	free( (void*) sha3context);


	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }	
	


	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("	[%s] \r\n"), logfile ); // file name only


	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s hash is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");


	EB_Printf(TEXT("[dnw] "));

#if 1 // 2017.09.13
	memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );

	for (ii = 0; ii < SHA3_256_DIGEST_SIZE; ii++)
	{
		sprintf(&sha3digestTxt[ii*2], (char*)"%02x", sha3digest[ii]);
	}

	EB_Printf(TEXT("%s"), sha3digestTxt);
	fprintf(outFileCRC, "%s", sha3digestTxt);

#else

	for (ii = 0; ii < SHA3_256_DIGEST_SIZE; ii++)
	{
		EB_Printf(TEXT("%02x"), sha3digest[ii] );
		fprintf(outFileCRC, "%02x", sha3digest[ii]);
	}
#endif

	if(1==isFind)
	{
		EB_Printf(TEXT("  *SHA3-256*%s (%I64u Byte) \r\n"), gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "  *SHA3-256*%s (%I64u Byte) \r\n", gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("  *SHA3-256*%s (%I64u Byte) \r\n"), gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "  *SHA3-256*%s (%I64u Byte) \r\n", gszCRCFileName, ulCRCFileSize );
	}

	// Display Filename ---
	//fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));


	//EB_Printf(TEXT("[dnw] ------------------------------------------------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);
	/* _endthread given to terminate */
	_endthread();
}



void CheckSum_SHA3_KECCAK_384(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};
	int ii=0;

	memset( logfile, 0x00, sizeof(logfile) );



	{
	Sha3_384Context *sha3context=NULL;
	char	sha3_buf[SHA3_BUFLEN] = {0,};
	uint8_t sha3digest[SHA3_BUFLEN] = {0,};
	char    sha3digestTxt[SHA3_BUFLEN] = {0,};

	
	__int64 	kll;
	size_t 		ll;

	//Allocate a memory buffer to hold the SHA3-384 context
	sha3context = (Sha3_384Context *)malloc( sizeof(Sha3_384Context) );

	if(NULL == sha3context ) 
	{
		EB_Printf(TEXT("[dnw] can not open file KECCAK SHA3-384[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		/* _endthread given to terminate */
		_endthread();
		return;
	}
	
	memset( sha3context, 0x00, sizeof(Sha3_384Context) ); // 2017.08.22



	// 3. open File and Calcuate SHA3-384
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open file KECCAK SHA3-384[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		free( (void*) sha3context);	// 2020.06.11
		fclose(fiCRC);
		_endthread();
		return;
	}


	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] SHA3-384>> [%s] is hashing... \r\n"), gszPrtFileName );


	// --------------------------------------
	// Starting Checksum
	// --------------------------------------
	g_ChecksumCalcing = CHECKSUM_SHA3_KECCAK_384;


	//Initialize the SHA3-384 context
	sha3_384Init(sha3context);
		
	kll = 0;
	ulCRCFileRead = 0L;
	while((ll = fread(sha3_buf, 1, sizeof(sha3_buf), fiCRC)) > 0) 
	{
		ulCRCFileRead += ll;
		kll += ll;

		//Digest the message
		sha3_384Update(sha3context, sha3_buf, ll);
	}
	
	//Finalize the SHA3-384 message digest
	sha3_384Final(sha3context, sha3digest);

	

	//Free previously allocated memory
	free( (void*) sha3context);


	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }	
	


	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("	[%s] \r\n"), logfile ); // file name only


	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s hash is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");


	EB_Printf(TEXT("[dnw] "));

#if 1 // 2017.09.13
	memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );

	for (ii = 0; ii < SHA3_384_DIGEST_SIZE; ii++)
	{
		sprintf(&sha3digestTxt[ii*2], (char*)"%02x", sha3digest[ii]);
	}

	EB_Printf(TEXT("%s"), sha3digestTxt);
	fprintf(outFileCRC, "%s", sha3digestTxt);

#else

	for (ii = 0; ii < SHA3_384_DIGEST_SIZE; ii++)
	{
		EB_Printf(TEXT("%02x"), sha3digest[ii] );
		fprintf(outFileCRC, "%02x", sha3digest[ii]);
	}
#endif

	if(1==isFind)
	{
		EB_Printf(TEXT("  *SHA3-384*%s (%I64u Byte) \r\n"), gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "  *SHA3-384*%s (%I64u Byte) \r\n", gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("  *SHA3-384*%s (%I64u Byte) \r\n"), gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "  *SHA3-384*%s (%I64u Byte) \r\n", gszCRCFileName, ulCRCFileSize );
	}

	// Display Filename ---
	//fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));


	//EB_Printf(TEXT("[dnw] ------------------------------------------------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);
	/* _endthread given to terminate */
	_endthread();
}



// 2017.08.22
void CheckSum_SHA3_KECCAK_512(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};
	int ii=0;

	memset( logfile, 0x00, sizeof(logfile) );


	{
	Sha3_512Context *sha3context=NULL;
	char	sha3_buf[SHA3_BUFLEN] = {0,};
	uint8_t sha3digest[SHA3_BUFLEN] = {0,};
	char    sha3digestTxt[SHA3_BUFLEN] = {0,};

	
	__int64 	kll;
	size_t 		ll;

	//Allocate a memory buffer to hold the SHA3-512 context

	sha3context = (Sha3_512Context *)malloc( sizeof(Sha3_512Context) );
	//Failed to allocate memory.
	if(NULL == sha3context ) 
	{
		EB_Printf(TEXT("[dnw] can not open file KECCAK SHA3-512[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		/* _endthread given to terminate */
		_endthread();
		return;
	}
	
	memset( sha3context, 0x00, sizeof(Sha3_512Context) ); // 2017.08.22

	// 3. open File and Calcuate SHA3-512
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open file KECCAK SHA3-512[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		free( (void*) sha3context);	// 2020.06.11
		fclose(fiCRC);
		_endthread();
		return;
	}


	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] SHA3-512>> [%s] is hashing... \r\n"), gszPrtFileName );


	// --------------------------------------
	// Starting Checksum
	// --------------------------------------
	g_ChecksumCalcing = CHECKSUM_SHA3_KECCAK_512;

	/// SHA3-KECCAK 512 Calculate ----

	//Initialize the SHA3-512 context
	sha3_512Init(sha3context);

		
	kll = 0;
	ulCRCFileRead = 0L;
	while((ll = fread(sha3_buf, 1, sizeof(sha3_buf), fiCRC)) > 0) 
	{
		ulCRCFileRead += ll;
		kll += ll;

		//Digest the message
		sha3_512Update(sha3context, sha3_buf, ll);
	
	}
	
	//Finalize the SHA3-512 message digest
	sha3_512Final(sha3context, sha3digest);

	//Free previously allocated memory
	free( (void*) sha3context);


	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }	
	


	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("	[%s] \r\n"), logfile ); // file name only


	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s hash is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");


	EB_Printf(TEXT("[dnw] "));


#if 1 // 2017.09.13
	memset( sha3digestTxt, 0x00, sizeof(sha3digestTxt) );

	for (ii = 0; ii < SHA3_512_DIGEST_SIZE; ii++)
	{
		sprintf(&sha3digestTxt[ii*2], (char*)"%02x", sha3digest[ii]);
	}

	EB_Printf(TEXT("%s"), sha3digestTxt);
	fprintf(outFileCRC, "%s", sha3digestTxt);

#else

	for (ii = 0; ii < SHA3_512_DIGEST_SIZE; ii++)
	{
		EB_Printf(TEXT("%02x"), sha3digest[ii] );
		fprintf(outFileCRC, "%02x", sha3digest[ii]);
	}
#endif


	if(1==isFind)
	{
		EB_Printf(TEXT("  *SHA3-512*%s (%I64u Byte) \r\n"), gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "  *SHA3-512*%s (%I64u Byte) \r\n", gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("  *SHA3-512*%s (%I64u Byte) \r\n"), gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "  *SHA3-512*%s (%I64u Byte) \r\n", gszCRCFileName, ulCRCFileSize );
	}

	// Display Filename ---
	//fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));


	//EB_Printf(TEXT("[dnw] ------------------------------------------------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);
	/* _endthread given to terminate */
	_endthread();
}
#endif


#if 1 // 2017.08.29 SHAKE-128 / SHAKE-256

// 2017.08.29
// https://emn178.github.io/online-tools/shake_128.html
void CheckSum_SHAKE128(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};
	int ii=0;

	memset( logfile, 0x00, sizeof(logfile) );


	{
	Shake128Context *ShakeContext=NULL;
	char	shakeBuf[SHA3_BUFLEN] = {0,};
	uint8_t shakeDigest[SHA3_BUFLEN] = {0,};
	char    shakeDigestTxt[SHA3_BUFLEN] = {0,};
	
	__int64 	kll;
	size_t 		ll;

	//Allocate a memory buffer to hold the SHAKE128 context
	ShakeContext = (Shake128Context *)malloc( sizeof(Shake128Context) );
	//Failed to allocate memory.
	if(NULL == ShakeContext ) 
	{
		EB_Printf(TEXT("[dnw] can not open file SHAKE128[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		/* _endthread given to terminate */
		_endthread();
		return;
	}
	
	memset( ShakeContext, 0x00, sizeof(Shake128Context) ); // 2017.08.29

	// 3. open File and Calcuate SHA3-512
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open file SHAKE128[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		free( (void*) ShakeContext); // 2020.06.11
		fclose(fiCRC);
		_endthread();
		return;
	}


	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] SHAKE128>> [%s] is hashing... \r\n"), gszPrtFileName );


	// --------------------------------------
	// Starting Checksum
	// --------------------------------------
	g_ChecksumCalcing = CHECKSUM_SHAKE_128;


	//Initialize the SHAKE128 context
	shake128Init(ShakeContext);

	kll = 0;
	ulCRCFileRead = 0L;
	while((ll = fread(shakeBuf, 1, sizeof(shakeBuf), fiCRC)) > 0) 
	{
		ulCRCFileRead += ll;
		kll += ll;

		//Absorb input data
		shake128Absorb(ShakeContext, shakeBuf, ll);
	}
	
	//Finish absorbing phase
	shake128Final(ShakeContext);

	//Extract data from the squeezing phase
	shake128Squeeze(ShakeContext, shakeDigest, SHAKE128_DIGEST_DEFAULT_SIZE);
	

	//Free previously allocated memory
	free( (void*) ShakeContext);


	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }	
	


	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("	[%s] \r\n"), logfile ); // file name only


	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s hash is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");


	EB_Printf(TEXT("[dnw] "));


#if 1 // 2017.09.13
	memset( shakeDigestTxt, 0x00, sizeof(shakeDigestTxt) );

	for (ii = 0; ii < SHAKE128_DIGEST_DEFAULT_SIZE; ii++)
	{
		sprintf(&shakeDigestTxt[ii*2], (char*)"%02x", shakeDigest[ii]);
	}

	EB_Printf(TEXT("%s"), shakeDigestTxt);
	fprintf(outFileCRC, "%s", shakeDigestTxt);

#else

	for (ii = 0; ii < SHAKE128_DIGEST_DEFAULT_SIZE; ii++)
	{
		EB_Printf(TEXT("%02x"), shakeDigest[ii] );
		fprintf(outFileCRC, "%02x", shakeDigest[ii]);
	}
#endif



	if(1==isFind)
	{
		EB_Printf(TEXT("  *SHAKE128*%s (%I64u Byte) \r\n"), gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "  *SHAKE128*%s (%I64u Byte) \r\n", gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("  *SHAKE128*%s (%I64u Byte) \r\n"), gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "  *SHAKE128*%s (%I64u Byte) \r\n", gszCRCFileName, ulCRCFileSize );
	}

	// Display Filename ---
	//fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));


	//EB_Printf(TEXT("[dnw] ------------------------------------------------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);
	/* _endthread given to terminate */
	_endthread();
}



// 2017.08.29
// https://emn178.github.io/online-tools/shake_256.html
void CheckSum_SHAKE256(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};
	int ii=0;

	memset( logfile, 0x00, sizeof(logfile) );

	{
	Shake256Context *ShakeContext=NULL;
	char	shakeBuf[SHA3_BUFLEN] = {0,};
	uint8_t shakeDigest[SHA3_BUFLEN] = {0,};
	char    shakeDigestTxt[SHA3_BUFLEN] = {0,};

	__int64 	kll;
	size_t 		ll;

	//Allocate a memory buffer to hold the SHAKE256 context
	ShakeContext = (Shake256Context *)malloc( sizeof(Shake256Context) );
	//Failed to allocate memory.
	if(NULL == ShakeContext ) 
	{
		EB_Printf(TEXT("[dnw] can not open file SHAKE256[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		/* _endthread given to terminate */
		_endthread();
		return;
	}
	
	memset( ShakeContext, 0x00, sizeof(Shake256Context) ); // 2017.08.29

	// 3. open File and Calcuate SHAKE-256
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open file SHAKE256[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		free( (void*) ShakeContext); // 2020.06.11
		fclose(fiCRC);
		_endthread();
		return;
	}


	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] SHAKE256>> [%s] is hashing... \r\n"), gszPrtFileName );


	// --------------------------------------
	// Starting Checksum
	// --------------------------------------
	g_ChecksumCalcing = CHECKSUM_SHAKE_256;

	
	//Initialize the SHAKE256 context
	shake256Init(ShakeContext);

	kll = 0;
	ulCRCFileRead = 0L;
	while((ll = fread(shakeBuf, 1, sizeof(shakeBuf), fiCRC)) > 0) 
	{
		ulCRCFileRead += ll;
		kll += ll;

		//Absorb input data
		shake256Absorb(ShakeContext, shakeBuf, ll);
	}

	//Finish absorbing phase
	shake256Final(ShakeContext);
	//Extract data from the squeezing phase
	shake256Squeeze(ShakeContext, shakeDigest, SHAKE256_DIGEST_DEFAULT_SIZE);
	

	//Free previously allocated memory
	free( (void*) ShakeContext);


	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }	
	


	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("	[%s] \r\n"), logfile ); // file name only


	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s hash is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");


	EB_Printf(TEXT("[dnw] "));

#if 1 // 2017.09.13

	memset( shakeDigestTxt, 0x00, sizeof(shakeDigestTxt) );
	for (ii = 0; ii < SHAKE256_DIGEST_DEFAULT_SIZE; ii++)
	{
		sprintf(&shakeDigestTxt[ii*2], (char*)"%02x", shakeDigest[ii]);
	}

	EB_Printf(TEXT("%s"), shakeDigestTxt);
	fprintf(outFileCRC, "%s", shakeDigestTxt);

#else

	for (ii = 0; ii < SHAKE256_DIGEST_DEFAULT_SIZE; ii++)
	{
		EB_Printf(TEXT("%02x"), shakeDigest[ii] );
		fprintf(outFileCRC, "%02x", shakeDigest[ii]);
	}
#endif

	if(1==isFind)
	{
		EB_Printf(TEXT("  *SHAKE256*%s (%I64u Byte) \r\n"), gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "  *SHAKE256*%s (%I64u Byte) \r\n", gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("  *SHAKE256*%s (%I64u Byte) \r\n"), gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "  *SHAKE256*%s (%I64u Byte) \r\n", gszCRCFileName, ulCRCFileSize );
	}

	// Display Filename ---
	//fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));


	//EB_Printf(TEXT("[dnw] ------------------------------------------------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);
	/* _endthread given to terminate */
	_endthread();
}
#endif




void CheckSum_Blake224(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};
	int ii=0;

	memset( logfile, 0x00, sizeof(logfile) );

	{
	uint8_t inbuf[BLOCK224], out[BLAKE224_LEN]={0,}; 
	char outTxt[BLAKE224_LEN*2]={0,}; 
	state224 S; 
	

	__int64 	kll;
	size_t 		ll;


	// 3. open File and Calcuate BLAKE-512
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open file BLAKE224[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		fclose(fiCRC);
		_endthread();
		return;
	}


	memset( inbuf, 0x00, sizeof(inbuf) ); // 2018.06.15

	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] BLAKE224>> [%s] is hashing... \r\n"), gszPrtFileName );


	// --------------------------------------
	// Starting Checksum
	// --------------------------------------
	g_ChecksumCalcing = CHECKSUM_BLAKE224;


	//Initialize the BLAKE-224 context
	blake224_init( &S ); 
	
	
	kll = 0;
	ulCRCFileRead = 0L;
	while((ll = fread(inbuf, 1, sizeof(inbuf), fiCRC)) > 0) 
	{
		ulCRCFileRead += ll;
		kll += ll;

		//Absorb input data
		blake224_update( &S, inbuf, ll ); 
	}

	//Finish absorbing phase
	blake224_final( &S, out ); 
	
	

	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }	
	


	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("	[%s] \r\n"), logfile ); // file name only


	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s hash is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");


	EB_Printf(TEXT("[dnw] "));


	memset( outTxt, 0x00, sizeof(outTxt) );
	for (ii = 0; ii < BLAKE224_LEN; ii++)
	{
		sprintf(&outTxt[ii*2], (char*)"%02x", out[ii]);
	}
	


	EB_Printf(TEXT("%s"), outTxt);
	fprintf(outFileCRC, "%s", outTxt);


	if(1==isFind)
	{
		EB_Printf(TEXT("  *BLAKE224*%s (%I64u Byte) \r\n"), gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "  *BLAKE224*%s (%I64u Byte) \r\n", gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("  *BLAKE224*%s (%I64u Byte) \r\n"), gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "  *BLAKE224*%s (%I64u Byte) \r\n", gszCRCFileName, ulCRCFileSize );
	}

	// Display Filename ---
	fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));


	//EB_Printf(TEXT("[dnw] ------------------------------------------------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);
	/* _endthread given to terminate */
	_endthread();
}



void CheckSum_Blake256(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};
	int ii=0;

	memset( logfile, 0x00, sizeof(logfile) );

	{
	uint8_t inbuf[BLOCK256], out[BLAKE256_LEN]={0,}; 
	char outTxt[BLAKE256_LEN*2]={0,}; 
	state256 S; 	

	__int64 	kll;
	size_t 		ll;

	

	// 3. open File and Calcuate BLAKE-512
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open file BLAKE256[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		fclose(fiCRC);
		_endthread();
		return;
	}

	memset( inbuf, 0x00, sizeof(inbuf) ); // 2018.06.15

	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] BLAKE256>> [%s] is hashing... \r\n"), gszPrtFileName );


	// --------------------------------------
	// Starting Checksum
	// --------------------------------------
	g_ChecksumCalcing = CHECKSUM_BLAKE256;


	//Initialize the BLAKE-256 context
    blake256_init( &S ); 
	
	kll = 0;
	ulCRCFileRead = 0L;
	while((ll = fread(inbuf, 1, sizeof(inbuf), fiCRC)) > 0) 
	{
		ulCRCFileRead += ll;
		kll += ll;

		//Absorb input data
		blake256_update( &S, inbuf, ll ); 
	}

	//Finish absorbing phase
    blake256_final( &S, out ); 
	

	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }	
	


	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("	[%s] \r\n"), logfile ); // file name only


	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s hash is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");


	EB_Printf(TEXT("[dnw] "));


	memset( outTxt, 0x00, sizeof(outTxt) );
	for (ii = 0; ii < BLAKE256_LEN; ii++)
	{
		sprintf(&outTxt[ii*2], (char*)"%02x", out[ii]);
	}
	


	EB_Printf(TEXT("%s"), outTxt);
	fprintf(outFileCRC, "%s", outTxt);


	if(1==isFind)
	{
		EB_Printf(TEXT("  *BLAKE256*%s (%I64u Byte) \r\n"), gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "  *BLAKE256*%s (%I64u Byte) \r\n", gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("  *BLAKE256*%s (%I64u Byte) \r\n"), gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "  *BLAKE256*%s (%I64u Byte) \r\n", gszCRCFileName, ulCRCFileSize );
	}

	// Display Filename ---
	//fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));


	//EB_Printf(TEXT("[dnw] ------------------------------------------------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);
	/* _endthread given to terminate */
	_endthread();
}




void CheckSum_Blake384(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};
	int ii=0;

	memset( logfile, 0x00, sizeof(logfile) );

	{
	uint8_t inbuf[BLOCK384], out[BLAKE384_LEN] ={0,}; 
	char outTxt[BLAKE384_LEN*2]={0,}; 
	state384 S; 		

	__int64 	kll;
	size_t 		ll;

	
	// 3. open File and Calcuate BLAKE-512
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open file BLAKE384[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		fclose(fiCRC);
		_endthread();
		return;
	}

	memset( inbuf, 0x00, sizeof(inbuf) ); // 2018.06.15


	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] BLAKE384>> [%s] is hashing... \r\n"), gszPrtFileName );


	// --------------------------------------
	// Starting Checksum
	// --------------------------------------
	g_ChecksumCalcing = CHECKSUM_BLAKE384;


	//Initialize the BLAKE-384 context
    blake384_init( &S ); 


	kll = 0;
	ulCRCFileRead = 0L;
	while((ll = fread(inbuf, 1, sizeof(inbuf), fiCRC)) > 0) 
	{
		ulCRCFileRead += ll;
		kll += ll;

		//Absorb input data
		blake384_update( &S, inbuf, ll ); 
	}

	//Finish absorbing phase
    blake384_final( &S, out ); 


	

	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }	
	


	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("	[%s] \r\n"), logfile ); // file name only


	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s hash is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");


	EB_Printf(TEXT("[dnw] "));


	memset( outTxt, 0x00, sizeof(outTxt) );
	for (ii = 0; ii < BLAKE384_LEN; ii++)
	{
		sprintf(&outTxt[ii*2], (char*)"%02x", out[ii]);
	}
	


	EB_Printf(TEXT("%s"), outTxt);
	fprintf(outFileCRC, "%s", outTxt);


	if(1==isFind)
	{
		EB_Printf(TEXT("  *BLAKE384*%s (%I64u Byte) \r\n"), gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "  *BLAKE384*%s (%I64u Byte) \r\n", gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("  *BLAKE384*%s (%I64u Byte) \r\n"), gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "  *BLAKE384*%s (%I64u Byte) \r\n", gszCRCFileName, ulCRCFileSize );
	}

	// Display Filename ---
	//fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));


	//EB_Printf(TEXT("[dnw] ------------------------------------------------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);
	/* _endthread given to terminate */
	_endthread();
}




void CheckSum_Blake512(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};
	int ii=0;

	memset( logfile, 0x00, sizeof(logfile) );

	{
	uint8_t inbuf[BLOCK512], out[BLAKE512_LEN]={0,}; 
	char outTxt[BLAKE512_LEN*2]={0,}; 
	state512 S; 

	__int64 	kll;
	size_t 		ll;

	

	// 3. open File and Calcuate BLAKE-512
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open file BLAKE512[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		fclose(fiCRC);
		_endthread();
		return;
	}


	memset( inbuf, 0x00, sizeof(inbuf) ); // 2018.06.15

	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] BLAKE512>> [%s] is hashing... \r\n"), gszPrtFileName );


	// --------------------------------------
	// Starting Checksum
	// --------------------------------------
	g_ChecksumCalcing = CHECKSUM_BLAKE512;


	//Initialize the BLAKE512 context
	blake512_init( &S ); 

	kll = 0;
	ulCRCFileRead = 0L;
	while((ll = fread(inbuf, 1, sizeof(inbuf), fiCRC)) > 0) 
	{
		ulCRCFileRead += ll;
		kll += ll;

		//Absorb input data
		blake512_update( &S, inbuf, ll ); 
	}

	//Finish absorbing phase
	blake512_final( &S, out ); 
	

	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }	
	


	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("	[%s] \r\n"), logfile ); // file name only


	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s hash is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");


	EB_Printf(TEXT("[dnw] "));


	memset( outTxt, 0x00, sizeof(outTxt) );
	for (ii = 0; ii < BLAKE512_LEN; ii++)
	{
		sprintf(&outTxt[ii*2], (char*)"%02x", out[ii]);
	}

	EB_Printf(TEXT("%s"), outTxt);
	fprintf(outFileCRC, "%s", outTxt);


	if(1==isFind)
	{
		EB_Printf(TEXT("  *BLAKE512*%s (%I64u Byte) \r\n"), gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "  *BLAKE512*%s (%I64u Byte) \r\n", gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("  *BLAKE512*%s (%I64u Byte) \r\n"), gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "  *BLAKE512*%s (%I64u Byte) \r\n", gszCRCFileName, ulCRCFileSize );
	}

	// Display Filename ---
	//fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));


	//EB_Printf(TEXT("[dnw] ------------------------------------------------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);
	/* _endthread given to terminate */
	_endthread();
}


void CheckSum_KR_HAS160(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};
	int ii=0;



	memset( logfile, 0x00, sizeof(logfile) );

	{
	uint8_t	DigestValue[HAS160_DIGEST_VALUELEN];
	char	DigestValueTxt[HAS160_DIGEST_VALUELEN*2] = {0,};
	uint8_t	SrcData[1024+32];
	HAS160_ALG_INFO	AlgInfo;
	

	__int64 	kll;
	size_t 		ll;

	
	memset( DigestValue, 0x00, sizeof(DigestValue) );  
	memset( SrcData, 0x00, sizeof(SrcData) );  

	// 3. open File and Calcuate SHAKE-256
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open file KR_HAS160[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		fclose(fiCRC);
		_endthread();
		return;
	}


	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] KR_HAS160>> [%s] is hashing... \r\n"), gszPrtFileName );


	// --------------------------------------
	// Starting Checksum
	// --------------------------------------
	g_ChecksumCalcing = CHECKSUM_KR_HAS160;

	
	//Initialize the Korea HAS160 context
	HAS160_Init(&AlgInfo);

	kll = 0;
	ulCRCFileRead = 0L;
	while((ll = fread(SrcData, 1, sizeof(SrcData), fiCRC)) > 0) 
	{
		ulCRCFileRead += ll;
		kll += ll;

		//Absorb input data
		HAS160_Update(&AlgInfo, SrcData, ll);
	}

	//Finish absorbing phase
	HAS160_Final(&AlgInfo, DigestValue);


	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }	
	


	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("	[%s] \r\n"), logfile ); // file name only


	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s hash is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");


	EB_Printf(TEXT("[dnw] "));


#if 0
	EB_Printf(TEXT("\r\n\r\n") );
	for (ii = 0; ii < HAS160_DIGEST_VALUELEN; ii++)
	{
		// sprintf(&DigestValueTxt[ii*2], (char*)"%02x", DigestValue[ii]);
		EB_Printf(TEXT("%02x"), DigestValue[ii]);
	}
	EB_Printf(TEXT("\r\n\r\n") );
#endif


	memset( DigestValueTxt, 0x00, sizeof(DigestValueTxt) );
	for (ii = 0; ii < HAS160_DIGEST_VALUELEN; ii++)
	{
		sprintf(&DigestValueTxt[ii*2], (char*)"%02x", DigestValue[ii]);
	}

	EB_Printf(TEXT("%s"), DigestValueTxt);
	fprintf(outFileCRC, "%s", DigestValueTxt);


	if(1==isFind)
	{
		EB_Printf(TEXT("  *KR_HAS160*%s (%I64u Byte) \r\n"), gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "  *KR_HAS160*%s (%I64u Byte) \r\n", gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("  *KR_HAS160*%s (%I64u Byte) \r\n"), gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "  *KR_HAS160*%s (%I64u Byte) \r\n", gszCRCFileName, ulCRCFileSize );
	}

	// Display Filename ---
	//fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));


	//EB_Printf(TEXT("[dnw] ------------------------------------------------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);
	/* _endthread given to terminate */
	_endthread();
}



#ifdef __cplusplus
extern "C" {
#endif

extern void tiger(unsigned long *str, unsigned long length, unsigned long res[6]);
//extern void tiger(unsigned long long int *str, unsigned long long int length, unsigned long long int res[3]);

#ifdef __cplusplus
}
#endif

void CheckSum_Tiger1995(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};
	int ii=0;



	memset( logfile, 0x00, sizeof(logfile) );

	{
	//typedef unsigned long long int word64;
	typedef unsigned long word32;

	unsigned char buffer[65536];
	//unsigned __int64 res[3];
	unsigned long res[6];

	
	char	DigestValueTxt[8*6+1] = {0,};
	__int64 	kll;
	size_t 		ll;

	
	memset( buffer, 0x00, sizeof(buffer) );  

	// 3. open File and Calcuate TIGER
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open file TIGER[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		fclose(fiCRC);
		_endthread();
		return;
	}


	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] TIGER>> [%s] is hashing... \r\n"), gszPrtFileName );


	// --------------------------------------
	// Starting Checksum
	// --------------------------------------
	g_ChecksumCalcing = CHECKSUM_TIGER1995;

	

	kll = 0;
	ulCRCFileRead = 0L;
	while((ll = fread(buffer, 1, sizeof(buffer), fiCRC)) > 0) 
	{
		ulCRCFileRead += ll;
		kll += ll;

		
		/* Hash of a 64K byte string */
		//for (ii=0;ii<65536;ii++) buffer[ii] = ii&0xFF;

		//tiger( (word64*)buffer, (word64)ll /*65536*/, res);
		tiger( (word32*)buffer, (word32)ll /*65536*/, res);

		memset(buffer, 0x00, sizeof(buffer) );
		
	}


	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }	
	


	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("	[%s] \r\n"), logfile ); // file name only


	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s hash is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");


	EB_Printf(TEXT("[dnw] "));


	memset( DigestValueTxt, 0x00, sizeof(DigestValueTxt) );

#if 0
	//for (ii = 0; ii < HAS160_DIGEST_VALUELEN; ii++)
	{
		sprintf(&DigestValueTxt[0], "%08x%08x%08x%08x%08x%08x",
					   (word32)(res[0]>>32),
					   (word32)(res[0]),
					   (word32)(res[1]>>32),
					   (word32)(res[1]),
					   (word32)(res[2]>>32),
					   (word32)(res[2]) );
	}
#else
	//for (ii = 0; ii < HAS160_DIGEST_VALUELEN; ii++)
	{
		sprintf(&DigestValueTxt[0], "%08x%08x%08x%08x%08x%08x",
					(word32)(res[1]),
					(word32)(res[0]),
					(word32)(res[3]),
					(word32)(res[2]),
					(word32)(res[5]),
					(word32)(res[4]) );
	}
#endif

	




	EB_Printf(TEXT("%s"), DigestValueTxt);
	fprintf(outFileCRC, "%s", DigestValueTxt);


	if(1==isFind)
	{
		EB_Printf(TEXT("  *TIGER*%s (%I64u Byte) \r\n"), gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "  *TIGER*%s (%I64u Byte) \r\n", gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("  *TIGER*%s (%I64u Byte) \r\n"), gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "  *TIGER*%s (%I64u Byte) \r\n", gszCRCFileName, ulCRCFileSize );
	}

	// Display Filename ---
	//fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));


	//EB_Printf(TEXT("[dnw] ------------------------------------------------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);
	/* _endthread given to terminate */
	_endthread();
}






#define RMD160_INBUF_SIZ 		1024
#define RMD160_BUF_LEN 			5 // 160/32
#define RMD160_LENGTH 			20 // 160/8

#include ".\RIPEMD\rmd160.h"  

void CheckSum_RIPEMD160(void *args)
{
	FILE *fiCRC=NULL;
	char logfile[MAX_PATH] = {0,};
	int ii=0;

	memset( logfile, 0x00, sizeof(logfile) );

	{

	typedef unsigned long word32;

	uint8_t inbuf[RMD160_INBUF_SIZ];
	char outTxt[RMD160_INBUF_SIZ*2]={0,}; 

	word32		  MDbuf[RMD160_BUF_LEN];   /* contains (A, B, C, D(, E))   */
	unsigned char hashcode[RMD160_LENGTH]; /* for final hash-value		   */

	__int64 	kll;
	size_t 		nbytes; // ll;
	word32		  X[16];			   /* current 16-word chunk 	   */
	unsigned int  i, j; 			   /* counters					   */
	word32		  length[2];		   /* length in bytes of message   */
	word32		  offset;			   /* # of unprocessed bytes at    */

	

	// 3. open File and Calcuate RMD160
	fiCRC = fopen( (const char*)gszCRCFileName, "rb");
	if( NULL == fiCRC )
	{
		EB_Printf(TEXT("[dnw] can not open file RMD160[%s].. Err(%u)\n"), gszCRCFileName, GetLastError() );
		fclose(fiCRC);
		_endthread();
		return;
	}


	memset( inbuf, 0x00, sizeof(inbuf) ); // 2018.06.15

	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));
	EB_Printf(TEXT("[dnw] RMD160>> [%s] is hashing... \r\n"), gszPrtFileName );


	// --------------------------------------
	// Starting Checksum
	// --------------------------------------
	g_ChecksumCalcing = CHECKSUM_RIPEMD160;


	//Initialize the RIPEMD-160 context
	MDinit(MDbuf);
	length[0] = 0;
	length[1] = 0;
	


	kll = 0;
	ulCRCFileRead = 0L;
	while((nbytes = fread(inbuf, 1, sizeof(inbuf), fiCRC)) > 0) 
	{
		ulCRCFileRead += nbytes;
		kll += nbytes;

		//Absorb input data
		/* process all complete blocks */
		for (i=0; i<(nbytes>>6); i++) {
		   for (j=0; j<16; j++)
			  X[j] = BYTES_TO_DWORD(inbuf+64*i+4*j);
		   compress(MDbuf, X);
		}
		/* update length[] */
		if (length[0] + nbytes < length[0])
		   length[1]++; 				 /* overflow to msb of length */
		length[0] += nbytes;


	}

	//Finish absorbing phase

	/* finish: */
	offset = length[0] & 0x3C0;   /* extract bytes 6 to 10 inclusive */
	MDfinish(MDbuf, inbuf+offset, length[0], length[1]);

  

	if(fiCRC)  { fclose(fiCRC);  fiCRC=NULL; }	
	


	// ----------------------------
	strcpy(logfile,dnwCurrPath);
	strcat(logfile,CRC_FILENAME);

	outFileCRC = fopen(logfile, "a"); // "a+");
	//EB_Printf(TEXT("	[%s] \r\n"), logfile ); // file name only


	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");
	GetLocalTime( &mCRCSysTime );
	fprintf(outFileCRC, "%s hash is created at %04d/%02d/%02d %02d:%02d:%02d \n", 
						szCRCtype[g_ChecksumCalcing], 
						mCRCSysTime.wYear, mCRCSysTime.wMonth, mCRCSysTime.wDay, mCRCSysTime.wHour, mCRCSysTime.wMinute, mCRCSysTime.wSecond );
	fprintf(outFileCRC, "--------------------------------------------------------------------------------\n");


	EB_Printf(TEXT("[dnw] "));


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


	EB_Printf(TEXT("%s"), outTxt);
	fprintf(outFileCRC, "%s", outTxt);


	if(1==isFind)
	{
		EB_Printf(TEXT("  *RMD160*%s (%I64u Byte) \n"), gszPrtFileName, ulCRCFileSize ); // file name only
		fprintf(outFileCRC, "  *RMD160*%s (%I64u Byte) \n", gszPrtFileName, ulCRCFileSize );
	}
	else
	{
		EB_Printf(TEXT("  *RMD160*%s (%I64u Byte) \n"), gszCRCFileName, ulCRCFileSize ); // full path
		fprintf(outFileCRC, "  *RMD160*%s (%I64u Byte) \n", gszCRCFileName, ulCRCFileSize );
	}

	// Display Filename ---
	//fprintf(outFileCRC, "\r\n");
	fclose(outFileCRC);
	EB_Printf(TEXT("[dnw] ---------------------------------------------------------------------------------- \r\n"));


	//EB_Printf(TEXT("[dnw] ------------------------------------------------------------------------------------------------- dnw/dnw_checksum.txt is saved -- \r\n"));

	}
	g_ChecksumCalcing = CHECKSUM_UNKNOWN;
	ulCRCFileRead = 0UL;

	BeepSound(1);
	/* _endthread given to terminate */
	_endthread();
}


#ifdef __TEST__POPUP__
BOOL CALLBACK ChecksumProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR szTitle[FILENAMELEN] = {0,};
	TCHAR tUSBspeed[32] = {0,};
	int lo_wParam, lo_lParam;

	lo_wParam = LOWORD(wParam); // warning fix
	lo_lParam = LOWORD(lParam);

	switch(message)
	{
	    case WM_INITDIALOG:

			if( 1 ) // USB_TX_SIZE >= (4096*4) ) ///   TX_SIZE_1 ) /* 2013.04.22 */
			{
				lstrcpy(szTitle,TEXT("Downloading "));

				//////////////2010.5.27 added ////////////////////
				sprintf(tUSBspeed,"[%dK] ", (1024/1024) );
				lstrcat(szTitle, tUSBspeed);
				////////////////////////////////////////////////
				
				lstrcat(szTitle,szFileName);
				SetWindowText(hDlg,szTitle);
				_hDlgDownloadProgress=hDlg;
			}
			else
			{ 
				EB_Printf(TEXT("[dnw] DownloadProgressProc() error, szFileName=%s \r\n"),  szFileName );
			}
			return TRUE;
		
	    case WM_CLOSE: 
			//If the [X] button is clicked, WM_CLOSE message is received.
			//Originally, DefWindowProc() called DestroyWindow();
			//But, there is no DefWindowProc(), We have to process WM_CLOSE message.
			if( hDlg )
			{
				DestroyWindow(hDlg); 
				CloseHandle(hDlg); 
				hDlg=NULL; 
			} /// added.

			if( _hDlgDownloadProgress ) 
			{
				DestroyWindow(_hDlgDownloadProgress);
				CloseHandle(_hDlgDownloadProgress);
				_hDlgDownloadProgress=NULL;
			}
			//break;
			return TRUE; //why not?
    }

    return FALSE;
}
#endif 


void BeepSound(int beeptype)
{
	BOOL bRet = 0;

	switch(beeptype)
	{
		case 1: // OK1
			bRet = MessageBeep( MB_ICONINFORMATION ); 
			break;
			
		case 2: // OK1
			bRet = MessageBeep( MB_ICONASTERISK );
			break;

		case 3: // OK2
			bRet = MessageBeep( MB_ICONEXCLAMATION );
			break;
	
		case 4: // Error
			bRet = MessageBeep( MB_ICONERROR ); 
			break;
	
		case 5:
			bRet = MessageBeep( MB_ICONHAND );
			break;
	
		case 6:
			bRet = MessageBeep( MB_ICONQUESTION );
			break;
	
		case 7:
			bRet = MessageBeep( MB_ICONSTOP );
			break;
	
		case 8:
			bRet = MessageBeep( MB_ICONWARNING );
			break;
	
		case 9:
			bRet = MessageBeep( MB_OK );
			break;
	}
}


void MenuCheckSum(HWND hwnd, int checksum_type)
{
	uintptr_t threadResult;
	BOOL result = FALSE;
	HANDLE hFile = NULL;
	//TCHAR  szOpenTxt[MAX_PATH] = {0,};
	
	if( g_ChecksumCalcing )
	{
		EB_Printf(TEXT("[dnw] %s hash is processing (%I64u Byte)... wait! \r\n"), szCRCtype[g_ChecksumCalcing], ulCRCFileRead ); // , gszPrtFileName );
		return;
	}



	memset( gszCRCFileName, 0x00, sizeof(gszCRCFileName) );
	memset( szCRCTitleName, 0x00, sizeof(szCRCTitleName) );
	memset( gszPrtFileName, 0x00, sizeof(gszPrtFileName) );

#if 0
	if( (checksum_type > CHECKSUM_UNKNOWN) && (checksum_type < CHECKSUM_MAX) )
		wsprintf(szOpenTxt, TEXT("Select file for %s hash..."), szCRCtype[checksum_type] );
	else
		wsprintf(szOpenTxt, TEXT("Select file for Hash...") );

	result =  PopFileMultiOpenDlg(hwnd, gszCRCFileName, szCRCTitleName, szOpenTxt);

#else
	// -----------------------------------------------------------------
	// 1. popup
	// -----------------------------------------------------------------
	result =  PopFileMultiOpenDlg(hwnd, gszCRCFileName, szCRCTitleName );
#endif


	// -----------------------------------------------------------------
	// 2. Display Filename ---------------------------------------------
	iidx = strlen(gszCRCFileName);
	isFind=0;
	for(jidx=iidx; iidx>0; jidx--)
	{
		if( gszCRCFileName[jidx] == '\\' ) { isFind=1; break; }
	}
	strcpy( gszPrtFileName, &gszCRCFileName[jidx+1] );
	// -----------------------------------------------------------------

	//EB_Printf(TEXT("[dnw] file[%s][%s]. Len(%d) \n"), gszPrtFileName, gszCRCFileName, iidx );

	// -----------------------------------------------------------------
	// 3. File Read and size
	// -----------------------------------------------------------------
	hFile = CreateFile(gszCRCFileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
	{
		DWORD dwError = GetLastError();
		switch(dwError)
		{
			case ERROR_PATH_NOT_FOUND: // 3 (0x3)
				//EB_Printf(TEXT("[dnw] can not open file[%s]. The system cannot find the specified path. \n"), gszCRCFileName );
				break;

			case ERROR_SHARING_VIOLATION: // 32 (0x20)
				EB_Printf(TEXT("[dnw] can not open file[%s]. Because it is being used by another process. Len(%d) \r\n"), gszPrtFileName, iidx );
				break;			
			
			case ERROR_FILE_NOT_FOUND: // 2 (0x2)
				EB_Printf(TEXT("[dnw] The system cannot find the file specified. [%s] Len(%d) \n"), gszPrtFileName, iidx );
				break;

			default:
				EB_Printf(TEXT("[dnw] can not open file[%s]. Len(%d) Err(%u)\n"), gszPrtFileName, iidx, dwError );
				break;
		}

		if( iidx >= FILENAMELEN )
		{
			EB_Printf(TEXT("[dnw] Too long path and filename[%s]. Len(%d) \n"), gszCRCFileName, iidx );
		}

		return;
	}

#if 0 // 2016.10.11
	ulCRCFileSize = GetFileSize(hFile,NULL);
#else
	ulCRCFileSize = GetFileExSize(hFile);
#endif

	if(hFile)  { CloseHandle(hFile);  hFile=NULL; }




	// -----------------------------------------------------
	// -----------------------------------------------------
	// -- Checksum Type
	// -----------------------------------------------------
	// -----------------------------------------------------

	if( CHECKSUM_MD5 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_MD5, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{

		}
		else
		{
			EB_Printf("[dnw] can not create MD5 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}
	}
	else if( CHECKSUM_SHA1 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_SHA1, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			// OK - Run Thread
		}
		else
		{
			EB_Printf("[dnw] can not create SHA1 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}

	}
	else if( CHECKSUM_CRC32 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_CRC32, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			// OK - Run Thread
		}
		else
		{
			EB_Printf("[dnw] can not create CRC32 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}

	}
	else if( CHECKSUM_CRC64 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_CRC64, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			// OK - Run Thread
		}
		else
		{
			EB_Printf("[dnw] can not create CRC64 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}

	}
	else if( CHECKSUM_SHA2_256 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_SHA2_256, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			// OK - Run Thread
		}
		else
		{
			EB_Printf("[dnw] can not create SHA2-256 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}

	}
	else if( CHECKSUM_SHA2_384 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_SHA2_384, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			// OK - Run Thread
		}
		else
		{
			EB_Printf("[dnw] can not create SHA2-384 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}

	}
	else if( CHECKSUM_SHA2_512 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_SHA2_512, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			// OK - Run Thread
		}
		else
		{
			EB_Printf("[dnw] can not create SHA2-512 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}

	}
	else if( CHECKSUM_SHA2_224 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_SHA2_224, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			// OK - Run Thread
		}
		else
		{
			EB_Printf("[dnw] can not create SHA2-224 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}

	}
	else if( CHECKSUM_MD6 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_MD6, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			// OK - Run Thread
		}
		else
		{
			EB_Printf("[dnw] can not create MD6 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}

	}
#if 1 // 2017.08.22, SHA3 KECCAK 512
	else if( CHECKSUM_SHA3_KECCAK_224 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_SHA3_KECCAK_224, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			// OK - Run Thread
		}
		else
		{
			EB_Printf("[dnw] can not create SHA3-KECCAK 224 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}
	}
	else if( CHECKSUM_SHA3_KECCAK_256 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_SHA3_KECCAK_256, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			// OK - Run Thread
		}
		else
		{
			EB_Printf("[dnw] can not create SHA3-KECCAK 256 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}
	}
	else if( CHECKSUM_SHA3_KECCAK_384 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_SHA3_KECCAK_384, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			// OK - Run Thread
		}
		else
		{
			EB_Printf("[dnw] can not create SHA3-KECCAK 384 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}
	}
	else if( CHECKSUM_SHA3_KECCAK_512 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_SHA3_KECCAK_512, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			// OK - Run Thread
		}
		else
		{
			EB_Printf("[dnw] can not create SHA3-KECCAK 512 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}
	}
	else if( CHECKSUM_SHAKE_128 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_SHAKE128, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			// OK - Run Thread
		}
		else
		{
			EB_Printf("[dnw] can not create SHAKE-128 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}
	}
	else if( CHECKSUM_SHAKE_256 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_SHAKE256, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			// OK - Run Thread
		}
		else
		{
			EB_Printf("[dnw] can not create SHAKE-256 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}
	}
	else if( CHECKSUM_BLAKE224 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_Blake224, THREAD_STACK_SIZE,(void *)0);
	
		if(threadResult!=-1)
		{
			// OK - Run Thread
		}
		else
		{
			EB_Printf("[dnw] can not create BLAKE-224 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}
	}
	else if( CHECKSUM_BLAKE256 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_Blake256, THREAD_STACK_SIZE,(void *)0);
	
		if(threadResult!=-1)
		{
			// OK - Run Thread
		}
		else
		{
			EB_Printf("[dnw] can not create BLAKE-256 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}
	}
	else if( CHECKSUM_BLAKE384 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_Blake384, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			// OK - Run Thread
		}
		else
		{
			EB_Printf("[dnw] can not create BLAKE-384 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}
	}
	else if( CHECKSUM_BLAKE512 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_Blake512, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			// OK - Run Thread
		}
		else
		{
			EB_Printf("[dnw] can not create BLAKE-512 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}
	}
	else if( CHECKSUM_KR_HAS160 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_KR_HAS160, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			// OK - Run Thread
		}
		else
		{
			EB_Printf("[dnw] can not create KR_HAS160 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}
	}
	else if( CHECKSUM_TIGER1995 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_Tiger1995, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			// OK - Run Thread
		}
		else
		{
			EB_Printf("[dnw] can not create Tiger1995 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}
	}
	else if( CHECKSUM_RIPEMD160 == checksum_type )
	{
		threadResult = _beginthread( (void (*)(void *))CheckSum_RIPEMD160, THREAD_STACK_SIZE,(void *)0);

		if(threadResult!=-1)
		{
			// OK - Run Thread
		}
		else
		{
			EB_Printf("[dnw] can not create RIPEMD160 thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
			_endthread(); // 2017.09.14
			return;
		}
	}
#endif
	else
	{
		EB_Printf("[dnw] can not create checksum thread! Err:(%u) \r\n", GetLastError());
		return;
	}

}



void MenuSaveLog(HWND hwnd)
{

	if( 0==msgSaveOnOff )
	{
		msgSaveOnOff = 1;

		LOGFile_open();


	#if LOG_SAVE_BY_THREAD // 2016.02.25
		qindex = 0;
		//memset( buf2write, 0x00, sizeof(buf2write)*sizeof(TCHAR) );
		memset( buf2write, 0x00, (FILE_WRITE_QUEUE_SIZE*STRING_LEN+4096) );
	#endif

	}
	else // if( 1==msgSaveOnOff )
	{
		LOGFile_close();

		msgSaveOnOff = 0;
	
	#if LOG_SAVE_BY_THREAD // 2016.02.25
		qindex = 0;
		//memset( buf2write, 0x00, sizeof(buf2write)*sizeof(TCHAR) );
		memset( buf2write, 0x00, (FILE_WRITE_QUEUE_SIZE*STRING_LEN+4096) );
	#endif
	
	}


#if DNW_2ND_WINDOWS_ENABLE // 2016.10.24
	if( OKstatusBarInitial )
	{
		if( BAR_STATUS==isStatusBarEnable ) 
		{
			StatusBarSaveLog();
		}

	#if DNW_2ND_COMMAND // 2017.4.22
	#endif

	}
#endif

}




#if LOG_SAVE_BY_THREAD // 2016.02.25
void ThreadLogSaveFunc(void *args)
{
	/* log file save */
	if( msgSaveOnOff )
	{
		if( fFileWrite && args )
		{
	
		#if 1
			int Length = lstrlen( (TCHAR*)args );

			// fseek(fFileWrite, 0L, SEEK_END); // 2016.02.28 end of file pointer 
			fwrite( args, sizeof(TCHAR), Length, fFileWrite); 
		#else
			fwrite( args, sizeof(unsigned char), strlen((char*)args), fFileWrite); 
		#endif


		#if !LOG_SAVE_BY_TIMER  // 2016.02.24,  
			/* 2012.07.07 fflush() 사용으로 변경.... 출력버퍼는 즉시 처리가 된다... flclose() 대신 사용 가능 */
			/// NEVER use-- 2016.02.26 -- fflush(fFileWrite); 
		#endif

			// memset( buf2write[qindex], 0x00, sizeof(buf2write[qindex]) );
			memset( buf2write[qindex], 0x00, STRING_LEN+4096 );

		}
	}

	//EB_Printf("[dnw] Thread Ended \r\n");
	/* _endthread given to terminate */
	_endthread();
}
#endif // LOG_SAVE_BY_THREAD



void DoBeforeExit(HWND hwnd)
{
	int 		tID=0; /* eTimerID */


	for(tID = TIMER_ID_WIN_UPDATE; tID <TIMER_ID_MAX; tID ++)
	{
		KillTimer(hwnd, tID);
	}



	LOGFile_close();
	Quit(hwnd);
	ReleaseFont();
	downloadCanceled = 1;

#if 0 // 2016.02.12
	ScrClear(hwnd); /// 2013.04.10, added.
#else
	EditBufferClear(hwnd); // 2016.02.12
#endif


#if USE_WIN_OUTOF_AREA_TO_ZERO // 2016.03.31 LCD size
	// 2016.03.31
	// EB_Printf(TEXT("[dnw] +++2++++ :[%d, %d] \n"), WIN_XBgn, WIN_YBgn );


	//myLCDwidth  = GetSystemMetrics(SM_CXSCREEN);
	//myLCDheight = GetSystemMetrics(SM_CYSCREEN);

	//myVirWidth  = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	//myVirHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);


	//if( (WIN_XBgn > myLCDwidth-WIN_LIMIT_X_MARGIN) || WIN_XBgn < 0 ) 
	if( (WIN_XBgn > myVirWidth-WIN_LIMIT_X_MARGIN) || WIN_XBgn < 0 ) // 2017.08.01, : myLCDwidth -> myVirWidth
	{ 
		WIN_XBgn = 0; 
		//WIN_XSIZE = WINDOW_XSIZE;
		//WIN_YSIZE = WINDOW_YSIZE;
	}

	if( (WIN_YBgn > myLCDheight-WIN_LIMIT_Y_MARGIN) || WIN_YBgn < 0 ) 
	{ 
		WIN_YBgn = 0;
	}
		
#endif



	WriteUserConfig(); // 2016.04.12

	WriteQuickUMONConfig(); // 2016.04.12

	WriteQuickUserConfig(); // 2017.04.11
	

#if 1 // 2016.04.03
	SetRegistry(); /* Saved Last Windows size */
#else
	ThreadSetRegistry(NULL);
#endif

#if USE_RX_TX_QUEUE
						
	// Destory --
	UnCQueue(&Rx2Tx, MAX_QUEUE_SIZE);
#endif




#if KEYIN_HISTORY_SAVE // 2017.8.28

	if( OKstatusBarInitial && (BAR_STATUS==isStatusBarEnable) && idxKeyTot ) // saving
	{
		int iix=0;
		FILE *fHis=NULL;

		fHis = fopen( (const char*)USER_TXDATA_FILE, "wb");
		if( NULL == fHis )
		{
			EB_Printf(TEXT("[dnw] can not append file [%s] Error(%u)\n"), USER_TXDATA_FILE, GetLastError() );
			fclose(fHis);
		}
		else
		{
			for(iix=0; (iix<idxKeyTot) && (iix<MAX_AT_COMMAND_NUM) ; iix++)
			{
				fwrite(szKeyAll[iix], sizeof(char), strlen(szKeyAll[iix]), fHis); 
			}
			fclose(fHis);
		}

		//MessageBox(_hwndEdit,TEXT("PAUSE !212!! \n"),TEXT("PAUSE !212!! \n"), MB_OK | MB_ICONINFORMATION );

	}
#endif // KEYIN_HISTORY_SAVE --------------





#if DNW_2ND_WINDOWS_ENABLE // 2016.10.28
	DestroyWindow( hWndStatusBar ); // 2016.10.28
	DestroyWindow( hWndCommandBar ); // 2017.04.22		
#endif
	DestroyWindow( _MainHwnd ); /// 2010.05.12 added.
	DestroyWindow( _hwndEdit ); /// 2010.05.12 added.
	DestroyWindow( _EditHwnd ); /// 2015.08.08 added.

#if 1 // 2017.4.25
	DestroyWindow( _hDlgTxData2UART ); /// 2017.4.25
	DestroyWindow( _hTxData2UART ); /// 2017.4.25


#if USE_FLOAT2HEX_VALUE // 2018.07.18
	DestroyWindow( _hDlgFlt2Hexa );
#endif


	DestroyWindow( _hDlgHex2Binary ); /// 2017.4.25
	DestroyWindow( _hHex2Binary ); /// 2017.4.25

	DestroyWindow( _hDlgDownloadProgress ); /// 2017.4.25
	DestroyWindow( _hProgressControl ); /// 2017.4.25

	DestroyWindow( _hDlgDownloadProgressUART ); /// 2017.4.25
	DestroyWindow( _hProgressControlUART ); /// 2017.4.25

	DestroyWindow( _hDlgSerialSettings ); /// 2017.4.25

	if( _hDlgTxData2UART )			{ CloseHandle( _hDlgTxData2UART );			_hDlgTxData2UART=NULL; } 
	if( _hTxData2UART ) 			{ CloseHandle( _hTxData2UART ); 			_hTxData2UART=NULL; } 


#if USE_FLOAT2HEX_VALUE // 2018.07.18
	if( _hDlgFlt2Hexa )			{ CloseHandle( _hDlgFlt2Hexa );					_hDlgFlt2Hexa=NULL; } 
#endif

	if( _hDlgHex2Binary )			{ CloseHandle( _hDlgHex2Binary );			_hDlgHex2Binary=NULL; } 
	if( _hHex2Binary )				{ CloseHandle( _hHex2Binary );				_hHex2Binary=NULL; } 

	if( _hDlgDownloadProgress ) 	{ CloseHandle( _hDlgDownloadProgress ); 	_hDlgDownloadProgress=NULL; } 
	if( _hProgressControl ) 		{ CloseHandle( _hProgressControl ); 		_hProgressControl=NULL; } 

	if( _hDlgDownloadProgressUART ) { CloseHandle( _hDlgDownloadProgressUART ); _hDlgDownloadProgressUART=NULL; } 
	if( _hProgressControlUART ) 	{ CloseHandle( _hProgressControlUART ); 	_hProgressControlUART=NULL; } 

	if( _hDlgSerialSettings )		{ CloseHandle( _hDlgSerialSettings );		_hDlgSerialSettings=NULL; } 
#endif


#if DNW_2ND_WINDOWS_ENABLE // 2016.10.28
	if( hWndStatusBar ) { CloseHandle( hWndStatusBar ); hWndStatusBar=NULL; } /// 2016.12.28
	if( hWndCommandBar ) { CloseHandle( hWndCommandBar ); hWndCommandBar=NULL; } // 2017.04.22
#endif
	if( _MainHwnd ) { CloseHandle( _MainHwnd ); _MainHwnd=NULL; }
	if( _hwndEdit ) { CloseHandle( _hwndEdit ); _hwndEdit=NULL; }
	if( _EditHwnd ) { CloseHandle( _EditHwnd ); _EditHwnd=NULL; } // 2015.08.08 added.

	
#ifdef COLOR_DISP2 /// 2010.05.14
	if( m_Brush ) { DeleteObject( m_Brush ); m_Brush = NULL; }
#endif

#if RICHED_COLOR // 2016.09.21 +++++++++
	FreeLibrary(hDll); /// added.
#endif

}





#if DNW_2ND_WINDOWS_ENABLE // 2016.10.24

void StatusBarSaveLog(void)
{
	TCHAR 	szText[MAX_PATH] = {0,};

#if 0 // 2017.08.08
	memset(szText, 0x00, sizeof(szText) );
	switch(userScrBufSizIndex)
	{
		case 0:
			lstrcpy(szText, TXT_SCR_HUGE );
			break;
		case 1:
			lstrcpy(szText, TXT_SCR_LARGE );
			break;
		case 2:
			lstrcpy(szText, TXT_SCR_MIDDLE );
			break;
		case 3:
			lstrcpy(szText, TXT_SCR_SMALL );
			break;
		case 4:
			lstrcpy(szText, TXT_SCR_SMALLEST );
			break;
		default:
			lstrcpy(szText, TXT_SCR_HUGE );
			break;
	}
	SendMessage(hWndStatusBar, SB_SETTEXT, STATUS_IDX2,(LPARAM)szText);
#endif

	memset(szText, 0x00, sizeof(szText) );
	if( msgSaveOnOff )
		lstrcpy(szText, MSG_LOGGING);
	else
		lstrcpy(szText, MSG_LOG_OFF);
	SendMessage(hWndStatusBar, SB_SETTEXT, STATUS_IDX4,(LPARAM)szText);


	ShowWindow(hWndStatusBar, SW_SHOW);
}



void MenuStatusClear(HWND hwnd)
{

#if KEYIN_HISTORY_SAVE // 2017.5.2

	if( !DeleteFile( TEXT(USER_TXDATA_FILE) ) ) 
	{
		DWORD dwErr = GetLastError();
		if( ERROR_FILE_NOT_FOUND == dwErr ) {
			//EB_Printf("[dnw] file not exist! Error:%d \r\n", dwErr );
		}
		else {
			//EB_Printf("[dnw] can not delete file[%s] Error:%d\r\n", TEXT(USER_TXDATA_FILE), dwErr );
		}
	}

	memset( szKeyAll, 0x00, sizeof(TCHAR)*MAX_AT_COMMAND_NUM*MAX_PATH );

	idxKeyTot = 0;
	idxCurrKey = 0;

	arrowKeyTouched = 0; // ignore arrow key
	key_index = 0;

	#if 0 // 2017.08.29
	memset(szTempKeyIn, 0x00, sizeof(szTempKeyIn) );
	SendMessage(hWndStatusBar, SB_SETTEXT, STATUS_IDX5,(LPARAM)szTempKeyIn);
	#else
	SendMessage(hWndStatusBar, SB_SETTEXT, STATUS_IDX5,(LPARAM)TXT_KEYIN_INIT);
	#endif
	
	ShowWindow(hWndStatusBar, SW_SHOW);

#endif // KEYIN_HISTORY_SAVE --------------
}


#if KEYIN_HISTORY_SAVE // 2017.4.25
int 	TxDataOnStatusBar(void)
{
	int iloop=0, iLen=0;

	iLen = lstrlen(szKeyAll[idxCurrKey]);

	for(iloop=0; iloop<iLen && iloop<MAX_PATH; iloop++)
	{
		WriteCommBlock( szKeyAll[idxCurrKey][iloop] );
	}	

	if( iLen && txDataMsgOnOff && (1==isConnected) )
	{
		// EB_Printf(TEXT("[dnw] DN clear : 0x%x (%d) [%s] \n"), arrowKeyTouched, iLen, szKeyAll[idxCurrKey] );
		EB_Printf(TEXT("[dnw] %s \n"), szKeyAll[idxCurrKey] );
		EB_Printf(TEXT("[dnw] OK \n\n") );
	}

	return iLen;
}


void 	CmdDisplayOnStatusBar(void)
{
	memset(&szTmpTxt[0], 0x00, sizeof(szTmpTxt) );
	wsprintf(&szTmpTxt[0], TEXT("[%d]%s"), idxCurrKey+1, szKeyAll[idxCurrKey]);
	SendMessage(hWndStatusBar, SB_SETTEXT, STATUS_IDX5,(LPARAM)szTmpTxt);
	ShowWindow(hWndStatusBar, SW_SHOW);
}


int 	CheckTxDataFileOnStatusBar(void) // 2017.08.28
{
	if( idxKeyTot > 0 ) return idxKeyTot;


	if( OKstatusBarInitial )
	{
		if( BAR_STATUS==isStatusBarEnable ) 
		{
			FILE *fHistory=NULL;
			int  iread=0, kidx=0, iLen=0;

			idxKeyTot = -1; // no file
			memset( szKeyAll, 0x00, sizeof(szKeyAll) );
			if( NULL == (fHistory = fopen( (const char*)USER_TXDATA_FILE, "r") ) )
			{
				idxKeyTot = 0; // None
				return 0;
			}

			// ----------------------
			// if history file is
			kidx = 0;
			while ( ( iread = fscanf(fHistory, "%s",  szKeyAll[kidx]) )>0 )
			{
				// EB_Printf(TEXT("[dnw] kidx[%d] ==> [%s]  iread (%d) \n"), kidx, szKeyAll[kidx], iread );

				iLen = lstrlen(szKeyAll[kidx]);
				szKeyAll[kidx][iLen] = TEXT(ENTER_KEY); // 저장할 경우 마지막에 ENTER 가 없으므로,

				kidx++;
				if( kidx >= MAX_AT_COMMAND_NUM ) break;
			}
			idxKeyTot = kidx; // read total count
			fclose(fHistory);

		}
	}
	return idxKeyTot;
}

#endif			


void StatusBarText(void)
{
	TCHAR 	szText[MAX_PATH] = {0,};
	static int prevBaudRate=0xff;
	static int prevuserScrBufSizIndex=0xff;
	static int prevmsgSaveOnOff=0xff;
	static int prevTextorHexaMode=0xff;
	static int prevFlowControl=0xff; 
	static int prevFlowCtrlVal=0xff;


#if DNW_2ND_WINDOWS_ENABLE // 2017.08.02

	if( BAR_NONE==isStatusBarEnable ) return;

#endif


	
	if(1==isConnected)
	{
		wsprintf(szText, TEXT("COM%d"), userComPort );
	}
	else 
	{
		wsprintf(szText, TEXT("Disconnect") );
		//SendMessage(hWndStatusBar, SB_SETBKCOLOR , STATUS_IDX0, (LPARAM)COLOR_RED );
	}
	SendMessage(hWndStatusBar, SB_SETTEXT, STATUS_IDX0 /*|SBT_OWNERDRAW|SBT_POPOUT*/, (LPARAM)szText);



	if( prevBaudRate != userBaudRate )
	{
		memset(szText, 0x00, sizeof(szText) );
		wsprintf(szText, TEXT("%s bps"), PRTComma(userBaudRate) );
		SendMessage(hWndStatusBar, SB_SETTEXT, STATUS_IDX1, (LPARAM)szText);

		prevBaudRate = userBaudRate;
	}


#if 0 // 2017.08.03
	if( prevuserScrBufSizIndex != userScrBufSizIndex )
	{
		memset(szText, 0x00, sizeof(szText) );
		switch(userScrBufSizIndex)
		{
			case 0:
				lstrcpy(szText, TXT_SCR_HUGE );
				break;
			case 1:
				lstrcpy(szText, TXT_SCR_LARGE );
				break;
			case 2:
				lstrcpy(szText, TXT_SCR_MIDDLE );
				break;
			case 3:
				lstrcpy(szText, TXT_SCR_SMALL );
				break;
			case 4:
				lstrcpy(szText, TXT_SCR_SMALLEST );
				break;
			default:
				lstrcpy(szText, TXT_SCR_HUGE );
				break;
		}

		SendMessage(hWndStatusBar, SB_SETTEXT, STATUS_IDX2,(LPARAM)szText);

		prevuserScrBufSizIndex = userScrBufSizIndex;
	}

#else

	if( (prevFlowControl != userFlowControl) || (userFlowControl && (prevFlowCtrlVal != userFlowCtrlVal) ) )
	{
		if( userFlowControl && userFlowCtrlVal )
		{
			memset(szText, 0x00, sizeof(szText) );

			if( FLOWC_DTR_DSR == (userFlowCtrlVal & FLOWC_DTR_DSR) )
			{
				if( (FLOWC_RTS_CTS==(userFlowCtrlVal & FLOWC_RTS_CTS)) || FLOWC_XON_XOFF==(userFlowCtrlVal & FLOWC_XON_XOFF) ) 
					lstrcpy(szText, TXT_FCTRL_DTR_DTS );
				else
					lstrcpy(szText, TXT_FCTRL_DTR_DTS2 );
			}

			if( FLOWC_RTS_CTS == (userFlowCtrlVal & FLOWC_RTS_CTS) ) 
			{
				if( FLOWC_XON_XOFF==(userFlowCtrlVal & FLOWC_XON_XOFF) ) 
					lstrcat(szText, TXT_FCTRL_RTS_CTS );
				else
					lstrcat(szText, TXT_FCTRL_RTS_CTS2 );
			}

			if( FLOWC_XON_XOFF == (userFlowCtrlVal & FLOWC_XON_XOFF) ) 
				lstrcat(szText, TXT_FCTRL_XON_XOFF2 );

		}
		else
		{
			lstrcpy(szText, TXT_FCTRL_NONE );
		}

		SendMessage(hWndStatusBar, SB_SETTEXT, STATUS_IDX2,(LPARAM)szText); 	
		prevFlowControl = userFlowControl;
		prevFlowCtrlVal = userFlowCtrlVal;

	}

#endif


	if( prevTextorHexaMode != TextorHexaMode )
	{
	//#if HEXA_MODE_SUPPORT /* 2011.02.08 Hexa mode display */
		memset(szText, 0x00, sizeof(szText) );
		if(0==TextorHexaMode)
			lstrcpy(szText, STA_TX_TEXT);
		else
			lstrcpy(szText, STA_TX_HEXA );
	//#endif
		SendMessage(hWndStatusBar, SB_SETTEXT, STATUS_IDX3,(LPARAM)szText);

		prevTextorHexaMode = TextorHexaMode;
	}


	if( prevmsgSaveOnOff != msgSaveOnOff )
	{
		// Log save 	
		memset(szText, 0x00, sizeof(szText) );
		if( msgSaveOnOff )
			lstrcpy(szText, MSG_LOGGING);
		else
			lstrcpy(szText, MSG_LOG_OFF);
		SendMessage(hWndStatusBar, SB_SETTEXT, STATUS_IDX4,(LPARAM)szText);

		prevmsgSaveOnOff = msgSaveOnOff;
	}


	
	if( 1 != isPressKey && (FALSE==isFileExist( TEXT(USER_TXDATA_FILE) )) ) // Press [F9] or [ENTER]
	{
		memset(szTempKeyIn, 0x00, sizeof(szTempKeyIn) );
		key_index = 0;

#if KEYIN_HISTORY_SAVE // 2017.4.25
		arrowKeyTouched = 0; // ignore arrow key
#endif

		SendMessage(hWndStatusBar, SB_SETTEXT, STATUS_IDX5,(LPARAM)TXT_KEYIN_INIT );

	}
	else
	{
		CheckTxDataFileOnStatusBar();

		arrowKeyTouched = ARR_KEY_RT; /* -- 2017.08.28, History command 가 존재하면 [ENTER] 시 전송한다...  */
		CmdDisplayOnStatusBar();
	}

	//EB_Printf(TEXT("[dnw] ==22== %d ;;; %d ==22== \n"), previdxKeyTot , idxKeyTot );


	ShowWindow(hWndStatusBar, SW_SHOW);

}
#endif // DNW_2ND_WINDOWS_ENABLE ++++++++++++++++++++++++++++




#if 0
//////////////////////////////

#include <Windows.h>
#include <stdio.h>
#include <CommCtrl.h>
 
#define IDD_DIALOG                      101
#define IDE_EDIT3                       1004
#define ID_OK                           1002
#define IDE_EDIT1                       1001
 
HWND hWndDialog  = 0;
int Flag = 1;
 
void Append(int nID, LPCTSTR buf, HWND hWndDlg)
{
    HWND hEdit = GetDlgItem(hWndDlg, nID);
    // Append Line Feed
    int ndx = GetWindowTextLength (hEdit);
    if (ndx)
    {
        SendMessage (hEdit, EM_SETSEL, (WPARAM)ndx, (LPARAM)ndx);
        SendMessage (hEdit, EM_REPLACESEL, 0, (LPARAM) ((LPCTSTR)"\r\n"));
    }
    // Append text
    ndx = GetWindowTextLength(hEdit);
    SendMessage (hEdit, EM_SETSEL, (WPARAM)ndx, (LPARAM)ndx);
    SendMessage (hEdit, EM_REPLACESEL, 0, (LPARAM) buf);
}
 
BOOL CALLBACK lpDialogFunc(HWND hWndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    WCHAR wstr[8000] = L"";
    char  str[8000]  = "";
 
    switch(msg)
    {
        case WM_INITDIALOG:
            ShowWindow(hWndDlg, SW_SHOW);
            hWndDialog = hWndDlg;   
            SetDlgItemText(hWndDlg, IDE_EDIT1, "Type Text here: ");         
            return TRUE;        
             
        case WM_CLOSE:
            DestroyWindow(hWndDlg);
            return TRUE;
             
        case WM_CTLCOLORSTATIC:
            if(Flag == 1)
            {
              SetTextColor((HDC)wParam, RGB(0,0,200));              
            }
            else           
            SetTextColor((HDC)wParam, RGB(200,0,0));            
             
            return TRUE; 
             
        case WM_CTLCOLOREDIT:
            SetTextColor((HDC)wParam, RGB(200,0,0));
            return TRUE;  
             
        case WM_DESTROY:
            EndDialog(hWndDlg, 0);
            PostQuitMessage(0);
            return TRUE;
 
        case WM_COMMAND:
            switch(wParam)
            {
                case ID_OK:
                    GetDlgItemText(hWndDlg, IDE_EDIT1, str, 8000); // Get Text  
                    Append(IDE_EDIT3, str,hWndDlg);     
                    SetDlgItemText(hWndDlg, IDE_EDIT1, "");       // Remove Text                                
                    
                    if(Flag == 1)
                    Flag = 0;
                    else
                    Flag = 1;
                     
                    //Bring control to Box Back
                    keybd_event(VK_TAB, VK_TAB, 0, 0);                  //Tab Press
                    keybd_event(VK_TAB, VK_TAB, KEYEVENTF_KEYUP, 0);    //Tab Release
                    break;
            }
            return TRUE;
 
        case WM_HSCROLL:            
            return TRUE;
 
    }   
    return FALSE;
}
 
 
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */
     
    HINSTANCE g_hInstance;
    HWND hWnd   = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG), HWND_DESKTOP, lpDialogFunc); // Create the dialog
    g_hInstance = hInstance;
    MSG msg;    
    while(GetMessage(&msg, NULL, 0, 0)) // Process all the Win32 messages
    {
        if(!IsDialogMessage(hWnd, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}




CHARFORMAT2 format;
format.cbSize = sizeof(format);

format.dwMask = CFM_UNDERLINETYPE | CFM_UNDERLINE;
format.dwEffects = CFE_UNDERLINE;
format.bUnderlineType = CFU_UNDERLINEWAVE;
format.bUnderlineColor = 0x05;

SendMessage(hWndEdit,EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &format);


void AddInfo2(HWND con, char* text)
{
  CHARFORMAT cf;
  int start, stop;

  memset( &cf, 0, sizeof cf );
  cf.cbSize = sizeof cf;
  cf.dwMask = CFM_COLOR;
  cf.crTextColor = RGB(51, 204, 51);

  SendMessage(con, EM_SETSEL, -1, -1);
  start = SendMessage(con, WM_GETTEXTLENGTH, 0, 0);
  SendMessage(con, EM_REPLACESEL, FALSE, (LPARAM)text);
  stop = SendMessage(con, WM_GETTEXTLENGTH, 0, 0);
  SendMessage(con, EM_SETSEL, start, stop);
  SendMessage(con, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
  SendMessage(con, EM_SETSEL, -1, -1);
}

SendMessage(_hwndEdit, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf_old);
cf_old.dwMask = CFM_COLOR;
SendMessage(_hwndEdit, EM_SETSEL, -1, -1);
SendMessage(_hwndEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf);
SendMessage(_hwndEdit, EM_REPLACESEL, FALSE, (LPARAM)text);
SendMessage(_hwndEdit, EM_SETSEL, -1, -1);
SendMessage(_hwndEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM) &cf_old);

#endif







