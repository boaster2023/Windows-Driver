#pragma once

#ifdef __cplusplus
extern "C" {
#endif 

#include <wdm.h>

NTSTATUS
DriverEntry(
    __in struct _DRIVER_OBJECT *DriverObject,
    __in PUNICODE_STRING  RegistryPath
);

NTSTATUS
DriverAddDevice(
    _In_ struct _DRIVER_OBJECT *DriverObject,
    _In_ struct _DEVICE_OBJECT *PhysicalDeviceObject
);

NTSTATUS
DriverPnpDispatch(
    _In_ struct _DEVICE_OBJECT *DeviceObject,
    _Inout_ struct _IRP *Irp
);

NTSTATUS
DriverDefaultDispatch(
    _In_ struct _DEVICE_OBJECT *DeviceObject,
    _Inout_ struct _IRP *Irp
);

VOID
DriverUnload(
    _In_ struct _DRIVER_OBJECT *DriverObject
);

#ifdef __cplusplus
}
#endif 

#pragma alloc_text(INIT, DriverEntry)
#pragma alloc_text(PAGE, DriverAddDevice, \
                         DriverPnpDispatch, \
                         DriverDefaultDispatch, \
                         DriverUnload, \
                         )