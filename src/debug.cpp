#include "debug.hpp"
#include <fstream>
#include <codecvt>
#include <locale>

namespace debug
{
    void logOutput(const wchar_t *message)
    {
        // 使用追加模式打开文件，确保每次调用不会覆盖之前的日志
        std::wofstream logFile("./log.txt", std::ios::app);

        if (!logFile.is_open())
        {
            // 如果文件无法打开，可以在这里添加错误处理
            // 当前实现选择静默失败，避免影响主程序运行
            return;
        }

        // 设置locale以支持宽字符写入
        logFile.imbue(std::locale(logFile.getloc(), new std::codecvt_utf8<wchar_t>));

        // 写入消息并换行
        logFile << message << L'\n';

        // 文件会在析构时自动关闭
    }
}