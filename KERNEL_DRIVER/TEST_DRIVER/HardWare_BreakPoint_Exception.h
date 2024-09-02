#ifndef HW_BP_
#define HW_BP_

#include <ntifs.h>
#include "KEVENT_or_KMUTEX_init.h"

// 하드웨어 중단점 전용 구조체
typedef struct HardWare_BP_struct_from_targetprocess {

    /*PID*/
    HANDLE PID;

    /* CONTEXT */
    CONTEXT Register;

}HardWare_BP_struct_from_targetprocess, * PHardWare_BP_struct_from_targetprocess;

typedef struct HardWare_BP_Node {

    HardWare_BP_struct_from_targetprocess CONTEXT;

    PUCHAR Next_Addr;
}HardWare_BP_Node, *PHardWare_BP_Node;


extern PHardWare_BP_Node HardWare_BP_StartNode ;
extern PHardWare_BP_Node HardWare_BP_CurrentNode ;

/*
    연결리스트(각종 PID가 있을 수 있으며, 적절한 관리가 필요함 

    이 연결리스트 대기 큐 역할을 한다.

    만약 MFC쪽에서 윈도우 창을 닫으면, 해당 연결리스트는 모두 제거되어야한다. 
    
*/

// 생성
PHardWare_BP_Node Create_HardWare_BP_Node(HardWare_BP_struct_from_targetprocess* Input_Context);

// 추가
PHardWare_BP_Node Append_HardWare_BP_Node(PHardWare_BP_Node Current_Node, HardWare_BP_struct_from_targetprocess* Input_Context);

// 생성 and 추가 동시관리 with MUTEX
BOOLEAN ADD_HardWare_BP_Node(HardWare_BP_struct_from_targetprocess* Input_Context);

// 조회 후 가져오기 ( 나온것은 바로 없어진다. ) 
PHardWare_BP_Node Query_HardWare_BP_Node(PHardWare_BP_Node Start_Node, HANDLE PID);

// 특정 PID에 따른 모든 노드 삭제. ( MFC 창 닫을 때 트리거 ) 
BOOLEAN Remove_with_Specified_PID_HardWare_BP_Node(PHardWare_BP_Node Start_Node, HANDLE PID);


#endif // !HW_BP_
