// ===./include/utils.hpp===
// 工具函数

#ifndef UTILS_H
#define UTILS_H

#include <wchar.h>

// 函数声明
bool FileExists(const wchar_t *rawPath);

// 模板函数声明
template <typename T>
void safeRelease(T **resourcePointer);

#endif // UTILS_H