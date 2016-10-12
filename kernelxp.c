#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <winternl.h>
#include <ntstatus.h>
#include <tchar.h>
#include <winnls.h>

/*
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


// Dummy
VOID WINAPI DummyRaiseFailFastException(
  _In_opt_ PEXCEPTION_RECORD pExceptionRecord,
  _In_opt_ PCONTEXT          pContextRecord,
  _In_     DWORD             dwFlags
) {

}


/*
 * From WINE 1.9.11, once keep growing consider to link to wine dll
 */

#define strlenW wcslen
#define strcatW wcscat
#define strncmpiW _wcsnicmp
#define DECLSPEC_HIDDEN

//missing in mingw-w64 4.0 winternl
NTSYSAPI void      WINAPI RtlRaiseStatus(NTSTATUS);
NTSYSAPI NTSTATUS  WINAPI NtReleaseKeyedEvent(HANDLE,const void*,BOOLEAN,const LARGE_INTEGER*);
NTSYSAPI NTSTATUS  WINAPI NtWaitForKeyedEvent(HANDLE,const void*,BOOLEAN,const LARGE_INTEGER*);

//supress msg
#define TRACE(...) do { } while(0)
#define WARN(...) do { } while(0)
#define FIXME(...) do { } while(0)

//rename funcs
#define IdnToNameprepUnicode WineIdnToNameprepUnicode
#define IdnToUnicode WineIdnToUnicode
#define IdnToAscii WineIdnToAscii
#define GetFinalPathNameByHandleW WineGetFinalPathNameByHandleW
#define GetFinalPathNameByHandleA WineGetFinalPathNameByHandleA
#define RtlInitializeSRWLock WineRtlInitializeSRWLock
#define RtlAcquireSRWLockExclusive WineRtlAcquireSRWLockExclusive
#define RtlAcquireSRWLockShared WineRtlAcquireSRWLockShared
#define RtlReleaseSRWLockExclusive WineRtlReleaseSRWLockExclusive
#define RtlReleaseSRWLockShared WineRtlReleaseSRWLockShared
#define RtlTryAcquireSRWLockExclusive WineRtlTryAcquireSRWLockExclusive
#define RtlTryAcquireSRWLockShared WineRtlTryAcquireSRWLockShared
#define RtlInitializeConditionVariable WineRtlInitializeConditionVariable
#define RtlRunOnceInitialize WineRtlRunOnceInitialize

#ifdef WINE_IDN
#include "wineconsts.c"
static inline WCHAR tolowerW( WCHAR ch )
{
    extern const WCHAR wine_casemap_lower[];
    return ch + wine_casemap_lower[wine_casemap_lower[ch >> 8] + (ch & 0xff)];
}
#endif

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

/*
 * Locale support
 *
 * Copyright 1995 Martin von Loewis
 * Copyright 1998 David Lee Lambert
 * Copyright 2000 Julio César Gázquez
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

#ifdef WINE_IDN

enum {
    BASE = 36,
    TMIN = 1,
    TMAX = 26,
    SKEW = 38,
    DAMP = 700,
    INIT_BIAS = 72,
    INIT_N = 128
};

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

/******************************************************************************
 *           IdnToNameprepUnicode (KERNEL32.@)
 */
 
