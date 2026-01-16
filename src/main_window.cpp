// ===./src/main_window.cpp===
// 主窗口相关功能实现

#ifndef UNICODE // UNICODE宏可以让Windows函数自动的匹配到W版本
#define UNICODE
#endif

#include <windows.h>
#include <gdiplus.h>
#include <shellapi.h>
#include <winver.h>
#include <string>
#include "../include/main_window.hpp"
#include "../include/about.hpp"
#include "../include/setting.hpp"
#include "../include/keybonk_global.hpp"
#include "../resource/resources.hpp"

// 自定义消息
#define WM_WINDOW_HAS_CREAT (WM_APP + 4) // 窗口以及创建，由后面打开的进程发送到当前窗口

// 显示菜单
void showMenu()
{
    HMENU hMenu = LoadMenu(C_hInstance, MAKEINTRESOURCE(IDR_CONTEXT_MENU));
    HMENU hSubMenu = GetSubMenu(hMenu, 0);

    // 设置菜单项的初始选中状态
    UINT uWindowPenetrateState = WindowPenetrate ? MF_CHECKED : MF_UNCHECKED;
    UINT uMuteState = Mute ? MF_CHECKED : MF_UNCHECKED;
    UINT uMuteMouseState = MuteMouse ? MF_CHECKED : MF_UNCHECKED;
    UINT uMinimumState = minimum ? MF_CHECKED : MF_UNCHECKED;

    CheckMenuItem(hSubMenu, IDM_WINDOW_PENETRATE,
                  MF_BYCOMMAND | uWindowPenetrateState);
    CheckMenuItem(hSubMenu, IDM_MUTE,
                  MF_BYCOMMAND | uMuteState);
    CheckMenuItem(hSubMenu, IDM_MUTE_MOUSE,
                  MF_BYCOMMAND | uMuteMouseState);
    CheckMenuItem(hSubMenu, IDM_MINIMUM,
                  MF_BYCOMMAND | uMinimumState);

    POINT pt;
    GetCursorPos(&pt); // 获取当前鼠标的屏幕坐标

    // 确保窗口在前台，这样点击其他地方时会正确关闭菜单
    SetForegroundWindow(hwnd);

    // 显示右键菜单
    TrackPopupMenu(hSubMenu,
                   TPM_RIGHTBUTTON | TPM_LEFTALIGN,
                   pt.x, pt.y, 0, hwnd, NULL);

    PostMessage(hwnd, WM_NULL, 0, 0);

    DestroyMenu(hMenu);
}
// 设置窗口穿透
bool SetWindowMouseTransparent(HWND hWnd, bool enable)
{
    if (!hWnd || !IsWindow(hWnd)) // 没有句柄或句柄不是窗口
        return false;             // 返回错误

    LONG_PTR exStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);

    // 开启开始关闭
    if (enable)
    {
        exStyle |= WS_EX_TRANSPARENT;
        BYTE alpha = 153; // 255*0.6也就是60%不透明度
        SetLayeredWindowAttributes(hwnd, 13217535, alpha, LWA_ALPHA | LWA_COLORKEY);
    }
    else
    {
        exStyle &= ~WS_EX_TRANSPARENT;
        BYTE alpha = 255; // 100%不透明度
        SetLayeredWindowAttributes(hwnd, 13217535, alpha, LWA_COLORKEY | LWA_ALPHA);
    }

    SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle);

    // 刷新窗口
    SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    return true;
}

