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

LPCSTR WSAAPI inet_ntop(INT af, PVOID src, LPSTR dst, size_t size)
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

INT WSAAPI inet_pton(INT af, LPCSTR csrc, PVOID dst)
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
INT WSAAPI InetPtonW(INT family, LPCWSTR addr, PVOID buffer)
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
LPCWSTR WSAAPI InetNtopW(INT family, PVOID addr, LPCWSTR buffer, size_t size)
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
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2005, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 * $Id: select.c,v 1.13 2005/04/26 13:08:49 bagder Exp $
 ***************************************************************************/

#include <errno.h>
#include <stdbool.h>
#include <fcntl.h>

struct pollfd
{
    SOCKET fd;
    short events;
    short revents;
};

#define POLLIN      0x01
#define POLLPRI     0x02
#define POLLOUT     0x04
#define POLLERR     0x08
#define POLLHUP     0x10
#define POLLNVAL    0x20

#define POLLRDNORM POLLIN
#define POLLWRNORM POLLOUT
#define POLLRDBAND POLLPRI


#define VERIFY_SOCK(x)  /* Win-sockets are not in range [0..FD_SETSIZE> */

static long curlx_tvdiff(struct timeval newer, struct timeval older)
{
  return (newer.tv_sec-older.tv_sec)*1000+
    (newer.tv_usec-older.tv_usec)/1000;
}

static struct timeval curlx_tvnow(void)
{
  /*
  ** GetTickCount() is available on _all_ Windows versions from W95 up
  ** to nowadays. Returns milliseconds elapsed since last system boot,
  ** increases monotonically and wraps once 49.7 days have elapsed.
  */
  struct timeval now;
  DWORD milliseconds = GetTickCount();
  now.tv_sec = milliseconds / 1000;
  now.tv_usec = (milliseconds % 1000) * 1000;
  return now;
}

static int Curl_wait_ms(int timeout_ms)
{
  int r = 0;
  if(!timeout_ms)
    return 0;
  if(timeout_ms < 0) {
    WSASetLastError((int)EINVAL);
    return -1;
  }
  Sleep(timeout_ms);
  if(r)
    r = -1;
  return r;
}

int WSAAPI WSAPoll(struct pollfd ufds[], unsigned int nfds, int timeout_ms)
{

  struct timeval pending_tv;
  struct timeval *ptimeout;
  fd_set fds_read;
  fd_set fds_write;
  fd_set fds_err;
  SOCKET maxfd;

  struct timeval initial_tv = {0,0};
  bool fds_none = TRUE;
  unsigned int i;
  int pending_ms = 0;
  int error;
  int r;

  if(ufds) {
    for(i = 0; i < nfds; i++) {
      if(ufds[i].fd != INVALID_SOCKET) {
        fds_none = FALSE;
        break;
      }
    }
  }
  if(fds_none) {
    r = Curl_wait_ms(timeout_ms);
    return r;
  }

  /* Avoid initial timestamp, avoid curlx_tvnow() call, when elapsed
     time in this function does not need to be measured. This happens
     when function is called with a zero timeout or a negative timeout
     value indicating a blocking call should be performed. */

  if(timeout_ms > 0) {
    pending_ms = timeout_ms;
    initial_tv = curlx_tvnow();
  }
  FD_ZERO(&fds_read);
  FD_ZERO(&fds_write);
  FD_ZERO(&fds_err);
  maxfd = (SOCKET)-1;

  for(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    if(ufds[i].fd == INVALID_SOCKET)
      continue;
    VERIFY_SOCK(ufds[i].fd);
    if(ufds[i].events & (POLLIN|POLLOUT|POLLPRI|
                          POLLRDNORM|POLLWRNORM|POLLRDBAND)) {
      if(ufds[i].fd > maxfd)
        maxfd = ufds[i].fd;
      if(ufds[i].events & (POLLRDNORM|POLLIN))
        FD_SET(ufds[i].fd, &fds_read);
      if(ufds[i].events & (POLLWRNORM|POLLOUT))
        FD_SET(ufds[i].fd, &fds_write);
      if(ufds[i].events & (POLLRDBAND|POLLPRI))
        FD_SET(ufds[i].fd, &fds_err);
    }
  }

  ptimeout = (timeout_ms < 0) ? NULL : &pending_tv;

  do {
    if(timeout_ms > 0) {
      pending_tv.tv_sec = pending_ms / 1000;
      pending_tv.tv_usec = (pending_ms % 1000) * 1000;
    }
    else if(!timeout_ms) {
      pending_tv.tv_sec = 0;
      pending_tv.tv_usec = 0;
    }
    r = select((int)maxfd + 1, &fds_read, &fds_write, &fds_err, ptimeout);
    if(r != -1)
      break;
    error = (int)WSAGetLastError();
    if(error && (0 || error != WSAEINTR))
      break;
    if(timeout_ms > 0) {
      pending_ms = timeout_ms - curlx_tvdiff(curlx_tvnow(), initial_tv);
      if(pending_ms <= 0)
        break;
    }
  } while(r == -1);

  if(r < 0)
    return -1;
  if(r == 0)
    return 0;

  r = 0;
  for(i = 0; i < nfds; i++) {
    ufds[i].revents = 0;
    if(ufds[i].fd == INVALID_SOCKET)
      continue;
    if(FD_ISSET(ufds[i].fd, &fds_read))
      ufds[i].revents |= POLLIN;
    if(FD_ISSET(ufds[i].fd, &fds_write))
      ufds[i].revents |= POLLOUT;
    if(FD_ISSET(ufds[i].fd, &fds_err))
      ufds[i].revents |= POLLPRI;
    if(ufds[i].revents != 0)
      r++;
  }

  return r;
}
