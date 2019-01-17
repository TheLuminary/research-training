
// DeviceIO.C
// Routines for communicating with our kernel driver

#ifndef __DEVICEIO_H_
#define __DEVICEIO_H_

#include <windows.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define SINGLE_MODULE_CARD 0xFF

typedef struct {

	int nDevice;
	int nModule;

} t_ExcDevModPair;

// Functions called by user (through exported cover functions)
int   Exc_Get_Interrupt_Count(int nDevice, unsigned long int *pdwTotal);
int   Exc_Get_Interrupt_Channels(int nDevice, BYTE *pBitfield);
int   Exc_Get_Channel_Interrupt_Count(int nDevice, unsigned int nModule, unsigned long int *pdwTotal);

int   Exc_Wait_For_Interrupt(int nDevice);
int   Exc_Wait_For_Interrupt_Timeout(int nDevice, DWORD dwTimeout);

int   Exc_Wait_For_Module_Interrupt(int nDevice, int nModule, DWORD dwTimeout);
int   Exc_Wait_For_Multiple_Interrupts(int numDevModulePairs, t_ExcDevModPair *DevModPairs, DWORD dwTimeout, DWORD *pdwInterruptBitfield);

int	  Exc_Initialize_Interrupt(int nDevice);
int	  Exc_Initialize_Interrupt_For_Module(int nDevice, int nModule);

int   Exc_Request_Interrupt_Notification(int nDevice, int nModule, HANDLE hEvent);
int   Exc_Cancel_Interrupt_Notification(int nDevice, int nModule);

DWORD Exc_Get_Last_Kernel_Error(int nDevice);

// Functions used internally by Excalibur Software Tools
int ChangeCurrentDevice(int nDevice);
int OpenKernelDevice(int nDevice);
int CloseKernelDevice(int nDevice);
int MapMemory(int nDevice, void **pMemory, int nModule);
int UnMapMemory(int nDevice);
int Release_Event_Handle(int nDevice); 
int Release_Event_Handle_For_Module(int nDevice, int nModule);
int WriteIOByte(int nDevice, ULONG offset, BYTE value);
int ReadIOByte(int nDevice, ULONG offset, BYTE *pValue);
int GetRamSize(int nDevice, unsigned long *lRamSize);
int Get_IRQ_Number(int nDevice, int *pnIRQ);
int WriteAttributeMemory(int nDevice, ULONG offset, WORD value);
int ReadAttributeMemory(int nDevice, ULONG offset, WORD *pValue);
int GetBankRamSize(int nDevice, unsigned long *dwSize, int nModule);
int GetCardType(int nDevice, unsigned long *dwCardType);

BOOL IsWin2000();
int  IsWin2000Compatible(int nDevice, BOOL *fCompatible);

#ifdef __cplusplus
}
#endif

#endif  // end ifndef DEVICEIO_H


