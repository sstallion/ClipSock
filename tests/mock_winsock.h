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

#pragma once

#include "mock_export.h"
#include "mock_windows.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#ifndef WINSOCK_API_LINKAGE
#define WINSOCK_API_LINKAGE MOCK_EXPORT
#endif // WINSOCK_API_LINKAGE

#include <winsock2.h>

class MockWinsock {
public:
    MOCK_METHOD(SOCKET, accept, (SOCKET, struct sockaddr*, int*), (Calltype(MOCK_EXPORT)));
    MOCK_METHOD(int, closesocket, (SOCKET), (Calltype(MOCK_EXPORT)));
    MOCK_METHOD(int, recv, (SOCKET, char*, int, int), (Calltype(MOCK_EXPORT)));

    MOCK_METHOD(BOOL, WSACloseEvent, (WSAEVENT), (Calltype(MOCK_EXPORT)));
    MOCK_METHOD(WSAEVENT, WSACreateEvent, (), (Calltype(MOCK_EXPORT)));
    MOCK_METHOD(int, WSAEnumNetworkEvents, (SOCKET, WSAEVENT, LPWSANETWORKEVENTS), (Calltype(MOCK_EXPORT)));
    MOCK_METHOD(int, WSAEventSelect, (SOCKET, WSAEVENT, long), (Calltype(MOCK_EXPORT)));
    MOCK_METHOD(DWORD, WSAWaitForMultipleEvents, (DWORD, const WSAEVENT*, BOOL, DWORD, BOOL), (Calltype(MOCK_EXPORT)));
};
