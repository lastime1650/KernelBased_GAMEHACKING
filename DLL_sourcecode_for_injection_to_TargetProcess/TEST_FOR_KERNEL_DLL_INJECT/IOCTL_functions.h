#ifndef IOCTL_
#define IOCTL_

#include <stdio.h>
#include <Windows.h>
#include <winioctl.h>
#include <winternl.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

    extern HANDLE hDevice;

#define IOCTL_MY_IOCTL_CODE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

    // 하드웨어 중단점 전용 구조체
    typedef struct HardWare_BP_struct_from_targetprocess {

        /*PID*/
        HANDLE PID;

        /* CONTEXT */
        CONTEXT Register;

    }HardWare_BP_struct_from_targetprocess, * PHardWare_BP_struct_from_targetprocess;


    typedef enum IOCTL_command {

        Hardware_breakpoint_response = 2 // 하드웨어 중단점 확인용

    }IOCTL_command;

    typedef struct IOCTL_info {

        IOCTL_command information;

        HardWare_BP_struct_from_targetprocess Hardware_BP;

    }IOCTL_info, * PIOCTL_info;

    BOOLEAN Send_to_Kernel(PIOCTL_info INPUT);



#ifdef __cplusplus
}
#endif

#endif // !IOCTL_
