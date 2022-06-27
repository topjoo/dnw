#ifndef __RWBULK_H__
#define __RWBULK_H__

#include <windows.h>
#include <setupapi.h>

#ifdef __cplusplus
extern "C" {
#endif

HANDLE OpenOneDevice (
    IN       HDEVINFO                    HardwareDeviceInfo,
    IN       PSP_INTERFACE_DEVICE_DATA   DeviceInfoData,
    IN       char *devName
    );

HANDLE OpenUsbDevice( LPGUID  pGuid, char *outNameBuf);

BOOL GetUsbDeviceFileName( LPGUID  pGuid, char *outNameBuf);

HANDLE open_dev(VOID);

HANDLE open_file( char *filename, int err_count, BYTE funcKey );

int  dumpUsbConfig(void);





#define USB_PIPE_SIZE 		32

extern char inPipe[USB_PIPE_SIZE];
extern char outPipe[USB_PIPE_SIZE]; 
extern DWORD USB_TX_SIZE;


#define 	USB_OPEN_RETRY 		10 /// 2011.12.04, 500  /// added. USB port open retry!!!

#ifdef __cplusplus
}
#endif

#endif //__RWBULK_H__



