#include "Set_HardWare_BP.h"
#include "HardWare_BreakPoint_Exception.h"


BOOLEAN Set_HardWare_Breakpoint(PIOCTL_info inout_Ioctl, PMEMHACK_needs process_info) {


    ULONG32 bufferSize = 0; // Initial buffer size
    PUCHAR buffer = NULL;
    NTSTATUS status;

	/* 프로세스 PID 일치 찾기 */
    while (ZwQuerySystemInformation(SystemProcessInformation, buffer, bufferSize, &(ULONG)bufferSize) == STATUS_INFO_LENGTH_MISMATCH) {
        if (buffer == NULL) {
            buffer = ExAllocatePoolWithTag(NonPagedPool, bufferSize, 'QRPS'); // QueRyProceSs
            if (buffer == NULL) return FALSE;
        }
    }

    PSYSTEM_PROCESS_INFORMATION processInfo = (PSYSTEM_PROCESS_INFORMATION)buffer;

    while (processInfo) {
        if (processInfo->UniqueProcessId == process_info->target_process_PID) {
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " [하드웨어BP] 일치한 PID 찾음! %llu \n", processInfo->UniqueProcessId);

            /* 스레드 하나씩 반복 조회 */
            PSYSTEM_THREAD_INFORMATION threadInfo = processInfo->Threads;
            for (ULONG i = 0; i < processInfo->NumberOfThreads; i++) {
                HANDLE ThreadID = threadInfo->PID_and_THread_ID.UniqueThread;

                // 유효한 스레드 ID 인가? 
                PETHREAD targetprocess_THREAD = NULL;
                if (PsLookupThreadByThreadId(ThreadID, &targetprocess_THREAD) != STATUS_SUCCESS) {
                    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 스레드 객체 얻기 실패 \n");
                    threadInfo++;
                    continue;
                }
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " [하드웨어BP] [%llu]  --  TID 찾음! --> %llu  \n", processInfo->UniqueProcessId, ThreadID);


                // 스레드 핸들 얻기
                HANDLE ThreadHandle = 0;
                status = ObOpenObjectByPointer(targetprocess_THREAD,
                    OBJ_KERNEL_HANDLE,
                    NULL,
                    THREAD_ALL_ACCESS,
                    *PsThreadType,
                    KernelMode,
                    &ThreadHandle);
                if (status != STATUS_SUCCESS) {
                    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 스레드 핸들 얻기 실패 \n");
                    threadInfo++;
                    continue;
                }
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 스레드 핸들 얻음 \n");


                // 타겟프로세스에 접근하여 CONTEXT 추출 
                CONTEXT ctx_p_but_Kernelmode = { 0, };

                KAPC_STATE apcState;
                KeStackAttachProcess(process_info->target_eprocess, & apcState);

                PCONTEXT ctx_p_but_usermode = NULL;
                SIZE_T ctx_p_SIZE_but_usermode = sizeof(CONTEXT);

                // 가상주소할당NtCurrentProcess()
                ZwAllocateVirtualMemory(process_info->target_process_HANDLE, &ctx_p_but_usermode, 0, &ctx_p_SIZE_but_usermode, MEM_COMMIT, PAGE_READWRITE);
                memset(ctx_p_but_usermode, 0, sizeof(CONTEXT)); // 초기화

                // context확인
                ctx_p_but_usermode->ContextFlags = CONTEXT_ALL; // 모든정보

                // CONTEXT 가져오기 
                status = PsGetContextThread(targetprocess_THREAD, ctx_p_but_usermode, UserMode);
                if (status != STATUS_SUCCESS) {
                    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 스레드 context 얻기 실패 %p \n", status);
                    KeUnstackDetachProcess(&apcState);
                    threadInfo++;
                    continue;
                }
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 스레드 context 얻음 \n");

                // 커널에서 사용하므로, 커널로 memcpy()
                memcpy(&ctx_p_but_Kernelmode, ctx_p_but_usermode, sizeof(CONTEXT));

                KeUnstackDetachProcess(&apcState);

                // 하드웨어 브레이크 포인트 설정

                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 디버그 레지스터 Dr0 -> %lu \n", ctx_p_but_Kernelmode.Dr0);
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 디버그 레지스터 Dr1 -> %lu \n", ctx_p_but_Kernelmode.Dr1);
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 디버그 레지스터 Dr2 -> %lu \n", ctx_p_but_Kernelmode.Dr2);
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 디버그 레지스터 Dr3 -> %lu \n", ctx_p_but_Kernelmode.Dr3);
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 디버그 레지스터 Dr6 -> %lu \n", ctx_p_but_Kernelmode.Dr6);
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 디버그 레지스터 Dr7 -> %lu \n", ctx_p_but_Kernelmode.Dr7);

                /*
                    한 주소에 Dr0~3모두 쓰는데 다음과 같이 지정한다.

                    Dr0: Read - 4바이트길이
                    Dr1: Write - 4바이트길이
                    Dr2: Read - 8바이트길이
                    Dr3: Write - 8바이트길이
                */
                ULONG64 main_addr = inout_Ioctl->HWBP_info.SET.Monitor_TargetProcess_Address;

                

                /* Dr0: Read/Write( 11 ) - 4바이트길이(레지스터 크기기준) */
                ctx_p_but_Kernelmode.Dr0 = main_addr;
                ctx_p_but_Kernelmode.Dr7 |= (1 << 0);         // 로컬 활성화
                ctx_p_but_Kernelmode.Dr7 |= (3 << (16 + 0));  // Read/Write
                ctx_p_but_Kernelmode.Dr7 |= (3 << (18 + 0));  // 크기(4바이트) 
                
                /* Dr1: Execute( 00 ) - 4바이트길이 (레지스터 크기기준)*/
                ctx_p_but_Kernelmode.Dr1 = main_addr;
                ctx_p_but_Kernelmode.Dr7 |= (1 << 2);        
                ctx_p_but_Kernelmode.Dr7 |= (0 << (16 + 1 * 4)); // Execute
                ctx_p_but_Kernelmode.Dr7 |= (3 << (18 + 1 * 4)); // 크기(4바이트) 
                

                /* Dr2: Read/Write( 11 ) - 8바이트길이 */
                ctx_p_but_Kernelmode.Dr2 = main_addr;
                ctx_p_but_Kernelmode.Dr7 |= (1 << 4);       
                ctx_p_but_Kernelmode.Dr7 |= (3 << (16 + 2 * 4)); // ReadWrite
                ctx_p_but_Kernelmode.Dr7 |= (2 << (18 + 2 * 4)); // 크기 8바이트
                

                /* Dr3: Execute( 00 ) - 8바이트길이 */
                ctx_p_but_Kernelmode.Dr3 = main_addr;
                ctx_p_but_Kernelmode.Dr7 |= (1 << 6);        
                ctx_p_but_Kernelmode.Dr7 |= (0 << (16 + 3 * 4));  // Execute
                ctx_p_but_Kernelmode.Dr7 |= (2 << (18 + 3 * 4));  // 크기 8바이트


                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 디버그 레지스터 Dr0 -> %lu \n", ctx_p_but_Kernelmode.Dr0);
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 디버그 레지스터 Dr1 -> %lu \n", ctx_p_but_Kernelmode.Dr1);
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 디버그 레지스터 Dr2 -> %lu \n", ctx_p_but_Kernelmode.Dr2);
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 디버그 레지스터 Dr3 -> %lu \n", ctx_p_but_Kernelmode.Dr3);
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 디버그 레지스터 Dr6 -> %lu \n", ctx_p_but_Kernelmode.Dr6);
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 디버그 레지스터 Dr7 -> %lu \n", ctx_p_but_Kernelmode.Dr7);

                KeStackAttachProcess(process_info->target_eprocess, &apcState);

                memcpy(ctx_p_but_usermode, &ctx_p_but_Kernelmode, sizeof(CONTEXT));

                status = PsSetContextThread(targetprocess_THREAD, ctx_p_but_usermode, UserMode);
                if (status != STATUS_SUCCESS) {
                    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 스레드 context 설정 실패 %p \n", status);
                }
                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 스레드 context 설정 성공 %p \n", status);
                KeUnstackDetachProcess(&apcState);



                // 타겟프로세스에 접근하여 CONTEXT 설정



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
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " 하드웨어 BP 노드 추출 성공? --> %p \n", Output);
    if (Output) {

        // 유저모드에게 전달 

       
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