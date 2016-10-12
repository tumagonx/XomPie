#define _WIN32_WINNT 0x0601
#include <windows.h>
#include <ntstatus.h>
#include <winternl.h>
#include <wchar.h>
#include <tchar.h>

/*
 * gcc -shared -Wl,--kill-at,--enable-stdcall-fixup,-s advapixp.def 
 * -o advapixp.dll advapixp.c -lmsvcrt -lntdll
 */
 
HINSTANCE hmodule = NULL;
HINSTANCE sysdll = NULL;
BOOL WINAPI DllMain(HINSTANCE hInst,DWORD reason,LPVOID lpvReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        hmodule = hInst;
        sysdll = LoadLibrary(_T("ADVAPI32.dll"));
        if (!sysdll) return FALSE;
    }
    if (reason == DLL_PROCESS_DETACH) FreeLibrary(sysdll);
    return TRUE;
}


/*
 * From WINE 1.9.11, once keep growing consider to link to wine dll
 */

#define __WINE_ALLOC_SIZE(x) __attribute__((__alloc_size__(x)))
//shouldn't hurt much, badly want to avoid tolowerW here
#define memicmpW wmemcmp

//supress msg
#define TRACE(...) do { } while(0)
#define WARN(...) do { } while(0)
#define FIXME(...) do { } while(0)

//rename funcs
#define RegGetValueW WineRegGetValueW
#define RegCopyTreeW WineRegCopyTreeW
#define RegCopyTreeA WineRegCopyTreeA
#define RegDeleteKeyExW WineRegDeleteKeyExW
#define RegDeleteKeyExA WineRegDeleteKeyExA

//missing winternl funcs in mingw-w64 4.0
NTSYSAPI NTSTATUS  WINAPI NtDeleteKey(HANDLE);
NTSYSAPI NTSTATUS  WINAPI NtCreateKey(PHANDLE,ACCESS_MASK,const OBJECT_ATTRIBUTES*,ULONG,const UNICODE_STRING*,ULONG,PULONG);
NTSYSAPI NTSTATUS  WINAPI NtOpenKey(PHANDLE,ACCESS_MASK,const OBJECT_ATTRIBUTES *);
NTSYSAPI NTSTATUS  WINAPI RtlOpenCurrentUser(ACCESS_MASK,PHANDLE);

/*
 * Registry management
 *
 * Copyright (C) 1999 Alexandre Julliard
 *
 * Based on misc/registry.c code
 * Copyright (C) 1996 Marcus Meissner
 * Copyright (C) 1998 Matthew Becker
 * Copyright (C) 1999 Sylvain St-Germain
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


/* heap allocation helpers */
static void *heap_alloc( size_t len ) __WINE_ALLOC_SIZE(1);
static inline void *heap_alloc( size_t len )
{
    return HeapAlloc( GetProcessHeap(), 0, len );
}

static void *heap_alloc_zero( size_t len ) __WINE_ALLOC_SIZE(1);
static inline void *heap_alloc_zero( size_t len )
{
    return HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, len );
}

static void *heap_realloc( void *mem, size_t len ) __WINE_ALLOC_SIZE(2);
static inline void *heap_realloc( void *mem, size_t len )
{
    return HeapReAlloc( GetProcessHeap(), 0, mem, len );
}

static inline BOOL heap_free( void *mem )
{
    return HeapFree( GetProcessHeap(), 0, mem );
}


#define HKEY_SPECIAL_ROOT_FIRST   HKEY_CLASSES_ROOT
#define HKEY_SPECIAL_ROOT_LAST    HKEY_DYN_DATA

static const WCHAR name_CLASSES_ROOT[] =
    {'\\','R','e','g','i','s','t','r','y','\\',
     'M','a','c','h','i','n','e','\\',
     'S','o','f','t','w','a','r','e','\\',
     'C','l','a','s','s','e','s',0};
static const WCHAR name_LOCAL_MACHINE[] =
    {'\\','R','e','g','i','s','t','r','y','\\',
     'M','a','c','h','i','n','e',0};
static const WCHAR name_USERS[] =
    {'\\','R','e','g','i','s','t','r','y','\\',
     'U','s','e','r',0};
