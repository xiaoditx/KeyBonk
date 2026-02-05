#include "hook/mouse_hook.hpp"
#include "global.hpp"
#include "functions/randnum.hpp"
#include "functions/audioPlay.hpp"

// 低级鼠标钩子的回调函数
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if ((wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN) and not MuteMouse)
    {
        // 临时版本，文件写死在代码里
        const wchar_t *audioList[] = {L"74", L"77", L"78", L"84"};
        const int audioFileNumber = sizeof(audioList) / sizeof(audioList[0]);

        // 随机挑一个音频播放
        const wchar_t *name = audioList[random::getInt(0, audioFileNumber - 1)];
        PlayAudioFile(name);
    }
    // 按照规定需要将事件传递给下一个钩子或系统
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}