#include <ntifs.h>

#include "DriverEntry.h" // NEW 

NTSTATUS
NTAPI
IoCreateDriver(
    _In_ PUNICODE_STRING DriverName,
    _In_ PDRIVER_INITIALIZE InitializationFunction
);

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT   DriverObjects,
    _In_ PUNICODE_STRING  RegistryPath
)
{

    UNREFERENCED_PARAMETER(DriverObjects);
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS status = STATUS_SUCCESS;

    UNICODE_STRING driverName = { 0, };

    // 드라이버 이름 초기화
    RtlInitUnicodeString(&driverName, L"\\Driver\\My_TEST_Driver");

    // IoCreateDriver를 사용하여 드라이버 오브젝트 생성
    status = IoCreateDriver(&driverName, &MyDriverEntry);

    if (!NT_SUCCESS(status))
    {
        KdPrint(("Failed to create driver object. Status: 0x%X\n", status));
        return status;
    }

    return status;

}
