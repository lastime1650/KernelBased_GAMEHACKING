#include "MEM_HACK.h"
#include "KEVENT_or_KMUTEX_init.h"
#include "JOB_QUEUE_Manger.h" // 유저모드에게 순차적으로 전달가능. 

#include "Process_manager.h" // 프로세스 관련 유틸

VOID START_MEMHACK(MOVE* Inout__put_IOCTL_DATA) {
	if (Inout__put_IOCTL_DATA == NULL ) return;

	PJOB_QUEUE Copied_DATA = ExAllocatePoolWithTag(PagedPool, sizeof(JOB_QUEUE), 'QUEU');;
	memcpy(Copied_DATA, Inout__put_IOCTL_DATA->DATA, sizeof(JOB_QUEUE));

	K_object_lock_Release(Inout__put_IOCTL_DATA->KEVENT); // KeSetEvent로 일시중지된 IOCTL 디바이스 스레드 실행재개 ( 더이상 Inout__put_IOCTL_DATA 포인터 주소 유효하지 않음 ) 
	

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// IOCTL (  Copied_DATA 안에서 복사된 정보로 처리할 수 있음 ) 
	//IOCTL_info IOCTL_DATA = Copied_DATA.INPUT_DATA;
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "  [%d] START_MEMHACK 데이터 확인 / [핸들] IOCTL_REQUESTOR: %lld , TARGET: %lld \n", Copied_DATA->INPUT_DATA.information, Copied_DATA->INPUT_DATA.Ioctl_requestor_PID , Copied_DATA->INPUT_DATA.target_process_info.target_PID);

	/* 필요정보 습득 */
	HANDLE IOCTL_Requestor_PID = Copied_DATA->INPUT_DATA.Ioctl_requestor_PID;
	HANDLE IOCTL_Requestor_HANDLE = 0;
	if (PID_TO_HANDLE(&IOCTL_Requestor_HANDLE, IOCTL_Requestor_PID) == FALSE) return ;
	PEPROCESS IOCTL_Requestor_eprocess = NULL;
	if (PsLookupProcessByProcessId(IOCTL_Requestor_PID, &IOCTL_Requestor_eprocess) != STATUS_SUCCESS) return ;

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "IOCTL정보 얻기 성공\n");

	HANDLE target_process_PID = Copied_DATA->INPUT_DATA.target_process_info.target_PID;
	HANDLE target_process_HANDLE = 0;
	if (PID_TO_HANDLE(&target_process_HANDLE, target_process_PID) == FALSE) return ;
	PEPROCESS target_process_eprocess = NULL;
	if (PsLookupProcessByProcessId(target_process_PID, &target_process_eprocess) != STATUS_SUCCESS) {
		ObDereferenceObject(IOCTL_Requestor_eprocess);
		return ;
	}
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "target 정보 얻기 성공\n");

	PEPROCESS SYSTEM_eprocess = NULL;
	if (PsLookupProcessByProcessId((HANDLE)4, &SYSTEM_eprocess) != STATUS_SUCCESS) {
		ObDereferenceObject(IOCTL_Requestor_eprocess);
		ObDereferenceObject(target_process_eprocess);
		return ;
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "SYSTEM.exe 정보 얻기 성공\n");

	PUCHAR find_data = NULL;
	SIZE_T find_data_size = 0;

	if (Copied_DATA->INPUT_DATA.ioctl_data_for_search != NULL || Copied_DATA->INPUT_DATA.ioctl_data_for_search_SIZE != 0) {
		// 요청자의 데이터를 " MmCopyVirtualMemory " 으로 안전하게 가져옴 
		find_data_size = Copied_DATA->INPUT_DATA.ioctl_data_for_search_SIZE;
		find_data = ExAllocatePoolWithTag(PagedPool, find_data_size, 'SCAN');
		if (find_data == NULL) {
			ObDereferenceObject(IOCTL_Requestor_eprocess);
			ObDereferenceObject(target_process_eprocess);
			return ;
		}


		SIZE_T output = 0;
		if (MmCopyVirtualMemory(IOCTL_Requestor_eprocess, Copied_DATA->INPUT_DATA.ioctl_data_for_search, SYSTEM_eprocess, find_data, find_data_size, KernelMode, &output) != STATUS_SUCCESS) {
			ObDereferenceObject(IOCTL_Requestor_eprocess);
			ObDereferenceObject(target_process_eprocess);
			return ;
		}
	}

	




	//ULONG Scanning_PAGE_Area = ((PIOCTL_info)*Inout__put_IOCTL_DATA)->target_process_info.PAGE_Area_mode; 

	// 정리 
	MEMHACK_needs HACK_needs = {
		IOCTL_Requestor_PID,
		target_process_PID,

		IOCTL_Requestor_HANDLE,
		target_process_HANDLE,

		IOCTL_Requestor_eprocess,
		target_process_eprocess
	};

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "HACK_needs 구조체 성공\n");

	switch (Copied_DATA->INPUT_DATA.information) {
	case general_scan:
		/*
			일반적인_스캔
			
			데이터를 유저모드 컨텍스트 접근으로 받아서, 처리 
		
		*/
		NULL;

		if (Copied_DATA->INPUT_DATA.known_data.known_target_address == NULL) {
			/*
			
				known_target_address 이 NULL 이면, NEW_SCAN

			*/
			/* 타겟 프로세스 메모리 모두 조회 */
			PALL_SCANNED_MEM_DATAS ALL_SCAN_LIST = ALL_SCAN(&HACK_needs);
			if (ALL_SCAN_LIST == NULL) {
				ObDereferenceObject(IOCTL_Requestor_eprocess);
				ObDereferenceObject(target_process_eprocess);
				return ;
			}

			NEW_SCAN(&HACK_needs, ALL_SCAN_LIST, &Copied_DATA->INPUT_DATA, find_data, find_data_size, SYSTEM_eprocess);
		}
		else {
			/*
				known_target_address 이 존재하면, 이 주소만 바로 비교하면됨.
			*/
			NEXT_SCAN(&HACK_needs, &Copied_DATA->INPUT_DATA, find_data, find_data_size, SYSTEM_eprocess);
		}
		

		break;
	case refresh:
		// 이때 무조건 " PLinked_list_from_searching " 값이 있어야함  ( 물론 이 값은 IOCTL 요청자 가상주소에서 유효함  ) 
		NULL;
		Copied_DATA->INPUT_DATA.Start_Node_Address = query_for_Refresh(Copied_DATA->INPUT_DATA.refresh_info.From_Ioctl_requestor, HACK_needs.IOCTL_requstor_HANDLE, HACK_needs.IOCTL_eprocess, SYSTEM_eprocess);
		break;
	case editing:
		NULL;
		Editing_MEM_to_targetprocess(&HACK_needs, &Copied_DATA->INPUT_DATA, SYSTEM_eprocess);
		break;
	case dump:
		NULL;
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "--dump일치\n");
		QUERY_Module_Buffer(&HACK_needs, SYSTEM_eprocess, &Copied_DATA->INPUT_DATA);
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "[DUMP마무리] START_ADDR -> %p \n", Copied_DATA->INPUT_DATA.dump_info.To_Ioctl_requestor);
		break;
	case ATTACH:
		NULL;
		// 유저모드에서 어태치함 
		Copied_DATA->INPUT_DATA.information = ioctl_true;
		break;
	default:
		ObDereferenceObject(IOCTL_Requestor_eprocess);
		ObDereferenceObject(target_process_eprocess);
		return ;
	}

	/*
		이제 유저모드에게 결과 IOCTL데이터를 전달하려면 JOB_QUEUE 매니저를 통해서 순차적 ( Mutex )으로 전달할 수 있도록 한다. 
	*/

	/* 해제 */
	ObDereferenceObject(IOCTL_Requestor_eprocess);
	ObDereferenceObject(target_process_eprocess);


	/* 완료 큐에 전달 */
	Copied_DATA->is_finished_job = TRUE;

	/* 완료된 UOCTL_info를 IOCTL유저프로세스 동적공간에 할당 후 복사 */
	PIOCTL_info BaseAddress = NULL;
	SIZE_T SIZE = sizeof(IOCTL_info);

	ZwAllocateVirtualMemory(HACK_needs.IOCTL_requstor_HANDLE, &(PVOID)BaseAddress, 0, &SIZE, MEM_COMMIT, PAGE_READWRITE); // 할당
	Copied_DATA->OUTPUT_DATA = BaseAddress; // IOCTL 요청자 유저모드 프로그램에 동적할당하여 전달.

	
	

	// Kernel -> IOCTL user 전달.
	SIZE_T output = 0;
	MmCopyVirtualMemory(SYSTEM_eprocess, (PVOID)&Copied_DATA->INPUT_DATA, HACK_needs.IOCTL_eprocess, (PVOID)BaseAddress, SIZE, KernelMode, &output); // 복사 

	//데이터확인
	KAPC_STATE a = { 0, };
	KeStackAttachProcess(HACK_needs.IOCTL_eprocess, &a);

	
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "BaseAddress  ---> %p \n", BaseAddress->dump_info.To_Ioctl_requestor);
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "BaseAddress->dump_info.To_Ioctl_requestor ---> %p \n", BaseAddress->dump_info.To_Ioctl_requestor);
	KeUnstackDetachProcess(&a);

	Create_or_Append_Job_Queue_Node(Copied_DATA); // Copied_DATA 이변수는 동적할당되어 노드 필드에 담겨짐 


	// 스레드 종료~
	return ;

}



