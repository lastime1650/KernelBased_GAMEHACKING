#include "MEM_HACK.h"
#include "KEVENT_or_KMUTEX_init.h"
#include "JOB_QUEUE_Manger.h" // 유저모드에게 순차적으로 전달가능. 

#include "Process_manager.h" // 프로세스 관련 유틸
#include "Memory_Viewer_processing.h" // 메모리 뷰어를 위한 로직

#include "Kernel_Based_DLL_Injection.h" // 커널기반 DLL 인젝션

VOID START_MEMHACK(MOVE* Inout__put_IOCTL_DATA) {
	if (Inout__put_IOCTL_DATA == NULL ) return;
	
	PJOB_QUEUE Copied_DATA = ExAllocatePoolWithTag(NonPagedPool, sizeof(JOB_QUEUE), 'QUEU');;
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
			스캐닝 ( First, Next )
			
			데이터를 유저모드 컨텍스트 접근으로 받아서, 처리 
		
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

		// 요청자의 데이터를 " MmCopyVirtualMemory " 으로 안전하게 가져옴 
		find_data = ExAllocatePoolWithTag(PagedPool, find_data_size, 'SCAN');
		if (find_data == NULL) {
			break;
		}


		SIZE_T output = 0;
		if (MmCopyVirtualMemory(IOCTL_Requestor_eprocess, IOCTL_request_virtual_addr, SYSTEM_eprocess, find_data, find_data_size, KernelMode, &output) != STATUS_SUCCESS) {
			
			ExFreePoolWithTag(find_data, 'SCAN');
			break;
		}

		//이제 First 또는 Next 수행
		if (Copied_DATA->INPUT_DATA.Mem_Scanning_info.Scan_mode == FIRST_SCAN) {
			First_Scan(&HACK_needs, &Copied_DATA->INPUT_DATA, SYSTEM_eprocess, find_data, find_data_size);
		}
		else if (Copied_DATA->INPUT_DATA.Mem_Scanning_info.Scan_mode == NEXT_SCAN) {
			Next_Scan(&HACK_needs, &Copied_DATA->INPUT_DATA, SYSTEM_eprocess, find_data, find_data_size, Copied_DATA->INPUT_DATA.Mem_Scanning_info.next_scan_info.Link__Start_Address_for_Kernel);
		}
		
		
		ExFreePoolWithTag(find_data, 'SCAN');
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
	case  Memory_Viewer:
		NULL;
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "--Memory_Viewer일치\n");
		// 이때는 "타겟 가상주소를 받아와야함 BaseAddress 필드 값을 통해 받기 때문임"
		MemViewer_Processing(&Copied_DATA->INPUT_DATA, &HACK_needs, SYSTEM_eprocess);
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " [MEM_VIEWER마무리] ioctl_virtual_ADDR -> %p \n", Copied_DATA->INPUT_DATA.Mem_Viewer.Output_Allocated_IOCTL_Address_for_return);
		break;
	case Memory_Page_Scan:
			NULL;
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "--Memory_Page_Scan일치\n");
			MEM_PAGE_SCAN_to_Target(&HACK_needs, &Copied_DATA->INPUT_DATA, SYSTEM_eprocess);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " [MEM_PAGE마무리] 할당된 IOCTL 요청자 가상주소 %p \n", Copied_DATA->INPUT_DATA.Mem_Page_scan_viewer.Output_Ioctl_user_Start_Address);
			break;
	case Kernel_Based_DLL_Injection:
		NULL;
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "--Kernel_Based_DLL_Injection일치\n");
		KERNNEL_based_Dll_inject(&HACK_needs, &Copied_DATA->INPUT_DATA);
		break;
	default:
		break;
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

	
	/*
		[ 경고 ]
		ZwAllocateVirtualMemory 를 호출하고 난 뒤, 얻은 SIZE는 페이지 단위로 크기를 할당한다.
		
		만약, SIZE = 100을 넣으면, 기본 페이지 단위 크기가 4096이라면,

		100이 아닌, 무려 4096 사이즈나 할당되어버리는데,

		이 크기가 실제 크기보다 너무 크면 Page Fault가 일어난다.  

		아래 MmCopyVirtualMemory에서 그러한 현상 일어남.
	*/

	// Kernel -> IOCTL user 전달.
	SIZE_T output = 0;
	MmCopyVirtualMemory(SYSTEM_eprocess, (PVOID)&Copied_DATA->INPUT_DATA, HACK_needs.IOCTL_eprocess, (PVOID)BaseAddress, sizeof(IOCTL_info), KernelMode, &output); // 복사 

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


