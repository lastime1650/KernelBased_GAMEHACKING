#include "pch.h"
#include "FILE_IO.h"

BOOLEAN File_Create(PUCHAR INPUT_DATA, ULONG32 INPUT_DATA_SIZE, LPCSTR FILE_PATH) {
    HANDLE fileHandle = CreateFileA(
        FILE_PATH,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (fileHandle == INVALID_HANDLE_VALUE) {
        printf("Failed to create file: %lu\n", GetLastError());
        return FALSE;
    }

    DWORD bytesWritten = 0;
    BOOL result = WriteFile(
        fileHandle,
        INPUT_DATA,
        (DWORD)INPUT_DATA_SIZE,
        &bytesWritten,
        NULL
    );

    if (!result || bytesWritten != INPUT_DATA_SIZE) {
        printf("Failed to write data to file: %lu\n", GetLastError());
    }

    CloseHandle(fileHandle);
    return result;
}