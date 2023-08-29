#pragma once

// NT 驱动的头文件
#include <NTDDK.h>

#define DEVICE_NAME L"\\Device\\MyDevice"
#define SYMBOLICLINK_NAME L"\\DosDevices\\MySymbolicLink"

#ifdef __cplusplus
EXTERN_C_START
#endif

typedef struct _DEVICE_EXTENSION
{
    PDEVICE_OBJECT DeviceObject;
    UNICODE_STRING DeviceName;
    UNICODE_STRING SymbolicLinkName;
}DEVICE_EXTENSION, *PDEVICE_EXTENSION;

NTSTATUS DriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING RegisterPath
);

NTSTATUS CreateDevice(PDRIVER_OBJECT DriverObject);

NTSTATUS DefaultDispatchRoutine(PDEVICE_OBJECT DeviceObject, PIRP Irp);

VOID DriverUnload(PDRIVER_OBJECT DriverObject);

#ifdef __cplusplus
EXTERN_C_END
#endif