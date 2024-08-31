#ifndef MEM__VIEWER
#define MEM__VIEWER

#include <ntifs.h>

#include "IOCTL.h"
#include "MEM_HACK.h"



BOOLEAN MemViewer_Processing( PIOCTL_info Input_IOCTL,  PMEMHACK_needs input_process_info, PEPROCESS SYSTEM_eprocess);

#endif