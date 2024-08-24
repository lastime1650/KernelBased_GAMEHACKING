#ifndef  MEM_HACK_LIST_H
#define MEM_HACK_LIST_H

#include <ntifs.h>
/*

    [ATTENTION]
        �� ���Ḯ��Ʈ�� IOCTL ��û�� ������忡�� ����� ����


*/
// ���Ḯ��Ʈ ( �� ����ü �ּҴ� IOCTL������忡���� ��ȿ�� ) 
typedef struct Linked_list_from_searching {

    PUCHAR previous_addr;

    SIZE_T node_index;

    PUCHAR Searched__target_process_virtual_memory; // �� �ּҴ� Ÿ�� ���μ��� �ּ��� 
    SIZE_T Searched_memory_data_size; // ������ ����

    BOOLEAN is_this_Pointer; // �� �ּ��� ���� �������ΰ�? 


    PUCHAR next_addr;

}Linked_list_from_searching, * PLinked_list_from_searching;


PLinked_list_from_searching Create__Linked_list_for_searching(PLinked_list_from_searching previous_addr, PUCHAR Searched__target_process_virtual_memory, SIZE_T Searched_memory_data_size, BOOLEAN is_this_Pointer, HANDLE IOCTL_requestor_HANDLE, PEPROCESS IOCTL_requestor_eprocess, PEPROCESS SYSTEM_eprocess, SIZE_T node_index);

PLinked_list_from_searching Append__Linked_list_for_searching(PLinked_list_from_searching current_addr, PUCHAR Searched__target_process_virtual_memory, SIZE_T Searched_memory_data_size, BOOLEAN is_this_Pointer, HANDLE IOCTL_requestor_HANDLE, PEPROCESS IOCTL_requestor_eprocess, PEPROCESS SYSTEM_eprocess, SIZE_T node_index);

// IOCTL ��û�ڷκ��� ���� "�����ּ��� ���Ḯ��Ʈ �����ּ�"�� �޾� ������ ��ȸ.
PLinked_list_from_searching query_for_Refresh(PLinked_list_from_searching Start_Address, HANDLE IOCTL_requestor_HANDLE, PEPROCESS IOCTL_requestor_eprocess, PEPROCESS SYSTEM_eprocess);


#endif // ! MEM_HACK_LIST_H
