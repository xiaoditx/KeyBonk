// ===./include/setting.hpp===
// 设置窗口相关功能声明

#ifndef SETTING_H
#define SETTING_H

#include <windows.h>
#include <gdiplus.h>
#include "global.hpp"

// 函数声明
void settingWindowOpen();                                                                // 打开设置窗口
LRESULT CALLBACK WindowProc_setting(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // 设置窗口消息处理
#endif                                                                                   // SETTING_H