static const WCHAR name_PERFORMANCE_DATA[] =
    {'\\','R','e','g','i','s','t','r','y','\\',
     'P','e','r','f','D','a','t','a',0};
static const WCHAR name_CURRENT_CONFIG[] =
    {'\\','R','e','g','i','s','t','r','y','\\',
     'M','a','c','h','i','n','e','\\',
     'S','y','s','t','e','m','\\',
     'C','u','r','r','e','n','t','C','o','n','t','r','o','l','S','e','t','\\',
     'H','a','r','d','w','a','r','e',' ','P','r','o','f','i','l','e','s','\\',
     'C','u','r','r','e','n','t',0};
static const WCHAR name_DYN_DATA[] =
    {'\\','R','e','g','i','s','t','r','y','\\',
     'D','y','n','D','a','t','a',0};

static const WCHAR * const root_key_names[] =
{
    name_CLASSES_ROOT,
    NULL,         /* HKEY_CURRENT_USER is determined dynamically */
    name_LOCAL_MACHINE,
    name_USERS,
    name_PERFORMANCE_DATA,
    name_CURRENT_CONFIG,
    name_DYN_DATA
};

#define NB_SPECIAL_ROOT_KEYS   (sizeof(root_key_names)/sizeof(root_key_names[0]))

static HKEY special_root_keys[NB_SPECIAL_ROOT_KEYS];
static BOOL hkcu_cache_disabled;

static const BOOL is_win64 = (sizeof(void *) > sizeof(int));

/* check if value type needs string conversion (Ansi<->Unicode) */
static inline BOOL is_string( DWORD type )
{
    return (type == REG_SZ) || (type == REG_EXPAND_SZ) || (type == REG_MULTI_SZ);
}

/* check if current version is NT or Win95 */
static inline BOOL is_version_nt(void)
{
    return !(GetVersion() & 0x80000000);
}

static BOOL is_wow6432node( const UNICODE_STRING *name )
{
    static const WCHAR wow6432nodeW[] = {'W','o','w','6','4','3','2','N','o','d','e'};

    return (name->Length == sizeof(wow6432nodeW) &&
            !memicmpW( name->Buffer, wow6432nodeW, sizeof(wow6432nodeW)/sizeof(WCHAR) ));
}

/* open the Wow6432Node subkey of the specified key */
static HANDLE open_wow6432node( HANDLE key )
{
    static const WCHAR wow6432nodeW[] = {'W','o','w','6','4','3','2','N','o','d','e',0};
    OBJECT_ATTRIBUTES attr;
    UNICODE_STRING nameW;
    HANDLE ret;

    attr.Length = sizeof(attr);
    attr.RootDirectory = key;
    attr.ObjectName = &nameW;
    attr.Attributes = 0;
    attr.SecurityDescriptor = NULL;
    attr.SecurityQualityOfService = NULL;
    RtlInitUnicodeString( &nameW, wow6432nodeW );
    if (NtOpenKey( &ret, MAXIMUM_ALLOWED, &attr )) ret = 0;
    return ret;
}

