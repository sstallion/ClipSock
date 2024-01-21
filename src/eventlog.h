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
#include <string_view>
#include <utility>

namespace ClipSock::EventLog {

inline constexpr auto CATEGORY_NONE = 0;

extern HANDLE hEventLog;

template<auto wCategory>
class EventLogger {
public:
    BOOL ReportInfo(DWORD dwEventId)
    {
        return ReportType(EVENTLOG_INFORMATION_TYPE, dwEventId);
    }

    template<typename... Args>
    BOOL ReportInfo(DWORD dwEventId, auto svFormat, Args&&... vaArguments)
    {
        return ReportType(EVENTLOG_INFORMATION_TYPE, dwEventId, svFormat,
                          std::forward<Args>(vaArguments)...);
    }

    BOOL ReportWarn(DWORD dwEventId)
    {
        return ReportType(EVENTLOG_WARNING_TYPE, dwEventId);
    }

    template<typename... Args>
    BOOL ReportWarn(DWORD dwEventId, auto svFormat, Args&&... vaArguments)
    {
        return ReportType(EVENTLOG_WARNING_TYPE, dwEventId, svFormat,
                          std::forward<Args>(vaArguments)...);
    }

    BOOL ReportError(DWORD dwEventId)
    {
        return ReportType(EVENTLOG_ERROR_TYPE, dwEventId);
    }

    template<typename... Args>
    BOOL ReportError(DWORD dwEventId, auto svFormat, Args&&... vaArguments)
    {
        return ReportType(EVENTLOG_ERROR_TYPE, dwEventId, svFormat,
                          std::forward<Args>(vaArguments)...);
    }

protected:
    BOOL ReportType(WORD wType, DWORD dwEventId)
    {
        return ReportEvent(hEventLog, wType, wCategory, dwEventId, nullptr,
                           0, 0, nullptr, nullptr);
    }

    template<typename... Args>
    BOOL ReportType(WORD wType, DWORD dwEventId, std::string_view svFormat, Args&&... vaArguments)
    {
        auto sArg = std::vformat(svFormat, std::make_format_args(vaArguments...));
        PCSTR pStrings[]{sArg.data()};
        return ReportEventA(hEventLog, wType, wCategory, dwEventId, nullptr,
                            ARRAYSIZE(pStrings), 0, pStrings, nullptr);
    }

    template<typename... Args>
    BOOL ReportType(WORD wType, DWORD dwEventId, std::wstring_view svFormat, Args&&... vaArguments)
    {
        auto sArg = std::vformat(svFormat, std::make_wformat_args(vaArguments...));
        PCWSTR pStrings[]{sArg.data()};
        return ReportEventW(hEventLog, wType, wCategory, dwEventId, nullptr,
                            ARRAYSIZE(pStrings), 0, pStrings, nullptr);
    }
};

using DefaultLogger = EventLogger<CATEGORY_NONE>;

void Init();

} // namespace ClipSock::EventLog
