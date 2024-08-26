#pragma once
#include "afxdialogex.h"
#include "IOCTL.h"

// ATTACH_PROCESS 대화 상자

class ATTACH_PROCESS : public CDialogEx
{
	DECLARE_DYNAMIC(ATTACH_PROCESS)

public:
	ATTACH_PROCESS(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~ATTACH_PROCESS();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ATTACH_PROCESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	CHAR LIST_columns[2][10] = {
		"PID",
		"ImageName"
	};

	VOID ATTACH_PROCESS::ReFresh_Processes();// 다시 프로세스 질의

	void ATTACH_PROCESS::On_Off_from_ID(CWnd* Insert_Instance, BOOLEAN is_Disable, CString Caption);
	
	void ATTACH_PROCESS::ATTACH_SUCCESS(HANDLE PID, CString ImageName);
	void ATTACH_PROCESS::ATTACH_FAILED();

	DECLARE_MESSAGE_MAP()
public:

	// 메인 윈도우로부터 얻는 것 
	typedef struct MOVE {
		BOOLEAN* is_Attach_from_Main;
		HANDLE* PID;
		CListCtrl* Address_Table_LISTCTRL_P;

		CWnd* PID_instance;
		CWnd* Attach_STATUS_instance;
		CWnd* ImageName_instance;

		CWnd* Check_Box_instance;
		CWnd* Address_Button_instance;

		PIOCTL_info Input_Data;

	}MOVE, *PMOVE;

	MOVE Parameter = { 0, };

	afx_msg void OnNMDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult);

	BOOL ATTACH_PROCESS::OnInitDialog();



	
	CListCtrl LIST_CTRL_Attach_PROCESS_inst; // ID: LIST_CTRL_ATTACH_PROCESS_ID
	afx_msg void OnBnClickedOk();
};
