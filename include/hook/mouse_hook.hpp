#pragma once

#include <Windows.h>

// 鼠标钩子回调函数类型
typedef LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);

// 安装鼠标钩子
HHOOK InstallMouseHook(MouseHookProc* hookProc);

// 卸载鼠标钩子
BOOL UninstallMouseHook(HHOOK hHook);
