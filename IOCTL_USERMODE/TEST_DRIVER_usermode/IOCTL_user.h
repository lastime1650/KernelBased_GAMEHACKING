#ifndef IOCTL_user
#define IOCTL_user

#include <stdio.h>
#include <Windows.h>
#include "Linked_list_MEM_query.h"
#include "MEM_DUMP.h"
#define IOCTL_MY_IOCTL_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

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

typedef enum IOCTL_command {

    general_scan = 1, // user -( PID )-> kernel
    dump, // Ŀ�δܿ��� ����
    refresh, // IOCTL ��û�ڿ��� �˰� �ִ� Ÿ�����μ��� �ּ��� �����͸� �ٽ��ѹ� ��ȸ�ϴ� ��.

    editing // user -(PID + data_for_edit) -> kernel


}IOCTL_command;

typedef struct IOCTL_info {

    IOCTL_command information;

    target_process_information target_process_info; // Ÿ�� ���μ����� ���� ���� + �˻��� ������ 

    PLinked_list_from_searching Start_Node_Address; // IOCTL ��û�ڿ��� �����Ҵ�� ���Ḯ��Ʈ �����ּ�  / Kernel -> User 

    HANDLE Ioctl_requestor_PID; // ���� ���ؽ�Ʈ ������ ���� PID 



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


}IOCTL_info, * PIOCTL_info;


BOOL Send_to_Kernel(HANDLE hDevice, PIOCTL_info Input_data, PIOCTL_info output_data);


BOOLEAN query_print__Linked_list_for_searching(PLinked_list_from_searching Start_Address);



BOOLEAN query_editing__Linked_list_for_searching(PLinked_list_from_searching Start_Address, SIZE_T selected_node_index, PUCHAR editing_data, SIZE_T editing_data_Size, HANDLE hDevice, PIOCTL_info send_data_original);

VOID free_pool_Linked_list_for_searching(PLinked_list_from_searching Start_Address);

#endif