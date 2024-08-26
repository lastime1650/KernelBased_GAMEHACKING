#include "MEM_DUMP_Link.h"
#include "IOCTL.h"

PMEM_DUMP_NODE Create__MEM_DUMP_NODE(PMEM_DUMP_NODE Previous, PUCHAR Insert_targetprocess_address, SIZE_T Insert_targetprocess_result_regionsize, HANDLE IOCTL_requestor_HANDLE, PEPROCESS IOCTL_requestor_eprocess, PEPROCESS target_process_eprocess, PEPROCESS SYSTEM_eprocess, SIZE_T node_index) {

	/*
		[����]
			1. ���Ḯ��Ʈ �� �����ּ� �Ҵ�
			2. To_IOCTL_Requestor_Address �ʵ� �� �����ּ� �Ҵ��� 

			�� 2��!
	*/
	SIZE_T output = 0;

	// 1ȸ �����ּ��Ҵ�
	PVOID BaseAddress = NULL;
	SIZE_T node_SIZE = sizeof(MEM_DUMP_NODE);
	if (ZwAllocateVirtualMemory(IOCTL_requestor_HANDLE, &BaseAddress, 0, &node_SIZE, MEM_COMMIT, PAGE_READWRITE) != STATUS_SUCCESS) {
		return NULL;
	}

	PMEM_DUMP_NODE tmp = ExAllocatePoolWithTag(PagedPool, sizeof(MEM_DUMP_NODE), 'SCAN');

	tmp->Previous_Addr = (PUCHAR)Previous;
	tmp->node_index = node_index; // ��� ����


	// ������ 2ȸ �����ּ��Ҵ� 
	PVOID BaseAddress2 = NULL;// exe �ּ�
	
	if (ZwAllocateVirtualMemory(IOCTL_requestor_HANDLE, &BaseAddress2, 0, &Insert_targetprocess_result_regionsize, MEM_COMMIT, PAGE_READWRITE) != STATUS_SUCCESS) {
		ExFreePoolWithTag(tmp, 'SCAN');
		return NULL;
	}
	MmCopyVirtualMemory(target_process_eprocess, Insert_targetprocess_address, IOCTL_requestor_eprocess, BaseAddress2, Insert_targetprocess_result_regionsize, KernelMode, &output); // ���� EXE������ IOCTL ��û�� ���μ����� ���� target -> ioctl_requestor

	tmp->To_IOCTL_Requestor_Address = BaseAddress2;
	tmp->SIZE = Insert_targetprocess_result_regionsize;


	//////////

	tmp->Next_Addr = NULL;


	
	MmCopyVirtualMemory(SYSTEM_eprocess, tmp, IOCTL_requestor_eprocess, BaseAddress, node_SIZE, KernelMode, &output);

	ExFreePoolWithTag(tmp, 'SCAN');

	return (PMEM_DUMP_NODE)BaseAddress;

}

PMEM_DUMP_NODE Append__MEM_DUMP_NODE(PMEM_DUMP_NODE Current, PUCHAR Insert_targetprocess_address, SIZE_T Insert_targetprocess_result_regionsize, HANDLE IOCTL_requestor_HANDLE, PEPROCESS IOCTL_requestor_eprocess, PEPROCESS target_process_eprocess, PEPROCESS SYSTEM_eprocess, SIZE_T node_index) {


	PMEM_DUMP_NODE newnode = Create__MEM_DUMP_NODE(Current, Insert_targetprocess_address, Insert_targetprocess_result_regionsize, IOCTL_requestor_HANDLE, IOCTL_requestor_eprocess, target_process_eprocess, SYSTEM_eprocess, node_index);
	if (newnode == NULL) return NULL;

	KAPC_STATE STAT = { 0, };
	KeStackAttachProcess(IOCTL_requestor_eprocess, &STAT);
	Current->Next_Addr = (PUCHAR)newnode;
	KeUnstackDetachProcess(&STAT);

	return newnode;
}
