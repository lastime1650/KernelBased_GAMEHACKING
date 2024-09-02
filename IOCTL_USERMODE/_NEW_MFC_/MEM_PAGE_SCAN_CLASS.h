#pragma once
#include "afxdialogex.h"

#include "IOCTL.h"
// MEM_PAGE_SCAN_CLASS 대화 상자

class MEM_PAGE_SCAN_CLASS : public CDialogEx
{
	DECLARE_DYNAMIC(MEM_PAGE_SCAN_CLASS)

public:
	MEM_PAGE_SCAN_CLASS(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~MEM_PAGE_SCAN_CLASS();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MEMORY_PAGE_SCAN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	VOID MEM_PAGE_SCAN_CLASS::Start_Page_Scan();

	DECLARE_MESSAGE_MAP()

public:
	BOOL MEM_PAGE_SCAN_CLASS::OnInitDialog();
	




	PIOCTL_info IOCTL_info_From_Main_Window;
	afx_msg void OnLvnItemchangedPageScanListControlId(NMHDR* pNMHDR, LRESULT* pResult);
	CListCtrl MEM_PAGE_SCAN_LIST_CTRL;
};


	
