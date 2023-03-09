#include "infzoom.h"

#define INI_IMPLEMENTATION
#include <ini.h>

typedef enum _ZOOM_MODE {
    ZoomModeNone,
    ZoomMode1p,
    ZoomMode2p,
    ZoomModeDp,
    ZoomModeMax
} ZOOM_MODE, *PZOOM_MODE;

typedef union _HOTKEY {
    struct {
        USHORT Mod;
        USHORT Key;
    } k;
    UINT AsUInt;
} HOTKEY, *PHOTKEY;

typedef struct _RELATIVE_RECT {
    SHORT OffsetX;
    SHORT OffsetY;
    USHORT Width;
    USHORT Height;
} RELATIVE_RECT, *PRELATIVE_RECT;

typedef struct _CONFIG {
    ULONG monitor;
    HOTKEY HotKeyExit;
    HOTKEY HotKeyNormal;
    HOTKEY HotKey1P;
    HOTKEY HotKey2P;
    HOTKEY HotKeyDP;
    HOTKEY HotKeyUp;
    HOTKEY HotKeyDown;
    HOTKEY HotKeyLeft;
    HOTKEY HotKeyRight;
    HOTKEY HotKeyLong;
    HOTKEY HotKeyShort;
    HOTKEY HotKeyNarrow;
    HOTKEY HotKeyWide;
    RELATIVE_RECT ZoomManual;
    RELATIVE_RECT Zoom1P;
    RELATIVE_RECT Zoom2P;
    RELATIVE_RECT ZoomDP;
    BOOL AlwaysOnTop;
} CONFIG, *PCONFIG;

typedef struct _MONITOR_DATA {
    ULONG CurrentCount;
    ULONG TargetCount;
    HMONITOR Monitor;
    LONG OffsetX;
    LONG OffsetY;
    LONG Width;
    LONG Height;
} MONITOR_DATA, *PMONITOR_DATA;

CONFIG GlobalConfig;
int HotKeyId = 0;
MONITOR_DATA GlobalMonitorData;
HWND InfWindow = INVALID_HANDLE_VALUE;
FILE *fp = NULL;
ZOOM_MODE CurrentMode = ZoomModeNone;

INT
CleanupBeforeExit(
    INT ExitCode
    )
{
    if (fp != NULL) {
        log_info("*** InfZoom.exe is exiting *** ");
        fclose(fp);
        fp = NULL;
    }
    ExitProcess(ExitCode);
    return ExitCode;
}

DECLSPEC_NORETURN
VOID
FailFast (
    _In_ UINT ExitCode
    )
{
    log_fatal("FATAL: Exiting with error 0x%x", ExitCode);
    CleanupBeforeExit(ExitCode);
}

ULONG
ClampUlong (
    ULONG d,
    ULONG min,
    ULONG max) {
    ULONG t = (d < min) ? min : d;
    return (t > max) ? max : t;
}

VOID
ParseConfigForHotkey(
    initable_t* IniRoot,
    PCHAR Name,
    PHOTKEY Hotkey
    )
{
    CHAR Buffer[24];

    sprintf(Buffer, "%s_mod", Name);
    Hotkey->k.Mod = (USHORT)ini_as_uint(ini_get(IniRoot, Buffer));
    sprintf(Buffer, "%s_key", Name);
    Hotkey->k.Key = (USHORT)ini_as_uint(ini_get(IniRoot, Buffer));
    log_trace("CONFIG: hotkey %s: 0x%x + 0x%x", Name, Hotkey->k.Mod, Hotkey->k.Key);
}

VOID
ParseConfigForZoom(
    initable_t* IniRoot,
    PCHAR Name,
    PRELATIVE_RECT RelRect
    )
{
    CHAR Buffer[24];

    sprintf(Buffer, "%s_zoom_x", Name);
    RelRect->OffsetX = (USHORT)ini_as_uint(ini_get(IniRoot, Buffer));
    sprintf(Buffer, "%s_zoom_y", Name);
    RelRect->OffsetY = (USHORT)ini_as_uint(ini_get(IniRoot, Buffer));
    sprintf(Buffer, "%s_zoom_w", Name);
    RelRect->Width = (USHORT)ini_as_uint(ini_get(IniRoot, Buffer));
    sprintf(Buffer, "%s_zoom_h", Name);
    RelRect->Height = (USHORT)ini_as_uint(ini_get(IniRoot, Buffer));
    log_trace(
        "CONFIG: zoom %s: %d %d %d %d",
        Name,
        RelRect->OffsetX,
        RelRect->OffsetY,
        RelRect->Width,
        RelRect->Height);
}


VOID
ParseConfig(
    PCONFIG Config
    )

