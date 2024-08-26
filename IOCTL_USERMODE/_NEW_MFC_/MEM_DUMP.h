#ifndef MEM_DUMP
#define MEM_DUMP

#include <stdio.h>
#include <Windows.h>


typedef struct MEM_DUMP_ {

	PUCHAR Previous_Addr;

	SIZE_T node_index;

	PUCHAR To_IOCTL_Requestor_Address;// 가상주소. 커널에서 가상주소 할당한 주소인 것.
	SIZE_T SIZE;

	PUCHAR Next_Addr;


}MEM_DUMP_NODE, * PMEM_DUMP_NODE;

#ifdef __cplusplus
extern "C" {
#endif

	BOOLEAN query_MEM_DUMP(PMEM_DUMP_NODE Start_Addr, LPCSTR SAVE_PATH);

#ifdef __cplusplus
	}
#endif


#endif