
// MFC_GAME_HACKDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MFC_GAME_HACK.h"
#include "MFC_GAME_HACKDlg.h"
#include "afxdialogex.h"

#include "Color.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCGAMEHACKDlg 대화 상자



CMFCGAMEHACKDlg::CMFCGAMEHACKDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFC_GAME_HACK_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCGAMEHACKDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_Address_Table, Address_Table_LISTCTRL);
	DDX_Control(pDX, LIST_BOX_Attach, LIST_BOX_BaseAddress);
	DDX_Control(pDX, EDIT_ADDRESS_INPUT, EDIT_ADDRESS_INPUT_inst);
}

BEGIN_MESSAGE_MAP(CMFCGAMEHACKDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(Input_Address_or_Offset_BUTTON, &CMFCGAMEHACKDlg::OnBnClickedOk)
	ON_BN_CLICKED(CHECK_BOX_for_Input_Address_or_Offset, &CMFCGAMEHACKDlg::OnBnClickedCheck1)



	ON_WM_CTLCOLOR()
	ON_STN_CLICKED(IDC_STATIC_Address_Table, &CMFCGAMEHACKDlg::OnStnClickedStaticAddressTable)
	ON_COMMAND(ID_ATTACH_STARTATTACH, &CMFCGAMEHACKDlg::OnAttachStartattach)
	ON_COMMAND(ID_MEMORY_STARTMEMORYSCAN, &CMFCGAMEHACKDlg::OnMemoryStartmemoryscan)
	ON_COMMAND(ID_DUMP_PROCESSDUMP, &CMFCGAMEHACKDlg::OnDumpProcessdump)
	ON_COMMAND(ID_HELP_HOWTOUSEIT, &CMFCGAMEHACKDlg::OnHelpHowtouseit)


END_MESSAGE_MAP()


// CMFCGAMEHACKDlg 메시지 처리기

#include "IOCTL.h"

BOOL CMFCGAMEHACKDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.






	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	is_attached = FALSE;
	
	Initialize_by_Attach();

	// IOCTL 동적할당
	Input_Data = (PIOCTL_info)malloc(sizeof(IOCTL_info));

	// 비동기 스레드 시작 ( 커널에게 지속적으로 Loop_Chekc 신호를 보내고, 반환에서 작업 큐를 회수해야함 ) 
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Kernel_LoopCheck, NULL, 0, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMFCGAMEHACKDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCGAMEHACKDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCGAMEHACKDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CMFCGAMEHACKDlg::OnCtlColorStatic(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)lParam;

	

	if (::GetDlgCtrlID(hWnd) == IDC_STATIC_Address_Table)
		return (LRESULT)CBrush(BLUE).GetSafeHandle();
	
	return DefWindowProc(WM_CTLCOLORSTATIC, wParam, lParam);
}

// 초기화 ( Attach 상태 변경 발각시 호출됨 )
void CMFCGAMEHACKDlg::Initialize_by_Attach() {


	// 어태치 안됨
	/* 주소 입력 구간 초기화 (어태치 되면 Enable 처리할 것 ) */
	On_Off_from_ID(Input_Address_or_Offset_BUTTON, TRUE, "please Attach first");
	On_Off_from_ID(CHECK_BOX_for_Input_Address_or_Offset, TRUE, "cant choose");

	/* Attach 결과 문자열 초기화 */
	On_Off_from_ID(EDIT_Attach_STATUS, TRUE, "Please Attach First");
	On_Off_from_ID(EDIT_Attach_PID, TRUE, "Please Attach First");
	On_Off_from_ID(EDIT_Attach_ImageName, TRUE, "Please Attach First");


	/*
		리스트 박스 컨트롤 (초기화)==> 칼럼 생성
	*/
	// 리스트 컨트롤에 LVS_REPORT 스타일을 추가
	Address_Table_LISTCTRL.ModifyStyle(0, LVS_REPORT);
	Address_Table_LISTCTRL.InsertColumn(0, _T("Address"), LVCFMT_LEFT, 200);
	Address_Table_LISTCTRL.InsertColumn(1, _T("Offset"), LVCFMT_LEFT, 150);


	LIST_BOX_BaseAddress.InsertString(0, _T("Please Attach First\n"));
	LIST_BOX_BaseAddress.InsertString(1, _T("\"go to Menu_bar -> Attach Process -> Start Attach\" \n"));
	

}


