#include <windows.h>
#include <winternl.h>
#include <ntstatus.h>
#include <tchar.h>

/*
 * To compile make sure _WIN32_WINNT < 0x0600
 * gcc -shared -Wl,--kill-at,--enable-stdcall-fixup,-s kernelxp.def 
 * -o kernelxp.dll kernelxp.c -L. -lmsvcp140 -lmsvcrt -lntdll -lpsapi
 *
 * In MinGW, any executable will load at least kernel32.dll
 * (and msvcrt.dll, unless other crt specified) so skip that.
 */

HINSTANCE hmodule = NULL;
HINSTANCE sysdll = NULL;
BOOL WINAPI DllMain(HINSTANCE hInst,DWORD reason,LPVOID lpvReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        hmodule = hInst;
        sysdll = LoadLibrary(_T("PSAPI.dll"));
        if (!sysdll) return FALSE;
    }
    if (reason == DLL_PROCESS_DETACH) FreeLibrary(sysdll);
    return TRUE;
}

/* 
 * Functions from msvcp140.dll, declaration available in mingw64 headers
 * msvcp140.dll has unusual linkage, can't reliably forward it directly?
 */

typedef struct _TP_TIMER TP_TIMER,*PTP_TIMER;
typedef enum _FILE_INFO_BY_HANDLE_CLASS {
    FileBasicInfo /* is zero? */,
    FileStandardInfo,
    FileNameInfo,
    FileRenameInfo,
    FileDispositionInfo,
    FileAllocationInfo,
    FileEndOfFileInfo,
    FileStreamInfo,
    FileCompressionInfo,
    FileAttributeTagInfo,
    FileIdBothDirectoryInfo,
    FileIdBothDirectoryRestartInfo,
    FileIoPriorityHintInfo,
    FileRemoteProtocolInfo,
    FileFullDirectoryInfo,
    FileFullDirectoryRestartInfo,
    FileStorageInfo,
    FileAlignmentInfo,
    FileIdInfo,
    FileIdExtdDirectoryInfo,
    FileIdExtdDirectoryRestartInfo,
    MaximumFileInfoByHandleClass
} FILE_INFO_BY_HANDLE_CLASS, *PFILE_INFO_BY_HANDLE_CLASS;

