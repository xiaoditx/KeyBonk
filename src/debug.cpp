#include "windows.h"
#include "debug.hpp"

namespace debug
{
    void logWinError(DWORD errorCode)
    {
        LPWSTR messageBuffer = nullptr;
        FormatMessageW(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)&messageBuffer, 0, NULL);
        logOutput(L"    - 错误代码：", std::to_wstring(errorCode).c_str(), L"\n    - 错误信息：", messageBuffer, L"\n");
        LocalFree(messageBuffer);
    }
}