#include <stdio.h>
#include <conio.h>
#include "proto_mch.h"
int handle;

#define Print_n_Exit( title, error )      \
{                                         \
  if (error != 0)                         \
      {                                   \
      printf("%s: %s\n", title, Print_Error_MCH(error));         \
      Release_Module_MCH(handle);                     \
      getch();                            \
      exit(0);                            \
      }                                   \
}


void main (void)
{
       WORD rtdatablk[4096] = {0};
       WORD datainblk[34];
       WORD * rtdata = rtdatablk;
       WORD * datain = datainblk;

       int error, i, module, address;
       usint device;

    printf ("Excalibur RT test for PCMCIA in Multibuffer (buffer2) mode version 1.0\n");

    printf("Enter the device number of the card you wish to use.\n");
    scanf("%d", &device);
        printf("Enter the module number\n");
    scanf("%d", &module);
    printf("Calling Init_Card_Mch with device %d module %d\n\n", device, module);
    error = Init_Module_MCH(device, module);
    if (error != 0)
	{
    		printf("Init_Card_MCH Failure. %s\n", Print_Error_MCH(error));
      		getch();
      		exit(0);
    	}
    handle = error;
    error = Set_Mode_MCH(handle, RT_MODE_MCH);
    Print_n_Exit( "RT_MODE_MCH FAILURE:  ", error );
    error = Set_RT_Num_MCH(handle, 1);
    Print_n_Exit( "Setup_RT FAILURE:  ", error );
    error = Enable_Multibuffer_MCH(handle);
    Print_n_Exit("Enable_Multibuffer FAILURE:  ", error);
    address = 128 * 32;
    error = Setup_Multibuffer_MCH(handle,5, 128, address);
    Print_n_Exit( "Setup_Multibuffer FAILURE:  ", error );
    for (i=0; i < 4096; i++) {
		 rtdatablk[i] = i;
	 }

    Load_Multibuffer_Data_MCH(handle,4096, address, rtdatablk);
    Print_n_Exit( "Load_Multibuffer_Data FAILURE:  ", error );
    error = Load_Datablk_MCH(handle,4, 0, rtdatablk);
    Print_n_Exit( "Load_Datablk FAILURE:  ", error );

    error = Run_RT_MCH(handle);
    Print_n_Exit( "Run_RT FAILURE:  ", error );

    printf("The card has now been set up to receive messages to RT #1.\n");
    printf("Set up your Bus Controller to send out transmit messages\n");
    printf("to RT1 sa 5. Check that each message gets different data.\n");
    printf("The data ranges from 0000, 0001 ... 0ffe, 0fff.\n");
   printf("Setup sa 3 as BC->RT with data 4444 4444 4444\n");
    printf("Setup sa 4 as RT->BC with 3 data words\n");
    printf("Send a sync with data mode command\n");
 
    printf("Hit any key to exit\n");
    getch();
	 error = Read_Datablk_MCH(handle,3, 0, datainblk, STANDARD_CMD, RECEIVE);
    Print_n_Exit( "Read_Datablk FAILURE:  ", error );
    printf("sa3:msginfo = %x, ttag = %x, data1 = %x ,data2 = %x ,data3 = %x\n",
            datainblk[0],datainblk[1],datainblk[2],datainblk[3],datainblk[4]);
	 error = Read_Datablk_MCH(handle,MODECODE, 17, datainblk, MODE_CMD, RECEIVE);
    Print_n_Exit( "Read_Datablk FAILURE:  ", error );
    printf("sync with data: msginfo = %x, ttag = %x, data1 = %x ,data2 = %x ,data3 = %x\n",
            datainblk[0],datainblk[1],datainblk[2]);
  
       /* halt RT */
    error = Stop_Card_MCH(handle);
       /* release PCMCIA resources */
    error = Disable_Multibuffer_MCH(handle);
    Print_n_Exit("Disable_Multibuf FAILURE: ", error);
    Release_Module_MCH(handle);
    printf("Test Complete\n");
    getch();
}


