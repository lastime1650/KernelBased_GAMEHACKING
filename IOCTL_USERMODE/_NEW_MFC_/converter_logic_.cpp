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


void ReverseHexString(CStringA& hexStringA)
{
    // CStringA�� std::string���� ��ȯ
    std::string str(hexStringA);

    // ���ڿ��� �������� ��ȯ
    std::reverse(str.begin(), str.end());

    // �������� ��ȯ�� ���ڿ��� CStringA�� �ٽ� ��ȯ
    hexStringA = CStringA(str.c_str());
}

void ReverseCString(CString& str)
{
    // CString�� std::string���� ��ȯ
    LPCSTR res = Convert___Wide__to__Ansi____(str.GetString());
    std::string tempStr(res);
    free((PVOID)res);

    // std::reverse�� ����Ͽ� ���ڿ��� �������� ��ȯ
    std::reverse(tempStr.begin(), tempStr.end());

    // �������� ��ȯ�� ���ڿ��� CString���� �ٽ� ��ȯ
    str = CString(tempStr.c_str());
}


BOOLEAN ReversePairs_for_1Bytes(CString& str)
{
    int length = str.GetLength();

    // ���ڿ� ���̰� ¦������ Ȯ��
    if (length % 2 != 0)
    {
        return FALSE;
    }

    CString result;

    // 2����Ʈ�� ��� �������� �� ���ڿ� ����
    for (int i = length - 2; i >= 0; i -= 2)
    {
        result += str.Mid(i, 2);
    }

    // ���� ���ڿ��� ����� ��ü
    str = result;

    return TRUE;
}





/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////


BOOL HexStringToDWORD(const CString& hexString, DWORD* outValue)
{
    if (hexString.GetLength() > 8) return FALSE;

    for (int i = 0; i < hexString.GetLength(); i++)
        if (!_istxdigit(hexString[i])) return FALSE;

    LPCTSTR str = hexString;
    LPTSTR end;
    *outValue = _tcstoul(str, &end, 16);
    return (*end == 0); // ��ü ���ڿ��� ���������� ��ȯ�Ǿ����� Ȯ��
}

BOOL HexStringToQWORD(const CString& hexString, ULONG64* outValue)
{
    if (hexString.GetLength() > 16) return FALSE;

    for (int i = 0; i < hexString.GetLength(); i++)
        if (!_istxdigit(hexString[i])) return FALSE;

    LPCTSTR str = hexString;
    LPTSTR end;
    *outValue = _tcstoui64(str, &end, 16);
    return (*end == 0); // ��ü ���ڿ��� ���������� ��ȯ�Ǿ����� Ȯ��
}

BOOL HexStringToFLOAT(const CString& hexString, FLOAT* outValue)
{
    if (hexString.GetLength() != 8) return FALSE;

    DWORD temp;
    if (!HexStringToDWORD(hexString, &temp)) return FALSE;

    *outValue = *reinterpret_cast<FLOAT*>(&temp);
    return TRUE;
}

BOOL HexStringToDOUBLE(const CString& hexString, DOUBLE* outValue)
{
    if (hexString.GetLength() != 16) return FALSE;

    UINT64 temp;
    if (!HexStringToQWORD(hexString, &temp)) return FALSE;

    *outValue = *reinterpret_cast<DOUBLE*>(&temp);
    return TRUE;
}


BOOL HexStringToByteArray_Custom(const CString& hexString, BYTE** outArray, DWORD* outSize)
{
    // �Է� ���ڿ� ���� Ȯ��
    int strLength = hexString.GetLength();
    if (strLength % 2 != 0) { // ���ڿ� ���̴� ¦������ ��
        strLength--;// ��ȸ�� �ش�.
        if (strLength % 2 != 0) return FALSE;
    }

    // 16���� �������� ����
    for (int i = 0; i < strLength; i++)
    {
        if (!_istxdigit(hexString[i])) return FALSE;
    }

    // ����Ʈ �迭�� ũ�� ���
    *outSize = strLength / 2;

    // malloc�� ����Ͽ� �޸� ���� �Ҵ�
    *outArray = (BYTE*)malloc(*outSize * sizeof(BYTE));
    if (*outArray == NULL) return FALSE;  // �޸� �Ҵ� ���� ��

    // 16���� ���ڿ��� ����Ʈ�� ��ȯ�Ͽ� �迭�� ����
    for (DWORD i = 0; i < *outSize; i++)
    {
        CString byteString = hexString.Mid(i * 2, 2);
        (*outArray)[i] = (BYTE)_tcstoul(byteString, NULL, 16);
    }

    return TRUE;
}