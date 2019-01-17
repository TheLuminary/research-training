// DeviceIO.C
// Routines for interacting with the Excalibur kernel driver
// For all Windows Operating Systems

#include <stdio.h>
#include <windows.h>
#include <winioctl.h>
#include "excsysio.h"
#include "error_devio.h"
#include "deviceio.h"

// File variables (static data)

// .. Flag regarding intialization of static datas
static BOOL		mfInitialized = FALSE;

// .. Operating system type
// .. (pre-init necessary for proper error handling in IsWin2000() function)
static  DWORD	mdwOS = EXC_OS_UNDEFINED;

// .. Handles to kernel driver 
// .. Under Win9x, single handle
// .. Under WinNT, per device
static  HANDLE  mhDevice9X;
static  HANDLE	mhDeviceNT[MAX_DEVICES];

// .. Keep track of open devices
static  BOOL    mfDeviceOpen[MAX_DEVICES];
static  DWORD   mdwDevRefCount[MAX_DEVICES];

// .. Interrupt Semaphores
static  HANDLE  mhEvent[MAX_DEVICES];
static	HANDLE	mhEventForModule[MAX_DEVICES][MAX_MODULES];

// .. Specific error messages
static  DWORD   mKrnlError[MAX_DEVICES];

// .. Pointer to function for Win9x kernel event conversion
static HANDLE (WINAPI *pOpenVxDHandle)(HANDLE);

// Module functions
static	BOOL	mInitialize(void);
static	BOOL	mSetOS(void);
static	BOOL	mAllDevicesClosed(void);
static	int		mGetEventHandle(int nDevice, HANDLE *phEvent);
static	int		mGetEventHandleForModule(int nDevice, int nModule, HANDLE *phEvent);

static	HANDLE	(WINAPI *mGetAddressOfOpenVxDHandle(void))(HANDLE);

/*
	OpenKernelDevice()

	Description:
		Retrieves a handle to our device. This is the first step
		before we can access the device.
		OpenKernelDevice() is called by init_card.

	Input:
		Device number

	Output:
		Returns error message or 0 for success.
*/

// CR1
int OpenKernelDevice(int nDevice) {
	
	BOOL			bResult;
	unsigned long	nBytes;
	char			szFullDeviceName[EXC_BUFFER_SIZE_LARGE];

	if (!mfInitialized) {
		// First call to file. Initialize static data
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameter
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;

	// If device is already open, just increment reference count
	if (mfDeviceOpen[nDevice]) {
		mdwDevRefCount[nDevice]++;
		return 0;
	}

	// Win9x: Need to first open the vxd if not yet opened
	if (mdwOS == EXC_OS_WIN9X) {
		
		if (mhDevice9X == INVALID_HANDLE_VALUE) {
			
			// Open the vxd
			mhDevice9X = CreateFile(EXC_VXD_DEVICE_NAME, 0,0,0,
				CREATE_NEW, FILE_FLAG_DELETE_ON_CLOSE, 0);
			
			if (mhDevice9X == INVALID_HANDLE_VALUE) {
				DWORD dwError = GetLastError();
				mKrnlError[nDevice] = dwError;
				
				// WinAPI documentation recommends this catch
				if (dwError == ERROR_NOT_SUPPORTED)
					DeleteFile(EXC_VXD_DEVICE_NAME);

				return (eopenkernel);
			}
		}
	}
	
	// WINNT4 and WIN2000: Get the name of the device
	if (mdwOS == EXC_OS_WINNT4) {
		sprintf(szFullDeviceName, "%s%d", EXC_KERNEL_DRIVER_DEVICE_NAME, nDevice);
	}
	else if (mdwOS == EXC_OS_WIN2000) {
		
		HINSTANCE hFrontDesk;
		BOOL (__stdcall *lpExcFindDevice)(DWORD dwRegistry, char *szDeviceName, DWORD dwBufferSize) ;
		
		// Open handle to our Win2000 DLL which enumerates available Excalibur devices
		hFrontDesk = LoadLibrary("frontdesk.dll");
		if (!hFrontDesk) {
			return ekernelfrontdeskload;
		}
		
		// get a pointer to the one (and only) function it exports
		lpExcFindDevice = (BOOL (__stdcall *)(DWORD dwRegistry, char *szDeviceName, DWORD dwBufferSize)) GetProcAddress(hFrontDesk, (LPCSTR) 1);
		if (!lpExcFindDevice) {
			FreeLibrary(hFrontDesk);
			return ekernelfrontdeskload;
		}
		
		// call the function to get our device name
		bResult = lpExcFindDevice(nDevice, szFullDeviceName, EXC_BUFFER_SIZE_LARGE);
		if (!bResult) {
			FreeLibrary(hFrontDesk);
			return ekernelfrontdesk;
		}
		FreeLibrary(hFrontDesk);
	}
	
	// Open the device
	if (mdwOS == EXC_OS_WIN9X) {
		
		t_ExcDeviceIOPacket pktDeviceIO;
		
		pktDeviceIO.nDevice = nDevice;
		
		bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_OPEN_DEVICE,
			&pktDeviceIO, sizeof(pktDeviceIO),NULL,0,&nBytes,NULL);
	}
	else {
		mhDeviceNT[nDevice] = CreateFile(szFullDeviceName, GENERIC_READ | GENERIC_WRITE,
			0, NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL);
		
		if (mhDeviceNT[nDevice] == INVALID_HANDLE_VALUE)
			bResult = FALSE;
		else
			bResult = TRUE;
	}
	
	// If failed to open device, return error
	if (!bResult) {
		
		// get specific error
		DWORD dwError = GetLastError();
		mKrnlError[nDevice] = dwError;
		
		// win9x: close the vxd if relevant
		if (mdwOS == EXC_OS_WIN9X) {
			if (mAllDevicesClosed()) {
				CloseHandle(mhDevice9X);
				mhDevice9X = INVALID_HANDLE_VALUE;
			}
		}

		// attempt to return specific error
		switch (dwError) {
			
		case ERROR_NOT_ENOUGH_MEMORY:
		case ERROR_INVALID_ADDRESS:
		case ERROR_NO_SYSTEM_RESOURCES:
			return eallocresources;
			
		case ERROR_RESOURCE_TYPE_NOT_FOUND:
		case ERROR_RESOURCE_DATA_NOT_FOUND:
			return regnotset;
			
		default:
			return eopenkernel;
		}
	}
	
	mfDeviceOpen[nDevice] = TRUE;
	mdwDevRefCount[nDevice] = 1;
	
	return 0;
}