/* wrapper for NtCreateKey that creates the key recursively if necessary */
static NTSTATUS create_key( HKEY *retkey, ACCESS_MASK access, OBJECT_ATTRIBUTES *attr,
                            const UNICODE_STRING *class, ULONG options, PULONG dispos )
{
    BOOL force_wow32 = is_win64 && (access & KEY_WOW64_32KEY);
    NTSTATUS status = STATUS_OBJECT_NAME_NOT_FOUND;
    HANDLE subkey, root = attr->RootDirectory;

    if (!force_wow32) status = NtCreateKey( &subkey, access, attr, 0, class, options, dispos );

    if (status == STATUS_OBJECT_NAME_NOT_FOUND)
    {
        static const WCHAR registry_root[] = {'\\','R','e','g','i','s','t','r','y','\\'};
        WCHAR *buffer = attr->ObjectName->Buffer;
        DWORD attrs, pos = 0, i = 0, len = attr->ObjectName->Length / sizeof(WCHAR);
        UNICODE_STRING str;

        /* don't try to create registry root */
        if (!attr->RootDirectory && len > sizeof(registry_root)/sizeof(WCHAR) &&
            !memicmpW( buffer, registry_root, sizeof(registry_root)/sizeof(WCHAR)))
            i += sizeof(registry_root)/sizeof(WCHAR);

        while (i < len && buffer[i] != '\\') i++;
        if (i == len && !force_wow32) return status;

        attrs = attr->Attributes;
        attr->ObjectName = &str;

        for (;;)
        {
            str.Buffer = buffer + pos;
            str.Length = (i - pos) * sizeof(WCHAR);
            if (force_wow32 && pos)
            {
                if (is_wow6432node( &str )) force_wow32 = FALSE;
                else if ((subkey = open_wow6432node( attr->RootDirectory )))
                {
                    if (attr->RootDirectory != root) NtClose( attr->RootDirectory );
                    attr->RootDirectory = subkey;
                    force_wow32 = FALSE;
                }
            }
            if (i == len)
            {
                attr->Attributes = attrs;
                status = NtCreateKey( &subkey, access, attr, 0, class, options, dispos );
            }
            else
            {
                attr->Attributes = attrs & ~OBJ_OPENLINK;
                status = NtCreateKey( &subkey, access, attr, 0, class,
                                      options & ~REG_OPTION_CREATE_LINK, dispos );
            }
            if (attr->RootDirectory != root) NtClose( attr->RootDirectory );
            if (status) return status;
            if (i == len) break;
            attr->RootDirectory = subkey;
            while (i < len && buffer[i] == '\\') i++;
            pos = i;
            while (i < len && buffer[i] != '\\') i++;
        }
    }
    attr->RootDirectory = subkey;
    if (force_wow32 && (subkey = open_wow6432node( attr->RootDirectory )))
    {
        if (attr->RootDirectory != root) NtClose( attr->RootDirectory );
        attr->RootDirectory = subkey;
    }
    *retkey = attr->RootDirectory;
    return status;
}

/* create one of the HKEY_* special root keys */
static HKEY create_special_root_hkey( HKEY hkey, DWORD access )
{
    HKEY ret = 0;
    int idx = HandleToUlong(hkey) - HandleToUlong(HKEY_SPECIAL_ROOT_FIRST);

    if (HandleToUlong(hkey) == HandleToUlong(HKEY_CURRENT_USER))
    {
        if (RtlOpenCurrentUser( access, (HANDLE *)&hkey )) return 0;
        TRACE( "HKEY_CURRENT_USER -> %p\n", hkey );

        /* don't cache the key in the table if caching is disabled */
        if (hkcu_cache_disabled)
            return hkey;
    }
    else
    {
        OBJECT_ATTRIBUTES attr;
        UNICODE_STRING name;

        attr.Length = sizeof(attr);
        attr.RootDirectory = 0;
        attr.ObjectName = &name;
        attr.Attributes = 0;
        attr.SecurityDescriptor = NULL;
        attr.SecurityQualityOfService = NULL;
        RtlInitUnicodeString( &name, root_key_names[idx] );
        if (create_key( &hkey, access, &attr, NULL, 0, NULL )) return 0;
        TRACE( "%s -> %p\n", debugstr_w(attr.ObjectName->Buffer), hkey );
    }

    if (!(access & (KEY_WOW64_64KEY | KEY_WOW64_32KEY)))
    {
        if (!(ret = InterlockedCompareExchangePointer( (void **)&special_root_keys[idx], hkey, 0 )))
            ret = hkey;
        else
            NtClose( hkey );  /* somebody beat us to it */
    }
    else
        ret = hkey;
    return ret;
}

/* map the hkey from special root to normal key if necessary */
static inline HKEY get_special_root_hkey( HKEY hkey, REGSAM access )
{
    HKEY ret = hkey;

    if ((HandleToUlong(hkey) >= HandleToUlong(HKEY_SPECIAL_ROOT_FIRST))
            && (HandleToUlong(hkey) <= HandleToUlong(HKEY_SPECIAL_ROOT_LAST)))
    {
        REGSAM mask = 0;

        if (HandleToUlong(hkey) == HandleToUlong(HKEY_CLASSES_ROOT))
            mask = KEY_WOW64_32KEY | KEY_WOW64_64KEY;

        if ((access & mask) ||
                !(ret = special_root_keys[HandleToUlong(hkey) - HandleToUlong(HKEY_SPECIAL_ROOT_FIRST)]))
            ret = create_special_root_hkey( hkey, MAXIMUM_ALLOWED | (access & mask) );
    }
    return ret;
}

