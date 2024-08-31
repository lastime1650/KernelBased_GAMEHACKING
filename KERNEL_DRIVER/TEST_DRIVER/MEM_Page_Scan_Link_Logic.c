#include "MEM_Page_Scan_Link.h"





PPAGE_SCAN_NODE Create__MEM_PAGE_NODE(
	MEMORY_BASIC_INFORMATION* Input_MEM_info, HANDLE IOCTL_requestor_HANDLE, PEPROCESS IOCTL_requestor_eprocess, PEPROCESS SYSTEM_eprocess) {

	/*
	    PVOID BaseAddress;
		PVOID AllocationBase;
		ULONG AllocationProtect;

		SIZE_T RegionSize;
		ULONG State;
		ULONG Protect;
		ULONG Type;
	
	*/
	SIZE_T output = 0;

	// 1회 가상주소할당
	PVOID BaseAddress = NULL;
	SIZE_T node_SIZE = sizeof(PAGE_SCAN_NODE);
	if (ZwAllocateVirtualMemory(IOCTL_requestor_HANDLE, &BaseAddress, 0, &node_SIZE, MEM_COMMIT, PAGE_READWRITE) != STATUS_SUCCESS) {
		return NULL;
	}

	PPAGE_SCAN_NODE tmp = ExAllocatePoolWithTag(NonPagedPool, sizeof(PAGE_SCAN_NODE), 'SCA3');
	memset(tmp, 0, sizeof(PAGE_SCAN_NODE));

	
	tmp->BaseAddress = Input_MEM_info->BaseAddress;
	tmp->AllocationBase = Input_MEM_info->AllocationBase;
	tmp->AllocationProtect = Input_MEM_info->AllocationProtect;

	tmp->RegionSize = Input_MEM_info->RegionSize;
	tmp->State = Input_MEM_info->State;
	tmp->Protect = Input_MEM_info->Protect;
	tmp->Type = Input_MEM_info->Type;

	tmp->Next_Addr = NULL;


	MmCopyVirtualMemory(SYSTEM_eprocess, tmp, IOCTL_requestor_eprocess, BaseAddress, sizeof(PAGE_SCAN_NODE), KernelMode, &output);

	ExFreePoolWithTag(tmp, 'SCA3');

	return (PPAGE_SCAN_NODE)BaseAddress;


}


PPAGE_SCAN_NODE Append__MEM_PAGE_NODE(
	PPAGE_SCAN_NODE Current, MEMORY_BASIC_INFORMATION* Input_MEM_info, HANDLE IOCTL_requestor_HANDLE, PEPROCESS IOCTL_requestor_eprocess, PEPROCESS SYSTEM_eprocess) {

	PPAGE_SCAN_NODE result = Create__MEM_PAGE_NODE(Input_MEM_info, IOCTL_requestor_HANDLE, IOCTL_requestor_eprocess,  SYSTEM_eprocess);
	if (result == NULL) return NULL;


	KAPC_STATE STAT = { 0, };
	KeStackAttachProcess(IOCTL_requestor_eprocess, &STAT);
	Current->Next_Addr = (PUCHAR)result;
	KeUnstackDetachProcess(&STAT);
	
	return result;
}

