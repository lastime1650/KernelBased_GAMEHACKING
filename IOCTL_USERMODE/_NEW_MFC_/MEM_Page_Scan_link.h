#ifndef MEM_PAGE_SCAN
#define MEM_PAGE_SCAN

#include "afxdialogex.h"
#include <Windows.h>
#include <winternl.h> // NTSTATUS 및 NtQuerySystemInformation을 위한 헤더
#include <ntstatus.h>

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

}PAGE_SCAN_NODE, * PPAGE_SCAN_NODE;

#ifdef __cplusplus
extern "C" {
#endif

	

#ifdef __cplusplus
}
#endif



#endif