#ifndef __DNSTATUS_H__
#define __DNSTATUS_H__





#define TXDATA_BY_MENU 		"by MENU"
#define TXDATA_BY_F6Key 	"by F6"


// 2018.07.06
enum {
	HEX2BIN_REAL_ADDR = 0,  // Never modified!!!
	HEX2BIN_ZERO_FORCED = 1,
	HEX2BIN_HIGH_ADDR = 2,

	HEX2BIN_MAX = 0xff
};


//#if USE_FLOAT2HEX_VALUE // 2018.07.18
enum {
	F2H_LITTLE_ENDIAN = 0,
	F2H_BIG_ENDIAN =1,

	F2H_MAX = 0xff
};

enum {
	F2H_FLOAT_TYPE = 0,
	F2H_DOUBLE_TYPE,
	F2H_AUTO_TYPE,

	F2H_MAX_TYPE = 0xff
};
//#endif


BOOL CALLBACK DownloadProgressProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DownloadProgressProcUART(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

void InitDownloadProgress(void); // USB port
void InitDownloadProgressUART(void); // COM port

void DisplayDownloadProgress(unsigned int percent);
void DisplayDownloadProgressUART(__int64 read, __int64 tot);

void CloseDownloadProgress(void); // USB port
void CloseDownloadProgressUART(void); // COM port

int ReadUserConfig(HWND hwnd);
int WriteUserConfig(void);


int ReadQuickUserConfig(HWND hwnd);
int ReWriteQuickUserConfig(void);
int ReadQuickUMONConfig(HWND hwnd);
int WriteQuickUMONConfig(void);
int WriteQuickUserConfig(void);

__int64 GetFileExSize(HANDLE hFile);
	

BOOL CALLBACK OptionsProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

#if USE_HEX2BIN_CONVERT // 2016.03.04
BOOL CALLBACK cbHex2BinProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
int MenuHex2BinSelect(HWND hwnd, int BinType);
#endif


#if USE_TXDATA_TO_SERIAL // 2016.03.27
BOOL CALLBACK cbTxData2UART(HWND hDlgTxData, UINT message, WPARAM wParam, LPARAM lParam);
#endif

#if USE_FLOAT2HEX_VALUE // 2018.07.18
BOOL CALLBACK cbFloat2Hexa(HWND hDlgTxData, UINT message, WPARAM wParam, LPARAM lParam);
#endif

#if (RICHED_COLOR) || (TEXT_FIND_MARK) // 2016.09.23
BOOL CALLBACK cbLogTxtColor(HWND hDlgTxtColor, UINT message, WPARAM wParam, LPARAM lParam);
#endif


extern int downloadCanceled;

extern volatile HWND _hDlgDownloadProgress;
extern volatile HWND _hDlgDownloadProgressUART;


#define DNW_LOG_FILE 					TEXT(".\\dnw\\dnw.ini")  /// 2014.04.10, TEXT("c:\\dnw.ini")
#define DNW_LOG_FILE2 					"\\dnw\\dnw.ini"

#define EBOOT_IDX 				0
#define STEPLDR_IDX 			1	
#define NK_IDX 					2	



#define CRC_LOG_FILE 					".\\dnw\\dnw_checksum.txt"
#define CRC_VIEW_FILE 					"dnw\\dnw_checksum.txt"   // notepad dnw_checksum.txt



#define UMON_TEXT_CONFIG 				".\\dnw\\QuickUMON\\dnw_UmonTx.ini"
#define USER_TEXT_CONFIG 				".\\dnw\\QuickUMON\\dnw_usbboot.ini"
#define USER_TXDATA_FILE 				".\\dnw\\dnw_cmd_history.txt"


#define UMON_FILES_NUM 			2
#define NAND_FILES_NUM 			3

#define USER_DOWNLOADFILE_EBOOT 		(TEXT("C:\\TOP.JOO\\") APP_VER TEXT("\\Release\\L2_EBOOT.bin \r\n"))
#define USER_DOWNLOADFILE_STEPLDR 		(TEXT("C:\\TOP.JOO\\") APP_VER TEXT("\\Release\\STEPLDR.nb0 \r\n")) 
#define USER_DOWNLOADFILE_NKBIN 		(TEXT("C:\\TOP.JOO\\") APP_VER TEXT("\\Release\\NK.bin \r\n")) 


#define DNW_LOG_FOLDER1 				".\\Log"
#define DNW_LOG_FOLDER2 				"Log"

#define DNW_FOLDER_NAME1 				".\\dnw"
#define DNW_FOLDER_NAME2 				"dnw"

#define DNW_QUICK_FOLDER_NAME1 			"dnw\\QuickUMON"
#define DNW_QUICK_USB_FILE_NAME1 		".\\dnw\\QuickUMON\\c100_bl1_usb.bin"
#define DNW_QUICK_EBOOT_FILE_NAME1 		".\\dnw\\QuickUMON\\EBOOT.nb0"

#define DNW_QUICK_USB_FILE_iNAND_BOOT	".\\dnw\\QuickUMON\\IROM_SDMMCBoot.nb0"

#define DNW_QUICK_USB_FILE_NAME_CREATE  L".\\dnw\\QuickUMON\\c100_bl1_usb.bin"



#define DNW_TX_SCRIPT_EX_FILE 			".\\dnw\\dnwTxScript_Ex2.txt" // 2016.10.6

#define DNW_TX_SCRIPT_BLT1_FILE 		".\\dnw\\dnwTxScript_BLT1.txt" // 2016.10.7
#define DNW_TX_SCRIPT_BLT2_FILE 		".\\dnw\\dnwTxScript_BLT2.txt" // 2016.10.7
//#define DNW_TX_SCRIPT_FUNC_FILE 		".\\dnw\\dnwTxScript_FUNC.txt" // 2016.10.7
#define DNW_TX_SCRIPT_FUNC_FILE 		".\\dnw\\dnwTxScript_Sample.txt" // 2016.10.7
#define DNW_TX_SCRIPT_AVN_FILE 			".\\dnw\\dnwTxScript_AVN.txt" // 2020.03.13

#define BAUD_RATE_NUM 		22


const int baudTable[BAUD_RATE_NUM]={
		921600,     /* 1, CBR_921600  -> MediaTek MT2712 Serial bard-rate */
		576000, 	/* 2, IDC_RADIO576000 */
		460800,     /* 3, CBR_460800 */
		380400,     /* 4, CBR_380400 */
		312500,     /* 5, added 2011.1.27 */
		CBR_256000, /* 6, 256000 */
		230400,     /* 7 */
		153600,     /* 8, added 2011.1.27, TOYOTA Vehicle : AVC-LAN comm */
		CBR_128000, /* 9 */
		CBR_115200, /* 10 */
        76800,      /* 11 */
		CBR_57600,  /* 12 */
		CBR_56000,  /* 13 */
		41600,      // 14  // --2017.03.09, -CAN---
		CBR_38400,  /* 15 */
		31250,      /* 16, added 2011.1.27 */
		CBR_19200,  /* 17, */
		CBR_14400,  /* 18, */
		10400,      // 19, -- // --2017.03.09, -CAN---
		CBR_9600,   /* 20, */
		CBR_4800,   /* 21, */
		CBR_2400,	/* 22, */
		// CBR_110,CBR_300,CBR_600,CBR_1200,
	};



#define MAX_BAUD_RATE 		baudTable[0]

#define DATE_LEN 			255 // 32




/* +++++ 2020.09.04 ++++++ */

// Flow control flags
#define FC_DTRDSR       0x01   /* 데이터 단말기(DTR) 대기,데이터 세트(DSR) 대기를 위한 신호 */
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04

// ascii definitions
#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13


#endif //__DNSTATUS_H__


