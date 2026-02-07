// ===./src/keyboard_hook.cpp/
// 键盘钩子相关功能实现

#ifndef UNICODE
// UNICODE宏可以让Windows函数自动的匹配到W版本
#define UNICODE
#endif

#include <windows.h>
#include <wchar.h>
#include "debug.hpp"
#include "hook/keyboard_hook.hpp"
#include "global.hpp"
#include "functions/audioPlay.hpp"

// 低级键盘钩子的回调函数
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION)
    {
        KBDLLHOOKSTRUCT *keyInfo = (KBDLLHOOKSTRUCT *)lParam;
        // 判断是否为按键按下事件
        if ((wParam == WM_KEYDOWN /*|| wParam == WM_SYSKEYDOWN*/) and not Mute)
        {
            DWORD vkCode = keyInfo->vkCode;
            wchar_t fileName[10]{};
            swprintf_s(fileName, _countof(fileName), L"%lu", vkCode);
            PlayAudioFile(fileName);
        }
    }
    // 按照规定需要将事件传递给下一个钩子或系统
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}