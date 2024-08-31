#include "Memory_Viewer_processing.h"

BOOLEAN MemViewer_Processing(PIOCTL_info Input_IOCTL,  PMEMHACK_needs input_process_info, PEPROCESS SYSTEM_eprocess) {
	

	/*
		[[ 커널 동적할당이 필요가 없음 ]]

		IOCTL 프로세스 가상주소 동적할당 후 거기에 실제 덤프값 저장 
	*/
	NTSTATUS status;

	PVOID IOCTL_allocated_address = NULL;
	SIZE_T IOCTL_allocated_size = Input_IOCTL->Mem_Viewer.Scan_SIZE;
	status = ZwAllocateVirtualMemory(input_process_info->IOCTL_requstor_HANDLE, &IOCTL_allocated_address, 0, &IOCTL_allocated_size, MEM_COMMIT, PAGE_READWRITE);
	if (status != STATUS_SUCCESS ||
		IOCTL_allocated_address == NULL 
		) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "1. ZwAllocateVirtualMemory 실패,[%p] %llu \n", status, IOCTL_allocated_size);
		return FALSE;
	}

	// memset IOCTL 접근
	KAPC_STATE STAT= { 0, };
	KeStackAttachProcess(input_process_info->IOCTL_eprocess, &STAT);
	memset(IOCTL_allocated_address, 0, IOCTL_allocated_size);
	KeUnstackDetachProcess(&STAT);

	SIZE_T output = 0;
	status = MmCopyVirtualMemory(

		input_process_info->target_eprocess, Input_IOCTL->Mem_Viewer.BaseAddress_of_TargetProcess,
		input_process_info->IOCTL_eprocess, IOCTL_allocated_address,
		Input_IOCTL->Mem_Viewer.Scan_SIZE,
		KernelMode,
		&output
	);

	if (status == STATUS_SUCCESS) {

		Input_IOCTL->Mem_Viewer.Output_Allocated_IOCTL_Address_for_return = (PUCHAR)IOCTL_allocated_address;

		return TRUE;
	}
	else {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "2. MmCopyVirtualMemory 실패 %p \n", status);
		
		if (status == STATUS_PARTIAL_COPY) {
			/*
				이 경우 데이터영역일 수도 있고, 페이지가 지원하지 않은 경우일 수 있으므로, ( 어거지로 memcpy로 하려는 것 ) 

				[!] 이번엔 유저 컨텍스트에 접근하여 확인한다. ( 하지만 위험하므로 실험용 )
			*/
			PVOID SearchAddress = Input_IOCTL->Mem_Viewer.BaseAddress_of_TargetProcess;
			MEMORY_BASIC_INFORMATION memoryInfo = { 0 };

			status = ZwQueryVirtualMemory(input_process_info->target_process_HANDLE, SearchAddress, MemoryBasicInformation, &memoryInfo, sizeof(memoryInfo), NULL);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "2-1. ZwQueryVirtualMemory %p \n", status);
			if ( status == STATUS_SUCCESS) {
				if (memoryInfo.Protect == PAGE_READWRITE || memoryInfo.Protect == PAGE_WRITECOPY || memoryInfo.Protect == PAGE_EXECUTE_READWRITE || memoryInfo.Protect == PAGE_EXECUTE_WRITECOPY) {
					DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "2-2. WRITE가 포함된 PAGE임  \n");


					// 타겟 컨텍스트 접근 후, cpy시도 (잘못하면 BSOD )
					PUCHAR tmp = ExAllocatePoolWithTag(PagedPool, Input_IOCTL->Mem_Viewer.Scan_SIZE, 'TMPS');

					KAPC_STATE KAS = { 0, };
					KeStackAttachProcess(input_process_info->target_eprocess, &KAS);
					memcpy(tmp, SearchAddress, Input_IOCTL->Mem_Viewer.Scan_SIZE);
					KeUnstackDetachProcess(&KAS);
					
					// 다시 복사
					status = MmCopyVirtualMemory(

						SYSTEM_eprocess, tmp,
						input_process_info->IOCTL_eprocess, IOCTL_allocated_address,
						Input_IOCTL->Mem_Viewer.Scan_SIZE,
						KernelMode,
						&output
					);
					if (status == STATUS_SUCCESS) {
						DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "2-3. MmCopyVirtualMemory 결국 성공  \n");
						Input_IOCTL->Mem_Viewer.Output_Allocated_IOCTL_Address_for_return = (PUCHAR)IOCTL_allocated_address;
						ExFreePoolWithTag(tmp, 'TMPS');
						return TRUE;
					}
					else {
						DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "2-3. MmCopyVirtualMemory 또 실패!  \n");
						ExFreePoolWithTag(tmp, 'TMPS');
					}
				}
			}
		}
		
	}

	ZwFreeVirtualMemory(input_process_info->IOCTL_requstor_HANDLE, &IOCTL_allocated_address, &IOCTL_allocated_size, MEM_FREE);
	Input_IOCTL->Mem_Viewer.Output_Allocated_IOCTL_Address_for_return = NULL;
	return FALSE;
}