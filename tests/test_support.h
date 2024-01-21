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

#include <gtest/gtest.h>

#include <concepts>
#include <cstddef>
#include <cstring>
#include <type_traits>

// UniqueGenerator provides a callable interface to generate unique values for
// the specified type. Values returned will be unique for a reasonable number
// of calls. Generated pointers should only be used to mock opaque handles
// and should never be dereferenced.
template<typename T, auto Start = 1>
    requires std::integral<T> || std::is_pointer_v<T>
class UniqueGenerator {
public:
    T operator()()
    {
        if constexpr (std::integral<T>) {
            return static_cast<T>(m_uCount++);
        } else if constexpr (std::is_pointer_v<T>) {
            return reinterpret_cast<T>(m_uCount++ * alignof(T));
        }
    }

private:
    std::size_t m_uCount{Start};
};

MATCHER_P(HasFlags, Flags, "")
{
    static_cast<void>(result_listener);
    return (arg & Flags) == Flags;
}

ACTION_P(FillPointer, Fill, Length)
{
    std::memset(arg0, Fill, Length);
}
