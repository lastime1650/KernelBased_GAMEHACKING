#pragma once
#include "afxdialogex.h"
#include "converter.h"
#include "IOCTL.h"

// DUMP_PROCESS 대화 상자

class DUMP_PROCESS : public CDialogEx
{
	DECLARE_DYNAMIC(DUMP_PROCESS)

public:
	DUMP_PROCESS(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~DUMP_PROCESS();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DUMP_PROCESS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	void DUMP_PROCESS::OnClose();



	DECLARE_MESSAGE_MAP()
public:


	BOOL DUMP_PROCESS::OnInitDialog();

	afx_msg void OnBnClickedDumpProcessIdButton();
	afx_msg void OnEnChangeDumpSavePathEdit();
	CEdit SAVE_PATH_EDIT;
	CEdit PROCESS_ID_EDIT;
	afx_msg void OnEnChangeDumpProcessIdEdit();
};
