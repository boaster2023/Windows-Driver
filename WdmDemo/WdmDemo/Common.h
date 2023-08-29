#pragma once

#ifdef __cplusplus
extern "C" {
#endif 

#include <wdm.h>

#define DEVICE_NAME L"\\Device\\DeviceName"
#define SYMBOLICLINK_NAME L"\\DosDevices\\DosDeviceName"


#define ERRMSG(f, s, m) \
    KdPrint(("[Error] %s:%d, Function=%s, Status=%X, Message=%s", __FILE__, __LINE__, f, s, m))



typedef struct _DEVICE_EXTENSION
{
    PDEVICE_OBJECT DeviceObject;
    PDEVICE_OBJECT NextDeviceObject;

    UNICODE_STRING symbolicLinkName;

}DEVICE_EXTENSION, *PDEVICE_EXTENSION;

#ifdef __cplusplus
}
#endif 
