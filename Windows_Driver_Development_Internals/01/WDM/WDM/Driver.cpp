#include "Driver.h"


NTSTATUS
DriverEntry(
    __in struct _DRIVER_OBJECT* DriverObject,
    __in PUNICODE_STRING  RegistryPath
)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    KdPrint(("[Debug] DriverEntry"));

    DriverObject->DriverExtension->AddDevice = AddDriver;
    DriverObject->MajorFunction[IRP_MJ_PNP] = PnpDispatch;
    DriverObject->DriverUnload = Unload;
   
    return STATUS_SUCCESS;
}

NTSTATUS 
AddDriver(
    _In_ struct _DRIVER_OBJECT* DriverObject,
    _In_ struct _DEVICE_OBJECT* PhysicalDeviceObject
)
{
    NTSTATUS Status;
    UNICODE_STRING DeviceName;
    UNICODE_STRING SymbolicLinkName;
    PDEVICE_OBJECT FunctionalDeviceObject;
    PDEVICE_EXTENSION DeviceExtension;
    PDEVICE_OBJECT NextStackDeviceObject;

    PAGED_CODE();

    KdPrint(("[Debug] AddDriver"));

    RtlInitUnicodeString(&DeviceName, DEVICE_NAME);
    Status = IoCreateDevice(
        DriverObject,
        sizeof(DEVICE_EXTENSION),
        &DeviceName,
        FILE_DEVICE_UNKNOWN,
        0,
        TRUE,
        &FunctionalDeviceObject
    );
    if (!NT_SUCCESS(Status))
    {
        return Status;
    }
    NextStackDeviceObject = IoAttachDeviceToDeviceStack(
        FunctionalDeviceObject,
        PhysicalDeviceObject
    );

    RtlInitUnicodeString(&SymbolicLinkName, SYMBOLICLINK_NAME);
    Status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);
    if (!NT_SUCCESS(Status))
    {
        IoDeleteSymbolicLink(&SymbolicLinkName);
        Status = IoCreateSymbolicLink(&SymbolicLinkName, &DeviceName);
        if (!NT_SUCCESS(Status))
        {
            IoDeleteDevice(FunctionalDeviceObject);
            return Status;
        }
    }

    DriverObject->Flags |= DO_BUFFERED_IO | DO_POWER_PAGABLE;
    DriverObject->Flags &= ~DO_DEVICE_INITIALIZING;
    DeviceExtension = (PDEVICE_EXTENSION)DriverObject->DriverExtension;
    DeviceExtension->FunctionalDeviceObject = FunctionalDeviceObject;
    DeviceExtension->NextStackDeviceObject = NextStackDeviceObject;
    DeviceExtension->FunctionalDeviceObject = FunctionalDeviceObject;
    DeviceExtension->SymbolicLinkName = SymbolicLinkName;

    return STATUS_SUCCESS;
}

