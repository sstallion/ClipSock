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

#include "notify.h"

#include "resource.h"
#include "server.h"
#include "settings.h"
#include "util.h"

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>

#include <format>

namespace ClipSock::Notify {

HICON hIcon;
HMENU hContextMenu;
PWSTR szVersion;

void SetTip(NOTIFYICONDATA& nid, PCWSTR szText)
{
    auto [out, _] = std::format_to_n(nid.szTip, ARRAYSIZE(nid.szTip)-1,
                                     L"ClipSock v{}\n{}", szVersion, szText);
    *out = '\0';
}

void AddIcon(HWND hWnd)
{
    NOTIFYICONDATA nid{
        .cbSize = sizeof(NOTIFYICONDATA),
        .hWnd = hWnd,
        .uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_GUID | NIF_SHOWTIP,
        .uCallbackMessage = WM_APP_NOTIFY,
        .hIcon = hIcon,
        .uVersion = NOTIFYICON_VERSION_4,
        .guidItem = __uuidof(Icon)
    };
    SetTip(nid, L"Stopped");
    ASSERT_WIN32(Shell_NotifyIcon(NIM_ADD, &nid));
    ASSERT_WIN32(Shell_NotifyIcon(NIM_SETVERSION, &nid));
}

void UpdateIcon(HWND hWnd, PCWSTR szText)
{
    NOTIFYICONDATA nid{
        .cbSize = sizeof(NOTIFYICONDATA),
        .hWnd = hWnd,
        .uFlags = NIF_TIP | NIF_GUID | NIF_SHOWTIP,
        .guidItem = __uuidof(Icon)
    };
    SetTip(nid, szText);
    ASSERT_WIN32(Shell_NotifyIcon(NIM_MODIFY, &nid));
}

void DeleteIcon(HWND hWnd)
{
    NOTIFYICONDATA nid{
        .cbSize = sizeof(NOTIFYICONDATA),
        .hWnd = hWnd,
        .uFlags = NIF_GUID,
        .guidItem = __uuidof(Icon)
    };
    ASSERT_WIN32(Shell_NotifyIcon(NIM_DELETE, &nid));
}

void ShowContextMenu(HWND hWnd, POINT& pt)
{
    auto hMenu = GetSubMenu(hContextMenu, 0);
    ASSERT_WIN32(hMenu);

    auto uFlags = TPM_RIGHTBUTTON |
        GetSystemMetrics(SM_MENUDROPALIGNMENT) ? TPM_RIGHTALIGN : TPM_LEFTALIGN;

    SetForegroundWindow(hWnd);
    TrackPopupMenuEx(hMenu, uFlags, pt.x, pt.y, hWnd, nullptr);
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_CREATE:
        AddIcon(hWnd);
        Server::Start();
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDM_SETTINGS:
            Settings::ShowDialog();
            break;

        case IDM_QUIT:
            DestroyWindow(hWnd);
            break;
        }
        break;

    case WM_APP_NOTIFY:
        switch (LOWORD(lParam)) {
        case WM_CONTEXTMENU: {
            auto pt = POINT{GET_X_LPARAM(wParam), GET_Y_LPARAM(wParam)};
            ShowContextMenu(hWnd, pt);
            break;
        }

        case WM_LBUTTONUP:
            PostMessage(hWnd, WM_COMMAND, IDM_SETTINGS, 0);
            break;
        }
        break;

    case WM_APP_UPDATE: {
        auto szText = reinterpret_cast<PCWSTR>(wParam);
        UpdateIcon(hWnd, szText);
        break;
    }

    case WM_DESTROY:
        Server::Stop();
        DeleteIcon(hWnd);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

void SendUpdate(PCWSTR szText)
{
    auto hWnd = FindWindow(CLASSNAME, nullptr);
    ASSERT_WIN32(hWnd);

    SendMessage(hWnd, WM_APP_UPDATE, reinterpret_cast<WPARAM>(szText), 0);
}

void LoadVersion(HINSTANCE hInstance, PCWSTR szName, PWSTR* pszVersion)
{
    auto hResource = FindResource(hInstance, szName, RT_VERSION);
    ASSERT_WIN32(hResource);

    auto hVersionInfo = LoadResource(hInstance, hResource);
    ASSERT_WIN32(hVersionInfo);

    auto pVersionInfo = LockResource(hVersionInfo);
    ASSERT_WIN32(pVersionInfo);

    PDWORD pdwTranslation;
    UINT uLen;
    ASSERT_WIN32(VerQueryValue(pVersionInfo, L"\\VarFileInfo\\Translation",
                               reinterpret_cast<PVOID*>(&pdwTranslation), &uLen));

    auto sProductVersion = std::format(L"\\StringFileInfo\\{:04X}{:04X}\\ProductVersion",
                                       LOWORD(*pdwTranslation), HIWORD(*pdwTranslation));

    ASSERT_WIN32(VerQueryValue(pVersionInfo, sProductVersion.data(),
                               reinterpret_cast<PVOID*>(pszVersion), &uLen));
}

void Init(HINSTANCE hInstance)
{
    hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
    ASSERT_WIN32(hIcon);

    hContextMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDC_CONTEXTMENU));
    ASSERT_WIN32(hContextMenu);

    LoadVersion(hInstance, MAKEINTRESOURCE(VS_VERSION_INFO), &szVersion);

    const WNDCLASSEX wcex{
        .cbSize = sizeof(WNDCLASSEX),
        .lpfnWndProc = WindowProc,
        .lpszClassName = CLASSNAME
    };
    ASSERT_WIN32(RegisterClassEx(&wcex));

    auto hWnd = CreateWindow(CLASSNAME, nullptr, 0, 0, 0, 0, 0, nullptr, nullptr, nullptr, nullptr);
    ASSERT_WIN32(hWnd);

    ShowWindow(hWnd, SW_HIDE);
}

} // namespace ClipSock::Notify
