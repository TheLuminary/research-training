#include "mchIncl.h"
extern INSTANCE_MCH cardmch[];

/* setup a BC message */

int borland_dll Set_BC_Cmd (usint cmdnum, struct CMDENTRY *entry)
{
	return Set_BC_Cmd_MCH (g_handlemch, cmdnum, entry);
}
int borland_dll Set_BC_Cmd_MCH (int handlemch, usint cmdnum, struct CMDENTRY *entry)
{
	if ((handlemch) <0 || (handlemch >= NUMCARDS)) return ebadhandle;
	if (!cardmch[handlemch].allocated) return ebadhandle;

	if (cardmch[handlemch].curmode != BC_MODE_MCH)
		return (emode);
	if (cmdnum > MAXCMDS)
		return (einval);

     /* do some error checking if it is RT to RT command */
	if (entry->control & CW_RT2RT)  /* if RT to RT */
	{
		if ( (entry->command1 & TRANSMIT_BIT) ||  /* if 1st command is not RCV */
		(!(entry->command2 & TRANSMIT_BIT)) ||  /* if 2nd command is not XMT */
							/*if different word count*/
		((entry->command1 & WORD_CNT_MASK) != (entry->command2 & WORD_CNT_MASK)) ) 			
			return (ert2rtcmd);
	}
	cardmch[handlemch].bc_command_blocks[cmdnum].control  = entry->control;
	cardmch[handlemch].bc_command_blocks[cmdnum].command1 = entry->command1;
	cardmch[handlemch].bc_command_blocks[cmdnum].command2 = entry->command2;
	cardmch[handlemch].bc_command_blocks[cmdnum].data_ptr =(usint) ((entry->datablk * DATABLKSIZE) + Start_DPRAM);
	cardmch[handlemch].bc_command_blocks[cmdnum].branch   =(usint) ((entry->gotocmd * CMDENTRYSIZE) + CMDOFFSET);
	cardmch[handlemch].bc_command_blocks[cmdnum].timer    = entry->timer;

	return(0);
} /* end Set_BC_Cmd */


int borland_dll Get_BC_Cmd (usint cmdnum, struct CMDENTRY *entry)
{
	return Get_BC_Cmd_MCH (g_handlemch, cmdnum, entry);
}
int borland_dll Get_BC_Cmd_MCH (int handlemch, usint cmdnum, struct CMDENTRY *entry)
{
	if ((handlemch) <0 || (handlemch >= NUMCARDS)) return ebadhandle;
	if (!cardmch[handlemch].allocated) return ebadhandle;

	if (cardmch[handlemch].curmode != BC_MODE_MCH)
		return (emode);
	if (cmdnum > MAXCMDS)
		return (einval);
	entry->control  = cardmch[handlemch].bc_command_blocks[cmdnum].control;
	entry->command1 = cardmch[handlemch].bc_command_blocks[cmdnum].command1;
	entry->command2 = cardmch[handlemch].bc_command_blocks[cmdnum].command2;
	entry->status1  = cardmch[handlemch].bc_command_blocks[cmdnum].status1;
	entry->status2  = cardmch[handlemch].bc_command_blocks[cmdnum].status2;
	entry->datablk  = (usint) ((cardmch[handlemch].bc_command_blocks[cmdnum].data_ptr - Start_DPRAM) / DATABLKSIZE);
	entry->gotocmd  = (usint)((cardmch[handlemch].bc_command_blocks[cmdnum].branch - CMDOFFSET) / CMDENTRYSIZE);
	entry->timer    = cardmch[handlemch].bc_command_blocks[cmdnum].timer;

	return(0);
} /* end Get_BC_Cmd */

int borland_dll Load_BC_Datablk (usint blknum, usint wdcnt, usint *data)
{
	return Load_BC_Datablk_MCH (g_handlemch, blknum, wdcnt, data);
}
int borland_dll Load_BC_Datablk_MCH (int handlemch, usint blknum, usint wdcnt, usint *data)
{
	short dataind;
	usint cardindex;

	if ((handlemch) <0 || (handlemch >= NUMCARDS)) return ebadhandle;
	if (!cardmch[handlemch].allocated) return ebadhandle;

	if (cardmch[handlemch].curmode != BC_MODE_MCH)
		return (emode);
	if (blknum > MAXDATABLKS)
		return (einval);
	if (wdcnt > DATABLKSIZE)
		return (einval);
	cardindex =(usint) ((blknum * DATABLKSIZE) + Start_DPRAM);
	for (dataind = 0; dataind < wdcnt; dataind++)
		cardmch[handlemch].exc_summit_int[cardindex + dataind] = data[dataind];

	return(0);
} /* end Load_BC_Datablk */


