// ATTACH_PROCESS.cpp: 구현 파일
//

#include "pch.h"
#include "MFC_GAME_HACK.h"
#include "afxdialogex.h"
#include "ATTACH_PROCESS.h"

#include "Query_Process.h"// 프로세스 유저모드기반 스캔

// ATTACH_PROCESS 대화 상자

IMPLEMENT_DYNAMIC(ATTACH_PROCESS, CDialogEx)

ATTACH_PROCESS::ATTACH_PROCESS(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ATTACH_PROCESS, pParent)
{

}

ATTACH_PROCESS::~ATTACH_PROCESS()
{

}

void ATTACH_PROCESS::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, LIST_CTRL_ATTACH_PROCESS_ID, LIST_CTRL_Attach_PROCESS_inst);
}


BEGIN_MESSAGE_MAP(ATTACH_PROCESS, CDialogEx)
	ON_BN_CLICKED(IDOK, &ATTACH_PROCESS::OnBnClickedOk)


	ON_NOTIFY(NM_DBLCLK, LIST_CTRL_ATTACH_PROCESS_ID, &ATTACH_PROCESS::OnNMDblclkListCtrl)

END_MESSAGE_MAP()

/*
	초기실행
*/
BOOL ATTACH_PROCESS::OnInitDialog() {
	CDialogEx::OnInitDialog();

	/*
		리스트 컨트롤 초기화 ( 칼럼생성 ) 

		1. PID
		2. ImageName
	*/
	ReFresh_Processes();
	

	return TRUE;
}


// ATTACH_PROCESS 메시지 처리기







void ATTACH_PROCESS::OnNMDblclkListCtrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	int nItem = pNMItemActivate->iItem;
	int nSubItem = pNMItemActivate->iSubItem;

	HANDLE PID = 0;
	CString ImageName;

	if (nItem != -1 && nSubItem != -1)
	{
		CString strText = LIST_CTRL_Attach_PROCESS_inst.GetItemText(nItem, 0);// PID

		// 문자열을 숫자로 변환
		DWORD dwPID = _ttoi(strText); // CString을 DWORD로 변환 (PID는 일반적으로 DWORD 타입)
		PID = (HANDLE)dwPID;
		
		//////

		ImageName = LIST_CTRL_Attach_PROCESS_inst.GetItemText(nItem, 1);// ImageName


		/////
		// 
		// 커널에게 Attach  요청
		/*
			커널에게 질의 ( 동적할당 해서 보내야함 (확인됨) )
		*/
		PIOCTL_info INPUT_DATA = (PIOCTL_info)malloc(sizeof(IOCTL_info) ) ;
		memset(INPUT_DATA, 0, sizeof(IOCTL_info));
		INPUT_DATA->information = ATTACH;
		INPUT_DATA->Ioctl_requestor_PID = (HANDLE)GetCurrentProcessId();
		INPUT_DATA->target_process_info.target_PID = PID; // 커널에서 이 정보로 ATTACH할 것.

		PIOCTL_info OUTPUT_DATA = NULL; // 커널에서 동적할당한 주소가 들어가며, 결과를 볼  수 있음 

		strText.Format(_T("IOCTL 요청중.. -> myPID: %lld / target: %lld "), INPUT_DATA->Ioctl_requestor_PID, PID);
		AfxMessageBox(strText);
		if (Send_to_Kernel(INPUT_DATA, TRUE, &OUTPUT_DATA, NULL)) {
			AfxMessageBox(_T("IOCTL성공"));

			WaitForSingleObject(Parameter.Input_Data->IOCTL_User_Mutex, INFINITE);

			Parameter.Input_Data->Ioctl_requestor_PID = INPUT_DATA->Ioctl_requestor_PID;
			Parameter.Input_Data->target_process_info = INPUT_DATA->target_process_info;

			ReleaseMutex(Parameter.Input_Data->IOCTL_User_Mutex);


			*Parameter.is_Attach_from_Main = TRUE;
		}
		else {
			// 실패처리
			ATTACH_FAILED();
			AfxMessageBox(_T("IOCTL 실패"));
			*Parameter.is_Attach_from_Main = FALSE;
		}

		free(INPUT_DATA);

	}

	
	
	

	if (*Parameter.is_Attach_from_Main) {
		
		ATTACH_SUCCESS(PID, ImageName);
	}
	else {
		ATTACH_FAILED();
	}
	

}

