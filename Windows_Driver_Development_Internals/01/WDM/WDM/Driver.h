#pragma once

#include <wdm.h>

#ifdef __cplusplus
EXTERN_C_START
#endif 

#define DEVICE_NAME L""
#define SYMBOLICLINK_NAME L""

#define ARRAY_SIZE(p) (sizeof(p)/sizeof(*p))

typedef struct _DEVICE_EXTENSION 
{
    PDEVICE_OBJECT FunctionalDeviceObject;
    UNICODE_STRING DeviceName;
    UNICODE_STRING SymbolicLinkName;
    PDEVICE_OBJECT NextStackDeviceObject;
}DEVICE_EXTENSION, *PDEVICE_EXTENSION;

NTSTATUS
DriverEntry(
    __in struct _DRIVER_OBJECT* DriverObject,
    __in PUNICODE_STRING  RegistryPath
);

NTSTATUS
AddDriver(
    _In_ struct _DRIVER_OBJECT* DriverObject,
    _In_ struct _DEVICE_OBJECT* PhysicalDeviceObject
);

NTSTATUS
PnpDispatch(
    _In_ struct _DEVICE_OBJECT* DeviceObject,
    _Inout_ struct _IRP* Irp
);

NTSTATUS
PnpDefaultDispatchFunction(
    _In_ struct _DEVICE_OBJECT* DeviceObject,
    _Inout_ struct _IRP* Irp
);

NTSTATUS
PnpRemoveDevice(
    _In_ struct _DEVICE_OBJECT* DeviceObject,
    _Inout_ struct _IRP* Irp
);

VOID
Unload(
    _In_ struct _DRIVER_OBJECT* DriverObject
);

#ifdef __cplusplus
EXTERN_C_END
#endif 

#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGED, AddDriver)
#pragma alloc_text(PAGED, PnpDispatch)
#pragma alloc_text(PAGED, PnpDefaultDispatchFunction)
#pragma alloc_text(PAGED, PnpRemoveDevice)
#pragma alloc_text(PAGED, Unload)

