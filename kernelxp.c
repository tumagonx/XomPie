#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <winternl.h>
#include <ntstatus.h>
#include <tchar.h>
#include <string.h>
#include <winnls.h>

/*
 * gcc -shared -D_UNICODE -DUNICODE -Wl,--kill-at,--enable-stdcall-fixup,-s -L.
 * kernelxp.def -o kernelxp.dll kernelxp.c -lmsvcp140 -lmsvcrt -lntdll -lpsapi
 *
 * In MinGW, any executable will load at least kernel32.dll
 * (and msvcrt.dll, unless other crt specified) so skip that.
 */

HINSTANCE hmodule = NULL;
HINSTANCE msvcr120 = NULL;
HINSTANCE psapi = NULL;
HINSTANCE advapixp = NULL;
BOOL WINAPI DllMain(HINSTANCE hInst,DWORD reason,LPVOID lpvReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        hmodule = hInst;
        msvcr120 = LoadLibrary(_T("MSVCR120.dll"));
        psapi = LoadLibrary(_T("PSAPI.dll"));
        advapixp = LoadLibrary(_T("ADVAPIXP.dll"));
        if (!msvcr120) return FALSE;
        if (!psapi) return FALSE;
        if (!advapixp) return FALSE;
    }
    if (reason == DLL_PROCESS_DETACH) {
        FreeLibrary(msvcr120);
        FreeLibrary(psapi);
        FreeLibrary(advapixp);
    }
    return TRUE;
}

/* 
 * Functions from msvcp140.dll, declaration available in mingw64 headers
 * msvcp140.dll has unusual linkage, can't reliably forward it directly?
 */

typedef struct _TP_TIMER TP_TIMER,*PTP_TIMER;

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