/******************************************************************************
 * ADVAPI_ApplyRestrictions   [internal]
 *
 * Helper function for RegGetValueA/W.
 */
static VOID ADVAPI_ApplyRestrictions( DWORD dwFlags, DWORD dwType,
                                      DWORD cbData, PLONG ret )
{
    /* Check if the type is restricted by the passed flags */
    if (*ret == ERROR_SUCCESS || *ret == ERROR_MORE_DATA)
    {
        DWORD dwMask = 0;

        switch (dwType)
        {
        case REG_NONE: dwMask = RRF_RT_REG_NONE; break;
        case REG_SZ: dwMask = RRF_RT_REG_SZ; break;
        case REG_EXPAND_SZ: dwMask = RRF_RT_REG_EXPAND_SZ; break;
        case REG_MULTI_SZ: dwMask = RRF_RT_REG_MULTI_SZ; break;
        case REG_BINARY: dwMask = RRF_RT_REG_BINARY; break;
        case REG_DWORD: dwMask = RRF_RT_REG_DWORD; break;
        case REG_QWORD: dwMask = RRF_RT_REG_QWORD; break;
        }

        if (dwFlags & dwMask)
        {
            /* Type is not restricted, check for size mismatch */
            if (dwType == REG_BINARY)
            {
                DWORD cbExpect = 0;

                if ((dwFlags & RRF_RT_ANY) == RRF_RT_DWORD)
                    cbExpect = 4;
                else if ((dwFlags & RRF_RT_ANY) == RRF_RT_QWORD)
                    cbExpect = 8;

                if (cbExpect && cbData != cbExpect)
                    *ret = ERROR_DATATYPE_MISMATCH;
            }
        }
        else *ret = ERROR_UNSUPPORTED_TYPE;
    }
}


/******************************************************************************
 * RegCopyTreeW [ADVAPI32.@]
 *
 */
LONG WINAPI RegCopyTreeW( HKEY hsrc, const WCHAR *subkey, HKEY hdst )
{
    DWORD name_size, max_name;
    DWORD value_size, max_value;
    DWORD max_subkey, i, type;
    WCHAR *name_buf = NULL;
    BYTE *value_buf = NULL;
    HKEY hkey;
    LONG ret;

    TRACE( "(%p, %s, %p)\n", hsrc, debugstr_w(subkey), hdst );

    if (subkey)
    {
        ret = RegOpenKeyExW( hsrc, subkey, 0, KEY_READ, &hsrc );
        if (ret) return ret;
    }

    ret = RegQueryInfoKeyW( hsrc, NULL, NULL, NULL, NULL, &max_subkey,
                            NULL, NULL, &max_name, &max_value, NULL, NULL );
    if (ret)
        goto cleanup;

    max_name = max( max_subkey, max_name ) + 1;
    if (!(name_buf = heap_alloc( max_name * sizeof(WCHAR) )))
    {
        ret = ERROR_NOT_ENOUGH_MEMORY;
        goto cleanup;
    }

    if (!(value_buf = heap_alloc( max_value )))
    {
        ret = ERROR_NOT_ENOUGH_MEMORY;
        goto cleanup;
    }

    /* Copy values */
    for (i = 0;; i++)
    {
        name_size = max_name;
        value_size = max_value;
        ret = RegEnumValueW( hsrc, i, name_buf, &name_size, NULL, &type, value_buf, &value_size );
        if (ret == ERROR_NO_MORE_ITEMS) break;
        if (ret) goto cleanup;
        ret = RegSetValueExW( hdst, name_buf, 0, type, value_buf, value_size );
        if (ret) goto cleanup;
    }

    /* Recursively copy subkeys */
    for (i = 0;; i++)
    {
        name_size = max_name;
        ret = RegEnumKeyExW( hsrc, i, name_buf, &name_size, NULL, NULL, NULL, NULL );
        if (ret == ERROR_NO_MORE_ITEMS) break;
        if (ret) goto cleanup;
        ret = RegCreateKeyExW( hdst, name_buf, 0, NULL, 0, KEY_WRITE, NULL, &hkey, NULL );
        if (ret) goto cleanup;
        ret = RegCopyTreeW( hsrc, name_buf, hkey );
        RegCloseKey( hkey );
        if (ret) goto cleanup;
    }

    ret = ERROR_SUCCESS;

cleanup:
    heap_free( name_buf );
    heap_free( value_buf );
    if (subkey)
        RegCloseKey( hsrc );
    return ret;
}


