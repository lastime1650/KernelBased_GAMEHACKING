#ifndef HW_BP_
#define HW_BP_

#include <ntifs.h>
#include "KEVENT_or_KMUTEX_init.h"

// �ϵ���� �ߴ��� ���� ����ü
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
    ���Ḯ��Ʈ(���� PID�� ���� �� ������, ������ ������ �ʿ��� 

    �� ���Ḯ��Ʈ ��� ť ������ �Ѵ�.

    ���� MFC�ʿ��� ������ â�� ������, �ش� ���Ḯ��Ʈ�� ��� ���ŵǾ���Ѵ�. 
    
*/

// ����
PHardWare_BP_Node Create_HardWare_BP_Node(HardWare_BP_struct_from_targetprocess* Input_Context);

// �߰�
PHardWare_BP_Node Append_HardWare_BP_Node(PHardWare_BP_Node Current_Node, HardWare_BP_struct_from_targetprocess* Input_Context);

// ���� and �߰� ���ð��� with MUTEX
BOOLEAN ADD_HardWare_BP_Node(HardWare_BP_struct_from_targetprocess* Input_Context);

// ��ȸ �� �������� ( ���°��� �ٷ� ��������. ) 
PHardWare_BP_Node Query_HardWare_BP_Node(PHardWare_BP_Node Start_Node, HANDLE PID);

// Ư�� PID�� ���� ��� ��� ����. ( MFC â ���� �� Ʈ���� ) 
BOOLEAN Remove_with_Specified_PID_HardWare_BP_Node(PHardWare_BP_Node Start_Node, HANDLE PID);


#endif // !HW_BP_
