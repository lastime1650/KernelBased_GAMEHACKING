#include "MEM_HACK.h"



#include "Process_manager.h" // ���μ��� ���� ��ƿ

BOOLEAN START_MEMHACK(PIOCTL_info* Inout__put_IOCTL_DATA) {
	if (Inout__put_IOCTL_DATA == NULL || *Inout__put_IOCTL_DATA == NULL) return FALSE;

	/* �ʿ����� ���� */
	HANDLE IOCTL_Requestor_PID = ((PIOCTL_info)*Inout__put_IOCTL_DATA)->Ioctl_requestor_PID;
	HANDLE IOCTL_Requestor_HANDLE = 0;
	if (PID_TO_HANDLE(&IOCTL_Requestor_HANDLE, IOCTL_Requestor_PID) == FALSE) return FALSE;
	PEPROCESS IOCTL_Requestor_eprocess = NULL;
	if (PsLookupProcessByProcessId(IOCTL_Requestor_PID, &IOCTL_Requestor_eprocess) != STATUS_SUCCESS) return FALSE;


	HANDLE target_process_PID = ((PIOCTL_info)*Inout__put_IOCTL_DATA)->target_process_info.target_PID;
	HANDLE target_process_HANDLE = 0;
	if (PID_TO_HANDLE(&target_process_HANDLE, target_process_PID) == FALSE) return FALSE;
	PEPROCESS target_process_eprocess = NULL;
	if (PsLookupProcessByProcessId(target_process_PID, &target_process_eprocess) != STATUS_SUCCESS) {
		ObDereferenceObject(IOCTL_Requestor_eprocess);
		return FALSE;
	}


	PEPROCESS SYSTEM_eprocess = NULL;
	if (PsLookupProcessByProcessId((HANDLE)4, &SYSTEM_eprocess) != STATUS_SUCCESS) {
		ObDereferenceObject(IOCTL_Requestor_eprocess);
		ObDereferenceObject(target_process_eprocess);
		return FALSE;
	}

	PUCHAR find_data = NULL;
	SIZE_T find_data_size = 0;

	if (((PIOCTL_info)*Inout__put_IOCTL_DATA)->ioctl_data_for_search != NULL || ((PIOCTL_info)*Inout__put_IOCTL_DATA)->ioctl_data_for_search_SIZE != 0) {
		// ��û���� �����͸� " MmCopyVirtualMemory " ���� �����ϰ� ������ 
		find_data_size = ((PIOCTL_info)*Inout__put_IOCTL_DATA)->ioctl_data_for_search_SIZE;
		find_data = ExAllocatePoolWithTag(PagedPool, find_data_size, 'SCAN');
		if (find_data == NULL) {
			ObDereferenceObject(IOCTL_Requestor_eprocess);
			ObDereferenceObject(target_process_eprocess);
			return FALSE;
		}


		SIZE_T output = 0;
		if (MmCopyVirtualMemory(IOCTL_Requestor_eprocess, ((PIOCTL_info)*Inout__put_IOCTL_DATA)->ioctl_data_for_search, SYSTEM_eprocess, find_data, find_data_size, KernelMode, &output) != STATUS_SUCCESS) {
			ObDereferenceObject(IOCTL_Requestor_eprocess);
			ObDereferenceObject(target_process_eprocess);
			return FALSE;
		}
	}

	




	//ULONG Scanning_PAGE_Area = ((PIOCTL_info)*Inout__put_IOCTL_DATA)->target_process_info.PAGE_Area_mode; 

	// ���� 
	MEMHACK_needs HACK_needs = {
		IOCTL_Requestor_PID,
		target_process_PID,

		IOCTL_Requestor_HANDLE,
		target_process_HANDLE,

		IOCTL_Requestor_eprocess,
		target_process_eprocess
	};

	

	switch (((PIOCTL_info)*Inout__put_IOCTL_DATA)->information) {
	case general_scan:
		/*
			�Ϲ�����_��ĵ
			
			�����͸� ������� ���ؽ�Ʈ �������� �޾Ƽ�, ó�� 
		
		*/
		NULL;

		if (((PIOCTL_info)*Inout__put_IOCTL_DATA)->known_data.known_target_address == NULL) {
			/*
			
				known_target_address �� NULL �̸�, NEW_SCAN

			*/
			/* Ÿ�� ���μ��� �޸� ��� ��ȸ */
			PALL_SCANNED_MEM_DATAS ALL_SCAN_LIST = ALL_SCAN(&HACK_needs);
			if (ALL_SCAN_LIST == NULL) {
				ObDereferenceObject(IOCTL_Requestor_eprocess);
				ObDereferenceObject(target_process_eprocess);
				return FALSE;
			}

			NEW_SCAN(&HACK_needs, ALL_SCAN_LIST, Inout__put_IOCTL_DATA, find_data, find_data_size, SYSTEM_eprocess);
		}
		else {
			/*
				known_target_address �� �����ϸ�, �� �ּҸ� �ٷ� ���ϸ��.
			*/
			NEXT_SCAN(&HACK_needs, Inout__put_IOCTL_DATA, find_data, find_data_size, SYSTEM_eprocess);
		}
		

		break;
	case refresh:
		// �̶� ������ " PLinked_list_from_searching " ���� �־����  ( ���� �� ���� IOCTL ��û�� �����ּҿ��� ��ȿ��  ) 
		NULL;
		((PIOCTL_info)*Inout__put_IOCTL_DATA)->Start_Node_Address = query_for_Refresh(((PIOCTL_info)*Inout__put_IOCTL_DATA)->refresh_info.From_Ioctl_requestor, HACK_needs.IOCTL_requstor_HANDLE, HACK_needs.IOCTL_eprocess, SYSTEM_eprocess);
		break;
	case editing:
		NULL;
		Editing_MEM_to_targetprocess(&HACK_needs, Inout__put_IOCTL_DATA, SYSTEM_eprocess);
		break;
	case dump:
		NULL;
		QUERY_Module_Buffer(&HACK_needs, SYSTEM_eprocess, Inout__put_IOCTL_DATA);
		break;
	default:
		ObDereferenceObject(IOCTL_Requestor_eprocess);
		ObDereferenceObject(target_process_eprocess);
		return FALSE;
	}


	/* ���� */
	ObDereferenceObject(IOCTL_Requestor_eprocess);
	ObDereferenceObject(target_process_eprocess);

	return TRUE;

}



