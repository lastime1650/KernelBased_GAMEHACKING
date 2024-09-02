#ifndef JOB_QUEUE_M
#define JOB_QUEUE_M

#include <ntifs.h>
#include "IOCTL.h"
#include "KEVENT_or_KMUTEX_init.h"

typedef struct JOB_QUEUE_STRUCT {

    IOCTL_info INPUT_DATA;

    BOOLEAN is_finished_job; // �۾��� �����°�? ( �ʱ⿣ FALSE ) 

    PUCHAR This_Node_Start_Address; // �۾���ȣ ��ü�� ( �� ����ü�� �����Ҵ�� �ּ�/ �۾� ����� �����Ϸ��� �־���� (�񵿱��̱� ����) ) 

    


    HANDLE Usermode_Mutex;


    PIOCTL_info OUTPUT_DATA; //Ŀ�ο��� �����Ҵ��Ͽ� �������� ��ȯ

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

PJOB_QUEUE Receive_Job_for_transmit_to_the_IOCTL_USER(); // ��¥�� �����ּҸ� �������� �� ( �����ּҸ� ���� �ּҷ� �ű�� �۾� �ʼ� ) 


#endif
