#ifndef DRIVER_IOCTL_H
#define DRIVER_IOCTL_H

#include <ntifs.h>

#include "MEM_HACK_LINK_LIST.h"
#include "MEM_DUMP_Link.h"

#define IOCTL_MY_IOCTL_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

// 타겟 프로세스에 대한 최소한의 정보 ( 스캔할 때 도움이 됨 ) 
typedef struct target_process_information {

    HANDLE target_PID;

    ULONG32 PAGE_Area_mode;// PAGE_READWRITE , PAGE_EXECUTE_READWRITE ... etc


}target_process_information, * Ptarget_process_information;

typedef struct known_address_information {
    PUCHAR known_target_address; // User -> Kernel 
    BOOLEAN is_same_value;
}known_address_information, *Pknown_address_information;

typedef struct refresh_address_information {
    PLinked_list_from_searching From_Ioctl_requestor; // User -> Kernel 
}refresh_address_information, *Prefresh_address_information;

typedef struct editing_address_information {

    PUCHAR edit_destination_Targetprocess_address;

    // ioctl_data_for_editing  ----->>> edit_destination_Targetprocess_address 주소에 삽입 
    PUCHAR ioctl_data_for_editing; // 이 주소의 데이터는 타겟프로세스 주소에 copy될 것.
    SIZE_T ioctl_data_for_editing_SIZE;

    BOOLEAN is_same_value;

}editing_address_information, *Pediting_address_information;

typedef struct DUMP_information {
    PMEM_DUMP_NODE To_Ioctl_requestor;// Kernel -> User
}DUMP_information, *PDUMP_information;


typedef enum IOCTL_command {

    loop__Check= 1,  // 유저모드에서 지속적으로 커널에게 체크하면서 ( "JOB_QUEUE'리턴 값 회수시도 ) 


    ATTACH,// user -> kernel 어태치 ( 커널에서 주소접근 가능한가? ) 



    general_scan, // user -( PID )-> kernel
    dump, // 커널단에서 덤프
    refresh, // IOCTL 요청자에서 알고 있는 타겟프로세스 주소의 데이터를 다시한번 조회하는 것.

    editing, // user -(PID + data_for_edit) -> kernel


    /* 응답용 */
    ioctl_true,
    ioctl_false

}IOCTL_command;

typedef struct IOCTL_info {

    IOCTL_command information;

    target_process_information target_process_info; // 타겟 프로세스에 대한 정보 + 검색할 데이터 

    PLinked_list_from_searching Start_Node_Address; // IOCTL 요청자에게 동적할당될 연결리스트 시작주소  / Kernel -> User 

    HANDLE Ioctl_requestor_PID; // 유저 컨텍스트 접근을 위한 PID 



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


}IOCTL_info, * PIOCTL_info;


#include "JOB_QUEUE_Manger.h"


#include "KEVENT_or_KMUTEX_init.h"



extern K_EVENT_or_MUTEX_struct IOCTL_thread_KEVENT;

NTSTATUS MyDeviceControl(
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp
);

#endif