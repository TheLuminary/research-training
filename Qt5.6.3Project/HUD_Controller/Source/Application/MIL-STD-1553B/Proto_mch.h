#ifndef __MCH_PROTO_H
#define __MCH_PROTO_H


#define borland_dll __declspec(dllexport)
#include <windows.h>

typedef unsigned short int usint;
#include "error_mch.h"
#include "error_devio.h"
#include "flags_mch.h"

/* function prototypes */

#ifdef __cplusplus
extern "C"
{
#endif

/* INITCARD.C */
int borland_dll Init_Module_MCH(usint, usint) ;
int borland_dll Release_Module_MCH (int);

/* GSET.C */
int borland_dll Set_PingPong_MCH(int, short);
int borland_dll Set_Broadcast_MCH(int, short);
int borland_dll Get_Broadcast_Setting_MCH(int, usint *broadcast);
int borland_dll Get_BIT_Value_MCH(int, usint *bitval);
int borland_dll Get_Pending_Value_MCH(int, usint *pendval);
int borland_dll Set_Mode_MCH(int, short);
int borland_dll Stop_Card_MCH(int);
int borland_dll Set_Protocol_MCH(int, short mode);
int borland_dll Get_Curr_Command_Word_MCH(int, usint *curcmd);
int borland_dll Get_Op_Status_Reg_MCH(int, usint *opstat);
/* internal function needed for test programs */
int borland_dll Get_Memory_Pointer_MCH(int, unsigned int offset, usint **memptr); 

/* BMSET.C */
int borland_dll Run_BM_MCH(int);
int borland_dll Run_BMRT_MCH (int handlemch);
int borland_dll Monitor_All_RT_MCH(int);
int borland_dll Monitor_RT_MCH(int, short rtid);
int borland_dll Clear_Monitor_RT_MCH(int, short rtid);
int borland_dll Read_Next_Message_MCH(int, struct BM_MSG *msg);

/* RTSET.C */
int borland_dll Set_RT_Num_MCH(int, short rtid);
int borland_dll Load_Datablk_MCH(int, usint subaddress, usint modecode, usint *data);
int borland_dll Read_Datablk_MCH(int, usint subaddress, usint modecode, usint *data, short msgtype, short txorrx);
int borland_dll Set_RT_Status_MCH(int, short status);
int borland_dll Set_Status_Clear_MCH(int, short flag);
int borland_dll Run_RT_MCH(int);
int borland_dll Set_Interrupt_Mask_MCH(int, usint intr);
int borland_dll Set_Legal_Command_MCH(int, usint subaddress, short brdcst, short txorrx, short ismode, short legal);
int borland_dll Set_Subaddr_Int_MCH(int, usint subaddr, short txorrx, short msgtype, short inttype);
int borland_dll Get_Datablk_Accessed_MCH(int, unsigned short subaddr, short txorrx, short msgtype, usint *accessed);
int borland_dll Load_Buffer_MCH (int brdmch, usint buffernum, usint *data);
int borland_dll Assign_Buffer_MCH(int brdmch, usint subaddr, usint buffernum);
int borland_dll Setup_Multibuffer_MCH(int brdmch,unsigned short subaddr, short numbufs, short startaddr);
int borland_dll Load_Multibuffer_Data_MCH(int brdmch,unsigned short numwords, short address, short *data);
int borland_dll Enable_Multibuffer_MCH(int brdmch);
int borland_dll Disable_Multibuffer_MCH(int brdmch);

/* ERROR.C */
int borland_dll Get_Error_String_MCH(int errcode, int errlen, char *errstring);
borland_dll char * Print_Error_MCH (int errorcode);

/* BCSET.C */
int borland_dll Set_BC_Cmd_MCH(int, usint cmdnum, struct CMDENTRY *entry);
int borland_dll Get_BC_Cmd_MCH(int, usint cmdnum, struct CMDENTRY *entry);
int borland_dll Load_BC_Datablk_MCH(int, usint blknum, usint wdcnt, usint *data);
int borland_dll Read_BC_Datablk_MCH(int, usint blknum, usint wdcnt, usint *data);
int borland_dll Run_BC_MCH(int );
int borland_dll Last_BC_Cmdnum_MCH(int, usint *cmdnum);
int borland_dll Command_Word_MCH(int rtnum, int type, int subaddr, int wordcount, usint *commandword);
int borland_dll Control_Word_MCH(usint opcode, usint retrynum, usint bus, usint rt2rt_flag, usint ccodes, usint *controlword);

/* deviceio_Mch.c */
int borland_dll Wait_For_Interrupt_MCH(int handle, unsigned int timeout);
int borland_dll Get_Interrupt_Count_MCH(int handle, unsigned long int*);
int borland_dll Wait_For_Multiple_Interrupts_MCH(int numints, int *handle_array, unsigned int timeout, unsigned long *Interrupt_Bitfield);
int borland_dll GetCardType_MCH(int handle, unsigned long *cardtype);

#ifdef __cplusplus
}
#endif

#endif
