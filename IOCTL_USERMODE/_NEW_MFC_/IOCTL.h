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



/* ����ü */
// Ÿ�� ���μ����� ���� �ּ����� ���� ( ��ĵ�� �� ������ �� ) 
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

    PUCHAR ioctl_data_for_editing; // �� �ּ��� �����ʹ� Ÿ�����μ��� �ּҿ� copy�� ��.
    SIZE_T ioctl_data_for_editing_SIZE;

    BOOLEAN is_same_value; // ���Ȯ�ο� 

}editing_address_information, * Pediting_address_information;

typedef struct DUMP_information {
    PMEM_DUMP_NODE To_Ioctl_requestor;// Kernel -> User
}DUMP_information, * PDUMP_information;

// �޸� �� ���� ����ü
typedef struct MEM_VIEWR{
    PUCHAR BaseAddress_of_TargetProcess; 
    SIZE_T Scan_SIZE; // �� ���̸�ŭ �о <Ÿ�ٰ����ּ��� �����͸� ������,> IOCTL �����ּҿ� �Ҵ��� ��.
    
    PUCHAR Output_Allocated_IOCTL_Address_for_return; // �޸� �並 ���� ������ ���� < �ּҰ� ��ȿ���� �ʾҴٸ� NULL���� �� >
}MEM_VIEWR, *PMEM_VIEWR;


// �޸� ��ü ������ ( MEM_COMMIT �ϋ��� ��ü ) 
typedef struct MEM_PAGE {
    PPAGE_SCAN_NODE Output_Ioctl_user_Start_Address;
}MEM_PAGE, * PMEM_PAGE;



// �޸� ��ĳ��
typedef enum MEM_SCANNING_ENUM {
    FIRST_SCAN = 1,
    NEXT_SCAN
}MEM_SCANNING_ENUM;
typedef struct MEM_SCANNING_FIRST_SCAN {
    /* �� �ʵ�� IOCTL ��û ���μ����� �����Ҵ�� �ּ� */
    PUCHAR IOCTL_allocated_addr__for_data_scan;
    SIZE_T IOCTL_allocated_size__for_data_scan;
}MEM_SCANNING_FIRST_SCAN, * PMEM_SCANNING_FIRST_SCAN;


typedef struct MEM_SCANNING_NEXT_SCAN {

    /* �� �ʵ�� IOCTL ��û ���μ����� �����Ҵ�� �ּ� */
    PUCHAR IOCTL_allocated_addr__for_data_scan;
    SIZE_T IOCTL_allocated_size__for_data_scan;

    // IOCTL �����ּҿ� �Ҵ�� ���Ḯ��Ʈ( �̹� Ÿ���ּҸ� �˰� �ִ� ������ ������带 Ŀ�ο��� ��ȸ.)
    PLinked_list_from_searching Link__Start_Address_for_Kernel;
}MEM_SCANNING_NEXT_SCAN, * PMEM_SCANNING_NEXT_SCAN;


typedef struct MEM_SCANNING {
    MEM_SCANNING_ENUM Scan_mode;

    MEM_SCANNING_FIRST_SCAN first_scan_info;

    MEM_SCANNING_NEXT_SCAN next_scan_info;

    // Ŀ�ο��� IOCTL���μ��� �����ּҿ� ����� ���Ḯ��Ʈ�� ������.
    PLinked_list_from_searching Link__Start_Address_for_IOCTL; // Output
}MEM_SCANNING, * PMEM_SCANNING;


/* Ŀ�α�� DLL ������ */
typedef struct Kernel_Dll_Injection {

    PUCHAR LoadLibrary_Address; // Ansi or Wide

    PUCHAR DLL_PATH__from__ioctl_virtualmem; // dll ���ڿ��� ioctl �������α׷��� �����Ѵ�.
    ULONG32 DLL_PATH_length;



    /* Output*/
    BOOLEAN is_SUCCESS;//output
    HANDLE TargetProcess_run_threadID; // output

}Kernel_Dll_Injection, * PKernel_Dll_Injection;


/*IOCTL ��û�ڿ����� �ϵ����BP ���� */
typedef struct HardWare_BP_Set {

    ULONG64 Monitor_TargetProcess_Address;

    BOOLEAN is_SUCCESS;
}HardWare_BP_Set, * PHardWare_BP_Set; // Ŀ�ο��� �ϵ���� BP �ɱ� 

typedef struct HardWare_BP_Get {
    PHardWare_BP_struct_from_targetprocess OUTPUT; // IOCTL �����ּҿ� �����Ҵ� 
}HardWare_BP_Get, * PHardWare_BP_Get; // Ŀ�ο��� �ϵ���� BP �������� 

typedef struct HardWare_BP_info {

    HardWare_BP_Set SET; // �ϵ����BP�� Ÿ�����μ����� �ɱ� 
    HardWare_BP_Get GET; // �ϵ����BP�� ���Ḯ��Ʈ ��ȸ�Ͽ� ��������

}HardWare_BP_info, * PHardWare_BP_info;

