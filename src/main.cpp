// === ./src/main.cpp ===
// 主程序存放的文件
// 包含消息循环，作为入口

#ifndef UNICODE
// UNICODE宏可以让Windows函数自动的匹配到W版本
#define UNICODE
#endif

#include <windows.h>
#include <gdiplus.h>
#include <shellapi.h>
#include <string>

#include "global.hpp"
#include "debug.hpp"
#include "window_manager.hpp"
#include "hook/keyboard_hook.hpp"
#include "hook/mouse_hook.hpp"
#include "functions/files.hpp"
#include "../resource/resources.hpp"

// 自定义消息
#define WM_WINDOW_HAS_CREAT (WM_APP + 4) // 窗口以及创建，由后面打开的进程发送到当前窗口

// 检查软件是否重复启动
// 检查标准为能否根据“窗口类名+窗口标题”的组合查询到主窗口的句柄
BOOL IsInstanceAlreadyRunning(LPCTSTR windowClass, LPCTSTR windowTitle)
{
    HWND f_hWnd = FindWindow(windowClass, windowTitle);
    if (f_hWnd)
    {
        // 找到窗口，激活它
        PostMessageW(f_hWnd, WM_WINDOW_HAS_CREAT, 0, 0); // 发送窗口消息给已经存在的窗口
        return TRUE;
    }
    return FALSE;
}

