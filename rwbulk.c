/*++

    Modified RWBulk.c

--*/

#include "resource.h"

#include <windows.h>

///#include <lusb0_usb.h> /// 2014.1.6
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <memory.h> 
#include "devioctl.h"

#include <initguid.h> 
#include <setupapi.h>
#include <basetyps.h>
#include "BulkUsb.h"
#include "GUID.h"

#include "usbdi.h"
#include "rwbulk.h"
#include "dnw.h" // 2016.03.25

#pragma comment(lib, "setupapi") /// 2012.05.23


void EB_Printf(const TCHAR *fmt,...); //engine.h


#define NOISY(_x_) EB_Printf _x_ ;



/* Pipe
-------------------------------------------------------------------------------
- Bulk Pipe        : 많은 양의 Data 전송, Data 손실이 없어야 하는 경우
- Isochronous Pipe : 많은 양의 Data 전송, 약간의 Data 손실은 허용되나 정확한 시간에 전송되어야 하는 경우
- Interrupt Pipe   : Device의 상태를 알고자 할 때 사용
- Control Pipe     : 적은 양의 Data 전송에 사용
-------------------------------------------------------------------------------
*/

char inPipe[USB_PIPE_SIZE] = "PIPE00";     // pipe name for bulk input pipe on our test board
char outPipe[USB_PIPE_SIZE] = "PIPE01";    // pipe name for bulk output pipe on our test board
char completeDeviceName[512] = "";  // 256-> 512 //generated from the GUID registered by the driver itself


#define TX_SIZE_2 (4096*4*16)  /* 256K */
//#define USB_RX_SIZE (4096*4*16) 
#define TX_SIZE_1 (4096*4)
//#define RX_SIZE (4096*4) 
DWORD	USB_TX_SIZE = TX_SIZE_1; /* initial */
/*
BOOL fDumpUsbConfig = FALSE;    // flags set in response to console command line switches
BOOL fDumpReadData = FALSE;
BOOL fRead = FALSE;
BOOL fWrite = FALSE;


int gDebugLevel = 1;      // higher == more verbose, default is 1, 0 turns off all

ULONG IterationCount = 1; //count of iterations of the test we are to perform
int WriteLen = 0;         // #bytes to write
int ReadLen = 0;          // #bytes to read
*/
// functions


//// #pragma unmanaged  /// added... 2012.05.23


/*++++++++++++++++++++++++++++++++++++++++++++++++++++
Routine Description:

    Given the HardwareDeviceInfo, representing a handle to the plug and
    play information, and deviceInfoData, representing a specific usb device,
    open that device and fill in all the relevant information in the given
    USB_DEVICE_DESCRIPTOR structure.

Arguments:

    HardwareDeviceInfo:  handle to info obtained from Pnp mgr via SetupDiGetClassDevs()
    DeviceInfoData:      ptr to info obtained via SetupDiEnumInterfaceDevice()

Return Value:

    return HANDLE if the open and initialization was successfull,
    else INVLAID_HANDLE_VALUE.

++++++++++++++++++++++++++++++++++++++++++++++++++--*/
HANDLE OpenOneDevice (
	IN		 HDEVINFO					 HardwareDeviceInfo,
	IN		 PSP_INTERFACE_DEVICE_DATA	 DeviceInfoData,
	IN		 char *devName
	)
{
    PSP_INTERFACE_DEVICE_DETAIL_DATA     functionClassDeviceData = NULL;
    ULONG                                predictedLength = 0; /// 2012.05.23 ULONG -> static DWORD -> ULONG
    ULONG                                requiredLength = 0;  /// 2012.05.23 ULONG -> static DWORD -> ULONG
    HANDLE                               hOut = INVALID_HANDLE_VALUE;
	BOOL                                 bStatus;
	static DWORD                         error_code;

//	ZeroMemory(&hidAttributes, sizeof(HIDD_ATTRIBUTES)); /// 2012.05.23 
//	hidAttributes.Size = sizeof(HIDD_ATTRIBUTES);

    //
    // allocate a function class device data structure to receive the
    // goods about this particular device.
    // SetupDiGetInterfaceDeviceDetail() --- Size을 취득 ---
    bStatus = SetupDiGetInterfaceDeviceDetail (
									    HardwareDeviceInfo,
									    DeviceInfoData,
									    NULL, // probing so no output buffer yet
									    0, // probing so output buffer length of zero
									    &requiredLength,
									    NULL); // not interested in the specific dev-node


	///if( FALSE == bStatus )
	///{
	///	if( ERROR_INSUFFICIENT_BUFFER != GetLastError() )
	///	return INVALID_HANDLE_VALUE;
	///}


    predictedLength = requiredLength;
    // sizeof (SP_FNCLASS_DEVICE_DATA) + 512;

    functionClassDeviceData = (PSP_DEVICE_INTERFACE_DETAIL_DATA) malloc (predictedLength);

	if( NULL == functionClassDeviceData) // added 2012.05.23
	{
		/// malloc error 		
		error_code = GetLastError();
        return INVALID_HANDLE_VALUE;
	}



    functionClassDeviceData->cbSize = sizeof (SP_INTERFACE_DEVICE_DETAIL_DATA);

    //
    // Retrieve the information from Plug and Play.
    //
    bStatus = SetupDiGetInterfaceDeviceDetail (
									       HardwareDeviceInfo,
									       DeviceInfoData,
									       functionClassDeviceData, // probing so no output buffer yet
									       predictedLength, // probing so output buffer length of zero
									       &requiredLength,
									       NULL);

    if ( !bStatus ) // not interested in the specific dev-node
    {
		error_code = GetLastError();

		if(functionClassDeviceData) { free( functionClassDeviceData ); functionClassDeviceData =NULL; }
		return INVALID_HANDLE_VALUE;
    }

	/// 2012.07.02, error_code = GetLastError(); /// added 2012.05.23

    strcpy( devName,functionClassDeviceData->DevicePath ) ;
    //EB_Printf( "[dnw] Attempting to open %s\n", devName );

    hOut = CreateFile (
						  functionClassDeviceData->DevicePath,
						  GENERIC_READ | GENERIC_WRITE,
						  FILE_SHARE_READ | FILE_SHARE_WRITE,
						  (LPSECURITY_ATTRIBUTES)NULL, // no SECURITY_ATTRIBUTES structure
						  OPEN_EXISTING, // No special create flags
						  0, /// |FILE_ATTRIBUTE_NORMAL, /// FILE_FLAG_OVERLAPPED // No special attributes
						  NULL); // No template file

    if (INVALID_HANDLE_VALUE==hOut) //  || NULL==hOut) 
	{
		if(functionClassDeviceData) { free( functionClassDeviceData ); functionClassDeviceData=NULL; }

		//EB_Printf( "[dnw] FAILED to open %s\n", devName );
		error_code = GetLastError();
        return INVALID_HANDLE_VALUE; /// 2013.04.20
    }

    free( functionClassDeviceData );

    return hOut;
}