PALL_SCANNED_MEM_DATAS ALL_SCAN(PMEMHACK_needs NEEDS) {

	PALL_SCANNED_MEM_DATAS Start_addr = NULL;
	PALL_SCANNED_MEM_DATAS Curent_addr = Start_addr;

	PVOID baseAddress = NULL;
	MEMORY_BASIC_INFORMATION memoryInfo = { 0 };

	//(1) Ÿ�� ���μ����� ���� ���� ( ���� �޸� �ּ�, �޸� ��ȣ ���, ����ũ�� ) 



	while (NT_SUCCESS(ZwQueryVirtualMemory(NEEDS->target_process_HANDLE, baseAddress, MemoryBasicInformation, &memoryInfo, sizeof(memoryInfo), NULL))) {

		baseAddress = (PVOID)((SIZE_T)memoryInfo.BaseAddress + memoryInfo.RegionSize);
		//(2-1) �޸� ��ȣ ������ PAGE_READWRITE �Ǵ� PAGE_EXECUTE_READWRITE �ΰ�?
		if (memoryInfo.Protect == PAGE_READWRITE && memoryInfo.State == MEM_COMMIT) {
			//(2-2) ���� ���� �ּҿ� ���� �ּ� ( ����+��ü ����ũ�� ) ����
			ULONG_PTR  start_addr = (ULONG_PTR)memoryInfo.BaseAddress;
			//ULONG_PTR  last_addr = (ULONG_PTR)memoryInfo.BaseAddress + (ULONG_PTR)memoryInfo.RegionSize;

			if (Curent_addr == NULL) {
				Start_addr = Create__ALL_SCANNED_MEM_DATA__Node(NULL, PAGE_READWRITE, (PUCHAR)start_addr, memoryInfo.RegionSize);
				Curent_addr = Start_addr;
			}
			else {
				Curent_addr = Append__ALL_SCANNED_MEM_DATA__Node(Curent_addr, PAGE_READWRITE, (PUCHAR)memoryInfo.BaseAddress, memoryInfo.RegionSize);
			}

		}

		if ((PUCHAR)((PUCHAR)memoryInfo.BaseAddress + memoryInfo.RegionSize) < (PUCHAR)baseAddress) break;

	}


	return Start_addr;
}

