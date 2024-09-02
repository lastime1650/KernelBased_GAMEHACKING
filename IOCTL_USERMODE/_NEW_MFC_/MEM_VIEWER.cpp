// MEM_VIEWER.cpp: 구현 파일
//

#include "pch.h"
#include "MFC_GAME_HACK.h"
#include "afxdialogex.h"
#include "MEM_VIEWER.h"
#include <tchar.h>
#include <string> // std::wstring
#include <cstdlib> // std::wcstod
#include "converter.h"

// MEM_VIEWER 대화 상자

IMPLEMENT_DYNAMIC(MEM_VIEWER, CDialogEx)

MEM_VIEWER::MEM_VIEWER(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MEMORY_VIEWER, pParent)
{

}

MEM_VIEWER::~MEM_VIEWER()
{
}

void MEM_VIEWER::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MEMORY_VIEW, MEMORY_VIEW_list_ctrl);
	DDX_Control(pDX, Control_Panel_Set_Address_ID, Control_Panel_Set_Address);
	DDX_Control(pDX, Control_Panel_Set_Scan_Size_ID, Control_Panel_Set_Scan_Size);
	DDX_Control(pDX, ATTACHED_TEXT_EDIT, Attached_PID_edit);
	DDX_Control(pDX, IDC_CHECK1, is_loop_Check_box_var);
	DDX_Control(pDX, IDC_EDIT4, Edit_Memory_selected_offset_var);
	DDX_Control(pDX, IDC_COMBO1, Edit_Memory_Combo_BOX);
	DDX_Control(pDX, IDC_EDIT3, Edit_Memory__Edit_Data_var);
}


BEGIN_MESSAGE_MAP(MEM_VIEWER, CDialogEx)
	ON_WM_CLOSE()

	// MEMORY_VIEW_list_ctrl ------- 리스트 컨트롤 
	ON_NOTIFY(NM_CLICK, IDC_MEMORY_VIEW, &MEM_VIEWER::OnNMClickMemoryViewList)



	ON_BN_CLICKED(IDC_BUTTON1, &MEM_VIEWER::OnBnClickedButton1)
	ON_BN_CLICKED(PID_BUTTON, &MEM_VIEWER::OnBnClickedButton)
	ON_BN_CLICKED(IDC_CHECK1, &MEM_VIEWER::OnBnClickedCheck1)

	ON_BN_CLICKED(IDC_BUTTON3, &MEM_VIEWER::OnBnClickedButton2)

	ON_BN_CLICKED(IDC_BUTTON2, &MEM_VIEWER::OnBnClickedButton3)
END_MESSAGE_MAP()


