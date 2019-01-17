#include "mchIncl.h"

/*
Get_Error_String_MCH
               
Description: Accepts the error returns from other Software Tools.
This routine returns a char string containing a corresponding error 
message, of max length errlen.

Input parameters:  errorcode   The error code returned from a function call
                   errorlen    
Output parameters: errorstring	A message string that contains a
                              	corresponding error message. In case of bad
                               	input this routine returns a string denoting
                               	that.
				Note that if the length of the string as specified
				by errorlen is less than the actual error msg length,
				only errorlen-1 characters will be returned, and the
				message will not be terminated with \n.
				

Return values:  0		Always

Use this routine as per the following example:

#define ERRORLEN 255
char ErrorStr[ERRORLEN];

Get_Error_String_MCH(errcode, ERRORLEN, ErrorStr);
printf("error is: %s", ErrorStr);
              
*/

int borland_dll Get_Error_String_MCH(int errcode, int errlen, char *errstring)
{
	char *localerr;
	int i;

	localerr = Print_Error_MCH(errcode);
	/* do this so that there will be null termination */
	for (i=0; i<errlen; i++)
		errstring[i] = '\0';

	strncpy(errstring, localerr, errlen-1);
	return 0;
}


/*
Print_Error     Coded 6-12-90 by D. Koppel
 
Obsolete. Retained for backward compatibility.
*/
borland_dll char * Print_Error_MCH (int errorcode)
{
   if ((int) errorcode >= 0)
      return ("Function Succeeded");

   switch (errorcode)
   {
	case einval: return("Illegal value used as input [einval]\n");
	case emode: return("Mode specific command called in the wrong mode [emode]\n");
	case sim_no_mem: return("Not enough memory For Simulation [sim_no_mem]\n");
	case enomsg: return("No new messages available [enomsg]\n");
	case etimeoutreset: return("Timed out waiting for reset [etimeoutreset]\n");
	case ewrngmodule: return ("Module specified on EXC-4000 board is not MCH Module [ewrngmodule]\n");
	case enomodule: return ("No EXC-4000 module present at specified location [enomodule]\n");
	case ebadhandle: return ("Invalid handle specified; should be value returned by Initialization function [ebadhandle]\n");
	case eboardtoomany: return ("Attempt to initialize too many modules or boards [eboardtoomany]\n");
	case noirqset: return("No interrupt allocated [noirqset]\n");

	case ebadrtid: return("Undefined RT id used as input [ebadrtid]\n");
	case ebadchannel: return("Tried to set channel to illegal value [ebadchannel]\n");
	case ememerr: return("Memory test failed [emerr]\n");
	case incompatible_parameters: return("Function parameters are not consistent [incompatible_parameters]\n");
	case ert2rtcmd: return ("Error in format of RT to RT command [ertrtcmd]\n");

case eopenkernel: return ("Cannot open kernel device; check ExcConfig settings [eopenkernel]\n");
case ekernelcantmap: return ("Kernel driver cannot map memory [ekernelcantmap]\n");
case ereleventhandle: return ("Error in kernel Release_Event_Handle [ereleventhandle]\n");
case egetintcount: return ("Error in kernel Get_Interrupt_Count [egetintcount]\n");
case egetchintcount: return ("Error in kernel Get_Channel_Interrupt_Count [egetchintcount]\n");
case egetintchannels: return ("Error in kernel Get_Interrupt_Channels [egetintchannels]\n");
case ewriteiobyte: return ("Error in kernel writeIOByte [ewriteiobyte]\n");
case ereadiobyte: return("Error in kernel ReadIoByte [ereadiobyte]\n");
case egeteventhand1: return("Error in kernel Get_Event_Handle, first part [egeteventhand1]\n");
case egeteventhand2: return("Error in kernel Get_Event_Handle, second part [egeteventhand2]\n");
case eopenscmant: return("Error in OpenSCManager in StartKernelDriver [eopenscmant]\n");
case eopenservicet: return("Error in OpenService in StartKernelDriver [eopenservicet]\n");
case estartservice: return("Error in StartService in StartKernelDriver [estartservice]\n");
case eopenscmanp: return("Error in OpenSCManager in StopKernelDriver [eopenscmanp]\n");
case eopenservicep: return("Error in OpenService in StopKernelDriver [eopenservicep]\n");
case econtrolservice: return("Error in ControlService in StopKernelDriver [econtrolservice]\n");
case eunmapmem: return("Error in kernel UnMapMemory [eunmapmem]\n");
case egetirq: return("Error in Get_IRQ_Number [egetirq]\n");
case eallocresources: return("Error allocating resources. see readme for details on resource allocation problems[eallocresources]\n");
case egetramsize: return ("Error in kernel GetRamSize [egetramsize]\n");
case ekernelwriteattrib: return ("Error in kernel write attrib [ekernelwriteattrib]\n");
case ekernelreadattrib: return ("Kernel readattrib error [ekernelreadattrib]\n");
case ekernelfrontdesk: return ("Kernel frontdesk error [ekernelfrontdesk]\n");
case ekernelOscheck: return ("Kernel Oscheck error [ekernelOscheck]\n");
case ekernelfrontdeskload: return ("Kernel frontdeskload error [ekernelfrontdeskload]\n");
case ekerneliswin2000compatible: return ("Kernel iswin2000compatible error [ekerneliswin2000compatible]\n");
case ekernelbankramsize: return("Kernel bankramsize error [ekernelbankramsize]\n");
case ekernelgetcardtype: return ("Kernel getcardtype error [ekernelgetcardtype]\n");	
case emodnum: return ("Invalid module number specified [emodnum]\n");
case regnotset: return("Card not configured. Reboot after ExcConfig is run and card is in slot [regnotset]\n");
case ekernelbankphysaddr: return ("Kernel GetBankPhysAddr error [ekernelbankphysaddr]\n");
case ekernelclosedevice: return ("Kernel CloseKernelDevice error [ekernelclosedevice]\n");
case ekerneldevicenotopen: return ("Kernel error: device not open [ekerneldevicenotopen]\n");
case ekernelinitmodule: return ("Kernel intialization error [ekernelinitmodule]\n");
case ekernelbadparam: return ("Kernel error: bad input parameter [ekernelbadparam]\n");
case ekernelbadpointer: return ("Kernel error: invalid pointer to output buffer [ekernelbadpointer]\n");
case ekerneltimeout: return ("Wait for Interrupt function returned with timeout [ekerneltimeout]\n");
case ekernelnotwin2000: return ("Operating System is not Windows 2000 [ekernelnotwin2000]\n");
	default: return ("No such error\n");
	}
}