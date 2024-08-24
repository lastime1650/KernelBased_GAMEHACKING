#ifndef DRIVERENTRY_H
#define DRIVERENTRY_H

#include <ntifs.h>

#define DEVICE_NAME L"\\Device\\My_TEST_Device"
#define SYMLINK_NAME L"\\??\\My_TEST_SYM_Device"

NTSTATUS MyDriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
);


// ����̽� ���ٽ� ó���ϴ� �Լ� ( ������� ������, IOCTL �� �⺻���� �ʿ� ) 
NTSTATUS CreateClose(PDEVICE_OBJECT pDeviceObject, PIRP Irp);

// IOCTL ����ġ ��ƾ
NTSTATUS MyDeviceControl(
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp
);

// ����̹� ��ε� ��ƾ
VOID MyDriverUnload(
    _In_ PDRIVER_OBJECT DriverObject
);

#endif