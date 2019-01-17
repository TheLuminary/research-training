#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "proto_mch.h"
char errstr[255];

#define Print_n_Exit( title, error )      \
{                                         \
	if (error < 0)                         \
	{                                       \
   	Get_Error_String_MCH(error, 255, errstr);                         \
		printf("%s: %s\n", title, errstr);         \
		Release_Module_MCH(handle);                     \
		getch();                            \
		exit(0);                            \
	}                                   \
}

#define ID_TIMER 1

struct BM_MSG msgbuf;
struct BM_MSG *msgptr = &msgbuf;
int i = 0;
int handle;

void main(void)
{
	int error, devnum, modnum;
	usint device, module,num;

	printf("Excalibur Bus Monitor Demo Program\n");
   	printf("(Default device number for single board when ExcConfig not used:\n");
	printf("  M4k-1553MCH -> 25, PCI/MCH -> 29)\n");
	printf("Enter the device number of the card you wish to use.\n");
	scanf("%d",&devnum);
	printf("Enter the module number.\n");
	scanf("%d",&modnum);
	device = (usint)devnum;
	module = (usint)modnum;

	printf("Calling Init_Module_MCH with device %x, module %x\n\n", device, module);
	error = Init_Module_MCH(device, module);
	if (error < 0)
	{
   	Get_Error_String_MCH(error, 255, errstr);
		printf("Init_Module_MCH Failure: %s\n", errstr);
		getch();
		exit(0);
	}
	handle = error;

	/* set up as Bus Monitor */
	error = Set_Mode_MCH(handle, BM_MODE_MCH);
	Print_n_Exit( "Set_Mode FAILURE:  ", error );

	error = Clear_Monitor_RT_MCH(handle, 2);
	Print_n_Exit( "Clear_Monitor_RT FAILURE:  ", error );
	error = Monitor_All_RT_MCH(handle );
	Print_n_Exit( "Monitor_All_RT FAILURE:  ", error );
	error = Get_BIT_Value_MCH(handle, &num );
	Print_n_Exit( "Get_BIT FAILURE:  ", error );
   error = Get_Pending_Value_MCH(handle, &num);
	Print_n_Exit( "Get_Pending FAILURE:  ", error );
	/* start the board running */
	error = Run_BM_MCH(handle );
	Print_n_Exit( "Run_BM FAILURE:  ", error );

	printf("The board is set up to begin monitoring messages.\n");
	printf("All messages received will be displayed.\n");
	printf("Type any character to exit the program\n");
	/* loop until someone hits a key */
	while (1)
	{
		if (kbhit())
		{
			Stop_Card_MCH(handle );
			Release_Module_MCH(handle);                     \
			exit(0);
		}
		error = Read_Next_Message_MCH(handle, msgptr);
		if (error < 0)
		{
			if (error != enomsg)
         {
         	Get_Error_String_MCH(error, 255, errstr);
				printf("Read_Next_Message FAILURE: %s\n", errstr);
         }
		}
		else
		{
			printf("%5u: cmd1 = %4x,  stat1 = %x,  ttag = %4x,  msginfo = %4x\n",
			i++, msgptr->command1, msgptr->status1, msgptr->timetag, msgptr->msginfo);
		}
	}
}


