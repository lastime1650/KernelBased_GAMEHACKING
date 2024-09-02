// HARDWARE_BP_C.cpp: 구현 파일
//

#include "pch.h"
#include "MFC_GAME_HACK.h"
#include "afxdialogex.h"
#include "HARDWARE_BP_C.h"


// HARDWARE_BP_C 대화 상자

IMPLEMENT_DYNAMIC(HARDWARE_BP_C, CDialogEx)

HARDWARE_BP_C::HARDWARE_BP_C(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HARDWARE_BP, pParent)
{

}

HARDWARE_BP_C::~HARDWARE_BP_C()
{
}

void HARDWARE_BP_C::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DR0_EDIT, DR0_EDIT);
	DDX_Control(pDX, IDC_DEBUG_PID_EDIT, DEBUG_PID_EDIT);
	DDX_Control(pDX, IDC_DR0_BUTTON, DR0_BUTTON);
	DDX_Control(pDX, IDC_DEBUG_LIST, DEBuG_LIST_Var);
}


BEGIN_MESSAGE_MAP(HARDWARE_BP_C, CDialogEx)
	ON_WM_CLOSE()

	ON_BN_CLICKED(IDC_DR0_BUTTON, &HARDWARE_BP_C::OnBnClickedDr0Button)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_DEBUG_LIST, &HARDWARE_BP_C::OnLvnItemchangedDebugList)
END_MESSAGE_MAP()


