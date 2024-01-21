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

#include "buffer.h"
#include "eventlog.h"

#include <windows.h>
#include <winsock2.h>

#include <unordered_map>
#include <vector>

namespace ClipSock::Server {

inline constexpr auto MAXIMUM_BUFFER_SIZE = 65535;

using EventLogger = EventLog::DefaultLogger;
using EventBuffer = GlobalBuffer<CHAR, INT, MAXIMUM_BUFFER_SIZE>;
using EventVector = std::vector<WSAEVENT>;
using EventSocketMap = std::unordered_map<WSAEVENT, SOCKET>;
using EventBufferMap = std::unordered_map<WSAEVENT, EventBuffer>;

extern EventLogger Logger;
extern HANDLE hThread;
extern BOOL bStopRequested;
extern EventVector Events;
extern EventSocketMap Sockets;
extern EventBufferMap Buffers;

void CleanupEvent(WSAEVENT hEvent);
void CleanupEvents();

void Accept(SOCKET hSocket);
void Read(SOCKET hSocket, EventBuffer& Buffer);
void Close(WSAEVENT hEvent);

DWORD WINAPI ThreadProc(PVOID pParam);

SIZE_T GetAddress(PCWSTR szAddress, PSOCKADDR_STORAGE pAddress);

void Start();
void Stop();
void Restart();

void Fail(PCSTR szReason);

void Init();

} // namespace ClipSock::Server
