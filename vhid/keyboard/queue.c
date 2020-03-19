#include "queue.h"
#include "device.h"

NTSTATUS CreateQueue(_In_ WDFDEVICE Device, _Out_ WDFQUEUE* Queue) {
    NTSTATUS status;
    WDF_IO_QUEUE_CONFIG queueConfig;
    WDF_OBJECT_ATTRIBUTES queueAttributes;
    WDFQUEUE queue;
    PQUEUE_CONTEXT queueContext;

	KdPrint(("Creating queue to process keyboard events\n"));

    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchSequential);
    queueConfig.EvtIoInternalDeviceControl = QueueControl;

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&queueAttributes, QUEUE_CONTEXT);

    status = WdfIoQueueCreate(Device,
        &queueConfig,
        &queueAttributes,
        &queue);

    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to create virtual keyboard queue due error 0x%x\n", status));
        return status;
    }

    queueContext = QueueContext(queue);
    queueContext->device = Device;
    queueContext->queue = queue;
    *Queue = queue;

	return status;
}

VOID QueueControl(_In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t OutputBufferLength,
    _In_ size_t InputBufferLength,
    _In_ ULONG IoControlCode
) {
    NTSTATUS status;
    WDFDEVICE device = WdfIoQueueGetDevice(Queue);
    PQUEUE_CONTEXT queueContext = QueueContext(Queue);
    PDEVICE_CONTEXT deviceContext = DeviceContext(device);

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    KdPrint(("Virtual keyboard received device control event\n"));

    switch (IoControlCode) {
    case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
        KdPrint(("Get device descriptor event\n"));

        status = RequestCopyFromBuffer(Request,
            &deviceContext->HidDescriptor,
            deviceContext->HidDescriptor->bLength);
        break;
    case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
        KdPrint(("Get device attrbites event\n"));

        status = RequestCopyFromBuffer(Request,
            &deviceContext->HidDeviceAttributes,
            sizeof(HID_DEVICE_ATTRIBUTES));
        break;
    case IOCTL_HID_GET_REPORT_DESCRIPTOR:
        KdPrint(("Get report descriptor event\n"));

        status = RequestCopyFromBuffer(Request,
            deviceContext->ReportDescriptor,
            deviceContext->HidDescriptor->DescriptorList[0].wReportLength);
        break;
    default:
        KdPrint(("Received unsupported event of type %d\n", IoControlCode));
        status = STATUS_NOT_IMPLEMENTED;
        break;
    }

    WdfRequestComplete(Request, status);
}

NTSTATUS RequestCopyFromBuffer(_In_ WDFREQUEST Request,
    _In_ PVOID SourceBuffer,
    _In_  size_t NumBytesToCopyFrom) {
    NTSTATUS status;
    WDFMEMORY memory;
    size_t outputBufferLength;

    status = WdfRequestRetrieveOutputMemory(Request, &memory);

    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to retrieve output memory due error 0x%x\n", status));
        return status;
    }

    WdfMemoryGetBuffer(memory, &outputBufferLength);

    if (NumBytesToCopyFrom > outputBufferLength) {
        status = STATUS_INVALID_BUFFER_SIZE;
        KdPrint(("Output buffer too small. Size %d, expect %d\n", (int) outputBufferLength, (int) NumBytesToCopyFrom));
        return status;
    }

    status = WdfMemoryCopyFromBuffer(memory,
        0,
        SourceBuffer,
        NumBytesToCopyFrom);

    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to copy memory buffer due error 0x%x\n", status));
        return status;
    }

    WdfRequestSetInformation(Request, NumBytesToCopyFrom);

    return status;
}