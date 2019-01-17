#include "mchIncl.h"
extern INSTANCE_MCH cardmch[];
#include <stdio.h>

#define IMMEDIATE_BIT	0x8000
#define MAX_BUFFERS	480
#define BUFFER_AREA	0x4000

/* set the RT number in single RT mode */
#define NUMBITS_IN_RTID 5
int borland_dll Set_RT_Num(short rtid)
{
	return Set_RT_Num_MCH(g_handlemch, rtid);
}
int borland_dll Set_RT_Num_MCH(int handlemch, short rtid)
{
	usint parity, i, num_of_ones = 0;

	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	if ((cardmch[handlemch].curmode != RT_MODE_MCH) && (cardmch[handlemch].curmode != BM_RT_MODE_MCH))
		return (emode);
	if ((rtid < 0) || (rtid > 31))
		return (einval);

	/* Compute how many of the bits of the rtid are 1's */
	for (i = 0; i < NUMBITS_IN_RTID; i++)
		num_of_ones += (usint) ((rtid >> i) & 1);

	/* Now we check the lsbit of 'num_of_ones'.
	 If lsb is 0,  number of 1s was even; else odd */

	parity = num_of_ones & 1; /* rev_parity indicates odd (1) or even (0) parity */
	parity ^= 1; /* Reverse the above (0 for odd parity, 1 for even parity) */

	/* The rtid and its parity are now written into the top 6 bits of opstatus reg */

	cardmch[handlemch].exc_summit->status = (usint) ((parity << 10) + (rtid << 11)) | (cardmch[handlemch].exc_summit->status & 0x03ff);

	return (0);
} /* end Set_RT_Num */

int borland_dll Load_Datablk(usint subaddress, usint modecode, usint *data)
{
	return Load_Datablk_MCH(g_handlemch, subaddress, modecode, data);
}
int borland_dll Load_Datablk_MCH(int handlemch, usint subaddress, usint modecode, usint *data)
{
	usint dataind, cardindex, maxdata, offset;
	int ctrl_word_offset;

	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	if ((cardmch[handlemch].curmode != RT_MODE_MCH) && (cardmch[handlemch].curmode != BM_RT_MODE_MCH))
		return (emode);
	if ((subaddress == 0) || (subaddress > MODECODE))
		return (einval);
	if (modecode > 31)
		return (einval);

	if (subaddress == MODECODE)
	{
		offset = (usint) 64;
		offset += (usint) 32; /* for transmit */
		maxdata = 1;
		offset += modecode;
	}
	else
	{
		offset = (usint) 32; /*for transmit */
		maxdata = 32;
		offset += subaddress;
	}

	ctrl_word_offset = INIT_RT_DSCRPT + (offset * DESCRIPTORSIZE);

	if (cardmch[handlemch].pingpong)
	{
		if (cardmch[handlemch].exc_summit_int[ctrl_word_offset] & PINGPONG)
			cardindex = cardmch[handlemch].exc_summit_int[ctrl_word_offset + 1];
		else
			cardindex = cardmch[handlemch].exc_summit_int[ctrl_word_offset + 2];
	}
	else
		cardindex = cardmch[handlemch].exc_summit_int[ctrl_word_offset + 1];

	for (dataind = 0; dataind < maxdata; dataind++)
		cardmch[handlemch].exc_summit_int[cardindex + dataind + 2] = data[dataind];

	return (0);
} /* end Load_Datablk */

