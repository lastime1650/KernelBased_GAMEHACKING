#include "Set_HardWare_BP.h"
#include "HardWare_BreakPoint_Exception.h"


BOOLEAN Set_HardWare_Breakpoint(PIOCTL_info inout_Ioctl, PMEMHACK_needs process_info) {


    ULONG32 bufferSize = 0; // Initial buffer size
    PUCHAR buffer = NULL;
    NTSTATUS status;

	/* ���μ��� PID ��ġ ã�� */
    while (ZwQuerySystemInformation(SystemProcessInformation, buffer, bufferSize, &(ULONG)bufferSize) == STATUS_INFO_LENGTH_MISMATCH) {
        if (buffer == NULL) {
            buffer = ExAllocatePoolWithTag(NonPagedPool, bufferSize, 'QRPS'); // QueRyProceSs
            if (buffer == NULL) return FALSE;
        }
    }

    PSYSTEM_PROCESS_INFORMATION processInfo = (PSYSTEM_PROCESS_INFORMATION)buffer;

    while (processInfo) {
        if (processInfo->UniqueProcessId == process_info->target_process_PID) {
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " [�ϵ����BP] ��ġ�� PID ã��! %llu \n", processInfo->UniqueProcessId);

            /* ������ �ϳ��� �ݺ� ��ȸ */
            PSYSTEM_THREAD_INFORMATION threadInfo = processInfo->Threads;
            for (ULONG i = 0; i < processInfo->NumberOfThreads; i++) {
                HANDLE ThreadID = threadInfo->PID_and_THread_ID.UniqueThread;

                // ��ȿ�� ������ ID �ΰ�? 
                PETHREAD targetprocess_THREAD = NULL;
                if (PsLookupThreadByThreadId(ThreadID, &targetprocess_THREAD) != STATUS_SUCCESS) {
                    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " ������ ��ü ��� ���� \n");
                    threadInfo++;
                    continue;
                }
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " [�ϵ����BP] [%llu]  --  TID ã��! --> %llu  \n", processInfo->UniqueProcessId, ThreadID);


                // ������ �ڵ� ���
                HANDLE ThreadHandle = 0;
                status = ObOpenObjectByPointer(targetprocess_THREAD,
                    OBJ_KERNEL_HANDLE,
                    NULL,
                    THREAD_ALL_ACCESS,
                    *PsThreadType,
                    KernelMode,
                    &ThreadHandle);
                if (status != STATUS_SUCCESS) {
                    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " ������ �ڵ� ��� ���� \n");
                    threadInfo++;
                    continue;
                }
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " ������ �ڵ� ���� \n");


                // Ÿ�����μ����� �����Ͽ� CONTEXT ���� 
                CONTEXT ctx_p_but_Kernelmode = { 0, };

                KAPC_STATE apcState;
                KeStackAttachProcess(process_info->target_eprocess, & apcState);

                PCONTEXT ctx_p_but_usermode = NULL;
                SIZE_T ctx_p_SIZE_but_usermode = sizeof(CONTEXT);

                // �����ּ��Ҵ�NtCurrentProcess()
                ZwAllocateVirtualMemory(process_info->target_process_HANDLE, &ctx_p_but_usermode, 0, &ctx_p_SIZE_but_usermode, MEM_COMMIT, PAGE_READWRITE);
                memset(ctx_p_but_usermode, 0, sizeof(CONTEXT)); // �ʱ�ȭ

                // contextȮ��
                ctx_p_but_usermode->ContextFlags = CONTEXT_ALL; // �������

                // CONTEXT �������� 
                status = PsGetContextThread(targetprocess_THREAD, ctx_p_but_usermode, UserMode);
                if (status != STATUS_SUCCESS) {
                    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " ������ context ��� ���� %p \n", status);
                    KeUnstackDetachProcess(&apcState);
                    threadInfo++;
                    continue;
                }
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " ������ context ���� \n");

                // Ŀ�ο��� ����ϹǷ�, Ŀ�η� memcpy()
                memcpy(&ctx_p_but_Kernelmode, ctx_p_but_usermode, sizeof(CONTEXT));

                KeUnstackDetachProcess(&apcState);

                // �ϵ���� �극��ũ ����Ʈ ����

                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " ����� �������� Dr0 -> %lu \n", ctx_p_but_Kernelmode.Dr0);
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " ����� �������� Dr1 -> %lu \n", ctx_p_but_Kernelmode.Dr1);
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " ����� �������� Dr2 -> %lu \n", ctx_p_but_Kernelmode.Dr2);
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " ����� �������� Dr3 -> %lu \n", ctx_p_but_Kernelmode.Dr3);
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " ����� �������� Dr6 -> %lu \n", ctx_p_but_Kernelmode.Dr6);
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " ����� �������� Dr7 -> %lu \n", ctx_p_but_Kernelmode.Dr7);

                /*
                    �� �ּҿ� Dr0~3��� ���µ� ������ ���� �����Ѵ�.

                    Dr0: Read - 4����Ʈ����
                    Dr1: Write - 4����Ʈ����
                    Dr2: Read - 8����Ʈ����
                    Dr3: Write - 8����Ʈ����
                */
                ULONG64 main_addr = inout_Ioctl->HWBP_info.SET.Monitor_TargetProcess_Address;

                

                /* Dr0: Read/Write( 11 ) - 4����Ʈ����(�������� ũ�����) */
                ctx_p_but_Kernelmode.Dr0 = main_addr;
                ctx_p_but_Kernelmode.Dr7 |= (1 << 0);         // ���� Ȱ��ȭ
                ctx_p_but_Kernelmode.Dr7 |= (3 << (16 + 0));  // Read/Write
                ctx_p_but_Kernelmode.Dr7 |= (3 << (18 + 0));  // ũ��(4����Ʈ) 
                
                /* Dr1: Execute( 00 ) - 4����Ʈ���� (�������� ũ�����)*/
                ctx_p_but_Kernelmode.Dr1 = main_addr;
                ctx_p_but_Kernelmode.Dr7 |= (1 << 2);        
                ctx_p_but_Kernelmode.Dr7 |= (0 << (16 + 1 * 4)); // Execute
                ctx_p_but_Kernelmode.Dr7 |= (3 << (18 + 1 * 4)); // ũ��(4����Ʈ) 
                

                /* Dr2: Read/Write( 11 ) - 8����Ʈ���� */
                ctx_p_but_Kernelmode.Dr2 = main_addr;
                ctx_p_but_Kernelmode.Dr7 |= (1 << 4);       
                ctx_p_but_Kernelmode.Dr7 |= (3 << (16 + 2 * 4)); // ReadWrite
                ctx_p_but_Kernelmode.Dr7 |= (2 << (18 + 2 * 4)); // ũ�� 8����Ʈ
                

                /* Dr3: Execute( 00 ) - 8����Ʈ���� */
                ctx_p_but_Kernelmode.Dr3 = main_addr;
                ctx_p_but_Kernelmode.Dr7 |= (1 << 6);        
                ctx_p_but_Kernelmode.Dr7 |= (0 << (16 + 3 * 4));  // Execute
                ctx_p_but_Kernelmode.Dr7 |= (2 << (18 + 3 * 4));  // ũ�� 8����Ʈ


                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " ����� �������� Dr0 -> %lu \n", ctx_p_but_Kernelmode.Dr0);
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " ����� �������� Dr1 -> %lu \n", ctx_p_but_Kernelmode.Dr1);
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " ����� �������� Dr2 -> %lu \n", ctx_p_but_Kernelmode.Dr2);
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " ����� �������� Dr3 -> %lu \n", ctx_p_but_Kernelmode.Dr3);
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " ����� �������� Dr6 -> %lu \n", ctx_p_but_Kernelmode.Dr6);
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " ����� �������� Dr7 -> %lu \n", ctx_p_but_Kernelmode.Dr7);

                KeStackAttachProcess(process_info->target_eprocess, &apcState);

                memcpy(ctx_p_but_usermode, &ctx_p_but_Kernelmode, sizeof(CONTEXT));

                status = PsSetContextThread(targetprocess_THREAD, ctx_p_but_usermode, UserMode);
                if (status != STATUS_SUCCESS) {
                    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " ������ context ���� ���� %p \n", status);
                }
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " ������ context ���� ���� %p \n", status);
                KeUnstackDetachProcess(&apcState);



                // Ÿ�����μ����� �����Ͽ� CONTEXT ����



                threadInfo++;
            }
            break;
        }

        processInfo = (PSYSTEM_PROCESS_INFORMATION)((PUCHAR)processInfo + processInfo->NextEntryOffset);
    }

    ExFreePoolWithTag(buffer, 'QRPS');
	return TRUE;
}