/*
	CloseKernelDevice()

	Description:
		Closes the handle to our device.
		CloseKernelDevice() is called by release_card or release_module

	Input:
		None

	Output:
		Returns error message or 0 for success.
*/

// CR1
int CloseKernelDevice(int nDevice) {

	BOOL			bResult;
	unsigned long   nBytes;

	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameter
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;

	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;

	// decrement reference count
	mdwDevRefCount[nDevice]--;

	// if this is the last instance, clean up and close device
	if (mdwDevRefCount[nDevice] == 0) {

		int mod;

		// Release Events
		// .. global event for the device
		Release_Event_Handle(nDevice);

		// .. specific events for each module
		for (mod = 0; mod < MAX_MODULES; mod++) {
			Release_Event_Handle_For_Module(nDevice, mod);
		}

		// Unmap Memory
		UnMapMemory(nDevice);

		// Do the Close
		if (mdwOS == EXC_OS_WIN9X) {

			t_ExcDeviceIOPacket pktDeviceIO;

			pktDeviceIO.nDevice = nDevice;

			bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_CLOSE_DEVICE,
				&pktDeviceIO, sizeof(pktDeviceIO),NULL,0,&nBytes,NULL);
		}
		else {
			bResult = CloseHandle(mhDeviceNT[nDevice]);
			mhDeviceNT[nDevice] = INVALID_HANDLE_VALUE;
		}

		// Mark device as closed
		mfDeviceOpen[nDevice] = FALSE;

		// If there was an error with either part of the closing, return it now
		// As far as deviceio.c is concerned, however, the device is closed
		if (!bResult) {
			mKrnlError[nDevice] = GetLastError();
			return (ekernelclosedevice);
        }

		// Win9x Only: if all devices closed, unload vxd
		if (mdwOS == EXC_OS_WIN9X) {
			if (mAllDevicesClosed()) {

				bResult = CloseHandle(mhDevice9X);
				mhDevice9X = INVALID_HANDLE_VALUE;

				if (!bResult) {
					mKrnlError[nDevice] = GetLastError();
					return (ekernelclosedevice);
				}
			}
		}	
	}
	
	return 0;
}

/*
	MapMemory()

  Description:
		Retrieves application accessible addresses to the memory banks
		of the device.
		Called from init_card.

	Input:
		Number of device
		Number of memory bank (starting from 0)

	Output:
		Returns address in provided buffer
		Returns error message or 0 for success.
*/

//CR1
int MapMemory(int nDevice, void **pMemoryPointer, int nBank)
{
	void	*pDeviceMemoryWindow;
	ULONG   nOutput;
	BOOL    bResult;
	
	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;
	if (nBank < 0 || nBank >= MAX_BANKS)
		return ekernelbadparam;
	if (IsBadWritePtr(pMemoryPointer, sizeof(void *))) 
		return ekernelbadpointer;

	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;
	
	if (mdwOS == EXC_OS_WIN9X) {
		
		t_ExcDeviceIOPacket pktDeviceIO;
		
		pktDeviceIO.nDevice = nDevice;
		pktDeviceIO.dwData = (DWORD) nBank;
		bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_MAPMEM_MAP_DEVICE,
			&pktDeviceIO, sizeof(pktDeviceIO), &pDeviceMemoryWindow, sizeof(void *),
			&nOutput, NULL);
	}
	else {
		bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_MAPMEM_MAP_DEVICE,
			&nBank, sizeof(int), &pDeviceMemoryWindow, sizeof(void *),
			&nOutput, NULL);
	}
	
	if (!bResult) {
		mKrnlError[nDevice] = GetLastError();
		return (ekernelcantmap);
	}
	
	*pMemoryPointer = pDeviceMemoryWindow;
	return 0;
}

/*
	UnMapMemory()

	Description:
		Unmaps all memory banks on given device.

	Input:
		None

	Output:
		Returns error message or 0 for success.
*/


// CR1
int UnMapMemory(int nDevice)
{
	unsigned long   nBytes;
	BOOL			bResult;

	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameter
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;

	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;

	if (mdwOS == EXC_OS_WIN9X) {

        t_ExcDeviceIOPacket pktDeviceIO;

        pktDeviceIO.nDevice = nDevice;
        bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_MAPMEM_UNMAP_DEVICE,
                                &pktDeviceIO, sizeof(pktDeviceIO),NULL,0,&nBytes,NULL);
	}
	else {
		bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_MAPMEM_UNMAP_DEVICE,
				NULL,0,NULL,0,&nBytes,NULL);
	}
	
	if (!bResult) {
		mKrnlError[nDevice] = GetLastError();
		return (eunmapmem);
	}
	
	return 0;
}

//CR1
int Exc_Wait_For_Interrupt(int nDevice) {

	return Exc_Wait_For_Interrupt_Timeout(nDevice, INFINITE);

}

//CR1
int Exc_Wait_For_Interrupt_Timeout(int nDevice, DWORD dwTimeout) {

	int		status;
	HANDLE	hEvent;
	BOOL	bResult;
	DWORD	dwReturn;

	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;

	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;

	status = mGetEventHandle(nDevice, &hEvent);
	if (status < 0) {
		return status;
	}

	dwReturn = WaitForSingleObject(hEvent, dwTimeout);

	if (dwReturn == WAIT_TIMEOUT) {
		return ekerneltimeout;
	}

	ResetEvent(hEvent);
	return (0);
}

//CR1
int Exc_Wait_For_Module_Interrupt(int nDevice, int nModule, DWORD dwTimeout)
{
	HANDLE	hEvent;
	int		status;
	BOOL	bResult;
	DWORD	dwReturn;

	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;
	if ((nModule < 0 || nModule >= MAX_MODULES) && (nModule != SINGLE_MODULE_CARD) )
		return ekernelbadparam;

	// if a single module card is specified, pass the call over to the proper
	// function (which will do its own validation)
	if (nModule == SINGLE_MODULE_CARD) {
		return Exc_Wait_For_Interrupt_Timeout(nDevice, dwTimeout);
	}

	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;

	status = mGetEventHandleForModule(nDevice, nModule, &hEvent);
	if (status < 0) {
		return status;
	}

	dwReturn = WaitForSingleObject(hEvent, dwTimeout);

	if (dwReturn == WAIT_TIMEOUT) {
		return ekerneltimeout;
	}

	ResetEvent(hEvent);

	return (0);
}

