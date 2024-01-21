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

#include "mock_global.h"
#include "mock_windows.h"
#include "mock_winsock.h"
#include "test_support.h"

#include "server.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstring>
#include <stdexcept>
#include <tuple>

using namespace ClipSock::Server;
using namespace testing;

class ServerTest : public Test {
protected:
    GlobalMock<MockWindows> mock_Windows;
    GlobalMock<MockWinsock> mock_Winsock;

    void SetUpBuffer(auto& mock_hMem)
    {
        ON_CALL(mock_Windows, GlobalAlloc)
            .WillByDefault(Return(mock_hMem));

        ON_CALL(mock_Windows, GlobalLock(mock_hMem) )
            .WillByDefault(Return(mock_hMem));
    }

    auto SetUpEvent()
    {
        auto mock_hEvent = UniqueEvent();
        Events.push_back(mock_hEvent);
        return mock_hEvent;
    }

    auto SetUpSocket()
    {
        auto mock_hEvent = SetUpEvent();
        auto mock_hSocket = UniqueSocket();
        Sockets[mock_hEvent] = mock_hSocket;
        return std::make_tuple(mock_hEvent, mock_hSocket);
    }

    auto SetUpNetworkEvent(auto& mock_NetworkEvents)
    {
        auto SocketResult = SetUpSocket();
        auto [mock_hEvent, mock_hSocket] = SocketResult;

        bStopRequested = FALSE;
        EXPECT_CALL(mock_Winsock, WSAWaitForMultipleEvents)
            .WillOnce(Return(WSA_WAIT_EVENT_0))
            .WillOnce(DoAll(Assign(&bStopRequested, TRUE),
                            Return(WSA_WAIT_IO_COMPLETION)));

        EXPECT_CALL(mock_Winsock, WSAEnumNetworkEvents(mock_hSocket, mock_hEvent, _))
            .WillOnce(DoAll(SetArgPointee<2>(mock_NetworkEvents),
                            Return(0)));

        return SocketResult;
    }

    void TearDown() override
    {
        Events.clear();
        Sockets.clear();
        Buffers.clear();
    }

    UniqueGenerator<WSAEVENT> UniqueEvent;
    UniqueGenerator<SOCKET> UniqueSocket;
};

TEST_F(ServerTest, CleanupEvent)
{
    auto [mock_hEvent, mock_hSocket] = SetUpSocket();
    EventBuffer::ValueType mock_hMem[MAXIMUM_BUFFER_SIZE+1]{};
    SetUpBuffer(mock_hMem);

    EXPECT_CALL(mock_Windows, GlobalFree(mock_hMem));
    EXPECT_CALL(mock_Winsock, closesocket(mock_hSocket));
    EXPECT_CALL(mock_Winsock, WSACloseEvent(mock_hEvent));

    // Verify behavior when cleaning up an event:
    Buffers[mock_hEvent];
    CleanupEvent(mock_hEvent);
}

TEST_F(ServerTest, CleanupWithoutBuffer)
{
    auto [mock_hEvent, mock_hSocket] = SetUpSocket();

    EXPECT_CALL(mock_Windows, GlobalFree).Times(0);
    EXPECT_CALL(mock_Winsock, closesocket(mock_hSocket));
    EXPECT_CALL(mock_Winsock, WSACloseEvent(mock_hEvent));

    // Verify behavior when cleaning up an event without a buffer:
    CleanupEvent(mock_hEvent);
}

TEST_F(ServerTest, CleanupWithoutSocket)
{
    auto mock_hEvent = SetUpEvent();

    EXPECT_CALL(mock_Windows, GlobalFree).Times(0);
    EXPECT_CALL(mock_Winsock, closesocket).Times(0);
    EXPECT_CALL(mock_Winsock, WSACloseEvent(mock_hEvent));

    // Verify behavior when cleaning up an event without a socket:
    CleanupEvent(mock_hEvent);
}

