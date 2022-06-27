
#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <string.h>
#include <commdlg.h>

#include "resource.h"

#include "def.h"
#include "dnw.h"
#include "engine.h"
#include "fileopen.h"

TCHAR szFileNameOnPopUP[FILENAMELEN] = {0,};

TCHAR szFileName[FILENAMELEN] = {0,},      szTitleName[FILENAMELEN] = {0,};
TCHAR szEBOOTFileName[FILENAMELEN] = {0,}, szEBOOTTitleName[FILENAMELEN] = {0,};  
TCHAR szUBOOTFileName[FILENAMELEN] = {0,}, szUBOOTTitleName[FILENAMELEN] = {0,};  
TCHAR szUmonFileName[FILENAMELEN] = {0,},  szUmonTitleName[FILENAMELEN] = {0,};  /// added.
    //szTitleName doesn't affect the GetOpenFileName();
    //Only szFileName is important to GetOpenFileName() to 
			

static OPENFILENAME ofn ;  // open file name
static OPENFILENAME sfn ;  // save file name

void PopFileInitialize (HWND hwnd)
{
     static TCHAR szFilter[] =	
	 		TEXT ("BIN Files (*.bin;*.nb0;*.lst;*.ubi;*.dio)\0*.bin;*.nb0;*.lst;*.ubi;*.dio\0")  \
			TEXT ("All Files (*.*)\0*.*\0\0") ;

 	 memset( &ofn, 0x00, sizeof(OPENFILENAME) ); /// 2013.04.21

     ofn.lStructSize       = sizeof (OPENFILENAME);
     ofn.hwndOwner         = hwnd ;
     ofn.hInstance         = NULL ;
     ofn.lpstrFilter       = szFilter;
     ofn.lpstrCustomFilter = NULL ;
     ofn.nMaxCustFilter    = 0 ;
     ofn.nFilterIndex      = 0 ;
     ofn.lpstrFile         = NULL ;          // Set in Open and Close functions
     ofn.nMaxFile          = MAX_PATH ;
     ofn.lpstrFileTitle    = NULL ;          // Set in Open and Close functions
     ofn.nMaxFileTitle     = MAX_PATH ;
     ofn.lpstrInitialDir   = NULL ;
     ofn.lpstrTitle        = NULL ;
     ofn.Flags             = 0 ;             // Set in Open and Close functions
     ofn.nFileOffset       = 0 ;
     ofn.nFileExtension    = 0 ;
     ofn.lpstrDefExt       = TEXT ("bin") ;
     ofn.lCustData         = 0L ;
     ofn.lpfnHook          = NULL ;
     ofn.lpTemplateName    = NULL ;
}



BOOL PopFileOpenDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
     ofn.hwndOwner         = hwnd ;
     ofn.lpstrFile         = pstrFileName ;
     ofn.lpstrFileTitle    = pstrTitleName ;
     ofn.Flags             = OFN_HIDEREADONLY | OFN_CREATEPROMPT ;
     
     return GetOpenFileName (&ofn) ;
}




BOOL PopFileMultiOpenDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName) //, PTSTR pfilTitName)
{
	static TCHAR szCheckSumFilter[] =  
		   TEXT ("BIN Files (*.lgu;*.bin;*.nb0)\0*.lgu;*.bin;*.nb0\0")	\
		   TEXT ("ZIP Files (*.zip;*.egg;*.alz;*.tar;*.tgz;*.lzh)\0*.zip;*.egg;*.alz;*.tar;*.tgz;*.lzh\0")  \
		   TEXT ("All Files (*.*)\0*.*\0\0") ;


	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = hwnd ;
	ofn.lpstrFilter	   = szCheckSumFilter; // 2016.1.27 added.
	ofn.lpstrFile         = pstrFileName ;
	ofn.lpstrFileTitle    = pstrTitleName ;

	//ofn.nMaxFile          = 2048;
	//ofn.nMaxFile          = sizeof(pstrFileName); // ¹®ÀÚ °¹¼ö
	//ofn.lpstrTitle        = pfilTitName; // "Select..";

	ofn.Flags             = OFN_HIDEREADONLY | OFN_EXPLORER | OFN_CREATEPROMPT; //  OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT ;

	return GetOpenFileName (&ofn) ;
}