// 主程序
int WINAPI wWinMain(HINSTANCE hInstance, [[maybe_unused]] HINSTANCE hPrevInstance, [[maybe_unused]] PWSTR pCmdLine, int nCmdShow)
{
    fullDebugFilePath = new wchar_t[MAX_PATH]{};
    GetExeRelativePath(L"./log.txt", fullDebugFilePath, MAX_PATH);

    if (IsInstanceAlreadyRunning(L"KeyBonk主窗口", L"KeyBonk主窗口"))
    {
        debug::logOutput(L"\n===检查到KeyBonk主窗口重复创建,软件退出===");
        debug::logOutputWithoutEndl(L"退出信息：\nhInstance:");
        debug::logOutputWithoutEndl(std::to_wstring(reinterpret_cast<long long>(hInstance)).c_str());
        debug::logOutput(L"\n===============================\n");
        return 0;
    }

    // 保存一些参数到全局
    C_hInstance = hInstance;
    C_nCmdShow = nCmdShow;

    // 读取配置项文件中的record（上次退出时记录）部分
    // 格式化出ini文件的完整路径
    fullIniFilePath = new wchar_t[MAX_PATH]{};
    GetExeRelativePath(L"./config.ini", fullIniFilePath, MAX_PATH);
    // win-x和win-y 上次退出时的窗口位置
    int windowPositionX = GetPrivateProfileInt(L"record", L"win-x", 100, fullIniFilePath);
    int windowPositionY = GetPrivateProfileInt(L"record", L"win-y", 100, fullIniFilePath);
    // 把上次退出记录的静音情况读到Mute和MuteMouse里
    Mute = bool(GetPrivateProfileInt(L"record", L"mute", 0, fullIniFilePath));
    MuteMouse = bool(GetPrivateProfileInt(L"record", L"mute-m", 0, fullIniFilePath));

    // 读取设置信息
    GetPrivateProfileString(L"settings", L"lib", L".\\bin\\default", audioLibPath, MAX_PATH, fullIniFilePath);

    // 初始化COM库（其实这是一个很久的未来才会有的功能要用的初始化，只是提前写了）
    hrMain = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    if (FAILED(hrMain))
    {
        debug::logOutput(L"[初始化]初始化COM库异常");
        MessageBoxExW(
            NULL, L"错误：00001，初始化COM库时发生异常，请检查系统相关文件是否完好",
            L"KB - 运行时发生错误", MB_OK | MB_ICONEXCLAMATION, 0); // 消息框提示出错
        return 0;
    }
    comInitialized = true;

    // 注册窗口类
    const wchar_t CLASS_NAME[] = L"KeyBonk主窗口";
    WNDCLASSEX wc = {};             // 用0初始化整个WindowClass
    wc.cbSize = sizeof(WNDCLASSEX); // 设置结构体大小
    wc.lpfnWndProc = WindowProc;    // 指定WindowProc函数
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME; // 窗口类名称
    wc.hIcon = (HICON)LoadImage(C_hInstance, MAKEINTRESOURCE(IDI_MY_ICON), IMAGE_ICON, 64, 64, 0);
    wc.hIconSm = (HICON)LoadImage(C_hInstance, MAKEINTRESOURCE(IDI_MY_ICON), IMAGE_ICON, 64, 64, 0); // 小图标（窗口标题栏）
    RegisterClassEx(&wc);                                                                            // 注册

    // 创建窗口
    hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, // 支持透明，设置置顶，隐藏软件图标
        CLASS_NAME,                                       // 窗口类
        L"KeyBonk主窗口",                                 // 窗口文本
        WS_POPUP | WS_VISIBLE | WS_CLIPCHILDREN,          // 窗口风格
                                                          // 大小+位置
        windowPositionX, windowPositionY, 160, 180,
        NULL,      // 父窗口
        NULL,      // 菜单
        hInstance, // 示例句柄
        NULL       // 附带的软件数据
    );

    // 创建失败则提示并返回，结束运行
    if (hwnd == NULL)
    {
        debug::logOutput(L"[初始化]主窗口创建异常");
        MessageBoxExW(
            NULL, L"错误：00002，创建窗口时发生异常，请检查系统各项设置是否正常",
            L"KB - 运行时发生错误", MB_OK | MB_ICONEXCLAMATION, 0); // 消息框提示出错
        releaseGlobalResources();
        return 0;
    }

    // 初始化GDI+
    Gdiplus::GpStatus GDIpStatus; // 接收GDI+库的状态（错误码）
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    GDIpStatus = Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);
    if (GDIpStatus != Gdiplus::Ok)
    { // 如果存在问题
        debug::logOutput(L"[初始化]GDI+初始化异常");
        MessageBoxExW(
            NULL, L"错误：00003，初始化GDI+库时发生异常",
            L"KB - 运行时发生错误", MB_OK | MB_ICONEXCLAMATION, 0); // 消息框提示出错
        releaseGlobalResources();
        return 0;
    }

    // 加载背景图片
    wchar_t *imgPath = new wchar_t[MAX_PATH]{};
    wchar_t *exePath = new wchar_t[MAX_PATH]{};
    GetExeDirectory(exePath, MAX_PATH);
    swprintf_s(imgPath, MAX_PATH, L"%ls\\%ls\\background.png", exePath, audioLibPath);
    delete[] exePath;
    Gdiplus::Bitmap *pBitmap;
    if (FileExists(imgPath))
    {
        pBitmap = Gdiplus::Bitmap::FromFile(imgPath);
        if (!pBitmap || pBitmap->GetLastStatus() != Gdiplus::GpStatus::Ok)
        {
            // 图片加载失败，创建红色矩形作为替代
            pBitmap = new Gdiplus::Bitmap(160, 180);
            Gdiplus::Graphics g(pBitmap);
            Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 0, 0));
            g.FillRectangle(&brush, 0, 0, 160, 180);
        }
    }
    else
    {
        debug::logOutput(L"[初始化]找不到背景图片");
        MessageBoxExW(
            NULL, L"错误：00004，当前声音库找不到背景图片，请检查文件夹完整性",
            L"KB - 运行时发生错误", MB_OK | MB_ICONEXCLAMATION, 0); // 消息框提示出错
        delete[] imgPath;
        releaseGlobalResources();
        return 0;
    }
    delete[] imgPath;

    pBitmap->Gdiplus::Bitmap::GetHBITMAP(
        Gdiplus::Color(0, 0, 0, 0),
        &hBmp); // 透明背景

    delete pBitmap;

    // 创建内存DC
    hdcScreen = GetDC(hwnd);
    memDC = CreateCompatibleDC(hdcScreen);
    hOldBmp = (HBITMAP)SelectObject(memDC, hBmp);

    // 获取图片尺寸
    BITMAP bm;
    GetObject(hBmp, sizeof(BITMAP), &bm);
    SIZE size = {bm.bmWidth, bm.bmHeight};

    // 使用UpdateLayeredWindow
    POINT ptSrc = {0, 0};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(hwnd, hdcScreen, NULL, &size, memDC, &ptSrc, 0, &bf, ULW_ALPHA);

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // 安装键盘钩子
    KeyboardHook = SetWindowsHookExW(
        WH_KEYBOARD_LL, // 低级键盘钩子
        // 似乎也能用WH_KEYBOARD，但低级钩子用起来更稳定、简单些
        LowLevelKeyboardProc, // 传递回调函数地址
        GetModuleHandle(NULL),
        0);
    if (KeyboardHook == NULL)
    { // 没拿到句柄则失败
        debug::logOutput(L"[初始化]键盘钩子安装失败");
        MessageBoxExW(
            NULL, L"错误：00005，钩子安装失败，请检查杀毒软件是否关闭",
            L"KB - 运行时发生错误", MB_OK | MB_ICONEXCLAMATION, 0); // 消息框提示出错
        // 释放已分配的资源
        releaseGlobalResources();
        return 0;
    }

    // 鼠标钩子
    MouseHook = SetWindowsHookExW(
        WH_MOUSE_LL,       // 低级鼠标钩子
        LowLevelMouseProc, // 传递回调函数地址
        GetModuleHandle(NULL),
        0);
    if (MouseHook == NULL)
    { // 没拿到句柄则失败
        debug::logOutput(L"[初始化]鼠标钩子安装失败");
        MessageBoxExW(
            NULL, L"错误：00005，钩子安装失败，请检查杀毒软件是否关闭",
            L"KB - 运行时发生错误", MB_OK | MB_ICONEXCLAMATION, 0); // 消息框提示出错
        // 释放已分配的资源
        releaseGlobalResources();
        return 0;
    }

    // 消息循环
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    releaseGlobalResources();
    return 0;
}