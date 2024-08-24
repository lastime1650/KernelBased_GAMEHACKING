#include "MEM_DUMP.h"

#include "FILE_IO.h"

BOOLEAN query_MEM_DUMP(PMEM_DUMP_NODE Start_Addr) {
	if (Start_Addr == NULL) return FALSE;

	PMEM_DUMP_NODE current = Start_Addr;

	SIZE_T node_index = 0;

	while (current != NULL) {

		printf(" [%llu] EXE 덤프된 동적주소-> %p 사이즈 %llu  \n", current->node_index,current->To_IOCTL_Requestor_Address, current->SIZE);

		CHAR savepath[256];
		snprintf(savepath, sizeof(savepath), "C:\\sample_%llu_.bin", current->node_index);

		File_Create(current->To_IOCTL_Requestor_Address, (ULONG32)current->SIZE, savepath);

		current = (PMEM_DUMP_NODE)current->Next_Addr;
	}


	return TRUE;
}