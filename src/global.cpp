// global.cpp
// 全局变量定义

#include <windows.h>
#include <gdiplus.h>
#include "../include/keybonk_global.h"

// 全局变量定义
ULONG_PTR g_gdiplusToken = 0; // GDI+的token
HWND hwnd = NULL; // 窗口句柄
Gdiplus::Image* g_pBackgroundImage = NULL; // 背景图片
bool Mute = false; // 是否静音
bool WindowPenetrate = false; // 窗口穿透
NOTIFYICONDATA nid = {}; // 任务栏通知区域图标状态
bool minimum = false;
HINSTANCE C_hInstance = NULL;
HRESULT hrMain; // 接受Windows函数的返回结果