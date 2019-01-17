/* Set General Information

 Description - These routines are called to set various parameters or
 modes on the Excalibur card.
 */

#include <stdio.h>
#include "mchIncl.h"
extern INSTANCE_MCH cardmch[];

int temp;
void Setup_RT_Descriptors(int handlemch);
void zeroMultibufFlag_MCH(int handlemch);

/*
 * Set_PingPong allows the user to choose whether the board should
 * enable the ping pong double buffering system.
 */

int borland_dll Set_PingPong(short Pingpong_Flag)
{
	return Set_PingPong_MCH(g_handlemch, Pingpong_Flag);
}
int borland_dll Set_PingPong_MCH(int handlemch, short Pingpong_Flag)
{
	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	if (Pingpong_Flag)
	{
		cardmch[handlemch].exc_summit->control |= PPEN;
		cardmch[handlemch].pingpong = PPEN;
	}
	else
	{
		cardmch[handlemch].exc_summit->control &= ~PPEN;
		cardmch[handlemch].pingpong = 0;
	}

	return 0;
}

/*
 * Set_Broadcast allows the user to choose whether the board should regard
 * RT #31 as a regular RT or as the RT Broadcast address.
 */

int borland_dll Set_Broadcast(short Broadcast_Flag)
{
	return Set_Broadcast_MCH(g_handlemch, Broadcast_Flag);
}
int borland_dll Set_Broadcast_MCH(int handlemch, short Broadcast_Flag)
{
	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	if (Broadcast_Flag == BROADCAST)
	{
		cardmch[handlemch].exc_summit->control |= BROADCAST;
		cardmch[handlemch].broadcast = BROADCAST;
	}
	else if (Broadcast_Flag == NOBROADCAST)
	{
		cardmch[handlemch].exc_summit->control &= ~BROADCAST;
		cardmch[handlemch].broadcast = 0;
	}
	else
		return einval;

	return 0;
} /* end Set_Broadcast */

/*
 * Get_Broadcast returns 0 if RT 31 is a regular RT and >0 if RT 31
 * represents a Broadcast command.
 */

int borland_dll Get_Broadcast(void)
{
	usint broadcast;
	int status;

	status = Get_Broadcast_Setting_MCH(g_handlemch, &broadcast);
	if (status < 0)
		return status;
	else
		return (int) broadcast;
}
int borland_dll Get_Broadcast_Setting_MCH(int handlemch, usint *broadcast)
{
	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	*broadcast = cardmch[handlemch].exc_summit->control & BROADCAST;
	return 0;
} /* end Get_Broadcast */

/*
 * Get_BIT returns information on the current health of the board
 */

int borland_dll Get_BIT(void)
{
	usint bitval;
	int status;

	status = Get_BIT_Value_MCH(g_handlemch, &bitval);
	if (status < 0)
		return status;
	else
		return (int) bitval;
}
int borland_dll Get_BIT_Value_MCH(int handlemch, usint *bitval)
{
	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	*bitval = cardmch[handlemch].exc_summit->bit_word;
	return 0;

} /* end Get_BIT */

/*
 * Get_Pending returns all interrupts received since the last call on
 * Get_Pending, and reads another register in order to clear the Pending register
 */

int borland_dll Get_Pending(void)
{
	usint pendval;
	int status;

	status = Get_Pending_Value_MCH(g_handlemch, &pendval);
	if (status < 0)
		return status;
	else
		return (int) pendval;
}
int borland_dll Get_Pending_Value_MCH(int handlemch, usint *pendval)
{
	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	*pendval = cardmch[handlemch].exc_summit->int_pending;
	/*now read another register in order to clear the Pending register */
	temp = cardmch[handlemch].exc_summit->bit_word;
	return 0;

} /* end Get_Pending */

/*
 * Set Mode routine, e.g., BC MODE
 * This routine changes the current mode to any other
 * mode. It performs a reset of the card and initializes
 * the board to its default values for the desired mode.
 */
#define RESET_LOOPTIMES	10
#define RESET_SLEEPTIME	100L
#define HARDWARE_RESET 0xfffe/2 

