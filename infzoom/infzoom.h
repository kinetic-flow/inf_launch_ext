#pragma once

#include <stdio.h>
#include <malloc.h>
#include <wchar.h>
#include <Windows.h>
#include <strsafe.h>

#include <log.h>

DECLSPEC_NORETURN
VOID
FailFast (
    _In_ UINT ExitCode
    );

VOID
MessageLoop(
    PRAWKEYBOARD Kbd
    );

// window.c
VOID
CreateNewWindow(
    VOID
    );

VOID
RegisterRawInput(
    VOID
    );
