#include "MEM_HACK.h"
#include "KEVENT_or_KMUTEX_init.h"
#include "JOB_QUEUE_Manger.h" // ������忡�� ���������� ���ް���. 

#include "Process_manager.h" // ���μ��� ���� ��ƿ
#include "Memory_Viewer_processing.h" // �޸� �� ���� ����

#include "Kernel_Based_DLL_Injection.h" // Ŀ�α�� DLL ������

VOID START_MEMHACK(MOVE* Inout__put_IOCTL_DATA) {
	if (Inout__put_IOCTL_DATA == NULL ) return;
	
	PJOB_QUEUE Copied_DATA = ExAllocatePoolWithTag(NonPagedPool, sizeof(JOB_QUEUE), 'QUEU');;
	memcpy(Copied_DATA, Inout__put_IOCTL_DATA->DATA, sizeof(JOB_QUEUE));
	
	K_object_lock_Release(Inout__put_IOCTL_DATA->KEVENT); // KeSetEvent�� �Ͻ������� IOCTL ����̽� ������ �����簳 ( ���̻� Inout__put_IOCTL_DATA ������ �ּ� ��ȿ���� ���� ) 
	

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// IOCTL (  Copied_DATA �ȿ��� ����� ������ ó���� �� ���� ) 
	//IOCTL_info IOCTL_DATA = Copied_DATA.INPUT_DATA;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "  [%d] START_MEMHACK ������ Ȯ�� / [�ڵ�] IOCTL_REQUESTOR: %lld , TARGET: %lld \n", Copied_DATA->INPUT_DATA.information, Copied_DATA->INPUT_DATA.Ioctl_requestor_PID , Copied_DATA->INPUT_DATA.target_process_info.target_PID);

	/* �ʿ����� ���� */
	HANDLE IOCTL_Requestor_PID = Copied_DATA->INPUT_DATA.Ioctl_requestor_PID;
	HANDLE IOCTL_Requestor_HANDLE = 0;
	if (PID_TO_HANDLE(&IOCTL_Requestor_HANDLE, IOCTL_Requestor_PID) == FALSE) return ;
	PEPROCESS IOCTL_Requestor_eprocess = NULL;
	if (PsLookupProcessByProcessId(IOCTL_Requestor_PID, &IOCTL_Requestor_eprocess) != STATUS_SUCCESS) return ;

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "IOCTL���� ��� ����\n");

	HANDLE target_process_PID = Copied_DATA->INPUT_DATA.target_process_info.target_PID;
	HANDLE target_process_HANDLE = 0;
	if (PID_TO_HANDLE(&target_process_HANDLE, target_process_PID) == FALSE) return ;
	PEPROCESS target_process_eprocess = NULL;
	if (PsLookupProcessByProcessId(target_process_PID, &target_process_eprocess) != STATUS_SUCCESS) {
		ObDereferenceObject(IOCTL_Requestor_eprocess);
		return ;
	}
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "target ���� ��� ����\n");

	PEPROCESS SYSTEM_eprocess = NULL;
	if (PsLookupProcessByProcessId((HANDLE)4, &SYSTEM_eprocess) != STATUS_SUCCESS) {
		ObDereferenceObject(IOCTL_Requestor_eprocess);
		ObDereferenceObject(target_process_eprocess);
		return ;
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "SYSTEM.exe ���� ��� ����\n");

	
	

	




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

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "HACK_needs ����ü ����\n");

	switch (Copied_DATA->INPUT_DATA.information) {
	case general_scan:
		/*
			��ĳ�� ( First, Next )
			
			�����͸� ������� ���ؽ�Ʈ �������� �޾Ƽ�, ó�� 
		
		*/
		NULL;

		PUCHAR find_data = NULL;
		SIZE_T find_data_size = 0;

		PUCHAR IOCTL_request_virtual_addr = NULL;
		if (Copied_DATA->INPUT_DATA.Mem_Scanning_info.Scan_mode == FIRST_SCAN) {
			IOCTL_request_virtual_addr = Copied_DATA->INPUT_DATA.Mem_Scanning_info.first_scan_info.IOCTL_allocated_addr__for_data_scan;
			find_data_size = Copied_DATA->INPUT_DATA.Mem_Scanning_info.first_scan_info.IOCTL_allocated_size__for_data_scan;
		}
		else if (Copied_DATA->INPUT_DATA.Mem_Scanning_info.Scan_mode == NEXT_SCAN) {
			IOCTL_request_virtual_addr = Copied_DATA->INPUT_DATA.Mem_Scanning_info.next_scan_info.IOCTL_allocated_addr__for_data_scan;
			find_data_size = Copied_DATA->INPUT_DATA.Mem_Scanning_info.next_scan_info.IOCTL_allocated_size__for_data_scan;
		}
		else {
			break;
		}

		// ��û���� �����͸� " MmCopyVirtualMemory " ���� �����ϰ� ������ 
		find_data = ExAllocatePoolWithTag(PagedPool, find_data_size, 'SCAN');
		if (find_data == NULL) {
			break;
		}


		SIZE_T output = 0;
		if (MmCopyVirtualMemory(IOCTL_Requestor_eprocess, IOCTL_request_virtual_addr, SYSTEM_eprocess, find_data, find_data_size, KernelMode, &output) != STATUS_SUCCESS) {
			
			ExFreePoolWithTag(find_data, 'SCAN');
			break;
		}

		//���� First �Ǵ� Next ����
		if (Copied_DATA->INPUT_DATA.Mem_Scanning_info.Scan_mode == FIRST_SCAN) {
			First_Scan(&HACK_needs, &Copied_DATA->INPUT_DATA, SYSTEM_eprocess, find_data, find_data_size);
		}
		else if (Copied_DATA->INPUT_DATA.Mem_Scanning_info.Scan_mode == NEXT_SCAN) {
			Next_Scan(&HACK_needs, &Copied_DATA->INPUT_DATA, SYSTEM_eprocess, find_data, find_data_size, Copied_DATA->INPUT_DATA.Mem_Scanning_info.next_scan_info.Link__Start_Address_for_Kernel);
		}
		
		
		ExFreePoolWithTag(find_data, 'SCAN');
		break;
	case refresh:
		// �̶� ������ " PLinked_list_from_searching " ���� �־����  ( ���� �� ���� IOCTL ��û�� �����ּҿ��� ��ȿ��  ) 
		NULL;
		Copied_DATA->INPUT_DATA.Start_Node_Address = query_for_Refresh(Copied_DATA->INPUT_DATA.refresh_info.From_Ioctl_requestor, HACK_needs.IOCTL_requstor_HANDLE, HACK_needs.IOCTL_eprocess, SYSTEM_eprocess);
		break;
	case editing:
		NULL;
		Editing_MEM_to_targetprocess(&HACK_needs, &Copied_DATA->INPUT_DATA, SYSTEM_eprocess);
		break;
	case dump:
		NULL;
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "--dump��ġ\n");
		QUERY_Module_Buffer(&HACK_needs, SYSTEM_eprocess, &Copied_DATA->INPUT_DATA);
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[DUMP������] START_ADDR -> %p \n", Copied_DATA->INPUT_DATA.dump_info.To_Ioctl_requestor);
		break;
	case ATTACH:
		NULL;
		// ������忡�� ����ġ�� 
		Copied_DATA->INPUT_DATA.information = ioctl_true;
		break;
	case  Memory_Viewer:
		NULL;
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "--Memory_Viewer��ġ\n");
		// �̶��� "Ÿ�� �����ּҸ� �޾ƿ;��� BaseAddress �ʵ� ���� ���� �ޱ� ������"
		MemViewer_Processing(&Copied_DATA->INPUT_DATA, &HACK_needs, SYSTEM_eprocess);
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " [MEM_VIEWER������] ioctl_virtual_ADDR -> %p \n", Copied_DATA->INPUT_DATA.Mem_Viewer.Output_Allocated_IOCTL_Address_for_return);
		break;
	case Memory_Page_Scan:
			NULL;
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "--Memory_Page_Scan��ġ\n");
			MEM_PAGE_SCAN_to_Target(&HACK_needs, &Copied_DATA->INPUT_DATA, SYSTEM_eprocess);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " [MEM_PAGE������] �Ҵ�� IOCTL ��û�� �����ּ� %p \n", Copied_DATA->INPUT_DATA.Mem_Page_scan_viewer.Output_Ioctl_user_Start_Address);
			break;
	case Kernel_Based_DLL_Injection:
		NULL;
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "--Kernel_Based_DLL_Injection��ġ\n");
		KERNNEL_based_Dll_inject(&HACK_needs, &Copied_DATA->INPUT_DATA);
		break;
	default:
		break;
	}

	/*
		���� ������忡�� ��� IOCTL�����͸� �����Ϸ��� JOB_QUEUE �Ŵ����� ���ؼ� ������ ( Mutex )���� ������ �� �ֵ��� �Ѵ�. 
	*/

	/* ���� */
	ObDereferenceObject(IOCTL_Requestor_eprocess);
	ObDereferenceObject(target_process_eprocess);


	/* �Ϸ� ť�� ���� */
	Copied_DATA->is_finished_job = TRUE;

	/* �Ϸ�� UOCTL_info�� IOCTL�������μ��� ���������� �Ҵ� �� ���� */
	PIOCTL_info BaseAddress = NULL;
	SIZE_T SIZE = sizeof(IOCTL_info);

	ZwAllocateVirtualMemory(HACK_needs.IOCTL_requstor_HANDLE, &(PVOID)BaseAddress, 0, &SIZE, MEM_COMMIT, PAGE_READWRITE); // �Ҵ�
	Copied_DATA->OUTPUT_DATA = BaseAddress; // IOCTL ��û�� ������� ���α׷��� �����Ҵ��Ͽ� ����.

	
	/*
		[ ��� ]
		ZwAllocateVirtualMemory �� ȣ���ϰ� �� ��, ���� SIZE�� ������ ������ ũ�⸦ �Ҵ��Ѵ�.
		
		����, SIZE = 100�� ������, �⺻ ������ ���� ũ�Ⱑ 4096�̶��,

		100�� �ƴ�, ���� 4096 ����� �Ҵ�Ǿ�����µ�,

		�� ũ�Ⱑ ���� ũ�⺸�� �ʹ� ũ�� Page Fault�� �Ͼ��.  

		�Ʒ� MmCopyVirtualMemory���� �׷��� ���� �Ͼ.
	*/

	// Kernel -> IOCTL user ����.
	SIZE_T output = 0;
	MmCopyVirtualMemory(SYSTEM_eprocess, (PVOID)&Copied_DATA->INPUT_DATA, HACK_needs.IOCTL_eprocess, (PVOID)BaseAddress, sizeof(IOCTL_info), KernelMode, &output); // ���� 

	Create_or_Append_Job_Queue_Node(Copied_DATA); // Copied_DATA �̺����� �����Ҵ�Ǿ� ��� �ʵ忡 ����� 


	// ������ ����~
	return ;

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
BOOLEAN NEW_SCAN(PMEMHACK_needs NEEDS, PALL_SCANNED_MEM_DATAS ALL_SCAN_LIST, PIOCTL_info Inout__put_IOCTL_DATA,    PUCHAR Insert_data_for_search, SIZE_T Insert_data_size_for_search, PEPROCESS SYSTEM_eprocess) {

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

	Inout__put_IOCTL_DATA->Start_Node_Address = Start_Address_IOCTL; // �� �ּҴ� ���� IOCTL ��û ���μ������� ��밡��.
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Start_Address_IOCTL �����ּ�  -> %p \n", Start_Address_IOCTL);

	return TRUE;
}


