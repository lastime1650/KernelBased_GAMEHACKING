#include "pch.h"
#include "framework.h"
#include "afxdialogex.h"
#include <stdint.h>
#include <Windows.h>

#include "SAME_MFC_FUNCTIONS.h"

VOID On_Off_from_ID(HWND Dig, ULONG32 item_ID, BOOLEAN is_Disable, LPCSTR Caption) {
	// ��ư ��Ȱ��ȭ
	CWnd* pButton = GetDlgItem(Dig, (int)item_ID); // ��ư ID�� ��ư �ڵ� ��������
	if (is_Disable)
	{



		pButton->EnableWindow(FALSE); // ��ư ��Ȱ��ȭ
	}
	else {
		pButton->EnableWindow(TRUE); // ��ư Ȱ��ȭ
	}

	// Caption ó��
	if (Caption != NULL) {
		CString cstrstring(Caption);
		pButton->SetWindowTextW(cstrstring);
	}
}