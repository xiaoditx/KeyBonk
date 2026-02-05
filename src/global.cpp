// ===./src/global.cpp===
// 全局变量定义

#ifndef UNICODE
// UNICODE宏可以让Windows函数自动的匹配到W版本
#define UNICODE
#endif

#include <windows.h>
#include "global.hpp"
#include <gdiplus.h>

// 全局变量定义
ULONG_PTR g_gdiplusToken = 0;         // GDI+的token
bool comInitialized = false;          // COM库是否初始化成功
wchar_t *fullIniFilePath = nullptr;   // 配置文件完整路径
wchar_t *fullDebugFilePath = nullptr; // 日志文件完整路径
HWND hwnd = NULL;                     // 主窗口句柄
HWND hwndAbout = NULL;                // "关于"窗口句柄
HWND hwndSetting = NULL;              // “设置”窗口句柄
bool Mute = false;                    // 键盘是否静音
bool MuteMouse = false;               // 鼠标是否静音
bool WindowPenetrate = false;         // 窗口穿透
NOTIFYICONDATA nid = {};              // 任务栏通知区域图标状态
wchar_t audioLibPath[MAX_PATH];       // 音频库位置
bool minimum = false;
HINSTANCE C_hInstance;
HHOOK KeyboardHook = nullptr; // 钩子句柄
HHOOK MouseHook = nullptr;    // 钩子句柄
HBITMAP hBmp = nullptr;       // 存储背景图片的位图
HDC hdcScreen = nullptr;      // 主窗口屏幕DC
HDC memDC = nullptr;          // 主窗口内存DC
HBITMAP hOldBmp = nullptr;    // 主窗口内存DC默认位图
int C_nCmdShow;
HRESULT hrMain; // 接受Windows函数的返回结果

// 全局资源释放
void releaseGlobalResources()
{
    if (fullIniFilePath != nullptr)
    {
        delete[] fullIniFilePath;
        fullIniFilePath = nullptr;
    }
    if (fullDebugFilePath != nullptr)
    {
        delete[] fullDebugFilePath;
        fullDebugFilePath = nullptr;
    }
    // 释放钩子资源
    if (KeyboardHook != NULL)
    {
        UnhookWindowsHookEx(KeyboardHook);
        KeyboardHook = NULL;
    }
    if (MouseHook != NULL)
    {
        UnhookWindowsHookEx(MouseHook);
        MouseHook = NULL;
    }
    // 关闭GDI+和COM库
    if (g_gdiplusToken != 0)
    {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
    if (comInitialized == true)
    {
        CoUninitialize();
        comInitialized = false;
    }
    // 清理DC和位图
    if (memDC != nullptr && hOldBmp != nullptr)
    {
        SelectObject(memDC, hOldBmp); // 选出自定义位图
        DeleteDC(memDC);
        memDC = nullptr;
        hOldBmp = nullptr;
    }
    if (hdcScreen != nullptr)
    {
        ReleaseDC(hwnd, hdcScreen);
        hdcScreen = nullptr;
    }
    if (hBmp != nullptr)
    {
        DeleteObject(hBmp);
        hBmp = nullptr;
    }
}