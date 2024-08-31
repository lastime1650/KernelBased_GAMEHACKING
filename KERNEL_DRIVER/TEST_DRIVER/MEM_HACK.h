#ifndef MEM_HACK_H
#define MEM_HACK_H

#include "IOCTL.h"

NTSTATUS NTAPI MmCopyVirtualMemory
(
	PEPROCESS SourceProcess,
	PVOID SourceAddress,
	PEPROCESS TargetProcess,
	PVOID TargetAddress,
	SIZE_T BufferSize,
	KPROCESSOR_MODE PreviousMode,
	PSIZE_T ReturnSize
);





#include "MEM_HACK_LINK_LIST.h" // IOCTL 요청자 용 



#include "MEM_HACK_private_link_list.h" // 내부용

#define MEM_IMAGE 0x1000000


typedef struct MEMHACK_needs {

	HANDLE IOCTL_requestor_PID;
	HANDLE target_process_PID;

	HANDLE IOCTL_requstor_HANDLE;
	HANDLE target_process_HANDLE;

	PEPROCESS IOCTL_eprocess;
	PEPROCESS target_eprocess;


}MEMHACK_needs, *PMEMHACK_needs;


VOID START_MEMHACK(MOVE* Inout__put_IOCTL_DATA);

PALL_SCANNED_MEM_DATAS ALL_SCAN(PMEMHACK_needs NEEDS); // 일단 타겟 프로세스의 모든 주소를 조회한다.

BOOLEAN First_Scan(PMEMHACK_needs NEEDS, PIOCTL_info Inout__put_IOCTL_DATA, PEPROCESS SYSTEM_eprocess, PUCHAR find_data, SIZE_T find_data_size);

BOOLEAN Next_Scan(PMEMHACK_needs NEEDS, PIOCTL_info Inout__put_IOCTL_DATA, PEPROCESS SYSTEM_eprocess, PUCHAR find_data, SIZE_T find_data_size, PLinked_list_from_searching Start_IOCTL_VIrtual_Addr); 

// 주소 수정 
BOOLEAN Editing_MEM_to_targetprocess(PMEMHACK_needs NEEDS, PIOCTL_info Inout__put_IOCTL_DATA, PEPROCESS SYSTEM_eprocess);


// 타겟 프로세스 가상주소 페이지 단위 전체 탐색
BOOLEAN MEM_PAGE_SCAN_to_Target(PMEMHACK_needs NEEDS, PIOCTL_info Inout__put_IOCTL_DATA, PEPROCESS SYSTEM_eprocess);

#endif // !MEM_HACK_H
