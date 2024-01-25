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

#include <functional>
#include <utility>

// GlobalMock provides a straightforward method of mocking free functions.
// To function properly, derived mocks and the gMock library must be
// dynamically linked to the test executable with instances of GlobalMock
// managed by the test executable in an RAII fashion.
template<typename T>
class GlobalMock : public testing::NiceMock<T> {
public:
    MOCK_EXPORT static void SetUp(GlobalMock<T>* pMock)
    {
        EXPECT_EQ(m_pMock, nullptr);
        m_pMock = pMock;
    }

    MOCK_EXPORT static void TearDown()
    {
        EXPECT_NE(m_pMock, nullptr);
        m_pMock = nullptr;
    }

    template<typename F, typename... Args,
             typename R = std::invoke_result_t<F, T*, Args...>>
    static R Call(F&& fnMock, Args&&... vaArguments)
    {
        if (!m_pMock) [[unlikely]] {
            ADD_FAILURE() << "Global mock not active";
            return R(); // must be default initializable or void
        }
        return std::invoke(std::forward<F>(fnMock),
                           std::forward<T*>(m_pMock),
                           std::forward<Args>(vaArguments)...);
    }

    GlobalMock() { SetUp(this); }
    virtual ~GlobalMock() { TearDown(); }

private:
    static GlobalMock<T>* m_pMock;
};

template<typename T>
GlobalMock<T>* GlobalMock<T>::m_pMock;
