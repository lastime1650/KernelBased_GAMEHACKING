#ifndef MEM_HACK_private_link_list_h
#define MEM_HACK_private_link_list_h

#include <ntifs.h>
typedef struct ALL_SCANNED_MEM_DATAS {

	PUCHAR Previous_Addr;

	ULONG32 PAGE_Area; // PAGE_READWRITE.. etc

	PUCHAR TargetProcess_Start_Address;
	SIZE_T Region_Size;

	PUCHAR Next_Addr;

}ALL_SCANNED_MEM_DATAS, * PALL_SCANNED_MEM_DATAS;

PALL_SCANNED_MEM_DATAS Create__ALL_SCANNED_MEM_DATA__Node(PALL_SCANNED_MEM_DATAS Previous_Addr, ULONG32 PAGE_Area, PUCHAR TargetProcess_Start_Address, SIZE_T Region_Size);

PALL_SCANNED_MEM_DATAS Append__ALL_SCANNED_MEM_DATA__Node(PALL_SCANNED_MEM_DATAS Current_Addr, ULONG32 PAGE_Area, PUCHAR TargetProcess_Start_Address, SIZE_T Region_Size);

BOOLEAN RemoveALL__ALL_SCANNED_MEM_DATA__Node(PALL_SCANNED_MEM_DATAS Start_Addr);

#endif // !MEM_HACK_private_link_list_h
