#include "HardWare_BreakPoint_Exception.h"


K_EVENT_or_MUTEX_struct HardWare_BP_thread_KMUTEX = { NULL, K_MUTEX, FALSE };

PHardWare_BP_Node HardWare_BP_StartNode = NULL;
PHardWare_BP_Node HardWare_BP_CurrentNode = NULL;


VOID query_HardWare_BP(PHardWare_BP_Node StartNode) {
	PHardWare_BP_Node current = StartNode;
	while (current) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " PID -> %lld \n", current->CONTEXT.PID);
		current = (PHardWare_BP_Node)current->Next_Addr;
	}
}



// 생성
PHardWare_BP_Node Create_HardWare_BP_Node(HardWare_BP_struct_from_targetprocess* Input_Context) {

	PHardWare_BP_Node NewNode = ExAllocatePoolWithTag(NonPagedPool, sizeof(HardWare_BP_Node), 'HWBP');
	if (NewNode == NULL) return NULL;

	NewNode->CONTEXT.PID = Input_Context->PID;
	NewNode->CONTEXT.Register = Input_Context->Register;


	NewNode->Next_Addr = NULL;

	return NewNode;

}

// 추가
PHardWare_BP_Node Append_HardWare_BP_Node(PHardWare_BP_Node Current_Node, HardWare_BP_struct_from_targetprocess* Input_Context) {
	Current_Node->Next_Addr = (PUCHAR)Create_HardWare_BP_Node(Input_Context);

	return (PHardWare_BP_Node)Current_Node->Next_Addr;
}









// 생성 and 추가 한번에
BOOLEAN ADD_HardWare_BP_Node(HardWare_BP_struct_from_targetprocess* Input_Context) {

	K_object_init_check_also_lock_ifyouwant(&HardWare_BP_thread_KMUTEX, TRUE);

	if (HardWare_BP_StartNode == NULL) {
		HardWare_BP_StartNode = Create_HardWare_BP_Node(Input_Context);
		HardWare_BP_CurrentNode = HardWare_BP_StartNode;
	}
	else {
		HardWare_BP_CurrentNode = Append_HardWare_BP_Node(HardWare_BP_CurrentNode, Input_Context);
	}


	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "HardWare_BP_StartNode -> %p  /  HardWare_BP_CurrentNode -> %p \n", HardWare_BP_StartNode, HardWare_BP_CurrentNode);


	query_HardWare_BP(HardWare_BP_StartNode);

	K_object_lock_Release(&HardWare_BP_thread_KMUTEX);


	return TRUE;
}




// 조회 후 가져오기 ( 나온것은 바로 연결노드상에서 없어짐.  ) 
PHardWare_BP_Node Query_HardWare_BP_Node(PHardWare_BP_Node Start_Node, HANDLE PID) {

	K_object_init_check_also_lock_ifyouwant(&HardWare_BP_thread_KMUTEX, TRUE);


	PHardWare_BP_Node current = Start_Node;


	PHardWare_BP_Node remember_node = NULL; /// 이어주기 전용 
	while (current) {
		DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, " PID -> %lld  == ? 파라미터 PID => %lld \n", current->CONTEXT.PID, PID);
		if (current->CONTEXT.PID == PID) {
			PHardWare_BP_Node output_node = current;

			// 다음거 이어주기 ( 이 current는 이제 연결노드에서 끊어짐  ) 
			if (remember_node) {
				remember_node->Next_Addr = current->Next_Addr;

				if (HardWare_BP_CurrentNode == current) {
					HardWare_BP_CurrentNode = remember_node;
				}

			}
			else {
				if (HardWare_BP_CurrentNode == HardWare_BP_StartNode) {
					HardWare_BP_StartNode = (PHardWare_BP_Node)current->Next_Addr;
					HardWare_BP_CurrentNode = HardWare_BP_StartNode;
				}
				else {
					HardWare_BP_StartNode = (PHardWare_BP_Node)current->Next_Addr;
				}
				
				

			}
			

			K_object_lock_Release(&HardWare_BP_thread_KMUTEX);
			return output_node;
		}

		remember_node = current;
		current = (PHardWare_BP_Node)current->Next_Addr;
	}


	K_object_lock_Release(&HardWare_BP_thread_KMUTEX);
	return NULL;
}

// 특정 PID에 따른 모든 노드 삭제. ( MFC 창 닫을 때 트리거 ) 
BOOLEAN Remove_with_Specified_PID_HardWare_BP_Node(PHardWare_BP_Node Start_Node, HANDLE PID) {
	K_object_init_check_also_lock_ifyouwant(&HardWare_BP_thread_KMUTEX, TRUE);


	PHardWare_BP_Node current = Start_Node;

	PHardWare_BP_Node remember_node = Start_Node; /// 이어주기 전용 
	while (current) {

		if (current->CONTEXT.PID == PID) {



			// 다음거 이어주기 ( 이 current는 이제 연결노드에서 끊어짐  ) 
			remember_node = (PHardWare_BP_Node)current->Next_Addr;

			ExFreePoolWithTag(current, 'HWBP');


			if (remember_node == NULL) break;

			current = remember_node;
			continue;


		}

		remember_node = current;
		current = (PHardWare_BP_Node)current->Next_Addr;
	}


	K_object_lock_Release(&HardWare_BP_thread_KMUTEX);
	return TRUE;
}