int borland_dll Set_Mode(short mode)
{
	return Set_Mode_MCH(g_handlemch, mode);
}
int borland_dll Set_Mode_MCH(int handlemch, short mode)
{
	int i;

	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	if ((mode != BC_MODE_MCH) && (mode != RT_MODE_MCH) && (mode != BM_RT_MODE_MCH) && (mode != BM_MODE_MCH))
		return einval;
	cardmch[handlemch].curmode = mode;
	zeroMultibufFlag_MCH(handlemch);

	/* Set a dummy value, use it check if reset worked */
	cardmch[handlemch].exc_summit->control = 0xFF;
	/* do the hardware reset */
	cardmch[handlemch].exc_summit_int[HARDWARE_RESET] = 1; /* 0xfffe/2 */

	/* Clear memory  */
	for (i = Start_DPRAM; i < BANKSIZE; i++)
		cardmch[handlemch].exc_summit_int[i] = Clear_Pattern;

	for (i = 0; i < RESET_LOOPTIMES; i++)
	{
		if (cardmch[handlemch].exc_summit->control == 0)
			break;
		Sleep(RESET_SLEEPTIME);
	}

	if (i == RESET_LOOPTIMES)
		return etimeoutreset;

	switch (mode)
	{
	case RT_MODE_MCH:
		cardmch[handlemch].exc_summit->control = cardmch[handlemch].broadcast;
		cardmch[handlemch].exc_summit->status = RT_MODE_MCH;
		cardmch[handlemch].exc_summit->current_cmd_block = 0;
		cardmch[handlemch].exc_summit->int_mask = 0;
		cardmch[handlemch].exc_summit->int_pending = 0;
		/* last 32 words of data area */
		cardmch[handlemch].exc_summit->intr_log_list = INIT_RT_DSCRPT - 32;
		cardmch[handlemch].exc_summit->bit_word = 0;
		cardmch[handlemch].exc_summit->timetag_minor = 0;
		/* descriptor blocks */
		cardmch[handlemch].exc_summit->pointer = INIT_RT_DSCRPT;
		cardmch[handlemch].exc_summit->status1553 = 0;
		Setup_RT_Descriptors(handlemch);
		break;

	case BC_MODE_MCH:
		cardmch[handlemch].exc_summit->control = cardmch[handlemch].broadcast;
		cardmch[handlemch].exc_summit->status = BC_MODE_MCH;
		cardmch[handlemch].exc_summit->current_cmd_block = 0;
		cardmch[handlemch].exc_summit->int_mask = 0;
		cardmch[handlemch].exc_summit->int_pending = 0;
		/* last 32 words of data area */
		cardmch[handlemch].exc_summit->intr_log_list = CMDOFFSET - 32;
		cardmch[handlemch].exc_summit->bit_word = 0;
		cardmch[handlemch].exc_summit->timetag_minor = 0;
		/* Command blocks */
		cardmch[handlemch].exc_summit->pointer = CMDOFFSET;
		cardmch[handlemch].exc_summit->BC_initcount = 0;
		break;

	case BM_MODE_MCH:
		cardmch[handlemch].exc_summit->control = cardmch[handlemch].broadcast;
		cardmch[handlemch].exc_summit->status = BM_MODE_MCH;
		cardmch[handlemch].exc_summit->current_cmd_block = 0;
		cardmch[handlemch].exc_summit->int_mask = 0;
		cardmch[handlemch].exc_summit->int_pending = 0;
		/* last 32 words of data area */
		cardmch[handlemch].exc_summit->intr_log_list = INIT_MON_BLK - 32;
		cardmch[handlemch].exc_summit->bit_word = 0;
		cardmch[handlemch].exc_summit->timetag_minor = 0;
		cardmch[handlemch].exc_summit->cmd_blk_ptr = INIT_MON_BLK;
		cardmch[handlemch].exc_summit->data_ptr = INIT_DATA_BLK;
		cardmch[handlemch].exc_summit->blk_counter = MAXMESSAGES;
		cardmch[handlemch].exc_summit->filter1 = 0;
		cardmch[handlemch].exc_summit->filter2 = 0;
		break;

	case BM_RT_MODE_MCH:
		cardmch[handlemch].exc_summit->control = cardmch[handlemch].broadcast;
		cardmch[handlemch].exc_summit->status = BM_RT_MODE_MCH;
		cardmch[handlemch].exc_summit->current_cmd_block = 0;
		cardmch[handlemch].exc_summit->int_mask = 0;
		cardmch[handlemch].exc_summit->int_pending = 0;
		/* last 32 words of data area */
		cardmch[handlemch].exc_summit->intr_log_list = INIT_RT_DSCRPT - 32;
		cardmch[handlemch].exc_summit->bit_word = 0;
		cardmch[handlemch].exc_summit->timetag_minor = 0;
		cardmch[handlemch].exc_summit->cmd_blk_ptr = INIT_MON_BLK;
		cardmch[handlemch].exc_summit->data_ptr = INIT_DATA_BLK;
		cardmch[handlemch].exc_summit->blk_counter = MAXMESSAGES;
		/* descriptor blocks */
		cardmch[handlemch].exc_summit->pointer = INIT_RT_DSCRPT;
		cardmch[handlemch].exc_summit->status1553 = 0;
		cardmch[handlemch].exc_summit->filter1 = 0;
		cardmch[handlemch].exc_summit->filter2 = 0;
		Setup_RT_Descriptors(handlemch);
		break;

	default:
		break;
	}
	return 0;
}