BOOL HARDWARE_BP_C::OnInitDialog() {
	CDialogEx::OnInitDialog();

	/* 리스트 컨트롤 차트 초기화 */
	/*
		1. id
		2. count
		3. RIP
		4. Read
		5. Write
		6. Dr6

		7+n. ... 레지스터들
	*/
	CONTEXT a;
	DEBuG_LIST_Var.InsertColumn(0, _T("id"), LVCFMT_LEFT, 100);  // 이미 생성된 열

	DEBuG_LIST_Var.InsertColumn(1, _T("count"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(2, _T("RIP"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(3, _T("Read"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(4, _T("Write"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(5, _T("Dr6"), LVCFMT_LEFT, 100);

	DEBuG_LIST_Var.InsertColumn(6, _T("Rax"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(7, _T("Rcx"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(8, _T("Rdx"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(9, _T("Rbx"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(10, _T("Rsp"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(11, _T("Rbp"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(12, _T("Rsi"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(13, _T("Rdi"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(14, _T("R8"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(15, _T("R9"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(16, _T("R10"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(17, _T("R11"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(18, _T("R12"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(19, _T("R13"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(20, _T("R14"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(21, _T("R15"), LVCFMT_LEFT, 100);

	DEBuG_LIST_Var.InsertColumn(22, _T("Xmm0"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(23, _T("Xmm1"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(24, _T("Xmm2"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(25, _T("Xmm3"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(26, _T("Xmm4"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(27, _T("Xmm5"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(28, _T("Xmm6"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(29, _T("Xmm7"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(30, _T("Xmm8"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(31, _T("Xmm9"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(32, _T("Xmm10"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(33, _T("Xmm11"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(34, _T("Xmm12"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(35, _T("Xmm13"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(36, _T("Xmm14"), LVCFMT_LEFT, 100);
	DEBuG_LIST_Var.InsertColumn(37, _T("Xmm15"), LVCFMT_LEFT, 100);

	// 칼럼이 상당히 많다. / 가로 스크롤 삽입!!
	DEBuG_LIST_Var.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 행 선택도 가능.
	DEBuG_LIST_Var.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);










	/* PID창 비활성화 및 초기화 */
	DEBUG_PID_EDIT.EnableWindow(FALSE);

	CString PID;
	PID.Format(_T("%lld"), IOCTL_info_From_Main_Window->target_process_info.target_PID);
	DEBUG_PID_EDIT.SetWindowTextW(PID);


	/* 비동기 GET 스레드가져오기  */
	HWBP_thread_DATA = { 0, };
	HWBP_thread_DATA.PID = IOCTL_info_From_Main_Window->target_process_info.target_PID;
	HWBP_thread_DATA.is_close_window = FALSE;
	HWBP_thread_DATA.List_Control_var = &DEBuG_LIST_Var; // 비동기 스레드에서 사용할 리스트 컨트롤

	/*
		지속적으로 하드웨어 결과를 얻는 
		비동기 스레드 실행.
	*/
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HardWare_BP_Get_THREAD, &HWBP_thread_DATA, 0, NULL);

	return TRUE;
}

// HARDWARE_BP_C 메시지 처리기


void HARDWARE_BP_C::OnBnClickedDr0Button()
{

	// PID
	CString String;
	DEBUG_PID_EDIT.GetWindowTextW(String);
	CStringA StringA1(String);
	HANDLE PID = (HANDLE)strtoull(StringA1, NULL, 10);



	// DR0
	DR0_EDIT.GetWindowTextW(String);
	CStringA StringA2(String);
	PUCHAR pAddress = (PUCHAR)strtoull(StringA2, NULL, 16);

	/*
		커널 요청
	*/
	PIOCTL_info INPUT_DATA = (PIOCTL_info)malloc(sizeof(IOCTL_info));
	memset(INPUT_DATA, 0, sizeof(IOCTL_info));// 필수 
	INPUT_DATA->information = Hardware_breakpoint_set;
	INPUT_DATA->Ioctl_requestor_PID = (HANDLE)GetCurrentProcessId();

	INPUT_DATA->target_process_info.target_PID = PID;

	INPUT_DATA->HWBP_info.SET.Monitor_TargetProcess_Address = (ULONG64)pAddress;
	INPUT_DATA->HWBP_info.SET.is_SUCCESS = FALSE;

	PIOCTL_info OUTPUT_DATA = NULL;
	if (Send_to_Kernel(INPUT_DATA, TRUE, &OUTPUT_DATA, NULL) && OUTPUT_DATA) {
		AfxMessageBox(_T("하드웨어 BP 성공"));
	}
	else {
		AfxMessageBox(_T("하드웨어 BP 실패"));
	}
}


// 창닫기
void HARDWARE_BP_C::OnClose()
{
	HWBP_thread_DATA.is_close_window = FALSE;
	EndDialog(IDD_MEMORY_VIEWER); // 또는 IDCANCEL
}


/* 비동기 GET 스레드  */
VOID HardWare_BP_Get_THREAD(PHW_BP_thread_parm parm) {
	/* 만약 이 창이 닫아지면, 이 스레드도 닫혀야하므로 부울 포인터를 얻어야함 */
	while (1) {
		if (parm->is_close_window == FALSE) {
			PIOCTL_info INPUT_DATA = (PIOCTL_info)malloc(sizeof(IOCTL_info));
			memset(INPUT_DATA, 0, sizeof(IOCTL_info));// 필수 
			INPUT_DATA->information = Hardware_breakpoint_request_for_GET;
			INPUT_DATA->Ioctl_requestor_PID = (HANDLE)GetCurrentProcessId();

			INPUT_DATA->target_process_info.target_PID = parm->PID;

			INPUT_DATA->HWBP_info.GET.OUTPUT = NULL;

			PIOCTL_info OUTPUT_DATA = NULL;
			if (Send_to_Kernel(INPUT_DATA, TRUE, &OUTPUT_DATA, NULL) && OUTPUT_DATA && OUTPUT_DATA->HWBP_info.GET.OUTPUT) {
				

				PHardWare_BP_struct_from_targetprocess Target_info = OUTPUT_DATA->HWBP_info.GET.OUTPUT;
				

				AfxMessageBox(_T("하드웨어 BP 성공"));

				/* 테이블에 추가 */
				int nItem = parm->List_Control_var->InsertItem(0, _T("id"));  // 아이템 추가

				// CString을 사용하여 각 필드를 포맷하고 리스트에 삽입
				CString str;

				// count
				str.Format(_T("%u"), 0);
				parm->List_Control_var->SetItemText(nItem, 1, str);

				// RIP
				str.Format(_T("0x%016llX"), Target_info->context.Rip);
				parm->List_Control_var->SetItemText(nItem, 2, str);
				
				// Read
				str.Format(_T("0x%08X"), 1);//read
				parm->List_Control_var->SetItemText(nItem, 3, str);

				// Write
				str.Format(_T("0x%08X"),2 );//write
				parm->List_Control_var->SetItemText(nItem, 4, str);

				// Dr6
				str.Format(_T("0x%08X"), Target_info->context.Dr6);
				parm->List_Control_var->SetItemText(nItem, 5, str);

				// Rax
				str.Format(_T("0x%016llX"), Target_info->context.Rax);
				parm->List_Control_var->SetItemText(nItem, 6, str);

				// Rcx
				str.Format(_T("0x%016llX"), Target_info->context.Rcx);
				parm->List_Control_var->SetItemText(nItem, 7, str);

				// Rdx
				str.Format(_T("0x%016llX"), Target_info->context.Rdx);
				parm->List_Control_var->SetItemText(nItem, 8, str);

				// Rbx
				str.Format(_T("0x%016llX"), Target_info->context.Rbx);
				parm->List_Control_var->SetItemText(nItem, 9, str);

				// Rsp
				str.Format(_T("0x%016llX"), Target_info->context.Rsp);
				parm->List_Control_var->SetItemText(nItem, 10, str);

				// Rbp
				str.Format(_T("0x%016llX"), Target_info->context.Rbp);
				parm->List_Control_var->SetItemText(nItem, 11, str);

				// Rsi
				str.Format(_T("0x%016llX"), Target_info->context.Rsi);
				parm->List_Control_var->SetItemText(nItem, 12, str);

				// Rdi
				str.Format(_T("0x%016llX"), Target_info->context.Rdi);
				parm->List_Control_var->SetItemText(nItem, 13, str);

				// R8
				str.Format(_T("0x%016llX"), Target_info->context.R8);
				parm->List_Control_var->SetItemText(nItem, 14, str);

				// R9
				str.Format(_T("0x%016llX"), Target_info->context.R9);
				parm->List_Control_var->SetItemText(nItem, 15, str);

				// R10
				str.Format(_T("0x%016llX"), Target_info->context.R10);
				parm->List_Control_var->SetItemText(nItem, 16, str);

				// R11
				str.Format(_T("0x%016llX"), Target_info->context.R11);
				parm->List_Control_var->SetItemText(nItem, 17, str);

				// R12
				str.Format(_T("0x%016llX"), Target_info->context.R12);
				parm->List_Control_var->SetItemText(nItem, 18, str);

				// R13
				str.Format(_T("0x%016llX"), Target_info->context.R13);
				parm->List_Control_var->SetItemText(nItem, 19, str);

				// R14
				str.Format(_T("0x%016llX"), Target_info->context.R14);
				parm->List_Control_var->SetItemText(nItem, 20, str);

				// R15
				str.Format(_T("0x%016llX"), Target_info->context.R15);
				parm->List_Control_var->SetItemText(nItem, 21, str);

				// Xmm0
				str.Format(_T("0x%016llX%016llX"), Target_info->context.Xmm0.High, Target_info->context.Xmm0.Low);
				parm->List_Control_var->SetItemText(nItem, 22, str);

				// Xmm1
				str.Format(_T("0x%016llX%016llX"), Target_info->context.Xmm1.High, Target_info->context.Xmm1.Low);
				parm->List_Control_var->SetItemText(nItem, 23, str);

				// Xmm2
				str.Format(_T("0x%016llX%016llX"), Target_info->context.Xmm2.High, Target_info->context.Xmm2.Low);
				parm->List_Control_var->SetItemText(nItem, 24, str);

				// Xmm3
				str.Format(_T("0x%016llX%016llX"), Target_info->context.Xmm3.High, Target_info->context.Xmm3.Low);
				parm->List_Control_var->SetItemText(nItem, 25, str);
				

				
			}
			else {
			}
		}
		else {
			return;
		}
		Sleep(1000);
	}
	
}

void HARDWARE_BP_C::OnLvnItemchangedDebugList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}