int Exc_Request_Interrupt_Notification(int nDevice, int nModule, HANDLE hEvent) {

	// Note that we assume that the handle provided will be an event handle
	// to a manual-reset, non-initially signalled event

	BOOL	bResult;
	ULONG   dwBytesRead;

	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;
	if ((nModule < 0 || nModule >= MAX_MODULES) && (nModule != SINGLE_MODULE_CARD) )
		return ekernelbadparam;

	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;

	// Pass event handle to kernel mode driver
	if (mdwOS == EXC_OS_WIN9X) {

		t_ExcDeviceIOPacket     pktDeviceIO;
		HANDLE					hEventR0;

		pktDeviceIO.nDevice = nDevice;

		// Convert to a ring0 handle to pass down to kernel level
		hEventR0 = pOpenVxDHandle(hEvent);
		if (!hEventR0) {
			return erequestnotification;
		}

		if (nModule == SINGLE_MODULE_CARD) {

			// If this is a single-module card (such as the PCMCIA/EP), send it off directly

			pktDeviceIO.dwData = (DWORD) hEventR0;
			bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_SET_NOTIFICATION_EVENT,
				&pktDeviceIO, sizeof(pktDeviceIO), NULL, 0,
				&dwBytesRead, NULL);
		}
		else {

			// Otherwise, specify the specific module in a SetEventPacket
			t_ExcSetEventPkt		pktSetEvent;

			pktSetEvent.dwModule = nModule;
			pktSetEvent.hEvent = hEventR0;
	
			pktDeviceIO.dwData = (DWORD) &pktSetEvent;

			bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_SET_NOTIFICATION_EVENT_MODULE,
				&pktDeviceIO, sizeof(pktDeviceIO), NULL, 0,
				&dwBytesRead, NULL);
		}
	}
	else {

		// WinNT/2000/XP processing
		// On these operating systems, the event handle is the same in Ring0 as in Ring3

		if (nModule == SINGLE_MODULE_CARD) {

			bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_SET_NOTIFICATION_EVENT, 
				&hEvent, sizeof(hEvent), NULL, 0,
				&dwBytesRead, NULL);
		}
		else {

			t_ExcSetEventPkt			pktSetEvent;

			pktSetEvent.dwModule = nModule;
			pktSetEvent.hEvent = hEvent;

			bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_SET_NOTIFICATION_EVENT_MODULE, 
				&pktSetEvent, sizeof(pktSetEvent), NULL, 0,
				&dwBytesRead, NULL);
		}
	}

	if (!bResult) {
		mKrnlError[nDevice] = GetLastError();
		return (erequestnotification);
	}
	
	return (0);
}

int Exc_Cancel_Interrupt_Notification(int nDevice, int nModule) {

	BOOL    bResult;
	ULONG   dwBytesRead;

	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;
	if ((nModule < 0 || nModule >= MAX_MODULES) && (nModule != SINGLE_MODULE_CARD) )
		return ekernelbadparam;

	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;
	
	if (mdwOS == EXC_OS_WIN9X) {

		t_ExcDeviceIOPacket pktDeviceIO;
		pktDeviceIO.nDevice = nDevice;

		if (nModule == SINGLE_MODULE_CARD) {
			
			bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_CLEAR_NOTIFICATION_EVENT,
				&pktDeviceIO, sizeof(pktDeviceIO), NULL, 0, &dwBytesRead, NULL);
		}
		else {
			pktDeviceIO.dwData = nModule;
			bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_CLEAR_NOTIFICATION_EVENT_MODULE,
				&pktDeviceIO, sizeof(pktDeviceIO), NULL, 0, &dwBytesRead, NULL);
		}
	}
		
	else {

		if (nModule == SINGLE_MODULE_CARD) {
			bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_CLEAR_NOTIFICATION_EVENT,
				NULL, 0, NULL, 0, &dwBytesRead, NULL);
		}
		else {
			bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_CLEAR_NOTIFICATION_EVENT_MODULE,
				&nModule, sizeof(&nModule), NULL, 0, &dwBytesRead, NULL);
		}
	}

	if (!bResult) {
		return (ereleventhandle);
	}

	return(0);
}

//CR1
int Exc_Wait_For_Multiple_Interrupts(int numDevModulePairs, t_ExcDevModPair *DevModPairs, DWORD dwTimeout, DWORD *pdwInterruptBitfield)
{
	// Wait for an interrupt on any one of the specified modules
	// The location of each module is specified via a device/module pair
	// When returning, provide a bitfield of which of the specified modules have interrupted 
	// (note that more than one may have interrupted simultaenously)

	HANDLE			hEvents[MAX_DEVICES * MAX_MODULES];
	DWORD			dwBitfield;
	int				i;
	int				status;
	BOOL			bResult;
	DWORD			dwReturn;

	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate number of modules is within our defines
	if (numDevModulePairs <= 0 || numDevModulePairs > MAX_DEVICES * MAX_MODULES)
		return ekernelbadparam;

	// Validate that the bitfield will fit in the DWORD bitfield
	if (numDevModulePairs > sizeof(DWORD)) 
		return ekernelbadparam;

	// Validate output buffer
	if (IsBadWritePtr(pdwInterruptBitfield, sizeof(DWORD)))
		return ekernelbadpointer;

	for (i=0; i < numDevModulePairs; i++) {

		// Validate dev and mod parameters
		if (DevModPairs[i].nDevice < 0 || DevModPairs[i].nDevice >= MAX_DEVICES) 
			return ekernelbadparam;
		if ( (DevModPairs[i].nModule < 0 || DevModPairs[i].nModule >= MAX_MODULES) && (DevModPairs[i].nModule != SINGLE_MODULE_CARD) )
			return ekernelbadparam;

		// Validate that card is open
		if (!mfDeviceOpen[DevModPairs[i].nDevice])
			return ekerneldevicenotopen;

		// Get Semaphore
		if (DevModPairs[i].nModule == SINGLE_MODULE_CARD)
			status = mGetEventHandle(DevModPairs[i].nDevice, &hEvents[i]);
		else
			status = mGetEventHandleForModule(DevModPairs[i].nDevice, DevModPairs[i].nModule, &hEvents[i]);

		if (status < 0) {
			return status;
		}	
	}

	dwReturn = WaitForMultipleObjects(numDevModulePairs, hEvents, FALSE, dwTimeout);
	if (dwReturn == WAIT_TIMEOUT) {
		*pdwInterruptBitfield = 0;
		return ekerneltimeout;
	}

	// Build bitfield to send back
	// Note that multiple objects may signal at the same time during a WaitForMultipleObjects call
	dwBitfield = 0;
	for (i=0; i < numDevModulePairs; i++) {

		DWORD dwWait;

		// Checking the state of a synch object in the WinAPI
		// is performed via a call to WaitForSingleObj with a timeout value of 0.
		dwWait = WaitForSingleObject(hEvents[i], 0);

		if (dwWait == WAIT_OBJECT_0) {

			// object is signalled 
			dwBitfield |= (1<<i);

			// reset object
			ResetEvent(hEvents[i]);
		}
	}

	*pdwInterruptBitfield = dwBitfield;
	return (0);
}

