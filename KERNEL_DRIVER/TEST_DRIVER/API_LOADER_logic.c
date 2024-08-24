#include "API_LOADER.h"

// dm
NTSTATUS Bring_API() {
	UNICODE_STRING API_NAME = { 0 };


	/*
		ZwQuerySystemInformation API 주소 가져오기
	*/
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ZwQuerySystemInformation 주소를 가져오겠습니다. \n");
	RtlInitUnicodeString(&API_NAME, L"ZwQuerySystemInformation");
	ZwQuerySystemInformation = (Bring_ZwQuerySystemInformation)MmGetSystemRoutineAddress(&API_NAME);
	if (ZwQuerySystemInformation == NULL) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ZwQuerySystemInformation 실패 \n");
		return STATUS_UNSUCCESSFUL;
	}
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ZwQuerySystemInformation 성공 \n");


	///////


	/*
		ZwQueryInformationProcess API 주소 가져오기
	*/
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ZwQueryInformationProcess 주소를 가져오겠습니다. \n");
	RtlInitUnicodeString(&API_NAME, L"ZwQueryInformationProcess");
	ZwQueryInformationProcess = (Bring_ZwQueryInformationProcess)MmGetSystemRoutineAddress(&API_NAME);
	if (ZwQueryInformationProcess == NULL) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ZwQueryInformationProcess 실패 \n");
		return STATUS_UNSUCCESSFUL;
	}
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ZwQueryInformationProcess 성공 \n");


	/*
		ObSetSecurityObjectByPointer API 주소 가져오기
	*/
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ObSetSecurityObjectByPointer 주소를 가져오겠습니다. \n");
	RtlInitUnicodeString(&API_NAME, L"ObSetSecurityObjectByPointer");
	ObSetSecurityObjectByPointer = (Bring_ObSetSecurityObjectByPointer)MmGetSystemRoutineAddress(&API_NAME);
	if (ObSetSecurityObjectByPointer == NULL) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ObSetSecurityObjectByPointer 실패 \n");
		return STATUS_UNSUCCESSFUL;
	}
	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "ObSetSecurityObjectByPointer 성공 \n");


	return STATUS_SUCCESS;
}

Bring_ZwQuerySystemInformation ZwQuerySystemInformation = NULL;
Bring_ZwQueryInformationProcess ZwQueryInformationProcess = NULL;
Bring_ObSetSecurityObjectByPointer ObSetSecurityObjectByPointer = NULL;