INT WINAPI IdnToNameprepUnicode(DWORD dwFlags, LPCWSTR lpUnicodeCharStr, INT cchUnicodeChar,
                                LPWSTR lpNameprepCharStr, INT cchNameprepChar)
{
    enum {
        UNASSIGNED = 0x1,
        PROHIBITED = 0x2,
        BIDI_RAL   = 0x4,
        BIDI_L     = 0x8
    };

    extern const unsigned short nameprep_char_type[] DECLSPEC_HIDDEN;
    extern const WCHAR nameprep_mapping[] DECLSPEC_HIDDEN;
    const WCHAR *ptr;
    WORD flags;
    WCHAR buf[64], *map_str, norm_str[64], ch;
    DWORD i, map_len, norm_len, mask, label_start, label_end, out = 0;
    BOOL have_bidi_ral, prohibit_bidi_ral, ascii_only;

    TRACE("%x %p %d %p %d\n", dwFlags, lpUnicodeCharStr, cchUnicodeChar,
        lpNameprepCharStr, cchNameprepChar);

    if(dwFlags & ~(IDN_ALLOW_UNASSIGNED|IDN_USE_STD3_ASCII_RULES)) {
        SetLastError(ERROR_INVALID_FLAGS);
        return 0;
    }

    if(!lpUnicodeCharStr || cchUnicodeChar<-1) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }

    if(cchUnicodeChar == -1)
        cchUnicodeChar = strlenW(lpUnicodeCharStr)+1;
    if(!cchUnicodeChar || (cchUnicodeChar==1 && lpUnicodeCharStr[0]==0)) {
        SetLastError(ERROR_INVALID_NAME);
        return 0;
    }

    for(label_start=0; label_start<cchUnicodeChar;) {
        ascii_only = TRUE;
        for(i=label_start; i<cchUnicodeChar; i++) {
            ch = lpUnicodeCharStr[i];

            if(i!=cchUnicodeChar-1 && !ch) {
                SetLastError(ERROR_INVALID_NAME);
                return 0;
            }
            /* check if ch is one of label separators defined in RFC3490 */
            if(!ch || ch=='.' || ch==0x3002 || ch==0xff0e || ch==0xff61)
                break;

            if(ch > 0x7f) {
                ascii_only = FALSE;
                continue;
            }

            if((dwFlags&IDN_USE_STD3_ASCII_RULES) == 0)
                continue;
            if((ch>='a' && ch<='z') || (ch>='A' && ch<='Z')
                    || (ch>='0' && ch<='9') || ch=='-')
                continue;

            SetLastError(ERROR_INVALID_NAME);
            return 0;
        }
        label_end = i;
        /* last label may be empty */
        if(label_start==label_end && ch) {
            SetLastError(ERROR_INVALID_NAME);
            return 0;
        }

        if((dwFlags&IDN_USE_STD3_ASCII_RULES) && (lpUnicodeCharStr[label_start]=='-' ||
                    lpUnicodeCharStr[label_end-1]=='-')) {
            SetLastError(ERROR_INVALID_NAME);
            return 0;
        }

        if(ascii_only) {
            /* maximal label length is 63 characters */
            if(label_end-label_start > 63) {
                SetLastError(ERROR_INVALID_NAME);
                return 0;
            }
            if(label_end < cchUnicodeChar)
                label_end++;

            if(!lpNameprepCharStr) {
                out += label_end-label_start;
            }else if(out+label_end-label_start <= cchNameprepChar) {
                memcpy(lpNameprepCharStr+out, lpUnicodeCharStr+label_start,
                        (label_end-label_start)*sizeof(WCHAR));
                if(lpUnicodeCharStr[label_end-1] > 0x7f)
                    lpNameprepCharStr[out+label_end-label_start-1] = '.';
                out += label_end-label_start;
            }else {
                SetLastError(ERROR_INSUFFICIENT_BUFFER);
                return 0;
            }

            label_start = label_end;
            continue;
        }

        map_len = 0;
        for(i=label_start; i<label_end; i++) {
            ch = lpUnicodeCharStr[i];
            ptr = nameprep_mapping + nameprep_mapping[ch>>8];
            ptr = nameprep_mapping + ptr[(ch>>4)&0x0f] + 3*(ch&0x0f);

            if(!ptr[0]) map_len++;
            else if(!ptr[1]) map_len++;
            else if(!ptr[2]) map_len += 2;
            else if(ptr[0]!=0xffff || ptr[1]!=0xffff || ptr[2]!=0xffff) map_len += 3;
        }
        if(map_len*sizeof(WCHAR) > sizeof(buf)) {
            map_str = HeapAlloc(GetProcessHeap(), 0, map_len*sizeof(WCHAR));
            if(!map_str) {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                return 0;
            }
        }else {
            map_str = buf;
        }
        map_len = 0;
        for(i=label_start; i<label_end; i++) {
            ch = lpUnicodeCharStr[i];
            ptr = nameprep_mapping + nameprep_mapping[ch>>8];
            ptr = nameprep_mapping + ptr[(ch>>4)&0x0f] + 3*(ch&0x0f);

            if(!ptr[0]) {
                map_str[map_len++] = ch;
            }else if(!ptr[1]) {
                map_str[map_len++] = ptr[0];
            }else if(!ptr[2]) {
                map_str[map_len++] = ptr[0];
                map_str[map_len++] = ptr[1];
            }else if(ptr[0]!=0xffff || ptr[1]!=0xffff || ptr[2]!=0xffff) {
                map_str[map_len++] = ptr[0];
                map_str[map_len++] = ptr[1];
                map_str[map_len++] = ptr[2];
            }
        }

        norm_len = FoldStringW(MAP_FOLDCZONE, map_str, map_len,
                norm_str, sizeof(norm_str)/sizeof(WCHAR)-1);
        if(map_str != buf)
            HeapFree(GetProcessHeap(), 0, map_str);
        if(!norm_len) {
            if(GetLastError() == ERROR_INSUFFICIENT_BUFFER)
                SetLastError(ERROR_INVALID_NAME);
            return 0;
        }

        if(label_end < cchUnicodeChar) {
            norm_str[norm_len++] = lpUnicodeCharStr[label_end] ? '.' : 0;
            label_end++;
        }

        if(!lpNameprepCharStr) {
            out += norm_len;
        }else if(out+norm_len <= cchNameprepChar) {
            memcpy(lpNameprepCharStr+out, norm_str, norm_len*sizeof(WCHAR));
            out += norm_len;
        }else {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return 0;
        }

        have_bidi_ral = prohibit_bidi_ral = FALSE;
        mask = PROHIBITED;
        if((dwFlags&IDN_ALLOW_UNASSIGNED) == 0)
            mask |= UNASSIGNED;
        for(i=0; i<norm_len; i++) {
            ch = norm_str[i];
            flags = get_table_entry( nameprep_char_type, ch );

            if(flags & mask) {
                SetLastError((flags & PROHIBITED) ? ERROR_INVALID_NAME
                        : ERROR_NO_UNICODE_TRANSLATION);
                return 0;
            }

            if(flags & BIDI_RAL)
                have_bidi_ral = TRUE;
            if(flags & BIDI_L)
                prohibit_bidi_ral = TRUE;
        }

        if(have_bidi_ral) {
            ch = norm_str[0];
            flags = get_table_entry( nameprep_char_type, ch );
            if((flags & BIDI_RAL) == 0)
                prohibit_bidi_ral = TRUE;

            ch = norm_str[norm_len-1];
            flags = get_table_entry( nameprep_char_type, ch );
            if((flags & BIDI_RAL) == 0)
                prohibit_bidi_ral = TRUE;
        }

        if(have_bidi_ral && prohibit_bidi_ral) {
            SetLastError(ERROR_INVALID_NAME);
            return 0;
        }

        label_start = label_end;
    }

    return out;
}

