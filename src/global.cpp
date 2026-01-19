// ===./src/global.cpp===
// 全局变量定义

#ifndef UNICODE
// UNICODE宏可以让Windows函数自动的匹配到W版本
#define UNICODE
#endif

#include <windows.h>
#include <gdiplus.h>
#include "../include/keybonk_global.hpp"

// 全局变量定义
ULONG_PTR g_gdiplusToken = 0;              // GDI+的token
HWND hwnd = NULL;                          // 主窗口句柄
HWND hwndAbout = NULL;                     // "关于"窗口句柄
HWND hwndSetting = NULL;                   // “设置”窗口句柄
Gdiplus::Image *g_pBackgroundImage = NULL; // 背景图片
bool Mute = false;                         // 键盘是否静音
bool MuteMouse = false;                    // 鼠标是否静音
bool WindowPenetrate = false;              // 窗口穿透
NOTIFYICONDATA nid = {};                   // 任务栏通知区域图标状态
wchar_t audioLibPath[MAX_PATH];            // 音频库位置
bool minimum = false;
HINSTANCE C_hInstance = NULL;
int C_nCmdShow;
HRESULT hrMain; // 接受Windows函数的返回结果