int	  Exc_Initialize_Interrupt(int nDevice) {

	HANDLE hEvent;

	// Force the creation of an interrupt event
	// by calling geteventhandle

	// note:
	// a] validity checking will be done there
	// b] we do not need to save the event handle, since this
	//    is saved internally in static module data
	return mGetEventHandle(nDevice, &hEvent);
}


int	  Exc_Initialize_Interrupt_For_Module(int nDevice, int nModule) {

	HANDLE hEvent;

	// Force the creation of an interrupt event
	// by calling geteventhandle

	// If a single module card is specified, pass the call over to the proper
	// function
	if (nModule == SINGLE_MODULE_CARD) {
		return Exc_Initialize_Interrupt(nDevice);
	}

	// note:
	// a] validity checking will be done there
	// b] we do not need to save the event handle, since this
	//    is saved internally in static module data
	return mGetEventHandleForModule(nDevice, nModule, &hEvent);
}

//////////////////////////
// mGetEventHandle()
// Retrieves a Ring0/Ring3 event for given device.
// If such an event has not yet been created, 
//   a] Create the event
//   b] Pass a copy of the handle to Ring 0

// CR1
static int mGetEventHandle(int nDevice, HANDLE *phEvent)
{
	// Create a semaphore ("event") so kernel driver can notify us
	ULONG                   dwBytesRead;
	BOOL                    bResult;
	HANDLE                  hEventR0, hEventR3;

	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;
	if (IsBadWritePtr(phEvent, sizeof(HANDLE *))) 
		return ekernelbadpointer;

	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;
	
	// If we have not yet created an event handle, create it
	// and pass the handle down to the driver.
	if (mhEvent[nDevice] == NULL) {

		// Create Event
		// We create a manual-reset event, to allow us to determine
		// the states of multiple objects.
		hEventR3 = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (hEventR3 == NULL) {
			return egeteventhand1;
		}
		mhEvent[nDevice] = hEventR3;

		// Win9x: convert to a ring0 handle to pass down to kernel level
		if (mdwOS == EXC_OS_WIN9X) {

			hEventR0 = pOpenVxDHandle(hEventR3);
			if (!hEventR0) {
				return egeteventhand1;
			}
		}

		// Pass event handle to kernel mode driver.
		if (mdwOS == EXC_OS_WIN9X) {

			t_ExcDeviceIOPacket      pktDeviceIO;

			pktDeviceIO.nDevice = nDevice;
			pktDeviceIO.dwData = (DWORD) hEventR0;
		
			bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_SET_NOTIFICATION_EVENT,
				&pktDeviceIO, sizeof(pktDeviceIO), NULL, 0,
				&dwBytesRead, NULL);
		}
		else {
			bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_SET_NOTIFICATION_EVENT, 
				&mhEvent[nDevice], sizeof(mhEvent[nDevice]), NULL, 0,
				&dwBytesRead, NULL);
		}

		if (!bResult) {
			mKrnlError[nDevice] = GetLastError();
			return (egeteventhand2);
		}
	}

	// .. return event handle to calling function
	*phEvent = mhEvent[nDevice];
	return(0);
}

//CR1
// Note: this private method only support multiple-module cards. For single
// module cards, use mGetEventHandle()
static int mGetEventHandleForModule(int nDevice, int nModule, HANDLE *phEvent)
{
	// Create a semaphore for this specific module
	ULONG                   dwBytesRead;
	BOOL                    bResult;
	HANDLE                  hEventR0, hEventR3;

	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;
	if (nModule < 0 || nModule >= MAX_MODULES)
		return ekernelbadparam;
	if (IsBadWritePtr(phEvent, sizeof(HANDLE *))) 
		return ekernelbadpointer;

	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;

	// If we have not already created an event
	if (mhEventForModule[nDevice][nModule] == NULL) {

		// Create Event
		// We create a manual-reset event, to allow us to determine
		// the states of multiple objects.
		hEventR3 = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (hEventR3 == NULL) {
			return egeteventhand1;
		}
		mhEventForModule[nDevice][nModule] = hEventR3;

		// Win9x: convert to a ring0 handle to pass down to kernel level
		if (mdwOS == EXC_OS_WIN9X) {

			hEventR0 = pOpenVxDHandle(hEventR3);
			if (!hEventR0) {
				return egeteventhand1;
			}
		}

		// Pass event handle to kernel mode driver
		if (mdwOS == EXC_OS_WIN9X) {

			t_ExcDeviceIOPacket      pktDeviceIO;
			t_ExcSetEventPkt			pktSetEvent;

			pktDeviceIO.nDevice = nDevice;
			pktDeviceIO.dwData = (DWORD) &pktSetEvent;

			// For Win9x, there is a separate Ring 0 handle which we retrieved above.
			pktSetEvent.dwModule = nModule;
			pktSetEvent.hEvent = hEventR0;

			bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_SET_NOTIFICATION_EVENT_MODULE,
				&pktDeviceIO, sizeof(pktDeviceIO), NULL, 0,
				&dwBytesRead, NULL);
		}
		else {

			t_ExcSetEventPkt			pktSetEvent;

			// For WinNT/2000, the event handle is the same in Ring0 as in Ring3.
			pktSetEvent.dwModule = nModule;
			pktSetEvent.hEvent = hEventR3;

			bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_SET_NOTIFICATION_EVENT_MODULE, 
				&pktSetEvent, sizeof(pktSetEvent), NULL, 0,
				&dwBytesRead, NULL);
		}

		if (!bResult) {
			mKrnlError[nDevice] = GetLastError();
			return (egeteventhand2);
		}
	}

	// .. return event handle to caller
	*phEvent = mhEventForModule[nDevice][nModule];
	return(0);
}


