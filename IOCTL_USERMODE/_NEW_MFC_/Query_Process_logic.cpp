#include "pch.h"
#include "Query_Process.h"

PSYSTEM_PROCESS_INFORMATION Query_Processes() {
	ULONG bufferSize = 0;
	NTSTATUS status = NtQuerySystemInformation(SystemProcessInformation, nullptr, 0, &bufferSize);
	if (status == STATUS_INFO_LENGTH_MISMATCH)
	{
		// 필요한 버퍼 크기를 할당
		PUCHAR buffer = (PUCHAR)malloc(bufferSize);
		status = NtQuerySystemInformation(SystemProcessInformation, buffer, bufferSize, nullptr);

		if (status == STATUS_SUCCESS) {
			return (PSYSTEM_PROCESS_INFORMATION)buffer;

			
		}
	}

	return NULL;
}