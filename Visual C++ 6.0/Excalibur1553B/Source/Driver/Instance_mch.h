#ifndef __MCH_INSTANCE_H
#define __MCH_INSTANCE_H

/* for backward compatibility */
extern int g_handlemch;

typedef struct
{
	usint card_addr;  /* devnum used in init_module, needed for kernel functions*/
	usint module;
	BOOL allocated;     /* mark array element in use; check when doing close_kernel */
	usint *exc_summit_int;
	struct exc_bd *exc_summit;
	struct bc_command_block *bc_command_blocks; /* BC command blocks */
	struct bm_command_block *bm_command_blocks; /* monitor command blocks */
	usint milstd1553B; /* if 0, its milstd1553A */
  	usint curmode;
	usint curmsg;
  	usint nextmsg;
	usint broadcast;
	usint pingpong;
	int intnum;
	int mMultiBuf;
} INSTANCE_MCH;

#endif