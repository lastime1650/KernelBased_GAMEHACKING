#ifndef Query_Process_H
#define Query_Process_H

#include <vector> // ���� ���
#include <Windows.h>
#include <winternl.h> // NTSTATUS �� NtQuerySystemInformation�� ���� ���
#include <ntstatus.h>

#ifdef __cplusplus
extern "C" {
#endif

	PSYSTEM_PROCESS_INFORMATION Query_Processes();

#ifdef __cplusplus
}
#endif

#endif // !Query_Process_H
