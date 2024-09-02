#pragma once
#include "afxdialogex.h"
#include "IOCTL.h"




typedef struct HW_BP_thread_parm {
	BOOLEAN is_close_window;
	HANDLE PID;
	CListCtrl* List_Control_var;
}HW_BP_thread_parm, * PHW_BP_thread_parm;

// HARDWARE_BP_C 대화 상자

class HARDWARE_BP_C : public CDialogEx
{
	DECLARE_DYNAMIC(HARDWARE_BP_C)

public:
	HARDWARE_BP_C(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~HARDWARE_BP_C();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HARDWARE_BP };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	BOOL HARDWARE_BP_C::OnInitDialog();
	PIOCTL_info IOCTL_info_From_Main_Window;

	CEdit DR0_EDIT;
	CEdit DEBUG_PID_EDIT;
	CButton DR0_BUTTON;
	CButton DEBUG_PID_BUTTON;
	afx_msg void OnBnClickedDr0Button();

	void OnClose();

	HW_BP_thread_parm HWBP_thread_DATA; // 비동기 전용 구조체
	
	afx_msg void OnLvnItemchangedDebugList(NMHDR* pNMHDR, LRESULT* pResult);
	CListCtrl DEBuG_LIST_Var;
};

VOID HardWare_BP_Get_THREAD(PHW_BP_thread_parm parm);
