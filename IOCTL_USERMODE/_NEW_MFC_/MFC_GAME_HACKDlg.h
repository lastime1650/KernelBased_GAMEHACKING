
// MFC_GAME_HACKDlg.h: 헤더 파일
//



#include "ATTACH_PROCESS.h"
#include "DUMP_PROCESS.h"
#include "MEM_VIEWER.h"
#include "MEM_PAGE_SCAN_CLASS.h"
#include "MEM_SCAN_WIN.h"
#include "KERNELBASED_DLL_INJECTION__.h"
#include "HARDWARE_BP_C.h"

#include "IOCTL.h"
#pragma once


// CMFCGAMEHACKDlg 대화 상자
class CMFCGAMEHACKDlg : public CDialogEx
{
// 생성입니다.
public:
	CMFCGAMEHACKDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFC_GAME_HACK_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	
	afx_msg LRESULT CMFCGAMEHACKDlg::OnCtlColorStatic(WPARAM wParam, LPARAM lParam);

	

	// ATTACH_PROCESS 다이얼로그
	ATTACH_PROCESS Attach_process_dialog_inst;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	
	afx_msg void OnLvnItemchangedListAttachedProcessTable(NMHDR* pNMHDR, LRESULT* pResult);
	void CMFCGAMEHACKDlg::On_Off_from_ID(ULONG32 item_ID, BOOLEAN is_Disable, LPCSTR Caption);
	/* 프로세스 어태치 확인 ( 나중에 구조체 )  */
	BOOLEAN is_attached;
	HANDLE PID; // 커널 전송용 
	//
	afx_msg void OnStnClickedStaticAddressTable();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnAttachStartattach();
	afx_msg void OnMemoryStartmemoryscan();
	afx_msg void OnDumpProcessdump();
	afx_msg void OnHelpHowtouseit();

	void CMFCGAMEHACKDlg::TESTTT(PVOID a);
	

	void CMFCGAMEHACKDlg::Initialize_by_Attach();

	/*
		해야할 것.
		1. 커널 손보기.
		2. EDIT_ADDRESS_INPUT_inst , Address_Table_LISTCTRL , LIST_BOX_BaseAddress 동작시키기 
		3. IOCTL 사용
		4. 커널 통신 시도 
	*/

	CListCtrl Address_Table_LISTCTRL;
	CListBox LIST_BOX_BaseAddress;///////////////////// ( 어태치 후 얻은 EXE 베이스주소, 리스트박스인 경우는 , 2개이상인 경우를 목격해봤기 때문임 ) 
	CEdit EDIT_ADDRESS_INPUT_inst;
	afx_msg void OnEnChangeAddressInput();


	/*
		커널에게 전달할 구조체 
	*/
	PIOCTL_info Input_Data; // 초기에 동적할당 해야함 
	afx_msg void OnMemoryOpen();
	afx_msg void OnMemoryOpen32778();
	afx_msg void OnFunctionsKernel();
	afx_msg void OnFunctionsHardware();
};
