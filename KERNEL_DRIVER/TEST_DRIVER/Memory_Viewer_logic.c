#include "Memory_Viewer_processing.h"

BOOLEAN MemViewer_Processing(PIOCTL_info Input_IOCTL,  PMEMHACK_needs input_process_info, PEPROCESS SYSTEM_eprocess) {
	

	/*
		[[ Ŀ�� �����Ҵ��� �ʿ䰡 ���� ]]

		IOCTL ���μ��� �����ּ� �����Ҵ� �� �ű⿡ ���� ������ ���� 
	*/
	NTSTATUS status;

	PVOID IOCTL_allocated_address = NULL;
	SIZE_T IOCTL_allocated_size = Input_IOCTL->Mem_Viewer.Scan_SIZE;
	status = ZwAllocateVirtualMemory(input_process_info->IOCTL_requstor_HANDLE, &IOCTL_allocated_address, 0, &IOCTL_allocated_size, MEM_COMMIT, PAGE_READWRITE);
	if (status != STATUS_SUCCESS ||
		IOCTL_allocated_address == NULL 
		) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "1. ZwAllocateVirtualMemory ����,[%p] %llu \n", status, IOCTL_allocated_size);
		return FALSE;
	}

	// memset IOCTL ����
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
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "2. MmCopyVirtualMemory ���� %p \n", status);
		
		if (status == STATUS_PARTIAL_COPY) {
			/*
				�� ��� �����Ϳ����� ���� �ְ�, �������� �������� ���� ����� �� �����Ƿ�, ( ������� memcpy�� �Ϸ��� �� ) 

				[!] �̹��� ���� ���ؽ�Ʈ�� �����Ͽ� Ȯ���Ѵ�. ( ������ �����ϹǷ� ����� )
			*/
			PVOID SearchAddress = Input_IOCTL->Mem_Viewer.BaseAddress_of_TargetProcess;
			MEMORY_BASIC_INFORMATION memoryInfo = { 0 };

			status = ZwQueryVirtualMemory(input_process_info->target_process_HANDLE, SearchAddress, MemoryBasicInformation, &memoryInfo, sizeof(memoryInfo), NULL);
			DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "2-1. ZwQueryVirtualMemory %p \n", status);
			if ( status == STATUS_SUCCESS) {
				if (memoryInfo.Protect == PAGE_READWRITE || memoryInfo.Protect == PAGE_WRITECOPY || memoryInfo.Protect == PAGE_EXECUTE_READWRITE || memoryInfo.Protect == PAGE_EXECUTE_WRITECOPY) {
					DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "2-2. WRITE�� ���Ե� PAGE��  \n");


					// Ÿ�� ���ؽ�Ʈ ���� ��, cpy�õ� (�߸��ϸ� BSOD )
					PUCHAR tmp = ExAllocatePoolWithTag(PagedPool, Input_IOCTL->Mem_Viewer.Scan_SIZE, 'TMPS');

					KAPC_STATE KAS = { 0, };
					KeStackAttachProcess(input_process_info->target_eprocess, &KAS);
					memcpy(tmp, SearchAddress, Input_IOCTL->Mem_Viewer.Scan_SIZE);
					KeUnstackDetachProcess(&KAS);
					
					// �ٽ� ����
					status = MmCopyVirtualMemory(

						SYSTEM_eprocess, tmp,
						input_process_info->IOCTL_eprocess, IOCTL_allocated_address,
						Input_IOCTL->Mem_Viewer.Scan_SIZE,
						KernelMode,
						&output
					);
					if (status == STATUS_SUCCESS) {
						DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "2-3. MmCopyVirtualMemory �ᱹ ����  \n");
						Input_IOCTL->Mem_Viewer.Output_Allocated_IOCTL_Address_for_return = (PUCHAR)IOCTL_allocated_address;
						ExFreePoolWithTag(tmp, 'TMPS');
						return TRUE;
					}
					else {
						DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "2-3. MmCopyVirtualMemory �� ����!  \n");
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