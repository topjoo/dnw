#ifndef __DNW_H__
#define __DNW_H__



#define TX_DATA_LENGTH 			MAX_PATH // 2016.03.23


#define LOG_ERR(format, ...) 		fprintf(stderr, format "\n", ## __VA_ARGS__)
#define LOG_INF(format, ...)  		fprintf(stdout, format "\n", ## __VA_ARGS__)



#if 0 // 2017.08.01, move to Version.h
// ----------------------------------------------
// DNW Tool Version
// ----------------------------------------------
#define APP_DNW 		TEXT("DNW ")
#define APP_AUTHOR 		TEXT(" - TOP.JOO")
//#define APP_AUTHOR 		TEXT(" - tj")
#ifdef CUSTOMER_MODE
#define APP_VER 		TEXT("v7.52C")  
#define APPNAME 		(APP_DNW APP_VER APP_AUTHOR) /* -- SMDKC100 CUSTOMER_MODE on !! AutoSend Downlloading ¿ë -- */
#else
#define APP_VER 		TEXT("v7.52E")  
#define APPNAME 		(APP_DNW APP_VER APP_AUTHOR) /* -- ENGINEER_MODE on !! Engineer ¿ë -- */
#endif

#define APPTITLE 		APP_DNW APP_VER TEXT(" - ")

#else

#include "Version.h"

#endif



enum {
	UBOOT_NONE = 0,
	UBOOT_SHIFT_F7,
	UBOOT_SHIFT_F8,
	UBOOT_SHIFT_F9,
	UBOOT_SHIFT_F10,
	UBOOT_SHIFT_F11,

	UBOOT_SHIFT_ALL, // F4
	UBOOT_TIMER,
	UBOOT_OTHER,
	
	UBOOT_SHIFT_MAX = 0xff
};

enum {
	MSG_OFF = 0,
	MSG_ON,

	MSG_MAX = 0xff
};


#if USE_TXDATA_TO_SERIAL // 2016.10.08
enum {
	TX_TYPE_CHAR = 0,    // TxData Char type to UART
	TX_TYPE_HEXA,        // TxData Hexa-decimal type to UART
	TX_TYPE_FILE,        // TxData File type to UART
	TX_TYPE_SCRIPT = 3,  // TxData Script type to UART
	TX_TYPE_RANDOM = 4,  // 2018.03.23 TxData Random number Generator
	TX_TYPE_SEQUENTIAL,  // 2018.03.26 TxData Sequential number Generator
	
	TX_TYPE_MAX = 0xFF
};
#endif


// Flow Control 2017.08.03
#define FLOWC_MASK          0x000F
#define FLOWC_DTR_DSR 		0x0001
#define FLOWC_RTS_CTS 		0x0002 
#define FLOWC_XON_XOFF 		0x0004 


// 2018.03.22
#define HEXA_DISP_LOWERCASE     0x0001
#define HEXA_DISP_COMMA         0x0002
#define HEXA_DISP_ADD0X         0x0004


// 2017.04.22
#define BAR_NONE 			0
#define BAR_STATUS 			1
#define BAR_COMMAND 		2


#define COM_PORT_TIMEOUT 				1 /// 2014.08.12

#define WAIT_TIME_USB_DOWNLOAD 			5 // 2015.11.30 <- 10 msec


#define HISTORYMENUID 			41000
#define MAXHISTORY 				24
#define FILENAMELEN				256

#define ADDR_LENGTH 			16

#define X_DIFF_CAL				8
#define Y_DIFF_CAL				50


#if 1 /// 2012.05.23
#define THREAD_STACK_SIZE 		(4*1024) // 2020.09.10, 0x4000  /* 16KB, LSI original */
#else
#define THREAD_STACK_SIZE 		0x4000  /* 16KB */
#endif


#define TX_SIZE 			2048 // UART Tx Buffer size
//TX_SIZE must be less than 4096



// ----------------------------------------------
// Colors
// ----------------------------------------------
#define COLOR_BLACK				RGB(0,0,0)
#define COLOR_BLACK1			RGB(40,40,40)
#define COLOR_BLUE				RGB(0,0,255)
#define COLOR_RED				RGB(255,0,0)
#define COLOR_GREEN				RGB(0,255,0)
#define COLOR_GREEN2			RGB(0,190,0)
#define COLOR_WHITE				RGB(255,255,255)
#define COLOR_YELLOW			RGB(255,255,0)
#define COLOR_YELLOW2			RGB(255,230,0)
#define COLOR_CYAN				RGB(0x00,0xff,0xff)
#define COLOR_MAGENTA			RGB(0xff,0x00,0xff)
#define COLOR_TRANSPARENT		RGB(255,0,255)
#define COLOR_J_GRAY			RGB(0x7F,0x7F,0x7F)
#define COLOR_GRAY1				RGB(180,180,180)
#define COLOR_GRAY				RGB(192,192,192)
#define COLOR_GRAY2				RGB(205,205,205)
#define COLOR_GRAY3				RGB(215,215,215)
#define COLOR_GRAY4				RGB(225,225,225)
#define COLOR_GRAY5				RGB(235,235,235)
#define COLOR_GRAY6				RGB(245,245,245)
#define COLOR_GRAY7				RGB(240,240,240)
//#define COLOR_GRAY				RGB(175,175,175)
#define COLOR_GRAY0				RGB(100,100,100)
#define COLOR_GRAY9				RGB(180,180,180)
#define COLOR_NULL				-1

#define COLOR_NUM_MAX 			9





