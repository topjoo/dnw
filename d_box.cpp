// D-Box.cpp
// ------------------------------------

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include "resource.h" /* warning fix */


#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <tchar.h>
#include <string.h>
#include <process.h>
#include <stdlib.h>
#include <math.h>

//#include </include/wx/ctb/win32/serport.h>
//#include <serport.h>


#include "def.h"
#include "d_box.h"
#include "dnw.h"
#include "engine.h"
#include "fileopen.h"
#include "regmgr.h" 
#include "rwbulk.h" 

#include <stdio.h>
#include <time.h>
#include <memory.h>

#if DNW_C100_INC_USB_BINARY // 2016.04.14
#include "c100_bl1_usb.txt" /// 2014.08.31, C100 USB download binary--
#include "c100_EBOOT_nb0.txt" // SLC NAND EBOOT
#include "c100_IROM_SDMMCBoot.txt" // 2015.11.9, iNAND IROM_SDBOOT binary
#endif



#define TXFILE_EXIT_COUNT 			30			

#define SKIP_NUM			2


FILE 	*fFileWrite = NULL;
unsigned char buff_fin[DATE_LEN] = { 0, };

extern char dnwCurrPath[];
extern char dnwDefaultPath[];

extern int isRegSave; // Registry Saving

BOOL isFileExist(TCHAR *file);
extern int   BinFileIndex; /// added.
extern WORD  DownloadedBin; /// added.

extern TCHAR szFileNameOnPopUP[FILENAMELEN];

extern TCHAR szFileName[FILENAMELEN];
extern TCHAR szUmonFileName[FILENAMELEN];
extern HWND _EditHwnd, _MainHwnd;
extern void SetFont(HWND hwndEdit);
extern void GetRegistry_USBAddr(void);
extern char *PRTComma(long long deci);
extern __int64 GetTxDataFileSize(HANDLE hFile);


#if defined(COLOR_DISP2) /// 2010.05.14
extern HBRUSH m_Brush; /// background color
extern DWORD ColorTable[]; ///COLOR_NUM_MAX
#endif

// 'volatile' is important because _hDlgDownloadProgress is accessed in two threads.
volatile HWND _hDlgDownloadProgress=NULL; // USB port Diaglog
HWND _hProgressControl;

volatile HWND _hDlgDownloadProgressUART=NULL; // 2016.10.12, COM port Diaglog
HWND _hProgressControlUART;

volatile HWND _hDlgSerialSettings=NULL;



int downloadCanceled = 1; // initial value :download is canceled by user.

extern volatile int isConnected; // 0:Disconnect, 1:Connected, 2:Try to connect

extern int isMenuTxing; // Completed File Transferred!!


extern int idBaudRate,userBaudRate;
extern int userComPort;
extern int autoSendKey, msgSaveOnOff, TextorHexaMode;
extern int FontType;
extern int RetryUSB; /// 2012.03.10
extern int CPUtype; /// 6410:0, SMDKC100:1
extern int sendTxDataType; // 2016.03.23
extern int txDataMsgOnOff; // off 

#define TX_STOP_WAIT_TIME 		5 // 5 ms


#if DISPLAY_PC_SYSTEM_TIME /*--- 2016.02.12 ------ */
extern int localTime;
extern int isTimeInfoOnce; // Time 정보를 맨 앞에 한번 표시한다. 
#endif

extern int cmdCRLF; // 2019.12.21
extern int LogFileType; // 2020.04.07


#if DNW_2ND_WINDOWS_ENABLE // 2017.04.13
extern int isStatusBarEnable;
extern int OKstatusBarInitial;
#endif


#if USE_HEX2BIN_CONVERT // 2016.03.04

void UserButtonEnable(HWND hWnd, int OnOff);
void UserButtonPadByteOnOff(HWND hDlgHex2Bin); // 2018.07.06

extern void ScrClear(HWND hwnd);


#define MAX_LENGTH_SIZE 			0x800000 // Max_Length 8MByte Limit
#define MAX_LENGTH_TXT 				TEXT("Max. Binary limit is 8 MBytes!")

#define UNIT_TXT_KBYTE 				"(KByte)"
#define UNIT_TXT_BYTE 				"(Byte)"
	

#define MAX_ERR_COUNT 				50

volatile HWND _hDlgHex2Binary=NULL;
HWND _hHex2Binary;


static int tempHexType, tempHex2BinLogOnOff;
static int tempCRCtype, tempEndian, tempCRCverify, tempSizeUnit;
static int tempSwapWordwise;
static int tempAddrAlignWord;
static int tempHexAddrZeroForced;
static int tempuserPadByteOnOff; // 2018.07.06

int dwHex2BinExit = 0;
int iCovertedCompleted = 1; // NOT run, 1: completed

extern int userHexType;
extern int userHex2BinLogOnOff;
extern int userCRCtype; // CRC8
extern int userEndian; // little
extern int userHexAddrZeroForced; // hex address zero-forced
extern int userAddrAlignWord;

extern int userCRCverify; // ON
extern int userSwapWordwise;
extern int userPadByteOnOff; // 2018.07.06


#if USE_FLOAT2HEX_VALUE // 2018.07.18
extern int iChoiceFlt2Hex, iFlt2HexEndian;
#endif


extern int userSizeUnit;
extern DWORD dwSizeUnit; // Byte or KByte




extern TCHAR szH2BFullInputFile[MAX_PATH];
extern TCHAR szH2BTitleInpFile[MAX_PATH];
extern TCHAR szH2BInpFile[MAX_PATH];

extern TCHAR szH2BFullOutputFile[MAX_PATH];
extern TCHAR szH2BOutFile[MAX_PATH];

extern TCHAR szH2BFullCrcOutFile[MAX_PATH];
extern TCHAR szH2BCRCFile[MAX_PATH];

extern DWORD dwHex2BinMaxSize;
extern TCHAR szHex2BinMaxSize[16];

extern int  dwPadByte;
extern TCHAR szPadByte[16];

int isPadByteAllArea = 0; // original version 2.5
int tempPadbyte = 0;


int iCheckedInputFile = 0; // Inputfile checking
int isHex2BinConvert4CRC = 0;

#endif

int   isPadValue = 0; // 0:OK else Error



#if defined(COLOR_DISP2) /// 2010.05.14
extern int ColorType; /// 2010.05.14 added
#endif

BOOL bBaudrate = FALSE;


extern int TotalROMfilenum, UmonFileNum;

extern DWORD downloadAddress;
extern TCHAR szDownloadAddress[ADDR_LENGTH];

extern TCHAR szUserBaudRate[16]; /// 2014.04.15

#if HEXA_MODE_SUPPORT	/* 2012.07.03 Hexa mode display */
extern DWORD ColumnNumber;
extern TCHAR szColumnNumber[16];
extern DWORD HexaEndChar;
TCHAR szHexaEnd[16] = {0,}; // 2016.04.12
#endif



#if USE_TXDATA_TO_SERIAL

#if 0
char		*TxScriptExample[100] = {
	";-----------------------------------------\r\n",
	";How to use Tx script. \r\n",
	"; 1) Modify Sample Tx Script. \r\n",
	"; 2) Connect COM port to send TxData. \r\n",
	"; 3) Press the [Ctrl+F6] key \r\n",
	"; 4) Select the modified Tx script file. \r\n",
	"; 5) Press [Send TxData] button. \r\n",
	"; * Max char is 4096 char. per line.\r\n",
	";-----------------------------------------\r\n\r\n\r\n",
	"[START] --------------------- \r\n\r\n",
	"[HEXA] 80 aa bb cc dd ee ff 11 00 22 33 \r\n"
	"[SLEEP] 2200 \r\n"
	"[CHAR] abcdefg qw qUIz xx\r\n",
	" \r\n",
	" \r\n",
	" \r\n",
	"[DO] ;+++++#1+++++ \r\n\r\n",
	"[HEXA] 11 2233 80 aa bb cc dd ee ff 11 00 22 33 \r\n",
	"[sleep] 1000 \r\n",
	"[WHILE] 5 ;++#1++++ \r\n\r\n",
	" \r\n\r\n",
	";[HEXA] 11 2233 80 aa bb cc dd ee ff 11 00 22 33 --> SKIP because of 1st column ; \r\n",
	"  [HEXA] 11 2233 80 aa bb cc dd ee ff 11 00 22 33 --> SKIP because of 1st column SPACE \r\n\r\n",
	"[sleep] 500 \r\n\r\n"
	"[hexa] 11 22,33 80 aa bb cc dd ee ff 11 00 22 33\r\n\r\n",
	"\r\n\r\n",
	"[do] ;===== #2 ===== \r\n",
	"[char]abcdefg!@#$%^&*()_+=-09876543211234567890abcdef   \r\n\r\n",
	"[while] 100 ; ===#2==== \r\n",
	"\r\n",
	"[END] --------------------- \r\n\r\n",
	"\r\n",

	NULL
};
#endif


#if 0 // 2018.03.08
const char		*TxScript_BLT1[200] = {
	";-----------------------------------------\r\n",
	";How to use Tx script. \r\n",
	"; 1) Modify BLT1 Tx Script file. \r\n",
	"; 2) Connect COM port to send TxData. \r\n",
	"; 3) Press the [Ctrl+F6] key \r\n",
	"; 4) Select the modified Tx script file. \r\n",
	"; 5) Press [Send TxData] button. \r\n",
	"; * Max Char. are 4096 Char. per line.\r\n",
	";-----------------------------------------\r\n\r\n\r\n",
	"[START] \r\n\r\n",
	";====================================== \r\n",
	";========= BLT 1 TEST MODE ============ \r\n",
	";====================================== \r\n",
	"\r\n",
	"[DO] \r\n",
	"\r\n",
	"[SLEEP] 12000 ; Unit: msec \r\n",
	"\r\n",
	"\r\n",
	";========= BLT 1: Req to Move to Diag Mode\r\n",
	"[HEXA] AA 88 01 00 23 \r\n",
	"[SLEEP] 15000 \r\n",
	"\r\n",
	"\r\n",
	";======= System Mode ====== \r\n",
	";======= BLT 1: Req S/W Version(MCU, System, Map, DMB, GPS) \r\n",
	"[HEXA] AA 88 0d 00 2F \r\n",
	"[SLEEP] 3000  \r\n",
	"\r\n",
	";======= BLT 1: Req H/W PCB Version \r\n",
	"[HEXA] AA 88 0e 00 2C \r\n",
	"[SLEEP] 5000 \r\n",
	"\r\n",
	"\r\n",
	";======= Start to NV Write == \r\n",
	";======= BLT 1: Write BSN Info \r\n",
	"[HEXA] AA 88 3A 01 02 1B \r\n",
	"[SLEEP] 5000 \r\n",
	"\r\n",
	"\r\n",
	";======= BLT 1: Write GPS S/N \r\n",
	"[HEXA] AA 88 3A 01 04 1D \r\n",
	"[SLEEP] 5000 \r\n",
	"\r\n",
	"\r\n",
	";======= BLT 1: Write DMB S/N \r\n",
	"[HEXA] AA 88 3A 01 05 1C \r\n",
	"[sleep] 5000 \r\n",
	"; ===== System End == \r\n",
	"\r\n",
	"\r\n",
	";======= TDMB Mode ====== \r\n",
	";======= BLT 1: Req to DMB Screen \r\n",
	"[HEXA] AA 88 21 00 03 \r\n",
	"[sleep] 5000 \r\n",
	"\r\n",
	"\r\n",
	";====== BLT 1: Req Dmb Scan \r\n",
	"[HEXA] AA 88  22 01 FF FE \r\n",
	"[sleep] 32000 \r\n",
	"\r\n",
	"\r\n",
	";====== BLT 1: Req DMB SNR/CER  \r\n",
	"[HEXA] AA 88 05 00 27 \r\n",
	"[sleep] 5000 \r\n",
	"\r\n",
	"\r\n",
	";===== BLT 1: Req DMB RSE ON \r\n",
	"[HEXA] AA 88 06 00 24 \r\n",
	"[sleep] 15000 \r\n",
	"\r\n",
	"\r\n",
	";===== BLT 1: Req DMB RSE OFF \r\n",
	"[HEXA] AA 88 07 00 25 \r\n",
	"[sleep] 15000 \r\n",
	"\r\n",
	"\r\n",
	";======= DR/GPS Mode ====== \r\n",
	";======= BLT 1: Req GPS Screen \r\n",
	"[HEXA] AA 88 26 00 04 \r\n",
	"[sleep] 5000 \r\n",
	"\r\n",
	"\r\n",
	";====== BLT 1: Req GPS SNR For SV 32 \r\n",
	"[HEXA] AA 88 09 01 20 0A \r\n",
	"[sleep] 3000 \r\n",
	"\r\n",
	"\r\n",
	";====== Start to NV Write == \r\n",
	";====== BLT 1: Complete BLT1 Process \r\n",
	"[HEXA] AA 88 3A 01 10 09 \r\n",
	"[sleep] 5000 \r\n",
	"\r\n",
	"\r\n",
	";=========== BLT1 Ended ========= \r\n",
	"\r\n",
	"\r\n",
	"; ====== Rebooting Command ===== \r\n",
	"[HEXA] AA 88 3D 00 1F \r\n",
	"[sleep] 10000 \r\n",
	"\r\n",
	"\r\n",
	"[WHILE] 100,000 \r\n",
	" \r\n",
	" \r\n",
	"[END] \r\n\r\n\r\n",

	NULL
};


const char		*TxScript_BLT2[200] = {
	";-----------------------------------------\r\n",
	";How to use Tx script. \r\n",
	"; 1) Modify BLT2 Tx Script file. \r\n",
	"; 2) Connect COM port to send TxData. \r\n",
	"; 3) Press the [Ctrl+F6] key \r\n",
	"; 4) Select the modified Tx script file. \r\n",
	"; 5) Press [Send TxData] button. \r\n",
	"; * Max Char. are 4096 Char. per line.\r\n",
	";-----------------------------------------\r\n\r\n\r\n",
	"[START] \r\n\r\n",
	";====================================== \r\n",
	";========= BLT 2 TEST MODE ============ \r\n",
	";====================================== \r\n",
	"\r\n",
	"[DO] \r\n",
	"\r\n",
	"[SLEEP] 7000 \r\n",
	"\r\n",
	"\r\n",
	";====== BLT 2: Req to Move to Diag Mode \r\n",
	"[HEXA] AA 88 01 00 23 \r\n",
	"[SLEEP] 5000 \r\n",
	"\r\n",
	"\r\n",
	";======= TDMB Mode ====== \r\n",
	"\r\n",
	"\r\n",
	";======== BLT 2: Req to DMB Screen \r\n",
	"[HEXA] AA 88 21 00 03 \r\n",
	"[sleep] 1000 \r\n",
	"\r\n",
	"\r\n",
	";======== BLT 2: Req Dmb Scan \r\n",
	"[HEXA] AA 88  22 01 FF FE \r\n",
	"[sleep] 10000 \r\n",
	"\r\n",
	"\r\n",
	";======== BLT 2: Req Channel's Info for DMB \r\n",
	"[HEXA] AA 88 3C 00 1E \r\n",
	"[sleep] 10000 \r\n",
	"\r\n",
	"\r\n",
	";======= BLT 2: Req Channel Up Command for DMB \r\n",
	"[HEXA] AA 88  24 00 05 \r\n",
	"[sleep] 10000 \r\n",
	"\r\n",
	"\r\n",
	";======= BLT 2: Req Channel's Info for DMB \r\n",
	"[HEXA] AA 88 3C 00 1E \r\n",
	"[sleep] 10000 \r\n",
	"\r\n",
	"\r\n",
	";======= BLT 2: Req Channel Down Command for DMB \r\n",
	"[HEXA] AA 88  25 00 07 \r\n",
	"[sleep] 10000 \r\n",
	"\r\n",
	"\r\n",
	";====== BLT 2: Req Channel's Info for DMB \r\n",
	"[HEXA] AA 88 3C 00 1E \r\n",
	"[sleep] 10000 \r\n",
	"\r\n",
	"\r\n",
	";====== BLT 2: Req Antenna Detection for DMB \r\n",
	"[HEXA] AA 88 31 00 13 \r\n",
	"[sleep] 3000 \r\n",
	"\r\n",
	"\r\n",
	";======= DR/GPS Mode ====== \r\n",
	"\r\n",
	"\r\n",
	";====== BLT 2: Req DR/GPS Screen Mode \r\n",
	"[HEXA] AA 88 08 00 2A \r\n",
	"[sleep] 3000 \r\n",
	"\r\n",
	"\r\n",
	";====== BLT 2: Req Antenna Detection for DR/GPS \r\n",
	"[HEXA] AA 88 29 00 0B \r\n",
	"[sleep] 3000 \r\n",
	"\r\n",
	"\r\n",
	";===== BLT 2: Req Speed Pulse \r\n",
	"[HEXA] AA 88 33 00 11 \r\n",
	"[sleep] 3000 \r\n",
	"\r\n",
	"\r\n",
	";===== BLT 2: Req Reverse \r\n",
	"[HEXA] AA 88 34 00 16 \r\n",
	"[sleep] 3000 \r\n",
	"\r\n",
	"\r\n",
	";====== BLT 2: Req Hot start in GPS TTFF \r\n",
	"[HEXA] AA 88 2A 01 01 08 \r\n",
	"[sleep] 3000 \r\n",
	"\r\n",
	"\r\n",
	";===== BLT 2: Req Hot Fix Time \r\n",
	"[HEXA] AA 88 2B 00 09 \r\n",
	"[sleep] 3000 \r\n",
	"\r\n",
	"\r\n",
	";====== BLT 2: Req SV Cnt, SNR that's fix and fix value(1/0) \r\n",
	"[HEXA] AA 88 09 01 20 0A \r\n",
	"[sleep] 3000 \r\n",
	"\r\n",
	"\r\n",
	";======= Audio in System mode ====== \r\n",
	";======= BLT 2: Req Audio Screen of System Mode \r\n",
	"[HEXA] AA 88 0a 00 28 \r\n",
	"[sleep] 3000 \r\n",
	"\r\n",
	"\r\n",
	";======= BLT 2: Req 1kHz Level Test with Audio  \r\n",
	"[HEXA] AA 88 0b 00 29 \r\n",
	"[sleep] 3000 \r\n",
	"\r\n",
	"\r\n",
	";======= BLT 2: Req Audio stop \r\n",
	"[HEXA] AA 88 0c 00 2E \r\n",
	"[sleep] 3000 \r\n",
	"\r\n",
	"\r\n",
	"; ===== Start to NV Write == \r\n",
	";===== BLT 2: Complete BLT2 Process \r\n",
	"[hexa] AA 88 3A 01 11 08 \r\n",
	"[sleep] 3000 \r\n",
	"; ==== End == \r\n",
	"\r\n",
	"\r\n",
	"; ====== Rebooting Command ===== \r\n",
	"[HEXA] AA 88 3D 00 1F \r\n",
	"[sleep] 15000 ; Unit:msec \r\n",
	"\r\n",
	"\r\n",
	"[while] 100,000 ; Loop Count \r\n",
	"\r\n",
	"\r\n",
	"[end] \r\n\r\n\r\n",

	NULL
};
#endif

#if 0 // 2018.03.08
const char		*TxScript_FUNC[200] = {
	";-----------------------------------------\r\n",
	";How to use Tx script. \r\n",
	"; 1) Modify FUNCTION TEST Tx Script file. \r\n",
	"; 2) Connect COM port to send TxData. \r\n",
	"; 3) Press the [Ctrl+F6] key \r\n",
	"; 4) Select the modified Tx script file. \r\n",
	"; 5) Press [Send TxData] button. \r\n",
	"; * Max Char. are 4096 Char. per line.\r\n",
	";-----------------------------------------\r\n\r\n\r\n",
	"[START] \r\n\r\n",
	";====================================== \r\n",
	";====== FUNCTION TEST MODE ============ \r\n",
	";====================================== \r\n",
	"\r\n",
	"\r\n",
	"[DO] \r\n",
	"\r\n",
	"\r\n",
	";[CHAR] abcdefg qw qUIz xx\r\n",
	";[char] abcdefg qw qUIz xx\r\n",
	";\r\n",
	"[SLEEP] 7,000 ; Unit:msec \r\n",
	";======= FUNC : Request to move to Diag Mode \r\n",
	"[HEXA] AA 88 01 00 23 \r\n",
	"[SLEEP] 5,000 \r\n",
	"\r\n",
	"\r\n",
	";======= GYRO TEST In DR/GPS Mode ======= \r\n",
	";======= FUNC : Move to GYRO Screen \r\n",
	"[HEXA] AA 88 2C 00 0E \r\n",
	"[SLEEP] 3000  \r\n",
	"\r\n",
	"\r\n",
	";======= FUNC : Send Result of GYRO Test \r\n",
	"[HEXA] AA 88 2D 00 0F \r\n",
	"[SLEEP] 3000  \r\n",
	"\r\n",
	"\r\n",
	";======= Angle Cal. In System Mode ====== \r\n",
	";FUNC : Move to Angle Calibration Screen \r\n",
	"[HEXA] AA 88 3E 00 1C \r\n",
	"[SLEEP] 3000  \r\n",
	"\r\n",
	"\r\n",
	";FUNC : Angle Cal. Test \r\n",
	"[HEXA] AA 88 30 01 01 12 \r\n",
	"[SLEEP] 5000  \r\n",
	"\r\n",
	"\r\n",
	"; ======= NV Write FUNC ============ \r\n",
	";FUNC : Complete ;FUNC Process \r\n",
	"[HEXA] AA 88 3A 10 12 32 30 31 36 31 31 30 32 31 37 31 35 2D 30 31 33\r\n",
	"[sleep] 6000 \r\n",
	"\r\n",
	"\r\n",
	"; == Start to NV Write ============= \r\n",
	";BLT 1: Write Model Name \r\n",
	"[HEXA] AA 88 3A 05 01 31 37 54 32 7c \r\n",
	";[char]17L66U000045 \r\n",
	";[hexa] 1d \r\n",
	"[SLEEP] 6000 \r\n",
	"\r\n",
	";BLT 1: Write BSN Info  \r\n",
	"[HEXA] AA 88 3A 0D 02 31 37 54 32 31 37 54 32 31 37 2D 37 0b \r\n",
	"[SLEEP] 6000 \r\n",
	"\r\n",
	";BLT 1: Write MES Info \r\n",
	"[HEXA] AA 88 3A 1A 03 31 37 54 32 31 37 54 32 31 37 54 32 31 37 54 32 31 37 54 32 31 37 54 2D 37 29 \r\n",
	"[SLEEP] 6000 \r\n",
	"\r\n",
	";BLT 1: Write GPS S/N \r\n",
	"[HEXA] AA 88 3A 01 04 1d \r\n",
	"[SLEEP] 6000 \r\n",
	"\r\n",
	";BLT 1: Write DMB S/N \r\n",
	"[HEXA] AA 88 3A 21 05 31 37 54 32 31 37 54 32 31 37 54 32 2D 2D 54 32 31 37 54 32 31 37 54 32 31 37 54 32 31 37 2D 37 46 \r\n",
	";-- iA-TD4417A26-99T4417ABP116040230 \r\n",
	"[sleep] 6000 \r\n",
	"\r\n",
	"\r\n",
	";--Write BLT1 \r\n",
	"[HEXA] AA 88 3A 10 10 32 30 31 36 31 30 33 31 31 37 31 35 2D 30 31 30 \r\n",
	";[char]17L66U000045 \r\n",
	"[SLEEP] 6000 \r\n",
	"\r\n",
	";--Write BLT2 \r\n",
	"[HEXA] AA 88 3A 10 11 32 30 31 36 31 31 30 31 31 37 31 35 2D 30 31 33 \r\n",
	";[char]17L66U000045 \r\n",
	"[SLEEP] 6000 \r\n",
	"\r\n",
	";--Write FUNC  ;; OK \r\n",
	"[HEXA] AA 88 3A 10 12 32 30 31 36 31 31 30 32 31 37 31 35 2D 30 31 33 \r\n",
	";[char]17L66U000045 \r\n",
	"[SLEEP] 6000 \r\n",
	"\r\n",
	";--Write Aging ;; OK \r\n",
	"[HEXA] AA 88 3A 10 13 32 30 31 36 31 31 30 33 31 37 31 35 2D 30 31 33 \r\n",
	";[char]17L66U000045 \r\n",
	"[SLEEP] 6000 \r\n",
	"\r\n",
	";--Write Final \r\n",
	"[HEXA] AA 88 3A 10 14 32 30 31 36 31 31 30 34 31 37 31 35 2D 30 31 33 \r\n",
	";[char]17L66U000045 \r\n",
	"[SLEEP] 6000 \r\n",
	"\r\n",
	";--Write OQC \r\n",
	"[HEXA] AA 88 3A 10 15 32 30 31 36 31 31 30 35 31 37 31 35 2D 30 31 33 \r\n",
	";[char]17L66U000045 \r\n",
	"[SLEEP] 6000 \r\n",
	"\r\n",
	"; ====== Rebooting Command ===== \r\n",
	"[hexa] AA 88 3D 00 1F \r\n",
	"[sleep] 15000 \r\n",
	"\r\n",
	"\r\n",
	"[while] 100,000 \r\n",
	"\r\n",
	"\r\n",
	"[END] \r\n\r\n\r\n",

	NULL
};

#else

const char		*TxScript_AVN[50] = {
	";-----------------------------------------\n",
	"; AVN script \n",
	";-----------------------------------------\n",
	"[START] \r\n",
	"[DO] \r\n",
	"[sleep] 100  \n",
	"[char]reboot \n",
	"[SLEEP] 10000 \n",
	"[char]logcat -v time -s VideoWidget \n",
	"[sleep] 45000 \r",
	"[hexa] 03 ; CTRL+C value\r",
	"[WHILE] 10000 \r\n",
	"[END] \r"
	};



const char		*TxScript_FUNC[400] = {
	";-----------------------------------------\r\n",
	";How to use the Tx script. \r\n",
	"; 1) Modify test Tx Script file. \r\n",
	"; 2) Connect COM port to send TxData. \r\n",
	"; 3) Press the [Ctrl+F6] key \r\n",
	"; 4) Select the modified Tx script file. \r\n",
	"; 5) Press [Send TxData] button. \r\n",
	"; * Max Char. are 4096 Char. per line.\r\n",
	";-----------------------------------------\r\n\r\n\r\n",
	"[START] \r\n\r\n",
	"[SLEEP] 12000\r\n",
	";/////////////////  BLT 1 /////////////////////////\r\n",
	";BLT 1: Req to Move to Diag Mode \r\n",
	"[HEXA] AA 88 01 00 23 \r\n\r\n",
	"[SLEEP] 15000 \r\n\r\n",
	"[RANDOM] 550 ; 550 cycle, random numer genration, 1cycle(0x00~0xff) \r\n\r\n",
	"",
	"[SEQNUM] 100 ; 100 cycle, sequencial numer genration, 1cycle(0x00~0xff) \r\n\r\n\r\n\r\n",
	"[DO] \r\n",
	";======= System Mode ====== \r\n\r\n",
	";BLT 1: Req S/W Version(MCU, System, Map, DMB, GPS) \r\n",
	"[HEXA] AA 88 0d 00 2F \r\n\r\n",
	";--BLT 1: Req S/W Version (MCU, System, Map, DMB, GPS)  \r\n"
	"[HEXA] AA 88 0D 01 01 2F  ; MCU Version \r\n"
	"[HEXA] AA 88 0D 01 02 2C  ; System Version \r\n"
	"[HEXA] AA 88 0D 01 03 2D  ; Map Version \r\n"
	"[HEXA] AA 88 0D 01 04 2A  ; DMB Module Version \r\n"
	"[HEXA] AA 88 0D 01 05 2B  ; DR/GPS Module Version \r\n\r\n"
	"[SLEEP] 3000  \r\n\r\n",
	";BLT 1: Req H/W PCB Version \r\n",
	"[HEXA] AA 88 0e 00 2C \r\n\r\n",
	"[SLEEP] 5000 \r\n\r\n",
	";[char]17L66U000045 \r\n",
	";[hexa] 1d \r\n\r\n",
	";BLT 1: Req Batt. ADC \r\n",
	"[HEXA] AA 88 32 00 10 \r\n",
	"[sleep] 5000 \r\n",
	";; 9f9352c8 + 00010000 - 9F9452C8    00042c70 \r\n\r\n",
	";;=== Start Write to NV ========== \r\n\r\n",
	";BLT 1: Write Model Name \r\n",
	"[HEXA] AA 88 3A 05 01 31 37 54 32 7c \r\n",
	";[char]17L66U000045 \r\n",
	";[hexa] 1d \r\n\r\n",
	"[SLEEP] 6000 \r\n\r\n",
	"\r\n",
	";BLT 1: Write BSN Info \r\n",
	"[HEXA] AA 88 3A 0D 02 31 37 54 32 31 37 54 32 31 37 2D 37 0b \r\n",
	";[hexa] 1d \r\n",
	"\r\n",
	"[SLEEP] 6000 \r\n\r\n",
	";BLT 1: Write MES Info \r\n",
	"[HEXA] AA 88 3A 1A 03 31 37 54 32 31 37 54 32 31 37 54 32 31 37 54 32 31 37 54 32 31 37 54 2D 37 29 \r\n",
	";[hexa] 1d \r\n\r\n",
	"[SLEEP] 6000 \r\n\r\n",
	";BLT 1: Write GPS S/N \r\n",
	"[HEXA] AA 88 3A 01 04 1d \r\n",
	"[SLEEP] 6000 \r\n",
	"\r\n\r\n",
	";BLT 1: Write DMB S/N \r\n",
	"[HEXA] AA 88 3A 21 05 31 37 54 32 31 37 54 32 31 37 54 32 2D 2D 54 32 31 37 54 32 31 37 54 32 31 37 54 32 31 37 2D 37 46 \r\n",
	";; iA-TD4417A26-99T4417ABP116040230 \r\n",
	"[sleep] 6000 \r\n\r\n",
	";Write BLT1 \r\n",
	"[HEXA] AA 88 3A 10 10 32 30 31 36 31 30 33 31 31 37 31 35 2D 30 31 30 \r\n",
	";[char]17L66U000045 \r\n",
	";[hexa] 1d \r\n\r\n",
	"[SLEEP] 6000 \r\n\r\n",
	";Write BLT2 \r\n",
	"[HEXA] AA 88 3A 10 11 32 30 31 36 31 31 30 31 31 37 31 35 2D 30 31 33 \r\n",
	";[char]17L66U000045 \r\n",
	";[hexa] 1d \r\n\r\n",
	"[SLEEP] 6000 \r\n\r\n",
	";Write FUNC  ;; OK \r\n",
	"[HEXA] AA 88 3A 10 12 32 30 31 36 31 31 30 32 31 37 31 35 2D 30 31 33 \r\n",
	";[char]17L66U000045 \r\n",
	";[hexa] 1d \r\n\r\n",
	"[SLEEP] 6000 \r\n\r\n",
	";Write Aging ;; OK \r\n",
	"[HEXA] AA 88 3A 10 13 32 30 31 36 31 31 30 33 31 37 31 35 2D 30 31 33 \r\n",
	";[char]17L66U000045 \r\n",
	";[hexa] 1d \r\n\r\n",
	"[SLEEP] 6000 \r\n\r\n",
	";Write Final \r\n",
	"[HEXA] AA 88 3A 10 14 32 30 31 36 31 31 30 34 31 37 31 35 2D 30 31 33 \r\n",
	";[char]17L66U000045 \r\n",
	";[hexa] 1d \r\n\r\n",
	"[SLEEP] 6000 \r\n\r\n\r\n",
	";Write OQC \r\n",
	"[HEXA] AA 88 3A 10 15 32 30 31 36 31 31 30 35 31 37 31 35 2D 30 31 33 \r\n",
	";[char]17L66U000045 \r\n",
	";[hexa] 1d \r\n\r\n",
	"[SLEEP] 6000 \r\n\r\n\r\n",
	";== Finish Write in NV === \r\n\r\n",
	";BLT 1: Read Nv about BSN Info \r\n",
	"[HEXA] AA 88 3B 01 02 1A \r\n\r\n",
	"[SLEEP] 5000 \r\n\r\n",
	";BLT 1: Read Nv about GPS S/N \r\n",
	"[HEXA] AA 88 3B 01 04 1C \r\n",
	"[SLEEP] 5000 \r\n\r\n",
	";BLT 1: Read Nv about DMB S/N \r\n",
	"[HEXA] AA 88 3B 01 05 1D \r\n",
	"[sleep] 5000 \r\n\r\n\r\n",
	";======= TDMB Mode ====== \r\n\r\n",
	";BLT 1: Req to DMB Screen \r\n",
	"[HEXA] AA 88 21 00 03 \r\n",
	"[sleep] 5000 \r\n\r\n",
	";BLT 1: Req Dmb Scan \r\n",
	"[HEXA] AA 88  22 01 FF FE \r\n",
	"[sleep] 32000 \r\n\r\n\r\n",
	";BLT 1: Req DMB SNR/CER \r\n",
	"[HEXA] AA 88 05 00 27 \r\n",
	"[sleep] 5000 \r\n\r\n\r\n",
	";BLT 1: Req DMB RSE ON \r\n",
	"[HEXA] AA 88 06 00 24 \r\n",
	"[sleep] 15000 \r\n\r\n",
	";BLT 1: Req DMB RSE OFF \r\n",
	"[HEXA] AA 88 07 00 25 \r\n",
	"[sleep] 15000 \r\n\r\n\r\n",
	";======= DR/GPS Mode ====== \r\n\r\n",
	";BLT 1: Req GPS Screen \r\n",
	"[HEXA] AA 88 26 00 04 \r\n",
	"[sleep] 5000 \r\n\r\n",
	";BLT 1: Req GPS SNR For SV 32 \r\n",
	"[HEXA] AA 88 09 01 20 0A \r\n",
	"[sleep] 3000 \r\n\r\n",
	"; == Start to NV Write == \r\n",
	";BLT 1: Complete BLT1 Process \r\n",
	";;[HEXA] AA 88 3A 01 10 09 \r\n",
	"[sleep] 5000 \r\n\r\n",
	";BLT 1: Read Nv about BLT1 \r\n",
	"[HEXA] AA 88 3B 01 10 08 \r\n",
	"[sleep] 5000 \r\n",
	"; == End == \r\n\r\n\r\n",
	";=========== BLT 2 ================== \r\n\r\n",
	";======= TDMB Mode ====== \r\n\r\n",
	";BLT 2: Req to DMB Screen \r\n",
	"[HEXA] AA 88 21 00 03 \r\n",
	"[sleep] 1000 \r\n\r\n",
	";BLT 2: Req Dmb Scan \r\n",
	"[HEXA] AA 88  22 01 FF FE \r\n",
	"[sleep] 10000 \r\n\r\n",
	";BLT 2: Req Channel's Info for DMB \r\n",
	"[HEXA] AA 88 3C 00 1E \r\n",
	"[sleep] 10000 \r\n\r\n",
	";BLT 2: Req Channel Up Command for DMB \r\n",
	"[HEXA] AA 88  24 00 06 \r\n",
	"[sleep] 10000 \r\n\r\n",
	";BLT 2: Req Channel's Info for DMB \r\n",
	"[HEXA] AA 88 3C 00 1E \r\n",
	"[sleep] 10000 \r\n\r\n\r\n",
	";BLT 2: Req Channel Down Command for DMB \r\n",
	"[HEXA] AA 88  25 00 07 \r\n",
	"[sleep] 10000 \r\n\r\n",
	";BLT 2: Req Channel's Info for DMB \r\n",
	"[HEXA] AA 88 3C 00 1E \r\n",
	"[sleep] 10000 \r\n\r\n",
	";BLT 2: Req Antenna Detection for DMB \r\n",
	"[HEXA] AA 88 31 00 13 \r\n",
	"[sleep] 3000 \r\n\r\n\r\n",
	";======= DR/GPS Mode ====== \r\n\r\n",
	";BLT 2: Req DR/GPS Screen Mode \r\n",
	"[HEXA] AA 88 08 00 2A \r\n",
	"[sleep] 3000 \r\n\r\n",
	";BLT 2: Req Antenna Detection for DR/GPS \r\n",
	"[HEXA] AA 88 29 00 0B \r\n",
	"[sleep] 3000 \r\n\r\n\r\n",
	";BLT 2: Req Speed Pulse \r\n",
	"[HEXA] AA 88 33 00 11 \r\n",
	"[sleep] 3000 \r\n\r\n",
	";BLT 2: Req Reverse \r\n",
	"[HEXA] AA 88 34 00 16 \r\n",
	"[sleep] 3000 \r\n\r\n",
	";BLT 2: Req Cold start in GPS TTFF \r\n",
	"[HEXA] AA 88 2A 01 01 08 \r\n",
	"[sleep] 50,000 \r\n\r\n\r\n",
	";BLT 2: Req Hot Fix Time \r\n",
	"[HEXA] AA 88 2B 00 09 \r\n",
	"[sleep] 3000 \r\n\r\n",
	";BLT 2: Req SV Cnt, SNR that's fix and fix value(1/0) \r\n",
	"[HEXA] AA 88 09 01 20 0A \r\n",
	"[SLEEP] 3000 \r\n\r\n\r\n",
	";======= Audio in System mode ====== \r\n\r\n",
	";BLT 2: Req Audio Screen of System Mode \r\n",
	"[HEXA] AA 88 0a 00 28 \r\n",
	"[sleep] 3000 \r\n\r\n",
	";BLT 2: Req 1kHz Level Test with Audio  \r\n",
	"[HEXA] AA 88 0b 00 29 \r\n",
	"[sleep] 3000 \r\n\r\n",
	";BLT 2: Req Audio stop \r\n",
	"[HEXA] AA 88 0c 00 2E \r\n",
	"[sleep] 3000 \r\n\r\n",
	";Write BLT2 \r\n",
	"[HEXA] AA 88 3A 10 11 32 30 31 36 31 31 30 31 31 37 31 35 2D 30 31 33 \r\n",
	";[char]17L66U000045 \r\n",
	";[hexa] 1d \r\n\r\n",
	"[SLEEP] 6000 \r\n\r\n",
	";BLT 2: Read Nv about BLT2 \r\n",
	"[HEXA] AA 88 3B 01 11 09 \r\n",
	"[sleep] 5000 \r\n\r\n",
	";///////////////// FUNC ///////////////////////////////////////// \r\n\r\n",
	";======= GYRO TEST In DR/GPS Mode ======= \r\n",
	";FUNC : Move GYRO Screen \r\n",
	"[HEXA] AA 88 2C 00 0E \r\n\r\n",
	"[SLEEP] 3000  \r\n\r\n",
	";FUNC : Send Result of GYRO Test \r\n",
	"[HEXA] AA 88 2D 00 0F \r\n\r\n",
	"[SLEEP] 3000  \r\n\r\n",
	";======= Angle Cal. In System Mode ====== \r\n",
	";FUNC : Move to Angle Cal Screen \r\n",
	"[HEXA] AA 88 3E 00 1C \r\n\r\n",
	"[SLEEP] 3000  \r\n\r\n",
	";FUNC : Angle Cal. Test \r\n",
	"[HEXA] AA 88 30 01 01 12 \r\n\r\n",
	"[SLEEP] 10000  \r\n\r\n",
	";Write FUNC  ;; OK \r\n",
	"[HEXA] AA 88 3A 10 12 32 30 31 36 31 31 30 32 31 37 31 35 2D 30 31 33 \r\n\r\n",
	"[SLEEP] 6000 \r\n\r\n",
	";BLT 2: Read Nv about BLT2 \r\n",
	";[HEXA] AA 88 3B 01 12 0A \r\n",
	"[sleep] 5000 \r\n\r\n",
	"; ///////////////// ETC - 진단 모든 기능 테스트 //// \r\n",
	"[HEXA]AA 88 1B 01 00 38     ;// S/W Version Info. Screen \r\n",
	"[sleep] 3000 \r\n",
	"[HEXA]AA 88 1B 01 01 39     ;// Angle Cal. Screen \r\n",
	"[sleep] 3000 \r\n",
	";FUNC : Angle Cal. Test \r\n",
	"[HEXA] AA 88 30 01 01 12  \r\n",
	"[SLEEP] 10000   \r\n",
	"[HEXA]AA 88 30 01 03 10 \r\n",
	"[SLEEP] 10000   \r\n",
	"[HEXA]AA 88 30 01 0a 19  \r\n",
	"[SLEEP] 10000  \r\n",
	"[HEXA]AA 88 30 01 0f 1c \r\n\r\n",
	"[SLEEP] 11000 \r\n\r\n",
	"[HEXA]AA 88 1B 01 02 3A     ;// System Verify and Factory Reset Screen \r\n",
	"[sleep] 3000 \r\n",
	"[HEXA]AA 88 14 01 00 37 \r\n",
	"[sleep] 5000 \r\n",
	"[HEXA]AA 88 13 00 31    ;// factory reset \r\n",
	"[sleep] 10000 \r\n\r\n",
	"[HEXA]AA 88 1B 01 03 3B     ;// USB Test Screen \r\n",
	"[sleep] 3000 \r\n\r\n",
	"[HEXA]AA 88 15 01 01 37    ;// USB Ping Test \r\n",
	"[sleep] 10000 \r\n",
	"[HEXA]AA 88 15 01 02 34    ;// USB Write Test  \r\n",
	"[sleep] 10000 \r\n\r\n",
	"[HEXA]AA 88 15 01 03 35    ;// USB Read Test  \r\n",
	"[sleep] 10000 \r\n\r\n",
	"[HEXA]AA 88 1B 01 04 3C     ;// Audio Test Screen \r\n",
	"[sleep] 5000 \r\n",
	"[HEXA]AA 88 19 01 00 3A     ;// Audio 100Hz Test \r\n",
	"[sleep] 10000 \r\n",
	"[HEXA]AA 88 19 01 02 38     ;// Audio 10kHz Test \r\n",
	"[sleep] 10000 \r\n",
	"[HEXA]AA 88 19 01 03 39     ;// Audio EWS Test \r\n",
	"[sleep] 10000 \r\n",
	"[HEXA]AA 88 19 01 04 3E     ;// Audio Navi Test \r\n",
	"[sleep] 10000 \r\n",
	"[HEXA]AA 88 18 01 01 3A     ;// Audio Volume Up \r\n",
	"[sleep] 6000 \r\n",
	"[HEXA]AA 88 18 01 01 3A     ;// Audio Volume Up \r\n",
	"[sleep] 6000 \r\n",
	"[HEXA]AA 88 18 01 02 39     ;// Audio Volume Down \r\n",
	"[sleep] 6000 \r\n",
	"[HEXA]AA 88 18 01 00 3B     ;// Audio Volume Default \r\n",
	"[sleep] 5000 \r\n\r\n\r\n",
	"[HEXA] AA 88 0c 00 2E       ; Req Audio stop \r\n",
	"[sleep] 3000 \r\n\r\n\r\n",
	"; ================================================================== \r\n",
	";[HEXA]AA 88 1B 01 05 3D     ;// Log On/Off Screen \r\n",
	";[sleep] 3000 \r\n\r\n",
	";[HEXA]AA 88 10 06 00 10 00 08  01 00 2D ;// log on \r\n",
	";[sleep] 7000 \r\n",
	";[HEXA]AA 88 10 06 00 17 01 88  01 00 ab // Log on \r\n",
	";[sleep] 7000 \r\n",
	";[HEXA]AA 88 11 00 33 // log off \r\n",
	";[sleep] 7000 \r\n\r\n",
	";[HEXA]AA 88 10 06 00 10 00 88  01 20 8d ;// Log on and file save  \r\n",
	";[sleep] 7000 \r\n",
	";[HEXA]AA 88 10 06 00 10 00 88  01 00 ad ;// Log on \r\n",
	";[sleep] 7000 \r\n",
	"; ================================================================== \r\n\r\n\r\n",

	"[HEXA]AA 88 1C 01 00 3F     ;// Micom Main(버젼정보 등) Screen \r\n",
	"[sleep] 3000 \r\n",
	"[HEXA]AA 88 1C 01 01 3E     ;// Power Voltage Screen \r\n",
	"[sleep] 3000 \r\n\r\n",

	"[HEXA]AA 88 1D 01 00 3E     ;/ DMB Main(채널스캔 및 이동 등) Screen \r\n",
	"[sleep] 3000 \r\n",
	";BLT 2: Req Dmb Scan \r\n",
	"[HEXA] AA 88  22 01 FF FE \r\n",
	"[sleep] 10000 \r\n",
	";BLT 2: Req Channel's Info for DMB \r\n",
	"[HEXA] AA 88 3C 00 1E \r\n",
	"[sleep] 10000 \r\n\r\n",

	"[HEXA] AA 88  22 01 03 02    ; // 8B \r\n",
	"[sleep] 7000 \r\n\r\n",

	"[HEXA] AA 88  22 01 0b 0a    ; // 10B \r\n",
	"[sleep] 7000 \r\n\r\n",

	"[HEXA] AA 88  22 01 0f 0e    ; // 12B \r\n",
	"[sleep] 7000 \r\n\r\n",

	";BLT 2: Req Channel Up Command for DMB \r\n",
	"[HEXA] AA 88  24 00 06 \r\n",
	"[sleep] 10000 \r\n",
	";BLT 2: Req Channel's Info for DMB \r\n",
	"[HEXA] AA 88 3C 00 1E \r\n",
	"[sleep] 10000 \r\n\r\n",

	";BLT 2: Req Channel Down Command for DMB \r\n",
	"[HEXA] AA 88  25 00 07 \r\n",
	"[sleep] 10000 \r\n",
	";BLT 2: Req Channel's Info for DMB \r\n",
	"[HEXA] AA 88 3C 00 1E \r\n",
	"[sleep] 10000 \r\n\r\n",

	"[HEXA]AA 88 1D 01 01 3F     ;// DMB TPEG Service Screen \r\n",
	"[sleep] 3000 \r\n",
	"[HEXA]AA 88 1D 01 02 3C     ;// DMB Module Test Screen \r\n",
	"[sleep] 3000 \r\n",
	"[HEXA]AA 88 17 00 35     ;// DMB Module Test \r\n",
	"[sleep] 10000 \r\n\r\n",

	"[HEXA]AA 88 1E 01 00 3D     ;// DR/GPS Main(TTFF) Screen \r\n",
	"[sleep] 3000 \r\n",
	";BLT 2: Req Hot start in GPS TTFF \r\n",
	"[HEXA] AA 88 2A 01 03 0A \r\n",
	"[sleep] 40,000 \r\n\r\n",

	"[HEXA]AA 88 1E 01 01 3C     ;// Gryo Counts and Accuracy Info Screen \r\n",
	"[sleep] 3000 \r\n",
	"[HEXA]AA 88 1E 01 02 3F     ;// GPS Status Screen \r\n",
	"[sleep] 3000 \r\n",
	"[HEXA]AA 88 1E 01 03 3E     ;// Satellite Info. Screen \r\n",
	"[sleep] 3000 \r\n\r\n",

	"[HEXA]AA 88 3E 00 1C    ;// angle cal screen \r\n",
	"[sleep] 7000  \r\n",
	"[HEXA]AA 88 21 00 03    ;// dmb screen \r\n",
	"[sleep] 5000 \r\n",
	"[HEXA]AA 88 12 00 30    ;// factory screen \r\n",
	"[sleep] 5000 \r\n",
	"[HEXA]AA 88 13 00 31    ;// factory reset \r\n",
	"[sleep] 10000 \r\n",
	"[HEXA]AA 88 0a 00 28    ;// audio screen \r\n",
	"[sleep] 5000 \r\n",
	"[HEXA]AA 88 1B 01 00 38     ;// S/W Version Info. Screen \r\n",
	"[sleep] 3000 \r\n\r\n",

	"; == 재부팅 == \r\n",
	";[HEXA] AA 88 3D 00 1F \r\n",
	";[sleep] 10000 \r\n\r\n\r\n",

	"[WHILE] 1000 \r\n\r\n",

	"[END] \r\n\r\n",
	";================================================== \r\n",
};

#endif





#define SLEEP_WAIT_TIME_MIN 			1000
#define TX_SCRIPT_SIZ 					(1024*4)  // fgets() MAX_PATH buffer size


#define TITLE_TX_DATA 					TEXT("Serial(UART) Fuzz Test") // 2018.10.25, // "Tx Data to Serial")
#define TITLE_HEX2BIN					TEXT("Convert Hex2Binary")
#define TITLE_SETUPS_F12				TEXT("UART/USB Options") // 2018.05.15
#define TITLE_FLT2HEXA 					TEXT("Convert Float value to hexa") // 2018.07.20


#define SCRPIT_WHILE_POS 		7
#define SCRPIT_RANDOM_POS 		8


enum {
	SCRIPT_NONE=0,
	SCRIPT_START=1,
	SCRIPT_END=2,
	SCRIPT_HEXA=3,
	SCRIPT_CHAR=4,
	SCRIPT_SLEEP=5,
	SCRIPT_DO_START=6,
	SCRIPT_WHILE_END=7,

	SCRIPT_RANDOM_GEN=8,
	SCRIPT_SEQNUM_GEN=9,

	SCRIPT_SEC,
	SCRIPT_MSEC, // msec

	SCRIPT_MAX = 0xff
};


volatile HWND _hDlgTxData2UART=NULL;
HWND _hTxData2UART;

volatile HWND _hDlgFlt2Hexa=NULL; // 2018.07.18



extern TCHAR szSerialTxCharData[TX_DATA_LENGTH]; // 2016.03.23
extern TCHAR szSerialTxHexaData[TX_DATA_LENGTH]; // 2016.03.23
extern TCHAR szSerialTxFileData[TX_DATA_LENGTH]; // 2016.03.28
extern TCHAR szSerialTxFileTitl[TX_DATA_LENGTH]; // 2016.03.28
extern TCHAR szSerialTxDispName[TX_DATA_LENGTH]; 

extern TCHAR szSerialTxHexaXOR[TX_DATA_LENGTH]; // 2018.08.21


#if USE_FLOAT2HEX_VALUE // 2018.07.18
extern TCHAR szFlt2Hex_In1[MAX_PATH]; // 1st input
extern TCHAR szFlt2Hex_In2[MAX_PATH]; // 2nd input
extern TCHAR szFlt2Hex_In3[MAX_PATH]; // step
extern TCHAR szFlt2Hex_Out[MAX_PATH];

extern TCHAR szDbl2Hex_In1[MAX_PATH];
extern TCHAR szDbl2Hex_In2[MAX_PATH];
extern TCHAR szDbl2Hex_In3[MAX_PATH];

extern int isFlt2HexStop;
#endif


extern int isTxData2UART;
extern int isTxUARTDataExit; // MENU to UART
int isTxDataExit = 0; // [F6] or Ctrl+[F6]

unsigned int TxScriptCommandLines = 0;

int DelSpecialChar(char *cch, int iStartPos, int iLen);
int TxScriptParser(char *szScriptText);

#endif // USE_TXDATA_TO_SERIAL


#if (RICHED_COLOR) || (TEXT_FIND_MARK) // 2016.09.23
volatile HWND _hDlgTxtColorLOG = NULL;

extern int txtColorRed, txtColorMegenta, txtColorBlue, txtColorGreen, txtColorYellow;

TCHAR szDataRed[MAX_PATH] = { 0, };
#if 0
TCHAR szDataMagenta[MAX_PATH] = { 0, }; 
TCHAR szDataBlue[MAX_PATH] = { 0, };  
TCHAR szDataGreen[MAX_PATH] = { 0, };  
TCHAR szDataYellow[MAX_PATH] = { 0, };  
#endif
int iLenDataRed=0, iLenDataMagenta=0, iLenDataBlue=0, iLenDataGreen=0, iLenDataYellow=0;
long long HashRed=0, HashMagenta=0;
#endif


#if TEXT_FIND_MARK // 2016.09.23
extern void IsMarking(void);
extern long long make_hash_p(TCHAR str[], int _size);
#endif


#ifdef COM_SETTINGS /// 2012.07.11
extern int userCOMparity, userCOMstop, userCOMdataIdx;
extern int userFlowControl; // 2017.08.03, None Flow Control 
extern int userFlowCtrlVal; // 2017.08.04, None Flow Control 

extern int userHexaDispVal; // 2018.03.22, Hexa display

static int tempCOMparity=0;
static int tempCOMstop=0;
static int tempCOMdataIdx=1; /// 8bit default
static int tempFlowControl=0; // 2017.08.03, Flow Control default:None (index 0)
static int tempFlowCtrlVal=0; // 2017.08.04, None Flow Control 
static int tempHexaDispVal=0; // 2018.03.22, Hexa Display
#endif

#if USER_SCREEN_BUF_SIZE // 2016.02.14
extern int userScrBufSizIndex;

static int tempScrBufSizIndex=0;
int tempuserScrBufSizIndex = 0xff;
#endif


static int tempIdBaudRate, tempComPort;
static int tempAutoSend, tempMsgSave, tempLocalTime, tempCPUtype;
static int tempFontType;
static int tempRetryUSB;
static int tempTxDataType, tempMsgOnOff;
static int tempCRLF=0; // 2019.12.21
static int tempLogFile=0; // 2020.04.07

static int tempChoiceFlt2Hex, tempFlt2HexEndian=0; // 2018.07.18

#if HEXA_MODE_SUPPORT	/* 2011.02.08 Hexa mode display */
static int tempTextorHexaOutMode; /* Text display or Hexa display */
#endif

#if (RICHED_COLOR) || (TEXT_FIND_MARK) // 2016.09.23
static int tmptxtColorRed, tmptxtColorMegenta, tmptxtColorBlue, tmptxtColorGreen, tmptxtColorYellow;
#endif

#if defined(COLOR_DISP2)
static int tempColorType;
extern COLORREF BGColor;
#endif


#if DNW_2ND_WINDOWS_ENABLE // 2017.08.31
static int tempisStatusBarEnable;
#endif

extern int transmitMenuNumber;
extern int UbootMenuNumber;
extern int UbootMenuNumber2;
extern TCHAR szDownloadString[MAXHISTORY][FILENAMELEN];

extern TCHAR szQuickDownloadString[MAXHISTORY][FILENAMELEN];
extern TCHAR szQuickFileName[MAXHISTORY][FILENAMELEN];
extern TCHAR szUMONDownloadString[MAXHISTORY][FILENAMELEN];
extern TCHAR szUMONSFileName[MAXHISTORY][FILENAMELEN];



UINT hex2int(TCHAR *str);
UINT str2int(TCHAR *str);
UINT str2int_(TCHAR *str, int len);
UINT str2hex(TCHAR *str); // 2016.03.08
int  isinteger(TCHAR *str); // 2016.03.12
int ExtractHexVal(TCHAR *str, TCHAR *converted);


int WriteQuickUserConfig(void);
BOOL LOGFile_open(void);
void LOGFile_close(void);
extern void BeepSound(int beeptype);


BOOL CALLBACK DownloadProgressProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR szTitle[FILENAMELEN] = {0,};
	TCHAR tUSBspeed[32] = {0,};
	int lo_wParam, lo_lParam;

	lo_wParam = LOWORD(wParam); // warning fix
	lo_lParam = LOWORD(lParam);

	switch(message)
	{
		case WM_INITDIALOG:

		#if 0 // 2016.10.12
			#if 0
			lstrcpy(txFileName, TEXT("FileName: ") );
			lstrcat(txFileName, szFileNameOnPopUP );


			SetDlgItemText( hDlg, IDC_PROC_FILENAME, txFileName ); 
			#else
			szFileNameOnPopUP[FILENAMELEN-1]='\0';
			SendMessage(GetDlgItem(hDlg,IDC_PROC_FILENAME),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szFileNameOnPopUP);	
			#endif

		#endif


			if( USB_TX_SIZE >= (4096*4) ) ///   TX_SIZE_1 ) /* 2013.04.22 */
			{
				lstrcpy(szTitle,TEXT("Downloading "));

				//////////////2010.5.27 added ////////////////////
				sprintf(tUSBspeed,"[%dK] ", (USB_TX_SIZE/1024) );
				lstrcat(szTitle, tUSBspeed);
				////////////////////////////////////////////////
				
				lstrcat(szTitle,szFileName);
				SetWindowText(hDlg,szTitle);
				_hDlgDownloadProgress=hDlg;
			}
			else
			{ /// 2013.04.22 added...

				downloadCanceled = 1;
				EB_Printf(TEXT("[dnw] DownloadProgressProc() error, USB_TX_SIZE[%d], szFileName=[%s] \r\n"),  USB_TX_SIZE , szFileName );
			}
			return TRUE;


		
		case WM_CLOSE: 
		case WM_QUIT: // 2016.02.22
		case WM_DESTROY: // 2016.02.22

			//If the [X] button is clicked, WM_CLOSE message is received.
			//Originally, DefWindowProc() called DestroyWindow();
			//But, there is no DefWindowProc(), We have to process WM_CLOSE message.
			if( hDlg )
			{
				DestroyWindow(hDlg); 
				CloseHandle(hDlg); 
				hDlg=NULL; 
			} 

			if( _hDlgDownloadProgress ) 
			{
				DestroyWindow(_hDlgDownloadProgress);
				CloseHandle(_hDlgDownloadProgress);
				_hDlgDownloadProgress=NULL;
			}
			downloadCanceled=1;

			//break;
			return TRUE; //why not?


		case WM_COMMAND:

			//wmEvent = HIWORD(wParam);
			//wmId	= LOWORD(wParam); 
			switch(LOWORD(wParam))
			{

				case IDCANCEL: // Exit!!
					//EB_Printf(TEXT("[dnw] DownloadProgressProc() Cancelled!!! \r\n") );

					if( hDlg )
					{
						DestroyWindow(hDlg); 
						CloseHandle(hDlg); 
						hDlg=NULL; 
					} 
					
					if( _hDlgDownloadProgress ) 
					{
						DestroyWindow(_hDlgDownloadProgress);
						CloseHandle(_hDlgDownloadProgress);
						_hDlgDownloadProgress=NULL;
					}
					downloadCanceled=1;

					return TRUE;

				default:
				case IDOK:
					return FALSE;
			}

	}

	return FALSE;
}




BOOL CALLBACK DownloadProgressProcUART(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR szTitle[FILENAMELEN] = {0,};
	TCHAR tUSBspeed[32] = {0,};
	//TCHAR txFileName[MAX_PATH] = {0,};
	int lo_wParam, lo_lParam;

	lo_wParam = LOWORD(wParam); // warning fix
	lo_lParam = LOWORD(lParam);

	switch(message)
	{
		case WM_INITDIALOG:

	#if 0
			lstrcpy(txFileName, TEXT("FileName: ") );
			lstrcat(txFileName, szFileNameOnPopUP );

			SetDlgItemText( hDlg, IDC_PROC_FILENAME, txFileName ); 
	#else
			szFileNameOnPopUP[FILENAMELEN-1]='\0';
			SendMessage(GetDlgItem(hDlg,IDC_PROC_FILENAME),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szFileNameOnPopUP);  
	#endif
	
			//else if( 1==isTxbyCOM ) // Transmit to COM port
			{
				lstrcpy(szTitle,TEXT("Downloading to COM "));

				//////////////2010.5.27 added ////////////////////
				// -- sprintf(tUSBspeed,"[%dK] ", (TX_SIZE/1024) );
				// -- lstrcat(szTitle, tUSBspeed);
				////////////////////////////////////////////////
	
				lstrcat(szTitle,szFileName);
				SetWindowText(hDlg,szTitle);
				_hDlgDownloadProgressUART = hDlg;
			}

			return TRUE;


		
		case WM_CLOSE: 
		case WM_QUIT: // 2016.02.22
		case WM_DESTROY: // 2016.02.22

			//If the [X] button is clicked, WM_CLOSE message is received.
			//Originally, DefWindowProc() called DestroyWindow();
			//But, there is no DefWindowProc(), We have to process WM_CLOSE message.
			if( hDlg )
			{
				DestroyWindow(hDlg); 
				CloseHandle(hDlg); 
				hDlg=NULL; 
			} 

			if( _hDlgDownloadProgressUART ) 
			{
				DestroyWindow(_hDlgDownloadProgressUART);
				CloseHandle(_hDlgDownloadProgressUART);
				_hDlgDownloadProgressUART=NULL;
			}
			downloadCanceled = 1;
			isTxUARTDataExit = 1;

			isMenuTxing = 0; // Completed File Transferred!!

			//break;
			return TRUE; //why not?


		case WM_COMMAND:

			//wmEvent = HIWORD(wParam);
			//wmId	= LOWORD(wParam); 
			switch(LOWORD(wParam))
			{

				case IDCANCEL: // Exit!!
					//EB_Printf(TEXT("[dnw] DownloadProgressProc() Cancelled!!! \r\n") );
					isTxUARTDataExit = 1;

					if( hDlg )
					{
						DestroyWindow(hDlg); 
						CloseHandle(hDlg); 
						hDlg=NULL; 
					} 
					
					if( _hDlgDownloadProgressUART ) 
					{
						DestroyWindow(_hDlgDownloadProgressUART);
						CloseHandle(_hDlgDownloadProgressUART);
						_hDlgDownloadProgressUART=NULL;
					}
					downloadCanceled=1;

					isMenuTxing = 0; // Completed File Transferred!!

					return TRUE;

				default:
				case IDOK:
					return FALSE;
			}

	}

	return FALSE;
}



void InitDownloadProgress(void) // USB
{
	while( NULL == _hDlgDownloadProgress )
	{
		Sleep(0); //wait until the progress dialog box is ready.
	}

	_hProgressControl = GetDlgItem(_hDlgDownloadProgress,IDC_PROGRESS1); // USB port
	SendMessage(_hProgressControl,PBM_SETRANGE,0,MAKELPARAM(0, 100));
	downloadCanceled=0;
}


void InitDownloadProgressUART(void) // COM port
{
	while( NULL == _hDlgDownloadProgressUART )
	{
		Sleep(0); //wait until the progress dialog box is ready.
	}

	_hProgressControlUART = GetDlgItem(_hDlgDownloadProgressUART,IDC_PROGRESS6); // COM port
	SendMessage(_hProgressControlUART,PBM_SETRANGE,0,MAKELPARAM(0, 100));
	downloadCanceled=0;
}

void DisplayDownloadProgress(unsigned int percent) // USB port
{
	TCHAR szProc[MAX_PATH] = {0,};
	SendMessage(_hProgressControl,PBM_SETPOS,(WPARAM)percent,0); 

	wsprintf(szProc, TEXT("Tx Bytes: %d %%"), percent );

	SetDlgItemText( _hDlgDownloadProgress, IDC_PROC_TX_BYTES, szProc ); 
}


void DisplayDownloadProgressUART( __int64 read, __int64 tot) // COM port
{
	TCHAR szProc[MAX_PATH] = {0,};
	unsigned int percent = 0;

	percent = (unsigned int)( read*100UL/tot );
	SendMessage(_hProgressControlUART,PBM_SETPOS,(WPARAM)percent,0); 

	wsprintf(szProc, TEXT("Tx Bytes: %u %%  (%I64d / %I64d)Byte"), percent, read, tot);
	SetDlgItemText( _hDlgDownloadProgressUART, IDC_PROC_TX_BYTES, szProc ); 
}


void CloseDownloadProgress(void) // USB port
{
	if(_hDlgDownloadProgress!=NULL)
	{
	    	//DestroyWindow(_hDlgDownloadProgress); 
		//Doesn't work because CloseDownloadProgress() is called another thread,
		//which is different the thread calling CreatDialog().
    
		SendMessage(_hDlgDownloadProgress,WM_CLOSE,0,0); 
		_hDlgDownloadProgress=NULL;
	}
}

void CloseDownloadProgressUART(void) // UART
{
	if( NULL != _hDlgDownloadProgressUART )
	{
	    	//DestroyWindow(_hDlgDownloadProgress); 
		//Doesn't work because CloseDownloadProgress() is called another thread,
		//which is different the thread calling CreatDialog().
    
		SendMessage(_hDlgDownloadProgressUART, WM_CLOSE,0,0); 
		_hDlgDownloadProgressUART=NULL;
	}
}


BOOL GetComPortFromReg(DWORD PortNum, UCHAR *szPortName)
{
#if 0
	LONG Result = ERROR_SUCCESS;
	HKEY hKey;
	TCHAR szKey[MAX_PATH];
	DWORD dwKeySize = MAX_PATH, dwClassSize = MAX_PATH, dwType;
	if(ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		"HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_ALL_ACCESS, &hKey))
	{
		MessageBox(NULL, TEXT("Cannot Get Serial Port Infomation"), TEXT("ERROR"), MB_OK);
		return FALSE;
	}
	DWORD PortCount = 0;
	while(ERROR_SUCCESS == Result){
		memset(szKey, 0, dwKeySize);
		dwKeySize = MAX_PATH;
		dwClassSize = MAX_PATH;
		Result=RegEnumValue(hKey, PortCount, szKey, &dwKeySize, NULL, &dwType, (unsigned char *)szPortName, &dwClassSize);
		if(PortNum == PortCount)
			break;
		PortCount++;
	}
	RegCloseKey(hKey);
	if(ERROR_SUCCESS == Result)
	{
//		MessageBox(NULL, szKey, szClass, MB_OK);
	}
	else if(ERROR_NO_MORE_ITEMS == Result)
	{
//		MessageBox(NULL, "No more Items", "Info", MB_OK);
		return FALSE;
	}
	else
	{
		MessageBox(NULL, TEXT("Get Serial Port Info fail"), TEXT("Info"), MB_OK);
		return FALSE;
	}
#endif

	return TRUE;
}


BOOL CALLBACK OptionsProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int tmp;
	static int ctemp, sbarTemp=0xff;
	static int iprevCPUtype = 1;
	WORD pmId;
	WORD wmEvent, wmId;

	
	pmId      = LOWORD(lParam); // warning fix

	wmEvent   = HIWORD(wParam);
	wmId      = LOWORD(wParam); 

	switch(message)
	{
	case WM_INITDIALOG:
		_hDlgSerialSettings=hDlg;

		tempIdBaudRate = idBaudRate;
		tempComPort    = userComPort;
		tempAutoSend   = autoSendKey;

		tempMsgSave    = msgSaveOnOff;
		tempFontType   = FontType;
#if HEXA_MODE_SUPPORT	/* 2011.02.08 Hexa mode display */
		tempTextorHexaOutMode = TextorHexaMode; /* Text display(0) or Hexa display(1 or 2) */
#endif

		tempRetryUSB   = RetryUSB;

#if defined(COLOR_DISP2)
		tempColorType  = ColorType;
#endif

#if DNW_2ND_WINDOWS_ENABLE // 2017.08.31
		tempisStatusBarEnable = isStatusBarEnable;
#endif		

		/// 2014.04.11, added
		tempCPUtype    = CPUtype;

		// 2015.11.13, System Time Info display. added
		tempLocalTime  = localTime;
		if( localTime ) isTimeInfoOnce=1; // 2016.04.02
		else isTimeInfoOnce=0;


		// 2019.12.21, added
		tempCRLF       = cmdCRLF;
	
		tempLogFile    = LogFileType; // 2020.04.07


#if USE_TXDATA_TO_SERIAL
		// 2016.03.23, Tx Data
		tempTxDataType = sendTxDataType;
		tempMsgOnOff   = txDataMsgOnOff;
#endif

		CheckRadioButton(hDlg,IDC_RADIO921600,IDC_RADIO2400,IDC_RADIO921600+tempIdBaudRate);

		CheckRadioButton(hDlg,IDC_RADIOCOM0,IDC_RADIOCOM197,IDC_RADIOCOM0+(tempComPort /*-1*/));

		

		/* SPACE-BAR key auto send on/off */
		CheckRadioButton(hDlg,IDC_RADIO_AUTO_SEND_OFF,IDC_RADIO_AUTO_SEND_ON,IDC_RADIO_AUTO_SEND_OFF+tempAutoSend);

#if LOG_SAVE_MENU_IN_SETUP // 2016.02.24
		/* Message log save on/off */
		CheckRadioButton(hDlg,IDC_RADIO_MSG_SAVE_OFF,IDC_RADIO_MSG_SAVE_ON,IDC_RADIO_MSG_SAVE_OFF+tempMsgSave);
#endif

		/* Font style */
		CheckRadioButton(hDlg,IDC_FONT0_OEM_FIX, IDC_FONT3_SYSTEM_FIX, IDC_FONT0_OEM_FIX+tempFontType);

	#if HEXA_MODE_SUPPORT	/* 2011.02.08 Hexa mode display */
		/* Text / Hex mode */
		CheckRadioButton(hDlg,IDC_OUT_TEXT_MODE, IDC_OUT_HEXA2_MODE, IDC_OUT_TEXT_MODE+tempTextorHexaOutMode);

		if( 1==tempTextorHexaOutMode ) // Hexa1_Line mode
		{
			EnableWindow( GetDlgItem(hDlg, IDC_EDIT_HEX1_COL ), 1 ); // Enabled!!		
			EnableWindow( GetDlgItem(hDlg, IDC_EDIT_HEX2_COL ), 0 ); // Disable!!
		
			// Enable Check Box
			EnableWindow( GetDlgItem(hDlg, IDC_HEXA_DISP_LOWERCASE ), 1 ); // Enabled!! 	
			EnableWindow( GetDlgItem(hDlg, IDC_HEXA_DISP_COMMA ), 1 ); // Enabled!!
			EnableWindow( GetDlgItem(hDlg, IDC_HEXA_DISP_ADD_0X ), 1 ); // Enabled!!
		}
		else if( 2==tempTextorHexaOutMode ) // Hexa2_Char mode
		{
			EnableWindow( GetDlgItem(hDlg, IDC_EDIT_HEX1_COL ), 0 ); // Disable!!
			EnableWindow( GetDlgItem(hDlg, IDC_EDIT_HEX2_COL ), 1 ); // Enabled!!
		
			// Enable Check Box
			EnableWindow( GetDlgItem(hDlg, IDC_HEXA_DISP_LOWERCASE ), 1 ); // Enabled!! 	
			EnableWindow( GetDlgItem(hDlg, IDC_HEXA_DISP_COMMA ), 1 ); // Enabled!!
			EnableWindow( GetDlgItem(hDlg, IDC_HEXA_DISP_ADD_0X ), 1 ); // Enabled!!
		}
		else // Text Mode
		{
			EnableWindow( GetDlgItem(hDlg, IDC_EDIT_HEX1_COL ), 0 ); // Disable!!
			EnableWindow( GetDlgItem(hDlg, IDC_EDIT_HEX2_COL ), 0 ); // Disable!!
		
			// Disable Check Box
			EnableWindow( GetDlgItem(hDlg, IDC_HEXA_DISP_LOWERCASE ), 0 ); // Disabled!!		
			EnableWindow( GetDlgItem(hDlg, IDC_HEXA_DISP_COMMA ), 0 ); // Disabled!!
			EnableWindow( GetDlgItem(hDlg, IDC_HEXA_DISP_ADD_0X ), 0 ); // Disabled!!
		}

	
		// 2018.03.22
		tempHexaDispVal = userHexaDispVal;
		if( userHexaDispVal )
		{
			if( HEXA_DISP_LOWERCASE == (userHexaDispVal & HEXA_DISP_LOWERCASE) )
			{
				if (SendDlgItemMessage(_hDlgSerialSettings, IDC_HEXA_DISP_LOWERCASE, BM_GETCHECK, 0, 0)==BST_UNCHECKED) 
					SendDlgItemMessage(_hDlgSerialSettings, IDC_HEXA_DISP_LOWERCASE, BM_SETCHECK, BST_CHECKED, 0);
			}
		
			if( HEXA_DISP_COMMA == (userHexaDispVal & HEXA_DISP_COMMA) ) 
			{
				if (SendDlgItemMessage(_hDlgSerialSettings, IDC_HEXA_DISP_COMMA, BM_GETCHECK, 0, 0)==BST_UNCHECKED) 
					SendDlgItemMessage(_hDlgSerialSettings, IDC_HEXA_DISP_COMMA, BM_SETCHECK, BST_CHECKED, 0);
			}

		#if 1
			if( HEXA_DISP_ADD0X == (userHexaDispVal & HEXA_DISP_ADD0X) ) 
			{
				if (SendDlgItemMessage(_hDlgSerialSettings, IDC_HEXA_DISP_ADD_0X, BM_GETCHECK, 0, 0)==BST_UNCHECKED) 
					SendDlgItemMessage(_hDlgSerialSettings, IDC_HEXA_DISP_ADD_0X, BM_SETCHECK, BST_CHECKED, 0);
			}
		#endif
		
		}

	#endif


	#if 0
		/* 2016.03.23, Tx Data */
		CheckRadioButton(hDlg,IDC_TX_CHAR_MODE, IDC_TX_HEXA_MODE, IDC_TX_CHAR_MODE+tempTxDataType);
		if( 1==tempTxDataType ) // Hexa Data
		{
			EnableWindow( GetDlgItem(hDlg, IDC_TX_EDIT_CHAR_COL ), 0 ); // Disable!!		
			EnableWindow( GetDlgItem(hDlg, IDC_TX_EDIT_HEXA_COL ), 1 ); // Enabled!!		
		}
		else if( 0==tempTxDataType )// Char Data
		{
			EnableWindow( GetDlgItem(hDlg, IDC_TX_EDIT_CHAR_COL ), 1 ); // Enabled!!		
			EnableWindow( GetDlgItem(hDlg, IDC_TX_EDIT_HEXA_COL ), 0 ); // Disable!!		
		}

		CheckRadioButton(hDlg,IDC_TX_DISPLAY_OFF, IDC_TX_DISPLAY_ON, IDC_TX_DISPLAY_OFF+tempMsgOnOff);
	#endif


		/* 2012.03.10, Retry USB port */
		CheckRadioButton(hDlg,IDC_RETRY_USB_DEFAULT, IDC_RETRY_USB_40_TIMES, IDC_RETRY_USB_DEFAULT+tempRetryUSB);



#ifdef COM_SETTINGS /// 2012.07.11

		// 2015.12.30
		tempCOMparity  = userCOMparity;
		tempCOMstop    = userCOMstop;
		tempCOMdataIdx = userCOMdataIdx;


		/* 2012.07.11, COM port - parity */
		CheckRadioButton(hDlg,IDC_COM_NOPARITY, IDC_COM_SPACEPARITY, IDC_COM_NOPARITY+tempCOMparity);

		/* 2012.07.11, COM port - stop bit */
		CheckRadioButton(hDlg,IDC_COM_ONESTOPBIT, IDC_COM_TWOSTOPBITS, IDC_COM_ONESTOPBIT+tempCOMstop);

		/* 2012.07.11, COM port - data bit */
		CheckRadioButton(hDlg,IDC_COM_DATA_7BIT, IDC_COM_DATA_8BIT, IDC_COM_DATA_7BIT+tempCOMdataIdx);



		// 2017.08.03, COM Flow Control
		tempFlowControl = userFlowControl; 
		CheckRadioButton(hDlg,IDC_FLOWCTRL_NONE, IDC_FLOWCTRL_USED, IDC_FLOWCTRL_NONE+tempFlowControl);

		switch( tempFlowControl )
		{
			case 0: // None
				EnableWindow( GetDlgItem(hDlg, IDC_FLOWCTRL_DTR_DSR ), 0 ); // Disabled!!		
				EnableWindow( GetDlgItem(hDlg, IDC_FLOWCTRL_RTS_CTS ), 0 ); // Disabled!!
				EnableWindow( GetDlgItem(hDlg, IDC_FLOWCTRL_XON_XOFF ), 0 ); // Disabled!!
				break;
			case 1: // Use
				EnableWindow( GetDlgItem(hDlg, IDC_FLOWCTRL_DTR_DSR ), 1 ); // Enabled!!		
				EnableWindow( GetDlgItem(hDlg, IDC_FLOWCTRL_RTS_CTS ), 1 ); // Enabled!!
				EnableWindow( GetDlgItem(hDlg, IDC_FLOWCTRL_XON_XOFF ), 1 ); // Enabled!!
				break;
		}

		// 2017.08.04
		tempFlowCtrlVal = userFlowCtrlVal;
		if( /*userFlowControl && */ userFlowCtrlVal )
		{
			if( FLOWC_DTR_DSR == (userFlowCtrlVal & FLOWC_DTR_DSR) )
			{
				if (SendDlgItemMessage(_hDlgSerialSettings, IDC_FLOWCTRL_DTR_DSR, BM_GETCHECK, 0, 0)==BST_UNCHECKED) 
					SendDlgItemMessage(_hDlgSerialSettings, IDC_FLOWCTRL_DTR_DSR, BM_SETCHECK, BST_CHECKED, 0);
			}

			if( FLOWC_RTS_CTS == (userFlowCtrlVal & FLOWC_RTS_CTS) ) 
			{
				if (SendDlgItemMessage(_hDlgSerialSettings, IDC_FLOWCTRL_RTS_CTS, BM_GETCHECK, 0, 0)==BST_UNCHECKED) 
					SendDlgItemMessage(_hDlgSerialSettings, IDC_FLOWCTRL_RTS_CTS, BM_SETCHECK, BST_CHECKED, 0);
			}

			if( FLOWC_XON_XOFF == (userFlowCtrlVal & FLOWC_XON_XOFF) ) 
			{
				if (SendDlgItemMessage(_hDlgSerialSettings, IDC_FLOWCTRL_XON_XOFF, BM_GETCHECK, 0, 0)==BST_UNCHECKED) 
					SendDlgItemMessage(_hDlgSerialSettings, IDC_FLOWCTRL_XON_XOFF, BM_SETCHECK, BST_CHECKED, 0);
			}
		}

#endif



#if USER_SCREEN_BUF_SIZE // 2016.02.14
		/* 2016.02.14*/

		tempScrBufSizIndex = userScrBufSizIndex;

		CheckRadioButton(hDlg,IDC_SCR_BUF_HUGE, IDC_SCR_BUF_SMALLEST, IDC_SCR_BUF_HUGE+tempScrBufSizIndex);
#endif


#if DNW_2ND_COMMAND // 2017.4.22
		CheckRadioButton(hDlg,IDC_STATUS_BAR_OFF, IDC_STATUS_CMD_ON, IDC_STATUS_BAR_OFF+tempisStatusBarEnable);
#else

	#if DNW_2ND_WINDOWS_ENABLE // 2017.4.14
		CheckRadioButton(hDlg,IDC_STATUS_BAR_OFF, IDC_STATUS_BAR_ON, IDC_STATUS_BAR_OFF+tempisStatusBarEnable);
	#endif // DNW_2ND_WINDOWS_ENABLE --

#endif


#if MODIFIED_JULIAN_DATE // 2017.08.30
		CheckRadioButton(hDlg,IDC_LOCALTIME_NONE, IDC_LOCALTIME_LINUX_HEX, IDC_LOCALTIME_NONE+tempLocalTime);
#else
		CheckRadioButton(hDlg,IDC_LOCALTIME_NONE, IDC_LOCALTIME_DATETIME, IDC_LOCALTIME_NONE+tempLocalTime);
#endif

#if 0
		// 2019.12.21, added
		CheckRadioButton(hDlg,IDC_CR_LF_ENABLE, IDC_LF_ONLY_ENABLE, IDC_CR_LF_ENABLE+tempCRLF);
#endif

		// 2015.12.14 added
		CheckRadioButton(hDlg,IDC_CPU_SMDK6410, IDC_CPU_OTHER, IDC_CPU_SMDK6410+tempCPUtype);


		// 2020.04.07
		CheckRadioButton(hDlg,ID_LOG_FILE_AUTO, ID_LOG_FILE_POPUP, ID_LOG_FILE_AUTO+tempLogFile);


#if defined(COLOR_DISP2)
		/* Color background */
		CheckRadioButton(hDlg,IDC_COLOR_TYPE_GRAY_DEF, IDC_COLOR_TYPE_GRAY2, IDC_COLOR_TYPE_GRAY_DEF+tempColorType);
#endif


		SendMessage(GetDlgItem(hDlg,IDC_EDIT1),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szDownloadAddress);  

		
		/// 2014.04.15, Baudrate ---
		if( userBaudRate > 0 )
		{
			wsprintf(szUserBaudRate, TEXT("%d"), userBaudRate);
		}
		else if( 0xFFFF != idBaudRate )
		{
			userBaudRate = baudTable[idBaudRate];
			wsprintf(szUserBaudRate, TEXT("%d"), userBaudRate);
		}
		else
		{
			idBaudRate = 9; // 8;
			userBaudRate = baudTable[idBaudRate];
			wsprintf(szUserBaudRate, TEXT("%d"), userBaudRate);
		}
		SendMessage(GetDlgItem(hDlg,IDC_EDIT2),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szUserBaudRate);	


#if HEXA_MODE_SUPPORT	/* 2012.07.03 Hexa mode display */
		// Hexa Line Mode
		SendMessage(GetDlgItem(hDlg,IDC_EDIT_HEX1_COL),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szColumnNumber);  

		// Hexa End Char Mode
		if( HexaEndChar > 0xFFFF ) // 2018.03.21
			wsprintf(szHexaEnd, TEXT("%06X"), HexaEndChar);
		else if( HexaEndChar > 0xFF ) // 2018.03.21
			wsprintf(szHexaEnd, TEXT("%04X"), HexaEndChar);
		else
			wsprintf(szHexaEnd, TEXT("%02X"), HexaEndChar);
		
		SendMessage(GetDlgItem(hDlg,IDC_EDIT_HEX2_COL),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szHexaEnd);  

		if( 1==tempTextorHexaOutMode ) // Hexa1_Line mode
		{
			EnableWindow( GetDlgItem(hDlg, IDC_EDIT_HEX1_COL ), 1 ); // Enabled!!		
			EnableWindow( GetDlgItem(hDlg, IDC_EDIT_HEX2_COL ), 0 ); // Disable!!
		}
		else if( 2==tempTextorHexaOutMode ) // Hexa2_Char mode
		{
			EnableWindow( GetDlgItem(hDlg, IDC_EDIT_HEX1_COL ), 0 ); // Disable!!
			EnableWindow( GetDlgItem(hDlg, IDC_EDIT_HEX2_COL ), 1 ); // Enabled!!
		}
		else // Text Mode
		{
			EnableWindow( GetDlgItem(hDlg, IDC_EDIT_HEX1_COL ), 0 ); // Disable!!
			EnableWindow( GetDlgItem(hDlg, IDC_EDIT_HEX2_COL ), 0 ); // Disable!!
		}

#endif	


	#if 0
		// 2016.03.23
		SendMessage(GetDlgItem(hDlg,IDC_TX_EDIT_CHAR_COL),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szSerialTxCharData);  

		SendMessage(GetDlgItem(hDlg,IDC_TX_EDIT_HEXA_COL),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szSerialTxHexaData);  
	#endif


		SetWindowText(_hDlgSerialSettings, APPTITLE TITLE_SETUPS_F12); // 2018.05.15, update title

		return TRUE;





	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:

			EnableWindow( GetDlgItem(_hDlgSerialSettings, IDOK), 0 ); // Disabled!!

		#if 0
			// 2014.04.11, USB Addr, 6410, C100 ////////////
			if( bCheckedCPU )
			{
				bCheckedCPU = FALSE;

				/// 2014.04.11, added
				switch( tempCPUtype )
				{
					case 0: // SMDK6410,  IDC_CPU_SMDK6410
						downloadAddress=0x50100000;
						lstrcpy(szDownloadAddress,TEXT("0x50100000"));
						break;

					case 1: // SMDKC100, IDC_CPU_SMDKC100
						downloadAddress=0x20030000;
						lstrcpy(szDownloadAddress,TEXT("0x20030000"));
						break;

					case 2: // IDC_CPU_OTHER
					default:
						GetRegistry_USBAddr();
						//downloadAddress=0x20090000;
						//lstrcpy(szDownloadAddress,TEXT("0x20090000"));
						break;
				}
				CPUtype = tempCPUtype;
			}
			else
		#endif
			{

			    //get downloadAddress
			    *((WORD *)szDownloadAddress)=15;
				//The first WORD should be max character number for EM_GETLINE!
			    tmp=SendMessage(GetDlgItem(hDlg,IDC_EDIT1),
						        EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szDownloadAddress);
			    szDownloadAddress[tmp]='\0'; //EM_GETLINE string doesn't have '\0'.
			    downloadAddress=hex2int(szDownloadAddress);	
			}


			if( bBaudrate )  /// Checked Button !!!
			{
				bBaudrate = FALSE;

				wsprintf(szUserBaudRate, TEXT("%d"), userBaudRate);
			}
			else
			{
				static int iEditBaud = 0;
				int  iBaud;
				BOOL bBaudOK = FALSE;
			
				/// 2014.04.15, UART baudrate
				//get downloadAddress
				*((WORD *)szUserBaudRate)=15;
				//The first WORD should be max character number for EM_GETLINE!
				tmp=SendMessage(GetDlgItem(hDlg,IDC_EDIT2),
								EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szUserBaudRate);
				szUserBaudRate[tmp]='\0'; //EM_GETLINE string doesn't have '\0'.
				iEditBaud = str2int(szUserBaudRate); 	 

				bBaudOK = FALSE;
				for(iBaud=0; iBaud<BAUD_RATE_NUM; iBaud++)
				{
					if( iEditBaud == baudTable[iBaud] )
					{
						idBaudRate   = tempIdBaudRate = iBaud;  /// index
						userBaudRate = baudTable[idBaudRate];
						bBaudOK = TRUE;
						break;
					}
				}

				if( FALSE == bBaudOK )
				{
					idBaudRate   = tempIdBaudRate = 0xFFFF; /// NOne  Index In COM Port List 
					userBaudRate = iEditBaud;

					if( iEditBaud > MAX_BAUD_RATE ) // 2016.11.02
						MessageBox( _MainHwnd, TEXT("Out of baudrate!!!\n"), "Baudrate Check!",MB_OK);
				}
			}



		#if HEXA_MODE_SUPPORT	/* 2012.07.03 Hexa mode display */
			// Hexa Line Mode
			*((WORD *)szColumnNumber)=15;
		    tmp=SendMessage(GetDlgItem(hDlg,IDC_EDIT_HEX1_COL),
					        EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szColumnNumber);
		    szColumnNumber[tmp]='\0'; //EM_GETLINE string doesn't have '\0'.
		    ColumnNumber=str2int(szColumnNumber);


			// Hexa End Char Mode
			*((WORD *)szHexaEnd)=15;

		    tmp=SendMessage(GetDlgItem(hDlg,IDC_EDIT_HEX2_COL),
					        EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szHexaEnd);
			szHexaEnd[tmp] = '\0';
			HexaEndChar = str2hex(szHexaEnd);

		#endif



		#if 0
			// 2016.03.23, Tx Data
			*((WORD *)szSerialTxCharData)=(TX_DATA_LENGTH-1);
		    tmp=SendMessage(GetDlgItem(hDlg,IDC_TX_EDIT_CHAR_COL),
					        EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szSerialTxCharData);
		    szSerialTxCharData[tmp]='\0';


			// 2016.03.23, Tx Data
			*((WORD *)szSerialTxHexaData)=(TX_DATA_LENGTH-1);
			tmp=SendMessage(GetDlgItem(hDlg,IDC_TX_EDIT_HEXA_COL),
							EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szSerialTxHexaData);
			szSerialTxHexaData[tmp]='\0';
		#endif


		
			if( 0xFFFF != tempIdBaudRate )
			{
			    idBaudRate    = tempIdBaudRate;
			    userBaudRate  = baudTable[idBaudRate];
			}
			userComPort   = tempComPort;
			autoSendKey   = tempAutoSend;

			localTime     = tempLocalTime;
			if( localTime ) isTimeInfoOnce=1; // 2016.04.02
			else isTimeInfoOnce=0;


			// 2019.12.21 added
			cmdCRLF = tempCRLF;
			
			LogFileType = tempLogFile;


			BinFileIndex  = 0; /* automatic 1st index */
			DownloadedBin = 0x00;

			msgSaveOnOff  = tempMsgSave;
			FontType      = tempFontType;
	
		#if HEXA_MODE_SUPPORT	/* 2011.02.08 Hexa mode display */
			TextorHexaMode = tempTextorHexaOutMode; /* 2011.02.08, Text display or Hexa display */
		#endif

			RetryUSB      = tempRetryUSB; /// 2012.03.10

#if USE_TXDATA_TO_SERIAL
			sendTxDataType = tempTxDataType; // 2016.03.23
			txDataMsgOnOff = tempMsgOnOff;
#endif
	


#ifdef COM_SETTINGS /// 2012.07.11
			userCOMparity  = tempCOMparity;
			userCOMstop    = tempCOMstop;
			userCOMdataIdx = tempCOMdataIdx;


			userFlowControl = tempFlowControl; // 2017.08.03
			//~~userFlowCtrlVal = tempFlowCtrlVal; // 2017.08.04

			if( /* userFlowControl && */ userFlowCtrlVal )
			{
				if( FLOWC_DTR_DSR == (userFlowCtrlVal & FLOWC_DTR_DSR) )
				{
					if (SendDlgItemMessage(_hDlgSerialSettings, IDC_FLOWCTRL_DTR_DSR, BM_GETCHECK, 0, 0)==BST_UNCHECKED) 
						SendDlgItemMessage(_hDlgSerialSettings, IDC_FLOWCTRL_DTR_DSR, BM_SETCHECK, BST_CHECKED, 0);
				}

				if( FLOWC_RTS_CTS == (userFlowCtrlVal & FLOWC_RTS_CTS) ) 
				{
					if (SendDlgItemMessage(_hDlgSerialSettings, IDC_FLOWCTRL_RTS_CTS, BM_GETCHECK, 0, 0)==BST_UNCHECKED) 
						SendDlgItemMessage(_hDlgSerialSettings, IDC_FLOWCTRL_RTS_CTS, BM_SETCHECK, BST_CHECKED, 0);
				}

				if( FLOWC_XON_XOFF == (userFlowCtrlVal & FLOWC_XON_XOFF) ) 
				{
					if (SendDlgItemMessage(_hDlgSerialSettings, IDC_FLOWCTRL_XON_XOFF, BM_GETCHECK, 0, 0)==BST_UNCHECKED) 
						SendDlgItemMessage(_hDlgSerialSettings, IDC_FLOWCTRL_XON_XOFF, BM_SETCHECK, BST_CHECKED, 0);
				}
			}


			// 2018.03.22
			userHexaDispVal = tempHexaDispVal;
			if( userHexaDispVal )
			{
				if( HEXA_DISP_LOWERCASE == (userHexaDispVal & HEXA_DISP_LOWERCASE) )
				{
					if (SendDlgItemMessage(_hDlgSerialSettings, IDC_HEXA_DISP_LOWERCASE, BM_GETCHECK, 0, 0)==BST_UNCHECKED) 
						SendDlgItemMessage(_hDlgSerialSettings, IDC_HEXA_DISP_LOWERCASE, BM_SETCHECK, BST_CHECKED, 0);
				}

				if( HEXA_DISP_COMMA == (userHexaDispVal & HEXA_DISP_COMMA) ) 
				{
					if (SendDlgItemMessage(_hDlgSerialSettings, IDC_HEXA_DISP_COMMA, BM_GETCHECK, 0, 0)==BST_UNCHECKED) 
						SendDlgItemMessage(_hDlgSerialSettings, IDC_HEXA_DISP_COMMA, BM_SETCHECK, BST_CHECKED, 0);
				}

			#if 1
				if( HEXA_DISP_ADD0X == (userHexaDispVal & HEXA_DISP_ADD0X) ) 
				{
					if (SendDlgItemMessage(_hDlgSerialSettings, IDC_HEXA_DISP_ADD_0X, BM_GETCHECK, 0, 0)==BST_UNCHECKED) 
						SendDlgItemMessage(_hDlgSerialSettings, IDC_HEXA_DISP_ADD_0X, BM_SETCHECK, BST_CHECKED, 0);
				}
			#endif
			}

#endif


#if USER_SCREEN_BUF_SIZE // 2016.02.14
			userScrBufSizIndex = tempScrBufSizIndex;
#endif


#if 0 // deleted 2016.04.13, execute at End
			SetFont( _EditHwnd );

			WriteUserConfig();

			WriteQuickUMONConfig();
			///WriteQuickUserConfig(); /// added.

			LOGFile_open();

		#if 1 // 2016.04.03
			SetRegistry();
		#else
			ThreadSetRegistry(NULL);
		#endif
#endif

			// Destroys a modal dialog box, causing the system to end any processing for the dialog box. 
			if( 0==EndDialog(hDlg,1) ) 
			{
		    	// Error
		    	MessageBox( _MainHwnd, TEXT("Error!!! - OK\n"), "Saved",MB_OK);
			}

			if( hDlg ) { CloseHandle(hDlg); hDlg=NULL; }

			/// ~ 2017.5.29 NEVER~~~~ ~~DestroyWindow( _hDlgSerialSettings ); /// 2017.5.8
			if( _hDlgSerialSettings ) { CloseHandle(_hDlgSerialSettings); _hDlgSerialSettings=NULL; }




		#if defined(COLOR_DISP2) // moved to here 20170831, 2010.05.14
			ctemp = ColorType;
			ColorType = tempColorType;
		#endif
		

		#if DNW_2ND_WINDOWS_ENABLE // 2017.08.31
			sbarTemp = isStatusBarEnable;
			isStatusBarEnable = tempisStatusBarEnable;
		#endif




		#if defined(COLOR_DISP2) || defined(DNW_2ND_WINDOWS_ENABLE) // 2017.08.31
			if( ctemp != ColorType && sbarTemp != isStatusBarEnable)
			{
				MessageBox( _MainHwnd, TEXT("Background color style and StatusBar on/off are changed. \r\n\r\nPlease re-run!!!"), "BackgroundColor, StatusBar",MB_OK);
				SendMessage( _MainHwnd, WM_COMMAND, CM_EXIT, 0 );
			} 
		#endif

		#if defined(COLOR_DISP2) /// 2010.05.14
			/* background color 를 변경하면 이상하여 appl 종료후 다시 실행하라 !!*/
			else if( ctemp != ColorType )
			{
				MessageBox( _MainHwnd, TEXT("Background color style is changed. \r\n\r\nPlease re-run!!!"), "BackgroundColor",MB_OK);
				SendMessage( _MainHwnd, WM_COMMAND, CM_EXIT, 0 );
			}
		#endif


		#if DNW_2ND_WINDOWS_ENABLE // 2017.08.31
			/* Enable StatusBar Changed!! */
			else if( sbarTemp != isStatusBarEnable )
			{
				MessageBox( _MainHwnd, TEXT("StatusBar on/off is changed. \r\n\r\nPlease re-run!!!"), "StatusBar",MB_OK);
				SendMessage( _MainHwnd, WM_COMMAND, CM_EXIT, 0 );
			}
		#endif

			return TRUE;



		case IDCANCEL:
			if( 0==EndDialog(hDlg,0) )
			{
				// Error 
		    	MessageBox( _MainHwnd, TEXT("Error!!! - Cancel\n"), "Cancel",MB_OK);
			}

			if( hDlg ) { CloseHandle(hDlg); hDlg=NULL; }

			/// ~ 2017.5.29 ~~ DestroyWindow( _hDlgSerialSettings ); /// 2017.5.8
			if( _hDlgSerialSettings ) { CloseHandle(_hDlgSerialSettings); _hDlgSerialSettings=NULL; }

			return TRUE;



		case IDC_RADIOCOM0:
		case IDC_RADIOCOM1:
		case IDC_RADIOCOM2:
		case IDC_RADIOCOM3:
		case IDC_RADIOCOM4:
		case IDC_RADIOCOM5:
		case IDC_RADIOCOM6:
		case IDC_RADIOCOM7:
		case IDC_RADIOCOM8:
		case IDC_RADIOCOM9:
		case IDC_RADIOCOM10:
		case IDC_RADIOCOM11:
		case IDC_RADIOCOM12:
		case IDC_RADIOCOM13:
		case IDC_RADIOCOM14:
		case IDC_RADIOCOM15:  
		case IDC_RADIOCOM16:
		case IDC_RADIOCOM17:
		case IDC_RADIOCOM18:
		case IDC_RADIOCOM19:
		case IDC_RADIOCOM20:
		case IDC_RADIOCOM21:
		case IDC_RADIOCOM22:
		case IDC_RADIOCOM23:
		case IDC_RADIOCOM24:
		case IDC_RADIOCOM25:  
		case IDC_RADIOCOM26:
		case IDC_RADIOCOM27:
		case IDC_RADIOCOM28:
		case IDC_RADIOCOM29:
		case IDC_RADIOCOM30:
		case IDC_RADIOCOM31:
		case IDC_RADIOCOM32:
		case IDC_RADIOCOM33:
		case IDC_RADIOCOM34:
		case IDC_RADIOCOM35:  
		case IDC_RADIOCOM36:
		case IDC_RADIOCOM37:
		case IDC_RADIOCOM38:
		case IDC_RADIOCOM39:
		case IDC_RADIOCOM40:
		case IDC_RADIOCOM41:
		case IDC_RADIOCOM42:
		case IDC_RADIOCOM43:
		case IDC_RADIOCOM44:
		case IDC_RADIOCOM45:  
		case IDC_RADIOCOM46:
		case IDC_RADIOCOM47:
		case IDC_RADIOCOM48:
		case IDC_RADIOCOM49:
		case IDC_RADIOCOM50:
		case IDC_RADIOCOM51:
		case IDC_RADIOCOM52:
		case IDC_RADIOCOM53:
		case IDC_RADIOCOM54:
		case IDC_RADIOCOM55:  
		case IDC_RADIOCOM56:
		case IDC_RADIOCOM57:
		case IDC_RADIOCOM58:
		case IDC_RADIOCOM59:
		case IDC_RADIOCOM60:
		case IDC_RADIOCOM61:
		case IDC_RADIOCOM62:
		case IDC_RADIOCOM63:
		case IDC_RADIOCOM64:
		case IDC_RADIOCOM65:
		case IDC_RADIOCOM66:
		case IDC_RADIOCOM67:
		case IDC_RADIOCOM68:
		case IDC_RADIOCOM69:
		case IDC_RADIOCOM70:
		case IDC_RADIOCOM71:
		case IDC_RADIOCOM72:
		case IDC_RADIOCOM73:
		case IDC_RADIOCOM74:
		case IDC_RADIOCOM75:
		case IDC_RADIOCOM76:
		case IDC_RADIOCOM77:
		case IDC_RADIOCOM78:
		case IDC_RADIOCOM79:
		case IDC_RADIOCOM80:
		case IDC_RADIOCOM81:
		case IDC_RADIOCOM82:
		case IDC_RADIOCOM83:
		case IDC_RADIOCOM84:
		case IDC_RADIOCOM85:
		case IDC_RADIOCOM86:
		case IDC_RADIOCOM87:
		case IDC_RADIOCOM88:
		case IDC_RADIOCOM89:
		case IDC_RADIOCOM90:
		case IDC_RADIOCOM91:
		case IDC_RADIOCOM92:
		case IDC_RADIOCOM93:
		case IDC_RADIOCOM94:
		case IDC_RADIOCOM95:
		case IDC_RADIOCOM96:
		case IDC_RADIOCOM97:
		case IDC_RADIOCOM98:
		case IDC_RADIOCOM99:
		case IDC_RADIOCOM100:
		case IDC_RADIOCOM101:
		case IDC_RADIOCOM102:
		case IDC_RADIOCOM103:
		case IDC_RADIOCOM104:
		case IDC_RADIOCOM105:
		case IDC_RADIOCOM106:
		case IDC_RADIOCOM107:
		case IDC_RADIOCOM108:
		case IDC_RADIOCOM109:

		case IDC_RADIOCOM110:
		case IDC_RADIOCOM111:
		case IDC_RADIOCOM112:
		case IDC_RADIOCOM113:
		case IDC_RADIOCOM114:
		case IDC_RADIOCOM115:
		case IDC_RADIOCOM116:
		case IDC_RADIOCOM117:
		case IDC_RADIOCOM118:
		case IDC_RADIOCOM119:
		case IDC_RADIOCOM120:
		case IDC_RADIOCOM121:
		case IDC_RADIOCOM122:
		case IDC_RADIOCOM123:
		case IDC_RADIOCOM124:
		case IDC_RADIOCOM125:
		case IDC_RADIOCOM126:
		case IDC_RADIOCOM127:
		case IDC_RADIOCOM128:
		case IDC_RADIOCOM129:
		case IDC_RADIOCOM130:
		case IDC_RADIOCOM131:
		case IDC_RADIOCOM132:
		case IDC_RADIOCOM133:
		case IDC_RADIOCOM134:
		case IDC_RADIOCOM135:
		case IDC_RADIOCOM136:
		case IDC_RADIOCOM137:
		case IDC_RADIOCOM138:
		case IDC_RADIOCOM139:
		case IDC_RADIOCOM140:
		case IDC_RADIOCOM141:
		case IDC_RADIOCOM142:
		case IDC_RADIOCOM143:
		case IDC_RADIOCOM144:
		case IDC_RADIOCOM145:
		case IDC_RADIOCOM146:
		case IDC_RADIOCOM147:
		case IDC_RADIOCOM148:
		case IDC_RADIOCOM149:
		case IDC_RADIOCOM150:
		case IDC_RADIOCOM151:
		case IDC_RADIOCOM152:
		case IDC_RADIOCOM153:
		case IDC_RADIOCOM154:
		case IDC_RADIOCOM155:
		case IDC_RADIOCOM156:
		case IDC_RADIOCOM157:
		case IDC_RADIOCOM158:
		case IDC_RADIOCOM159:
		case IDC_RADIOCOM160:
		case IDC_RADIOCOM161:
		case IDC_RADIOCOM162:
		case IDC_RADIOCOM163:
		case IDC_RADIOCOM164:
		case IDC_RADIOCOM165:
		case IDC_RADIOCOM166:
		case IDC_RADIOCOM167:
		case IDC_RADIOCOM168:
		case IDC_RADIOCOM169:
		case IDC_RADIOCOM170:
		case IDC_RADIOCOM171:
		case IDC_RADIOCOM172:
		case IDC_RADIOCOM173:
		case IDC_RADIOCOM174:
		case IDC_RADIOCOM175:
		case IDC_RADIOCOM176:
		case IDC_RADIOCOM177:
		case IDC_RADIOCOM178:
		case IDC_RADIOCOM179:
		case IDC_RADIOCOM180:
		case IDC_RADIOCOM181:
		case IDC_RADIOCOM182:
		case IDC_RADIOCOM183:
		case IDC_RADIOCOM184:
		case IDC_RADIOCOM185:
		case IDC_RADIOCOM186:
		case IDC_RADIOCOM187:
		case IDC_RADIOCOM188:
		case IDC_RADIOCOM189:
		case IDC_RADIOCOM190:
		case IDC_RADIOCOM191:
		case IDC_RADIOCOM192:
		case IDC_RADIOCOM193:
		case IDC_RADIOCOM194:
		case IDC_RADIOCOM195:
		case IDC_RADIOCOM196:
		case IDC_RADIOCOM197:
 			tempComPort=LOWORD(wParam)-IDC_RADIOCOM0 /*+1*/;
			return TRUE;


		case IDC_RADIO921600:  /* 921.6 Kbps */
		case IDC_RADIO576000:
		case IDC_RADIO460800:
		case IDC_RADIO380400:
		case IDC_RADIO312500: 
		case IDC_RADIO256000:
		case IDC_RADIO230400:
		case IDC_RADIO153600:  // added 2011.1.27, AVC-LAN comm.
		case IDC_RADIO128000:
		case IDC_RADIO115200:
		case IDC_RADIO76800: 
		case IDC_RADIO57600:
		case IDC_RADIO56000:
		case IDC_RADIO41600:
		case IDC_RADIO38400:
		case IDC_RADIO31250: 
		case IDC_RADIO19200:
		case IDC_RADIO14400:
		case IDC_RADIO10400:
		case IDC_RADIO9600:
		case IDC_RADIO4800:
		case IDC_RADIO2400:
		//case IDC_RADIO1200:


			tempIdBaudRate=LOWORD(wParam)-IDC_RADIO921600;

			bBaudrate = TRUE;

			userBaudRate = baudTable[tempIdBaudRate];

			memset( szUserBaudRate, 0x00, sizeof(szUserBaudRate) );
			wsprintf(szUserBaudRate, TEXT("%d"), userBaudRate);


			// display clear ---------------
			// If the start is 0 and the end is -1, all the text in the edit control is selected. 
			// If the start is -1, any current selection is deselected. 
			SendMessage(GetDlgItem(hDlg,IDC_EDIT2),EM_SETSEL,0,-1); // all the text is selected. 
			SendMessage(GetDlgItem(hDlg,IDC_EDIT2),EM_REPLACESEL,0, (LPARAM)""); 
			
			/// new address displayed
			SendMessage(GetDlgItem(hDlg,IDC_EDIT2),EM_REPLACESEL, (WPARAM)0,(LPARAM)(LPCSTR)szUserBaudRate); 

			return TRUE;

	
		case IDC_RADIO_AUTO_SEND_OFF:
		case IDC_RADIO_AUTO_SEND_ON:
			tempAutoSend=LOWORD(wParam)-IDC_RADIO_AUTO_SEND_OFF;
			return TRUE;

#if LOG_SAVE_MENU_IN_SETUP // 2016.02.24
		case IDC_RADIO_MSG_SAVE_OFF:
		case IDC_RADIO_MSG_SAVE_ON:
			tempMsgSave=LOWORD(wParam)-IDC_RADIO_MSG_SAVE_OFF;
			return TRUE;
#endif

		case IDC_FONT0_OEM_FIX:
		case IDC_FONT1_ANSI_FIX:
		case IDC_FONT2_DEVICE:
		case IDC_FONT3_SYSTEM_FIX:
			tempFontType=LOWORD(wParam)-IDC_FONT0_OEM_FIX;
			return TRUE;

		case IDC_RETRY_USB_DEFAULT:
		case IDC_RETRY_USB_20_TIMES:
		case IDC_RETRY_USB_30_TIMES:
		case IDC_RETRY_USB_40_TIMES:
			tempRetryUSB=LOWORD(wParam)-IDC_RETRY_USB_DEFAULT;
			return TRUE;

	#if HEXA_MODE_SUPPORT	/* 2011.02.08 Hexa mode display */
		/* Text display or Hexa display */
		case IDC_OUT_TEXT_MODE:
		case IDC_OUT_HEXA1_MODE:
		case IDC_OUT_HEXA2_MODE:
			tempTextorHexaOutMode=LOWORD(wParam)-IDC_OUT_TEXT_MODE;

			if( 1==tempTextorHexaOutMode ) // Hexa1_Line mode
			{
				EnableWindow( GetDlgItem(hDlg, IDC_EDIT_HEX1_COL ), 1 ); // Enabled!!		
				EnableWindow( GetDlgItem(hDlg, IDC_EDIT_HEX2_COL ), 0 ); // Disable!!

				// Enable Check Box
				EnableWindow( GetDlgItem(hDlg, IDC_HEXA_DISP_LOWERCASE ), 1 ); // Enabled!! 	
				EnableWindow( GetDlgItem(hDlg, IDC_HEXA_DISP_COMMA ), 1 ); // Enabled!!
				EnableWindow( GetDlgItem(hDlg, IDC_HEXA_DISP_ADD_0X ), 1 ); // Enabled!!
			}
			else if( 2==tempTextorHexaOutMode ) // Hexa2_Char mode
			{
				EnableWindow( GetDlgItem(hDlg, IDC_EDIT_HEX1_COL ), 0 ); // Disable!!
				EnableWindow( GetDlgItem(hDlg, IDC_EDIT_HEX2_COL ), 1 ); // Enabled!!

				// Enable Check Box
				EnableWindow( GetDlgItem(hDlg, IDC_HEXA_DISP_LOWERCASE ), 1 ); // Enabled!! 	
				EnableWindow( GetDlgItem(hDlg, IDC_HEXA_DISP_COMMA ), 1 ); // Enabled!!
				EnableWindow( GetDlgItem(hDlg, IDC_HEXA_DISP_ADD_0X ), 1 ); // Enabled!!
			}
			else // Text Mode
			{
				EnableWindow( GetDlgItem(hDlg, IDC_EDIT_HEX1_COL ), 0 ); // Disable!!
				EnableWindow( GetDlgItem(hDlg, IDC_EDIT_HEX2_COL ), 0 ); // Disable!!

				// Disable Check Box
				EnableWindow( GetDlgItem(hDlg, IDC_HEXA_DISP_LOWERCASE ), 0 ); // Disabled!!		
				EnableWindow( GetDlgItem(hDlg, IDC_HEXA_DISP_COMMA ), 0 ); // Disabled!!
				EnableWindow( GetDlgItem(hDlg, IDC_HEXA_DISP_ADD_0X ), 0 ); // Disabled!!

			}

			return TRUE;
	#endif


	#if 0
		case IDC_TX_CHAR_MODE:
		case IDC_TX_HEXA_MODE:
			tempTxDataType = LOWORD(wParam)-IDC_TX_CHAR_MODE;

			if( 0==tempTxDataType )// Char Data
			{
				EnableWindow( GetDlgItem(hDlg, IDC_TX_EDIT_CHAR_COL ), 1 ); // Enabled!!		
				EnableWindow( GetDlgItem(hDlg, IDC_TX_EDIT_HEXA_COL ), 0 ); // Disable!!		
			}
			else if( 1==tempTxDataType ) // Hexa Data
			{
				EnableWindow( GetDlgItem(hDlg, IDC_TX_EDIT_CHAR_COL ), 0 ); // Disable!!		
				EnableWindow( GetDlgItem(hDlg, IDC_TX_EDIT_HEXA_COL ), 1 ); // Enabled!!		
			}

			return TRUE;	


		case IDC_TX_EDIT_CHAR_COL:
		case IDC_TX_EDIT_HEXA_COL:
			return TRUE;


		case IDC_TX_DISPLAY_OFF:
		case IDC_TX_DISPLAY_ON:
			tempMsgOnOff = LOWORD(wParam)-IDC_TX_DISPLAY_OFF;
			return TRUE;
	#endif


	#if defined(COLOR_DISP2) // 2010.05.14
		case IDC_COLOR_TYPE_GRAY_DEF:
		case IDC_COLOR_TYPE_BLACK:
		case IDC_COLOR_TYPE_GRAY1:
		case IDC_COLOR_TYPE_GRAY2:
			tempColorType = LOWORD(wParam)-IDC_COLOR_TYPE_GRAY_DEF;

			// 2017.04.24
			if( 1 == tempColorType ) 
			{
				BGColor = COLOR_BLACK;
				InvalidateRect(_EditHwnd, NULL, TRUE);
			}
			else if( 2 == tempColorType )
			{
				BGColor = COLOR_GRAY0;
				InvalidateRect(_EditHwnd, NULL, TRUE);
			}
			else if( 3 == tempColorType )
			{
				BGColor = COLOR_GRAY9;
				InvalidateRect(_EditHwnd, NULL, TRUE);
			}
			
			return TRUE;
	#endif 


	#ifdef COM_SETTINGS // 2012.07.11
		// 2015.12.30 added.
		case ID_COM_DEFAULT:
			tempCOMparity = userCOMparity=0;
			tempCOMstop = userCOMstop=0;
			tempCOMdataIdx = userCOMdataIdx=1;

			/* COM port - parity */
			CheckRadioButton(hDlg,IDC_COM_NOPARITY, IDC_COM_SPACEPARITY, IDC_COM_NOPARITY+tempCOMparity);
			/* COM port - stop bit */
			CheckRadioButton(hDlg,IDC_COM_ONESTOPBIT, IDC_COM_TWOSTOPBITS, IDC_COM_ONESTOPBIT+tempCOMstop);
			/* data bit */
			CheckRadioButton(hDlg,IDC_COM_DATA_7BIT, IDC_COM_DATA_8BIT, IDC_COM_DATA_7BIT+tempCOMdataIdx);

			
			// 2017.08.03, COM Flow Control
			tempFlowControl = userFlowControl = 0;  // clear to index 0
			CheckRadioButton(hDlg,IDC_FLOWCTRL_NONE, IDC_FLOWCTRL_USED, IDC_FLOWCTRL_NONE+tempFlowControl);

			switch( tempFlowControl )
			{
				case 0: // None
					EnableWindow( GetDlgItem(hDlg, IDC_FLOWCTRL_DTR_DSR ), 0 ); // Disabled!!		
					EnableWindow( GetDlgItem(hDlg, IDC_FLOWCTRL_RTS_CTS ), 0 ); // Disabled!!
					EnableWindow( GetDlgItem(hDlg, IDC_FLOWCTRL_XON_XOFF ), 0 ); // Disabled!!
					break;
				case 1: // Use
					EnableWindow( GetDlgItem(hDlg, IDC_FLOWCTRL_DTR_DSR ), 1 ); // Enabled!!		
					EnableWindow( GetDlgItem(hDlg, IDC_FLOWCTRL_RTS_CTS ), 1 ); // Enabled!!
					EnableWindow( GetDlgItem(hDlg, IDC_FLOWCTRL_XON_XOFF ), 1 ); // Enabled!!
					break;
			}
			
		

		#if 1 // 2017.08.04, Flow Control
			tempFlowCtrlVal = userFlowCtrlVal = 0; // 2017.08.04

			if (SendDlgItemMessage(_hDlgSerialSettings, IDC_FLOWCTRL_DTR_DSR, BM_GETCHECK, 0, 0)==BST_CHECKED) 
				SendDlgItemMessage(_hDlgSerialSettings, IDC_FLOWCTRL_DTR_DSR, BM_SETCHECK, BST_UNCHECKED, 0);

			if (SendDlgItemMessage(_hDlgSerialSettings, IDC_FLOWCTRL_RTS_CTS, BM_GETCHECK, 0, 0)==BST_CHECKED) 
				SendDlgItemMessage(_hDlgSerialSettings, IDC_FLOWCTRL_RTS_CTS, BM_SETCHECK, BST_UNCHECKED, 0);

			if (SendDlgItemMessage(_hDlgSerialSettings, IDC_FLOWCTRL_XON_XOFF, BM_GETCHECK, 0, 0)==BST_CHECKED) 
				SendDlgItemMessage(_hDlgSerialSettings, IDC_FLOWCTRL_XON_XOFF, BM_SETCHECK, BST_UNCHECKED, 0);
		#endif
	


			//SetRegistry();
			//EB_Printf(TEXT("[dnw] ID_COM_DEFAULT. \n") );
			return TRUE;



		/* 2012.07.11, COM port - parity */
		case IDC_COM_NOPARITY: 
		case IDC_COM_ODDPARITY:
		case IDC_COM_EVENPARITY:
		case IDC_COM_MARKPARITY:
		case IDC_COM_SPACEPARITY:
			tempCOMparity=LOWORD(wParam)-IDC_COM_NOPARITY;
			return TRUE;
		
		case IDC_COM_ONESTOPBIT: // 1 stop bit
		case IDC_COM_ONE5STOPBITS: // 1.5 stop bits.
		case IDC_COM_TWOSTOPBITS: // 2 (2 stop bit
			tempCOMstop=LOWORD(wParam)-IDC_COM_ONESTOPBIT;
			return TRUE;
		
		case IDC_COM_DATA_7BIT: /// 7 bit
		case IDC_COM_DATA_8BIT: /// 8 bit
			tempCOMdataIdx=LOWORD(wParam)-IDC_COM_DATA_7BIT; /* 7, 8*/
			return TRUE;


		case IDC_FLOWCTRL_NONE:
		case IDC_FLOWCTRL_USED:
  			tempFlowControl=LOWORD(wParam)-IDC_FLOWCTRL_NONE; // Flow Control, NONE or USED!!

			switch( tempFlowControl )
			{
				case 0: // None
					EnableWindow( GetDlgItem(hDlg, IDC_FLOWCTRL_DTR_DSR ), 0 ); // Disabled!!		
					EnableWindow( GetDlgItem(hDlg, IDC_FLOWCTRL_RTS_CTS ), 0 ); // Disabled!!
					EnableWindow( GetDlgItem(hDlg, IDC_FLOWCTRL_XON_XOFF ), 0 ); // Disabled!!
					break;
				case 1: // Use
					EnableWindow( GetDlgItem(hDlg, IDC_FLOWCTRL_DTR_DSR ), 1 ); // Enabled!!		
					EnableWindow( GetDlgItem(hDlg, IDC_FLOWCTRL_RTS_CTS ), 1 ); // Enabled!!
					EnableWindow( GetDlgItem(hDlg, IDC_FLOWCTRL_XON_XOFF ), 1 ); // Enabled!!
					break;
			}
			return TRUE;


		// 2017.08.03 Flow COntrol
		case IDC_FLOWCTRL_DTR_DSR:
		case IDC_FLOWCTRL_RTS_CTS:
		case IDC_FLOWCTRL_XON_XOFF:

			switch (HIWORD(wParam))
			{
				case BN_CLICKED:
					if (SendDlgItemMessage(_hDlgSerialSettings, LOWORD(wParam), BM_GETCHECK, 0, 0)) 
					{
						// MessageBox(NULL, TEXT("Checkbox Selected"), TEXT("Success"), MB_OK | MB_ICONINFORMATION);
						if( IDC_FLOWCTRL_DTR_DSR==LOWORD(wParam) )  userFlowCtrlVal |= FLOWC_DTR_DSR;
						if( IDC_FLOWCTRL_RTS_CTS==LOWORD(wParam) )  userFlowCtrlVal |= FLOWC_RTS_CTS;
						if( IDC_FLOWCTRL_XON_XOFF==LOWORD(wParam) ) userFlowCtrlVal |= FLOWC_XON_XOFF;

						//EB_Printf( TEXT("[dnw] Flow Control 0x%02X == ON \r\n"), userFlowCtrlVal );
					}
					else
					{
						// MessageBox(NULL, TEXT("Checkbox Unselected"), TEXT("Success"), MB_OK | MB_ICONINFORMATION);
						if( IDC_FLOWCTRL_DTR_DSR==LOWORD(wParam) )  userFlowCtrlVal &= ~FLOWC_DTR_DSR;
						if( IDC_FLOWCTRL_RTS_CTS==LOWORD(wParam) )  userFlowCtrlVal &= ~FLOWC_RTS_CTS;
						if( IDC_FLOWCTRL_XON_XOFF==LOWORD(wParam) ) userFlowCtrlVal &= ~FLOWC_XON_XOFF;

						//EB_Printf( TEXT("[dnw] Flow Control 0x%02X == OFF \r\n"), userFlowCtrlVal );
					}

					tempFlowCtrlVal = userFlowCtrlVal;
					break; 
			}

			return TRUE;

	#endif // COM_SETTINGS ---------------


		// 2018.03.22 Hexa Display
		case IDC_HEXA_DISP_LOWERCASE:
		case IDC_HEXA_DISP_COMMA:
		case IDC_HEXA_DISP_ADD_0X:
		
			switch (HIWORD(wParam))
			{
				case BN_CLICKED:
					if (SendDlgItemMessage(_hDlgSerialSettings, LOWORD(wParam), BM_GETCHECK, 0, 0)) 
					{
						// MessageBox(NULL, TEXT("Checkbox Selected"), TEXT("Success"), MB_OK | MB_ICONINFORMATION);
						if( IDC_HEXA_DISP_LOWERCASE==LOWORD(wParam) )	userHexaDispVal |= HEXA_DISP_LOWERCASE;
						if( IDC_HEXA_DISP_COMMA==LOWORD(wParam) )	    userHexaDispVal |= HEXA_DISP_COMMA;
						if( IDC_HEXA_DISP_ADD_0X==LOWORD(wParam) )      userHexaDispVal |= HEXA_DISP_ADD0X;
		
						//EB_Printf( TEXT("[dnw] Hexa Display 0x%02X == ON \r\n"), userHexaDispVal );
					}
					else
					{
						// MessageBox(NULL, TEXT("Checkbox Unselected"), TEXT("Success"), MB_OK | MB_ICONINFORMATION);
						if( IDC_HEXA_DISP_LOWERCASE==LOWORD(wParam) )	userHexaDispVal &= ~HEXA_DISP_LOWERCASE;
						if( IDC_HEXA_DISP_COMMA==LOWORD(wParam) )	    userHexaDispVal &= ~HEXA_DISP_COMMA;
						if( IDC_HEXA_DISP_ADD_0X==LOWORD(wParam) )      userHexaDispVal &= ~HEXA_DISP_ADD0X;
		
						//EB_Printf( TEXT("[dnw] Hexa Display 0x%02X == OFF \r\n"), userHexaDispVal );
					}
		
					tempHexaDispVal = userHexaDispVal;
					break; 
			}
		
			return TRUE;


#if USER_SCREEN_BUF_SIZE // 2016.02.14
		case IDC_SCR_BUF_HUGE: // screen buffer size :100MB
		case IDC_SCR_BUF_LARGE: // screen buffer size :60MB
		case IDC_SCR_BUF_MIDDLE: // screen buffer size :20MB
		case IDC_SCR_BUF_SMALL: // screen buffer size :2MB
		case IDC_SCR_BUF_SMALLEST: // screen buffer size :1MB
			tempScrBufSizIndex=LOWORD(wParam)-IDC_SCR_BUF_HUGE;
			return TRUE;
#endif


#if DNW_2ND_WINDOWS_ENABLE // 2017.4.14
		case IDC_STATUS_BAR_OFF:
		case IDC_STATUS_BAR_ON:
		case IDC_STATUS_CMD_ON:
			tempisStatusBarEnable = LOWORD(wParam)-IDC_STATUS_BAR_OFF;

		#if 0 // Deleted it 2017.08.31
			if( sbarTemp != tempisStatusBarEnable )
			{

			#if DNW_2ND_COMMAND // 2017.4.22
				if( BAR_COMMAND==tempisStatusBarEnable )
					MessageBox( _hDlgSerialSettings, TEXT("Restart to activate the command bar!!! \n"), "CommandBar On",MB_OK);
				else
			#endif
				if( BAR_STATUS==tempisStatusBarEnable )
					MessageBox( _hDlgSerialSettings, TEXT("Restart to activate the status bar!!! \n"), "StatusBar On",MB_OK);
				else
					MessageBox( _hDlgSerialSettings, TEXT("Restart to disable the status bar!!! \n"), "StatusBar Off",MB_OK);

				sbarTemp = tempisStatusBarEnable;

				//EB_Printf( TEXT("[dnw] Restart to %s the status bar!!! \r\n"), tempisStatusBarEnable ? TEXT("activate") : TEXT("disable") );
				BeepSound(4);
			}
		#endif
		
			return TRUE;

#endif // DNW_2ND_WINDOWS_ENABLE

		// 2020.04.07
		case ID_LOG_FILE_AUTO:
		case ID_LOG_FILE_POPUP:
			tempLogFile=LOWORD(wParam)-ID_LOG_FILE_AUTO;
			return TRUE;
		

		case IDC_LOCALTIME_NONE:
		case IDC_LOCALTIME_TIMEONLY:
		case IDC_LOCALTIME_DATETIME:
	#if MODIFIED_JULIAN_DATE // 2017.08.30
		case IDC_LOCALTIME_MODIFIED_JD:
	#endif
		case IDC_LOCALTIME_DATETIME_WEEK: // 2020.06.30
		case IDC_LOCALTIME_LINUX_DEC:
		case IDC_LOCALTIME_LINUX_HEX:
			
			tempLocalTime=LOWORD(wParam)-IDC_LOCALTIME_NONE;
			return TRUE;

#if 0 // 2020.08.14
		// 2019.12.21, added
		case IDC_CR_LF_ENABLE:
		case IDC_CR_ONLY_ENABLE:
		case IDC_LF_ONLY_ENABLE:
			tempCRLF=LOWORD(wParam)-IDC_CR_LF_ENABLE;
			return TRUE;
#endif

		// 2015.12.14, added
		case IDC_CPU_SMDK6410:
		case IDC_CPU_SMDKC100:
		case IDC_CPU_S3C2440A: // 2017.07.20
		case IDC_CPU_OTHER:

			tempCPUtype=LOWORD(wParam)-IDC_CPU_SMDK6410;

			/// 2014.04.11, added
			switch( tempCPUtype )
			{
				case 0: // SMDK6410,  IDC_CPU_SMDK6410
					downloadAddress=0x50100000;
					memset( szDownloadAddress, 0x00, sizeof(szDownloadAddress) );
					lstrcpy(szDownloadAddress,TEXT("0x50100000"));
					break;

				case 1: // SMDKC100, IDC_CPU_SMDKC100
					downloadAddress=0x20030000;
					memset( szDownloadAddress, 0x00, sizeof(szDownloadAddress) );
					lstrcpy(szDownloadAddress,TEXT("0x20030000"));
					break;

				case 2: // S3C2440A
					downloadAddress=0x30000000;
					memset( szDownloadAddress, 0x00, sizeof(szDownloadAddress) );
					lstrcpy(szDownloadAddress,TEXT("0x30000000"));
					break;

				case 3: // IDC_CPU_OTHER : User input --
				default:
					GetRegistry_USBAddr();

					//downloadAddress=0x20090000;
					//memset( szDownloadAddress, 0x00, sizeof(szDownloadAddress) );
					//lstrcpy(szDownloadAddress,TEXT("0x20090000"));
					break;
			}
			CPUtype = tempCPUtype;

			// display clear ---------------
			// If the start is 0 and the end is -1, all the text in the edit control is selected. 
			// If the start is -1, any current selection is deselected. 
			SendMessage(GetDlgItem(hDlg,IDC_EDIT1),EM_SETSEL,0,-1); // all the text is selected. 
			SendMessage(GetDlgItem(hDlg,IDC_EDIT1),EM_REPLACESEL,0, (LPARAM)""); 

			/// new address displayed
			SendMessage(GetDlgItem(hDlg,IDC_EDIT1),EM_REPLACESEL, (WPARAM)0,(LPARAM)(LPCSTR)szDownloadAddress);	

			return TRUE;

		default:
			return FALSE;


		}

#if 0
	case WM_SETFOCUS:
		SetFocus( hDlg );
		EB_Printf( TEXT("[dnw] WM_SETFOCUS in OptionsProc ---\r\n") );
		return TRUE;
#endif


	default:
		return FALSE;
	}

	return FALSE;
}





#if USE_HEX2BIN_CONVERT // 2016.03.04


/*********************************************************************
 *                                                                   *
 *   Library         : lib_crc                                       *
 *   File            : lib_crc.c                                     *
 *   Author          : Lammert Bies  1999-2008                       *
 *   E-mail          : info@lammertbies.nl                           *
 *   Language        : ANSI C                                        *
 *                                                                   *
 *                                                                   *
 *   Description                                                     *
 *   ===========                                                     *
 *                                                                   *
 *   The file lib_crc.c contains the private  and  public  func-     *
 *   tions  used  for  the  calculation of CRC-16, CRC-CCITT and     *
 *   CRC-32 cyclic redundancy values.                                *
 *                                                                   *
 *                                                                   *
 *   Dependencies                                                    *
 *   ============                                                    *
 *                                                                   *
 *   libcrc.h       CRC definitions and prototypes                   *
 *                                                                   *
 ********************************************************************/

#include "binary.h"
#include "Libcrc.h"


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










/*---------------------------------------------------------------------------*
 * binary.c                                                                  *
 * Copyright (C) 2014  Jacques Pelletier                                     *
 *                                                                           *
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:
  Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
  Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/

//#include <stdint.h>


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
	return (uint8_t)((value & 0xFF00000000000000) >> 56);
}

uint8_t u64_b6(uint64_t value)
{
	return (uint8_t)((value & 0x00FF000000000000) >> 48);
}

uint8_t u64_b5(uint64_t value)
{
	return (uint8_t)((value & 0x0000FF0000000000) >> 40);
}

uint8_t u64_b4(uint64_t value)
{
	return (uint8_t)((value & 0x000000FF00000000) >> 32);
}

uint8_t u64_b3(uint64_t value)
{
	return (uint8_t)((value & 0x00000000FF000000) >> 24);
}

uint8_t u64_b2(uint64_t value)
{
	return (uint8_t)((value & 0x0000000000FF0000) >> 16);
}

uint8_t u64_b1(uint64_t value)
{
	return (uint8_t)((value & 0x000000000000FF00) >> 8);
}

uint8_t u64_b0(uint64_t value)
{
	return (uint8_t)(value & 0x00000000000000FF);
}

/* Checksum/CRC conversion to ASCII */
uint8_t nibble2ascii(uint8_t value)
{
  uint8_t result = value & 0x0f;

  if (result > 9) return result + 0x41-0x0A;
  else return result + 0x30;
}

int cs_isdecdigit(char c)
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






/* The data records can contain 255 bytes: this means 512 characters. */
#define MAX_LINE_SIZE 1024

//typedef unsigned char byte;
//typedef unsigned short word;

#define LAST_CHECK_METHOD 4


#define TYPE_CHK8_SUM  			0
#define TYPE_CHK16 				1
#define TYPE_CRC8 				2
#define TYPE_CRC16 				3
#define TYPE_CRC32 				4
#define TYPE_CHK16_8 			5 // 2017.04.13


FILE        *Filin=NULL,             /* input files */
            *Filout=NULL;            /* output files */





/* This will hold binary codes translated from hex file. */
unsigned char *Memory_Block = NULL;
unsigned int Lowest_Address, Highest_Address = 0;
unsigned int Starting_Address=0, Phys_Addr;

unsigned int Phys_AddrTemp = 0x00000000; // added at 2016.03.10
unsigned int iOnceCheck = 1; // added at 2016.03.10
unsigned int iOnceINTELlower = 1; // added at 2020.06.29

unsigned int Records_Start = 0; // Lowest address of the records

unsigned int Max_Length = 0;
unsigned int Real_Length = 0; // 2016.03.07, real file size

unsigned int Minimum_Block_Size = 0x1000; // 4096 byte
int Module;


int Minimum_Block_Size_Setted = 0; // -m option: The output file size will be a multiple of Minimum block size. 
			// It will be filled with FF or the specified pattern.
			// Length must be a power of 2 in hexadecimal [see -l option].
			// Attention this option is STRONGER than Maximal Length.
			// hex2bin -m [size] example.hex


int Starting_Address_Setted = 0;   // -s option: If the lowest address isn't 0000, 
			// ex: 0100: (the first record begins with :nn010000xxx ) there will be problems 
			// when using the binary file to program a EPROM since the first byte supposed to be at 0100 is 
			// stored in the binary file at 0000.
			// you can specify the binary file's starting address on the command line:
			// hex2bin -s 0000 start_at_0100.hex
			// The bytes will be stored in the binary file with a padding from 0000 to the lowest address 
			// minus 1 (00FF in this case).
			// Similarly, the binary file can be padded up to Length -1 with FF or another byte.
			// Here, the space between the last byte and 07FF will be filled with FF.
			// hex2bin -l 0800 ends_before_07FF.hex
			// EPROM, EEPROM and Flash memories contain all FF when erased.
			// When the source file name is for-example.test.hex the binary created will have 
			// the name for-example.bin the ".test" part will be dropped.
			// Hex2bin/mot2bin assume the source file doesn't contain overlapping records, 
			// if so, overlaps will be reported.


int Max_Length_Setted = 0;         // -l option
int Swap_Wordwise = 0;             // -w option : for each pair of bytes, exchange the low and high part.

int Address_Alignment_Word = 0;    // -a option : Hex with record type, where data is represented in Word (2 Byte)
			// e.g Texas Instruments: TMS320F2835, TMS320F28065.Some compilers such as Microchip's MPLAB IDE can generate byte swapped hex files.
			// -a Address Alignment Word.

int Batch_Mode = 0;                // -b option: Normally, if the specified hex file doesn't exist, 
			// hex2bin/mot2bin ask repeatedly for a valid filename. 
			// A batch/script mode option is provided for exiting with an error instead of asking for a file.
			// If the file xxxx.hex doesn't exist, the program exits immediately with the error code = 1.


int Verbose_Flag = 0;              // -v option Off

int Endian = 0;                    // -E option

int Cks_Type = TYPE_CRC8;          // -k option TYPE_CHK8_SUM;


unsigned int Cks_Start = 0, Cks_End = 0; // -r option
int Cks_range_set = 0;             // -r option


unsigned int Cks_Addr = 0, Cks_Value = 0;
int Cks_Addr_set = 0;	           // -f option

int Force_Value = 0;               // -F option


unsigned int Crc_Poly = 0x07, Crc_Init = 0, Crc_XorOut = 0; // -C option
int Crc_RefIn = 0;
int Crc_RefOut = 0;



int Pad_Byte = 0xFF;               // -p option : By default, unused locations will be filled with FF. 
								   // Another value can be specified.
int Enable_Checksum_Error = 1;     // -c option, CRC verify On default refer userCRCverify=1;
			// By default, it ignores checksum errors, 
			// so that someone can change by hand some bytes allowing quick and dirty changes.
			// If you want checksum error reporting, specify the option -c.
			// If there is a checksum error somewhere, the program will continue the conversion anyway. 
			// For convenience, hex2bin/mot2bin displays the expected checksum at each faulty records.


int Status_Checksum_Error = 0; // 0: No CRC Error, 1: CRC error
unsigned char Checksum;
unsigned int Record_Nb = 0;
unsigned int Nb_Bytes;


int Enable_HexaAddr_Zero_Forced = 0; // HEX2BIN_REAL_ADDR; // 2016.03.10, hex file address zero-forced!!










// ----------------------------------------------------------
// ----------------------------------------------------------
// ----------------------------------------------------------
// ----------------------------------------------------------
// ----------------------------------------------------------

#define NO_ADDRESS_TYPE_SELECTED 0
#define LINEAR_ADDRESS 1
#define SEGMENTED_ADDRESS 2

//const char *Pgm_Name = PROGRAM;



#if 0 // --------------------------------------
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
             "  -k [0-4]      Select check method (checksum or CRC) and size\n"
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
             "1:  Checksum 16-bit\n"
             "2:  CRC8\n"
             "3:  CRC16\n"
             "4:  CRC32\n");
    exit(1);
}

#define LAST_CHECK_METHOD 4

void *NoFailMalloc (size_t size)
{
    void *result;

    if ((result = malloc (size)) == NULL)
    {
        fprintf (stderr,"Can't allocate memory.\n");
        exit(1);
    }
    return (result);
}

/* Open the input file, with error checking */
void NoFailOpenInputFile (char *Flnm)
{
    while ((Filin = fopen(Flnm,"r")) == NULL)
    {
    	if (Batch_Mode)
    	{
			fprintf (stderr,"Input file %s cannot be opened.\n", Flnm);
    		exit(1);
    	}
    	else
    	{
			fprintf (stderr,"Input file %s cannot be opened. Enter new filename: ",Flnm);
			if (Flnm[strlen(Flnm) - 1] == '\n') Flnm[strlen(Flnm) - 1] = '\0';
    	}
    }

#ifdef USE_FILE_BUFFERS
    FilinBuf = (char *) NoFailMalloc (BUFFSZ);
    setvbuf(Filin, FilinBuf, _IOFBF, BUFFSZ);
#endif
} /* procedure OPENFILIN */

/* Open the output file, with error checking */
void NoFailOpenOutputFile (char *Flnm)
{
    while ((Filout = fopen(Flnm,"wb")) == NULL)
    {
    	if (Batch_Mode)
    	{
			fprintf (stderr,"Output file %s cannot be opened.\n", Flnm);
    		exit(1);
    	}
    	else
    	{
			/* Failure to open the output file may be
			 simply due to an insufficient permission setting. */
			fprintf(stderr,"Output file %s cannot be opened. Enter new file name: ", Flnm);
			if (Flnm[strlen(Flnm) - 1] == '\n') Flnm[strlen(Flnm) - 1] = '\0';
    	}
    }

#ifdef USE_FILE_BUFFERS
    FiloutBuf = (char *) NoFailMalloc (BUFFSZ);
    setvbuf(Filout, FiloutBuf, _IOFBF, BUFFSZ);
#endif

} /* procedure OPENFILOUT */
#endif // -------------------------------------------------------



void GetLine(char* str, FILE *in)
{
    char *result = NULL;

    result = fgets(str,MAX_LINE_SIZE,in);
    if ((result == NULL) && !feof (in)) 
    {
    	EB_Printf(TEXT("[dnw] Error occurred while reading from file. \n") );
    }
}


#ifdef __NOT_USED__
// 0 or 1
int GetBin(const char *str)
{
    int result;
    unsigned int value;

    result = sscanf(str,"%u",&value);

    if (result == 1) return value & 1;
    else
    {
        fprintf(stderr,"GetBin: some error occurred when parsing options.\n");
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
        fprintf(stderr,"GetDec: some error occurred when parsing options.\n");
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
        fprintf(stderr,"GetHex: some error occurred when parsing options.\n");
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

void GetExtension(const char *str,char *ext)
{
    if (strlen(str) > MAX_EXTENSION_SIZE)
        usage();

    strcpy(ext, str);
}

/* Adds an extension to a file name */
void PutExtension(char *Flnm, char *Extension)
{
    char        *Period;        /* location of period in file name */
    bool     Samename;

    /* This assumes DOS like file names */
    /* Don't use strchr(): consider the following filename:
     ../my.dir/file.hex
    */
    if ((Period = strrchr(Flnm,'.')) != NULL)
        *(Period) = '\0';

    Samename = false;
    if (strcmp(Extension, Period+1) == 0)
        Samename = true;

    strcat(Flnm,".");
    strcat(Flnm, Extension);
    if (Samename)
    {
        fprintf (stderr,"Input and output filenames (%s) are the same.", Flnm);
        exit(1);
    }
}
#endif // __NOT_USED__


void VerifyChecksumValue(unsigned int CalcChecksum, unsigned char rdChecksum)
{
    if ((Checksum != 0) && Enable_Checksum_Error)
	{
	#if 0
		fprintf(stderr,"Checksum error in record %d: should be %02X\n",
			Record_Nb, (256 - Checksum) & 0xFF);
	#else
    	EB_Printf(TEXT("[dnw] Checksum error in record %d: should be 0x%02X. wrong data: 0x%02X  \r\n"), Record_Nb, CalcChecksum%256, rdChecksum&0xFF ); // , (256-Checksum)&0xFF );
	#endif

		Status_Checksum_Error = 1; // true;
	}


#if 0
	Checksum = ( ~Checksum + 1 ) % 256;
	if (Checksum != RecChkSum) 
	{
		Status_Checksum_Error = 1;

    	EB_Printf(TEXT("[dnw] Checksum error in record %d: should be 0x%02X\n"), Record_Nb, Checksum );
		
		break;
	}	
#endif

}


void CrcParamsCheck(void)
{
    switch (Cks_Type)
    {
    case TYPE_CRC8:
        Crc_Poly &= 0xFF;
        Crc_Init &= 0xFF;
        Crc_XorOut &= 0xFF;
        break;

    case TYPE_CRC16:
        Crc_Poly &= 0xFFFF;
        Crc_Init &= 0xFFFF;
        Crc_XorOut &= 0xFFFF;
        break;

    case TYPE_CRC32:
        break;

    default:

	#if 0
        fprintf (stderr,"See file CRC list.txt for parameters\n");
        exit(1);
	#else
		EB_Printf(TEXT("[dnw] See file CRC list.txt for parameters. select CRC8 or CRC16 or CRC32. (%d) \r\n"), Cks_Type );
	#endif

    }
}


void WriteMemBlock16(uint16_t Value)
{
    if (Endian == 1)
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
    if (Endian == 1)
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


int WriteMemory(void)
{
	unsigned int i; // modified at 2016.03.05

    if ((Cks_Addr >= Lowest_Address) && (Cks_Addr < Highest_Address))
    {
        if(Force_Value)
        {
            switch (Cks_Type)
            {
                case 0:
                    Memory_Block[Cks_Addr - Lowest_Address] = Cks_Value;
                    //fprintf(stdout,"Addr %08X set to %02X\n",Cks_Addr, Cks_Value);
					EB_Printf(TEXT("[dnw] 0-Addr 0x%08X set to 0x%02X\n"),Cks_Addr, Cks_Value );
                    break;
                case 1:
                    WriteMemBlock16(Cks_Value);
                    //fprintf(stdout,"Addr %08X set to %04X\n",Cks_Addr, Cks_Value);
					EB_Printf(TEXT("[dnw] 1-Addr 0x%08X set to 0x%04X\n"),Cks_Addr, Cks_Value );
                    break;
                case 2:
                    WriteMemBlock32(Cks_Value);
                    //fprintf(stdout,"Addr %08X set to %08X\n",Cks_Addr, Cks_Value);
					EB_Printf(TEXT("[dnw] 2-Addr 0x%08X set to 0x%08X\n"),Cks_Addr, Cks_Value );
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
                //fprintf(stdout,"Modifying range start from %X to %X\n",Cks_Start,Lowest_Address);
				EB_Printf(TEXT("[dnw] Modifying range start from 0x%X to 0x%X \r\n"),Cks_Start,Lowest_Address );
                Cks_Start = Lowest_Address;
            }
            if (Cks_End > Highest_Address)
            {
                //fprintf(stdout,"Modifying range end from %X to %X\n",Cks_End,Highest_Address);
				EB_Printf(TEXT("[dnw] Modifying range end from 0x%X to 0x%X \r\n"),Cks_End,Highest_Address );
                Cks_End = Highest_Address;
            }



            crc_table = NULL; // 2017.04.13

            switch (Cks_Type)
            {
            case TYPE_CHK8_SUM:
            {
                uint8_t wCKS = 0;

				for (i=Cks_Start; i<=Cks_End; i++)
                {
                    wCKS += Memory_Block[i - Lowest_Address];
                }

                // fprintf(stdout,"8-bit Checksum = %02X\n",wCKS & 0xff);
				EB_Printf(TEXT("[dnw] 8-bit Checksum = 0x%02X \n"), wCKS & 0xff );

                Memory_Block[Cks_Addr - Lowest_Address] = wCKS;

                // fprintf(stdout,"Addr %08X set to %02X\n",Cks_Addr, wCKS);
				EB_Printf(TEXT("[dnw] Addr 0x%08X set to 0x%02X\n"), Cks_Addr, wCKS );
            }
            break;

            case TYPE_CHK16:
            {
                uint16_t wCKS, w;

                wCKS = 0;

                if (Endian == 1)
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

                // fprintf(stdout,"16-bit Checksum = %04X\n",wCKS);
				EB_Printf(TEXT("[dnw] 16-bit Checksum = 0x%04X \n"), wCKS );

                WriteMemBlock16(wCKS);

                // fprintf(stdout,"Addr %08X set to %04X\n",Cks_Addr, wCKS);
				EB_Printf(TEXT("[dnw] Addr 0x%08X set to 0x%04X\n"), Cks_Addr, wCKS );
            }
            break;


            case TYPE_CHK16_8:
            {
                uint16_t wCKS;

                wCKS = 0;

                for (unsigned int i=Cks_Start; i<=Cks_End; i++)
                {
                    wCKS += Memory_Block[i - Lowest_Address];
                }

                // fprintf(stdout,"16-bit Checksum = %04X\n",wCKS);
				EB_Printf(TEXT("[dnw] 16-bit Checksum = %04X \n"), wCKS );
				
                WriteMemBlock16(wCKS);

                //fprintf(stdout,"Addr %08X set to %04X\n",Cks_Addr, wCKS);
				EB_Printf(TEXT("[dnw] Addr %08X set to %04X\n"), Cks_Addr, wCKS );
            }
            break;


            case TYPE_CRC8:
            {
                uint8_t CRC8;

			#if 0 // 2016.03.05
                crc_table = NoFailMalloc(256);
			#else
				if( (crc_table = malloc(256)) == NULL )
				{
					EB_Printf(TEXT("\r\n[dnw] Can't allocate memory. 256: Err(%d) \r\n"), GetLastError() );
			        return 0;
			    }
			#endif
		

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

                // fprintf(stdout,"Addr %08X set to %02X\n",Cks_Addr, CRC8);
				EB_Printf(TEXT("[dnw] Addr 0x%08X set to 0x%02X\n"), Cks_Addr, CRC8 );

            }
            break;

            case TYPE_CRC16:
            {
                uint16_t CRC16;

			#if 0 // 2016.03.05
                crc_table = NoFailMalloc(256 * 2);
			#else
				if( (crc_table = malloc(256*2)) == NULL )
				{
					EB_Printf(TEXT("\r\n[dnw] Can't allocate memory. 256*2: Err(%d) \r\n"), GetLastError() );
					return 0;
				}

			#endif

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

                //fprintf(stdout,"Addr %08X set to %04X\n",Cks_Addr, CRC16);
				EB_Printf(TEXT("[dnw] Addr 0x%08X set to 0x%04X\n"), Cks_Addr, CRC16 );

            }
            break;

            case TYPE_CRC32:
            {
                uint32_t CRC32;

			#if 0 // 2016.03.05
                crc_table = NoFailMalloc(256 * 4);
			#else
				if( (crc_table = malloc(256*4)) == NULL )
				{
					EB_Printf(TEXT("\r\n[dnw] Can't allocate memory. 256*4: Err(%d) \r\n"), GetLastError() );
					return 0;
				}
			#endif	

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

                //fprintf(stdout,"Addr %08X set to %08X\n",Cks_Addr, CRC32);
				EB_Printf(TEXT("[dnw] Addr 0x%08X set to 0x%08X\n"), Cks_Addr, CRC32 );

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
        //fprintf (stderr,"Force/Check address outside of memory range\n");
		EB_Printf(TEXT("[dnw] Force/Check address outside of memory range. \r\n"));
		EB_Printf(TEXT("[dnw]     Low addr[0x%X] Addr[0x%X] High addr[0x%X] Max length[0x%X] \r\n"), Lowest_Address, Cks_Addr, Highest_Address, Max_Length );
		EB_Printf(TEXT("[dnw] --------------------------------------------------------\r\n"));
    }


    /* write binary file */
	if(Filout)
	{
	    fwrite (Memory_Block,
	            Max_Length,
	            1,
	            Filout);
	}

    free (Memory_Block);

    // Minimum_Block_Size is set; the memory buffer is multiple of this?
    if (Minimum_Block_Size_Setted == 1) // true)
    {
        Module = Max_Length % Minimum_Block_Size;
        if (Module)
        {
            Module = Minimum_Block_Size - Module;


			if( Module > MAX_LENGTH_SIZE ) // 2016.03.07
			{
				EB_Printf(TEXT("\r\n[dnw] Module buffer size is %.2f MB. %s \r\n"), (float)Module/(1024.0*1024.0), MAX_LENGTH_TXT );
				EB_Printf(TEXT("[dnw] Hexa File :[%s] \r\n"), szH2BInpFile); 
				return 0; // over !!
			}
			else
			{
				if( (Memory_Block = (unsigned char *) malloc(Module)) == NULL )
				{
					EB_Printf(TEXT("\r\n[dnw] Can't allocate Module memory. Err(%d) \r\n"), GetLastError() );
					return 0;
				}
			}

		#if 0 // 2020.06.25
            memset (Memory_Block,Pad_Byte,Module);

		#else
			if( 1==isPadByteAllArea ) // original version 2.5
				memset (Memory_Block, Pad_Byte,Module);
			else
				memset (Memory_Block,0xff /*Pad_Byte*/,Module);
		#endif


			if(Filout)
			{
	            fwrite (Memory_Block,
	                    Module,
	                    1,
	                    Filout);
			}

            free (Memory_Block);

			//EB_Printf(TEXT("[dnw] 0x%X Pad Length (%d), 0x%x (%d, %d) \r\n"), Pad_Byte, Module, Module, Max_Length, Minimum_Block_Size );


            if (Max_Length_Setted == 1 ) // true)
            {
                //fprintf(stdout,"Attention Max Length changed by Minimum Block Size\n");
				EB_Printf(TEXT("[dnw] Attention Max Length changed by Minimum Block Size. Module:(%d)\r\n"), Module );
            }
        }
    }
	return 1; // OK
}



int Allocate_Memory_And_Rewind(void)
{
unsigned int tmp_Highest_Address = 0x0000; // 2018.07.06


    Records_Start = Lowest_Address;

    if (Starting_Address_Setted == 1) // true)
    {
        Lowest_Address = Starting_Address;
    }
    else
    {
        Starting_Address = Lowest_Address;
    }

    if (Max_Length_Setted == 0) // false)
    {
        Max_Length = Highest_Address - Lowest_Address + 1;
    }
    else
    {
        Highest_Address = Lowest_Address + Max_Length - 1;
		tmp_Highest_Address = Highest_Address; // display only

	#if 1 // 2016.03.05
	
		if( (Real_Length+1 > Max_Length) || (Real_Length > Highest_Address) )
		{
			// Real_Length(0xD34B) Max_Length(0xC800) Highest_Address (0xC7FF)
			if(Verbose_Flag)
				EB_Printf(TEXT("\r\n[dnw] Real addr and highest addr are different. Max(0x%X) Real length(0x%X) High addr(0x%X) \r\n"), Max_Length, Real_Length, Highest_Address );

			BeepSound(8);
			
			if( HEX2BIN_HIGH_ADDR != Enable_HexaAddr_Zero_Forced ) // 2018.07.05
			{
				Highest_Address = Real_Length;
				Max_Length = Real_Length+1;
			}
		}
		else // 2020.06.29
		{
			//EB_Printf(TEXT("\r\n[dnw] +++Size+++ (0x%X) by user!! (0x%X != 0x%X) \r\n\r\n"), Max_Length, Real_Length, Highest_Address );
		}
	#endif
		
    }



#if 0
    fprintf(stdout,"Allocate_Memory_and_Rewind:\n");
	fprintf(stdout,"Lowest address:   %08X\n",Lowest_Address);
	fprintf(stdout,"Highest address:  %08X\n",Highest_Address);
	fprintf(stdout,"Starting address: %08X\n",Starting_Address);
	fprintf(stdout,"Max Length:       %u\n\n",Max_Length);
#else
	//EB_Printf(TEXT("[dnw] Allocate_Memory_and_Rewind:\n"));
	
	EB_Printf(TEXT("\r\n"));
	EB_Printf(TEXT("[dnw] --------------------------------------------------------\r\n"));
	EB_Printf(TEXT("[dnw] Lowest address           : 0x%08X \r\n"), Lowest_Address );

	if( HEX2BIN_ZERO_FORCED==Enable_HexaAddr_Zero_Forced )
		EB_Printf(TEXT("[dnw] Highest addr (ZERO-Force): 0x%08X \r\n"), Highest_Address );
	else
		EB_Printf(TEXT("[dnw] Highest address (Real)   : 0x%08X \r\n"), Highest_Address );

	if( Max_Length_Setted )
		EB_Printf(TEXT("[dnw] Highest addr. (set Max)  : 0x%08X \r\n"), tmp_Highest_Address );

	EB_Printf(TEXT("[dnw] Starting address         : 0x%08X \r\n"), Starting_Address );
	EB_Printf(TEXT("[dnw] Bin File Length          : %u (%.2f KB) \r\n"), (Real_Length+1), (float)(Real_Length+1)/1024.0); // bescause of 0,1..
	EB_Printf(TEXT("[dnw] Bin Max. Length          : %u (%.2f KB) \r\n"), Max_Length, (float)Max_Length/1024.0);
	EB_Printf(TEXT("[dnw] Pad Byte (default:0xFF)  : 0x%02X "), Pad_Byte );

	if( isPadValue==0 )
		EB_Printf(TEXT("\r\n") );
	else
		EB_Printf(TEXT(" --> Pad Byte is wrong!!! Check it. \r\n") );

	if( 1==isPadByteAllArea )
		EB_Printf(TEXT("[dnw] Pad Byte(0x%02X) in empty area is filled. \r\n"), Pad_Byte );

	EB_Printf(TEXT("[dnw] --------------------------------------------------------\r\n"));

#if 1 // 2016.03.10
	// ----------------------------------------
	// Hex Address Zero based converted 
	// ----------------------------------------
	if( HEX2BIN_ZERO_FORCED==Enable_HexaAddr_Zero_Forced )
	{
		if( Phys_AddrTemp )
		{
			EB_Printf(TEXT("[dnw] Hex file real start      : 0x%08X \r\n"), Lowest_Address + Phys_AddrTemp );
			EB_Printf(TEXT("[dnw] Hex real Highest address : 0x%08X -> %.2f kB \r\n"), Highest_Address + Phys_AddrTemp, (float)(Highest_Address + Phys_AddrTemp)/1024.0 );
			EB_Printf(TEXT("[dnw] Hex real Starting addr   : 0x%08X \r\n"), Starting_Address + Phys_AddrTemp);
			EB_Printf(TEXT("[dnw] --------------------------------------------------------\r\n"));
		}
	}
#endif


#endif

    /* Now that we know the buffer size, we can allocate it. */
    /* allocate a buffer */
#if 0 // 2016.03.05
    Memory_Block = (unsigned char *) NoFailMalloc(Max_Length);
#else

	//if( (Cks_End - Cks_Start) > MAX_LENGTH_SIZE /* Max_Length > MAX_LENGTH_SIZE */ ) // 2016.03.07
	if( Max_Length > MAX_LENGTH_SIZE ) // 2016.03.07
	{
		EB_Printf(TEXT("\r\n[dnw] Parsed binary length is %.2f MB. %s \r\n"), (float)Max_Length/(1024.0*1024.0), MAX_LENGTH_TXT );
		EB_Printf(TEXT("[dnw] Hexa File :[%s] \r\n\r\n"), szH2BInpFile); 
		EB_Printf(TEXT("[dnw] Use the Zero forced Address option!!! \r\n")); 
		return 0;
	}
	else
	{
    	//Memory_Block = (unsigned char *) malloc(Max_Length);
		if( (Memory_Block = (unsigned char *) malloc(Max_Length)) == NULL )
		{
			EB_Printf(TEXT("\r\n[dnw] Can't allocate Max_Length memory. Err(%d) \r\n"), GetLastError() );
			return 0;
		}
	}
	
#endif

    /* For EPROM or FLASH memory types, fill unused bytes with FF or the value specified by the p option */

#if 0 // 2020.06.25
    memset (Memory_Block, 0xff /* Pad_Byte */,Max_Length);
#else
	if( 1==isPadByteAllArea ) // original version 2.5
	{
		//EB_Printf(TEXT("\r\n[dnw] Max_Length1  [0x%08x] \r\n"), Max_Length );

		memset (Memory_Block, Pad_Byte, Max_Length);
	}
	else
	{	//[dnw] Max_Length1  [0x00004df0] [0x08004def] [0xf8000000] 
		//EB_Printf(TEXT("\r\n[dnw] Max_Length1  [0x%08x] [0x%08x] [0x%08x] \r\n"), Real_Length, Max_Length, (Max_Length-Real_Length-1) );

		memset (Memory_Block,0xff /*Pad_Byte*/,Max_Length);
		if( Max_Length-1 > Real_Length )
		{
			memset (&Memory_Block[Real_Length+1], Pad_Byte, (Max_Length-Real_Length-1) );
		}
	}
#endif

    rewind(Filin);

	return 1; // OK
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
		if (result != 1) 
		{
			//fprintf(stderr,"ReadDataBytes: error in line %d of hex file\n", Record_Nb);
			EB_Printf(TEXT("[dnw] ReadDataBytes: error in line %d of hex file. \r\n"), Record_Nb );
		}

		p += 2;

		/* Check that the physical address stays in the buffer's range. */
		if (Phys_Addr < Max_Length)
		{
			/* Overlapping record will erase the pad bytes */
			if (Swap_Wordwise)
			{
				if (Memory_Block[Phys_Addr ^ 1] != Pad_Byte) 
				{
					//fprintf(stderr,"Overlapped record detected\n");
					EB_Printf(TEXT("[dnw] Overlapped record detected. Phys_Addr:0x%X, Max_Length:0x%X \r\n"), Phys_Addr, Max_Length );
				}
				Memory_Block[Phys_Addr++ ^ 1] = temp2;
			}
			else
			{
				if (Memory_Block[Phys_Addr] != Pad_Byte) 
				{
					//fprintf(stderr,"Overlapped record detected\n");
					//~ EB_Printf(TEXT("[dnw] Overlapped record detected.. Phys_Addr:0x%X, Max_Length:0x%X \r\n"), Phys_Addr, Max_Length );
				}
				Memory_Block[Phys_Addr++] = temp2;
			}

			Checksum = (Checksum + temp2) & 0xFF;
		}
	}
	while (--i != 0);

	return p;
}


#ifdef __NOT_USED__

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
            case 'a':
                Address_Alignment_Word = true;
                i = 0;
                break;
			case 'b':
				Batch_Mode = true;
				i = 0;
				break;
            case 'c':
                Enable_Checksum_Error = true;
                i = 0;
                break;
            case 'd':
                DisplayCheckMethods();
            case 'e':
                GetExtension(argv[Param + 1],Extension);
                i = 1; /* add 1 to Param */
                break;
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
            case 'k':
                Cks_Type = GetHex(argv[Param + 1]);
                {
                    if (Cks_Type > LAST_CHECK_METHOD) usage();
                }
                i = 1; /* add 1 to Param */
                break;
            case 'l':
                Max_Length = GetHex(argv[Param + 1]);
				if (Max_Length > 0x800000)
				{
					fprintf(stderr,"Max_Length = %u\n", Max_Length);
					exit(1);
				}
                Max_Length_Setted = true;
                i = 1; /* add 1 to Param */
                break;
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
            case 's':
                Starting_Address = GetHex(argv[Param + 1]);
                Starting_Address_Setted = true;
                i = 1; /* add 1 to Param */
                break;
            case 'v':
                Verbose_Flag = true;
                i = 0;
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

            case '?':
            case 'h':
            default:
                usage();
            } /* switch */

            /* Last parameter is not a filename */
            if (Param == argc-1) usage();

            // fprintf(stderr,"Param: %d, option: %c\n",Param,c);

            /* if (Param + i) < (argc -1) */
            if (Param < argc -1 -i) Param += i;
            else usage();

        }
        else
            break;
        /* if option */
    } /* for Param */
}

/* ------------------------------------------------------------------------
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 2 -C 07  0 f f 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 2 -C 07  0 f f 55 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 2 -C 07 FF t t 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 2 -C 39  0 t t 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 2 -C 1D FD f f 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 2 -C 1D FF f f FF testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 2 -C 31  0 t t 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 2 -C 9B  0 t t 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 2 -C 8D  0 f f 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 2 -C D5  0 f f 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 3 -C 8005 0 t t 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 3 -C 8005 0 f f 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 3 -C 8005 800D f f 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 3 -C 8005 0 t t FFFF testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 3 -C 8005 FFFF t t FFFF testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 3 -C 8005 FFFF t t 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 3 -C 1021 1D0F f f 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 3 -C 1021 FFFF f f 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 3 -C 1021 FFFF f f FFFF testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 3 -C 1021 0 f f 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 3 -C 1021 FFFF t t 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 3 -C 1021 B2AA t t 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 3 -C 1021 0 t t 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 3 -C 1021 FFFF t t FFFF testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 3 -C 0589 0 f f 1 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 3 -C 0589 0 f f 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 3 -C 3D65 0 t t FFFF testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 3 -C 3D65 0 f f FFFF testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 3 -C 8BB7 0 f f 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 3 -C A097 0 f f 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 4 -C 04C11DB7 FFFFFFFF t t FFFFFFFF testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 4 -C 04C11DB7 FFFFFFFF f f FFFFFFFF testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 4 -C 04C11DB7 FFFFFFFF f f 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 4 -C 04C11DB7 00000000 f f FFFFFFFF testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 4 -C 04C11DB7 FFFFFFFF t t 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 4 -C 1EDC6F41 FFFFFFFF t t FFFFFFFF testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 4 -C A833982B FFFFFFFF t t FFFFFFFF testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 4 -C 741B8CD7 0 f f 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 4 -C 814141AB 0 f f 0 testcrc.hex
hex2bin -c -f 10A -r 100 108 -l 20 -E 0 -k 4 -C 000000AF 0 f f 0 testcrc.hex
--------------------------------------------------------------------------------------*/

#endif






///////////////////////////////////////////////////////////////////////////////////
/*
  hex2bin converts an Intel hex file to binary.

  Copyright (C) 2015,  Jacques Pelletier
  checksum extensions Copyright (C) 2004 Rockwell Automation
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:
  Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
  Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  20040617 Alf Lacis: Added pad byte (may not always want FF).
  Added 'break;' to remove GNU compiler warning about label at
  end of compound statement
  Added PROGRAM & VERSION strings.

  20071005 PG: Improvements on options parsing
  20091212 JP: Corrected crash on 0 byte length data records
  20100402 JP: Corrected bug on physical address calculation for extended
  linear address record.
  ADDRESS_MASK is now calculated from MEMORY_SIZE

  20120125 Danny Schneider:
  Added code for filling a binary file to a given Max_Length relative to
  Starting Address if Max-Address is larger than Highest-Address
  20120509 Yoshimasa Nakane:
  modified error checking (also for output file, JP)
  20141005 JP: added support for byte swapped hex files
           corrected bug caused by extra LF at end or within file
  20141008 JP: removed junk code
  20141121 Slucx: added line for removing extra CR when entering file name at run time.
  20141122 Simone Fratini: small feature added
  20150116 Richard Genoud (Paratronic): correct buffer overflows/wrong results with the -l flag
  20150122 JP: added support for different check methods
  20150221 JP: rewrite of the checksum write/force value
  20150725 Simone Fratini: added option for word sized hex files
  20150804 JP: added batch file option
*/

/*--------------------------------------------------
#define PROGRAM "hex2bin"
#define VERSION "2.2"

#include "common.h"

#define NO_ADDRESS_TYPE_SELECTED 0
#define LINEAR_ADDRESS 1
#define SEGMENTED_ADDRESS 2
------------------------------------------------------ */

// ---------------------------------------------
// Intel checksum method
// CalcChkSum = ( ~CalcChkSum + 1 ) % 256;
// ---------------------------------------------

// ----------------------------------------------------------
// Convert Intel Hex to Binary
// ----------------------------------------------------------
//int ConvertIntelHex2Bin(TCHAR *inpFilename, TCHAR *outFilename)
int ConvertIntelHex2Bin(void *args)
{
#define INTEL_BASE_ADDR_CMP 		0x0010 

	unsigned int iErrCount = 0; // added at 2016.03.05

    /* line inputted from file */
    char Line[MAX_LINE_SIZE];

    /* flag that a file was read */
    int Fileread;

    /* cmd-line parameter # */
    char *p = NULL;

    int result;

    /* Application specific */
    unsigned int First_Word, Address, Segment, Upper_Address;
    unsigned int Type;
    unsigned int Offset = 0x00;
    unsigned int temp;

    /* We will assume that when one type of addressing is selected, it will be valid for all the
     current file. Records for the other type will be ignored. */
    unsigned int Seg_Lin_Select = NO_ADDRESS_TYPE_SELECTED;

    unsigned int temp2; //, Record_Checksum;

    unsigned char  Data_Str[MAX_LINE_SIZE];


	iCovertedCompleted = 0; // added at 2016.03.07 started
	dwHex2BinExit = 0;
	Phys_AddrTemp = 0; // added at 2016.03.10

	iErrCount = 0; // added at 2016.03.07, Error count clear


#if 0 // 2016.03.05
    fprintf (stdout,PROGRAM" "VERSION", Copyright (C) 2017 Jacques Pelletier & contributors\n\n");

    if (argc == 1)
        usage();

    strcpy(Extension, "bin"); /* default is for binary file extension */

	ParseOptions(argc, argv);

    /* when user enters input file name */

    /* Assume last parameter is filename */
    strcpy(Filename,argv[argc -1]);

    /* Just a normal file name */
    NoFailOpenInputFile (inpFilename);

#if 0
    PutExtension(Filename, Extension);
#endif

    NoFailOpenOutputFile(outFilename);
#else


	Filin = fopen( (const char*)szH2BFullInputFile, "r");
	if( NULL == Filin )
	{
		EB_Printf(TEXT("[dnw] can not open IntelHex2Bin input [%s]. Err(%u)\n"), szH2BFullInputFile, GetLastError() );
		fclose(Filin);

		dwHex2BinExit = 0;
		iCovertedCompleted = 1; // completed!
		Status_Checksum_Error = 0; // no error

		// added at 2016.03.11, button Enabled!!
		UserButtonEnable(_hDlgHex2Binary, 1);

		UserButtonPadByteOnOff(_hDlgHex2Binary); // 2018.07.06

		/* _endthread given to terminate */
		_endthread();

		return 1;
	}

	Filout = fopen( (const char*)szH2BFullOutputFile, "wb");
	if( NULL == Filout )
	{
		EB_Printf(TEXT("[dnw] can not open IntelHex2Bin output [%s]. Err(%u)\n"), szH2BFullOutputFile, GetLastError() );
		fclose(Filin);
		fclose(Filout);

		dwHex2BinExit = 0;
		iCovertedCompleted = 1; // completed!
		Status_Checksum_Error = 0; // no error

		// added at 2016.03.11, button Enabled!!
		UserButtonEnable(_hDlgHex2Binary, 1);

		UserButtonPadByteOnOff(_hDlgHex2Binary); // 2018.07.06

		/* _endthread given to terminate */
		_endthread();
		return 1;
	}
#endif


	//EB_Printf(TEXT("[dnw] inpFilename:[%s] outFilename:[%s] \r\n"), inpFilename, outFilename );


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
    Record_Nb = 0;    // Used for reporting errors
    First_Word = 0;


    /* get highest and lowest addresses so that we can allocate the right size */
    do
    {
        unsigned int i;

		if(dwHex2BinExit) goto ForceEnded; // added  at 2016.03.05

		memset( Line, 0x00, sizeof(Line) ); // added at 2016.03.09_initial

        /* Read a line from input file. */
        GetLine(Line,Filin);
        Record_Nb++;

        /* Remove carriage return/line feed at the end of line. */
        i = strlen(Line);

	#if 0 // 2016.03.09_initial
        if (--i != 0)
	#else
		if (--i > 0)
	#endif
        {
            if (Line[i] == '\n') Line[i] = '\0';
            if (Line[i] == '\r') Line[i] = '\0'; // added by 2016.03.10

            /* Scan the first two bytes and nb of bytes.
               The two bytes are read in First_Word since its use depend on the
               record type: if it's an extended address record or a data record.
               */

			/* -----------------------------------------------------------------
			// Length(1), Offset(2), Type(1), data(0~255), checksum(1)
			:10 0800 00 71EF04F0FF00FF004582FACF0DF0FBCF 3F
			:10 0810 00 0EF0E9CF0FF0EACF10F0E1CF11F0E2CF 08
			:10 0820 00 12F0D9CF13F0DACF14F0F3CF15F0F4CF E4

			:10 1130 00 1CF0000E00018A6F000E896F21D0100E 86
			:10 1140 00 000189251B6E000E00018A211C6E040E 11

			:10 7FE0 00 0A00286E756C6C29006A776B616E6700F9

			:02 0000 04 0030 CA -> Extended Linear Address Record (32bit address format)
			:02 0000 05        -> Extended Linear Address Record (32bit address format)
			
            :02 0000 02        -> Extended Linear Address Record (20bit address format)
            :02 0000 03        -> Extended Linear Address Record (20bit address format)

			:0E 0000 00 FF730332FFA480FF0F800FA00F409C
			:00 0000 01 FF -> end-of-file recoed
			 ----------------------------------------------------------------- */

			memset( Data_Str, 0x00, sizeof(Data_Str) ); // added at 2016.03.10

            result = sscanf (Line, ":%2x%4x%2x%s",&Nb_Bytes,&First_Word,&Type,Data_Str);
			if (result != 4 && (result!=-1 && i>0) ) 
			{
				// fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
				EB_Printf(TEXT("[dnw] 0-Error in line %d of hex file. (%d) Length:%d \r\n"), Record_Nb, result, i );
				EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );

				iErrCount ++; // 2016.03.05
			}
			
            p = (char *) Data_Str;

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

			#if 1 // 2020.06.29
					// ----------------------------------------
					// Hex Address Zero based converted 
					// ----------------------------------------
					if( HEX2BIN_ZERO_FORCED==Enable_HexaAddr_Zero_Forced )
					{
						if( Upper_Address>INTEL_BASE_ADDR_CMP )
						{
							Phys_AddrTemp = Upper_Address;
							Upper_Address = 0x0000;
							
							if(Verbose_Flag)
							EB_Printf(TEXT("[dnw] :%d: Upper_Addr: 0x%X to zero-based Addr: 0x%X \r\n"), Type, Phys_AddrTemp, Upper_Address );
						}
					}
			#endif

                    /* LINEAR_ADDRESS or NO_ADDRESS_TYPE_SELECTED
                       Upper_Address = 0 as specified in the Intel spec. until an extended address
                       record is read. */
                    Phys_Addr = ((Upper_Address << 16) + Address);
                }

                if (Verbose_Flag) 
                {
					// fprintf(stderr,"Physical Address: %08X\n",Phys_Addr);
					EB_Printf(TEXT("[dnw] :%d: Phys_Addr: 0x%08X, Lowest_Addr: 0x%08X, Upper_Addr:0x%04X, Segment:0x%04X \r\n"), Type, Phys_Addr, Lowest_Address, Upper_Address, Segment );
                }

                /* Set the lowest address as base pointer. */
                if (Phys_Addr < Lowest_Address)
            	{
                    Lowest_Address = Phys_Addr;
					//EB_Printf(TEXT("[dnw] ++++++++++++++ 0x%08X +++++++=== 0x%08X ==+++ \r\n"), Lowest_Address , Phys_Addr);
            	}
				
                /* Same for the top address. */
                temp = Phys_Addr + Nb_Bytes -1;

                if (temp > Highest_Address)
                {
                    Highest_Address = temp;
					Real_Length = Highest_Address; // added at 2016.03.05, for real file size

                    if (Verbose_Flag) 
                    {
						//fprintf(stderr,"Highest_Address: %08X\n",Highest_Address);
						EB_Printf(TEXT("[dnw] :%d: Lowest_Address: 0x%08X, Highest Address: 0x%08X\n"), Type, Lowest_Address, Highest_Address );
                    }
                }
                break;

            case 1:
                if (Verbose_Flag) 
               	{
					//fprintf(stderr,"End of File record\n");
					EB_Printf(TEXT("[dnw] :%d: End of File record. \r\n"), Type );
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
                    result = sscanf (p, "%4x%2x",&Segment,&temp2);
					if (result != 2) 
					{
						//fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
						EB_Printf(TEXT("[dnw] 2-Error in line %d of hex file. (%d) \r\n"), Record_Nb, result );
						EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );

						iErrCount ++; // 2016.03.05
					}
                    if (Verbose_Flag) 
                    {
						//fprintf(stderr,"Extended Segment Address record: %04X\n",Segment);
						EB_Printf(TEXT("[dnw] :%d: Extended Segment Address record: Phys_Addr: 0x%08X, Upper_Addr:0x%04X, Segment:0x%04X \r\n"), Type, Phys_Addr, Upper_Address, Segment );
                    }
                    /* Update the current address. */
                    Phys_Addr = (Segment << 4);
                }
                else
                {
                    //fprintf(stderr,"Ignored extended linear address record %d\n", Record_Nb);
					EB_Printf(TEXT("[dnw] :%d: Ignored extended linear address record %d \r\n"), Type, Record_Nb );
                }
                break;

            case 3: // 20bit address
                if (Verbose_Flag) 
                {
					//fprintf(stderr,"Start Segment Address record: ignored\n");
					EB_Printf(TEXT("[dnw] :%d: Start Segment Address record: ignored\n"), Type );
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
                    result = sscanf (p, "%4x%2x",&Upper_Address,&temp2);
					if (result != 2) 
					{
						//fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
						EB_Printf(TEXT("[dnw] 4-Error in line %d of hex file. (%d) \r\n"), Record_Nb, result );
						EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );

						iErrCount ++; // 2016.03.05
					}

                    if (Verbose_Flag) 
                    {
						//fprintf(stderr,"Extended Linear Address record: %04X\n",Upper_Address);
						EB_Printf(TEXT("[dnw] :%d: Extended Linear Address record: Upper_Addr:0x%04X, Segment:0x%04X \r\n"), Type, Upper_Address, Segment );
                    }

                    /* Update the current address. */
                    Phys_Addr = (Upper_Address << 16);

				#if 1 // 2020.06.29
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
								
								if(Verbose_Flag)
								EB_Printf(TEXT("[dnw] :%d: Upper_Address: 0x%X to zero-based addr: 0x%X \r\n"), Type, Phys_AddrTemp, Upper_Address );
							}
						}

						if( Phys_AddrTemp && (0==iOnceCheck) )
						{
							Phys_Addr -= Phys_AddrTemp;
						}

					}
				#endif


                    if (Verbose_Flag) 
                    {
						//fprintf(stderr,"Physical Address: %08X\n",Phys_Addr);
						EB_Printf(TEXT("[dnw] :%d: Physical Address: 0x%08X \r\n"), Type, Phys_Addr );
                    }
                }
                else
                {
                    //fprintf(stderr,"Ignored extended segment address record %d\n", Record_Nb);
					EB_Printf(TEXT("[dnw] :%d: Ignored extended segment address record %d \r\n"), Type, Record_Nb );
                }
                break;

            case 5: // 32bit address
                if (Verbose_Flag) 
                {
					//fprintf(stderr,"Start Linear Address record: ignored\n");
					EB_Printf(TEXT("[dnw] :%d: Start Linear Address record: ignored. \r\n"), Type );
                }
                break;

            default:
                if (Verbose_Flag) 
                {
					//fprintf(stderr,"Unknown record type: %d at %d\n",Type,Record_Nb);
					EB_Printf(TEXT("[dnw] :%d: Unknown record Type:%d at %d\n"),Type, Type, Record_Nb );
					EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
                }
                break;
            }

        }
    }
    while (!feof (Filin));


	// ---------------------------------
	if( iErrCount > MAX_ERR_COUNT ) // 2016.03.05
	{

#ifdef USE_FILE_BUFFERS
		free (FilinBuf);
		free (FiloutBuf);
#endif
	
		fclose (Filin);
		fclose (Filout);

		iErrCount = 0; // clear

		EB_Printf(TEXT("\r\n[dnw] Too many error.  Check hex type (Intel or Motorola!!) \r\n"));


// added at 2016.03.11, button Enabled!!
		UserButtonEnable(_hDlgHex2Binary, 1);

		UserButtonPadByteOnOff(_hDlgHex2Binary); // 2018.07.06

		BeepSound(4);

		dwHex2BinExit = 0;
		iCovertedCompleted = 1; // completed!
		Status_Checksum_Error = 0; // no error

		/* _endthread given to terminate */
		_endthread();

		return 1;
	}
	// ---------------------------------


	

    if (Address_Alignment_Word)
        Highest_Address += (Highest_Address - Lowest_Address) + 1;


#if 0 // 2016.03.07
    Allocate_Memory_And_Rewind();
#else
    if( 0==Allocate_Memory_And_Rewind() )
		goto ForceEnded;
#endif

    Segment = 0;
    Upper_Address = 0;
    Record_Nb = 0;

    /* Read the file & process the lines. */
    do /* repeat until EOF(Filin) */
    {
        unsigned int i;

        Checksum = 0; // added at 2016.03.09

		if(dwHex2BinExit) goto ForceEnded; // added at 2016.03.05

		memset( Line, 0x00, sizeof(Line) ); // added at 2016.03.09_initial

        /* Read a line from input file. */
        GetLine(Line,Filin);
        Record_Nb++;

        /* Remove carriage return/line feed at the end of line. */
        i = strlen(Line);

        //fprintf(stderr,"Record: %d; length: %d\n", Record_Nb, i);

	#if 0 // 2016.03.09_initial
        if (--i != 0)
	#else
		if (--i > 0)
	#endif
        {
            if (Line[i] == '\n') Line[i] = '\0';
            if (Line[i] == '\r') Line[i] = '\0'; // added by 2016.03.10

            /* Scan the first two bytes and nb of bytes.
               The two bytes are read in First_Word since its use depend on the
               record type: if it's an extended address record or a data record.
            */

			memset( Data_Str, 0x00, sizeof(Data_Str) ); // added at 2016.03.09

            result = sscanf (Line, ":%2x%4x%2x%s",&Nb_Bytes,&First_Word,&Type,Data_Str);
            if (result != 4 && (result!=-1 && i>0) ) 
            {
				//fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
				EB_Printf(TEXT("[dnw] :%d: 0-Error in line %d of selected hex file. (%d) (%d) \r\n"), Type, Record_Nb, result, i );
				EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
            }
			
            Checksum = Nb_Bytes + (First_Word >> 8) + (First_Word & 0xFF) + Type;

            p = (char *) Data_Str;

            /* If we're reading the last record, ignore it. */
            switch (Type)
            {
            /* Data record */
            case 0:
                if (Nb_Bytes == 0)
                {
                    //fprintf(stderr,"0 byte length Data record ignored\n");
					EB_Printf(TEXT("[dnw] :%d: 0 byte length Data record ignored. \n"), Type );
					EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
                    break;
                }

                Address = First_Word;

                if (Seg_Lin_Select == SEGMENTED_ADDRESS)
                    Phys_Addr = (Segment << 4) + Address;
                else
            	{

			#if 1 // 2020.06.29
					// ----------------------------------------
					// Hex Address Zero based converted 
					// ----------------------------------------
					if( HEX2BIN_ZERO_FORCED==Enable_HexaAddr_Zero_Forced )
					{
						if( Upper_Address>INTEL_BASE_ADDR_CMP )
						{
							Phys_AddrTemp = Upper_Address;
							Upper_Address = 0x0000;
							
							if(Verbose_Flag)
							EB_Printf(TEXT("[dnw] :%d: Upper_Addr:: 0x%X to zero-based Addr:: 0x%X \r\n"), Type, Phys_AddrTemp, Upper_Address );
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

					p = ReadDataBytes(p);

                    /* Read the Checksum value. */
                    result = sscanf (p, "%2x",&temp2);
                    if (result != 1) 
                    {
						//fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
						EB_Printf(TEXT("[dnw] :%d: Error in line %d of selected hex file. (%d) \r\n"), Type, Record_Nb, result );
						EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
                    }
					/* Verify Checksum value. */
                    Checksum = (Checksum + temp2) & 0xFF;
                    VerifyChecksumValue( ~(Checksum-temp2-1), temp2 ); // modified at 2016.03.07
                }
                else
                {
                    if (Seg_Lin_Select == SEGMENTED_ADDRESS)
                    {
                        //fprintf(stderr,"Data record skipped at %4X:%4X\n",Segment,Address);
                        if( Verbose_Flag )
						EB_Printf(TEXT("[dnw] Line#%6d :%d: Data record skipped at 0x%04X:0x%04X \r\n"), Record_Nb, Type, Segment,Address );
                    }
                    else
                    {
                        //fprintf(stderr,"Data record skipped at %8X\n",Phys_Addr);
                        if( Verbose_Flag )
						EB_Printf(TEXT("[dnw] Line#%6d :%d: Data record skipped at 0x%08X (High addr:0x%08X) \r\n"), Record_Nb, Type, Phys_Addr, Highest_Address );
                    }
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
                    result = sscanf (p, "%4x%2x",&Segment,&temp2);
                    if (result != 2) 
                    {
						//fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
						EB_Printf(TEXT("[dnw] :%d: Error in line %d of selected hex file. (%d) \r\n"), Type, Record_Nb, result );
						EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
                    }

                    /* Update the current address. */
                    Phys_Addr = (Segment << 4);

                    /* Verify Checksum value. */
                    Checksum = (Checksum + (Segment >> 8) + (Segment & 0xFF) + temp2) & 0xFF;
					VerifyChecksumValue( ~(Checksum-temp2-1), temp2 ); // modified at 2016.03.07
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
                    sscanf (p, "%4x",&Offset);
                    Offset = Offset << 16;
                    Offset -= Lowest_Address;

                }
                /* First extended linear address record ? */
                if (Seg_Lin_Select == NO_ADDRESS_TYPE_SELECTED)
                    Seg_Lin_Select = LINEAR_ADDRESS;

                /* Then ignore subsequent extended segment address records */
                if (Seg_Lin_Select == LINEAR_ADDRESS)
                {
                    result = sscanf (p, "%4x%2x",&Upper_Address,&temp2);
                    if (result != 2) 
                    {
						//fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
						EB_Printf(TEXT("[dnw] :%d: Error in line %d of selected hex file. (%d) \r\n"), Type, Record_Nb, result );
						EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
                    }

				#if 1 // 2020.06.29
					// ----------------------------------------
					// Hex Address Zero based converted 
					// ----------------------------------------
					if( HEX2BIN_ZERO_FORCED==Enable_HexaAddr_Zero_Forced )
					{
						if( Upper_Address>INTEL_BASE_ADDR_CMP )
						{
							Phys_AddrTemp = Upper_Address;
							Upper_Address = 0x0000;
							
							if(Verbose_Flag)
							EB_Printf(TEXT("[dnw] :%d: Upper_Addr:*: 0x%X to zero-based Addr:*: 0x%X \r\n"), Type, Phys_AddrTemp, Upper_Address );
						}
					}
				#endif


                    /* Update the current address. */
                    Phys_Addr = (Upper_Address << 16);

                    /* Verify Checksum value. */
                    Checksum = (Checksum + (Upper_Address >> 8) + (Upper_Address & 0xFF) + temp2) & 0xFF;

				#if 1 // 2020.06.29
					// ----------------------------------------
					// Hex Address Zero based converted 
					// ----------------------------------------
					if( HEX2BIN_ZERO_FORCED==Enable_HexaAddr_Zero_Forced )
					{
						Checksum = (Checksum + (Phys_AddrTemp >> 8) + (Phys_AddrTemp & 0xFF) ) & 0xFF;							
					}
				#endif

					VerifyChecksumValue( ~(Checksum-temp2-1), temp2 ); // modified at 2016.03.07
                }
                break;

            /* Start linear address record */
            case 5:
                /* Nothing to be done since it's for specifying the starting address for
                   execution of the binary code */
                break;
            default:
                //fprintf(stderr,"Unknown record type\n");
				EB_Printf(TEXT("[dnw] Line#%6d :%d: *Unknown record type:%d\n"), Record_Nb, Type, Type);
				EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
                break;
            }

        }
    }
    while (!feof (Filin));
    /*-----------------------------------------------------------------------------*/

#if 0 // 2016.03.06
    fprintf(stdout,"Binary file start = %08X\n",Lowest_Address);
    fprintf(stdout,"Records start     = %08X\n",Records_Start);
    fprintf(stdout,"Highest address   = %08X\n",Highest_Address);
    fprintf(stdout,"Pad Byte          = %X\n",  Pad_Byte);
#else
	if( Verbose_Flag )
	{
		EB_Printf(TEXT("[dnw] --------------------------------------------------------\r\n"));
		EB_Printf(TEXT("[dnw] Binary file start        = 0x%08X\n"), Lowest_Address);
		EB_Printf(TEXT("[dnw] Records start            = 0x%08X\n"), Records_Start);
		EB_Printf(TEXT("[dnw] Highest address          = 0x%08X\n"), Highest_Address);
		EB_Printf(TEXT("[dnw] Pad Byte                 = 0x%X\n"), Pad_Byte);
		EB_Printf(TEXT("[dnw] --------------------------------------------------------\r\n"));

	#if 1 // 2016.03.10
		// ----------------------------------------
		// Hex Address Zero based converted 
		// ----------------------------------------
		if( HEX2BIN_ZERO_FORCED==Enable_HexaAddr_Zero_Forced )
		{
			if( Phys_AddrTemp )
			{
				EB_Printf(TEXT("[dnw] --------------------------------------------------------\r\n"));
				EB_Printf(TEXT("[dnw] Hex file real start      = 0x%08X \n"),Lowest_Address + Phys_AddrTemp );
				EB_Printf(TEXT("[dnw] Hex real Highest address = 0x%08X -> %.2f kB \n"),Highest_Address + Phys_AddrTemp, (float)(Highest_Address + Phys_AddrTemp)/1024.0);
				EB_Printf(TEXT("[dnw] --------------------------------------------------------\r\n"));
			}
		}
	#endif


	}
#endif


	WriteMemory();

#ifdef USE_FILE_BUFFERS
    free (FilinBuf);
    free (FiloutBuf);
#endif

    fclose (Filin);
    fclose (Filout);



    if (Status_Checksum_Error && Enable_Checksum_Error)
    {
        //fprintf(stderr,"Checksum error detected.\n");
		EB_Printf(TEXT("[dnw] Intel Checksum error detected. \r\n"));

		dwHex2BinExit = 0;
		iCovertedCompleted = 1; // completed!
		Status_Checksum_Error = 0; // no error

		// added at 2016.03.11, button Enabled!!
		UserButtonEnable(_hDlgHex2Binary, 1);

		UserButtonPadByteOnOff(_hDlgHex2Binary); // 2018.07.06

		BeepSound(4);

		/* _endthread given to terminate */
		_endthread();
		return 1;
    }


#if 0
    if (!Fileread)
        usage();
#endif


	EB_Printf(TEXT("[dnw] --------------------------------------------------------\r\n"));
	EB_Printf(TEXT("[dnw] Input File :[%s] \n"), szH2BInpFile); 
	EB_Printf(TEXT("[dnw] Output File:[%s] \n"), szH2BOutFile); // szH2BFullOutputFile );
	if( HEX2BIN_ZERO_FORCED==Enable_HexaAddr_Zero_Forced )
		EB_Printf(TEXT("[dnw] Intel Hex address Zero-forced!!! Phys_Base_Addr:0x%08X \r\n"), Phys_AddrTemp );


	if( Address_Alignment_Word )
		EB_Printf(TEXT("[dnw] Intel Hex Address Word-alignment!!! \r\n"));
	if( Swap_Wordwise )
		EB_Printf(TEXT("[dnw] Intel Hex Swap Word-wise!!! \r\n"));
	EB_Printf(TEXT("[dnw] Intel Hex is converted to binary. OK! (Total parsed line#:%d) \r\n"), Record_Nb );

	BeepSound(3); // OK


ForceEnded:

	if(Filin)  { fclose (Filin);  Filin=NULL;  }
	if(Filout) { fclose (Filout); Filout=NULL;  }


	// added at 2016.03.11, button Enabled!!
	UserButtonEnable(_hDlgHex2Binary, 1);

	UserButtonPadByteOnOff(_hDlgHex2Binary); // 2018.07.06
	

	if( dwHex2BinExit )
	{
		BeepSound(4);
		EB_Printf(TEXT("\r\n[dnw] Intel Hex>> User forced ended! \r\n"));
	}
	dwHex2BinExit = 0;

	iCovertedCompleted = 1; // completed!
	
	Status_Checksum_Error = 0; // no error

	/* _endthread given to terminate */
	_endthread();
	return 0;
}




//////////////////////////////////////////////////////////////////////////////////
/*-------------------------------------------------------------------------------
mot2bin converts a Motorola hex file to binary.

Copyright (C) 2015,  Jacques Pelletier
checksum extensions Copyright (C) 2004 Rockwell Automation
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

20040617 Alf Lacis: Added pad byte (may not always want FF).
         Added initialisation to Checksum to remove GNU
         compiler warning about possible uninitialised usage
         Added 2x'break;' to remove GNU compiler warning about label at
         end of compound statement
         Added PROGRAM & VERSION strings.

20071005 PG: Improvements on options parsing
20091212 JP: Corrected crash on 0 byte length data records
20100402 JP: ADDRESS_MASK is now calculated from MEMORY_SIZE

20120125 Danny Schneider:
         Added code for filling a binary file to a given Max_Length relative to
         Starting Address if Max-Address is larger than Highest-Address
20120509 Yoshimasa Nakane:
         modified error checking (also for output file, JP)
20141005 JP: added support for byte swapped hex files
         corrected bug caused by extra LF at end or within file
20141121 Slucx: added line for removing extra CR when entering file name at run time.
20150116 Richard Genoud (Paratronic): correct buffer overflows/wrong results with the -l flag
20150122 JP: added support for different check methods
20150221 JP: rewrite of the checksum write/force value
20150804 JP: added batch file option
*/

/* ---------------------------------
#define PROGRAM "mot2bin"
#define VERSION "2.1"

#include "common.h"
------------------------------------ */




// ----------------------------------------------------------
// Convert Motorola S-record Hex to Binary
// ----------------------------------------------------------
//int ConvertMotorolaSRecordHex2Bin(TCHAR *inpFilename, TCHAR *outFilename)
int ConvertMotorolaSRecordHex2Bin(void *args)
{
	unsigned int iErrCount = 0; // added at 2016.03.05

    /* line inputted from file */
    char Line[MAX_LINE_SIZE];

    /* flag that a file was read */
    int Fileread;

    /* cmd-line parameter # */
    char *p = NULL;

    int result;

    /* Application specific */
    unsigned int First_Word, Address;
    unsigned int Type;
    unsigned int Exec_Address;
    unsigned int temp;
    unsigned int Record_Count, Record_Checksum;

    unsigned char  Data_Str[MAX_LINE_SIZE];


	iCovertedCompleted = 0; // added at 2016.03.07 started
	dwHex2BinExit = 0;
	Phys_AddrTemp = 0; // added at 2016.03.10

	iErrCount = 0; // clear


#if 0 // 2016.03.05
    fprintf (stdout,PROGRAM" v"VERSION", Copyright (C) 2015 Jacques Pelletier & contributors\n\n");

    if (argc == 1)
        usage();

    strcpy(Extension, "bin"); /* default is for binary file extension */

	ParseOptions(argc, argv);

    /* when user enters input file name */

    /* Assume last parameter is filename */
    strcpy(Filename,argv[argc -1]);

    /* Just a normal file name */
    NoFailOpenInputFile (Filename);
    PutExtension(Filename, Extension);
    NoFailOpenOutputFile(Filename);
#else

	
	Filin = fopen( (const char*)szH2BFullInputFile, "r");
	if( NULL == Filin )
	{
		EB_Printf(TEXT("[dnw] can not open Motorola2Bin input [%s]. Err(%u)\n"), szH2BFullInputFile, GetLastError() );
		fclose(Filin);

		dwHex2BinExit = 0;
		iCovertedCompleted = 1; // completed!
		Status_Checksum_Error = 0; // no error

		// added at 2016.03.11, button Enabled!!
		UserButtonEnable(_hDlgHex2Binary, 1);

		UserButtonPadByteOnOff(_hDlgHex2Binary); // 2018.07.06

		/* _endthread given to terminate */
		_endthread();

		return 1;
	}

	Filout = fopen( (const char*)szH2BFullOutputFile, "wb");
	if( NULL == Filout )
	{
		EB_Printf(TEXT("[dnw] can not open Motorola2bin output [%s]. Err(%u)\n"), szH2BFullOutputFile, GetLastError() );
		fclose(Filin);
		fclose(Filout);

		dwHex2BinExit = 0;
		iCovertedCompleted = 1; // completed!
		Status_Checksum_Error = 0; // no error

		// added at 2016.03.11, button Enabled!!
		UserButtonEnable(_hDlgHex2Binary, 1);

		UserButtonPadByteOnOff(_hDlgHex2Binary); // 2018.07.06

		/* _endthread given to terminate */
		_endthread();
		return 1;
	}
#endif



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
    Record_Nb = 0;    // Used for reporting errors
    First_Word = 0;


    /* get highest and lowest addresses so that we can allocate the right size */
    do
    {
        unsigned int i;

		if(dwHex2BinExit) goto ForceEnded; // added at 2016.03.05

		memset( Line, 0x00, sizeof(Line) ); // added at 2016.03.09_initial

        /* Read a line from input file. */
        GetLine(Line,Filin);
        Record_Nb++;

        /* Remove carriage return/line feed at the end of line. */
        i = strlen(Line);

	#if 0 // 2016.03.09_initial
        if (--i != 0)
	#else
		if (--i > 0)
	#endif
        {
            if (Line[i] == '\n') Line[i] = '\0';
            if (Line[i] == '\r') Line[i] = '\0'; // added by 2016.03.10
		
            p = (char *) Data_Str;

            switch(Line[1])
            {
            case '0':
            	Nb_Bytes = 1; /* This is to fix the Highest_Address set to -1 when Nb_Bytes = 0 */

				// if( Verbose_Flag ) EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
                break;

            /* 16 bits address */
			// S1 Record. The type of record field is 'S1' (0x5331).
			// The address field is intrepreted as a 2-byte address. 
			// The data field is composed of memory loadable data.
            case '1':
                result = sscanf (Line,"S%1x%2x%4x",&Type,&Nb_Bytes,&First_Word);
	            if (result != 3 && (i>0) ) 
	            {
					// fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
					EB_Printf(TEXT("[dnw] S1-Error in line %d of hex file. (%d) \r\n"), Record_Nb, result );
					EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );

					iErrCount ++; // 2016.03.05
	            }
                /* Adjust Nb_Bytes for the number of data bytes */
                Nb_Bytes = Nb_Bytes - 3;
                break;

            /* 24 bits address */
			// S2 Record. The type of record field is 'S2' (0x5332). 
			// The address field is intrepreted as a 3-byte address. 
			// The data field is composed of memory loadable data.
            case '2':
                result = sscanf (Line,"S%1x%2x%6x",&Type,&Nb_Bytes,&First_Word);
	            if (result != 3 && (i>0) ) 
	            {
					//fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
					EB_Printf(TEXT("[dnw] S2-Error in line %d of hex file. (%d) \r\n"), Record_Nb, result );
					EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );

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
				// S 3 15 08000000000C0020F99C0208799102087B910208 ED
                result = sscanf (Line,"S%1x%2x%8x",&Type,&Nb_Bytes,&First_Word);
	            if (result != 3 && (i>0) ) 
            	{
					//fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
					EB_Printf(TEXT("[dnw] S3-Error in line %d of hex file. (%d) \r\n"), Record_Nb, result );
					EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );

					iErrCount ++; // 2016.03.05

	            }

                /* Adjust Nb_Bytes for the number of data bytes */
                Nb_Bytes = Nb_Bytes - 5;
                break;


			// S5 Record. The type of record field is 'S5' (0x5335). 
			// The address field is intrepreted as a 2-byte value and 
			//  contains the count of S1, S2, and S3 records previously transmitted. 
			// There is no data field. 
            case '5':
			case '4':

			
			// The srec_cat command will only ever use one, provided the number of lines fits in 16 bits, 
			// otherwise it will use S6.
			// S6 : An optional record containing the number of S1, S2 and S3 records transmitted 
			// in a particular block. The count appears in the 3-byte address field. 
			// There is no data field.
			// This record is optional, you do not have to use it. 
			// Nobody knows if you can have more than one in a file; 
			// and if you do, nobody knows whether or not the line count resets after each one.
			// Nobody knows what happens if you mix S5 and S6 records in a file.
			// The srec_cat command will only ever use one, provided the number of lines fits in 24 bits.

            case '6':
				// if (Verbose_Flag) EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
				break;

			// S7 Record. The type of record field is 'S7' (0x5337). 
			// The address field contains the starting execution address and is intrepreted as 4-byte address. 
			// There is no data field. 
            case '7':
				// if (Verbose_Flag) EB_Printf(TEXT("[dnw] 4-byte addr:S7 [%s] \r\n"), Line );
				break;
				
			// S8 Record. The type of record field is 'S8' (0x5338). 
			// The address field contains the starting execution address and is intrepreted as 3-byte address. 
			// There is no data field. 
			case '8':
				//if (Verbose_Flag) EB_Printf(TEXT("[dnw] 3-byte addr:S8 [%s] \r\n"), Line );
				break;
				
			// S9 Record. The type of record field is 'S9' (0x5339). 
			// The address field contains the starting execution address and is intrepreted as 2-byte address. 
			// There is no data field. 
			case '9':
				// if (Verbose_Flag) EB_Printf(TEXT("[dnw] 2-byte addr:S9 [%s] \r\n"), Line );
				break;

            }

            Phys_Addr = First_Word;

		

		#if 1 // added at 2016.03.10. Zero-Forced!!
			// ----------------------------------------
			// Hex Address Zero based converted 
			// ----------------------------------------
			if( HEX2BIN_ZERO_FORCED==Enable_HexaAddr_Zero_Forced )
			{
				// S 3 0D 00000000 FFFFFFFF8007E80B7C
				// S 3 15 08000000 000C0020F99C0208799102087B910208ED
				// S 3 15 9FC00000 00B4083C2D20093C40002935201009AD7D
		
				if( iOnceCheck && (Line[1]=='1' || Line[1]=='2' || Line[1]=='3') )
				{
					iOnceCheck = 0;
					if( Phys_Addr>0x00000000 )
					{
						Phys_AddrTemp = Phys_Addr;
						EB_Printf(TEXT("[dnw] --------------------------------------------------------\r\n"));
						EB_Printf(TEXT("[dnw] Line%6d :S%c-Record: Phys_Addr = 0x%08X. \r\n"), Record_Nb,Line[1], Phys_Addr );
					}
				}
				
				if( Phys_AddrTemp && 0==iOnceCheck)
				{
					Phys_Addr -= Phys_AddrTemp;
					// EB_Printf(TEXT("[dnw] S%c-rrecord: Phys_Addr = 0x%08X \r\n"), Line[1], Phys_Addr );
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

				if (Verbose_Flag) 
				{
					EB_Printf(TEXT("[dnw] Highest Address : 0x%08X  Read Byte:0x%02X \r\n"), Highest_Address, Nb_Bytes );
				}
            }
        }
    }
    while (!feof (Filin));


	// ---------------------------------
	if( iErrCount > MAX_ERR_COUNT ) // added at 2016.03.05
	{

	#ifdef USE_FILE_BUFFERS
		free (FilinBuf);
		free (FiloutBuf);
	#endif
	
		fclose (Filin);
		fclose (Filout);

		iErrCount = 0; // clear

		EB_Printf(TEXT("\r\n[dnw] Too many error.  Check hex type (Intel or Motorola!!) \r\n"));


		// added at 2016.03.11, button Enabled!!
		UserButtonEnable(_hDlgHex2Binary, 1);

		UserButtonPadByteOnOff(_hDlgHex2Binary); // 2018.07.06

		BeepSound(4);

		dwHex2BinExit = 0;
		iCovertedCompleted = 1; // completed!
		Status_Checksum_Error = 0; // no error

		/* _endthread given to terminate */
		_endthread();

		return 1;
	}
	// ---------------------------------


#if 0 // 2016.03.07	
    Allocate_Memory_And_Rewind();
#else
	if( 0==Allocate_Memory_And_Rewind() )
		goto ForceEnded;
#endif


    Record_Nb = 0;

    /* Read the file & process the lines. */
    do /* repeat until EOF(Filin) */
    {
        int i;

        Checksum = 0;

		if(dwHex2BinExit) goto ForceEnded; // added at 2016.03.05


		memset( Line, 0x00, sizeof(Line) ); // added at 2016.03.09_initial

        /* Read a line from input file. */
        GetLine(Line,Filin);
        Record_Nb++;

        /* Remove carriage return/line feed at the end of line. */
        i = strlen(Line);

	#if 0 // 2016.03.09_initial
        if (--i != 0)
	#else
        if (--i > 0)
	#endif
        {
            if (Line[i] == '\n') Line[i] = '\0';
            if (Line[i] == '\r') Line[i] = '\0'; // added by 2016.03.10

            /* Scan starting address and nb of bytes. */
            /* Look at the record type after the 'S' */
            Type = 0;

            switch(Line[1])
            {
            case '0':
                result = sscanf (Line,"S0%2x0000484452%2x",&Nb_Bytes,&Record_Checksum);
	            if (result != 2 && (i>0) ) 
	            {
					// ------------------------------------------------------------
					// -- [S0 17 52656C65617365204275696C645C726F6D702E6F7574 77] 
					// ------------------------------------------------------------
					char TempC[3], TempIdx=2;
					int  wasChar=0;
					unsigned char TempVal=0, TempCRCSumVal=0, TempLenVal=0, TempCRC=0, iCount=0;
					TCHAR szS0tmp[5]={0,}, szS0rectxt[MAX_PATH]={0,};

					memset( szS0rectxt, 0x00, sizeof(szS0rectxt) );

					lstrcpy( szS0rectxt, TEXT("[dnw] S0-record: [") );
					for(TempIdx=2; Line[TempIdx] != 0x00; TempIdx+=2 )
					{
						memset( TempC, 0x00, sizeof(TempC) );
						strncpy( (char*)TempC, (char *)&Line[TempIdx], 2);
						TempVal = str2hex(TempC);
						iCount ++;
						if( 2==TempIdx ) 
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
									wsprintf(szS0tmp, TEXT("%c"), TempVal);
									wasChar=1;
								}
								else
								{
									if(wasChar)
									{
										wasChar=0;	
										wsprintf(szS0tmp, TEXT(" %02x "), TempVal);
									}
									else
									{
										wsprintf(szS0tmp, TEXT("%02x "), TempVal);
									}
								}
								
								lstrcat( szS0rectxt, szS0tmp );
							}
						}
						//EB_Printf(TEXT("[%x] \r\n"), TempVal, TempCRCSumVal );
					}
					EB_Printf(TEXT("%s] \r\n"), szS0rectxt );

					
					TempCRC    = TempVal;  // minus last Checksum 
					TempCRCSumVal = TempCRCSumVal - TempVal; // minus last Checksum 
					TempCRCSumVal = TempCRCSumVal + Nb_Bytes;
					// EB_Printf(TEXT("\r\n %X == %X \r\n"), TempCRCSumVal, Record_Checksum%0xFF);

					// checksum confirm!!
					if( ((255-TempCRCSumVal)&0xFF) == (TempVal&0xFF) )
					{
						// OK !!!!
						//Checksum = Nb_Bytes + 0x52 + 0x65 + 0x6C + 0x65 + 0x61 + 0x73 + 0x65 + 0x20 + 0x42 + 0x75 + 0x69 + 0x6C + 0x64 + 0x5C + 0x72 + 0x6F + 0x6D + 0x70 + 0x2E + 0x6F + 0x75 + 0x74;
						//EB_Printf(TEXT("[dnw] Checksum = 0x%X \r\n"), (255-Checksum) );
					}
					else
					{
						//fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
						EB_Printf(TEXT("[dnw] S0-Error in line %d of selected hex file. (%d) \r\n"), Record_Nb, result );
						EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
					}

					// read checksum ---
					Checksum = TempCRCSumVal;
					Record_Checksum = TempCRC;
	            }
				else // 0000484452
				{
	                Checksum = Nb_Bytes + 0x48 + 0x44 + 0x52;

					EB_Printf(TEXT("[dnw] S0-record(default): %c%c%c \r\n"), 0x48, 0x44, 0x52 );
				}
				
                /* Adjust Nb_Bytes for the number of data bytes */
                Nb_Bytes = 0;
                break;

            /* 16 bits address */
            case '1':
                result = sscanf (Line,"S%1x%2x%4x%s",&Type,&Nb_Bytes,&Address,Data_Str);
	            if (result != 4 && (i>0) ) 
	            {
					//fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
					EB_Printf(TEXT("[dnw] S1-Error in line %d of selected hex file. (%d) \r\n"), Record_Nb, result );
					EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
	            }
                Checksum = Nb_Bytes + (Address >> 8) + (Address & 0xFF);

                /* Adjust Nb_Bytes for the number of data bytes */
                Nb_Bytes = Nb_Bytes - 3;
                break;

            /* 24 bits address */
            case '2':
                result = sscanf (Line,"S%1x%2x%6x%s",&Type,&Nb_Bytes,&Address,Data_Str);
	            if (result != 4 && (i>0) ) 
	            {
					//fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
					EB_Printf(TEXT("[dnw] S2-Error in line %d of selected hex file. (%d) \r\n"), Record_Nb, result );
					EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
	            }
                Checksum = Nb_Bytes + (Address >> 16) + (Address >> 8) + (Address & 0xFF);

                /* Adjust Nb_Bytes for the number of data bytes */
                Nb_Bytes = Nb_Bytes - 4;
                break;

            /* 32 bits address */
            case '3':
                result = sscanf (Line,"S%1x%2x%8x%s",&Type,&Nb_Bytes,&Address,Data_Str);
	            if (result != 4 && (i>0) ) 
	            {
					//fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
					EB_Printf(TEXT("[dnw] S3-Error in line %d of selected hex file. (%d) \r\n"), Record_Nb, result );
					EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
	            }

                Checksum = Nb_Bytes + (Address >> 24) + (Address >> 16) + (Address >> 8) + (Address & 0xFF);

                /* Adjust Nb_Bytes for the number of data bytes */
                Nb_Bytes = Nb_Bytes - 5;
                break;

            case '5':
                result = sscanf (Line,"S%1x%2x%4x%2x",&Type,&Nb_Bytes,&Record_Count,&Record_Checksum);
	            if (result != 4 && (i>0) ) 
	           	{
					//fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
					EB_Printf(TEXT("[dnw] S5-Error in line %d of selected hex file. (%d) \r\n"), Record_Nb, result );
					EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
	            }
                Checksum = Nb_Bytes + (Record_Count >> 8) + (Record_Count & 0xFF);

                /* Adjust Nb_Bytes for the number of data bytes */
                Nb_Bytes = 0;
                break;

            case '7':
                result = sscanf (Line,"S%1x%2x%8x%2x",&Type,&Nb_Bytes,&Exec_Address,&Record_Checksum);
	            if (result != 4 && (i>0) ) 
            	{
					//fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
					EB_Printf(TEXT("[dnw] S7-Error in line %d of selected hex file. (%d) \r\n"), Record_Nb, result );
					EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
            	}
                Checksum = Nb_Bytes + (Exec_Address >> 24) + (Exec_Address >> 16) + (Exec_Address >> 8) + (Exec_Address & 0xFF);
                Nb_Bytes = 0;
                break;

            case '8':
                result = sscanf (Line,"S%1x%2x%6x%2x",&Type,&Nb_Bytes,&Exec_Address,&Record_Checksum);
	            if (result != 4 && (i>0) ) 
            	{
					//fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
					EB_Printf(TEXT("[dnw] S8-Error in line %d of selected hex file. (%d) \r\n"), Record_Nb, result );
					EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
	            }
                Checksum = Nb_Bytes + (Exec_Address >> 16) + (Exec_Address >> 8) + (Exec_Address & 0xFF);
                Nb_Bytes = 0;
                break;

            case '9':
                result = sscanf (Line,"S%1x%2x%4x%2x",&Type,&Nb_Bytes,&Exec_Address,&Record_Checksum);
	            if (result != 4 && (i>0) ) 
	            {
					//fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
					EB_Printf(TEXT("[dnw] S9-Error in line %d of selected hex file. (%d) \r\n"), Record_Nb, result );
					EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
	            }
                Checksum = Nb_Bytes + (Exec_Address >> 8) + (Exec_Address & 0xFF);
                Nb_Bytes = 0;
                break;
            }

            p = (char *) Data_Str;

            /* If we're reading the last record, ignore it. */
            switch (Type)
            {
            case 0: // added at 2016.03.10
				break;

            /* Data record */
            case 1:
            case 2:
            case 3:
                if (Nb_Bytes == 0)
                {
                    //fprintf(stderr,"0 byte length Data record ignored\n");
					EB_Printf(TEXT("[dnw] S%d-Record: byte length Data record ignored. \r\n"), Type );
					EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
                    break;
                }

                Phys_Addr = Address;



			#if 1 // 2016.03.10
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
							EB_Printf(TEXT("[dnw] S%d-Record: Zero-forced Error!!! Phys_Addr = 0x%08X,  Phys_Addr_Base = 0x%08X \r\n"), Type, Phys_Addr, Phys_AddrTemp );
							//BeepSound(4);
						}
					}
				}
			#endif



				p = ReadDataBytes(p);

                /* Read the Checksum value. */
                result = sscanf (p, "%2x",&Record_Checksum);
	            if (result != 1) 
				{
					//fprintf(stderr,"Error in line %d of hex file\n", Record_Nb);
					EB_Printf(TEXT("[dnw] S%d-Error in line %d of selected hex file. (%d) \r\n"), Type, Record_Nb, result );
					EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
            	}
                break;


            case 5:
                //fprintf(stderr,"Record total: %d\n",Record_Count);
				EB_Printf(TEXT("[dnw] S5-Record total: %d \r\n"), Record_Count );
				//EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
                break;

            case 7:
                //fprintf(stderr,"Execution Address (unused): %08X\n",Exec_Address);
				EB_Printf(TEXT("[dnw] S7-Execution Address (unused): 0x%08X \r\n"),Exec_Address );
				//EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
                break;

            case 8:
                //fprintf(stderr,"Execution Address (unused): %06X\n",Exec_Address);
				EB_Printf(TEXT("[dnw] S8-Execution Address (unused): 0x%06X \r\n"),Exec_Address );
				//EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
                break;

            case 9:
                //fprintf(stderr,"Execution Address (unused): %04X\n",Exec_Address);
				EB_Printf(TEXT("[dnw] S9-Execution Address (unused): 0x%04X \r\n"),Exec_Address );
				//EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
                break;

            /* Ignore all other records */
            default: // S4, S6
				if( Verbose_Flag )
				{
					EB_Printf(TEXT("[dnw] S%d-Record: Phys_Addr = 0x%08X \r\n"), Type, Phys_Addr );
					EB_Printf(TEXT("[dnw] [%s] \r\n"), Line );
				}
            	// ignore -- break;
            }

            Record_Checksum &= 0xFF;

            /* Verify Checksum value. */
            if (((Record_Checksum + Checksum) != 0xFF) && Enable_Checksum_Error)
            {
                //fprintf(stderr,"Checksum error in record %d: should be %02X\n",Record_Nb, 255-Checksum);
				EB_Printf(TEXT("[dnw] Line%6d : Checksum error: should be 0x%02X, wrong checksum: 0x%02X \r\n"),Record_Nb, 255-Checksum, Record_Checksum );
                Status_Checksum_Error = true;
            }
        }
    }
    while (!feof (Filin));
    /*-----------------------------------------------------------------------------*/

#if 0
    fprintf(stdout,"Binary file start = %08X\n",Lowest_Address);
    fprintf(stdout,"Records start     = %08X\n",Records_Start);
    fprintf(stdout,"Highest address   = %08X\n",Highest_Address);
    fprintf(stdout,"Pad Byte          = %X\n",  Pad_Byte);

#else
	if( Verbose_Flag )
	{
		EB_Printf(TEXT("[dnw] --------------------------------------------------------\r\n"));
		EB_Printf(TEXT("[dnw] Binary file start        = 0x%08X\n"),Lowest_Address );
		EB_Printf(TEXT("[dnw] Records start            = 0x%08X\n"),Records_Start );
		EB_Printf(TEXT("[dnw] Highest address          = 0x%08X\n"),Highest_Address);
		EB_Printf(TEXT("[dnw] Pad Byte                 = 0x%X\n"),Pad_Byte );
		EB_Printf(TEXT("[dnw] --------------------------------------------------------\r\n"));

	#if 1 // 2016.03.10
		// ----------------------------------------
		// Hex Address Zero based converted 
		// ----------------------------------------
		if( HEX2BIN_ZERO_FORCED==Enable_HexaAddr_Zero_Forced )
		{
			if( Phys_AddrTemp )
			{
				EB_Printf(TEXT("[dnw] --------------------------------------------------------\r\n"));
				EB_Printf(TEXT("[dnw] Hex file real start      = 0x%08X \n"),Lowest_Address + Phys_AddrTemp );
				EB_Printf(TEXT("[dnw] Hex real Highest address = 0x%08X -> %.2f kB \n"),Highest_Address + Phys_AddrTemp, (float)(Highest_Address + Phys_AddrTemp)/1024.0);
				EB_Printf(TEXT("[dnw] --------------------------------------------------------\r\n"));
			}
		}
	#endif


	}
#endif


    WriteMemory();

#ifdef USE_FILE_BUFFERS
    free (FilinBuf);
    free (FiloutBuf);
#endif

    fclose (Filin);
    fclose (Filout);

    if (Status_Checksum_Error && Enable_Checksum_Error)
    {
        //fprintf(stderr,"Checksum error detected.\n");
		EB_Printf(TEXT("[dnw] Motorola S-Record Checksum error detected. \r\n"));

		dwHex2BinExit = 0;
		iCovertedCompleted = 1; // completed!
		Status_Checksum_Error = 0; // no error

		// added at 2016.03.11, button Enabled!!
		UserButtonEnable(_hDlgHex2Binary, 1);

		UserButtonPadByteOnOff(_hDlgHex2Binary); // 2018.07.06

		BeepSound(4);

		/* _endthread given to terminate */
		_endthread();
		return 1;
    }

#if 0 // 2016.03.05
    if (!Fileread)
        usage();
#endif

	EB_Printf(TEXT("[dnw] --------------------------------------------------------\r\n"));
	EB_Printf(TEXT("[dnw] Input File :[%s] \n"), szH2BInpFile); 
	EB_Printf(TEXT("[dnw] Output File:[%s] \n"), szH2BOutFile); // szH2BFullOutputFile );
	if( HEX2BIN_ZERO_FORCED==Enable_HexaAddr_Zero_Forced )
		EB_Printf(TEXT("[dnw] Motorola S-Record address Zero-forced!!! Phys_Base_Addr:0x%08X \n"), Phys_AddrTemp );

	if( Swap_Wordwise )
		EB_Printf(TEXT("[dnw] Motorola S-Record Swap Word-wise!!! \r\n"));

	EB_Printf(TEXT("[dnw] Motorola S-Record is converted to binary. OK! (Total parsed line#:%d) \r\n"), Record_Nb );


	BeepSound(3); // OK



ForceEnded:

	if(Filin)  { fclose (Filin);  Filin=NULL;  }
	if(Filout) { fclose (Filout); Filout=NULL;  }

	// added at 2016.03.11, button Enabled!!
	UserButtonEnable(_hDlgHex2Binary, 1);

	UserButtonPadByteOnOff(_hDlgHex2Binary); // 2018.07.06

	if( dwHex2BinExit )
	{
		BeepSound(4);
		EB_Printf(TEXT("\r\n[dnw] Motorola S-Record>>User forced ended! \r\n"));
	}
	dwHex2BinExit = 0;

	iCovertedCompleted = 1; // completed!
	Status_Checksum_Error = 0; // no error

	/* _endthread given to terminate */
	_endthread();

	return 0;
}


int MenuHex2BinSelect(HWND hwnd, int BinType)
{
//	uintptr_t threadResult;
	BOOL result = FALSE;
	HANDLE hFile = NULL;
	int iidx=0, jidx=0, isFind=0;
	__int64 ulFileSize = 0UL;

	TCHAR tmpH2BFullInputFile[MAX_PATH] = {0,}; // 2017.09.06
	TCHAR tmpH2BTitleInpFile[MAX_PATH] = {0,}; // 2017.09.06



	// -----------------------------------------------------------------
	// 1. popup
	// -----------------------------------------------------------------
	//result =  PopFileHex2BinOpenDlg(hwnd, szH2BFullInputFile, szH2BTitleInpFile);
	result =  PopFileHex2BinOpenDlg(hwnd, tmpH2BFullInputFile, tmpH2BTitleInpFile);
	if( 1 != result )
	{
		//EB_Printf(TEXT("[dnw] Select Hex file to convert binary!! (%d) \r\n"), result );
		return result;
	}


	// input Filename
	memset( szH2BFullInputFile, 0x00, sizeof(szH2BFullInputFile) );
	memset( szH2BTitleInpFile, 0x00, sizeof(szH2BTitleInpFile) );
	memset( szH2BInpFile, 0x00, sizeof(szH2BInpFile) );

	lstrcpy( szH2BFullInputFile, tmpH2BFullInputFile ); // 2017.09.06
	lstrcpy( szH2BTitleInpFile, tmpH2BTitleInpFile );   // 2017.09.06

	// output Filename
	memset( szH2BFullOutputFile, 0x00, sizeof(szH2BFullOutputFile) );
	memset( szH2BOutFile, 0x00, sizeof(szH2BOutFile) );

	// CRC Filename
	memset( szH2BFullCrcOutFile, 0x00, sizeof(szH2BFullCrcOutFile) );
	memset( szH2BCRCFile, 0x00, sizeof(szH2BCRCFile) );

	dwHex2BinExit = 0;
	iCheckedInputFile = 0;

	
	//EB_Printf(TEXT("[dnw] result (%d)  \r\n"), result );
	// -----------------------------------------------------------------
	// 2. Display Filename ---------------------------------------------
	iidx = strlen(szH2BFullInputFile);
	if( iidx > MAX_PATH )
	{
		EB_Printf(TEXT("\r\n[dnw] Input file length is over! length(%d) \r\n"), iidx );
		BeepSound(4);
	}


	isFind=0;
	for(jidx=iidx; iidx>0; jidx--)
	{
		if( szH2BFullInputFile[jidx] == '\\' ) { isFind=1; break; }
	}
	strcpy( szH2BInpFile, &szH2BFullInputFile[jidx+1] ); // input filename


	// --------------------------------------------------
	// output file -> extension .bin
	// --------------------------------------------------
	strcpy( szH2BFullOutputFile, szH2BFullInputFile); // output filename
	strcpy( szH2BFullCrcOutFile, szH2BFullInputFile); // CRC output filename
	
	iidx = strlen(szH2BFullOutputFile);
	isFind=0;
	for(jidx=iidx; iidx>0; jidx--)
	{
		if( szH2BFullOutputFile[jidx] == '.' ) 
		{ 
			isFind=1; 
			break; 
		}
	}
	// Output Filename extension : .bin
	szH2BFullOutputFile[jidx+1] = 'b';
	szH2BFullOutputFile[jidx+2] = 'i';
	szH2BFullOutputFile[jidx+3] = 'n';
	szH2BFullOutputFile[jidx+4] = '\0';

	// CRC output filename extension : .crc
	szH2BFullCrcOutFile[jidx+1] = 'c';
	szH2BFullCrcOutFile[jidx+2] = 'r';
	szH2BFullCrcOutFile[jidx+3] = 'c';
	szH2BFullCrcOutFile[jidx+4] = '\0';


	iidx = strlen(szH2BFullOutputFile);
	isFind=0;
	for(jidx=iidx; iidx>0; jidx--)
	{
		if( szH2BFullOutputFile[jidx] == '\\' ) { isFind=1; break; }
	}
	strcpy( szH2BOutFile, &szH2BFullOutputFile[jidx+1] ); // output filename	
	// -----------------------------------------------------------------


	// CRC filename ----
	iidx = strlen(szH2BFullCrcOutFile);
	isFind=0;
	for(jidx=iidx; iidx>0; jidx--)
	{
		if( szH2BFullCrcOutFile[jidx] == '\\' ) { isFind=1; break; }
	}
	strcpy( szH2BCRCFile, &szH2BFullCrcOutFile[jidx+1] ); // CRC filename	
	// -----------------------------------------------------------------


	// -----------------------------------------------------------------
	// 3. File Read and size
	// -----------------------------------------------------------------
	hFile = CreateFile(szH2BFullInputFile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
	{
		DWORD dwError = GetLastError();
		switch(dwError)
		{
			case ERROR_PATH_NOT_FOUND: // 3 (0x3)
				//EB_Printf(TEXT("[dnw] can not open file[%s]. The system cannot find the specified path. \n"), szH2BFullInputFile );
				break;

			case ERROR_SHARING_VIOLATION: // 32 (0x20)
				EB_Printf(TEXT("[dnw] can not open file[%s]. Because it is being used by another process. \r\n"), szH2BInpFile );
				break;			
			
			case ERROR_FILE_NOT_FOUND: // 2 (0x2)
				EB_Printf(TEXT("[dnw] The system cannot find the file specified. [%s] \n"), szH2BInpFile );
				break;

			default:
				EB_Printf(TEXT("[dnw] can not open file[%s]. Err(%u)\n"), szH2BInpFile /*szH2BFullInputFile*/, dwError );
				break;
		}
		return -1;
	}

#if 0 // 2016.10.11
	ulFileSize = GetFileSize(hFile,NULL);
#else
	ulFileSize = GetFileExSize(hFile);
#endif

	if(hFile)  { CloseHandle(hFile);  hFile=NULL; }


	// -----------------------------------------------------
	// -----------------------------------------------------

	return result;

}


// -------------------------------------------
// Hex2bin Convertor buttons
// -------------------------------------------
void UserButtonEnable(HWND hWnd, int OnOff)
{
	// added at 2016.03.11, button Enabled or Disabled!!
	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDOK), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, ID_HEX2BIN_INPUT ), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, ID_HEX2BIN_OUTPUT ), OnOff );
		
	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_HEX2BIN_EDIT_INFILE ), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_HEX2BIN_EDIT_FILESIZE ), OnOff );

	EnableWindow( GetDlgItem(_hDlgHex2Binary, ID_TYPE_INTEL_FAMILY ), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, ID_TYPE_MOTOROLA_FAMILY ), OnOff );
	
	EnableWindow( GetDlgItem(_hDlgHex2Binary, ID_CRC_VERYFY_OFF ), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, ID_CRC_VERYFY_ON ), OnOff );

	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_HEX2BIN_ADDR_ALIGN_WORD_OFF ), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_HEX2BIN_ADDR_ALIGN_WORD_ON ), OnOff );
	
	EnableWindow( GetDlgItem(_hDlgHex2Binary, ID_UNIT_KBYTES ), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, ID_UNIT_BYTE ), OnOff );
	
	EnableWindow( GetDlgItem(_hDlgHex2Binary, ID_PHY_ADDR_REAL_ADDR ), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, ID_PHY_ADDR_ZERO_FORCED ), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, ID_PHY_ADDR_HIGH ), OnOff ); // 2018.07.05

	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_HEX2BIN_SWAP_WORDWISE_OFF ), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_HEX2BIN_SWAP_WORDWISE_ON ), OnOff );

	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_HEX2BIN_PADBYTE_OFF ), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_HEX2BIN_PADBYTE_ON ), OnOff );

	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_HEX2BIN_EDIT_PADBYTE ), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_HEX2BIN_PADBYTE_SPREAD_CHECK ), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_H2B_TITLE_PADBYTE ), OnOff );

	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_H2B_TITLE_BIN_SIZE ), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_H2B_TITLE_FILE_SIZE ), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_H2B_TITLE_BIN_UNIT ), OnOff );

	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_H2B_STATIC_HEX_FILE ), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_H2B_STATIC_HEX_TYPE ), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_H2B_STATIC_CRC ), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_H2B_STATIC_WORD_ALIGN ), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_H2B_STATIC_PAD_BYTE ), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_STATIC_UNIT ), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_H2B_STATIC_SWAP ), OnOff );
	EnableWindow( GetDlgItem(_hDlgHex2Binary, IDC_H2B_STATIC_HEX_ADDR ), OnOff );


	return;
}


void UserButtonPadByteOnOff(HWND hDlgHex2Bin) // unsigned int OnOff)
{
	switch( userPadByteOnOff )
	{
		case 0: // None
			EnableWindow( GetDlgItem(hDlgHex2Bin, IDC_HEX2BIN_EDIT_FILESIZE ), 0 ); // Disabled!!		
			EnableWindow( GetDlgItem(hDlgHex2Bin, IDC_STATIC_UNIT ), 0 ); // Disabled!! 	
			EnableWindow( GetDlgItem(hDlgHex2Bin, ID_UNIT_KBYTES ), 0 ); // Enabled!!		
			EnableWindow( GetDlgItem(hDlgHex2Bin, ID_UNIT_BYTE ), 0 ); // Enabled!! 	

			EnableWindow( GetDlgItem(hDlgHex2Bin, IDC_H2B_TITLE_BIN_SIZE ), 0 );
			EnableWindow( GetDlgItem(hDlgHex2Bin, IDC_H2B_TITLE_FILE_SIZE ), 0 );
			EnableWindow( GetDlgItem(hDlgHex2Bin, IDC_H2B_TITLE_BIN_UNIT ), 0 );

			EnableWindow( GetDlgItem(hDlgHex2Bin, IDC_HEX2BIN_EDIT_PADBYTE ), 0 ); // 2020.06.25
			EnableWindow( GetDlgItem(hDlgHex2Bin, IDC_HEX2BIN_PADBYTE_SPREAD_CHECK ), 0 );
			EnableWindow( GetDlgItem(hDlgHex2Bin, IDC_H2B_TITLE_PADBYTE ), 0 );

			if (SendDlgItemMessage(hDlgHex2Bin, IDC_HEX2BIN_PADBYTE_SPREAD_CHECK, BM_GETCHECK, 0, 0)==BST_CHECKED) 
			{
				SendDlgItemMessage(hDlgHex2Bin, IDC_HEX2BIN_PADBYTE_SPREAD_CHECK, BM_SETCHECK, BST_UNCHECKED, 0);
				tempPadbyte = isPadByteAllArea = 0;
			}
			break;

		case 1: // Use
			EnableWindow( GetDlgItem(hDlgHex2Bin, IDC_HEX2BIN_EDIT_FILESIZE ), 1 ); // Enabled!!		
			EnableWindow( GetDlgItem(hDlgHex2Bin, IDC_STATIC_UNIT ), 1 ); // Enabled!!		
			EnableWindow( GetDlgItem(hDlgHex2Bin, ID_UNIT_KBYTES ), 1 ); // Enabled!!		
			EnableWindow( GetDlgItem(hDlgHex2Bin, ID_UNIT_BYTE ), 1 ); // Enabled!! 	

			EnableWindow( GetDlgItem(hDlgHex2Bin, IDC_H2B_TITLE_BIN_SIZE ), 1 );
			EnableWindow( GetDlgItem(hDlgHex2Bin, IDC_H2B_TITLE_FILE_SIZE ), 1 );
			EnableWindow( GetDlgItem(hDlgHex2Bin, IDC_H2B_TITLE_BIN_UNIT ), 1 );

			EnableWindow( GetDlgItem(hDlgHex2Bin, IDC_HEX2BIN_EDIT_PADBYTE ), 1 ); // 2020.06.25
			EnableWindow( GetDlgItem(hDlgHex2Bin, IDC_HEX2BIN_PADBYTE_SPREAD_CHECK ), 1 );
			EnableWindow( GetDlgItem(hDlgHex2Bin, IDC_H2B_TITLE_PADBYTE ), 1 );

			break;
	}
}

// (__cdecl, __stdcall, __fastcall 또는 __vectorcall)
BOOL CALLBACK cbHex2BinProc(HWND hDlgHex2Bin, UINT message, WPARAM wParam, LPARAM lParam)
{
	WORD pmId;
	WORD wmEvent, wmId;
	int tmpLen=0;
	//int txtEnd=0;
	uintptr_t threadResult;

	
	pmId      = LOWORD(lParam);

	wmEvent   = HIWORD(wParam);
	wmId      = LOWORD(wParam); 

	switch(message)
	{
	case WM_INITDIALOG:

		_hDlgHex2Binary      = hDlgHex2Bin;


		// ~~ GetRegistry();
		Status_Checksum_Error = 0; // no error
		Starting_Address = 0;


		// Hex file type
		tempHexType          = userHexType;
		CheckRadioButton(hDlgHex2Bin, ID_TYPE_INTEL_FAMILY, ID_TYPE_MOTOROLA_FAMILY, ID_TYPE_INTEL_FAMILY+tempHexType);


		// Log On/Off
		tempHex2BinLogOnOff  = userHex2BinLogOnOff;
		CheckRadioButton(hDlgHex2Bin, ID_HEX2BIN_LOG_OFF, ID_HEX2BIN_LOG_ON, ID_HEX2BIN_LOG_OFF+tempHex2BinLogOnOff);


		// CRC Type
		tempCRCtype          = userCRCtype; // CRC8:default
		CheckRadioButton(hDlgHex2Bin, ID_CRCTYPE_CHK8_SUM, ID_CRCTYPE_CRC32, ID_CRCTYPE_CHK8_SUM+tempCRCtype);

		// Endian
		tempEndian           = userEndian;
		CheckRadioButton(hDlgHex2Bin, ID_ENDIAN_LITTLE, ID_ENDIAN_BIG, ID_ENDIAN_LITTLE+tempEndian);


		// Hex address zero-forced!
		tempHexAddrZeroForced  = userHexAddrZeroForced;
		CheckRadioButton(hDlgHex2Bin, ID_PHY_ADDR_REAL_ADDR, ID_PHY_ADDR_HIGH, ID_PHY_ADDR_REAL_ADDR+tempHexAddrZeroForced);


		// CRC Verify
		tempCRCverify        = userCRCverify;
		CheckRadioButton(hDlgHex2Bin, ID_CRC_VERYFY_OFF, ID_CRC_VERYFY_ON, ID_CRC_VERYFY_OFF+tempCRCverify);


		// Address Word-alignment
		tempAddrAlignWord    = userAddrAlignWord;
		CheckRadioButton(hDlgHex2Bin, IDC_HEX2BIN_ADDR_ALIGN_WORD_OFF, IDC_HEX2BIN_ADDR_ALIGN_WORD_ON, IDC_HEX2BIN_ADDR_ALIGN_WORD_OFF+tempAddrAlignWord);


		// Swap Wordwise
		tempSwapWordwise        = userSwapWordwise;
		CheckRadioButton(hDlgHex2Bin, IDC_HEX2BIN_SWAP_WORDWISE_OFF, IDC_HEX2BIN_SWAP_WORDWISE_ON, IDC_HEX2BIN_SWAP_WORDWISE_OFF+tempSwapWordwise);


		tempuserPadByteOnOff    = userPadByteOnOff; // 2018.07.06
		CheckRadioButton(hDlgHex2Bin, IDC_HEX2BIN_PADBYTE_OFF, IDC_HEX2BIN_PADBYTE_ON, IDC_HEX2BIN_PADBYTE_OFF+tempuserPadByteOnOff);

		if(tempuserPadByteOnOff==0)
			Max_Length_Setted = 0;
		else
			Max_Length_Setted = 1;


		tempSizeUnit         = userSizeUnit;
		CheckRadioButton(hDlgHex2Bin, ID_UNIT_KBYTES, ID_UNIT_BYTE, ID_UNIT_KBYTES+tempSizeUnit);
		if( 0 == userSizeUnit ) // KB
		{
			dwSizeUnit = 1024; // KByte
			SetDlgItemText( hDlgHex2Bin, IDC_STATIC_UNIT, UNIT_TXT_KBYTE); 
		}
		else
		{
			dwSizeUnit = 1; // Byte
			SetDlgItemText( hDlgHex2Bin, IDC_STATIC_UNIT, UNIT_TXT_BYTE ); 
		}


		// GetDlgItem(hDlgHex2Bin,IDC_STATIC_UNIT)->SetWindowText(hDlgHex2Bin, "change text");


		//dwHex2BinMaxSize = 640; // Kbyte
		//wsprintf(szHex2BinMaxSize, TEXT("%d"), dwHex2BinMaxSize);

		SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_FILESIZE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szHex2BinMaxSize);  
		szHex2BinMaxSize[15]='\0';
		dwHex2BinMaxSize = str2int(szHex2BinMaxSize);	

		if( dwHex2BinMaxSize < 1 )
		{
		
			// ----------------------------------------------------------------------------
			//get Pad Byte
			*((WORD *)szHex2BinMaxSize)=15;
			//The first WORD should be max character number for EM_GETLINE!
			tmpLen = SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_FILESIZE), 
											EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szHex2BinMaxSize);

			if( tmpLen < 1 )
			{
				memset(szHex2BinMaxSize, 0x00, sizeof(szHex2BinMaxSize) );
				lstrcpy(szHex2BinMaxSize, TEXT("640") );
				dwHex2BinMaxSize = str2int(szHex2BinMaxSize);	

				SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_FILESIZE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szHex2BinMaxSize);  
			}
		}


	#if 1

		if (SendDlgItemMessage(hDlgHex2Bin, IDC_HEX2BIN_PADBYTE_SPREAD_CHECK, BM_GETCHECK, 0, 0)==BST_CHECKED) 
		{
			SendDlgItemMessage(hDlgHex2Bin, IDC_HEX2BIN_PADBYTE_SPREAD_CHECK, BM_SETCHECK, BST_UNCHECKED, 0);
			tempPadbyte = isPadByteAllArea = 0;
		}
	

		// ++++++++++++++++++++++++++++++++++++
		
		SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_PADBYTE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szPadByte);  
		szPadByte[15]='\0';
		dwPadByte = Pad_Byte = str2hex(szPadByte);	

		if( dwPadByte < 1 )
		{
			// ----------------------------------------------------------------------------
			//get Pad Byte
			*((WORD *)szPadByte)=15;
			//The first WORD should be max character number for EM_GETLINE!
			tmpLen = SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_PADBYTE), 
											EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szPadByte);

			if( tmpLen < 1 )
			{
				memset(szPadByte, 0x00, sizeof(szPadByte) );
				lstrcpy(szPadByte, TEXT("FF") );
				dwPadByte = Pad_Byte = str2hex(szPadByte);	

				SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_PADBYTE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szPadByte);  
			}
		}
	#endif


	

		// last input file
		szH2BFullInputFile[MAX_PATH-1]='\0';
		SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_INFILE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szH2BFullInputFile);  

		szH2BInpFile[MAX_PATH-1]='\0';
		SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_INFILE_SH),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szH2BInpFile);  


		// output file : make the end of the text shown.
		// txtEnd=SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_OUTFILE),WM_GETTEXTLENGTH,0x0,0x0)-1;
		// SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_OUTFILE),EM_SETSEL,txtEnd+1,txtEnd+2); 

		// last output file	
		szH2BFullOutputFile[MAX_PATH-1]='\0';
		SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_OUTFILE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szH2BFullOutputFile);  

		szH2BOutFile[MAX_PATH-1]='\0';
		SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_OUTFILE_SH),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szH2BOutFile);  


		SetWindowText(_hDlgHex2Binary, APPTITLE TITLE_HEX2BIN); // update title

		// added at 2016.03.11, button Enabled!!
		UserButtonEnable(_hDlgHex2Binary, 1);

		UserButtonPadByteOnOff(hDlgHex2Bin); // 2018.07.06


		isHex2BinConvert4CRC = 1;
		return TRUE;



	case WM_COMMAND:
		switch(LOWORD(wParam))
		{

		case IDCANCEL:
			// -------------------------------------------------------
			// Clear before exit
			// -------------------------------------------------------
		#if 0 // 2016.03.09
			memset( szH2BFullInputFile, 0x00, sizeof(szH2BFullInputFile) );
			memset( szH2BTitleInpFile, 0x00, sizeof(szH2BTitleInpFile) );
			memset( szH2BInpFile, 0x00, sizeof(szH2BInpFile) );

			memset( szH2BFullOutputFile, 0x00, sizeof(szH2BFullOutputFile) );
			memset( szH2BOutFile, 0x00, sizeof(szH2BOutFile) );
		#endif
	


			// ----------------------------------------------
			// Initialized parameter for next running
			// ----------------------------------------------
			
			isHex2BinConvert4CRC = 0;

			iCheckedInputFile = 0;

			userSwapWordwise = 0; // initial Swap Off mode
			Swap_Wordwise = 0; // Swap Off mode


			userHex2BinLogOnOff = 0; // Log Off
			Verbose_Flag = 0;  // Log Off

			Status_Checksum_Error = 0; // no error after exit!!

			userAddrAlignWord = 0; // intel address word-alignment off!
			Address_Alignment_Word = 0; // intel address word-alignment off!

			dwHex2BinExit = 1; // exit!!

			iOnceCheck = 1; // added at 2016.03.10

			// ----------------------------------------------




			if( 0==EndDialog(hDlgHex2Bin,0) )
			{
				// Error 
			}
			
			if( hDlgHex2Bin ) { CloseHandle(hDlgHex2Bin); hDlgHex2Bin=NULL; }
			if( _hDlgHex2Binary ) { CloseHandle( _hDlgHex2Binary ); _hDlgHex2Binary=NULL; }

			return TRUE;


		case IDOK:

			Status_Checksum_Error = 0; // no error
			iOnceCheck = 1; // added at 2016.03.10

			// added at 2016.03.11, button Disabled!!
			UserButtonEnable(_hDlgHex2Binary, 0); // Disabled !!


			if( 0==iCovertedCompleted )
			{
				// added at 2016.03.11, button Enabled!!
				UserButtonEnable(_hDlgHex2Binary, 1);

				UserButtonPadByteOnOff(_hDlgHex2Binary); // 2018.07.06

				EB_Printf( TEXT("\r\n[dnw] Now Hex2Bin is running! wait!!\r\n") );
				BeepSound(4); // Error

				return FALSE;
			}


			// input file check
			if( FALSE == isFileExist(szH2BFullInputFile) )
			{
				// added at 2016.03.11, button Enabled!!
				UserButtonEnable(_hDlgHex2Binary, 1);

				UserButtonPadByteOnOff(_hDlgHex2Binary); // 2018.07.06

				EB_Printf(TEXT("[dnw] Intel hex or Motorola hex file not found! \r\n") );
				BeepSound(4); // Error

				return FALSE;
			}

		#if 0
			if( FALSE == isFileExist(szH2BFullOutputFile) )
			{
				EB_Printf( TEXT("[dnw] output file exists! \r\n") );
			}
		#endif	



//			SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_FILESIZE),EM_SETSEL,0,-1); // all the text is selected. 
//			SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_FILESIZE),EM_REPLACESEL,0, (LPARAM)""); 

			// ----------------------------------------------------------------------------
			//get MaxFileSize
			*((WORD *)szHex2BinMaxSize)=15;
			//The first WORD should be max character number for EM_GETLINE!
			tmpLen = SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_FILESIZE), 
											EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szHex2BinMaxSize);

			szHex2BinMaxSize[tmpLen]='\0';
			dwHex2BinMaxSize = str2int(szHex2BinMaxSize);	
			// ----------------------------------------------------------------------------



		#if 1 // 2020.06.26
			if( userPadByteOnOff == 0 )
			{
				dwPadByte = Pad_Byte = 0xff;
				tempPadbyte = isPadByteAllArea = 0;
				Max_Length_Setted = 0;
				//EB_Printf(TEXT("\r\n[dnw] userPadByteOnOff disable (%d) \r\n"), userPadByteOnOff );
			}
			else
			{

				// ----------------------------------------------------------------------------
				//get Pad Byte
				*((WORD *)szPadByte)=15;
				//The first WORD should be max character number for EM_GETLINE!
				tmpLen = SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_PADBYTE), 
												EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szPadByte);
				szPadByte[tmpLen]='\0';
				dwPadByte = Pad_Byte= str2hex(szPadByte);	
				// ----------------------------------------------------------------------------

				//EB_Printf(TEXT("\r\n[dnw] IDC_HEX2BIN_PADBYTE_SPREAD_CHECK   (%d) ! %#X \r\n"), isPadByteAllArea, dwPadByte );
			}
		#endif

		
			// ----------------------------------------------------------------------------
			// Max File size
		#if 1 // 2018.07.06
			if( (dwHex2BinMaxSize > 0) && userPadByteOnOff) 
		#else
			if( dwHex2BinMaxSize > 0 )
		#endif
			{
				Max_Length_Setted = 1;
				if(Verbose_Flag)
				{
					if( 1024 == dwSizeUnit )
						EB_Printf( TEXT("[dnw] Max. binary file size is set. [%d] KBytes \r\n"), dwHex2BinMaxSize );
					else
						EB_Printf( TEXT("[dnw] Max. binary file size is set. [%d] Bytes \r\n"), dwHex2BinMaxSize );
				}
			}
			else if( (dwHex2BinMaxSize==0) || (0==userPadByteOnOff) ) // size 0 or Pad-Byte Off
			{
				Max_Length_Setted = 0;
				if(Verbose_Flag)
					EB_Printf( TEXT("[dnw] Max. binary file size is clear [%d]. Pad-Byte Off(%d) \r\n"), dwHex2BinMaxSize, userPadByteOnOff );
			}
			else
			{
				Max_Length_Setted = 0;
				if(Verbose_Flag)
					EB_Printf( TEXT("[dnw] Max. binary file size is clear [%d]. Pad-Byte(%d) \r\n"), dwHex2BinMaxSize, userPadByteOnOff );
			}
			
		#if 0 // 2020.07.03
			// Hex file zero-forced!!
			if( HEX2BIN_ZERO_FORCED==userHexAddrZeroForced ) // Zero-forced
				Enable_HexaAddr_Zero_Forced = 1;
			else if( HEX2BIN_HIGH_ADDR==userHexAddrZeroForced )
				Enable_HexaAddr_Zero_Forced = 2;
			else 
				Enable_HexaAddr_Zero_Forced = 0;
		#else
			Enable_HexaAddr_Zero_Forced = userHexAddrZeroForced;
		
		#endif
		
		
			Max_Length = (dwHex2BinMaxSize * dwSizeUnit); // because of Unit KByte (dwSizeUnit)
			if(Max_Length > MAX_LENGTH_SIZE ) // 8MByte
			{
				//fprintf(stderr,"Max_Length = %u\n", Max_Length);
				EB_Printf( TEXT("[dnw] Error Max_Length = %u KB. %s \r\n"), Max_Length>>10, MAX_LENGTH_TXT );
			}
			// ----------------------------------------------------------------------------
	
			//EB_Printf( TEXT("[dnw] [%s] (%d)  (%d)---\r\n"), szHex2BinMaxSize, dwHex2BinMaxSize, Max_Length/1024 );



			if( 0 == userHexType ) // Intel
			{
				threadResult = _beginthread( (void (*)(void *))ConvertIntelHex2Bin, THREAD_STACK_SIZE,(void *)0);
				if(threadResult!=-1)
				{
					Sleep(10);
				}
				else
				{
					EB_Printf("[dnw] can not create Intel Hex2Bin thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());

					// added at 2016.03.11, button Enabled!!
					UserButtonEnable(_hDlgHex2Binary, 1);

					UserButtonPadByteOnOff(_hDlgHex2Binary); // 2018.07.06

					return FALSE;
				}
			}
			else if( 1 == userHexType ) // Motorola S-record
			{
				threadResult = _beginthread( (void (*)(void *))ConvertMotorolaSRecordHex2Bin, THREAD_STACK_SIZE,(void *)0);
				if(threadResult!=-1)
				{
					// OK - Run Thread
					Sleep(10);
				}
				else
				{
					EB_Printf("[dnw] can not create Motorola S-record thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());

					// added at 2016.03.11, button Enabled!!
					UserButtonEnable(_hDlgHex2Binary, 1);

					UserButtonPadByteOnOff(_hDlgHex2Binary); // 2018.07.06

					return FALSE;
				}
			}


			Sleep(100);

		#if 1// 2016.04.03
			SetRegistry();
		#else
			ThreadSetRegistry(NULL);
		#endif


		#if 0
			// Destroys a modal dialog box, causing the system to end any processing for the dialog box. 
			if( 0==EndDialog(hDlgHex2Bin,1) )
			{
		    	// Error
			}
			if( hDlgHex2Bin ) { CloseHandle(hDlgHex2Bin); hDlgHex2Bin=NULL; }
			if( _hDlgHex2Binary ) { CloseHandle( _hDlgHex2Binary ); _hDlgHex2Binary=NULL; }

		#endif
	
			return TRUE;


		case ID_HEX2BIN_CLEAR_SCR:
			ScrClear(_EditHwnd);
			return TRUE;


		case ID_UNIT_KBYTES:
		case ID_UNIT_BYTE:
			userSizeUnit = tempSizeUnit = LOWORD(wParam)-ID_UNIT_KBYTES;	

			if( 0 == userSizeUnit ) // KB
			{
				dwSizeUnit = 1024; // KByte
				SetDlgItemText( hDlgHex2Bin, IDC_STATIC_UNIT, UNIT_TXT_KBYTE ); 
			}
			else
			{
				dwSizeUnit = 1; // Byte
				SetDlgItemText( hDlgHex2Bin, IDC_STATIC_UNIT, UNIT_TXT_BYTE ); 
			}

			return TRUE;


		case ID_HEX2BIN_INPUT:

			MenuHex2BinSelect(hDlgHex2Bin, userHexType);
			//~~~ if( MenuHex2BinSelect(hDlgHex2Bin, userHexType) )
			{
				// OK - clear screen
				// input file
				SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_INFILE),EM_SETSEL,0,-1); // all the text is selected. 
				SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_INFILE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)"");
				
				SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_INFILE_SH),EM_SETSEL,0,-1); // all the text is selected. 
				SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_INFILE_SH),EM_REPLACESEL,0,(LPARAM)(LPCSTR)"");

				// output file
				SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_OUTFILE),EM_SETSEL,0,-1); // all the text is selected. 
				SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_OUTFILE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)"");

				SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_OUTFILE_SH),EM_SETSEL,0,-1); // all the text is selected. 
				SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_OUTFILE_SH),EM_REPLACESEL,0,(LPARAM)(LPCSTR)"");

			}


			// Input filename
			// SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_INFILE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szH2BInpFile);
			SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_INFILE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szH2BFullInputFile);
			// EB_Printf( TEXT("[dnw] szH2BFullInputFile[%s] ---\r\n"), szH2BFullInputFile );
			// EB_Printf( TEXT("[dnw] szH2BInpFile[%s] ---\r\n"), szH2BInpFile );

			SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_INFILE_SH),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szH2BInpFile);



			// Output filename
			// SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_OUTFILE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szH2BOutFile);
			SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_OUTFILE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szH2BFullOutputFile);
			// EB_Printf( TEXT("[dnw] szH2BFullOutputFile[%s] ---\r\n"), szH2BFullOutputFile );
			// EB_Printf( TEXT("[dnw] szH2BOutFile[%s] ---\r\n"), szH2BOutFile );

			SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_OUTFILE_SH),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szH2BOutFile);

			
			return TRUE;



		case ID_HEX2BIN_OUTPUT:

			return TRUE;



		case IDC_HEX2BIN_EDIT_INFILE:
		{
			int iidx=0, jidx=0, isFind=0;

			iCheckedInputFile = 1;

		#if 0
			// ------------------------------
			// modify in INFILE --
			// ------------------------------
			*((WORD *)szH2BFullInputFile)=MAX_PATH-1;
			tmpLen = SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_INFILE), 
											EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szH2BFullInputFile);
			szH2BFullInputFile[tmpLen]='\0';


			// ------------------------------
			// input file -> short name
			// ------------------------------
			iidx = strlen(szH2BFullInputFile);
			isFind=0;
			for(jidx=iidx; iidx>0; jidx--)
			{
				if( szH2BFullInputFile[jidx] == '\\' ) { isFind=1; break; }
			}
			strcpy( szH2BInpFile, &szH2BFullInputFile[jidx+1] ); // short filename 

			
			// ------------------------------
			// output file -> extension .bin
			// ------------------------------
			strcpy( szH2BFullOutputFile, szH2BFullInputFile);
			iidx = strlen(szH2BFullOutputFile);
			isFind=0;
			for(jidx=iidx; iidx>0; jidx--)
			{
				if( szH2BFullOutputFile[jidx] == '.' ) 
				{ 
					isFind=1; 
					break; 
				}
			}
			szH2BFullOutputFile[jidx+1] = 'b';
			szH2BFullOutputFile[jidx+2] = 'i';
			szH2BFullOutputFile[jidx+3] = 'n';
			szH2BFullOutputFile[jidx+4] = '\0';
			

			// ------------------------------
			// output file -> short name
			// ------------------------------
			iidx = strlen(szH2BFullOutputFile);
			isFind=0;
			for(jidx=iidx; iidx>0; jidx--)
			{
				if( szH2BFullOutputFile[jidx] == '\\' ) { isFind=1; break; }
			}
			strcpy( szH2BOutFile, &szH2BFullOutputFile[jidx+1] ); // short filename 
			

			EB_Printf( TEXT("[dnw] HEX InFile Pos  [%s] \r\n[%s] (%d) \r\n"), szH2BFullInputFile, szH2BFullOutputFile, tmpLen );


			#if 0
			// OK - clear screen
			SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_INFILE),EM_SETSEL,0,-1); // all the text is selected. 
			SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_INFILE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)"");
			
			SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_OUTFILE),EM_SETSEL,0,-1); // all the text is selected. 
			SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_OUTFILE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)"");
		
			// Input filename
			SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_INFILE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szH2BFullInputFile);
			// Output filename
			SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_OUTFILE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szH2BFullOutputFile);
			#endif
			
		#endif


			}
			return TRUE;

		case IDC_HEX2BIN_EDIT_OUTFILE:
			// EB_Printf( TEXT("[dnw] HEX OutFile Pos  \r\n") );

			return TRUE;


		case IDC_HEX2BIN_EDIT_INFILE_SH:
			return TRUE;

		case IDC_HEX2BIN_EDIT_OUTFILE_SH:
			return TRUE;
			

		case IDC_HEX2BIN_EDIT_PADBYTE:

			// ------------------------------------------------------------
			//get PadByte
			*((WORD *)szPadByte)=15;
			//The first WORD should be max character number for EM_GETLINE!
			tmpLen = SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_PADBYTE), 
											EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szPadByte);

			szPadByte[tmpLen]='\0';
			dwPadByte = Pad_Byte = str2hex(szPadByte);	

			isPadValue = 0; // OK
			if( 0==tmpLen )
			{
				isPadValue = 0;
				//dwPadByte = Pad_Byte = 0xff;
			}
			else if( 1==tmpLen )
			{
				if( (szPadByte[tmpLen-1] >= '0' && szPadByte[tmpLen-1] <= '9')
					|| (szPadByte[tmpLen-1] >= 'A' && szPadByte[tmpLen-1] <= 'F')
					|| (szPadByte[tmpLen-1] >= 'a' && szPadByte[tmpLen-1] <= 'f')
					)
				{
					// OK
					//EB_Printf(TEXT("[dnw] 1 [%c] %d \r\n"), szPadByte[0], tmpLen  );
				}
				else
				{
					isPadValue++; // error count
					//EB_Printf(TEXT("[dnw] Wrong1 Pad byte [%s]. Check it!! %d \r\n"), szPadByte, tmpLen );
				}
					
			}
			else if( 2==tmpLen )
			{
				if( (szPadByte[1] >= '0' && szPadByte[1] <= '9')
					|| (szPadByte[1] >= 'A' && szPadByte[1] <= 'F')
					|| (szPadByte[1] >= 'a' && szPadByte[1] <= 'f')
					)
				{
					// OK
					//EB_Printf(TEXT("[dnw] 2 [%c] %d \r\n"), szPadByte[1], tmpLen  );
				}
				else
				{
					isPadValue++;
					//EB_Printf(TEXT("[dnw] Wrong2 Pad byte [%s]. Check it!! %d \r\n"), szPadByte, tmpLen );
				}					
			}
			else 
			{
				if( (szPadByte[tmpLen-1] >= 'a' && szPadByte[tmpLen-1] <= 'z') )
					szPadByte[tmpLen-1] -= 32; // uppercase

				isPadValue ++;
				//EB_Printf(TEXT("[dnw] Wrong3 Pad byte [%s]. Check it!! %d \r\n"), szPadByte, tmpLen );
			}
			
			return TRUE;


		case IDC_HEX2BIN_PADBYTE_SPREAD_CHECK:
			
			if (SendDlgItemMessage(hDlgHex2Bin, IDC_HEX2BIN_PADBYTE_SPREAD_CHECK, BM_GETCHECK, 0, 0)==BST_UNCHECKED) 
			{
				tempPadbyte = isPadByteAllArea = 0;
			}
			else 
			{
				tempPadbyte = isPadByteAllArea = 1;
			}
			//if(Verbose_Flag)
			//	EB_Printf(TEXT("[dnw] Pad byte empty all area (%d)! \r\n"), isPadByteAllArea );

			return TRUE;

		case IDC_HEX2BIN_EDIT_FILESIZE:

			// ------------------------------------------------------------
			//get MaxFileSize
			*((WORD *)szHex2BinMaxSize)=15;
			//The first WORD should be max character number for EM_GETLINE!
			tmpLen = SendMessage(GetDlgItem(hDlgHex2Bin,IDC_HEX2BIN_EDIT_FILESIZE), 
											EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szHex2BinMaxSize);

			szHex2BinMaxSize[tmpLen]='\0';

			if( -1 == isinteger(szHex2BinMaxSize) )
			{
				dwHex2BinMaxSize = 0; // because of error! set to zero!
				// EB_Printf( TEXT("[dnw] It is NOT digit!!  [%s] \r\n"), szHex2BinMaxSize  );
			}
			else
			{
				dwHex2BinMaxSize = str2int(szHex2BinMaxSize);	
				// EB_Printf( TEXT("[dnw] digit OK  -- digit: [%d] \r\n"),   dwHex2BinMaxSize);
			}

			// ------------------------------------------------------------
		#if 1 // 2018.07.06
			if( dwHex2BinMaxSize > 0 && userPadByteOnOff) 
		#else
			if( dwHex2BinMaxSize > 0 )
		#endif
			{
				Max_Length = (dwHex2BinMaxSize * dwSizeUnit); // because of Unit KByte (dwSizeUnit)
				Max_Length_Setted = 1;
			}
			else
			{
				Max_Length_Setted = 0;
			}


			return TRUE;



		case ID_TYPE_INTEL_FAMILY:
		case ID_TYPE_MOTOROLA_FAMILY:
			userHexType = tempHexType = LOWORD(wParam)-ID_TYPE_INTEL_FAMILY;

			// 0 : Intel
			// 1 : Motorola S-record
		
			return TRUE;


		// -----------------------------------------------
		// -k option
		// -----------------------------------------------
		case ID_CRCTYPE_CHK8_SUM: // 0:  Checksum  8-bit
		case ID_CRCTYPE_CHK16:    // 1:  Checksum 16-bit
		case ID_CRCTYPE_CRC8:     // 2:  CRC8 - default
		case ID_CRCTYPE_CRC16:    // 3:  CRC16
		case ID_CRCTYPE_CRC32:    // 4:  CRC32
			userCRCtype = tempCRCtype = LOWORD(wParam)-ID_CRCTYPE_CHK8_SUM;

			Cks_Type = userCRCtype;

			return TRUE;


		// -------------------------------------------------------
		// -E option : Endian for checksum/CRC, 0: little, 1: big
		// -------------------------------------------------------
		case ID_ENDIAN_LITTLE:
		case ID_ENDIAN_BIG:
			userEndian = tempEndian = LOWORD(wParam)-ID_ENDIAN_LITTLE;

			if( 0 == userEndian ) 
				Endian = 0; // Little-Endian
			else 
				Endian = 1; // Big-Endian

			return TRUE;



		case ID_PHY_ADDR_REAL_ADDR:
		case ID_PHY_ADDR_ZERO_FORCED:
		case ID_PHY_ADDR_HIGH:
			userHexAddrZeroForced = tempHexAddrZeroForced = LOWORD(wParam)-ID_PHY_ADDR_REAL_ADDR;

		#if 0 // 2020.07.03

			if( HEX2BIN_ZERO_FORCED==userHexAddrZeroForced )
				Enable_HexaAddr_Zero_Forced = 1;
			else if( HEX2BIN_HIGH_ADDR==userHexAddrZeroForced )
				Enable_HexaAddr_Zero_Forced = 2;
			else
				Enable_HexaAddr_Zero_Forced = 0;
		#else
			Enable_HexaAddr_Zero_Forced = userHexAddrZeroForced;
		#endif

			return TRUE;

		// --------------------------------------------------------------
		// -c option, CRC verify On default refer userCRCverify=1;
		// --------------------------------------------------------------
		case ID_CRC_VERYFY_OFF:
		case ID_CRC_VERYFY_ON:
			userCRCverify = tempCRCverify = LOWORD(wParam)-ID_CRC_VERYFY_OFF;

			if( userCRCverify )
				Enable_Checksum_Error = 1; // true;
			else
				Enable_Checksum_Error = 0;

			return TRUE;


		// ---------------------------------------------------------------------------
		// -a option : Hex with record type, where data is represented in Word (2 Byte)
		// ---------------------------------------------------------------------------

		case IDC_HEX2BIN_ADDR_ALIGN_WORD_OFF:
		case IDC_HEX2BIN_ADDR_ALIGN_WORD_ON:
			userAddrAlignWord = tempAddrAlignWord = LOWORD(wParam)-IDC_HEX2BIN_ADDR_ALIGN_WORD_OFF;

			if( userAddrAlignWord ) // Intel only!!
				Address_Alignment_Word = 1;
			else
				Address_Alignment_Word = 0;

			return TRUE;


		// ------------------------------------------------------------------------
		// -w option : for each pair of bytes, exchange the low and high part.
		// (low <-> high)
		// ------------------------------------------------------------------------
		case IDC_HEX2BIN_SWAP_WORDWISE_OFF:
		case IDC_HEX2BIN_SWAP_WORDWISE_ON:
			userSwapWordwise = tempSwapWordwise = LOWORD(wParam)-IDC_HEX2BIN_SWAP_WORDWISE_OFF;

			if( 0 == userSwapWordwise )
				Swap_Wordwise = 0;
			else
				Swap_Wordwise = 1; // On
				
			return TRUE;

		case IDC_HEX2BIN_PADBYTE_OFF: // 2018.07.06
		case IDC_HEX2BIN_PADBYTE_ON:
			userPadByteOnOff = tempuserPadByteOnOff = LOWORD(wParam)-IDC_HEX2BIN_PADBYTE_OFF;

			if( userPadByteOnOff == 0 )
			{
				dwPadByte = Pad_Byte = 0xff;
				tempPadbyte = isPadByteAllArea = 0;
				Max_Length_Setted = 0;
			}
			else
			{
				Max_Length_Setted = 1;
			}

			
			UserButtonPadByteOnOff(hDlgHex2Bin);

			return TRUE;

		


		// -------------------------------------
		// -v option Off : Log On/Off
		// -------------------------------------
		case ID_HEX2BIN_LOG_OFF:
		case ID_HEX2BIN_LOG_ON:
			userHex2BinLogOnOff = tempHex2BinLogOnOff = LOWORD(wParam)-ID_HEX2BIN_LOG_OFF;

			if( userHex2BinLogOnOff )
				Verbose_Flag = 1; // Log On true;
			else
				Verbose_Flag = 0; // Log Off

			return TRUE;


		#if 0
		// -----------------------------------------
		
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

			case 'm':
				Minimum_Block_Size = GetHex(argv[Param + 1]);
				Minimum_Block_Size_Setted = true;
				i = 1; /* add 1 to Param */
				break;

			case 'r':
				Cks_Start = GetHex(argv[Param + 1]);
				Cks_End = GetHex(argv[Param + 2]);
				Cks_range_set = true;
				i = 2; /* add 2 to Param */
				break;
			case 's':
				Starting_Address = GetHex(argv[Param + 1]);
				Starting_Address_Setted = true;
				i = 1; /* add 1 to Param */
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
		// -----------------------------------------
		#endif



		default:
			return FALSE;


		} // case WM_COMMAND:

#if 0
	case WM_SETFOCUS:
		SetFocus( hDlgHex2Bin );
		EB_Printf( TEXT("[dnw] WM_SETFOCUS in cbHex2BinProc ---\r\n") );
		return TRUE;
#endif


	default:
		return FALSE;
	}

	return FALSE;
}

#endif // USE_HEX2BIN_CONVERT // 2016.03.04




#if USE_TXDATA_TO_SERIAL // 2016.03.27

__int64 ulTxDataUARTFileSize = 0UL;

FILE *fpTxData = NULL;
//int isTxDataExit = 0;


void UserButtonTxData(HWND hWnd, int OnOff)
{

	SetWindowText(_hDlgTxData2UART, APPTITLE TITLE_TX_DATA); // update title

	if( TX_TYPE_CHAR==sendTxDataType ) // Char Type
	{
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDOK ), OnOff );
		//EnableWindow( GetDlgItem(_hDlgTxData2UART, IDCANCEL ), 1 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_SAVE ), OnOff );
		//EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_CLEAR_SCR ), 1 );
		//EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_STOP ), 1 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_CHAR_MODE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_CHAR ), OnOff );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_HEXA_MODE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_HEXA ), 0 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_FILE_MODE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_SCRI_MODE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_RAND_MODE ), OnOff ); // 2018.03.23 Random number
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_SEQU_MODE ), OnOff ); // 2018.03.26 Sequential number

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_BTN_FILE ), 0 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_FILE ), 0 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_DISP_FILE ), 0 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_CHAR ), OnOff ); // 2018.07.06
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_HEXA ), 0 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_FILE ), 0 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_XOR ), 0 ); // 2018.08.21
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_XOR ), 0 ); // 2018.08.21

	}
	else if( TX_TYPE_HEXA==sendTxDataType ) // Hexa Type
	{
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDOK ), OnOff );
		//EnableWindow( GetDlgItem(_hDlgTxData2UART, IDCANCEL ), 1 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_SAVE ), OnOff );
		//EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_CLEAR_SCR ), 1 );
		//EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_STOP ), 1 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_CHAR_MODE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_CHAR ), 0 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_HEXA_MODE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_HEXA ), OnOff );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_FILE_MODE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_SCRI_MODE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_RAND_MODE ), OnOff ); // 2018.03.23 Random number
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_SEQU_MODE ), OnOff ); // 2018.03.26 Sequential number

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_BTN_FILE ), 0 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_FILE ), 0 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_DISP_FILE ), 0 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_CHAR ), 0 ); // 2018.07.06
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_HEXA ), OnOff ); // 2018.07.06
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_FILE ), 0 ); // 2018.07.06
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_XOR ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_XOR ), 0 ); // 2018.08.21

	}
	else if( TX_TYPE_FILE==sendTxDataType ) // File Type  
	{
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDOK ), OnOff );
		//EnableWindow( GetDlgItem(_hDlgTxData2UART, IDCANCEL ), 1 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_SAVE ), OnOff );
		//EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_CLEAR_SCR ), 1 );
		//EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_STOP ), 1 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_CHAR_MODE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_CHAR ), 0 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_HEXA_MODE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_HEXA ), 0 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_FILE_MODE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_SCRI_MODE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_RAND_MODE ), OnOff ); // 2018.03.23 Random number
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_SEQU_MODE ), OnOff ); // 2018.03.26 Sequential number

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_BTN_FILE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_FILE ), 0 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_DISP_FILE ), 0 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_CHAR ), 0 ); // 2018.07.06
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_HEXA ), 0 ); // 2018.07.06
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_FILE ), OnOff ); // 2018.07.06
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_XOR ), 0 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_XOR ), 0 ); // 2018.08.21

	}
	else if( TX_TYPE_SCRIPT==sendTxDataType ) // Script File Type  
	{
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDOK ), OnOff );
		//EnableWindow( GetDlgItem(_hDlgTxData2UART, IDCANCEL ), 1 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_SAVE ), OnOff );
		//EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_CLEAR_SCR ), 1 );
		//EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_STOP ), 1 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_CHAR_MODE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_CHAR ), 0 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_HEXA_MODE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_HEXA ), 0 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_FILE_MODE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_SCRI_MODE ), OnOff ); // script
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_RAND_MODE ), OnOff ); // 2018.03.23 Random number
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_SEQU_MODE ), OnOff ); // 2018.03.26 Sequential number

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_BTN_FILE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_FILE ), 0 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_DISP_FILE ), 0 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_CHAR ), 0 ); // 2018.07.06
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_HEXA ), 0 ); // 2018.07.06
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_FILE ), OnOff ); // 2018.07.06
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_XOR ), 0 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_XOR ), 0 ); // 2018.08.21

	}
	else if( TX_TYPE_RANDOM==sendTxDataType ) // 2018.03.23
	{
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDOK ), OnOff );
		//EnableWindow( GetDlgItem(_hDlgTxData2UART, IDCANCEL ), 1 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_SAVE ), OnOff );
		//EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_CLEAR_SCR ), 1 );
		//EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_STOP ), 1 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_CHAR_MODE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_CHAR ), 0 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_HEXA_MODE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_HEXA ), 0 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_FILE_MODE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_SCRI_MODE ), OnOff ); // script
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_RAND_MODE ), OnOff ); // 2018.03.23 Random number
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_SEQU_MODE ), OnOff ); // 2018.03.26 Sequential number

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_BTN_FILE ), 0 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_FILE ), 0 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_DISP_FILE ), 0 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_CHAR ), 0 ); // 2018.07.06
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_HEXA ), 0 ); // 2018.07.06
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_FILE ), 0 ); // 2018.07.06
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_XOR ), 0 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_XOR ), 0 ); // 2018.08.21

	}
	else if( TX_TYPE_SEQUENTIAL==sendTxDataType ) // 2018.03.26
	{
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDOK ), OnOff );
		//EnableWindow( GetDlgItem(_hDlgTxData2UART, IDCANCEL ), 1 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_SAVE ), OnOff );
		//EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_CLEAR_SCR ), 1 );
		//EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_STOP ), 1 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_CHAR_MODE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_CHAR ), 0 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_HEXA_MODE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_HEXA ), 0 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_FILE_MODE ), OnOff );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_SCRI_MODE ), OnOff ); // script
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_RAND_MODE ), OnOff ); // 2018.03.23 Random number
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_SEQU_MODE ), OnOff ); // 2018.03.26 Sequential number

		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_BTN_FILE ), 0 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_FILE ), 0 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_DISP_FILE ), 0 );

		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_CHAR ), 0 ); // 2018.07.06
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_HEXA ), 0 ); // 2018.07.06
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_FILE ), 0 ); // 2018.07.06		
		EnableWindow( GetDlgItem(_hDlgTxData2UART, IDC_TXDATA_TITLE_XOR ), 0 );
		EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_XOR ), 0 ); // 2018.08.21

	}


}



int MenuTxDataSelect(HWND hwnd)
{
//	uintptr_t threadResult;
	BOOL result = FALSE;
	HANDLE hFile = NULL;
	int iidx=0, jidx=0, isFind=0;
	TCHAR szLenTxt[MAX_PATH] = {0,};

	TCHAR tmpSerialTxFileData[TX_DATA_LENGTH] = { 0, }; // 2016.09.06
	TCHAR tmpSerialTxFileTitl[TX_DATA_LENGTH] = { 0, }; // 2016.09.06

#if 0 // 201709.06
	// input Filename
	memset( szSerialTxFileData, 0x00, sizeof(szSerialTxFileData) );
	memset( szSerialTxFileTitl, 0x00, sizeof(szSerialTxFileTitl) );
	memset( szSerialTxDispName, 0x00, sizeof(szSerialTxDispName) );
#endif

	// -----------------------------------------------------------------
	// 1. popup
	// -----------------------------------------------------------------
	//result =  PopFileTxDataOpenDlg(hwnd, szSerialTxFileData, szSerialTxFileTitl);
	result =  PopFileTxDataOpenDlg(hwnd, tmpSerialTxFileData, tmpSerialTxFileTitl);
	if( 1 != result )
	{
		//EB_Printf(TEXT("\r\n[dnw] Input file length is NULL! \r\n") );
		return result;
	}


	// input Filename
	memset( szSerialTxFileData, 0x00, sizeof(szSerialTxFileData) );
	memset( szSerialTxFileTitl, 0x00, sizeof(szSerialTxFileTitl) );
	memset( szSerialTxDispName, 0x00, sizeof(szSerialTxDispName) );
	
	lstrcpy(szSerialTxFileData, tmpSerialTxFileData); // 2016.09.06
	lstrcpy(szSerialTxFileTitl, tmpSerialTxFileTitl); // 2016.09.06
		
	// -----------------------------------------------------------------
	// 2. Display Filename ---------------------------------------------
	iidx = strlen(szSerialTxFileData);
	if( iidx > MAX_PATH || iidx<=0 )
	{
		EB_Printf(TEXT("\r\n[dnw] Input file length is over!! or length(%d) is zero!! \r\n"), iidx );
		BeepSound(4);
	}


	isFind=0;
	for(jidx=iidx; iidx>0; jidx--)
	{
		if( szSerialTxFileData[jidx] == '\\' ) { isFind=1; break; }
	}
	strcpy( szSerialTxDispName, &szSerialTxFileData[jidx+1] );


	// -----------------------------------------------------------------
	// 3. File Read and size
	// -----------------------------------------------------------------
	hFile = CreateFile(szSerialTxFileData,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
	{
		DWORD dwError = GetLastError();
		switch(dwError)
		{
			case ERROR_PATH_NOT_FOUND: // 3 (0x3)
				break;

			case ERROR_SHARING_VIOLATION: // 32 (0x20)
				EB_Printf(TEXT("[dnw] can not open file[%s]. because of being used by another process. \r\n"), szSerialTxDispName );
				break;			
			
			case ERROR_FILE_NOT_FOUND: // 2 (0x2)
				EB_Printf(TEXT("[dnw] The system cannot find the file specified. [%s] \n"), szSerialTxDispName );
				break;

			default:
				EB_Printf(TEXT("[dnw] can not open file[%s]. Err(%u)\n"), szSerialTxDispName, dwError );
				break;
		}
		return -1;
	}

#if 0 // 2016.08.12
	ulTxDataUARTFileSize = GetFileSize(hFile,NULL);
#else
	ulTxDataUARTFileSize = GetTxDataFileSize(hFile);
#endif

	if(hFile)  { CloseHandle(hFile);  hFile=NULL; }

	//wsprintf(szLenTxt, TEXT(" == (%I64d Byte)"), ulTxDataUARTFileSize);
	wsprintf(szLenTxt, TEXT(" == (%s Byte)"), PRTComma(ulTxDataUARTFileSize) );
	strcat( szSerialTxDispName, szLenTxt );


	return result;
}



unsigned int WriteTXComm(void *args)
{
	unsigned int iloop=0;
	int isHotKey = 0; // by F6-key(0) or by MENU(1)
	HANDLE hFile = NULL;
	TCHAR  szTitle[MAX_PATH] = {0,};
	
	if( 0==strncmp( (char*)args, (char*)TXDATA_BY_F6Key, 5) 
		&& (TX_TYPE_FILE==sendTxDataType || TX_TYPE_SCRIPT==sendTxDataType || TX_TYPE_RANDOM==sendTxDataType || TX_TYPE_SEQUENTIAL==sendTxDataType) )
	{
		isHotKey = 1;
		EB_Printf(TEXT("[dnw] Do use MENU button to send file/script/random/sequential number!! \r\n") ); 
	}


	if ( 1 != isConnected )
	{
		//MessageBox(_hDlgTxData2UART, TEXT("[dnw] Check!!!! "), NULL,MB_OK);
		UserButtonTxData(_hDlgTxData2UART, 1);

		EB_Printf(TEXT("[dnw] The COM%d port is NOT connected!! \r\n"), userComPort ); 

		isTxData2UART = 0; // Tx Data for UpdateWindowTitle()

		/* _endthread given to terminate */
		_endthread();	
		return 0;
	}


	wsprintf(szTitle, TITLE_TX_DATA);
	     if( TX_TYPE_CHAR==sendTxDataType )   lstrcat(szTitle, TEXT(" - Char to UART"));
	else if( TX_TYPE_HEXA==sendTxDataType )   lstrcat(szTitle, TEXT(" - Hexa to UART"));
	else if( TX_TYPE_FILE==sendTxDataType )   lstrcat(szTitle, TEXT(" - File to UART"));
	else if( TX_TYPE_SCRIPT==sendTxDataType ) lstrcat(szTitle, TEXT(" - Script file to UART"));
	else if( TX_TYPE_RANDOM==sendTxDataType ) lstrcat(szTitle, TEXT(" - Random to UART")); // 2018.03.23
	else if( TX_TYPE_SEQUENTIAL==sendTxDataType ) lstrcat(szTitle, TEXT(" - Seq. to UART")); // 2018.03.26
	else lstrcat(szTitle, TEXT(" "));

	SetWindowText(_hDlgTxData2UART, szTitle); // update title


	if( TX_TYPE_CHAR==sendTxDataType ) // send Char type to UART
	{
		isTxData2UART = 2; // Tx Data for UpdateWindowTitle()

		for(iloop=0; iloop<TX_DATA_LENGTH && (szSerialTxCharData[iloop]!=0x00); iloop++)
		{
			WriteCommBlock( szSerialTxCharData[iloop] );
		}
	
		if( txDataMsgOnOff ) // Log
		{
			if( iloop )
				EB_Printf(TEXT("[dnw] TxChar(%u):[%s] \r\n"), iloop, szSerialTxCharData ); 
			else
				EB_Printf(TEXT("[dnw] No Send TxChar because of NULL data \r\n") ); 
		}
	}
	else if( TX_TYPE_HEXA==sendTxDataType ) // send Hexa type to UART
	{
		TCHAR TxHexaData[MAX_PATH] = {0,} ;
		TCHAR TxSendData[3] = {0,};
		TCHAR TxHexaValue[MAX_PATH*SKIP_NUM+1] = {0,} ;
		int iloop=0, len=0;
		UINT hexval=0, iLineFd=0, iNextPo=0;
		
		memset( TxHexaValue, 0x00, sizeof(TxHexaValue) );
		memset( TxHexaData, 0x00, sizeof(TxHexaData) );
		
		len = ExtractHexVal(szSerialTxHexaData, TxHexaData);
		
		isTxData2UART = 2; // Tx Data for UpdateWindowTitle()
		iLineFd = 0;
		iNextPo = 0;

		for(iloop=0; len && (iloop<TX_DATA_LENGTH) && (TxHexaData[iloop]!=0x00); iloop+=SKIP_NUM )
		{
			memset( TxSendData, 0x00, sizeof(TxSendData) );
			strncpy( TxSendData, &TxHexaData[iloop], SKIP_NUM );
			hexval = str2hex(TxSendData);
	
			WriteCommBlock( hexval );

			if( txDataMsgOnOff ) // Log
			{
				// One time serial output - so speed-up.
				wsprintf( &TxHexaValue[iLineFd*3], TEXT("%02X "), hexval );
		#if 0 // 2016.09.26
				if( iLineFd==15 || (iLineFd>16 && (iLineFd%16)==15) ) 
				{ 
					wsprintf( &TxHexaValue[iLineFd*3+2], TEXT("\r\n") ); 
				}
		#endif	
				iLineFd ++;
			}

		}
		
		if( txDataMsgOnOff ) // Log
		{
			if( iloop )
				EB_Printf(TEXT("[dnw] TxHexa(%u):%s \r\n"), iLineFd, TxHexaValue ); 
			else
				EB_Printf(TEXT("[dnw] No Send TxHexa because of NULL data! \r\n") ); 
		}
	
	}
	else if( 0==isHotKey && TX_TYPE_FILE==sendTxDataType ) // send File (char)type to UART
	{
		// -------------------------------------------------------------------------------
		// Menu에서만 send 가 가능하도록 한다. (0==isHotKey) ------ File 이 큰 경우에 문제 발생 -------
		// -------------------------------------------------------------------------------
#define TXCHAR_DATA_LEN 		16

		unsigned char cch[TXCHAR_DATA_LEN]= {0,};
		unsigned int TotalBytes = 0, rdnBytes = 0, ilp=0;
		TCHAR szcch[TXCHAR_DATA_LEN*3+TXCHAR_DATA_LEN] = {0,};


		EB_Printf(TEXT("\r\n[dnw] Tx File:[%s] \r\n"), szSerialTxDispName ); 

		// input file check
		if( FALSE == isFileExist(szSerialTxFileData) )
		{
			UserButtonTxData(_hDlgTxData2UART, 1); // Enabled!!!

			isTxData2UART = 0; // Tx Data for UpdateWindowTitle()

			/* _endthread given to terminate */
			_endthread();

			EB_Printf( TEXT("[dnw] UART Tx-file not found!! \r\n") );
			BeepSound(4); // Error
		
			return FALSE;
		}

		if( fpTxData ) fclose(fpTxData); // 2019.02.28

		fpTxData = fopen( (const char*)szSerialTxFileData, "rb");
		if( NULL == fpTxData )
		{
			EB_Printf(TEXT("[dnw] Can not open input [%s]. Err(%u)\n"), szSerialTxFileData, GetLastError() );
			fclose(fpTxData);

			UserButtonTxData(_hDlgTxData2UART, 1); // Enabled!!!

			isTxData2UART = 0; // Tx Data for UpdateWindowTitle()
			
			/* _endthread given to terminate */
			_endthread();
			return 1;
		}


		/*-----------------------------------------------------------------------------*/
		/* Fuzz Test LOG File - 2019.1.28 */
		/*-----------------------------------------------------------------------------*/





		/*-----------------------------------------------------------------------------*/
		ilp = 0;
		TotalBytes = 0;
		rdnBytes = 0;

		//if( txDataMsgOnOff ) EB_Printf( TEXT("[dnw] "));
		do {
			if( isTxDataExit ) break;


			memset( cch, 0x00, sizeof(cch) );
			memset( szcch, 0x00, sizeof(szcch) );

			rdnBytes = fread ( cch, 1, TXCHAR_DATA_LEN /*sizeof(cch)*/, fpTxData);
			if( rdnBytes <= 0 ) break; /* 2019.03.01*/

			TotalBytes += rdnBytes;

			for(ilp=0; /*ilp<TXCHAR_DATA_LEN && */ ilp<rdnBytes; ilp++)
			{
				cch[ilp] = cch[ilp] & 0x00FF;

				WriteCommBlock( cch[ilp] );
				if( txDataMsgOnOff ) {
					wsprintf( &szcch[ilp*3], TEXT("%02X "), cch[ilp] );
				}
			}
			if( txDataMsgOnOff ) {
				// cch[16] = '\0';
				// lstrcat( szcch, (TCHAR*)cch );
				EB_Printf( TEXT("[dnw] %s\r\n"), szcch);
			}			

		} while (!feof (fpTxData));

		fclose(fpTxData);
		fpTxData = NULL;

		iloop = TotalBytes;
		isTxData2UART = 0; // Tx Data for UpdateWindowTitle()

		/*-----------------------------------------------------------------------------*/

		if( isTxDataExit ) 
		{
			EB_Printf(TEXT("[dnw] Tx File was stopped by user or COM port problem!!! TxSize(%u) \r\n\r\n"), TotalBytes );
		}
		else
		{
			EB_Printf(TEXT("[dnw] The file was sent on UART. TxSize(%u) \r\n\r\n"), TotalBytes ); 
		}	
	}	
	else if( 0==isHotKey && TX_TYPE_SCRIPT==sendTxDataType ) // send Script File (char)type to UART
	{
		// -------------------------------------------------------------------------------
		// Tx Script File
		// -------------------------------------------------------------------------------

		EB_Printf(TEXT("\r\n[dnw] Tx Script file:[%s] \r\n"), szSerialTxDispName ); 

		int iparsed=0, iOK=0, isLoopStart=0, bLpStart=0, isLoopEnd=0;
		long iFileStartPos=0L, iFileEndPos=0L;
		unsigned int iLen=0, hexval=0, intval=0, ilp=0;
		char cch[TX_SCRIPT_SIZ+1]= {0,};
		TCHAR szcch[TX_SCRIPT_SIZ*2] = {0,};
		char *result = NULL;
		TCHAR TxSendData[TX_SCRIPT_SIZ] = {0,};


		// input file check
		if( FALSE == isFileExist(szSerialTxFileData) )
		{
			UserButtonTxData(_hDlgTxData2UART, 1); // Enabled!!!

			isTxData2UART = 0; // Tx Data for UpdateWindowTitle()

			/* _endthread given to terminate */
			_endthread();

			EB_Printf( TEXT("\r\n[dnw] input script file not found! \r\n") );
			BeepSound(4); // Error
		
			return FALSE;
		}


		fpTxData = fopen( (const char*)szSerialTxFileData, "r");
		if( NULL == fpTxData )
		{
			EB_Printf(TEXT("[dnw] can not open input script file[%s]. Err(%u)\n"), szSerialTxFileData, GetLastError() );
			fclose(fpTxData);

			UserButtonTxData(_hDlgTxData2UART, 1); // Enabled!!!

			isTxData2UART = 0; // Tx Data for UpdateWindowTitle()
			
			/* _endthread given to terminate */
			_endthread();
			return 1;
		}


		/*-----------------------------------------------------------------------------*/
		ilp = 0;
		TxScriptCommandLines = 0;
		isLoopStart = 0;
		isLoopEnd = 0;
		bLpStart = 0;

		do {
			if( isTxDataExit ) break;

			memset( cch, 0x00, sizeof(cch) );

			result = fgets(cch, TX_SCRIPT_SIZ, fpTxData);
			if ((result == NULL) && !feof (fpTxData)) 
			{
				EB_Printf(TEXT("[dnw] Tx Script File read ended. Err(%u)\n"), GetLastError() );
				break;
			}

			//EB_Printf(TEXT("[dnw] == READED[%s] \r\n"), cch );

			iparsed = TxScriptParser(cch); 

			if( '\0'==cch[0] || '\r'==cch[0] || '\n'==cch[0] || '\t'==cch[0] || '\b'==cch[0] || ' '==cch[0] || ','==cch[0] || ';'==cch[0] || ':'==cch[0] || '/'==cch[0] ) 
				continue;

			//if( SCRIPT_NONE==iparsed ) break;
			if( SCRIPT_END==iparsed ) 
			{
				iOK=0; // 2018.10.29, clear
				if( txDataMsgOnOff ) EB_Printf(TEXT("[dnw] TxScript[END]: \r\n"));
				break;
			}
			else if( SCRIPT_START==iparsed ) 
			{ 
				iOK=1; 
				if( txDataMsgOnOff ) EB_Printf(TEXT("[dnw] TxScript[START]: \r\n"));
				continue; 
			}
			else if( 1==iOK && SCRIPT_HEXA==iparsed ) 
			{
				TxScriptCommandLines ++;
				iLen = strlen(cch)-1;

				for(iloop=6, ilp=0; iloop<iLen && iloop<TX_SCRIPT_SIZ; )
				{
					if( isTxDataExit ) break; // User Stop it!!!

					if( 0x20==cch[iloop] || ' '==cch[iloop] || ';'==cch[iloop] || ','==cch[iloop] || '.'==cch[iloop] || '\b'==cch[iloop] || '\t'==cch[iloop] ) 
					{
						iloop++; // next point
					}
					else if( 0x00==cch[iloop] || '\0'==cch[iloop] || '\n'==cch[iloop] || '\r'==cch[iloop] ) 
					{
						break;
					}
					else {
						memset( TxSendData, 0x00, sizeof(TxSendData) );

						strncpy( TxSendData, &cch[iloop], 2 );
						hexval = str2hex(TxSendData);

						WriteCommBlock( hexval );

						if( txDataMsgOnOff ) wsprintf( &szcch[ilp*3], TEXT("%02X "), hexval );
			
						iloop += 2;
						ilp++;
					}

				}
				if( txDataMsgOnOff ) EB_Printf( TEXT("[dnw] TxScript[HEXA>]:{ %s} \r\n"), szcch );

			}
			else if( 1==iOK && SCRIPT_CHAR==iparsed ) 
			{
				TxScriptCommandLines ++;

				iLen = strlen(cch)-1;
				cch[iLen] = '\0';


				if( '\n'==cch[iLen-0] || '\r'==cch[iLen-0]) { cch[iLen-0] = '\0'; }
				if( '\n'==cch[iLen-1] || '\r'==cch[iLen-1]) { cch[iLen-1] = '\0'; iLen -= 1; }
				if( '\n'==cch[iLen-2] || '\r'==cch[iLen-2]) { cch[iLen-2] = '\0'; iLen -= 1; }

				WriteCommBlockArray( &cch[6], iLen-6 );

		
				cch[iLen] = '\0';
				if( txDataMsgOnOff ) EB_Printf( TEXT("[dnw] TxScript[CHAR>]:[%s] (%d) \r\n"), &cch[6], iLen-6 );


			}
			else if( 1==iOK && SCRIPT_SLEEP==iparsed ) 
			{

				int idelay=0, iQuota=0, iRemainder=0;
				TxScriptCommandLines ++;
				iLen = strlen(cch)-1;


				DelSpecialChar(cch, 7, iLen);

				memset( TxSendData, 0x00, sizeof(TxSendData) );
				strncpy( TxSendData, &cch[7], iLen-7 );
				intval = str2int(TxSendData);

				if( txDataMsgOnOff ) EB_Printf( TEXT("[dnw] TxScript[SLEEP]: %d msec --> Waiting...\r\n"), intval );

		#if 1 // for STOP break;
				iQuota     = intval/SLEEP_WAIT_TIME_MIN;
				iRemainder = intval%SLEEP_WAIT_TIME_MIN;

				for(idelay=0; idelay<iQuota; idelay++) {
					if( isTxDataExit ) break;
					Sleep(SLEEP_WAIT_TIME_MIN);
				}
				if(iRemainder) Sleep(iRemainder);

				if( txDataMsgOnOff && isTxDataExit ) {
					EB_Printf( TEXT("[dnw] TxScript[SLEEP]: %d msec --> User stop!! \r\n"), idelay*SLEEP_WAIT_TIME_MIN + iRemainder );
				}
		#else
				Sleep(intval);
		#endif
			}
			else if( 1==iOK && SCRIPT_SEC==iparsed ) // 2020.04.17
			{	
#define SEC_POSITION 		5

				int idelay=0, iQuota=0, iRemainder=0;
				TxScriptCommandLines ++;
				iLen = strlen(cch)-1;
		
		
				DelSpecialChar(cch, SEC_POSITION, iLen);
		
				memset( TxSendData, 0x00, sizeof(TxSendData) );
				strncpy( TxSendData, &cch[SEC_POSITION], iLen-SEC_POSITION );
				intval = str2int(TxSendData);
		
				if( txDataMsgOnOff ) EB_Printf( TEXT("[dnw] TxScript[SEC]: %d sec --> Waiting...\r\n"), intval );
		
#if 0 // for STOP break;
				iQuota	   = intval/SLEEP_WAIT_TIME_MIN;
				iRemainder = intval%SLEEP_WAIT_TIME_MIN;
		
				for(idelay=0; idelay<iQuota; idelay++) {
					if( isTxDataExit ) break;
					Sleep(SLEEP_WAIT_TIME_MIN);
				}
				if(iRemainder) Sleep(iRemainder);
		
				if( txDataMsgOnOff && isTxDataExit ) {
					EB_Printf( TEXT("[dnw] TxScript[SLEEP]: %d msec --> User stop!! \r\n"), idelay*SLEEP_WAIT_TIME_MIN + iRemainder );
				}
#else
				Sleep(intval*SLEEP_WAIT_TIME_MIN);
#endif
			}
			else if( 1==iOK && SCRIPT_MSEC==iparsed ) // 2020.04.17
			{	
#define MSEC_POSITION 		6

				int idelay=0, iQuota=0, iRemainder=0;
				TxScriptCommandLines ++;
				iLen = strlen(cch)-1;
		
		
				DelSpecialChar(cch, MSEC_POSITION, iLen);
		
				memset( TxSendData, 0x00, sizeof(TxSendData) );
				strncpy( TxSendData, &cch[MSEC_POSITION], iLen-MSEC_POSITION );
				intval = str2int(TxSendData);
		
				if( txDataMsgOnOff ) EB_Printf( TEXT("[dnw] TxScript[MSEC]: %d msec --> Waiting...\r\n"), intval );
		
#if 1 // for STOP break;
				iQuota	   = intval/SLEEP_WAIT_TIME_MIN;
				iRemainder = intval%SLEEP_WAIT_TIME_MIN;
		
				for(idelay=0; idelay<iQuota; idelay++) {
					if( isTxDataExit ) break;
					Sleep(SLEEP_WAIT_TIME_MIN);
				}
				if(iRemainder) Sleep(iRemainder);
		
				if( txDataMsgOnOff && isTxDataExit ) {
					EB_Printf( TEXT("[dnw] TxScript[MSEC]: %d msec --> User stop!! \r\n"), idelay*SLEEP_WAIT_TIME_MIN + iRemainder );
				}
#else
				Sleep(intval*SLEEP_WAIT_TIME_MIN);
#endif
			}
			else if( 1==iOK && SCRIPT_DO_START==iparsed ) 
			{

				bLpStart = 1; // ON DO_START
				TxScriptCommandLines ++;

				iFileStartPos = ftell(fpTxData);

				if( txDataMsgOnOff ) EB_Printf( TEXT("[dnw] TxScript[---DO]: ----- LOOP START --------- \r\n") );

			}	
			else if( 1==iOK && SCRIPT_WHILE_END==iparsed ) 
			{
				static int iTotCount = 0;


				//if( isLoopStart < 1 ) {
				iLen = strlen(cch)-1;

				DelSpecialChar(cch, SCRPIT_WHILE_POS, iLen);
		
				memset( TxSendData, 0x00, sizeof(TxSendData) );
				strncpy( TxSendData, &cch[SCRPIT_WHILE_POS], iLen-SCRPIT_WHILE_POS );
				iTotCount = str2int(TxSendData);
				//}

				iFileEndPos = ftell(fpTxData);
				isLoopStart++;

				if( bLpStart && (isLoopStart < iTotCount) ) {
					fseek(fpTxData, -1*(iFileEndPos-iFileStartPos), SEEK_CUR);
					TxScriptCommandLines ++;

					if( txDataMsgOnOff ) 
						EB_Printf( TEXT("[dnw] TxScript[-----]: ----- LOOP COUNT --------- Loop(%d/%d) ------------- \r\n"), isLoopStart, iTotCount );
				}
				
				if( isLoopStart >= iTotCount ) {
					isLoopStart = 0;
					bLpStart = 0;

					if( txDataMsgOnOff ) 
						EB_Printf( TEXT("[dnw] TxScript[WHILE]: ----- LOOP ENDED --------- Total Loop(%d) ------------\r\n"), iTotCount );
				}
			}
			else if( 1==iOK && SCRIPT_RANDOM_GEN==iparsed ) // 2018.10.29 added
			{
				static unsigned int iTotCount = 0;
				unsigned char iRandNum = 0x0;
				long long iRandCount = 0LL;

				iLen = strlen(cch)-1;

				DelSpecialChar(cch, SCRPIT_RANDOM_POS, iLen);

		
				memset( TxSendData, 0x00, sizeof(TxSendData) );
				strncpy( TxSendData, &cch[SCRPIT_RANDOM_POS], iLen-SCRPIT_RANDOM_POS );
				iTotCount = str2int(TxSendData);

				if( txDataMsgOnOff ) 
					EB_Printf( TEXT("[dnw] TxScript[RANDOM]: Total random num: %u --> Waiting...\r\n"), iTotCount );


				if( iTotCount <= 0 )
					EB_Printf( TEXT("[dnw] TxScript[RANDOM]: Total random num: zero...\r\n\r\n") );
				else
				do {
					
					if( isTxDataExit ) break;
			
					iRandNum = rand()%0x100;
					iRandNum &= 0xFF;

					if( iRandCount > iTotCount * 256 -1 ) {
						if( txDataMsgOnOff ) 
							EB_Printf( TEXT("\r\n[dnw] TxScript[RANDOM]: Total random num: %u --> Ended \r\n"), iRandCount );

						break;
					}

					
					WriteCommBlock( iRandNum );

					if( txDataMsgOnOff ) {
						if( 0==iRandCount%16 ) EB_Printf( TEXT("\r\n"));
						EB_Printf( TEXT("%02x "), iRandNum);
					}			

					iRandCount ++;

				} while ( !isTxDataExit );

				if( isTxDataExit ) 
					EB_Printf( TEXT("\r\n[dnw] TxScript[RANDOM]: Total random num: %u --> User cancelled!!! \r\n"), iRandCount );


			}
			else if( 1==iOK && SCRIPT_SEQNUM_GEN==iparsed )  // 2018.10.29 added
			{
				static unsigned int iTotCount = 0;
				unsigned char iRandNum = 0x0;
				long long iRandCount = 0LL;
				long long iCount = 0LL;

				iLen = strlen(cch)-1;

				DelSpecialChar(cch, SCRPIT_RANDOM_POS, iLen);

		
				memset( TxSendData, 0x00, sizeof(TxSendData) );
				strncpy( TxSendData, &cch[SCRPIT_RANDOM_POS], iLen-SCRPIT_RANDOM_POS );
				iTotCount = str2int(TxSendData);

				if( txDataMsgOnOff ) 
					EB_Printf( TEXT("[dnw] TxScript[SEQNUM]: Total sequencial num: %u --> Waiting...\r\n"), iTotCount );


				if( iTotCount <= 0 )
					EB_Printf( TEXT("[dnw] TxScript[SEQNUM]: Total sequencial num: zero...\r\n\r\n") );
				else
				do {
			
					if( isTxDataExit ) break;

					if( txDataMsgOnOff ) {
						if( 0==iCount%16 ) EB_Printf( TEXT("\r\n"));
					}	

					if( iCount > iTotCount * 256 -1 ) {
						if( txDataMsgOnOff ) 
							EB_Printf( TEXT("\r\n[dnw] TxScript[SEQNUM]: Total sequencial num: %u --> Ended \r\n"), iCount );

						break;
					}


					iRandNum = (unsigned char)( iRandCount%0x100 );
					iRandNum &= 0xFF;

					WriteCommBlock( iRandNum );

					if( txDataMsgOnOff ) {
						EB_Printf( TEXT("%02x "), iRandNum);
					}				


					iCount ++;

					iRandCount ++;
					if( iRandCount >= 0x100 ) iRandCount=0;


				} while ( !isTxDataExit );

				if( isTxDataExit ) 
					EB_Printf( TEXT("\r\n[dnw] TxScript[SEQNUM]: Total sequencial num: %u --> User cancelled!!! \r\n"), iCount );

			}



		} while ( !feof(fpTxData) );

		fclose(fpTxData);
		fpTxData = NULL;

		isTxData2UART = 0; // Tx Data for UpdateWindowTitle()

		/*-----------------------------------------------------------------------------*/

		if( 0==TxScriptCommandLines ) 
		{
			EB_Printf(TEXT("[dnw] Check the sample script file, dnwTxScript_Sample.txt. \r\n\r\n")); 
		}
		else //if( txDataMsgOnOff ) // Log
		{
			EB_Printf(TEXT("[dnw] Tx Script is ended. TxScriptCommandLines(%d) \r\n\r\n"), TxScriptCommandLines ); 
		}	
	}

	else if( 0==isHotKey && TX_TYPE_RANDOM==sendTxDataType ) // 2018.03.23
	{
		unsigned char iRandNum = 0x0;
		long long iRandCount = 0LL;
		

		//if( txDataMsgOnOff ) EB_Printf( TEXT("[dnw] "));

		do {
			
			if( isTxDataExit ) break;

			iRandNum = rand()%0x100;
			iRandNum &= 0xFF;

			WriteCommBlock( iRandNum );

			if( txDataMsgOnOff ) {
				if( 0==iRandCount%16 ) EB_Printf( TEXT("\r\n"));
				EB_Printf( TEXT("%02x "), iRandNum);
			}			

			iRandCount ++;

		} while ( !isTxDataExit );

		/*-----------------------------------------------------------------------------*/

		if( isTxDataExit ) 
		{
			EB_Printf(TEXT("[dnw] Tx Random number is stopped by user or COM port problem!! TxSize(%lu) \r\n\r\n"), iRandCount );
		}
		else
		{
			EB_Printf(TEXT("[dnw] Tx Random number is ended. TxSize(%lu) \r\n\r\n"), iRandCount ); 
		}	

	
	}
	else if( 0==isHotKey && TX_TYPE_SEQUENTIAL==sendTxDataType ) // 2018.03.26
	{
		unsigned char iRandNum = 0;
		unsigned int iRandCount = 0;
		

		do {
			
			if( isTxDataExit ) break;

			if( txDataMsgOnOff ) {
				if( 0==iRandCount%16 ) EB_Printf( TEXT("\r\n"));
			}	

			iRandNum = (unsigned char)( iRandCount%0x100 );
			iRandNum &= 0xFF;

			WriteCommBlock( iRandNum );

			if( txDataMsgOnOff ) {
				EB_Printf( TEXT("%02x "), iRandNum);
			}				

			iRandCount ++;
			if( iRandCount >= 0x100 ) iRandCount=0;


		} while ( !isTxDataExit );

		/*-----------------------------------------------------------------------------*/

		if( isTxDataExit ) 
		{
			EB_Printf(TEXT("[dnw] Tx Sequential number is stopped by user or COM port problem!! TxSize(%lu) \r\n\r\n"), iRandCount );
		}
		else
		{
			EB_Printf(TEXT("[dnw] Tx Sequential number is ended. TxSize(%lu) \r\n\r\n"), iRandCount ); 
		}	
	
	}



	//MessageBox(_hDlgTxData2UART, TEXT("[dnw] Check!!!! "), NULL,MB_OK);
	UserButtonTxData(_hDlgTxData2UART, 1);

	isTxData2UART = 0; // Tx Data for UpdateWindowTitle()

	/* _endthread given to terminate */
	_endthread();

	return iloop;
}


BOOL ThreadWriteTXComm(void *args)
{
	uintptr_t threadResult;


	threadResult = _beginthread( (void (*)(void *))WriteTXComm, THREAD_STACK_SIZE, (void *)args );
	if(threadResult!=-1)
	{
		isTxData2UART = 1; // Tx Data for UpdateWindowTitle()
		//Sleep(10);
	}
	else
	{
		EB_Printf("[dnw] can not create WriteTXComm thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
		return FALSE;
	}
	return TRUE;
}


__int64 GetTxDataFileSize(HANDLE hFile)
{
	LARGE_INTEGER FilelargeInt;

	memset(&FilelargeInt, 0x0, sizeof(FilelargeInt));
	if( FALSE==GetFileSizeEx(hFile, &FilelargeInt) )
	{
		EB_Printf(TEXT("[dnw] can not open TxData file size error[%s]. \r\n"), szSerialTxDispName );
	}
	else
	{
		if(FilelargeInt.HighPart == 0  && FilelargeInt.LowPart == 0)
		{
			// file size is zero
			ulTxDataUARTFileSize = 0UL;
		}
		else
		{
			ulTxDataUARTFileSize = FilelargeInt.QuadPart;
		/*
			ulTxDataUARTFileSize = FilelargeInt.HighPart;
			ulTxDataUARTFileSize <<= 32;
			ulTxDataUARTFileSize |= FilelargeInt.LowPart; */
		}
	}
	return ulTxDataUARTFileSize;
}


BOOL CALLBACK cbTxData2UART(HWND hDlgTxData, UINT message, WPARAM wParam, LPARAM lParam)
{
	WORD pmId;
	WORD wmEvent, wmId;
	int tmpLen=0;
	int idx=0, jdx=0, isFind=0;
	//uintptr_t threadResult;
	HANDLE hFile = NULL;
	TCHAR szLenTxt[MAX_PATH] = {0,};
	
	pmId      = LOWORD(lParam);

	wmEvent   = HIWORD(wParam);
	wmId      = LOWORD(wParam); 

	switch(message)
	{
	case WM_INITDIALOG:

		_hDlgTxData2UART      = hDlgTxData;

		isTxDataExit = 0;

		isTxData2UART = 0; // Tx Data for UpdateWindowTitle()

		// Message On/Off 
		tempMsgOnOff          = txDataMsgOnOff;
		CheckRadioButton(hDlgTxData, ID_TXDATA_DISPLAY_MSGOFF, ID_TXDATA_DISPLAY_MSGON, ID_TXDATA_DISPLAY_MSGOFF+tempMsgOnOff);

		// Tx Data Type 
		tempTxDataType  = sendTxDataType;
		CheckRadioButton(hDlgTxData, ID_TXDATA_CHAR_MODE, ID_TXDATA_SEQU_MODE, ID_TXDATA_CHAR_MODE+tempTxDataType);

		UserButtonTxData(_hDlgTxData2UART, 1);

		// last input file
		szSerialTxCharData[MAX_PATH-1]='\0';
		SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_CHAR),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szSerialTxCharData);  

		szSerialTxHexaData[MAX_PATH-1]='\0';
		SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_HEXA),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szSerialTxHexaData);  

		szSerialTxFileData[MAX_PATH-1]='\0';
		SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_FILE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szSerialTxFileData);  

		szSerialTxHexaXOR[MAX_PATH-1]='\0';
		SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_XOR),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szSerialTxHexaXOR);  

		#if 0
		if( TX_TYPE_HEXA==sendTxDataType )
		{
			extern int ExtractHexVal(TCHAR *, TCHAR *);
			extern UINT str2hex(TCHAR *);
			
			TCHAR TxHexaData[MAX_PATH] = {0,} ;
			TCHAR TxSendData[3] = {0,};
			unsigned int len, iloop=0, hexval, xorValue=0x00;
			
			memset( TxHexaData, 0x00, sizeof(TxHexaData) );
			memset( TxSendData, 0x00, sizeof(TxSendData) );

			len = ExtractHexVal(szSerialTxHexaData, TxHexaData);
			
			
			for(iloop=0; len && (iloop<TX_DATA_LENGTH) && (TxHexaData[iloop]!=0x00); iloop+=SKIP_NUM )
			{
				memset( TxSendData, 0x00, sizeof(TxSendData) );
				strncpy( TxSendData, &TxHexaData[iloop], SKIP_NUM );
				hexval = str2hex(TxSendData);
			
				xorValue ^= hexval;
				
				if( txDataMsgOnOff ) EB_Printf(TEXT("[dnw] hexval(0x%02x) -> XOR:0x%02X \r\n"), hexval, xorValue ); 
			
			}
			if( txDataMsgOnOff ) EB_Printf(TEXT("[dnw] ------------------------- \r\n") ); 
			//EB_Printf(TEXT("[dnw] Result XOR:0x%02X \r\n"), xorValue ); 

			SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_XOR),EM_SETSEL,0,-1); // all the text is selected. 
			SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_XOR),EM_REPLACESEL,0,(LPARAM)(LPCSTR)"");

			memset(szSerialTxHexaXOR, 0x00, sizeof(szSerialTxHexaXOR) );
			wsprintf( szSerialTxHexaXOR, TEXT(" %02X"), xorValue );
			SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_XOR),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szSerialTxHexaXOR);  

		}
		#endif



		// ----------------------------------------------------------------------------
		// -----------------------------------------------------------------
		// 2. Display Filename ---------------------------------------------
		idx = strlen(szSerialTxFileData);

		if( idx <= 0 ) // 2017.07.28
		{
			// script or Tx file is Not exist!!!
			//EB_Printf(TEXT("\r\n[dnw] TxData Input is NOT exist!! length(%d) \r\n"), idx );
			return TRUE;
		}
		else if( idx >= MAX_PATH )
		{
			EB_Printf(TEXT("\r\n[dnw] TxData Input file length is over!!! length(%d) \r\n"), idx );
			BeepSound(4);
		}
		
		isFind=0;
		for(jdx=idx; idx>0; jdx--)
		{
			if( szSerialTxFileData[jdx] == '\\' ) { isFind=1; break; }
		}
		strcpy( szSerialTxDispName, &szSerialTxFileData[jdx+1] );
		// -----------------------------------------------------------------

		hFile = CreateFile(szSerialTxFileData,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
		{
			EB_Printf(TEXT("[dnw] can not open TxData file1[%s] Err(%u) \r\n"), szSerialTxDispName, GetLastError() );
			return -1;
		}

#if 0 // 2016.08.12
		ulTxDataUARTFileSize = GetFileSize(hFile,NULL);
#else
		ulTxDataUARTFileSize = GetTxDataFileSize(hFile);
#endif
		
		if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
		
		//wsprintf(szLenTxt, TEXT(" == (%I64d Byte)"), ulTxDataUARTFileSize);
		wsprintf(szLenTxt, TEXT(" == (%s Byte)"), PRTComma(ulTxDataUARTFileSize) );
		strcat( szSerialTxDispName, szLenTxt );


		szSerialTxDispName[MAX_PATH-1]='\0';
		SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_DISP_FILE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szSerialTxDispName);  

		return TRUE;



	case WM_COMMAND:

		wmEvent = HIWORD(wParam);
		wmId	= LOWORD(wParam); 

		switch(LOWORD(wParam))
		{

		case IDCANCEL: // Exit!!
			// -------------------------------------------------------
			// Clear before exit
			// -------------------------------------------------------
			if( TX_TYPE_FILE==sendTxDataType || TX_TYPE_SCRIPT==sendTxDataType || TX_TYPE_RANDOM==sendTxDataType || TX_TYPE_SEQUENTIAL==sendTxDataType) // txDATA File Type
			{
				int iLoopTxFileCnt=0;

				isTxDataExit = 1;
				do {
					if(fpTxData) fseek(fpTxData, 0L, SEEK_END); // 2016.03.28 end of file pointer 
					else break;
					Sleep(TX_STOP_WAIT_TIME); // delay for fclose()
				} while(fpTxData && iLoopTxFileCnt++ < TXFILE_EXIT_COUNT);

				Sleep(TX_STOP_WAIT_TIME); // delay for fclose()
			}

			if( 0==EndDialog(hDlgTxData,0) )
			{
				// Error 
			}
			
			if( hDlgTxData ) { CloseHandle(hDlgTxData); hDlgTxData=NULL; }
			if( _hDlgTxData2UART ) { CloseHandle( _hDlgTxData2UART ); _hDlgTxData2UART=NULL; }

			return TRUE;


		case IDOK:


			UserButtonTxData(_hDlgTxData2UART, 0);
			EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_CHAR ), 0 );
			EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_EDIT_HEXA ), 0 );
			EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_BTN_FILE ), 0 );
			

			isTxDataExit = 0; // start Tx Data

	
			if( 0 == isConnected ) // If disconected!!
				SendMessage( _hwnd, WM_COMMAND, CM_CONNECT, 0 ); // try to connect to COM!
			

	#if 0
			// input file check
			if( FALSE == isFileExist(szH2BFullInputFile) )
			{
				// added at 2016.03.11, button Enabled!!
				UserButtonEnable(_hDlgTxData2UART, 1);

				EB_Printf( TEXT("\r\n[dnw] Intel hex or Motorola hex file not found!! \r\n") );
				BeepSound(4); // Error

				return FALSE;
			}
	#endif


			// ----------------------------------------------------------------------------
			//get MaxFileSize
			*((WORD *)szSerialTxCharData)=MAX_PATH-1;
			//The first WORD should be max character number for EM_GETLINE!
			tmpLen = SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_CHAR), 
											EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szSerialTxCharData);
			szSerialTxCharData[tmpLen]='\0';
			// ----------------------------------------------------------------------------

			// ----------------------------------------------------------------------------
			//get MaxFileSize
			*((WORD *)szSerialTxHexaData)=MAX_PATH-1;
			//The first WORD should be max character number for EM_GETLINE!
			tmpLen = SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_HEXA), 
											EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szSerialTxHexaData);
			szSerialTxHexaData[tmpLen]='\0';
			// ----------------------------------------------------------------------------


			// ----------------------------------------------------------------------------
			//get MaxFileSize
			*((WORD *)szSerialTxFileData)=MAX_PATH-1;
			//The first WORD should be max character number for EM_GETLINE!
			tmpLen = SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_FILE), 
											EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szSerialTxFileData);
			szSerialTxFileData[tmpLen]='\0';
			// ----------------------------------------------------------------------------

			// -----------------------------------------------------------------
			// 2. Display Filename ---------------------------------------------
			idx = strlen(szSerialTxFileData);
			if( (TX_TYPE_FILE==sendTxDataType || TX_TYPE_SCRIPT==sendTxDataType) && (idx > MAX_PATH || idx<=0) ) // File
			{
				EB_Printf(TEXT("[dnw] Input file length is over!! or length(%d) is zero!! \r\n\r\n"), idx );
				BeepSound(4);

				UserButtonTxData(_hDlgTxData2UART, 1);
				return -1;
			}
			else if( TX_TYPE_FILE==sendTxDataType || TX_TYPE_SCRIPT==sendTxDataType ) // File and Script
			{
			
				isFind=0;
				for(jdx=idx; idx>0; jdx--)
				{
					if( szSerialTxFileData[jdx] == '\\' ) { isFind=1; break; }
				}
				strcpy( szSerialTxDispName, &szSerialTxFileData[jdx+1] );
				// -----------------------------------------------------------------

				hFile = CreateFile(szSerialTxFileData,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
				if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
				{
					EB_Printf(TEXT("[dnw] can not open TxData file2[%s]. \r\n\r\n"), szSerialTxDispName );
					BeepSound(4);

					UserButtonTxData(_hDlgTxData2UART, 1);

					return -1;
				}

		#if 0 // 2016.08.12
				ulTxDataUARTFileSize = GetFileSize(hFile,NULL);
		#else
				ulTxDataUARTFileSize = GetTxDataFileSize(hFile);
		#endif
						
				if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
				
				//wsprintf(szLenTxt, TEXT(" == (%I64d Byte)"), ulTxDataUARTFileSize);
				wsprintf(szLenTxt, TEXT(" == (%s Byte)"), PRTComma(ulTxDataUARTFileSize) );
				strcat( szSerialTxDispName, szLenTxt );
				
			}
			else if( TX_TYPE_RANDOM==sendTxDataType ) // 2018.03.23, Random number Generator 
			{
				srand( (unsigned)time(NULL) );
			}
			else if( TX_TYPE_SEQUENTIAL==sendTxDataType ) // 2018.03.26, Sequentialnumber Generator 
			{
			
			}

			ThreadWriteTXComm(TXDATA_BY_MENU);

			Sleep(100);

			
		#if 0 // 2016.04.03
			SetRegistry();
		#endif


		#if 0
			// Destroys a modal dialog box, causing the system to end any processing for the dialog box. 
			if( 0==EndDialog(hDlgTxData,1) )
			{
		    	// Error
			}
			if( hDlgTxData ) { CloseHandle(hDlgTxData); hDlgTxData=NULL; }
			if( _hDlgTxData2UART ) { CloseHandle( _hDlgTxData2UART ); _hDlgTxData2UART=NULL; }

		#endif
	
			return TRUE;


		case ID_TXDATA_STOP:
			// -------------------------------------------------------
			// Clear before exit
			// -------------------------------------------------------
			if( TX_TYPE_FILE==sendTxDataType || TX_TYPE_SCRIPT==sendTxDataType || TX_TYPE_RANDOM==sendTxDataType || TX_TYPE_SEQUENTIAL==sendTxDataType ) // txDATA File Type
			{
				int iLoopTxFileCnt = 0;

				isTxDataExit = 1;

				do {
					if(fpTxData) fseek(fpTxData, 0L, SEEK_END); // 2016.03.28 end of file pointer 
					else break;
					Sleep(TX_STOP_WAIT_TIME); // delay for fclose()
				} while(fpTxData && iLoopTxFileCnt++ < TXFILE_EXIT_COUNT);

				Sleep(TX_STOP_WAIT_TIME); // delay for fclose()
			}
			
			UserButtonTxData(_hDlgTxData2UART, 1);

			return TRUE;



		case ID_TXDATA_XOR:

			//if( TX_TYPE_HEXA==sendTxDataType )
			{
				TCHAR TxHexaData[MAX_PATH] = {0,} ;
				TCHAR TxSendData[3] = {0,};
				unsigned int len, icnt=0, iloop=0, hexval, xorValue=0x00;

					
				memset( TxHexaData, 0x00, sizeof(TxHexaData) );
				memset( TxSendData, 0x00, sizeof(TxSendData) );

				SetDlgItemText( hDlgTxData, IDC_TXDATA_TITLE_XOR, "[ XOR ]" ); 

				// -- Reading ----
				//The first WORD should be max character number for EM_GETLINE!
				*((WORD *)szSerialTxHexaData)=MAX_PATH-1;
				tmpLen = SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_HEXA), 
												EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szSerialTxHexaData);
				szSerialTxHexaData[tmpLen]='\0';
				// --------------------


				len = ExtractHexVal(szSerialTxHexaData, TxHexaData);
				
				
				for(iloop=0; len && (iloop<TX_DATA_LENGTH) && (TxHexaData[iloop]!=0x00); iloop+=SKIP_NUM )
				{
					memset( TxSendData, 0x00, sizeof(TxSendData) );
					strncpy( TxSendData, &TxHexaData[iloop], SKIP_NUM );
					hexval = str2hex(TxSendData);
				
					xorValue ^= hexval;
					
					icnt ++;
					if( txDataMsgOnOff ) EB_Printf(TEXT("[dnw] %2d: hexval(0x%02x) -> XOR:0x%02x \r\n"), icnt, hexval, xorValue ); 
				
				}
				if( txDataMsgOnOff ) EB_Printf(TEXT("[dnw] ----------------------------- \r\n") ); 
				//EB_Printf(TEXT("[dnw] Result XOR:0x%02X \r\n"), xorValue ); 

				SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_XOR),EM_SETSEL,0,-1); // all the text is selected. 
				SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_XOR),EM_REPLACESEL,0,(LPARAM)(LPCSTR)"");

				memset(szSerialTxHexaXOR, 0x00, sizeof(szSerialTxHexaXOR) );

				wsprintf( szSerialTxHexaXOR, TEXT(" %02X"), xorValue );
				
				SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_XOR),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szSerialTxHexaXOR);  

			}


			return TRUE;



		case ID_TXDATA_SUM:

			//if( TX_TYPE_HEXA==sendTxDataType )
			{
				
				TCHAR TxHexaData[MAX_PATH] = {0,} ;
				TCHAR TxSendData[3] = {0,};
				unsigned int len, icnt=0, iloop=0, hexval; // , xorValue=0x00;
				unsigned char sumValue=0x00;
				unsigned int  tmpValue=0x00;
				
				memset( TxHexaData, 0x00, sizeof(TxHexaData) );
				memset( TxSendData, 0x00, sizeof(TxSendData) );


				SetDlgItemText( hDlgTxData, IDC_TXDATA_TITLE_XOR, "[ SUM ]" ); 
				

				// -- Reading ----
				//The first WORD should be max character number for EM_GETLINE!
				*((WORD *)szSerialTxHexaData)=MAX_PATH-1;
				tmpLen = SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_HEXA), 
												EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szSerialTxHexaData);
				szSerialTxHexaData[tmpLen]='\0';
				// --------------------


				len = ExtractHexVal(szSerialTxHexaData, TxHexaData);
				
				
				for(iloop=0; len && (iloop<TX_DATA_LENGTH) && (TxHexaData[iloop]!=0x00); iloop+=SKIP_NUM )
				{
					memset( TxSendData, 0x00, sizeof(TxSendData) );
					strncpy( TxSendData, &TxHexaData[iloop], SKIP_NUM );
					hexval = str2hex(TxSendData);
				
					tmpValue += hexval;
					sumValue += hexval;
					
					icnt ++;
					if( txDataMsgOnOff ) EB_Printf(TEXT("[dnw] %2d: hexval(0x%02x) -> Sum:0x%02x -> 2's(0x%02x) \r\n"), icnt, hexval, tmpValue, (~(tmpValue&0xff)&0xff)+1 ); 
				
				}
				if( txDataMsgOnOff ) EB_Printf(TEXT("[dnw] ------------------------------------------ \r\n") ); 
				//EB_Printf(TEXT("[dnw] Result XOR:0x%02X \r\n"), xorValue ); 

				SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_XOR),EM_SETSEL,0,-1); // all the text is selected. 
				SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_XOR),EM_REPLACESEL,0,(LPARAM)(LPCSTR)"");

				memset(szSerialTxHexaXOR, 0x00, sizeof(szSerialTxHexaXOR) );

				wsprintf( szSerialTxHexaXOR, TEXT(" %02X"), sumValue );
				
				SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_XOR),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szSerialTxHexaXOR);  

			}

			return TRUE;


		case ID_TXDATA_SAVE:

			EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_SAVE ), 0 );

			// ----------------------------------------------------------------------------
			//get MaxFileSize
			*((WORD *)szSerialTxCharData)=MAX_PATH-1;
			//The first WORD should be max character number for EM_GETLINE!
			tmpLen = SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_CHAR), 
											EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szSerialTxCharData);
			szSerialTxCharData[tmpLen]='\0';
			// ----------------------------------------------------------------------------

			// ----------------------------------------------------------------------------
			//get MaxFileSize
			*((WORD *)szSerialTxHexaData)=MAX_PATH-1;
			//The first WORD should be max character number for EM_GETLINE!
			tmpLen = SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_HEXA), 
											EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szSerialTxHexaData);
			szSerialTxHexaData[tmpLen]='\0';
			// ----------------------------------------------------------------------------


			// ----------------------------------------------------------------------------
			//get MaxFileSize
			*((WORD *)szSerialTxFileData)=MAX_PATH-1;
			//The first WORD should be max character number for EM_GETLINE!
			tmpLen = SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_FILE), 
											EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szSerialTxFileData);
			szSerialTxFileData[tmpLen]='\0';
	
			// ----------------------------------------------------------------------------
			// -----------------------------------------------------------------
			// 2. Display Filename ---------------------------------------------
			idx = strlen(szSerialTxFileData);
			if( idx >= MAX_PATH )
			{
				EB_Printf(TEXT("\r\n[dnw] Input file length is over!! [%s] length(%d) \r\n"), szSerialTxFileData, idx );
				BeepSound(4);
			}
			
			isFind=0;
			for(jdx=idx; idx>0; jdx--)
			{
				if( szSerialTxFileData[jdx] == '\\' ) { isFind=1; break; }
			}
			strcpy( szSerialTxDispName, &szSerialTxFileData[jdx+1] );
			// -----------------------------------------------------------------


		#if 1 // 2016.04.03
			SetRegistry();
		#else
			ThreadSetRegistry(NULL);
		#endif


			if( 0==isRegSave ) // Save Finished!!!
			{
				EnableWindow( GetDlgItem(_hDlgTxData2UART, ID_TXDATA_SAVE ), 1 ); // Enabled!!
			}

			return TRUE;


		case ID_TXDATA_BTN_FILE:

			MenuTxDataSelect(hDlgTxData);
			// ~~ if( MenuTxDataSelect(hDlgTxData) )
			{
				SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_FILE),EM_SETSEL,0,-1); // all the text is selected. 
				SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_FILE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)"");

				SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_DISP_FILE),EM_SETSEL,0,-1); // all the text is selected. 
				SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_DISP_FILE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)"");
			}

			SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_FILE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szSerialTxFileData);


			// ----------------------------------------------------------------------------
			// -----------------------------------------------------------------
			// 2. Display Filename ---------------------------------------------
			idx = strlen(szSerialTxFileData);
			if( idx >= MAX_PATH || idx<=0 )
			{
				EB_Printf(TEXT("[dnw] Input file is wrong!! length(%d) \r\n"), idx );

				UserButtonTxData(_hDlgTxData2UART, 1);
				BeepSound(4);
				return -1; // 2017.07.31
			}
			
			isFind=0;
			for(jdx=idx; idx>0; jdx--)
			{
				if( szSerialTxFileData[jdx] == '\\' ) { isFind=1; break; }
			}
			strcpy( szSerialTxDispName, &szSerialTxFileData[jdx+1] );
			// -----------------------------------------------------------------

			hFile = CreateFile(szSerialTxFileData,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
			if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
			{
				EB_Printf(TEXT("[dnw] can not open TxData file[%s]. Error(%u) \r\n"), szSerialTxDispName, GetLastError() );
				UserButtonTxData(_hDlgTxData2UART, 1);
				BeepSound(4);

				return -1;
			}

		#if 0 // 2016.08.12
			ulTxDataUARTFileSize = GetFileSize(hFile,NULL);
		#else
			ulTxDataUARTFileSize = GetTxDataFileSize(hFile);
		#endif

			if(hFile)  { CloseHandle(hFile);  hFile=NULL; }
			
			//wsprintf(szLenTxt, TEXT(" == (%I64d Byte)"), ulTxDataUARTFileSize);
			wsprintf(szLenTxt, TEXT(" == (%s Byte)"), PRTComma(ulTxDataUARTFileSize) );
			strcat( szSerialTxDispName, szLenTxt );
			// -----------------------------------------------------------------


			SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_DISP_FILE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szSerialTxDispName);

			return TRUE;


		case ID_TXDATA_EDIT_CHAR:
	#if 0
			{
			int iLength=0, lineIdx=0, txtlEnd;
			TCHAR TxCharData[4][MAX_PATH] = {0,};
			
			// ----------------------------------------------------------------------------
			//get MaxFileSize
			*((WORD *)szSerialTxCharData)=MAX_PATH-1;
			*((WORD *)TxCharData[0])=MAX_PATH-1;
			*((WORD *)TxCharData[1])=MAX_PATH-1;
			*((WORD *)TxCharData[2])=MAX_PATH-1;
			*((WORD *)TxCharData[3])=MAX_PATH-1;
			//The first WORD should be max character number for EM_GETLINE!
			
			lineIdx = SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_CHAR),EM_LINEFROMCHAR,MAX_PATH-1,0x0);
			txtlEnd = SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_CHAR),EM_LINEINDEX,lineIdx,0x0)-1;
			iLength = SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_CHAR),WM_GETTEXTLENGTH,0x0,0x0)-1;

			tmpLen = SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_CHAR), 
											EM_GETLINE,(WPARAM)lineIdx,(LPARAM)(LPCSTR)&TxCharData[lineIdx]);
			if( 0==lineIdx )
				lstrcpy(szSerialTxCharData, TxCharData[0] );
			else if( 1==lineIdx )
				lstrcat(szSerialTxCharData, TxCharData[1] );
			else if( 2==lineIdx )
				lstrcat(szSerialTxCharData, TxCharData[2] );
			else if( 3==lineIdx )
				lstrcat(szSerialTxCharData, TxCharData[3] );

			szSerialTxCharData[iLength]='\0';

			// ----------------------------------------------------------------------------
			EB_Printf(TEXT(" ID_TXDATA_EDIT_CHAR ++++  tmpLen (%d) %d %d %d \r\n"), tmpLen, lineIdx, txtlEnd, iLength );

			//EB_Printf(TEXT(" 0[%s] \r\n"), TxCharData[0] );
			//EB_Printf(TEXT(" 1[%s] \r\n"), TxCharData[1] );
			//EB_Printf(TEXT(" 2[%s] \r\n"), TxCharData[2] );
			//EB_Printf(TEXT(" 3[%s] \r\n"), TxCharData[3] );
		
			EB_Printf(TEXT(" T[%s] \r\n"), szSerialTxCharData );
		

			}
	#endif

			return TRUE;
		
		
		case ID_TXDATA_EDIT_HEXA:
			//EB_Printf(TEXT(" T[---] \r\n")  );
			return TRUE;


		case ID_TXDATA_CLEAR_SCR:
			ScrClear(_EditHwnd);

		#if 0
			if( 0==sendTxDataType ) // Char Type
			{
				memset( szSerialTxCharData, 0x00, sizeof(szSerialTxCharData) );
				szSerialTxCharData[MAX_PATH-1]='\0';

				SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_CHAR),EM_SETSEL,0,-1); // all the text is selected. 
				SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_CHAR),EM_REPLACESEL,0,(LPARAM)(LPCSTR)"");

			}
			else if( 1==sendTxDataType ) // Hexa Type
			{
				memset( szSerialTxHexaData, 0x00, sizeof(szSerialTxHexaData) );
				szSerialTxHexaData[MAX_PATH-1]='\0';

				SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_HEXA),EM_SETSEL,0,-1); // all the text is selected. 
				SendMessage(GetDlgItem(hDlgTxData,ID_TXDATA_EDIT_HEXA),EM_REPLACESEL,0,(LPARAM)(LPCSTR)"");
			}
			else if( 2==sendTxDataType ) // File Type
			{
			 	// by MENU ony. NOT F6-key
			}
		#endif
			return TRUE;

								
		case ID_TXDATA_CHAR_MODE:
		case ID_TXDATA_HEXA_MODE:
		case ID_TXDATA_FILE_MODE:
		case ID_TXDATA_SCRI_MODE:
		case ID_TXDATA_RAND_MODE: // 2018.03.23
		case ID_TXDATA_SEQU_MODE: // 2018.03.26
		
			// Message On/Off 
			sendTxDataType = tempTxDataType = LOWORD(wParam)-ID_TXDATA_CHAR_MODE;

			UserButtonTxData(_hDlgTxData2UART, 1);
			return TRUE;

		case ID_TXDATA_DISPLAY_MSGOFF:
		case ID_TXDATA_DISPLAY_MSGON:
			// Message On/Off 
			txDataMsgOnOff = tempMsgOnOff = LOWORD(wParam)-ID_TXDATA_DISPLAY_MSGOFF;
		
			return TRUE;

		default:
			return FALSE;


		} // case WM_COMMAND:
		break;
		
#if 0
	case WM_SETFOCUS:
		SetFocus( hDlgTxData );
		EB_Printf( TEXT("[dnw] WM_SETFOCUS in cbTxData2UART ---\r\n") );
		return TRUE;
#endif


	default:
		return FALSE;
	}

	return FALSE;
}


int DelSpecialChar(char *cch, int iStartPos, int iLen)
{
	int iloop=0, irep=0;
	
	for(iloop=iStartPos /*7*/; iloop<iLen && iloop<TX_SCRIPT_SIZ; iloop++)
	{
		if( ','==cch[iloop] ) 
		{
			for(irep = iloop; irep<iLen && irep<TX_SCRIPT_SIZ; irep++)
				cch[irep] = cch[irep+1];
		}
		else if( ';'==cch[iloop] || ':'==cch[iloop] || '.'==cch[iloop] || '/'==cch[iloop] || '-'==cch[iloop] 
			|| '\t'==cch[iloop] || '\b'==cch[iloop] || '='==cch[iloop] )
		{
			cch[iloop] = '\0';
			return 1;
			// break;
		}
	}
	return 0;
}


int TxScriptParser(char *szScriptText)
{
	// -- script command --
	// [START]
	// [END]
	// [HEXA] 20 aa ff aa
	// [CHAR] abcdefg ghhhhh
	// [SLEEP] 1000 msec
	// [DO]
	// [WHILE] 50
	// [RANDOM] 100000 ; 0x00 ~ 0xFF
	// [SEQNUM] 100000 ; 0x00 ~ 0xFF

	     if( 0==strncmp( szScriptText, ("[START]"), 7 ) ) return SCRIPT_START; // start
	else if( 0==strncmp( szScriptText, ("[END]"), 5 ) )   return SCRIPT_END;  // ended
	else if( 0==strncmp( szScriptText, ("[HEXA]"), 6 ) )  return SCRIPT_HEXA;
	else if( 0==strncmp( szScriptText, ("[CHAR]"), 6 ) )  return SCRIPT_CHAR;
	else if( 0==strncmp( szScriptText, ("[SLEEP]"), 7 ) ) return SCRIPT_SLEEP;
	else if( 0==strncmp( szScriptText, ("[DO]"), 4 ) )    return SCRIPT_DO_START;
	else if( 0==strncmp( szScriptText, ("[WHILE]"), 7 ) ) return SCRIPT_WHILE_END;
	else if( 0==strncmp( szScriptText, ("[RANDOM]"), 8 ) ) return SCRIPT_RANDOM_GEN;
	else if( 0==strncmp( szScriptText, ("[SEQNUM]"), 8 ) ) return SCRIPT_SEQNUM_GEN;
	else if( 0==strncmp( szScriptText, ("[SEC]"), 5 ) ) return SCRIPT_SEC;
	else if( 0==strncmp( szScriptText, ("[MSEC]"), 6 ) ) return SCRIPT_MSEC;

	else if( 0==strncmp( szScriptText, ("[start]"), 7 ) ) return SCRIPT_START;
	else if( 0==strncmp( szScriptText, ("[end]"), 5 ) )   return SCRIPT_END;
	else if( 0==strncmp( szScriptText, ("[hexa]"), 6 ) )  return SCRIPT_HEXA;
	else if( 0==strncmp( szScriptText, ("[char]"), 6 ) )  return SCRIPT_CHAR;
	else if( 0==strncmp( szScriptText, ("[sleep]"), 7 ) ) return SCRIPT_SLEEP;
	else if( 0==strncmp( szScriptText, ("[do]"), 4 ) )    return SCRIPT_DO_START;
	else if( 0==strncmp( szScriptText, ("[while]"), 7 ) ) return SCRIPT_WHILE_END;
	else if( 0==strncmp( szScriptText, ("[sec]"), 5 ) ) return SCRIPT_SEC;
	else if( 0==strncmp( szScriptText, ("[msec]"), 6 ) ) return SCRIPT_MSEC;

	return SCRIPT_NONE;
	
}


#endif


#if USE_FLOAT2HEX_VALUE // 2018.07.18

/// --float / double to hex --------------------------
typedef union {
	float		   fv; 	
	unsigned char  fb[sizeof(float)]; 
} __float_hex_union; /// = { 0.866f };	 


typedef union {
	unsigned char  di[sizeof(double)]; 
	//long long      di;
	double         dd;
 } __double_hex_union;

/// --float / double to hex --------------------------



void EnableButtonFlt2Hex(int onoff)
{
	EnableWindow( GetDlgItem(_hDlgFlt2Hexa, IDOK ), onoff );
	EnableWindow( GetDlgItem(_hDlgFlt2Hexa, ID_FLT2HEX_FLT2HEX ), onoff );
	EnableWindow( GetDlgItem(_hDlgFlt2Hexa, ID_FLT2HEX_DBL2HEX ), onoff );
	EnableWindow( GetDlgItem(_hDlgFlt2Hexa, ID_FLT2HEX_AUTO_CREATE ), onoff );
	EnableWindow( GetDlgItem(_hDlgFlt2Hexa, ID_FLT2HEX_EDIT_IN1_VALUE ), onoff );
	EnableWindow( GetDlgItem(_hDlgFlt2Hexa, ID_FLT2HEX_EDIT_IN2_VALUE ), onoff );
	EnableWindow( GetDlgItem(_hDlgFlt2Hexa, ID_FLT2HEX_EDIT_IN3_VALUE ), onoff );

	EnableWindow( GetDlgItem(_hDlgFlt2Hexa, ID_FLT2HEX_TXT_IN2 ), onoff );
	EnableWindow( GetDlgItem(_hDlgFlt2Hexa, ID_FLT2HEX_TXT_IN3 ), onoff );

	EnableWindow( GetDlgItem(_hDlgFlt2Hexa, ID_FLT2HEX_LITTLE ), onoff );
	EnableWindow( GetDlgItem(_hDlgFlt2Hexa, ID_FLT2HEX_BIG ), onoff );

	EnableWindow( GetDlgItem(_hDlgFlt2Hexa, ID_F2H_TYPE_TEXT ), onoff );
	EnableWindow( GetDlgItem(_hDlgFlt2Hexa, ID_F2H_ENDIAN_TEXT ), onoff );
	EnableWindow( GetDlgItem(_hDlgFlt2Hexa, ID_F2H_INPUT_TEXT ), onoff );
	EnableWindow( GetDlgItem(_hDlgFlt2Hexa, ID_F2H_OUTPUT_TEXT ), onoff );
}

unsigned int CalcFlt2Hex(void *args)
{
	unsigned int iloop=0;
	int isHotKey = 0; // by F6-key(0) or by MENU(1)
	HANDLE hFile = NULL;
	TCHAR  szTitle[MAX_PATH] = {0,};
	
	int result1=0, result2=0;
	float inFltValue=0.0;
	float inFltValue1=0.0;
	float inFltValue2=0.0;
	float inFltValue3=0.0;
	__float_hex_union    flthex;

	double inDblValue=0.0;
	double inDblValue1=0.0;
	double inDblValue2=0.0;
	double inDblValue3=0.0;
	__double_hex_union	 dblhex;
	TCHAR rstFlt2Hex[100] = {0,};
	TCHAR rstDbl2Hex[100] = {0,};


	EnableButtonFlt2Hex(0);


#if 0
	flthex.fb[3] = 0x41;
	flthex.fb[2] = 0x09; 
	flthex.fb[1] = 0x00;
	flthex.fb[0] = 0x22;
	EB_Printf(TEXT("[dnw] 41 09 00 22 -> %lf \r\n"), flthex.fv);

	flthex.fb[0] = 0x41;
	flthex.fb[1] = 0x09; 
	flthex.fb[2] = 0x00;
	flthex.fb[3] = 0x22;
	EB_Printf(TEXT("[dnw] 22 00 09 41-> %lf \r\n"), flthex.fv);
#endif


	
	// 1st INPUT
	result1 = sscanf(szFlt2Hex_In1, "%f", &inFltValue1);
	result2 = sscanf(szDbl2Hex_In1, "%lf", &inDblValue1);

	if (result1 != 1) 
	{
		EB_Printf(TEXT("[dnw] Error float Value1 [%s] (%f) (%lf) \r\n"), szFlt2Hex_In1, inFltValue1, inDblValue1 );
	}
	else
	{
	
		flthex.fv = inFltValue1;
		memset( rstFlt2Hex, 0x00, sizeof(rstFlt2Hex) );

		dblhex.dd = inDblValue1;
		memset( rstDbl2Hex, 0x00, sizeof(rstDbl2Hex) );

		if( F2H_LITTLE_ENDIAN==iFlt2HexEndian ) // Little Endian
		{
			// LSB
			wsprintf(rstFlt2Hex, TEXT("%02X-%02X-%02X-%02X"), flthex.fb[0], flthex.fb[1], flthex.fb[2], flthex.fb[3]);
			//EB_Printf(TEXT("[dnw] float (%f) -> Little hexa [%s] \r\n"), flthex.fv, rstFlt2Hex );
		}
		else if( F2H_BIG_ENDIAN==iFlt2HexEndian ) // Big Endian
		{
			// MSB
			wsprintf(rstFlt2Hex, TEXT("%02X-%02X-%02X-%02X"), flthex.fb[3], flthex.fb[2], flthex.fb[1], flthex.fb[0]);
			//EB_Printf(TEXT("[dnw] float (%f) -> Big hexa [%s] \r\n"), flthex.fv, rstFlt2Hex );
		}
					
		// ----------------------------------------------------------------------------
		// 2. Display Filename --------------------------------------------------------
		lstrcpy(szFlt2Hex_Out,	rstFlt2Hex);
		szFlt2Hex_Out[MAX_PATH-1]='\0';
		SendMessage(GetDlgItem(_hDlgFlt2Hexa,ID_FLT2HEX_EDIT_OUT_VALUE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szFlt2Hex_Out);  
		// ----------------------------------------------------------------------------
		
	
	
		// 2nd INPUT
		result1 = sscanf(szFlt2Hex_In2, "%f", &inFltValue2);
		result2 = sscanf(szDbl2Hex_In2, "%lf", &inDblValue2);
		if (result1 != 1) 
		{
			EB_Printf(TEXT("[dnw] Error float Value2 [%s] (%f) (%lf) \r\n"), szFlt2Hex_In2, inFltValue2, inDblValue2 );
		}
		else
		{
	
			// 3rd INPUT
			result1 = sscanf(szFlt2Hex_In3, "%f", &inFltValue3);
			result2 = sscanf(szDbl2Hex_In3, "%lf", &inDblValue3);

			if (result1 != 1) 
			{
				EB_Printf(TEXT("[dnw] Error float Value3 [%s] (%f) (%lf) \r\n"), szFlt2Hex_In3, inFltValue3, inDblValue3 );
			}
			else
			{
			
				memset( rstFlt2Hex, 0x00, sizeof(rstFlt2Hex) );
				memset( rstDbl2Hex, 0x00, sizeof(rstDbl2Hex) );

				if(inFltValue3<=0.1) { inFltValue2 += inFltValue3; }
				if(inDblValue3<=0.1) { inDblValue2 += inDblValue3; }
				
				if(1!=isFlt2HexStop)
					EB_Printf(TEXT("[dnw] ----- float value to hexa --------------------------------- \r\n") );

				for(inFltValue=inFltValue1; (1!=isFlt2HexStop) && inFltValue<=inFltValue2; inFltValue+=inFltValue3)
				{
			
					flthex.fv  = inFltValue;  // float

					if( F2H_LITTLE_ENDIAN==iFlt2HexEndian ) // Little Endian
					{
						// LSB
						wsprintf(rstFlt2Hex, TEXT("%02X-%02X-%02X-%02X"), flthex.fb[0], flthex.fb[1], flthex.fb[2], flthex.fb[3]);

						EB_Printf(TEXT("[dnw] float (%f) -> Little hexa [%s] \r\n"), flthex.fv, rstFlt2Hex  );
					}
					else if( F2H_BIG_ENDIAN==iFlt2HexEndian ) // Big Endian
					{
						// MSB
						wsprintf(rstFlt2Hex, TEXT("%02X-%02X-%02X-%02X"), flthex.fb[3], flthex.fb[2], flthex.fb[1], flthex.fb[0]);

						EB_Printf(TEXT("[dnw] float (%f) -> Big hexa [%s] \r\n"), flthex.fv, rstFlt2Hex );
					}

				}

				if(1!=isFlt2HexStop)
					EB_Printf(TEXT("[dnw] ----- double value to hexa -------------------------------- \r\n") );

				for(inDblValue=inDblValue1; (1!=isFlt2HexStop) && inDblValue<=inDblValue2; inDblValue+=inDblValue3)
				{
				
					dblhex.dd   = inDblValue; // double
				
					if( F2H_LITTLE_ENDIAN==iFlt2HexEndian ) // Little Endian
					{
						// LSB
						wsprintf(rstDbl2Hex, TEXT("%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X"), 
							dblhex.di[0], dblhex.di[1], dblhex.di[2], dblhex.di[3], dblhex.di[4], dblhex.di[5], dblhex.di[6], dblhex.di[7]);
				
						EB_Printf(TEXT("[dnw] double (%lf) -> Little hexa [%s] \r\n"), dblhex.dd, rstDbl2Hex );
					}
					else if( F2H_BIG_ENDIAN==iFlt2HexEndian ) // Big Endian
					{
						// MSB
						wsprintf(rstDbl2Hex, TEXT("%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X"), 
							dblhex.di[7], dblhex.di[6], dblhex.di[5], dblhex.di[4], dblhex.di[3], dblhex.di[2], dblhex.di[1], dblhex.di[0]);

						EB_Printf(TEXT("[dnw] double (%lf) -> Big hexa [%s] \r\n"), dblhex.dd, rstDbl2Hex );
					}

				}

				
			}
		
		}
		if(isFlt2HexStop)
			EB_Printf(TEXT("[dnw] User break!! \r\n\r\n"));

	}


	EnableButtonFlt2Hex(1);

	/* _endthread given to terminate */
	_endthread();

	return iloop;
}


BOOL ThreadCalcFlt2Hex(void *args)
{
	uintptr_t threadResult;


	threadResult = _beginthread( (void (*)(void *))CalcFlt2Hex, THREAD_STACK_SIZE, (void *)args );
	if(threadResult!=-1)
	{
		//isFlt2HexStop = 0;
		
		//Sleep(10);
	}
	else
	{
		EB_Printf("[dnw] can not create CalcFlt2Hex thread! (Result=%d, Err:%u) \r\n", threadResult, GetLastError());
		return FALSE;
	}
	return TRUE;
}

#define FLOAT_MIN_VALIE 		1.175494351E-38 
#define FLOAT_MAX_VALIE 		3.402823466E38 
#define DOUBLE_MIN_VALIE 		2.2250738585072014E-308 
#define DOUBLE_MAX_VALIE 		1.7976931348623158E308 


BOOL CALLBACK cbFloat2Hexa(HWND hDlgFlt2Hex, UINT message, WPARAM wParam, LPARAM lParam)
{
	WORD pmId;
	WORD wmEvent, wmId;
	int tmpLen=0;
	int tmpLen1=0, tmpLen2=0, tmpLen3=0;
	int idx=0, jdx=0, isFind=0;
	HANDLE hFile = NULL;
	TCHAR szLenTxt[MAX_PATH] = {0,};
	float inFltValue=0.0;
	double inDblValue=0.0;

	char inBinValue[4] = {0,};
	int result=0;
	__float_hex_union    flthex;
	__double_hex_union   dblhex;
	TCHAR rstFlt2Hex[100] = {0,};

	

	pmId	  = LOWORD(lParam);

	wmEvent   = HIWORD(wParam);
	wmId	  = LOWORD(wParam); 

	switch(message)
	{
	case WM_INITDIALOG:

		_hDlgFlt2Hexa	  = hDlgFlt2Hex;

		isFlt2HexStop     = 0; // start

		// Message On/Off 
		tempFlt2HexEndian = iFlt2HexEndian;
		CheckRadioButton(hDlgFlt2Hex, ID_FLT2HEX_LITTLE, ID_FLT2HEX_BIG, ID_FLT2HEX_LITTLE+tempFlt2HexEndian);

		// Tx Data Type 
		tempChoiceFlt2Hex = iChoiceFlt2Hex;
		CheckRadioButton(hDlgFlt2Hex, ID_FLT2HEX_FLT2HEX, ID_FLT2HEX_AUTO_CREATE, ID_FLT2HEX_FLT2HEX+tempChoiceFlt2Hex);

		SetWindowText(_hDlgFlt2Hexa, APPTITLE TITLE_FLT2HEXA); // update title



		if(F2H_FLOAT_TYPE==iChoiceFlt2Hex || F2H_DOUBLE_TYPE==iChoiceFlt2Hex)
		{
			EnableWindow( GetDlgItem(hDlgFlt2Hex, ID_FLT2HEX_EDIT_IN2_VALUE ), 0 );
			EnableWindow( GetDlgItem(hDlgFlt2Hex, ID_FLT2HEX_EDIT_IN3_VALUE ), 0 );
			EnableWindow( GetDlgItem(hDlgFlt2Hex, ID_FLT2HEX_TXT_IN2 ), 0 );
			EnableWindow( GetDlgItem(hDlgFlt2Hex, ID_FLT2HEX_TXT_IN3 ), 0 );
		}
		else
		{
			EnableWindow( GetDlgItem(hDlgFlt2Hex, ID_FLT2HEX_EDIT_IN2_VALUE ), 1 );
			EnableWindow( GetDlgItem(hDlgFlt2Hex, ID_FLT2HEX_EDIT_IN3_VALUE ), 1 );
			EnableWindow( GetDlgItem(hDlgFlt2Hex, ID_FLT2HEX_TXT_IN2 ), 1 );
			EnableWindow( GetDlgItem(hDlgFlt2Hex, ID_FLT2HEX_TXT_IN3 ), 1 );
		}



		// screen input1 buffer clear
		SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_IN1_VALUE),EM_SETSEL,0,-1); // all the text is selected. 
		SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_IN1_VALUE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)"");	

		// screen input1 buffer clear
		SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_IN2_VALUE),EM_SETSEL,0,-1); // all the text is selected. 
		SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_IN2_VALUE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)"");	

		// screen input1 buffer clear
		SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_IN3_VALUE),EM_SETSEL,0,-1); // all the text is selected. 
		SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_IN3_VALUE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)"");	
	
		// screen buffer clear
		SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_OUT_VALUE),EM_SETSEL,0,-1); // all the text is selected. 
		SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_OUT_VALUE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)"");	


		// input file
		szFlt2Hex_In1[MAX_PATH-1]='\0';
		SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_IN1_VALUE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szFlt2Hex_In1);  

		// 2nd input 
		szFlt2Hex_In2[MAX_PATH-1]='\0';
		SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_IN2_VALUE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szFlt2Hex_In2);  

		// step input
		szFlt2Hex_In3[MAX_PATH-1]='\0';
		SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_IN3_VALUE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szFlt2Hex_In3);  

		// double type
		lstrcpy(szDbl2Hex_In1, szFlt2Hex_In1);
		lstrcpy(szDbl2Hex_In2, szFlt2Hex_In2);
		lstrcpy(szDbl2Hex_In3, szFlt2Hex_In3);


		

		szFlt2Hex_Out[MAX_PATH-1]='\0';
		SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_OUT_VALUE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szFlt2Hex_Out);  
		EnableWindow( GetDlgItem(hDlgFlt2Hex, ID_FLT2HEX_EDIT_OUT_VALUE ), 0 );


	#if 0
		//---- get ------------------------------------
		*((WORD *)szFlt2Hex_In1)=MAX_PATH-1;
		//The first WORD should be max character number for EM_GETLINE!
		tmpLen1 = SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_IN1_VALUE), 
										EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szFlt2Hex_In1);
		// ----------------------------------------------------------------------------

		//---- get ------------------------------------
		*((WORD *)szFlt2Hex_In2)=MAX_PATH-1;
		//The first WORD should be max character number for EM_GETLINE!
		tmpLen2 = SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_IN2_VALUE), 
										EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szFlt2Hex_In2);
		// ----------------------------------------------------------------------------

		//---- get ------------------------------------
		*((WORD *)szFlt2Hex_In3)=MAX_PATH-1;
		//The first WORD should be max character number for EM_GETLINE!
		tmpLen3 = SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_IN3_VALUE), 
										EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szFlt2Hex_In3);
		// ----------------------------------------------------------------------------
	#endif
		
		// -----------------------------------------------------------------
		// 2. Display Filename ---------------------------------------------
		idx = strlen(szFlt2Hex_In1);

		if( idx <= 0 ) // 2017.07.28
		{
			// script or Tx file is Not exist!!!
			//EB_Printf(TEXT("\r\n[dnw] cbFloat2Hexa Input is NOT exist!! length(%d) \r\n"), idx );
			BeepSound(4);
			return TRUE;
		}
		else if( idx >= MAX_PATH-1 )
		{
			EB_Printf(TEXT("\r\n[dnw] cbFloat2Hexa Input file length is over!!! length(%d) \r\n"), idx );
			BeepSound(4);
			return TRUE;
		}

	#if 1 // added , same to [OK]
		result = sscanf(szFlt2Hex_In1, "%f", &inFltValue);
		if (result != 1) 
		{
			EB_Printf(TEXT("[dnw] Error float Value [%s] (%f) -> (%d, %u)\r\n"), szFlt2Hex_In1, inFltValue, result, GetLastError() );
		}
		else
		{
			memset( rstFlt2Hex, 0x00, sizeof(rstFlt2Hex) );

			flthex.fv = inFltValue;


			if( F2H_LITTLE_ENDIAN==iFlt2HexEndian ) // Little Endian
			{
				// LSB
				wsprintf(rstFlt2Hex, TEXT("%02X-%02X-%02X-%02X"), flthex.fb[0], flthex.fb[1], flthex.fb[2], flthex.fb[3] );
				EB_Printf(TEXT("[dnw] float (%f) -> Little hexa [%s] \r\n"), flthex.fv, rstFlt2Hex );
			}
			else if( F2H_BIG_ENDIAN==iFlt2HexEndian ) // Big Endian
			{
				// MSB
				wsprintf(rstFlt2Hex, TEXT("%02X-%02X-%02X-%02X"), flthex.fb[3], flthex.fb[2], flthex.fb[1], flthex.fb[0] );
				EB_Printf(TEXT("[dnw] float (%f) -> Big hexa [%s] \r\n"), flthex.fv, rstFlt2Hex );
			}
		}

		// ----------------------------------------------------------------------------
		
		// ----------------------------------------------------------------------------
		// 2. Display Filename --------------------------------------------------------
		idx = strlen(szFlt2Hex_Out);
	
		if( idx > 0 ) // 2018.07.19
		{
			// Screen Buffer Clear
			SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_OUT_VALUE),EM_SETSEL,0,-1); // all the text is selected. 
			SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_OUT_VALUE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)"");	
		}

		lstrcpy(szFlt2Hex_Out,	rstFlt2Hex);
		szFlt2Hex_Out[MAX_PATH-1]='\0';
		SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_OUT_VALUE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szFlt2Hex_Out);  
		// ----------------------------------------------------------------------------
	#endif


		return TRUE;



	case WM_COMMAND:

		wmEvent = HIWORD(wParam);
		wmId	= LOWORD(wParam); 

		switch(LOWORD(wParam))
		{

		case IDCANCEL: // Exit!!

			isFlt2HexStop = 1; // stop

			SetRegistry();

			if( 0==EndDialog(hDlgFlt2Hex,0) )
			{
				// Error 
			}
			
			if( hDlgFlt2Hex ) { CloseHandle(hDlgFlt2Hex); hDlgFlt2Hex=NULL; }
			if( _hDlgFlt2Hexa ) { CloseHandle( _hDlgFlt2Hexa ); _hDlgFlt2Hexa=NULL; }

			return TRUE;


		case IDOK:

			isFlt2HexStop	  = 0; // start

			// ----------------------------------------------------------------------------
			//get ------------
			*((WORD *)szFlt2Hex_In1)=MAX_PATH-1;
			//The first WORD should be max character number for EM_GETLINE!
			tmpLen1 = SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_IN1_VALUE), 
											EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szFlt2Hex_In1);
			szFlt2Hex_In1[tmpLen1]='\0';
			// -------------------------------------------------------------------------------
			//get ------------
			*((WORD *)szFlt2Hex_In2)=MAX_PATH-1;
			//The first WORD should be max character number for EM_GETLINE!
			tmpLen2 = SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_IN2_VALUE), 
											EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szFlt2Hex_In2);
			szFlt2Hex_In2[tmpLen2]='\0';
			// -------------------------------------------------------------------------------
			//get ------------
			*((WORD *)szFlt2Hex_In3)=MAX_PATH-1;
			//The first WORD should be max character number for EM_GETLINE!
			tmpLen3 = SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_IN3_VALUE), 
											EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szFlt2Hex_In3);
			szFlt2Hex_In3[tmpLen3]='\0';
			// -------------------------------------------------------------------------------

			lstrcpy(szDbl2Hex_In1, szFlt2Hex_In1);
			lstrcpy(szDbl2Hex_In2, szFlt2Hex_In2);
			lstrcpy(szDbl2Hex_In3, szFlt2Hex_In3);
			

			
			//idx = strlen(szFlt2Hex_In);

			// clear buffer
			// *((WORD *)szFlt2Hex_Out)=15;
			memset(szFlt2Hex_Out, 0x00, sizeof(szFlt2Hex_Out) );
			szFlt2Hex_Out[MAX_PATH-1]='\0';

			// screen buffer clear
			SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_OUT_VALUE),EM_SETSEL,0,-1); // all the text is selected. 
			SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_OUT_VALUE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)"");  



			switch(iChoiceFlt2Hex)
			{

				case F2H_FLOAT_TYPE: // Float 2 hex
					result = sscanf(szFlt2Hex_In1, "%f", &inFltValue);
					if (result != 1) 
					{
						EB_Printf(TEXT("[dnw] Error float Value [%s] (%f), Error(%u, %d) \r\n"), szFlt2Hex_In1, inFltValue, GetLastError(), result );
					}
					else
					{
						memset( rstFlt2Hex, 0x00, sizeof(rstFlt2Hex) );

						flthex.fv = inFltValue;


			#if 0
						if( F2H_FLOAT_TYPE==iChoiceFlt2Hex && (flthex.fv > FLOAT_MAX_VALIE || flthex.fv < FLOAT_MIN_VALIE) )
						{
							EB_Printf(TEXT("[dnw] Out of range error float Value [%s] (%f)\r\n"), szFlt2Hex_In1, inFltValue );
							BeepSound(4);
							return TRUE;
						}
			#endif

						if( F2H_LITTLE_ENDIAN==iFlt2HexEndian ) // Little Endian
						{
							// LSB
							wsprintf(rstFlt2Hex, TEXT("%02X-%02X-%02X-%02X"), flthex.fb[0], flthex.fb[1], flthex.fb[2], flthex.fb[3] );
							EB_Printf(TEXT("[dnw] float (%f) -> Little hexa [%s] \r\n"), flthex.fv, rstFlt2Hex );
						}
						else if( F2H_BIG_ENDIAN==iFlt2HexEndian ) // Big Endian
						{
							// MSB
							wsprintf(rstFlt2Hex, TEXT("%02X-%02X-%02X-%02X"), flthex.fb[3], flthex.fb[2], flthex.fb[1], flthex.fb[0] );
							EB_Printf(TEXT("[dnw] float (%f) -> Big hexa [%s] \r\n"), flthex.fv, rstFlt2Hex );
						}
						


					}

					// ----------------------------------------------------------------------------
					
					// ----------------------------------------------------------------------------
					// 2. Display Filename --------------------------------------------------------
					
					lstrcpy(szFlt2Hex_Out,	rstFlt2Hex);
					szFlt2Hex_Out[MAX_PATH-1]='\0';
					SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_OUT_VALUE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szFlt2Hex_Out);  
					// ----------------------------------------------------------------------------

					break;
					

				case F2H_DOUBLE_TYPE:

					result = sscanf(szDbl2Hex_In1, "%lf", &inDblValue);
					if (result != 1) 
					{
						EB_Printf(TEXT("[dnw] Error float Value [%s] (%f), Error(%u, %d) \r\n"), szDbl2Hex_In1, inDblValue, GetLastError(), result );
					}
					else
					{
						memset( rstFlt2Hex, 0x00, sizeof(rstFlt2Hex) );

						dblhex.dd = inDblValue;


			#if 0
						if( F2H_DOUBLE_TYPE==iChoiceFlt2Hex && (dblhex.dd > DOUBLE_MAX_VALIE || dblhex.dd < DOUBLE_MIN_VALIE) )
						{
							EB_Printf(TEXT("[dnw] Out of range error double Value [%s] (%f)\r\n"), szDbl2Hex_In1, inDblValue );
							BeepSound(4);
							return TRUE;
						}
			#endif


						if( F2H_LITTLE_ENDIAN==iFlt2HexEndian ) // Little Endian
						{
							// LSB
							wsprintf(rstFlt2Hex, TEXT("%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X"), 
								dblhex.di[0], dblhex.di[1], dblhex.di[2], dblhex.di[3], dblhex.di[4], dblhex.di[5], dblhex.di[6], dblhex.di[7] );
							EB_Printf(TEXT("[dnw] double (%lf) -> Little hexa [%s] \r\n"), dblhex.dd, rstFlt2Hex );
						}
						else if( F2H_BIG_ENDIAN==iFlt2HexEndian ) // Big Endian
						{
							// MSB
							wsprintf(rstFlt2Hex, TEXT("%02X-%02X-%02X-%02X-%02X-%02X-%02X-%02X"), 
								dblhex.di[7], dblhex.di[6], dblhex.di[5], dblhex.di[4], dblhex.di[3], dblhex.di[2], dblhex.di[1], dblhex.di[0] );
							EB_Printf(TEXT("[dnw] double (%lf) -> Big hexa [%s] \r\n"), dblhex.dd, rstFlt2Hex );
						}
						


					}

					// ----------------------------------------------------------------------------
					
					// ----------------------------------------------------------------------------
					// 2. Display Filename --------------------------------------------------------
					
					lstrcpy(szFlt2Hex_Out,	rstFlt2Hex);
					szFlt2Hex_Out[MAX_PATH-1]='\0';
					SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_OUT_VALUE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szFlt2Hex_Out);  
					// ----------------------------------------------------------------------------
					
					break;


				case F2H_AUTO_TYPE: // Hexa 2 float
					
					ThreadCalcFlt2Hex(NULL);

					break;
				
			}

		#if 0
			// Destroys a modal dialog box, causing the system to end any processing for the dialog box. 
			if( 0==EndDialog(hDlgFlt2Hex,1) )
			{
				// Error
			}
			if( hDlgFlt2Hex ) { CloseHandle(hDlgFlt2Hex); hDlgFlt2Hex=NULL; }
			if( _hDlgFlt2Hexa ) { CloseHandle( _hDlgFlt2Hexa ); _hDlgFlt2Hexa=NULL; }

		#endif

			SetRegistry();

			return TRUE;


		case ID_FLT2HEX_CLEAR_SCR:
			ScrClear(_EditHwnd);

		#if 0
			memset(szFlt2Hex_Out, 0x00, sizeof(szFlt2Hex_Out) );
			szFlt2Hex_Out[MAX_PATH-1]='\0';

			// screen buffer clear
			SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_OUT_VALUE),EM_SETSEL,0,-1); // all the text is selected. 
			SendMessage(GetDlgItem(hDlgFlt2Hex,ID_FLT2HEX_EDIT_OUT_VALUE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)""); 
		#endif
		
			return TRUE;

		case ID_FLT2HEX_STOP:
			isFlt2HexStop = 1; // stop

			return TRUE;


		case ID_FLT2HEX_FLT2HEX: // float to hexa
		case ID_FLT2HEX_DBL2HEX:
		case ID_FLT2HEX_AUTO_CREATE: // AutoCreate
	
			iChoiceFlt2Hex = tempChoiceFlt2Hex = LOWORD(wParam)-ID_FLT2HEX_FLT2HEX;

			if(F2H_FLOAT_TYPE==iChoiceFlt2Hex || F2H_DOUBLE_TYPE==iChoiceFlt2Hex) 
			{
				EnableWindow( GetDlgItem(hDlgFlt2Hex, ID_FLT2HEX_EDIT_IN2_VALUE ), 0 );
				EnableWindow( GetDlgItem(hDlgFlt2Hex, ID_FLT2HEX_EDIT_IN3_VALUE ), 0 );
				EnableWindow( GetDlgItem(hDlgFlt2Hex, ID_FLT2HEX_TXT_IN2 ), 0 );
				EnableWindow( GetDlgItem(hDlgFlt2Hex, ID_FLT2HEX_TXT_IN3 ), 0 );
			}
			else
			{
				EnableWindow( GetDlgItem(hDlgFlt2Hex, ID_FLT2HEX_EDIT_IN2_VALUE ), 1 );
				EnableWindow( GetDlgItem(hDlgFlt2Hex, ID_FLT2HEX_EDIT_IN3_VALUE ), 1 );
				EnableWindow( GetDlgItem(hDlgFlt2Hex, ID_FLT2HEX_TXT_IN2 ), 1 );
				EnableWindow( GetDlgItem(hDlgFlt2Hex, ID_FLT2HEX_TXT_IN3 ), 1 );
			}

			return TRUE;
			
		
		case ID_FLT2HEX_LITTLE:
		case ID_FLT2HEX_BIG:
			// Message On/Off 
			iFlt2HexEndian = tempFlt2HexEndian = LOWORD(wParam)-ID_FLT2HEX_LITTLE;
			return TRUE;


		case ID_FLT2HEX_EDIT_IN1_VALUE:
			
			return TRUE;

		case ID_FLT2HEX_EDIT_IN2_VALUE: // 2nd value
		case ID_FLT2HEX_EDIT_IN3_VALUE: // step

			return TRUE;
			
		case ID_FLT2HEX_EDIT_OUT_VALUE:
			return TRUE;


		default:
			return FALSE;


		} // case WM_COMMAND:
		break;


	default:
		return FALSE;
	}

	return FALSE;
}
#endif


#if (RICHED_COLOR) || (TEXT_FIND_MARK) // 2016.09.23
void CheckTxtColorLen(void)
{
	int idx=0;
	
	idx = lstrlen(szDataRed);
	if( idx > MAX_PATH )
	{
		EB_Printf(TEXT("\r\n[dnw] Input txt-szDataRed length is over!!! length(%d) \r\n"), idx );
		szDataRed[MAX_PATH-1] = '\0';
		BeepSound(4);
	}


#if 0
	idx = lstrlen(szDataMagenta);
	if( idx > MAX_PATH )
	{
		EB_Printf(TEXT("\r\n[dnw] Input txt-szDataMagenta length is over!!! length(%d) \r\n"), idx );
		szDataMagenta[MAX_PATH-1] = '\0';
		BeepSound(4);
	}

	idx = lstrlen(szDataBlue);
	if( idx > MAX_PATH )
	{
		EB_Printf(TEXT("\r\n[dnw] Input txt-szDataBlue length is over!!! length(%d) \r\n"), idx );
		szDataBlue[MAX_PATH-1] = '\0';
		BeepSound(4);
	}

	idx = lstrlen(szDataGreen);
	if( idx > MAX_PATH )
	{
		EB_Printf(TEXT("\r\n[dnw] Input txt-szDataGreen length is over!!! length(%d) \r\n"), idx );
		szDataGreen[MAX_PATH-1] = '\0';
		BeepSound(4);
	}

	idx = lstrlen(szDataYellow);
	if( idx > MAX_PATH )
	{
		EB_Printf(TEXT("\r\n[dnw] Input txt length-szDataYellow is over!!! length(%d) \r\n"), idx );
		szDataYellow[MAX_PATH-1] = '\0';
		BeepSound(4);
	}
#endif
}


BOOL CALLBACK cbLogTxtColor(HWND hDlgTxtColor, UINT message, WPARAM wParam, LPARAM lParam)
{
	WORD pmId;
	WORD wmEvent, wmId;
	int tmpLen=0;
	int idx=0, jdx=0, isFind=0;
	//uintptr_t threadResult;
	HANDLE hFile = NULL;
	TCHAR szLenTxt[MAX_PATH] = {0,};
	
	pmId      = LOWORD(lParam);

	wmEvent   = HIWORD(wParam);
	wmId      = LOWORD(wParam); 

	switch(message)
	{
	case WM_INITDIALOG:

		_hDlgTxtColorLOG      = hDlgTxtColor;

		// 
		tmptxtColorRed  = txtColorRed;
		CheckRadioButton(hDlgTxtColor, ID_COLOR_CHECK_RED_OFF,     ID_COLOR_CHECK_RED_ON,     ID_COLOR_CHECK_RED_OFF+tmptxtColorRed);

		tmptxtColorMegenta = txtColorMegenta;
		CheckRadioButton(hDlgTxtColor, ID_COLOR_CHECK_MAGENTA_OFF, ID_COLOR_CHECK_MAGENTA_ON, ID_COLOR_CHECK_MAGENTA_OFF+tmptxtColorMegenta);

		tmptxtColorBlue = txtColorBlue; 
		CheckRadioButton(hDlgTxtColor, ID_COLOR_CHECK_BLUE_OFF,    ID_COLOR_CHECK_BLUE_ON,    ID_COLOR_CHECK_BLUE_OFF+tmptxtColorBlue);

		tmptxtColorGreen = txtColorGreen; 
		CheckRadioButton(hDlgTxtColor, ID_COLOR_CHECK_GREEN_OFF,   ID_COLOR_CHECK_GREEN_ON,   ID_COLOR_CHECK_GREEN_OFF+tmptxtColorGreen);

		tmptxtColorYellow = txtColorYellow;
		CheckRadioButton(hDlgTxtColor, ID_COLOR_CHECK_YELLOW_OFF,  ID_COLOR_CHECK_YELLOW_ON,  ID_COLOR_CHECK_YELLOW_OFF+tmptxtColorYellow);

		EnableWindow( GetDlgItem(hDlgTxtColor, ID_COLOR_EDIT_RED ),     txtColorRed );
		EnableWindow( GetDlgItem(hDlgTxtColor, ID_COLOR_EDIT_MAGENTA ), txtColorMegenta );

		txtColorBlue = 0;
		EnableWindow( GetDlgItem(hDlgTxtColor, ID_COLOR_EDIT_BLUE ),    txtColorBlue );

		txtColorGreen = 0;
		EnableWindow( GetDlgItem(hDlgTxtColor, ID_COLOR_EDIT_GREEN ),   txtColorGreen );

		txtColorYellow = 0;
		EnableWindow( GetDlgItem(hDlgTxtColor, ID_COLOR_EDIT_YELLOW ),  txtColorYellow );


		#if 1
		EnableWindow( GetDlgItem(hDlgTxtColor, ID_COLOR_CHECK_BLUE_OFF ),  0 );
		EnableWindow( GetDlgItem(hDlgTxtColor, ID_COLOR_CHECK_BLUE_ON ),  0 );

		EnableWindow( GetDlgItem(hDlgTxtColor, ID_COLOR_CHECK_GREEN_OFF ),  0 );
		EnableWindow( GetDlgItem(hDlgTxtColor, ID_COLOR_CHECK_GREEN_ON ),  0 );

		EnableWindow( GetDlgItem(hDlgTxtColor, ID_COLOR_CHECK_YELLOW_OFF ),	0 );
		EnableWindow( GetDlgItem(hDlgTxtColor, ID_COLOR_CHECK_YELLOW_ON ),	0 );

		EnableWindow( GetDlgItem(hDlgTxtColor, ID_COLOR_CHECK_MAGENTA_OFF ),	0 );
		EnableWindow( GetDlgItem(hDlgTxtColor, ID_COLOR_CHECK_MAGENTA_ON ),	0 );

		#endif


		// last input file
		szDataRed[MAX_PATH-1]='\0';
		SendMessage(GetDlgItem(hDlgTxtColor,ID_COLOR_EDIT_RED),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szDataRed);  

#if 0
		szDataMagenta[MAX_PATH-1]='\0';
		SendMessage(GetDlgItem(hDlgTxtColor,ID_COLOR_EDIT_MAGENTA),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szDataMagenta);  

		szDataBlue[MAX_PATH-1]='\0';
		SendMessage(GetDlgItem(hDlgTxtColor,ID_COLOR_EDIT_BLUE),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szDataBlue);  

		szDataGreen[MAX_PATH-1]='\0';
		SendMessage(GetDlgItem(hDlgTxtColor,ID_COLOR_EDIT_GREEN),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szDataGreen);  

		szDataYellow[MAX_PATH-1]='\0';
		SendMessage(GetDlgItem(hDlgTxtColor,ID_COLOR_EDIT_YELLOW),EM_REPLACESEL,0,(LPARAM)(LPCSTR)szDataYellow);  
#endif
		// ----------------------------------------------------------------------------
		// -----------------------------------------------------------------

		CheckTxtColorLen();
		IsMarking();

		return TRUE;



	case WM_COMMAND:

		wmEvent = HIWORD(wParam);
		wmId	= LOWORD(wParam); 

		switch(LOWORD(wParam))
		{

		case IDCANCEL: // Exit!!
			// -------------------------------------------------------
			// Clear before exit
			// -------------------------------------------------------

			if( 0==EndDialog(hDlgTxtColor,0) )
			{
				// Error 
			}
			
			if( hDlgTxtColor ) { CloseHandle(hDlgTxtColor); hDlgTxtColor=NULL; }
			if( _hDlgTxtColorLOG ) { CloseHandle(_hDlgTxtColorLOG); _hDlgTxtColorLOG=NULL; }

			return TRUE;


		case IDOK:
		case ID_COLOR_EDIT_SAVE:

			// ----------------------------------------------------------------------------
			//get szDataRed MaxFileSize
			*((WORD *)szDataRed)=MAX_PATH-1;
			//The first WORD should be max character number for EM_GETLINE!
			tmpLen = SendMessage(GetDlgItem(hDlgTxtColor,ID_COLOR_EDIT_RED), 
											EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szDataRed);
			szDataRed[tmpLen]='\0';
			iLenDataRed = lstrlen(szDataRed);

			HashRed = make_hash_p(szDataRed, iLenDataRed);

		
			// ----------------------------------------------------------------------------

	#if 0
			// ----------------------------------------------------------------------------
			//get szDataMagenta MaxFileSize
			*((WORD *)szDataMagenta)=MAX_PATH-1;
			//The first WORD should be max character number for EM_GETLINE!
			tmpLen = SendMessage(GetDlgItem(hDlgTxtColor,ID_COLOR_EDIT_MAGENTA), 
											EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szDataMagenta);
			szDataMagenta[tmpLen]='\0';
			iLenDataMagenta = lstrlen(szDataMagenta);

			HashMagenta = make_hash_p(szDataMagenta, iLenDataMagenta);
			// ----------------------------------------------------------------------------

			// ----------------------------------------------------------------------------
			//get szDataBlue MaxFileSize
			*((WORD *)szDataBlue)=MAX_PATH-1;
			//The first WORD should be max character number for EM_GETLINE!
			tmpLen = SendMessage(GetDlgItem(hDlgTxtColor,ID_COLOR_EDIT_BLUE), 
											EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szDataBlue);
			szDataBlue[tmpLen]='\0';
			iLenDataBlue = lstrlen(szDataBlue);
			// ----------------------------------------------------------------------------

			// ----------------------------------------------------------------------------
			//get szDataGreen MaxFileSize
			*((WORD *)szDataGreen)=MAX_PATH-1;
			//The first WORD should be max character number for EM_GETLINE!
			tmpLen = SendMessage(GetDlgItem(hDlgTxtColor,ID_COLOR_EDIT_GREEN), 
											EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szDataGreen);
			szDataGreen[tmpLen]='\0';
			iLenDataGreen = lstrlen(szDataGreen);
			// ----------------------------------------------------------------------------

			// ----------------------------------------------------------------------------
			//get szDataYellow MaxFileSize
			*((WORD *)szDataYellow)=MAX_PATH-1;
			//The first WORD should be max character number for EM_GETLINE!
			tmpLen = SendMessage(GetDlgItem(hDlgTxtColor,ID_COLOR_EDIT_YELLOW), 
											EM_GETLINE,(WPARAM)0,(LPARAM)(LPCSTR)szDataYellow);
			szDataYellow[tmpLen]='\0';
			iLenDataYellow = lstrlen(szDataYellow);
			// ----------------------------------------------------------------------------
	#endif
	
			CheckTxtColorLen();

			
			SetRegistry();
			IsMarking();


			// Destroys a modal dialog box, causing the system to end any processing for the dialog box. 
			if( 0==EndDialog(hDlgTxtColor,1) )
			{
		    	// Error
			}
			if( hDlgTxtColor ) { CloseHandle(hDlgTxtColor); hDlgTxtColor=NULL; }
			if( _hDlgTxtColorLOG ) { CloseHandle(_hDlgTxtColorLOG); _hDlgTxtColorLOG=NULL; }

			return TRUE;

		case ID_COLOR_EDIT_CLEAR:
			ScrClear(_EditHwnd);

			IsMarking();
			return TRUE;


		case ID_COLOR_CHECK_RED_OFF:
		case ID_COLOR_CHECK_RED_ON:
			tmptxtColorRed  = txtColorRed = LOWORD(wParam)-ID_COLOR_CHECK_RED_OFF;
			EnableWindow( GetDlgItem(hDlgTxtColor, ID_COLOR_EDIT_RED ), 	txtColorRed );

			IsMarking();
			return TRUE;

		case ID_COLOR_CHECK_MAGENTA_OFF:
		case ID_COLOR_CHECK_MAGENTA_ON:
			tmptxtColorMegenta = txtColorMegenta = LOWORD(wParam)-ID_COLOR_CHECK_MAGENTA_OFF;
			EnableWindow( GetDlgItem(hDlgTxtColor, ID_COLOR_EDIT_MAGENTA ), txtColorMegenta );

			IsMarking();
			return TRUE;

		case ID_COLOR_CHECK_BLUE_OFF:
		case ID_COLOR_CHECK_BLUE_ON:
			//tmptxtColorBlue = txtColorBlue = LOWORD(wParam)-ID_COLOR_CHECK_BLUE_OFF;
			tmptxtColorBlue = txtColorBlue = 0;
			EnableWindow( GetDlgItem(hDlgTxtColor, ID_COLOR_EDIT_BLUE ),	txtColorBlue );

			IsMarking();
			return TRUE;

		case ID_COLOR_CHECK_GREEN_OFF:
		case ID_COLOR_CHECK_GREEN_ON:
			//tmptxtColorGreen = txtColorGreen = LOWORD(wParam)-ID_COLOR_CHECK_GREEN_OFF;
			tmptxtColorGreen = txtColorGreen = 0; 
			EnableWindow( GetDlgItem(hDlgTxtColor, ID_COLOR_EDIT_GREEN ),	txtColorGreen );

			IsMarking();
			return TRUE;

		case ID_COLOR_CHECK_YELLOW_OFF:
		case ID_COLOR_CHECK_YELLOW_ON:
			//tmptxtColorYellow = txtColorYellow = LOWORD(wParam)-ID_COLOR_CHECK_YELLOW_OFF;
			tmptxtColorYellow = txtColorYellow = 0; 
			EnableWindow( GetDlgItem(hDlgTxtColor, ID_COLOR_EDIT_YELLOW ),	txtColorYellow );

			IsMarking();
			return TRUE;


		default:
			return FALSE;


		} // case WM_COMMAND:
		break;
		
#if 0
	case WM_SETFOCUS:
		SetFocus( hDlgTxtColor );
		EB_Printf( TEXT("[dnw] WM_SETFOCUS in cbTxData2UART ---\r\n") );
		return TRUE;
#endif


	default:
		return FALSE;
	}

	return FALSE;
}
#endif



//ReadUserConfig() is called once at first
int ReadUserConfig(HWND hwnd)
{
	HANDLE hFile = NULL;
	DWORD fileSize = 0;
	DWORD CurrentPoint;
	DWORD temp;
	TCHAR *buf = NULL;
	TCHAR *token = NULL;
	DWORD i, j;
	HMENU MenuBar;
	HMENU SubMenuBar;
	int   iTx=0;
	TCHAR confile[MAX_PATH] = {0,};


	for ( i = 0; i < MAXHISTORY; i++ )
	{
		szDownloadString[i][0] = '\0';
	}


#if 1 // 2016.02.01
	strcpy(confile,dnwCurrPath);
	strcat(confile,DNW_LOG_FILE2);

	hFile = CreateFile(confile,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
#else
	hFile = CreateFile(DNW_LOG_FILE,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
#endif

	if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
	{
AAAA:

	#if 0 /// 2014.04.14
		//EB_Printf(TEXT("[dnw] [ERROR:Can't open dnw.ini. So, the default configuration is used.]\n") );
		userComPort=1;
		idBaudRate=9; /* 115200 bps*/
//		autoSendKey=0; /// added.
//		tempMsgSave=0;
//		tempTextorHexaOutMode=0;

		userBaudRate=baudTable[idBaudRate];
		///downloadAddress=0xc000000;
		///lstrcpy(szDownloadAddress,TEXT("0xc000000"));


		downloadAddress=0x20030000;
		lstrcpy(szDownloadAddress,TEXT("0x20030000"));

		#ifdef COM_SETTINGS /// 2012.07.11
		userCOMparity = 0; //// IDC_COM_NOPARITY
		userCOMstop = 0; /// IDC_COM_ONESTOPBIT
		userCOMdataIdx = 1; /// index1: 8bit , IDC_COM_DATA_8BIT
		#endif

	#endif /// deleted at 2014.04.14---

		autoSendKey=0; /// added.
		tempAutoSend=0; // added 2017.06.28

	
	#if HEXA_MODE_SUPPORT	/* 2012.07.03 Hexa mode display */
		HexaEndChar = 0x0D0A; // 2016.04.12
		lstrcpy(szHexaEnd,TEXT("0D0A")); // 16 column
		#if 1 // 2017.04.21
		ColumnNumber = 0;
		lstrcpy(szColumnNumber,TEXT("0")); // all data 
		#else
		ColumnNumber = 16;
		lstrcpy(szColumnNumber,TEXT("16")); // 16 column
		#endif
	#endif



	#if USE_TXDATA_TO_SERIAL
		lstrcpy(szSerialTxCharData,TEXT(" qUIz")); // SiRFprima2 boot menu password
		lstrcpy(szSerialTxHexaData,TEXT("0x207155497a"));
		lstrcpy(szSerialTxFileData,TEXT(""));
	#endif

	
		return FALSE;
	}



	fileSize = GetFileSize(hFile,NULL);
	if(fileSize == 0)
	{
		MessageBox(hwnd, TEXT("[dnw] dnw.ini can not read. It is corrupted!!!"), NULL,MB_OK);
		if(buf) { free(buf); buf=NULL; }
		if(hFile) { CloseHandle(hFile); hFile=NULL; }
		goto AAAA;
	}

	CurrentPoint = 0;
	buf = (char *)malloc(fileSize);
	if( NULL==buf )
	{
		MessageBox(hwnd, TEXT("[dnw] buf is NULL !!!"), NULL,MB_OK);
		if(hFile) { CloseHandle(hFile); hFile=NULL; }
		goto AAAA;
	}
	
	memset(buf, 0, sizeof(TCHAR)*fileSize);
    
    ReadFile(hFile,buf,fileSize,&temp,NULL);


#if 0 /// 2014.04.14
    userComPort = *((TCHAR *)buf+0)-'0';	    //UNICODE problem!!!
    idBaudRate  = *((TCHAR *)buf+1)-'0';	    //UNICODE problem!!!
	//autoSendKey = *((TCHAR *)buf+2)-'0';  /// added.

    userBaudRate=baudTable[idBaudRate];

 #define ADDR_START 		2  /// 2
 
    if(buf[ADDR_START]!='\n')
    {

		for(i=0;i<fileSize;i++) //search first '\n'
		{
			if(buf[i]=='\n')
			{
				token = &buf[i+1];
				break;
			}
		}
		CurrentPoint = i+1;
//		token = strtok(buf, "\n");
		strncpy(szDownloadAddress,(TCHAR *)buf+ADDR_START, i-ADDR_START);
		downloadAddress=hex2int(szDownloadAddress);	
		token = &buf[CurrentPoint];
    }
    else
    {
		///downloadAddress=0xc000000;
		///lstrcpy(szDownloadAddress,TEXT("0xc000000"));

	#if 1
		/// ------- 2014.04.11, added -------
		switch( CPUtype )
		{
			case 0: /// SMDK6410,  IDC_CPU_SMDK6410
				downloadAddress=0x50100000;
				lstrcpy(szDownloadAddress,TEXT("0x50100000"));
				break;
	
			case 1: /// SMDKC100, IDC_CPU_SMDKC100
			default:
				downloadAddress=0x20030000;
				lstrcpy(szDownloadAddress,TEXT("0x20030000"));
				break;
		}

	#else
		downloadAddress=0x20030000;
		lstrcpy(szDownloadAddress,TEXT("0x20030000"));
	#endif		

    }

#ifdef COM_SETTINGS /// 2012.07.11
	userCOMparity = 0; //// IDC_COM_NOPARITY
	userCOMstop = 0; /// IDC_COM_ONESTOPBIT
	userCOMdataIdx = 1; /// index 1, 8bit IDC_COM_DATA_8BIT
#endif

#endif /// deleted at 2014.04.14---






//	token = strtok(buf, "\n");
	for ( i = 0; i < MAXHISTORY; i++ )
	{
		for(j=0;j<fileSize-CurrentPoint;j++)
		{
			if(buf[CurrentPoint+j]=='\n')
			{
				CurrentPoint += (j+1);
				break;
			}
		}
		if ( j != 0 )
		{
			strncpy(szDownloadString[i], token, j);
			if ( i < (MAXHISTORY/3) ) transmitMenuNumber++;
			if ( i >= (MAXHISTORY/3) && i < MAXHISTORY*2/3 ) UbootMenuNumber++;
			if ( i >= (MAXHISTORY*2/3) && i < MAXHISTORY ) UbootMenuNumber2++;
		}
		token = &buf[CurrentPoint];
	}

	if(buf) { free(buf); buf=NULL; }
	if(hFile) { CloseHandle(hFile); hFile=NULL; }

	MenuBar = GetSubMenu(GetMenu(hwnd), 1);
	SubMenuBar = GetSubMenu(MenuBar, 0);
	for ( iTx = 0; iTx < transmitMenuNumber; iTx++ )
	{
		AppendMenu(SubMenuBar, MF_STRING, HISTORYMENUID + iTx, szDownloadString[iTx]);
	}

	MenuBar = GetSubMenu(GetMenu(hwnd), 1);
	SubMenuBar = GetSubMenu(MenuBar, 1);
	for ( iTx = (MAXHISTORY*2/3); iTx < (MAXHISTORY*2/3)+UbootMenuNumber2; iTx++ )
	{
		AppendMenu(SubMenuBar, MF_STRING, HISTORYMENUID + iTx, szDownloadString[iTx]);
	}

	return TRUE;
}



//WinExec("brainpower/coz.exe",SW_SHOW);
BOOL ExecProgram (LPCTSTR lpAppName, LPTSTR lpCmdLine)
{
	DWORD dwCreationFlags;
	PROCESS_INFORMATION pi;
	STARTUPINFO sinfo = {0};
	int rc;


	ZeroMemory( &pi, sizeof(pi) );

	sinfo.cb = sizeof( STARTUPINFO );
	//sinfo.dwFlags = STARTF_USEPOSITION | STARTF_USESIZE;
  
	dwCreationFlags = 0;
	__try {
		rc = CreateProcess (lpAppName, 
							lpCmdLine, 
							NULL, 
							NULL, 
							FALSE,  // 부모프로세스중 상속가능한 핸들 상속 : TRUE
							dwCreationFlags, // CREATE_NEW_CONSOLE
							NULL, 
							NULL, 
							&sinfo,  // STARTUPINFO 구조체 정보를 위에서 만들어줬죠.
							&pi); //이젠 프로세스의 정보를 가져올때 이 구조체를 사용!

		if (rc) 
		{
			CloseHandle (pi.hThread);
			CloseHandle (pi.hProcess);
		} 
		else 
		{
			EB_Printf(TEXT("[dnw] ExecProgram::CreateProcess[%s] Error:%u \r\n"), lpAppName, GetLastError() );
			return FALSE;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		if(pi.hThread) CloseHandle(pi.hThread);
		if(pi.hProcess) CloseHandle(pi.hProcess);

		EB_Printf(TEXT("[dnw] ExecProgram:: CreateProcess  __except[%s] rc(%d) Error:%u \r\n"), lpAppName, rc, GetLastError() );
		return FALSE;
	}
	return TRUE;
}


BOOL isFileExist(TCHAR *file)
{
	if(0==lstrcmp(file,TEXT("")) || NULL==file) return FALSE;

	HANDLE hFind = NULL;
	WIN32_FIND_DATA fd;

	memset( &fd, 0x00, sizeof(WIN32_FIND_DATA) );

	hFind = FindFirstFile (file, &fd);
	if( hFind != INVALID_HANDLE_VALUE ) 
	{
		FindClose (hFind);
		return TRUE;
	}

	return FALSE;
}


int WriteUserConfig(void)
{
	HANDLE hFile = NULL;
	DWORD temp;
	TCHAR buf[MAXHISTORY*FILENAMELEN+1] = {0,};
	int i;
	DWORD dwFileAttr, dwError;
	TCHAR confile[MAX_PATH] = {0,};

	strcpy(confile,dnwCurrPath);
	strcat(confile,DNW_LOG_FILE2);


#if 1 // 2016.02.01
	if( isFileExist(confile) )
#else
	if( isFileExist(DNW_LOG_FILE) )
#endif
	{
		//dwFileAttr = FILE_ATTRIBUTE_ARCHIVE;
		//dwFileAttr = FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_ARCHIVE|FILE_ATTRIBUTE_HIDDEN; // FILE_ATTRIBUTE_SYSTEM
		dwFileAttr = FILE_ATTRIBUTE_ARCHIVE|FILE_ATTRIBUTE_HIDDEN;

	#if 1
		if( FALSE == SetFileAttributes(confile, dwFileAttr) )
	#else
		if( FALSE == SetFileAttributes(DNW_LOG_FILE, dwFileAttr) )
	#endif
		{
			dwError = GetLastError();					
			EB_Printf(TEXT("[dnw]::WriteUserConfig FileAttr:1:[0x%X], dwError(%d) \r\n"), confile, dwFileAttr, dwError  );
		}
	}
	else
	{

		hFile = CreateFile(confile, GENERIC_WRITE,FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,0,NULL);
		if( (INVALID_HANDLE_VALUE==hFile) || (NULL==hFile) )
		{
			EB_Printf(TEXT("[dnw] can not create dnw.ini file. (Error:%u) \n"), GetLastError() );
			return FALSE;
		}


#if 0 /// 2014.04.14
	    buf[0]=userComPort+'0';
	    buf[1]=idBaudRate+'0';
	    ///buf[2]=autoSendKey+'0'; /// added.
	    lstrcpy(buf+2,szDownloadAddress);
	    lstrcat(buf,TEXT("\n"));
	//    lstrcat(&buf[0],TEXT("||+download Address\n"));
	//    lstrcat(&buf[0],TEXT("|+-0:115200 1:57600 2:38400 3:19200 4:14400 5:9600\n"));
	//    lstrcat(&buf[0],TEXT("+- 1:COM1 2:COM2 3:COM3 4:COM4......... 9:COM9\n"));


		for ( i = 0; i < MAXHISTORY; i++ )
		{
			lstrcat(buf,szDownloadString[i]);
			lstrcat(buf,TEXT("\n"));
		}
#else
		lstrcpy(buf,szDownloadString[0]);
		lstrcat(buf,TEXT("\n"));

		for ( i = 1; i < MAXHISTORY; i++ )
		{
			lstrcat(buf,szDownloadString[i]);
		    lstrcat(buf,TEXT("\n"));
		}
#endif



		//	EB_Printf(TEXT("[dnw] %d\n"), lstrlen(buf));
		//	EB_Printf(TEXT("[dnw] %s\n"), buf);

		if(WriteFile(hFile,buf,lstrlen(buf),&temp,NULL)==0)
		{
			/// 2011.11.29,  EB_Printf(TEXT("[dnw] ERROR:Can't write dnw.ini file! \n"));
		}
		    
		CloseHandle(hFile);

	}

#if 0 // 2016.04.13
	/* --- READ-ONLY dnw.ini ----- */
	if( isFileExist(confile) )
	{
		dwFileAttr = FILE_ATTRIBUTE_READONLY|FILE_ATTRIBUTE_ARCHIVE|FILE_ATTRIBUTE_HIDDEN; // FILE_ATTRIBUTE_SYSTEM
		
	#if 1
		if( FALSE == SetFileAttributes(confile, dwFileAttr) )
	#else
		if( FALSE == SetFileAttributes(DNW_LOG_FILE, dwFileAttr) )
	#endif
		{
			dwError = GetLastError();					
			EB_Printf(TEXT("[dnw]::WriteUserConfig FileAttr:2:[0x%X], dwError(%d) \r\n"), confile, dwFileAttr, dwError  );
		}
	}
#endif


	return TRUE;
}


#if 0
char upcase(char c)
{
    if (c >= 'a' && c <= 'z')
        c -= 32;
    return c;
}
 
int hex2dec(char c)
{
    if (c >= '0' && c <= '9')
        return (int) c - '0';
    else
    {
        c = upcase(c);
        if (c >= 'A' && c <= 'F')
            return (int) (10 + c - 'A');
        else
            return 0;
    }
}
#endif


UINT hex2int(TCHAR *str)
{
	int i;
	UINT number=0; 
	int order=1;
	TCHAR ch;

	for(i=lstrlen(str)-1;i>=0;i--)
	{
		ch=str[i];
		if(ch=='x' || ch=='X')break;
		
		if(ch>='0' && ch<='9')
		{
		    number += order*(ch-'0');
		    order  *= 16;
		}
		if(ch>='A' && ch<='F')
		{
		    number += order*(ch-'A'+10);
		    order  *= 16;
		}
		if(ch>='a' && ch<='f')
		{
		    number += order*(ch-'a'+10);
		    order  *= 16;
		}
	}
	return number;
}



ULONG hex2dec(TCHAR *str)
{
	int i;
	ULONG number=0L; 
	int order=1;
	TCHAR ch;

	for(i=lstrlen(str)-1;i>=0;i--)
	{
		ch=str[i];
		if(ch=='x' || ch=='X')break;
		
		if(ch>='0' && ch<='9')
		{
		    number += order*(ch-'0');
		    order  *= 16;
		}
		if(ch>='A' && ch<='F')
		{
		    number += order*(ch-'A'+10);
		    order  *= 16;
		}
		if(ch>='a' && ch<='f')
		{
		    number += order*(ch-'a'+10);
		    order  *= 16;
		}
	}
	return number;
}




int  isinteger(TCHAR *str)
{
	int i;
	int number=0; 
	int order=1;
	TCHAR ch;

	for(i=lstrlen(str)-1;i>=0;i--)
	{
		ch=str[i];

		if(ch=='.' || ch=='-') return -1; 

		if(ch>='0' && ch<='9')
		{
		    number+=order*(ch-'0');
		    order*=10;
		}

		if(ch>='A' && ch<='F') return -1;
		if(ch>='a' && ch<='f') return -1;
	}
	return number;
}


UINT str2int_(TCHAR *str, int len)
{
	int i;
	UINT number=0; 
	int order=1;
	TCHAR ch;

	for(i=len-1;i>=0;i--)
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
	EB_Printf(TEXT("[dnw] ===[%X] \r\n"), number );

	return number;
}


UINT str2int(TCHAR *str)
{
	int i;
	UINT number=0; 
	int order=1;
	TCHAR ch;

	for(i=lstrlen(str)-1;i>=0;i--)
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
#if 0		
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
#endif

	}
	return number;
}



UINT str2hex(TCHAR *str)
{
	int i;
	UINT number=0; 
	int order=1;
	TCHAR ch;

	for(i=lstrlen(str)-1; i>=0; i--)
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



ULONGLONG str2llhex(TCHAR *str)
{
	int i;
	unsigned int order=1;
	ULONGLONG number=0L; 
	TCHAR ch;

	// EB_Printf(TEXT(" [%s] -> [%d] %d 0x010%X\r\n"), str, lstrlen(str), sizeof(ULONGLONG) );

	for(i=lstrlen(str)-1; i>=0; i--)
	{
		ch=str[i];

		// EB_Printf(TEXT(" %3d -> [%x][%c] : 0x%010X \r\n"), i, ch, ch, number );

		if(ch=='x' || ch=='X') break;

		if(ch=='.' || ch=='-') return 0; 

		if(ch>='0' && ch<='9')
		{
		    number += order*(ch-'0');
		    order*=16L;
		}
	
		if(ch>='A' && ch<='F')
		{
		    number += order*(ch-'A'+10);
		    order*=16L;
		}
		if(ch>='a' && ch<='f')
		{
		    number += order*(ch-'a'+10);
		    order*=16L;
		}
		// EB_Printf(TEXT(" [%I64d] \r\n"), number );
		// EB_Printf(TEXT(" [%08X] [%08X] %d \r\n"), number, number * number, i );

	}
	return number;
}




int ExtractHexVal(TCHAR *str, TCHAR *HexaVal)
{
	int i, iLive=0, len=0;
	TCHAR ch = 0;
	
	iLive = 0;
	len = lstrlen(str);
	for(i=0; i<len; i++)
	{
		ch = str[i];
		if(ch=='x' || ch=='X') { iLive = 0; continue; }

		if( (ch>='0' && ch<='9') )
		{
			HexaVal[iLive]=ch;
			iLive ++;
		}
		if( (ch>='A' && ch<='F') )
		{
			HexaVal[iLive]=ch;
			iLive ++;
		}
		if( (ch>='a' && ch<='f') )
		{
			HexaVal[iLive]=ch;
			iLive ++;
		}
	}
	return iLive;
}


void 	AutoFileNaming(void)
{
	time_t	sys_t;
	unsigned char buff_tmp[DATE_LEN] = {0,}, buff_txt[DATE_LEN] = {0,};


	if( FALSE==SetCurrentDirectory(dnwDefaultPath) )
	{
		EB_Printf(TEXT("[dnw] AutoFileNaming: SetCurrentDirectory is failed. Err(%u) \n\n"), GetLastError() );
	}
	

	memset(buff_tmp, 0x00, sizeof(buff_tmp));
	memset(buff_txt, 0x00, sizeof(buff_txt));
	memset(buff_fin, 0x00, sizeof(buff_fin));

	time( &sys_t ); // Get time in seconds
	strcpy( (char *)buff_tmp, ctime( &sys_t ) );

	/* ------------------------------------------------
		012345679012345678901234
		Mon May 03 20:06:43 2010
	--------------------------------------------------- */
	if( FALSE == isFileExist( TEXT(DNW_LOG_FOLDER1) ) ) /// dnwlog
	{
		///EB_Printf(TEXT("[dnw] none === \r\n"));
		if( FALSE == CreateDirectory( TEXT(DNW_LOG_FOLDER2), NULL) )  // dnwlog
		{
			DWORD dwError = GetLastError(); // 2015.11.28
			if( ERROR_ALREADY_EXISTS == dwError ) /// The specified directory already exists ---
			{
				///EB_Printf(TEXT("[dnw] Can not create the LOG folder or already it!!! "));
			}
			else
			{
				EB_Printf(TEXT("[dnw] Can not create the dnw folder!!2!! Err(%u) \r\n"), dwError );
				return;
			}

		}
	}
	else
	{
		///EB_Printf(TEXT("[dnw] found!!!! === \r\n"));
	}

	/* year */
	strncpy((char *)&buff_txt[0],  (char *)&buff_tmp[20], 4);
	/* Month */
	strncpy((char *)&buff_txt[4],  (char *)&buff_tmp[4], 3);
	/* Date */
	strncpy((char *)&buff_txt[7],  (char *)&buff_tmp[8], 2);

	/* Seperator -- */
	strncpy((char *)&buff_txt[9],  (char *)"_", 1);

	/* Hour */
	strncpy((char *)&buff_txt[10],  (char *)&buff_tmp[11], 2);

	/* Minute */
	strncpy((char *)&buff_txt[12],  (char *)&buff_tmp[14], 2);

	/* Second  */
	strncpy((char *)&buff_txt[14],	(char *)&buff_tmp[17], 2);

	//EB_Printf(TEXT("..... [%s] \n"), TEXT(buff_fin) );

#if 0 // 2016.1.26
	strncpy( (char *)buff_fin, (char *)DNW_FOLDER_NAME2,  sizeof(DNW_FOLDER_NAME2));
#else
	strcpy( (char *)buff_fin, (char *)dnwCurrPath );
#endif

	strcat( (char *)buff_fin, (char *)"\\Log\\dnw_" );
	strcat( (char *)buff_fin, (char *)buff_txt); /// date and time
	strcat( (char *)buff_fin, (char *)".LOG");
}


int 	PopUpFileNaming(void)
{
	BOOL result = 0;
	TCHAR pstrFileName[MAX_PATH] = {0,};
	TCHAR pstrTitleName[MAX_PATH] = {0,};
	TCHAR pfilTitName[MAX_PATH] = {0,};

	
	if( FALSE==SetCurrentDirectory(dnwDefaultPath) )
	{
		//EB_Printf(TEXT("[dnw] PopUpFileNaming: SetCurrentDirectory is failed. Err(%u) \r\n"), GetLastError() );
		//return 0;
	}


	memset(pstrFileName, 0x00, sizeof(pstrFileName));
	memset(pstrTitleName, 0x00, sizeof(pstrTitleName));
	memset(pfilTitName, 0x00, sizeof(pfilTitName));

	memset(buff_fin, 0x00, sizeof(buff_fin));


	wsprintf(pfilTitName, TEXT("Input LOG file name...") );

	result = PopFileMultiSaveDlg (_MainHwnd, pstrFileName, pstrTitleName, pfilTitName);
	if( 1 != result )
	{
		//EB_Printf(TEXT("\r\n[dnw] Input file length is NULL! \r\n") );
		return 0;
	}


	if( FALSE == isFileExist( dnwDefaultPath ) ) 
	{
	#if 0
		TCHAR szTxt[MAX_PATH] = {0,};
		wsprintf( szTxt, TEXT("[[%s]] [[ %s ]] "), pstrFileName , dnwDefaultPath );
		MessageBox(_MainHwnd, szTxt, TEXT("Message!!! \n"), MB_OK | MB_ICONINFORMATION );	
		EB_Printf(TEXT("[dnw] No default path [%s] [%s] \r\n"), pstrFileName, dnwDefaultPath );
	#endif
	}
	
	strcpy( (char *)buff_fin, (const char *)pstrFileName);
	//strcat( (char *)buff_fin, (char *)".LOG");


	#if 0
	{
	TCHAR szTxt[MAX_PATH] = {0,};
	wsprintf( szTxt, TEXT("[[%s]] [[ %s] "), pstrFileName , dnwDefaultPath );
	MessageBox(_MainHwnd, szTxt, TEXT("Message !!! \n"), MB_OK | MB_ICONINFORMATION );	
	}
	#endif


	return 1;
}



void LOGFile_close(void)
{
	if( fFileWrite )
	{

	#if USER_SCREEN_BUF_SIZE // 2016.02.23
		if( 0xff != tempuserScrBufSizIndex ) 
		{
			userScrBufSizIndex = tempuserScrBufSizIndex;
		}
	#endif

	#if LOG_SAVE_BY_TIMER  // 2016.02.24
		KillTimer(_MainHwnd, TIMER_ID_MSG_WRITE_TO_FILE );
	#endif

		fclose(fFileWrite);
		fFileWrite = NULL;

		if( 0 == LogFileType )
			EB_Printf(TEXT("[dnw] Log-auto:[%s] is saved! \r\n\r\n"), buff_fin );
		else
			EB_Printf(TEXT("[dnw] Log-user:[%s] is saved! \r\n\r\n"), buff_fin );


		memset(buff_fin, 0x00, sizeof(buff_fin) );

		BeepSound(1);
	}
}



BOOL LOGFile_open(void)
{
	LOGFile_close();

	if(msgSaveOnOff)
	{

	#if USER_SCREEN_BUF_SIZE // 2016.02.23
		if( userScrBufSizIndex < 3 ) // huge, large, middle
		{
			tempuserScrBufSizIndex = userScrBufSizIndex;
			userScrBufSizIndex = 3; // change to small size(3)
		}
	#endif

		if( NULL == fFileWrite )
		{

			if( 0==LogFileType ) // auto
			{
				AutoFileNaming(); /* date & time */
			}
			else // popup
			{
				int ret = 0;
				do {
					ret = PopUpFileNaming(); // 2020.08.14
				} while( 0==ret );

			}
			

			// -----------------------------------------------------------------------
			// 'a+' : Open for reading and appending (writing at end of file). 
			// The file is created if it does not exist. 
			// The initial file position for reading is at the beginning of the file, 
			// but output is always appended to the end of the file.
			// -----------------------------------------------------------------------
			// 'w+' : Open for reading and writing. 
			// The file is created if it does not exist, otherwise it is truncated. 
			// The stream is positioned at the beginning of the file.
			// -----------------------------------------------------------------------

			fFileWrite = fopen( (const char*)buff_fin, "a"); //"a+");
			if( NULL == fFileWrite )
			{
				if( 0==LogFileType ) // auto
					EB_Printf(TEXT("[dnw] can not write log file by auto. Error(%u) \r\n"), GetLastError() );
				else
					EB_Printf(TEXT("[dnw] can not write log file by user-input. Error(%u) \r\n"), GetLastError() );


			#if USER_SCREEN_BUF_SIZE // 2016.02.23
				if( 0xff != tempuserScrBufSizIndex ) 
				{
					userScrBufSizIndex = tempuserScrBufSizIndex;
				}
			#endif

				msgSaveOnOff = tempMsgSave = 0; // 2020.04.08

				return FALSE;
			}
			else
			{
				BeepSound(3);
				if( 0 == LogFileType )
					EB_Printf(TEXT("\r\n[dnw] Log-auto:[%s] is starting... \r\n"), buff_fin );
				else
					EB_Printf(TEXT("\r\n[dnw] Log-user:[%s] is starting... \r\n"), buff_fin );
			}

		#if LOG_SAVE_BY_TIMER  // 2016.02.24,   /* 2012.07.07 fflush() 사용으로 변경.... 출력버퍼는 즉시 처리가 된다... flclose() 대신 사용 가능 */
			if( _MainHwnd )
				SetTimer(_MainHwnd, TIMER_ID_MSG_WRITE_TO_FILE, 3*60*1000, NULL); /* 3분 주기로... */
		#endif

			return TRUE;
		}
	}
	return FALSE;
}



int ReWriteQuickUserConfig(void)
{
	int idx, subidx, MaxLen, idx_seq=1;
	int eboot_idx=0xff, stepldr_idx=0xff, nk_idx=0xff;
	TCHAR sztmpString[MAXHISTORY][FILENAMELEN] = { 0, };
	TCHAR szuppString[MAXHISTORY][FILENAMELEN] = { 0, };
	HANDLE hFile = NULL;
	TCHAR buf[MAXHISTORY*FILENAMELEN+1] = {0,};
	DWORD temp = 0;


	memset( szuppString, 0x00, sizeof(szuppString) );


	if( FALSE==SetCurrentDirectory(dnwDefaultPath) )
	{
		EB_Printf(TEXT("[dnw] ReWriteQuickUserConfig: SetCurrentDirectory is failed. Err(%u) \n\n"), GetLastError() );
	}
	

	/* --------------- uppercase ------------------------------------- */
	for ( idx = 0; idx < MAXHISTORY; idx++ )
	{
		lstrcpy( szuppString[idx], szQuickDownloadString[idx] );

		MaxLen = strlen((char *)szuppString[idx]);
		for( subidx=0; subidx<MaxLen; subidx++) 
		{ 
			if(szuppString[idx][subidx] >= 'a' && szuppString[idx][subidx] <= 'z' )
				szuppString[idx][subidx] = (szuppString[idx][subidx])^0x20; 
			else
				szuppString[idx][subidx] = szuppString[idx][subidx];
		}
	}	
	/* -------------------------------------------------------------- */

	memset( sztmpString, 0x00, sizeof(sztmpString) );
	idx_seq = 1;
	for ( idx = 0; idx < MAXHISTORY; idx++ )
	{
		if( (0xff==nk_idx) && (NULL != strstr(szuppString[idx], "NK" )) )
		{
			lstrcpy(sztmpString[NK_IDX], szQuickDownloadString[idx] );
			lstrcat(sztmpString[NK_IDX], " \r\n");
			nk_idx = idx;
		}
		else if( (0xff==eboot_idx) && (NULL != strstr(szuppString[idx], "BOOT" )) )
		{
			lstrcpy(sztmpString[EBOOT_IDX], szQuickDownloadString[idx] );
			lstrcat(sztmpString[EBOOT_IDX], " \r\n");
			eboot_idx = idx;
		}
		else if( (0xff==stepldr_idx) && ( (NULL != strstr(szuppString[idx], "STEP" )) || (NULL != strstr(szuppString[idx], "BLOCK" )) ) )
		{
			lstrcpy(sztmpString[STEPLDR_IDX], szQuickDownloadString[idx] );
			lstrcat(sztmpString[STEPLDR_IDX], " \r\n");
			stepldr_idx = idx;
		}
		else
		{
			lstrcpy(sztmpString[NK_IDX+idx_seq], szQuickDownloadString[idx] );
			lstrcat(sztmpString[NK_IDX+idx_seq], " \r\n");
			idx_seq++;
		}
	}

	if( (EBOOT_IDX == eboot_idx) && (STEPLDR_IDX == stepldr_idx) && (NK_IDX == nk_idx) )
	{
		return TRUE;
	}
	//////////////////////////////////////////////////////////////////
	
	
	memset( szQuickDownloadString, 0x00, sizeof(szQuickDownloadString) );
	for ( idx = 0; idx < MAXHISTORY; idx++ )
	{
		lstrcpy( szQuickDownloadString[idx], sztmpString[idx] );
	}

	hFile = CreateFile( TEXT(USER_TEXT_CONFIG),GENERIC_WRITE, 0,NULL,CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);

	memset(buf, 0x00, sizeof(buf));
	if( hFile !=INVALID_HANDLE_VALUE )
	{
		buf[0]=TotalROMfilenum+'0';
		lstrcat(buf,TEXT("\r\n"));

		for(idx=0; idx<TotalROMfilenum ; idx++)
		{
			lstrcat( &buf[0], szQuickDownloadString[idx] );
		}

		if(WriteFile(hFile,buf,lstrlen(buf),&temp,NULL)==0)
		{
			EB_Printf(TEXT("[dnw] can not write dnw_usbboot.ini file! temp(%d) (Error:%u) \n"), temp, GetLastError());
		}
	}
	else
	{
		EB_Printf(TEXT("[dnw] can not create dnw_usbboot.ini file in ReWriteQuickUserConfig! (Error:%u) \n"), GetLastError());
	}

	if(hFile) CloseHandle(hFile);
	return TRUE;
}


int WriteQuickUserConfig(void)
{
	HANDLE hFile = NULL;
	DWORD temp = 0;
	TCHAR buf[MAXHISTORY*FILENAMELEN+1] = {0,};


	if( FALSE == isFileExist( TEXT(USER_TEXT_CONFIG) ) )
	{
	
		if( FALSE == CreateDirectory( TEXT(DNW_QUICK_FOLDER_NAME1), NULL) ) 
		{
			DWORD dwError = GetLastError();
			if( ERROR_ALREADY_EXISTS == dwError ) /// The specified directory already exists ---
			{
			}
			else
			{
				EB_Printf(TEXT("[dnw] Can not create the QuickUMON!99! dwError(%u) \n\n"), dwError );
				return FALSE;
			}
	
		}	

		hFile = CreateFile( TEXT(USER_TEXT_CONFIG),GENERIC_WRITE, 0,NULL,CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);

		if( hFile != INVALID_HANDLE_VALUE )
		{

			memset(buf, 0x00, sizeof(buf));

			buf[0]=NAND_FILES_NUM+'0';
			lstrcat(buf,TEXT("\r\n"));
		#if 1 /* yhjoo87_20110219_BEGIN -- */
			lstrcat(&buf[0], USER_DOWNLOADFILE_EBOOT );
			lstrcat(&buf[0], USER_DOWNLOADFILE_STEPLDR );
			lstrcat(&buf[0], USER_DOWNLOADFILE_NKBIN );
		#else
			lstrcat(&buf[0],TEXT("C:\\TOP.JOO\\dnw105C\\Release\\IROM_SDMMCBoot.bin \r\n"));
			lstrcat(&buf[0],TEXT("C:\\TOP.JOO\\dnw105C\\Release\\IROM_SDMMCStepldr.nb0 \r\n"));
			lstrcat(&buf[0],TEXT("C:\\TOP.JOO\\dnw105C\\Release\\NK.bin \r\n"));
		#endif /* AAA -- yhjoo87_20110219_END */


		#if 0
			lstrcat(&buf[0],TEXT(".\\UMON\\c100_bl1_usb.bin,"));
				lstrcat(&buf[0], szDownloadAddress );
				lstrcat(&buf[0],TEXT(" \r\n"));

			lstrcat(&buf[0],TEXT(".\\UMON\\IROM_SDMMCBoot.nb0,"));
				lstrcat(&buf[0], szDownloadAddress );
				lstrcat(&buf[0],TEXT(" \r\n"));
		#endif
			
		//	EB_Printf(TEXT("[dnw] %d\n"), lstrlen(buf));
		//	EB_Printf(TEXT("[dnw] %s\n"), buf);

			if(WriteFile(hFile,buf,lstrlen(buf),&temp,NULL)==0)
			{
				EB_Printf(TEXT("[dnw] can not write dnw_usbboot.ini file in WriteQuickUserConfig! (Error:%u) \n"), GetLastError());
			}
		}
		else
		{
			DWORD dwError = GetLastError();
			switch(dwError)
			{
				case ERROR_PATH_NOT_FOUND: // 3 (0x3)
					// EB_Printf(TEXT("[dnw] can not create dnw_usbboot.ini file. \r\n"));
					break;		
				default:
					EB_Printf(TEXT("[dnw] can not create dnw_usbboot.ini file in WriteQuickUserConfig! (Err:%u) \n"), GetLastError());
					break;
			}
		}

		if(hFile) { CloseHandle(hFile);  hFile=NULL; }
	}
	return TRUE;
}



int ReadQuickUserConfig(HWND hwnd)
{
	HANDLE hFile = NULL;
	DWORD fileSize = 0;
	DWORD CurrentPoint = 0;
	DWORD temp = 0;
	TCHAR *buf = NULL;
	TCHAR *token = NULL;
	UINT i, j, len;
	int idx=0;


	if( FALSE==SetCurrentDirectory(dnwDefaultPath) )
	{
		EB_Printf(TEXT("[dnw] ReadQuickUserConfig: SetCurrentDirectory is failed. Err(%u) \n\n"), GetLastError() );
	}
	

	for ( i = 0; i < MAXHISTORY; i++ )
	{
		szQuickDownloadString[i][0] = '\0';
	}

	hFile = CreateFile( TEXT(USER_TEXT_CONFIG),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);

	if( (INVALID_HANDLE_VALUE==hFile)|| (NULL==hFile) )
	{
AAAA:
		// --- EB_Printf(TEXT("[dnw] can not open dnw_usbboot.ini file! Retry it.. Error:%u  \n"), GetLastError());
		WriteQuickUserConfig();
		return FALSE;
	}
    
	fileSize=GetFileSize(hFile,NULL);
	if (fileSize == 0)
	{
		/// EB_Printf(TEXT("[dnw] ERROR:Can't open dnw_usbboot.ini. \n") );
		MessageBox(hwnd, TEXT("[dnw] dnw_usbboot.ini is corrupted!!!"), NULL,MB_OK);
		if(hFile) { CloseHandle(hFile); hFile=NULL; }
		goto AAAA;
	}

	CurrentPoint = 0;

	buf = (char *)malloc(fileSize);
	if( NULL==buf )
	{
		MessageBox(hwnd, TEXT("[dnw] buf is NULL!!"), NULL,MB_OK);
		if(hFile) { CloseHandle(hFile); hFile=NULL; }
		goto AAAA;
	}

	memset(buf, 0, sizeof(TCHAR)*fileSize);

	ReadFile(hFile,buf,fileSize,&temp,NULL);

	TotalROMfilenum=*((TCHAR *)buf+0)-'0';	    //UNICODE problem!!!

	if(buf[1]!='\n')
	{
		for(i=0;i<fileSize;i++) //search first '\n'
		{
			if(buf[i]=='\n')
			{
				token = &buf[i+1];
				break;
			}
		}
		CurrentPoint = i+1;
	}
	else
	{
		///TotalROMfilenum=NAND_FILES_NUM;
		////EB_Printf(TEXT("buf[1] = (0x%X)\n"),buf[1] );
	}
	///EB_Printf(TEXT("[dnw] ++Searched ROM Files = %d:++\n"), TotalROMfilenum );

	for ( idx = 0; idx < TotalROMfilenum; idx++ )
	{
		for(j=0;j<fileSize-CurrentPoint;j++)
		{
			if(buf[CurrentPoint+j]=='\n')
			{
				CurrentPoint += (j+1);
				break;
			}
		}
		if ( j != 0 )
		{
			strncpy(szQuickDownloadString[idx], token, j);
			///strncpy(szQuickDownloadString[idx], token, j-1);
			///EB_Printf(TEXT("[dnw] ++%d:[%s]++\n"), idx, szQuickDownloadString[idx] );
		}
		token = &buf[CurrentPoint];
	}


	if(buf) { free(buf); buf=NULL; }
	if(hFile) { CloseHandle(hFile); hFile=NULL; }


	memset(szQuickFileName, 0x00, sizeof(szQuickFileName) );
	for ( idx=0; idx < TotalROMfilenum; idx++ )
	{
		for(len=FILENAMELEN-1; len>=0; len--)
		{
			if( szQuickDownloadString[idx][len] == ' ' 
				|| szQuickDownloadString[idx][len] == '\n'
				|| szQuickDownloadString[idx][len] == '\r'
				) 
			{ 
				szQuickDownloadString[idx][len]='\0'; 
			}

			if(szQuickDownloadString[idx][len] == '\\' )
			{
				lstrcpy(szQuickFileName[idx], &szQuickDownloadString[idx][len+1] );   /* strcpy() -> lstrcpy() */
				break;
			}
		}
	}

#if 0
	for(idx=0; idx<TotalROMfilenum; idx++)
		EB_Printf(TEXT("\nROM File = %s [%s]"), szQuickDownloadString[idx],szQuickFileName[idx] );
#endif


	return TRUE;
}


// -----------------------------------
// --- File Date & Time
// -----------------------------------
BOOL SetCreateFileDate(TCHAR *filename)
{
	SYSTEMTIME SystemTime;
	FILETIME FileTime;
	BOOL bResult = FALSE;
	HANDLE hFile=NULL;
	BOOL bRet = TRUE;
	
	hFile = CreateFile( filename, /*DNW_QUICK_USB_FILE_NAME_CREATE, */  /// DNW_QUICK_USB_FILE_NAME1
							GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,
							NULL,
							OPEN_EXISTING,
							FILE_FLAG_BACKUP_SEMANTICS,
							NULL);
  
	if( INVALID_HANDLE_VALUE==hFile || NULL==hFile )
	{
		// 파일이 없다.
		return FALSE;
	}

	memset( &SystemTime, 0x00, sizeof(SYSTEMTIME) ); 
	memset( &FileTime, 0x00, sizeof(FILETIME) );
 
  
	SystemTime.wYear		 = 2012;
	SystemTime.wMonth		 = 02;
	SystemTime.wDayOfWeek	 = 0;
	SystemTime.wDay 		 = 02;
	SystemTime.wHour		 = 22;
	SystemTime.wMinute		 = 07;
	SystemTime.wSecond		 = 05;
	SystemTime.wMilliseconds = 0;

	if( FALSE == SystemTimeToFileTime(&SystemTime,&FileTime) )
	{
		/// Error
		bRet = FALSE;
	}

	/// Converts a local file time to a file time based on the Coordinated Universal Time (UTC).
	if( FALSE == LocalFileTimeToFileTime(&FileTime, &FileTime) )
	{
		/// Error
		bRet = FALSE;
	}
	bResult = SetFileTime(hFile, &FileTime, NULL, &FileTime);
	if(hFile) CloseHandle(hFile);

	return bRet;
}


int WriteQuickUMONConfig(void)
{
	HANDLE hFile = NULL;
	DWORD temp = 0, dwError=0;
	TCHAR buf[2048] = {0,};
	FILE *fileck = NULL;


	if( FALSE==SetCurrentDirectory(dnwDefaultPath) )
	{
		EB_Printf(TEXT("[dnw] WriteQuickUMONConfig: SetCurrentDirectory is failed. Err(%u) \n\n"), GetLastError() );
	}


	if( FALSE == isFileExist( TEXT(UMON_TEXT_CONFIG) ) )
	{

		if( FALSE == CreateDirectory( TEXT(DNW_QUICK_FOLDER_NAME1), NULL) ) 
		{
			DWORD dwError = GetLastError();
			if( ERROR_ALREADY_EXISTS == dwError ) /// The specified directory already exists ---
			{
				// EB_Printf(TEXT("[dnw] DNW_QUICK_FOLDER_NAME1 folder already it!!! "));
			}
			else
			{
				EB_Printf(TEXT("[dnw] Can not create the QuickUMON!1! dwError(%u) \n\n"), dwError );
				return FALSE;
			}

		}


		fileck = fopen( (const char*)UMON_TEXT_CONFIG, "r");
		if( NULL != fileck )
		{
			fclose(fileck);
			///EB_Printf(TEXT("[dnw] can open dnw_UmonTx.ini file! (L:%d) -- OK \n"), __LINE__ );
			return TRUE;
		}
		
		hFile = CreateFile( TEXT(UMON_TEXT_CONFIG),GENERIC_WRITE, 0,NULL,CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);

		memset(buf, 0x00, sizeof(buf));
		if(hFile)
		{
			buf[0]=UMON_FILES_NUM+'0';
			lstrcat(buf,TEXT("\r\n"));

			lstrcat(&buf[0],TEXT(DNW_QUICK_USB_FILE_NAME1));
			lstrcat(&buf[0],TEXT(","));
			lstrcat(&buf[0], szDownloadAddress );
			lstrcat(&buf[0],TEXT(" \r\n"));

			///lstrcat(&buf[0],TEXT(".\\QuickUMON\\IROM_SDMMCBoot.nb0,")); /* for iNAND */
			lstrcat(&buf[0],TEXT(DNW_QUICK_EBOOT_FILE_NAME1));
			lstrcat(&buf[0],TEXT(","));
			lstrcat(&buf[0], szDownloadAddress );
			lstrcat(&buf[0],TEXT(" \r\n"));
			
		//	EB_Printf(TEXT("[dnw] %d\n"), lstrlen(buf));
		//	EB_Printf(TEXT("[dnw] %s\n"), buf);

			if(WriteFile(hFile,buf,lstrlen(buf),&temp,NULL)==0)
			{
				// dwError = GetLastError();
				// EB_Printf(TEXT("[dnw] can not write dnw_UmonTx.ini file! (Error:%u) \n"), dwError );
			}
		}
		else
		{
			dwError = GetLastError();
			EB_Printf(TEXT("[dnw] can not create dnw_UmonTx.ini file! (Error:%u) \n"), dwError );
		}

		if(hFile) { CloseHandle(hFile); hFile=NULL; }
	}




#if DNW_C100_INC_USB_BINARY // 2016.04.14
	// C100 USB loading binary files----
	if( FALSE == isFileExist( TEXT(DNW_QUICK_USB_FILE_NAME1) ) ) /// DNW_FOLDER_NAME1 , DNW_FOLDER_NAME2 
	{
		int idx_bin = 0;

		// ------------------------------------------
		// Create c100_bl1_usb.bin file -----------
		// ------------------------------------------
		fileck = fopen(DNW_QUICK_USB_FILE_NAME1, "wb");
		for(idx_bin=0; idx_bin < (16*750); idx_bin++)
		{
			fputc( c100_bl1_usb_bin[idx_bin], fileck);
		}		
		fclose(fileck);
		fileck = NULL;

		SetCreateFileDate( TEXT(DNW_QUICK_USB_FILE_NAME1) );


		// ------------------------------------------
		// Create IROM_SDMMCBoot.bin file -----------
		// ------------------------------------------
		//- iNAND용 - 2015.11.9
		fileck = fopen(DNW_QUICK_USB_FILE_iNAND_BOOT, "wb");
		for(idx_bin=0; idx_bin < (16*32768); idx_bin++)
		{
			fputc( C100_IROM_SDMMCBoot[idx_bin], fileck);
		}		
		fclose(fileck);
		fileck = NULL;

		SetCreateFileDate( TEXT(DNW_QUICK_USB_FILE_iNAND_BOOT) );


		// ------------------------------------------
		// Create EBOOT.nb0 file -----------
		// ------------------------------------------
		idx_bin = 0;
		fileck = fopen(DNW_QUICK_EBOOT_FILE_NAME1, "wb");
		for(idx_bin=0; idx_bin < (16*32768); idx_bin++)
		{
			fputc( c100_EBOOT_nb0[idx_bin], fileck);
		}

		fclose(fileck);
		fileck = NULL;

		SetCreateFileDate( TEXT(DNW_QUICK_EBOOT_FILE_NAME1) );

	}
#endif




#if USE_TXDATA_TO_SERIAL

	#if 0
	if( FALSE == isFileExist( TEXT(DNW_TX_SCRIPT_EX_FILE) ) ) 
	{
		int idx_bin = 0;

		fileck = fopen(DNW_TX_SCRIPT_EX_FILE, "w");
		for(idx_bin=0; NULL != TxScriptExample[idx_bin]; idx_bin++)
		{
			fputs( TxScriptExample[idx_bin], fileck);
		}		
		fclose(fileck);
		fileck = NULL;	
	}
	#endif

#if 0 // 2018.03.08 - comment
	// BLT1 test script ----
	if( FALSE == isFileExist( TEXT(DNW_TX_SCRIPT_BLT1_FILE) ) ) 
	{
		int idx_bin = 0;

		fileck = fopen(DNW_TX_SCRIPT_BLT1_FILE, "w");
		for(idx_bin=0; NULL != TxScript_BLT1[idx_bin]; idx_bin++)
		{
			fputs( TxScript_BLT1[idx_bin], fileck);
		}		
		fclose(fileck);
		fileck = NULL;	
	}


	// BLT2 test script ----
	if( FALSE == isFileExist( TEXT(DNW_TX_SCRIPT_BLT2_FILE) ) ) 
	{
		int idx_bin = 0;

		fileck = fopen(DNW_TX_SCRIPT_BLT2_FILE, "w");
		for(idx_bin=0; NULL != TxScript_BLT2[idx_bin]; idx_bin++)
		{
			fputs( TxScript_BLT2[idx_bin], fileck);
		}		
		fclose(fileck);
		fileck = NULL;	
	}
#endif

	// FUNCTION test script ----
	if( FALSE == isFileExist( TEXT(DNW_TX_SCRIPT_FUNC_FILE) ) ) 
	{
		int idx_bin = 0;

		fileck = fopen(DNW_TX_SCRIPT_FUNC_FILE, "w");
		for(idx_bin=0; NULL != TxScript_FUNC[idx_bin]; idx_bin++)
		{
			fputs( TxScript_FUNC[idx_bin], fileck);
		}
		fclose(fileck);
		fileck = NULL;	
	}

	// FUNCTION test script ----
	if( FALSE == isFileExist( TEXT(DNW_TX_SCRIPT_AVN_FILE) ) ) 
	{
		int idx_bin = 0;

		fileck = fopen(DNW_TX_SCRIPT_AVN_FILE, "w");
		for(idx_bin=0; NULL != TxScript_AVN[idx_bin]; idx_bin++)
		{
			fputs( TxScript_AVN[idx_bin], fileck);
		}
		fclose(fileck);
		fileck = NULL;	
	}

#endif 

	

	if(hFile) { CloseHandle(hFile); hFile=NULL; }
	if(fileck) { fclose(fileck); fileck=NULL; }

	return TRUE;
}



int ReadQuickUMONConfig(HWND hwnd)
{
	HANDLE hFile = NULL;
	DWORD fileSize = 0;
	DWORD CurrentPoint = 0;
	DWORD temp = 0;
	TCHAR *buf = NULL;
	TCHAR *token = NULL;
	UINT i, j, len;
	int idx=0;
	// HMENU MenuBar;
	// HMENU SubMenuBar;


	if( FALSE==SetCurrentDirectory(dnwDefaultPath) )
	{
		EB_Printf(TEXT("[dnw] ReadQuickUMONConfig: SetCurrentDirectory is failed. Err(%u) \n\n"), GetLastError() );
	}

	
	for ( i = 0; i < MAXHISTORY; i++ )
	{
		szUMONDownloadString[i][0] = '\0';
	}

	hFile = CreateFile( TEXT(UMON_TEXT_CONFIG),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);

	if( (INVALID_HANDLE_VALUE==hFile)|| (NULL==hFile) )
	{

AAAA:
		// --- EB_Printf(TEXT("[dnw] can not open dnw_UmonTx.ini. so retry it... Error=%u\n"), GetLastError() );
		WriteQuickUMONConfig();
		return FALSE;
	}

	fileSize=GetFileSize(hFile,NULL);
	if (fileSize == 0)
	{
		///EB_Printf(TEXT("[dnw] [ERROR:Can't open dnw_UmonTx.ini.]\n") );
		MessageBox(hwnd, TEXT("[dnw] can not open dnw_UmonTx.ini. maybe corrupted!!!"), NULL,MB_OK);
		CloseHandle(hFile);
		goto AAAA;
	}

	CurrentPoint = 0;

	buf = (char *)malloc( fileSize * sizeof(TCHAR) );
	if( NULL == buf )
	{
		EB_Printf(TEXT("[dnw] ReadQuickUMONConfig: buffer is NULL. fileSize(%d) Err(%u) \n\n"), fileSize, GetLastError() );
		if(hFile) CloseHandle(hFile);
		return FALSE;
	}
	
	memset(buf, 0, sizeof(TCHAR)*fileSize);

	ReadFile(hFile,buf,fileSize,&temp,NULL);

	UmonFileNum=*((TCHAR *)buf+0)-'0';	    //UNICODE problem!!!

	if(buf[1]!='\n')
	{
		for(i=0;i<fileSize;i++) //search first '\n'
		{
			if(buf[i]=='\n')
			{
				token = &buf[i+1];
				break;
			}
		}
		CurrentPoint = i+1;
	}
	else
	{
		UmonFileNum=UMON_FILES_NUM;
	}
	///EB_Printf(TEXT("[dnw] ++Searched ROM Files = %d:++\n"), TotalROMfilenum );

	for ( idx = 0; idx < UmonFileNum; idx++ )
	{
		for(j=0; j<fileSize-CurrentPoint; j++)
		{
			if(buf[CurrentPoint+j]=='\n')
			{
				CurrentPoint += (j+1);
				break;
			}
		}
		if ( j != 0 )
		{
			strncpy(szUMONDownloadString[idx], token, j);
			///strncpy(szUMONDownloadString[idx], token, j-1);
			///EB_Printf(TEXT("[dnw] ++%d:[%s]++\n"), idx, szUMONDownloadString[idx] );
		}
		token = &buf[CurrentPoint];
	}

	if(buf) { free(buf); buf=NULL; }
	if(hFile) { CloseHandle(hFile); hFile=NULL; }

	memset(szUMONSFileName, 0x00, sizeof(szUMONSFileName) );
	for ( idx=0; idx < UmonFileNum; idx++ )
	{
		for(len=FILENAMELEN-1; len>=0; len--)
		{
			if( szUMONDownloadString[idx][len] == ' ' 
				|| szUMONDownloadString[idx][len] == '\n'
				|| szUMONDownloadString[idx][len] == '\r'
				) 
			{ 
				szUMONDownloadString[idx][len]='\0'; 
			}

			if(szUMONDownloadString[idx][len] == '\\' )
			{
				lstrcpy(szUMONSFileName[idx], &szUMONDownloadString[idx][len+1] );  /* strcpy() -> lstrcpy() */
				break;
			}
		}
	}

#if 0
	for(idx=0; idx<TotalROMfilenum; idx++)
		EB_Printf(TEXT("\nROM File = %s [%s]"), szUMONDownloadString[idx],szUMONSFileName[idx] );
#endif


	return TRUE;
}



__int64 GetFileExSize(HANDLE hFile)
{
	LARGE_INTEGER FilelargeInt;
	__int64 ulFileSize = 0UL;

	memset(&FilelargeInt, 0x0, sizeof(FilelargeInt));
	if( FALSE==GetFileSizeEx(hFile, &FilelargeInt) )
	{
		EB_Printf(TEXT("[dnw] can not open GetFileExSize error:%u \r\n"), GetLastError() );
	}
	else
	{
		if(FilelargeInt.HighPart == 0  && FilelargeInt.LowPart == 0)
		{
			// file size is zero
			ulFileSize = 0UL;
		}
		else
		{
			ulFileSize = FilelargeInt.QuadPart;

		/*
			ulFileSize = FilelargeInt.HighPart;
			ulFileSize <<= 32;
			ulFileSize |= FilelargeInt.LowPart; */
		}
	}
	return ulFileSize;
}



