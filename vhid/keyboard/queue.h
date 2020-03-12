#include <ntddk.h>
#include <wdf.h>

NTSTATUS CreateQueue(_In_ WDFDEVICE Device, _Out_ WDFQUEUE* Queue);

NTSTATUS RequestCopyFromBuffer(_In_ WDFREQUEST Request,
    _In_ PVOID SourceBuffer,
    _In_  size_t NumBytesToCopyFrom);

typedef struct _QUEUE_CONTEXT {
    WDFDEVICE device;
    WDFQUEUE queue;
} QUEUE_CONTEXT, * PQUEUE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(QUEUE_CONTEXT, QueueContext);
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL QueueControl;