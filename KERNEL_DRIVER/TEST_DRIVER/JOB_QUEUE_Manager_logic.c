#include "JOB_QUEUE_Manger.h"


K_EVENT_or_MUTEX_struct JOB_mutex = { NULL, K_MUTEX, FALSE };

PJOB_QUEUE_NODE Start_Address = NULL;
PJOB_QUEUE_NODE Current_Address = NULL;


BOOLEAN Create_or_Append_Job_Queue_Node(PJOB_QUEUE To_IOCTL_DATA) {

	/*
		To_IOCTL_DATA 는 이미 동적할당되어 있어야만 한다.
	*/

	K_object_init_check_also_lock_ifyouwant(&JOB_mutex, TRUE);




	if (Start_Address == NULL) {
		Start_Address = Create_Job_Queue_Node(To_IOCTL_DATA);
		Current_Address = Start_Address;
	}
	else {
		Current_Address = Append_Job_Queue_Node(Current_Address, To_IOCTL_DATA);
	}






	if (Current_Address) {
		K_object_lock_Release(&JOB_mutex);
		return TRUE;
	}
	else {
		K_object_lock_Release(&JOB_mutex);
		return FALSE;
	}

}

// 연결리스트 큐에 있는 데이터를 외부로 반환 
PJOB_QUEUE Receive_Job_for_transmit_to_the_IOCTL_USER() {
	PJOB_QUEUE output_data = NULL;
	K_object_init_check_also_lock_ifyouwant(&JOB_mutex, TRUE);

	if (Start_Address == NULL) {
		K_object_lock_Release(&JOB_mutex);
		return NULL;
	}

	output_data = Start_Address->To_IOCTL_DATA; // IOCTL에게 전달할 데이터 
	PJOB_QUEUE_NODE remember = Start_Address;
	Start_Address = (PJOB_QUEUE_NODE)Start_Address->Next_Addr; // 이제 Start_Address 는 큐에서 나감

	/* 노드 하나만 동적할당하고, IOCTL에게 전달할 데이터는 외부에서 할당해제 해야함 */
	ExFreePoolWithTag(remember, 'JOB');

	K_object_lock_Release(&JOB_mutex);
	return output_data; // NULL일 수도 있음 ! 
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


PJOB_QUEUE_NODE Create_Job_Queue_Node(PJOB_QUEUE To_IOCTL_DATA) {
	PJOB_QUEUE_NODE New_Node = ExAllocatePoolWithTag(NonPagedPool, sizeof(JOB_QUEUE_NODE), 'JOB');
	if (New_Node == NULL) return NULL;


	New_Node->To_IOCTL_DATA = To_IOCTL_DATA;

	New_Node->Next_Addr = NULL;

	return New_Node;
}

PJOB_QUEUE_NODE Append_Job_Queue_Node(PJOB_QUEUE_NODE Current_Node, PJOB_QUEUE To_IOCTL_DATA) {
	if (Current_Node == NULL) return NULL;

	Current_Node->Next_Addr = (PUCHAR) Create_Job_Queue_Node(To_IOCTL_DATA);

	return (PJOB_QUEUE_NODE)Current_Node->Next_Addr;
}
