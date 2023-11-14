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

#include "buffer.h"
#include "mock_kernel32.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <windows.h>

#include <cstddef>
#include <stdexcept>

namespace ClipSock::Test {

inline constexpr auto BUFFER_SIZE = 64;

using TestBuffer = GlobalBuffer<std::byte, std::size_t, BUFFER_SIZE>;

TEST(GlobalBuffer, GlobalAllocFail)
{
    testing::StrictMock<MockKernel32> mock_kernel32;

    EXPECT_CALL(mock_kernel32, GlobalAlloc(testing::_, testing::_))
        .WillOnce(testing::Return(nullptr));

    EXPECT_THROW(TestBuffer{}, std::runtime_error);
}

TEST(GlobalBuffer, GlobalLockFail)
{
    testing::StrictMock<MockKernel32> mock_kernel32;
    TestBuffer::Type mock_hMem[BUFFER_SIZE];

    testing::InSequence Sequence;

    EXPECT_CALL(mock_kernel32, GlobalAlloc(testing::_, testing::_))
        .WillOnce(testing::Return(mock_hMem));

    EXPECT_CALL(mock_kernel32, GlobalLock(mock_hMem))
        .WillOnce(testing::Return(nullptr));

    EXPECT_CALL(mock_kernel32, GlobalFree(mock_hMem));

    EXPECT_THROW(TestBuffer{}, std::runtime_error);
}

TEST(GlobalBuffer, Release)
{
    testing::StrictMock<MockKernel32> mock_kernel32;
    TestBuffer::Type mock_hMem[BUFFER_SIZE];

    testing::InSequence Sequence;

    EXPECT_CALL(mock_kernel32, GlobalAlloc(testing::_, testing::_))
        .WillOnce(testing::Return(mock_hMem));

    EXPECT_CALL(mock_kernel32, GlobalLock(mock_hMem))
        .WillOnce(testing::Return(mock_hMem));

    EXPECT_CALL(mock_kernel32, GlobalUnlock(mock_hMem));

    TestBuffer buffer;

    EXPECT_EQ(buffer.Release(), mock_hMem);
    EXPECT_EQ(&buffer, nullptr);
    EXPECT_EQ(buffer.Length(), 0);
}

} // namespace ClipSock::Test
