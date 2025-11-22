// window_manager.h
// 窗口管理相关功能

#ifndef WINDOW_MANAGER_H
#define WINDOW_MANAGER_H

#include <windows.h>
#include <gdiplus.h>
#include "keybonk_global.h"

// 函数声明
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // 消息处理
BOOL AddTrayIcon(HWND hWnd);
void RemoveTrayIcon();
bool SetWindowMouseTransparent(HWND hWnd, bool enable);

#endif // WINDOW_MANAGER_H