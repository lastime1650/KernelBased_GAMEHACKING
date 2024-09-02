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
    K_object_init_check_also_lock_ifyouwant(&IOCTL_thread_KEVENT, FALSE); // KEVENT�Ҵ縸 


    UNREFERENCED_PARAMETER(DeviceObject);
    PIO_STACK_LOCATION irpSp;
    NTSTATUS status = STATUS_SUCCESS;
    PJOB_QUEUE buffer;
    ULONG bufferLength;

    // IRP�� I/O ���� ��ġ�� �����ɴϴ�.
    irpSp = IoGetCurrentIrpStackLocation(Irp);
    buffer = (PJOB_QUEUE)Irp->AssociatedIrp.SystemBuffer;
    bufferLength = irpSp->Parameters.DeviceIoControl.InputBufferLength;

    switch (irpSp->Parameters.DeviceIoControl.IoControlCode) {
    case IOCTL_MY_IOCTL_CODE:
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " IOCTL ��û�� ������ �� ���� %d \n", bufferLength);


        if (buffer->INPUT_DATA.information == loop__Check ) {
            

            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " loop_check Ȯ�� \n");

        }
        else if (buffer->INPUT_DATA.information == Hardware_breakpoint_response) {

            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " Ÿ�����μ������� ��û���� \n");

            /*
                ����� TargetProcess�κ��� �������� ��. 
            */

            MOVE_for_TargetProcess Parameter = { 0, };
            Parameter.KEVENT = &IOCTL_thread_KEVENT;

            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " Ÿ�����μ������� ��û���� -> PID -> %lld  \n", ((PIOCTL_info_but_Hardware_BP_from_TargetProcess)Irp->AssociatedIrp.SystemBuffer)->Hardware_BP.PID  );

            Parameter.DATA = (PUCHAR)(&((PIOCTL_info_but_Hardware_BP_from_TargetProcess)Irp->AssociatedIrp.SystemBuffer)->Hardware_BP);
            Parameter.information = ((PIOCTL_info_but_Hardware_BP_from_TargetProcess)Irp->AssociatedIrp.SystemBuffer)->information;

            // ���� ������ 
            HANDLE THREAD = 0;
            PsCreateSystemThread(&THREAD, THREAD_ALL_ACCESS, NULL, 0, NULL, START_targetprocess_analyze, &Parameter);


            K_object_init_check_also_lock_ifyouwant(&IOCTL_thread_KEVENT, TRUE); // KEVENT ������ �Ͻ��ߴ� ( ������ ������ �ٸ� �����忡�� ó��. 

            /* targetprocess ��û�� ���� ��ȯ�� ������Ѵ�. */
            Irp->IoStatus.Status = status;
            Irp->IoStatus.Information = 0;
            break;
        }
        else if(buffer->INPUT_DATA.information > 0){

            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " IOCTL ��û ���� %d  \n", buffer->INPUT_DATA.information);
            /*
                loop__Check �� IOCTL ��û�ڿ��� ���������� üũ�� ������ �� ( �۾� ȸ���� )
            */

            //START_MEMHACK(&buffer);
            NULL;
            MOVE Parameter = { 0, };
            Parameter.KEVENT = &IOCTL_thread_KEVENT;
            Parameter.DATA = buffer;

            // ���� ������ 
            HANDLE THREAD = 0;
            PsCreateSystemThread(&THREAD, THREAD_ALL_ACCESS, NULL, 0, NULL, START_MEMHACK, &Parameter);


            K_object_init_check_also_lock_ifyouwant(&IOCTL_thread_KEVENT, TRUE); // KEVENT ������ �Ͻ��ߴ� ( ������ ������ �ٸ� �����忡�� ó��. 


           
        }
        else {
            status = STATUS_INVALID_DEVICE_REQUEST;
            Irp->IoStatus.Status = status;
            Irp->IoStatus.Information = 0;
            break;
        }




        // ������忡�� ������ JOB_QUEUE�� �����ؾ���. ( ���Ḯ��Ʈ�� ��带 �����ּҺ��� ��ȸ�Ͽ� ��ȿ�� �ּ��� ���, ������ )

        PJOB_QUEUE Output = Receive_Job_for_transmit_to_the_IOCTL_USER();
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " Receive_Job_for_transmit_to_the_IOCTL_USER ��� -> %p \n", Output);
        if (Output) {

            memcpy(Irp->AssociatedIrp.SystemBuffer, Output, sizeof(JOB_QUEUE)); // Output����


            ExFreePoolWithTag(Output, 'QUEU');
            Irp->IoStatus.Information = sizeof(JOB_QUEUE);
        }
        else {
            memset(Irp->AssociatedIrp.SystemBuffer, 0, sizeof(JOB_QUEUE)); // Ȥ�ø� ���� 
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
    
    

    // IRP �Ϸ�
    IoCompleteRequest(Irp, IO_NO_INCREMENT);



    K_object_lock_Release(&private_IOCTL_thread_KMUTEX);
    return status;
}
