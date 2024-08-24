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


BOOLEAN START_MEMHACK(PIOCTL_info* Inout__put_IOCTL_DATA);

PALL_SCANNED_MEM_DATAS ALL_SCAN(PMEMHACK_needs NEEDS); // 일단 타겟 프로세스의 모든 주소를 조회한다.

BOOLEAN NEW_SCAN(PMEMHACK_needs NEEDS, PALL_SCANNED_MEM_DATAS ALL_SCAN_LIST, PIOCTL_info* Inout__put_IOCTL_DATA, PUCHAR Insert_data_for_search, SIZE_T Insert_data_size_for_search, PEPROCESS SYSTEM_eprocess); // 처음스캔을 요청한 경우 

BOOLEAN NEXT_SCAN(PMEMHACK_needs NEEDS, PIOCTL_info* Inout__put_IOCTL_DATA, PUCHAR Insert_data_for_search, SIZE_T Insert_data_size_for_search, PEPROCESS SYSTEM_eprocess);

// 주소 수정 
BOOLEAN Editing_MEM_to_targetprocess(PMEMHACK_needs NEEDS, PIOCTL_info* Inout__put_IOCTL_DATA, PEPROCESS SYSTEM_eprocess);

#endif // !MEM_HACK_H
