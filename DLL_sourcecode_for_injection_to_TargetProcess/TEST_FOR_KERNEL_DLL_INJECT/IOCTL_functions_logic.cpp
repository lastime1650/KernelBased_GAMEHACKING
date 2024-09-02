#include "pch.h"
#include "IOCTL_functions.h"

HANDLE hDevice = 0;

BOOLEAN Send_to_Kernel(PIOCTL_info INPUT) {
	if (hDevice == 0) {
		// IOCTL 접근시도
		hDevice = CreateFile(TEXT("\\??\\My_TEST_SYM_Device"),
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);

		if (hDevice == INVALID_HANDLE_VALUE) {
			//printf("Failed to obtain handle to driver. Error: %d\n", GetLastError());
			hDevice = 0;
			return FALSE;
		}
	}

	DWORD bytesReturned;

	// IOCTL 요청 보내기
	return (BOOLEAN)DeviceIoControl(hDevice,
		IOCTL_MY_IOCTL_CODE,
		INPUT,
		sizeof(IOCTL_info),
		NULL,
		0,
		&bytesReturned,
		NULL
	);

}