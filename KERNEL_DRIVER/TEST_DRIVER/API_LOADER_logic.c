#include "API_LOADER.h"

// dm
NTSTATUS Bring_API() {
	UNICODE_STRING API_NAME = { 0 };


	/*
		ZwQuerySystemInformation API �ּ� ��������
	*/
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ZwQuerySystemInformation �ּҸ� �������ڽ��ϴ�. \n");
	RtlInitUnicodeString(&API_NAME, L"ZwQuerySystemInformation");
	ZwQuerySystemInformation = (Bring_ZwQuerySystemInformation)MmGetSystemRoutineAddress(&API_NAME);
	if (ZwQuerySystemInformation == NULL) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ZwQuerySystemInformation ���� \n");
		return STATUS_UNSUCCESSFUL;
	}
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ZwQuerySystemInformation ���� \n");


	///////


	/*
		ZwQueryInformationProcess API �ּ� ��������
	*/
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ZwQueryInformationProcess �ּҸ� �������ڽ��ϴ�. \n");
	RtlInitUnicodeString(&API_NAME, L"ZwQueryInformationProcess");
	ZwQueryInformationProcess = (Bring_ZwQueryInformationProcess)MmGetSystemRoutineAddress(&API_NAME);
	if (ZwQueryInformationProcess == NULL) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ZwQueryInformationProcess ���� \n");
		return STATUS_UNSUCCESSFUL;
	}
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ZwQueryInformationProcess ���� \n");


	/*
		ObSetSecurityObjectByPointer API �ּ� ��������
	*/
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ObSetSecurityObjectByPointer �ּҸ� �������ڽ��ϴ�. \n");
	RtlInitUnicodeString(&API_NAME, L"ObSetSecurityObjectByPointer");
	ObSetSecurityObjectByPointer = (Bring_ObSetSecurityObjectByPointer)MmGetSystemRoutineAddress(&API_NAME);
	if (ObSetSecurityObjectByPointer == NULL) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ObSetSecurityObjectByPointer ���� \n");
		return STATUS_UNSUCCESSFUL;
	}
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ObSetSecurityObjectByPointer ���� \n");


	return STATUS_SUCCESS;
}

Bring_ZwQuerySystemInformation ZwQuerySystemInformation = NULL;
Bring_ZwQueryInformationProcess ZwQueryInformationProcess = NULL;
Bring_ObSetSecurityObjectByPointer ObSetSecurityObjectByPointer = NULL;