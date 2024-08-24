#include "DriverEntry.h"

#include "API_LOADER.h"

#include "IOCTL.h"

NTSTATUS MyDriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
) {

    Bring_API(); // API 불러오기 

    UNREFERENCED_PARAMETER(RegistryPath);
    NTSTATUS status;
    PDEVICE_OBJECT deviceObject = NULL;
    UNICODE_STRING deviceName, symLinkName;

    // 디바이스 이름 초기화
    RtlInitUnicodeString(&deviceName, DEVICE_NAME);

    // 디바이스 객체 생성
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

    // 심볼릭 링크 이름 초기화
    RtlInitUnicodeString(&symLinkName, SYMLINK_NAME);

    // 심볼릭 링크 생성
    status = IoCreateSymbolicLink(&symLinkName, &deviceName);

    if (!NT_SUCCESS(status))
    {
        KdPrint(("Failed to create symbolic link. Status: 0x%X\n", status));
        IoDeleteDevice(deviceObject);
        return status;
    }

    // IOCTL 디스패치 루틴 설정
    DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = CreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MyDeviceControl;

    // 언로드 루틴 설정
    DriverObject->DriverUnload = MyDriverUnload;


    deviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

    KdPrint(("Driver initialized successfully\n"));
    return STATUS_SUCCESS;
}


// 디바이스 접근시 처리하는 함수 ( 사용하지 않지만, IOCTL 시 기본으로 필요 ) 
NTSTATUS CreateClose(PDEVICE_OBJECT pDeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(pDeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

// 드라이버 언로드 루틴
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