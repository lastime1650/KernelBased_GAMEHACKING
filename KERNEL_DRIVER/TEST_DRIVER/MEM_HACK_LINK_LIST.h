#ifndef  MEM_HACK_LIST_H
#define MEM_HACK_LIST_H

#include <ntifs.h>
/*

    [ATTENTION]
        이 연결리스트는 IOCTL 요청자 유저모드에서 만드는 것임


*/
// 연결리스트 ( 이 구조체 주소는 IOCTL유저모드에서만 유효함 ) 
typedef struct Linked_list_from_searching {

    PUCHAR previous_addr;

    SIZE_T node_index;

    PUCHAR Searched__target_process_virtual_memory; // 이 주소는 타겟 프로세스 주소임 
    SIZE_T Searched_memory_data_size; // 데이터 길이

    BOOLEAN is_this_Pointer; // 이 주소의 값이 포인터인가? 


    PUCHAR next_addr;

}Linked_list_from_searching, * PLinked_list_from_searching;


PLinked_list_from_searching Create__Linked_list_for_searching(PLinked_list_from_searching previous_addr, PUCHAR Searched__target_process_virtual_memory, SIZE_T Searched_memory_data_size, BOOLEAN is_this_Pointer, HANDLE IOCTL_requestor_HANDLE, PEPROCESS IOCTL_requestor_eprocess, PEPROCESS SYSTEM_eprocess, SIZE_T node_index);

PLinked_list_from_searching Append__Linked_list_for_searching(PLinked_list_from_searching current_addr, PUCHAR Searched__target_process_virtual_memory, SIZE_T Searched_memory_data_size, BOOLEAN is_this_Pointer, HANDLE IOCTL_requestor_HANDLE, PEPROCESS IOCTL_requestor_eprocess, PEPROCESS SYSTEM_eprocess, SIZE_T node_index);

// IOCTL 요청자로부터 받은 "가상주소인 연결리스트 시작주소"를 받아 재차례 조회.
PLinked_list_from_searching query_for_Refresh(PLinked_list_from_searching Start_Address, HANDLE IOCTL_requestor_HANDLE, PEPROCESS IOCTL_requestor_eprocess, PEPROCESS SYSTEM_eprocess);


#endif // ! MEM_HACK_LIST_H