BOOL MEM_VIEWER::OnInitDialog() {

	CDialogEx::OnInitDialog();


	/* 대따 큰 리스트 컨트롤 칼럼 초기화 */
	MEMORY_VIEW_list_ctrl.ModifyStyle(0, LVS_REPORT);
	MEMORY_VIEW_list_ctrl.InsertColumn(0, _T("Offset"), LVCFMT_LEFT, 100);
	MEMORY_VIEW_list_ctrl.InsertColumn(1, _T("Address"), LVCFMT_LEFT, 150);
	MEMORY_VIEW_list_ctrl.InsertColumn(2, _T("8_Byte"), LVCFMT_LEFT, 200);
	//MEMORY_VIEW_list_ctrl.InsertColumn(3, _T("4_Byte"), LVCFMT_LEFT, 150);
	MEMORY_VIEW_list_ctrl.InsertColumn(3, _T("Float32"), LVCFMT_LEFT, 150);
	MEMORY_VIEW_list_ctrl.InsertColumn(4, _T("Double64"), LVCFMT_LEFT, 150);
	MEMORY_VIEW_list_ctrl.InsertColumn(5, _T("DWORD"), LVCFMT_LEFT, 150);
	MEMORY_VIEW_list_ctrl.InsertColumn(6, _T("QWORD"), LVCFMT_LEFT, 150);
	MEMORY_VIEW_list_ctrl.InsertColumn(7, _T("int32"), LVCFMT_LEFT, 150);
	MEMORY_VIEW_list_ctrl.InsertColumn(8, _T("int64"), LVCFMT_LEFT, 150);
	//MEMORY_VIEW_list_ctrl.InsertColumn(6, _T("PTR32"), LVCFMT_LEFT, 150);
	MEMORY_VIEW_list_ctrl.InsertColumn(9, _T("PTR64"), LVCFMT_LEFT, 200);
	MEMORY_VIEW_list_ctrl.InsertColumn(10, _T("HEAP"), LVCFMT_LEFT, 200);
	MEMORY_VIEW_list_ctrl.InsertColumn(11, _T("Ansi"), LVCFMT_LEFT, 200);

	// 칼럼이 상당히 많다. / 가로 스크롤 삽입!!
	MEMORY_VIEW_list_ctrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 행 선택도 가능.
	MEMORY_VIEW_list_ctrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	////////////////
	/* 콤보박스 초기화 */
	Edit_Memory_Combo_BOX.ResetContent();

	Edit_Memory_Combo_BOX.AddString(_T("8_Byte"));
	Edit_Memory_Combo_BOX.AddString(_T("Float32"));
	Edit_Memory_Combo_BOX.AddString(_T("Double64"));
	Edit_Memory_Combo_BOX.AddString(_T("DWORD"));
	Edit_Memory_Combo_BOX.AddString(_T("QWORD"));
	Edit_Memory_Combo_BOX.AddString(_T("int32"));
	Edit_Memory_Combo_BOX.AddString(_T("int64"));
	Edit_Memory_Combo_BOX.AddString(_T("PTR64"));

	// 기본적으로 첫 번째 항목을 선택 상태로 설정
	Edit_Memory_Combo_BOX.SetCurSel(0);
	




	// 커널에게 Memory Viewer 요청하여 커널로부터 할당된 "자신의 동적주소"를 참조. NULL이면;;... 경고창 띄워서 다른 주소로 유도.
	// 변수 초기화
	Set_Address_value = (PUCHAR)0x0;
	Set_Scan_Size_value = 0x100;

	CString String;
	String.Format(_T("%p"), Set_Address_value);
	Control_Panel_Set_Address.SetWindowTextW(String.GetString());

	String.Format(_T("%X"), Set_Scan_Size_value);
	Control_Panel_Set_Scan_Size.SetWindowTextW(String.GetString());


	// Attached_PID static text 변수는 PID 
	/*
		처음 윈도우를  생성하면, attach된 PID를 보여주고, attach가 아직 안되면 빈칸.
	*/

	selected_PID = IOCTL_info_From_Main_Window->target_process_info.target_PID;
	String.Format(_T("%lld"), selected_PID);
	Attached_PID_edit.SetWindowTextW(String.GetString());

	is_loop = FALSE;

	return TRUE;
}

// MEM_VIEWER 메시지 처리기




/* 컨트롤 패널 -> Scan Size 설정 */
void MEM_VIEWER::OnBnClickedButton1()
{
	if (selected_PID == 0) {
		AfxMessageBox(_T(" Please input PID first!! "));
		return;
	}


	// 이전에 TRUE설정된 경우 FALSE처리
	if (is_loop) is_loop = FALSE;




	// 루프요청인가?
	// 체크박스의 상태를 확인
	int checkStatus = is_loop_Check_box_var.GetCheck();

	if (checkStatus == BST_CHECKED) {
		is_loop = TRUE;
	}
	else if (checkStatus == BST_UNCHECKED) {
		is_loop = FALSE;
	}
	else if (checkStatus == BST_INDETERMINATE) {
		is_loop = FALSE;
	}

	Get_Memory_Data();
	

}

