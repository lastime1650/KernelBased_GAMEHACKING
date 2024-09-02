#ifndef HW_BP
#define HW_BP

#include "afxdialogex.h"
#include <Windows.h>
#include <winternl.h> // NTSTATUS 및 NtQuerySystemInformation을 위한 헤더
#include <ntstatus.h>

// 하드웨어 중단점 전용 구조체
typedef struct HardWare_BP_struct_from_targetprocess {

    /*PID*/
    HANDLE PID;

    CONTEXT context;

}HardWare_BP_struct_from_targetprocess, * PHardWare_BP_struct_from_targetprocess;



#endif