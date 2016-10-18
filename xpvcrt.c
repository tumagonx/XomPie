#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <winnt.h>
#include <tchar.h>

HINSTANCE hmodule = NULL;
HINSTANCE msvcr120 = NULL;
BOOL WINAPI DllMain(HINSTANCE hInst,DWORD reason,LPVOID lpvReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        hmodule = hInst;
        msvcr120 = LoadLibrary(_T("MSVCR120.dll"));
        if (!msvcr120) return FALSE;
    }
    if (reason == DLL_PROCESS_DETACH) FreeLibrary(msvcr120);
    return TRUE;
}