NTSTATUS
PnpDispatch(
    _In_ struct _DEVICE_OBJECT* DeviceObject,
    _Inout_ struct _IRP* Irp
)
{
    NTSTATUS Status;
    PIO_STACK_LOCATION IrpStack;
    UCHAR MajorFunction;

    static PDRIVER_DISPATCH PnpDispatchFunction[] =
    {
        PnpDefaultDispatchFunction,  //IRP_MN_START_DEVICE                 
        PnpDefaultDispatchFunction,  //IRP_MN_QUERY_REMOVE_DEVICE          
        PnpRemoveDevice,    //IRP_MN_REMOVE_DEVICE                
        PnpDefaultDispatchFunction,  //IRP_MN_CANCEL_REMOVE_DEVICE         
        PnpDefaultDispatchFunction,  //IRP_MN_STOP_DEVICE                  
        PnpDefaultDispatchFunction,  //IRP_MN_QUERY_STOP_DEVICE            
        PnpDefaultDispatchFunction,  //IRP_MN_CANCEL_STOP_DEVICE           
        PnpDefaultDispatchFunction,  //IRP_MN_QUERY_DEVICE_RELATIONS       
        PnpDefaultDispatchFunction,  //IRP_MN_QUERY_INTERFACE              
        PnpDefaultDispatchFunction,  //IRP_MN_QUERY_CAPABILITIES           
        PnpDefaultDispatchFunction,  //IRP_MN_QUERY_RESOURCES              
        PnpDefaultDispatchFunction,  //IRP_MN_QUERY_RESOURCE_REQUIREMENTS  
        PnpDefaultDispatchFunction,  //IRP_MN_QUERY_DEVICE_TEXT            
        PnpDefaultDispatchFunction,  //IRP_MN_FILTER_RESOURCE_REQUIREMENTS 
        PnpDefaultDispatchFunction,  //
        PnpDefaultDispatchFunction,  //IRP_MN_READ_CONFIG                  
        PnpDefaultDispatchFunction,  //IRP_MN_WRITE_CONFIG                 
        PnpDefaultDispatchFunction,  //IRP_MN_EJECT                        
        PnpDefaultDispatchFunction,  //IRP_MN_SET_LOCK                     
        PnpDefaultDispatchFunction,  //IRP_MN_QUERY_ID                     
        PnpDefaultDispatchFunction,  //IRP_MN_QUERY_PNP_DEVICE_STATE       
        PnpDefaultDispatchFunction,  //IRP_MN_QUERY_BUS_INFORMATION        
        PnpDefaultDispatchFunction,  //IRP_MN_DEVICE_USAGE_NOTIFICATION    
        PnpDefaultDispatchFunction,  //IRP_MN_SURPRISE_REMOVAL             
    };

    IrpStack = IoGetCurrentIrpStackLocation(Irp);
    MajorFunction = IrpStack->MajorFunction;

#ifdef DBG
    PCCHAR PnpFunctionTypeName[] =
    {
        "IRP_MN_START_DEVICE",
        "IRP_MN_QUERY_REMOVE_DEVICE",
        "IRP_MN_REMOVE_DEVICE",
        "IRP_MN_CANCEL_REMOVE_DEVICE",
        "IRP_MN_STOP_DEVICE",
        "IRP_MN_QUERY_STOP_DEVICE",
        "IRP_MN_CANCEL_STOP_DEVICE",
        "IRP_MN_QUERY_DEVICE_RELATIONS",
        "IRP_MN_QUERY_INTERFACE",
        "IRP_MN_QUERY_CAPABILITIES",
        "IRP_MN_QUERY_RESOURCES",
        "IRP_MN_QUERY_RESOURCE_REQUIREMENTS",
        "IRP_MN_QUERY_DEVICE_TEXT",
        "IRP_MN_FILTER_RESOURCE_REQUIREMENTS",
        "",
        "IRP_MN_READ_CONFIG",
        "IRP_MN_WRITE_CONFIG",
        "IRP_MN_EJECT",
        "IRP_MN_SET_LOCK",
        "IRP_MN_QUERY_ID",
        "IRP_MN_QUERY_PNP_DEVICE_STATE",
        "IRP_MN_QUERY_BUS_INFORMATION",
        "IRP_MN_DEVICE_USAGE_NOTIFICATION",
        "IRP_MN_SURPRISE_REMOVAL"
    };
    KdPrint(("[Debug] Pnp Dispatch Type=%X, %s", MajorFunction, PnpFunctionTypeName[MajorFunction]));
#endif

    if (ARRAY_SIZE(PnpDispatchFunction) <= MajorFunction)
    {
        Status = PnpDefaultDispatchFunction(DeviceObject, Irp);
    }
    else
    {
        Status = PnpDispatchFunction[MajorFunction](DeviceObject, Irp);
    }

    return Status;
}

NTSTATUS
PnpDefaultDispatchFunction(
    _In_ struct _DEVICE_OBJECT* DeviceObject,
    _Inout_ struct _IRP* Irp
)
{
    PDEVICE_EXTENSION DeviceExtension;

    PAGED_CODE();

    DeviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
    IoSkipCurrentIrpStackLocation(Irp);
    return IoCallDriver(DeviceExtension->NextStackDeviceObject, Irp);
}

NTSTATUS
PnpRemoveDevice(
    _In_ struct _DEVICE_OBJECT* DeviceObject,
    _Inout_ struct _IRP* Irp
)
{
    PDEVICE_EXTENSION DeviceExtension;

    UNREFERENCED_PARAMETER(Irp);

    PAGED_CODE();

    DeviceExtension = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
    IoDeleteSymbolicLink(&DeviceExtension->SymbolicLinkName);
    if (DeviceExtension->NextStackDeviceObject)
    {
        IoDetachDevice(DeviceExtension->NextStackDeviceObject);
    }
    IoDeleteDevice(DeviceExtension->FunctionalDeviceObject);
    return STATUS_SUCCESS;
}

VOID
Unload(
    _In_ struct _DRIVER_OBJECT* DriverObject
)
{
    UNREFERENCED_PARAMETER(DriverObject);
}
