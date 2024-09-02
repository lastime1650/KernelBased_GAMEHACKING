// MEM_PAGE_SCAN_CLASS.cpp: 구현 파일
//

#include "pch.h"
#include "MFC_GAME_HACK.h"
#include "afxdialogex.h"
#include "MEM_PAGE_SCAN_CLASS.h"


// MEM_PAGE_SCAN_CLASS 대화 상자

IMPLEMENT_DYNAMIC(MEM_PAGE_SCAN_CLASS, CDialogEx)

MEM_PAGE_SCAN_CLASS::MEM_PAGE_SCAN_CLASS(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MEMORY_PAGE_SCAN, pParent)
{

}

MEM_PAGE_SCAN_CLASS::~MEM_PAGE_SCAN_CLASS()
{
}

void MEM_PAGE_SCAN_CLASS::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, MEM_PAGE_SCAN_LIST_CONTROL_ID, MEM_PAGE_SCAN_LIST_CTRL);
}


BEGIN_MESSAGE_MAP(MEM_PAGE_SCAN_CLASS, CDialogEx)
	ON_NOTIFY(LVN_ITEMCHANGED, MEM_PAGE_SCAN_LIST_CONTROL_ID, &MEM_PAGE_SCAN_CLASS::OnLvnItemchangedPageScanListControlId)
END_MESSAGE_MAP()


// MEM_PAGE_SCAN_CLASS 메시지 처리기

//초기
BOOL MEM_PAGE_SCAN_CLASS::OnInitDialog() {

	CDialogEx::OnInitDialog();

	Start_Page_Scan();


	return TRUE;
}

