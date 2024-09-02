// KERNELBASED_DLL_INJECTION__.cpp: 구현 파일
//

#include "pch.h"
#include "MFC_GAME_HACK.h"
#include "afxdialogex.h"
#include "KERNELBASED_DLL_INJECTION__.h"
#include "Windows.h"

// KERNELBASED_DLL_INJECTION__ 대화 상자

IMPLEMENT_DYNAMIC(KERNELBASED_DLL_INJECTION__, CDialogEx)

KERNELBASED_DLL_INJECTION__::KERNELBASED_DLL_INJECTION__(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_KERNEL_DLL_INJECTION, pParent)
{

}

KERNELBASED_DLL_INJECTION__::~KERNELBASED_DLL_INJECTION__()
{
}

void KERNELBASED_DLL_INJECTION__::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DLL_ABS_PATH_EDIT, DLL_PATH_EDIT);
	DDX_Control(pDX, IDC_PID_EDIT, PID_EDIT);
	DDX_Control(pDX, IDC_CHOOSE_LOADLIBRARY_COMBO, LoadLibrary_COMBO);
}


BEGIN_MESSAGE_MAP(KERNELBASED_DLL_INJECTION__, CDialogEx)
	ON_BN_CLICKED(IDC_FILE_CHOOSE_BUTTT, &KERNELBASED_DLL_INJECTION__::OnBnClickedFileChooseButtt)
	ON_BN_CLICKED(IDC_INJECT_BUU, &KERNELBASED_DLL_INJECTION__::OnBnClickedInjectBuu)
END_MESSAGE_MAP()


// KERNELBASED_DLL_INJECTION__ 메시지 처리기

// 초기
BOOL KERNELBASED_DLL_INJECTION__::OnInitDialog() {
	CDialogEx::OnInitDialog();

	LoadLibrary_COMBO.ResetContent();

	CString API_str;
	API_str.Format(_T("LoadLibraryA (%p)"), LoadLibraryA);
	LoadLibrary_COMBO.AddString(API_str);

	API_str.Format(_T("LoadLibraryW (%p)"), LoadLibraryW);
	LoadLibrary_COMBO.AddString(API_str);

	LoadLibrary_COMBO.SetCurSel(0); //첫 아이템 띄우게함 

	return 1;
}

// 파일선택 버튼
void KERNELBASED_DLL_INJECTION__::OnBnClickedFileChooseButtt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	DLL_PATH_EDIT;

	CString str = _T("DLL files(*.dll)|*.dll*|All files(*.*)|*.*|"); // 모든 파일 표시
	// 확장자 제한
	CFileDialog dlg(TRUE, _T("*.dat"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, str, this);

	if (dlg.DoModal() == IDOK) {
		CString PATH;
		PATH.Format(_T("%s"), dlg.GetPathName() );
		DLL_PATH_EDIT.SetWindowTextW(PATH);
	}
	
}


/*
	Fire 버튼
	3가지 정보를 조합하여 처리.

*/
void KERNELBASED_DLL_INJECTION__::OnBnClickedInjectBuu()
{

	CString tmp_Str;

	// PID가져오기
	HANDLE PID = 0;
	PID_EDIT.GetWindowTextW(tmp_Str);
	if (tmp_Str.IsEmpty()) {
		AfxMessageBox(_T("please input target PID "));
		return;
	}
	CStringA tmp_PID(tmp_Str);
	PID = (HANDLE)strtoul(tmp_PID, NULL, 10);


	// DLL 문자열가져오기
	DLL_PATH_EDIT.GetWindowTextW(tmp_Str);
	if (tmp_Str.IsEmpty()) {
		AfxMessageBox(_T("please input dll path "));
		return;
	}

	PUCHAR DLL_PATH = NULL;
	ULONG32 DLL_PATH_LENGTH = 0;


	// LoadLibrary 종류 가져오기
	PUCHAR API = NULL;
	int selected_Index = LoadLibrary_COMBO.GetCurSel();
	CString selectedString_Value;
	LoadLibrary_COMBO.GetLBText(selected_Index, selectedString_Value);

	CString LoadLibraryA_STR;
	LoadLibraryA_STR.Format(_T("LoadLibraryA (%p)"), LoadLibraryA);
	CString LoadLibraryW_STR;
	LoadLibraryW_STR.Format(_T("LoadLibraryW (%p)"), LoadLibraryW);

	if (selectedString_Value == LoadLibraryA_STR) {
		API = (PUCHAR)LoadLibraryA;
		/*
			ANSI - CHAR
			형태로 DLL를인젝션한다는 말. 
			동적할당하여 커널에게 전달해야한다.
		*/
		DLL_PATH = (PUCHAR)Convert___Wide__to__Ansi____( tmp_Str.GetString() ) ; // 이미 할당된.
		DLL_PATH_LENGTH = strlen((PCHAR)DLL_PATH) + 1; // 널포함
		
	}
	else if (selectedString_Value == LoadLibraryW_STR) {
		API = (PUCHAR)LoadLibraryW;
		PWCH PATH = (PWCH)tmp_Str.GetString();

		DLL_PATH_LENGTH = (wcslen(PATH) + sizeof(WCHAR) )+2 ;
		DLL_PATH = (PUCHAR)malloc(DLL_PATH_LENGTH);
		memcpy(DLL_PATH, PATH, DLL_PATH_LENGTH);
	}
	else {
		AfxMessageBox(_T("hmm i cant understand what you choose one that API "));
		return;
	}


	PUCHAR Allocated_DLL_PATH;

	// 커널요청
	PIOCTL_info INPUT_DATA = (PIOCTL_info)malloc(sizeof(IOCTL_info));
	memset(INPUT_DATA, 0, sizeof(IOCTL_info));// 필수 
	INPUT_DATA->information = Kernel_Based_DLL_Injection;///////////////////////////////////////////////////////////////////////////
	INPUT_DATA->Ioctl_requestor_PID = (HANDLE)GetCurrentProcessId();

	INPUT_DATA->target_process_info.target_PID = PID;

	INPUT_DATA->DLL_info.LoadLibrary_Address = API;
	INPUT_DATA->DLL_info.DLL_PATH__from__ioctl_virtualmem = DLL_PATH;
	INPUT_DATA->DLL_info.DLL_PATH_length = DLL_PATH_LENGTH;

	INPUT_DATA->DLL_info.TargetProcess_run_threadID = 0; //output



	PIOCTL_info OUTPUT_DATA = NULL;
	if (Send_to_Kernel(INPUT_DATA, TRUE, &OUTPUT_DATA, NULL) && OUTPUT_DATA && OUTPUT_DATA->DLL_info.TargetProcess_run_threadID != 0) {
		AfxMessageBox(_T(" DLL injection SUCCESS "));
	}
	else {
		AfxMessageBox(_T(" DLL injection FAILED "));
	}
}
