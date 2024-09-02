#pragma once
#include "afxdialogex.h"
#include "IOCTL.h"
#include "converter.h"

// MEM_VIEWER 대화 상자

class MEM_VIEWER : public CDialogEx
{
	DECLARE_DYNAMIC(MEM_VIEWER)

public:
	MEM_VIEWER(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~MEM_VIEWER();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MEMORY_VIEWER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	
	void MEM_VIEWER::OnClose();
	
	void MEM_VIEWER::Get_Memory_Data();

	// MEMORY_VIEW_list_ctrl 특정 행 클릭시 이벤트임
	afx_msg void OnNMClickMemoryViewList(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
public:
	BOOL MEM_VIEWER::OnInitDialog();

	PIOCTL_info IOCTL_info_From_Main_Window;

	HANDLE selected_PID; 
	/*
		1. Offset
		2. Address
		3. Bytes ( 8b )
		4. Bytes ( 4b )
		3. Float32
		4. Double64
		5. Dword32
		6. PTR32
		7. PTR64
		8. HEAP ( 힙 공간이 아니면 빈칸 )
	*/
	CListCtrl MEMORY_VIEW_list_ctrl;
	afx_msg void OnEnChangePanelSetAddressId();

	// 컨트롤 패널
	CEdit Control_Panel_Set_Address;
	PUCHAR Set_Address_value;

	CEdit Control_Panel_Set_Scan_Size;
	SIZE_T Set_Scan_Size_value;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton1();

	// Attached_PID static text 변수화
	CEdit Attached_PID_edit;

	afx_msg void OnBnClickedButton();
	afx_msg void OnBnClickedCheck1();

	// 쿼리 성공 후, 재반복할지?
	CButton is_loop_Check_box_var;
	BOOLEAN is_loop;
	afx_msg void OnBnClickedButton2();


	/*
		Edit Memory 영역
	*/
	CEdit Edit_Memory_selected_offset_var; // Offset
	CComboBox Edit_Memory_Combo_BOX; // Choose Column
	CEdit Edit_Memory__Edit_Data_var; // Edit Data

	CString Get_already_Data; // Edit할 문자열 
};
