#include <winsock2.h>
#include <ws2tcpip.h>
#include <errno.h>
#include <tchar.h>

/*
 * gcc -shared -Wl,--kill-at,--enable-stdcall-fixup,-s -D_UNICODE -DUNICODE
 * -O ws2_xp.def -o ws2_xp.dll ws2_xp.c -lws2_32
 */
 
HINSTANCE hmodule = NULL;
HINSTANCE winsock = NULL;
BOOL WINAPI DllMain(HINSTANCE hInst,DWORD reason,LPVOID lpvReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        hmodule = hInst;
        winsock = LoadLibrary(_T("WS2_32.dll"));
        if (!winsock) return FALSE;
    }
    if (reason == DLL_PROCESS_DETACH) FreeLibrary(winsock);
    return TRUE;
}

/*
 * Copyright (c) 1995, 1996, 1997 Kungliga Tekniska Högskolan
 * (Royal Institute of Technology, Stockholm, Sweden).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
 
static const char *
inet_ntop_v4 (const void *src, char *dst, size_t size)
{
    const char digits[] = "0123456789";
    int i;
    struct in_addr *addr = (struct in_addr *)src;
    u_long a = ntohl(addr->s_addr);
    const char *orig_dst = dst;

    if (size < INET_ADDRSTRLEN) {
        errno = ENOSPC;
        return NULL;
    }
    for (i = 0; i < 4; ++i) {
    int n = (a >> (24 - i * 8)) & 0xFF;
    int non_zerop = 0;

    if (non_zerop || n / 100 > 0) {
        *dst++ = digits[n / 100];
        n %= 100;
        non_zerop = 1;
    }
    if (non_zerop || n / 10 > 0) {
        *dst++ = digits[n / 10];
        n %= 10;
        non_zerop = 1;
    }
    *dst++ = digits[n];
    if (i != 3)
        *dst++ = '.';
    }
    *dst++ = '\0';
    return orig_dst;
}

static const char *
inet_ntop_v6 (const void *src, char *dst, size_t size)
{
    const char xdigits[] = "0123456789abcdef";
    int i;
    const struct in6_addr *addr = (struct in6_addr *)src;
    const u_char *ptr = addr->s6_addr;
    const char *orig_dst = dst;
    int compressed = 0;

    if (size < INET6_ADDRSTRLEN) {
        errno = ENOSPC;
        return NULL;
    }
    for (i = 0; i < 8; ++i) {
    int non_zerop = 0;

        if (compressed == 0 &&
            ptr[0] == 0 && ptr[1] == 0 &&
            i <= 5 &&
            ptr[2] == 0 && ptr[3] == 0 &&
            ptr[4] == 0 && ptr[5] == 0) {

            compressed = 1;

            if (i == 0)
                *dst++ = ':';
            *dst++ = ':';

            for (ptr += 6, i += 3;
                 i < 8 && ptr[0] == 0 && ptr[1] == 0;
                 ++i, ptr += 2);

            if (i >= 8)
                break;
        }

    if (non_zerop || (ptr[0] >> 4)) {
        *dst++ = xdigits[ptr[0] >> 4];
        non_zerop = 1;
    }
    if (non_zerop || (ptr[0] & 0x0F)) {
        *dst++ = xdigits[ptr[0] & 0x0F];
        non_zerop = 1;
    }
    if (non_zerop || (ptr[1] >> 4)) {
        *dst++ = xdigits[ptr[1] >> 4];
        non_zerop = 1;
    }
    *dst++ = xdigits[ptr[1] & 0x0F];
    if (i != 7)
        *dst++ = ':';
    ptr += 2;
    }
    *dst++ = '\0';
    return orig_dst;
}

//const char * inet_ntop(int af, const void *src, char *dst, size_t size)
LPCSTR WINAPI inet_ntop(INT af, PVOID src, LPSTR dst, size_t size)
{
    LPCSTR pdst;
    if (!dst)
    {
        SetLastError( STATUS_INVALID_PARAMETER );
        return NULL;
    }
    switch (af) {
    case AF_INET :
        pdst = inet_ntop_v4 (src, dst, size);

    case AF_INET6 :
        pdst = inet_ntop_v6 (src, dst, size);

    default:
        WSASetLastError( WSAEAFNOSUPPORT );
        return NULL;
    }

    if (!pdst) SetLastError( STATUS_INVALID_PARAMETER );
    return pdst;
}

INT WINAPI inet_pton(INT af, LPCSTR csrc, PVOID dst)
{
    char * src;

    if (!dst || !csrc)
    {
        WSASetLastError(WSAEFAULT);
        return SOCKET_ERROR;
    }

    if (csrc == NULL || (src = strdup(csrc)) == NULL) {
        SetLastError( ENOMEM );
        return 0;
    }

    switch (af) {
    case AF_INET:
    {
        struct sockaddr_in  si4;
        INT r;
        INT s = sizeof(si4);

        si4.sin_family = AF_INET;
        r = WSAStringToAddressA(src, AF_INET, NULL, (LPSOCKADDR) &si4, &s);
        free(src);
        src = NULL;

        if (r == 0) {
            memcpy(dst, &si4.sin_addr, sizeof(si4.sin_addr));
            return 1;
        }
    }
    break;

    case AF_INET6:
    {
        struct sockaddr_in6 si6;
        INT r;
        INT s = sizeof(si6);

        si6.sin6_family = AF_INET6;
        r = WSAStringToAddressA(src, AF_INET6, NULL, (LPSOCKADDR) &si6, &s);
        free(src);
        src = NULL;

        if (r == 0) {
            memcpy(dst, &si6.sin6_addr, sizeof(si6.sin6_addr));
            return 1;
        }
    }
    break;

    default:
        WSASetLastError( WSAEAFNOSUPPORT );
        return SOCKET_ERROR;
    }

    /* the call failed */
    {
    int le = WSAGetLastError();

    if (le == WSAEINVAL)
        return SOCKET_ERROR;

    WSASetLastError(le);
    return -1;
    }
}

