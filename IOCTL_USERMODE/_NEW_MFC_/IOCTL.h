#ifndef  IOCTL_H
#define IOCTL_H

#include <stdio.h>
#include <Windows.h>
#include <cstdlib>
#include <winternl.h>
#include <winioctl.h> // IOCTL

#include "Linked_list_MEM_query.h"
#include "MEM_DUMP.h"
#include "MEM_Page_Scan_link.h"
#include "Hardware_BreakPoint.h"



/* 구조체 */
// 타겟 프로세스에 대한 최소한의 정보 ( 스캔할 때 도움이 됨 ) 
typedef struct target_process_information {

    HANDLE target_PID;

    ULONG32 PAGE_Area_mode;// PAGE_READWRITE , PAGE_EXECUTE_READWRITE ... etc


}target_process_information, * Ptarget_process_information;

typedef struct known_address_information {
    PUCHAR known_target_address; // User -> Kernel 
    BOOLEAN is_same_value;
}known_address_information, * Pknown_address_information;

typedef struct refresh_address_information {
    PLinked_list_from_searching From_Ioctl_requestor; // User -> Kernel 
}refresh_address_information, * Prefresh_address_information;

typedef struct editing_address_information {

    PUCHAR edit_destination_Targetprocess_address;

    PUCHAR ioctl_data_for_editing; // 이 주소의 데이터는 타겟프로세스 주소에 copy될 것.
    SIZE_T ioctl_data_for_editing_SIZE;

    BOOLEAN is_same_value; // 결과확인용 

}editing_address_information, * Pediting_address_information;

typedef struct DUMP_information {
    PMEM_DUMP_NODE To_Ioctl_requestor;// Kernel -> User
}DUMP_information, * PDUMP_information;

// 메모리 뷰 전용 구조체
typedef struct MEM_VIEWR{
    PUCHAR BaseAddress_of_TargetProcess; 
    SIZE_T Scan_SIZE; // 이 길이만큼 읽어서 <타겟가상주소의 데이터를 가져와,> IOCTL 가상주소에 할당할 것.
    
    PUCHAR Output_Allocated_IOCTL_Address_for_return; // 메모리 뷰를 위한 데이터 전달 < 주소가 유효하지 않았다면 NULL값이 들어감 >
}MEM_VIEWR, *PMEM_VIEWR;


// 메모리 전체 페이지 ( MEM_COMMIT 일떄의 전체 ) 
typedef struct MEM_PAGE {
    PPAGE_SCAN_NODE Output_Ioctl_user_Start_Address;
}MEM_PAGE, * PMEM_PAGE;



// 메모리 스캐닝
typedef enum MEM_SCANNING_ENUM {
    FIRST_SCAN = 1,
    NEXT_SCAN
}MEM_SCANNING_ENUM;
typedef struct MEM_SCANNING_FIRST_SCAN {
    /* 이 필드는 IOCTL 요청 프로세스의 동적할당된 주소 */
    PUCHAR IOCTL_allocated_addr__for_data_scan;
    SIZE_T IOCTL_allocated_size__for_data_scan;
}MEM_SCANNING_FIRST_SCAN, * PMEM_SCANNING_FIRST_SCAN;


typedef struct MEM_SCANNING_NEXT_SCAN {

    /* 이 필드는 IOCTL 요청 프로세스의 동적할당된 주소 */
    PUCHAR IOCTL_allocated_addr__for_data_scan;
    SIZE_T IOCTL_allocated_size__for_data_scan;

    // IOCTL 가상주소에 할당된 연결리스트( 이미 타겟주소를 알고 있는 상태의 여러노드를 커널에서 조회.)
    PLinked_list_from_searching Link__Start_Address_for_Kernel;
}MEM_SCANNING_NEXT_SCAN, * PMEM_SCANNING_NEXT_SCAN;


typedef struct MEM_SCANNING {
    MEM_SCANNING_ENUM Scan_mode;

    MEM_SCANNING_FIRST_SCAN first_scan_info;

    MEM_SCANNING_NEXT_SCAN next_scan_info;

    // 커널에서 IOCTL프로세스 가상주소에 결과를 연결리스트로 저장함.
    PLinked_list_from_searching Link__Start_Address_for_IOCTL; // Output
}MEM_SCANNING, * PMEM_SCANNING;


/* 커널기반 DLL 인젝션 */
typedef struct Kernel_Dll_Injection {

    PUCHAR LoadLibrary_Address; // Ansi or Wide

    PUCHAR DLL_PATH__from__ioctl_virtualmem; // dll 문자열은 ioctl 유저프로그램에 존재한다.
    ULONG32 DLL_PATH_length;



    /* Output*/
    BOOLEAN is_SUCCESS;//output
    HANDLE TargetProcess_run_threadID; // output

}Kernel_Dll_Injection, * PKernel_Dll_Injection;


/*IOCTL 요청자에서의 하드웨어BP 관련 */
typedef struct HardWare_BP_Set {

    ULONG64 Monitor_TargetProcess_Address;

    BOOLEAN is_SUCCESS;
}HardWare_BP_Set, * PHardWare_BP_Set; // 커널에서 하드웨어 BP 걸기 

typedef struct HardWare_BP_Get {
    PHardWare_BP_struct_from_targetprocess OUTPUT; // IOCTL 가상주소에 동적할당 
}HardWare_BP_Get, * PHardWare_BP_Get; // 커널에서 하드웨어 BP 가져오기 

