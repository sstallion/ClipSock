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

#include "eventlog.h"
#include "notify.h"
#include "server.h"
#include "settings.h"

#include <windows.h>

#include <exception>

using namespace ClipSock;

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
                    _In_opt_ HINSTANCE /*hPrevInstance*/,
                    _In_ PWSTR /*pCmdLine*/,
                    _In_ int /*nCmdShow*/)
{
    CreateMutex(nullptr, TRUE, L"ClipSock Mutex");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        return 0;
    }

    try {
        EventLog::Init();
        Server::Init();
        Settings::Init(hInstance);
        Notify::Init(hInstance);

        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            if (!Settings::IsMessage(&msg)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        return static_cast<int>(msg.wParam);
    }
    catch (const std::exception& e) {
        ShowError("Fatal exception: {}", e.what());
    }

    return 0;
}
