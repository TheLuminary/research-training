#include "mchIncl.h"
extern INSTANCE_MCH cardmch[];

int borland_dll Run_BM (void)
{
	return Run_BM_MCH (g_handlemch);
}
int borland_dll Run_BM_MCH (int handlemch)
{
	if ((handlemch) <0 || (handlemch >= NUMCARDS)) return ebadhandle;
	if (!cardmch[handlemch].allocated) return ebadhandle;

	if (cardmch[handlemch].curmode != BM_MODE_MCH)
	   return (emode);

	cardmch[handlemch].exc_summit->cmd_blk_ptr = INIT_MON_BLK;
	cardmch[handlemch].exc_summit->data_ptr = INIT_DATA_BLK;
	cardmch[handlemch].exc_summit->blk_counter = MAXMESSAGES;
	cardmch[handlemch].nextmsg = 0;

	cardmch[handlemch].exc_summit->control |= START_EXECUTION;

	return (0);
} /* end Run_BM */

int borland_dll Run_BMRT_MCH (int handlemch)
{
	#define BUFFERMODE2 0x180
	usint controlval;

	if ((handlemch) <0 || (handlemch >= NUMCARDS)) return ebadhandle;
	if (!cardmch[handlemch].allocated) return ebadhandle;

	if (cardmch[handlemch].curmode != BM_RT_MODE_MCH)
	   return (emode);

/* Initialize BM Stuff */
	cardmch[handlemch].exc_summit->cmd_blk_ptr = INIT_MON_BLK;
	cardmch[handlemch].exc_summit->data_ptr = INIT_DATA_BLK;
	cardmch[handlemch].exc_summit->blk_counter = MAXMESSAGES;
	cardmch[handlemch].nextmsg = 0;

/* Initialize RT Stuff */
	controlval = (usint)( START_EXECUTION | BUAEN | BUBEN | 
		cardmch[handlemch].broadcast | cardmch[handlemch].pingpong);
	if (cardmch[handlemch].mMultiBuf == 1)
		controlval |= (usint)BUFFERMODE2; /* hopefully pingpong and broadcast not set */

	cardmch[handlemch].exc_summit->control = controlval;

	return (0);
} /* end Run_BM */


int borland_dll Monitor_All_RT (void)
{
	return Monitor_All_RT_MCH (g_handlemch);
}
int borland_dll Monitor_All_RT_MCH (int handlemch)
{
	if ((handlemch) <0 || (handlemch >= NUMCARDS)) return ebadhandle;
	if (!cardmch[handlemch].allocated) return ebadhandle;

	if ((cardmch[handlemch].curmode != BM_MODE_MCH)
	&& (cardmch[handlemch].curmode != BM_RT_MODE_MCH))
		return (emode);
	
	/*bm select and monitor all terminals*/
	cardmch[handlemch].exc_summit->control &=  ~BMTC; 

	/* set filter value for all rtids */
	cardmch[handlemch].exc_summit->filter2 = (usint)0xffff;
	cardmch[handlemch].exc_summit->filter1 = (usint)0xffff;

	return (0);
} /* end Monitor_All_RT */


int borland_dll Monitor_RT (short rtid)
{
	return Monitor_RT_MCH (g_handlemch, rtid);
}
int borland_dll Monitor_RT_MCH (int handlemch, short rtid)
{
	if ((handlemch) <0 || (handlemch >= NUMCARDS)) return ebadhandle;
	if (!cardmch[handlemch].allocated) return ebadhandle;

	if ((cardmch[handlemch].curmode != BM_MODE_MCH) &&
	(cardmch[handlemch].curmode != BM_RT_MODE_MCH))
		return (emode);
	if ((rtid<0) || (rtid>31))
		return (ebadrtid);

	/*stop monitor all terminals*/
	cardmch[handlemch].exc_summit->control |=  BMTC; 

	if (rtid>15)
		cardmch[handlemch].exc_summit->filter1 |= (usint) (1 << (rtid-16));
	else
		cardmch[handlemch].exc_summit->filter2 |=(usint) (1 << rtid);

	return (0);
} /* end Monitor_RT */


int borland_dll Clear_Monitor_RT (short rtid)
{
	return Clear_Monitor_RT_MCH (g_handlemch, rtid);
}
int borland_dll Clear_Monitor_RT_MCH (int handlemch, short rtid)
{
	if ((handlemch) <0 || (handlemch >= NUMCARDS)) return ebadhandle;
	if (!cardmch[handlemch].allocated ) return ebadhandle;

	if ((cardmch[handlemch].curmode != BM_MODE_MCH) &&
	(cardmch[handlemch].curmode != BM_RT_MODE_MCH))
	   return (emode);
	if ((rtid<0) || (rtid>31))
	   return (ebadrtid);

	/*stop monitor all terminals*/
	cardmch[handlemch].exc_summit->control |= BMTC; 

	if (rtid>15)
		 cardmch[handlemch].exc_summit->filter1 &= (usint)~(1 << (rtid-16));
	else
		cardmch[handlemch].exc_summit->filter2 &=(usint) ~(1 << rtid);

	return (0);
} /* end Clear_Monitor_RT */


int borland_dll Read_Next_Message (struct BM_MSG  *msg)
{
	return Read_Next_Message_MCH (g_handlemch, msg);
}
int borland_dll Read_Next_Message_MCH (int handlemch, struct BM_MSG  *msg)
{
	usint msgindex, nextmsgindex;
	usint i, wordcnt, subaddr;

	if ((handlemch) <0 || (handlemch >= NUMCARDS)) return ebadhandle;
	if (!cardmch[handlemch].allocated ) return ebadhandle;

	nextmsgindex = cardmch[handlemch].nextmsg;

	if ((cardmch[handlemch].curmode != BM_MODE_MCH)
	&& (cardmch[handlemch].curmode != BM_RT_MODE_MCH))
	   return (emode);

	if (cardmch[handlemch].bm_command_blocks[nextmsgindex].msginfo == Clear_Pattern)
	   return(enomsg);
	
	msg->command1 = cardmch[handlemch].bm_command_blocks[nextmsgindex].command1;
	msg->command2 = cardmch[handlemch].bm_command_blocks[nextmsgindex].command2;
	msg->status1  = cardmch[handlemch].bm_command_blocks[nextmsgindex].status1;
	msg->status2  = cardmch[handlemch].bm_command_blocks[nextmsgindex].status2;
	msg->timetag  = cardmch[handlemch].bm_command_blocks[nextmsgindex].timetag;
	msg->msginfo  = cardmch[handlemch].bm_command_blocks[nextmsgindex].msginfo;

	cardmch[handlemch].bm_command_blocks[nextmsgindex].msginfo = Clear_Pattern;
	
	msgindex = cardmch[handlemch].bm_command_blocks[nextmsgindex].data_ptr;
	subaddr =(usint)( (msg->command1 >> 5) & 0x1f);
	if ((subaddr == 0) || (cardmch[handlemch].milstd1553B && (subaddr == 31)))
		wordcnt = 1;
	else
	{
		wordcnt = (usint) (msg->command1 & 0x1f);
		if (wordcnt == 0)
			wordcnt = 32;
	}
	for (i = 0; i < wordcnt; i++)
		msg->data[i] = cardmch[handlemch].exc_summit_int[(msgindex) + i];

	if (cardmch[handlemch].nextmsg++ == MAXMESSAGES)
		cardmch[handlemch].nextmsg = 0;

	return(0);
} /* end Read_Next_Message */
