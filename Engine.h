// ================================================
// Name: engine.h
// Programmer: Inwook,Kong
// Description: Header file for engine.cpp
// ================================================

#ifndef __ENGINE_H__
#define __ENGINE_H__

// Macros
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))


#define TEXT_LEN_MIN_SIZ 			6


#if 0 // deleted at 2016.02.12
// The Engine Class
BOOL Engine_OnCreate(HWND hwnd, CREATESTRUCT FAR* lpCreateStruct);
void Engine_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void Engine_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);
void Engine_OnPaint(HWND hwnd);
void Engine_OnChar(HWND hwnd,WPARAM wParam);
#endif


BOOL CALLBACK ChecksumProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


enum {
	CHECKSUM_UNKNOWN = 0,
	CHECKSUM_MD5 = 1,
	CHECKSUM_SHA1,
	CHECKSUM_CRC32,
	CHECKSUM_CRC64,
	CHECKSUM_SHA2_256,
	CHECKSUM_SHA2_384,
	CHECKSUM_SHA2_512,
	CHECKSUM_SHA2_224,
	CHECKSUM_MD6,

	CHECKSUM_SHA3_KECCAK_224, // 2017.08.22
	CHECKSUM_SHA3_KECCAK_256,
	CHECKSUM_SHA3_KECCAK_384,
	CHECKSUM_SHA3_KECCAK_512,

	CHECKSUM_SHAKE_128, // 2017.08.29
	CHECKSUM_SHAKE_256,

	CHECKSUM_BLAKE224, // 2018.06.15
	CHECKSUM_BLAKE256,
	CHECKSUM_BLAKE384,
	CHECKSUM_BLAKE512,

	CHECKSUM_KR_HAS160, // 2018.06.15
	CHECKSUM_TIGER1995, // 2018.06.15

	CHECKSUM_RIPEMD160,
	
	CHECKSUM_MAX
};



// Variables
// .....
extern HINSTANCE _hInstance;
extern HWND _hwnd;

// Some Procs
BOOL Register(HINSTANCE hInstance);
BOOL Create(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWindow, UINT Message, WPARAM wParam, LPARAM lParam);
//BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam);

extern "C" void EB_Printf(const TCHAR *fmt,...);
extern "C" void EB_PrintfHexa(int iLength, TCHAR *fmtData); /// ,...); // 2016.10.24
extern "C" void EB_PrintfText(int iLength, TCHAR *fmtData); /// ,...); // 2016.10.24


#if DNW_2ND_WINDOWS_ENABLE // 2016.10.24
void StatusBarText(void);
void StatusBarSaveLog(void);

#define STATUS_BAR_HEIGHT 			22 // 20

#define NUM_STATUS_BAR 				6

#define STATUS_IDX0 				0
#define STATUS_IDX1 				1
#define STATUS_IDX2 				2
#define STATUS_IDX3 				3
#define STATUS_IDX4 				4
#define STATUS_IDX5 				5

#define ENTER_KEY 					0x0d
#define DEL_KEY 					0x08 // 2017.05.15



// Text on Status Bar 
#define MSG_LOG_OFF 				TEXT("Display")
#define MSG_LOGGING 				TEXT("Logging...")


#define STA_TX_TEXT 				TEXT("TEXT")
#define STA_TX_HEXA 				TEXT("Hexa")


#define TXT_SCR_HUGE 				TEXT("Scr-Huge")
#define TXT_SCR_LARGE 				TEXT("Scr-Large")
#define TXT_SCR_MIDDLE 				TEXT("Scr-Middle")
#define TXT_SCR_SMALL 				TEXT("Scr-Small")
#define TXT_SCR_SMALLEST 			TEXT("Scr-Smallest")


#define TXT_FCTRL_NONE 				TEXT("None") // 2017.8.03, Flow COntrol None (index 0)
#define TXT_FCTRL_DTR_DTS 			TEXT("DTR/")
#define TXT_FCTRL_DTR_DTS2 			TEXT("DTR")
#define TXT_FCTRL_RTS_CTS 			TEXT("RTS/")
#define TXT_FCTRL_RTS_CTS2 			TEXT("RTS")
#define TXT_FCTRL_XON_XOFF 			TEXT("Xon/")
#define TXT_FCTRL_XON_XOFF2 		TEXT("Xon")



#if KEYIN_HISTORY_SAVE // 2017.5.2
#define TXT_KEYIN_INIT 				TEXT(" Key-in... [F9]:delete/clear, [ENTER]:send and clear, [UP]/[DN]:history")
#else
#define TXT_KEYIN_INIT 				TEXT(" Key-in... [F9]:clear, [ENTER]:send and clear")
#endif

	

#else

#define STATUS_BAR_HEIGHT 			0

#endif



//void EB_Update(void);
//void _ScrBufScroll(void);
//void _ScrCrLf(void);
void ScrClear(HWND hwnd);
void EditBufferClear(HWND hwnd); // 2016.02.12
LRESULT GetEditBufferSize(HWND hwnd); // 2016.02.12



#if 0 // deleted at 2016.02.12
#define CONSOLE_XSIZE	(80)
#define CONSOLE_YSIZE	(24)
#define CHAR_HEIGHT	(16)
#endif


typedef enum _eTimerID {
	TIMER_ID_WIN_UPDATE = 1,
	TIMER_ID_AFTER_BL1 ,
	TIMER_ID_AUTO_DOWNLOAD_KEY_SEND ,
	TIMER_ID_MSG_WRITE_TO_FILE ,

	
	TIMER_ID_MAX
} eTimerID;



#define NK_WAIT_TIME 			2000  /// added.
#define EBOOT_WAIT_TIME 		200  /// added.


#define STRING_LEN  			4096  



#if MODIFIED_JULIAN_DATE /// 2018.08.04
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


#endif //__ENGINE_H__


