#ifndef _DEVIO_ERROR_H
#define _DEVIO_ERROR_H

/* error codes for the Excalibur Card Kernel Drivers */

#define eopenkernel		-1001
#define ekernelcantmap	     	-1002
#define ereleventhandle		-1003
#define egetintcount		-1004
#define egetchintcount		-1005
#define egetintchannels		-1006
#define ewriteiobyte		-1007
#define ereadiobyte		-1008
#define egeteventhand1		-1009
#define egeteventhand2		-1010
#define eopenscmant		-1011
#define eopenservicet		-1012
#define estartservice		-1013
#define eopenscmanp		-1014
#define eopenservicep		-1015
#define econtrolservice		-1016
#define eunmapmem		-1017
#define egetirq			-1018
#define eallocresources		-1019
#define egetramsize		-1020
#define ekernelwriteattrib	-1021
#define ekernelreadattrib	-1022
#define ekernelfrontdesk	-1023	
#define ekernelOscheck		-1024
#define ekernelfrontdeskload	-1025
#define ekerneliswin2000compatible -1026
#define ekernelbankramsize 	-1027
#define ekernelgetcardtype	-1028
#define emodnum			-1029
#define regnotset		-1030
#define ekernelbankphysaddr	-1031
#define ekernelclosedevice	-1032
#define ekerneldevicenotopen	-1034
#define ekernelinitmodule	-1035
#define ekernelbadparam		-1036
#define ekernelbadpointer	-1037
#define ekerneltimeout		-1038
#define ekernelnotwin2000	-1039
#define erequestnotification	-1040
#define ekernelnot4000card	-1041

#define eclocksource		-1059
#define eparmglobalreset	-1062
#define etimernotrunning	-1063
#define etimerrunning		-1064
#define eparmreload		-1065
#define eparminterrupt		-1066

#define ebaddevhandle		-1067
#define edevtoomany		-1068
#endif
