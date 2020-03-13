#include "device.h"
#include "report.h"
#include "queue.h"

HID_REPORT_DESCRIPTOR DefaultReportDescriptor[] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x85, KEYBOARD_REPORT_ID,      //   REPORT_ID (1)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0x95, 0x06,                    //   REPORT_COUNT (6)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xc0,                          // END_COLLECTION
};

HID_DESCRIPTOR DefaultHidDescriptor = {
    0x09,   // length of HID descriptor
    0x21,   // descriptor type == HID  0x21
    0x0100, // hid spec release
    0x00,   // country code == Not Specified
    0x01,   // number of HID class descriptors
    {                                       //DescriptorList[0]
        0x22,                               //report descriptor type 0x22
        sizeof(DefaultReportDescriptor)   //total length of report descriptor
    }
};

NTSTATUS CreateDevice(_In_ WDFDRIVER Driver, _Inout_ PWDFDEVICE_INIT DeviceInit) {
    NTSTATUS status;
    PDEVICE_CONTEXT deviceContext;
    WDF_OBJECT_ATTRIBUTES deviceAttributes;
    WDFDEVICE device;
    PHID_DEVICE_ATTRIBUTES hidAttributes;

    UNREFERENCED_PARAMETER(Driver);

    KdPrint(("Adding virtual keyboard device\n"));

    WdfFdoInitSetFilter(DeviceInit);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(
        &deviceAttributes,
        DEVICE_CONTEXT);

    status = WdfDeviceCreate(&DeviceInit,
        &deviceAttributes,
        &device);

    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to create virtual keyboard device due error 0x%x\n", status));
        return status;
    }

    KdPrint(("Initialzing virtual keyboard context\n"));

    deviceContext = DeviceContext(device);
    deviceContext->Device = device;
    deviceContext->Queue = NULL;
    deviceContext->HidDescriptor = &DefaultHidDescriptor;
    deviceContext->ReportDescriptor = DefaultReportDescriptor;

    KdPrint(("Initializing virtual keyboard hid attributes\n"));

    hidAttributes = &deviceContext->HidDeviceAttributes;
    RtlZeroMemory(hidAttributes, sizeof(HID_DEVICE_ATTRIBUTES));
    hidAttributes->Size = sizeof(HID_DEVICE_ATTRIBUTES);
    hidAttributes->VendorID = KEYBOARD_PRODUCT_ID;
    hidAttributes->ProductID = KEYBOARD_VENDOR_ID;
    hidAttributes->VersionNumber = KEYBOARD_DEVICE_VERSION;

    status = CreateQueue(device, &deviceContext->Queue);

    return status;
}
