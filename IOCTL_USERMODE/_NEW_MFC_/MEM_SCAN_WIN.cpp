// MEM_SCAN_WIN.cpp: 구현 파일
//

#include "pch.h"
#include "MFC_GAME_HACK.h"
#include "afxdialogex.h"
#include "MEM_SCAN_WIN.h"
#include <tchar.h>
#include <string> // std::wstring
#include <cstdlib> // std::wcstod

// MEM_SCAN_WIN 대화 상자

IMPLEMENT_DYNAMIC(MEM_SCAN_WIN, CDialogEx)

MEM_SCAN_WIN::MEM_SCAN_WIN(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SCAN_WINDOW, pParent)
{

}

MEM_SCAN_WIN::~MEM_SCAN_WIN()
{
}

void MEM_SCAN_WIN::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, Left_List_Control, Left_List_Control_Var);
	DDX_Control(pDX, Below_List_Control, Below_List_Control_Var);
	DDX_Control(pDX, IDC_COMBO2, Scan_Type_Combo_Box);
	DDX_Control(pDX, IDC_COMBO3, Value_Type_Combo_Box);
	DDX_Control(pDX, IDC_FirstScan_Button, First_Scan_Button);
	DDX_Control(pDX, IDC_NextScan_Button, Next_Scan_Button);
	DDX_Control(pDX, INPUT_text_search_edit, INPUT_text_search_edit_var);
	DDX_Control(pDX, Check_is_HEX, is_HEX);
}


BEGIN_MESSAGE_MAP(MEM_SCAN_WIN, CDialogEx)
	ON_BN_CLICKED(Check_is_HEX, &MEM_SCAN_WIN::OnBnClickedisHex)
	ON_BN_CLICKED(IDC_FirstScan_Button, &MEM_SCAN_WIN::OnBnClickedFirstscanButton)
	ON_BN_CLICKED(IDC_NextScan_Button, &MEM_SCAN_WIN::OnBnClickedNextscanButton)
END_MESSAGE_MAP()

//인스턴스 생성시 
BOOL MEM_SCAN_WIN::OnInitDialog() {
	CDialogEx::OnInitDialog();

	/*
		초기화
	*/

	// 초기 NextScan은 비활성화해야함.
	is_NextScanning = FALSE;
	Next_Scan_Button.EnableWindow(FALSE);


	// 왼쪽 리스트 컨트롤
	Left_List_Control_Var.InsertColumn(0, _T("index"), LVCFMT_LEFT, 30);
	Left_List_Control_Var.InsertColumn(1, _T("Address"), LVCFMT_LEFT, 200);
	Left_List_Control_Var.InsertColumn(2, _T("Value"), LVCFMT_LEFT, 200);
	Left_List_Control_Var.InsertColumn(3, _T("Size"), LVCFMT_LEFT, 100);

	Left_List_Control_Var.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	Left_List_Control_Var.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 아랫쪽 리스트 컨트롤
	Below_List_Control_Var.InsertColumn(0, _T("loop"), LVCFMT_LEFT, 50);
	Below_List_Control_Var.InsertColumn(1, _T("Address"), LVCFMT_LEFT, 200);
	Below_List_Control_Var.InsertColumn(2, _T("Value"), LVCFMT_LEFT, 200);
	Below_List_Control_Var.InsertColumn(3, _T("Type"), LVCFMT_LEFT, 100);

	Below_List_Control_Var.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	Below_List_Control_Var.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// Scan Type 콤보박스
	Scan_Type_Combo_Box.ResetContent();
	Scan_Type_Combo_Box.AddString(_T("All matched"));

	Scan_Type_Combo_Box.SetCurSel(0);

	// Value Type 검색 데이터 사이즈 
	Value_Type_Combo_Box.ResetContent();
	Value_Type_Combo_Box.AddString(_T("DWORD - 4Byte"));
	Value_Type_Combo_Box.AddString(_T("QWORD - 8Byte"));
	Value_Type_Combo_Box.AddString(_T("FLOAT - 4Byte"));
	Value_Type_Combo_Box.AddString(_T("DOUBLE - 8Byte"));

	Value_Type_Combo_Box.SetCurSel(0);


	///

	current_start_node = NULL;

	return TRUE;
}

