#ifndef __MCH_ERROR_H
#define __MCH_ERROR_H

/* error codes for the Excalibur MCH Cards */
#define einval                  -2  /* illegal value used as input */
#define emode                   -3  /* Mode specific command called when in the wrong mode */
#define sim_no_mem             	-5
#define enomsg			-18 /* no new messages available */
#define etimeoutreset		-26
#define ewrngmodule		-27
#define enomodule		-28
#define ebadhandle 		-33
#define eboardtoomany	        -36
#define noirqset		-53

#define ebadrtid    		-300 /* undefined rt id used as input */
#define ebadchannel		-301 /* tried to set channel to illegal value */
#define ememerr			-302 /* memory test failed */
#define incompatible_parameters	-303 /* Incompatible parameters */
#define ert2rtcmd		-304 /* error in format of RT to RT command */

#endif
