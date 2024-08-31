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


VOID START_MEMHACK(MOVE* Inout__put_IOCTL_DATA);

PALL_SCANNED_MEM_DATAS ALL_SCAN(PMEMHACK_needs NEEDS); // �ϴ� Ÿ�� ���μ����� ��� �ּҸ� ��ȸ�Ѵ�.

BOOLEAN First_Scan(PMEMHACK_needs NEEDS, PIOCTL_info Inout__put_IOCTL_DATA, PEPROCESS SYSTEM_eprocess, PUCHAR find_data, SIZE_T find_data_size);

BOOLEAN Next_Scan(PMEMHACK_needs NEEDS, PIOCTL_info Inout__put_IOCTL_DATA, PEPROCESS SYSTEM_eprocess, PUCHAR find_data, SIZE_T find_data_size, PLinked_list_from_searching Start_IOCTL_VIrtual_Addr); 

// �ּ� ���� 
BOOLEAN Editing_MEM_to_targetprocess(PMEMHACK_needs NEEDS, PIOCTL_info Inout__put_IOCTL_DATA, PEPROCESS SYSTEM_eprocess);


// Ÿ�� ���μ��� �����ּ� ������ ���� ��ü Ž��
BOOLEAN MEM_PAGE_SCAN_to_Target(PMEMHACK_needs NEEDS, PIOCTL_info Inout__put_IOCTL_DATA, PEPROCESS SYSTEM_eprocess);

#endif // !MEM_HACK_H
