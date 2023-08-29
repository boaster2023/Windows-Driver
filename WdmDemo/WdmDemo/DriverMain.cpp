#include "DriverMain.h"
#include "Common.h"

NTSTATUS
DriverEntry(
    __in struct _DRIVER_OBJECT *DriverObject,
    __in PUNICODE_STRING  RegistryPath
    )
{
    UNREFERENCED_PARAMETER(RegistryPath);

    DriverObject->DriverExtension->AddDevice = DriverAddDevice;
    DriverObject->MajorFunction[IRP_MJ_PNP] = DriverPnpDispatch;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = DriverDefaultDispatch;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DriverDefaultDispatch;
    DriverObject->MajorFunction[IRP_MJ_WRITE] = DriverDefaultDispatch;
    DriverObject->MajorFunction[IRP_MJ_READ] = DriverDefaultDispatch;

    DriverObject->DriverUnload = DriverUnload;

    return STATUS_SUCCESS;
}


NTSTATUS
DriverAddDevice(
    _In_ struct _DRIVER_OBJECT *DriverObject,
    _In_ struct _DEVICE_OBJECT *PhysicalDeviceObject
)
{
    PAGED_CODE();
    UNREFERENCED_PARAMETER(PhysicalDeviceObject);

    NTSTATUS status;
    UNICODE_STRING devName;
    UNICODE_STRING symbolicLinkName;
    PDEVICE_OBJECT pDevObj = NULL;
    PDEVICE_OBJECT pNextDevObj = NULL;
    PDEVICE_EXTENSION pDevExt = NULL;

    // 创建设备对象
    RtlInitUnicodeString(&devName, DEVICE_NAME);
    status = IoCreateDevice(
        DriverObject,
        sizeof(DEVICE_EXTENSION),
        NULL,
        FILE_DEVICE_UNKNOWN,
        FILE_DEVICE_SECURE_OPEN,
        TRUE,
        &pDevObj
    );
    if (!NT_SUCCESS(status))
    {
        ERRMSG("IoCreateDevice", status, NULL);
        return status;
    }

    pNextDevObj = IoAttachDeviceToDeviceStack(pDevObj, PhysicalDeviceObject);

    // 创建符号链接
    RtlInitUnicodeString(&symbolicLinkName, SYMBOLICLINK_NAME);
    status = IoCreateSymbolicLink(&symbolicLinkName, &devName);
    if (!NT_SUCCESS(status))
    {
        ERRMSG("IoCreateSymbolicLink", status, NULL);
        IoDetachDevice(pNextDevObj);
        IoDeleteDevice(pDevObj);
        return status;
    }

    pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
    pDevExt->DeviceObject = pDevObj;
    pDevExt->NextDeviceObject = pNextDevObj;
    RtlCopyUnicodeString(&pDevExt->symbolicLinkName, &symbolicLinkName);

    return STATUS_SUCCESS;
}

NTSTATUS
DriverPnpDispatch(
    _In_ struct _DEVICE_OBJECT *DeviceObject,
    _Inout_ struct _IRP *Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);
    return STATUS_SUCCESS;
}

NTSTATUS
DriverDefaultDispatch(
    _In_ struct _DEVICE_OBJECT *DeviceObject,
    _Inout_ struct _IRP *Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);
    return STATUS_SUCCESS;
}

VOID
DriverUnload(
    _In_ struct _DRIVER_OBJECT *DriverObject
    )
{
    UNREFERENCED_PARAMETER(DriverObject);
}