//missing in mingw-w64 4.0 winternl, use from wine
typedef enum _MEMORY_INFORMATION_CLASS {
    MemoryBasicInformation,
    MemoryWorkingSetList,
    MemorySectionName,
    MemoryBasicVlmInformation
} MEMORY_INFORMATION_CLASS;
typedef struct _THREAD_BASIC_INFORMATION
{
    NTSTATUS  ExitStatus;
    PVOID     TebBaseAddress;
    CLIENT_ID ClientId;
    ULONG_PTR AffinityMask;
    LONG      Priority;
    LONG      BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;
NTSYSAPI NTSTATUS WINAPI NtQueryVirtualMemory(HANDLE,LPCVOID,MEMORY_INFORMATION_CLASS,PVOID,SIZE_T,SIZE_T*);
NTSYSAPI NTSTATUS WINAPI NtReleaseKeyedEvent(HANDLE,const void*,BOOLEAN,const LARGE_INTEGER*);
NTSYSAPI NTSTATUS WINAPI NtWaitForKeyedEvent(HANDLE,const void*,BOOLEAN,const LARGE_INTEGER*);
NTSYSAPI NTSTATUS WINAPI RtlEnterCriticalSection(RTL_CRITICAL_SECTION *);
NTSYSAPI NTSTATUS WINAPI RtlLeaveCriticalSection(RTL_CRITICAL_SECTION *);
NTSYSAPI NTSTATUS WINAPI RtlOemStringToUnicodeString(UNICODE_STRING*,const STRING*,BOOLEAN);
NTSYSAPI void WINAPI RtlRaiseStatus(NTSTATUS);

// mingw64 werapi.h is broken use from wine
typedef HANDLE HREPORT;
#define WER_MAX_PREFERRED_MODULES_BUFFER    256
typedef enum _WER_SUBMIT_RESULT
{
    WerReportQueued = 1,
    WerReportUploaded,
    WerReportDebug,
    WerReportFailed,
    WerDisabled,
    WerReportCancelled,
    WerDisabledQueue,
    WerReportAsync,
    WerCustomAction
} WER_SUBMIT_RESULT, *PWER_SUBMIT_RESULT;
typedef enum _WER_REGISTER_FILE_TYPE
{
    WerRegFileTypeUserDocument = 1,
    WerRegFileTypeOther = 2,
    WerRegFileTypeMax
} WER_REGISTER_FILE_TYPE;

// Dummy
VOID WINAPI DummyRaiseFailFastException(PEXCEPTION_RECORD pExceptionRecord, PCONTEXT pContextRecord, DWORD dwFlags) {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
}
HANDLE WINAPI DummyPowerCreateRequest(REASON_CONTEXT *context) {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return INVALID_HANDLE_VALUE;
}
BOOL WINAPI DummyPowerSetRequest(HANDLE request, POWER_REQUEST_TYPE type) {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
BOOL WINAPI DummyPowerClearRequest(HANDLE request, POWER_REQUEST_TYPE type) {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
}
HRESULT WINAPI DummyRegisterApplicationRecoveryCallback (APPLICATION_RECOVERY_CALLBACK pRecoveyCallback, PVOID pvParameter, DWORD dwPingInterval, DWORD dwFlags) {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return E_FAIL;
}
HRESULT WINAPI DummyRegisterApplicationRestart (PCWSTR pwzCommandline, DWORD dwFlags) {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return E_FAIL;
}
BOOL WINAPI DummySetFileCompletionNotificationModes( HANDLE handle, UCHAR flags )
{
    return TRUE;
}
HRESULT WINAPI DummyWerGetFlags(HANDLE hProcess,PDWORD pdwFlags) {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return E_FAIL;
}
HRESULT WINAPI DummyWerRegisterFile(PCWSTR pwzFile,WER_REGISTER_FILE_TYPE regFileType,DWORD dwFlags) {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return E_FAIL;
}
HRESULT WINAPI DummyWerRegisterMemoryBlock(PVOID pvAddress,DWORD dwSize) {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return E_FAIL;
}
HRESULT WINAPI DummyWerRegisterRuntimeExceptionModule(PCWSTR pwszOutOfProcessCallbackDll, PVOID pContext) {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return E_FAIL;
}
HRESULT WINAPI DummyWerSetFlags(DWORD dwFlags) {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return E_FAIL;
}
HRESULT WINAPI DummyWerUnregisterFile(PCWSTR pwzFilePath) {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return E_FAIL;
}
HRESULT WINAPI DummyWerUnregisterMemoryBlock(PVOID pvAddress) {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return E_FAIL;
}
HRESULT WINAPI DummyWerUnregisterRuntimeExceptionModule(PCWSTR pwszOutOfProcessCallbackDll, PVOID pContext) {
    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return E_FAIL;
}
DLL_DIRECTORY_COOKIE WINAPI DummyAddDllDirectory (PCWSTR NewDirectory) {
}


// Intrinsic
LONGLONG WINAPI InterlockedCompareExchange64(LONGLONG volatile *Destination, LONGLONG Exchange, LONGLONG Comperand) {
    return _InterlockedCompareExchange64(Destination, Exchange, Comperand);
}

/*
 * From WINE 1.9.11, once keep growing consider to link to wine dll
 */

#define strlenW wcslen
#define strcatW wcscat
#define strncmpiW _wcsnicmp
#define strcmpW wcscmp
#define strpbrkW wcspbrk
#define strcpyW wcscpy
#define strchrW wcschr
#define tolowerW towlower
#define DECLSPEC_HIDDEN

//supress msg
#define TRACE(...) do { } while(0)
#define WARN(...) do { } while(0)
#define FIXME(...) do { } while(0)

//rename funcs
#define CreateEventExA WineCreateEventExA
#define CreateSemaphoreExA WineCreateSemaphoreExA
#define CreateSymbolicLinkA WineCreateSymbolicLinkA
#define CreateEventExA WineCreateEventExA
#define InitOnceBeginInitialize WineInitOnceBeginInitialize
#define InitOnceComplete WineInitOnceComplete
#define GetFinalPathNameByHandleW WineGetFinalPathNameByHandleW
#define GetFinalPathNameByHandleA WineGetFinalPathNameByHandleA
#define GetSystemDefaultLocaleName WineGetSystemDefaultLocaleName
#define GetSystemPreferredUILanguages WineGetSystemPreferredUILanguages
#define GetUserPreferredUILanguages WineGetUserPreferredUILanguages
#define GetThreadPreferredUILanguages WineGetThreadPreferredUILanguages
#define GetThreadId WineGetThreadId
#define K32QueryWorkingSetEx WineK32QueryWorkingSetEx
#define LCIDToLocaleName WineLCIDToLocaleName
#define LocaleNameToLCID WineLocaleNameToLCID
#define NeedCurrentDirectoryForExePathW WineNeedCurrentDirectoryForExePathW
#define NeedCurrentDirectoryForExePathA WineNeedCurrentDirectoryForExePathA
#define RtlInitializeSRWLock WineRtlInitializeSRWLock
#define RtlAcquireSRWLockExclusive WineRtlAcquireSRWLockExclusive
#define RtlAcquireSRWLockShared WineRtlAcquireSRWLockShared
#define RtlReleaseSRWLockExclusive WineRtlReleaseSRWLockExclusive
#define RtlReleaseSRWLockShared WineRtlReleaseSRWLockShared
#define RtlTryAcquireSRWLockExclusive WineRtlTryAcquireSRWLockExclusive
#define RtlTryAcquireSRWLockShared WineRtlTryAcquireSRWLockShared
//#define RtlInitializeConditionVariable WineRtlInitializeConditionVariable
#define InitializeConditionVariable BWCInitializeConditionVariable
#define RtlRunOnceInitialize WineRtlRunOnceInitialize
//#define RtlSleepConditionVariableCS WineRtlSleepConditionVariableCS
#define SleepConditionVariableCS BWCSleepConditionVariableCS
//#define RtlWakeAllConditionVariable WineRtlWakeAllConditionVariable
#define WakeAllConditionVariable BWCWakeAllConditionVariable
//#define RtlWakeConditionVariable WineRtlWakeConditionVariable
#define WakeConditionVariable BWCWakeConditionVariable
#define SetThreadPreferredUILanguages WineSetThreadPreferredUILanguages
//#define SleepConditionVariableCS WineSleepConditionVariableCS
#define SleepConditionVariableSRW WineSleepConditionVariableSRW
#define QueryFullProcessImageNameW WineQueryFullProcessImageNameW
#define QueryFullProcessImageNameA WineQueryFullProcessImageNameA

#include "wineconsts.c"

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
 *           FILE_name_AtoW
 *
 * Convert a file name to Unicode, taking into account the OEM/Ansi API mode.
 *
 * If alloc is FALSE uses the TEB static buffer, so it can only be used when
 * there is no possibility for the function to do that twice, taking into
 * account any called function.
 */
WCHAR *FILE_name_AtoW( LPCSTR name, BOOL alloc )
{
    ANSI_STRING str;
    UNICODE_STRING strW, *pstrW;
    NTSTATUS status;

    RtlInitAnsiString( &str, name );
    // StaticUnicodeString is WINE specific, always use alloc
    // pstrW = alloc ? &strW : &NtCurrentTeb()->StaticUnicodeString;
    pstrW = &strW;
    if (oem_file_apis)
        status = RtlOemStringToUnicodeString( pstrW, &str, alloc );
    else
        status = RtlAnsiStringToUnicodeString( pstrW, &str, alloc );
    if (status == STATUS_SUCCESS) return pstrW->Buffer;

    if (status == STATUS_BUFFER_OVERFLOW)
        SetLastError( ERROR_FILENAME_EXCED_RANGE );
    else
        SetLastError( RtlNtStatusToDosError(status) );
    return NULL;
}

/*************************************************************************
 *           CreateSymbolicLinkA   (KERNEL32.@)
 */
BOOLEAN WINAPI CreateSymbolicLinkA(LPCSTR link, LPCSTR target, DWORD flags)
{
    WCHAR linkbuffer[MAX_PATH];
    WCHAR targetbuffer[MAX_PATH];
    if (!MultiByteToWideChar( CP_ACP, 0, link, -1, linkbuffer, MAX_PATH ))
    {
        SetLastError( ERROR_FILENAME_EXCED_RANGE );
        return 0;
    }
    if (!MultiByteToWideChar( CP_ACP, 0, target, -1, targetbuffer, MAX_PATH ))
    {
        SetLastError( ERROR_FILENAME_EXCED_RANGE );
        return 0;
    }
    return __crtCreateSymbolicLinkW (linkbuffer, targetbuffer, flags);
}

/***********************************************************************
 *           GetFinalPathNameByHandleW (KERNEL32.@)
 */

DWORD WINAPI GetFinalPathNameByHandleW(HANDLE file, LPWSTR path, DWORD charcount, DWORD flags)
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
DWORD WINAPI GetFinalPathNameByHandleA(HANDLE file, LPSTR path, DWORD charcount, DWORD flags)
{
    WCHAR *str;
    DWORD result, len, cp;

    TRACE( "(%p,%p,%d,%x)\n", file, path, charcount, flags);

    len = GetFinalPathNameByHandleW(file, NULL, 0, flags);
    if (len == 0)
        return 0;

    str = HeapAlloc(GetProcessHeap(), 0, len * sizeof(WCHAR));
    if (!str)
    {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return 0;
    }

    result = GetFinalPathNameByHandleW(file, str, len, flags);
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

/***********************************************************************
 *           NeedCurrentDirectoryForExePathW   (KERNEL32.@)
 */
BOOL WINAPI NeedCurrentDirectoryForExePathW( LPCWSTR name )
{
    static const WCHAR env_name[] = {'N','o','D','e','f','a','u','l','t',
                                     'C','u','r','r','e','n','t',
                                     'D','i','r','e','c','t','o','r','y',
                                     'I','n','E','x','e','P','a','t','h',0};
    WCHAR env_val;

    TRACE("(%s)\n", debugstr_w(name));

    if (strchrW(name, '\\'))
        return TRUE;

    /* Check the existence of the variable, not value */
    if (!GetEnvironmentVariableW( env_name, &env_val, 1 ))
        return TRUE;

    return FALSE;
}


/***********************************************************************
 *           NeedCurrentDirectoryForExePathA   (KERNEL32.@)
 */
BOOL WINAPI NeedCurrentDirectoryForExePathA( LPCSTR name )
{
    WCHAR *nameW;

    if (!(nameW = FILE_name_AtoW( name, TRUE ))) {
        HeapFree( GetProcessHeap(), 0, nameW );
        return TRUE;
    }
    HeapFree( GetProcessHeap(), 0, nameW );
    return NeedCurrentDirectoryForExePathW( nameW );
}

/*
 * Locale support
 *
 * Copyright 1995 Martin von Loewis
 * Copyright 1998 David Lee Lambert
 * Copyright 2000 Julio C�sar G�zquez
 * Copyright 2002 Alexandre Julliard for CodeWeavers
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
HMODULE kernel32_handle DECLSPEC_HIDDEN;

enum {
    BASE = 36,
    TMIN = 1,
    TMAX = 26,
    SKEW = 38,
    DAMP = 700,
    INIT_BIAS = 72,
    INIT_N = 128
};

static const struct charset_entry
{
    const char *charset_name;
    UINT        codepage;
} charset_names[] =
{
    { "BIG5", 950 },
    { "CP1250", 1250 },
    { "CP1251", 1251 },
    { "CP1252", 1252 },
    { "CP1253", 1253 },
    { "CP1254", 1254 },
    { "CP1255", 1255 },
    { "CP1256", 1256 },
    { "CP1257", 1257 },
    { "CP1258", 1258 },
    { "CP932", 932 },
    { "CP936", 936 },
    { "CP949", 949 },
    { "CP950", 950 },
    { "EUCJP", 20932 },
    { "GB2312", 936 },
    { "IBM037", 37 },
    { "IBM1026", 1026 },
    { "IBM424", 424 },
    { "IBM437", 437 },
    { "IBM500", 500 },
    { "IBM850", 850 },
    { "IBM852", 852 },
    { "IBM855", 855 },
    { "IBM857", 857 },
    { "IBM860", 860 },
    { "IBM861", 861 },
    { "IBM862", 862 },
    { "IBM863", 863 },
    { "IBM864", 864 },
    { "IBM865", 865 },
    { "IBM866", 866 },
    { "IBM869", 869 },
    { "IBM874", 874 },
    { "IBM875", 875 },
    { "ISO88591", 28591 },
    { "ISO885910", 28600 },
    { "ISO885913", 28603 },
    { "ISO885914", 28604 },
    { "ISO885915", 28605 },
    { "ISO885916", 28606 },
    { "ISO88592", 28592 },
    { "ISO88593", 28593 },
    { "ISO88594", 28594 },
    { "ISO88595", 28595 },
    { "ISO88596", 28596 },
    { "ISO88597", 28597 },
    { "ISO88598", 28598 },
    { "ISO88599", 28599 },
    { "KOI8R", 20866 },
    { "KOI8U", 21866 },
    { "UTF8", CP_UTF8 }
};

struct locale_name
{
    WCHAR  win_name[128];   /* Windows name ("en-US") */
    WCHAR  lang[128];       /* language ("en") (note: buffer contains the other strings too) */
    WCHAR *country;         /* country ("US") */
    WCHAR *charset;         /* charset ("UTF-8") for Unix format only */
    WCHAR *script;          /* script ("Latn") for Windows format only */
    WCHAR *modifier;        /* modifier or sort order */
    LCID   lcid;            /* corresponding LCID */
    int    matches;         /* number of elements matching LCID (0..4) */
    UINT   codepage;        /* codepage corresponding to charset */
};

/***********************************************************************
 *              charset_cmp (internal)
 */
static int charset_cmp( const void *name, const void *entry )
{
    const struct charset_entry *charset = entry;
    return strcasecmp( name, charset->charset_name );
}

/***********************************************************************
 *		find_charset
 */
static UINT find_charset( const WCHAR *name )
{
    const struct charset_entry *entry;
    char charset_name[16];
    size_t i, j;

    /* remove punctuation characters from charset name */
    for (i = j = 0; name[i] && j < sizeof(charset_name)-1; i++)
        if (isalnum((unsigned char)name[i])) charset_name[j++] = name[i];
    charset_name[j] = 0;

    entry = bsearch( charset_name, charset_names,
                     sizeof(charset_names)/sizeof(charset_names[0]),
                     sizeof(charset_names[0]), charset_cmp );
    if (entry) return entry->codepage;
    return 0;
}

/***********************************************************************
 *           find_locale_id_callback
 */
static BOOL CALLBACK find_locale_id_callback( HMODULE hModule, LPCWSTR type,
                                              LPCWSTR name, WORD LangID, LPARAM lParam )
{
    struct locale_name *data = (struct locale_name *)lParam;
    WCHAR buffer[128];
    int matches = 0;
    LCID lcid = MAKELCID( LangID, SORT_DEFAULT );  /* FIXME: handle sort order */

    if (PRIMARYLANGID(LangID) == LANG_NEUTRAL) return TRUE; /* continue search */

    /* first check exact name */
    if (data->win_name[0] &&
        GetLocaleInfoW( lcid, LOCALE_SNAME | LOCALE_NOUSEROVERRIDE,
                        buffer, sizeof(buffer)/sizeof(WCHAR) ))
    {
        if (!strcmpW( data->win_name, buffer ))
        {
            matches = 4;  /* everything matches */
            goto done;
        }
    }

    if (!GetLocaleInfoW( lcid, LOCALE_SISO639LANGNAME | LOCALE_NOUSEROVERRIDE,
                         buffer, sizeof(buffer)/sizeof(WCHAR) ))
        return TRUE;
    if (strcmpW( buffer, data->lang )) return TRUE;
    matches++;  /* language name matched */

    if (data->country)
    {
        if (GetLocaleInfoW( lcid, LOCALE_SISO3166CTRYNAME|LOCALE_NOUSEROVERRIDE,
                            buffer, sizeof(buffer)/sizeof(WCHAR) ))
        {
            if (strcmpW( buffer, data->country )) goto done;
            matches++;  /* country name matched */
        }
    }
    else  /* match default language */
    {
        if (SUBLANGID(LangID) == SUBLANG_DEFAULT) matches++;
    }

    if (data->codepage)
    {
        UINT unix_cp;
        if (GetLocaleInfoW( lcid, LOCALE_RETURN_NUMBER,
                            (LPWSTR)&unix_cp, sizeof(unix_cp)/sizeof(WCHAR) ))
        {
            if (unix_cp == data->codepage) matches++;
        }
    }

    /* FIXME: check sort order */

done:
    if (matches > data->matches)
    {
        data->lcid = lcid;
        data->matches = matches;
    }
    return (data->matches < 4);  /* no need to continue for perfect match */
}

/***********************************************************************
 *		parse_locale_name
 *
 * Parse a locale name into a struct locale_name, handling both Windows and Unix formats.
 * Unix format is: lang[_country][.charset][@modifier]
 * Windows format is: lang[-script][-country][_modifier]
 */
static void parse_locale_name( const WCHAR *str, struct locale_name *name )
{
    static const WCHAR sepW[] = {'-','_','.','@',0};
    static const WCHAR winsepW[] = {'-','_',0};
    static const WCHAR posixW[] = {'P','O','S','I','X',0};
    static const WCHAR cW[] = {'C',0};
    static const WCHAR latinW[] = {'l','a','t','i','n',0};
    static const WCHAR latnW[] = {'-','L','a','t','n',0};
    WCHAR *p;

    TRACE("%s\n", debugstr_w(str));

    name->country = name->charset = name->script = name->modifier = NULL;
    name->lcid = MAKELCID( MAKELANGID(LANG_ENGLISH,SUBLANG_DEFAULT), SORT_DEFAULT );
    name->matches = 0;
    name->codepage = 0;
    name->win_name[0] = 0;
    lstrcpynW( name->lang, str, sizeof(name->lang)/sizeof(WCHAR) );

    if (!*name->lang)
    {
        name->lcid = LOCALE_INVARIANT;
        name->matches = 4;
        return;
    }

    if (!(p = strpbrkW( name->lang, sepW )))
    {
        if (!strcmpW( name->lang, posixW ) || !strcmpW( name->lang, cW ))
        {
            name->matches = 4;  /* perfect match for default English lcid */
            return;
        }
        strcpyW( name->win_name, name->lang );
    }
    else if (*p == '-')  /* Windows format */
    {
        strcpyW( name->win_name, name->lang );
        *p++ = 0;
        name->country = p;
        if (!(p = strpbrkW( p, winsepW ))) goto done;
        if (*p == '-')
        {
            *p++ = 0;
            name->script = name->country;
            name->country = p;
            if (!(p = strpbrkW( p, winsepW ))) goto done;
        }
        *p++ = 0;
        name->modifier = p;
    }
    else  /* Unix format */
    {
        if (*p == '_')
        {
            *p++ = 0;
            name->country = p;
            p = strpbrkW( p, sepW + 2 );
        }
        if (p && *p == '.')
        {
            *p++ = 0;
            name->charset = p;
            p = strchrW( p, '@' );
        }
        if (p)
        {
            *p++ = 0;
            name->modifier = p;
        }

        if (name->charset)
            name->codepage = find_charset( name->charset );

        /* rebuild a Windows name if possible */

        if (name->charset) goto done;  /* can't specify charset in Windows format */
        if (name->modifier && strcmpW( name->modifier, latinW ))
            goto done;  /* only Latn script supported for now */
        strcpyW( name->win_name, name->lang );
        if (name->modifier) strcatW( name->win_name, latnW );
        if (name->country)
        {
            p = name->win_name + strlenW(name->win_name);
            *p++ = '-';
            strcpyW( p, name->country );
        }
    }
done:
    EnumResourceLanguagesW( kernel32_handle, (LPCWSTR)RT_STRING, (LPCWSTR)LOCALE_ILANGUAGE,
                            find_locale_id_callback, (LPARAM)name );
}


static inline INT adapt(INT delta, INT numpoints, BOOL firsttime)
{
    INT k;

    delta /= (firsttime ? DAMP : 2);
    delta += delta/numpoints;

    for(k=0; delta>((BASE-TMIN)*TMAX)/2; k+=BASE)
        delta /= BASE-TMIN;
    return k+((BASE-TMIN+1)*delta)/(delta+SKEW);
}

static inline unsigned short get_table_entry( const unsigned short *table, WCHAR ch )
{
    return table[table[table[ch >> 8] + ((ch >> 4) & 0x0f)] + (ch & 0xf)];
}

static BOOL get_dummy_preferred_ui_language( DWORD flags, ULONG *count, WCHAR *buffer, ULONG *size, int isuser )
{
    LCTYPE type;
    int lsize;

    FIXME("(0x%x %p %p %p) returning a dummy value (current locale)\n", flags, count, buffer, size);

    if (flags & MUI_LANGUAGE_ID)
        type = LOCALE_ILANGUAGE;
    else
        type = LOCALE_SNAME;

    if (isuser)
        lsize = GetLocaleInfoW(LOCALE_USER_DEFAULT, type, NULL, 0);
    else
        lsize = GetLocaleInfoW(LOCALE_SYSTEM_DEFAULT, type, NULL, 0);
    if (!lsize)
    {
        /* keep last error from callee */
        return FALSE;
    }
    lsize++;
    if (!*size)
    {
        *size = lsize;
        *count = 1;
        return TRUE;
    }

    if (lsize > *size)
    {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return FALSE;
    }

    if (isuser)
        if (!GetLocaleInfoW(LOCALE_USER_DEFAULT, type, buffer, *size))
        {
            /* keep last error from callee */
            return FALSE;
        }
    else
        if (!GetLocaleInfoW(LOCALE_SYSTEM_DEFAULT, type, buffer, *size))
        {
            /* keep last error from callee */
            return FALSE;
        }
    buffer[lsize-1] = 0;
    *size = lsize;
    *count = 1;
    TRACE("returned variable content: %d, \"%s\", %d\n", *count, debugstr_w(buffer), *size);
    return TRUE;

}

/***********************************************************************
 *           LocaleNameToLCID  (KERNEL32.@)
 */
LCID WINAPI LocaleNameToLCID( LPCWSTR name, DWORD flags )
{
    struct locale_name locale_name;
    static int once;

    if (flags && !once++)
        FIXME( "unsupported flags %x\n", flags );

    if (name == LOCALE_NAME_USER_DEFAULT)
        return GetUserDefaultLCID();

    /* string parsing */
    parse_locale_name( name, &locale_name );

    TRACE( "found lcid %x for %s, matches %d\n",
           locale_name.lcid, debugstr_w(name), locale_name.matches );

    if (!locale_name.matches)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if (locale_name.matches == 1)
        WARN( "locale %s not recognized, defaulting to %s\n",
              debugstr_w(name), debugstr_w(locale_name.lang) );

    return locale_name.lcid;
}


/***********************************************************************
 *           LCIDToLocaleName  (KERNEL32.@)
 */
INT WINAPI LCIDToLocaleName( LCID lcid, LPWSTR name, INT count, DWORD flags )
{
    static int once;
    if (flags && !once++) FIXME( "unsupported flags %x\n", flags );

    return GetLocaleInfoW( lcid, LOCALE_SNAME | LOCALE_NOUSEROVERRIDE, name, count );
}

/***********************************************************************
 *		GetSystemDefaultLocaleName (KERNEL32.@)
 */
INT WINAPI GetSystemDefaultLocaleName(LPWSTR localename, INT len)
{
    LCID lcid = GetSystemDefaultLCID();
    return LCIDToLocaleName(lcid, localename, len, 0);
}

/***********************************************************************
 *             GetSystemPreferredUILanguages (KERNEL32.@)
 */
BOOL WINAPI GetSystemPreferredUILanguages(DWORD flags, ULONG* count, WCHAR* buffer, ULONG* size)
{
    if (flags & ~(MUI_LANGUAGE_NAME | MUI_LANGUAGE_ID | MUI_MACHINE_LANGUAGE_SETTINGS))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if ((flags & MUI_LANGUAGE_NAME) && (flags & MUI_LANGUAGE_ID))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if (*size && !buffer)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    return get_dummy_preferred_ui_language( flags, count, buffer, size, 0 );
}

/***********************************************************************
 *             GetUserPreferredUILanguages (KERNEL32.@)
 */
BOOL WINAPI GetUserPreferredUILanguages(DWORD flags, ULONG* count, WCHAR* buffer, ULONG* size)
{
    if (flags & ~(MUI_LANGUAGE_NAME | MUI_LANGUAGE_ID | MUI_MACHINE_LANGUAGE_SETTINGS))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if ((flags & MUI_LANGUAGE_NAME) && (flags & MUI_LANGUAGE_ID))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if (*size && !buffer)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    return get_dummy_preferred_ui_language( flags, count, buffer, size, 1 );
}

/***********************************************************************
 *              SetThreadPreferredUILanguages (KERNEL32.@)
 */
BOOL WINAPI SetThreadPreferredUILanguages( DWORD flags, PCZZWSTR buffer, PULONG count )
{
    FIXME( "%u, %p, %p\n", flags, buffer, count );
    return TRUE;
}

/***********************************************************************
 *              GetThreadPreferredUILanguages (KERNEL32.@)
 */
BOOL WINAPI GetThreadPreferredUILanguages( DWORD flags, ULONG *count, WCHAR *buf, ULONG *size )
{
    FIXME( "%08x, %p, %p %p\n", flags, count, buf, size );
    return get_dummy_preferred_ui_language( flags, count, buf, size, 0 );
}

/*
 * Win32 threads
 *
 * Copyright 1996 Alexandre Julliard
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

/**********************************************************************
 * GetThreadId [KERNEL32.@]
 *
 * Retrieve the identifier of a thread.
 *
 * PARAMS
 *  Thread [I] The thread to retrieve the identifier of.
 *
 * RETURNS
 *    Success: Identifier of the target thread.
 *    Failure: 0
 */
DWORD WINAPI GetThreadId(HANDLE Thread)
{
    THREAD_BASIC_INFORMATION tbi;
    NTSTATUS status;

    TRACE("(%p)\n", Thread);

    status = NtQueryInformationThread(Thread, ThreadBasicInformation, &tbi,
                                      sizeof(tbi), NULL);
    if (status)
    {
        SetLastError( RtlNtStatusToDosError(status) );
        return 0;
    }

    return HandleToULong(tbi.ClientId.UniqueThread);
}

//Interlock
static inline int interlocked_cmpxchg( int *dest, int xchg, int compare )
{
    int ret;
    __asm__ __volatile__( "lock; cmpxchgl %2,(%1)"
                          : "=a" (ret) : "r" (dest), "r" (xchg), "0" (compare) : "memory" );
    return ret;
}

static inline void *interlocked_cmpxchg_ptr( void **dest, void *xchg, void *compare )
{
    void *ret;
#ifdef __x86_64__
    __asm__ __volatile__( "lock; cmpxchgq %2,(%1)"
                          : "=a" (ret) : "r" (dest), "r" (xchg), "0" (compare) : "memory" );
#else
    __asm__ __volatile__( "lock; cmpxchgl %2,(%1)"
                          : "=a" (ret) : "r" (dest), "r" (xchg), "0" (compare) : "memory" );
#endif
    return ret;
}

static inline int interlocked_xchg( int *dest, int val )
{
    int ret;
    __asm__ __volatile__( "lock; xchgl %0,(%1)"
                          : "=r" (ret) : "r" (dest), "0" (val) : "memory" );
    return ret;
}

static inline void *interlocked_xchg_ptr( void **dest, void *val )
{
    void *ret;
#ifdef __x86_64__
    __asm__ __volatile__( "lock; xchgq %0,(%1)"
                          : "=r" (ret) :"r" (dest), "0" (val) : "memory" );
#else
    __asm__ __volatile__( "lock; xchgl %0,(%1)"
                          : "=r" (ret) : "r" (dest), "0" (val) : "memory" );
#endif
    return ret;
}

static inline int interlocked_xchg_add( int *dest, int incr )
{
    int ret;
    __asm__ __volatile__( "lock; xaddl %0,(%1)"
                          : "=r" (ret) : "r" (dest), "0" (incr) : "memory" );
    return ret;
}

static inline int interlocked_dec_if_nonzero( int *dest )
{
    int val, tmp;
    for (val = *dest;; val = tmp)
    {
        if (!val || (tmp = interlocked_cmpxchg( dest, val - 1, val )) == val)
            break;
    }
    return val;
}

/* helper for kernel32->ntdll timeout format conversion */
static inline PLARGE_INTEGER get_nt_timeout( PLARGE_INTEGER pTime, DWORD timeout )
{
    if (timeout == INFINITE) return NULL;
    pTime->QuadPart = (ULONGLONG)timeout * -10000;
    return pTime;
}

/*
 *	Process synchronisation
 *
 * Copyright 1996, 1997, 1998 Marcus Meissner
 * Copyright 1997, 1999 Alexandre Julliard
 * Copyright 1999, 2000 Juergen Schmied
 * Copyright 2003 Eric Pouech
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

/***********************************************************************
 *           CreateEventExA    (KERNEL32.@)
 */
HANDLE WINAPI CreateEventExA( SECURITY_ATTRIBUTES *sa, LPCSTR name, DWORD flags, DWORD access )
{
    WCHAR buffer[MAX_PATH];

    if (!name) return __crtCreateEventExW( sa, NULL, flags, access );

    if (!MultiByteToWideChar( CP_ACP, 0, name, -1, buffer, MAX_PATH ))
    {
        SetLastError( ERROR_FILENAME_EXCED_RANGE );
        return 0;
    }
    return __crtCreateEventExW( sa, buffer, flags, access );
}

/***********************************************************************
 *           CreateSemaphoreExA   (KERNEL32.@)
 */
HANDLE WINAPI CreateSemaphoreExA( SECURITY_ATTRIBUTES *sa, LONG initial, LONG max,
                                                    LPCSTR name, DWORD flags, DWORD access )
{
    WCHAR buffer[MAX_PATH];

    if (!name) return __crtCreateSemaphoreExW( sa, initial, max, NULL, flags, access );

    if (!MultiByteToWideChar( CP_ACP, 0, name, -1, buffer, MAX_PATH ))
    {
        SetLastError( ERROR_FILENAME_EXCED_RANGE );
        return 0;
    }
    return __crtCreateSemaphoreExW( sa, initial, max, buffer, flags, access );
}

/***********************************************************************
 *           K32QueryWorkingSetEx (KERNEL32.@)
 */
BOOL WINAPI K32QueryWorkingSetEx( HANDLE process, LPVOID buffer, DWORD size )
{
    NTSTATUS status;

    TRACE( "(%p, %p, %d)\n", process, buffer, size );

    status = NtQueryVirtualMemory( process, NULL, MemoryWorkingSetList, buffer,  size, NULL );

    if (status)
    {
        SetLastError( RtlNtStatusToDosError( status ) );
        return FALSE;
    }
    return TRUE;
}

/******************************************************************
 *		QueryFullProcessImageNameW (KERNEL32.@)
 */
BOOL WINAPI QueryFullProcessImageNameW(HANDLE hProcess, DWORD dwFlags, LPWSTR lpExeName, PDWORD pdwSize)
{
    BYTE buffer[sizeof(UNICODE_STRING) + MAX_PATH*sizeof(WCHAR)];  /* this buffer should be enough */
    UNICODE_STRING *dynamic_buffer = NULL;
    UNICODE_STRING *result = NULL;
    NTSTATUS status;
    DWORD needed;

    /* FIXME: On Windows, ProcessImageFileName return an NT path. In Wine it
     * is a DOS path and we depend on this. */
    status = NtQueryInformationProcess(hProcess, ProcessImageFileName, buffer,
                                       sizeof(buffer) - sizeof(WCHAR), &needed);
    if (status == STATUS_INFO_LENGTH_MISMATCH)
    {
        dynamic_buffer = HeapAlloc(GetProcessHeap(), 0, needed + sizeof(WCHAR));
        status = NtQueryInformationProcess(hProcess, ProcessImageFileName, (LPBYTE)dynamic_buffer, needed, &needed);
        result = dynamic_buffer;
    }
    else
        result = (PUNICODE_STRING)buffer;

    if (status) goto cleanup;

    if (dwFlags & PROCESS_NAME_NATIVE)
    {
        WCHAR drive[3];
        WCHAR device[1024];
        DWORD ntlen, devlen;

        if (result->Buffer[1] != ':' || result->Buffer[0] < 'A' || result->Buffer[0] > 'Z')
        {
            /* We cannot convert it to an NT device path so fail */
            status = STATUS_NO_SUCH_DEVICE;
            goto cleanup;
        }

        /* Find this drive's NT device path */
        drive[0] = result->Buffer[0];
        drive[1] = ':';
        drive[2] = 0;
        if (!QueryDosDeviceW(drive, device, sizeof(device)/sizeof(*device)))
        {
            status = STATUS_NO_SUCH_DEVICE;
            goto cleanup;
        }

        devlen = lstrlenW(device);
        ntlen = devlen + (result->Length/sizeof(WCHAR) - 2);
        if (ntlen + 1 > *pdwSize)
        {
            status = STATUS_BUFFER_TOO_SMALL;
            goto cleanup;
        }
        *pdwSize = ntlen;

        memcpy(lpExeName, device, devlen * sizeof(*device));
        memcpy(lpExeName + devlen, result->Buffer + 2, result->Length - 2 * sizeof(WCHAR));
        lpExeName[*pdwSize] = 0;
        TRACE("NT path: %s\n", debugstr_w(lpExeName));
    }
    else
    {
        if (result->Length/sizeof(WCHAR) + 1 > *pdwSize)
        {
            status = STATUS_BUFFER_TOO_SMALL;
            goto cleanup;
        }

        *pdwSize = result->Length/sizeof(WCHAR);
        memcpy( lpExeName, result->Buffer, result->Length );
        lpExeName[*pdwSize] = 0;
    }

cleanup:
    HeapFree(GetProcessHeap(), 0, dynamic_buffer);
    if (status) SetLastError( RtlNtStatusToDosError(status) );
    return !status;
}

/******************************************************************
 *		QueryFullProcessImageNameA (KERNEL32.@)
 */
BOOL WINAPI QueryFullProcessImageNameA(HANDLE hProcess, DWORD dwFlags, LPSTR lpExeName, PDWORD pdwSize)
{
    BOOL retval;
    DWORD pdwSizeW = *pdwSize;
    LPWSTR lpExeNameW = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, *pdwSize * sizeof(WCHAR));

    retval = QueryFullProcessImageNameW(hProcess, dwFlags, lpExeNameW, &pdwSizeW);

    if(retval)
        retval = (0 != WideCharToMultiByte(CP_ACP, 0, lpExeNameW, -1,
                               lpExeName, *pdwSize, NULL, NULL));
    if(retval)
        *pdwSize = strlen(lpExeName);

    HeapFree(GetProcessHeap(), 0, lpExeNameW);
    return retval;
}

HANDLE keyed_event = NULL;

/* SRW locks implementation
 *
 * The memory layout used by the lock is:
 *
 * 32 31            16               0
 *  ________________ ________________
 * | X| #exclusive  |    #shared     |
 *  ���������������������������������
 * Since there is no space left for a separate counter of shared access
 * threads inside the locked section the #shared field is used for multiple
 * purposes. The following table lists all possible states the lock can be
 * in, notation: [X, #exclusive, #shared]:
 *
 * [0,   0,   N] -> locked by N shared access threads, if N=0 it's unlocked
 * [0, >=1, >=1] -> threads are requesting exclusive locks, but there are
 * still shared access threads inside. #shared should not be incremented
 * anymore!
 * [1, >=1, >=0] -> lock is owned by an exclusive thread and the #shared
 * counter can be used again to count the number of threads waiting in the
 * queue for shared access.
 *
 * the following states are invalid and will never occur:
 * [0, >=1,   0], [1,   0, >=0]
 *
 * The main problem arising from the fact that we have no separate counter
 * of shared access threads inside the locked section is that in the state
 * [0, >=1, >=1] above we cannot add additional waiting threads to the
 * shared access queue - it wouldn't be possible to distinguish waiting
 * threads and those that are still inside. To solve this problem the lock
 * uses the following approach: a thread that isn't able to allocate a
 * shared lock just uses the exclusive queue instead. As soon as the thread
 * is woken up it is in the state [1, >=1, >=0]. In this state it's again
 * possible to use the shared access queue. The thread atomically moves
 * itself to the shared access queue and releases the exclusive lock, so
 * that the "real" exclusive access threads have a chance. As soon as they
 * are all ready the shared access threads are processed.
 */

#define SRWLOCK_MASK_IN_EXCLUSIVE     0x80000000
#define SRWLOCK_MASK_EXCLUSIVE_QUEUE  0x7fff0000
#define SRWLOCK_MASK_SHARED_QUEUE     0x0000ffff
#define SRWLOCK_RES_EXCLUSIVE         0x00010000
#define SRWLOCK_RES_SHARED            0x00000001

#ifdef WORDS_BIGENDIAN
#define srwlock_key_exclusive(lock)   (&lock->Ptr)
#define srwlock_key_shared(lock)      ((void *)((char *)&lock->Ptr + 2))
#else
#define srwlock_key_exclusive(lock)   ((void *)((char *)&lock->Ptr + 2))
#define srwlock_key_shared(lock)      (&lock->Ptr)
#endif

static inline void srwlock_check_invalid( unsigned int val )
{
    /* Throw exception if it's impossible to acquire/release this lock. */
    if ((val & SRWLOCK_MASK_EXCLUSIVE_QUEUE) == SRWLOCK_MASK_EXCLUSIVE_QUEUE ||
            (val & SRWLOCK_MASK_SHARED_QUEUE) == SRWLOCK_MASK_SHARED_QUEUE)
        RtlRaiseStatus(STATUS_RESOURCE_NOT_OWNED);
}

static inline unsigned int srwlock_lock_exclusive( unsigned int *dest, int incr )
{
    unsigned int val, tmp;
    /* Atomically modifies the value of *dest by adding incr. If the shared
     * queue is empty and there are threads waiting for exclusive access, then
     * sets the mark SRWLOCK_MASK_IN_EXCLUSIVE to signal other threads that
     * they are allowed again to use the shared queue counter. */
    for (val = *dest;; val = tmp)
    {
        tmp = val + incr;
        srwlock_check_invalid( tmp );
        if ((tmp & SRWLOCK_MASK_EXCLUSIVE_QUEUE) && !(tmp & SRWLOCK_MASK_SHARED_QUEUE))
            tmp |= SRWLOCK_MASK_IN_EXCLUSIVE;
        if ((tmp = interlocked_cmpxchg( (int *)dest, tmp, val )) == val)
            break;
    }
    return val;
}

static inline unsigned int srwlock_unlock_exclusive( unsigned int *dest, int incr )
{
    unsigned int val, tmp;
    /* Atomically modifies the value of *dest by adding incr. If the queue of
     * threads waiting for exclusive access is empty, then remove the
     * SRWLOCK_MASK_IN_EXCLUSIVE flag (only the shared queue counter will
     * remain). */
    for (val = *dest;; val = tmp)
    {
        tmp = val + incr;
        srwlock_check_invalid( tmp );
        if (!(tmp & SRWLOCK_MASK_EXCLUSIVE_QUEUE))
            tmp &= SRWLOCK_MASK_SHARED_QUEUE;
        if ((tmp = interlocked_cmpxchg( (int *)dest, tmp, val )) == val)
            break;
    }
    return val;
}

static inline void srwlock_leave_exclusive( RTL_SRWLOCK *lock, unsigned int val )
{
    /* Used when a thread leaves an exclusive section. If there are other
     * exclusive access threads they are processed first, followed by
     * the shared waiters. */
    if (val & SRWLOCK_MASK_EXCLUSIVE_QUEUE)
        NtReleaseKeyedEvent( keyed_event, srwlock_key_exclusive(lock), FALSE, NULL );
    else
    {
        val &= SRWLOCK_MASK_SHARED_QUEUE; /* remove SRWLOCK_MASK_IN_EXCLUSIVE */
        while (val--)
            NtReleaseKeyedEvent( keyed_event, srwlock_key_shared(lock), FALSE, NULL );
    }
}

static inline void srwlock_leave_shared( RTL_SRWLOCK *lock, unsigned int val )
{
    /* Wake up one exclusive thread as soon as the last shared access thread
     * has left. */
    if ((val & SRWLOCK_MASK_EXCLUSIVE_QUEUE) && !(val & SRWLOCK_MASK_SHARED_QUEUE))
        NtReleaseKeyedEvent( keyed_event, srwlock_key_exclusive(lock), FALSE, NULL );
}

/***********************************************************************
 *              RtlInitializeSRWLock (NTDLL.@)
 *
 * NOTES
 *  Please note that SRWLocks do not keep track of the owner of a lock.
 *  It doesn't make any difference which thread for example unlocks an
 *  SRWLock (see corresponding tests). This implementation uses two
 *  keyed events (one for the exclusive waiters and one for the shared
 *  waiters) and is limited to 2^15-1 waiting threads.
 */
void WINAPI RtlInitializeSRWLock( RTL_SRWLOCK *lock )
{
    lock->Ptr = NULL;
}

/***********************************************************************
 *              RtlAcquireSRWLockExclusive (NTDLL.@)
 *
 * NOTES
 *  Unlike RtlAcquireResourceExclusive this function doesn't allow
 *  nested calls from the same thread. "Upgrading" a shared access lock
 *  to an exclusive access lock also doesn't seem to be supported.
 */
void WINAPI RtlAcquireSRWLockExclusive( RTL_SRWLOCK *lock )
{
    if (srwlock_lock_exclusive( (unsigned int *)&lock->Ptr, SRWLOCK_RES_EXCLUSIVE ))
        NtWaitForKeyedEvent( keyed_event, srwlock_key_exclusive(lock), FALSE, NULL );
}

/***********************************************************************
 *              RtlAcquireSRWLockShared (NTDLL.@)
 *
 * NOTES
 *   Do not call this function recursively - it will only succeed when
 *   there are no threads waiting for an exclusive lock!
 */
void WINAPI RtlAcquireSRWLockShared( RTL_SRWLOCK *lock )
{
    unsigned int val, tmp;
    /* Acquires a shared lock. If it's currently not possible to add elements to
     * the shared queue, then request exclusive access instead. */
    for (val = *(unsigned int *)&lock->Ptr;; val = tmp)
    {
        if ((val & SRWLOCK_MASK_EXCLUSIVE_QUEUE) && !(val & SRWLOCK_MASK_IN_EXCLUSIVE))
            tmp = val + SRWLOCK_RES_EXCLUSIVE;
        else
            tmp = val + SRWLOCK_RES_SHARED;
        if ((tmp = interlocked_cmpxchg( (int *)&lock->Ptr, tmp, val )) == val)
            break;
    }

    /* Drop exclusive access again and instead requeue for shared access. */
    if ((val & SRWLOCK_MASK_EXCLUSIVE_QUEUE) && !(val & SRWLOCK_MASK_IN_EXCLUSIVE))
    {
        NtWaitForKeyedEvent( keyed_event, srwlock_key_exclusive(lock), FALSE, NULL );
        val = srwlock_unlock_exclusive( (unsigned int *)&lock->Ptr, (SRWLOCK_RES_SHARED
                                        - SRWLOCK_RES_EXCLUSIVE) ) - SRWLOCK_RES_EXCLUSIVE;
        srwlock_leave_exclusive( lock, val );
    }

    if (val & SRWLOCK_MASK_EXCLUSIVE_QUEUE)
        NtWaitForKeyedEvent( keyed_event, srwlock_key_shared(lock), FALSE, NULL );
}

/***********************************************************************
 *              RtlReleaseSRWLockExclusive (NTDLL.@)
 */
void WINAPI RtlReleaseSRWLockExclusive( RTL_SRWLOCK *lock )
{
    srwlock_leave_exclusive( lock, srwlock_unlock_exclusive( (unsigned int *)&lock->Ptr,
                             - SRWLOCK_RES_EXCLUSIVE ) - SRWLOCK_RES_EXCLUSIVE );
}

/***********************************************************************
 *              RtlReleaseSRWLockShared (NTDLL.@)
 */
void WINAPI RtlReleaseSRWLockShared( RTL_SRWLOCK *lock )
{
    srwlock_leave_shared( lock, srwlock_lock_exclusive( (unsigned int *)&lock->Ptr,
                          - SRWLOCK_RES_SHARED ) - SRWLOCK_RES_SHARED );
}

/***********************************************************************
 *              RtlTryAcquireSRWLockExclusive (NTDLL.@)
 *
 * NOTES
 *  Similar to AcquireSRWLockExclusive recusive calls are not allowed
 *  and will fail with return value FALSE.
 */
BOOLEAN WINAPI RtlTryAcquireSRWLockExclusive( RTL_SRWLOCK *lock )
{
    return interlocked_cmpxchg( (int *)&lock->Ptr, SRWLOCK_MASK_IN_EXCLUSIVE |
                                SRWLOCK_RES_EXCLUSIVE, 0 ) == 0;
}

/***********************************************************************
 *              RtlTryAcquireSRWLockShared (NTDLL.@)
 */
BOOLEAN WINAPI RtlTryAcquireSRWLockShared( RTL_SRWLOCK *lock )
{
    unsigned int val, tmp;
    for (val = *(unsigned int *)&lock->Ptr;; val = tmp)
    {
        if (val & SRWLOCK_MASK_EXCLUSIVE_QUEUE)
            return FALSE;
        if ((tmp = interlocked_cmpxchg( (int *)&lock->Ptr, val + SRWLOCK_RES_SHARED, val )) == val)
            break;
    }
    return TRUE;
}

/***********************************************************************
 *           RtlInitializeConditionVariable   (NTDLL.@)
 *
 * Initializes the condition variable with NULL.
 *
 * PARAMS
 *  variable [O] condition variable
 *
 * RETURNS
 *  Nothing.

void WINAPI RtlInitializeConditionVariable( RTL_CONDITION_VARIABLE *variable )
{
    variable->Ptr = NULL;
}
 */
/******************************************************************
 *              RtlRunOnceInitialize (NTDLL.@)
 */
void WINAPI RtlRunOnceInitialize( RTL_RUN_ONCE *once )
{
    once->Ptr = NULL;
}

/******************************************************************
 *              RtlRunOnceBeginInitialize (NTDLL.@)
 */
DWORD WINAPI RtlRunOnceBeginInitialize( RTL_RUN_ONCE *once, ULONG flags, void **context )
{
    if (flags & RTL_RUN_ONCE_CHECK_ONLY)
    {
        ULONG_PTR val = (ULONG_PTR)once->Ptr;

        if (flags & RTL_RUN_ONCE_ASYNC) return STATUS_INVALID_PARAMETER;
        if ((val & 3) != 2) return STATUS_UNSUCCESSFUL;
        if (context) *context = (void *)(val & ~3);
        return STATUS_SUCCESS;
    }

    for (;;)
    {
        ULONG_PTR next, val = (ULONG_PTR)once->Ptr;

        switch (val & 3)
        {
        case 0:  /* first time */
            if (!interlocked_cmpxchg_ptr( &once->Ptr,
                                          (flags & RTL_RUN_ONCE_ASYNC) ? (void *)3 : (void *)1, 0 ))
                return STATUS_PENDING;
            break;

        case 1:  /* in progress, wait */
            if (flags & RTL_RUN_ONCE_ASYNC) return STATUS_INVALID_PARAMETER;
            next = val & ~3;
            if (interlocked_cmpxchg_ptr( &once->Ptr, (void *)((ULONG_PTR)&next | 1),
                                         (void *)val ) == (void *)val)
                NtWaitForKeyedEvent( keyed_event, &next, FALSE, NULL );
            break;

        case 2:  /* done */
            if (context) *context = (void *)(val & ~3);
            return STATUS_SUCCESS;

        case 3:  /* in progress, async */
            if (!(flags & RTL_RUN_ONCE_ASYNC)) return STATUS_INVALID_PARAMETER;
            return STATUS_PENDING;
        }
    }
}

/******************************************************************
 *              RtlRunOnceComplete (NTDLL.@)
 */
DWORD WINAPI RtlRunOnceComplete( RTL_RUN_ONCE *once, ULONG flags, void *context )
{
    if ((ULONG_PTR)context & 3) return STATUS_INVALID_PARAMETER;

    if (flags & RTL_RUN_ONCE_INIT_FAILED)
    {
        if (context) return STATUS_INVALID_PARAMETER;
        if (flags & RTL_RUN_ONCE_ASYNC) return STATUS_INVALID_PARAMETER;
    }
    else context = (void *)((ULONG_PTR)context | 2);

    for (;;)
    {
        ULONG_PTR val = (ULONG_PTR)once->Ptr;

        switch (val & 3)
        {
        case 1:  /* in progress */
            if (interlocked_cmpxchg_ptr( &once->Ptr, context, (void *)val ) != (void *)val) break;
            val &= ~3;
            while (val)
            {
                ULONG_PTR next = *(ULONG_PTR *)val;
                NtReleaseKeyedEvent( keyed_event, (void *)val, FALSE, NULL );
                val = next;
            }
            return STATUS_SUCCESS;

        case 3:  /* in progress, async */
            if (!(flags & RTL_RUN_ONCE_ASYNC)) return STATUS_INVALID_PARAMETER;
            if (interlocked_cmpxchg_ptr( &once->Ptr, context, (void *)val ) != (void *)val) break;
            return STATUS_SUCCESS;

        default:
            return STATUS_UNSUCCESSFUL;
        }
    }
}

/***********************************************************************
 *           RtlWakeConditionVariable   (NTDLL.@)
 *
 * Wakes up one thread waiting on the condition variable.
 *
 * PARAMS
 *  variable [I/O] condition variable to wake up.
 *
 * RETURNS
 *  Nothing.
 *
 * NOTES
 *  The calling thread does not have to own any lock in order to call
 *  this function.

void WINAPI RtlWakeConditionVariable( RTL_CONDITION_VARIABLE *variable )
{
    if (interlocked_dec_if_nonzero( (int *)&variable->Ptr ))
        NtReleaseKeyedEvent( keyed_event, &variable->Ptr, FALSE, NULL );
}
 */
/***********************************************************************
 *           RtlWakeAllConditionVariable   (NTDLL.@)
 *
 * See WakeConditionVariable, wakes up all waiting threads.

void WINAPI RtlWakeAllConditionVariable( RTL_CONDITION_VARIABLE *variable )
{
    int val = interlocked_xchg( (int *)&variable->Ptr, 0 );
    while (val-- > 0)
        NtReleaseKeyedEvent( keyed_event, &variable->Ptr, FALSE, NULL );
}
 */
/***********************************************************************
 *           RtlSleepConditionVariableCS   (NTDLL.@)
 *
 * Atomically releases the critical section and suspends the thread,
 * waiting for a Wake(All)ConditionVariable event. Afterwards it enters
 * the critical section again and returns.
 *
 * PARAMS
 *  variable  [I/O] condition variable
 *  crit      [I/O] critical section to leave temporarily
 *  timeout   [I]   timeout
 *
 * RETURNS
 *  see NtWaitForKeyedEvent for all possible return values.
 */
NTSTATUS WINAPI RtlSleepConditionVariableCS( RTL_CONDITION_VARIABLE *variable, RTL_CRITICAL_SECTION *crit,
                                             const LARGE_INTEGER *timeout )
{
    NTSTATUS status;
    interlocked_xchg_add( (int *)&variable->Ptr, 1 );
    RtlLeaveCriticalSection( crit );

    status = NtWaitForKeyedEvent( keyed_event, &variable->Ptr, FALSE, timeout );
    if (status != STATUS_SUCCESS)
    {
        if (!interlocked_dec_if_nonzero( (int *)&variable->Ptr ))
            status = NtWaitForKeyedEvent( keyed_event, &variable->Ptr, FALSE, NULL );
    }

    RtlEnterCriticalSection( crit );
    return status;
}

/***********************************************************************
 *           RtlSleepConditionVariableSRW   (NTDLL.@)
 *
 * Atomically releases the SRWLock and suspends the thread,
 * waiting for a Wake(All)ConditionVariable event. Afterwards it enters
 * the SRWLock again with the same access rights and returns.
 *
 * PARAMS
 *  variable  [I/O] condition variable
 *  lock      [I/O] SRWLock to leave temporarily
 *  timeout   [I]   timeout
 *  flags     [I]   type of the current lock (exclusive / shared)
 *
 * RETURNS
 *  see NtWaitForKeyedEvent for all possible return values.
 *
 * NOTES
 *  the behaviour is undefined if the thread doesn't own the lock.
 */
NTSTATUS WINAPI RtlSleepConditionVariableSRW( RTL_CONDITION_VARIABLE *variable, RTL_SRWLOCK *lock,
                                              const LARGE_INTEGER *timeout, ULONG flags )
{
    NTSTATUS status;
    interlocked_xchg_add( (int *)&variable->Ptr, 1 );

    if (flags & RTL_CONDITION_VARIABLE_LOCKMODE_SHARED)
        RtlReleaseSRWLockShared( lock );
    else
        RtlReleaseSRWLockExclusive( lock );

    status = NtWaitForKeyedEvent( keyed_event, &variable->Ptr, FALSE, timeout );
    if (status != STATUS_SUCCESS)
    {
        if (!interlocked_dec_if_nonzero( (int *)&variable->Ptr ))
            status = NtWaitForKeyedEvent( keyed_event, &variable->Ptr, FALSE, NULL );
    }

    if (flags & RTL_CONDITION_VARIABLE_LOCKMODE_SHARED)
        RtlAcquireSRWLockShared( lock );
    else
        RtlAcquireSRWLockExclusive( lock );
    return status;
}

/***********************************************************************
 *           InitOnceBeginInitialize    (KERNEL32.@)
 */
BOOL WINAPI InitOnceBeginInitialize( INIT_ONCE *once, DWORD flags, BOOL *pending, void **context )
{
    NTSTATUS status = RtlRunOnceBeginInitialize( once, flags, context );
    if (status >= 0) *pending = (status == STATUS_PENDING);
    else SetLastError( RtlNtStatusToDosError(status) );
    return status >= 0;
}

/***********************************************************************
 *           InitOnceComplete    (KERNEL32.@)
 */
BOOL WINAPI InitOnceComplete( INIT_ONCE *once, DWORD flags, void *context )
{
    NTSTATUS status = RtlRunOnceComplete( once, flags, context );
    if (status != STATUS_SUCCESS) SetLastError( RtlNtStatusToDosError(status) );
    return !status;
}

/***********************************************************************
 *           SleepConditionVariableCS   (KERNEL32.@)

BOOL WINAPI SleepConditionVariableCS( CONDITION_VARIABLE *variable, CRITICAL_SECTION *crit, DWORD timeout )
{
    NTSTATUS status;
    LARGE_INTEGER time;

    status = RtlSleepConditionVariableCS( variable, crit, get_nt_timeout( &time, timeout ) );

    if (status != STATUS_SUCCESS)
    {
        SetLastError( RtlNtStatusToDosError(status) );
        return FALSE;
    }
    return TRUE;
}
 */
/***********************************************************************
 *           SleepConditionVariableSRW   (KERNEL32.@)
 */
BOOL WINAPI SleepConditionVariableSRW( RTL_CONDITION_VARIABLE *variable, RTL_SRWLOCK *lock, DWORD timeout, ULONG flags )
{
    NTSTATUS status;
    LARGE_INTEGER time;

    status = RtlSleepConditionVariableSRW( variable, lock, get_nt_timeout( &time, timeout ), flags );

    if (status != STATUS_SUCCESS)
    {
        SetLastError( RtlNtStatusToDosError(status) );
        return FALSE;
    }
    return TRUE;
}

//typedef PVOID RTL_CONDITION_VARIABLE;
//typedef RTL_CONDITION_VARIABLE CONDITION_VARIABLE, *PCONDITION_VARIABLE;
//typedef CONDITION_VARIABLE pthread_cond_t;
typedef CRITICAL_SECTION pthread_mutex_t;
typedef struct win32_cond_t {
    pthread_mutex_t mtx_broadcast;
    pthread_mutex_t mtx_waiter_count;
    volatile int waiter_count;
    HANDLE semaphore;
    HANDLE waiters_done;
    volatile int is_broadcast;
} win32_cond_t;
//typedef CONDITION_VARIABLE pthread_cond_t;
typedef struct pthread_cond_t {
    void *Ptr;
} pthread_cond_t;

void WINAPI InitializeConditionVariable(pthread_cond_t *cond) {
    win32_cond_t *win32_cond = NULL;
    win32_cond = (win32_cond_t *)LocalAlloc(0x40,sizeof(win32_cond_t));
    if (!win32_cond)
        return;
    cond->Ptr = win32_cond;
    win32_cond->semaphore = CreateSemaphore(NULL, 0, 0x7fffffff, NULL);
    if (!win32_cond->semaphore)
        return;
    win32_cond->waiters_done = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!win32_cond->waiters_done)
        return;
    InitializeCriticalSection(&win32_cond->mtx_waiter_count);
    InitializeCriticalSection(&win32_cond->mtx_broadcast);
    return;
}