BOOLEAN First_Scan(PMEMHACK_needs NEEDS, PIOCTL_info Inout__put_IOCTL_DATA, PEPROCESS SYSTEM_eprocess, PUCHAR find_data, SIZE_T find_data_size) {

	/*
		�޸� ó�� ��ĵ�� ��,
	*/


	// IOCTL�� �־��� ���Ḯ��Ʈ ��� ( Ŀ�ο��� ������ ���� ) / Ȯ�ο����θ� ����
	PLinked_list_from_searching Start_Address_IOCTL = NULL;
	PLinked_list_from_searching Current_Address_IOCTL = NULL;
	SIZE_T node_index = 0;

	PVOID baseAddress = NULL;
	MEMORY_BASIC_INFORMATION memoryInfo = { 0 };

	SIZE_T output = 0;

	while (NT_SUCCESS(ZwQueryVirtualMemory(NEEDS->target_process_HANDLE, baseAddress, MemoryBasicInformation, &memoryInfo, sizeof(memoryInfo), NULL))) {

		baseAddress = (PVOID)((SIZE_T)memoryInfo.BaseAddress + memoryInfo.RegionSize);
		//(2-1) �޸� ��ȣ ������ PAGE_READWRITE �Ǵ� PAGE_EXECUTE_READWRITE �ΰ�?
		if (( memoryInfo.Protect == PAGE_READWRITE /* || memoryInfo.Protect == PAGE_WRITECOPY || memoryInfo.Protect == PAGE_EXECUTE_WRITECOPY || memoryInfo.Protect == PAGE_EXECUTE_READWRITE*/) && memoryInfo.State == MEM_COMMIT) {
			//(2-2) ���� ���� �ּҿ� ���� �ּ� ( ����+��ü ����ũ�� ) ����
			PUCHAR  start_addr = (PUCHAR)memoryInfo.BaseAddress;
			PUCHAR  last_addr = (PUCHAR)memoryInfo.BaseAddress + memoryInfo.RegionSize;

			// �ѹ��� �� ��������, �ݺ��Ͽ� ���Ḯ��Ʈ ����
			PUCHAR DUMP_BUFFER = NULL;
			DUMP_BUFFER = ExAllocatePoolWithTag(PagedPool, memoryInfo.RegionSize, 'SCAN');
			if (DUMP_BUFFER == NULL) continue;
			MmCopyVirtualMemory(NEEDS->target_eprocess, (PVOID)start_addr, SYSTEM_eprocess, DUMP_BUFFER, memoryInfo.RegionSize, KernelMode, &output);
			
			PUCHAR current_DUMP_BUFFER = DUMP_BUFFER;
			while (start_addr < last_addr) {

				if (memcmp(current_DUMP_BUFFER, find_data, find_data_size) == 0) {
					// ��ġ�� ���� ����
					DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "�޸� ��ĵ ��ġ -> %p \n", start_addr);
					/* ������忡�� �̸� �˾��������ϹǷ� ���� �����Ҵ� �Ͽ� ������忡�� ���Ḯ��Ʈ ��ȸ�����ϵ��� �Ѵ�. */
					if (Current_Address_IOCTL == NULL) {
						Start_Address_IOCTL = Create__Linked_list_for_searching(NULL, start_addr, find_data_size, FALSE, NEEDS->IOCTL_requstor_HANDLE, NEEDS->IOCTL_eprocess, SYSTEM_eprocess, node_index);
						Current_Address_IOCTL = Start_Address_IOCTL;
					}
					else {
						Current_Address_IOCTL = Append__Linked_list_for_searching(Current_Address_IOCTL, start_addr, find_data_size, FALSE, NEEDS->IOCTL_requstor_HANDLE, NEEDS->IOCTL_eprocess, SYSTEM_eprocess, node_index);
					}
					node_index++;
				}
				
				// IOCTL ��û�� ��ŭ �̵�
				current_DUMP_BUFFER += find_data_size; // Ŀ�� ������
				start_addr = start_addr + find_data_size; // Ÿ�� ���� ����޸�
			}

			ExFreePoolWithTag(DUMP_BUFFER, 'SCAN');
		}

		if ((PUCHAR)((PUCHAR)memoryInfo.BaseAddress + memoryInfo.RegionSize) < (PUCHAR)baseAddress) break;

	}


	// Output
	Inout__put_IOCTL_DATA->Mem_Scanning_info.Link__Start_Address_for_IOCTL = Start_Address_IOCTL; 
	return TRUE;
}