/******************************************************************************
 *           IdnToUnicode (KERNEL32.@)
 */
INT WINAPI IdnToUnicode(DWORD dwFlags, LPCWSTR lpASCIICharStr, INT cchASCIIChar,
                        LPWSTR lpUnicodeCharStr, INT cchUnicodeChar)
{
    extern const unsigned short nameprep_char_type[];

    INT i, label_start, label_end, out_label, out = 0;
    WCHAR ch;

    TRACE("%x %p %d %p %d\n", dwFlags, lpASCIICharStr, cchASCIIChar,
        lpUnicodeCharStr, cchUnicodeChar);

    for(label_start=0; label_start<cchASCIIChar;) {
        INT n = INIT_N, pos = 0, old_pos, w, k, bias = INIT_BIAS, delim=0, digit, t;

        out_label = out;
        for(i=label_start; i<cchASCIIChar; i++) {
            ch = lpASCIICharStr[i];

            if(ch>0x7f || (i!=cchASCIIChar-1 && !ch)) {
                SetLastError(ERROR_INVALID_NAME);
                return 0;
            }

            if(!ch || ch=='.')
                break;
            if(ch == '-')
                delim = i;

            if((dwFlags&IDN_USE_STD3_ASCII_RULES) == 0)
                continue;
            if((ch>='a' && ch<='z') || (ch>='A' && ch<='Z')
                    || (ch>='0' && ch<='9') || ch=='-')
                continue;

            SetLastError(ERROR_INVALID_NAME);
            return 0;
        }
        label_end = i;
        /* last label may be empty */
        if(label_start==label_end && ch) {
            SetLastError(ERROR_INVALID_NAME);
            return 0;
        }

        if((dwFlags&IDN_USE_STD3_ASCII_RULES) && (lpASCIICharStr[label_start]=='-' ||
                    lpASCIICharStr[label_end-1]=='-')) {
            SetLastError(ERROR_INVALID_NAME);
            return 0;
        }
        if(label_end-label_start > 63) {
            SetLastError(ERROR_INVALID_NAME);
            return 0;
        }

        if(label_end-label_start<4 ||
                tolowerW(lpASCIICharStr[label_start])!='x' ||
                tolowerW(lpASCIICharStr[label_start+1])!='n' ||
                lpASCIICharStr[label_start+2]!='-' || lpASCIICharStr[label_start+3]!='-') {
            if(label_end < cchASCIIChar)
                label_end++;

            if(!lpUnicodeCharStr) {
                out += label_end-label_start;
            }else if(out+label_end-label_start <= cchUnicodeChar) {
                memcpy(lpUnicodeCharStr+out, lpASCIICharStr+label_start,
                        (label_end-label_start)*sizeof(WCHAR));
                out += label_end-label_start;
            }else {
                SetLastError(ERROR_INSUFFICIENT_BUFFER);
                return 0;
            }

            label_start = label_end;
            continue;
        }

        if(delim == label_start+3)
            delim++;
        if(!lpUnicodeCharStr) {
            out += delim-label_start-4;
        }else if(out+delim-label_start-4 <= cchUnicodeChar) {
            memcpy(lpUnicodeCharStr+out, lpASCIICharStr+label_start+4,
                    (delim-label_start-4)*sizeof(WCHAR));
            out += delim-label_start-4;
        }else {
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return 0;
        }
        if(out != out_label)
            delim++;

        for(i=delim; i<label_end;) {
            old_pos = pos;
            w = 1;
            for(k=BASE; ; k+=BASE) {
                ch = i<label_end ? tolowerW(lpASCIICharStr[i++]) : 0;
                if((ch<'a' || ch>'z') && (ch<'0' || ch>'9')) {
                    SetLastError(ERROR_INVALID_NAME);
                    return 0;
                }
                digit = ch<='9' ? ch-'0'+'z'-'a'+1 : ch-'a';
                pos += digit*w;
                t = k<=bias ? TMIN : k>=bias+TMAX ? TMAX : k-bias;
                if(digit < t)
                    break;
                w *= BASE-t;
            }
            bias = adapt(pos-old_pos, out-out_label+1, old_pos==0);
            n += pos/(out-out_label+1);
            pos %= out-out_label+1;

            if((dwFlags&IDN_ALLOW_UNASSIGNED)==0 &&
                    get_table_entry(nameprep_char_type, n)==1/*UNASSIGNED*/) {
                SetLastError(ERROR_INVALID_NAME);
                return 0;
            }
            if(!lpUnicodeCharStr) {
                out++;
            }else if(out+1 <= cchASCIIChar) {
                memmove(lpUnicodeCharStr+out_label+pos+1,
                        lpUnicodeCharStr+out_label+pos,
                        (out-out_label-pos)*sizeof(WCHAR));
                lpUnicodeCharStr[out_label+pos] = n;
                out++;
            }else {
                SetLastError(ERROR_INSUFFICIENT_BUFFER);
                return 0;
            }
            pos++;
        }

        if(out-out_label > 63) {
            SetLastError(ERROR_INVALID_NAME);
            return 0;
        }

        if(label_end < cchASCIIChar) {
            if(!lpUnicodeCharStr) {
                out++;
            }else if(out+1 <= cchUnicodeChar) {
                lpUnicodeCharStr[out++] = lpASCIICharStr[label_end];
            }else {
                SetLastError(ERROR_INSUFFICIENT_BUFFER);
                return 0;
            }
        }
        label_start = label_end+1;
    }

    return out;
}