TEST_F(ServerTest, CleanupInvalidEvent)
{
    EXPECT_CALL(mock_Windows, GlobalFree).Times(0);
    EXPECT_CALL(mock_Winsock, closesocket).Times(0);
    EXPECT_CALL(mock_Winsock, WSACloseEvent).Times(0);

    // Verify behavior when cleaning up an invalid event:
    CleanupEvent(WSA_INVALID_EVENT);
}

TEST_F(ServerTest, CleanupEvents)
{
    auto mock_hEvent1 = SetUpEvent();
    auto mock_hEvent2 = SetUpEvent();
    auto mock_hEvent3 = SetUpEvent();

    EXPECT_CALL(mock_Winsock, WSACloseEvent(mock_hEvent3));
    EXPECT_CALL(mock_Winsock, WSACloseEvent(mock_hEvent2));
    EXPECT_CALL(mock_Winsock, WSACloseEvent(mock_hEvent1));

    // Verify behavior when cleaning up multiple events:
    CleanupEvents();
}

TEST_F(ServerTest, AcceptEvent)
{
    WSANETWORKEVENTS mock_NetworkEvents{ .lNetworkEvents = FD_ACCEPT };
    auto [mock_hEvent, mock_hSocket] = SetUpNetworkEvent(mock_NetworkEvents);
    auto [mock_hNewEvent, mock_hNewSocket] = SetUpSocket();

    EXPECT_CALL(mock_Winsock, WSACreateEvent())
        .WillOnce(Return(mock_hNewEvent));

    EXPECT_CALL(mock_Winsock, accept(mock_hSocket, _, _))
        .WillOnce(Return(mock_hNewSocket));

    long expect_lNetworkEvents{FD_READ | FD_CLOSE};
    EXPECT_CALL(mock_Winsock, WSAEventSelect(mock_hNewSocket,
                                             mock_hNewEvent,
                                             HasFlags(expect_lNetworkEvents)))
        .WillOnce(Return(0));

    // Verify behavior when an FD_ACCEPT network event occurs:
    ThreadProc(nullptr);
}

TEST_F(ServerTest, AcceptEventError)
{
    WSANETWORKEVENTS mock_NetworkEvents{ .lNetworkEvents = FD_ACCEPT };
    mock_NetworkEvents.iErrorCode[FD_ACCEPT_BIT] = WSAENETDOWN;
    auto [mock_hEvent, mock_hSocket] = SetUpNetworkEvent(mock_NetworkEvents);

    EXPECT_CALL(mock_Windows, ReportEventA);
    EXPECT_CALL(mock_Winsock, closesocket(mock_hSocket));
    EXPECT_CALL(mock_Winsock, WSACloseEvent(mock_hEvent));

    // Verify behavior when an FD_ACCEPT network event fails:
    ThreadProc(nullptr);
}

TEST_F(ServerTest, AcceptFull)
{
    auto [mock_hEvent, mock_hSocket] = SetUpSocket();

    EXPECT_CALL(mock_Windows, ReportEventA);

    // Verify behavior when server is full:
    Events.resize(WSA_MAXIMUM_WAIT_EVENTS);
    EXPECT_NO_THROW(Accept(mock_hSocket));
}

TEST_F(ServerTest, AcceptInvalidEvent)
{
    auto [mock_hEvent, mock_hSocket] = SetUpSocket();

    EXPECT_CALL(mock_Winsock, WSACreateEvent)
        .WillOnce(Return(WSA_INVALID_EVENT));

    EXPECT_CALL(mock_Windows, ReportEventA);

    // Verify behavior when WSACreateEvent() fails:
    EXPECT_NO_THROW(Accept(mock_hSocket));
}

