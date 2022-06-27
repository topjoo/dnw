#ifndef __FILEOPEN_H__
#define __FILEOPEN_H__

void PopFileInitialize (HWND hwnd);
BOOL PopFileOpenDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName);
BOOL PopFileMultiOpenDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName); // , PTSTR pfilTitName);

#if USE_HEX2BIN_CONVERT // 2016.03.04
BOOL PopFileHex2BinOpenDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName);
#endif

#if USE_TXDATA_TO_SERIAL // 2016.03.28
BOOL PopFileTxDataOpenDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName);
#endif

void PopFileSaveInitialize (HWND hwnd);
BOOL PopFileMultiSaveDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName, PTSTR pfilTitName);



//void WriteCommBlock(char *lpByte, DWORD dwBytesWrite);

extern TCHAR szFileName[FILENAMELEN],szTitleName[FILENAMELEN];
extern TCHAR szEBOOTFileName[FILENAMELEN],szEBOOTTitleName[FILENAMELEN];  
extern TCHAR szUBOOTFileName[FILENAMELEN],szUBOOTTitleName[FILENAMELEN];  
extern TCHAR szUmonFileName[FILENAMELEN],szUmonTitleName[FILENAMELEN]; /// added.
#endif //__FILEOPEN_H__


