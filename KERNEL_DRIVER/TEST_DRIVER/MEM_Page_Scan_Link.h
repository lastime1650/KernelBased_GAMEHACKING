#ifndef MEM_PAGE_SCAN_H
#define MEM_PAGE_SCAN_H

#include <ntifs.h>

NTSTATUS NTAPI MmCopyVirtualMemory
(
	PEPROCESS SourceProcess,
	PVOID SourceAddress,
	PEPROCESS TargetProcess,
	PVOID TargetAddress,
	SIZE_T BufferSize,
	KPROCESSOR_MODE PreviousMode,
	PSIZE_T ReturnSize
);

typedef struct PAGE_SCAN_NODE {

	
	// MEMORY_BASIC_INFORMATION
	PVOID BaseAddress;
	PVOID AllocationBase;
	ULONG AllocationProtect;
	SIZE_T RegionSize;
	ULONG32 State;
	ULONG32 Protect;
	ULONG32 Type;

	PUCHAR Next_Addr;

}PAGE_SCAN_NODE, *PPAGE_SCAN_NODE;

PPAGE_SCAN_NODE Create__MEM_PAGE_NODE(
	MEMORY_BASIC_INFORMATION* Input_MEM_info, HANDLE IOCTL_requestor_HANDLE, PEPROCESS IOCTL_requestor_eprocess, PEPROCESS SYSTEM_eprocess);


PPAGE_SCAN_NODE Append__MEM_PAGE_NODE(
	PPAGE_SCAN_NODE Current, MEMORY_BASIC_INFORMATION* Input_MEM_info, HANDLE IOCTL_requestor_HANDLE, PEPROCESS IOCTL_requestor_eprocess,  PEPROCESS SYSTEM_eprocess);



#endif