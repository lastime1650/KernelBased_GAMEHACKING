#include <stdio.h>
#include <Windows.h>


// ���� ó���� �Լ�
LONG CALLBACK ExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo) {
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP) {
		// Single step exception ó��
		printf("Single step exception occurred.\n");
		// ���ܸ� ��� ó���ϵ��� ��ȯ
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	// �ٸ� ���ܴ� �⺻ ���� ó����� ����
	return EXCEPTION_CONTINUE_SEARCH;
}


VOID function1() {
	printf("���� function1�Լ��Դϴ�! ���� ȣ���Ѱ���?~\n");
	return;
}


int main() {

	PVOID handler = AddVectoredExceptionHandler(1, ExceptionHandler);

	ULONG32 data = 1650;

	

	for (ULONG32 i = 0; i < 9999; i++) {
		PUCHAR data1 = malloc(sizeof(ULONG32));
		memcpy(data1, &data, sizeof(ULONG32));
	}
	
	PUCHAR data2 = malloc(sizeof(ULONG32));
	memcpy(data2, &data, sizeof(ULONG32)); 


	FLOAT data3 = 1650.0;
	DOUBLE data4 = 1111.0;
	

	while (1) {
		HANDLE my_pid = (HANDLE)GetCurrentProcessId();

		printf("���ּ� %llu \n", my_pid);
		printf(" �����ּ�: %p ������ %d ���� %d \n", &data, data, sizeof(ULONG32));
		memcpy(&data, data2, sizeof(ULONG32));
		data += 1;
		

		printf(" �����ּ�: %p ������ %d ���� %d \n", data2, (ULONG32)(*(ULONG32*)data2), sizeof(ULONG32));

		printf(" ���� FLOAT �ּ�: %p ������ %f ���� %d \n", &data3, data3, sizeof(FLOAT));
		printf(" ���� DOUBLE �ּ�: %p ������ %lf ���� %d \n", &data4, data4, sizeof(DOUBLE));

		printf("LoadLibraryA �ּ� -> %p \n", LoadLibraryA);
		printf("LoadLibraryW �ּ� -> %p \n", LoadLibraryW);
		printf("\n\n function1 �ּ�(dll ������ �׽�Ʈ��) -> %p \n", function1);
		system("pause");
	}

	RemoveVectoredExceptionHandler(handler);
	return 0;
}