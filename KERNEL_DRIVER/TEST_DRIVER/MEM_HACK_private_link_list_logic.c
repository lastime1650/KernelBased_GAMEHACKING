#include "MEM_HACK_private_link_list.h"

PALL_SCANNED_MEM_DATAS Create__ALL_SCANNED_MEM_DATA__Node(PALL_SCANNED_MEM_DATAS Previous_Addr, ULONG32 PAGE_Area, PUCHAR TargetProcess_Start_Address, SIZE_T Region_Size) {

	PALL_SCANNED_MEM_DATAS NewNode = ExAllocatePoolWithTag(NonPagedPool, sizeof(ALL_SCANNED_MEM_DATAS), 'ALSC'); // ALL_SCAN
	if (NewNode == NULL) return NULL;

	NewNode->Previous_Addr = (PUCHAR)Previous_Addr;

	NewNode->PAGE_Area = PAGE_Area;

	NewNode->TargetProcess_Start_Address = TargetProcess_Start_Address;
	NewNode->Region_Size = Region_Size;

	NewNode->Next_Addr = NULL;

	return NewNode;
}

PALL_SCANNED_MEM_DATAS Append__ALL_SCANNED_MEM_DATA__Node(PALL_SCANNED_MEM_DATAS Current_Addr, ULONG32 PAGE_Area, PUCHAR TargetProcess_Start_Address, SIZE_T Region_Size) {
	if (Current_Addr == NULL) return NULL;

	Current_Addr->Next_Addr = (PUCHAR)Create__ALL_SCANNED_MEM_DATA__Node(Current_Addr, PAGE_Area, TargetProcess_Start_Address, Region_Size);

	if (Current_Addr->Next_Addr == NULL) {
		return NULL;
	}

	return (PALL_SCANNED_MEM_DATAS)Current_Addr->Next_Addr;
}

BOOLEAN RemoveALL__ALL_SCANNED_MEM_DATA__Node(PALL_SCANNED_MEM_DATAS Start_Addr) {
	if (Start_Addr == NULL) return FALSE;

	PALL_SCANNED_MEM_DATAS current = Start_Addr;

	while (current != NULL) {

		PALL_SCANNED_MEM_DATAS remember = (PALL_SCANNED_MEM_DATAS)current->Next_Addr;

		ExFreePoolWithTag(current, 'ALSC');

		current = remember;
	}

	return TRUE;

}