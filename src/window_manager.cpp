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
#include "../include/window_manager.hpp"
#include "../resource/resources.hpp"

// 设置窗口消息处理向前声明
LRESULT CALLBACK WindowProc_setting(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
// 关于窗口的向前声明
LRESULT CALLBACK WindowProc_about(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// 设置窗口穿透
bool SetWindowMouseTransparent(HWND hWnd, bool enable)
{
    if (!hWnd || !IsWindow(hWnd)) // 没有句柄或句柄不是窗口
        return false;             // 返回错误

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

// 打开“关于”窗口
void aboutWindowOpen()
{
    // 检查关于窗口是否已经存在
    if (hwndAbout != NULL && IsWindow(hwndAbout))
    {
        // 检查窗口是否被最小化或隐藏
        if (IsIconic(hwndAbout))
        {
            // 恢复窗口
            ShowWindow(hwndAbout, SW_RESTORE);
        }
        else if (!IsWindowVisible(hwndAbout))
        {
            // 显示窗口
            ShowWindow(hwndAbout, SW_SHOW);
        }
        // 激活窗口并设置为前台
        SetForegroundWindow(hwndAbout);
        return;
    }

    // 注册窗口类
    const wchar_t CLASS_NAME[] = L"KeyBonk“关于”窗口";
    WNDCLASSEX wc = {};                // 用0初始化整个WindowClass
    wc.cbSize = sizeof(WNDCLASSEX);    // 设置结构体大小
    wc.lpfnWndProc = WindowProc_about; // 指定WindowProc_about函数
    wc.hInstance = C_hInstance;
    wc.lpszClassName = CLASS_NAME; // 窗口类名称
    wc.hIcon = (HICON)LoadImage(C_hInstance, MAKEINTRESOURCE(IDI_MY_ICON), IMAGE_ICON, 64, 64, 0);
    wc.hIconSm = (HICON)LoadImage(C_hInstance, MAKEINTRESOURCE(IDI_MY_ICON), IMAGE_ICON, 64, 64, 0); // 小图标（窗口标题栏）
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassEx(&wc); // 注册

    hwndAbout = CreateWindowExW(
        WS_EX_APPWINDOW, CLASS_NAME, L"关于 KeyBonk",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 600, 400,
        NULL, NULL, C_hInstance, NULL);

    // 创建失败则提示并返回，结束运行
    if (hwndAbout == NULL)
    {
        MessageBoxExW(
            NULL, L"错误：00002，创建窗口时发生异常，请检查系统各项设置是否正常",
            L"KB - 运行时发生错误", MB_OK | MB_ICONEXCLAMATION, 0); // 消息框提示出错
    }
    ShowWindow(hwndAbout, C_nCmdShow);
    UpdateWindow(hwndAbout);
}

// 打开“设置”窗口
void settingWindowOpen()
{
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
        case IDM_WINDOW_PENETRATE: // 菜单-窗口穿透
            WindowPenetrate = (!WindowPenetrate);
            SetWindowMouseTransparent(hwnd, WindowPenetrate);
            break;
        case IDM_MUTE: // 菜单-静音
            Mute = (!Mute);
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
            MessageBoxExW(
                NULL, L"还没有开发呢",
                L"嘻嘻", MB_OK | MB_ICONEXCLAMATION, 0);
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

// “关于”窗口使用：控件ID定义
#define IDC_LOGO 1001
#define IDC_SOFTWARE_NAME 1002
#define IDC_VERSION 1003
#define IDC_AUTHOR 1004
#define IDC_DESCRIPTION 1005
#define IDC_CLOSE_BUTTON 1006
#define IDC_WEBSITE_LINK 1007

// 关于窗口消息处理
LRESULT CALLBACK WindowProc_about(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        // 获取客户区大小
        RECT rect;
        GetClientRect(hwnd, &rect);
        int clientWidth = rect.right - rect.left;
        int clientHeight = rect.bottom - rect.top;

        // 创建Logo图标（静态图片控件）
        HWND hLogo = CreateWindowExW(0, L"STATIC", L"",
                                     WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_CENTERIMAGE,
                                     20, 20, 96, 96,
                                     hwnd, (HMENU)IDC_LOGO, C_hInstance, NULL);

        // 使用GDI+加载PNG图片作为Logo
        Gdiplus::Image *pLogoImage = Gdiplus::Image::FromFile(L"./resource/icon-org.png");
        if (pLogoImage && pLogoImage->GetLastStatus() == Gdiplus::Ok)
        {
            // 创建一个位图来绘制图片
            HDC hdc = GetDC(NULL);
            HDC hMemDC = CreateCompatibleDC(hdc);
            HBITMAP hBitmap = CreateCompatibleBitmap(hdc, 96, 96);
            SelectObject(hMemDC, hBitmap);

            // 绘制背景为白色（与窗口颜色一致）
            RECT rect = {0, 0, 96, 96};
            FillRect(hMemDC, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));

            // 绘制PNG图片
            Gdiplus::Graphics graphics(hMemDC);
            graphics.DrawImage(pLogoImage, 0, 0, 96, 96);

            // 设置位图到静态控件
            SendMessageW(hLogo, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);

            // 释放资源
            delete pLogoImage;
            DeleteDC(hMemDC);
            ReleaseDC(NULL, hdc);
        }

        // 创建软件名称文本
        CreateWindowExW(0, L"STATIC", L"KeyBonk",
                        WS_CHILD | WS_VISIBLE | SS_CENTER | SS_NOPREFIX,
                        130, 20, clientWidth - 150, 30,
                        hwnd, (HMENU)IDC_SOFTWARE_NAME, C_hInstance, NULL);

        // 创建软件版本文本
        CreateWindowExW(0, L"STATIC", L"版本: v1.1.0.1",
                        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX,
                        130, 60, clientWidth - 150, 20,
                        hwnd, (HMENU)IDC_VERSION, C_hInstance, NULL);

        // 创建软件作者文本
        CreateWindowExW(0, L"STATIC", L"作者: 小狄同学呀",
                        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX,
                        130, 90, clientWidth - 150, 20,
                        hwnd, (HMENU)IDC_AUTHOR, C_hInstance, NULL);

        // 创建软件介绍文本
        CreateWindowExW(0, L"STATIC",
                        L"    KeyBonk是一款键盘音效软件，为了让您收获更优秀的按键效果。"
                        L"软件基于旧版由易语言开发的软件坤音键盘进行重开发，运行效率等方面均有所提升\r\n"
                        L"    当前版本的变化：更改窗口尺寸",
                        WS_CHILD | WS_VISIBLE | SS_LEFT | SS_NOPREFIX,
                        20, 130, clientWidth - 40, 120, // 增加高度以容纳更多内容
                        hwnd, (HMENU)IDC_DESCRIPTION, C_hInstance, NULL);

        // 创建个人网站链接
        CreateWindowExW(WS_EX_TRANSPARENT, L"STATIC", L"访问我的网站: https://xiaoditx.github.io/",
                        WS_CHILD | WS_VISIBLE | SS_CENTER | SS_NOTIFY | SS_NOPREFIX,
                        20, 270, clientWidth - 40, 30, // 向下移动位置
                        hwnd, (HMENU)IDC_WEBSITE_LINK, C_hInstance, NULL);

        // 创建关闭按钮
        CreateWindowExW(0, L"BUTTON", L"关闭",
                        WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                        clientWidth - 100, clientHeight - 50, 80, 30,
                        hwnd, (HMENU)IDC_CLOSE_BUTTON, C_hInstance, NULL);

        // 设置文本控件的字体
        HFONT hFont = CreateFontW(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                  DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                  CLEARTYPE_QUALITY, VARIABLE_PITCH, L"微软雅黑");
        SendMessageW(GetDlgItem(hwnd, IDC_SOFTWARE_NAME), WM_SETFONT, (WPARAM)hFont, TRUE);

        HFONT hNormalFont = CreateFontW(24, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                        DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                        CLEARTYPE_QUALITY, VARIABLE_PITCH, L"微软雅黑");
        SendMessageW(GetDlgItem(hwnd, IDC_VERSION), WM_SETFONT, (WPARAM)hNormalFont, TRUE);
        SendMessageW(GetDlgItem(hwnd, IDC_AUTHOR), WM_SETFONT, (WPARAM)hNormalFont, TRUE);
        SendMessageW(GetDlgItem(hwnd, IDC_DESCRIPTION), WM_SETFONT, (WPARAM)hNormalFont, TRUE);
        SendMessageW(GetDlgItem(hwnd, IDC_WEBSITE_LINK), WM_SETFONT, (WPARAM)hNormalFont, TRUE);

        // 为关闭按钮设置微软雅黑字体
        SendMessageW(GetDlgItem(hwnd, IDC_CLOSE_BUTTON), WM_SETFONT, (WPARAM)hNormalFont, TRUE);

        // 设置链接文本颜色为蓝色
        // 使用WM_CTLCOLORSTATIC消息来处理文本颜色，这里先设置控件的背景模式
        SetWindowLongW(GetDlgItem(hwnd, IDC_WEBSITE_LINK), GWL_STYLE,
                       GetWindowLongW(GetDlgItem(hwnd, IDC_WEBSITE_LINK), GWL_STYLE) | SS_NOTIFY);

        return 0;
    }

    case WM_SYSCOMMAND:
    {
        // 移除自定义的最小化处理，使用默认行为，让窗口正常显示在任务栏
        break;
    }

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_CLOSE_BUTTON:
            // 关闭按钮点击事件
            DestroyWindow(hwnd);
            return 0;

        case IDC_WEBSITE_LINK:
            // 网站链接点击事件
            ShellExecuteW(NULL, L"open", L"https://xiaoditx.github.io/", NULL, NULL, SW_SHOWNORMAL);
            return 0;
        }
        break;
    }

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

    case WM_SETCURSOR:
    {
        // 为网站链接添加手型光标
        if (LOWORD(lParam) == HTCLIENT)
        {
            HWND hControl = (HWND)wParam;
            if (hControl == GetDlgItem(hwnd, IDC_WEBSITE_LINK))
            {
                SetCursor(LoadCursor(NULL, IDC_HAND));
                return TRUE;
            }
        }
        break;
    }

    case WM_LBUTTONDOWN:
    {
        // 处理链接的点击事件（确保SS_NOTIFY生效）
        POINT pt;
        GetCursorPos(&pt);
        ScreenToClient(hwnd, &pt);

        HWND hLink = GetDlgItem(hwnd, IDC_WEBSITE_LINK);
        RECT linkRect;
        GetWindowRect(hLink, &linkRect);
        ScreenToClient(hwnd, (POINT *)&linkRect);
        ScreenToClient(hwnd, (POINT *)&linkRect.right);

        if (PtInRect(&linkRect, pt))
        {
            ShellExecuteW(NULL, L"open", L"https://www.keybonk.com", NULL, NULL, SW_SHOWNORMAL);
        }
        break;
    }

    case WM_CTLCOLORSTATIC:
    {
        HWND hWndCtrl = (HWND)lParam;
        HDC hdcCtrl = (HDC)wParam;

        if (hWndCtrl == GetDlgItem(hwnd, IDC_WEBSITE_LINK))
        {
            // 设置文本颜色为蓝色
            SetTextColor(hdcCtrl, RGB(0, 0, 255));
            // 设置背景模式为透明
            SetBkMode(hdcCtrl, TRANSPARENT);
            // 返回与窗口相同的背景刷子
            return (LRESULT)(HBRUSH)GetStockObject(WHITE_BRUSH);
        }
        else
        {
            // 对于其他所有文本控件，设置与窗口相同的背景色
            SetBkColor(hdcCtrl, RGB(255, 255, 255));
            SetTextColor(hdcCtrl, RGB(0, 0, 0));
            // 返回与窗口相同的背景刷子
            return (LRESULT)(HBRUSH)GetStockObject(WHITE_BRUSH);
        }
        break;
    }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// 设置窗口消息处理
LRESULT CALLBACK WindowProc_setting(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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