WINBASEAPI VOID WINAPI __crtCloseThreadpoolTimer (PTP_TIMER pti);
WINBASEAPI VOID WINAPI CloseThreadpoolTimer (PTP_TIMER pti) {
__crtCloseThreadpoolTimer (pti);
}
WINBASEAPI VOID WINAPI __crtCloseThreadpoolWait (PTP_WAIT pwa);
WINBASEAPI VOID WINAPI CloseThreadpoolWait (PTP_WAIT pwa) {
__crtCloseThreadpoolWait (pwa);
}
WINBASEAPI int WINAPI __crtCompareStringEx (LPCWSTR lpLocaleName, DWORD dwCmpFlags, LPCWCH lpString1, int cchCount1, LPCWCH lpString2, int cchCount2, LPNLSVERSIONINFO lpVersionInformation, LPVOID lpReserved, LPARAM lParam);
WINBASEAPI int WINAPI CompareStringEx (LPCWSTR lpLocaleName, DWORD dwCmpFlags, LPCWCH lpString1, int cchCount1, LPCWCH lpString2, int cchCount2, LPNLSVERSIONINFO lpVersionInformation, LPVOID lpReserved, LPARAM lParam) {
return __crtCompareStringEx (lpLocaleName, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2, lpVersionInformation, lpReserved, lParam);
}
WINBASEAPI HANDLE WINAPI __crtCreateEventExW (LPSECURITY_ATTRIBUTES lpEventAttributes, LPCWSTR lpName, DWORD dwFlags, DWORD dwDesiredAccess);
WINBASEAPI HANDLE WINAPI CreateEventExW (LPSECURITY_ATTRIBUTES lpEventAttributes, LPCWSTR lpName, DWORD dwFlags, DWORD dwDesiredAccess) {
return __crtCreateEventExW (lpEventAttributes, lpName, dwFlags, dwDesiredAccess);
}
WINBASEAPI HANDLE WINAPI __crtCreateSemaphoreExW (LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCWSTR lpName, DWORD dwFlags, DWORD dwDesiredAccess);
WINBASEAPI HANDLE WINAPI CreateSemaphoreExW (LPSECURITY_ATTRIBUTES lpSemaphoreAttributes, LONG lInitialCount, LONG lMaximumCount, LPCWSTR lpName, DWORD dwFlags, DWORD dwDesiredAccess) {
return __crtCreateSemaphoreExW (lpSemaphoreAttributes, lInitialCount, lMaximumCount, lpName, dwFlags, dwDesiredAccess);
}
WINBASEAPI BOOLEAN APIENTRY __crtCreateSymbolicLinkW (LPCWSTR lpSymlinkFileName, LPCWSTR lpTargetFileName, DWORD dwFlags);
WINBASEAPI BOOLEAN APIENTRY CreateSymbolicLinkW (LPCWSTR lpSymlinkFileName, LPCWSTR lpTargetFileName, DWORD dwFlags) {
return __crtCreateSymbolicLinkW (lpSymlinkFileName, lpTargetFileName, dwFlags);
}
WINBASEAPI PTP_TIMER WINAPI __crtCreateThreadpoolTimer (PTP_TIMER_CALLBACK pfnti, PVOID pv, PTP_CALLBACK_ENVIRON pcbe);
WINBASEAPI PTP_TIMER WINAPI CreateThreadpoolTimer (PTP_TIMER_CALLBACK pfnti, PVOID pv, PTP_CALLBACK_ENVIRON pcbe) {
return __crtCreateThreadpoolTimer (pfnti, pv, pcbe);
}
WINBASEAPI PTP_WAIT WINAPI __crtCreateThreadpoolWait (PTP_WAIT_CALLBACK pfnwa, PVOID pv, PTP_CALLBACK_ENVIRON pcbe);
WINBASEAPI PTP_WAIT WINAPI CreateThreadpoolWait (PTP_WAIT_CALLBACK pfnwa, PVOID pv, PTP_CALLBACK_ENVIRON pcbe) {
return __crtCreateThreadpoolWait (pfnwa, pv, pcbe);
}
WINBASEAPI VOID WINAPI __crtFlushProcessWriteBuffers (VOID);
WINBASEAPI VOID WINAPI FlushProcessWriteBuffers (VOID) {
__crtFlushProcessWriteBuffers ();
}
WINBASEAPI VOID WINAPI __crtFreeLibraryWhenCallbackReturns (PTP_CALLBACK_INSTANCE pci, HMODULE mod);
WINBASEAPI VOID WINAPI FreeLibraryWhenCallbackReturns (PTP_CALLBACK_INSTANCE pci, HMODULE mod) {
__crtFreeLibraryWhenCallbackReturns (pci, mod);
}
WINBASEAPI DWORD WINAPI __crtGetCurrentProcessorNumber (VOID);
WINBASEAPI DWORD WINAPI GetCurrentProcessorNumber (VOID) {
return __crtGetCurrentProcessorNumber ();
}
WINBASEAPI WINBOOL WINAPI __crtGetFileInformationByHandleEx (HANDLE hFile, FILE_INFO_BY_HANDLE_CLASS FileInformationClass, LPVOID lpFileInformation, DWORD dwBufferSize);
WINBASEAPI WINBOOL WINAPI GetFileInformationByHandleEx (HANDLE hFile, FILE_INFO_BY_HANDLE_CLASS FileInformationClass, LPVOID lpFileInformation, DWORD dwBufferSize) {
return __crtGetFileInformationByHandleEx (hFile, FileInformationClass, lpFileInformation, dwBufferSize);
}
WINBASEAPI int WINAPI __crtGetLocaleInfoEx (LPCWSTR lpLocaleName, LCTYPE LCType, LPWSTR lpLCData, int cchData);
WINBASEAPI int WINAPI GetLocaleInfoEx (LPCWSTR lpLocaleName, LCTYPE LCType, LPWSTR lpLCData, int cchData) {
return __crtGetLocaleInfoEx (lpLocaleName, LCType, lpLCData, cchData);
}
WINBASEAPI VOID WINAPI __crtGetSystemTimePreciseAsFileTime (LPFILETIME lpSystemTimeAsFileTime);
WINBASEAPI VOID WINAPI GetSystemTimePreciseAsFileTime (LPFILETIME lpSystemTimeAsFileTime) {
__crtGetSystemTimePreciseAsFileTime (lpSystemTimeAsFileTime);
}
WINBASEAPI ULONGLONG WINAPI __crtGetTickCount64 (VOID);
WINBASEAPI ULONGLONG WINAPI GetTickCount64 (VOID) {
return __crtGetTickCount64 ();
}
WINBASEAPI WINBOOL WINAPI __crtInitOnceExecuteOnce (PINIT_ONCE InitOnce, PINIT_ONCE_FN InitFn, PVOID Parameter, LPVOID *Context);
WINBASEAPI WINBOOL WINAPI InitOnceExecuteOnce (PINIT_ONCE InitOnce, PINIT_ONCE_FN InitFn, PVOID Parameter, LPVOID *Context) {
return __crtInitOnceExecuteOnce (InitOnce, InitFn, Parameter, *Context);
}
WINBASEAPI WINBOOL WINAPI __crtInitializeCriticalSectionEx (LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount, DWORD Flags);
WINBASEAPI WINBOOL WINAPI InitializeCriticalSectionEx (LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount, DWORD Flags) {
return __crtInitializeCriticalSectionEx (lpCriticalSection, dwSpinCount, Flags);
}
WINBASEAPI int WINAPI __crtLCMapStringEx (LPCWSTR lpLocaleName, DWORD dwMapFlags, LPCWSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest, LPNLSVERSIONINFO lpVersionInformation, LPVOID lpReserved, LPARAM sortHandle);
WINBASEAPI int WINAPI LCMapStringEx (LPCWSTR lpLocaleName, DWORD dwMapFlags, LPCWSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest, LPNLSVERSIONINFO lpVersionInformation, LPVOID lpReserved, LPARAM sortHandle) {
return __crtLCMapStringEx (lpLocaleName, dwMapFlags, lpSrcStr, cchSrc, lpDestStr, cchDest, lpVersionInformation, lpReserved, sortHandle);
}
WINBASEAPI WINBOOL WINAPI __crtSetFileInformationByHandle (HANDLE hFile, FILE_INFO_BY_HANDLE_CLASS FileInformationClass, LPVOID lpFileInformation, DWORD dwBufferSize);
WINBASEAPI WINBOOL WINAPI SetFileInformationByHandle (HANDLE hFile, FILE_INFO_BY_HANDLE_CLASS FileInformationClass, LPVOID lpFileInformation, DWORD dwBufferSize) {
return __crtSetFileInformationByHandle (hFile, FileInformationClass, lpFileInformation, dwBufferSize);
}
WINBASEAPI VOID WINAPI __crtSetThreadpoolTimer (PTP_TIMER pti, PFILETIME pftDueTime, DWORD msPeriod, DWORD msWindowLength);
WINBASEAPI VOID WINAPI SetThreadpoolTimer (PTP_TIMER pti, PFILETIME pftDueTime, DWORD msPeriod, DWORD msWindowLength) {
__crtSetThreadpoolTimer (pti, pftDueTime, msPeriod, msWindowLength);
}
WINBASEAPI VOID WINAPI __crtSetThreadpoolWait (PTP_WAIT pwa, HANDLE h, PFILETIME pftTimeout);
WINBASEAPI VOID WINAPI SetThreadpoolWait (PTP_WAIT pwa, HANDLE h, PFILETIME pftTimeout) {
__crtSetThreadpoolWait (pwa, h, pftTimeout);
}
WINBASEAPI VOID WINAPI __crtWaitForThreadpoolTimerCallbacks (PTP_TIMER pti, WINBOOL fCancelPendingCallbacks);
WINBASEAPI VOID WINAPI WaitForThreadpoolTimerCallbacks (PTP_TIMER pti, WINBOOL fCancelPendingCallbacks) {
__crtWaitForThreadpoolTimerCallbacks (pti, fCancelPendingCallbacks);
}


