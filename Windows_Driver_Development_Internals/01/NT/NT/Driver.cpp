#include "Driver.h"

#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGEABLE, CreateDevice)
#pragma alloc_text(PAGEABLE, DefaultDispatchRoutine)
#pragma alloc_text(PAGEABLE, DriverUnload)


NTSTATUS DriverEntry(
    PDRIVER_OBJECT DriverObject, 
    PUNICODE_STRING RegisterPath
    )
{
    UNREFERENCED_PARAMETER(RegisterPath);

    // 1. 设置卸载回调函数和驱动派遣函数
    DriverObject->DriverUnload = DriverUnload;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = DefaultDispatchRoutine;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DefaultDispatchRoutine;
    DriverObject->MajorFunction[IRP_MJ_READ] = DefaultDispatchRoutine;
    DriverObject->MajorFunction[IRP_MJ_WRITE] = DefaultDispatchRoutine;

    // 2. 创建设备对象和符号链接
    return CreateDevice(DriverObject);
}

NTSTATUS CreateDevice(PDRIVER_OBJECT DriverObject)
{
    NTSTATUS Status;
    UNICODE_STRING DeviceName;
    UNICODE_STRING SymbolicLinkName;
    PDEVICE_OBJECT DeviceObject;
    PDEVICE_EXTENSION DeviceExtension;

    RtlInitUnicodeString(&DeviceName, DEVICE_NAME);
    Status = IoCreateDevice(
        DriverObject,
        sizeof(DEVICE_EXTENSION),
        &DeviceName,
        FILE_DEVICE_UNKNOWN,
        0,
        TRUE,
        &DeviceObject
    );
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }

    RtlInitUnicodeString(&SymbolicLinkName, SYMBOLICLINK_NAME);
    Status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);
    if (!NT_SUCCESS(Status))
    {
        IoDeleteDevice(DeviceObject);
        return Status;
    }

    DriverObject->Flags |= DO_BUFFERED_IO;
    DeviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
    DeviceExtension->DeviceObject = DeviceObject;
    DeviceExtension->DeviceName = DeviceName;
    DeviceExtension->SymbolicLinkName = SymbolicLinkName;
    return Status;
}


NTSTATUS DefaultDispatchRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
    PDEVICE_OBJECT DeviceObject;
    PDEVICE_OBJECT NextDeviceObject;
    PDEVICE_EXTENSION DeviceExtension;

    DeviceObject = DriverObject->DeviceObject;
    while (DeviceObject)
    {
        NextDeviceObject = DeviceObject->NextDevice;
        DeviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
        IoDeleteSymbolicLink(&DeviceExtension->SymbolicLinkName);
        IoDeleteDevice(DeviceObject);
        DeviceObject = NextDeviceObject;
    }
}
