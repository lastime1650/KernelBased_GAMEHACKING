#ifndef Query_Process_H
#define Query_Process_H

#include <vector> // 벡터 사용
#include <Windows.h>
#include <winternl.h> // NTSTATUS 및 NtQuerySystemInformation을 위한 헤더
#include <ntstatus.h>

#ifdef __cplusplus
extern "C" {
#endif

	PSYSTEM_PROCESS_INFORMATION Query_Processes();

#ifdef __cplusplus
}
#endif

#endif // !Query_Process_H