//
BOOLEAN NEW_SCAN(PMEMHACK_needs NEEDS, PALL_SCANNED_MEM_DATAS ALL_SCAN_LIST, PIOCTL_info* Inout__put_IOCTL_DATA,    PUCHAR Insert_data_for_search, SIZE_T Insert_data_size_for_search, PEPROCESS SYSTEM_eprocess) {

	if (ALL_SCAN_LIST == NULL) return FALSE;

	// IOCTL�� �־��� ���Ḯ��Ʈ ��� ( Ŀ�ο��� ������ ���� ) / Ȯ�ο����θ� ����
	PLinked_list_from_searching Start_Address_IOCTL = NULL;
	PLinked_list_from_searching Current_Address_IOCTL = NULL;


	SIZE_T output = 0;

	SIZE_T node_index = 0;

	PALL_SCANNED_MEM_DATAS current_ALL_SCAN_node = ALL_SCAN_LIST;
	while (current_ALL_SCAN_node != NULL) {

		PUCHAR Start_address = current_ALL_SCAN_node->TargetProcess_Start_Address;
		SIZE_T RegionSize = current_ALL_SCAN_node->Region_Size;

		PUCHAR Last_address = (PUCHAR)( (PUCHAR)Start_address + RegionSize ) ;
		

		while (Last_address > Start_address) {


			PUCHAR DUMP_BUFFER = ExAllocatePoolWithTag(PagedPool, Insert_data_size_for_search, 'SCAN');
			MmCopyVirtualMemory(NEEDS->target_eprocess, (PVOID)Start_address, SYSTEM_eprocess, DUMP_BUFFER, Insert_data_size_for_search, KernelMode, &output);

			if (memcmp(DUMP_BUFFER, Insert_data_for_search, Insert_data_size_for_search) == 0) {
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "�޸� ��ĵ ��ġ -> %p \n", Start_address);
				/* ������忡�� �̸� �˾��������ϹǷ� ���� �����Ҵ� �Ͽ� ������忡�� ���Ḯ��Ʈ ��ȸ�����ϵ��� �Ѵ�. */
				if (Current_Address_IOCTL == NULL) {
					Start_Address_IOCTL = Create__Linked_list_for_searching(NULL, Start_address, Insert_data_size_for_search, FALSE, NEEDS->IOCTL_requstor_HANDLE, NEEDS->IOCTL_eprocess, SYSTEM_eprocess, node_index);
					Current_Address_IOCTL = Start_Address_IOCTL;
				}
				else {
					Current_Address_IOCTL = Append__Linked_list_for_searching(Current_Address_IOCTL, Start_address, Insert_data_size_for_search, FALSE, NEEDS->IOCTL_requstor_HANDLE, NEEDS->IOCTL_eprocess, SYSTEM_eprocess, node_index);
				}
				node_index++;
			}
			ExFreePoolWithTag(DUMP_BUFFER, 'SCAN');

			Start_address += Insert_data_size_for_search;
		}


		current_ALL_SCAN_node = (PALL_SCANNED_MEM_DATAS)current_ALL_SCAN_node->Next_Addr;
	}

	((PIOCTL_info)*Inout__put_IOCTL_DATA)->Start_Node_Address = Start_Address_IOCTL; // �� �ּҴ� ���� IOCTL ��û ���μ������� ��밡��.
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Start_Address_IOCTL �����ּ�  -> %p \n", Start_Address_IOCTL);

	return TRUE;
}

