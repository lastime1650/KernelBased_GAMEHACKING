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

/* IOCTL ��û�ڰ� ��û�� �ϵ���� �극��ũ ����Ʈ �ɱ� */
BOOLEAN Set_HardWare_Breakpoint( PIOCTL_info inout_Ioctl,PMEMHACK_needs process_info);

/* IOCTL ��û�ڰ� ��û�� �ϵ���� �극��ũ ����Ʈ ������ ��� ���� ( 1���� ���� )  */
BOOLEAN Get_HardWare_Breakpoint(PIOCTL_info inout_Ioctl, PMEMHACK_needs process_info, PEPROCESS SYSTEM_eprocess);



#endif