{
    ini_t ini = ini_parse("infzoom.ini", NULL);
    initable_t* root = ini_get_table(&ini, INI_ROOT);

    Config->monitor = (ULONG)ini_as_uint(ini_get(root, "monitor"));
    log_trace("CONFIG: monitor %d", Config->monitor);

    Config->AlwaysOnTop = ini_as_bool(ini_get(root, "always_on_top"));
    log_trace("CONFIG: always_on_top %s", Config->AlwaysOnTop ? "true" : "false");

    ParseConfigForHotkey(root, "exit", &GlobalConfig.HotKeyExit);
    ParseConfigForHotkey(root, "normal", &GlobalConfig.HotKeyNormal);

    ParseConfigForHotkey(root, "1p", &GlobalConfig.HotKey1P);
    ParseConfigForHotkey(root, "2p", &GlobalConfig.HotKey2P);
    ParseConfigForHotkey(root, "dp", &GlobalConfig.HotKeyDP);

    ParseConfigForHotkey(root, "up", &GlobalConfig.HotKeyUp);
    ParseConfigForHotkey(root, "down", &GlobalConfig.HotKeyDown);
    ParseConfigForHotkey(root, "left", &GlobalConfig.HotKeyLeft);
    ParseConfigForHotkey(root, "right", &GlobalConfig.HotKeyRight);

    ParseConfigForHotkey(root, "long", &GlobalConfig.HotKeyLong);
    ParseConfigForHotkey(root, "short", &GlobalConfig.HotKeyShort);
    ParseConfigForHotkey(root, "narrow", &GlobalConfig.HotKeyNarrow);
    ParseConfigForHotkey(root, "wide", &GlobalConfig.HotKeyWide);

    ParseConfigForZoom(root, "1p", &GlobalConfig.Zoom1P);
    ParseConfigForZoom(root, "2p", &GlobalConfig.Zoom2P);
    ParseConfigForZoom(root, "dp", &GlobalConfig.ZoomDP);

    ini_free(&ini);
}

HMONITOR
GetPrimaryMonitorHandle(
    VOID
    )
{
    const POINT ptZero = { 0, 0 };
    return MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
}

VOID
FillMonitorData(
    HMONITOR Monitor,
    PMONITOR_DATA MonitorData
    )
{
    MONITORINFO Info;
    Info.cbSize = sizeof(Info);
    if (GetMonitorInfo(Monitor, &Info)) {
        MonitorData->Monitor = Monitor;
        MonitorData->Width = abs(Info.rcMonitor.right - Info.rcMonitor.left);
        MonitorData->Height = abs(Info.rcMonitor.top - Info.rcMonitor.bottom);
        MonitorData->OffsetX = Info.rcMonitor.left;
        MonitorData->OffsetY = Info.rcMonitor.top;
    }
}

BOOL
CALLBACK
MonitorEnumProc(
    HMONITOR hMonitor,
    HDC hdcMonitor,
    LPRECT lprcMonitor,
    LPARAM dwData)
{
    PMONITOR_DATA MonitorData;

    MonitorData = (PMONITOR_DATA )dwData;
    MonitorData->CurrentCount += 1;
    if (MonitorData->CurrentCount != MonitorData->TargetCount) {
        // return TRUE to continue enumerating
        return TRUE;
    }

    FillMonitorData(hMonitor, MonitorData);
    // return FALSE to stop enumerating
    return FALSE;
}

VOID
ResizeWindow (
    int x,
    int y,
    int w,
    int h
    )
{
    BOOL BoolResult;
    DWORD Flags;
    HWND InsertAfter;

    Flags = (
        SWP_ASYNCWINDOWPOS |
        SWP_NOCOPYBITS |
        SWP_NOREDRAW |
        SWP_NOSENDCHANGING
    );

    if (GlobalConfig.AlwaysOnTop) {
        InsertAfter = HWND_TOPMOST;
    } else {
        InsertAfter = HWND_TOP;
    }

    // SetWindowPos works better than MoveWindow in this case, because of SWP_NOSENDCHANGING.
    // SWP_NOSENDCHANGING allows us to bypass the checks done by the game which restricts how big
    // the window can be (usually capped around 120% or so).
    BoolResult = SetWindowPos(
        InfWindow,
        HWND_TOP,
        x,
        y,
        w,
        h,
        Flags);

    if (!BoolResult) {
        log_error("ERROR: Call to SetWindowPos failed: GLE: %d", GetLastError());
    }

    return;
}

VOID
GetMonitorsHandle(
    ULONG monitor,
    PMONITOR_DATA MonitorData
    )
{
    ZeroMemory(MonitorData, sizeof(*MonitorData));

    // fill with primary monitor info first
    FillMonitorData(GetPrimaryMonitorHandle(), MonitorData);
    MonitorData->TargetCount = monitor;

    // and then enumerate as needed
    if (monitor != 0) {
        EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)MonitorData);
    }
}

