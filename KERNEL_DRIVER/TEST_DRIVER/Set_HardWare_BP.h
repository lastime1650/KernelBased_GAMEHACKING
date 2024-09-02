#ifndef SET_HW_BP_
#define SET_HW_BP_

#include <ntifs.h>
#include "IOCTL.h"
#include "MEM_HACK.h"
#include "API_LOADER.h"

NTKERNELAPI
NTSTATUS
NTAPI
PsGetContextThread(
    __in PETHREAD Thread,
    __inout PCONTEXT ThreadContext,
    __in KPROCESSOR_MODE PreviousMode
);

NTKERNELAPI
NTSTATUS
NTAPI
PsSetContextThread(
    __in PETHREAD Thread,
    __in PCONTEXT ThreadContext,
    __in KPROCESSOR_MODE PreviousMode
);

/* IOCTL 요청자가 요청한 하드웨어 브레이크 포인트 걸기 */
BOOLEAN Set_HardWare_Breakpoint( PIOCTL_info inout_Ioctl,PMEMHACK_needs process_info);

/* IOCTL 요청자가 요청한 하드웨어 브레이크 포인트 설정한 노드 추출 ( 1개씩 추출 )  */
BOOLEAN Get_HardWare_Breakpoint(PIOCTL_info inout_Ioctl, PMEMHACK_needs process_info, PEPROCESS SYSTEM_eprocess);



#endif