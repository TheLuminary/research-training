/* DeviceIO_MCH.C for MCH Module windows95/nt
Wrapper functions for excalibur kernel driver routines in deviceio.c for MCH Module.
This way, the kernel routines can be used in an application that uses various types of cards; handle is used here instead of device/modele number used in deviceio.c 
*/


#include "mchIncl.h"
extern INSTANCE_MCH cardmch[];
#include "deviceio.h"

int borland_dll Wait_For_Interrupt_MCH(int handlemch, unsigned int timeout)
{
	if ((handlemch) <0 || (handlemch >= NUMCARDS)) return ebadhandle;
	if (!cardmch[handlemch].allocated) return ebadhandle;

	if (cardmch[handlemch].card_addr == SIMULATE)
		return(0);
	else
		return Exc_Wait_For_Module_Interrupt(cardmch[handlemch].card_addr, cardmch[handlemch].module, timeout);
}

int borland_dll Get_Interrupt_Count_MCH(int handlemch, unsigned long *Sys_Interrupts_Ptr)
{
	if ((handlemch) <0 || (handlemch >= NUMCARDS)) return ebadhandle;
	if (!cardmch[handlemch].allocated) return ebadhandle;

	if (cardmch[handlemch].card_addr == SIMULATE)
	{
		*Sys_Interrupts_Ptr = 0;
		return(0);
	}
	else
		return Exc_Get_Channel_Interrupt_Count(cardmch[handlemch].card_addr, cardmch[handlemch].module, Sys_Interrupts_Ptr);
}

int borland_dll Wait_For_Multiple_Interrupts_MCH(int numints, int *handle_array, unsigned int timeout, unsigned long *Interrupt_Bitfield)
{
	int i, handle;
	t_ExcDevModPair devmodpair[NUMCARDS];

	for (i=0; i<numints; i++)
	{
		handle = handle_array[i];
		if ((handle) <0 || (handle >= NUMCARDS)) return ebadhandle;
		if (!cardmch[handle].allocated) return ebadhandle;
		if (cardmch[handle].card_addr == SIMULATE)
		{
			*Interrupt_Bitfield = 0;
			return 0;
		}
		devmodpair[i].nDevice = cardmch[handle].card_addr;
		devmodpair[i].nModule = cardmch[handle].module;
	}
	return Exc_Wait_For_Multiple_Interrupts(numints, devmodpair, timeout, Interrupt_Bitfield);

}
/* 17 is EXC-1553PCI/MCH, 18 is EXC-4000PCI */

int borland_dll GetCardType_MCH(int handlemch, unsigned long *cardtype)
{
	
	if ((handlemch) <0 || (handlemch >= NUMCARDS)) return ebadhandle;
	if (!cardmch[handlemch].allocated) return ebadhandle;
	if (cardmch[handlemch].card_addr == SIMULATE)
	{
		*cardtype = 0;
		return(0);
	}
	return GetCardType(cardmch[handlemch].card_addr, cardtype);
}

