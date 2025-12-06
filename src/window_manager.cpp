// ===./src/window_manager.cpp===
// 窗口管理相关功能实现

#ifndef UNICODE
// UNICODE宏可以让Windows函数自动的匹配到W版本
#define UNICODE
#endif

#include <windows.h>
#include <gdiplus.h>
#include <shellapi.h>
#include <winver.h>
#include <string>
#include "../include/window_manager.h"
#include "../resource/resources.h"

// 关于窗口的向前声明
LRESULT CALLBACK WindowProc_about(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 设置窗口穿透
bool SetWindowMouseTransparent(HWND hWnd, bool enable)
{
    if (!hWnd || !IsWindow(hWnd))
        return false;

    LONG_PTR exStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);

    if (enable)
        exStyle |= WS_EX_TRANSPARENT;
    else
        exStyle &= ~WS_EX_TRANSPARENT;

    SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle);

    // 刷新窗口
    SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    return true;
}

void aboutWindowOpen()
{

    // 注册窗口类
    const wchar_t CLASS_NAME[] = L"KeyBonk“关于”窗口";
    WNDCLASSEX wc = {};                // 用0初始化整个WindowClass
    wc.cbSize = sizeof(WNDCLASSEX);    // 设置结构体大小
    wc.lpfnWndProc = WindowProc_about; // 指定WindowProc_about函数
    wc.hInstance = C_hInstance;
    wc.lpszClassName = CLASS_NAME; // 窗口类名称
    wc.hIcon = (HICON)LoadImage(C_hInstance, MAKEINTRESOURCE(IDI_MY_ICON), IMAGE_ICON, 64, 64, 0);
    wc.hIconSm = (HICON)LoadImage(C_hInstance, MAKEINTRESOURCE(IDI_MY_ICON), IMAGE_ICON, 64, 64, 0); // 小图标（窗口标题栏）
    RegisterClassEx(&wc);                                                                            // 注册

    hwndAbout = CreateWindowExW(
        0, CLASS_NAME, L"关于",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        hwnd, NULL, C_hInstance, NULL);

    // 创建失败则提示并返回，结束运行
    if (hwndAbout == NULL)
    {
        MessageBoxExW(
            NULL, L"错误：00002，创建窗口时发生异常，请检查系统各项设置是否正常",
            L"KB - 运行时发生错误", MB_OK | MB_ICONEXCLAMATION, 0); // 消息框提示出错
    }
    ShowWindow(hwndAbout, C_nCmdShow);
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
        }

    case WM_DESTROY:
        if (g_pBackgroundImage)
        {
            delete g_pBackgroundImage; // 释放背景图片
            g_pBackgroundImage = NULL; // 设置为空防止重新解引用
        }
        RemoveTrayIcon();
        Gdiplus::GdiplusShutdown(g_gdiplusToken); // 关闭GDI库
        CoUninitialize();                         // 关闭COM库
        PostQuitMessage(0);
        return 0;

    case WM_RBUTTONDOWN:
    {
        // 显示右键菜单
        HMENU hMenu = LoadMenu(C_hInstance, MAKEINTRESOURCE(IDR_CONTEXT_MENU));
        HMENU hSubMenu = GetSubMenu(hMenu, 0);

        // 设置菜单项的初始选中状态
        UINT uWindowPenetrateState = WindowPenetrate ? MF_CHECKED : MF_UNCHECKED;
        UINT uMuteState = Mute ? MF_CHECKED : MF_UNCHECKED;

        CheckMenuItem(hSubMenu, IDM_WINDOW_PENETRATE,
                      MF_BYCOMMAND | uWindowPenetrateState);
        CheckMenuItem(hSubMenu, IDM_MUTE,
                      MF_BYCOMMAND | uMuteState);

        POINT pt = {LOWORD(lParam), HIWORD(lParam)};
        ClientToScreen(hwnd, &pt);

        // 显示右键菜单
        TrackPopupMenu(hSubMenu,
                       TPM_RIGHTBUTTON | TPM_LEFTALIGN,
                       pt.x, pt.y, 0, hwnd, NULL);
        DestroyMenu(hMenu);
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
        case IDM_WINDOW_PENETRATE:
            WindowPenetrate = (!WindowPenetrate);
            SetWindowMouseTransparent(hwnd, WindowPenetrate);
            break;
        case IDM_MUTE:
            Mute = (!Mute);
            break;
        case IDM_EXIT:
            PostMessage(hwnd, WM_CLOSE, 0, 0);
            break;
        case IDM_MINIMUM:
            minimum = !minimum;
            if (minimum)
            {
                ShowWindow(hwnd, SW_HIDE);
            }
            else
            {
                ShowWindow(hwnd, SW_SHOW);
            }
            break;
        case IDM_SETTINGS:
            MessageBoxExW(
                NULL, L"还没有开发呢",
                L"嘻嘻", MB_OK | MB_ICONEXCLAMATION, 0);
            break;
        case IDM_ABOUT:
            aboutWindowOpen();
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

    case (WM_USER + 1):
        if (lParam == WM_RBUTTONDOWN)
        {
            // 显示右键菜单
            HMENU hMenu = LoadMenu(C_hInstance, MAKEINTRESOURCE(IDR_CONTEXT_MENU));
            HMENU hSubMenu = GetSubMenu(hMenu, 0);

            // 设置菜单项的初始选中状态
            UINT uWindowPenetrateState = WindowPenetrate ? MF_CHECKED : MF_UNCHECKED;
            UINT uMuteState = Mute ? MF_CHECKED : MF_UNCHECKED;
            UINT uMinimumState = minimum ? MF_CHECKED : MF_UNCHECKED;

            CheckMenuItem(hSubMenu, IDM_WINDOW_PENETRATE,
                          MF_BYCOMMAND | uWindowPenetrateState);
            CheckMenuItem(hSubMenu, IDM_MUTE,
                          MF_BYCOMMAND | uMuteState);
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

// 关于窗口消息处理
LRESULT CALLBACK WindowProc_about(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        DestroyWindow(hwndAbout);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);
    }
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
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
