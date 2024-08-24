#ifndef DRIVERENTRY_H
#define DRIVERENTRY_H

#include <ntifs.h>

#define DEVICE_NAME L"\\Device\\My_TEST_Device"
#define SYMLINK_NAME L"\\??\\My_TEST_SYM_Device"

NTSTATUS MyDriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
);


// 디바이스 접근시 처리하는 함수 ( 사용하지 않지만, IOCTL 시 기본으로 필요 ) 
NTSTATUS CreateClose(PDEVICE_OBJECT pDeviceObject, PIRP Irp);

// IOCTL 디스패치 루틴
NTSTATUS MyDeviceControl(
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp
);

// 드라이버 언로드 루틴
VOID MyDriverUnload(
    _In_ PDRIVER_OBJECT DriverObject
);

#endif