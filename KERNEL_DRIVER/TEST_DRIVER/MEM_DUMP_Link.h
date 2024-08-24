#ifndef MEM_DUMP
#define MEM_DUMP

#include <ntifs.h>

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

typedef struct MEM_DUMP_ {

	PUCHAR Previous_Addr;

	SIZE_T node_index;

	PUCHAR To_IOCTL_Requestor_Address;// 가상주소. 커널에서 가상주소 할당한 주소인 것.
	SIZE_T SIZE;

	PUCHAR Next_Addr;


}MEM_DUMP_NODE, *PMEM_DUMP_NODE;


PMEM_DUMP_NODE Create__MEM_DUMP_NODE(PMEM_DUMP_NODE Previous, PUCHAR Insert_targetprocess_address, SIZE_T Insert_targetprocess_result_regionsize ,HANDLE IOCTL_requestor_HANDLE, PEPROCESS IOCTL_requestor_eprocess, PEPROCESS target_process_eprocess, PEPROCESS SYSTEM_eprocess, SIZE_T node_index);

PMEM_DUMP_NODE Append__MEM_DUMP_NODE(PMEM_DUMP_NODE Current, PUCHAR Insert_targetprocess_address, SIZE_T Insert_targetprocess_result_regionsize, HANDLE IOCTL_requestor_HANDLE, PEPROCESS IOCTL_requestor_eprocess, PEPROCESS target_process_eprocess, PEPROCESS SYSTEM_eprocess, SIZE_T node_index);

#endif