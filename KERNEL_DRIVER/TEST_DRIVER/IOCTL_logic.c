#include "IOCTL.h"

#include "MEM_HACK.h"

#include "IOCTL_request_from_TargetProcess.h"

K_EVENT_or_MUTEX_struct IOCTL_thread_KEVENT = { NULL, K_EVENT, FALSE };

K_EVENT_or_MUTEX_struct private_IOCTL_thread_KMUTEX = { NULL, K_MUTEX, FALSE };

NTSTATUS MyDeviceControl(
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp
) {

    K_object_init_check_also_lock_ifyouwant(&private_IOCTL_thread_KMUTEX, TRUE);
    K_object_init_check_also_lock_ifyouwant(&IOCTL_thread_KEVENT, FALSE); // KEVENT할당만 


    UNREFERENCED_PARAMETER(DeviceObject);
    PIO_STACK_LOCATION irpSp;
    NTSTATUS status = STATUS_SUCCESS;
    PJOB_QUEUE buffer;
    ULONG bufferLength;

    // IRP의 I/O 스택 위치를 가져옵니다.
    irpSp = IoGetCurrentIrpStackLocation(Irp);
    buffer = (PJOB_QUEUE)Irp->AssociatedIrp.SystemBuffer;
    bufferLength = irpSp->Parameters.DeviceIoControl.InputBufferLength;

    switch (irpSp->Parameters.DeviceIoControl.IoControlCode) {
    case IOCTL_MY_IOCTL_CODE:
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " IOCTL 요청한 데이터 총 길이 %d \n", bufferLength);


        if (buffer->INPUT_DATA.information == loop__Check ) {
            

            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " loop_check 확인 \n");

        }
        else if (buffer->INPUT_DATA.information == Hardware_breakpoint_response) {

            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 타겟프로세스에서 요청들어옴 \n");

            /*
                여기는 TargetProcess로부터 가져오는 것. 
            */

            MOVE_for_TargetProcess Parameter = { 0, };
            Parameter.KEVENT = &IOCTL_thread_KEVENT;

            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 타겟프로세스에서 요청들어옴 -> PID -> %lld  \n", ((PIOCTL_info_but_Hardware_BP_from_TargetProcess)Irp->AssociatedIrp.SystemBuffer)->Hardware_BP.PID  );

            Parameter.DATA = (PUCHAR)(&((PIOCTL_info_but_Hardware_BP_from_TargetProcess)Irp->AssociatedIrp.SystemBuffer)->Hardware_BP);
            Parameter.information = ((PIOCTL_info_but_Hardware_BP_from_TargetProcess)Irp->AssociatedIrp.SystemBuffer)->information;

            // 병렬 스레드 
            HANDLE THREAD = 0;
            PsCreateSystemThread(&THREAD, THREAD_ALL_ACCESS, NULL, 0, NULL, START_targetprocess_analyze, &Parameter);


            K_object_init_check_also_lock_ifyouwant(&IOCTL_thread_KEVENT, TRUE); // KEVENT 스레드 일시중단 ( 스레드 해제는 다른 스레드에서 처리. 

            /* targetprocess 요청은 절대 반환이 없어야한다. */
            Irp->IoStatus.Status = status;
            Irp->IoStatus.Information = 0;
            break;
        }
        else if(buffer->INPUT_DATA.information > 0){

            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " IOCTL 요청 들어옴 %d  \n", buffer->INPUT_DATA.information);
            /*
                loop__Check 는 IOCTL 요청자에서 지속적으로 체크를 보내는 것 ( 작업 회수용 )
            */

            //START_MEMHACK(&buffer);
            NULL;
            MOVE Parameter = { 0, };
            Parameter.KEVENT = &IOCTL_thread_KEVENT;
            Parameter.DATA = buffer;

            // 병렬 스레드 
            HANDLE THREAD = 0;
            PsCreateSystemThread(&THREAD, THREAD_ALL_ACCESS, NULL, 0, NULL, START_MEMHACK, &Parameter);


            K_object_init_check_also_lock_ifyouwant(&IOCTL_thread_KEVENT, TRUE); // KEVENT 스레드 일시중단 ( 스레드 해제는 다른 스레드에서 처리. 


           
        }
        else {
            status = STATUS_INVALID_DEVICE_REQUEST;
            Irp->IoStatus.Status = status;
            Irp->IoStatus.Information = 0;
            break;
        }




        // 유저모드에게 전달할 JOB_QUEUE를 전달해야함. ( 연결리스트의 노드를 시작주소부터 조회하여 유효한 주소인 경우, 전달함 )

        PJOB_QUEUE Output = Receive_Job_for_transmit_to_the_IOCTL_USER();
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " Receive_Job_for_transmit_to_the_IOCTL_USER 결과 -> %p \n", Output);
        if (Output) {

            memcpy(Irp->AssociatedIrp.SystemBuffer, Output, sizeof(JOB_QUEUE)); // Output복사


            ExFreePoolWithTag(Output, 'QUEU');
            Irp->IoStatus.Information = sizeof(JOB_QUEUE);
        }
        else {
            memset(Irp->AssociatedIrp.SystemBuffer, 0, sizeof(JOB_QUEUE)); // 혹시모를 정리 
            Irp->IoStatus.Information = 0;
        }
        

        Irp->IoStatus.Status = status;
        

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