// 버튼 클릭 핸들러 구현
void CMFCGAMEHACKDlg::On_Off_from_ID(ULONG32 item_ID, BOOLEAN is_Disable, LPCSTR Caption)
{

	// 버튼 비활성화
	CWnd* pButton = GetDlgItem((int)item_ID); // 버튼 ID로 버튼 핸들 가져오기
	if (is_Disable)
	{
		pButton->EnableWindow(FALSE); // 버튼 비활성화
	}
	else {
		pButton->EnableWindow(TRUE); // 버튼 활성화
	}

	// Caption 처리
	if (Caption != NULL) {
		CString cstrstring(Caption);
		pButton->SetWindowTextW(cstrstring);
	}
}


void CMFCGAMEHACKDlg::OnBnClickedOk()
{
	// ID : Input_Address_or_Offset_BUTTON
	// 이 이벤트는 ATTACH된 후 활성화된 버튼을 통해 호출되는 이벤트함수 

	// 입력한 데이터(Attach된 타겟 프로세스의 가상주소 )를 칼럼에 등록
	// 입력한 정보가 주소인지 오프셋인지 확인하고 접근해야함 

	// Check박스 확인
	CWnd* Check_BOX = GetDlgItem((int)CHECK_BOX_for_Input_Address_or_Offset);
	if (Check_BOX) {
		BOOLEAN is_offset = FALSE;

		// CWnd*를 CButton*로 캐스팅
		CButton* pCheckBox = ( CButton* )Check_BOX;

		if (pCheckBox->GetCheck() == BST_CHECKED) {
			is_offset = TRUE;
		}

		// 주소 입력칸 확인
		CWnd* Edit_Box = GetDlgItem((int)EDIT_ADDRESS_INPUT);
		CEdit* pEdit_Box = (CEdit*)Edit_Box;
		
		// 입력칸 검증 
		CString strText;
		pEdit_Box->GetWindowText(strText);

		if (strText.IsEmpty()) {
			AfxMessageBox(_T("입력값이 비어있습니다. 주소 및 오프셋값을 입력하세요"));
			return;
		}

		// 입력한 데이터 확인
		if (is_offset) {
			// offset 값임 
			/*
				BASEADDRESS + offset 한 후 칼럼에 등록
			*/
			AfxMessageBox(_T("체크박스가 체크되었습니다."));
		}
		else {
			// 일반 주소값임 


			/*
				(PUCHAR)로 변환 후 그대로 등록
			*/
			PUCHAR Input_Address = NULL;

			
			CStringA strAnsi(strText);
			// 진짜 주소값인지확인해야함. 
			for (int i = 0; i < strAnsi.GetLength(); i++)
			{
				char ch = strAnsi[i];
				if (!(ch >= '0' && ch <= '9') &&  // 0-9
					!(ch >= 'a' && ch <= 'f') &&  // a-f
					!(ch >= 'A' && ch <= 'F'))    // A-F
				{
					AfxMessageBox(_T("주소를 해석할 수 없습니다."));
					return;
				}
			}

			Input_Address = (PUCHAR)(strtoull (strAnsi, NULL, 16)); // strtoull --> unsigned long long ( 8바이트 체계 까지 식별 됨 ) / 단, 모든 문자 하나하나 16진수인지 검증을 안함.

			if (Input_Address == NULL) {
				AfxMessageBox(_T("주소를 해석할 수 없습니다."));
				return;
			}


			CString strMessage;
			strMessage.Format(_T("주소 등록 %p "), Input_Address);

			AfxMessageBox(strMessage);
			strMessage.Format(_T("%p"), Input_Address);

			// 칼럼에 등록 
			// 주소
			int nItem = Address_Table_LISTCTRL.InsertItem(0, strMessage); // 첫 번째 열

			// 계산 된 오프셋 ( BaseAddress 기준) 
			Address_Table_LISTCTRL.SetItemText(nItem, 1, strMessage); // 두 번째 열

		}

	}
	

	// 칼럼 등록

	
}


void CMFCGAMEHACKDlg::OnBnClickedCheck1()
{
	// ID : CHECK_BOX_for_Input_Address_or_Offset / 체크박스

	CWnd* Check_BOX = GetDlgItem((int)CHECK_BOX_for_Input_Address_or_Offset);
	

}





void CMFCGAMEHACKDlg::OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}




void CMFCGAMEHACKDlg::OnStnClickedStaticAddressTable()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