// Attach_SUCCESS
void ATTACH_PROCESS::ATTACH_SUCCESS(HANDLE PID, CString ImageName) {

	*Parameter.is_Attach_from_Main = TRUE;
	*Parameter.PID = PID;

	CString result;

	// PID 활성
	result.Format(_T("%lld"), PID);

	On_Off_from_ID(Parameter.PID_instance, TRUE, result);

	// ImageName 활성 
	On_Off_from_ID(Parameter.ImageName_instance, TRUE, ImageName);

	// STATUS 활성
	result.Format(_T("ATTACH SUCCESS!"));
	On_Off_from_ID(Parameter.Attach_STATUS_instance, TRUE, result);


	// MAIN 윈도우 하단 ADdress 등록


	//체크박스
	result.Format(_T("is_Offset?"));
	On_Off_from_ID(Parameter.Check_Box_instance, FALSE, result); // 활성

	//버튼
	result.Format(_T("Submit"));
	On_Off_from_ID(Parameter.Address_Button_instance, FALSE, result); // 활성

}

// Attach_FAILED
void ATTACH_PROCESS::ATTACH_FAILED() {
	*Parameter.is_Attach_from_Main = FALSE;
	*Parameter.PID = 0;



	// PID 활성

	On_Off_from_ID(Parameter.PID_instance, TRUE, _T("please Attach first") );

	// ImageName 활성 
	On_Off_from_ID(Parameter.ImageName_instance, TRUE, _T("please Attach first"));

	// STATUS 활성
	On_Off_from_ID(Parameter.Attach_STATUS_instance, TRUE, _T("please Attach first"));


	// MAIN 윈도우 하단 ADdress 등록


	//체크박스
	On_Off_from_ID(Parameter.Check_Box_instance, TRUE, _T("please Attach first")); // 활성

	//버튼
	On_Off_from_ID(Parameter.Address_Button_instance, TRUE, _T("please Attach first")); // 활성

}

// 버튼 클릭 핸들러 구현
void ATTACH_PROCESS::On_Off_from_ID(CWnd* Insert_Instance, BOOLEAN is_Disable, CString Caption)
{

	if (is_Disable)
	{



		Insert_Instance->EnableWindow(FALSE); // 비활성화
	}
	else {
		Insert_Instance->EnableWindow(TRUE); //  활성화
	}

	// Caption 처리
	Insert_Instance->SetWindowTextW(Caption);
	
	
	
}



// 프로세스 쿼리 후 칼럼 만들기
VOID ATTACH_PROCESS::ReFresh_Processes() {


	LIST_CTRL_Attach_PROCESS_inst.DeleteAllItems();
	

	// 모든 열 제거
	int nColumnCount = LIST_CTRL_Attach_PROCESS_inst.GetHeaderCtrl()->GetItemCount();
	for (int i = nColumnCount - 1; i >= 0; i--) {
		LIST_CTRL_Attach_PROCESS_inst.DeleteColumn(i);
	}


	LIST_CTRL_Attach_PROCESS_inst.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);


	//CString cstrstring(LIST_columns[i]);
	LIST_CTRL_Attach_PROCESS_inst.InsertColumn(0, _T("PID"), LVCFMT_LEFT, 100);
	LIST_CTRL_Attach_PROCESS_inst.InsertColumn(1, _T("ImageName"), LVCFMT_LEFT, 100);



	/* 프로세스_조회 */
	PSYSTEM_PROCESS_INFORMATION pProcInfo = Query_Processes();
	if (pProcInfo) {
		AfxMessageBox(_T("pProcInfo 존재함"));
	}
	
	
	if (pProcInfo) {

		PSYSTEM_PROCESS_INFORMATION remember_for_free = pProcInfo;

		while (TRUE) {

			CString string;

			// PID
			string.Format(_T("%ld"), pProcInfo->UniqueProcessId);
			// 아이템 추가
			int nItem = LIST_CTRL_Attach_PROCESS_inst.InsertItem(0, string); // 첫 번째 열

			// ImageName
			string.Format(_T("%wZ"), pProcInfo->ImageName);
			LIST_CTRL_Attach_PROCESS_inst.SetItemText(nItem, 1, string); // 두 번째 열



			// 다음 프로세스로 이동
			if (pProcInfo->NextEntryOffset == 0)
				break;

			pProcInfo = (PSYSTEM_PROCESS_INFORMATION)( (PBYTE)pProcInfo + pProcInfo->NextEntryOffset);
		}

		free(remember_for_free);
	}
	


}

void ATTACH_PROCESS::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	ReFresh_Processes();

}