typedef struct HardWare_BP_info {

    HardWare_BP_Set SET; // 하드웨어BP를 타겟프로세스에 걸기 
    HardWare_BP_Get GET; // 하드웨어BP된 연결리스트 조회하여 가져오기

}HardWare_BP_info, * PHardWare_BP_info;

typedef enum IOCTL_command {

    loop__Check = 1,  // 유저모드에서 지속적으로 커널에게 체크하면서 ( "JOB_QUEUE'리턴 값 회수시도 ) 


    /* 타겟 프로세스 요청 info*/
    Hardware_breakpoint_response, // 하드웨어 중단점 확인용





    /* IOCTL 요청자 프로세스 요청 info  */
    ATTACH,// user -> kernel 어태치 ( 커널에서 주소접근 가능한가? ) 

    Memory_Viewer, // 메모리 뷰를 위한 데이터 요청 ( 쿼리 ) -> ( 타겟 프로세스 데이터 copy )-> ( IOCTL 유저 ) 

    general_scan, // user -( PID )-> kernel
    dump, // 커널단에서 덤프
    refresh, // IOCTL 요청자에서 알고 있는 타겟프로세스 주소의 데이터를 다시한번 조회하는 것.

    editing, // user -(PID + data_for_edit) -> kernel

    Memory_Page_Scan, // 타겟 프로세스의 페이지를 모두 스캔한 결과를 IOCTL 유저 프로세스 공간에 연결리스트로 구축함 

    Kernel_Based_DLL_Injection, // 커널에서 DLL 인젝션

    /* 하드웨어 BP 관련 BP */
    Hardware_breakpoint_set, // 커널에서 하드웨어BP 걸때 
    Hardware_breakpoint_request_for_GET,// IOCTL요청자에서 연결리스트 노드 데이터를 가져올때 


    /* 응답용 */
    ioctl_true,
    ioctl_false

}IOCTL_command;

typedef struct IOCTL_info {

    IOCTL_command information;


    target_process_information target_process_info; // 타겟 프로세스에 대한 정보 + 검색할 데이터 

    PLinked_list_from_searching Start_Node_Address; // IOCTL 요청자에게 동적할당될 연결리스트 시작주소  / Kernel -> User 

    HANDLE Ioctl_requestor_PID; // 유저 컨텍스트 접근을 위한 PID 

    // 메모리 스캔용 First_Scan 그리고 Next_Scan 전용
    MEM_SCANNING Mem_Scanning_info;

    // 커널기반 DLL 인젝션용 구조체
    Kernel_Dll_Injection DLL_info;

    // 메모리 페이지 스캔 전용
    MEM_PAGE Mem_Page_scan_viewer;

    // 메모리 뷰어 전용 
    MEM_VIEWR Mem_Viewer;// 메모리 뷰어 열때 커널에게 요청할 구조체  

    // Scan 필수 조건 
    PUCHAR ioctl_data_for_search; // IOCTL 요청자 유저모드의 가상주소이며, 이 포인터는 검색을 위한 데이터가 있음. 
    SIZE_T ioctl_data_for_search_SIZE; // ioctl_data_for_search 의 사이즈 

    // DUMP 전용 ( 커널이 ioctl에게 동적할당된 주소를 알려줄때 사용 ) 
    DUMP_information dump_info;

    // Next_Scan 또는 이미 주소를 알고 있을 때, 
    known_address_information known_data; // user->kernel->user

    // Refresh 용
    refresh_address_information refresh_info;

    // Editing ( 데이터 수정 ) 
    editing_address_information editing_info;

    // 하드웨어 BP 용 ( Get,Set 모두 포함 ) 
    HardWare_BP_info HWBP_info;

    HANDLE IOCTL_User_Mutex;
}IOCTL_info, * PIOCTL_info;


#define IOCTL_MY_IOCTL_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

extern HANDLE* IOCTL_hDevice;

typedef struct JOB_QUEUE_STRUCT {

    IOCTL_info INPUT_DATA;

    BOOLEAN is_finished_job; // 작업이 끝났는가? ( 초기엔 FALSE ) 

    PUCHAR This_Node_Start_Address; // 작업번호 대체재 ( 이 구조체의 동적할당된 주소/ 작업 결과를 적용하려면 넣어야함 (비동기이기 때문) ) 

   

    
    HANDLE Usermode_Mutex;
    

    PIOCTL_info OUTPUT_DATA; //커널에서 동적할당하여 유저에게 반환

}JOB_QUEUE, * PJOB_QUEUE;


#ifdef __cplusplus
extern "C" {
#endif

    BOOLEAN Send_to_Kernel(PIOCTL_info DATA, BOOLEAN is_Waiting, PIOCTL_info* Output_IOCTL_info, PJOB_QUEUE* Output_JOB_QUEUE); // 커널에게 전달하고, 동적할당된 정보 얻기 

    PIOCTL_info STill_Checking_a_Job_when_finish_it(PJOB_QUEUE Input_your_Job); // Send_to_Kernel 후 완료될때까지 반복 조회 ( 완료되면 (PIOCTL_info 포인터 정상반환 ) 성공 ) 

    DWORD WINAPI Kernel_LoopCheck(PVOID unused);

#ifdef __cplusplus
	}
#endif

#endif // ! IOCTL_H