void MEM_VIEWER::Get_Memory_Data() {

	


	//////////////////////////////////////////////////////////////////////

	/*
		BaseAddress
	*/
	CString ADDR_String;
	Control_Panel_Set_Address.GetWindowTextW(ADDR_String);

	// CString을 CStringA로 변환 (LPCSTR로 변환하기 위해)
	CStringA addrStringA(ADDR_String);

	// 16진수 문자열을 unsigned long long (8바이트) 값으로 변환
	unsigned long long ullAddress = strtoull(addrStringA, NULL, 16);

	// 변환된 주소를 PUCHAR로 캐스팅
	PUCHAR pAddress = (PUCHAR)(ullAddress);

	ADDR_String.Format(_T("%p"), pAddress);
	Control_Panel_Set_Address.SetWindowTextW(ADDR_String);


	///////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////

	/*
		SIZE
	*/
	Control_Panel_Set_Scan_Size.GetWindowTextW(ADDR_String);

	// CString을 CStringA로 변환 (LPCSTR로 변환하기 위해)
	CStringA addrStringA2(ADDR_String);

	// 16진수 문자열을 unsigned long long (8바이트) 값으로 변환
	unsigned long long get_SIZE = strtoull(addrStringA2, NULL, 16);

	// 8의 배수로 맞추기 ( 64비트 계열 )
	while ((get_SIZE % (SIZE_T)8) != 0) {
		get_SIZE++;
	}
	SIZE_T SIZE = (SIZE_T)get_SIZE;

	ADDR_String.Format(_T("%x"), SIZE);
	Control_Panel_Set_Scan_Size.SetWindowTextW(ADDR_String);

	ADDR_String.Format(_T("you input this -> %x"), SIZE);
	//AfxMessageBox(ADDR_String);

	/////////////////////////////////////////////////////////////////////

	/*
		커널 요청
	*/
	PIOCTL_info INPUT_DATA = (PIOCTL_info)malloc(sizeof(IOCTL_info));
	memset(INPUT_DATA, 0, sizeof(IOCTL_info));// 필수 
	INPUT_DATA->information = Memory_Viewer;
	INPUT_DATA->Ioctl_requestor_PID = (HANDLE)GetCurrentProcessId();

	INPUT_DATA->target_process_info.target_PID = selected_PID;

	INPUT_DATA->Mem_Viewer.BaseAddress_of_TargetProcess = (PUCHAR)pAddress;
	INPUT_DATA->Mem_Viewer.Scan_SIZE = (SIZE_T)SIZE;

	PIOCTL_info OUTPUT_DATA = NULL;
	if (Send_to_Kernel(INPUT_DATA, TRUE, &OUTPUT_DATA, NULL) && OUTPUT_DATA && OUTPUT_DATA->Mem_Viewer.Output_Allocated_IOCTL_Address_for_return) {
		//AfxMessageBox(_T("Memory_Viewer 성공"));

		// 리스트 컨트롤의 스크롤 위치를 저장. ( 중간에 볼려고 했는데, 스크롤 위치초기화되면 보기 불편하니까 도입 )
		CRect rect;
		MEMORY_VIEW_list_ctrl.GetWindowRect(&rect);
		int scrollPosY = MEMORY_VIEW_list_ctrl.GetScrollPos(SB_VERT);


		// 리스트 컨트롤의 화면 업데이트 비활성화 ( 자연스러운 화면전환 ) ( 이후 아이템 다 넣으면 다시 활성화 ) 
		MEMORY_VIEW_list_ctrl.SetRedraw(FALSE);


		/*
			메모리뷰 칼럼 아이템 초기화
		*/
		MEMORY_VIEW_list_ctrl.DeleteAllItems();




		/*
			메모리 뷰 아이템 넣기
		*/
		OUTPUT_DATA->Mem_Viewer.Output_Allocated_IOCTL_Address_for_return;
		OUTPUT_DATA->Mem_Viewer.Scan_SIZE;

		PUCHAR targetprocess_address = INPUT_DATA->Mem_Viewer.BaseAddress_of_TargetProcess;

		PUCHAR Start_Address = OUTPUT_DATA->Mem_Viewer.Output_Allocated_IOCTL_Address_for_return;
		PUCHAR End_Address = OUTPUT_DATA->Mem_Viewer.Output_Allocated_IOCTL_Address_for_return + OUTPUT_DATA->Mem_Viewer.Scan_SIZE;
		ULONG32 Jump_Size = 8;
		SIZE_T offset = 0;


		while (End_Address > Start_Address) {

			PUCHAR current_Address = Start_Address;
			PUCHAR current_Data = (PUCHAR)malloc(Jump_Size);
			memcpy(current_Data, current_Address, Jump_Size);


			CString Strings;

			//Offset 
			Strings.Format(_T("0x%02X"), offset);
			int nItem = MEMORY_VIEW_list_ctrl.InsertItem(MEMORY_VIEW_list_ctrl.GetItemCount(), Strings); // 첫 번째 열


			//Address 칼럼
			Strings.Format(_T("%p"), targetprocess_address);
			MEMORY_VIEW_list_ctrl.SetItemText(nItem, 1, Strings); // 2..


			// 8_Byte 칼럼
			CString HexString;
			for (size_t i = 0; i < Jump_Size; i++) {
				CString temp;
				temp.Format(_T("%02X "), current_Data[i]); // 1바이트씩 16진수로 변환
				HexString += temp; // 변환된 문자열을 HexString에 추가
			}
			MEMORY_VIEW_list_ctrl.SetItemText(nItem, 2, HexString); // 3

			//float32
			CString Float_Str;
			FLOAT float_data = 0.0;
			memcpy(&float_data, current_Data, sizeof(FLOAT));
			Float_Str.Format(_T("%f"), float_data);
			MEMORY_VIEW_list_ctrl.SetItemText(nItem, 3, Float_Str); // 4

			//double64
			CString Double_Str;
			DOUBLE double_data = 0.0;
			memcpy(&double_data, current_Data, sizeof(DOUBLE));
			Double_Str.Format(_T("%f"), double_data);
			MEMORY_VIEW_list_ctrl.SetItemText(nItem, 4, Double_Str); // 5


			// DWORD - ULONG32
			CString DWORD_Str;
			ULONG32 dword_data;
			memcpy(&dword_data, current_Data, sizeof(ULONG32));
			DWORD_Str.Format(_T("%lu"), dword_data);
			MEMORY_VIEW_list_ctrl.SetItemText(nItem, 5, DWORD_Str); // 6

			// QWORD - ULONG64
			CString QWORD_Str;
			ULONG64 qword_data;
			memcpy(&qword_data, current_Data, sizeof(ULONG64));
			QWORD_Str.Format(_T("%llu"), qword_data);
			MEMORY_VIEW_list_ctrl.SetItemText(nItem, 6, QWORD_Str); // 7

			// int32 
			CString int32_Str;
			INT32 int32_data;
			memcpy(&int32_data, current_Data, sizeof(INT32));
			int32_Str.Format(_T("%d"), int32_data);
			MEMORY_VIEW_list_ctrl.SetItemText(nItem, 7, int32_Str); // 8


			// int64
			CString int64_Str;
			INT64 int64_data;
			memcpy(&int64_data, current_Data, sizeof(INT64));
			int64_Str.Format(_T("%lld"), int64_data);
			MEMORY_VIEW_list_ctrl.SetItemText(nItem, 8, int64_Str); // 9


			//PTR64
			ULONG64 PTR64;
			memcpy(&PTR64, current_Data, sizeof(ULONG64));
			Strings.Format(_T("%016llX"), PTR64);
			MEMORY_VIEW_list_ctrl.SetItemText(nItem, 9, Strings); // 10

			//HEAP
			// 해당 주소가 유효한 주소인지 확인해야함 -> mem_commit인가? 

			//Ansi
			CString characters;
			for (size_t i = 0; i < Jump_Size; i++) {
				CString temp;
				temp.Format(_T("%c"), current_Data[i]); // 1바이트씩 16진수로 변환
				characters += temp; // 변환된 문자열을 HexString에 추가
			}
			MEMORY_VIEW_list_ctrl.SetItemText(nItem, 10, characters); // 12


			free(current_Data);

			targetprocess_address += Jump_Size;
			Start_Address += Jump_Size;
			offset += Jump_Size;

		}

		// 리스트 컨트롤의 화면 업데이트 활성화
		MEMORY_VIEW_list_ctrl.SetRedraw(TRUE);
		MEMORY_VIEW_list_ctrl.Invalidate();  // 강제로 다시 그리기


		// 저장한 스크롤 위치로 복원
		MEMORY_VIEW_list_ctrl.ScrollWindow(0, scrollPosY - MEMORY_VIEW_list_ctrl.GetScrollPos(SB_VERT));

		if (is_loop) {
			Sleep(1000);
			Get_Memory_Data();
		}

	}
	else {
		AfxMessageBox(_T("Memory_Viewer 실패"));
		return;
	}
}



