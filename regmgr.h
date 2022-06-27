//================================================
// Name: registry manager file 
// Programmer: yhjoo87
// Description: Header file for regmgr.cpp
// ================================================

#ifndef __REGMGR_H__
#define __REGMGR_H__

extern void GetComPortRegistry(void); // 2012.07.07
extern void GetRegistry(void);
extern void SetRegistry(void); // void *args);
//extern BOOL ThreadSetRegistry(void *args); // Thread for SetRegistry


#define REG_COM_PORT_AVAIL 		TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM")
#define REG_COM_PORT_KEY 		TEXT("\\Device\\VCP")
#define REG_COM_PORT_KEY0 		TEXT("\\Device\\VCP0")
#define REG_COM_PORT_KEY1 		TEXT("\\Device\\VCP1")
#define REG_COM_PORT_KEY2 		TEXT("\\Device\\VCP2")
#define REG_COM_PORT_KEY3 		TEXT("\\Device\\VCP3") 
#define REG_COM_PORT_KEY4 		TEXT("\\Device\\VCP4") 
#define REG_COM_PORT_KEY5 		TEXT("\\Device\\VCP5") 
#define REG_COM_PORT_KEY6 		TEXT("\\Device\\VCP6") 
#define REG_COM_PORT_KEY7 		TEXT("\\Device\\VCP7") 
#define REG_COM_PORT_KEY8 		TEXT("\\Device\\VCP8") 
#define REG_COM_PORT_KEY9 		TEXT("\\Device\\VCP9") 


#define REG_LOC 					TEXT("Software\\DNW")

#define REG_X_TEXT 					TEXT("V_WXsize")
#define REG_Y_TEXT 					TEXT("V_WYsize")
#define REG_AUTO_TEXT 				TEXT("V_AutoKeyOnOff")
#define REG_MSG_SAVE_TEXT 			TEXT("V_MsgSaveOnOff")

#define REG_COM_Port_TEXT 			TEXT("V_SerialPort")
#define REG_B_RATE_IDX_TEXT 		TEXT("V_BaudRateIdx")
#define REG_BAUDRATE_TEXT 			TEXT("V_BaudRate")

#define REG_COLOR_TYPE				TEXT("V_ColorType")
#define REG_FONT_TYPE				TEXT("V_FontType")


#define REG_RETRY_USB           	TEXT("RetryUSBPort")
#define REG_X_START					TEXT("V_WXstart")
#define REG_Y_START					TEXT("V_WYstart")

/* main window */
#define REG_MAIN_XPOS				TEXT("V_MXPOS")
#define REG_MAIN_YPOS				TEXT("V_MYPOS")
#define REG_MAIN_WID				TEXT("V_MWID")
#define REG_MAIN_HEI				TEXT("V_MHEI")

#define REG_SAMSUNG_CPU				TEXT("SAMSUNG_CPU")
#define REG_DNW_COMPORT				TEXT("DNW_COMPORT")
#define REG_DNW_BAUDRATE			TEXT("DNW_BAUDRATE_Index") /// Baudrate index
#define REG_DNW_USRBAUD				TEXT("DNW_BAUDRATE") /// Baudrate

#define REG_DNW_COM_PARITY			TEXT("DNW_COMParity")
#define REG_DNW_COM_STOPBIT			TEXT("DNW_COMStopBit")
#define REG_DNW_COM_DATABIT			TEXT("DNW_COMDataBit")

#define REG_DNW_COM_FLOWCTRL		TEXT("DNW_COMFLOWCTRL") // 2017.08.03, COM Port Flow Control
#define REG_DNW_COM_FLOWCTRL_VAL	TEXT("DNW_COMFLOWCTRL_VAL") // 2017.08.04, COM Port Flow Control

#define REG_DNW_HEXA_DISP_VAL	    TEXT("DNW_HEXA_DISP_VAL") // 2018.03.22, Hexa Display

#define REG_DNW_SCR_BUF_SIZE 		TEXT("DNW_SCR_BUFSIZE")


#define REG_DNW_USB_STR_ADDR		TEXT("DNW_USB_ADDR_TXT")
#define REG_DNW_USB_ADDR_DWORD		TEXT("DNW_USB_ADDR_DWORD") // 6410 or C100

#define REG_DNW_USB_STR_O_ADDR		TEXT("DNW_USB_ADDR_O_TXT") // Other
#define REG_DNW_USB_ADDR_OTHER		TEXT("DNW_USB_ADDR_OTHER") // USB addr of other CPU

#define REG_TIME_INFO				TEXT("DNW_TIME_INFO") // 2015.11.13

#define REG_LINEFEED				TEXT("DNW_CR_LF") // 2019.12.21

#define REG_LOGFILE_TYPE			TEXT("DNW_LOGFILE_TYPE") // 2019.12.21

#define REG_OUT_MODE				TEXT("DNW_OUTMODE_TXT_HEX")


