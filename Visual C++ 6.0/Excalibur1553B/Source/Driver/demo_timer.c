#include<stdio.h>
#include <conio.h>
#include "exc4000.h"
#include "error_devio.h"

int main(void)
{

	int device_num, errorcond, offset, timeout, handle, iRet;
	BOOL resp;
	unsigned long timervalue, counter, timeoutms, intcnt;

	printf("Enter the device number of the Excalibur board (default 25): ");
	scanf("%d",&device_num);
	iRet = Init_Timers_4000((WORD)device_num, &handle);
   if (iRet < 0)
   {
	   	printf("Init_Timers returned %d\n", iRet);
		printf("Press any key to exit program\n");
		getch();
		exit(0);
   }

	printf("Enter the number of SECONDS you want timer to time:\n");
	scanf("%d", &timeout);
   timeoutms = timeout * 1000000;
	printf("\nSetting timer to %d seconds. Look at clock for start and end time.\n", timeout);
	printf("Press any key to begin. ");
	getch();
	printf("Timer starting NOW ... ");

	resp = StartTimer_4000(handle, timeoutms, TIMER_NO_RELOAD, TIMER_NO_INTERRUPT, TIMER_NO_GLOBRESET, &offset);
   if (resp < 0)
   {
   	printf("StartTimer returned %d\n", resp);
			Release_Timers_4000(handle);
			printf("Press any key to exit program\n");
			getch();
			exit(0);
   }

	while(IsTimerRunning_4000(handle, &errorcond) != 0);
	printf("Timer completed NOW.\n", offset);


	printf("\nNow timer will be set for 20 seconds, so we can do some other things\n");

	StartTimer_4000(handle, 20000000, TIMER_NO_RELOAD, TIMER_NO_INTERRUPT, TIMER_NO_GLOBRESET, &offset);
	resp = IsTimerRunning_4000(handle, &errorcond);
	if (resp == 0)
	{
		{
			printf("Timer should be running -- There's a problem here\n");
			printf("Press any key to exit program\n");
			getch();
			exit(0);
		}
	}

  	printf("\nIsTimerRunning should now return TRUE:\n");
	resp = IsTimerRunning_4000(handle, &errorcond);
	if (resp == 0)
	{
		if (errorcond != 0)
		{
			Release_Timers_4000(handle);
			printf("IsTimerRunning incorrectly returned %d -- There's a problem here\n", errorcond);
			printf("Press any key to exit program\n");
			getch();
			exit(0);
		}
		else
		{
			Release_Timers_4000(handle);
			printf("Timer is not running -- There's a problem here\n");
			printf("Press any key to exit program\n");
			getch();
			exit(0);
		}
	}
	printf("IsTimerRunning correctly returned TRUE.\n");
	ReadTimerValue_4000(handle, &timervalue);
	printf("Timer value when read is: %d\n", timervalue);

	// wait for timer to finish
	printf("\nWaiting for timer to finish...");
	while(IsTimerRunning_4000(handle, &errorcond) != 0);
	printf("\nTimer completed NOW.\n", offset);

	// Interrupt test
	printf("\nNow timer will be set for 20 seconds, with interrupt\n");

	resp = InitializeInterrupt_P4000(handle);
	if (resp < 0) printf("\nError in initialize interrupt: %d", resp);

	StartTimer_4000(handle, 20000000, TIMER_NO_RELOAD, TIMER_INTERRUPT, TIMER_NO_GLOBRESET, &offset);
	resp = Wait_for_Interrupt_P4000(handle, INFINITE);
	if (resp < 0) printf("\nError in WaitForInterrupt: %d", resp);

	printf("\nReceived timer interrupt\n");

	printf("\nAgain, for two seconds");
	StartTimer_4000(handle, 2000000, TIMER_NO_RELOAD, TIMER_INTERRUPT, TIMER_NO_GLOBRESET, &offset);
	resp = Wait_for_Interrupt_P4000(handle, INFINITE);
	if (resp < 0) printf("\nError in WaitForInterrupt: %d", resp);

	printf("\nReceived timer interrupt\n");

	printf("\nCheck interrupt count");
	resp = Get_Interrupt_Count_P4000(handle, &intcnt);
	if (resp < 0) printf("\nError in GetInterruptCount: %d", resp);

	printf("\nInterrupt count: %d\n\n", intcnt);

	StopTimer_4000(handle, &counter);
	Release_Timers_4000(handle);
	printf("\nTimer stopped.\n");
	printf(" Time left was %d\n", counter);
	printf("\nDemo complete. Press any key to exit program\n");
	getch();
	return 0;
}