// 리스트 컨트롤 특정 행 선택시
void MEM_VIEWER::OnNMClickMemoryViewList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// 클릭된 항목의 인덱스 가져오기
	int nItem = pNMItemActivate->iItem;

	// 유효한 인덱스인지 확인
	if (nItem != -1)
	{
		// Offset 칼럼(0번 칼럼)의 텍스트 가져오기
		CString offsetValue = MEMORY_VIEW_list_ctrl.GetItemText(nItem, 0);

		// CEdit 컨트롤에 Offset 값을 설정
		Edit_Memory_selected_offset_var.SetWindowTextW(offsetValue);
	}

	*pResult = 0;
}


















void MEM_VIEWER::OnBnClickedButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString PID_Str;
	Attached_PID_edit.GetWindowTextW(PID_Str);
	if (PID_Str.IsEmpty()) {
		AfxMessageBox(_T(" Please INPUT valid PID "));
		return;
	}
	else {
		selected_PID = (HANDLE)_ttoi(PID_Str); // CString을 DWORD로 변환 (PID는 일반적으로 DWORD 타입)

		PID_Str.Format(_T("%lu"), selected_PID);
		AfxMessageBox(PID_Str);
	}
}


















// 창닫기
void MEM_VIEWER::OnClose()
{
	is_loop = FALSE;
	EndDialog(IDD_MEMORY_VIEWER); // 또는 IDCANCEL
}

