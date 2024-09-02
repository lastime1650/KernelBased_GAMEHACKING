#ifndef JOB_QUEUE_M
#define JOB_QUEUE_M

#include <ntifs.h>
#include "IOCTL.h"
#include "KEVENT_or_KMUTEX_init.h"

typedef struct JOB_QUEUE_STRUCT {

    IOCTL_info INPUT_DATA;

    BOOLEAN is_finished_job; // 작업이 끝났는가? ( 초기엔 FALSE ) 

    PUCHAR This_Node_Start_Address; // 작업번호 대체재 ( 이 구조체의 동적할당된 주소/ 작업 결과를 적용하려면 넣어야함 (비동기이기 때문) ) 

    


    HANDLE Usermode_Mutex;


    PIOCTL_info OUTPUT_DATA; //커널에서 동적할당하여 유저에게 반환

}JOB_QUEUE, * PJOB_QUEUE;

typedef struct MOVE {
    K_EVENT_or_MUTEX_struct* KEVENT;
    PJOB_QUEUE DATA;
}MOVE, * PMOVE;

typedef struct JOB_QUEUE_NODE {

    PJOB_QUEUE To_IOCTL_DATA;

    PUCHAR Next_Addr;
}JOB_QUEUE_NODE, *PJOB_QUEUE_NODE;

BOOLEAN Create_or_Append_Job_Queue_Node(PJOB_QUEUE To_IOCTL_DATA);

PJOB_QUEUE_NODE Create_Job_Queue_Node( PJOB_QUEUE To_IOCTL_DATA);

PJOB_QUEUE_NODE Append_Job_Queue_Node(PJOB_QUEUE_NODE Current_Node, PJOB_QUEUE To_IOCTL_DATA);

PJOB_QUEUE Receive_Job_for_transmit_to_the_IOCTL_USER(); // 어짜피 시작주소만 가져오면 됨 ( 시작주소를 다음 주소로 옮기는 작업 필수 ) 


#endif