int borland_dll Read_Datablk(usint subaddress, usint modecode, usint *data, short msgtype, short txorrx)
{
	return Read_Datablk_MCH(g_handlemch, subaddress, modecode, data, msgtype, txorrx);
}
int borland_dll Read_Datablk_MCH(int handlemch, usint subaddress, usint modecode, usint *data, short msgtype, short txorrx)
{
	usint dataind, cardindex, maxdata, offset = 0;
	int ctrl_word_offset;

	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	if ((cardmch[handlemch].curmode != RT_MODE_MCH) && (cardmch[handlemch].curmode != BM_RT_MODE_MCH))
		return (emode);
	if ((subaddress == 0) || (subaddress > MODECODE))
		return (einval);
	if (modecode > 31)
		return (einval);

	if ((msgtype != STANDARD_CMD) && (msgtype != BRDCAST_CMD) && (msgtype != MODE_CMD) && (msgtype != BROADCAST_MODE_CMD))
		return (einval);
	if ((txorrx != TRANSMIT) && (txorrx != RECEIVE))
		return (einval);
	if ((subaddress == MODECODE) ^ ((msgtype == MODE_CMD) || (msgtype == BROADCAST_MODE_CMD)))
		return (incompatible_parameters);

	if ((msgtype == MODE_CMD) || (msgtype == BROADCAST_MODE_CMD))
	{
		offset = (usint) 64 + modecode;
		offset += modecode; /* 15/07/03 */
		maxdata = 1;
	}
	else
	{
		maxdata = 32;
		offset += subaddress;
	}

	if (txorrx == TRANSMIT)
		offset += 32;

	ctrl_word_offset = INIT_RT_DSCRPT + (offset * DESCRIPTORSIZE);

	if ((msgtype == BRDCAST_CMD) || (msgtype == BROADCAST_MODE_CMD))
		cardindex = cardmch[handlemch].exc_summit_int[ctrl_word_offset + 3];
	else
	{
		if (cardmch[handlemch].pingpong)
		{
			if (cardmch[handlemch].exc_summit_int[ctrl_word_offset] & PINGPONG)
				cardindex = cardmch[handlemch].exc_summit_int[ctrl_word_offset + 1];
			else
				cardindex = cardmch[handlemch].exc_summit_int[ctrl_word_offset + 2];
		}
		else
			cardindex = cardmch[handlemch].exc_summit_int[ctrl_word_offset + 1];
	}

	for (dataind = 0; dataind < maxdata + 2; dataind++)
		data[dataind] = cardmch[handlemch].exc_summit_int[cardindex + dataind];

	return (0);
} /* end Read_Datablk */

int borland_dll Set_RT_Status(short status)
{
	return Set_RT_Status_MCH(g_handlemch, status);
}
int borland_dll Set_RT_Status_MCH(int handlemch, short status)
{
	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	if ((cardmch[handlemch].curmode != RT_MODE_MCH) && (cardmch[handlemch].curmode != BM_RT_MODE_MCH))
		return (emode);

	/* don't touch Immediate Clear bit */
	cardmch[handlemch].exc_summit->status1553 = ((usint) status & ~IMMEDIATE_BIT);

	return (0);
} /* end Set_RT_Status */

int borland_dll Set_Status_Clear(short flag)
{
	return Set_Status_Clear_MCH(g_handlemch, flag);
}
int borland_dll Set_Status_Clear_MCH(int handlemch, short flag)
{
	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	if ((cardmch[handlemch].curmode != RT_MODE_MCH) && (cardmch[handlemch].curmode != BM_RT_MODE_MCH))
		return (emode);
	if ((flag != IMMEDIATE) && (flag != STATIC))
		return (einval);

	/* touch only Immediate Clear bit */
	if (flag == IMMEDIATE)
		cardmch[handlemch].exc_summit->status1553 |= IMMEDIATE_BIT;
	else
		cardmch[handlemch].exc_summit->status1553 &= ~IMMEDIATE_BIT;

	return (0);
} /* end Set_Status_Clear */

int borland_dll Set_Interrupt_Mask(usint intr)
{
	return Set_Interrupt_Mask_MCH(g_handlemch, intr);
}
int borland_dll Set_Interrupt_Mask_MCH(int handlemch, usint intr)
{
	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	/* Merlin uses mask without a real interrupt */
	cardmch[handlemch].exc_summit->int_mask = intr;
	if (cardmch[handlemch].intnum == 0)
		return (noirqset);

	return (0);
} /* end Set_Interrupt_Mask */

int borland_dll Run_RT(void)
{
	return Run_RT_MCH(g_handlemch);
}
int borland_dll Run_RT_MCH(int handlemch)
{
#define BUFFERMODE2 0x180
	usint controlval;

	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	if (cardmch[handlemch].curmode != RT_MODE_MCH)
		return (emode);
	controlval = (usint) ( START_EXECUTION | BUAEN | BUBEN | cardmch[handlemch].broadcast | cardmch[handlemch].pingpong);
	if (cardmch[handlemch].mMultiBuf == 1)
		controlval |= (usint) BUFFERMODE2; /* hopefully pingpong and broadcast not set */

	cardmch[handlemch].exc_summit->control = controlval;

	return (0);
} /* end Run_RT */

