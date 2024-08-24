#ifndef  PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <ntifs.h>

//#include "API_LOADER.h"
#include "MEM_HACK.h"
#include "IOCTL.h"
BOOLEAN PID_TO_HANDLE(HANDLE* OUTPUT_HANDLE, HANDLE INPUT_PID);


BOOLEAN QUERY_Module_Buffer(PMEMHACK_needs NEEDS, PEPROCESS SYSTEM_eprocess, PIOCTL_info* Inout_IOCTL);

#endif // ! PROCESS_MANAGER_H