/******************************************************************************
 * RegCopyTreeA [ADVAPI32.@]
 *
 */
LONG WINAPI RegCopyTreeA( HKEY hsrc, const char *subkey, HKEY hdst )
{
    UNICODE_STRING subkeyW;
    LONG ret;

    if (subkey) RtlCreateUnicodeStringFromAsciiz( &subkeyW, subkey );
    else subkeyW.Buffer = NULL;
    ret = RegCopyTreeW( hsrc, subkeyW.Buffer, hdst );
    RtlFreeUnicodeString( &subkeyW );
    return ret;
}


/******************************************************************************
 * RegDeleteKeyExW   [ADVAPI32.@]
 */
LSTATUS WINAPI RegDeleteKeyExW( HKEY hkey, LPCWSTR name, REGSAM access, DWORD reserved )
{
    DWORD ret;
    HKEY tmp;

    if (!name) return ERROR_INVALID_PARAMETER;

    if (!(hkey = get_special_root_hkey( hkey, access ))) return ERROR_INVALID_HANDLE;

    access &= KEY_WOW64_64KEY | KEY_WOW64_32KEY;
    if (!(ret = RegOpenKeyExW( hkey, name, 0, access | DELETE, &tmp )))
    {
        ret = RtlNtStatusToDosError( NtDeleteKey( tmp ) );
        RegCloseKey( tmp );
    }
    TRACE("%s ret=%08x\n", debugstr_w(name), ret);
    return ret;
}

/******************************************************************************
 * RegDeleteKeyExA   [ADVAPI32.@]
 */
LSTATUS WINAPI RegDeleteKeyExA( HKEY hkey, LPCSTR name, REGSAM access, DWORD reserved )
{
    DWORD ret;
    HKEY tmp;

    if (!name) return ERROR_INVALID_PARAMETER;

    if (!(hkey = get_special_root_hkey( hkey, access ))) return ERROR_INVALID_HANDLE;

    access &= KEY_WOW64_64KEY | KEY_WOW64_32KEY;
    if (!(ret = RegOpenKeyExA( hkey, name, 0, access | DELETE, &tmp )))
    {
        if (!is_version_nt()) /* win95 does recursive key deletes */
        {
            CHAR sub[MAX_PATH];

            while(!RegEnumKeyA(tmp, 0, sub, sizeof(sub)))
            {
                if(RegDeleteKeyExA(tmp, sub, access, reserved))  /* recurse */
                    break;
            }
        }
        ret = RtlNtStatusToDosError( NtDeleteKey( tmp ) );
        RegCloseKey( tmp );
    }
    TRACE("%s ret=%08x\n", debugstr_a(name), ret);
    return ret;
}


/******************************************************************************
 * RegGetValueW   [ADVAPI32.@]
 *
 * Retrieves the type and data for a value name associated with a key,
 * optionally expanding its content and restricting its type.
 *
 * PARAMS
 *  hKey      [I] Handle to an open key.
 *  pszSubKey [I] Name of the subkey of hKey.
 *  pszValue  [I] Name of value under hKey/szSubKey to query.
 *  dwFlags   [I] Flags restricting the value type to retrieve.
 *  pdwType   [O] Destination for the values type, may be NULL.
 *  pvData    [O] Destination for the values content, may be NULL.
 *  pcbData   [I/O] Size of pvData, updated with the size in bytes required to
 *                  retrieve the whole content, including the trailing '\0'
 *                  for strings.
 *
 * RETURNS
 *  Success: ERROR_SUCCESS
 *  Failure: nonzero error code from Winerror.h
 *
 * NOTES
 *  - Unless RRF_NOEXPAND is specified, REG_EXPAND_SZ values are automatically
 *    expanded and pdwType is set to REG_SZ instead.
 *  - Restrictions are applied after expanding, using RRF_RT_REG_EXPAND_SZ 
 *    without RRF_NOEXPAND is thus not allowed.
 *    An exception is the case where RRF_RT_ANY is specified, because then
 *    RRF_NOEXPAND is allowed.
 */
