#include "MEM_HACK_LINK_LIST.h"



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

PLinked_list_from_searching Create__Linked_list_for_searching(PLinked_list_from_searching previous_addr, PUCHAR Searched__target_process_virtual_memory, SIZE_T Searched_memory_data_size, BOOLEAN is_this_Pointer, HANDLE IOCTL_requestor_HANDLE, PEPROCESS IOCTL_requestor_eprocess, PEPROCESS SYSTEM_eprocess, SIZE_T node_index) {


	PVOID BaseAddress = NULL;
	SIZE_T SIZE = sizeof(Linked_list_from_searching) ;
	if (ZwAllocateVirtualMemory(IOCTL_requestor_HANDLE, &BaseAddress, 0, &SIZE, MEM_COMMIT, PAGE_READWRITE) != STATUS_SUCCESS) {
		return NULL;
	}

	PLinked_list_from_searching tmp = ExAllocatePoolWithTag(PagedPool, sizeof(Linked_list_from_searching), 'SCAN');
	
	tmp->previous_addr = (PUCHAR)previous_addr;
	tmp->node_index = node_index; // 노드 순번
	tmp->Searched__target_process_virtual_memory = Searched__target_process_virtual_memory;
	tmp->Searched_memory_data_size = Searched_memory_data_size;
	tmp->is_this_Pointer = is_this_Pointer;
	tmp->next_addr = NULL;


	SIZE_T output = 0;
	MmCopyVirtualMemory(SYSTEM_eprocess, tmp, IOCTL_requestor_eprocess, BaseAddress, SIZE, KernelMode, &output);

	ExFreePoolWithTag(tmp, 'SCAN');

	return (PLinked_list_from_searching)BaseAddress;
}

PLinked_list_from_searching Append__Linked_list_for_searching(PLinked_list_from_searching current_addr, PUCHAR Searched__target_process_virtual_memory, SIZE_T Searched_memory_data_size, BOOLEAN is_this_Pointer, HANDLE IOCTL_requestor_HANDLE, PEPROCESS IOCTL_requestor_eprocess, PEPROCESS SYSTEM_eprocess, SIZE_T node_index) {

	PLinked_list_from_searching new_node = Create__Linked_list_for_searching(current_addr, Searched__target_process_virtual_memory, Searched_memory_data_size, is_this_Pointer, IOCTL_requestor_HANDLE, IOCTL_requestor_eprocess, SYSTEM_eprocess, node_index);
	if (new_node == NULL) {
		return NULL;
	}

	KAPC_STATE STAT = { 0, };
	KeStackAttachProcess(IOCTL_requestor_eprocess, &STAT);
	current_addr->next_addr = (PUCHAR)new_node;
	KeUnstackDetachProcess(&STAT);

	return new_node;

}

PLinked_list_from_searching query_for_Refresh(PLinked_list_from_searching Start_Address, HANDLE IOCTL_requestor_HANDLE, PEPROCESS IOCTL_requestor_eprocess, PEPROCESS SYSTEM_eprocess) {
	if (Start_Address == NULL) return NULL;
	/*
		IOCTL에서 얻은 연결리스트를 조회하여, 그 주소만을 찾아 조회
	*/
	PLinked_list_from_searching current = Start_Address;

	PUCHAR Searched__target_process_virtual_memory = NULL;
	SIZE_T Searched_memory_data_size = 0;

	SIZE_T node_index = 0;

	PLinked_list_from_searching NEW_link_node_StartAddress = NULL;
	PLinked_list_from_searching NEW_link_node_CurrentAddress = NULL;

	while (current != NULL) {
		KAPC_STATE STAT = { 0, };
		KeStackAttachProcess(IOCTL_requestor_eprocess, &STAT);

		Searched__target_process_virtual_memory = current->Searched__target_process_virtual_memory;
		Searched_memory_data_size = current->Searched_memory_data_size;
		current = (PLinked_list_from_searching)current->next_addr;

		KeUnstackDetachProcess(&STAT);

		if (NEW_link_node_CurrentAddress == NULL) {
			NEW_link_node_StartAddress = Create__Linked_list_for_searching(NULL, Searched__target_process_virtual_memory, Searched_memory_data_size, FALSE, IOCTL_requestor_HANDLE, IOCTL_requestor_eprocess, SYSTEM_eprocess, node_index);
			NEW_link_node_CurrentAddress = NEW_link_node_StartAddress;
		}
		else {
			NEW_link_node_CurrentAddress = Append__Linked_list_for_searching(NEW_link_node_CurrentAddress, Searched__target_process_virtual_memory, Searched_memory_data_size, FALSE, IOCTL_requestor_HANDLE, IOCTL_requestor_eprocess, SYSTEM_eprocess, node_index);
		}
		node_index++;

	}


	return NEW_link_node_StartAddress;

}