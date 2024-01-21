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
#include "test_support.h"

#include "buffer.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <stdexcept>

using namespace ClipSock;
using namespace testing;

class BufferTest : public Test {
protected:
    static constexpr auto TEST_BUFFER_SIZE = 42;

    using TestBuffer = GlobalBuffer<std::byte, std::size_t, TEST_BUFFER_SIZE>;

    GlobalMock<MockWindows> mock_Windows;

    void SetUpBuffer(auto& mock_hMem)
    {
        ON_CALL(mock_Windows, GlobalAlloc)
            .WillByDefault(Return(mock_hMem));

        ON_CALL(mock_Windows, GlobalLock)
            .WillByDefault(Return(mock_hMem));
    }
};

TEST_F(BufferTest, CreateAndDestroy)
{
    TestBuffer::ValueType mock_hMem[TEST_BUFFER_SIZE+1]{};

    UINT expect_uFlags{GMEM_MOVEABLE | GMEM_ZEROINIT};
    EXPECT_CALL(mock_Windows, GlobalAlloc(HasFlags(expect_uFlags), sizeof(mock_hMem)))
        .WillOnce(Return(mock_hMem));

    EXPECT_CALL(mock_Windows, GlobalLock(mock_hMem))
        .WillOnce(Return(mock_hMem));

    EXPECT_CALL(mock_Windows, GlobalFree(mock_hMem))
        .WillOnce(Return(nullptr));

    // Verify behavior when creating and destroying:
    TestBuffer test_Buffer;
}

TEST_F(BufferTest, GlobalAllocFails)
{
    EXPECT_CALL(mock_Windows, GlobalAlloc)
        .WillOnce(Return(nullptr));

    // Verify behavior when GlobalAlloc() fails:
    EXPECT_THROW(TestBuffer{}, std::runtime_error);
}

TEST_F(BufferTest, GlobalLockFails)
{
    TestBuffer::ValueType mock_hMem[TEST_BUFFER_SIZE+1]{};

    ON_CALL(mock_Windows, GlobalAlloc)
        .WillByDefault(Return(mock_hMem));

    ON_CALL(mock_Windows, GlobalLock)
        .WillByDefault(Return(nullptr));

    EXPECT_CALL(mock_Windows, GlobalFree(mock_hMem))
        .WillOnce(Return(nullptr));

    // Verify behavior when GlobalLock() fails:
    EXPECT_THROW(TestBuffer{}, std::runtime_error);
}

TEST_F(BufferTest, Increment)
{
    TestBuffer::ValueType mock_hMem[TEST_BUFFER_SIZE+1]{};
    SetUpBuffer(mock_hMem);

    // Verify behavior when using the increment operator:
    TestBuffer test_Buffer;
    auto test_hMem = &test_Buffer;

    EXPECT_TRUE(test_Buffer.IsEmpty());
    for (auto i = 0; i < TEST_BUFFER_SIZE; i++) {
        EXPECT_EQ(&test_Buffer, test_hMem);
        EXPECT_EQ(test_Buffer.Length(), TEST_BUFFER_SIZE - i);
        test_Buffer++;
        test_hMem++;
    }
    EXPECT_TRUE(test_Buffer.IsFull());
}

TEST_F(BufferTest, Release)
{
    TestBuffer::ValueType mock_hMem[TEST_BUFFER_SIZE+1]{};
    SetUpBuffer(mock_hMem);

    EXPECT_CALL(mock_Windows, GlobalUnlock(mock_hMem))
        .WillOnce(Return(FALSE));

    // Verify behavior when releasing the memory object:
    TestBuffer test_Buffer;
    auto test_hMem = test_Buffer.Release();

    EXPECT_EQ(test_hMem, mock_hMem);
    EXPECT_EQ(&test_Buffer, nullptr);
    EXPECT_EQ(test_Buffer.Length(), 0);
}

TEST_F(BufferTest, DoubleRelease)
{
    TestBuffer::ValueType mock_hMem[TEST_BUFFER_SIZE+1]{};
    SetUpBuffer(mock_hMem);

    // Verify behavior when releasing the memory object again:
    EXPECT_THROW({
        TestBuffer test_Buffer;
        test_Buffer.Release();
        test_Buffer.Release();
    }, std::runtime_error);
}
