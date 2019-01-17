#include <stdio.h>
#include "exc4000.h"
#include "deviceio.h"
#include "error_devio.h"

#define GETBIT(x,y) (((x) & (1<<(y))) != 0)
#define MAX_MODULES	4
#define GLOBALREG_BANK	4

#define EXC4000PCI_BOARDIDMASK		0x000F
#define EXC4000PCI_BOARDSIGMASK		0xFF00
#define EXC4000PCI_BOARDSIGVALUE	0x4000

#define IRIG_TIME_AVAIL			0x400

#define TM_STARTBITVAL	1

#define TM_STARTBIT	0
#define TM_RELOADBIT	1
#define TM_INTERRUPTBIT 2
#define TM_GLOBRESETBIT 3
WORD g_preloadval;

typedef struct
{
	WORD boardSigId;
	WORD softwareReset;
	WORD interruptStatus;
	WORD interruptReset;
	WORD moduleInfo[4];
	WORD clocksourceSelect;
	WORD reserved;
	WORD IRcommand;
	WORD IRsecondsOfDay;
	WORD IRdaysHours;
	WORD IRminsSecs;
	WORD IRcontrol1;
	WORD IRcontrol2;
  	WORD fpgaRevision;
	WORD TMprescale;
	WORD TMpreload;
	WORD TMcontrol;
	WORD TMcounter;
} t_globalRegs4000;
typedef struct
{
	int device_num; 
	int allocCounter;
	t_globalRegs4000 *globreg;

} INSTANCE_EXC4000;

INSTANCE_EXC4000 exc4000card[NUMBOARDS];
static int g_firsttimeTimers = 1;

int __declspec(dllexport) Get_4000Module_Type(WORD device_num, WORD module_num, WORD *modtype)
{
	int iError;
	WORD wBoardType;
	t_globalRegs4000 *globreg;

	if (module_num >= MAX_MODULES)
		return emodnum;

	iError = OpenKernelDevice(device_num);
	if (iError)
		return iError;

	iError = MapMemory(device_num, (void **)&(globreg), GLOBALREG_BANK);
	if (iError)
	{
		CloseKernelDevice(device_num);
		return iError; /* error from mapmemory */
	}
	/* verify that this is indeed a 4000 card */
	wBoardType = (WORD)(globreg->boardSigId & EXC4000PCI_BOARDSIGMASK);
	if (wBoardType != EXC4000PCI_BOARDSIGVALUE) /* not a 4000 card! */
	{
		CloseKernelDevice(device_num);
		return ekernelnot4000card;
	}

	*modtype = (WORD)(globreg->moduleInfo[module_num] & 0x1F);
	CloseKernelDevice(device_num);
	return 0;
}

int __declspec(dllexport) Get_UniqueID_P4000(WORD device_num, WORD *pwUniqueID)
{
	int iError;
	WORD wBoardType;
	t_globalRegs4000 *globreg;

	iError = OpenKernelDevice(device_num);
	if (iError)
		return iError;

	iError = MapMemory(device_num, (void **)&(globreg), GLOBALREG_BANK);
	if (iError) 
	{
		CloseKernelDevice(device_num);
		return iError; 
	}

	/* verify that this is indeed a 4000 card */
	wBoardType = (WORD)(globreg->boardSigId & EXC4000PCI_BOARDSIGMASK);
	if (wBoardType != EXC4000PCI_BOARDSIGVALUE) /*not a 4000 card */
	{
		CloseKernelDevice(device_num);
		return ekernelnot4000card; 
	}

	*pwUniqueID = (WORD)(globreg->boardSigId & EXC4000PCI_BOARDIDMASK);

	CloseKernelDevice(device_num);
	return 0;
}
/*
Select_Time_Tag_Source	

Description: Selects the source of the time tag used on all modules
INTERNAL CLOCK uses the on board 4 usec. timer. See EXTTCLKx pin in
connectors in the  User�s Manual.

Input parameters:  source  INTERNAL_CLOCK or EXTERNAL_CLOCK

Output parameters: none

Return values:	   einval  If parameter is out of range
		   0	   If successful

*/
#define EXC4000_INTERNAL_CLOCK	0
#define EXC4000_EXTERNAL_CLOCK	1
int __declspec(dllexport) Select_Time_Tag_Source_4000 (WORD device_num, WORD source)
{
	int iError;
	WORD wBoardType;
	t_globalRegs4000 *globreg;

	if ((source != EXC4000_INTERNAL_CLOCK) && (source != EXC4000_EXTERNAL_CLOCK))
		return(eclocksource);

	iError = OpenKernelDevice(device_num);
	if (iError)
		return iError;

	iError = MapMemory(device_num, (void **)&(globreg), GLOBALREG_BANK);
	if (iError)
	{
		CloseKernelDevice(device_num);
		return iError;
	}
	/* verify that this is indeed a 4000 card */
	wBoardType = (WORD)(globreg->boardSigId & EXC4000PCI_BOARDSIGMASK);
	if (wBoardType != EXC4000PCI_BOARDSIGVALUE) /*not a 4000 card */
	{
		CloseKernelDevice(device_num);
		return ekernelnot4000card; 
	}

	globreg->clocksourceSelect = (WORD)source;

	CloseKernelDevice(device_num);
	return(0);
}

