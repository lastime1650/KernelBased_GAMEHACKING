#include "IOCTL_request_from_TargetProcess.h"
#include "HardWare_BreakPoint_Exception.h"

// 이것은 DLL 인젝션에 의하여 TarGetProcess가 요청한 명령을 처리하는것. ( 주로 전용별 전역 연결리스트에 쌓인다 ) 
VOID START_targetprocess_analyze(PMOVE_for_TargetProcess move) {

	switch (move->information) {
	case Hardware_breakpoint_response:
	{
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " START_targetprocess_analyze ==> \"Hardware_breakpoint_response\" 받음 // PID -> %lld  \n", ((PHardWare_BP_struct_from_targetprocess)move->DATA)->PID );
		/*
			타겟 프로세스로 부터 얻은 COntext를 전역 연결리스트에 등록만 하면 됨.
		*/
		PHardWare_BP_struct_from_targetprocess get_DATA = ExAllocatePoolWithTag(NonPagedPool, sizeof(HardWare_BP_struct_from_targetprocess), 'HWBP');
		memcpy(get_DATA, move->DATA, sizeof(HardWare_BP_struct_from_targetprocess));

		K_object_lock_Release(move->KEVENT);// KeSetEvent로 일시중지된 IOCTL 디바이스 스레드 실행재개 

		ADD_HardWare_BP_Node(get_DATA);
		ExFreePoolWithTag(get_DATA, 'HWBP');
		
	}
	default:
		K_object_lock_Release(move->KEVENT); // KeSetEvent로 일시중지된 IOCTL 디바이스 스레드 실행재개 
		return;
	}
	return;
}