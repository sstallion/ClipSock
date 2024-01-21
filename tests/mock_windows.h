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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#ifndef WINADVAPI
#define WINADVAPI MOCK_EXPORT
#endif // WINADVAPI

#ifndef WINBASEAPI
#define WINBASEAPI MOCK_EXPORT
#endif // WINBASEAPI

#ifndef WINUSERAPI
#define WINUSERAPI MOCK_EXPORT
#endif // WINUSERAPI

#include <windows.h>

class MockWindows {
public:
    MOCK_METHOD(HGLOBAL, GlobalAlloc, (UINT, SIZE_T), (Calltype(MOCK_EXPORT)));
    MOCK_METHOD(HGLOBAL, GlobalFree, (HGLOBAL), (Calltype(MOCK_EXPORT)));
    MOCK_METHOD(LPVOID, GlobalLock, (HGLOBAL), (Calltype(MOCK_EXPORT)));
    MOCK_METHOD(BOOL, GlobalUnlock, (HGLOBAL), (Calltype(MOCK_EXPORT)));

    MOCK_METHOD(BOOL, CloseClipboard, (), (Calltype(MOCK_EXPORT)));
    MOCK_METHOD(BOOL, EmptyClipboard, (), (Calltype(MOCK_EXPORT)));
    MOCK_METHOD(BOOL, OpenClipboard, (HWND), (Calltype(MOCK_EXPORT)));
    MOCK_METHOD(HANDLE, SetClipboardData, (UINT, HANDLE), (Calltype(MOCK_EXPORT)));

    MOCK_METHOD(BOOL, ReportEventA, (HANDLE, WORD, WORD, DWORD, PSID, WORD, DWORD, LPCSTR*, LPVOID),
                (Calltype(MOCK_EXPORT)));
    MOCK_METHOD(BOOL, ReportEventW, (HANDLE, WORD, WORD, DWORD, PSID, WORD, DWORD, LPCWSTR*, LPVOID),
                (Calltype(MOCK_EXPORT)));
};