/* 메뉴바 이벤트 임 */
/*
// 메인 윈도우로부터 얻는 것 
typedef struct MOVE {
	BOOLEAN* is_Attach_from_Main;
	HANDLE* PID;
	CListCtrl* Address_Table_LISTCTRL_P;

	CWnd* PID_instance;
	CWnd* Attach_STATUS_instance;
	CWnd* ImageName_instance;

	CWnd* Check_Box_instance;
	CWnd* Address_Button_instance;

	PIOCTL_info Input_Data;

}MOVE, * PMOVE;

UINT test_thread(PMOVE parm) {

	ATTACH_PROCESS* TEST_INST = new ATTACH_PROCESS();

	if (TEST_INST->Create(IDD_ATTACH_PROCESS)) {

		TEST_INST->Parameter.is_Attach_from_Main = parm->is_Attach_from_Main;
		TEST_INST->Parameter.PID = parm->PID;
		//TEST_INST->Parameter.Address_Table_LISTCTRL_P = parm->Address_Table_LISTCTRL;

		TEST_INST->Parameter.PID_instance = parm->Attach_STATUS_instance;
		TEST_INST->Parameter.Attach_STATUS_instance = parm->Attach_STATUS_instance;
		TEST_INST->Parameter.ImageName_instance = parm->ImageName_instance;
		TEST_INST->Parameter.Check_Box_instance = parm->Check_Box_instance;
		TEST_INST->Parameter.Address_Button_instance = parm->Address_Button_instance;

		TEST_INST->Parameter.Input_Data = parm->Input_Data; // 전달. Attach가 성공되면, target_process_information 구조체에 값이 들어와있음 

		TEST_INST->ShowWindow(SW_SHOW); // 윈도우를 표시
		TEST_INST->RunModalLoop(); // 메시지 루프 시작 (모달 대화상자처럼 동작)
	}

	return 0;
}
*/
// Attach Start
void CMFCGAMEHACKDlg::OnAttachStartattach()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.

	
	Attach_process_dialog_inst.Parameter.is_Attach_from_Main = &is_attached;
	Attach_process_dialog_inst.Parameter.PID = &PID;
	Attach_process_dialog_inst.Parameter.Address_Table_LISTCTRL_P = &Address_Table_LISTCTRL;
	
	Attach_process_dialog_inst.Parameter.PID_instance = GetDlgItem((int)EDIT_Attach_PID);
	Attach_process_dialog_inst.Parameter.Attach_STATUS_instance = GetDlgItem((int)EDIT_Attach_STATUS);
	Attach_process_dialog_inst.Parameter.ImageName_instance = GetDlgItem((int)EDIT_Attach_ImageName);
	Attach_process_dialog_inst.Parameter.Check_Box_instance = GetDlgItem((int)CHECK_BOX_for_Input_Address_or_Offset);;
	Attach_process_dialog_inst.Parameter.Address_Button_instance = GetDlgItem((int)Input_Address_or_Offset_BUTTON);;

	Attach_process_dialog_inst.Parameter.Input_Data = Input_Data; // 전달. Attach가 성공되면, target_process_information 구조체에 값이 들어와있음 
	
	Attach_process_dialog_inst.DoModal();
	/*
	PMOVE test = (PMOVE)malloc(sizeof(MOVE));
	test->PID = &PID;
	test->is_Attach_from_Main = &is_attached;
	test->Address_Button_instance = GetDlgItem((int)Input_Address_or_Offset_BUTTON);
	test->Address_Table_LISTCTRL_P = &Address_Table_LISTCTRL;
	test->Attach_STATUS_instance = GetDlgItem((int)EDIT_Attach_STATUS);
	test->Check_Box_instance = GetDlgItem((int)CHECK_BOX_for_Input_Address_or_Offset);
	test->ImageName_instance = GetDlgItem((int)EDIT_Attach_ImageName);
	test->Input_Data = Input_Data;


	AfxBeginThread((AFX_THREADPROC)test_thread, test);
	*/
}


// Memory Scan 
void CMFCGAMEHACKDlg::OnMemoryStartmemoryscan()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


/*
	비동기 덤프 열기
*/
VOID APCCC(PVOID a) {
	AfxMessageBox(_T("열림"));
	DUMP_PROCESS* p_Dump_Process = new DUMP_PROCESS();
	if (p_Dump_Process->Create(IDD_DUMP_PROCESS)) {
		p_Dump_Process->ShowWindow(SW_SHOW);
		p_Dump_Process->RunModalLoop();
	}
}


// DUMP -> Process
void CMFCGAMEHACKDlg::OnDumpProcessdump()
{

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)APCCC, NULL, 0, NULL);


	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	/*
	DUMP_PROCESS* p_Dump_Process = new DUMP_PROCESS();
	if (p_Dump_Process->Create(IDD_DUMP_PROCESS)) {
		p_Dump_Process->ShowWindow(SW_SHOW); 
		p_Dump_Process->RunModalLoop();
	}
	*/
}



// Help -> How to use it?
void CMFCGAMEHACKDlg::OnHelpHowtouseit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}