BOOLEAN Next_Scan(PMEMHACK_needs NEEDS, PIOCTL_info Inout__put_IOCTL_DATA, PEPROCESS SYSTEM_eprocess, PUCHAR find_data, SIZE_T find_data_size, PLinked_list_from_searching Start_IOCTL_VIrtual_Addr) {

	// IOCTL�� �־��� ���Ḯ��Ʈ ��� ( Ŀ�ο��� ������ ���� ) / Ȯ�ο����θ� ����
	PLinked_list_from_searching Start_Address_IOCTL = NULL;
	PLinked_list_from_searching Current_Address_IOCTL = NULL;
	SIZE_T node_index = 0;

	MEMORY_BASIC_INFORMATION memoryInfo = { 0 };

	SIZE_T output = 0;

	PLinked_list_from_searching current = Start_IOCTL_VIrtual_Addr;
	while (current) {

		PUCHAR baseAddress = NULL;
		PLinked_list_from_searching remember_current_for_next_node = NULL;

		KAPC_STATE STAT = { 0, };
		KeStackAttachProcess(NEEDS->IOCTL_eprocess, &STAT);
		baseAddress = current->Searched__target_process_virtual_memory;

		remember_current_for_next_node = (PLinked_list_from_searching)current->next_addr;
		KeUnstackDetachProcess(&STAT);

		if (ZwQueryVirtualMemory(NEEDS->target_process_HANDLE, baseAddress, MemoryBasicInformation, &memoryInfo, sizeof(memoryInfo), NULL) != STATUS_SUCCESS) {
			current = remember_current_for_next_node;
			continue;
		}
		else {

			if ((memoryInfo.Protect == PAGE_READWRITE /* || memoryInfo.Protect == PAGE_WRITECOPY || memoryInfo.Protect == PAGE_EXECUTE_WRITECOPY || memoryInfo.Protect == PAGE_EXECUTE_READWRITE*/) && memoryInfo.State == MEM_COMMIT) {


				PUCHAR DUMP_BUFFER = NULL;
				DUMP_BUFFER = ExAllocatePoolWithTag(PagedPool, find_data_size, 'SCAN');
				if (DUMP_BUFFER == NULL) continue;
				if (MmCopyVirtualMemory(NEEDS->target_eprocess, (PVOID)baseAddress, SYSTEM_eprocess, DUMP_BUFFER, find_data_size, KernelMode, &output) != STATUS_SUCCESS) {
					current = remember_current_for_next_node;
					ExFreePoolWithTag(DUMP_BUFFER, 'SCAN');
					continue;
				}

				if (memcmp(DUMP_BUFFER, find_data, find_data_size) == 0) {
					// ��ġ�� ���� ����
					DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "�޸� Next��ĵ ��ġ -> %p \n", baseAddress);
					/* ������忡�� �̸� �˾��������ϹǷ� ���� �����Ҵ� �Ͽ� ������忡�� ���Ḯ��Ʈ ��ȸ�����ϵ��� �Ѵ�. */
					if (Current_Address_IOCTL == NULL) {
						Start_Address_IOCTL = Create__Linked_list_for_searching(NULL, baseAddress, find_data_size, FALSE, NEEDS->IOCTL_requstor_HANDLE, NEEDS->IOCTL_eprocess, SYSTEM_eprocess, node_index);
						Current_Address_IOCTL = Start_Address_IOCTL;
					}
					else {
						Current_Address_IOCTL = Append__Linked_list_for_searching(Current_Address_IOCTL, baseAddress, find_data_size, FALSE, NEEDS->IOCTL_requstor_HANDLE, NEEDS->IOCTL_eprocess, SYSTEM_eprocess, node_index);
					}
					node_index++;
				}
				ExFreePoolWithTag(DUMP_BUFFER, 'SCAN');
			}
			current = remember_current_for_next_node;
			continue;
		}
	}

	// Output
	Inout__put_IOCTL_DATA->Mem_Scanning_info.Link__Start_Address_for_IOCTL = Start_Address_IOCTL;
	return TRUE;
}









