#pragma once
#include "afxdialogex.h"
#include "IOCTL.h"
#include "converter.h"
#include <afx.h>  // MFC에서 CString을 사용할 경우 필요
#include <iostream>
// MEM_SCAN_WIN 대화 상자

class MEM_SCAN_WIN : public CDialogEx
{
	DECLARE_DYNAMIC(MEM_SCAN_WIN)

public:
	MEM_SCAN_WIN(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~MEM_SCAN_WIN();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SCAN_WINDOW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	void MEM_SCAN_WIN::OnClose();

	void MEM_SCAN_WIN::Parallel_Kernel_Request();

	void MEM_SCAN_WIN::Parsing_Input_Edit(PUCHAR* Allocated_search_data, SIZE_T* Allocated_search_data_SIZE, CString& Value_of_all_type);

	PLinked_list_from_searching current_start_node; // 스캔시 사용되는 연결리스트


	DECLARE_MESSAGE_MAP()

public:

	BOOL MEM_SCAN_WIN::OnInitDialog();
	
	BOOLEAN is_this_window_closing; // 비동기 스레드 닫을 떄 확인

	BOOLEAN is_NextScanning;

	PIOCTL_info IOCTL_info_From_Main_Window;

	// 왼쪽 리스트 컨트롤 초기화
	void MEM_SCAN_WIN::init_left_list_ctrl();

	// 하단 리스트 컨트롤 초기화
	void MEM_SCAN_WIN::init_below_list_ctrl();

	CListCtrl Left_List_Control_Var;
	CListCtrl Below_List_Control_Var;

	CComboBox Scan_Type_Combo_Box;
	CComboBox Value_Type_Combo_Box;

	CButton First_Scan_Button;
	CButton Next_Scan_Button;

	CEdit INPUT_text_search_edit_var;


	CButton is_HEX;
	BOOLEAN is_HEX_bool;


	afx_msg void OnBnClickedisHex();
	afx_msg void OnBnClickedFirstscanButton();
	afx_msg void OnBnClickedNextscanButton();
};
