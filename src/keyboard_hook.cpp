// ===./src/keyboard_hook.cpp/
// 键盘钩子相关功能实现

#ifndef UNICODE
// UNICODE宏可以让Windows函数自动的匹配到W版本
#define UNICODE
#endif

#include <windows.h>
#include <mmsystem.h>
#include <wchar.h>
#include "../include/keyboard_hook.hpp"
#include "../include/utils.hpp"

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
            wchar_t szPath[MAX_PATH];
            swprintf_s(szPath,
                       _countof(szPath),
                       L"./resource/audios/%lu.wav", // 格式串
                       vkCode);                      // 对应的数字
            if (FileExists(szPath))
                PlaySoundW(szPath, NULL, SND_FILENAME | SND_ASYNC);
        }
    }
    // 按照规定需要将事件传递给下一个钩子或系统
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// 低级键盘钩子的回调函数
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if ((wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN) and not Mute)
    {
        // 临时版本，文件写死在代码里
        const wchar_t *audioList[] = {L"74", L"77", L"78", L"84"};
        const int audioFileNumber = sizeof(audioList) / sizeof(audioList[0]);

        // 随机挑一个音频播放
        const wchar_t *name = audioList[random::getInt(0, audioFileNumber - 1)];

        // 拼成完整路径
        wchar_t path[256];
        swprintf(path, 256, L"./resource/audios/%ls.wav", name);
        // MessageBoxW(hwnd, L"HELLO", path, MB_OK);
        PlaySoundW(path, NULL, SND_FILENAME | SND_ASYNC);
    }
    // 按照规定需要将事件传递给下一个钩子或系统
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}