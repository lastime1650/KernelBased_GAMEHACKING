#include "pch.h"
#include "Exception_Handler.h"


LONG CALLBACK ExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo) {

	if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP) {
		printf("\nBP걸림\n");

		/*
			Exception 예외처리중에서, 하드웨어 브레이크 중단점이 걸린 경우임
		*/


		PCONTEXT context = ExceptionInfo->ContextRecord;
		SEND_Exception(context);

	}




	
	return EXCEPTION_CONTINUE_EXECUTION; // 정상종료
	
	
}

BOOLEAN SEND_Exception(PCONTEXT context) {
	/* 예외 로그를 커널에 전달하는 함수 */
	HardWare_BP_struct_from_targetprocess H_BP = { 0, };


	HANDLE mypid = (HANDLE)GetCurrentProcessId();

	H_BP.PID = mypid;

	memcpy(&H_BP.Register, context, sizeof(CONTEXT));

	PIOCTL_info DATA = (PIOCTL_info)malloc(sizeof(IOCTL_info));
	memset(DATA, 0, sizeof(IOCTL_info));
	
	DATA->information = Hardware_breakpoint_response;
	memcpy(&DATA->Hardware_BP, &H_BP, sizeof(HardWare_BP_struct_from_targetprocess));

	printf("예외 요청 PID->%lld \n", DATA->Hardware_BP.PID);

	/* 커널요청 */
	BOOLEAN status = Send_to_Kernel(DATA);

	free(DATA);

	return status;
}