typedef enum IOCTL_command {

    loop__Check = 1,  // ������忡�� ���������� Ŀ�ο��� üũ�ϸ鼭 ( "JOB_QUEUE'���� �� ȸ���õ� ) 


    /* Ÿ�� ���μ��� ��û info*/
    Hardware_breakpoint_response, // �ϵ���� �ߴ��� Ȯ�ο�





    /* IOCTL ��û�� ���μ��� ��û info  */
    ATTACH,// user -> kernel ����ġ ( Ŀ�ο��� �ּ����� �����Ѱ�? ) 

    Memory_Viewer, // �޸� �並 ���� ������ ��û ( ���� ) -> ( Ÿ�� ���μ��� ������ copy )-> ( IOCTL ���� ) 

    general_scan, // user -( PID )-> kernel
    dump, // Ŀ�δܿ��� ����
    refresh, // IOCTL ��û�ڿ��� �˰� �ִ� Ÿ�����μ��� �ּ��� �����͸� �ٽ��ѹ� ��ȸ�ϴ� ��.

    editing, // user -(PID + data_for_edit) -> kernel

    Memory_Page_Scan, // Ÿ�� ���μ����� �������� ��� ��ĵ�� ����� IOCTL ���� ���μ��� ������ ���Ḯ��Ʈ�� ������ 

    Kernel_Based_DLL_Injection, // Ŀ�ο��� DLL ������

    /* �ϵ���� BP ���� BP */
    Hardware_breakpoint_set, // Ŀ�ο��� �ϵ����BP �ɶ� 
    Hardware_breakpoint_request_for_GET,// IOCTL��û�ڿ��� ���Ḯ��Ʈ ��� �����͸� �����ö� 


    /* ����� */
    ioctl_true,
    ioctl_false

}IOCTL_command;

typedef struct IOCTL_info {

    IOCTL_command information;


    target_process_information target_process_info; // Ÿ�� ���μ����� ���� ���� + �˻��� ������ 

    PLinked_list_from_searching Start_Node_Address; // IOCTL ��û�ڿ��� �����Ҵ�� ���Ḯ��Ʈ �����ּ�  / Kernel -> User 

    HANDLE Ioctl_requestor_PID; // ���� ���ؽ�Ʈ ������ ���� PID 

    // �޸� ��ĵ�� First_Scan �׸��� Next_Scan ����
    MEM_SCANNING Mem_Scanning_info;

    // Ŀ�α�� DLL �����ǿ� ����ü
    Kernel_Dll_Injection DLL_info;

    // �޸� ������ ��ĵ ����
    MEM_PAGE Mem_Page_scan_viewer;

    // �޸� ��� ���� 
    MEM_VIEWR Mem_Viewer;// �޸� ��� ���� Ŀ�ο��� ��û�� ����ü  

    // Scan �ʼ� ���� 
    PUCHAR ioctl_data_for_search; // IOCTL ��û�� ��������� �����ּ��̸�, �� �����ʹ� �˻��� ���� �����Ͱ� ����. 
    SIZE_T ioctl_data_for_search_SIZE; // ioctl_data_for_search �� ������ 

    // DUMP ���� ( Ŀ���� ioctl���� �����Ҵ�� �ּҸ� �˷��ٶ� ��� ) 
    DUMP_information dump_info;

    // Next_Scan �Ǵ� �̹� �ּҸ� �˰� ���� ��, 
    known_address_information known_data; // user->kernel->user

    // Refresh ��
    refresh_address_information refresh_info;

    // Editing ( ������ ���� ) 
    editing_address_information editing_info;

    // �ϵ���� BP �� ( Get,Set ��� ���� ) 
    HardWare_BP_info HWBP_info;

    HANDLE IOCTL_User_Mutex;
}IOCTL_info, * PIOCTL_info;


#define IOCTL_MY_IOCTL_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

extern HANDLE* IOCTL_hDevice;

typedef struct JOB_QUEUE_STRUCT {

    IOCTL_info INPUT_DATA;

    BOOLEAN is_finished_job; // �۾��� �����°�? ( �ʱ⿣ FALSE ) 

    PUCHAR This_Node_Start_Address; // �۾���ȣ ��ü�� ( �� ����ü�� �����Ҵ�� �ּ�/ �۾� ����� �����Ϸ��� �־���� (�񵿱��̱� ����) ) 

   

    
    HANDLE Usermode_Mutex;
    

    PIOCTL_info OUTPUT_DATA; //Ŀ�ο��� �����Ҵ��Ͽ� �������� ��ȯ

}JOB_QUEUE, * PJOB_QUEUE;


#ifdef __cplusplus
extern "C" {
#endif

    BOOLEAN Send_to_Kernel(PIOCTL_info DATA, BOOLEAN is_Waiting, PIOCTL_info* Output_IOCTL_info, PJOB_QUEUE* Output_JOB_QUEUE); // Ŀ�ο��� �����ϰ�, �����Ҵ�� ���� ��� 

    PIOCTL_info STill_Checking_a_Job_when_finish_it(PJOB_QUEUE Input_your_Job); // Send_to_Kernel �� �Ϸ�ɶ����� �ݺ� ��ȸ ( �Ϸ�Ǹ� (PIOCTL_info ������ �����ȯ ) ���� ) 

    DWORD WINAPI Kernel_LoopCheck(PVOID unused);

#ifdef __cplusplus
	}
#endif

#endif // ! IOCTL_H
