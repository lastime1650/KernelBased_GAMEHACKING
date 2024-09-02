// dllmain.cpp : DLL 애플리케이션의 진입점을 정의합니다.
#include "pch.h"
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>


#include "Exception_Handler.h"


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        // 현재 프로세스 ID를 가져옴
        DWORD processId = GetCurrentProcessId();

        // 파일 이름 설정
        //const TCHAR* filePath = _T("C:\\dll_injection_by_kernel.txt");
        TCHAR filePath[512];
        swprintf_s(filePath, _T("C:\\dll_injection_by_kernel_%d.txt"), processId);

        // 파일 열기 (존재하지 않으면 생성)
        HANDLE hFile = CreateFile(filePath,                // 파일 경로
            GENERIC_WRITE,           // 쓰기 권한
            0,                       // 공유 모드 (공유 안 함)
            NULL,                    // 보안 속성 (기본값)
            CREATE_ALWAYS,           // 항상 새 파일 생성
            FILE_ATTRIBUTE_NORMAL,   // 일반 파일 속성
            NULL);                   // 템플릿 파일 핸들 (없음)

        // 파일 열기 실패 시
        if (hFile == INVALID_HANDLE_VALUE)
        {
            return FALSE; // DLL_PROCESS_ATTACH에서 실패하면 FALSE를 반환
        }

        // 프로세스 ID를 문자열로 변환
        TCHAR buffer[32];
        StringCchPrintf(buffer, ARRAYSIZE(buffer), _T("%u"), processId);

        // 문자열을 파일에 쓰기
        DWORD bytesWritten;
        WriteFile(hFile,                     // 파일 핸들
            buffer,                    // 데이터 버퍼
            (DWORD)(_tcslen(buffer) * sizeof(TCHAR)), // 버퍼 크기
            &bytesWritten,             // 실제로 쓰여진 바이트 수
            NULL);                     // 비동기 I/O 안 함

        // 파일 닫기
        CloseHandle(hFile);


        /*
            예외 처리기 등록 ( 
        */
        AddVectoredExceptionHandler(1, ExceptionHandler);


        /*
            테스트 
        */
        /*
        CONTEXT context = { 0, };
        context.Rip = 0x1650;
        for (int i = 0; i < 50; i++) {
            SEND_Exception(&context);
        }
        */

        break;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

