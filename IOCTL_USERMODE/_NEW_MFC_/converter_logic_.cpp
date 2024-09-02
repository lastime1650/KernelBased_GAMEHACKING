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


void ReverseHexString(CStringA& hexStringA)
{
    // CStringA를 std::string으로 변환
    std::string str(hexStringA);

    // 문자열을 역순으로 변환
    std::reverse(str.begin(), str.end());

    // 역순으로 변환된 문자열을 CStringA로 다시 변환
    hexStringA = CStringA(str.c_str());
}

void ReverseCString(CString& str)
{
    // CString을 std::string으로 변환
    LPCSTR res = Convert___Wide__to__Ansi____(str.GetString());
    std::string tempStr(res);
    free((PVOID)res);

    // std::reverse를 사용하여 문자열을 역순으로 변환
    std::reverse(tempStr.begin(), tempStr.end());

    // 역순으로 변환된 문자열을 CString으로 다시 변환
    str = CString(tempStr.c_str());
}


BOOLEAN ReversePairs_for_1Bytes(CString& str)
{
    int length = str.GetLength();

    // 문자열 길이가 짝수인지 확인
    if (length % 2 != 0)
    {
        return FALSE;
    }

    CString result;

    // 2바이트씩 묶어서 역순으로 새 문자열 생성
    for (int i = length - 2; i >= 0; i -= 2)
    {
        result += str.Mid(i, 2);
    }

    // 원래 문자열을 결과로 대체
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
    return (*end == 0); // 전체 문자열이 성공적으로 변환되었는지 확인
}

BOOL HexStringToQWORD(const CString& hexString, ULONG64* outValue)
{
    if (hexString.GetLength() > 16) return FALSE;

    for (int i = 0; i < hexString.GetLength(); i++)
        if (!_istxdigit(hexString[i])) return FALSE;

    LPCTSTR str = hexString;
    LPTSTR end;
    *outValue = _tcstoui64(str, &end, 16);
    return (*end == 0); // 전체 문자열이 성공적으로 변환되었는지 확인
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
    // 입력 문자열 길이 확인
    int strLength = hexString.GetLength();
    if (strLength % 2 != 0) { // 문자열 길이는 짝수여야 함
        strLength--;// 기회를 준다.
        if (strLength % 2 != 0) return FALSE;
    }

    // 16진수 문자인지 검증
    for (int i = 0; i < strLength; i++)
    {
        if (!_istxdigit(hexString[i])) return FALSE;
    }

    // 바이트 배열의 크기 계산
    *outSize = strLength / 2;

    // malloc을 사용하여 메모리 동적 할당
    *outArray = (BYTE*)malloc(*outSize * sizeof(BYTE));
    if (*outArray == NULL) return FALSE;  // 메모리 할당 실패 시

    // 16진수 문자열을 바이트로 변환하여 배열에 저장
    for (DWORD i = 0; i < *outSize; i++)
    {
        CString byteString = hexString.Mid(i * 2, 2);
        (*outArray)[i] = (BYTE)_tcstoul(byteString, NULL, 16);
    }

    return TRUE;
}