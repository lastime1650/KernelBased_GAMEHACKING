// DUMP_PROCESS.cpp: 구현 파일
//

#include "pch.h"
#include "MFC_GAME_HACK.h"
#include "afxdialogex.h"
#include "DUMP_PROCESS.h"


// DUMP_PROCESS 대화 상자

IMPLEMENT_DYNAMIC(DUMP_PROCESS, CDialogEx)

DUMP_PROCESS::DUMP_PROCESS(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DUMP_PROCESS, pParent)
{

}

DUMP_PROCESS::~DUMP_PROCESS()
{
}

void DUMP_PROCESS::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DUMP_SAVE_PATH_EDIT, SAVE_PATH_EDIT);
	DDX_Control(pDX, IDC_DUMP_PROCESS_ID_EDIT, PROCESS_ID_EDIT);
}


BEGIN_MESSAGE_MAP(DUMP_PROCESS, CDialogEx)
	ON_WM_CLOSE()


	ON_BN_CLICKED(IDC_DUMP_PROCESS_ID_BUTTON, &DUMP_PROCESS::OnBnClickedDumpProcessIdButton)
	ON_EN_CHANGE(IDC_DUMP_SAVE_PATH_EDIT, &DUMP_PROCESS::OnEnChangeDumpSavePathEdit)
	ON_EN_CHANGE(IDC_DUMP_PROCESS_ID_EDIT, &DUMP_PROCESS::OnEnChangeDumpProcessIdEdit)
END_MESSAGE_MAP()


// DUMP_PROCESS 메시지 처리기

// 초기실행
BOOL DUMP_PROCESS::OnInitDialog() {
	CDialogEx::OnInitDialog();



	return 1;
}


void DUMP_PROCESS::OnBnClickedDumpProcessIdButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	/*
		SAVE_PATH 와 PROCESS_ID 2개의 EDIT을 가지고 값을 가져오고, 커널 질의
	*/
	CString Warning;

	CString SAVE_PATH_str;
	CString PID_str;

	LPCSTR SAVE_PATH_ANSI;

	SAVE_PATH_EDIT.GetWindowTextW(SAVE_PATH_str);
	
	BOOLEAN was_empty_path = FALSE;

	// 빈문자열인가?
	if (SAVE_PATH_str.IsEmpty()) {
		Warning.Format(_T("빈문자열입니다. / 기본경로인 \"C:\\sample\" 이름으로 저장하겠습니다. "));
		AfxMessageBox(Warning);
		SAVE_PATH_ANSI = "C:\\sample";
		was_empty_path = TRUE;
	}
	else {
		SAVE_PATH_ANSI = Convert___Wide__to__Ansi____(SAVE_PATH_str.GetString());
	}
	// 경로 생성
	

	//////////

	PROCESS_ID_EDIT.GetWindowTextW(PID_str);


	// PID 얻기
	DWORD dwPID = _ttoi(PID_str); // CString을 DWORD로 변환 (PID는 일반적으로 DWORD 타입)
	HANDLE PID = (HANDLE)dwPID;
	Warning.Format(_T("입력한 PID: %lld "), PID);
	AfxMessageBox(Warning);

	/*
		커널 요청
	*/
	PIOCTL_info INPUT_DATA = (PIOCTL_info)malloc(sizeof(IOCTL_info));
	memset(INPUT_DATA, 0, sizeof(IOCTL_info));// 필수 
	INPUT_DATA->information = dump;
	INPUT_DATA->Ioctl_requestor_PID = (HANDLE)GetCurrentProcessId();
	INPUT_DATA->target_process_info.target_PID = PID; // 커널에서 이 정보로 ATTACH할 것.

	INPUT_DATA->dump_info.To_Ioctl_requestor = NULL;

	PIOCTL_info OUTPUT_DATA = NULL; 
	if (Send_to_Kernel(INPUT_DATA, TRUE, &OUTPUT_DATA, NULL) && OUTPUT_DATA) {


		Warning.Format(_T("성공 얻은 연결리스트 시작주소: %p "), OUTPUT_DATA->dump_info.To_Ioctl_requestor);
		AfxMessageBox(Warning);
		// OUTPUT_DATA->dump_info.To_Ioctl_requestor 이주소에는 이 IOCTL 요청 프로세스 동적HEAP에 덤프가 저장됨.

		query_MEM_DUMP(OUTPUT_DATA->dump_info.To_Ioctl_requestor, SAVE_PATH_ANSI);

		if (!was_empty_path) {
			free((PVOID)SAVE_PATH_ANSI);
		}
	}
	else {
		AfxMessageBox(_T("실패"));
	}


	return;
}


void DUMP_PROCESS::OnEnChangeDumpSavePathEdit()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void DUMP_PROCESS::OnEnChangeDumpProcessIdEdit()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


// 창닫기
void DUMP_PROCESS::OnClose()
{
	EndDialog(IDD_DUMP_PROCESS); // 또는 IDCANCEL
}