/*++++++++++++++++++++++++++++++++++++++++++++++++++++
Routine Description:

   Do the required PnP things in order to find
   the next available proper device in the system at this time.

Arguments:

    pGuid:      ptr to GUID registered by the driver itself
    outNameBuf: the generated name for this device

Return Value:

    return HANDLE if the open and initialization was successful,
    else INVLAID_HANDLE_VALUE.
++++++++++++++++++++++++++++++++++++++++++++++++++--*/
HANDLE OpenUsbDevice( LPGUID  pGuid, char *outNameBuf)
{
    ULONG NumberDevices;  /// 2012.05.23 ULONG -> DWORD -> ULONG
	HANDLE hOut = INVALID_HANDLE_VALUE;
	HDEVINFO                 hardwareDeviceInfo;
	static SP_INTERFACE_DEVICE_DATA deviceInfoData; // added 2012.05.23, static
	ULONG                    i;
	BOOLEAN                  done;
	PUSB_DEVICE_DESCRIPTOR   usbDeviceInst;
	PUSB_DEVICE_DESCRIPTOR   *UsbDevices = &usbDeviceInst;
	static DWORD 			error_code;
	//	GUID HidGuid;
   

	*UsbDevices = NULL;
	NumberDevices = 0;

	ZeroMemory(&hardwareDeviceInfo,    sizeof(HDEVINFO)); /// added. 2012.05.23
	ZeroMemory(&deviceInfoData,        sizeof(SP_INTERFACE_DEVICE_DATA)); /// added. 2012.05.23

	//
	// Open a handle to the plug and play dev node.
	// SetupDiGetClassDevs() returns a device information set that contains info on all
	// installed devices of a specified class.
	// -------------------------------------------------------------------------------
	// 1. SetupDiGetClassDevs () --- 특정 Class의 모든 Device가 담고 있는 Device 정보 Set을 Return ---
	// -------------------------------------------------------------------------------
	//DIGCF_DEFAULT	   = 0x00000001,  // only valid with DIGCF_DEVICEINTERFACE
	//DIGCF_PRESENT	   = 0x00000002,
	//DIGCF_ALLCLASSES    = 0x00000004,
	//DIGCF_PROFILE	   = 0x00000008,
	//DIGCF_DEVICEINTERFACE   = 0x00000010,

	//HidD_GetHidGuid(&guidHID);
	hardwareDeviceInfo = SetupDiGetClassDevs (
										   pGuid,  ///&guidHID
										   NULL, // Define no enumerator (global)
										   NULL, // Define no
										   (DIGCF_PRESENT | // Only Devices present
										    DIGCF_INTERFACEDEVICE)
										    ); // Function class devices.


	if ( INVALID_HANDLE_VALUE==hardwareDeviceInfo || NULL==hardwareDeviceInfo ) 
	{
		error_code = GetLastError(); /// added 2012.05.23
		//EB_Printf( "[dnw] FAILED to open %s\n", devName );
		return INVALID_HANDLE_VALUE; // Insert error handling here.
	}

	//
	// Take a wild guess at the number of devices we have;
	// Be prepared to realloc and retry if there are more than we guessed
	//
	NumberDevices = 4;
	done = FALSE;
	deviceInfoData.cbSize = sizeof (SP_INTERFACE_DEVICE_DATA);

	i=0;
	while (!done) 
	{

		NumberDevices *= 2;

		if (*UsbDevices) 
		{
			*UsbDevices = (USB_DEVICE_DESCRIPTOR *)realloc(*UsbDevices, (NumberDevices * sizeof (USB_DEVICE_DESCRIPTOR)));
		} 
		else 
		{
			*UsbDevices = (USB_DEVICE_DESCRIPTOR *)calloc(NumberDevices, sizeof (USB_DEVICE_DESCRIPTOR));
		}


		if (NULL == *UsbDevices) 
		{
			// SetupDiDestroyDeviceInfoList destroys a device information set
			// and frees all associated memory.

			error_code = GetLastError(); /// added 2012.05.23

			/* Cleanup */
			SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);
			return INVALID_HANDLE_VALUE;
		}

		usbDeviceInst = *UsbDevices + i;

		for (; i < NumberDevices; i++) 
		{

			// SetupDiEnumDeviceInterfaces() returns information about device interfaces
			// exposed by one or more devices. Each call returns information about one interface;
			// the routine can be called repeatedly to get information about several interfaces
			// exposed by one or more devices.

			if (SetupDiEnumDeviceInterfaces (
										hardwareDeviceInfo,
										NULL, // We don't care about specific PDOs
										pGuid,
										i,
										&deviceInfoData)) 
			{
				hOut = OpenOneDevice (hardwareDeviceInfo, &deviceInfoData, outNameBuf);
				if ( INVALID_HANDLE_VALUE != hOut ) 
				{
					done = TRUE;
					break;
				}
			} 
			else 
			{
				if (ERROR_NO_MORE_ITEMS == GetLastError()) /// always fail here
				{
					/// EB_Printf("[dnw] ERROR_NO_MORE_ITEMS = i=%u,NumberDevices=%u. +++++++++ \n", i, NumberDevices);
					done = TRUE;
					break;
				}
			}

			ZeroMemory(&deviceInfoData,   sizeof(SP_INTERFACE_DEVICE_DATA)); /// added. 2012.05.23
			deviceInfoData.cbSize = sizeof (SP_INTERFACE_DEVICE_DATA); /// added. 2012.05.23
		}
	}

	NumberDevices = i;  /* Find Port number */

	// SetupDiDestroyDeviceInfoList() destroys a device information set
	// and frees all associated memory.

	/// 2012.07.02,  error_code = GetLastError(); /// added 2012.05.23

	/* Cleanup */
	SetupDiDestroyDeviceInfoList (hardwareDeviceInfo);

	free ( *UsbDevices );

	return hOut;
}



