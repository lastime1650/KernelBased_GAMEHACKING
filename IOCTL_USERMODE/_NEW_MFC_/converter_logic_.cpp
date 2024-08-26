#include "pch.h"
#include "converter.h"


LPCSTR Convert___Wide__to__Ansi____(LPCWSTR lpwstr) {

    // ��ȯ�� ���ڿ��� ���� ���
    int length = lstrlenW(lpwstr);

    // ��ȯ�� ���ڿ��� ũ�� ���
    int bufferSize = WideCharToMultiByte(CP_ACP, 0, lpwstr, length, NULL, 0, NULL, NULL);

    // ���� �Ҵ�(C ��Ÿ��)
     LPSTR lpsz = (LPSTR)malloc(bufferSize + 1); // +1 for null terminator
    if (lpsz == NULL)
    {
        return NULL; // �޸� �Ҵ� ����
    }

    // ��ȯ ����
    WideCharToMultiByte(CP_ACP, 0, lpwstr, length, lpsz, bufferSize, NULL, NULL);

    // null terminator �߰�
    lpsz[bufferSize] = '\0';

    return lpsz; // ȣ���ڰ� �޸𸮸� �����ؾ� ��

}