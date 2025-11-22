// keyboard_hook.cpp
// 键盘钩子相关功能实现

#include <windows.h>
#include <mmsystem.h>
#include <wchar.h>
#include "../include/keyboard_hook.h"
#include "../include/utils.h"

// 低级键盘钩子的回调函数
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* keyInfo = (KBDLLHOOKSTRUCT*)lParam;
        // 判断是否为按键按下事件
        if ((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) and not Mute) {
            DWORD vkCode = keyInfo->vkCode;
            wchar_t szPath[MAX_PATH];
            swprintf_s(szPath,
                    _countof(szPath),
                    L"./resource/audios/%lu.wav",   // 格式串
                    vkCode);   // 对应的数字
            if(FileExists(szPath))
                PlaySoundW(szPath, NULL, SND_FILENAME | SND_ASYNC);
        }
    }
    // 按照规定需要将事件传递给下一个钩子或系统
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}