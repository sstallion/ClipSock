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
#include "mock_windows.h"

template class GlobalMock<MockWindows>;

using MockGlobal = GlobalMock<MockWindows>;

MOCK_EXPORT DECLSPEC_ALLOCATOR HGLOBAL WINAPI GlobalAlloc(UINT uFlags, SIZE_T dwBytes)
{
    return MockGlobal::Call(&MockWindows::GlobalAlloc, uFlags, dwBytes);
}

MOCK_EXPORT HGLOBAL WINAPI GlobalFree(HGLOBAL hMem)
{
    return MockGlobal::Call(&MockWindows::GlobalFree, hMem);
}

MOCK_EXPORT LPVOID WINAPI GlobalLock(HGLOBAL hMem)
{
    return MockGlobal::Call(&MockWindows::GlobalLock, hMem);
}

MOCK_EXPORT BOOL WINAPI GlobalUnlock(HGLOBAL hMem)
{
    return MockGlobal::Call(&MockWindows::GlobalUnlock, hMem);
}

MOCK_EXPORT BOOL WINAPI CloseClipboard()
{
    return MockGlobal::Call(&MockWindows::CloseClipboard);
}

MOCK_EXPORT BOOL WINAPI EmptyClipboard()
{
    return MockGlobal::Call(&MockWindows::EmptyClipboard);
}

MOCK_EXPORT BOOL WINAPI OpenClipboard(HWND hWndNewOwner)
{
    return MockGlobal::Call(&MockWindows::OpenClipboard, hWndNewOwner);
}

MOCK_EXPORT HANDLE WINAPI SetClipboardData(UINT uFormat, HANDLE hMem)
{
    return MockGlobal::Call(&MockWindows::SetClipboardData, uFormat, hMem);
}

MOCK_EXPORT BOOL WINAPI ReportEventA(HANDLE hEventLog,
                                     WORD wType,
                                     WORD wCategory,
                                     DWORD dwEventID,
                                     PSID lpUserSid,
                                     WORD wNumStrings,
                                     DWORD dwDataSize,
                                     LPCSTR* lpStrings,
                                     LPVOID lpRawData)
{
    return MockGlobal::Call(&MockWindows::ReportEventA, hEventLog, wType, wCategory, dwEventID,
                            lpUserSid, wNumStrings, dwDataSize, lpStrings, lpRawData);
}

MOCK_EXPORT BOOL WINAPI ReportEventW(HANDLE hEventLog,
                                     WORD wType,
                                     WORD wCategory,
                                     DWORD dwEventID,
                                     PSID lpUserSid,
                                     WORD wNumStrings,
                                     DWORD dwDataSize,
                                     LPCWSTR* lpStrings,
                                     LPVOID lpRawData)
{
    return MockGlobal::Call(&MockWindows::ReportEventW, hEventLog, wType, wCategory, dwEventID,
                            lpUserSid, wNumStrings, dwDataSize, lpStrings, lpRawData);
}
