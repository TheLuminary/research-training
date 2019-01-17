#ifndef __MCH_MEM_H
#define __MCH_MEM_H


struct exc_bd                      /* structure for Summit registers */
 {
  unsigned short  volatile control;           /* 0  Control Register                    */
  unsigned short  status;            /* 1  Operational Status Register         */
  unsigned short  current_cmd_block; /* 2  Current Command (Block)             */
  unsigned short  int_mask;          /* 3  interrupt mask register             */
  unsigned short  int_pending;       /* 4  pending interrupt register          */
  unsigned short  intr_log_list;     /* 5  Interrupt Log List Pointer Register */
  unsigned short  bit_word;          /* 6  BIT word                            */
                                   /*    MODE DEPENDENT REGISTER             */
  unsigned short  timetag_minor;     /* 7  RT/MON timtag / BC Minor Frame timer*/
  unsigned short  pointer;           /* 8  SRT Descriptor PTR /BC Cmd Block ptr*/
                                   /*    RT MODE ONLY                        */
  unsigned short  status1553;        /* 9  RT 1553 Status Word Bits            */
                                   /*    BC MODE ONLY                        */
  unsigned short  BC_initcount;      /* 10 BC Cmd Block Initialization Count   */
                                   /*    MONITOR MODE ONLY                   */
  unsigned short  cmd_blk_ptr;       /* 11 Initial Monitor Command Block Ptr   */
  unsigned short  data_ptr;          /* 12 Initial Monitor Data Pointer        */
  unsigned short  blk_counter;       /* 13 Monitor Block Count                 */
  unsigned short  filter1;           /* 14 Monitor Filter                      */
  unsigned short  filter2;           /* 15 Monitor Filter                      */
                                   /*    RT MODE ONLY                        */
  unsigned short  illegal[16];       /* 16 RT illegalization registers         */
 };                              /* COMMON MEMORY SPACE                    */

// global to help force a full WORD write for the structure above
// use when anding or or-ing to the structure; in these cases a compiler
// optimization would often turn the word write into a byte operation. 
// Needed for PCMCIA, but can't hurt in MCH
volatile usint wForceWordWrite;


struct bm_command_block           /* structure of one stack entry */
 {
  unsigned short  msginfo;    /* Message Information Word */
  unsigned short  command1;
  unsigned short  command2;
  unsigned short  data_ptr;
  unsigned short  status1;
  unsigned short  status2;
  unsigned short  timetag;
  unsigned short  reserved;
 };

struct bc_command_block           /* structure of one BC cmd entry */
 {
  unsigned short  control;    /* Control Word */
  unsigned short  command1;
  unsigned short  command2;
  unsigned short  data_ptr;
  unsigned short  status1;
  unsigned short  status2;
  unsigned short  branch;
  unsigned short  timer;
 };

#define  Start_DPRAM    0x40/2                          /* word offset */
//#define  BANKSIZE      0x8000                       /* in bytes    */
#define  BANKSIZE       0x7ffd                       /* in bytes    */

/* Memory Partitions */
#define INIT_RT_BUFFER    (0x4880/2)      /* location of first RT Buffer        */
#define INIT_RT_DSCRPT    (0x7b80/2)      /* location of first Descriptor Block */

#define INIT_DATA_BLK   Start_DPRAM       /* location of first Monitor Data */

#define MAXMESSAGES     (((INIT_RT_BUFFER - INIT_DATA_BLK)/(32 + 8)) - 1)
#define INIT_MON_BLK    (((MAXMESSAGES + 1) * 32) + INIT_DATA_BLK)    /* location of first Monitor Block */

#define CMDOFFSET     (0x6000)                          /* command block start            */
#define MAXCMDS       ((BANKSIZE - CMDOFFSET) / 8)     /* max num of 8 word commands     */
#define MAXDATABLKS   ((CMDOFFSET - Start_DPRAM) / 32)  /* max num of 32 word data blocks */


    /* Add buffers for ping pong double buffering. reduce mode code
       buffer size to make room for the new buffers
    */
#define TRANSMIT_SECTION 96
#define PINGPONG     4	/* bit in control word indicating current buffer */
#define MODEBUFSIZE  3	/* control + timetag + data word		 */
#define BUFSIZE  34	/* memory is allocated	    subaddr 0 receive A  */
#define MODEAREA 160*34 /* as follows:		    subaddr 0 receive B  */
			/*			    subaddr 0 broadcast  */
			/*			    subaddr 1 receive A  */
			/* each buffer is 34	    subaddr 1 receive B  */
			/* words long to allow		   .		 */
			/* for a time tag word		   .		 */
			/* a status word and	    subaddr 31 receive B */
			/* 32 data words	    subaddr 31 broadcast */
			/* (mode codes get 1	    subaddr 0  transmitA */
			/*  data word)		    subaddr 1  transmitB */
			/*				  .		 */
			/*				  .		 */
			/*			    subaddr 31 transmitA */
			/*			    subaddr 31 transmitB */
			/*			    mode    0  receive A */
			/*			    mode    0  receive B */
			/*			    mode    0  broadcast */
			/*			    mode    1  receive A */
			/*			    mode    1  receive B */
			/*			    mode    1  broadcast */
			/*				  .		 */
			/*				  .		 */
			/*			    mode   31 receive A  */
			/*			    mode   31 receive B  */
			/*			    mode   31 broadcast  */
			/*			    mode   0  transmit A */
			/*			    mode   0  transmit B */
			/*				  .		 */
			/*				  .		 */
			/*			    mode   31 transmit A */
			/*			    mode   31 transmit B */

#define STANDARD       0
#define BRDCAST        1
#define MODE           2
#define BROADCAST_MODE 3


/* BC mode defines */
#define DATABLKSIZE   32                                /* 32 words per data block        */
#define CMDENTRYSIZE  8                                 /* 8  words per command entry     */


/* Misc defines */
#define  Clear_Pattern  0  /* 0 is used because in BC mode EOL is code 0 */

#endif
