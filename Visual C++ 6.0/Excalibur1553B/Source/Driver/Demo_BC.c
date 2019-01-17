#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "proto_mch.h"
int handle;
char errstr[255];

#define Print_n_Exit( title, error )      \
{                                         \
	if (error < 0)                         \
	{  \
		Get_Error_String_MCH(error, 255, errstr);   	                              \
		printf("%s: %s\n", title, errstr);         \
		Release_Module_MCH(handle);                     \
		getch();                            \
		exit(0);                            \
	}                                   \
}

/* global variables used by interrupt handler and main program */
int numints = 0;
unsigned long Sys_Interrupts = 0;
unsigned long Tot_Sys_Interrupts = 0;

DWORD MUX_ISR(short Param);

void main(void)
{
	HANDLE hThreadMuxIsr;              // Thread commIsr handle
	DWORD threadIdMuxIsr;             // ID of thread commIsr
	int devnum, modnum, error;
	usint device, module, num;
	WORD bcdatablk[3] =
	{ 0x1111, 0x2222, 0x3333 };
	WORD datainblk[3];
	WORD * bcdata = bcdatablk;
	WORD * datain = datainblk;
	struct CMDENTRY entry;

	printf("Excalibur Bus Controller Demo Program with Interrupts\n");
	printf(
			"(Default device number for single board when ExcConfig not used:\n");
	printf("  M4k-1553MCH -> 25, PCI/MCH -> 29)\n");
	printf("Enter the device number of the card you wish to use.\n");
	scanf("%d", &devnum);
	printf("Enter the module number.\n");
	scanf("%d", &modnum);
	module = (usint) modnum;
	device = (usint) devnum;

	printf("Calling Init_Module_Mch with device %d module %d\n\n", device,
			module);
	error = Init_Module_MCH(device, module);
	if (error < 0)
	{
		Get_Error_String_MCH(error, 255, errstr);
		printf("Init_Module_MCH Failure. %s\n", errstr);
		getch();
		exit(0);
	}
	handle = error;

	printf(
			"On the other side, set RT 1 SA 1 to transmit 3 words: aaaa bbbb and cccc.\n");
	printf(
			"The demo is successful if many (thousands) of interrupts are received\n");
	printf(
			" and the words aaaa bbbb and cccc are received from Message 1.\n\n");

	error = Set_Mode_MCH(handle, BC_MODE_MCH);
	Print_n_Exit("Set_Mode(BC_MODE_MCH) FAILURE:  ", error);

	entry.control = SENDMSG | CW_BUS_A;
	entry.command1 = 0x0823; /* RT 1 subaddr 1 receive 3 words */
	entry.command2 = 0;
	entry.status1 = 0;
	entry.status2 = 0;
	entry.datablk = 0; /* use data block 0 for transmit data */
	entry.gotocmd = 0;
	entry.timer = 0;
	error = Set_BC_Cmd_MCH(handle, 0, &entry);
	Print_n_Exit("Set_BC_Cmd 0 FAILURE:  ", error);

	entry.control = SENDMSG | CW_BUS_A;
	entry.command1 = 0x0C23;              // RT 1 subaddr 1 Transmit 3 words
	entry.command2 = 0;
	entry.status1 = 0;
	entry.status2 = 0;
	entry.datablk = 1;                   // use data block 1 for received data
	entry.gotocmd = 0;
	entry.timer = 0;
	error = Set_BC_Cmd_MCH(handle, 1, &entry);
	Print_n_Exit("Set_BC_Cmd 1 FAILURE:  ", error);

	entry.control = 0xa000;    // Interrupt and continue, Command Block accessed
	entry.command1 = 0x0C23;              // RT 1 subaddr 1 Transmit 3 words
	entry.command2 = 0;
	entry.status1 = 0;
	entry.status2 = 0;
	entry.datablk = 1;                   // use data block 1 for received data
	entry.gotocmd = 0;
	entry.timer = 0;
	error = Set_BC_Cmd_MCH(handle, 2, &entry);
	Print_n_Exit("Set_BC_Cmd 2 FAILURE:  ", error);

	entry.control = GOTO_MSG;
	entry.command1 = 0;
	entry.command2 = 0;
	entry.status1 = 0;
	entry.status2 = 0;
	entry.datablk = 0;
	entry.gotocmd = 0;
	entry.timer = 0;
	error = Set_BC_Cmd_MCH(handle, 3, &entry);
	Print_n_Exit("Set_BC_Cmd 3 FAILURE:  ", error);

	error = Load_BC_Datablk_MCH(handle, 0, 3, bcdata);
	Print_n_Exit("Load_BC_Datablk FAILURE:  ", error);

	error = Set_Interrupt_Mask_MCH(handle, 0x0002);    //CBA
	Print_n_Exit("Set_Interrupt_Mask FAILURE:  ", error);

	hThreadMuxIsr = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) MUX_ISR,
			(LPVOID) 1,
			CREATE_SUSPENDED, &threadIdMuxIsr);
	// check creations
	if ((hThreadMuxIsr == NULL))
	{       // failed to create some thread
		printf("Error on Thread creation \n");
		exit(-1);
	}

	// Set the priority of each new thread.
	// This thread is at normal priority.
	SetThreadPriority(hThreadMuxIsr, THREAD_PRIORITY_TIME_CRITICAL);

	// Start up the threads
	ResumeThread(hThreadMuxIsr);
	Sleep(4000L);    //Make sure that ISR thread is entered
	printf("Setup Complete: Press any key to continue.\n");
	getch();

	error = Run_BC_MCH(handle);
	Print_n_Exit("Run_BC FAILURE:  ", error);
	Sleep(10000L);
	error = Stop_Card_MCH(handle);
	Print_n_Exit("Stop_Card FAILURE:  ", error);
	Sleep(2000L);

	printf("\n\n");
	error = Last_BC_Cmdnum_MCH(handle, &num);
	Print_n_Exit("Last_BC_Cmd FAILURE:  ", error);
	error = Get_BC_Cmd_MCH(handle, 0, &entry);
	Print_n_Exit("Get_BC_Cmd 0 FAILURE:  ", error);
	if (entry.control & ERROR)
		printf("Message 0 RT error \n");

	printf("status from message 0 (cmd = 0x0823) = %x\n", entry.status1);
	error = Get_BC_Cmd_MCH(handle, 1, &entry);
	Print_n_Exit("Get_BC_Cmd 1 FAILURE:  ", error);
	if (entry.control & ERROR)
		printf("Message 1 RT error \n");

	error = Read_BC_Datablk_MCH(handle, 1, 3, datain);
	Print_n_Exit("Read_BC_Datablk FAILURE:  ", error);
	printf("status from message 1 (cmd = 0x0C23) = %x  data = %x %x %x\n",
			entry.status1, datainblk[0], datainblk[1], datainblk[2]);

	error = Get_Pending_Value_MCH(handle, &num);
	Print_n_Exit("Get_Pending_Value_MCH FAILURE:  ", error);
	printf("status from Get_Pending_Value_MCH (hex):  %x\n", num);

	error = Get_Broadcast_Setting_MCH(handle, &num);
	Print_n_Exit("Get_Broadcast_Setting_MCH FAILURE:  ", error);
	printf("status from Get_Broadcast(hex):  %x\n", num);

	error = Get_BIT_Value_MCH(handle, &num);
	Print_n_Exit("Get_BIT_Value_MCH FAILURE:  ", error);
	printf("status from Get_BIT_Value_MCH(hex):  %x\n", num);

	error = Release_Module_MCH(handle);
	Print_n_Exit("Release_Card FAILURE:  ", error);

	printf("Test Complete\n");
	getch();
}

DWORD MUX_ISR(short Param)
{
	int prev_total;
	while (Param)
	{

		Wait_For_Interrupt_MCH(handle, INFINITE);
		prev_total = Tot_Sys_Interrupts;
		Get_Interrupt_Count_MCH(handle, &Tot_Sys_Interrupts);
		Sys_Interrupts = Tot_Sys_Interrupts - prev_total;
		numints++;
		printf(
				"Interrupt Test: numints:   %d, Sys_Ints:   %d, Tot_Sys_Ints: %d\r",
				numints, Sys_Interrupts, Tot_Sys_Interrupts);
	}

	return (0);
}
