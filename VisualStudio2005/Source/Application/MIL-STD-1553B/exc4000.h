
// DeviceIO.C
// Routines for communicating with our kernel driver

#ifndef __EXC4000_H_
#define __EXC4000_H_

#include <windows.h>
#define NUMBOARDS 4

/* Irig stuff - for user */
#define IRIG_TIME_AND_RESET	0x100
#define IRIG_TIME		0x200

#define TIMER_RELOAD		1
#define TIMER_NO_RELOAD		0
#define TIMER_INTERRUPT		1
#define TIMER_NO_INTERRUPT	0
#define TIMER_GLOBRESET		1
#define TIMER_NO_GLOBRESET	0

// the timer on M4K cards is treated as module 4
#define EXC4000_MODULE_TIMER	4

typedef struct{
	unsigned short int days;
	unsigned short int hours;
	unsigned short int minutes;
	unsigned short int seconds;
}t_IrigTime;

// Prototypes
// Exported

#ifdef __cplusplus
extern "C"
{
#endif

int __declspec(dllexport) Get_4000Module_Type(WORD device_num, WORD module_num, WORD *modtype);
int __declspec(dllexport) Get_UniqueID_P4000(WORD device_num, WORD *pwUniqueID);
int __declspec(dllexport) Select_Time_Tag_Source_4000 (WORD device_num, WORD source);

int __declspec(dllexport) Init_Timers_4000 (WORD device_num, int *handle);
int __declspec(dllexport) Release_Timers_4000 (int handle);

int __declspec(dllexport) SetIrig_4000(int handle, WORD flag);
int __declspec(dllexport) IsIrigTimeavail_4000(int handle, int *availFlag);
int __declspec(dllexport) GetIrigSeconds_4000(int handle, unsigned long *seconds);
int __declspec(dllexport) GetIrigTime_4000(int handle, t_IrigTime *IrigTime);
int __declspec(dllexport) GetIrigControl_4000(int handle, unsigned long *control);

int __declspec(dllexport) StartTimer_4000(int handle, unsigned long microsecsToCount, int reload_flag, int interrupt_flag, int globalreset_flag, int *timeoffset);
int __declspec(dllexport) StopTimer_4000(int handle, unsigned long *timervalue);
int __declspec(dllexport) ReadTimerValue_4000(int handle, DWORD *timervalue);
BOOL __declspec(dllexport) IsTimerRunning_4000(int handle, int *errorcondition);
int __declspec(dllexport) ResetWatchdogTimer_4000(int handle);

int __declspec(dllexport) InitializeInterrupt_P4000(int handle);
int __declspec(dllexport) Wait_for_Interrupt_P4000(int handle, unsigned int timeout);
int __declspec(dllexport) Get_Interrupt_Count_P4000(int handle, unsigned long *Sys_Interrupts_Ptr);

#ifdef __cplusplus
}
#endif

#endif