// is_loop? 체크박스
void MEM_VIEWER::OnBnClickedCheck1()
{
	// 루프요청인가?
	// 체크박스의 상태를 확인
	int checkStatus = is_loop_Check_box_var.GetCheck();

	if (checkStatus == BST_CHECKED) {
		is_loop = TRUE;
	}
	else if (checkStatus == BST_UNCHECKED) {
		is_loop = FALSE;
	}
	else if (checkStatus == BST_INDETERMINATE) {
		is_loop = FALSE;
	}
}


void MEM_VIEWER::OnBnClickedButton2()
{
	// 데이터 변조 -> Edit Memory 섹션버튼








	// Offset 값 확인 ( Offset값 가져오고, 이게 현재행에 존재하는 지 봐야함 ) 
	 // 1. CEdit에서 Offset 값 가져오기
	CString offsetString;
	Edit_Memory_selected_offset_var.GetWindowTextW(offsetString);

	// CString을 SIZE_T로 변환
	SIZE_T offsetValue = _tcstoul(offsetString, NULL, 16); // 16진수 문자열을 SIZE_T로 변환


	// 2. 리스트 컨트롤에서 특정 행 찾기
	int rowCount = MEMORY_VIEW_list_ctrl.GetItemCount();
	int targetRow = -1;

	for (int i = 0; i < rowCount; ++i)
	{
		CString itemOffsetString = MEMORY_VIEW_list_ctrl.GetItemText(i, 0);
		SIZE_T itemOffsetValue = _tcstoul(itemOffsetString, NULL, 16);

		if (itemOffsetValue == offsetValue)
		{
			targetRow = i;

			break;
		}
	}

	if (targetRow == -1) {
		AfxMessageBox(_T("설정한 Offset값이 현재 출력된 테이블에 없습니다.."));
		return;
	}




	// 칼럼 선택 ( 콤보 박스 ) 
	// 선택된 항목의 인덱스 가져오기
	int selectedIndex = Edit_Memory_Combo_BOX.GetCurSel();

	// 선택된 항목의 문자열 가져오기
	CString selectedString;
	Edit_Memory_Combo_BOX.GetLBText(selectedIndex, selectedString);

	
	
	// CString을 비교하여 조건에 따라 작업 수행
	if (selectedString == _T("8_Byte"))
	{
		Get_already_Data = MEMORY_VIEW_list_ctrl.GetItemText(targetRow, 2); // 3번째 칼럼
		
	}
	else if (selectedString == _T("Float32"))
	{
		Get_already_Data = MEMORY_VIEW_list_ctrl.GetItemText(targetRow, 3); // 3번째 칼럼
	}
	else if (selectedString == _T("Double64"))
	{
		Get_already_Data = MEMORY_VIEW_list_ctrl.GetItemText(targetRow, 4); // 4번째 칼럼
	}
	else if (selectedString == _T("DWORD"))
	{
		Get_already_Data = MEMORY_VIEW_list_ctrl.GetItemText(targetRow, 5); // 5번째 칼럼
	}
	else if (selectedString == _T("QWORD"))
	{
		Get_already_Data = MEMORY_VIEW_list_ctrl.GetItemText(targetRow, 6); // 6번째 칼럼
	}
	else if (selectedString == _T("int32"))
	{
		Get_already_Data = MEMORY_VIEW_list_ctrl.GetItemText(targetRow, 7); // 7번째 칼럼
	}
	else if (selectedString == _T("int64"))
	{
		Get_already_Data = MEMORY_VIEW_list_ctrl.GetItemText(targetRow, 8); // 8번째 칼럼
	}
	else if (selectedString == _T("PTR64"))
	{
		Get_already_Data = MEMORY_VIEW_list_ctrl.GetItemText(targetRow, 9); //9번째 칼럼
	}
	else
	{
		AfxMessageBox(_T("Unknown selection"));
		return;
	}

	// 변경할 데이터 -> 단, 칼럼에 따라 Input값이 다르기 때문에, "콤보박스"에서부터 인식하고, 변경되기전 현재 값을 Edit에 띄운다. ( 변경할데이터 식별이 가능한 장점 + 비교적 빠르고 가독성 높음 ) 
	//AfxMessageBox(Get_already_Data);

	Edit_Memory__Edit_Data_var.SetWindowTextW(Get_already_Data);
}













