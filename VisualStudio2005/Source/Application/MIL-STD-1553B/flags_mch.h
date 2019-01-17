#ifndef __MCH_FLAGS_H
#define __MCH_FLAGS_H


struct BM_MSG      /* structure needed by user to read monitored messages */
 {
  unsigned short data[32];  /* msg data words                       */
  unsigned short command1;  /* command word                         */
  unsigned short command2;  /* 2nd command word (for RT to RT)      */
  unsigned short status1;   /* status word                          */
  unsigned short status2;   /* 2nd status word (for RT to RT)       */
  unsigned short timetag;   /* time tag associated with the message */
  unsigned short msginfo;   /* bus and error information            */
 };

struct CMDENTRY
 {
  unsigned short control;   /* control word                         */
  unsigned short command1;  /* command word                         */
  unsigned short command2;  /* 2nd command word (for RT to RT)      */
  unsigned short status1;   /* status word                          */
  unsigned short status2;   /* 2nd status word (for RT to RT)       */
  unsigned short timer;     /* minor frame time or shortermessage gap */
  unsigned short datablk;   /* block number of associated data block*/
  unsigned short gotocmd;   /* command block number for branch cmds */
 };

/* flag used by Init_Module */
#define SIMULATE      0xFFFF
#define MAX_REGISTRY_DEVICES	16
#define EXC_4000PCI				MAX_REGISTRY_DEVICES+9

#define NUMBOARDS 4
#define NUMMODULES 4
/* can have 4 cards with 4 modules each, plus one simulated */
#define NUMCARDS ((NUMBOARDS * NUMMODULES) + 1)
#define INVALID_DEV 999
#define INVALID_MOD 999

/* Mode defines */
#define MSEL        0x0300   /* status word bits for mode selection */
#define BC_MODE_MCH     0x0000
#define RT_MODE_MCH     0x0100
#define BM_MODE_MCH     0x0200
#define BM_RT_MODE_MCH  0x0300


/* RT mode defines */
#define DESCRIPTORSIZE 4
#define TRANSMIT 1
#define RECEIVE  0
#define LEGAL    1
#define ILLEGAL  0
#define MODECODE 32
#define NOMODECODE 0

#define STANDARD_CMD       0
#define BRDCAST_CMD        1
#define MODE_CMD           2
#define BROADCAST_MODE_CMD 3

/* RT defines */
#define STATIC    0
#define IMMEDIATE 1

/* Control word opcode defines */
#define END_OF_LIST		0x0000  /* stop transmitting */
#define SKIP_WITH_DELAY		0x1000  
#define GOTO			0x2000
#define BUILT_IN_TEST		0x3000
#define EXEC_BLK_CONTINUE	0x4000
#define EXEC_BLK_BRANCH		0x5000
#define EXEC_BLK_BRANCH_ON_COND	0x6000
#define RETRY_ON_COND		0x7000
#define RETRY_ON_COND_BRANCH	0x8000
#define RETRY_ON_COND_BRANCH_ALL_FAIL	0x9000
#define INTERRUPT_CONTINUE	0xa000
#define CALL			0xb000
#define RETURN_TO_CALL		0xc000
#define LOAD_MINOR_FRAME_TIMER	0xe000
#define RETURN_TO_BRANCH	0xf000
#define INVALID_OPCODE_MASK	0x0fff

#define SENDMSG      0x4000 /* opcode for sending a message */
#define GOTO_MSG     0x2000 /* for infinite loops           */

/* Control word defines */
#define CW_BUS_A        0x0200 /* send message on bus A     */
#define CW_BUS_B        0x0000 /* send message on bus B     */
#define CW_RT2RT        0x0100 /* message is RT to RT       */
#define CW_NOT_RT2RT    0x0000 /* message is not RT to RT */
#define CW_RT_ERROR     0x0001 /* error in RT response      */

/* Condition Codes defines */
#define ALL_CCODES		0xFE
#define MESSAGE_ERROR		0x80
#define SWR_MESSAGE_ERROR	0x40
#define SWR_BUSY		0x20
#define SWR_TERMINAL_FLAG	0x10
#define SWR_SUBSYSFAIL		0x08
#define SWR_INSTRUM		0x04
#define SWR_SREQ		0x02
#define LOWEST_CCODE		0x02

/* Control Register */
#define START_EXECUTION 0x8000  /* STEX bit in Control register    */
#define START_BIT       0x4000  /* Start BIT                       */
#define RESET           0x2000  /* Reset Summit                 */
#define BUAEN           0x1000  /* Bus A enable                    */
#define BUBEN           0x0800  /* Bus B enable                    */
#define BMTC            0x0020  /* Bus Monitor Control             */
#define BROADCAST       0x0010  /* set to make RT31 Broadcast      */
#define NOBROADCAST     0x0     /* RT 31 is normal RT              */
#define PPEN		0x4	/* Ping Pong enable		   */

/* Status Register */
#define SUMMIT_READY    0x2 

/* Set Interrupt Mask */
#define MERR            0x0800  /* message error (bit, word, parity)	*/
#define SUBAD           0x0400  /* subaddress accessed			*/ 

/* Set Subaddress Interrupt */  
#define BROADCAST       0x0010  /* interrupt on Broadcast only		*/
#define ACCESS          0x0001  /* interrupt on any non_Broadcast access of the sa */
#define ACCESS_AND_BROADCAST	0x0002 /* interrupt on access and broadcast */
#define NO_SA_INTERRUPT	0x0000 /* Disable the interrupts */

/* Protocol */
#define MIL_STD_1553A           1
#define MIL_STD_1553B           2

/* BIT Word Registers bits */
#define DMAF   (1 << 15)
#define WRAPF  (1 << 14)
#define TAPF   (1 << 13)
#define BITF   (1 << 12)
#define BUAF   (1 << 11)
#define BUBF   (1 << 10)
#define HANGF  (1 << 0)    /* BIT failed to complete */
#define BIT_WORD_MASK  0xfc00
/* for backward compatibility */
#define CHAF   (1 << 11) 
#define CHBF   (1 << 10)

/* set_BC_Command */
#define TRANSMIT_BIT 0x400 
#define WORD_CNT_MASK 0x1f 


/* These were given new names (see above). Left here for backward compatibility.
If there is a conflict with Px defines, delete this section */
#define BC_MODE     0x0000
#define RT_MODE     0x0100
#define BM_MODE     0x0200
#define BM_RT_MODE  0x0300

#define STANDARD       0
#define BRDCAST        1
#define MODE           2
#define BROADCAST_MODE 3

#define SKIP         0x1000

#define BUS_A        0x0200
#define BUS_B        0x0000
#define RT2RT        0x0100
#define RT_ERROR     0x0001
#define NOT_RT2RT    0x0000
/* For compatibility with other products, delete until here */

#endif
