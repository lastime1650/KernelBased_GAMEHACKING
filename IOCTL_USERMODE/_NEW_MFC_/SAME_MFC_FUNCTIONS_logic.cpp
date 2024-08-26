#include "pch.h"
#include "framework.h"
#include "afxdialogex.h"
#include <stdint.h>
#include <Windows.h>

#include "SAME_MFC_FUNCTIONS.h"

VOID On_Off_from_ID(HWND Dig, ULONG32 item_ID, BOOLEAN is_Disable, LPCSTR Caption) {
	// 버튼 비활성화
	CWnd* pButton = GetDlgItem(Dig, (int)item_ID); // 버튼 ID로 버튼 핸들 가져오기
	if (is_Disable)
	{



		pButton->EnableWindow(FALSE); // 버튼 비활성화
	}
	else {
		pButton->EnableWindow(TRUE); // 버튼 활성화
	}

	// Caption 처리
	if (Caption != NULL) {
		CString cstrstring(Caption);
		pButton->SetWindowTextW(cstrstring);
	}
}