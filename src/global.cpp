// ===./src/global.cpp===
// 全局变量定义

#ifndef UNICODE
// UNICODE宏可以让Windows函数自动的匹配到W版本
#define UNICODE
#endif

#include "global.hpp"

namespace keybonk
{
    resource_manager::~resource_manager()
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
        if (gdiplusToken != 0)
        {
            Gdiplus::GdiplusShutdown(gdiplusToken);
            gdiplusToken = 0;
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
}