/*
	Release_Event_Handle()

	Description:
		Releases event allocated by GetEventHandle().

    Input:
		None

	Output:
		Returns error message or 0 for success.
*/
// CR1
int Release_Event_Handle(int nDevice)
{
	BOOL    bResult;
	ULONG   dwBytesRead;

	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;

	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;

	if (mhEvent[nDevice]) {

		if (mdwOS == EXC_OS_WIN9X) {
		
			t_ExcDeviceIOPacket      pktDeviceIO;
	
			pktDeviceIO.nDevice = nDevice;
			
			bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_CLEAR_NOTIFICATION_EVENT,
				&pktDeviceIO, sizeof(pktDeviceIO), NULL, 0, &dwBytesRead, NULL);
		}
		else {
			bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_CLEAR_NOTIFICATION_EVENT,
				NULL, 0, NULL, 0, &dwBytesRead, NULL);
		}

		if (!bResult) {
			return (ereleventhandle);
		}

		CloseHandle(mhEvent[nDevice]);
		mhEvent[nDevice] = NULL;
	}
	
	return(0);
}

// CR1
// Note: this method only supports multiple-module cards. For single
// module cards, use Release_Event_Handle()
int Release_Event_Handle_For_Module(int nDevice, int nModule)
{
	BOOL    bResult;
	ULONG   dwBytesRead;

	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;
	if (nModule < 0 || nModule >= MAX_MODULES)
		return ekernelbadparam;

	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;
	
	if (mhEventForModule[nDevice][nModule]) {

		if (mdwOS == EXC_OS_WIN9X) {
		
			t_ExcDeviceIOPacket      pktDeviceIO;

			pktDeviceIO.nDevice = nDevice;
			pktDeviceIO.dwData = nModule;

			bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_CLEAR_NOTIFICATION_EVENT_MODULE,
				&pktDeviceIO, sizeof(pktDeviceIO), NULL, 0, &dwBytesRead, NULL);
		}
		
		else {

			bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_CLEAR_NOTIFICATION_EVENT_MODULE,
				&nModule, sizeof(&nModule), NULL, 0, &dwBytesRead, NULL);
		}

		if (!bResult) {
			return (ereleventhandle);
		}

		CloseHandle(mhEventForModule[nDevice][nModule]);
		mhEventForModule[nDevice][nModule] = NULL;
	}
	
	return(0);
}


/*
	Exc_Get_Interrupt_Count()

	Description:
		Returns total interrupt count for the device, from the time the device 
		was opened with OpenKernelDevice().

	Input:
		Pointer to buffer to receive total interrupts

	Output:
		Returns interrupt count in provided buffer
		Returns error message or 0 for success.
*/

// CR1
int  Exc_Get_Interrupt_Count(int nDevice, unsigned long *pdwCount) {

	DWORD   dwInterrupts, dwBytesRead;
	BOOL    bResult;
	
	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}
	
	// Validate input parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;
	if (IsBadWritePtr(pdwCount, sizeof(unsigned long *))) 
		return ekernelbadpointer;
	
	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;
	
	if (mdwOS == EXC_OS_WIN9X) {
		
		t_ExcDeviceIOPacket      pktDeviceIO;
		
		pktDeviceIO.nDevice = nDevice;
		
		bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_GET_INTERRUPT_COUNT,
			&pktDeviceIO, sizeof(pktDeviceIO), &dwInterrupts, sizeof(dwInterrupts),&dwBytesRead,NULL);
	}
	else {
		bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_GET_INTERRUPT_COUNT,
			NULL, 0, &dwInterrupts, sizeof(dwInterrupts),&dwBytesRead,NULL);
	}
	
	if (!bResult) {
		mKrnlError[nDevice] = GetLastError();
		return (egetintcount);
	}
	
	*pdwCount = dwInterrupts;
	return(0);
}

/*
	Exc_Get_Channel_Interrupt_Count()

	Description:
		Retrieves total interrupt count for a particular channel.

	Input:
		Number of channel

	Output:
		Returns total interrupts in provided buffer
		Returns error message or 0 for success.
*/

//CR1
int Exc_Get_Channel_Interrupt_Count(int nDevice, unsigned int dwModule, unsigned long *pdwCount)
{
	DWORD	dwInterrupts, dwBytesRead;
	BOOL    bResult;
	
	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;
	if ((dwModule >= MAX_MODULES) && (dwModule != SINGLE_MODULE_CARD) )
		return ekernelbadparam;
	if (IsBadWritePtr(pdwCount, sizeof(unsigned long *))) 
		return ekernelbadpointer;

	// if a single module card is specified, pass the call over to the proper
	// function (which will do its own validation)
	if (dwModule == SINGLE_MODULE_CARD) {
		return Exc_Get_Interrupt_Count(nDevice, pdwCount);
	}

	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;
	
	if (mdwOS == EXC_OS_WIN9X) {
		
		t_ExcDeviceIOPacket pktDeviceIO;
		
		pktDeviceIO.nDevice = nDevice;
		pktDeviceIO.dwData = (DWORD) dwModule;
		
		bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_GET_MCH_INTERRUPT_COUNT,
			&pktDeviceIO, sizeof(pktDeviceIO), &dwInterrupts, sizeof(dwInterrupts),
			&dwBytesRead, NULL);
	}
	else {
		bResult = DeviceIoControl(mhDeviceNT[nDevice],EXC_IOCTLNT_GET_MCH_INTERRUPT_COUNT,
			&dwModule, sizeof(dwModule), &dwInterrupts, sizeof(dwInterrupts),
			&dwBytesRead, NULL);
	}
	
	if (!bResult) {
		mKrnlError[nDevice] = GetLastError();
		return (egetchintcount);
	}
	
	*pdwCount = dwInterrupts;
	return(0);
}

/*
	Exc_Get_Interrupt_Channels()

	Description:
		Retrieves a bitfield showing which channels/modules on the specified
		card have interrupted since the last time this function was called.

	Input:
		Device number

	Output:
		Returns bitfield in provided buffer
		Returns error message or 0 for success.
*/

