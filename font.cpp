// -------------------------------------------
// Font.cpp
// -------------------------------------------
#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <string.h>

#include "resource.h"

#include "def.h"
#include "dnw.h"
#include "engine.h"

static LOGFONT logFont;
static HFONT hFont = NULL;
extern int FontType;

void SetFont(HWND hwndEdit)
{

#if 1
    //TEXTMETRIC tm;
	static struct {
		int     idStockFont;
		TCHAR * szStockFont;
	} stockfont [] = { 
					OEM_FIXED_FONT,      TEXT ("OEM_FIXED_FONT"),
                    ANSI_FIXED_FONT,     TEXT ("ANSI_FIXED_FONT"),    
                    DEVICE_DEFAULT_FONT, TEXT ("DEVICE_DEFAULT_FONT"),
                    SYSTEM_FIXED_FONT,   TEXT ("SYSTEM_FIXED_FONT"),
					//ANSI_VAR_FONT,	   TEXT ("ANSI_VAR_FONT"),
					//SYSTEM_FONT,		   TEXT ("SYSTEM_FONT"),
                    //DEFAULT_GUI_FONT,    TEXT ("DEFAULT_GUI_FONT") 
	};



	if( FontType < 0 || FontType > 3 )  { FontType = 3;}
	if( hFont ) { DeleteObject(hFont); hFont = NULL; }


	memset( &logFont, 0x00, sizeof(LOGFONT) );

	GetObject(GetStockObject(stockfont[FontType].idStockFont), sizeof(LOGFONT), (PTSTR)&logFont);

	hFont = CreateFontIndirect(&logFont);

	#if 0
	SendMessage(hwndEdit, WM_SETFONT, (WPARAM)(LPLOGFONT)&logFont, (LPARAM)(BOOL)0);
	#else
	SendMessage(hwndEdit, WM_SETFONT, (WPARAM)hFont, (LPARAM)0);
	#endif

#else

	{	

	LOGFONT lfFont;

	memset( &lfFont, 0x00, sizeof(LOGFONT) );
	lfFont.lfHeight          = 20;//-MulDiv(9, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	lfFont.lfWidth           = 10;
	//lfFont.lfEscapement
	//lfFont.lfOrientation
	lfFont.lfWeight          = FW_HEAVY; //FW_EXTRABOLD  FW_THIN ; FW_SEMIBOLD FW_BOLD; FW_LIGHT // FW_NORMAL; //  | FW_REGULAR | FW_BOLD | FW_DONTCARE;
	lfFont.lfItalic          = FALSE;
	lfFont.lfUnderline       = FALSE;
	lfFont.lfStrikeOut       = FALSE;
	lfFont.lfCharSet         = ANSI_CHARSET; // DEFAULT_CHARSET; // OEM_CHARSET;
	lfFont.lfOutPrecision    = OUT_SCREEN_OUTLINE_PRECIS; // OUT_TT_PRECIS;
	//lfFont.lfClipPrecision
	lfFont.lfQuality         = ANTIALIASED_QUALITY;
	lfFont.lfPitchAndFamily  = VARIABLE_PITCH | FF_ROMAN; // DEFAULT_PITCH | FF_DONTCARE | VARIABLE_PITCH | FF_ROMAN;
	//lfFont.lfFaceName

	hFont = CreateFontIndirect(&lfFont);
	SendMessage(hwndEdit, WM_SETFONT, (WPARAM)(LPLOGFONT)&lfFont, (LPARAM)(BOOL)TRUE);
	}

#endif



}



void ReleaseFont(void)
{
    if(hFont) DeleteObject(hFont);
}




