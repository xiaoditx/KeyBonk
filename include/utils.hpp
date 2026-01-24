// ===./include/utils.hpp===
// 工具函数

#ifndef UTILS_H
#define UTILS_H

#include <wchar.h>

// 函数声明

/**
 * @brief 获取可执行文件的完整路径
 * @param outBuffer 输出缓冲区
 * @param bufferSize 缓冲区大小（以字符为单位）
 * @return 成功返回 true，失败返回 false
 */
bool GetExePath(wchar_t *outBuffer, size_t bufferSize);

/**
 * @brief 获取可执行文件所在目录
 * @param outBuffer 输出缓冲区
 * @param bufferSize 缓冲区大小（以字符为单位）
 * @return 成功返回 true，失败返回 false
 */
bool GetExeDirectory(wchar_t *outBuffer, size_t bufferSize);

/**
 * @brief (目前弃用)将相对路径转换为基于exe目录的绝对路径
 * @param rawPath 原始相对路径
 * @param outBuffer 输出缓冲区
 * @param bufferSize 缓冲区大小（以字符为单位）
 * @return 成功返回 true，失败返回 false
 */
bool GetExeRelativePath(const wchar_t *rawPath, wchar_t *outBuffer, size_t bufferSize);

/**
 * @brief 检查文件是否存在（基于exe目录的相对路径）
 * @param rawPath 原始相对路径
 * @return 文件存在返回 true，不存在返回 false
 */
bool FileExists(const wchar_t *rawPath);

// 取随机数
namespace random
{
    int getInt(int min, int max);
    double getDouble(double min, double max);
}

// 模板函数声明
template <typename T>
void safeRelease(T **resourcePointer);

#endif // UTILS_H