BOOLEAN Get_HardWare_Breakpoint(PIOCTL_info inout_Ioctl, PMEMHACK_needs process_info, PEPROCESS SYSTEM_eprocess) {
    inout_Ioctl->HWBP_info.GET.OUTPUT = NULL;
    PHardWare_BP_Node Output = Query_HardWare_BP_Node(HardWare_BP_StartNode, process_info->target_process_PID);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " �ϵ���� BP ��� ���� ����? --> %p \n", Output);
    if (Output) {

        // ������忡�� ���� 

       
        SIZE_T Allocated_SIZE = sizeof(HardWare_BP_struct_from_targetprocess);
        if (ZwAllocateVirtualMemory(process_info->IOCTL_requstor_HANDLE, &inout_Ioctl->HWBP_info.GET.OUTPUT, 0, &Allocated_SIZE, MEM_COMMIT, PAGE_READWRITE) != STATUS_SUCCESS) {
            ExFreePoolWithTag(Output, 'HWBP');
            return FALSE;
        }
        SIZE_T output = 0;
        MmCopyVirtualMemory(SYSTEM_eprocess, &Output->CONTEXT, process_info->IOCTL_eprocess, inout_Ioctl->HWBP_info.GET.OUTPUT, sizeof(HardWare_BP_struct_from_targetprocess), KernelMode, &output);


        ExFreePoolWithTag(Output, 'HWBP');
        return TRUE;
    }
    else {
        return FALSE;
    }
}