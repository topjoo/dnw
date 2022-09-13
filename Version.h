
#ifndef __DNW_TOP_JOO_VERSION__
#define __DNW_TOP_JOO_VERSION__


// _MSC_VER
//Visual C++ 8.0 (Visual Studio 2005)  1400
//Visual C++ 9.0 (Visual Studio 2008)  1500
//Visual C++ 10.0 (Visual Studio 2010) 1600
//Visual C++ 11.0 (Visual Studio 2012) 1700
//Visual C++ 12.0 (Visual Studio 2013) 1800
//Visual C++ 14.0 (Visual Studio 2015) 1900


#if _MSC_VER == 1400
#define BUILD_MSC_VER 	TEXT("/VS2005")
#elif _MSC_VER == 1500
#define BUILD_MSC_VER 	TEXT("/VS2008")
#elif _MSC_VER == 1600
#define BUILD_MSC_VER 	TEXT("/VS2010")
#elif _MSC_VER == 1700
#define BUILD_MSC_VER 	TEXT("/VS2012")
#elif _MSC_VER == 1800
#define BUILD_MSC_VER 	TEXT("/VS2013")
#elif _MSC_VER == 1900
#define BUILD_MSC_VER 	TEXT("/VS2015")
#elif _MSC_VER == 1910
#define BUILD_MSC_VER 	TEXT("/VS2017")
#elif _MSC_VER == 1920
#define BUILD_MSC_VER 	TEXT("/VS2019")
#elif _MSC_VER == 1930
#define BUILD_MSC_VER 	TEXT("/VS2022")
#elif _MSC_VER == 1933
#define BUILD_MSC_VER 	TEXT("/VS2022")
#else
#define BUILD_MSC_VER 	TEXT("/VS202?")
#endif


// ----------------------------------------------
// DNW Tool Version
// ----------------------------------------------
#define APP_DNW 			TEXT("DNW ")
#define APP_AUTHOR 			TEXT(" - TOP.JOO")
//#define APP_AUTHOR 		TEXT(" - tj")

#define DNW_FILEVERSION 	8,02
#define DNW_PRODUCTVERSION 	8,01,0,0


#ifdef CUSTOMER_MODE
#define APP_VER 		TEXT("v8.02C")  
#define APPNAME 		(APP_DNW APP_VER APP_AUTHOR) /* -- SMDKC100 CUSTOMER_MODE on !! AutoSend Downlloading ¿ë -- */
#else
#define APP_VER 		TEXT("v8.02E")  
#define APPNAME 		(APP_DNW APP_VER APP_AUTHOR) /* -- ENGINEER_MODE on !! Engineer ¿ë -- */
#endif

#define APPTITLE 		APP_DNW APP_VER TEXT(" - ")

#endif // __DNW_TOP_JOO_VERSION__