#define TRANSMIT_OFFSET	2
#define BROAD_OFFSET	4
#define MODE_OFFSET	8
int borland_dll Set_Legal_Command(usint subaddress, short brdcst, short txorrx, short ismode, short legal)
{
	return Set_Legal_Command_MCH(g_handlemch, subaddress, brdcst, txorrx, ismode, legal);
}
int borland_dll Set_Legal_Command_MCH(int handlemch, usint subaddress, short brdcst, short txorrx, short ismode, short legal)
{
	usint index = 0, bit_position;

	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	if ((cardmch[handlemch].curmode != RT_MODE_MCH) && (cardmch[handlemch].curmode != BM_RT_MODE_MCH))
		return (emode);
	if (subaddress > 31)
		return (einval);
	if ((brdcst != BROADCAST) && (brdcst != NOBROADCAST))
		return (einval);
	if ((txorrx != TRANSMIT) && (txorrx != RECEIVE))
		return (einval);
	if ((ismode != MODECODE) && (ismode != NOMODECODE))
		return (einval);
	if ((legal != LEGAL) && (legal != ILLEGAL))
		return (einval);

	if (brdcst == BROADCAST)
		index += BROAD_OFFSET;
	if (txorrx == TRANSMIT)
		index += TRANSMIT_OFFSET;
	if (ismode == MODECODE)
		index += MODE_OFFSET;

	if (subaddress > 15)
	{
		index++;
		bit_position = subaddress - (usint) 16;
	}
	else
		bit_position = subaddress;

	if (legal)
		cardmch[handlemch].exc_summit->illegal[index] &= (usint) ~(1 << bit_position);
	else
		cardmch[handlemch].exc_summit->illegal[index] |= (usint) (1 << bit_position);

	return (0);
} /* end Set_Legal_Command */

#define ALLINT_MASK 0x060
int borland_dll Set_Subaddr_Int_MCH(int handlemch, usint subaddr, short txorrx, short msgtype, short inttype)
{
	usint offset, intbits;
	int ctrl_word_offset;

	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	if ((cardmch[handlemch].curmode != RT_MODE_MCH) && (cardmch[handlemch].curmode != BM_RT_MODE_MCH))
		return (emode);
	if (subaddr > 31)
		return (einval);
	if ((txorrx != TRANSMIT) && (txorrx != RECEIVE))
		return (einval);
	if ((msgtype != MODE_CMD) && (msgtype != STANDARD_CMD))
		return (einval);
	if ((inttype != ACCESS) && (inttype != BROADCAST) && (inttype != ACCESS_AND_BROADCAST) && (inttype != NO_SA_INTERRUPT))
		return (einval);

	offset = subaddr;
	if (msgtype == MODE_CMD)
		offset += (usint) 64;
	if (txorrx == TRANSMIT)
		offset += (usint) 32;

	if (inttype == BROADCAST)
		intbits = 0x20;
	else if (inttype == ACCESS)
		intbits = 0x40;
	else if (inttype == ACCESS_AND_BROADCAST)
		intbits = ALLINT_MASK;
	else
		/* inttype = NO_SA_INTERRUPT */
		intbits = 0;

	ctrl_word_offset = INIT_RT_DSCRPT + (offset * DESCRIPTORSIZE);
	/* first unset the sa interrupts */
	cardmch[handlemch].exc_summit_int[ctrl_word_offset] &= (usint) ~ALLINT_MASK;
	/* now set the requested interrupt types */
	cardmch[handlemch].exc_summit_int[ctrl_word_offset] |= intbits;

	return (0);
} /* end Set_Subaddr_Int */

/* **********************************************************************
 *
 * Function: Datablk_Accessed (in module RTSET.C)
 *
 * Inputs:
 *          subaddr - the selected subaddress of the RT
 *          txorrx  - flag, indicates we are looking for a
 *                    Transmit or Receive msg
 *          msgtype - either MODE_CMD or STANDARD_CMD

 *
 * Outputs: none
 * Returns: 1 if datablock has been accessed (ie, completion of
 *               message processing)
 *          0 otherwise
 *
 *
 * Check the Block Accessed bit in the area of memory assigned to the
 * selected subaddress for the RT. If the Block Accessed (BAC) bit in
 * the Receive Control Word is set to 1, this indicates that the block
 * has been accessed (ie, completion of message processing, either of
 * a Transmit or a Receive message). Reset the bit to 0, and set the return
 * value to indicate accessed. If the BAC bit is set to 0, this indicates
 * that no message has completed processing since this bit was last checked.
 *
 ********************************************************************** */