BOOLEAN First_Scan(PMEMHACK_needs NEEDS, PIOCTL_info Inout__put_IOCTL_DATA, PEPROCESS SYSTEM_eprocess, PUCHAR find_data, SIZE_T find_data_size) {

	/*
		메모리 처음 스캔일 때,
	*/


	// IOCTL에 넣어줄 연결리스트 노드 ( 커널에서 참조는 금지 ) / 확인용으로만 사용됨
	PLinked_list_from_searching Start_Address_IOCTL = NULL;
	PLinked_list_from_searching Current_Address_IOCTL = NULL;
	SIZE_T node_index = 0;

	PVOID baseAddress = NULL;
	MEMORY_BASIC_INFORMATION memoryInfo = { 0 };

	SIZE_T output = 0;

	while (NT_SUCCESS(ZwQueryVirtualMemory(NEEDS->target_process_HANDLE, baseAddress, MemoryBasicInformation, &memoryInfo, sizeof(memoryInfo), NULL))) {

		baseAddress = (PVOID)((SIZE_T)memoryInfo.BaseAddress + memoryInfo.RegionSize);
		//(2-1) 메모리 보호 영역이 PAGE_READWRITE 또는 PAGE_EXECUTE_READWRITE 인가?
		if (( memoryInfo.Protect == PAGE_READWRITE /* || memoryInfo.Protect == PAGE_WRITECOPY || memoryInfo.Protect == PAGE_EXECUTE_WRITECOPY || memoryInfo.Protect == PAGE_EXECUTE_READWRITE*/) && memoryInfo.State == MEM_COMMIT) {
			//(2-2) 가상 시작 주소와 다음 주소 ( 시작+전체 영역크기 ) 선언
			PUCHAR  start_addr = (PUCHAR)memoryInfo.BaseAddress;
			PUCHAR  last_addr = (PUCHAR)memoryInfo.BaseAddress + memoryInfo.RegionSize;

			// 한번에 다 가져오고, 반복하여 연결리스트 생성
			PUCHAR DUMP_BUFFER = NULL;
			DUMP_BUFFER = ExAllocatePoolWithTag(PagedPool, memoryInfo.RegionSize, 'SCAN');
			if (DUMP_BUFFER == NULL) continue;
			MmCopyVirtualMemory(NEEDS->target_eprocess, (PVOID)start_addr, SYSTEM_eprocess, DUMP_BUFFER, memoryInfo.RegionSize, KernelMode, &output);
			
			PUCHAR current_DUMP_BUFFER = DUMP_BUFFER;
			while (start_addr < last_addr) {

				if (memcmp(current_DUMP_BUFFER, find_data, find_data_size) == 0) {
					// 일치할 때의 영역
					DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "메모리 스캔 일치 -> %p \n", start_addr);
					/* 유저모드에서 이를 알아차려야하므로 원격 동적할당 하여 유저모드에서 연결리스트 조회가능하도록 한다. */
					if (Current_Address_IOCTL == NULL) {
						Start_Address_IOCTL = Create__Linked_list_for_searching(NULL, start_addr, find_data_size, FALSE, NEEDS->IOCTL_requstor_HANDLE, NEEDS->IOCTL_eprocess, SYSTEM_eprocess, node_index);
						Current_Address_IOCTL = Start_Address_IOCTL;
					}
					else {
						Current_Address_IOCTL = Append__Linked_list_for_searching(Current_Address_IOCTL, start_addr, find_data_size, FALSE, NEEDS->IOCTL_requstor_HANDLE, NEEDS->IOCTL_eprocess, SYSTEM_eprocess, node_index);
					}
					node_index++;
				}
				
				// IOCTL 요청한 만큼 이동
				current_DUMP_BUFFER += find_data_size; // 커널 페이지
				start_addr = start_addr + find_data_size; // 타겟 유저 가상메모리
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

	// IOCTL에 넣어줄 연결리스트 노드 ( 커널에서 참조는 금지 ) / 확인용으로만 사용됨
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
					// 일치할 때의 영역
					DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "메모리 Next스캔 일치 -> %p \n", baseAddress);
					/* 유저모드에서 이를 알아차려야하므로 원격 동적할당 하여 유저모드에서 연결리스트 조회가능하도록 한다. */
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
	// IOCTL 로부터 먼저 변경할 데이터를 가져온다. 
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


	// 서로 값이 틀리든 같든 연결리스트의 한 노드를 전달하여 투명성 제공
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

	//(1) 타겟 프로세스의 정보 습득 ( 가상 메모리 주소, 메모리 보호 기법, 영역크기 ) 

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "MEM_PAGE_SCAN_to_Target 실행중...\n");
	while (NT_SUCCESS(ZwQueryVirtualMemory(NEEDS->target_process_HANDLE, baseAddress, MemoryBasicInformation, &memoryInfo, sizeof(memoryInfo), NULL))) {

		baseAddress = (PVOID)((SIZE_T)memoryInfo.BaseAddress + memoryInfo.RegionSize);



		if (memoryInfo.State == MEM_COMMIT ){//|| memoryInfo.State == MEM_RESERVE) {
			//DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "MEM_PAGE_SCAN_to_Target , ZwQueryVirtualMemory---BaseAddress -> (%p)\n", baseAddress);
			// (2) 얻은 메모리 정보를 그대로 유저모드에게 연결노드 축적
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


	// IOCTL 유저에게 시작주소 전달 
	Inout__put_IOCTL_DATA->Mem_Page_scan_viewer.Output_Ioctl_user_Start_Address =  Start_Address;

	return TRUE;
}