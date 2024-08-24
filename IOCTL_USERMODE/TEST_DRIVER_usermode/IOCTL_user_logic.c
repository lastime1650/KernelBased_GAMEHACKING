#include "IOCTL_user.h"

BOOL Send_to_Kernel(HANDLE hDevice, PIOCTL_info Input_data, PIOCTL_info output_data) {

    DWORD bytesReturned;

    // IOCTL 요청 보내기
    BOOL success = DeviceIoControl(hDevice,
        IOCTL_MY_IOCTL_CODE,
        Input_data,
        sizeof(IOCTL_info),
        output_data,
        sizeof(IOCTL_info),
        &bytesReturned,
        NULL
    );

    return success;

}


//////////
//////////
//////////


BOOLEAN query_print__Linked_list_for_searching(PLinked_list_from_searching Start_Address) {
	if (Start_Address == NULL) return FALSE;

	PLinked_list_from_searching current = Start_Address;

	while (current != NULL) {

		printf(" [찾은데이터] [%llu] 타겟프로세스주소: %p  / 탐색데이터 길이: %llu  /  이 값은 포인터인가?: %d  \n", current->node_index, current->Searched__target_process_virtual_memory, current->Searched_memory_data_size, current->is_this_Pointer);

		current = (PLinked_list_from_searching)current->next_addr;
	}

	return TRUE;
}


BOOLEAN query_editing__Linked_list_for_searching(PLinked_list_from_searching Start_Address, SIZE_T selected_node_index, PUCHAR editing_data, SIZE_T editing_data_Size, HANDLE hDevice, PIOCTL_info send_data_original) {
	if (Start_Address == NULL) return FALSE;

	PLinked_list_from_searching current = Start_Address;

	/* 사전 정보 등록 */
	send_data_original->information = editing;
	send_data_original->editing_info.ioctl_data_for_editing = editing_data;
	send_data_original->editing_info.ioctl_data_for_editing_SIZE = editing_data_Size;
	send_data_original->editing_info.is_same_value = FALSE;

	while (current != NULL) {

		if (current->node_index == selected_node_index) {
			printf(" [찾은데이터] [%llu] 타겟프로세스주소: %p  / 탐색데이터 길이: %llu  /  이 값은 포인터인가?: %d  \n", current->node_index, current->Searched__target_process_virtual_memory, current->Searched_memory_data_size, current->is_this_Pointer);

			// 노드로부터 얻은 데이터를 통하여 얻음
			send_data_original->editing_info.edit_destination_Targetprocess_address = current->Searched__target_process_virtual_memory;

			Send_to_Kernel(hDevice, send_data_original, send_data_original);

			printf("editing 결과 ->>>> %d \n", send_data_original->editing_info.is_same_value);

		}

		current = (PLinked_list_from_searching)current->next_addr;
	}

	return TRUE;
}






VOID free_pool_Linked_list_for_searching(PLinked_list_from_searching Start_Address) {



	return;
}