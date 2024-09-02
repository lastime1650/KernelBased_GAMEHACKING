#include "pch.h"
#include "IOCTL.h"
#include "afxdialogex.h"
HANDLE* IOCTL_hDevice = NULL;

HANDLE* IOCTL_MUTEX = NULL;


DWORD WINAPI SendKernel(PJOB_QUEUE JOB) {

	//���ؽ� 
	if (IOCTL_MUTEX == NULL) {
		IOCTL_MUTEX = (HANDLE*)malloc(sizeof(HANDLE));
		if (IOCTL_MUTEX == NULL) {
			JOB->is_finished_job = TRUE;
			return -1;
		}
		*IOCTL_MUTEX = CreateMutexA(NULL, FALSE, "MUTEEX");
	}

	// IOCTL ����̽�
	if (IOCTL_hDevice == NULL) {
		IOCTL_hDevice = (HANDLE*)malloc(sizeof(HANDLE));
		if (IOCTL_hDevice == NULL) {
			JOB->is_finished_job = TRUE;
			return -1;
		}

		
	}

	

	// IOCTL ���ٽõ�
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


	// ���ؽ� ����
	WaitForSingleObject(*IOCTL_MUTEX, INFINITE);


	JOB_QUEUE Output_DATA;

	DWORD bytesReturned;

	// IOCTL ��û ������
	BOOL success = DeviceIoControl(*IOCTL_hDevice,
		IOCTL_MY_IOCTL_CODE,
		JOB,
		sizeof(JOB_QUEUE),
		&Output_DATA,
		sizeof(JOB_QUEUE),
		&bytesReturned,
		NULL
	);

	// �ϴ� �۾�ť�� ����� �޾ƿԴ� �� Ȯ��.
	if (bytesReturned != 0) {
		// �޾ƿ��� ��, TRUE�� ���, �̰��� ��û�� "���"(Ư����������) �۾��� ����ϰ� �ִ� �ּҿ� "����"
		if (Output_DATA.is_finished_job) {

			// �۾��Ϸ�ó�� 
			// �̹� "This_Node_Start_Address"���� �ʱ� �����Ҵ�� ����ü�� �����Ƿ�, ������ �����ϴ�. 
			WaitForSingleObject( ((JOB_QUEUE*)Output_DATA.This_Node_Start_Address)->Usermode_Mutex, INFINITE);

			((JOB_QUEUE*)Output_DATA.This_Node_Start_Address)->OUTPUT_DATA = Output_DATA.OUTPUT_DATA; // Output_DATA.OUTPUT_DATA ���� �����Ҵ�� �ּ��� 
			((JOB_QUEUE*)Output_DATA.This_Node_Start_Address)->is_finished_job = TRUE;


			ReleaseMutex(((JOB_QUEUE*)Output_DATA.This_Node_Start_Address)->Usermode_Mutex);
		}
	}

	

	// ���ؽ� ����
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
			Ŀ���� ������ ������ ��ٸ� 
				--> Ŀ�ο��� �����Ҵ��� IOCTL �ּ� ��ȯ 
		*/
		*Output_IOCTL_info = STill_Checking_a_Job_when_finish_it(allocate_job);
		if (*Output_IOCTL_info == NULL) {
			return FALSE;
		}

	}
	else {
		/*
			Ŀ���� ������ ��ٸ��� ���� 
				--> ť ����ü ������ ��ȯ
		*/
		*Output_JOB_QUEUE = allocate_job;
		if (*Output_JOB_QUEUE == NULL) {
			free(allocate_job);
			return FALSE;
		}
	}

	return TRUE;

}


// ����Ȯ��.
PIOCTL_info STill_Checking_a_Job_when_finish_it(PJOB_QUEUE Input_your_Job) {

	while (TRUE) {
		WaitForSingleObject(Input_your_Job->Usermode_Mutex, INFINITE);
		if (Input_your_Job->is_finished_job) {



			PIOCTL_info OUTPUT_DATA = Input_your_Job->OUTPUT_DATA;

			ReleaseMutex(Input_your_Job->Usermode_Mutex);
			free(Input_your_Job);
			return OUTPUT_DATA; // Ŀ�ο��� �������� �Ҵ�� ���ο� ��ȯ������ 
		}
		ReleaseMutex(Input_your_Job->Usermode_Mutex);
	}

}


// Loop_Check -- �۾� ȸ����
DWORD WINAPI Kernel_LoopCheck(PVOID unused) {
	
	while (1) {
		PJOB_QUEUE check_test = (PJOB_QUEUE)malloc(sizeof(JOB_QUEUE));
		memset(check_test, 0, sizeof(JOB_QUEUE));
		check_test->INPUT_DATA.information = loop__Check;

		SendKernel(check_test);
		Sleep(1000);
	}
}