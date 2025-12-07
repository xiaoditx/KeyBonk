// ===./include/keyboard_hook.h===
// 键盘钩子相关功能

#ifndef KEYBOARD_HOOK_H
#define KEYBOARD_HOOK_H

#include <windows.h>
#include "keybonk_global.hpp"

// 函数声明
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam); // 钩子回调

#endif // KEYBOARD_HOOK_H