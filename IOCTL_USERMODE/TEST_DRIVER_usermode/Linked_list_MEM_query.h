#ifndef Linked_list_MEM_query
#define Linked_list_MEM_query

#include <stdio.h>
#include <Windows.h>

typedef struct Linked_list_from_searching {

    PUCHAR previous_addr;

    SIZE_T node_index;

    PUCHAR Searched__target_process_virtual_memory; // �� �ּҴ� Ÿ�� ���μ��� �ּ��� 
    SIZE_T Searched_memory_data_size; // ������ ����

    BOOLEAN is_this_Pointer; // �� �ּ��� ���� �������ΰ�? 


    PUCHAR next_addr;

}Linked_list_from_searching, * PLinked_list_from_searching;



#endif // !Linked_list_MEM_query