// MEM_SCAN_WIN 메시지 처리기
CString FilterAndCombineStrings(const CString input)
{
	CString result;
	int start = 0;
	int end = 0;

	while ((end = input.Find(' ', start)) != -1) {
		CString token = input.Mid(start, end - start);
		if (!token.IsEmpty()) {
			result += token;
		}
		start = end + 1;
	}
	// 마지막 부분 추가
	CString lastToken = input.Mid(start);
	if (!lastToken.IsEmpty()) {
		result += lastToken;
	}

	return result;
}



// 왼쪽 리스트 컨트롤 초기화
void MEM_SCAN_WIN::init_left_list_ctrl() {
}

// 하단 리스트 컨트롤 초기화
void MEM_SCAN_WIN::init_below_list_ctrl() {

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void MEM_SCAN_WIN::OnBnClickedFirstscanButton()
{
	/* First Scan Button */




	/*
		스캐닝뷰 칼럼 아이템 초기화
	*/
	Left_List_Control_Var.DeleteAllItems();


	// current_start_node 할당된 경우, 해제해야함.
	/* .. */



	// 이미 Next Scan이 열린 상태에서, FirstScan 누르면 "초기화" 됨
	if (is_NextScanning) {
		is_NextScanning = FALSE;
		Next_Scan_Button.EnableWindow(FALSE);
	}

	BOOLEAN is_success = FALSE;

	// 데이터 준비
	PUCHAR Allocated_search_data = NULL;
	SIZE_T Allocated_search_data_SIZE = 0;
	CString Value_of_all_type;
	

	// 입력한 데이터 가져옴
	Parsing_Input_Edit(&Allocated_search_data, &Allocated_search_data_SIZE, Value_of_all_type);





	// 커널 요청
	PIOCTL_info INPUT_DATA = (PIOCTL_info)malloc(sizeof(IOCTL_info));
	memset(INPUT_DATA, 0, sizeof(IOCTL_info));// 필수 
	INPUT_DATA->information = general_scan;///////////////////////////////////////////////////////////////////////////
	INPUT_DATA->Ioctl_requestor_PID = (HANDLE)GetCurrentProcessId();

	WaitForSingleObject(IOCTL_info_From_Main_Window->IOCTL_User_Mutex, INFINITE);
	INPUT_DATA->target_process_info = IOCTL_info_From_Main_Window->target_process_info; // 커널에서 이 정보로 ATTACH할 것.
	ReleaseMutex(IOCTL_info_From_Main_Window->IOCTL_User_Mutex);


	INPUT_DATA->Mem_Scanning_info.Scan_mode = FIRST_SCAN;
	INPUT_DATA->Mem_Scanning_info.first_scan_info.IOCTL_allocated_addr__for_data_scan = Allocated_search_data;
	INPUT_DATA->Mem_Scanning_info.first_scan_info.IOCTL_allocated_size__for_data_scan = Allocated_search_data_SIZE;
	INPUT_DATA->Mem_Scanning_info.Link__Start_Address_for_IOCTL = NULL;
	


	PIOCTL_info OUTPUT_DATA = NULL;
	if (Send_to_Kernel(INPUT_DATA, TRUE, &OUTPUT_DATA, NULL) && OUTPUT_DATA && OUTPUT_DATA->Mem_Scanning_info.Link__Start_Address_for_IOCTL) {
		AfxMessageBox(_T("FIRST_SCAN_성공"));

		current_start_node = OUTPUT_DATA->Mem_Scanning_info.Link__Start_Address_for_IOCTL;

		PLinked_list_from_searching current_node = OUTPUT_DATA->Mem_Scanning_info.Link__Start_Address_for_IOCTL;

		CString Strings;
		while (current_node!=NULL) {

			//current_node->Searched__target_process_virtual_memory;
			//current_node->Searched_memory_data_size;
			//current_node->node_index;

			//
			Strings.Format(_T("%llu"), current_node->node_index);
			int nItem = Left_List_Control_Var.InsertItem(Left_List_Control_Var.GetItemCount(), Strings); 

			Strings.Format(_T("%p"), current_node->Searched__target_process_virtual_memory);
			Left_List_Control_Var.SetItemText(nItem, 1, Strings); // 2..

			// Value_of_all_type 초기
			Left_List_Control_Var.SetItemText(nItem, 2, Value_of_all_type); // 3..

			Strings.Format(_T("%llu"), current_node->Searched_memory_data_size);
			Left_List_Control_Var.SetItemText(nItem, 3, Strings); // 4 - SIZE

			current_node = (PLinked_list_from_searching)current_node->next_addr;
		}
		is_success = TRUE;
	}
	else {
		AfxMessageBox(_T("FIRST_SCAN_실패"));
		is_success = FALSE;
	}

	free(INPUT_DATA);


	




	if (is_success) {
		// First Scan 성공하면 -> Next Scan 버튼 활성화
		is_NextScanning = TRUE;
		Next_Scan_Button.EnableWindow(TRUE);
	}
	else {
		is_NextScanning = FALSE;
		Next_Scan_Button.EnableWindow(FALSE);
	}
	
}

void MEM_SCAN_WIN::OnBnClickedNextscanButton()
{
	/* Next Scan */

	// 이미 Next Scan이 열린 상태에서, FirstScan 누르면 "초기화" 됨
	if (!is_NextScanning) {
		is_NextScanning = FALSE;
		Next_Scan_Button.EnableWindow(FALSE);
		return;
	}
	
	
	
	BOOLEAN is_success = FALSE;


	// 데이터 준비
	PUCHAR Allocated_search_data = NULL;
	SIZE_T Allocated_search_data_SIZE = 0;
	CString Value_of_all_type;


	// 입력한 데이터 가져옴
	Parsing_Input_Edit(&Allocated_search_data, &Allocated_search_data_SIZE, Value_of_all_type);


	// 커널 요청
	if (Left_List_Control_Var.GetItemCount() > 0) {
		
		if (current_start_node) {

			// 커널 요청
			PIOCTL_info INPUT_DATA = (PIOCTL_info)malloc(sizeof(IOCTL_info));
			memset(INPUT_DATA, 0, sizeof(IOCTL_info));// 필수 
			INPUT_DATA->information = general_scan;///////////////////////////////////////////////////////////////////////////
			INPUT_DATA->Ioctl_requestor_PID = (HANDLE)GetCurrentProcessId();

			WaitForSingleObject(IOCTL_info_From_Main_Window->IOCTL_User_Mutex, INFINITE);
			INPUT_DATA->target_process_info = IOCTL_info_From_Main_Window->target_process_info; // 커널에서 이 정보로 ATTACH할 것.
			ReleaseMutex(IOCTL_info_From_Main_Window->IOCTL_User_Mutex);


			INPUT_DATA->Mem_Scanning_info.Scan_mode = NEXT_SCAN;
			INPUT_DATA->Mem_Scanning_info.next_scan_info.IOCTL_allocated_addr__for_data_scan = Allocated_search_data;
			INPUT_DATA->Mem_Scanning_info.next_scan_info.IOCTL_allocated_size__for_data_scan = Allocated_search_data_SIZE;
			INPUT_DATA->Mem_Scanning_info.next_scan_info.Link__Start_Address_for_Kernel = current_start_node;
			INPUT_DATA->Mem_Scanning_info.Link__Start_Address_for_IOCTL = NULL;



			PIOCTL_info OUTPUT_DATA = NULL;
			if (Send_to_Kernel(INPUT_DATA, TRUE, &OUTPUT_DATA, NULL) && OUTPUT_DATA && OUTPUT_DATA->Mem_Scanning_info.Link__Start_Address_for_IOCTL) {
				/*
					스캐닝뷰 칼럼 아이템 초기화
				*/
				Left_List_Control_Var.DeleteAllItems();
				/*
					재생성
				*/
				current_start_node = OUTPUT_DATA->Mem_Scanning_info.Link__Start_Address_for_IOCTL;

				PLinked_list_from_searching current_node = OUTPUT_DATA->Mem_Scanning_info.Link__Start_Address_for_IOCTL;

				CString Strings;
				while (current_node != NULL) {

					//current_node->Searched__target_process_virtual_memory;
					//current_node->Searched_memory_data_size;
					//current_node->node_index;

					//
					Strings.Format(_T("%llu"), current_node->node_index);
					int nItem = Left_List_Control_Var.InsertItem(Left_List_Control_Var.GetItemCount(), Strings);

					Strings.Format(_T("%p"), current_node->Searched__target_process_virtual_memory);
					Left_List_Control_Var.SetItemText(nItem, 1, Strings); // 2..

					// Value_of_all_type 초기
					Left_List_Control_Var.SetItemText(nItem, 2, Value_of_all_type); // 3..

					Strings.Format(_T("%llu"), current_node->Searched_memory_data_size);
					Left_List_Control_Var.SetItemText(nItem, 3, Strings); // 4 - SIZE

					current_node = (PLinked_list_from_searching)current_node->next_addr;
				}
				is_success = TRUE;
				
			}
			else {
				is_success = FALSE;
			}
		}

	}
	else {
		is_success = FALSE;
	}





	if (is_success) {
		// First Scan 성공하면 -> Next Scan 버튼 활성화
		is_NextScanning = TRUE;
		Next_Scan_Button.EnableWindow(TRUE);
	}
	else {
		Left_List_Control_Var.DeleteAllItems(); // 아이템 초기화
		is_NextScanning = FALSE;
		Next_Scan_Button.EnableWindow(FALSE);
	}
}
////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MEM_SCAN_WIN::Parsing_Input_Edit(PUCHAR* Allocated_search_data, SIZE_T* Allocated_search_data_SIZE, CString& Value_of_all_type) {

	// Value 타입 보기
	int selected_VALUE_Index = Value_Type_Combo_Box.GetCurSel();
	CString selectedString_Value;
	Value_Type_Combo_Box.GetLBText(selected_VALUE_Index, selectedString_Value);

	// Scan 타입 보기
	int selected_SCAN_Index = Scan_Type_Combo_Box.GetCurSel();
	CString selectedString_Scan;
	Scan_Type_Combo_Box.GetLBText(selected_SCAN_Index, selectedString_Scan);



	////////

	CString Get_already_Data;
	INPUT_text_search_edit_var.GetWindowTextW(Get_already_Data);
	CStringA DataStringA(Get_already_Data);


	//CString Value_of_all_type; // 최초 value에 보여짐
	if (selectedString_Value == _T("DWORD - 4Byte")) {


		DWORD DWORD__Value = 0;
		if (is_HEX_bool) {
			HexStringToDWORD(FilterAndCombineStrings(Get_already_Data), &DWORD__Value);

		}
		else {
			DWORD__Value = (DWORD)strtoul(DataStringA, NULL, 10);

		}

		Value_of_all_type.Format(_T("%llu\n"), DWORD__Value);

		*Allocated_search_data_SIZE = sizeof(DWORD);
		*Allocated_search_data = (PUCHAR)malloc(*Allocated_search_data_SIZE);
		memcpy(*Allocated_search_data, &DWORD__Value, *Allocated_search_data_SIZE);

	}
	else if (selectedString_Value == _T("QWORD - 8Byte")) {


		QWORD QWORD__Value = 0;
		if (is_HEX_bool) {
			HexStringToQWORD(FilterAndCombineStrings(Get_already_Data), &QWORD__Value);
		}
		else {
			QWORD__Value = (QWORD)strtoul(DataStringA, NULL, 10);
		}

		Value_of_all_type.Format(_T("%llu\n"), QWORD__Value);

		*Allocated_search_data_SIZE = sizeof(QWORD);
		*Allocated_search_data = (PUCHAR)malloc(*Allocated_search_data_SIZE);
		memcpy(*Allocated_search_data, &QWORD__Value, *Allocated_search_data_SIZE);
	}
	else if (selectedString_Value == _T("FLOAT - 4Byte")) {


		FLOAT FLOAT__Value = 0;
		if (is_HEX_bool) {

			HexStringToFLOAT(FilterAndCombineStrings(Get_already_Data), &FLOAT__Value);
		}
		else {
			FLOAT__Value = ::_tstof(Get_already_Data);
		}


		Value_of_all_type.Format(_T("%f\n"), FLOAT__Value);

		*Allocated_search_data_SIZE = sizeof(FLOAT);
		*Allocated_search_data = (PUCHAR)malloc(*Allocated_search_data_SIZE);
		memcpy(*Allocated_search_data, &FLOAT__Value, *Allocated_search_data_SIZE);

	}
	else if (selectedString_Value == _T("DOUBLE - 8Byte")) {



		DOUBLE DOUBLE__Value = 0;

		if (is_HEX_bool) {
			HexStringToDOUBLE(FilterAndCombineStrings(Get_already_Data), &DOUBLE__Value);
		}
		else {
			DOUBLE__Value = ::_wtof(Get_already_Data);
		}

		Value_of_all_type.Format(_T("%lf\n"), DOUBLE__Value);

		*Allocated_search_data_SIZE = sizeof(DOUBLE);
		*Allocated_search_data = (PUCHAR)malloc(*Allocated_search_data_SIZE);
		memcpy(*Allocated_search_data, &DOUBLE__Value, *Allocated_search_data_SIZE);
	}
	else {
		AfxMessageBox(_T("Wrong Value Type request"));
		return;
	}

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void MEM_SCAN_WIN::OnBnClickedisHex()
{
	/* is_Hex??? 체크박스 */
	if (is_HEX.IsWindowEnabled()) {
		is_HEX_bool = TRUE;
	}
	else {
		is_HEX_bool = FALSE;
	}
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/* 왼쪽/하단 리스트 컨트롤의 경우, 표가 존재하면, 커널에게 지속적으로 값을 요청하는 것이필요.(비동기), 윈도우 닫으면 스레드 종료. */
void MEM_SCAN_WIN::Parallel_Kernel_Request() {
	while (1) {
		if (is_this_window_closing) {
			return;
		}
		else {
			CString String;
			/* 왼쪽 행 1개 이상이면 커널 요청 */
			if (Left_List_Control_Var.GetItemCount() > 0) {
				for (ULONG32 row = 0; row < Left_List_Control_Var.GetItemCount(); row++) {
					//Left_List_Control_Var.GetItemText(row, 0); // index

					// 1. 주소.
					CStringA addrStringA1 ( Left_List_Control_Var.GetItemText(row, 1) ); // Address
					// 16진수 문자열
					PUCHAR Target_Address = (PUCHAR)strtoull(addrStringA1, NULL, 16);


					// 2. Value
					CStringA addrStringA2 ( Left_List_Control_Var.GetItemText(row, 3) ); // Size
					// SIZE
					SIZE_T SIZE = strtoull(addrStringA2, NULL, 10);
					


					// 지속확인.
					if (Left_List_Control_Var.GetItemCount() == 0)break;
				}
			}


			/* 오른쪽 행 1개 이상이면 커널 요청 */

		}
	}
}

//

void MEM_SCAN_WIN::OnClose() {
	is_this_window_closing = TRUE;
	EndDialog(IDD_SCAN_WINDOW); // 또는 IDCANCEL
}


