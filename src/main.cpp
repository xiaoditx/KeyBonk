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

#include "../include/keybonk_global.hpp"
#include "../include/window_manager.hpp"
#include "../include/keyboard_hook.hpp"
#include "../include/utils.hpp"
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

    if (IsInstanceAlreadyRunning(L"KeyBonk主窗口", L"KeyBonk主窗口"))
    {
        return 0;
    }

    // 保存一些参数到全局
    C_hInstance = hInstance;
    C_nCmdShow = nCmdShow;

    // 读取配置项文件中的record（上次退出时记录）部分
    // win-x和win-y 上次退出时的窗口位置
    int windowPositionX = GetPrivateProfileInt(L"record", L"win-x", 100, L"./config.ini");
    int windowPositionY = GetPrivateProfileInt(L"record", L"win-y", 100, L"./config.ini");
    // 把上次退出记录的静音情况读到Mute和MuteMouse里
    Mute = bool(GetPrivateProfileInt(L"record", L"mute", 0, L"./config.ini"));
    MuteMouse = bool(GetPrivateProfileInt(L"record", L"mute-m", 0, L"./config.ini"));

    // 读取设置信息
    GetPrivateProfileString(L"settings", L"lib", L"./bin/default/audio/", audioLibPath, MAX_PATH, L"./config.ini");

    // 初始化COM库（其实这是一个很久的未来才会有的功能要用的初始化，只是提前写了）
    hrMain = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    if (FAILED(hrMain))
    {
        MessageBoxExW(
            NULL, L"错误：00001，初始化COM库时发生异常，请检查系统相关文件是否完好",
            L"KB - 运行时发生错误", MB_OK | MB_ICONEXCLAMATION, 0); // 消息框提示出错
        return 0;
    }

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
        MessageBoxExW(
            NULL, L"错误：00002，创建窗口时发生异常，请检查系统各项设置是否正常",
            L"KB - 运行时发生错误", MB_OK | MB_ICONEXCLAMATION, 0); // 消息框提示出错
        return 0;
    }

    // 设置透明，这个版本颜色先写死在代码里
    COLORREF crKey = 13217535; // 偏粉红色的颜色
    SetLayeredWindowAttributes(hwnd, crKey, 0, LWA_COLORKEY);

    ShowWindow(hwnd, nCmdShow); // 展示窗口

    // 初始化GDI+
    Gdiplus::GpStatus GDIpStatus; // 接收GDI+库的状态（错误码）
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    GDIpStatus = Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);
    if (GDIpStatus != Gdiplus::Ok)
    { // 如果存在问题
        MessageBoxExW(
            NULL, L"错误：00003，初始化GDI+库时发生异常",
            L"KB - 运行时发生错误", MB_OK | MB_ICONEXCLAMATION, 0); // 消息框提示出错
        return 0;
    }

    if (!FileExists(L"./resource/background.png"))
    {
        MessageBoxExW(
            NULL, L"错误：00004，找不到背景图片，请检查文件夹完整性",
            L"KB - 运行时发生错误", MB_OK | MB_ICONEXCLAMATION, 0); // 消息框提示出错
        return 0;
    }
    // 加载背景图片
    g_pBackgroundImage = new Gdiplus::Image(L"./resource/background.png");

    // 安装钩子
    HHOOK KeyboardHook = NULL; // 钩子句柄
    KeyboardHook = SetWindowsHookExW(
        WH_KEYBOARD_LL, // 低级键盘钩子
        // 似乎也能用WH_KEYBOARD，但低级钩子用起来更稳定、简单些
        LowLevelKeyboardProc, // 传递回调函数地址
        GetModuleHandle(NULL),
        0);
    if (KeyboardHook == NULL)
    { // 没拿到句柄则失败
        MessageBoxExW(
            NULL, L"错误：00005，钩子安装失败，请检查杀毒软件是否关闭",
            L"KB - 运行时发生错误", MB_OK | MB_ICONEXCLAMATION, 0); // 消息框提示出错
    }

    // 鼠标钩子
    HHOOK MouseHook = NULL; // 钩子句柄
    MouseHook = SetWindowsHookExW(
        WH_MOUSE_LL,       // 低级鼠标钩子
        LowLevelMouseProc, // 传递回调函数地址
        GetModuleHandle(NULL),
        0);
    if (MouseHook == NULL)
    { // 没拿到句柄则失败
        MessageBoxExW(
            NULL, L"错误：00005，钩子安装失败，请检查杀毒软件是否关闭",
            L"KB - 运行时发生错误", MB_OK | MB_ICONEXCLAMATION, 0); // 消息框提示出错
    }

    // 消息循环
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}