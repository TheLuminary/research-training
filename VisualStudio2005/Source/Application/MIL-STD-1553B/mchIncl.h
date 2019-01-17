#ifndef _MCH_INCLUDE_H
#define _MCH_INCLUDE_H

/* 
This file is include file for the MCH dll code, not for applications.
In application programs use proto_mch instead of including each header file separately.
*/

#include <windows.h>
#include "excdef.h"
#ifndef VMEVXI
#include "deviceio.h"
#endif
#include "proto_mch.h"
//#include "proto_mch_back.h"
#include "instance_mch.h"
#include "mem_mch.h"

#endif
