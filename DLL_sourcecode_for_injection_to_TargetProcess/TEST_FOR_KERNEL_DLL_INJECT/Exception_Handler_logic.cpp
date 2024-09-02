#include "pch.h"
#include "Exception_Handler.h"


LONG CALLBACK ExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo) {

	if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP) {
		printf("\nBP�ɸ�\n");

		/*
			Exception ����ó���߿���, �ϵ���� �극��ũ �ߴ����� �ɸ� �����
		*/


		PCONTEXT context = ExceptionInfo->ContextRecord;
		SEND_Exception(context);

	}




	
	return EXCEPTION_CONTINUE_EXECUTION; // ��������
	
	
}

BOOLEAN SEND_Exception(PCONTEXT context) {
	/* ���� �α׸� Ŀ�ο� �����ϴ� �Լ� */
	HardWare_BP_struct_from_targetprocess H_BP = { 0, };


	HANDLE mypid = (HANDLE)GetCurrentProcessId();

	H_BP.PID = mypid;

	memcpy(&H_BP.Register, context, sizeof(CONTEXT));

	PIOCTL_info DATA = (PIOCTL_info)malloc(sizeof(IOCTL_info));
	memset(DATA, 0, sizeof(IOCTL_info));
	
	DATA->information = Hardware_breakpoint_response;
	memcpy(&DATA->Hardware_BP, &H_BP, sizeof(HardWare_BP_struct_from_targetprocess));

	printf("���� ��û PID->%lld \n", DATA->Hardware_BP.PID);

	/* Ŀ�ο�û */
	BOOLEAN status = Send_to_Kernel(DATA);

	free(DATA);

	return status;
}