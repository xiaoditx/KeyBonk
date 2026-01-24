// ===./src/utils.cpp===
// 通用的工具函数实现

#ifndef UNICODE
// UNICODE宏可以让Windows函数自动的匹配到W版本
#define UNICODE
#endif

#include <filesystem>
#include <windows.h>
#include <string>
#include <random>
#include <chrono>
#include <algorithm>
#include "../include/utils.hpp"

// 各种文件路径处理函数
bool GetExePath(wchar_t *outBuffer, size_t bufferSize)
{
    if (!outBuffer || bufferSize == 0)
    {
        return false;
    }

    DWORD result = GetModuleFileNameW(nullptr, outBuffer, static_cast<DWORD>(bufferSize));

    // 检查错误
    if (result == 0 || GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    {
        outBuffer[0] = L'\0';
        return false;
    }

    return true;
}

bool GetExeDirectory(wchar_t *outBuffer, size_t bufferSize)
{
    if (!outBuffer || bufferSize == 0)
    {
        return false;
    }

    // 先获取exe完整路径
    if (!GetExePath(outBuffer, bufferSize))
    {
        return false;
    }

    // 去掉文件名，只保留目录
    wchar_t *lastSlash = nullptr;
    wchar_t *current = outBuffer;
    while (*current)
    {
        if (*current == L'\\' || *current == L'/')
        {
            lastSlash = current;
        }
        ++current;
    }

    if (lastSlash)
    {
        *lastSlash = L'\0';
    }
    else
    {
        // 没有找到目录分隔符，返回当前目录
        DWORD result = GetCurrentDirectoryW(static_cast<DWORD>(bufferSize), outBuffer);
        return result > 0 && result <= bufferSize;
    }

    return true;
}

bool GetExeRelativePath(const wchar_t *rawPath, wchar_t *outBuffer, size_t bufferSize)
{
    if (!rawPath || !outBuffer || bufferSize == 0)
    {
        return false;
    }

    // 获取exe目录
    wchar_t exeDir[MAX_PATH];
    if (!GetExeDirectory(exeDir, MAX_PATH))
    {
        return false;
    }

    // 检查是否是绝对路径
    if (rawPath[0] == L'\\' || rawPath[0] == L'/' ||
        (rawPath[1] == L':' && (rawPath[2] == L'\\' || rawPath[2] == L'/')))
    {
        // 已经是绝对路径，直接复制
        if (wcslen(rawPath) >= bufferSize)
        {
            return false;
        }
        wcscpy_s(outBuffer, bufferSize, rawPath);
        return true;
    }

    // 组合路径
    std::filesystem::path fullPath = std::filesystem::path(exeDir) / std::filesystem::path(rawPath);

    // 规范化路径（处理..和.）
    fullPath = std::filesystem::weakly_canonical(fullPath);

    // 复制到输出缓冲区
    std::wstring fullPathStr = fullPath.wstring();
    if (fullPathStr.length() >= bufferSize)
    {
        return false;
    }

    wcscpy_s(outBuffer, bufferSize, fullPathStr.c_str());
    return true;
}

bool FileExists(const wchar_t *rawPath)
{
    wchar_t fullPath[MAX_PATH];

    // 获取基于exe目录的完整路径
    if (!GetExeRelativePath(rawPath, fullPath, MAX_PATH))
    {
        return false;
    }
    // 检查文件是否存在
    return std::filesystem::exists(fullPath);
}

// 随机数生成
// （很难理解我为什么会在只要取四个随机数的情况下仍然不想用std::rand）
namespace random
{
    // 获取全局随机数生成器的单例引用
    // 使用Mersenne Twister 19937算法，线程安全（C++11保证静态局部变量初始化线程安全）
    static std::mt19937 &getGenerator()
    {
        // 静态局部变量，首次调用时初始化，后续调用返回同一实例
        static std::mt19937 generator(
            []() // 使用lambda表达式进行延迟初始化
            {
                // 创建真随机数设备，用于获取随机种子
                std::random_device rd;

                // 混合两种随机源生成种子：
                // 1. random_device的真随机数
                // 2. 当前时间戳（纳秒计数）
                // 使用异或操作混合两个随机源，增加熵值
                auto seed = rd() ^ static_cast<unsigned int>(
                                       std::chrono::system_clock::now().time_since_epoch().count());

                // 使用混合后的种子初始化Mersenne Twister生成器
                std::mt19937 gen(seed);
                // 预热，丢弃前10个随机数，Mersenne Twister初始状态的前几个数值随机性较差
                gen.discard(10);
                return gen;
            }()); // lambda立即调用，返回初始化好的生成器
        return generator;
    }
    // 生成[min, max]范围内的均匀分布随机整数
    // 参数：
    //   min - 范围下限（包含）
    //   max - 范围上限（包含）
    // 返回值：范围内的随机整数
    int getInt(int min, int max)
    {
        // 确保min <= max，如果输入相反则交换
        if (min > max)
            std::swap(min, max);
        // 创建均匀整数分布器
        std::uniform_int_distribution<int> dist(min, max);
        // 使用全局生成器产生随机数
        return dist(getGenerator());
    }
    // 生成[min, max)范围内的均匀分布随机浮点数（左闭右开）
    // 参数：
    //   min - 范围下限（包含）
    //   max - 范围上限（不包含）
    // 返回值：范围内的随机浮点数
    double getDouble(double min, double max)
    {
        // 确保min <= max，如果输入相反则交换
        if (min > max)
            std::swap(min, max);
        // 创建均匀实数分布器（默认区间为[min, max)）
        std::uniform_real_distribution<double> dist(min, max);
        // 使用全局生成器产生随机数
        return dist(getGenerator());
    }
}

// 短期内不会使用

// 资源的安全释放
template <typename T>
void safeRelease(T **resourcePointer)
{
    // 检查是否为空
    if (*resourcePointer)
    {
        // 释放资源
        (*resourcePointer)->Release();
        // 设置为空
        *resourcePointer = NULL;
    }
}