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

#include <format>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <type_traits>

#ifdef DEBUG
#define ERROR_LOCATION std::format("{}({},1): ", __FILE__, __LINE__)
#else
#define ERROR_LOCATION ""
#endif // DEBUG

#define THROW(svFormat, ...) \
    ThrowError(ERROR_LOCATION, svFormat, __VA_ARGS__)

#define VERIFY(bExpr_, svFormat_, ...)                     \
    [](auto bExpr, auto svFormat, auto... vaArguments) {   \
        if (!bExpr) [[unlikely]] {                         \
            THROW(svFormat, vaArguments...);               \
        }                                                  \
    }(bExpr_, svFormat_, __VA_ARGS__)

#define VERIFY_WIN32(bExpr_)                               \
    [](auto bExpr) {                                       \
        if (!bExpr) [[unlikely]] {                         \
            auto upError = GetLastErrorMessageA();         \
            THROW(upError.get());                          \
        }                                                  \
    }(bExpr_)

#define VERIFY_WIN32_RESULT(dwResult_)                     \
    [](auto dwResult) {                                    \
        if (dwResult != ERROR_SUCCESS) [[unlikely]] {      \
            auto upError = GetErrorMessageA(dwResult);     \
            THROW(upError.get());                          \
        }                                                  \
    }(dwResult_)

#define VERIFY_WIN32_RANGE(dwResult_, dwFirst_, dwLength_) \
    [](auto dwResult, auto dwFirst, auto dwLength) {       \
        VERIFY_WIN32(dwResult >= dwFirst &&                \
                     dwResult < dwFirst + dwLength);       \
    }(dwResult_, dwFirst_, dwLength_)

#ifdef DEBUG
#define ASSERT(bExpr, svFormat, ...)    VERIFY(bExpr, svFormat, __VA_ARGS__)
#define ASSERT_WIN32(bExpr)             VERIFY_WIN32(bExpr)
#define ASSERT_WIN32_RESULT(dwResult)   VERIFY_WIN32_RESULT(dwResult)
#else
#define ASSERT(bExpr, svFormat, ...)    static_cast<void>(bExpr)
#define ASSERT_WIN32(bExpr)             static_cast<void>(bExpr)
#define ASSERT_WIN32_RESULT(dwResult)   static_cast<void>(dwResult)
#endif // DEBUG

namespace ClipSock {

constexpr auto MakeUnique(auto hMem)
{
    return std::unique_ptr<std::remove_pointer_t<decltype(hMem)>>{hMem};
}

constexpr auto MakeUnique(auto hMem, auto fnFree)
{
    return std::unique_ptr<std::remove_pointer_t<decltype(hMem)>, decltype(fnFree)>{hMem, fnFree};
}

template<typename T, auto Formatter>
auto GetErrorMessageT(DWORD dwMessageId)
{
    T pBuffer{nullptr};
    Formatter(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        nullptr,
        dwMessageId,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<T>(&pBuffer),
        0,
        nullptr
    );
    return MakeUnique(pBuffer, LocalFree);
}

inline constexpr auto GetErrorMessageA = GetErrorMessageT<PSTR, FormatMessageA>;
inline constexpr auto GetErrorMessageW = GetErrorMessageT<PWSTR, FormatMessageW>;

inline auto GetLastErrorMessageA()
{
    return GetErrorMessageA(GetLastError());
}

inline auto GetLastErrorMessageW()
{
    return GetErrorMessageW(GetLastError());
}

template<typename... Args>
int ShowError(std::string_view svFormat, Args&&... vaArguments)
{
    auto sText = std::vformat(svFormat, std::make_format_args(vaArguments...));
    return MessageBoxA(nullptr, sText.c_str(), "ClipSock Error", MB_ICONERROR);
}

template<typename... Args>
int ShowError(std::wstring_view svFormat, Args&&... vaArguments)
{
    auto sText = std::vformat(svFormat, std::make_wformat_args(vaArguments...));
    return MessageBoxW(nullptr, sText.c_str(), L"ClipSock Error", MB_ICONERROR);
}

template<typename... Args>
[[noreturn]] void ThrowError(std::string_view svLocation,
                             std::string_view svFormat,
                             Args&&... vaArguments)
{
    auto sMessage = std::vformat(svFormat, std::make_format_args(vaArguments...));
    throw std::runtime_error{std::format("{}{}", svLocation, sMessage)};
}

} // namespace ClipSock