TEST_F(ServerTest, AcceptInvalidSocket)
{
    auto [mock_hEvent, mock_hSocket] = SetUpSocket();
    auto mock_hNewEvent = SetUpEvent();

    EXPECT_CALL(mock_Winsock, WSACreateEvent)
        .WillOnce(Return(mock_hNewEvent));

    EXPECT_CALL(mock_Winsock, accept)
        .WillOnce(Return(INVALID_SOCKET));

    EXPECT_CALL(mock_Windows, ReportEventA);
    EXPECT_CALL(mock_Winsock, WSACloseEvent(mock_hNewEvent));

    // Verify behavior when accept() fails:
    EXPECT_NO_THROW(Accept(mock_hSocket));
}

TEST_F(ServerTest, AcceptSelectFails)
{
    auto [mock_hEvent, mock_hSocket] = SetUpSocket();
    auto [mock_hNewEvent, mock_hNewSocket] = SetUpSocket();

    EXPECT_CALL(mock_Winsock, WSACreateEvent)
        .WillOnce(Return(mock_hNewEvent));

    EXPECT_CALL(mock_Winsock, accept)
        .WillOnce(Return(mock_hNewSocket));

    EXPECT_CALL(mock_Winsock, WSAEventSelect)
        .WillOnce(Return(SOCKET_ERROR));

    EXPECT_CALL(mock_Windows, ReportEventA);
    EXPECT_CALL(mock_Winsock, closesocket(mock_hNewSocket));
    EXPECT_CALL(mock_Winsock, WSACloseEvent(mock_hNewEvent));

    // Verify behavior when WSAEventSelect() fails:
    EXPECT_NO_THROW(Accept(mock_hSocket));
}

TEST_F(ServerTest, ReadEvent)
{
    WSANETWORKEVENTS mock_NetworkEvents{ .lNetworkEvents = FD_READ };
    auto [mock_hEvent, mock_hSocket] = SetUpNetworkEvent(mock_NetworkEvents);
    EventBuffer::ValueType mock_hMem[MAXIMUM_BUFFER_SIZE+1]{};
    SetUpBuffer(mock_hMem);

    auto expect_Fill = 'X';
    auto expect_Length = MAXIMUM_BUFFER_SIZE / 2;
    EXPECT_CALL(mock_Winsock, recv(mock_hSocket, mock_hMem, MAXIMUM_BUFFER_SIZE, _))
        .WillOnce(DoAll(WithArg<1>(FillPointer(expect_Fill, expect_Length)),
                        Return(expect_Length)));

    EXPECT_CALL(mock_Winsock, closesocket).Times(0);
    EXPECT_CALL(mock_Winsock, WSACloseEvent).Times(0);

    // Verify behavior when an FD_READ network event occurs:
    ThreadProc(nullptr);

    EXPECT_THAT(mock_hMem, Contains(expect_Fill).Times(expect_Length));
}

TEST_F(ServerTest, ReadEventFull)
{
    WSANETWORKEVENTS mock_NetworkEvents{ .lNetworkEvents = FD_READ };
    auto [mock_hEvent, mock_hSocket] = SetUpNetworkEvent(mock_NetworkEvents);
    EventBuffer::ValueType mock_hMem[MAXIMUM_BUFFER_SIZE+1]{};
    SetUpBuffer(mock_hMem);

    auto expect_Fill = 'X';
    auto expect_Length = MAXIMUM_BUFFER_SIZE;
    EXPECT_CALL(mock_Winsock, recv(mock_hSocket, mock_hMem, MAXIMUM_BUFFER_SIZE, _))
        .WillOnce(DoAll(WithArg<1>(FillPointer(expect_Fill, expect_Length)),
                        Return(expect_Length)));

    EXPECT_CALL(mock_Winsock, closesocket(mock_hSocket));
    EXPECT_CALL(mock_Winsock, WSACloseEvent(mock_hEvent));

    // Verify behavior when an FD_READ network event occurs with a full buffer:
    ThreadProc(nullptr);

    EXPECT_THAT(mock_hMem, Contains(expect_Fill).Times(expect_Length));
}

