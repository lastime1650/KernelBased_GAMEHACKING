#pragma once
#include "afxdialogex.h"
#include "IOCTL.h"
#include "converter.h"

// KERNELBASED_DLL_INJECTION__ 대화 상자

class KERNELBASED_DLL_INJECTION__ : public CDialogEx
{
	DECLARE_DYNAMIC(KERNELBASED_DLL_INJECTION__)

public:
	KERNELBASED_DLL_INJECTION__(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~KERNELBASED_DLL_INJECTION__();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KERNEL_DLL_INJECTION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	//PIOCTL_info IOCTL_info_From_Main_Window;
	afx_msg void OnBnClickedFileChooseButtt();

	BOOL KERNELBASED_DLL_INJECTION__::OnInitDialog();

	
	CEdit DLL_PATH_EDIT;// DLL 절대경로 
	afx_msg void OnEnChangePidEdit();
	CEdit PID_EDIT;
	CComboBox LoadLibrary_COMBO;
	afx_msg void OnBnClickedInjectBuu();
};
