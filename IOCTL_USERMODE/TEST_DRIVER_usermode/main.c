#include <stdio.h>
#include <Windows.h>

#include "IOCTL_user.h"



#include "FILE_IO.h"


int main() {
    HANDLE my_pid = (HANDLE)GetCurrentProcessId();

    HANDLE hDevice = 0;
    hDevice = CreateFile(TEXT("\\??\\My_TEST_SYM_Device"),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);
    printf("%ld\n", hDevice);
    system("pause");
    if (hDevice == INVALID_HANDLE_VALUE) {
        printf("Failed to obtain handle to driver. Error: %d\n", GetLastError());
        system("pause");
        return -1;
    }

    ///

    ULONG32 data = 1650;

    IOCTL_info send = { 0, };
    send.information = general_scan;

    send.dump_info.To_Ioctl_requestor = NULL;

    send.ioctl_data_for_search = malloc(sizeof(ULONG32));
    memcpy(send.ioctl_data_for_search, &data, sizeof(ULONG32));
    send.ioctl_data_for_search_SIZE = sizeof(ULONG32);
    send.Ioctl_requestor_PID = my_pid;
    send.Start_Node_Address = NULL;
    send.target_process_info.PAGE_Area_mode = PAGE_READWRITE;
    send.target_process_info.target_PID = (HANDLE)13904;
    /*
    PLinked_list_from_searching NEW_SCANNED_start_node = NULL;

    Send_to_Kernel(hDevice, &send, &send);

    printf("1종료 -> Start_Address_IOCTL -> %p \n", send.Start_Node_Address);

    NEW_SCANNED_start_node = send.Start_Node_Address;
    query_print__Linked_list_for_searching(NEW_SCANNED_start_node);


    ///

    send.information = refresh;

    send.refresh_info.From_Ioctl_requestor = send.Start_Node_Address;

    Send_to_Kernel(hDevice, &send, &send);

    printf("2종료 -> Start_Address_IOCTL -> %p \n", send.Start_Node_Address);

    NEW_SCANNED_start_node = send.Start_Node_Address;
    query_print__Linked_list_for_searching(NEW_SCANNED_start_node);

    ///
    PLinked_list_from_searching TEST = NEW_SCANNED_start_node;

    SIZE_T node_index = 0;
    //printf("수정할 노드를 선택하시오\n");
    //scanf("%llu", &node_index);


    PUCHAR datas = malloc(sizeof(ULONG32));
    data = 100;
    memcpy(datas, &data, sizeof(ULONG32));
    
    query_editing__Linked_list_for_searching(NEW_SCANNED_start_node, node_index, datas, sizeof(ULONG32), hDevice, &send);
    
    ///

    send.information = refresh;

    Send_to_Kernel(hDevice, &send, &send);

    printf("2종료 -> Start_Address_IOCTL -> %p \n", send.Start_Node_Address);

    query_print__Linked_list_for_searching(TEST);
    
    ///
    */
    send.information = dump;

    Send_to_Kernel(hDevice, &send, &send);

    query_MEM_DUMP(send.dump_info.To_Ioctl_requestor);

    //File_Create(send.dump_info.To_User_Address, (ULONG32)send.dump_info.SIZE, "C:\\sample.bin");

   // printf("덤프 -> Start_Address_IOCTL -> %p / 사이즈 : %llu \n", send.dump_info.To_User_Address, send.dump_info.SIZE);



    system("pause");

    return 0;
}