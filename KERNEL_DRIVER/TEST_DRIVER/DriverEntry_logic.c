#include "DriverEntry.h"

#include "API_LOADER.h"

#include "IOCTL.h"

NTSTATUS MyDriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
) {

    Bring_API(); // API �ҷ����� 

    UNREFERENCED_PARAMETER(RegistryPath);
    NTSTATUS status;
    PDEVICE_OBJECT deviceObject = NULL;
    UNICODE_STRING deviceName, symLinkName;

    // ����̽� �̸� �ʱ�ȭ
    RtlInitUnicodeString(&deviceName, DEVICE_NAME);

    // ����̽� ��ü ����
    status = IoCreateDevice(
        DriverObject,
        0,
        &deviceName,
        FILE_DEVICE_UNKNOWN,
        FILE_DEVICE_SECURE_OPEN,
        FALSE,
        &deviceObject
    );

    if (!NT_SUCCESS(status))
    {
        KdPrint(("Failed to create device object. Status: 0x%X\n", status));
        return status;
    }

    // �ɺ��� ��ũ �̸� �ʱ�ȭ
    RtlInitUnicodeString(&symLinkName, SYMLINK_NAME);

    // �ɺ��� ��ũ ����
    status = IoCreateSymbolicLink(&symLinkName, &deviceName);

    if (!NT_SUCCESS(status))
    {
        KdPrint(("Failed to create symbolic link. Status: 0x%X\n", status));
        IoDeleteDevice(deviceObject);
        return status;
    }

    // IOCTL ����ġ ��ƾ ����
    DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = CreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MyDeviceControl;

    // ��ε� ��ƾ ����
    DriverObject->DriverUnload = MyDriverUnload;


    deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    KdPrint(("Driver initialized successfully\n"));
    return STATUS_SUCCESS;
}


// ����̽� ���ٽ� ó���ϴ� �Լ� ( ������� ������, IOCTL �� �⺻���� �ʿ� ) 
NTSTATUS CreateClose(PDEVICE_OBJECT pDeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(pDeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

// ����̹� ��ε� ��ƾ
VOID MyDriverUnload(
    _In_ PDRIVER_OBJECT DriverObject
)
{
    UNICODE_STRING symLinkName;

    RtlInitUnicodeString(&symLinkName, SYMLINK_NAME);
    IoDeleteSymbolicLink(&symLinkName);
    IoDeleteDevice(DriverObject->DeviceObject);

    KdPrint(("Driver unloaded\n"));
}