/*
 * based on Windows Sockets 1.1 specs
 *
 * Copyright (C) 1993,1994,1996,1997 John Brezak, Erik Bos, Alex Korobka.
 * Copyright (C) 2001 Stefan Leichter
 * Copyright (C) 2004 Hans Leidekker
 * Copyright (C) 2005 Marcus Meissner
 * Copyright (C) 2006-2008 Kai Blin
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
 *
 * NOTE: If you make any changes to fix a particular app, make sure
 * they don't break something else like Netscape or telnet and ftp
 * clients and servers (www.winsite.com got a lot of those).
 */

/***********************************************************************
*              InetPtonW                      (WS2_32.@)
*/
INT WINAPI InetPtonW(INT family, LPCWSTR addr, PVOID buffer)
{
    char *addrA;
    int len;
    INT ret;

    if (!addr)
    {
        WSASetLastError(WSAEFAULT);
        return SOCKET_ERROR;
    }

    len = WideCharToMultiByte(CP_ACP, 0, addr, -1, NULL, 0, NULL, NULL);
    if (!(addrA = HeapAlloc(GetProcessHeap(), 0, len)))
    {
        WSASetLastError(WSA_NOT_ENOUGH_MEMORY);
        return SOCKET_ERROR;
    }
    WideCharToMultiByte(CP_ACP, 0, addr, -1, addrA, len, NULL, NULL);

    ret = inet_pton(family, addrA, buffer);

    HeapFree(GetProcessHeap(), 0, addrA);
    return ret;
}

/***********************************************************************
*              InetNtopW                      (WS2_32.@)
*/
LPCWSTR WINAPI InetNtopW(INT family, PVOID addr, LPCWSTR buffer, size_t size)
{
// FIXME should fix inet_ntop_v4/inet_ntop_v6 to be wchar variant, not like this
    LPSTR bufferA;
    LPCSTR retA;
    int len;
    LPWSTR retW = NULL;

    len = WideCharToMultiByte(CP_ACP, 0, buffer, -1, NULL, 0, NULL, NULL);
    if (!(bufferA = HeapAlloc(GetProcessHeap(), 0, len)))
    {
        WSASetLastError(WSA_NOT_ENOUGH_MEMORY);
        return NULL;
    }
    WideCharToMultiByte(CP_ACP, 0, buffer, -1, bufferA, len, NULL, NULL);
    retA = inet_ntop(family, addr, bufferA, size);

    if (!MultiByteToWideChar(CP_ACP, 0, retA, -1, retW, size))
    {
        WSASetLastError(WSA_NOT_ENOUGH_MEMORY);
        return NULL;
    }
    return retW;
}

/***********************************************************************
 *     WSAPoll
 */

// FIXME: maybe we could use curl poll()? but it's a fallback via select()