int borland_dll Read_BC_Datablk (usint blknum, usint wdcnt, usint *data)
{
	return Read_BC_Datablk_MCH (g_handlemch, blknum,wdcnt, data);
}
int borland_dll Read_BC_Datablk_MCH (int handlemch, usint blknum, usint wdcnt, usint *data)
{
	short dataind;
	short cardindex;

	if ((handlemch) <0 || (handlemch >= NUMCARDS)) return ebadhandle;
	if (!cardmch[handlemch].allocated) return ebadhandle;

	if (cardmch[handlemch].curmode != BC_MODE_MCH)
		return (emode);
	if (blknum > MAXDATABLKS)
		return (einval);
	if (wdcnt > DATABLKSIZE)
		return (einval);
	cardindex =(usint) ((blknum * DATABLKSIZE) + Start_DPRAM);
	for (dataind = 0; dataind < wdcnt; dataind++)
		data[dataind] = cardmch[handlemch].exc_summit_int[cardindex + dataind];

	return(0);
} /* end Read_BC_Datablk */


int borland_dll Run_BC (void)
{
	return Run_BC_MCH (g_handlemch);
}
int borland_dll Run_BC_MCH (int handlemch)
{
	if ((handlemch) <0 || (handlemch >= NUMCARDS)) return ebadhandle;
	if (!cardmch[handlemch].allocated) return ebadhandle;

	if (cardmch[handlemch].curmode != BC_MODE_MCH)
		return (emode);
	cardmch[handlemch].exc_summit->pointer  = CMDOFFSET; /* Command blocks */
	cardmch[handlemch].exc_summit->control |= START_EXECUTION;
	return(0);

} /* end Run_BC */

int borland_dll Last_BC_Cmd (void)
{
	usint cmdnum;
	int status;

	status = Last_BC_Cmdnum_MCH (g_handlemch, &cmdnum);
	if (status <0)
		return status;
	else
		return (int)cmdnum;
}
int borland_dll Last_BC_Cmdnum_MCH (int handlemch, usint *cmdnum)
{
	if ((handlemch) <0 || (handlemch >= NUMCARDS)) return ebadhandle;
	if (!cardmch[handlemch].allocated) return ebadhandle;

	if (cardmch[handlemch].curmode != BC_MODE_MCH)
		return (emode);
	/* Command blocks */
	*cmdnum = (cardmch[handlemch].exc_summit->pointer - CMDOFFSET)/CMDENTRYSIZE; 

	return 0;
} /* end Last_BC_Cmdnum */

int borland_dll Command_Word_MCH(int rtnum, int type, int subaddr, int numwords, usint *commandword)
{
	int rtid;

	if ((rtnum <0) || (rtnum > 31))
		return einval;
	if ((type != TRANSMIT) && (type != RECEIVE))
		return einval;
	if ((subaddr <0 ) || (subaddr > 31))
		return einval;

	if ((numwords <0) || (numwords >32))
		return einval;

	if (numwords == 32)
		numwords = 0;	

	rtid = (rtnum << 6) | (type <<5) | subaddr;

	*commandword = (rtid<<5) | numwords;
        return 0;
}
int borland_dll Control_Word_MCH(usint opcode, usint retrynum, usint bus, usint rt2rt_flag, usint ccodes, usint *controlword)
{
	if ((opcode & INVALID_OPCODE_MASK) != 0)
		return einval;
	if ((opcode & RETRY_ON_COND) || (opcode & RETRY_ON_COND_BRANCH) || (opcode & RETRY_ON_COND_BRANCH_ALL_FAIL))
	{
		if ((retrynum <1) || (retrynum > 4))
			return einval;
	}
	else
		retrynum = 0; /* ignored anyway */
	if ((bus != CW_BUS_A) && (bus != CW_BUS_B))
		return einval;
	if ((rt2rt_flag != CW_RT2RT ) && (rt2rt_flag != CW_NOT_RT2RT))
		return einval;
	if (((ccodes < LOWEST_CCODE) && (ccodes != 0)) || ( ccodes > ALL_CCODES))
		return einval;

	if (retrynum == 4)
		retrynum = 0;

	*controlword = (opcode | (retrynum <<10) | bus  | rt2rt_flag | ccodes );

	return 0;
}