VOID
MoveWindowRelative(
    PMONITOR_DATA MonitorData,
    PRELATIVE_RECT RelRect
    )
{
    int x, y, w, h;

    // start with borderless fullscreen to fill the monitor
    x = MonitorData->OffsetX;
    y = MonitorData->OffsetY;
    w = MonitorData->Width;
    h = MonitorData->Height;

    // apply resizes first
    if (RelRect->Width != 100) {
        w = (MonitorData->Width * RelRect->Width) / 1000;
    }
    if (RelRect->Height != 100) {
        h = (MonitorData->Height * RelRect->Height) / 1000;
    }

    // center to the screen
    x += (MonitorData->Width - w) / 2;
    y += (MonitorData->Height - h) / 2;

    // apply offsets
    if (RelRect->OffsetX != 0) {
        x += (MonitorData->Width * RelRect->OffsetX) / 1000;
    }
    if (RelRect->OffsetY != 0) {
        y += (MonitorData->Height * RelRect->OffsetY) / 1000;
    }

    log_trace(
        "        x:%d (%.1f%%)  y:%d (%.1f%%)  w:%d (%.1f%%)  h:%d (%.1f%%)",
        x, RelRect->OffsetX / 10.0,
        y, RelRect->OffsetY / 10.0,
        w, RelRect->Width / 10.0,
        h, RelRect->Height / 10.0);

    ResizeWindow(x, y, w, h);
}

VOID
ResetManualZoom (
    VOID
    )
{
    GlobalConfig.ZoomManual.OffsetX = 0;
    GlobalConfig.ZoomManual.OffsetY = 0;
    GlobalConfig.ZoomManual.Width = 1000;
    GlobalConfig.ZoomManual.Height = 1000;
}

USHORT ActiveModifiers = 0;

BOOL
TestHotKeyDown (
    PRAWKEYBOARD Kbd,
    PHOTKEY Hotkey
    )
{
    if (Hotkey->k.Mod != 0) {
        if ((ActiveModifiers & Hotkey->k.Mod) == 0) {
            return FALSE;
        }
    }
    return (Hotkey->k.Key == Kbd->VKey);
}

VOID
SwitchMode(
    ZOOM_MODE NewMode
    )
{
    if (ZoomModeMax <= NewMode) {
        log_warn("MODE: Invalid mode specified: %d", NewMode);
        return;
    }
    switch (NewMode) {
        case ZoomModeNone:
            log_info("MODE: reset zoom to normal view (manual adjustments possible)... ");
            ResetManualZoom();
            MoveWindowRelative(&GlobalMonitorData, &GlobalConfig.ZoomManual);
            break;

        case ZoomMode1p:
            log_info("MODE: activate 1P zoom view... ");
            MoveWindowRelative(&GlobalMonitorData, &GlobalConfig.Zoom1P);
            break;

        case ZoomMode2p:
            log_info("MODE: activate 2P zoom view... ");
            MoveWindowRelative(&GlobalMonitorData, &GlobalConfig.Zoom2P);
            break;

        case ZoomModeDp:
            log_info("MODE: activate DP zoom view... ");
            MoveWindowRelative(&GlobalMonitorData, &GlobalConfig.ZoomDP);
            break;

        case ZoomModeMax:
        default:
            FailFast(-1);
            break;
    }
    CurrentMode = NewMode;
}

