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
#include <shellapi.h>

namespace ClipSock::Notify {

inline constexpr auto CLASSNAME = L"Notify Window Class";

inline constexpr auto WM_APP_NOTIFY = WM_APP + 0;
inline constexpr auto WM_APP_UPDATE = WM_APP + 1;

class __declspec(uuid("7E8EDDD8-0A70-46FC-963F-F513CF8D7561")) Icon;

extern HICON hIcon;
extern HMENU hContextMenu;
extern PWSTR szVersion;

void SetTip(NOTIFYICONDATA& nid, PCWSTR szText);

void AddIcon(HWND hWnd);
void UpdateIcon(HWND hWnd, PCWSTR szText);
void DeleteIcon(HWND hWnd);

void ShowContextMenu(HWND hWnd, POINT& pt);

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void SendUpdate(PCWSTR szText);

void LoadVersion(HINSTANCE hInstance, PCWSTR szName, PWSTR* pszVersion);

void Init(HINSTANCE hInstance);

} // namespace ClipSock::Notify
