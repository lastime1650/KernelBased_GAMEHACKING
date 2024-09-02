#include <stdio.h>
#include <Windows.h>

#include <capstone/capstone.h>

// 예외 처리기 함수
LONG CALLBACK ExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo) {
	if (ExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP) {
		// Single step exception 처리
		printf("Single step exception occurred.\n");
		// 예외를 계속 처리하도록 반환
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	// 다른 예외는 기본 예외 처리기로 전달
	return EXCEPTION_CONTINUE_SEARCH;
}


VOID function1() {
	printf("저는 function1함수입니다! 누가 호출한거죠?~\n");
	return;
}


int main() {
	
	csh handle;
	cs_insn* insn;
	size_t count;

	uint8_t code[] = { 0x48, 0x89, 0xd8, 0xc3 };  // mov rax, rbx; ret

	if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK) {
		printf("Failed to initialize Capstone\n");
		return -1;
	}

	count = cs_disasm(handle, code, sizeof(code), 0x1000, 0, &insn);
	if (count > 0) {
		for (size_t i = 0; i < count; i++) {
			printf("0x%" PRIx64 ": %s %s\n", insn[i].address, insn[i].mnemonic, insn[i].op_str);
		}
		cs_free(insn, count);
	}
	else {
		printf("Failed to disassemble the code\n");
	}

	cs_close(&handle);

	//PVOID handler = AddVectoredExceptionHandler(1, ExceptionHandler);

	ULONG32 data = 1650;

	

	for (ULONG32 i = 0; i < 9999; i++) {
		PUCHAR data1 = malloc(sizeof(ULONG32));
		memcpy(data1, &data, sizeof(ULONG32));
	}
	
	PUCHAR data2 = malloc(sizeof(ULONG32));
	memcpy(data2, &data, sizeof(ULONG32)); 


	FLOAT data3 = 1650.0;
	DOUBLE data4 = 1111.0;
	

	while (1) {
		HANDLE my_pid = (HANDLE)GetCurrentProcessId();

		printf("내주소 %llu \n", my_pid);
		printf(" 정적주소: %p 데이터 %d 길이 %d \n", &data, data, sizeof(ULONG32));
		memcpy(&data, data2, sizeof(ULONG32));
		data += 1;
		

		printf(" 동적주소: %p 데이터 %d 길이 %d \n", data2, (ULONG32)(*(ULONG32*)data2), sizeof(ULONG32));

		printf(" 정적 FLOAT 주소: %p 데이터 %f 길이 %d \n", &data3, data3, sizeof(FLOAT));
		printf(" 정적 DOUBLE 주소: %p 데이터 %lf 길이 %d \n", &data4, data4, sizeof(DOUBLE));

		printf("LoadLibraryA 주소 -> %p \n", LoadLibraryA);
		printf("LoadLibraryW 주소 -> %p \n", LoadLibraryW);
		printf("\n\n function1 주소(dll 인젝션 테스트용) -> %p \n", function1);
		system("pause");
	}

	//RemoveVectoredExceptionHandler(handler);
	return 0;
}