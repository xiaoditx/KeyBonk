// ===./src/setting.cpp===
// 设置窗口相关功能实现

#ifndef UNICODE// UNICODE宏可以让Windows函数自动的匹配到W版本
#define UNICODE
#endif

#include <windows.h>
#include <gdiplus.h>
#include <shellapi.h>
#include <winver.h>
#include <string>
#include "../include/setting.hpp"
#include "../include/keybonk_global.hpp"
#include "../resource/resources.hpp"

// 打开“设置”窗口
void settingWindowOpen()
{
}

// 设置窗口消息处理
LRESULT CALLBACK WindowProc_setting(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}