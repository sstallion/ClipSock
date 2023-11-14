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

#pragma once

#include <windows.h>

#include "util.h"

namespace ClipSock {

template<typename T, typename C, auto Count, auto Padding = 1>
class GlobalBuffer {
    HGLOBAL m_hMem;
    T* m_pData;
    C m_cData;

public:
    using Type = T;
    using CountType = C;

    GlobalBuffer()
        : m_cData{Count}
    {
        constexpr auto dwBytes = (Count + Padding) * sizeof(T);
        m_hMem = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwBytes);
        VERIFY_WIN32(m_hMem);

        try {
            m_pData = reinterpret_cast<T*>(GlobalLock(m_hMem));
            VERIFY_WIN32(m_pData);
        }
        catch (...) {
            GlobalFree(m_hMem);
            throw;
        }
    }

    ~GlobalBuffer()
    {
        if (m_hMem) {
            GlobalFree(m_hMem);
        }
    }

    inline auto IsEmpty() const { return m_cData == Count; }
    inline auto IsFull() const { return m_cData == 0; }
    inline auto Length() const { return m_cData; }

    auto Release()
    {
        auto hMem = m_hMem;
        m_hMem = nullptr;
        m_pData = nullptr;
        m_cData = 0;
        GlobalUnlock(hMem);
        return hMem;
    }

    inline auto operator&() const
    {
        return m_pData;
    }

    inline void operator+=(auto cOffset)
    {
        m_pData += cOffset;
        m_cData -= cOffset;
    }
};

} // namespace ClipSock