// 主窗口消息处理
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        if (MessageBoxExW(hwnd, L"是否关闭软件主窗口？", L"KeyBonk提示", MB_OKCANCEL | MB_ICONQUESTION, 0) == 1)
        {
            RECT rect;                      // 窗口的尺寸信息
            if (GetWindowRect(hwnd, &rect)) // 记录窗口位置
            {
                std::wstring x = std::to_wstring(rect.left);
                std::wstring y = std::to_wstring(rect.top);
                WritePrivateProfileString(L"record", L"win-x", x.c_str(), L"./config.ini");
                WritePrivateProfileString(L"record", L"win-y", y.c_str(), L"./config.ini");
            }
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
        return 0;

    case WM_DESTROY:
        if (g_pBackgroundImage)
        {
            delete g_pBackgroundImage; // 释放背景图片
            g_pBackgroundImage = NULL; // 设置为空防止重新解引用
        }
        RemoveTrayIcon();
        Gdiplus::GdiplusShutdown(g_gdiplusToken); // 关闭GDI库
        CoUninitialize();                         // 关闭COM库
        // 记录静音状态
        WritePrivateProfileString(L"record", L"mute", std::to_wstring(Mute).c_str(), L"./config.ini");
        WritePrivateProfileString(L"record", L"mute-m", std::to_wstring(MuteMouse).c_str(), L"./config.ini");
        // 退出
        PostQuitMessage(0);
        return 0;

    case WM_WINDOW_HAS_CREAT:
    {
        minimum = false;              // 将窗口最小化状态设置为假
        ShowWindow(hwnd, SW_RESTORE); // 恢复窗口
        // 下面这些主要是为了防止特殊情况：
        SetForegroundWindow(hwnd); // 将窗口带到前台
        // 额外确保激活（某些系统需要）
        SetActiveWindow(hwnd);
        SetFocus(hwnd);
        // FlashWindow(hwnd, TRUE); // 我又没有任务栏图标和标题栏我写这玩意干啥？啧啧
        return 0;
    }

    case WM_RBUTTONDOWN:
    {
        showMenu();
        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        Gdiplus::Graphics graphics(hdc);

        // 获取客户区大小
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        int windowWidth = clientRect.right - clientRect.left;
        int windowHeight = clientRect.bottom - clientRect.top;

        // 将图片绘制到整个窗口客户区
        if (g_pBackgroundImage)
        {
            graphics.DrawImage(g_pBackgroundImage, 0, 0, windowWidth, windowHeight);
        }
        EndPaint(hwnd, &ps);
    }
        return 0;

    case WM_SIZE:
    {
        // 窗口大小改变时强制重绘
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    }

    case WM_NCHITTEST:
    {
        LRESULT hit = DefWindowProc(hwnd, uMsg, wParam, lParam);
        // 让客户区可拖动
        if (hit == HTCLIENT && (GetAsyncKeyState(VK_LBUTTON) & 0x8000))
            hit = HTCAPTION;
        return hit;
    }

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_WINDOW_PENETRATE: // 菜单-窗口穿透
            WindowPenetrate = (!WindowPenetrate);
            SetWindowMouseTransparent(hwnd, WindowPenetrate);
            break;
        case IDM_MUTE: // 菜单-静音
            Mute = (!Mute);
            break;
        case IDM_MUTE_MOUSE: // 菜单-静音
            MuteMouse = (!MuteMouse);
            break;
        case IDM_EXIT: // 菜单-退出
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        case IDM_MINIMUM:       // 菜单-最小化
            minimum = !minimum; // 将窗口最小化状态取反
            if (minimum)
            {
                ShowWindow(hwnd, SW_HIDE); // 隐藏窗口
            }
            else
            {
                ShowWindow(hwnd, SW_SHOW); // 展示窗口
            }
            break;
        case IDM_SETTINGS: // 菜单-设置
            settingWindowOpen();
            break;
        case IDM_ABOUT:        // 菜单-关于
            aboutWindowOpen(); // 创建“关于”窗口并展示
            break;
        }
        return 0;
    case WM_CREATE:
    {
        AddTrayIcon(hwnd);
        // 设置标准箭头鼠标指针
        SetCursor(LoadCursor(NULL, IDC_ARROW));
    }
    break;

    case (WM_USER + 1): // 自定义消息：托盘图标点击事件
        if (lParam == WM_RBUTTONDOWN)
        {
            // 显示右键菜单
            showMenu();
        }
        else if (lParam == WM_LBUTTONDBLCLK)
        {
            // 双击左键显示窗口
            ShowWindow(hwnd, SW_SHOW);
            SetForegroundWindow(hwnd);
        }
        break;
    default:
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// 添加托盘图标函数
BOOL AddTrayIcon(HWND hWnd)
{
    // 从资源加载图标
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MY_ICON));
    if (!hIcon)
        return FALSE;

    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = IDI_MY_ICON;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = (WM_USER + 1);
    nid.hIcon = hIcon;

    // 设置提示文本
    lstrcpy(nid.szTip, TEXT("KeyBonk"));

    return Shell_NotifyIcon(NIM_ADD, &nid);
}

// 删除托盘图标函数
void RemoveTrayIcon()
{
    Shell_NotifyIcon(NIM_DELETE, &nid);
}