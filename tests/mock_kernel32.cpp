/*
 * Copyright 2023 Steven Stallion
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

#include "mock_kernel32.h"

MockKernel32* g_pInstance;

template<>
MOCKBASEAPI void MockKernel32::Global::Lock(MockKernel32* pInstance)
{
    ASSERT_EQ(g_pInstance, nullptr);
    g_pInstance = pInstance;
}

template<>
MOCKBASEAPI void MockKernel32::Global::Unlock()
{
    ASSERT_NE(g_pInstance, nullptr);
    g_pInstance = nullptr;
}

WINBASEAPI DECLSPEC_ALLOCATOR HGLOBAL WINAPI GlobalAlloc(UINT uFlags, SIZE_T dwBytes)
{
    EXPECT_NE(g_pInstance, nullptr);
    return g_pInstance->GlobalAlloc(uFlags, dwBytes);
}

WINBASEAPI HGLOBAL WINAPI GlobalFree(HGLOBAL hMem)
{
    EXPECT_NE(g_pInstance, nullptr);
    return g_pInstance->GlobalFree(hMem);
}

WINBASEAPI LPVOID WINAPI GlobalLock(HGLOBAL hMem)
{
    EXPECT_NE(g_pInstance, nullptr);
    return g_pInstance->GlobalLock(hMem);
}

WINBASEAPI BOOL WINAPI GlobalUnlock(HGLOBAL hMem)
{
    EXPECT_NE(g_pInstance, nullptr);
    return g_pInstance->GlobalUnlock(hMem);
}