int borland_dll Datablk_Accessed(usint subaddr, short txorrx, short msgtype)
{
	usint accessed;
	int status;

	status = Get_Datablk_Accessed_MCH(g_handlemch, subaddr, txorrx, msgtype, &accessed);
	if (status < 0)
		return status;
	else
		return (int) accessed;
}
int borland_dll Get_Datablk_Accessed_MCH(int handlemch, usint subaddr, short txorrx, short msgtype, usint *accessed)
{
	usint temp, offset;
	short BAC_Bit = 0x10;
	int ctrl_word_offset;

	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	if ((cardmch[handlemch].curmode != RT_MODE_MCH) && (cardmch[handlemch].curmode != BM_RT_MODE_MCH))
		return (emode);
	if (subaddr > 31)
		return (einval);
	if ((txorrx != TRANSMIT) && (txorrx != RECEIVE))
		return (einval);
	if ((msgtype != MODE_CMD) && (msgtype != STANDARD_CMD) && (msgtype != MODECODE)) /* for compatibility with bug in documentation */
		return (einval);

	offset = subaddr;
	if ((msgtype == MODE_CMD) || (msgtype == MODECODE)) /* same bug */
		offset += (usint) 64;
	if (txorrx == TRANSMIT)
		offset += (usint) 32;

	ctrl_word_offset = INIT_RT_DSCRPT + (offset * DESCRIPTORSIZE);
	temp = cardmch[handlemch].exc_summit_int[ctrl_word_offset];

	if ((temp & BAC_Bit) == BAC_Bit)
	{
		*accessed = 1;
		cardmch[handlemch].exc_summit_int[ctrl_word_offset] &= (usint) ~BAC_Bit;
	}
	else
		*accessed = 0;

	return (0);
}
/*CR1*/
#define MAXDATA 32
int borland_dll Load_Buffer(usint buffernum, usint *data)
{
	return Load_Buffer_MCH(g_handlemch, buffernum, data);
}
int borland_dll Load_Buffer_MCH(int handlemch, usint buffernum, usint *data)
{
	usint dataind, buffstart;

	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	if ((cardmch[handlemch].curmode != RT_MODE_MCH) && (cardmch[handlemch].curmode != BM_RT_MODE_MCH))
		return (emode);
	if (buffernum > MAX_BUFFERS)
		return einval;

	/* +2: put data after info wd and ttag */
	buffstart = BUFFER_AREA + (buffernum * BUFSIZE) + 2;

	for (dataind = 0; dataind < MAXDATA; dataind++)
		cardmch[handlemch].exc_summit_int[buffstart + dataind] = data[dataind];

	return (0);
}
/*CR1*/
int borland_dll Assign_Buffer(usint subaddr, usint buffernum)
{
	return Assign_Buffer_MCH(g_handlemch, subaddr, buffernum);
}
int borland_dll Assign_Buffer_MCH(int handlemch, usint subaddr, usint buffernum)
{
	int ctrl_word_offset;
	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	if ((cardmch[handlemch].curmode != RT_MODE_MCH) && (cardmch[handlemch].curmode != BM_RT_MODE_MCH))
		return (emode);
	if (buffernum > MAX_BUFFERS)
		return einval;

	ctrl_word_offset = INIT_RT_DSCRPT + ((32 + subaddr) * DESCRIPTORSIZE);
	cardmch[handlemch].exc_summit_int[ctrl_word_offset + 1] = BUFFER_AREA + (buffernum * BUFSIZE);
	return 0;
}
/* **********************************************************************
 *
 * Function: Setup_Multibuffer
 *
 * Inputs:
 *          subaddr - the selected subaddress of the RT
 *          numbufs - the number of multibuffers to asociate with the subaddress
 *          startaddr - the word address of the beginning of the data buffer
 *
 * Outputs: none
 * Returns: einval  if the buffer would crash into the RT descriptors or other bad input
 *          0 otherwise
 *
 *
 * Sets up the descriptor associated with the selected suabddress to work in
 * buffer2 (multibuffer) mode. In this mode 2 circular buffers are used. The first contains * the data
 * to be sent on consecutive transmit messages. The pointer to this buffer is placed in the
 * second word of the descriptor block. The second buffer contains the timetag and
 * message information words for each command received. The pointer to this buffer is
 * placed in the fourth word of the descriptor block. Since the second buffer contains two
 * words per block, its size is twice the number of blocks. That number - 1 is placed in the
 * high byte of the control word.
 ********************************************************************** */

