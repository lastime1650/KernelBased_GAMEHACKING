#ifndef IOCTL_TARGETPROCESS
#define IOCTL_TARGETPROCESS

/*
	이 비동기 스레드는 TargetProcess로부터 IOCTL요청을 처리한다.

	이러한 이유는 전용 DLL을 이전에 TargetProcess에 인젝션한 이력이 있기 때문이다.

*/
#include <ntifs.h>
#include "KEVENT_or_KMUTEX_init.h"
#include "IOCTL.h"
typedef struct MOVE_for_TargetProcess {
	IOCTL_command information;

	K_EVENT_or_MUTEX_struct* KEVENT;
	PUCHAR DATA;
}MOVE_for_TargetProcess, * PMOVE_for_TargetProcess; // 스레드 전용 

VOID START_targetprocess_analyze(PMOVE_for_TargetProcess move);


#endif