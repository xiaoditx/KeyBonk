#include <windows.h>
#include "functions/audioPlay.hpp"
#include "debug.hpp"
#include "globalDevelopmentControl.hpp"
#include "functions/files.hpp"
#include "global.hpp"

// 播放音频文件的通用函数
void PlayAudioFile(const wchar_t *fileName)
{
    wchar_t audioPath[MAX_PATH]{};
    swprintf_s(audioPath,
               MAX_PATH,
               L"%ls\\audios\\%ls.wav", // 格式串
               audioLibPath,            // 音频库位置
               fileName);               // 文件名
    wchar_t *fullPath = new wchar_t[MAX_PATH]{};
    GetExeRelativePath(audioPath, fullPath, MAX_PATH);

    if (FileExists(fullPath))
    {
#ifdef KB_DEBUG
        debug::logOutput(L"[功能]播放了音频", fullPath, "\n");
#endif
        PlaySoundW(fullPath, NULL, SND_FILENAME | SND_ASYNC);
    }

    delete[] fullPath;
}