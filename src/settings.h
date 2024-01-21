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

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

namespace ClipSock::Settings {

inline constexpr auto CLASSNAME = L"Settings Window Class";

inline constexpr auto DEFAULT_LAUNCH_AT_STARTUP = TRUE;
inline constexpr auto DEFAULT_LISTEN_ADDRESS = L"127.0.0.1:5494";

inline constexpr auto REGKEY_APP = L"Software\\ClipSock";
inline constexpr auto REGKEY_RUN = L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";
inline constexpr auto REGVAL_APP = L"ClipSock";
inline constexpr auto REGVAL_LAUNCH_AT_STARTUP = L"LaunchAtStartup";
inline constexpr auto REGVAL_LISTEN_ADDRESS = L"ListenAddress";

extern BOOL bLaunchAtStartup;
extern WCHAR szListenAddress[INET6_ADDRSTRLEN];

BOOL GetRegValues();
void SetRegValues();

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

BOOL IsMessage(PMSG pMsg);
void ShowDialog();

void Init(HINSTANCE hInstance);

} // namespace ClipSock::Settings
