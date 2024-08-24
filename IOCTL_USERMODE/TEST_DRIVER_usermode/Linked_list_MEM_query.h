#ifndef Linked_list_MEM_query
#define Linked_list_MEM_query

#include <stdio.h>
#include <Windows.h>

typedef struct Linked_list_from_searching {

    PUCHAR previous_addr;

    SIZE_T node_index;

    PUCHAR Searched__target_process_virtual_memory; // 이 주소는 타겟 프로세스 주소임 
    SIZE_T Searched_memory_data_size; // 데이터 길이

    BOOLEAN is_this_Pointer; // 이 주소의 값이 포인터인가? 


    PUCHAR next_addr;

}Linked_list_from_searching, * PLinked_list_from_searching;



#endif // !Linked_list_MEM_query