// CR1
int  Exc_Get_Interrupt_Channels(int nDevice, BYTE *pBitfield)
{
	ULONG   dwBytesRead;
	BYTE    Bitfield;
	BOOL    bResult;
	
	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;
	if (IsBadWritePtr(pBitfield, sizeof(BYTE *))) 
		return ekernelbadpointer;
	
	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;
	
	if (mdwOS == EXC_OS_WIN9X) {
		
		t_ExcDeviceIOPacket      pktDeviceIO;
		
		pktDeviceIO.nDevice = nDevice;
		
		bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_GET_MCH_INT_SHADOW_REGISTER,
			&pktDeviceIO, sizeof(pktDeviceIO), &Bitfield, sizeof(Bitfield),
			&dwBytesRead, NULL);
	}
	else {
		bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_GET_MCH_INT_SHADOW_REGISTER,
			NULL, 0, &Bitfield, sizeof(Bitfield),
			&dwBytesRead, NULL);
	}
	
	if (!bResult) {
		mKrnlError[nDevice] = GetLastError();
		return(egetintchannels);
	}
	
	
	*pBitfield = Bitfield;
	return(0);
}

/*
	WriteIOByte()

	Description:
		Provide IO access to the device.

	Input:
		Offset to write to
		Value to write

	Output:
		Returns error message or 0 for success.
*/

// CR1
BOOL WriteIOByte(int nDevice, ULONG dwOffset, BYTE value)
{
	BOOL    bResult;
	ULONG   bytesRead;
	
	t_ExcWriteIOPacket       WriteIOInfo;
	
	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;

	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;
	
	WriteIOInfo.offset = dwOffset;
	WriteIOInfo.value  = value;
	
	if (mdwOS == EXC_OS_WIN9X) {
		
		t_ExcDeviceIOPacket      pktDeviceIO;
		
		pktDeviceIO.nDevice = nDevice;
		pktDeviceIO.dwData = (DWORD) &WriteIOInfo;
		
		bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_WRITEIOBYTE,
			&pktDeviceIO, sizeof(pktDeviceIO), NULL, 0,
			&bytesRead,NULL);
	}
	else {
		bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_WRITEIOBYTE, 
			&WriteIOInfo, sizeof(WriteIOInfo), NULL, 0,
			&bytesRead,NULL);
	}
	
	if (!bResult)
		return(ewriteiobyte);
	
	return (0);
}

/*
	ReadIOByte()

	Description:
		Used to read device's IO space

	Input:
		Device number
		Offset to read

	Output:
		Returns value in provided buffer
		Returns error message or 0 for success.
*/

// CR1
BOOL ReadIOByte(int nDevice, ULONG dwOffset, BYTE *pValue)
{
	BOOL    bResult;
	ULONG   bytesRead;

	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;
	if (IsBadWritePtr(pValue, sizeof(BYTE *))) 
		return ekernelbadpointer;
	
	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;
	
	if (mdwOS == EXC_OS_WIN9X) {
		
		t_ExcDeviceIOPacket pktDeviceIO;
		
		pktDeviceIO.nDevice = nDevice;
		pktDeviceIO.dwData = dwOffset;
		
		bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_READIOBYTE_ROBUST,
			&pktDeviceIO, sizeof(pktDeviceIO), pValue, sizeof(BYTE),
			&bytesRead,NULL);
	}
	else {
		bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_READIOBYTE_ROBUST, 
			&dwOffset, sizeof(dwOffset), pValue, sizeof(BYTE),
			&bytesRead,NULL);
	}

	if (!bResult) {

		// New readiobyte IOCTL not supported by this driver; use original IOCTL
		// which is maintained for backwards compatibility ONLY

		// Note that counterintuitively, the design of this IOCTL
		// takes the output buffer as part of the input packet.

		t_ExcReadIOPacket ReadIOInfo;

		ReadIOInfo.offset = dwOffset;
		ReadIOInfo.pValue = pValue;

		if (mdwOS == EXC_OS_WIN9X) {
		
			t_ExcDeviceIOPacket pktDeviceIO;
		
			pktDeviceIO.nDevice = nDevice;
			pktDeviceIO.dwData = (DWORD) &ReadIOInfo;
		
			bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_READIOBYTE,
				&pktDeviceIO, sizeof(pktDeviceIO), NULL, 0,
				&bytesRead,NULL);
		}
		else {
			bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_READIOBYTE, 
				&ReadIOInfo, sizeof(ReadIOInfo), NULL, 0,
				&bytesRead,NULL);
		}
	}
	
	if (!bResult) {
		mKrnlError[nDevice] = GetLastError();
		return(ereadiobyte);
	}
	
	return (0);
}

//CR1
int WriteAttributeMemory(int nDevice, ULONG dwOffset, WORD value)
{
	BOOL	bResult;
	ULONG	bytesRead;
	t_ExcWriteAttribPacket	WriteAttribInfo;

	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;

	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;

	WriteAttribInfo.offset = dwOffset;
	WriteAttribInfo.value  = value;

	if (mdwOS == EXC_OS_WIN9X) {

		t_ExcDeviceIOPacket      pktDeviceIO;

	    pktDeviceIO.nDevice = nDevice;
	    pktDeviceIO.dwData = (DWORD) &WriteAttribInfo;
		bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_WRITE_ATTRIB, 
				&pktDeviceIO, sizeof(pktDeviceIO), NULL, 0,
				&bytesRead,NULL);
	}
	else {
		bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_WRITE_ATTRIB, 
			&WriteAttribInfo, sizeof(WriteAttribInfo), NULL, 0,
			&bytesRead,NULL);
	}

	if (!bResult)
		return(ekernelwriteattrib);

	return (0);
}

// CR1
int ReadAttributeMemory(int nDevice, ULONG dwOffset, WORD *pValue)
{
	BOOL	bResult;
	ULONG	bytesRead;

	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;
	if (IsBadWritePtr(pValue, sizeof(WORD *))) 
		return ekernelbadpointer;

	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;

	if (mdwOS == EXC_OS_WIN9X) {
		
		t_ExcDeviceIOPacket pktDeviceIO;
		
		pktDeviceIO.nDevice = nDevice;
		pktDeviceIO.dwData = dwOffset;
		
		bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_READ_ATTRIB_ROBUST,
			&pktDeviceIO, sizeof(pktDeviceIO), pValue, sizeof(WORD),
			&bytesRead,NULL);
	}
	else {
		bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_READ_ATTRIB_ROBUST, 
			&dwOffset, sizeof(dwOffset), pValue, sizeof(WORD),
			&bytesRead,NULL);
	}

	if (!bResult) {

		// New readiobyte IOCTL not supported by this driver; use original IOCTL
		// which is maintained for backwards compatibility ONLY

		// Note that counterintuitively, the design of this IOCTL
		// takes the output buffer as part of the input packet.

		t_ExcReadAttribPacket	ReadAttribInfo;

		ReadAttribInfo.offset = dwOffset;
		ReadAttribInfo.pValue = pValue;

		if (mdwOS == EXC_OS_WIN9X) {
	
			t_ExcDeviceIOPacket pktDeviceIO;

			pktDeviceIO.nDevice = nDevice;
			pktDeviceIO.dwData = (DWORD) &ReadAttribInfo;

			bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_READ_ATTRIB, 
						&pktDeviceIO, sizeof(pktDeviceIO), NULL, 0,
						&bytesRead,NULL);
		}
		else {
			bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_READ_ATTRIB, 
						&ReadAttribInfo, sizeof(t_ExcReadAttribPacket), NULL, 0,
						&bytesRead,NULL);
		}
	}

	if (!bResult) {
		mKrnlError[nDevice] = GetLastError();
		return(ekernelreadattrib);
	}

	return (0);
}