#ifdef __REFER___
	--weights
	FW_DONTCARE         = 0
	FW_THIN             = 100
	FW_EXTRALIGHT       = 200
	FW_LIGHT            = 300
	FW_NORMAL           = 400
	FW_MEDIUM           = 500
	FW_SEMIBOLD         = 600
	FW_BOLD             = 700
	FW_EXTRABOLD        = 800
	FW_HEAVY            = 900
	FW_ULTRALIGHT       = FW_EXTRALIGHT
	FW_REGULAR          = FW_NORMAL
	FW_DEMIBOLD         = FW_SEMIBOLD
	FW_ULTRABOLD        = FW_EXTRABOLD
	FW_BLACK            = FW_HEAVY
	--charsets
	ANSI_CHARSET            = 0
	DEFAULT_CHARSET         = 1
	SYMBOL_CHARSET          = 2
	SHIFTJIS_CHARSET        = 128
	HANGEUL_CHARSET         = 129
	HANGUL_CHARSET          = 129
	GB2312_CHARSET          = 134
	CHINESEBIG5_CHARSET     = 136
	OEM_CHARSET             = 255
	JOHAB_CHARSET           = 130
	HEBREW_CHARSET          = 177
	ARABIC_CHARSET          = 178
	GREEK_CHARSET           = 161
	TURKISH_CHARSET         = 162
	VIETNAMESE_CHARSET      = 163
	THAI_CHARSET            = 222
	EASTEUROPE_CHARSET      = 238
	RUSSIAN_CHARSET         = 204
	MAC_CHARSET             = 77
	BALTIC_CHARSET          = 186
	--styles
	FS_LATIN1               = 0x00000001
	FS_LATIN2               = 0x00000002
	FS_CYRILLIC             = 0x00000004
	FS_GREEK                = 0x00000008
	FS_TURKISH              = 0x00000010
	FS_HEBREW               = 0x00000020
	FS_ARABIC               = 0x00000040
	FS_BALTIC               = 0x00000080
	FS_VIETNAMESE           = 0x00000100
	FS_THAI                 = 0x00010000
	FS_JISJAPAN             = 0x00020000
	FS_CHINESESIMP          = 0x00040000
	FS_WANSUNG              = 0x00080000
	FS_CHINESETRAD          = 0x00100000
	FS_JOHAB                = 0x00200000
	FS_SYMBOL               = 0x80000000
	--output precisions
	OUT_DEFAULT_PRECIS          = 0
	OUT_STRING_PRECIS           = 1
	OUT_CHARACTER_PRECIS        = 2
	OUT_STROKE_PRECIS           = 3
	OUT_TT_PRECIS               = 4
	OUT_DEVICE_PRECIS           = 5
	OUT_RASTER_PRECIS           = 6
	OUT_TT_ONLY_PRECIS          = 7
	OUT_OUTLINE_PRECIS          = 8
	OUT_SCREEN_OUTLINE_PRECIS   = 9
	OUT_PS_ONLY_PRECIS          = 10
	--clip precisions
	CLIP_DEFAULT_PRECIS     = 0
	CLIP_CHARACTER_PRECIS   = 1
	CLIP_STROKE_PRECIS      = 2
	CLIP_MASK               = 0xf
	CLIP_LH_ANGLES          = bit.lshift(1,4)
	CLIP_TT_ALWAYS          = bit.lshift(2,4)
	CLIP_DFA_DISABLE        = bit.lshift(4,4)
	CLIP_EMBEDDED           = bit.lshift(8,4)
	--qualities
	DEFAULT_QUALITY         = 0
	DRAFT_QUALITY           = 1
	PROOF_QUALITY           = 2
	NONANTIALIASED_QUALITY  = 3
	ANTIALIASED_QUALITY     = 4
	CLEARTYPE_QUALITY       = 5
	CLEARTYPE_NATURAL_QUALITY = 6
	--pitches
	DEFAULT_PITCH           = 0
	FIXED_PITCH             = 1
	VARIABLE_PITCH          = 2
	--families
	FF_DONTCARE         = bit.lshift(0,4) -- Don't care or don't know.
	FF_ROMAN            = bit.lshift(1,4) -- Variable stroke width, serifed. Times Roman, Century Schoolbook, etc.
	FF_SWISS            = bit.lshift(2,4) -- Variable stroke width, sans-serifed. Helvetica, Swiss, etc.
	FF_MODERN           = bit.lshift(3,4) -- Constant stroke width, serifed or sans-serifed. Pica, Elite, Courier, etc.
	FF_SCRIPT           = bit.lshift(4,4) -- Cursive, etc.
	FF_DECORATIVE       = bit.lshift(5,4) -- Old English, etc.

#endif