TEST_F(ServerTest, ReadEventError)
{
    WSANETWORKEVENTS mock_NetworkEvents{ .lNetworkEvents = FD_READ };
    mock_NetworkEvents.iErrorCode[FD_READ_BIT] = WSAENETDOWN;
    auto [mock_hEvent, mock_hSocket] = SetUpNetworkEvent(mock_NetworkEvents);

    EXPECT_CALL(mock_Windows, ReportEventA);
    EXPECT_CALL(mock_Winsock, closesocket(mock_hSocket));
    EXPECT_CALL(mock_Winsock, WSACloseEvent(mock_hEvent));

    // Verify behavior when an FD_READ network event fails:
    ThreadProc(nullptr);
}

TEST_F(ServerTest, ReadRecvFails)
{
    auto [mock_hEvent, mock_hSocket] = SetUpSocket();
    EventBuffer::ValueType mock_hMem[MAXIMUM_BUFFER_SIZE+1]{};
    SetUpBuffer(mock_hMem);

    EXPECT_CALL(mock_Winsock, recv)
        .WillOnce(Return(SOCKET_ERROR));

    // Verify behavior when recv() fails:
    EXPECT_THROW(Read(mock_hSocket, Buffers[mock_hEvent]), std::runtime_error);
}

TEST_F(ServerTest, CloseEvent)
{
    WSANETWORKEVENTS mock_NetworkEvents{ .lNetworkEvents = FD_CLOSE };
    auto [mock_hEvent, mock_hSocket] = SetUpNetworkEvent(mock_NetworkEvents);
    EventBuffer::ValueType mock_hMem[MAXIMUM_BUFFER_SIZE+1]{};
    SetUpBuffer(mock_hMem);

    EXPECT_CALL(mock_Windows, OpenClipboard);
    EXPECT_CALL(mock_Windows, EmptyClipboard);
    EXPECT_CALL(mock_Windows, GlobalUnlock(mock_hMem));
    EXPECT_CALL(mock_Windows, SetClipboardData(_, mock_hMem));
    EXPECT_CALL(mock_Windows, CloseClipboard);

    EXPECT_CALL(mock_Winsock, closesocket(mock_hSocket));
    EXPECT_CALL(mock_Winsock, WSACloseEvent(mock_hEvent));

    // Verify behavior when an FD_CLOSE network event occurs:
    Buffers[mock_hEvent]++;
    ThreadProc(nullptr);
}

TEST_F(ServerTest, CloseEventError)
{
    WSANETWORKEVENTS mock_NetworkEvents{ .lNetworkEvents = FD_CLOSE };
    mock_NetworkEvents.iErrorCode[FD_CLOSE_BIT] = WSAENETDOWN;
    auto [mock_hEvent, mock_hSocket] = SetUpNetworkEvent(mock_NetworkEvents);

    EXPECT_CALL(mock_Windows, ReportEventA);
    EXPECT_CALL(mock_Winsock, closesocket(mock_hSocket));
    EXPECT_CALL(mock_Winsock, WSACloseEvent(mock_hEvent));

    // Verify behavior when an FD_CLOSE network event fails:
    ThreadProc(nullptr);
}

TEST_F(ServerTest, CloseEmpty)
{
    auto [mock_hEvent, mock_hSocket] = SetUpSocket();

    EXPECT_CALL(mock_Windows, OpenClipboard).Times(0);
    EXPECT_CALL(mock_Windows, EmptyClipboard).Times(0);
    EXPECT_CALL(mock_Windows, SetClipboardData).Times(0);
    EXPECT_CALL(mock_Windows, CloseClipboard).Times(0);

    EXPECT_CALL(mock_Winsock, closesocket(mock_hSocket));
    EXPECT_CALL(mock_Winsock, WSACloseEvent(mock_hEvent));

    // Verify behavior closing with an empty buffer:
    Close(mock_hEvent);
}
