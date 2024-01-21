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

#include "server.h"

#include "buffer.h"
#include "eventlog.h"
#include "messages.h"
#include "notify.h"
#include "settings.h"
#include "util.h"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windns.h>
#include <iphlpapi.h>

#include <algorithm>
#include <cstdlib>
#include <exception>

namespace ClipSock::Server {

EventLogger Logger;
HANDLE hThread;
BOOL bStopRequested;
EventVector Events;
EventSocketMap Sockets;
EventBufferMap Buffers;

void CleanupEvent(WSAEVENT hEvent)
{
    if (hEvent == WSA_INVALID_EVENT) {
        return;
    }

    Buffers.erase(hEvent);
    if (Sockets.contains(hEvent)) {
        closesocket(Sockets[hEvent]);
        Sockets.erase(hEvent);
    }
    WSACloseEvent(hEvent);
    Events.erase(std::find(Events.begin(), Events.end(), hEvent));
}

void CleanupEvents()
{
    // Events are cleaned up in LIFO order without using iterators to avoid
    // unnecessary reallocation and iterator invalidation:
    while (!Events.empty()) {
        CleanupEvent(Events.back());
    }
}

void Accept(SOCKET hSocket)
{
    auto hNewEvent = WSA_INVALID_EVENT;

    // Care must be taken when establishing a new connection; if a failure
    // propagates, it will close the listening socket and halt the server.
    try {
        VERIFY(Events.size() < WSA_MAXIMUM_WAIT_EVENTS,
               "Maximum number of clients reached: {}", Events.size());

        hNewEvent = WSACreateEvent();
        VERIFY_WIN32(hNewEvent != WSA_INVALID_EVENT);
        Events.push_back(hNewEvent);

        auto hNewSocket = accept(hSocket, nullptr, nullptr);
        VERIFY_WIN32(hNewSocket != INVALID_SOCKET);
        Sockets[hNewEvent] = hNewSocket;

        VERIFY_WIN32(WSAEventSelect(hNewSocket, hNewEvent, FD_READ | FD_CLOSE) != SOCKET_ERROR);
    }
    catch (const std::exception& e) {
        Logger.ReportWarn(MSG_CONNECTION_FAILED, e.what());
        CleanupEvent(hNewEvent);
    }
}

void Read(SOCKET hSocket, EventBuffer& Buffer)
{
    auto nBytesRecvd = recv(hSocket, &Buffer, Buffer.Length(), 0);
    VERIFY_WIN32(nBytesRecvd != SOCKET_ERROR);
    Buffer += nBytesRecvd;
}

void Close(WSAEVENT hEvent)
{
    // Release ownership to the system and set clipboard data if a
    // non-empty buffer is associated with the event:
    if (Buffers.contains(hEvent)) {
        auto& Buffer = Buffers[hEvent];
        if (!Buffer.IsEmpty()) {
            OpenClipboard(nullptr);
            EmptyClipboard();
            SetClipboardData(CF_TEXT, Buffer.Release());
            CloseClipboard();
        }
    }

    CleanupEvent(hEvent);
}

DWORD WINAPI ThreadProc(PVOID /*pParam*/)
{
    try {
        for (;;) {
            auto cEvents = static_cast<DWORD>(Events.size());
            auto dwResult = WSAWaitForMultipleEvents(cEvents, Events.data(),
                                                     FALSE, WSA_INFINITE, TRUE);
            if (bStopRequested) {
                return 0;
            }
            VERIFY_WIN32_RANGE(dwResult, WSA_WAIT_EVENT_0, cEvents);

            auto& hEvent = Events[dwResult - WSA_WAIT_EVENT_0];
            auto& hSocket = Sockets[hEvent];
            try {
                WSANETWORKEVENTS NetworkEvents;
                VERIFY_WIN32(WSAEnumNetworkEvents(hSocket, hEvent, &NetworkEvents) != SOCKET_ERROR);

                if (NetworkEvents.lNetworkEvents & FD_ACCEPT) {
                    VERIFY_WIN32_RESULT(NetworkEvents.iErrorCode[FD_ACCEPT_BIT]);
                    Accept(hSocket);
                }

                if (NetworkEvents.lNetworkEvents & FD_READ) {
                    VERIFY_WIN32_RESULT(NetworkEvents.iErrorCode[FD_READ_BIT]);
                    auto& Buffer = Buffers[hEvent];
                    Read(hSocket, Buffer);
                    if (Buffer.IsFull()) {
                        Close(hEvent);
                    }
                }

                if (NetworkEvents.lNetworkEvents & FD_CLOSE) {
                    VERIFY_WIN32_RESULT(NetworkEvents.iErrorCode[FD_CLOSE_BIT]);
                    Close(hEvent);
                }
            }
            catch (const std::exception& e) {
                Logger.ReportWarn(MSG_CONNECTION_FAILED, e.what());
                CleanupEvent(hEvent);
            }
        }
    }
    catch (const std::exception& e) {
        Fail(e.what());
    }

    return 0;
}

SIZE_T GetAddress(PCWSTR szAddress, PSOCKADDR_STORAGE pAddress)
{
    NET_ADDRESS_INFO AddressInfo;
    VERIFY_WIN32_RESULT(ParseNetworkString(szAddress, NET_STRING_ANY_SERVICE,
                                           &AddressInfo, nullptr, nullptr));
    switch (AddressInfo.Format) {
    case NET_ADDRESS_DNS_NAME: {
        PADDRINFOW pResult;
        VERIFY_WIN32_RESULT(GetAddrInfoW(AddressInfo.NamedAddress.Address,
                                         AddressInfo.NamedAddress.Port,
                                         nullptr,
                                         &pResult));
        auto _ = MakeUnique(pResult, FreeAddrInfoW);
        std::memcpy(pAddress, pResult->ai_addr, pResult->ai_addrlen);
        return pResult->ai_addrlen;
    }

    case NET_ADDRESS_IPV4:
        std::memcpy(pAddress, &AddressInfo.IpAddress, sizeof(SOCKADDR_IN));
        return sizeof(SOCKADDR_IN);

    case NET_ADDRESS_IPV6:
        std::memcpy(pAddress, &AddressInfo.IpAddress, sizeof(SOCKADDR_IN6));
        return sizeof(SOCKADDR_IN6);

    default:
        THROW("Unsupported address format: {}", int{AddressInfo.Format});
    }
}

void Start()
{
    try {
        SOCKADDR_STORAGE ListenAddress;
        auto ListenAddressLength = GetAddress(Settings::szListenAddress, &ListenAddress);

        auto hNewEvent = WSACreateEvent();
        VERIFY_WIN32(hNewEvent != WSA_INVALID_EVENT);
        Events.push_back(hNewEvent);

        auto hNewSocket = socket(ListenAddress.ss_family, SOCK_STREAM, IPPROTO_TCP);
        VERIFY_WIN32(hNewSocket != INVALID_SOCKET);
        Sockets[hNewEvent] = hNewSocket;

        VERIFY_WIN32(bind(hNewSocket,
                          reinterpret_cast<PSOCKADDR>(&ListenAddress),
                          static_cast<int>(ListenAddressLength)) != SOCKET_ERROR);

        VERIFY_WIN32(WSAEventSelect(hNewSocket, hNewEvent, FD_ACCEPT | FD_CLOSE) != SOCKET_ERROR);

        VERIFY_WIN32(listen(hNewSocket, SOMAXCONN) != SOCKET_ERROR);

        bStopRequested = FALSE;
        hThread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
        VERIFY_WIN32(hThread);

        Logger.ReportInfo(MSG_SERVER_STARTED);
        Notify::SendUpdate(Settings::szListenAddress);
    }
    catch (const std::exception& e) {
        Fail(e.what());
        Settings::ShowDialog(); // prompt user to check settings
    }
}

void Stop()
{
    if (hThread) {
        // Alert WSAWaitForMultipleEvents using an APC to prevent blocking
        // until the next network event arrives:
        auto fnAPC = [](ULONG_PTR /*dwData*/) {
            bStopRequested = TRUE;
        };
        QueueUserAPC(fnAPC, hThread, 0);
        WaitForMultipleObjects(1, &hThread, TRUE, INFINITE);
        CloseHandle(hThread);
        hThread = nullptr;
    }

    Logger.ReportInfo(MSG_SERVER_STOPPED);
    Notify::SendUpdate(L"Stopped");
    CleanupEvents();
}

void Restart()
{
    Stop();
    Start();
}

void Fail(PCSTR szReason)
{
    Logger.ReportError(MSG_SERVER_FAILED, szReason);
    Notify::SendUpdate(L"Failed");
    CleanupEvents();
    ShowError("Server failed with exception: {}", szReason);
}

void Init()
{
    WSADATA wsaData;
    VERIFY_WIN32_RESULT(WSAStartup(MAKEWORD(2, 2), &wsaData));

    auto bMajorVersion = LOBYTE(wsaData.wVersion);
    auto bMinorVersion = HIBYTE(wsaData.wVersion);
    VERIFY(bMajorVersion == 2 && bMinorVersion == 2,
           "Unsupported Winsock version: {}.{}", bMajorVersion, bMinorVersion);
}

} // namespace ClipSock::Server
