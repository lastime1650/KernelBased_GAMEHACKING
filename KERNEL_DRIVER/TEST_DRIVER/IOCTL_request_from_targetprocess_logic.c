#include "IOCTL_request_from_TargetProcess.h"
#include "HardWare_BreakPoint_Exception.h"

// �̰��� DLL �����ǿ� ���Ͽ� TarGetProcess�� ��û�� ����� ó���ϴ°�. ( �ַ� ���뺰 ���� ���Ḯ��Ʈ�� ���δ� ) 
VOID START_targetprocess_analyze(PMOVE_for_TargetProcess move) {

	switch (move->information) {
	case Hardware_breakpoint_response:
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " START_targetprocess_analyze ==> \"Hardware_breakpoint_response\" ���� // PID -> %lld  \n", ((PHardWare_BP_struct_from_targetprocess)move->DATA)->PID );
		/*
			Ÿ�� ���μ����� ���� ���� COntext�� ���� ���Ḯ��Ʈ�� ��ϸ� �ϸ� ��.
		*/
		PHardWare_BP_struct_from_targetprocess get_DATA = ExAllocatePoolWithTag(NonPagedPool, sizeof(HardWare_BP_struct_from_targetprocess), 'HWBP');
		memcpy(get_DATA, move->DATA, sizeof(HardWare_BP_struct_from_targetprocess));

		K_object_lock_Release(move->KEVENT);// KeSetEvent�� �Ͻ������� IOCTL ����̽� ������ �����簳 

		ADD_HardWare_BP_Node(get_DATA);
		ExFreePoolWithTag(get_DATA, 'HWBP');
		
	}
	default:
		K_object_lock_Release(move->KEVENT); // KeSetEvent�� �Ͻ������� IOCTL ����̽� ������ �����簳 
		return;
	}
	return;
}