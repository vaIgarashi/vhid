#define KEYBOARD_REPORT_ID 0x01

typedef struct _KEYBOARD_REPORT {
    BYTE reportId;
    BYTE modifiers;
    BYTE  _reserved;
    BYTE keyCodes[6];
} KEYBOARD_REPORT, * PKEYBOARD_REPORT;
