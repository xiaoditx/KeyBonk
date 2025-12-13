// ===./include/main_window.hpp===
// 主窗口相关功能声明

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <windows.h>
#include <gdiplus.h>
#include "keybonk_global.hpp"

// 函数声明
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // 主窗口消息处理
BOOL AddTrayIcon(HWND hWnd);
void RemoveTrayIcon();
bool SetWindowMouseTransparent(HWND hWnd, bool enable);

#endif // MAIN_WINDOW_H