PALL_SCANNED_MEM_DATAS ALL_SCAN(PMEMHACK_needs NEEDS) {

	PALL_SCANNED_MEM_DATAS Start_addr = NULL;
	PALL_SCANNED_MEM_DATAS Curent_addr = Start_addr;

	PVOID baseAddress = NULL;
	MEMORY_BASIC_INFORMATION memoryInfo = { 0 };

	//(1) 타겟 프로세스의 정보 습득 ( 가상 메모리 주소, 메모리 보호 기법, 영역크기 ) 



	while (NT_SUCCESS(ZwQueryVirtualMemory(NEEDS->target_process_HANDLE, baseAddress, MemoryBasicInformation, &memoryInfo, sizeof(memoryInfo), NULL))) {

		baseAddress = (PVOID)((SIZE_T)memoryInfo.BaseAddress + memoryInfo.RegionSize);
		//(2-1) 메모리 보호 영역이 PAGE_READWRITE 또는 PAGE_EXECUTE_READWRITE 인가?
		if (memoryInfo.Protect == PAGE_READWRITE && memoryInfo.State == MEM_COMMIT) {
			//(2-2) 가상 시작 주소와 다음 주소 ( 시작+전체 영역크기 ) 선언
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

	// IOCTL에 넣어줄 연결리스트 노드 ( 커널에서 참조는 금지 ) / 확인용으로만 사용됨
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
				DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "메모리 스캔 일치 -> %p \n", Start_address);
				/* 유저모드에서 이를 알아차려야하므로 원격 동적할당 하여 유저모드에서 연결리스트 조회가능하도록 한다. */
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

	Inout__put_IOCTL_DATA->Start_Node_Address = Start_Address_IOCTL; // 이 주소는 이제 IOCTL 요청 프로세스에서 사용가능.
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Start_Address_IOCTL 시작주소  -> %p \n", Start_Address_IOCTL);

	return TRUE;
}

BOOLEAN NEXT_SCAN(PMEMHACK_needs NEEDS, PIOCTL_info Inout__put_IOCTL_DATA, PUCHAR Insert_data_for_search, SIZE_T Insert_data_size_for_search, PEPROCESS SYSTEM_eprocess) {

	SIZE_T output = 0;

	SIZE_T node_index = 0;

	PUCHAR DUMP_BUFFER = ExAllocatePoolWithTag(PagedPool, Insert_data_size_for_search, 'SCAN');
	MmCopyVirtualMemory(NEEDS->target_eprocess, (PVOID)Inout__put_IOCTL_DATA->known_data.known_target_address, SYSTEM_eprocess, DUMP_BUFFER, Insert_data_size_for_search, KernelMode, &output);

	if (memcmp(DUMP_BUFFER, Insert_data_for_search, Insert_data_size_for_search) == 0) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "2메모리 스캔 일치 -> %p \n", Inout__put_IOCTL_DATA->known_data.known_target_address);

		Inout__put_IOCTL_DATA->known_data.is_same_value = TRUE;
		
	}
	else {
		Inout__put_IOCTL_DATA->known_data.is_same_value = FALSE;
	}

	ExFreePoolWithTag(DUMP_BUFFER, 'SCAN');

	// 서로 값이 틀리든 같든 연결리스트 노드를 전달하여 투명성 제공
	Inout__put_IOCTL_DATA->Start_Node_Address = Create__Linked_list_for_searching(NULL, Inout__put_IOCTL_DATA->known_data.known_target_address, Insert_data_size_for_search, FALSE, NEEDS->IOCTL_requstor_HANDLE, NEEDS->IOCTL_eprocess, SYSTEM_eprocess, node_index);

	node_index++;

	return TRUE;
}

