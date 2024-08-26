#include "pch.h"
#include "MEM_DUMP.h"


#include "converter.h"
#include "FILE_IO.h"

BOOLEAN query_MEM_DUMP(PMEM_DUMP_NODE Start_Addr, LPCSTR SAVE_PATH) {
	if (Start_Addr == NULL) return FALSE;

	PMEM_DUMP_NODE current = Start_Addr;

	SIZE_T node_index = 0;

	while (current != NULL) {


		CString info;
		info.Format(_T("[%llu] EXE 덤프된 동적주소-> %p 사이즈 %llu"), current->node_index, current->To_IOCTL_Requestor_Address, current->SIZE);
		AfxMessageBox(info);

		// SAVE_PATH에 node_index 추가
		CString modifiedPath(SAVE_PATH);
		modifiedPath.Format(_T("%s_%llu"), modifiedPath, node_index);

		LPCSTR complete_path = Convert___Wide__to__Ansi____(modifiedPath.GetString());

		File_Create(current->To_IOCTL_Requestor_Address, (ULONG32)current->SIZE, complete_path );

		free((PVOID)complete_path);

		node_index++;

		current = (PMEM_DUMP_NODE)current->Next_Addr;
	}

	
	return TRUE;
}
