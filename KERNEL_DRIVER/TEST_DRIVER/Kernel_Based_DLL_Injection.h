#ifndef KERNEL_BASED_INJECT
#define KERNEL_BASED_INJECT

#include <ntifs.h>
#include "MEM_HACK.h"
#include "IOCTL.h"

// 커널에서 유저 프로세스 스레드 실행
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