void WINAPI WakeAllConditionVariable(pthread_cond_t *cond) {
    win32_cond_t *win32_cond = (win32_cond_t *)cond->Ptr;
    int have_waiter;
    EnterCriticalSection(&win32_cond->mtx_broadcast);
    EnterCriticalSection(&win32_cond->mtx_waiter_count);
    have_waiter = 0;
    if (win32_cond->waiter_count) {
        win32_cond->is_broadcast = 1;
        have_waiter = 1;
    }
    if (have_waiter) {
        ReleaseSemaphore(win32_cond->semaphore, win32_cond->waiter_count, NULL);
        LeaveCriticalSection(&win32_cond->mtx_waiter_count);
        WaitForSingleObject(win32_cond->waiters_done, INFINITE);
        ResetEvent(win32_cond->waiters_done);
        win32_cond->is_broadcast = 0;
    } else
        LeaveCriticalSection(&win32_cond->mtx_waiter_count);
    LeaveCriticalSection(&win32_cond->mtx_broadcast);
}

void WINAPI WakeConditionVariable(pthread_cond_t *cond) {
    win32_cond_t *win32_cond = (win32_cond_t *)cond->Ptr;
    int have_waiter;
    EnterCriticalSection(&win32_cond->mtx_broadcast);
    EnterCriticalSection(&win32_cond->mtx_waiter_count);
    have_waiter = win32_cond->waiter_count;
    LeaveCriticalSection(&win32_cond->mtx_waiter_count);
    if (have_waiter) {
        ReleaseSemaphore(win32_cond->semaphore, 1, NULL);
        WaitForSingleObject(win32_cond->waiters_done, INFINITE);
        ResetEvent(win32_cond->waiters_done);
    }
    LeaveCriticalSection(&win32_cond->mtx_broadcast);
}

int WINAPI SleepConditionVariableCS (pthread_cond_t *cond, 
    pthread_mutex_t *mutex, int slp) {
    win32_cond_t *win32_cond = (win32_cond_t *)cond->Ptr;
    int last_waiter;
    EnterCriticalSection(&win32_cond->mtx_broadcast);
    EnterCriticalSection(&win32_cond->mtx_waiter_count);
    win32_cond->waiter_count++;
    LeaveCriticalSection(&win32_cond->mtx_waiter_count);
    LeaveCriticalSection(&win32_cond->mtx_broadcast);

    LeaveCriticalSection(mutex);
    int i = WaitForSingleObject(win32_cond->semaphore, slp);
    if(i)
        SetLastError(i);
    EnterCriticalSection(&win32_cond->mtx_waiter_count);
    win32_cond->waiter_count--;
    last_waiter = !win32_cond->waiter_count || !win32_cond->is_broadcast;
    LeaveCriticalSection(&win32_cond->mtx_waiter_count);
    if (last_waiter)
        SetEvent(win32_cond->waiters_done);
    EnterCriticalSection(mutex);
    return (BOOL)(i==0);
}