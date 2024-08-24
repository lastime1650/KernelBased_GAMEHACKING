#include "Process_manager.h"



BOOLEAN PID_TO_HANDLE(HANDLE* OUTPUT_HANDLE, HANDLE INPUT_PID) {
	if (OUTPUT_HANDLE == NULL) return FALSE;

	NTSTATUS status = STATUS_SUCCESS;

	PEPROCESS eprocess = NULL;
	status = PsLookupProcessByProcessId(INPUT_PID, &eprocess);
	if (eprocess == NULL) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Get_real_HANDLE_from_pid ->  PsLookupProcessByProcessId 실패! %p", status);
		return FALSE;
	}

	status = ObOpenObjectByPointer(eprocess, 0, NULL, 0, 0, KernelMode, OUTPUT_HANDLE); // 참조 횟수 증가
	if (*OUTPUT_HANDLE == 0 || status != STATUS_SUCCESS) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Get_real_HANDLE_from_pid ->  ObOpenObjectByPointer 실패! %p", status);
		ObDereferenceObject(eprocess);
		return FALSE;
	}

	ObDereferenceObject(eprocess);

	return TRUE;
}

///
#include "PE_parsing.h"//pe 구조체 
#include "MEM_DUMP_Link.h" // 덤퍼 저장
BOOLEAN QUERY_Module_Buffer(PMEMHACK_needs NEEDS, PEPROCESS SYSTEM_eprocess, PIOCTL_info* Inout_IOCTL) {

	PVOID baseAddress = NULL;
	MEMORY_BASIC_INFORMATION memoryInfo = { 0 };


	SIZE_T output = 0;
	PUCHAR DUMP_BUFFER = NULL;


	PUCHAR CURRENT_EXE_DUMP_ADDR = NULL; // 추적용
	PUCHAR LAST_EXE_DUMP_ADDR = NULL; // 추적용 

	PMEM_DUMP_NODE START_ADDR = NULL;
	PMEM_DUMP_NODE CURRENT_ADDR = NULL;

	SIZE_T node_index = 0;
	

	while (NT_SUCCESS(ZwQueryVirtualMemory(NEEDS->target_process_HANDLE, baseAddress, MemoryBasicInformation, &memoryInfo, sizeof(memoryInfo), NULL))) {
		
		baseAddress = (PVOID)((SIZE_T)memoryInfo.BaseAddress + memoryInfo.RegionSize);


		if (memoryInfo.State == MEM_COMMIT && ( memoryInfo.Type == MEM_IMAGE || memoryInfo.Type == MEM_MAPPED ) ) {

			if (memoryInfo.RegionSize == (SIZE_T)0x1000 && CURRENT_EXE_DUMP_ADDR == NULL) {
				

				PUCHAR  start_addr = (PUCHAR)memoryInfo.BaseAddress;

				if (CURRENT_EXE_DUMP_ADDR == NULL) {

					// 최소한의 PE 크기 
					DUMP_BUFFER = ExAllocatePoolWithTag(PagedPool, memoryInfo.RegionSize, 'SCAN');
					MmCopyVirtualMemory(NEEDS->target_eprocess, (PVOID)start_addr, SYSTEM_eprocess, DUMP_BUFFER, memoryInfo.RegionSize, KernelMode, &output);

					
					ULONG32 SizeOfImage = 0;

					if (is_this_EXE_file(DUMP_BUFFER, &SizeOfImage)) {


						if (CURRENT_ADDR == NULL) {
							START_ADDR = Create__MEM_DUMP_NODE(NULL, start_addr, SizeOfImage, NEEDS->IOCTL_requstor_HANDLE, NEEDS->IOCTL_eprocess, NEEDS->target_eprocess, SYSTEM_eprocess, node_index);
							CURRENT_ADDR = START_ADDR;
						}
						else {
							CURRENT_ADDR = Append__MEM_DUMP_NODE(CURRENT_ADDR, start_addr, SizeOfImage, NEEDS->IOCTL_requstor_HANDLE, NEEDS->IOCTL_eprocess, NEEDS->target_eprocess, SYSTEM_eprocess, node_index);
						}
						node_index++;

						CURRENT_EXE_DUMP_ADDR = start_addr;
						LAST_EXE_DUMP_ADDR = (PUCHAR)start_addr + SizeOfImage;

						DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " SizeOfImage -> %d / CURRENT_EXE_DUMP_ADDR -> %p  /  LAST_EXE_DUMP_ADDR -> %p \n", SizeOfImage, CURRENT_EXE_DUMP_ADDR, LAST_EXE_DUMP_ADDR);

						/*
						// target_process -> IOCTL_requestor process 
						EXE_DUMP_SIZE = (SIZE_T)ntHeaders->OptionalHeader.SizeOfImage;
						if (ZwAllocateVirtualMemory(NEEDS->IOCTL_requstor_HANDLE, &(PVOID)EXE_DUMP, 0, &EXE_DUMP_SIZE, MEM_COMMIT, PAGE_READWRITE) != STATUS_SUCCESS) {
							DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " DUMP 가상주소할당 실패 \n");
							ExFreePoolWithTag(DUMP_BUFFER, 'SCAN');
							return FALSE;
						}
						((PIOCTL_info)*Inout_IOCTL)->dump_info.To_User_Address = EXE_DUMP;
						((PIOCTL_info)*Inout_IOCTL)->dump_info.SIZE = EXE_DUMP_SIZE;


						MmCopyVirtualMemory(NEEDS->target_eprocess, (PVOID)start_addr, NEEDS->IOCTL_eprocess, EXE_DUMP, EXE_DUMP_SIZE, KernelMode, &output);



						ExFreePoolWithTag(DUMP_BUFFER, 'SCAN');
						DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " SizeOfImage -> %d  / %p / %llu \n", ntHeaders->OptionalHeader.SizeOfImage, ((PIOCTL_info)*Inout_IOCTL)->dump_info.To_User_Address, ((PIOCTL_info)*Inout_IOCTL)->dump_info.SIZE);
						return TRUE;
						*/

					}

					ExFreePoolWithTag(DUMP_BUFFER, 'SCAN');

				}
			}
			



		}


		if (CURRENT_EXE_DUMP_ADDR != NULL) {
			/* 이때는 아직 EXE 덤프 유효한 주소에 있음을 확인. */
			CURRENT_EXE_DUMP_ADDR = (PUCHAR)memoryInfo.BaseAddress;

			if (CURRENT_EXE_DUMP_ADDR >= LAST_EXE_DUMP_ADDR) {
				// 모두 도달한 경우 초기화
				CURRENT_EXE_DUMP_ADDR = NULL;
				LAST_EXE_DUMP_ADDR = NULL;
			}

			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "  CURRENT_EXE_DUMP_ADDR -> %p  /  LAST_EXE_DUMP_ADDR -> %p \n",  CURRENT_EXE_DUMP_ADDR, LAST_EXE_DUMP_ADDR);

		}
		

		

		if ((PUCHAR)((PUCHAR)memoryInfo.BaseAddress + memoryInfo.RegionSize) < (PUCHAR)baseAddress) break;

	}

	((PIOCTL_info)*Inout_IOCTL)->dump_info.To_Ioctl_requestor=START_ADDR;

	// BUFFER 할당해제는 외부에서... 
	return TRUE;
}