BOOLEAN Editing_MEM_to_targetprocess(PMEMHACK_needs NEEDS, PIOCTL_info Inout__put_IOCTL_DATA, PEPROCESS SYSTEM_eprocess) {
	SIZE_T output = 0;
	// IOCTL 로부터 먼저 변경할 데이터를 가져온다. 
	PUCHAR find_edit_data = NULL;
	SIZE_T find_edit_data_size = 0;

	find_edit_data_size = Inout__put_IOCTL_DATA->editing_info.ioctl_data_for_editing_SIZE;
	find_edit_data = ExAllocatePoolWithTag(PagedPool, find_edit_data_size, 'SCAN');
	if (find_edit_data == NULL) {
		return FALSE;
	}


	if (MmCopyVirtualMemory(NEEDS->IOCTL_eprocess, (PVOID)Inout__put_IOCTL_DATA->editing_info.ioctl_data_for_editing, SYSTEM_eprocess, find_edit_data, find_edit_data_size, KernelMode, &output) != STATUS_SUCCESS) {
		return FALSE;
	}
	

	// 변경할 데이터 -> 타겟프로세스 삽입 / kernel -> targetprocess
	MmCopyVirtualMemory(SYSTEM_eprocess, (PVOID)find_edit_data, NEEDS->target_eprocess, (PVOID)Inout__put_IOCTL_DATA->editing_info.edit_destination_Targetprocess_address, find_edit_data_size, KernelMode, &output);
	

	/*  변경되었는 지 확인해야함  */
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


	// 서로 값이 틀리든 같든 연결리스트 노드를 전달하여 투명성 제공
	Inout__put_IOCTL_DATA->Start_Node_Address = Create__Linked_list_for_searching(NULL, Inout__put_IOCTL_DATA->editing_info.edit_destination_Targetprocess_address, find_edit_data_size, FALSE, NEEDS->IOCTL_requstor_HANDLE, NEEDS->IOCTL_eprocess, SYSTEM_eprocess, node_index);


	ExFreePoolWithTag(DUMP_BUFFER, 'SCAN');

	node_index++;

	return TRUE;
}