VOID
MessageLoop(
    PRAWKEYBOARD Kbd
    )
{
    USHORT ModifierChange;

    // keep track of modifiers
    ModifierChange = 0;
    switch (Kbd->VKey) {
        case VK_SHIFT:
            ModifierChange = MOD_SHIFT;
            break;
        case VK_MENU:
            ModifierChange = MOD_ALT;
            break;
        case VK_CONTROL:
            ModifierChange = MOD_CONTROL;
            break;
    }
    if (Kbd->Message == WM_KEYDOWN || Kbd->Message == WM_SYSKEYDOWN) {
        ActiveModifiers |= ModifierChange;
    } else if (Kbd->Message == WM_KEYUP || Kbd->Message == WM_SYSKEYUP) {
        ActiveModifiers &= ~ModifierChange;
    }

    if (Kbd->Message != WM_KEYDOWN && Kbd->Message != WM_SYSKEYDOWN) {
        return;
    }

    // everything below tests for WM_KEYDOWN / WM_SYSKEYDOWN events.

    if (TestHotKeyDown(Kbd, &GlobalConfig.HotKeyExit)) {
        log_info("HOTKEY: exit game...");
        PostMessage(InfWindow, WM_CLOSE, 0, 0);
        CleanupBeforeExit(0);

    } else if (TestHotKeyDown(Kbd, &GlobalConfig.HotKeyNormal)) {
        SwitchMode(ZoomModeNone);
    } else if (TestHotKeyDown(Kbd, &GlobalConfig.HotKey1P)) {
        if (CurrentMode == ZoomMode1p) {
            SwitchMode(ZoomModeNone);
        } else {
            SwitchMode(ZoomMode1p);
        }
    } else if (TestHotKeyDown(Kbd, &GlobalConfig.HotKey2P)) {
        if (CurrentMode == ZoomMode2p) {
            SwitchMode(ZoomModeNone);
        } else {
            SwitchMode(ZoomMode2p);
        }
    } else if (TestHotKeyDown(Kbd, &GlobalConfig.HotKeyDP)) {
        if (CurrentMode == ZoomModeDp) {
            SwitchMode(ZoomModeNone);
        } else {
            SwitchMode(ZoomModeDp);
        }

    } else if (CurrentMode == ZoomModeNone) {
        if (TestHotKeyDown(Kbd, &GlobalConfig.HotKeyUp)) {
            log_info("HOTKEY: manual move UP... ");
            GlobalConfig.ZoomManual.OffsetY -= 1;
            MoveWindowRelative(&GlobalMonitorData, &GlobalConfig.ZoomManual);
        } else if (TestHotKeyDown(Kbd, &GlobalConfig.HotKeyDown)) {
            log_info("HOTKEY: manual move DOWN... ");
            GlobalConfig.ZoomManual.OffsetY += 1;
            MoveWindowRelative(&GlobalMonitorData, &GlobalConfig.ZoomManual);
        } else if (TestHotKeyDown(Kbd, &GlobalConfig.HotKeyLeft)) {
            log_info("HOTKEY: manual move LEFT... ");
            GlobalConfig.ZoomManual.OffsetX -= 1;
            MoveWindowRelative(&GlobalMonitorData, &GlobalConfig.ZoomManual);
        } else if (TestHotKeyDown(Kbd, &GlobalConfig.HotKeyRight)) {
            log_info("HOTKEY: manual move RIGHT... ");
            GlobalConfig.ZoomManual.OffsetX += 1;
            MoveWindowRelative(&GlobalMonitorData, &GlobalConfig.ZoomManual);
        
        } else if (TestHotKeyDown(Kbd, &GlobalConfig.HotKeyLong)) {
            log_info("HOTKEY: manual increase height... ");
            GlobalConfig.ZoomManual.Height += 1;
            MoveWindowRelative(&GlobalMonitorData, &GlobalConfig.ZoomManual);
        } else if (TestHotKeyDown(Kbd, &GlobalConfig.HotKeyShort)) {
            log_info("HOTKEY: manual reduce height... ");
            GlobalConfig.ZoomManual.Height -= 1;
            MoveWindowRelative(&GlobalMonitorData, &GlobalConfig.ZoomManual);
        } else if (TestHotKeyDown(Kbd, &GlobalConfig.HotKeyWide)) {
            log_info("HOTKEY: manual increase width... ");
            GlobalConfig.ZoomManual.Width += 1;
            MoveWindowRelative(&GlobalMonitorData, &GlobalConfig.ZoomManual);
        } else if (TestHotKeyDown(Kbd, &GlobalConfig.HotKeyNarrow)) {
            log_info("HOTKEY: manual reduce width... ");
            GlobalConfig.ZoomManual.Width -= 1;
            MoveWindowRelative(&GlobalMonitorData, &GlobalConfig.ZoomManual);
        }
    }
}

int
main(
    int argc,
    char* argv[]
    )
{
    LONG Result;

    fp = fopen("infzoom.log", "w");
    if (fp == NULL) {
        printf("Couldn't open infzoom.log for write");
        return CleanupBeforeExit(-1);
    }

    log_add_fp(fp, LOG_TRACE);

    log_info("*** InfZoom.exe *** ");

    // parse config.json
    log_info("Config parsing start...");
    ParseConfig(&GlobalConfig);

    // find monitor
    GetMonitorsHandle(GlobalConfig.monitor, &GlobalMonitorData);

    // find window handle from args
    if (argc < 2) {
        log_fatal("ERROR: Missing handle to window");
        return CleanupBeforeExit(-1);
    }
    InfWindow = (HWND)atoi(argv[1]);
    log_info("Using window handle 0x%p", InfWindow);

    // make window borderless
    log_info("Call SetWindowLong to make window borderless...");
    Result = SetWindowLong(
        InfWindow,
        GWL_STYLE,
        WS_VISIBLE);

    if (Result == 0) {
        log_error("ERROR: SetWindowLong failed: GLE: %d", GetLastError());
    }

    // do initial call to resize
    SwitchMode(ZoomModeNone);

    log_info("Create an invisible window to capture hotkeys...");
    CreateNewWindow();

    return CleanupBeforeExit(0);
}
