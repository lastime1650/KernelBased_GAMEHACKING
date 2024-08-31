#include "Kernel_Based_DLL_Injection.h"


BOOLEAN KERNNEL_based_Dll_inject(PMEMHACK_needs input_process_info, PIOCTL_info inout_IOCTL) {

	inout_IOCTL->DLL_info.DLL_PATH__from__ioctl_virtualmem;
	inout_IOCTL->DLL_info.DLL_PATH_length;

	// 문자열을 타겟 프로세스에 전달.. IOCTL -> target
	PUCHAR Dll_Path_base = NULL;
	SIZE_T Dll_Path_length = inout_IOCTL->DLL_info.DLL_PATH_length;
	if (ZwAllocateVirtualMemory(input_process_info->target_process_HANDLE, &Dll_Path_base, 0, &Dll_Path_length, MEM_COMMIT, PAGE_READWRITE) != STATUS_SUCCESS) { // 타겟 프로세스에 문자열 공간 할당
		inout_IOCTL->DLL_info.is_SUCCESS = FALSE;
		return FALSE;
	}

	SIZE_T Output = 0;
	if (MmCopyVirtualMemory(input_process_info->IOCTL_eprocess, inout_IOCTL->DLL_info.DLL_PATH__from__ioctl_virtualmem, input_process_info->target_eprocess, Dll_Path_base, inout_IOCTL->DLL_info.DLL_PATH_length, KernelMode, &Output) != STATUS_SUCCESS) { // 문자열 삽입 
		inout_IOCTL->DLL_info.is_SUCCESS = FALSE;
		return FALSE;
	}

	// 타겟 프로세스 핸들을 통하여 스레드 원격 실행 
	if (RtlCreateUserThread(
		input_process_info->target_process_HANDLE,
		NULL,
		FALSE,
		0,
		0,
		0,
		inout_IOCTL->DLL_info.LoadLibrary_Address,
		Dll_Path_base,
		&inout_IOCTL->DLL_info.TargetProcess_run_threadID,
		NULL
	) != STATUS_SUCCESS) {
		inout_IOCTL->DLL_info.is_SUCCESS = FALSE;
		return FALSE;
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " KERNNEL_based_Dll_inject 최종 성공\n");
	inout_IOCTL->DLL_info.is_SUCCESS = TRUE;

	return TRUE;
}