///////////////////////////////////////////////////////////////////////////////////////////////////////


BOOLEAN Editing_MEM_to_targetprocess(PMEMHACK_needs NEEDS, PIOCTL_info Inout__put_IOCTL_DATA, PEPROCESS SYSTEM_eprocess) {
	SIZE_T output = 0;
	// IOCTL �κ��� ���� ������ �����͸� �����´�. 
	PUCHAR find_edit_data = NULL;
	SIZE_T find_edit_data_size = 0;

	find_edit_data_size = Inout__put_IOCTL_DATA->editing_info.ioctl_data_for_editing_SIZE;
	find_edit_data = ExAllocatePoolWithTag(PagedPool, find_edit_data_size, 'SCAN');
	if (find_edit_data == NULL) {
		return FALSE;
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Editing_MEM_to_targetprocess --> SIZE: %llu \n", find_edit_data_size);

	// User -> Kernel
	if (MmCopyVirtualMemory(NEEDS->IOCTL_eprocess, (PVOID)Inout__put_IOCTL_DATA->editing_info.ioctl_data_for_editing, SYSTEM_eprocess, find_edit_data, find_edit_data_size, KernelMode, &output) != STATUS_SUCCESS) {
		return FALSE;
	}
	
	// ������ ������ -> Ÿ�����μ��� ���� / kernel -> targetprocess
	MmCopyVirtualMemory(SYSTEM_eprocess, (PVOID)find_edit_data, NEEDS->target_eprocess, (PVOID)Inout__put_IOCTL_DATA->editing_info.edit_destination_Targetprocess_address, find_edit_data_size, KernelMode, &output);
	





	/*  ����Ǿ��� �� Ȯ���ؾ���  */
	SIZE_T node_index = 0;

	PUCHAR DUMP_BUFFER = ExAllocatePoolWithTag(PagedPool, find_edit_data_size, 'SCAN');
	MmCopyVirtualMemory(NEEDS->target_eprocess, (PVOID)Inout__put_IOCTL_DATA->editing_info.edit_destination_Targetprocess_address, SYSTEM_eprocess, DUMP_BUFFER, find_edit_data_size, KernelMode, &output);

	
	if (memcmp(find_edit_data, DUMP_BUFFER, find_edit_data_size) == 0) {
		Inout__put_IOCTL_DATA->editing_info.is_same_value = TRUE;
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Editing1\n");
	}
	else {
		Inout__put_IOCTL_DATA->editing_info.is_same_value = FALSE;
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Editing2\n");
	}


	// ���� ���� Ʋ���� ���� ���Ḯ��Ʈ�� �� ��带 �����Ͽ� ���� ����
	Inout__put_IOCTL_DATA->Start_Node_Address = Create__Linked_list_for_searching(NULL, Inout__put_IOCTL_DATA->editing_info.edit_destination_Targetprocess_address, find_edit_data_size, FALSE, NEEDS->IOCTL_requstor_HANDLE, NEEDS->IOCTL_eprocess, SYSTEM_eprocess, node_index);



	ExFreePoolWithTag(DUMP_BUFFER, 'SCAN');
	ExFreePoolWithTag(find_edit_data, 'SCAN');

	node_index++;

	return TRUE;
}

BOOLEAN MEM_PAGE_SCAN_to_Target(PMEMHACK_needs NEEDS, PIOCTL_info Inout__put_IOCTL_DATA, PEPROCESS SYSTEM_eprocess) {


	PVOID baseAddress = NULL;
	MEMORY_BASIC_INFORMATION memoryInfo = { 0 };

	PPAGE_SCAN_NODE Start_Address = NULL;
	PPAGE_SCAN_NODE Current_Address = NULL;

	//(1) Ÿ�� ���μ����� ���� ���� ( ���� �޸� �ּ�, �޸� ��ȣ ���, ����ũ�� ) 

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "MEM_PAGE_SCAN_to_Target ������...\n");
	while (NT_SUCCESS(ZwQueryVirtualMemory(NEEDS->target_process_HANDLE, baseAddress, MemoryBasicInformation, &memoryInfo, sizeof(memoryInfo), NULL))) {

		baseAddress = (PVOID)((SIZE_T)memoryInfo.BaseAddress + memoryInfo.RegionSize);



		if (memoryInfo.State == MEM_COMMIT ){//|| memoryInfo.State == MEM_RESERVE) {
			//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "MEM_PAGE_SCAN_to_Target , ZwQueryVirtualMemory---BaseAddress -> (%p)\n", baseAddress);
			// (2) ���� �޸� ������ �״�� ������忡�� ������ ����
			if (Current_Address == NULL) {
				Start_Address = Create__MEM_PAGE_NODE(&memoryInfo, NEEDS->IOCTL_requstor_HANDLE, NEEDS->IOCTL_eprocess, SYSTEM_eprocess);
				Current_Address = Start_Address;
			}
			else {
				Current_Address = Append__MEM_PAGE_NODE(Current_Address, &memoryInfo, NEEDS->IOCTL_requstor_HANDLE, NEEDS->IOCTL_eprocess, SYSTEM_eprocess);
			}
		}



		

		
		if ((PUCHAR)((PUCHAR)memoryInfo.BaseAddress + memoryInfo.RegionSize) < (PUCHAR)baseAddress) break;
	}


	// IOCTL �������� �����ּ� ���� 
	Inout__put_IOCTL_DATA->Mem_Page_scan_viewer.Output_Ioctl_user_Start_Address =  Start_Address;

	return TRUE;
}