BOOLEAN NEXT_SCAN(PMEMHACK_needs NEEDS, PIOCTL_info* Inout__put_IOCTL_DATA, PUCHAR Insert_data_for_search, SIZE_T Insert_data_size_for_search, PEPROCESS SYSTEM_eprocess) {

	SIZE_T output = 0;

	SIZE_T node_index = 0;

	PUCHAR DUMP_BUFFER = ExAllocatePoolWithTag(PagedPool, Insert_data_size_for_search, 'SCAN');
	MmCopyVirtualMemory(NEEDS->target_eprocess, (PVOID)((PIOCTL_info)*Inout__put_IOCTL_DATA)->known_data.known_target_address, SYSTEM_eprocess, DUMP_BUFFER, Insert_data_size_for_search, KernelMode, &output);

	if (memcmp(DUMP_BUFFER, Insert_data_for_search, Insert_data_size_for_search) == 0) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "2�޸� ��ĵ ��ġ -> %p \n", ((PIOCTL_info)*Inout__put_IOCTL_DATA)->known_data.known_target_address);

		((PIOCTL_info)*Inout__put_IOCTL_DATA)->known_data.is_same_value = TRUE;
		
	}
	else {
		((PIOCTL_info)*Inout__put_IOCTL_DATA)->known_data.is_same_value = FALSE;
	}

	ExFreePoolWithTag(DUMP_BUFFER, 'SCAN');

	// ���� ���� Ʋ���� ���� ���Ḯ��Ʈ ��带 �����Ͽ� ���� ����
	((PIOCTL_info)*Inout__put_IOCTL_DATA)->Start_Node_Address = Create__Linked_list_for_searching(NULL, ((PIOCTL_info)*Inout__put_IOCTL_DATA)->known_data.known_target_address, Insert_data_size_for_search, FALSE, NEEDS->IOCTL_requstor_HANDLE, NEEDS->IOCTL_eprocess, SYSTEM_eprocess, node_index);

	node_index++;

	return TRUE;
}

BOOLEAN Editing_MEM_to_targetprocess(PMEMHACK_needs NEEDS, PIOCTL_info* Inout__put_IOCTL_DATA, PEPROCESS SYSTEM_eprocess) {
	SIZE_T output = 0;
	// IOCTL �κ��� ���� ������ �����͸� �����´�. 
	PUCHAR find_edit_data = NULL;
	SIZE_T find_edit_data_size = 0;

	find_edit_data_size = ((PIOCTL_info)*Inout__put_IOCTL_DATA)->editing_info.ioctl_data_for_editing_SIZE;
	find_edit_data = ExAllocatePoolWithTag(PagedPool, find_edit_data_size, 'SCAN');
	if (find_edit_data == NULL) {
		return FALSE;
	}


	if (MmCopyVirtualMemory(NEEDS->IOCTL_eprocess, (PVOID)((PIOCTL_info)*Inout__put_IOCTL_DATA)->editing_info.ioctl_data_for_editing, SYSTEM_eprocess, find_edit_data, find_edit_data_size, KernelMode, &output) != STATUS_SUCCESS) {
		return FALSE;
	}
	

	// ������ ������ -> Ÿ�����μ��� ���� / kernel -> targetprocess
	MmCopyVirtualMemory(SYSTEM_eprocess, (PVOID)find_edit_data, NEEDS->target_eprocess, (PVOID)((PIOCTL_info)*Inout__put_IOCTL_DATA)->editing_info.edit_destination_Targetprocess_address, find_edit_data_size, KernelMode, &output);
	

	/*  ����Ǿ��� �� Ȯ���ؾ���  */
	SIZE_T node_index = 0;

	PUCHAR DUMP_BUFFER = ExAllocatePoolWithTag(PagedPool, find_edit_data_size, 'SCAN');
	MmCopyVirtualMemory(NEEDS->target_eprocess, (PVOID)((PIOCTL_info)*Inout__put_IOCTL_DATA)->editing_info.edit_destination_Targetprocess_address, SYSTEM_eprocess, DUMP_BUFFER, find_edit_data_size, KernelMode, &output);

	
	if (memcmp(find_edit_data, DUMP_BUFFER, find_edit_data_size) == 0) {
		((PIOCTL_info)*Inout__put_IOCTL_DATA)->editing_info.is_same_value = TRUE;
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Editing1\n");
	}
	else {
		((PIOCTL_info)*Inout__put_IOCTL_DATA)->editing_info.is_same_value = FALSE;
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Editing2\n");
	}


	// ���� ���� Ʋ���� ���� ���Ḯ��Ʈ ��带 �����Ͽ� ���� ����
	((PIOCTL_info)*Inout__put_IOCTL_DATA)->Start_Node_Address = Create__Linked_list_for_searching(NULL, ((PIOCTL_info)*Inout__put_IOCTL_DATA)->editing_info.edit_destination_Targetprocess_address, find_edit_data_size, FALSE, NEEDS->IOCTL_requstor_HANDLE, NEEDS->IOCTL_eprocess, SYSTEM_eprocess, node_index);


	ExFreePoolWithTag(DUMP_BUFFER, 'SCAN');

	node_index++;

	return TRUE;
}