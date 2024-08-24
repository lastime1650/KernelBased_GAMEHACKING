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





#include "MEM_HACK_LINK_LIST.h" // IOCTL ��û�� �� 



#include "MEM_HACK_private_link_list.h" // ���ο�

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

PALL_SCANNED_MEM_DATAS ALL_SCAN(PMEMHACK_needs NEEDS); // �ϴ� Ÿ�� ���μ����� ��� �ּҸ� ��ȸ�Ѵ�.

BOOLEAN NEW_SCAN(PMEMHACK_needs NEEDS, PALL_SCANNED_MEM_DATAS ALL_SCAN_LIST, PIOCTL_info* Inout__put_IOCTL_DATA, PUCHAR Insert_data_for_search, SIZE_T Insert_data_size_for_search, PEPROCESS SYSTEM_eprocess); // ó����ĵ�� ��û�� ��� 

BOOLEAN NEXT_SCAN(PMEMHACK_needs NEEDS, PIOCTL_info* Inout__put_IOCTL_DATA, PUCHAR Insert_data_for_search, SIZE_T Insert_data_size_for_search, PEPROCESS SYSTEM_eprocess);

// �ּ� ���� 
BOOLEAN Editing_MEM_to_targetprocess(PMEMHACK_needs NEEDS, PIOCTL_info* Inout__put_IOCTL_DATA, PEPROCESS SYSTEM_eprocess);

#endif // !MEM_HACK_H