VOID MEM_PAGE_SCAN_CLASS::Start_Page_Scan() {

	MEM_PAGE_SCAN_LIST_CTRL.DeleteAllItems();


	// 모든 열 제거
	int nColumnCount = MEM_PAGE_SCAN_LIST_CTRL.GetHeaderCtrl()->GetItemCount();
	for (int i = nColumnCount - 1; i >= 0; i--) {
		MEM_PAGE_SCAN_LIST_CTRL.DeleteColumn(i);
	}

	MEM_PAGE_SCAN_LIST_CTRL.ModifyStyle(0, LVS_REPORT);
	MEM_PAGE_SCAN_LIST_CTRL.InsertColumn(0, _T("count"), LVCFMT_LEFT, 100);
	MEM_PAGE_SCAN_LIST_CTRL.InsertColumn(1, _T("BaseAddress"), LVCFMT_LEFT, 200);
	MEM_PAGE_SCAN_LIST_CTRL.InsertColumn(2, _T("RegionSize"), LVCFMT_LEFT, 100);
	MEM_PAGE_SCAN_LIST_CTRL.InsertColumn(3, _T("Type"), LVCFMT_LEFT, 100);
	MEM_PAGE_SCAN_LIST_CTRL.InsertColumn(4, _T("Protect"), LVCFMT_LEFT, 100);
	MEM_PAGE_SCAN_LIST_CTRL.InsertColumn(5, _T("State"), LVCFMT_LEFT, 100);
	MEM_PAGE_SCAN_LIST_CTRL.InsertColumn(6, _T("AllocationBase"), LVCFMT_LEFT, 200);
	MEM_PAGE_SCAN_LIST_CTRL.InsertColumn(7, _T("AllocationProtect"), LVCFMT_LEFT, 100);

	// 칼럼이 상당히 많다. / 가로 스크롤 삽입!!
	MEM_PAGE_SCAN_LIST_CTRL.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 행 선택도 가능.
	MEM_PAGE_SCAN_LIST_CTRL.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);


	// 커널 질의 후 결과에 따라서 칼럼 값 넣기
	PIOCTL_info INPUT_DATA = (PIOCTL_info)malloc(sizeof(IOCTL_info));
	memset(INPUT_DATA, 0, sizeof(IOCTL_info));// 필수 
	INPUT_DATA->information = Memory_Page_Scan;///////////////////////////////////////////////////////////////////////////
	INPUT_DATA->Ioctl_requestor_PID = (HANDLE)GetCurrentProcessId();

	WaitForSingleObject(IOCTL_info_From_Main_Window->IOCTL_User_Mutex, INFINITE);
	INPUT_DATA->target_process_info = IOCTL_info_From_Main_Window->target_process_info; // 커널에서 이 정보로 ATTACH할 것.
	ReleaseMutex(IOCTL_info_From_Main_Window->IOCTL_User_Mutex);


	PIOCTL_info OUTPUT_DATA = NULL;
	if (Send_to_Kernel(INPUT_DATA, TRUE, &OUTPUT_DATA, NULL) && OUTPUT_DATA && OUTPUT_DATA->Mem_Page_scan_viewer.Output_Ioctl_user_Start_Address) {

		CString message;
		message.Format(_T("Memory_PAGE 성공 -> %p "), OUTPUT_DATA->Mem_Page_scan_viewer.Output_Ioctl_user_Start_Address);

		AfxMessageBox(message);
		/* 조회 */
		PPAGE_SCAN_NODE current = OUTPUT_DATA->Mem_Page_scan_viewer.Output_Ioctl_user_Start_Address;
		SIZE_T count = 0;
		while (current != NULL) {
			
			current->BaseAddress;
			current->RegionSize;
			current->State;
			current->Protect;
			current->Type;
			current->AllocationBase;
			current->AllocationProtect;

			CString string;

			// count
			string.Format(_T("%llu"), count);

			// 아이템 추가시작
			int nItem = MEM_PAGE_SCAN_LIST_CTRL.InsertItem(MEM_PAGE_SCAN_LIST_CTRL.GetItemCount(), string); // 첫 번째 열

			// BaseAddress;
			string.Format(_T("0x%p"), current->BaseAddress);
			MEM_PAGE_SCAN_LIST_CTRL.SetItemText(nItem, 1, string); // 2..

			// RegionSize;
			string.Format(_T("%llu"), current->RegionSize);
			MEM_PAGE_SCAN_LIST_CTRL.SetItemText(nItem, 2, string); 







			// Type;
			switch (current->Type) {
			case MEM_IMAGE:
				string = _T("MEM_IMAGE");
				break;
			case MEM_MAPPED:
				string = _T("MEM_MAPPED");
				break;
			case MEM_PRIVATE:
				string = _T("MEM_PRIVATE");
				break;
			default:
				string = _T("UNKNOWN");
				break;
			}
			MEM_PAGE_SCAN_LIST_CTRL.SetItemText(nItem, 3, string);






			// Protect;
			switch (current->Protect) {
			case PAGE_READONLY:
				string = _T("PAGE_READONLY");
				break;
			case PAGE_READWRITE:
				string = _T("PAGE_READWRITE");
				break;
			case PAGE_EXECUTE:
				string = _T("PAGE_EXECUTE");
				break;
			case PAGE_EXECUTE_READ:
				string = _T("PAGE_EXECUTE_READ");
				break;
			case PAGE_EXECUTE_READWRITE:
				string = _T("PAGE_EXECUTE_READWRITE");
				break;
			case PAGE_NOACCESS:
				string = _T("PAGE_NOACCESS");
				break;
				// 필요한 다른 PAGE_* 상수들에 대한 케이스 추가
			default:
				string = _T("UNKNOWN");
				break;
			}
			MEM_PAGE_SCAN_LIST_CTRL.SetItemText(nItem, 4, string);

			

			// State;
			switch (current->State) {
			case MEM_COMMIT:
				string = _T("MEM_COMMIT");
				break;
			case MEM_FREE:
				string = _T("MEM_FREE");
				break;
			case MEM_RESERVE:
				string = _T("MEM_RESERVE");
				break;
			default:
				string = _T("UNKNOWN");
				break;
			}
			MEM_PAGE_SCAN_LIST_CTRL.SetItemText(nItem, 5, string);



			// AllocationBase;
			string.Format(_T("0x%p"), current->AllocationBase);
			MEM_PAGE_SCAN_LIST_CTRL.SetItemText(nItem, 6, string);





			// AllocationProtect;
			switch (current->AllocationProtect) {
			case PAGE_READONLY:
				string = _T("PAGE_READONLY");
				break;
			case PAGE_READWRITE:
				string = _T("PAGE_READWRITE");
				break;
			case PAGE_EXECUTE:
				string = _T("PAGE_EXECUTE");
				break;
			case PAGE_EXECUTE_READ:
				string = _T("PAGE_EXECUTE_READ");
				break;
			case PAGE_EXECUTE_READWRITE:
				string = _T("PAGE_EXECUTE_READWRITE");
				break;
			case PAGE_NOACCESS:
				string = _T("PAGE_NOACCESS");
				break;
				// 필요한 다른 PAGE_* 상수들에 대한 케이스 추가
			default:
				string = _T("UNKNOWN");
				break;
			}
			MEM_PAGE_SCAN_LIST_CTRL.SetItemText(nItem, 7, string);
			

			count++;
			current = (PPAGE_SCAN_NODE)current->Next_Addr;
		}
		
	}
	else {
		AfxMessageBox(_T("Memory_PAGE 실패"));
	}

	free(INPUT_DATA);

	return;
}




void MEM_PAGE_SCAN_CLASS::OnLvnItemchangedPageScanListControlId(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}
