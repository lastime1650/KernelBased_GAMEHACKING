#ifndef KERNEL_BASED_INJECT
#define KERNEL_BASED_INJECT

#include <ntifs.h>
#include "MEM_HACK.h"
#include "IOCTL.h"

// Ŀ�ο��� ���� ���μ��� ������ ����
NTSTATUS NTAPI RtlCreateUserThread(
    HANDLE ProcessHandle,
    PSECURITY_DESCRIPTOR SecurityDescriptor,
    BOOLEAN CreateSuspended,
    ULONG StackZeroBits,
    SIZE_T StackReserve,
    SIZE_T StackCommit,
    PVOID StartAddress,
    PVOID Parameter,
    PHANDLE ThreadHandle,
    PCLIENT_ID ClientId
);

BOOLEAN KERNNEL_based_Dll_inject(PMEMHACK_needs input_process_info, PIOCTL_info inout_IOCTL );


#endif // !KERNEL_BASED_INJECT
