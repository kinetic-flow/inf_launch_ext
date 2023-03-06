#include "infzoom.h"

const wchar_t CLASS_NAME[]  = L"infzoom hotkey capture window";
HWND hwnd;

// pre-allocated for perf
PRAWINPUT RawInputData;
SIZE_T RawInputDataSize;

VOID
ProcessWmInput (
    _In_ PRAWINPUT Data
    )
{
    PRAWKEYBOARD Kbd;

    if (Data->header.dwType != RIM_TYPEKEYBOARD) {
        return;
    }

    Kbd = &Data->data.keyboard;
    MessageLoop(Kbd);
}

LRESULT
CALLBACK
WndProc(
    HWND hWnd,
    UINT msg,
    WPARAM wparam,
    LPARAM lParam
    )
{
    switch (msg) {
        case WM_CREATE: {
            LPCREATESTRUCT create_params = (LPCREATESTRUCT)lParam;
            SetWindowLongPtr(
                hWnd,
                GWLP_USERDATA,
                (LONG_PTR)(create_params->lpCreateParams));

            return 0;
        }
        case WM_INPUT: {
            UINT Size = RawInputDataSize;
            UINT BytesReturned;
            BytesReturned = GetRawInputData(
                (HRAWINPUT)lParam,
                RID_INPUT,
                RawInputData,
                &Size,
                sizeof(RAWINPUTHEADER));
            if (BytesReturned == -1) {
                printf("ERROR: GetRawInputData failed, GLE = 0x%x\n", GetLastError());
                break;
            }

            ProcessWmInput(RawInputData);
            return 0;
        }
        default:
            break;
    }

    return DefWindowProc(hWnd, msg, wparam, lParam);
}

DWORD
WINAPI
WindowThread(
    LPVOID lpParam
    ) 
{
    MSG msg;
    PWNDCLASSEX wc;

    wc = (PWNDCLASSEX)lpParam;

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    // Create the window.
    hwnd = CreateWindowEx(
        WS_EX_NOACTIVATE,
        CLASS_NAME,
        L"infzoom window",
        0,
        0,
        0,
        0,
        0,
        NULL,
        NULL,
        wc->hInstance,
        NULL
        );

    if (hwnd == NULL) {
        printf("ERROR: CreateWindowEx failed, GLE = 0x%x\n", GetLastError());
        return 0;
    }

    RawInputDataSize = sizeof(*RawInputData) + 0x10000;
    RawInputData = malloc(RawInputDataSize);
    RegisterRawInput();

    while (0 < GetMessage(&msg, hwnd, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DestroyWindow(hwnd);
    hwnd = NULL;
    return 0;
}

VOID
CreateNewWindow(
    VOID
    )
{
    HANDLE Thread;
    WNDCLASSEX wc = {0};

    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;
    if (!RegisterClassEx(&wc)) {
        FailFast(GetLastError());
    }

    Thread = CreateThread(NULL, 0, WindowThread, &wc, 0, NULL);
    if (Thread == NULL) {
        FailFast(GetLastError());
    }
    WaitForSingleObject(Thread, INFINITE);

    UnregisterClass(CLASS_NAME, wc.hInstance);
}

VOID
RegisterRawInput(
    VOID
    )
{
    RAWINPUTDEVICE device = {0};
    device.hwndTarget = hwnd;

    // keyboard
    device.dwFlags = RIDEV_INPUTSINK;
    device.usUsagePage = 1;
    device.usUsage = 0x06;
    if (!RegisterRawInputDevices(&device, 1, sizeof(device))) {
        printf("RegisterRawInputDevices failed, GLE = 0x%x\n", GetLastError());
    }
}