#define REG_AUTHOR2_TEXT 			TEXT("DNW:Author2")
#define REG_AUTHOR2_ID_TEXT         TEXT("mobilesol.cs@samsung.com")
#define REG_BUILDING2_TEXT 			TEXT("DNW:GiveDateTime(LSI Ahn)")

#define REG_AUTHOR1_TEXT 			TEXT("DNW:Author1")
#define REG_AUTHOR1_ID_TEXT         TEXT("tp.joo@daum.net or toppy_joo@naver.com")
//#define REG_AUTHOR1_ID_TEXT         TEXT("toppy_joo@naver.com")
#define REG_BUILDING1_TEXT 			TEXT("DNW:BuildDateTime")


// Hex file type
#define REG_HEX2BIN_HEX_TYPE 		TEXT("DNW_HEX2BIN_TYPE")

#define REG_TX_DATA_MODE 			TEXT("DNW_TX_DATAMODE")
#define REG_TX_CHAR_DAT 			TEXT("DNW_TX_CHAR")
#define REG_TX_HEXA_DAT 			TEXT("DNW_TX_HEXA")
#define REG_TX_FILE_DAT 			TEXT("DNW_TX_FILE")
#define REG_TX_MSG_ONOFF 			TEXT("DNW_TX_MSGONOFF")

// Float 2 hexa
#define REG_FLT2HEX_IN1 			TEXT("DNW_FLT2HEX_IN1")  // 1st
#define REG_FLT2HEX_IN2 			TEXT("DNW_FLT2HEX_IN2")  // 2nd
#define REG_FLT2HEX_IN3 			TEXT("DNW_FLT2HEX_IN3")  // step
#define REG_FLT2HEX_ENDIAN 			TEXT("DNW_FLT2HEX_ENDIAN")  // float to heaxa endian



// Log On/Off
#define REG_HEX2BIN_LOG 			TEXT("DNW_HEX2BIN_LOG")

// Swap mode
#define REG_HEX2BIN_SWAP 			TEXT("DNW_HEX2BIN_SWAP")

#define REG_HEX2BIN_PADBYTE_ON 		TEXT("DNW_HEX2BIN_PAD_BYTE") // 2018.07.06

// CRC Type
#define REG_HEX2BIN_CRCTYPE 		TEXT("DNW_HEX2BIN_CRCTYPE")
	
// Endian
#define REG_HEX2BIN_ENDIAN 			TEXT("DNW_HEX2BIN_ENDIAN")

// Hex Address Zero-Forced!
#define REG_HEX2BIN_ZERO_FORCECD 	TEXT("DNW_HEX2BIN_ADDR_ZERO")

// CRC Verify
#define REG_HEX2BIN_CRCVERIFY 		TEXT("DNW_HEX2BIN_CRCVERIFY")

// File size Unit
#define REG_HEX2BIN_SIZEUNIT 		TEXT("DNW_HEX2BIN_SIZEUNIT")

#define REG_HEX2BIN_MAX_FILESIZE 	TEXT("DNW_HEX2BIN_FILESIZE")
#define REG_HEX2BIN_PADBYTE 		TEXT("DNW_HEX2BIN_PADBYTE")

#define REG_HEX2BIN_INFILE 			TEXT("DNW_HEX2BIN_INFILE")
#define REG_HEX2BIN_INFILE_SHORT 	TEXT("DNW_HEX2BIN_INFILESH")

#define REG_HEX2BIN_OUTFILE 		TEXT("DNW_HEX2BIN_OUTFILE")
#define REG_HEX2BIN_OUTFILE_SHORT 	TEXT("DNW_HEX2BIN_OUTFILESH")

#define REG_HEX2BIN_ADDR_WORD_ALIGN 	TEXT("DNW_HEX2BIN_WORD_ALIGN")

#define REG_HEXA_COLUMN 			TEXT("DNW_HEXA_COLUMN")
#define REG_HEXA_ENDCHAR 			TEXT("DNW_HEXA_ENDCHAR")

#define REG_COLOR_RED				TEXT("DNW_LOGCOL_RED") 
#define REG_COLOR_MAGENTA			TEXT("DNW_LOGCOL_MAGENTA") 
#define REG_COLOR_BLUE				TEXT("DNW_LOGCOL_BLUE") 
#define REG_COLOR_GREEN				TEXT("DNW_LOGCOL_GREEN") 
#define REG_COLOR_YELLOW			TEXT("DNW_LOGCOL_YELLOW") 


#define REG_TEXT_RED				TEXT("DNW_TEXT_RED") 
#define REG_TEXT_MAGENTA			TEXT("DNW_TEXT_MAGENTA") 
#define REG_TEXT_BLUE				TEXT("DNW_TEXT_BLUE") 
#define REG_TEXT_GREEN				TEXT("DNW_TEXT_GREEN") 
#define REG_TEXT_YELLOW				TEXT("DNW_TEXT_YELLOW") 

#define REG_STATUS_BAR_EN 			TEXT("DNW_STATUSBAR") 
#endif /* __REGMGR_H__ */




