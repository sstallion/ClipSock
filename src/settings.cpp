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

#include "settings.h"

#include "resource.h"
#include "server.h"
#include "util.h"

#include <windows.h>
#include <commctrl.h>
#include <strsafe.h>
#include <winsock2.h>
#include <ws2tcpip.h>

namespace ClipSock::Settings {

BOOL bLaunchAtStartup;
WCHAR szListenAddress[INET6_ADDRSTRLEN];

BOOL GetRegValues()
{
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, REGKEY_APP, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return FALSE;

    DWORD cbData = sizeof(bLaunchAtStartup);
    RegGetValue(hKey, nullptr, REGVAL_LAUNCH_AT_STARTUP, RRF_RT_DWORD,
                nullptr, &bLaunchAtStartup, &cbData);

    cbData = sizeof(szListenAddress);
    RegGetValue(hKey, nullptr, REGVAL_LISTEN_ADDRESS, RRF_RT_REG_SZ,
                nullptr, szListenAddress, &cbData);

    return TRUE;
}

void SetRegValues()
{
    HKEY hKey;
    VERIFY_WIN32_RESULT(RegCreateKeyEx(HKEY_CURRENT_USER, REGKEY_APP, 0, nullptr,
                                       REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr,
                                       &hKey, nullptr));

    VERIFY_WIN32_RESULT(RegSetValueEx(hKey, REGVAL_LAUNCH_AT_STARTUP, 0, REG_DWORD,
                                      reinterpret_cast<PBYTE>(&bLaunchAtStartup),
                                      sizeof(bLaunchAtStartup)));

    VERIFY_WIN32_RESULT(RegSetValueEx(hKey, REGVAL_LISTEN_ADDRESS, 0, REG_SZ,
                                      reinterpret_cast<PBYTE>(szListenAddress),
                                      sizeof(szListenAddress)));

    ASSERT_WIN32_RESULT(RegOpenKeyEx(HKEY_CURRENT_USER, REGKEY_RUN, 0, KEY_WRITE, &hKey));
    if (bLaunchAtStartup) {
        WCHAR szFileName[MAX_PATH];
        ASSERT_WIN32(GetModuleFileName(nullptr, szFileName, ARRAYSIZE(szFileName)));
        VERIFY_WIN32_RESULT(RegSetValueEx(hKey, REGVAL_APP, 0, REG_SZ,
                                          reinterpret_cast<PBYTE>(szFileName),
                                          sizeof(szFileName)));
    } else {
        RegDeleteValue(hKey, REGVAL_APP);
    }
}

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM /*lParam*/)
{
    switch (message) {
    case WM_INITDIALOG:
        CheckDlgButton(hDlg, IDC_LAUNCH_AT_STARTUP, bLaunchAtStartup);
        SetDlgItemText(hDlg, IDC_LISTEN_ADDRESS, szListenAddress);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDC_LAUNCH_AT_STARTUP: {
            auto uCheck = IsDlgButtonChecked(hDlg, IDC_LAUNCH_AT_STARTUP);
            CheckDlgButton(hDlg, IDC_LAUNCH_AT_STARTUP, !uCheck);
            return TRUE;
        }

        case IDC_RESET:
            CheckDlgButton(hDlg, IDC_LAUNCH_AT_STARTUP, DEFAULT_LAUNCH_AT_STARTUP);
            SetDlgItemText(hDlg, IDC_LISTEN_ADDRESS, DEFAULT_LISTEN_ADDRESS);
            return TRUE;

        case IDOK:
            bLaunchAtStartup = IsDlgButtonChecked(hDlg, IDC_LAUNCH_AT_STARTUP);
            GetDlgItemText(hDlg, IDC_LISTEN_ADDRESS, szListenAddress, ARRAYSIZE(szListenAddress));
            SetRegValues();
            DestroyWindow(hDlg);
            Server::Restart();
            return TRUE;

        case IDCANCEL:
            DestroyWindow(hDlg);
            return TRUE;
        }
    }
    return FALSE;
}

BOOL IsMessage(PMSG pMsg)
{
    auto hDlg = FindWindow(CLASSNAME, nullptr);
    return hDlg && IsDialogMessage(hDlg, pMsg);
}

void ShowDialog()
{
    auto hDlg = FindWindow(CLASSNAME, nullptr);
    if (!hDlg) {
        hDlg = CreateDialog(nullptr, MAKEINTRESOURCE(IDD_SETTINGS), nullptr, DialogProc);
        ASSERT_WIN32(hDlg);
    }

    auto nCmdShow = IsIconic(hDlg) ? SW_RESTORE : SW_SHOW;
    ShowWindow(hDlg, nCmdShow);
    SetForegroundWindow(hDlg);
}

void Init(HINSTANCE hInstance)
{
    bLaunchAtStartup = DEFAULT_LAUNCH_AT_STARTUP;
    StringCchCopy(szListenAddress, ARRAYSIZE(szListenAddress), DEFAULT_LISTEN_ADDRESS);

    const INITCOMMONCONTROLSEX iccex{
        .dwSize = sizeof(INITCOMMONCONTROLSEX),
        .dwICC = ICC_STANDARD_CLASSES
    };
    ASSERT_WIN32(InitCommonControlsEx(&iccex));

    const WNDCLASSEX wcex{
        .cbSize = sizeof(WNDCLASSEX),
        .lpfnWndProc = DefDlgProc,
        .cbWndExtra = DLGWINDOWEXTRA,
        .hCursor = LoadCursor(nullptr, IDC_ARROW),
        .hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW+1),
        .lpszClassName = CLASSNAME,
        .hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON))
    };
    ASSERT_WIN32(RegisterClassEx(&wcex));

    // Show settings if registry is uninitialized:
    if (!GetRegValues()) {
        ShowDialog();
    }
}

} // namespace ClipSock::Settings