/*
 * From WINE 1.9.11, once keep growing consider to link to wine dll
 */

#define strlenW wcslen
#define strcatW wcscat
#define strncmpiW _wcsnicmp

//supress msg
#define TRACE(...) do { } while(0)
#define WARN(...) do { } while(0)
#define FIXME(...) do { } while(0)

/*
 * File handling functions
 *
 * Copyright 1993 John Burton
 * Copyright 1996, 2004 Alexandre Julliard
 * Copyright 2008 Jeff Zaroyko
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */
 
static BOOL oem_file_apis;

/***********************************************************************
 *           GetFinalPathNameByHandleW (KERNEL32.@)
 */

DWORD WINAPI WineGetFinalPathNameByHandleW(HANDLE file, LPWSTR path, DWORD charcount, DWORD flags)
{
    WCHAR buffer[sizeof(OBJECT_NAME_INFORMATION) + MAX_PATH + 1];
    OBJECT_NAME_INFORMATION *info = (OBJECT_NAME_INFORMATION*)&buffer;
    WCHAR drive_part[MAX_PATH];
    DWORD drive_part_len = 0;
    NTSTATUS status;
    DWORD result = 0;
    ULONG dummy;
    WCHAR *ptr;

    TRACE( "(%p,%p,%d,%x)\n", file, path, charcount, flags );

    if (flags & ~(FILE_NAME_OPENED | VOLUME_NAME_GUID | VOLUME_NAME_NONE | VOLUME_NAME_NT))
    {
        WARN("Unknown flags: %x\n", flags);
        SetLastError( ERROR_INVALID_PARAMETER );
        return 0;
    }

    /* get object name */
    status = NtQueryObject( file, ObjectNameInformation, &buffer, sizeof(buffer) - sizeof(WCHAR), &dummy );
    if (status != STATUS_SUCCESS)
    {
        SetLastError( RtlNtStatusToDosError( status ) );
        return 0;
    }
    if (!info->Name.Buffer)
    {
        SetLastError( ERROR_INVALID_HANDLE );
        return 0;
    }
    if (info->Name.Length < 4 * sizeof(WCHAR) || info->Name.Buffer[0] != '\\' ||
             info->Name.Buffer[1] != '?' || info->Name.Buffer[2] != '?' || info->Name.Buffer[3] != '\\' )
    {
        FIXME("Unexpected object name: %s\n", debugstr_wn(info->Name.Buffer, info->Name.Length / sizeof(WCHAR)));
        SetLastError( ERROR_GEN_FAILURE );
        return 0;
    }

    /* add terminating null character, remove "\\??\\" */
    info->Name.Buffer[info->Name.Length / sizeof(WCHAR)] = 0;
    info->Name.Length -= 4 * sizeof(WCHAR);
    info->Name.Buffer += 4;

    /* FILE_NAME_OPENED is not supported yet, and would require Wineserver changes */
    if (flags & FILE_NAME_OPENED)
    {
        FIXME("FILE_NAME_OPENED not supported\n");
        flags &= ~FILE_NAME_OPENED;
    }

    /* Get information required for VOLUME_NAME_NONE, VOLUME_NAME_GUID and VOLUME_NAME_NT */
    if (flags == VOLUME_NAME_NONE || flags == VOLUME_NAME_GUID || flags == VOLUME_NAME_NT)
    {
        if (!GetVolumePathNameW( info->Name.Buffer, drive_part, MAX_PATH ))
            return 0;

        drive_part_len = strlenW(drive_part);
        if (!drive_part_len || drive_part_len > strlenW(info->Name.Buffer) ||
                drive_part[drive_part_len-1] != '\\' ||
                strncmpiW( info->Name.Buffer, drive_part, drive_part_len ))
        {
            FIXME("Path %s returned by GetVolumePathNameW does not match file path %s\n",
                debugstr_w(drive_part), debugstr_w(info->Name.Buffer));
            SetLastError( ERROR_GEN_FAILURE );
            return 0;
        }
    }

    if (flags == VOLUME_NAME_NONE)
    {
        ptr    = info->Name.Buffer + drive_part_len - 1;
        result = strlenW(ptr);
        if (result < charcount)
            memcpy(path, ptr, (result + 1) * sizeof(WCHAR));
        else result++;
    }
    else if (flags == VOLUME_NAME_GUID)
    {
        WCHAR volume_prefix[51];

        /* GetVolumeNameForVolumeMountPointW sets error code on failure */
        if (!GetVolumeNameForVolumeMountPointW( drive_part, volume_prefix, 50 ))
            return 0;

        ptr    = info->Name.Buffer + drive_part_len;
        result = strlenW(volume_prefix) + strlenW(ptr);
        if (result < charcount)
        {
            path[0] = 0;
            strcatW(path, volume_prefix);
            strcatW(path, ptr);
        }
        else
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            result++;
        }
    }
    else if (flags == VOLUME_NAME_NT)
    {
        WCHAR nt_prefix[MAX_PATH];

        /* QueryDosDeviceW sets error code on failure */
        drive_part[drive_part_len - 1] = 0;
        if (!QueryDosDeviceW( drive_part, nt_prefix, MAX_PATH ))
            return 0;

        ptr    = info->Name.Buffer + drive_part_len - 1;
        result = strlenW(nt_prefix) + strlenW(ptr);
        if (result < charcount)
        {
            path[0] = 0;
            strcatW(path, nt_prefix);
            strcatW(path, ptr);
        }
        else
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            result++;
        }
    }
    else if (flags == VOLUME_NAME_DOS)
    {
        static const WCHAR dos_prefix[] = {'\\','\\','?','\\', '\0'};

        result = strlenW(dos_prefix) + strlenW(info->Name.Buffer);
        if (result < charcount)
        {
            path[0] = 0;
            strcatW(path, dos_prefix);
            strcatW(path, info->Name.Buffer);
        }
        else
        {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            result++;
        }
    }
    else
    {
        /* Windows crashes here, but we prefer returning ERROR_INVALID_PARAMETER */
        WARN("Invalid combination of flags: %x\n", flags);
        SetLastError( ERROR_INVALID_PARAMETER );
    }

    return result;
}