int __declspec(dllexport) Init_Timers_4000 (WORD device_num, int *handle)
{
	int iError, i;
	WORD wBoardType;
	
	if (g_firsttimeTimers)
	{
		for (i=0; i<NUMBOARDS; i++)
			exc4000card[i].allocCounter = 0;
		g_firsttimeTimers = 0;
	}
	else
	{	
		for (i=0; i<NUMBOARDS; i++)
			if ((exc4000card[i].device_num == device_num) && (exc4000card[i].allocCounter > 0))
			{
				*handle = i;
				exc4000card[i].allocCounter++;
				return 0;
			}	
	}
	for (i=0; i<NUMBOARDS; i++)
		if (exc4000card[i].allocCounter == 0)
			break;

	if (i == NUMBOARDS)
		return edevtoomany; 

	iError = OpenKernelDevice(device_num);
	if (iError)
		return iError;

	iError = MapMemory(device_num, (void **)&(exc4000card[i].globreg), GLOBALREG_BANK);
	if (iError) 
	{
		CloseKernelDevice(device_num);
		return iError; 
	}
	/* verify that this is indeed a 4000 card -- why not */
	wBoardType = (WORD)(exc4000card[i].globreg->boardSigId & EXC4000PCI_BOARDSIGMASK);
	if (wBoardType != EXC4000PCI_BOARDSIGVALUE) /* not a 4000 card! */
	{
		CloseKernelDevice(device_num);
		return ekernelnot4000card;
	}

	exc4000card[i].device_num = device_num;
	exc4000card[i].allocCounter++;
	*handle = i;	
	return 0;
}

int __declspec(dllexport) Release_Timers_4000 (int handle)
{
	if ((handle <0) || (handle >= NUMBOARDS))
		return ebaddevhandle;
	if (!exc4000card[handle].allocCounter)
		return ebaddevhandle;
	
	exc4000card[handle].allocCounter--;
	if (exc4000card[handle].allocCounter == 0)
		CloseKernelDevice(exc4000card[handle].device_num);

	return 0;
}


/*
Setting up IRIG B Mode
To set up IRIG B mode:
1.	Call SetIrig_4000, to set up the IRIG B feature.
2.	Wait, in a loop, until availFlag is TRUE � see  IsIrigTimeavail_4000 on page 2-7.
3.	IRIG B time is then available either in seconds (see  GetIrigSeconds_4000 on page 2-6) or in a string (see  GetIrigTime_4000 on page 2-5).
*/

