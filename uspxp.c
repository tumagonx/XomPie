#include <tchar.h>
#include <shlobj.h>

/*
 * gcc -shared -D_UNICODE -DUNICODE -O uspxp.def -o uspxp.dll uspxp.c -s -lshell32 
 */
 
TCHAR path[MAX_PATH];
HINSTANCE hmodule = NULL;
HINSTANCE usp = NULL;
BOOL WINAPI DllMain(HINSTANCE hInst,DWORD reason,LPVOID lpvReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        hmodule = hInst;
        SHGetSpecialFolderPath(NULL, path, CSIDL_COMMON_PROGRAMS, FALSE);
        _tcscat(path,_T("\\Microsoft Shared\\OFFICE14\\usp10.dll"));
        usp = LoadLibrary(path);
        if (!usp) return FALSE;
    }
    if (reason == DLL_PROCESS_DETACH) FreeLibrary(usp);
    return TRUE;
}
