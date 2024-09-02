#ifndef IOCTL_TARGETPROCESS
#define IOCTL_TARGETPROCESS

/*
	�� �񵿱� ������� TargetProcess�κ��� IOCTL��û�� ó���Ѵ�.

	�̷��� ������ ���� DLL�� ������ TargetProcess�� �������� �̷��� �ֱ� �����̴�.

*/
#include <ntifs.h>
#include "KEVENT_or_KMUTEX_init.h"
#include "IOCTL.h"
typedef struct MOVE_for_TargetProcess {
	IOCTL_command information;

	K_EVENT_or_MUTEX_struct* KEVENT;
	PUCHAR DATA;
}MOVE_for_TargetProcess, * PMOVE_for_TargetProcess; // ������ ���� 

VOID START_targetprocess_analyze(PMOVE_for_TargetProcess move);


#endif