/***********************************************************************
 *           GetFinalPathNameByHandleA (KERNEL32.@)
 */
DWORD WINAPI WineGetFinalPathNameByHandleA(HANDLE file, LPSTR path, DWORD charcount, DWORD flags)
{
    WCHAR *str;
    DWORD result, len, cp;

    TRACE( "(%p,%p,%d,%x)\n", file, path, charcount, flags);

    len = WineGetFinalPathNameByHandleW(file, NULL, 0, flags);
    if (len == 0)
        return 0;

    str = HeapAlloc(GetProcessHeap(), 0, len * sizeof(WCHAR));
    if (!str)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return 0;
    }

    result = WineGetFinalPathNameByHandleW(file, str, len, flags);
    if (result != len - 1)
    {
        HeapFree(GetProcessHeap(), 0, str);
        WARN("GetFinalPathNameByHandleW failed unexpectedly: %u\n", result);
        return 0;
    }

    cp = oem_file_apis ? CP_OEMCP : CP_ACP;

    len = WideCharToMultiByte(cp, 0, str, -1, NULL, 0, NULL, NULL);
    if (!len)
    {
        HeapFree(GetProcessHeap(), 0, str);
        WARN("Failed to get multibyte length\n");
        return 0;
    }

    if (charcount < len)
    {
        HeapFree(GetProcessHeap(), 0, str);
        return len - 1;
    }

    len = WideCharToMultiByte(cp, 0, str, -1, path, charcount, NULL, NULL);
    if (!len)
    {
        HeapFree(GetProcessHeap(), 0, str);
        WARN("WideCharToMultiByte failed\n");
        return 0;
    }

    HeapFree(GetProcessHeap(), 0, str);

    return len - 1;
}
