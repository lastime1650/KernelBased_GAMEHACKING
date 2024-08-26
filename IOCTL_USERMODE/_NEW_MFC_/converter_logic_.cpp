#include "pch.h"
#include "converter.h"


LPCSTR Convert___Wide__to__Ansi____(LPCWSTR lpwstr) {

    // 변환할 문자열의 길이 계산
    int length = lstrlenW(lpwstr);

    // 변환된 문자열의 크기 계산
    int bufferSize = WideCharToMultiByte(CP_ACP, 0, lpwstr, length, NULL, 0, NULL, NULL);

    // 버퍼 할당(C 스타일)
     LPSTR lpsz = (LPSTR)malloc(bufferSize + 1); // +1 for null terminator
    if (lpsz == NULL)
    {
        return NULL; // 메모리 할당 실패
    }

    // 변환 수행
    WideCharToMultiByte(CP_ACP, 0, lpwstr, length, lpsz, bufferSize, NULL, NULL);

    // null terminator 추가
    lpsz[bufferSize] = '\0';

    return lpsz; // 호출자가 메모리를 해제해야 함

}