#if USE_HEX2BIN_CONVERT // 2016.03.04
BOOL PopFileHex2BinOpenDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
	static TCHAR szCheckSumFilter[] =  
		   TEXT ("HEX Files (*.hex;*.s19;*.s28;*.s37;*.srec;*.sre;*.rec)\0*.hex;*.s19;*.s28;*s37;*.srec;*.sre;*.rec\0")	\
		   TEXT ("All Files (*.*)\0*.*\0\0") ;

	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = hwnd ;
	ofn.lpstrFilter	   = szCheckSumFilter; // 2016.1.27 added.
	ofn.lpstrFile         = pstrFileName ;
	ofn.lpstrFileTitle    = pstrTitleName ;

	//ofn.nMaxFile          = 2048;
	//ofn.nMaxFile          = sizeof(pstrFileName); // ¹®ÀÚ °¹¼ö
	ofn.lpstrTitle        = TEXT("Select file for converting hexa to binary...");

	ofn.Flags             = OFN_HIDEREADONLY | OFN_EXPLORER | OFN_CREATEPROMPT; //  OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT ;

	return GetOpenFileName (&ofn) ;
}

#endif



#if USE_TXDATA_TO_SERIAL // 2016.03.28

BOOL PopFileTxDataOpenDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
	static TCHAR szCheckSumFilter[] =  
		   TEXT ("All Files (*.*)\0*.*\0\0") ;
	

	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = hwnd ;
	ofn.lpstrFilter	      = szCheckSumFilter; // 2016.1.27 added.
	ofn.lpstrFile         = pstrFileName ;
	ofn.lpstrFileTitle    = pstrTitleName ;

	//ofn.nMaxFile          = 2048;
	//ofn.nMaxFile          = sizeof(pstrFileName); // ¹®ÀÚ °¹¼ö
	ofn.lpstrTitle        = TEXT("Select Tx file or script file...");

	ofn.Flags             = OFN_HIDEREADONLY | OFN_EXPLORER | OFN_CREATEPROMPT | OFN_FILEMUSTEXIST; // | OFN_ALLOWMULTISELECT ;

	return GetOpenFileName (&ofn) ;
}

#endif



void PopFileSaveInitialize (HWND hwnd)
{
     static TCHAR szFilter[] =	
	 		TEXT ("LOG Files (*.LOG)\0*.LOG\0\0");

 	 memset( &sfn, 0x00, sizeof(OPENFILENAME) ); 

	 //ZeroMemory(&sfn, sizeof(sfn)); // 2020.08.27 Fix

     sfn.lStructSize       = sizeof (OPENFILENAME);
     sfn.hwndOwner         = hwnd ;
     sfn.hInstance         = NULL ;
     sfn.lpstrFilter       = szFilter;
     sfn.lpstrCustomFilter = NULL ;
     sfn.nMaxCustFilter    = 0 ;
     sfn.nFilterIndex      = 0 ;
     sfn.lpstrFile         = NULL ;          // Set in Open and Close functions
     sfn.nMaxFile          = MAX_PATH ;
     sfn.lpstrFileTitle    = NULL ;          // Set in Open and Close functions
     sfn.nMaxFileTitle     = MAX_PATH ;
     sfn.lpstrInitialDir   = NULL ;
     sfn.lpstrTitle        = NULL ;
     sfn.Flags             = 0 ;             // Set in Open and Close functions
     sfn.nFileOffset       = 0 ;
     sfn.nFileExtension    = 0 ;
     sfn.lpstrDefExt       = TEXT ("LOG") ;
     sfn.lCustData         = 0L ;
     sfn.lpfnHook          = NULL ;
     sfn.lpTemplateName    = NULL ;
}


BOOL PopFileMultiSaveDlg (HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName, PTSTR pfilTitName)
{
	BOOL bRet = FALSE;
	
	static TCHAR szCheckSumFilter[] =  
		   TEXT ("LOG Files (*.LOG)\0*.LOG\0\0");


    ZeroMemory(&sfn, sizeof(sfn)); // 2020.08.27 Fix

	sfn.lStructSize       = sizeof(OPENFILENAME);
	sfn.hwndOwner         = hwnd ;
	sfn.lpstrFilter	      = szCheckSumFilter; // 2016.1.27 added.
	sfn.lpstrFile         = pstrFileName ;
	sfn.lpstrFileTitle    = pstrTitleName ;

	sfn.nMaxFile          = MAX_PATH;
	//sfn.nMaxFile          = sizeof(pstrFileName); // ¹®ÀÚ °¹¼ö
	sfn.lpstrTitle        = pfilTitName; // "Select..";
	// 2020.08.27 Fix, sfn.lpstrInitialDir   = "." ;
	sfn.lpstrDefExt 	  = TEXT("LOG") ;  // 2020.08.27 Fix

	sfn.Flags             = OFN_EXPLORER | OFN_CREATEPROMPT; //  OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT ;


#if 0
	return GetSaveFileName (&sfn) ;

#else
	bRet = GetSaveFileName (&sfn) ;

	Sleep(200);

	return bRet;
#endif

}