int borland_dll Setup_Multibuffer_MCH(int handlemch, unsigned short subaddr, short numbufs, short startaddr)
{
	unsigned short temp;
	unsigned short offset;

	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;
	if ((cardmch[handlemch].curmode != RT_MODE_MCH) && (cardmch[handlemch].curmode != BM_RT_MODE_MCH))
		return (emode);
	if (cardmch[handlemch].mMultiBuf == 0)
		return (emode);
	if (subaddr > 31)
		return (einval);
	if (((startaddr + (numbufs * 34)) > INIT_RT_DSCRPT) || (startaddr < Start_DPRAM))
		return (einval);
	offset = subaddr + 32;    //we only support transmit buffering right now

	temp = cardmch[handlemch].exc_summit_int[ INIT_RT_DSCRPT + (offset * DESCRIPTORSIZE)];
	temp |= ((numbufs * 2) - 1) << 8;    //size of ttag buffer - 1
	cardmch[handlemch].exc_summit_int[ INIT_RT_DSCRPT + (offset * DESCRIPTORSIZE)] = temp;
	//first pointer is the address of the data buffer
	cardmch[handlemch].exc_summit_int[ INIT_RT_DSCRPT + (offset * DESCRIPTORSIZE) + 1] = startaddr;
	//second pointer is used by the board as the current pointer
	cardmch[handlemch].exc_summit_int[ INIT_RT_DSCRPT + (offset * DESCRIPTORSIZE) + 2] = startaddr;
	//third pointer is address of circular  ttag buffer which follows the circular data buffer
	cardmch[handlemch].exc_summit_int[ INIT_RT_DSCRPT + (offset * DESCRIPTORSIZE) + 3] = startaddr + (numbufs * 32);
	return (0);
}

int borland_dll Load_Multibuffer_Data_MCH(int handlemch, unsigned short numwords, short address, short *data)
{
	int loop;

	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;
	if ((cardmch[handlemch].curmode != RT_MODE_MCH) && (cardmch[handlemch].curmode != BM_RT_MODE_MCH))
		return (emode);
	if (cardmch[handlemch].mMultiBuf == 0)
		return (emode);
	if (((address + numwords) > INIT_RT_DSCRPT) || (address < Start_DPRAM))
		return (einval);
	for (loop = 0; loop < numwords; loop++)
	{
		cardmch[handlemch].exc_summit_int[address + loop] = data[loop];
	}
	return 0;
}

int borland_dll Enable_Multibuffer_MCH(int handlemch)
{
	int i, idx;
	unsigned short bufstart;

	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;

	if ((cardmch[handlemch].curmode != RT_MODE_MCH) && (cardmch[handlemch].curmode != BM_RT_MODE_MCH))
		return (emode);
	idx = INIT_RT_DSCRPT;
	bufstart = INIT_RT_BUFFER;
	cardmch[handlemch].mMultiBuf = 1;
	for (i = 0; i < 128; i++) /* receive descriptors */
	{
		cardmch[handlemch].exc_summit_int[idx] = 0x0100;    //single buffer
		cardmch[handlemch].exc_summit_int[idx + 1] = bufstart + 2;    //pointer to data
		cardmch[handlemch].exc_summit_int[idx + 2] = bufstart + 2;    //current pointer
		cardmch[handlemch].exc_summit_int[idx + 3] = bufstart;    //ttag and msginfo
		idx += DESCRIPTORSIZE;
		bufstart += BUFSIZE;
	}

	return (0);
}

int borland_dll Disable_Multibuffer_MCH(int handlemch)
{
	if ((handlemch) < 0 || (handlemch >= NUMCARDS))
		return ebadhandle;
	if (!cardmch[handlemch].allocated)
		return ebadhandle;
	return Set_Mode_MCH(handlemch, cardmch[handlemch].curmode);
}

void zeroMultibufFlag_MCH(int handlemch)
{
	cardmch[handlemch].mMultiBuf = 0;
}