// ----------------------------------------------
// SMDKC100 upgrade interface protocol ---
// ----------------------------------------------
#define MSG_DNW_BEGIN_TXT          TEXT("$$BGN@$")
#define MSG_DNW_END_TXT            TEXT("$$END@$")
#define MSG_DNW_NK_FIN_TXT         TEXT("$$NKB@$")
#define MSG_DNW_EBOOT_FIN_TXT      TEXT("$$EBT@$") 
#define MSG_DNW_STEPLDR_FIN_TXT    TEXT("$$STL@$")  
#define MSG_DNW_SCR_CLEAR_TXT      TEXT("$$CLR@$")  


#define MSG_DNW_BEGIN_LEN          7 /// strlen(MSG_DNW_BEGIN_TXT)
#define MSG_DNW_END_LEN            7 /// strlen(MSG_DNW_END_TXT)
#define MSG_DNW_NK_FIN_LEN         7 /// strlen(MSG_DNW_NK_FIN_TXT)
#define MSG_DNW_EBOOT_FIN_LEN      7 /// strlen(MSG_DNW_EBOOT_FIN_TXT) 
#define MSG_DNW_STEPLDR_FIN_LEN    7 /// strlen(MSG_DNW_STEPLDR_FIN_TXT)  
#define MSG_DNW_SCR_CLEAR_LEN      7 /// strlen(MSG_DNW_SCR_CLEAR_TXT)  


#define DNW_CLR 		0x0001
#define DNW_BGN 		0x0002
#define DNW_END 		0x0004
#define DNW_NK 			0x0010
#define DNW_EBOOT		0x0020
#define DNW_STEP 		0x0040

#define DNW_OTHER 		0x0800
#define DNW_ING 		0x8000

#define TOTAL_BIN_OK    (DNW_NK | DNW_EBOOT | DNW_STEP)




// ----------------------------------------------
// DNW Windows Screen Size Definition
// ----------------------------------------------
#define EDIT_SCREEN_X 			(8*130) // 2018.08.17,  (8*100)
#define EDIT_SCREEN_Y 			(16*52) // 2018.08.17,  (16*34)


#define SCREEN_X    (EDIT_SCREEN_X+2+16+2) //border(1+1)+scroll_bar(16)+space(1+1)
#define SCREEN_Y    (EDIT_SCREEN_Y+2+16+2) //border(1+1)+scroll_bar(16)+space(1+1)
/*
#define WINDOW_XSIZE (SCREEN_X+6)
#define WINDOW_YSIZE (SCREEN_Y+6+38) 
*/
#define WINDOW_XSIZE (SCREEN_X+8)
#define WINDOW_YSIZE (SCREEN_Y+8+38) 

// 2016.03.31
#define WIN_LIMIT_XX 			1200
#define WIN_LIMIT_YY 			700
#define WIN_LIMIT_X_MARGIN 		300
#define WIN_LIMIT_Y_MARGIN 		100


BOOL MenuConnect(HWND hwnd);
BOOL MenuConnectDisconnect(HWND hwnd);
void MenuDisconnect(HWND hwnd);
void MenuTransmit(HWND hwnd);
void MenuOptions(HWND hwnd);
int MenuHex2Bin(HWND hwnd);
int MenuTxData2UART(HWND hwnd); // 2016.03.27

#if USE_FLOAT2HEX_VALUE // 2018.07.18
int MenuFloat2Hexa(HWND hwnd); // 2018.07.18
#endif


#if (RICHED_COLOR) || (TEXT_FIND_MARK) // 2016.09.23
int MenuLogTextColor(HWND hwnd);
#endif
void MenuAbout(HWND hwnd);
void Quit(HWND hwnd);

void UpdateWindowTitle(void);

BOOL OpenComPort(int port, int msg_onoff);
void CloseComPort(void);

void DoRxTx(void *args); 
int ReadCommBlock(TCHAR *buf,int maxLen); // char -> TCHAR
void TxFile(void *args);
void TxFileNew(void *args);
extern BOOL isFileExist(TCHAR *file);

void WriteCommBlock(char c);
void WriteCommBlockArray(char *szC, DWORD szSize); // 2016.10.13

void GMTTime2LocalTime(SYSTEMTIME gmtTime, SYSTEMTIME *local);
void LocalTime2GMTTime(SYSTEMTIME LocalTime, SYSTEMTIME *gmt);

LONG CALLBACK UnhandledExceptionHandler(EXCEPTION_POINTERS *lpExceptionInfo); // 2016.10.15


enum {
	HEXA_OUT_MODE = 0,
	TEXT_OUT_MODE = 1,

	MAX_OUT_MODE = 0x7f
};




#ifdef APPRUN_FILES_TO_ENCIPHER // 2016.05.27 by top.joo

#pragma pack(1)

// ------------------------------------------
// Application/DLL/Wave File Merged --------
// ------------------------------------------
#define BUFFER_MAX 					32
#define FILENAME_LEN 				32
#define TOTAL_SEARCH_FILE_NUM 		100

typedef struct{
	DWORD mergedTotalNum;
	char  mergedModel[BUFFER_MAX];
	char  mergedVersion[BUFFER_MAX];
} APPRUN_ROOT_INFO;

typedef struct{
	DWORD      mergedIndex;
	char       mergedFileName[FILENAME_LEN];
	DWORD      mergedFileSize;
	SYSTEMTIME mergedSysTime;
} APPRUN_MERGED_INFO;

#pragma pack()

#endif



#endif //__DNW_H__