/*
SetIrig_4000

	SetIrig_4000 sets the board to receive IRIG B time.
	SetIrig_4000 (WORD device_num, WORD flag)	
	device_num	
The define value EXC_4000PCI can be used instead of a device number. If more than one board is used, run ExcConfig.exe to set the device number
	flag	IRIG_TIME_AND_RESET
Sets the carrier board to receive IRIG B time. In addition for synchronization purposes, Time Tags on all modules on the carrier board will be reset to 0 when the IRIG B time comes in.
IRIG_TIME
Sets the carrier board to get IRIG B time but does not do a Time Tag reset.
	none	
	eopenkernel	Cannot open kernel device; check Excalibur Configuration Utility settings 
	ekernelcantmap	Kernel driver cannot map memory 
	0	If successful
*/
int __declspec(dllexport) SetIrig_4000(int handle, WORD flag)
{
	if ((handle <0) || (handle >= NUMBOARDS))
		return ebaddevhandle;
	if (!exc4000card[handle].allocCounter)
		return ebaddevhandle;

	flag &= (IRIG_TIME_AND_RESET | IRIG_TIME); /* only allow these two settings */
	exc4000card[handle].globreg->IRcommand = flag;

	return 0;
}
/*
IsIrigTimeavail_4000

	IsIrigTimeavail_4000 indicates if IRIG B time is available. Must be called after SetIrig_4000.	
	GetIrigControl_Word (WORD device_num, int *availFlag)	
	device_num	
The define value EXC_4000PCI can be used instead of a device number. If more than one board is used, run ExcConfig.exe to set the device number
	availFlag	1	IRIG B Time came in
			0	IRIG B Time not yet available
	eopenkernel	Cannot open kernel device; check Excalibur Configuration Utility settings 
	ekernelcantmap	Kernel driver cannot map memory 
	0	If successful
*/
int __declspec(dllexport) IsIrigTimeavail_4000(int handle, int *availFlag)
{
	if ((handle <0) || (handle >= NUMBOARDS))
		return ebaddevhandle;
	if (!exc4000card[handle].allocCounter)
		return ebaddevhandle;

	*availFlag = ((exc4000card[handle].globreg->IRcommand & IRIG_TIME_AVAIL) == IRIG_TIME_AVAIL);

	return 0;
}
/*
GetIrigSeconds_4000

Once IRIG B time is available, GetIrigSeconds_4000 returns the IRIG B time in seconds.
	GetIrigSeconds_4000 (WORD device_num, unsigned long *seconds)
	device_num
The define value EXC_4000PCI can be used instead of a device number. If more than one board is used, run ExcConfig.exe to set the device number
	seconds	IRIG B time in seconds
	eopenkernel	Cannot open kernel device; check Excalibur Configuration Utility settings
	ekernelcantmap	Kernel driver cannot map memory
	0	If successful
*/
int __declspec(dllexport) GetIrigSeconds_4000(int handle, unsigned long *seconds)
{
	if ((handle <0) || (handle >= NUMBOARDS))
		return ebaddevhandle;
	if (!exc4000card[handle].allocCounter)
		return ebaddevhandle;

	*seconds = ((long) (exc4000card[handle].globreg->IRcommand & 1) << 16) + exc4000card[handle].globreg->IRsecondsOfDay;

	return 0;
}
/*
GetIrigTime_4000

	GetIrigTime_4000 returns the IRIG B time as a string in the form of days, hours, minutes and seconds	
	GetIrigTime_4000 (WORD device_num, t_IrigTime *IrigTime)	
	device_num	
The define value EXC_4000PCI can be used instead of a device number. If more than one board is used, run ExcConfig.exe to set the device number

IrigTime	Days [1-366, number of days since January 1]: hours: minutes: seconds
return values:
eopenkernel	Cannot open kernel device; check Excalibur Configuration Utility settings
ekernelcantmap	Kernel driver cannot map memory 
	0	If successful
*/
int __declspec(dllexport) GetIrigTime_4000(int handle, t_IrigTime *IrigTime)
{
	if ((handle <0) || (handle >= NUMBOARDS))
		return ebaddevhandle;
	if (!exc4000card[handle].allocCounter)
		return ebaddevhandle;

	IrigTime->days = (WORD)(((exc4000card[handle].globreg->IRdaysHours & 0xc000) >> 14) * 100);	//first digit
	IrigTime->days += (WORD)(((exc4000card[handle].globreg->IRdaysHours & 0x3c00) >> 10) * 10);	//second digit
	IrigTime->days += (WORD)((exc4000card[handle].globreg->IRdaysHours & 0x03c0) >> 6);				//lowest digit
	IrigTime->hours = (WORD)((((exc4000card[handle].globreg->IRdaysHours & 0x0030) >> 4) * 10) +
		(exc4000card[handle].globreg->IRdaysHours & 0x000f));
	IrigTime->minutes = (WORD)((((exc4000card[handle].globreg->IRminsSecs & 0x7000) >> 12) * 10) +
		((exc4000card[handle].globreg->IRminsSecs & 0x0f00) >> 8));
	IrigTime->seconds = (WORD)((((exc4000card[handle].globreg->IRminsSecs & 0x0070) >> 4) * 10) +
   	(exc4000card[handle].globreg->IRminsSecs & 0x000f));

	return 0;
}

