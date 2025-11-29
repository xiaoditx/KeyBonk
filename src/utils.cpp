// ===./src/utils.cpp===
// 通用的工具函数实现

#ifndef UNICODE
// UNICODE宏可以让Windows函数自动的匹配到W版本
#define UNICODE
#endif

#include <filesystem>
#include <string>
#include "../include/utils.h"

// 判断文件是否存在
bool FileExists(const wchar_t *rawPath)
{
    std::wstring_view pathView{rawPath};
    return std::filesystem::exists(pathView);
}

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
        resourcePointer = NULL;
    }
}