CString FilterAndCombineStrings2(const CString input)
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


void MEM_VIEWER::OnBnClickedButton3() {
	// 실제 커널 요청을 하는 함수
	if (Get_already_Data.IsEmpty()) {
		AfxMessageBox(_T("please check Edit Data first"));
		return;
	}

	// Offset 값 확인 ( Offset값 가져오고, 이게 현재행에 존재하는 지 봐야함 ) 
	 // 1. CEdit에서 Offset 값 가져오기
	CString offsetString;
	Edit_Memory_selected_offset_var.GetWindowTextW(offsetString);

	// CString을 SIZE_T로 변환
	SIZE_T offsetValue = _tcstoul(offsetString, NULL, 16); // 16진수 문자열을 SIZE_T로 변환


	// 2. 리스트 컨트롤에서 특정 행 찾기
	int rowCount = MEMORY_VIEW_list_ctrl.GetItemCount();
	int targetRow = -1;

	for (int i = 0; i < rowCount; ++i)
	{
		CString itemOffsetString = MEMORY_VIEW_list_ctrl.GetItemText(i, 0);
		SIZE_T itemOffsetValue = _tcstoul(itemOffsetString, NULL, 16);

		if (itemOffsetValue == offsetValue)
		{
			targetRow = i;

			break;
		}
	}

	if (targetRow == -1) {
		AfxMessageBox(_T("설정한 Offset값이 현재 출력된 테이블에 없습니다.."));
		return;
	}

	// 변경할 타겟 프로세스 주소 습득.//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	CStringA addressStringA( MEMORY_VIEW_list_ctrl.GetItemText(targetRow, 1) );// LPCSTR로 변환

	// 16진수 문자열을 unsigned long long (8바이트) 값으로 변환
	PUCHAR target_address_for_edit = (PUCHAR)strtoull(addressStringA, NULL, 16);
	SIZE_T target_address_for_edit_Size = 0;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	
	// IOCTL 요청자에 할당된 데이터 ( 이는 실제 커널에서 참조할 데이터로, 타겟프로세스에게 전달될 것. )
	PUCHAR Allocated_search_data = NULL;// = (PUCHAR)malloc(target_address_for_edit_Size);


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 칼럼 선택 ( 콤보 박스 ) 
	// 선택된 항목의 인덱스 가져오기
	int selectedIndex = Edit_Memory_Combo_BOX.GetCurSel();

	// 선택된 항목의 문자열 가져오기
	CString selectedString;
	Edit_Memory_Combo_BOX.GetLBText(selectedIndex, selectedString);


	PUCHAR tmp_data_pointer = NULL;

	// CString을 비교하여 조건에 따라 작업 수행
	if (selectedString == _T("8_Byte"))
	{
		Edit_Memory__Edit_Data_var.GetWindowTextW(Get_already_Data);
		Get_already_Data = FilterAndCombineStrings2(Get_already_Data); // Cstring내 빈문자열 제거 후 저장.
		

		/*
			[역순]
			123456 일때
			563412 로 역순취해야함.
			
			0x12 0x34 0x 56
			0x56 0x34 0x12
		
		*/
		ReversePairs_for_1Bytes(Get_already_Data);// 역순 ( 바이트 단위로 넣는 것이기 때문에, 역순으로 취해서 넣어야 했음 ) 


		CStringA hexStringA(Get_already_Data);
		ULONG64 dataValue = (ULONG64)strtoull(hexStringA, NULL, 16);
		CString a;
		a.Format(_T("%p"), dataValue);
		AfxMessageBox(a);

		tmp_data_pointer = (PUCHAR) & dataValue;
		target_address_for_edit_Size = sizeof(ULONG64);


		Allocated_search_data = (PUCHAR)malloc(target_address_for_edit_Size);
		memset(Allocated_search_data, 0, target_address_for_edit_Size);
		memcpy(Allocated_search_data, tmp_data_pointer, target_address_for_edit_Size);

	}
	else if (selectedString == _T("Float32"))
	{
		Edit_Memory__Edit_Data_var.GetWindowTextW(Get_already_Data);
		FLOAT floatValue = ::_tstof(Get_already_Data);

		tmp_data_pointer = (PUCHAR)&floatValue;
		target_address_for_edit_Size = sizeof(FLOAT);


		Allocated_search_data = (PUCHAR)malloc(target_address_for_edit_Size);
		memset(Allocated_search_data, 0, target_address_for_edit_Size);
		memcpy(Allocated_search_data, tmp_data_pointer, target_address_for_edit_Size);
	}
	else if (selectedString == _T("Double64"))
	{
		Edit_Memory__Edit_Data_var.GetWindowTextW(Get_already_Data);
		DOUBLE doubleValue = ::_wtof(Get_already_Data);

		tmp_data_pointer = (PUCHAR)&doubleValue;
		target_address_for_edit_Size = sizeof(DOUBLE);


		Allocated_search_data = (PUCHAR)malloc(target_address_for_edit_Size);
		memset(Allocated_search_data, 0, target_address_for_edit_Size);
		memcpy(Allocated_search_data, tmp_data_pointer, target_address_for_edit_Size);
	}
	else if (selectedString == _T("DWORD"))
	{
		Edit_Memory__Edit_Data_var.GetWindowTextW(Get_already_Data);
		CStringA dwordStringA(Get_already_Data);

		DWORD dwordValue = (DWORD)strtoul(dwordStringA, NULL, 10);
		CString a;
		a.Format(_T("%d"), dwordValue);
		AfxMessageBox(a);

		tmp_data_pointer = (PUCHAR)&dwordValue;
		target_address_for_edit_Size = sizeof(DWORD);


		Allocated_search_data = (PUCHAR)malloc(target_address_for_edit_Size);
		memset(Allocated_search_data, 0, target_address_for_edit_Size);
		memcpy(Allocated_search_data, tmp_data_pointer, target_address_for_edit_Size);
	}
	else if (selectedString == _T("QWORD"))
	{
		Edit_Memory__Edit_Data_var.GetWindowTextW(Get_already_Data);
		CStringA qwordStringA(Get_already_Data);

		QWORD qwordValue = (QWORD)strtoul(qwordStringA, NULL, 10);

		tmp_data_pointer = (PUCHAR)&qwordValue;
		target_address_for_edit_Size = sizeof(QWORD);


		Allocated_search_data = (PUCHAR)malloc(target_address_for_edit_Size);
		memset(Allocated_search_data, 0, target_address_for_edit_Size);
		memcpy(Allocated_search_data, tmp_data_pointer, target_address_for_edit_Size);
	}
	else if (selectedString == _T("int32"))
	{
		Edit_Memory__Edit_Data_var.GetWindowTextW(Get_already_Data);
		CStringA int32StringA(Get_already_Data);

		INT32 int32Value = (INT32)strtoul(int32StringA, NULL, 10);

		tmp_data_pointer = (PUCHAR)&int32Value;
		target_address_for_edit_Size = sizeof(INT32);


		Allocated_search_data = (PUCHAR)malloc(target_address_for_edit_Size);
		memset(Allocated_search_data, 0, target_address_for_edit_Size);
		memcpy(Allocated_search_data, tmp_data_pointer, target_address_for_edit_Size);
	}
	else if (selectedString == _T("int64"))
	{
		Edit_Memory__Edit_Data_var.GetWindowTextW(Get_already_Data);
		CStringA int64StringA(Get_already_Data);

		INT32 int64Value = (INT32)strtoul(int64StringA, NULL, 10);

		tmp_data_pointer = (PUCHAR)&int64Value;
		target_address_for_edit_Size = sizeof(INT64);


		Allocated_search_data = (PUCHAR)malloc(target_address_for_edit_Size);
		memset(Allocated_search_data, 0, target_address_for_edit_Size);
		memcpy(Allocated_search_data, tmp_data_pointer, target_address_for_edit_Size);
	}
	else if (selectedString == _T("PTR64"))
	{
		Edit_Memory__Edit_Data_var.GetWindowTextW(Get_already_Data);
		CStringA hexStringA(Get_already_Data);
		ULONG64 dataValue = strtoull(hexStringA, NULL, 16);

		tmp_data_pointer = (PUCHAR)&dataValue;
		target_address_for_edit_Size = sizeof(ULONG64);


		Allocated_search_data = (PUCHAR)malloc(target_address_for_edit_Size);
		memset(Allocated_search_data, 0, target_address_for_edit_Size);
		memcpy(Allocated_search_data, tmp_data_pointer, target_address_for_edit_Size);
	}
	else
	{
		AfxMessageBox(_T("Unknown selection"));
		return;
	}

	

	//커널 요청하기.
	PIOCTL_info INPUT_DATA = (PIOCTL_info)malloc(sizeof(IOCTL_info));
	memset(INPUT_DATA, 0, sizeof(IOCTL_info));// 필수 
	INPUT_DATA->information = editing;///////////////////////////////////////////////////////////////////////////
	INPUT_DATA->Ioctl_requestor_PID = (HANDLE)GetCurrentProcessId();

	WaitForSingleObject(IOCTL_info_From_Main_Window->IOCTL_User_Mutex, INFINITE);
	INPUT_DATA->target_process_info = IOCTL_info_From_Main_Window->target_process_info; // 커널에서 이 정보로 ATTACH할 것.
	ReleaseMutex(IOCTL_info_From_Main_Window->IOCTL_User_Mutex);

	INPUT_DATA->editing_info.edit_destination_Targetprocess_address = target_address_for_edit;

	INPUT_DATA->editing_info.ioctl_data_for_editing = Allocated_search_data;
	INPUT_DATA->editing_info.ioctl_data_for_editing_SIZE = target_address_for_edit_Size;
	
	INPUT_DATA->editing_info.is_same_value = FALSE;




	PIOCTL_info OUTPUT_DATA = NULL;
	if (Send_to_Kernel(INPUT_DATA, TRUE, &OUTPUT_DATA, NULL) && OUTPUT_DATA) {
		AfxMessageBox(_T("Edit_Memory_성공"));
	}
	else {
		AfxMessageBox(_T("Edit_Memory_실패"));
	}



}