#include "pch.h"
#include "IOCTL.h"
#include "afxdialogex.h"
HANDLE* IOCTL_hDevice = NULL;

HANDLE* IOCTL_MUTEX = NULL;


DWORD WINAPI SendKernel(PJOB_QUEUE JOB) {

	//뮤텍스 
	if (IOCTL_MUTEX == NULL) {
		IOCTL_MUTEX = (HANDLE*)malloc(sizeof(HANDLE));
		if (IOCTL_MUTEX == NULL) {
			JOB->is_finished_job = TRUE;
			return -1;
		}
		*IOCTL_MUTEX = CreateMutexA(NULL, FALSE, "MUTEEX");
	}

	// IOCTL 디바이스
	if (IOCTL_hDevice == NULL) {
		IOCTL_hDevice = (HANDLE*)malloc(sizeof(HANDLE));
		if (IOCTL_hDevice == NULL) {
			JOB->is_finished_job = TRUE;
			return -1;
		}

		
	}

	

	// IOCTL 접근시도
	*IOCTL_hDevice = CreateFile(TEXT("\\??\\My_TEST_SYM_Device"),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (*IOCTL_hDevice == INVALID_HANDLE_VALUE) {
		printf("Failed to obtain handle to driver. Error: %d\n", GetLastError());
		JOB->is_finished_job = TRUE;
		return -1;
	}


	// 뮤텍스 점유
	WaitForSingleObject(*IOCTL_MUTEX, INFINITE);


	JOB_QUEUE Output_DATA;

	DWORD bytesReturned;

	// IOCTL 요청 보내기
	BOOL success = DeviceIoControl(*IOCTL_hDevice,
		IOCTL_MY_IOCTL_CODE,
		JOB,
		sizeof(JOB_QUEUE),
		&Output_DATA,
		sizeof(JOB_QUEUE),
		&bytesReturned,
		NULL
	);

	// 일단 작업큐를 제대로 받아왔는 지 확인.
	if (bytesReturned != 0) {
		// 받아왔을 때, TRUE인 경우, 이것을 요청한 "어느"(특정되지않은) 작업을 대기하고 있는 주소에 "복사"
		if (Output_DATA.is_finished_job) {

			// 작업완료처리 
			// 이미 "This_Node_Start_Address"에는 초기 동적할당된 구조체가 있으므로, 접근이 가능하다. 
			WaitForSingleObject( ((JOB_QUEUE*)Output_DATA.This_Node_Start_Address)->Usermode_Mutex, INFINITE);

			((JOB_QUEUE*)Output_DATA.This_Node_Start_Address)->OUTPUT_DATA = Output_DATA.OUTPUT_DATA; // Output_DATA.OUTPUT_DATA 값은 동적할당된 주소임 
			((JOB_QUEUE*)Output_DATA.This_Node_Start_Address)->is_finished_job = TRUE;


			ReleaseMutex(((JOB_QUEUE*)Output_DATA.This_Node_Start_Address)->Usermode_Mutex);
		}
	}

	

	// 뮤텍스 해제
	ReleaseMutex(*IOCTL_MUTEX);

	return 0;
}



BOOLEAN Send_to_Kernel(PIOCTL_info DATA, BOOLEAN is_Waiting, PIOCTL_info* Output_IOCTL_info, PJOB_QUEUE* Output_JOB_QUEUE ) {

	/*
		is_Waiting == TRUE 
			Output_JOB_QUEUE -> NULL
			Output_IOCTL_info -> valid
		
		is_Waiting == FALSE
			Output_JOB_QUEUE -> valid
			Output_IOCTL_info -> NULL

	*/

	PJOB_QUEUE allocate_job = (PJOB_QUEUE)malloc(sizeof(JOB_QUEUE));
	memset(allocate_job, 0, sizeof(JOB_QUEUE));
	if (allocate_job == NULL) return FALSE;

	memcpy(&allocate_job->INPUT_DATA, DATA, sizeof(IOCTL_info));

	allocate_job->OUTPUT_DATA = NULL;
	allocate_job->is_finished_job = FALSE;
	allocate_job->This_Node_Start_Address = (PUCHAR)allocate_job;
	allocate_job->Usermode_Mutex = CreateMutexA(NULL, FALSE, "JOB");



	HANDLE thread_handle =  CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendKernel, allocate_job, 0, NULL);
	
	if (Output_IOCTL_info) {
		*Output_IOCTL_info = NULL;
	}
	if (Output_JOB_QUEUE) {
		*Output_JOB_QUEUE = NULL;
	}
	

	if (is_Waiting) {
		/*
			커널의 응답을 무조건 기다림 
				--> 커널에서 동적할당한 IOCTL 주소 반환 
		*/
		*Output_IOCTL_info = STill_Checking_a_Job_when_finish_it(allocate_job);
		if (*Output_IOCTL_info == NULL) {
			return FALSE;
		}

	}
	else {
		/*
			커널의 응답을 기다리지 않음 
				--> 큐 구조체 포인터 반환
		*/
		*Output_JOB_QUEUE = allocate_job;
		if (*Output_JOB_QUEUE == NULL) {
			free(allocate_job);
			return FALSE;
		}
	}

	return TRUE;

}


// 지속확인.
PIOCTL_info STill_Checking_a_Job_when_finish_it(PJOB_QUEUE Input_your_Job) {

	while (TRUE) {
		WaitForSingleObject(Input_your_Job->Usermode_Mutex, INFINITE);
		if (Input_your_Job->is_finished_job) {



			PIOCTL_info OUTPUT_DATA = Input_your_Job->OUTPUT_DATA;

			ReleaseMutex(Input_your_Job->Usermode_Mutex);
			free(Input_your_Job);
			return OUTPUT_DATA; // 커널에서 원격으로 할당된 새로운 반환공간임 
		}
		ReleaseMutex(Input_your_Job->Usermode_Mutex);
	}

}


// Loop_Check -- 작업 회수용
DWORD WINAPI Kernel_LoopCheck(PVOID unused) {
	
	while (1) {
		PJOB_QUEUE check_test = (PJOB_QUEUE)malloc(sizeof(JOB_QUEUE));
		memset(check_test, 0, sizeof(JOB_QUEUE));
		check_test->INPUT_DATA.information = loop__Check;

		SendKernel(check_test);
		Sleep(1000);
	}
}