/******************************************************************************
 *           IdnToAscii (KERNEL32.@)
 * Implementation of Punycode based on RFC 3492.
 */
INT WINAPI IdnToAscii(DWORD dwFlags, LPCWSTR lpUnicodeCharStr, INT cchUnicodeChar,
                      LPWSTR lpASCIICharStr, INT cchASCIIChar)
{
    static const WCHAR prefixW[] = {'x','n','-','-'};

    WCHAR *norm_str;
    INT i, label_start, label_end, norm_len, out_label, out = 0;

    TRACE("%x %p %d %p %d\n", dwFlags, lpUnicodeCharStr, cchUnicodeChar,
        lpASCIICharStr, cchASCIIChar);

    norm_len = IdnToNameprepUnicode(dwFlags, lpUnicodeCharStr, cchUnicodeChar, NULL, 0);
    if(!norm_len)
        return 0;
    norm_str = HeapAlloc(GetProcessHeap(), 0, norm_len*sizeof(WCHAR));
    if(!norm_str) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return 0;
    }
    norm_len = IdnToNameprepUnicode(dwFlags, lpUnicodeCharStr,
            cchUnicodeChar, norm_str, norm_len);
    if(!norm_len) {
        HeapFree(GetProcessHeap(), 0, norm_str);
        return 0;
    }

    for(label_start=0; label_start<norm_len;) {
        INT n = INIT_N, bias = INIT_BIAS;
        INT delta = 0, b = 0, h;

        out_label = out;
        for(i=label_start; i<norm_len && norm_str[i]!='.' &&
                norm_str[i]!=0x3002 && norm_str[i]!='\0'; i++)
            if(norm_str[i] < 0x80)
                b++;
        label_end = i;

        if(b == label_end-label_start) {
            if(label_end < norm_len)
                b++;
            if(!lpASCIICharStr) {
                out += b;
            }else if(out+b <= cchASCIIChar) {
                memcpy(lpASCIICharStr+out, norm_str+label_start, b*sizeof(WCHAR));
                out += b;
            }else {
                HeapFree(GetProcessHeap(), 0, norm_str);
                SetLastError(ERROR_INSUFFICIENT_BUFFER);
                return 0;
            }
            label_start = label_end+1;
            continue;
        }

        if(!lpASCIICharStr) {
            out += 5+b; /* strlen(xn--...-) */
        }else if(out+5+b <= cchASCIIChar) {
            memcpy(lpASCIICharStr+out, prefixW, sizeof(prefixW));
            out += 4;
            for(i=label_start; i<label_end; i++)
                if(norm_str[i] < 0x80)
                    lpASCIICharStr[out++] = norm_str[i];
            lpASCIICharStr[out++] = '-';
        }else {
            HeapFree(GetProcessHeap(), 0, norm_str);
            SetLastError(ERROR_INSUFFICIENT_BUFFER);
            return 0;
        }
        if(!b)
            out--;

        for(h=b; h<label_end-label_start;) {
            INT m = 0xffff, q, k;

            for(i=label_start; i<label_end; i++) {
                if(norm_str[i]>=n && m>norm_str[i])
                    m = norm_str[i];
            }
            delta += (m-n)*(h+1);
            n = m;

            for(i=label_start; i<label_end; i++) {
                if(norm_str[i] < n) {
                    delta++;
                }else if(norm_str[i] == n) {
                    for(q=delta, k=BASE; ; k+=BASE) {
                        INT t = k<=bias ? TMIN : k>=bias+TMAX ? TMAX : k-bias;
                        INT disp = q<t ? q : t+(q-t)%(BASE-t);
                        if(!lpASCIICharStr) {
                            out++;
                        }else if(out+1 <= cchASCIIChar) {
                            lpASCIICharStr[out++] = disp<='z'-'a' ?
                                'a'+disp : '0'+disp-'z'+'a'-1;
                        }else {
                            HeapFree(GetProcessHeap(), 0, norm_str);
                            SetLastError(ERROR_INSUFFICIENT_BUFFER);
                            return 0;
                        }
                        if(q < t)
                            break;
                        q = (q-t)/(BASE-t);
                    }
                    bias = adapt(delta, h+1, h==b);
                    delta = 0;
                    h++;
                }
            }
            delta++;
            n++;
        }

        if(out-out_label > 63) {
            HeapFree(GetProcessHeap(), 0, norm_str);
            SetLastError(ERROR_INVALID_NAME);
            return 0;
        }

        if(label_end < norm_len) {
            if(!lpASCIICharStr) {
                out++;
            }else if(out+1 <= cchASCIIChar) {
                lpASCIICharStr[out++] = norm_str[label_end] ? '.' : 0;
            }else {
                HeapFree(GetProcessHeap(), 0, norm_str);
                SetLastError(ERROR_INSUFFICIENT_BUFFER);
                return 0;
            }
        }
        label_start = label_end+1;
    }

    HeapFree(GetProcessHeap(), 0, norm_str);
    return out;
}
#endif



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

HANDLE keyed_event = NULL;

/* SRW locks implementation
 *
 * The memory layout used by the lock is:
 *
 * 32 31            16               0
 *  ________________ ________________
 * | X| #exclusive  |    #shared     |
 *  ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
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
 */
void WINAPI RtlInitializeConditionVariable( RTL_CONDITION_VARIABLE *variable )
{
    variable->Ptr = NULL;
}

/******************************************************************
 *              RtlRunOnceInitialize (NTDLL.@)
 */
void WINAPI RtlRunOnceInitialize( RTL_RUN_ONCE *once )
{
    once->Ptr = NULL;
}