int __declspec(dllexport) GetIrigControl_4000(int handle, unsigned long *control)
{
	if ((handle <0) || (handle >= NUMBOARDS))
		return ebaddevhandle;
	if (!exc4000card[handle].allocCounter)
		return ebaddevhandle;

	*control = ((long)(exc4000card[handle].globreg->IRcontrol1 & 0x3ff) << 16) | exc4000card[handle].globreg->IRcontrol2;

	return 0;
}
/*
Section 1: Software Function Descriptions

StartTimer
� Description: Start the Timer on the EXC-4000 board.
� Input: device_num (value in ExcConfig, or default for one board);  time to count (in usecs); auto-restart flag (whether to automatically restart the Timer when it completes);  interrupt flag (whether to cause an interrupt when/each time Timer completes); global reset flag (whether to cause a global reset when/each time Timer completes).
� Ouput: (signed value) offset: difference between actual time being counted and time to count  requested (+ means the actual is greater, - means the actual is less than what was requested; 0 if same).
� Return: 0 if OK; illegal parameter value error codes; kernel errors if problem accessing board; error if Timer is currently on.
*/
int __declspec(dllexport) StartTimer_4000(int handle, unsigned long microsecsToCount, int reload_flag, int interrupt_flag, int globreset_flag, int *timeoffset)
{
	WORD microsecPerTick, numberTicks, controlval;

	*timeoffset = 0;
	if ((handle <0) || (handle >= NUMBOARDS))
		return ebaddevhandle;
	if (!exc4000card[handle].allocCounter)
		return ebaddevhandle;
	if ((reload_flag != TIMER_RELOAD) && (reload_flag != TIMER_NO_RELOAD))
		return eparmreload;
	if ((interrupt_flag != TIMER_INTERRUPT) && (interrupt_flag != TIMER_NO_INTERRUPT))
		return eparminterrupt;
	if ((globreset_flag != TIMER_GLOBRESET) && (globreset_flag != TIMER_NO_GLOBRESET))
		return eparmglobalreset;

   if (GETBIT(exc4000card[handle].globreg->TMcontrol, TM_STARTBIT))
   {
		return etimerrunning;
   }
	//exc4000card[handle].globreg->TMcontrol &= ~TM_STARTBITVAL;  /* Stop the Timer */

	/* The resolution, in microsecPerTick, goes in the prescale register;
	 numberTicks is loaded into the preload register*/

	microsecPerTick = (WORD)(((microsecsToCount-1) / 0xFFFF) + 1);
	numberTicks = (WORD)(microsecsToCount/microsecPerTick);
	*timeoffset = microsecsToCount - (microsecPerTick * numberTicks);

	exc4000card[handle].globreg->TMprescale = microsecPerTick;
	exc4000card[handle].globreg->TMpreload = numberTicks;

	controlval = (WORD)(TM_STARTBITVAL | (reload_flag << TM_RELOADBIT) | (interrupt_flag << TM_INTERRUPTBIT) | (globreset_flag << TM_GLOBRESETBIT));
	exc4000card[handle].globreg->TMcontrol = controlval;   /* Start the Timer with requested functionality*/

	return 0;
}