/*++++++++++++++++++++++++++++++++++++++++++++++++++++
Routine Description:

    Given a ptr to a driver-registered GUID, give us a string with the device name
    that can be used in a CreateFile() call.
    Actually briefly opens and closes the device and sets outBuf if successfull;
    returns FALSE if not

Arguments:

    pGuid:      ptr to GUID registered by the driver itself
    outNameBuf: the generated zero-terminated name for this device

Return Value:

    TRUE on success else FALSE

++++++++++++++++++++++++++++++++++++++++++++++++++--*/
BOOL GetUsbDeviceFileName( LPGUID  pGuid, char *outNameBuf)
{
	DWORD error_code = 0;
	//HidD_GetHidGuid(pGuid);
	HANDLE hDev = OpenUsbDevice( pGuid, outNameBuf );

	if ( INVALID_HANDLE_VALUE != hDev )
	{
		CloseHandle( hDev );
		return TRUE;
	}

	error_code = GetLastError(); /// added 2012.05.23
	return FALSE;
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++
Routine Description:

    Called by dumpUsbConfig() to open an instance of our device

Arguments:

    None

Return Value:

    Device handle on success else NULL

++++++++++++++++++++++++++++++++++++++++++++++++++--*/
HANDLE open_dev(VOID)
{
	DWORD error_code; 
	//GUID HidGuid;
	//HidD_GetHidGuid(&HidGuid);	
	HANDLE hDEV = OpenUsbDevice( (LPGUID)&GUID_CLASS_I82930_BULK, completeDeviceName);

	if ( INVALID_HANDLE_VALUE==hDEV ) // || NULL==hDEV ) 
	{
		error_code = GetLastError(); /// added 2012.05.23
		hDEV = OpenUsbDevice( (LPGUID)&GUID_CLASS_S3C2443X_BULK, completeDeviceName);
		if( INVALID_HANDLE_VALUE==hDEV ) 
		{
			error_code = GetLastError();

			hDEV = OpenUsbDevice( (LPGUID)&GUID_CLASS_I82930_MES_BULK, completeDeviceName); // 2017.08.16
			if( INVALID_HANDLE_VALUE==hDEV ) 
			{
				USB_TX_SIZE = 0; /// 2013.04.20
				//EB_Printf("[dnw] INVALID HANDLE = DeviceName = (%s)\n", completeDeviceName); // 2017.08.17

				return INVALID_HANDLE_VALUE; /* --- critical error fixed!!!!!  happen blue screen!!!!! ----- */
			}
			else
			{
				USB_TX_SIZE = TX_SIZE_1; // 2017.08.16
			}
			
		}
		else
		{
			USB_TX_SIZE = TX_SIZE_2; /* -- OK!!! -- */
		}
    } 
	else 
	{
		// EB_Printf("[dnw] DeviceName = (%s)\n", completeDeviceName);
		// EB_Printf("[dnw] completeDeviceName is NULL ++222+++++++++  \n" );

		USB_TX_SIZE = TX_SIZE_1; // OK~~~~
    }       

    return hDEV;
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++
Routine Description:

    Called by main() to open an instance of our device after obtaining its name

Arguments:

    None

Return Value:

    Device handle on success else NULL

++++++++++++++++++++++++++++++++++++++++++++++++++--*/
HANDLE open_file( char *filename, int err_count, BYTE funcKey )
{
	//GUID HidGuid;
	DWORD error_code;
	int success = 1;
	HANDLE h = NULL;

	// 2016.04.25
	TCHAR szKeyName[100] = {0,};
	
         if( UBOOT_SHIFT_F7 == funcKey ) lstrcpy( szKeyName, TEXT("Shift+[F7]") );
	else if( UBOOT_SHIFT_F8 == funcKey ) lstrcpy( szKeyName, TEXT("Shift+[F8]") );
	else if( UBOOT_SHIFT_F9 == funcKey ) lstrcpy( szKeyName, TEXT("Shift+[F9]") );
	else if( UBOOT_SHIFT_F10 == funcKey ) lstrcpy( szKeyName, TEXT("Shift+[F10]") );
	else if( UBOOT_SHIFT_F11 == funcKey ) lstrcpy( szKeyName, TEXT("Shift+[F11]") );
	else if( UBOOT_SHIFT_ALL == funcKey ) lstrcpy( szKeyName, TEXT("[F4]-key") );
	else if( UBOOT_TIMER == funcKey ) lstrcpy( szKeyName, TEXT("[Timer]") );
	else lstrcpy( szKeyName, TEXT("[Menu]") );




	//HidD_GetHidGuid(&HidGuid);
	if ( !GetUsbDeviceFileName( (LPGUID) &GUID_CLASS_I82930_BULK, completeDeviceName ) )
	{
		error_code = GetLastError(); /// added 2012.05.23
		//NOISY(("Failed to GetUsbDeviceFileName:%d\n", GetLastError()));
		if ( !GetUsbDeviceFileName(	(LPGUID) &GUID_CLASS_S3C2443X_BULK, completeDeviceName ) )
		{
			error_code = GetLastError(); /// added 2012.05.23

			if ( !GetUsbDeviceFileName( (LPGUID) &GUID_CLASS_I82930_MES_BULK, completeDeviceName ) ) // 2017.08.16
			{
				error_code = GetLastError(); /// added 2017.06.29

				if( USB_OPEN_RETRY==err_count )
				{
					EB_Printf("[dnw] can not get usb device!! Error(%u), siz(0x%x) by %s \r\n", error_code, USB_TX_SIZE, szKeyName );
				}

				USB_TX_SIZE = 0; /// 2013.04.21
				return  INVALID_HANDLE_VALUE; // invaild--------

			}
			else
			{
				USB_TX_SIZE = TX_SIZE_1; // 2017.08.16, Added
			}

		}
		else
		{
			USB_TX_SIZE = TX_SIZE_2; // OK-- SMDKC100, SMDK6410
		}
	}
	else
	{
		USB_TX_SIZE = TX_SIZE_1; // OK~~~::  S3C2440A 2017.07.19
	}



	if((strlen(completeDeviceName) + strlen(filename)) > 255) 
	{
		/// NOISY(("Failed to open handle - possibly long filenamen"));
		if( USB_OPEN_RETRY==err_count )
		{
			EB_Printf("[dnw] usb open_file... filename[%s], USB_TX_SIZE(0x%x), count(%d) by %s \r\n", filename, USB_TX_SIZE, err_count, szKeyName );
		}
		else {
			//EB_Printf("[dnw] usb open_file... filename[%s], USB_TX_SIZE(0x%x), too long filename(>255)!! \r\n", filename, USB_TX_SIZE, err_count  );
		}

		return INVALID_HANDLE_VALUE;
    }


	strcat (completeDeviceName, "\\"  );

	strcat (completeDeviceName, filename );                  


	//EB_Printf("[dnw] completeDeviceName = (%s)\n", completeDeviceName);

	h = CreateFile(completeDeviceName,
					GENERIC_WRITE | GENERIC_READ,
					FILE_SHARE_WRITE | FILE_SHARE_READ,
					NULL,
					OPEN_EXISTING,
					0,
					NULL);

	if( INVALID_HANDLE_VALUE==h ) // || NULL==h ) 
	{
		//NOISY(("Failed to open (%s) = %d", completeDeviceName, GetLastError()));
		error_code = GetLastError(); /// added 2012.05.23
		success = 0;
		///EB_Printf("[dnw] open_file ++++ Failed to open (%s) = %d", completeDeviceName, GetLastError() ); /// Err code 1450
		return  INVALID_HANDLE_VALUE; /// added.
	} 
	//else 
	//{
	//	NOISY(("Opened successfully.\n"));
	//}       

	return h;
}



// Begin, routines for USB configuration dump (Cmdline "rwbulk -u" )


/*++++++++++++++++++++++++++++++++++++++++++++++++++++
Routine Description:

    Called to get ascii string of USB descriptor

Arguments:

    PUSB_ENDPOINT_DESCRIPTOR->bDescriptorType or
    PUSB_DEVICE_DESCRIPTOR->bDescriptorType or
    PUSB_INTERFACE_DESCRIPTOR->bDescriptorType or
    PUSB_STRING_DESCRIPTOR->bDescriptorType or
    PUSB_POWER_DESCRIPTOR->bDescriptorType or
    PUSB_CONFIGURATION_DESCRIPTOR->bDescriptorType

Return Value:

    ptr to string

++++++++++++++++++++++++++++++++++++++++++++++++++--*/
char *usbDescriptorTypeString(UCHAR bDescriptorType )
{

    switch(bDescriptorType) {

    case USB_DEVICE_DESCRIPTOR_TYPE:
		return "USB_DEVICE_DESCRIPTOR_TYPE";

    case USB_CONFIGURATION_DESCRIPTOR_TYPE:
		return "USB_CONFIGURATION_DESCRIPTOR_TYPE";
	

    case USB_STRING_DESCRIPTOR_TYPE:
		return "USB_STRING_DESCRIPTOR_TYPE";
	

    case USB_INTERFACE_DESCRIPTOR_TYPE:
		return "USB_INTERFACE_DESCRIPTOR_TYPE";
	

    case USB_ENDPOINT_DESCRIPTOR_TYPE:
		return "USB_ENDPOINT_DESCRIPTOR_TYPE";
	

#ifdef USB_POWER_DESCRIPTOR_TYPE // this is the older definintion which is actually obsolete
    // workaround for temporary bug in 98ddk, older USB100.h file
    case USB_POWER_DESCRIPTOR_TYPE:
		return "USB_POWER_DESCRIPTOR_TYPE";
#endif
	
#ifdef USB_RESERVED_DESCRIPTOR_TYPE  // this is the current version of USB100.h as in NT5DDK

    case USB_RESERVED_DESCRIPTOR_TYPE:
		return "USB_RESERVED_DESCRIPTOR_TYPE";

    case USB_CONFIG_POWER_DESCRIPTOR_TYPE:
		return "USB_CONFIG_POWER_DESCRIPTOR_TYPE";

    case USB_INTERFACE_POWER_DESCRIPTOR_TYPE:
		return "USB_INTERFACE_POWER_DESCRIPTOR_TYPE";
#endif // for current nt5ddk version of USB100.h

    default:
		return "??? UNKNOWN!!"; 
    }
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++
Routine Description:

    Called to get ascii string of endpt descriptor type

Arguments:

    PUSB_ENDPOINT_DESCRIPTOR->bmAttributes

Return Value:

    ptr to string

++++++++++++++++++++++++++++++++++++++++++++++++++--*/
char *usbEndPointTypeString(UCHAR bmAttributes)
{
    UINT typ = bmAttributes & USB_ENDPOINT_TYPE_MASK;

    switch( typ) 
	{
	    case USB_ENDPOINT_TYPE_INTERRUPT:
			return "USB_ENDPOINT_TYPE_INTERRUPT";

	    case USB_ENDPOINT_TYPE_BULK:
			return "USB_ENDPOINT_TYPE_BULK";    

	    case USB_ENDPOINT_TYPE_ISOCHRONOUS:
			return "USB_ENDPOINT_TYPE_ISOCHRONOUS"; 
		
	    case USB_ENDPOINT_TYPE_CONTROL:
			return "USB_ENDPOINT_TYPE_CONTROL"; 
		
	    default:
			return "??? UNKNOWN!!"; 
    }
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++
Routine Description:

    Called to get ascii string of USB_CONFIGURATION_DESCRIPTOR attributes

Arguments:

    PUSB_CONFIGURATION_DESCRIPTOR->bmAttributes

Return Value:

    ptr to string

++++++++++++++++++++++++++++++++++++++++++++++++++--*/
char *usbConfigAttributesString(UCHAR bmAttributes)
{
    UINT typ = bmAttributes & USB_CONFIG_POWERED_MASK;
    static CHAR usbAttributesBuffer[256] = {0,};

    memset(usbAttributesBuffer,0x00, sizeof(usbAttributesBuffer));

    switch( typ )
	{
	    case USB_CONFIG_BUS_POWERED:
            strcpy(usbAttributesBuffer,"USB_CONFIG_BUS_POWERED");
            break;

	    case USB_CONFIG_SELF_POWERED:
            strcpy(usbAttributesBuffer,"USB_CONFIG_SELF_POWERED");
            break;

        default:
			sprintf(usbAttributesBuffer,"Default Attr(0x%x)", bmAttributes ); // 2017.08.17
            break;
    }

    if(bmAttributes & USB_CONFIG_REMOTE_WAKEUP) 
	{
        strcat(usbAttributesBuffer," -> USB_CONFIG_REMOTE_WAKEUP");
    }

    return usbAttributesBuffer;
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++
Routine Description:

    Called to do formatted ascii dump to console of a USB config descriptor

Arguments:

    ptr to USB configuration descriptor

Return Value:

    none

++++++++++++++++++++++++++++++++++++++++++++++++++--*/
void print_USB_CONFIGURATION_DESCRIPTOR(PUSB_CONFIGURATION_DESCRIPTOR cd)
{              

    EB_Printf("\n[dnw] ==================== USB DEVICE STATUS ===================\n[dnw] USB_CONFIGURATION_DESCRIPTOR\n");

    EB_Printf(
    "[dnw] bLength = 0x%X (%d)\n", cd->bLength, cd->bLength  );

    EB_Printf(
    "[dnw] bDescriptorType = 0x%X ( %s )\n", cd->bDescriptorType, usbDescriptorTypeString( cd->bDescriptorType ) );

    EB_Printf(
    "[dnw] wTotalLength = 0x%X (%d)\n", cd->wTotalLength, cd->wTotalLength );

    EB_Printf(
    "[dnw] bNumInterfaces = 0x%X (%d)\n", cd->bNumInterfaces, cd->bNumInterfaces );

    EB_Printf(
    "[dnw] bConfigurationValue = 0x%X (%d)\n", cd->bConfigurationValue, cd->bConfigurationValue );

    EB_Printf(
    "[dnw] iConfiguration = 0x%X, (%d) \n", cd->iConfiguration, cd->iConfiguration );

    EB_Printf(
    "[dnw] bmAttributes = 0x%X ( %s )\n", cd->bmAttributes, usbConfigAttributesString( cd->bmAttributes )  );

    EB_Printf(
    "[dnw] MaxPower = 0x%X (%d)\n", cd->MaxPower, cd->MaxPower );
    EB_Printf("[dnw] ===========================================================\n\n");
}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++
Routine Description:

    Called to do formatted ascii dump to console of a USB interface descriptor

Arguments:

    ptr to USB interface descriptor

Return Value:

    none

++++++++++++++++++++++++++++++++++++++++++++++++++--*/
void print_USB_INTERFACE_DESCRIPTOR(PUSB_INTERFACE_DESCRIPTOR id, UINT ix)
{

    EB_Printf("[dnw] USB_INTERFACE_DESCRIPTOR #%d\n", ix);


    EB_Printf(
    "[dnw] bLength = 0x%X\n", id->bLength );


    EB_Printf(
    "[dnw] bDescriptorType = 0x%X ( %s )\n", id->bDescriptorType, usbDescriptorTypeString( id->bDescriptorType ) );

    EB_Printf(
    "[dnw] bInterfaceNumber = 0x%X\n", id->bInterfaceNumber   );

    EB_Printf(
    "[dnw] bAlternateSetting = 0x%X\n", id->bAlternateSetting  );

    EB_Printf(
    "[dnw] bNumEndpoints = 0x%X\n", id->bNumEndpoints  );

    EB_Printf(
    "[dnw] bInterfaceClass = 0x%X\n", id->bInterfaceClass   );
	
    EB_Printf(
    "[dnw] bInterfaceSubClass = 0x%X\n", id->bInterfaceSubClass  );

    EB_Printf(
    "[dnw] bInterfaceProtocol = 0x%X\n", id->bInterfaceProtocol  );

    EB_Printf(
    "[dnw] bInterface = 0x%X\n", id->iInterface  );
    EB_Printf("[dnw] ===========================================================\n\n");

}



#ifdef __ADDED__
/*++++++++++++++++++++++++++++++++++++++++++++++++++++
Routine Description:

Called to do formatted ascii dump to console of a USB pipe information

Arguments:

ptr to USB pipe information

Return Value:

none

++++++++++++++++++++++++++++++++++++++++++++++++++--*/
void print_USBD_PIPE_INFORMATION(PUSBD_PIPE_INFORMATION PipeInfo,ULONG PipeNumber)
{
    PUCHAR text;
    EB_Printf("\n[dnw] -----------------------------\n[dnw] USBD_PIPE_INFORMATION #%d\n", PipeNumber);

    EB_Printf(
        "[dnw] MaximumPacketSize = 0x%x\n", PipeInfo->MaximumPacketSize
    );
    EB_Printf(
        "[dnw] EndpointAddress = 0x%x\n", PipeInfo->EndpointAddress
    );
    EB_Printf(
        "[dnw] Interval = 0x%x\n", PipeInfo->Interval
    );
    switch(PipeInfo->PipeType) {
        case UsbdPipeTypeControl:
            text = (PUCHAR) "UsbdPipeTypeControl";
            break;
        case UsbdPipeTypeIsochronous:
            text = (PUCHAR) "UsbdPipeTypeIsochronous";
            break;
        case UsbdPipeTypeBulk:
            text = (PUCHAR) "UsbdPipeTypeBulk";
            break;
        case UsbdPipeTypeInterrupt:
            text = (PUCHAR) "UsbdPipeTypeInterrupt";
            break;
    }
    EB_Printf(
        "[dnw] PipeType = 0x%x %s \n", PipeInfo->PipeType,text
    );
    EB_Printf(
        "[dnw] PipeHandle = 0x%p \n", PipeInfo->PipeHandle
    );
    EB_Printf(
        "[dnw] MaximumTransferSize = 0x%x \n", PipeInfo->MaximumPacketSize
    );

}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++
Routine Description:

Called to do formatted ascii dump to console of a USB interface information

Arguments:

ptr to USB interface information

Return Value:

none

++++++++++++++++++++++++++++++++++++++++++++++++++--*/
void print_USBD_INTERFACE_INFORMATION(PUSBD_INTERFACE_INFORMATION InterfaceInformation)
{
	ULONG index;

    EB_Printf("\n[dnw] -----------------------------\n[dnw] USBD_INTERFACE_INFORMATION\n");


    EB_Printf(
        "[dnw] Length = 0x%x \n", InterfaceInformation->Length
        );


    EB_Printf(
        "[dnw] InterfaceNumber = 0x%x \n", InterfaceInformation->InterfaceNumber
        );


    EB_Printf(
        "[dnw] AlternateSetting = 0x%x \n", InterfaceInformation->AlternateSetting
        );
    EB_Printf(
        "[dnw] Class = 0x%x \n", InterfaceInformation->Class
        );
    EB_Printf(
        "[dnw] SubClass = 0x%x \n", InterfaceInformation->SubClass
        );
    EB_Printf(
        "[dnw] Protocol = 0x%x \n", InterfaceInformation->Protocol
        );
    EB_Printf(
        "[dnw] InterfaceHandle = 0x%p \n", InterfaceInformation->InterfaceHandle
        );
    EB_Printf(
        "[dnw] NumberofPipes = 0x%x \n", InterfaceInformation->NumberOfPipes
        );

	for(index = 0; index < InterfaceInformation->NumberOfPipes; index++) {
        print_USBD_PIPE_INFORMATION(&InterfaceInformation->Pipes[index],index);
    }
}
#endif /// __ADDED__



/*++++++++++++++++++++++++++++++++++++++++++++++++++++
Routine Description:

    Called to do formatted ascii dump to console of a USB endpoint descriptor

Arguments:

    ptr to USB endpoint descriptor,
    index of this endpt in interface desc

Return Value:

    none

++++++++++++++++++++++++++++++++++++++++++++++++++--*/
void print_USB_ENDPOINT_DESCRIPTOR(PUSB_ENDPOINT_DESCRIPTOR ed, int i)
{

    EB_Printf(
    "[dnw] USB_ENDPOINT_DESCRIPTOR for Pipe%02d\n", i  );

    EB_Printf(
    "[dnw] bLength = 0x%X (%d) \n", ed->bLength, ed->bLength   );

    EB_Printf(
    "[dnw] bDescriptorType = 0x%X ( %s )\n", ed->bDescriptorType, usbDescriptorTypeString( ed->bDescriptorType )  );


    if ( USB_ENDPOINT_DIRECTION_IN( ed->bEndpointAddress ) ) {
		EB_Printf(
		"[dnw] bEndpointAddress= 0x%X ( INPUT )\n", ed->bEndpointAddress );
    } 
	else {
		EB_Printf(
		"[dnw] bEndpointAddress= 0x%X ( OUTPUT )\n", ed->bEndpointAddress );
    }

    EB_Printf(
    "[dnw] bmAttributes= 0x%X ( %s )\n", ed->bmAttributes, usbEndPointTypeString ( ed->bmAttributes )  );

    EB_Printf(
    "[dnw] wMaxPacketSize= 0x%X (%d) \n", ed->wMaxPacketSize, ed->wMaxPacketSize  );
    EB_Printf(
    "[dnw] bInterval = 0x%X (%d) \n", ed->bInterval, ed->bInterval  );
    EB_Printf("[dnw] ===========================================================\n\n");


}


/*++++++++++++++++++++++++++++++++++++++++++++++++++++
Routine Description:

    Called to do formatted ascii dump to console of  USB
    configuration, interface, and endpoint descriptors
    (Cmdline "rwbulk -u" )

Arguments:

    handle to device

Return Value:

    none

++++++++++++++++++++++++++++++++++++++++++++++++++--*/
/*void */
BOOL rw_dev( HANDLE hDEV )
{
    UINT success;
    DWORD siz, nBytes; /// int -> DWORD
    char buf[2048] = {0,}; /// 256 -> 2048
    PUSB_CONFIGURATION_DESCRIPTOR cd;
    PUSB_INTERFACE_DESCRIPTOR id;
    PUSB_ENDPOINT_DESCRIPTOR ed;
	DWORD error_code = 0;
	
    siz = sizeof(buf);

    if ( INVALID_HANDLE_VALUE==hDEV || NULL==hDEV) 
	{
		error_code = GetLastError(); /// added 2012.05.23
		//NOISY(("DEV not open"));
		return FALSE;
    }

	memset( buf, 0x00, sizeof(buf) ); // -- 2015.11.27
	
    success = DeviceIoControl(hDEV,  /// device handle
				#if 0 /// 2012.05.24
							IOCTL_USB_GET_NODE_INFORMATION,
				#else
						    IOCTL_BULKUSB_GET_CONFIG_DESCRIPTOR,
				#endif
						    buf,
						    siz,
						    buf,         /// output data buffer
						    siz,
						    (unsigned long *)&nBytes,
						    (LPOVERLAPPED)NULL);

    //NOISY(("request complete, success = %d nBytes = %d\n", success, nBytes));
    
    if (success) 
	{
		ULONG i;
		UINT  j, n;
		char *pch = NULL;

		pch = buf;
		n = 0;

		cd = (PUSB_CONFIGURATION_DESCRIPTOR) pch;

		print_USB_CONFIGURATION_DESCRIPTOR( cd );

#ifdef __ADDED__
        success = DeviceIoControl(hDEV,
					            IOCTL_OSRUSBFX2_GET_INTERFACE_INFORMATION,
					            0,
					            0,
					            buf,
					            siz,
					            &nBytes,
					            NULL);

        if (success) 
		{
            interfaceInformation = (PUSBD_INTERFACE_INFORMATION) buf;
            print_USBD_INTERFACE_INFORMATION(interfaceInformation);
        } 
		else {
            ///NOISY(("IOCTL_OSRUSBFX2_GET_INTERFACE_INFORMATION complete, error = %d nBytes = %dn", GetLastError(), nBytes));
        }
#endif /// __ADDED__

		pch += cd->bLength;

		do {

		    id = (PUSB_INTERFACE_DESCRIPTOR) pch;

		    print_USB_INTERFACE_DESCRIPTOR(id, n++);

		    pch += id->bLength;

		    for (j=0; j<id->bNumEndpoints; j++) 
			{
				ed = (PUSB_ENDPOINT_DESCRIPTOR) pch;

				print_USB_ENDPOINT_DESCRIPTOR(ed,j);

				pch += ed->bLength;
		    }
		    i = (ULONG)(pch - buf);
		} while (i<cd->wTotalLength);

    }
	else
	{
		error_code = GetLastError(); /// added 2012.05.23
		return FALSE;
	}
    
    return TRUE;

}



/*++++++++++++++++++++++++++++++++++++++++++++++++++++
Routine Description:

    Called to do formatted ascii dump to console of  USB
    configuration, interface, and endpoint descriptors
    (Cmdline "rwbulk -u" )

Arguments:

    none

Return Value:

    none

++++++++++++++++++++++++++++++++++++++++++++++++++--*/
int  dumpUsbConfig(void)
{
	DWORD error_code;

	HANDLE hDEV = open_dev();


	if ( hDEV )
	{
		if( FALSE == rw_dev( hDEV ) )
		{
			error_code = GetLastError();
			if(hDEV) CloseHandle(hDEV);
			EB_Printf("[dnw] can not read BULKUSB I/O control!! Error(%u) \r\n", error_code );
			return 0; /// added. 2012.02.21
		}
		if(hDEV) CloseHandle(hDEV);
		return 1; // OK!!
    }

	error_code = GetLastError();
	EB_Printf("[dnw] can not open BULKUSB I/O device!!! Error(%u) \r\n", error_code );
    return 0;
	
}
//  End, routines for USB configuration and pipe info dump  (Cmdline "rwbulk -u" )

/// #pragma managed /// 2012.05.23









/////

#ifdef __BULK___C__

// Enables this example to work with a device running the
// libusb-win32 PIC Benchmark Firmware.
#define BENCHMARK_DEVICE

//////////////////////////////////////////////////////////////////////////////
// TEST SETUP (User configurable)

// Issues a Set configuration request
#define TEST_SET_CONFIGURATION

// Issues a claim interface request
#define TEST_CLAIM_INTERFACE

// Use the libusb-win32 async transfer functions. see
// transfer_bulk_async() below.
#define TEST_ASYNC

// Attempts one bulk read.
#define TEST_BULK_READ

// Attempts one bulk write.
// #define TEST_BULK_WRITE

//////////////////////////////////////////////////////////////////////////////
// DEVICE SETUP (User configurable)

// Device vendor and product id.
#define MY_VID 0x0666
#define MY_PID 0x0001

// Device configuration and interface id.
#define MY_CONFIG 1
#define MY_INTF 0

// Device endpoint(s)
#define EP_IN 0x81
#define EP_OUT 0x01

// Device of bytes to transfer.
#define BUF_SIZE 64

//////////////////////////////////////////////////////////////////////////////
usb_dev_handle *open_devID(void);

static int transfer_bulk_async(usb_dev_handle *dev,
                               int ep,
                               char *bytes,
                               int size,
                               int timeout);

usb_dev_handle *open_devID(void)
{
    struct usb_bus *bus;
    struct usb_device *dev;

    for (bus = usb_get_busses(); bus; bus = bus->next)
    {
        for (dev = bus->devices; dev; dev = dev->next)
        {
            if (dev->descriptor.idVendor == MY_VID
                    && dev->descriptor.idProduct == MY_PID)
            {
                return usb_open(dev);
            }
        }
    }
    return NULL;
}

int CheckUSBDeviceID(void)
{
    usb_dev_handle *dev = NULL; /* the device handle */
    char tmp[BUF_SIZE];
    int ret;
    void* async_read_context = NULL;
    void* async_write_context = NULL;

    usb_init(); /* initialize the library */
    usb_find_busses(); /* find all busses */
    usb_find_devices(); /* find all connected devices */


    if (!(dev = open_devID()))
    {
        printf("error opening device: n%sn", usb_strerror());
        return 0;
    }
    else
    {
        printf("success: device %04X:%04X openedn", MY_VID, MY_PID);
    }

#ifdef TEST_SET_CONFIGURATION
    if (usb_set_configuration(dev, MY_CONFIG) < 0)
    {
        printf("error setting config #%d: %sn", MY_CONFIG, usb_strerror());
        usb_close(dev);
        return 0;
    }
    else
    {
        printf("success: set configuration #%dn", MY_CONFIG);
    }
#endif

#ifdef TEST_CLAIM_INTERFACE
    if (usb_claim_interface(dev, 0) < 0)
    {
        printf("error claiming interface #%d:n%sn", MY_INTF, usb_strerror());
        usb_close(dev);
        return 0;
    }
    else
    {
        printf("success: claim_interface #%dn", MY_INTF);
    }
#endif

#ifdef TEST_BULK_WRITE

#ifdef BENCHMARK_DEVICE
    ret = usb_control_msg(dev, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
                          14, /* set/get test */
                          2,  /* test type    */
                          MY_INTF,  /* interface id */
                          tmp, 1, 1000);
#endif

#ifdef TEST_ASYNC
    // Running an async write test
    ret = transfer_bulk_async(dev, EP_OUT, tmp, sizeof(tmp), 5000);
#else
    // Running a sync write test
    ret = usb_bulk_write(dev, EP_OUT, tmp, sizeof(tmp), 5000);
#endif
    if (ret < 0)
    {
        printf("error writing:n%sn", usb_strerror());
    }
    else
    {
        printf("success: bulk write %d bytesn", ret);
    }
#endif

#ifdef TEST_BULK_READ

#ifdef BENCHMARK_DEVICE
    ret = usb_control_msg(dev, USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
                          14, /* set/get test */
                          1,  /* test type    */
                          MY_INTF,  /* interface id */
                          tmp, 1, 1000);
#endif

#ifdef TEST_ASYNC
    // Running an async read test
    ret = transfer_bulk_async(dev, EP_IN, tmp, sizeof(tmp), 5000);
#else
    // Running a sync read test
    ret = usb_bulk_read(dev, EP_IN, tmp, sizeof(tmp), 5000);
#endif
    if (ret < 0)
    {
        printf("error reading:n%sn", usb_strerror());
    }
    else
    {
        printf("success: bulk read %d bytesn", ret);
    }
#endif

#ifdef TEST_CLAIM_INTERFACE
    usb_release_interface(dev, 0);
#endif

    if (dev)
    {
        usb_close(dev);
    }
    printf("Done.\n");

    return 0;
}

/*
* Read/Write using async transfer functions.
*
* NOTE: This function waits for the transfer to complete essentially making
* it a sync transfer function so it only serves as an example of how one might
* implement async transfers into thier own code.
*/
static int transfer_bulk_async(usb_dev_handle *dev,
                               int ep,
                               char *bytes,
                               int size,
                               int timeout)
{
    // Each async transfer requires it's own context. A transfer
    // context can be re-used.  When no longer needed they must be
    // freed with usb_free_async().
    //
    void* async_context = NULL;
    int ret;

    // Setup the async transfer.  This only needs to be done once
    // for multiple submit/reaps. (more below)
    //
    ret = usb_bulk_setup_async(dev, &async_context, ep);
    if (ret < 0)
    {
        printf("error usb_bulk_setup_async:n%sn", usb_strerror());
        goto Done;
    }

    // Submit this transfer.  This function returns immediately and the
    // transfer is on it's way to the device.
    //
    ret = usb_submit_async(async_context, bytes, size);
    if (ret < 0)
    {
        printf("error usb_submit_async:n%sn", usb_strerror());
        usb_free_async(&async_context);
        goto Done;
    }

    // Wait for the transfer to complete.  If it doesn't complete in the
    // specified time it is cancelled.  see also usb_reap_async_nocancel().
    //
    ret = usb_reap_async(async_context, timeout);

    // Free the context.
    usb_free_async(&async_context);

Done:
    return ret;
}
#endif