LSTATUS WINAPI RegGetValueW( HKEY hKey, LPCWSTR pszSubKey, LPCWSTR pszValue,
                          DWORD dwFlags, LPDWORD pdwType, PVOID pvData,
                          LPDWORD pcbData )
{
    DWORD dwType, cbData = pcbData ? *pcbData : 0;
    PVOID pvBuf = NULL;
    LONG ret;

    TRACE("(%p,%s,%s,%d,%p,%p,%p=%d)\n", 
          hKey, debugstr_w(pszSubKey), debugstr_w(pszValue), dwFlags, pdwType,
          pvData, pcbData, cbData);

    if (pvData && !pcbData)
        return ERROR_INVALID_PARAMETER;
    if ((dwFlags & RRF_RT_REG_EXPAND_SZ) && !(dwFlags & RRF_NOEXPAND) &&
            ((dwFlags & RRF_RT_ANY) != RRF_RT_ANY))
        return ERROR_INVALID_PARAMETER;

    if (pszSubKey && pszSubKey[0])
    {
        ret = RegOpenKeyExW(hKey, pszSubKey, 0, KEY_QUERY_VALUE, &hKey);
        if (ret != ERROR_SUCCESS) return ret;
    }

    ret = RegQueryValueExW(hKey, pszValue, NULL, &dwType, pvData, &cbData);
    
    /* If we are going to expand we need to read in the whole the value even
     * if the passed buffer was too small as the expanded string might be
     * smaller than the unexpanded one and could fit into cbData bytes. */
    if ((ret == ERROR_SUCCESS || ret == ERROR_MORE_DATA) &&
        dwType == REG_EXPAND_SZ && !(dwFlags & RRF_NOEXPAND))
    {
        do {
            heap_free(pvBuf);
            
            pvBuf = heap_alloc(cbData);
            if (!pvBuf)
            {
                ret = ERROR_NOT_ENOUGH_MEMORY;
                break;
            }

            if (ret == ERROR_MORE_DATA || !pvData)
                ret = RegQueryValueExW(hKey, pszValue, NULL, 
                                       &dwType, pvBuf, &cbData);
            else
            {
                /* Even if cbData was large enough we have to copy the 
                 * string since ExpandEnvironmentStrings can't handle
                 * overlapping buffers. */
                CopyMemory(pvBuf, pvData, cbData);
            }

            /* Both the type or the value itself could have been modified in
             * between so we have to keep retrying until the buffer is large
             * enough or we no longer have to expand the value. */
        } while (dwType == REG_EXPAND_SZ && ret == ERROR_MORE_DATA);

        if (ret == ERROR_SUCCESS)
        {
            /* Recheck dwType in case it changed since the first call */
            if (dwType == REG_EXPAND_SZ)
            {
                cbData = ExpandEnvironmentStringsW(pvBuf, pvData,
                                                   pcbData ? *pcbData : 0) * sizeof(WCHAR);
                dwType = REG_SZ;
                if(pvData && pcbData && cbData > *pcbData)
                    ret = ERROR_MORE_DATA;
            }
            else if (pvData)
                CopyMemory(pvData, pvBuf, *pcbData);
        }

        heap_free(pvBuf);
    }

    if (pszSubKey && pszSubKey[0])
        RegCloseKey(hKey);

    ADVAPI_ApplyRestrictions(dwFlags, dwType, cbData, &ret);

    if (pvData && ret != ERROR_SUCCESS && (dwFlags & RRF_ZEROONFAILURE))
        ZeroMemory(pvData, *pcbData);

    if (pdwType) *pdwType = dwType;
    if (pcbData) *pcbData = cbData;

    return ret;
}