/*
StopTimer
� Description: Stop the Timer immediately
� Input: device_num (value in ExcConfig, or default for one board)
� Output: timer_value (last value of Timer, in usecs)
�  Return: 0 if OK; kernel errors if problem accessing board.
*/
int __declspec(dllexport) StopTimer_4000(int handle, unsigned long *timervalue)
{
	*timervalue = 0;
	
	if ((handle <0) || (handle >= NUMBOARDS))
		return ebaddevhandle;
	if (!exc4000card[handle].allocCounter)
		return ebaddevhandle;

	exc4000card[handle].globreg->TMcontrol &= ~TM_STARTBITVAL;  /* Stop the Timer */
	*timervalue = (long)(exc4000card[handle].globreg->TMcounter * exc4000card[handle].globreg->TMprescale);

	return 0;
}
/*
ReadTimerValue
� Description: Returns how many usecs are left until Timer completes.
� Input: device_num (value in ExcConfig, or default for one board)
� Output: timer_value (current value of Timer, in usecs)
� Return: 0 if OK; kernel errors if problem accessing board
*/
int __declspec(dllexport) ReadTimerValue_4000(int handle, DWORD *timervalue)
{
	*timervalue = 0;

	if ((handle <0) || (handle >= NUMBOARDS))
		return ebaddevhandle;
	if (!exc4000card[handle].allocCounter)
		return ebaddevhandle;

	*timervalue = (long)(exc4000card[handle].globreg->TMcounter * exc4000card[handle].globreg->TMprescale);

	return 0;
}
/*
IsTimerRunning
� Description: Boolean. Returns 1 if Timer is running, 0 otherwise. This can be easily used in a loop, to wait for Timer to end. (Recommended for use in non- automatic-restart mode.) This is also useful for multiple modules on the board, to check the availability of the Timer.
� Input: device_num (value in ExcConfig, or default for one board)
� Output: error value, if there was one (kernel error if problem accessing board). 0 otherwise.
� Return: 1 if Timer is running (Timer value is >0); 0 if not (Timer value = 0) or error condition. (If value is 0, programmer can check the output value to see  if there is an error condition) 
*/
BOOL __declspec(dllexport) IsTimerRunning_4000(int handle, int *errorcondition)
{
	BOOL retval;

	*errorcondition = 0;
	if ((handle <0) || (handle >= NUMBOARDS))
		*errorcondition = ebaddevhandle;
	if (!exc4000card[handle].allocCounter)
		*errorcondition =  ebaddevhandle;

	if (GETBIT(exc4000card[handle].globreg->TMcontrol, TM_STARTBIT))
		retval = TRUE;
	else
		retval = FALSE;

	return retval;
}
/*
ResetWatchdog
� Description: Stops and re-starts Timer with the same value as the last time StartTimer was called.  (read and write back preload value)
� Input: device_num (value in ExcConfig, or default for one board)
� Return: 0 if OK; kernel error if problem accessing board; error if Timer not running.
*/
int __declspec(dllexport) ResetWatchdogTimer_4000(int handle)
{
	if ((handle <0) || (handle >= NUMBOARDS))
		return ebaddevhandle;
	if (!exc4000card[handle].allocCounter)
		return ebaddevhandle;
	if (!GETBIT(exc4000card[handle].globreg->TMcontrol, TM_STARTBIT))
	{
		return etimernotrunning;
	}

	exc4000card[handle].globreg->TMcontrol &= ~TM_STARTBITVAL;  /* Stop the Timer */
	g_preloadval = exc4000card[handle].globreg->TMpreload; /*make it global variable to avoid optimization*/
	exc4000card[handle].globreg->TMpreload = g_preloadval;
	exc4000card[handle].globreg->TMcontrol |= TM_STARTBITVAL;   /* Restart the Timer */

	return 0;
}

int __declspec(dllexport) InitializeInterrupt_P4000(int handle)
{
	int retval, device_num;

	if ((handle <0) || (handle >= NUMBOARDS))
		return ebaddevhandle;
	if (!exc4000card[handle].allocCounter)
		return ebaddevhandle;

	device_num = exc4000card[handle].device_num;
	retval = Exc_Initialize_Interrupt_For_Module(device_num, EXC4000_MODULE_TIMER);

	return retval;
}

int __declspec(dllexport) Wait_for_Interrupt_P4000(int handle, unsigned int timeout)
{
	int retval, device_num;

	if ((handle <0) || (handle >= NUMBOARDS))
		return ebaddevhandle;
	if (!exc4000card[handle].allocCounter)
		return ebaddevhandle;

	device_num = exc4000card[handle].device_num;

	retval = Exc_Wait_For_Module_Interrupt(device_num, EXC4000_MODULE_TIMER, timeout);

	return retval;
}

int __declspec(dllexport) Get_Interrupt_Count_P4000(int handle, unsigned long *Sys_Interrupts_Ptr)
{
	int retval, device_num;

	if ((handle <0) || (handle >= NUMBOARDS))
		return ebaddevhandle;
	if (!exc4000card[handle].allocCounter)
		return ebaddevhandle;

	device_num = exc4000card[handle].device_num;
	retval = Exc_Get_Channel_Interrupt_Count(device_num, EXC4000_MODULE_TIMER, Sys_Interrupts_Ptr);

	return retval;
}
