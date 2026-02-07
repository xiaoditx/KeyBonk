#ifndef DEBUG_HPP
#define DEBUG_HPP

#include "global.hpp"
#include <fstream>
#include <codecvt>
#include <locale>

namespace debug
{
    template <typename... Args>
    void logOutput(Args... args)
    {
        // 使用追加模式打开文件，确保每次调用不会覆盖之前的日志
        std::wofstream logFile(fullDebugFilePath, std::ios::app);

        if (!logFile.is_open())
        {
            // 如果文件无法打开，可以在这里添加错误处理
            // 当前实现选择静默失败，避免影响主程序运行
            return;
        }

        // 设置locale以支持宽字符写入
        logFile.imbue(std::locale(logFile.getloc(), new std::codecvt_utf8<wchar_t>));

        // 写入消息
        ((logFile << args), ...);

        // 文件会在析构时自动关闭
    }
}

#endif // DEBUG_HPP