#ifndef conVert
#define conVert

#include <windows.h>
#include <stdio.h>
#include <afxwin.h> // CStringA
#include <algorithm> // std::reverse
#include <string> // std::string
#include <cstdlib>   // std::free
#include <sstream>
#include <iomanip>

#ifdef __cplusplus
extern "C" {
#endif

	LPCSTR Convert___Wide__to__Ansi____(LPCWSTR lpwstr);
    void ReverseHexString(CStringA& hexStringA);
	void ReverseCString(CString& str);
	BOOLEAN ReversePairs_for_1Bytes(CString& str);

	
	/////////////////

	BOOL HexStringToFLOAT(const CString& hexString, FLOAT* outValue);
	BOOL HexStringToQWORD(const CString& hexString, UINT64* outValue);
	BOOL HexStringToDWORD(const CString& hexString, DWORD* outValue);
	BOOL HexStringToDOUBLE(const CString& hexString, DOUBLE* outValue);
	BOOL HexStringToByteArray_Custom(const CString& hexString, BYTE** outArray, DWORD* outSize); // 동적할당이 요구됨
#ifdef __cplusplus
	}
#endif

#endif // !conVert
