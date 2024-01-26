/*
 * Copyright 2024 Steven Stallion
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "mock_export.h"
#include "mock_global.h"
#include "mock_winsock.h"

template class GlobalMock<MockWinsock>;

using MockGlobal = GlobalMock<MockWinsock>;

MOCK_EXPORT SOCKET WSAAPI accept(SOCKET s, struct sockaddr* addr, int* addrlen)
{
    return MockGlobal::Call(&MockWinsock::accept, s, addr, addrlen);
}

MOCK_EXPORT int WSAAPI closesocket(SOCKET s)
{
    return MockGlobal::Call(&MockWinsock::closesocket, s);
}

MOCK_EXPORT int WSAAPI recv(SOCKET s, char* buf, int len, int flags)
{
    return MockGlobal::Call(&MockWinsock::recv, s, buf, len, flags);
}

MOCK_EXPORT BOOL WSAAPI WSACloseEvent(WSAEVENT hEvent)
{
    return MockGlobal::Call(&MockWinsock::WSACloseEvent, hEvent);
}

MOCK_EXPORT WSAEVENT WSAAPI WSACreateEvent()
{
    return MockGlobal::Call(&MockWinsock::WSACreateEvent);
}

MOCK_EXPORT int WSAAPI WSAEnumNetworkEvents(SOCKET s,
                                            WSAEVENT hEventObject,
                                            LPWSANETWORKEVENTS lpNetworkEvents)
{
    return MockGlobal::Call(&MockWinsock::WSAEnumNetworkEvents, s, hEventObject, lpNetworkEvents);
}

MOCK_EXPORT int WSAAPI WSAEventSelect(SOCKET s,
                                      WSAEVENT hEventObject,
                                      long lNetworkEvents)
{
    return MockGlobal::Call(&MockWinsock::WSAEventSelect, s, hEventObject, lNetworkEvents);
}

MOCK_EXPORT DWORD WSAAPI WSAWaitForMultipleEvents(DWORD cEvents,
                                                  const WSAEVENT* lphEvents,
                                                  BOOL fWaitAll,
                                                  DWORD dwTimeout,
                                                  BOOL fAlertable)
{
    return MockGlobal::Call(&MockWinsock::WSAWaitForMultipleEvents,
                            cEvents, lphEvents, fWaitAll, dwTimeout, fAlertable);
}