void Setup_RT_Descriptors(int handlemch)
{
	int i, idx;
	usint bufstart;

	for (i = 0; i < 16; i++)
		cardmch[handlemch].exc_summit->illegal[i] = 0;

	bufstart = INIT_RT_BUFFER;
	idx = INIT_RT_DSCRPT;

	for (i = 0; i < 32; i++) /* receive descriptors */
	{
		cardmch[handlemch].exc_summit_int[idx] = 0x1; /* separate Broadcast  */
		cardmch[handlemch].exc_summit_int[idx + 1] = bufstart;
		cardmch[handlemch].exc_summit_int[idx + 2] = (usint) (bufstart + BUFSIZE);
		cardmch[handlemch].exc_summit_int[idx + 3] = (usint) (bufstart + (BUFSIZE * 2));
		idx += DESCRIPTORSIZE;
		bufstart += (usint) (BUFSIZE * 3);
	}
	for (i = 0; i < 32; i++) /* transmit descriptors */
	{
		cardmch[handlemch].exc_summit_int[idx] = 0;
		cardmch[handlemch].exc_summit_int[idx + 1] = bufstart;
		cardmch[handlemch].exc_summit_int[idx + 2] = bufstart + BUFSIZE;
		cardmch[handlemch].exc_summit_int[idx + 3] = 0;
		idx += DESCRIPTORSIZE;
		bufstart += (usint) (BUFSIZE * 2);
	}
	for (i = 0; i < 32; i++) /* receive mode descriptors */
	{
		cardmch[handlemch].exc_summit_int[idx] = 0x1; /* separate Broadcast */
		cardmch[handlemch].exc_summit_int[idx + 1] = bufstart;
		cardmch[handlemch].exc_summit_int[idx + 2] = bufstart + MODEBUFSIZE;
		cardmch[handlemch].exc_summit_int[idx + 3] = (usint) (bufstart + (MODEBUFSIZE * 2));
		idx += DESCRIPTORSIZE;
		bufstart += (usint) (MODEBUFSIZE * 3);
	}
	for (i = 0; i < 32; i++) /* transmit mode descriptors */
	{
		cardmch[handlemch].exc_summit_int[idx] = 0;
		cardmch[handlemch].exc_summit_int[idx + 1] = bufstart;
		cardmch[handlemch].exc_summit_int[idx + 2] = bufstart + MODEBUFSIZE;
		cardmch[handlemch].exc_summit_int[idx + 3] = 0;
		idx += DESCRIPTORSIZE;
		bufstart += (usint) (MODEBUFSIZE * 2);
	}

} /* end Setup_RT_Descriptors */

int borland_dll Stop_Card(void) /*  Stop board execution */
{
	return Stop_Card_MCH(g_handlemch);
}
int borland_dll Stop_Card_MCH(int handlemch) /*  Stop board execution */
{
	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	cardmch[handlemch].exc_summit->control &= (usint) ~START_EXECUTION;
	return 0;

} /* end Stop_Card */

int borland_dll Set_Protocol(short mode) /*  Setup response time parameters */
{
	return Set_Protocol_MCH(g_handlemch, mode);
}
int borland_dll Set_Protocol_MCH(int handlemch, short mode)
{
	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	if ((mode != MIL_STD_1553A) && (mode != MIL_STD_1553B))
		return einval;

	if (mode == MIL_STD_1553A)
	{
		cardmch[handlemch].exc_summit->status |= 0x80;
		cardmch[handlemch].milstd1553B = 0;
	}
	else
	{
		cardmch[handlemch].exc_summit->status &= 0xff7f;
		cardmch[handlemch].milstd1553B = 1;
	}

	return 0;
} /* end Set_Protocol */

/*
 * Get_Curr_Command returns the most recently processed command word
 */

int borland_dll Get_Curr_Command(void)
{
	int status;
	usint curcmd;

	status = Get_Curr_Command_Word_MCH(g_handlemch, &curcmd);
	if (status < 0)
		return status;
	else
		return (int) curcmd;

}
int borland_dll Get_Curr_Command_Word_MCH(int handlemch, usint *curcmd)
{
	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	*curcmd = cardmch[handlemch].exc_summit->current_cmd_block;
	return 0;
}

int borland_dll Get_Op_Status(void)
{
	usint opstat;
	int status;

	status = Get_Op_Status_Reg_MCH(g_handlemch, &opstat);
	if (status < 0)
		return status;
	else
		return (int) opstat;
}
int borland_dll Get_Op_Status_Reg_MCH(int handlemch, usint *opstat)
{
	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	*opstat = cardmch[handlemch].exc_summit->status;
	return 0;
}

int borland_dll Get_Memory_Pointer(unsigned int offset, usint **memptr)
{
	return Get_Memory_Pointer_MCH(g_handlemch, offset, memptr);
}
int borland_dll Get_Memory_Pointer_MCH(int handlemch, unsigned int offset, usint **memptr)
{
	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	*memptr = cardmch[handlemch].exc_summit_int + offset;
	return 0;
}