// CR1
int GetRamSize(int nDevice, unsigned long *pdwSize)
{
	ULONG   dwSize, dwBytesRead;
	BOOL    bResult;

	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;
	if (IsBadWritePtr(pdwSize, sizeof(unsigned long *))) 
		return ekernelbadpointer;

	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;

	if (mdwOS == EXC_OS_WIN9X) {

        t_ExcDeviceIOPacket      pktDeviceIO;

        pktDeviceIO.nDevice = nDevice;

        bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_GET_RAM_SIZE,
                        &pktDeviceIO, sizeof(pktDeviceIO), &dwSize, sizeof(dwSize),&dwBytesRead,NULL);

	}
	else {

		bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_GET_RAM_SIZE,
				NULL, 0, &dwSize, sizeof(dwSize),
				&dwBytesRead, NULL);
	}

	if (!bResult) {
		mKrnlError[nDevice] = GetLastError();
		return (egetramsize);
	}

	*pdwSize = dwSize;
	return(0);
}

// CR1
int GetBankRamSize(int nDevice, unsigned long *pdwSize, int nBank)
{
	ULONG   dwBytesRead;
	BOOL    bResult;

	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;
	if (nBank < 0 || nBank >= MAX_BANKS)
		return ekernelbadparam;
	if (IsBadWritePtr(pdwSize, sizeof(unsigned long *))) 
		return ekernelbadpointer;

	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;

	if (mdwOS == EXC_OS_WIN9X) {

		t_ExcDeviceIOPacket pktDeviceIO;

		pktDeviceIO.nDevice = nDevice;
		pktDeviceIO.dwData = (DWORD) nBank;

		bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_GET_BANK_RAM_SIZE,
					&pktDeviceIO, sizeof(pktDeviceIO), pdwSize, sizeof(unsigned long),
					&dwBytesRead, NULL);
	}
	else {
	    bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_GET_BANK_RAM_SIZE,
					&nBank, sizeof(int), pdwSize, sizeof(unsigned long),
					&dwBytesRead, NULL);
	}

	if (!bResult) {
		mKrnlError[nDevice] = GetLastError();
		return (ekernelbankramsize);
	}
	return 0;
}

// CR1
int GetBankPhysAddress(int nDevice, DWORD *pdwAddress, int nBanks)
{
	ULONG   dwBytesRead;
	BOOL    bResult;

	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;
	if (nBanks < 0 || nBanks >= MAX_BANKS)
		return ekernelbadparam;
	if (IsBadWritePtr(pdwAddress, sizeof(DWORD *))) 
		return ekernelbadpointer;

	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;
	
	if (mdwOS == EXC_OS_WIN9X) {
		
		t_ExcDeviceIOPacket pktDeviceIO;
		
		pktDeviceIO.nDevice = nDevice;
		pktDeviceIO.dwData = (DWORD) nBanks;
		
		bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_GET_BANK_PHYS_ADDR,
			&pktDeviceIO, sizeof(pktDeviceIO), pdwAddress, sizeof(pdwAddress),
			&dwBytesRead, NULL);
	}
	else {
		bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_GET_BANK_PHYS_ADDR,
			&nBanks, sizeof(nBanks), pdwAddress, sizeof(pdwAddress),
			&dwBytesRead, NULL);
	}
		
	if (!bResult) {
		mKrnlError[nDevice] = GetLastError();
		return (ekernelbankphysaddr);
	}
	return 0;
}

//CR1
int GetCardType(int nDevice, unsigned long *pdwCardType)
{
	ULONG   dwBytesRead;
	BOOL    bResult;
	
	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;
	if (IsBadWritePtr(pdwCardType, sizeof(unsigned long *))) 
		return ekernelbadpointer;
	
	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;
	
	if (mdwOS == EXC_OS_WIN9X) {
		
		t_ExcDeviceIOPacket pktDeviceIO;
		
		pktDeviceIO.nDevice = nDevice;
		
		bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_GET_CARD_TYPE,
			&pktDeviceIO, sizeof(pktDeviceIO), pdwCardType, sizeof(pdwCardType),
			&dwBytesRead, NULL);
	}
	else {
		bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_GET_CARD_TYPE,
			NULL, 0, pdwCardType, sizeof(pdwCardType),
			&dwBytesRead, NULL);
	}
	
	if (!bResult) {
		mKrnlError[nDevice] = GetLastError();
		return (ekernelgetcardtype);
	}
	return 0;
}

/*
	Get_IRQ_Number()

	Description:
		Retrieves the number of the IRQ allocated for this device, or 0
		to indicate that the device does not use interrupts.

	Input:
		Device Number

	Output:
		Returns IRQ number (or 0) in provided buffer
		Returns error message or 0 for success.
*/

int Get_IRQ_Number(int nDevice, int *pnIRQ)
{
	int		nIRQ;
	ULONG   dwBytesRead;
	BOOL    bResult;
	
	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;
	if (IsBadWritePtr(pnIRQ, sizeof(int *))) 
		return ekernelbadpointer;
	
	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;
	
	if (mdwOS == EXC_OS_WIN9X) {
		
		t_ExcDeviceIOPacket      pktDeviceIO;
		
		pktDeviceIO.nDevice = nDevice;
		
		bResult = DeviceIoControl(mhDevice9X, EXC_IOCTL9X_GET_IRQ_NUMBER,
			&pktDeviceIO, sizeof(pktDeviceIO), &nIRQ, sizeof(nIRQ),&dwBytesRead,NULL);
	}
	else {
		bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_GET_IRQ_NUMBER,
			NULL, 0, &nIRQ, sizeof(nIRQ),&dwBytesRead,NULL);
	}
	
	if (!bResult) {
		mKrnlError[nDevice] = GetLastError();
		return (egetirq);
	}
	
	*pnIRQ = nIRQ;
	return(0);
}



