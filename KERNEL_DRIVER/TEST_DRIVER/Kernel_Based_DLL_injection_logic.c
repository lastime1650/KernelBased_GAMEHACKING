#include "Kernel_Based_DLL_Injection.h"


BOOLEAN KERNNEL_based_Dll_inject(PMEMHACK_needs input_process_info, PIOCTL_info inout_IOCTL) {

	inout_IOCTL->DLL_info.DLL_PATH__from__ioctl_virtualmem;
	inout_IOCTL->DLL_info.DLL_PATH_length;

	// ���ڿ��� Ÿ�� ���μ����� ����.. IOCTL -> target
	PUCHAR Dll_Path_base = NULL;
	SIZE_T Dll_Path_length = inout_IOCTL->DLL_info.DLL_PATH_length;
	if (ZwAllocateVirtualMemory(input_process_info->target_process_HANDLE, &Dll_Path_base, 0, &Dll_Path_length, MEM_COMMIT, PAGE_READWRITE) != STATUS_SUCCESS) { // Ÿ�� ���μ����� ���ڿ� ���� �Ҵ�
		inout_IOCTL->DLL_info.is_SUCCESS = FALSE;
		return FALSE;
	}

	SIZE_T Output = 0;
	if (MmCopyVirtualMemory(input_process_info->IOCTL_eprocess, inout_IOCTL->DLL_info.DLL_PATH__from__ioctl_virtualmem, input_process_info->target_eprocess, Dll_Path_base, inout_IOCTL->DLL_info.DLL_PATH_length, KernelMode, &Output) != STATUS_SUCCESS) { // ���ڿ� ���� 
		inout_IOCTL->DLL_info.is_SUCCESS = FALSE;
		return FALSE;
	}

	// Ÿ�� ���μ��� �ڵ��� ���Ͽ� ������ ���� ���� 
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

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " KERNNEL_based_Dll_inject ���� ����\n");
	inout_IOCTL->DLL_info.is_SUCCESS = TRUE;

	return TRUE;
}