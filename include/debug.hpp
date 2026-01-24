#ifndef DEBUG_HPP
#define DEBUG_HPP

namespace debug
{
    /**
     * @brief 将宽字符串日志写入到 ./log.txt 文件
     *
     * @param message 要写入的宽字符串消息
     *
     * 该函数会将传入的宽字符串消息追加写入到当前目录下的 log.txt 文件中。
     * 每次调用都会在消息后自动添加换行符。
     * 如果文件无法打开，函数会静默失败。
     *
     * @note 文件使用 UTF-8 编码，支持跨平台使用。
     * @note 使用追加模式，不会覆盖之前的日志内容。
     */
    void logOutput(const wchar_t *message);
}

#endif // DEBUG_HPP