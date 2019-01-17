#include<stdio.h>
#include <conio.h>
#include "exc4000.h"

int main(void)
{
	unsigned int hours, minutes, secsInMinute;
	unsigned long seconds;
	t_IrigTime IrigTime;
	WORD device_num;
	int devnum, availFlag, error, handle;

	printf("Enter the device number of the Excalibur board (default 25)\n");
	scanf("%d",&devnum);
	device_num = (WORD)devnum;

	error = Init_Timers_4000(device_num, &handle);
   if (error < 0)
   {
   	printf("Init_Timers returned error %d\n", error);
      getch();
      return 0;
   }
	SetIrig_4000(handle,IRIG_TIME);
	do{
		error = IsIrigTimeavail_4000(handle, &availFlag);
		if (error < 0)
		{
			printf("IsIrigTimeavail returned error %d\n", error);
         Release_Timers_4000(handle);
			getch();
			return 0;
		}
	}while(!availFlag);

	error =	GetIrigSeconds_4000(handle, &seconds);
	if (error < 0)
	{
		printf("GetIrigSeconds returned error %d\n", error);
      Release_Timers_4000(handle);
		getch();
		return 0;
	}

	hours = seconds/3600;
	minutes = (seconds/60) % 60;
	secsInMinute = seconds % 60;
	printf("SBS time = %02u:%02u:%02u\n",hours,minutes,secsInMinute);

	error = GetIrigTime_4000(handle, &IrigTime);
	if (error < 0)
	{
		printf("GetIrigTime returned error %d\n", error);
      Release_Timers_4000(handle);
		getch();
		return 0;
	}

	printf("days.hours.minutes.seconds = %3u.%02u.%02u.%02u\n",
		IrigTime.days,IrigTime.hours,IrigTime.minutes,IrigTime.seconds);

	printf("Part II: Reset timetags on all modules when Irig Time comes in.\n");
	printf("First look at the running timetag in Showhex, on any or all modules on the 4000 card.\n");
	printf("Then press any key, and make sure timetag/s are reset.\n");

	SetIrig_4000(handle,IRIG_TIME_AND_RESET);
	do{
		error = IsIrigTimeavail_4000(handle, &availFlag);
		if (error < 0)
		{
			printf("IsIrigTimeavail returned error %d\n", error);
         Release_Timers_4000(handle);
			getch();
			return 0;
		}
	}while(!availFlag);
	error =	GetIrigSeconds_4000(handle, &seconds);
	if (error < 0)
	{
		printf("GetIrigSeconds returned error %d\n", error);
      Release_Timers_4000(handle);
		getch();
 		return 0;
	}
	printf("If timetags were reset on modules, test succeeded.\n");
   Release_Timers_4000(handle);
	getch();
	return 0;
}