//////////////////////////////
// IsWin2000Compatible()
//   If running under Win2000, checks whether the card is Win2000 compatible.
//   If running under Win9x or WinNT, will always return TRUE, since there is
//   no compatiblity issue.

// CR1
int IsWin2000Compatible(int nDevice, BOOL *pfCompatible) {

	BOOL	bResult;
	ULONG	dwBytesRead;

	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;
	if (IsBadWritePtr(pfCompatible, sizeof(BOOL *))) 
		return ekernelbadpointer;

	// if device is not open, return error
	if (!mfDeviceOpen[nDevice])
		return ekerneldevicenotopen;

	if (mdwOS == EXC_OS_WIN9X || mdwOS == EXC_OS_WINNT4) {
		return ekernelnotwin2000;
	}
	else {

		bResult = DeviceIoControl(mhDeviceNT[nDevice], EXC_IOCTLNT_IS_WIN2000_COMPATIBLE, 
			NULL, 0, pfCompatible, sizeof(BOOL),
			&dwBytesRead,NULL);

		if (!bResult)
			return (ekerneliswin2000compatible);
	}

	return 0;
}

// CR1
BOOL IsWin2000() {

	if (!mfInitialized) {

		// Called out of order. Do initialize now.
		// Note that for our purposes here, we do not bother with an error return;
		// if the GetOS failed, mdwOS will remain at EXC_OS_UNDEFINED.
		mInitialize();
	}

	return (mdwOS == EXC_OS_WIN2000);
}

/*
	ChangeCurrentDevice()
	Obsolete Function

	Description:
		Early versions of deviceio.c maintained a module variable called
		nDevice which was set with this function. However, currently,
		every function takes a device parameter, such that there is no need or use
		for a module variable of this nature.
		It is kept for compatibility purposes, since older version of the Software
		Drivers call this function from SetCurrentCard().
*/

// CR1
int ChangeCurrentDevice(int nNewDevice) {
	return 0;
}

/*
	Exc_Get_Last_Kernel_Error()

	Description:
		Retrieves the specific error status from the last operation.

		Most functions in this module return only an error which is specific
		to that function, but generic across all possible errors within the 
		function.  However, the specific error code is saved, and is retrievable
		via this function.

	Input:
		Device Number

	Output:
		Returns the specific error status
*/
// CR1
DWORD Exc_Get_Last_Kernel_Error(int nDevice)
{
	BOOL	bResult;

	if (!mfInitialized) {
		// Called out of order. Do init to ensure accurate error return.
		bResult = mInitialize();
		if (!bResult) return ekernelinitmodule;
	}

	// Validate input parameters
	if (nDevice < 0 || nDevice >= MAX_DEVICES) 
		return ekernelbadparam;
	
	return mKrnlError[nDevice];
}


////////////////////////////
// Internal Service Functions

// CR1
static BOOL mInitialize() {

	int		dev, mod;
	BOOL	bResult;

	// Set OS type
	bResult = mSetOS();
	if (!bResult) return FALSE;

	// Device Handles
	// The WinAPI default value for device handles (returned by CreateFile on error)
	// is INVALID_HANDLE_VALUE. (Note that all other handles default to NULL in the Win API.)
	mhDevice9X = INVALID_HANDLE_VALUE;
	for (dev = 0; dev < MAX_DEVICES; dev++)
		mhDeviceNT[dev] = INVALID_HANDLE_VALUE;

	// Reference Counts
	for (dev = 0; dev < MAX_DEVICES; dev++) {
		mfDeviceOpen[dev]  = FALSE;
		mdwDevRefCount[dev] = 0;
	}

	// Interrupt Semaphores: per device
	for (dev = 0; dev < MAX_DEVICES; dev++)
		mhEvent[dev] = NULL;

	// Interrupt Semaphores: per module
	for (dev = 0; dev < MAX_DEVICES; dev++) {
		for (mod = 0; mod < MAX_MODULES; mod++) {
			mhEventForModule[dev][mod] = NULL;
		}
	}

	// Specific kernel error messages
	for (dev = 0; dev < MAX_DEVICES; dev++)
		mKrnlError[dev] = 0;

	// Routine to convert Win9x events to kernel level
	if (mdwOS == EXC_OS_WIN9X) {
	    pOpenVxDHandle = mGetAddressOfOpenVxDHandle();
		if (pOpenVxDHandle == NULL) {
			return FALSE;
		}
	}

	mfInitialized = TRUE;

	return TRUE;
}

// CR1
static	BOOL mSetOS() {

	BOOL			bResult;

	OSVERSIONINFO	osVer;
	osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	bResult = GetVersionEx(&osVer);
	if (!bResult) return FALSE;

	if (osVer.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
		mdwOS = EXC_OS_WIN9X;
	}
	else {
		if (osVer.dwMajorVersion == 4) {
			mdwOS = EXC_OS_WINNT4;
		}
		else {
			mdwOS = EXC_OS_WIN2000;
		}
	}

	return TRUE;
}

// CR1
static	BOOL mAllDevicesClosed() {

	int dev;

	for (dev = 0; dev < MAX_DEVICES; dev++) {
		if (mfDeviceOpen[dev]) return FALSE;
	}

	return TRUE;
}

// -------------------------------------------------------------------------
// mGetAddressOfOpenVxDHandle
//
// This function returns the address of OpenVxDHandle. OpenVxDHandle is a
// KERNEL32 function that returns a ring 0 event handle that corresponds to a
// given ring 3 event handle. The ring 0 handle can be used by VxDs to
// synchronize with the Win32 app.
//
// This function is copyright (c) 1995, Vireo Software, Inc.
// Used with permission.

static	HANDLE (WINAPI *mGetAddressOfOpenVxDHandle(void))(HANDLE)
{
        CHAR K32Path[MAX_PATH];
        HINSTANCE hK32;

        GetSystemDirectory(K32Path, MAX_PATH);
        strcat(K32Path, "\\kernel32.dll");
        if ((hK32 = LoadLibrary(K32Path)) == 0)
                return NULL;

        return (HANDLE(WINAPI *)(HANDLE))GetProcAddress(hK32, "OpenVxDHandle");
}

