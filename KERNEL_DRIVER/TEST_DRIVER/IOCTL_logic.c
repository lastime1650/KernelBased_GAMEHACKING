#include "IOCTL.h"

#include "MEM_HACK.h"

K_EVENT_or_MUTEX_struct IOCTL_thread_KEVENT = { NULL, K_EVENT, FALSE };

K_EVENT_or_MUTEX_struct private_IOCTL_thread_KMUTEX = { NULL, K_MUTEX, FALSE };

NTSTATUS MyDeviceControl(
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp
) {

    K_object_init_check_also_lock_ifyouwant(&private_IOCTL_thread_KMUTEX, TRUE);



    UNREFERENCED_PARAMETER(DeviceObject);
    PIO_STACK_LOCATION irpSp;
    NTSTATUS status = STATUS_SUCCESS;
    PIOCTL_info buffer;
    ULONG bufferLength;

    // IRP의 I/O 스택 위치를 가져옵니다.
    irpSp = IoGetCurrentIrpStackLocation(Irp);
    buffer = (PIOCTL_info)Irp->AssociatedIrp.SystemBuffer;
    bufferLength = irpSp->Parameters.DeviceIoControl.InputBufferLength;

    switch (irpSp->Parameters.DeviceIoControl.IoControlCode) {
    case IOCTL_MY_IOCTL_CODE:

        // 병렬 스레드 추가
        //K_object_init_check_also_lock_ifyouwant(&IOCTL_thread_KEVENT, TRUE); // KEVENT 스레드 일시중단 ( 스레드 해제는 다른 스레드에서 처리. 
        START_MEMHACK(&buffer);


        Irp->IoStatus.Status = status;
        Irp->IoStatus.Information = sizeof(IOCTL_info);

        break;
    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        Irp->IoStatus.Status = status;
        Irp->IoStatus.Information = 0;
        break;
    }
    
    

    // IRP 완료
    IoCompleteRequest(Irp, IO_NO_INCREMENT);



    K_object_lock_Release(&private_IOCTL_thread_KMUTEX);
    return status;
}
