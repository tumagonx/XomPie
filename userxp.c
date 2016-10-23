#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <winternl.h>
#include <ntstatus.h>
#include <tchar.h>
#include <string.h>

/*
 * gcc -shared -Wl,--kill-at,--enable-stdcall-fixup,-s -D_UNICODE -DUNICODE
 * -O userxp.def -o userxp.dll userxp.c -lole32 -luuid
 */
 
HINSTANCE hmodule = NULL;
HINSTANCE user = NULL;
BOOL WINAPI DllMain(HINSTANCE hInst,DWORD reason,LPVOID lpvReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        hmodule = hInst;
        user = LoadLibrary(_T("USER32.dll"));
        if (!user) return FALSE;
    }
    if (reason == DLL_PROCESS_DETACH) FreeLibrary(user);
    return TRUE;
}

//dummy
BOOL WINAPI DummySetProcessDPIAware( VOID ) {
    return TRUE;
}
