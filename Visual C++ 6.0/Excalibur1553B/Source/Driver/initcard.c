#include <stdio.h>
#include <malloc.h>
//#include <conio.h>
#include "mchIncl.h"
#include "excsysio.h"
#include "exc4000.h"

INSTANCE_MCH cardmch[NUMCARDS];
int g_handlemch=0;

int borland_dll Init_Module_MCH (usint device_num, usint module_num)
{
	static int struct_initialized = 0;
	int iError,i, handlemch;
	usint *intptr, modtype;

	if (module_num > NUMMODULES-1)
		return emodnum;

	/* If first time init_module called, initialize structure */
	if (struct_initialized == 0)
	{
		for (i=0; i<NUMCARDS; i++)
		{
			cardmch[i].allocated = FALSE;
  			cardmch[i].card_addr = INVALID_DEV;
			cardmch[i].module = INVALID_MOD;
 		        cardmch[i].intnum = 0;
			cardmch[i].mMultiBuf = 0;
		}
		struct_initialized = 1;
	}
	/* first check if this device number was already allocated a brd value */
	for (i=0; i<NUMCARDS; i++)
	{
		if ((cardmch[i].allocated == 1) &&
		    (cardmch[i].card_addr != SIMULATE) &&
		    (device_num == cardmch[i].card_addr) &&
		    (module_num == cardmch[i].module))
		{
			handlemch = i;
			g_handlemch = i;
        		return handlemch;
		}
	}

	for (i=0; i<NUMCARDS; i++)
	{
		if (cardmch[i].allocated == 0)
		{
			cardmch[i].card_addr = device_num;
			cardmch[i].module = module_num;
			handlemch = i;
			g_handlemch = i;
			break;
		}
	}
	if (i == NUMCARDS) /* no more elements in cardmch[handlemch] structure */
		return eboardtoomany;

	/* set pointers to all data structures in the given segment */
	if (device_num == SIMULATE)  /* for testing when no board present */
	{
	  //	cardmch[handlemch].exc_summit = (struct exc_bd *) malloc (sizeof (struct exc_bd ));
	     	cardmch[handlemch].exc_summit = (struct exc_bd *) malloc (BANKSIZE);
		if (cardmch[handlemch].exc_summit == NULL)
			return (sim_no_mem);

		cardmch[handlemch].exc_summit_int = (usint *) cardmch[handlemch].exc_summit;
		cardmch[handlemch].bm_command_blocks = (struct bm_command_block *) &cardmch[handlemch].exc_summit_int[INIT_MON_BLK];
		cardmch[handlemch].bc_command_blocks = (struct bc_command_block *) &cardmch[handlemch].exc_summit_int[CMDOFFSET];

		/* clear dual port ram */
		intptr = (usint *) cardmch[handlemch].exc_summit_int;
		for (i = 0; i < (BANKSIZE / 2); i++)
			*intptr++ = 0;

		/* set up some default values */
		cardmch[handlemch].milstd1553B = 1;
		cardmch[handlemch].broadcast = 0;
		cardmch[handlemch].intnum = 1;
	}
	else
	{
		iError = Get_4000Module_Type(device_num, module_num, &modtype);
		if (iError)
		{
  			cardmch[handlemch].card_addr = INVALID_DEV;
			cardmch[handlemch].module = INVALID_MOD;
			return iError; /* bad module_num, could not open kernel device, could not map memory */
		}

		if(modtype != EXC4000_MODTYPE_MCH)
		{
 			cardmch[handlemch].card_addr = INVALID_DEV;
			cardmch[handlemch].module = INVALID_MOD;

			if (modtype == EXC4000_MODTYPE_NONE)
				return enomodule;
			else
				return ewrngmodule;
		}
		iError = OpenKernelDevice(cardmch[handlemch].card_addr);
		if (iError)
		{
			cardmch[handlemch].card_addr = INVALID_DEV;
			cardmch[handlemch].module = INVALID_MOD;
			return iError; /* it returns eopenkernel */
		}

	        // Get a pointer to dual port ram
		iError = MapMemory(cardmch[handlemch].card_addr, (void **) &(cardmch[handlemch].exc_summit_int), cardmch[handlemch].module);
		if (iError)
		{
			CloseKernelDevice(cardmch[handlemch].card_addr);
  			cardmch[handlemch].card_addr = INVALID_DEV;
			cardmch[handlemch].module = INVALID_MOD;
			return iError; /* it returns ekernelcantmap */
		}

		// Assign other pointers to this block
		cardmch[handlemch].exc_summit = (struct exc_bd *)cardmch[handlemch].exc_summit_int;
		cardmch[handlemch].bm_command_blocks = (struct bm_command_block *) &cardmch[handlemch].exc_summit_int[INIT_MON_BLK];
		cardmch[handlemch].bc_command_blocks = (struct bc_command_block *) &cardmch[handlemch].exc_summit_int[CMDOFFSET];

		/* initialize memory to 0, and test memory */
  		for (i=Start_DPRAM; i<BANKSIZE; i++)
  		{
  			cardmch[handlemch].exc_summit_int[i] = Clear_Pattern;
  			if (cardmch[handlemch].exc_summit_int[i] != Clear_Pattern)
  			{
          			CloseKernelDevice(device_num);
				cardmch[handlemch].card_addr = INVALID_DEV;
				cardmch[handlemch].module = INVALID_MOD;
  				return (ememerr);
			}
		}
		cardmch[handlemch].milstd1553B = 1;
		cardmch[handlemch].broadcast = 0;
		Get_IRQ_Number(cardmch[handlemch].card_addr, &cardmch[handlemch].intnum);
	} /* end real board */

	cardmch[handlemch].allocated = TRUE;
	return (handlemch);
} /* end Init_Card */


/*
* Release_Module releases the resources allocated to the card/module, and powers
  down the module. This must be the last driver routine called for the particular    card/module combination.
*/

int borland_dll Release_Module_MCH (int handlemch)
{
	if ((handlemch) <0 || (handlemch >= NUMCARDS)) return ebadhandle;
	if (cardmch[handlemch].allocated)
	{
		cardmch[handlemch].allocated = FALSE;
	  	if (cardmch[handlemch].card_addr == SIMULATE)
	  		free (cardmch[handlemch].exc_summit_int);
		else
		{
Release_Event_Handle_For_Module(cardmch[handlemch].card_addr, cardmch[handlemch].module);
			CloseKernelDevice(cardmch[handlemch].card_addr);
		}
		cardmch[handlemch].card_addr = INVALID_DEV;
		cardmch[handlemch].module = INVALID_MOD;
		return 0;
	}
	else
		return ebadhandle;
}
/* for backward compatibility only!! Channel is actually another module on the card*/
int  borland_dll Select_Channel (unsigned short channel)
{
	int i;

	if (channel >= NUMMODULES)
		return ebadchannel;

	/* check if this device number was already allocated a handle value */
	for (i=0; i<NUMCARDS; i++)
	{
		if ((cardmch[i].allocated) && (cardmch[i].module == channel))
		{
			g_handlemch = i;
	  		return 0;
		}
	}

	return ebadchannel;
} /* end Select_Channel */
