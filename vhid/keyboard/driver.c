#include "driver.h"
#include "device.h"

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
    NTSTATUS status;
    WDF_DRIVER_CONFIG config;

    KdPrint(("Starting virtual keyboard driver\n"));

    WDF_DRIVER_CONFIG_INIT(&config, CreateDevice);

    status = WdfDriverCreate(DriverObject,
        RegistryPath,
        WDF_NO_OBJECT_ATTRIBUTES,
        &config,
        WDF_NO_HANDLE);

    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to create virtual keyboard driver due error 0x%x\n", status));
    }

    return status;
}