#ifndef HW_BP
#define HW_BP

#include "afxdialogex.h"
#include <Windows.h>
#include <winternl.h> // NTSTATUS �� NtQuerySystemInformation�� ���� ���
#include <ntstatus.h>

// �ϵ���� �ߴ��� ���� ����ü
typedef struct HardWare_BP_struct_from_targetprocess {

    /*PID*/
    HANDLE PID;

    CONTEXT context;

}HardWare_BP_struct_from_targetprocess, * PHardWare_BP_struct_from_targetprocess;



#endif