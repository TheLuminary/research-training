#include <stdio.h>
#include <stdlib.h>     /* Interrupt processing test program */
#include <conio.h>
#include "proto_mch.h"
char errstr[255];

#define Print_n_Exit(title,error )        \
{                                         \
	if (error < 0)                         \
	{  \
		Get_Error_String_MCH(error, 255, errstr);   	                     \
		printf("%s: %s\n",title,errstr);         \
		getch();                            \
		Release_Module_MCH(handle1);                     \
		Release_Module_MCH(handle2);                     \
		exit(0);                            \
	}                                   \
}

DWORD MuxIsr(int param);
unsigned int numints = 0, oldnumints = -1;
int handle1, handle2;
BYTE shadowByte;
ULONG uInterruptsTotal, uInterrupts[8];

int main(void)
{
	int error, devnum, modnum;
	usint device, module;

	HANDLE hThreadMuxIsr;	       // Thread commIsr handle
	DWORD threadIdMuxIsr;	       // ID of thread commIsr

	/*
	 * This sample program shows how to set up two EXC-4000PCI/MCH modules as two
	 * Remote Terminals and move data from one to the other in real time
	 */

	printf("EXCALIBUR Interrupt Demo (Remote Terminal)\n\n");
	printf("(Default device number for single board when ExcConfig not used:\n");
	printf("  M4k-1553MCH -> 25, PCI/MCH -> 29)\n");
	printf("Enter the device number of the board you wish to use for the first RT.\n");
	scanf("%d", &devnum);
	printf("Enter the module number.\n");
	scanf("%d", &modnum);
	module = (usint) modnum;
	device = (usint) devnum;

	printf("Calling Init_Module_Mch with device %d module %d\n\n", device, module);
	error = Init_Module_MCH(device, module);
	if (error < 0)
	{
		Get_Error_String_MCH(error, 255, errstr);
		printf("Init_Module_MCH Failure. %s\n", errstr);
		getch();
		exit(0);
	}
	handle1 = error;

	printf("Enter the device number of the board you wish to use for the second RT.\n");
	printf("This may be the same board as the first RT.\n");

	scanf("%d", &devnum);
	printf("Enter the module number.\n");
	scanf("%d", &modnum);
	if (((usint) devnum == device) && ((usint) modnum == module))
	{
		Release_Module_MCH(handle1);
		printf("\nError: Illegal use of same device AND module number for two different RTs.\nExiting program.\n");
		getch();
		exit(0);
	}
	module = (usint) modnum;
	device = (usint) devnum;

	printf("Calling Init_Card_Mch with device %d module %d\n\n", device, module);
	error = Init_Module_MCH(device, module);
	if (error < 0)
	{
		Release_Module_MCH(handle1);
		Get_Error_String_MCH(error, 255, errstr);
		printf("Init_Module_MCH Failure. %s\n", errstr);
		getch();
		exit(0);
	}
	handle2 = error;

	/* set up card as Remote Terminal 1 */
	error = Set_Mode_MCH(handle1, RT_MODE_MCH);
	Print_n_Exit("RT_MODE_MCH FAILURE", error);


	//这下面之后是特有的部分
	///////////////////////////////////////////////////////////////////////////

	/* set RT address to 1 */
	error = Set_RT_Num_MCH(handle1, 1);
	Print_n_Exit("Setup_RT FAILURE", error);

	/* Request interrupt on subaddress 1 receive message. This routine
	 *  may be called multiple times for different subaddress T/R
	 *  combinations.
	 */


	error = Set_Subaddr_Int_MCH(handle1, 1, RECEIVE, STANDARD_CMD, ACCESS);
	Print_n_Exit("Set_Subaddr_Int", error);

	/* Allow interrupts on subaddress access. This is called once. */
	error = Set_Interrupt_Mask_MCH(handle1, SUBAD);
	Print_n_Exit("Set_Interrupt Mask", error);

	/* Start the RT running */
	error = Run_RT_MCH(handle1);
	Print_n_Exit("Run_RT FAILURE", error);

	// set up card as  Remote Terminal 2
	error = Set_Mode_MCH(handle2, RT_MODE_MCH);
	Print_n_Exit("RT_MODE_MCH FAILURE", error);

	// set RT address to 2
	//nomi change 2 to 1 ?
	error = Set_RT_Num_MCH(handle2, 2);
	Print_n_Exit("Setup_RT FAILURE", error);

	error = Set_Subaddr_Int_MCH(handle2, 1, TRANSMIT, STANDARD_CMD, ACCESS);
	Print_n_Exit("Set_Subaddr_Int", error);

	/* Allow interrupts on subaddress access. This is called once. */
	error = Set_Interrupt_Mask_MCH(handle2, SUBAD);
	Print_n_Exit("Set_Interrupt Mask", error);

	// Start the RT running //
	error = Run_RT_MCH(handle2);
	Print_n_Exit("Run_RT FAILURE", error);

	// Set up our thread
	hThreadMuxIsr = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) MuxIsr, (LPVOID) 1,
	CREATE_SUSPENDED, &threadIdMuxIsr);
	// check creations
	if ((hThreadMuxIsr == NULL))
	{
		// failed to create some thread
		printf("Error on thread creation: %s", GetLastError());
		Release_Module_MCH(handle1);
		Release_Module_MCH(handle2);
		getch();
		exit(-1);
	}

	// Set the priority of each new thread.
	// This thread is at normal priority.
	//	SetThreadPriority(hThreadMuxIsr,      THREAD_PRIORITY_TIME_CRITICAL);

	// Start up the threads
	ResumeThread(hThreadMuxIsr);
	printf("Setup complete -- begin transmitting now as follows:\n\n");
	printf("Set up a BC to send a RECEIVE command to RT 1 SA 1, with 5 words of data\n");
	printf("   and a TRANSMIT command to RT 2 SA 1, also with wordcount 5.\n");
	printf("The demo is successful if interrupts are received by both modules,\n");
	printf("   and the BC receives the same 5 data words from RT 2 that it sent to RT 1.\n");

	printf("\nPress any key to complete program.\n");

	while (1)
	{
		/* update the screen with the number of interrupts that have been
		 * received.  This should correspond to the number of receive
		 * RT 1 subaddress 1 messages encountered.
		 */

		if (numints != oldnumints)
		{
			oldnumints = numints;
			Get_Interrupt_Count_MCH(handle1, &uInterrupts[0]);
			Get_Interrupt_Count_MCH(handle2, &uInterrupts[1]);

			printf("\rINTERRUPTS: First Module:    %u   Second Module: %u ", uInterrupts[0], uInterrupts[1]);
		}
		if (kbhit())
		{
			TerminateThread(hThreadMuxIsr, 0);
			getch();
			break;
		}
	}
	Stop_Card_MCH(handle2);    // stop channel 1
	Stop_Card_MCH(handle1);    // stop channel 0
	Release_Module_MCH(handle1);
	Release_Module_MCH(handle2);
	printf("\n\nTest complete.");
	getch();

	return (0);
} /* end main */

/***************************************************************************/

/* Interrupt routine for IRQ interrupt */

DWORD MuxIsr(int param)
{
	WORD rcvbuf[34];

	while (param) /* use param for TRUE */
	{
		Wait_For_Interrupt_MCH(handle1, INFINITE);
		numints++;

		/* get the data from subaddress 2 of the receiver */
		Read_Datablk_MCH(handle1, 1, 0, rcvbuf, STANDARD_CMD, RECEIVE);
		/* send it to subaddress 1 of the transmitter */
		Load_Datablk_MCH(handle2, 1, 0, rcvbuf + 2);

	}

	return (0);
}

